#include "CjvxAudioPWireDevice.h"
#include "CjvxAudioPWireTechnology.h"

#include <spa/pod/builder.h>
#include <pipewire/filter.h>

static void process_input(void *userdata)
{
	CjvxAudioPWireDevice* this_pointer = (CjvxAudioPWireDevice*)userdata;
	if(this_pointer)
	{
		this_pointer->process_buffer_input();
	}
};

// ========================================================================

jvxErrorType 
CjvxAudioPWireDevice::start_device_input()
{
	std::string str = _common_set.theName + "-sink";

	const struct spa_pod *params[1];
	uint8_t buffer[1024];
	struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

	input.stream_events.process = process_input;
	
	int idConnect = PW_ID_ANY;
	auto elmSS = theDevicehandle->sources.begin();
	if(elmSS != theDevicehandle->sources.end())
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

	input.stream = pw_stream_new_simple(
		pw_thread_loop_get_loop(common.loop),
		str.c_str(),
		pw_properties_new(
			PW_KEY_MEDIA_TYPE, "Audio",
			PW_KEY_MEDIA_CATEGORY, "Capture",
			PW_KEY_MEDIA_ROLE, "Music",
			NULL),
		&input.stream_events,
		this);

	params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat, &input.aud_info);

	int conn_result = pw_stream_connect(input.stream,
					  PW_DIRECTION_INPUT,
					  idConnect,
					  (pw_stream_flags)(PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS | PW_STREAM_FLAG_RT_PROCESS),
					  params, 1);

	pw_thread_loop_unlock(common.loop);

	if(conn_result < 0)
	{
		std::cout << __FUNCTION__ << "Error connecting the input stream!" << std::endl;
		goto error;
	}
	return JVX_NO_ERROR;
error:
	return JVX_ERROR_INTERNAL;
}

jvxErrorType 
CjvxAudioPWireDevice::stop_device_input()
{
 	pw_stream_destroy(input.stream);
 	return JVX_NO_ERROR;
}

void 
CjvxAudioPWireDevice::process_buffer_input()
{
        struct pw_buffer *b;
        struct spa_buffer *buf;
        int i, c, n_frames, stride;
        int16_t *dst, val;
 
        if ((b = pw_stream_dequeue_buffer(input.stream)) == NULL) {
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
 
#if 0
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
#endif 
        buf->datas[0].chunk->offset = 0;
        buf->datas[0].chunk->stride = stride;
        buf->datas[0].chunk->size = n_frames * stride;
 
        pw_stream_queue_buffer(input.stream, b);
}