#include "CjvxAudioPWireDevice.h"
#include "CjvxAudioPWireTechnology.h"

#include <spa/pod/builder.h>
#include <pipewire/filter.h>

static void process_input(void *userdata)
{
	CjvxAudioPWireDevice* this_pointer = (CjvxAudioPWireDevice*)userdata;
	if(this_pointer)
	{
		this_pointer->process_buffer_input_pw();
	}
};

// ========================================================================

jvxErrorType 
CjvxAudioPWireDevice::start_device_input_pw()
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

	common.aud_info.channels = common.numChansInMax;
	params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat, &common.aud_info);

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
CjvxAudioPWireDevice::stop_device_input_pw()
{
	pw_thread_loop_lock(common.loop);
	pw_stream_disconnect(input.stream);
 	pw_stream_destroy(input.stream);
	pw_thread_loop_unlock(common.loop);
 	return JVX_NO_ERROR;
}

void 
CjvxAudioPWireDevice::process_buffer_input_pw()
{
#if 0
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
 
        buf->datas[0].chunk->offset = 0;
        buf->datas[0].chunk->stride = stride;
        buf->datas[0].chunk->size = n_frames * stride;
 
        pw_stream_queue_buffer(input.stream, b);

#endif

	struct pw_buffer *b;
	struct spa_buffer *buf;
	int i, c, n_frames_max, stride;
	jvxSize n_frames = 0;
	jvxSize elmSize = jvxDataFormat_getsize(common.format);

	if ((b = pw_stream_dequeue_buffer(input.stream)) == NULL)
	{
		pw_log_warn("out of buffers: %m");
		return;
	}

	buf = b->buffer;
	stride = elmSize * common.numChansInMax;
	n_frames_max = buf->datas[0].maxsize / stride;
	n_frames = buf->datas[0].chunk->size / stride;

	if (n_frames == common.bsize)
	{
		// I am the master. I will start the chain here!
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			_common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
		}

		jvxSize cnt = 0;
		switch(common.format)
		{
		case JVX_DATAFORMAT_DATA:
		{
			input.src_data = (jvxData *)buf->datas[0].data;
			jvxData **bufsOut = (jvxData **)jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
			for (i = 0; i < common.numChansInMax; i++)
			{
				if (jvx_bitTest(common.maskInput, i))
				{
					assert(cnt <= _common_set_ocon.theData_out.con_params.number_channels);
					jvx_convertSamples_from_to<jvxData>(input.src_data,
														bufsOut[cnt],
														common.bsize,
														i, common.numChansInMax,
														0, 1);
					cnt++;
				}
			}
			break;
		}
		default:
			assert(0);
		}

		// We drive the buffer through the chain!
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			_common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
		}

		// Finish buffer processing
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			_common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
		}
	}
	else
	{
		// std::cout << __FUNCTION__ << ": Framesize mismatch!" << std::endl;
		genPWire_device::properties_active_higher.num_lost_buffers.value++;
	}
	pw_stream_queue_buffer(input.stream, b);
}
