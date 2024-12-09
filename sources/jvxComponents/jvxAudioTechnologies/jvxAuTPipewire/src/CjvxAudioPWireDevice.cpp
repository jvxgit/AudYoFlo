#include "CjvxAudioPWireDevice.h"
#include "CjvxAudioPWireTechnology.h"

#include <spa/pod/builder.h>
#include <pipewire/filter.h>

#define M_PI_M2 ( M_PI + M_PI )

struct port 
{
    CjvxAudioPWireDevice *this_pointer;
};

static void on_process(void *userdata, struct spa_io_position *position)
{
	CjvxAudioPWireDevice* this_pointer = (CjvxAudioPWireDevice*)userdata;
	if(this_pointer)
	{
		this_pointer-> process_buffer_duplex(position);
	}
};

static void process_output(void *userdata)
{
	CjvxAudioPWireDevice* this_pointer = (CjvxAudioPWireDevice*)userdata;
	if(this_pointer)
	{
		this_pointer->process_buffer_output();
	}
};

static void process_input(void *userdata)
{
	CjvxAudioPWireDevice* this_pointer = (CjvxAudioPWireDevice*)userdata;
	if(this_pointer)
	{
		this_pointer->process_buffer_input();
	}
};

static const struct pw_filter_events filter_events = {
        PW_VERSION_FILTER_EVENTS,
        .process = on_process,
};

CjvxAudioPWireDevice::CjvxAudioPWireDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
}


CjvxAudioPWireDevice::~CjvxAudioPWireDevice()
{
	if (parentTech)
	{		
	}

	// The device handle must have been removed before!!
	assert(this->theDevicehandle == nullptr);
}

jvxErrorType 
CjvxAudioPWireDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}
	
jvxErrorType 
CjvxAudioPWireDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}
	
jvxErrorType 
CjvxAudioPWireDevice::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	return CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb)) ;
}
	
jvxErrorType 
CjvxAudioPWireDevice::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType 
CjvxAudioPWireDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Specify the number of buffers we need. Must be at least "1"
	_common_set_ocon.theData_out.con_data.number_buffers = 1;
	return CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}
	
jvxErrorType 
CjvxAudioPWireDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	_common_set_ocon.theData_out.con_data.number_buffers = 0;
}

jvxErrorType 
CjvxAudioPWireDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		
	// Limitation: I can not provide a samplerate on the filter object as it seems!
	// https://gitlab.freedesktop.org/pipewire/pipewire/-/issues/4410

	switch(theDevicehandle->opMode)
	{
		case operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_DUPLEX:
			res = start_device_duplex();
			break;
		case operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_OUTPUT:
			res = start_device_output();
			break;
		default:
			res = start_device_input();
	}
	return res;
}

// ===========================================================================0
// ===========================================================================0

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
	
	pw_thread_loop_lock(parent->loop);

	output.stream = pw_stream_new_simple(
		pw_thread_loop_get_loop(parent->loop),
		str.c_str(),
		pw_properties_new(
			PW_KEY_MEDIA_TYPE, "Audio",
			PW_KEY_MEDIA_CATEGORY, "Playback",
			PW_KEY_MEDIA_ROLE, "Music",
			NULL),
		&output.stream_events,
		this);

	params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat, &output.aud_info);

	int conn_result = pw_stream_connect(output.stream,
					  PW_DIRECTION_OUTPUT,
					  idConnect,
					  (pw_stream_flags)(PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS | PW_STREAM_FLAG_RT_PROCESS),
					  params, 1);

	pw_thread_loop_unlock(parent->loop);

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
	
	pw_thread_loop_lock(parent->loop);

	input.stream = pw_stream_new_simple(
		pw_thread_loop_get_loop(parent->loop),
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

	pw_thread_loop_unlock(parent->loop);

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

jvxErrorType 
CjvxAudioPWireDevice::start_device_duplex()
{
	jvxSize i;
	std::string filtername = _common_set.theName + "-filter";
	const struct spa_pod *params[2];

	uint8_t buffer[1024];
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

	duplex.inProcessing.numChansInMax = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.size();
	duplex.inProcessing.numChansOutMax = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.size();

	pw_thread_loop_lock(parent->loop);

	duplex.inProcessing.filter = pw_filter_new_simple(
		pw_thread_loop_get_loop(parent->loop),
		filtername.c_str(),
		pw_properties_new(
			PW_KEY_MEDIA_TYPE, "Audio",
			PW_KEY_MEDIA_CATEGORY, "Filter",
			PW_KEY_MEDIA_ROLE, "DSP",
			NULL),
		&filter_events,
		this);

	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(duplex.inProcessing.in_ports, void*, duplex.inProcessing.numChansInMax);
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(duplex.inProcessing.out_ports, void*, duplex.inProcessing.numChansOutMax);

	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(duplex.inProcessing.in_data, float*, duplex.inProcessing.numChansInMax);
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(duplex.inProcessing.out_data, float*, duplex.inProcessing.numChansOutMax);

	auto elmSS = theDevicehandle->sources.begin();
   	oneNode *theNode = *elmSS;
   	auto elmP = theNode->out_ports.begin();       

	for(i = 0; i < duplex.inProcessing.numChansInMax; i++, elmP++)
	{
		std::string str = jvx_size2String((*elmP)->serial);
		/* make an audio DSP input port */
		duplex.inProcessing.in_ports[i] = pw_filter_add_port(duplex.inProcessing.filter,
								 PW_DIRECTION_INPUT,
								 PW_FILTER_PORT_FLAG_MAP_BUFFERS,
								 sizeof(struct port),
								 pw_properties_new(
									 PW_KEY_FORMAT_DSP, "32 bit float mono audio",
									 PW_KEY_PORT_NAME, "input",
									 PW_KEY_NODE_AUTOCONNECT, "true",
									 PW_KEY_TARGET_OBJECT, str.c_str(),
									 NULL),
								 NULL, 0);
	}

	elmSS = theDevicehandle->sinks.begin();
	theNode = *elmSS;
	elmP = theNode->in_ports.begin();   
	for(i = 0; i < duplex.inProcessing.numChansOutMax; i++)
	{
		std::string str = jvx_size2String((*elmP)->serial);
		/* make an audio DSP output port */
		duplex.inProcessing.out_ports[i] = pw_filter_add_port(duplex.inProcessing.filter,
								  PW_DIRECTION_OUTPUT,
								  PW_FILTER_PORT_FLAG_MAP_BUFFERS,
								  sizeof(struct port),
								  pw_properties_new(
									  PW_KEY_FORMAT_DSP, "32 bit float mono audio",
									  PW_KEY_PORT_NAME, "output",	
								      PW_KEY_NODE_AUTOCONNECT, "true",
									  PW_KEY_TARGET_OBJECT, str.c_str(),
									  NULL),
								  NULL, 0);
	}

	params[0] = spa_process_latency_build(&b,
										  SPA_PARAM_ProcessLatency,
										  &duplex.lat_info);

	// params[1] = pw_properties_new(PW_KEY_TARGET_OBJECT, theDevicehandle->serial);
	/* Now connect this filter. We ask that our process function is
	 * called in a realtime thread. */
	int conn_result = pw_filter_connect(duplex.inProcessing.filter, PW_FILTER_FLAG_RT_PROCESS, params, 1);
	pw_thread_loop_unlock(parent->loop);
	if(conn_result < 0)
	{
		std::cout << __FUNCTION__ << "Error connecting the filter!" << std::endl;
		goto error;
	}
	return JVX_NO_ERROR;
error:
	return JVX_ERROR_INTERNAL;
}
	

// ==========================================================================================

jvxErrorType 
CjvxAudioPWireDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res =  CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	switch(theDevicehandle->opMode)
	{
		case operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_DUPLEX:
			assert(0);
			break;
		case operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_OUTPUT:
			res = stop_device_output();
			break;
		default:
			res = stop_device_input();
			break;
	}
	return res;
}


 void 
 CjvxAudioPWireDevice::process_buffer_duplex(struct spa_io_position *position)
 {
	jvxSize i;
	 uint32_t n_samples = position->clock.duration;

	 pw_log_trace("do process %d", n_samples);

	for(i = 0; i < duplex.inProcessing.numChansInMax; i++)
	{
 		duplex.inProcessing.in_data[i] = (float *)pw_filter_get_dsp_buffer(
			duplex.inProcessing.in_ports[i], n_samples);
	}
	
	for(i = 0; i < duplex.inProcessing.numChansOutMax; i++)
	{
 		duplex.inProcessing.out_data[i] = (float *)pw_filter_get_dsp_buffer(
			duplex.inProcessing.out_ports[i], n_samples);
	}
	
	int nChansMax = JVX_MAX(duplex.inProcessing.numChansInMax, 
		duplex.inProcessing.numChansOutMax);

	for(i = 0; i < nChansMax; i++)
	{
		jvxSize idxIn = i % duplex.inProcessing.numChansInMax;
		jvxSize idxOut = i % duplex.inProcessing.numChansOutMax;
		float* in = duplex.inProcessing.in_data[idxIn];
		float* out = duplex.inProcessing.out_data[idxOut];
		if(in && out)
		{
			memcpy(out, in, n_samples * sizeof(float));
		}
	}
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