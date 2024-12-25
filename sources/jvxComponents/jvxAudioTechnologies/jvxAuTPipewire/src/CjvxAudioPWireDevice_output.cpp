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
		this_pointer->process_buffer_output_pw();
	}
};

// ============================================================================

jvxErrorType 
CjvxAudioPWireDevice::start_device_output_pw()
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

		// Using the id or the serial? According to latest doc: serial. My tests however seem to indicate id
		//idConnect = theNode->serial;
		idConnect = theNode->id;
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

	// Other formats specified in common code
	common.aud_info.channels = common.numChansOutMax;

	params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat, &common.aud_info);
	
	/*
	 * Setting any of the following options does not have any impact..
	 int interleave = 0;
	params[1] = (const spa_pod*)spa_pod_builder_add_object(&b,
                SPA_TYPE_OBJECT_Format, SPA_PARAM_EnumFormat,
                SPA_FORMAT_AUDIO_interleave, SPA_POD_OPT_Int(0),

				);
	
	params[2] = spa_pod_builder_add_object(&b,
		SPA_TYPE_OBJECT_ParamBuffers, SPA_PARAM_Buffers,
		SPA_PARAM_BUFFERS_buffers, SPA_POD_CHOICE_RANGE_Int(4, 2, 10),
		SPA_PARAM_BUFFERS_blocks,  SPA_POD_Int(1),
		SPA_PARAM_BUFFERS_size,    SPA_POD_Int(2048),
		SPA_PARAM_BUFFERS_stride,  SPA_POD_Int(data->stride));
		*/
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
CjvxAudioPWireDevice::stop_device_output_pw()
{
	pw_thread_loop_lock(common.loop);
	pw_stream_disconnect(output.stream);
 	pw_stream_destroy(output.stream);
	pw_thread_loop_unlock(common.loop);
 	return JVX_NO_ERROR;
}

void 
CjvxAudioPWireDevice::process_buffer_output_pw()
{
	struct pw_buffer *b;
	struct spa_buffer *buf;
	int i, c, n_frames_max, stride;
	jvxSize n_frames = 0;
	jvxSize elmSize = jvxDataFormat_getsize(common.format);

	if ((b = pw_stream_dequeue_buffer(output.stream)) == NULL)
	{
		pw_log_warn("out of buffers: %m");
		return;
	}

	buf = b->buffer;
	stride = elmSize * common.numChansOutMax;
	n_frames_max = buf->datas[0].maxsize / stride;
	assert(n_frames_max >= b->requested);

	if (b->requested == common.bsize)
	{
		n_frames = b->requested;

		// I am the master. I will start the chain here!
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			_common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
		}

		// Here, we do NOT fill in any data!!

		// We copy the pointer to the pw buffer though
		switch (common.format)
		{
		case JVX_DATAFORMAT_DATA:
			output.dst_data = (jvxData *)buf->datas[0].data;
			break;
		default:
			assert(0);
			break;
		}

		// We drive the buffer through the chain!
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			_common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
		}

		// Set the target buffer to nullptr to prevent any problem to occur
		output.dst_data = nullptr;

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
	// Finalize the output buffer
	buf->datas[0].chunk->offset = 0;
	buf->datas[0].chunk->stride = stride;
	buf->datas[0].chunk->size = n_frames * stride;
	pw_stream_queue_buffer(output.stream, b);
}

jvxErrorType 
CjvxAudioPWireDevice::process_buffer_icon_output(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvxSize cnt = 0;

	// This is the endpoint on the processing chain. We take the data that comes in and copy that to the
	// pw buffer reference!
	switch(common.format)
	{
	case JVX_DATAFORMAT_DATA:
	{
		jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
		if(output.dst_data)
		{
			for (i = 0; i < common.numChansOutMax; i++)
			{
				if(jvx_bitTest(common.maskOutput, i))
				{
					assert(cnt <= _common_set_icon.theData_in->con_params.number_channels);
					jvx_convertSamples_from_to<jvxData>(bufsIn[cnt],
													output.dst_data,
													common.bsize,
													0, 1,
													i, common.numChansOutMax);
													cnt++;
				}
			}
		}
		else
		{
			std::cout << __FUNCTION__ << ": Request to process buffer with nullptr target location!" << std::endl;
		}
		break;
	}
	}
	return res;
}
	
