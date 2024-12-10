#include "CjvxAudioPWireDevice.h"
#include "CjvxAudioPWireTechnology.h"

#include <spa/pod/builder.h>
#include <pipewire/filter.h>

#define M_PI_M2 ( M_PI + M_PI )

static void process_output(void *userdata)
{
	CjvxAudioPWireDevice* this_pointer = (CjvxAudioPWireDevice*)userdata;
	if(this_pointer)
	{
		this_pointer->process_buffer_output();
	}
};

// ============================================================================

jvxErrorType 
CjvxAudioPWireDevice::start_device_output()
{
	std::string str = _common_set.theName + "-src";

	const struct spa_pod *params[1];
	uint8_t buffer[1024];
	struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

	output.stream_events.process = process_output;
	
	int idConnect = PW_ID_ANY;
	auto elmSS = theDevicehandle->sinks.begin();
	if(elmSS != theDevicehandle->sinks.end())
	{
		oneNode* theNode = *elmSS;
		idConnect = theNode->serial;
	}
	
	common.loop = parent->loop_tech;
	if(loopLocal && loop_dev)	
	{
		common.loop = loop_dev;
	}

	pw_thread_loop_lock(common.loop);

	output.stream = pw_stream_new_simple(
		pw_thread_loop_get_loop(common.loop),
		str.c_str(),
		pw_properties_new(
			PW_KEY_MEDIA_TYPE, "Audio",
			PW_KEY_MEDIA_CATEGORY, "Playback",
			PW_KEY_MEDIA_ROLE, "Music",
			NULL),
		&output.stream_events,
		this);

	output.aud_info.format = SPA_AUDIO_FORMAT_S16; // SPA_AUDIO_FORMAT_F64_LE; // 
	output.aud_info.rate = 48000;
	output.aud_info.channels = 2;

	params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat, &output.aud_info);

	int conn_result = pw_stream_connect(output.stream,
					  PW_DIRECTION_OUTPUT,
					  idConnect,
					  (pw_stream_flags)(PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS | PW_STREAM_FLAG_RT_PROCESS),
					  params, 1);

	pw_thread_loop_unlock(common.loop);

	if(conn_result < 0)
	{
		std::cout << __FUNCTION__ << "Error connecting the output stream!" << std::endl;
		goto error;
	}
	return JVX_NO_ERROR;
error:
	return JVX_ERROR_INTERNAL;
}

jvxErrorType 
CjvxAudioPWireDevice::stop_device_output()
{
 	pw_stream_destroy(output.stream);
 	return JVX_NO_ERROR;
}

void 
CjvxAudioPWireDevice::process_buffer_output()
{
        struct pw_buffer *b;
        struct spa_buffer *buf;
        int i, c, n_frames, stride;
        int16_t *dst, val;
 
        if ((b = pw_stream_dequeue_buffer(output.stream)) == NULL) {
                pw_log_warn("out of buffers: %m");
                return;
        }
 
        buf = b->buffer;
        if ((dst = (int16_t*)buf->datas[0].data) == NULL)
                return;
 
        stride = sizeof(int16_t) * 2;
        n_frames = buf->datas[0].maxsize / stride;
        if (b->requested)
                n_frames = SPA_MIN(b->requested, n_frames);
 
        for (i = 0; i < n_frames; i++) {
                output.accumulator += M_PI_M2 * 440 / 48000;
                if (output.accumulator >= M_PI_M2)
                        output.accumulator -= M_PI_M2;
 
                /* sin() gives a value between -1.0 and 1.0, we first apply
                 * the volume and then scale with 32767.0 to get a 16 bits value
                 * between [-32767 32767].
                 * Another common method to convert a double to
                 * 16 bits is to multiple by 32768.0 and then clamp to
                 * [-32768 32767] to get the full 16 bits range. */
                val = sin(output.accumulator) * 1 * 32767.0;
                for (c = 0; c < 2; c++)
                        *dst++ = val;
        }
 
        buf->datas[0].chunk->offset = 0;
        buf->datas[0].chunk->stride = stride;
        buf->datas[0].chunk->size = n_frames * stride;
 
        pw_stream_queue_buffer(output.stream, b);
}
