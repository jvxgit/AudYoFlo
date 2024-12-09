#include "CjvxAudioPWireDevice.h"
#include "CjvxAudioPWireTechnology.h"

#include <spa/pod/builder.h>
#include <pipewire/filter.h>

struct port 
{
    CjvxAudioPWireDevice *this_pointer;
};

static void on_process(void *userdata, struct spa_io_position *position)
{
	CjvxAudioPWireDevice* this_pointer = (CjvxAudioPWireDevice*)userdata;
	if(this_pointer)
	{
		this_pointer-> process_buffer(position);
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

	uint8_t buffer[1024];
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
	std::string filtername = _common_set.theName + "-filter";
	const struct spa_pod *params[2];

	inProcessing.numChansInMax = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.size();
	inProcessing.numChansOutMax = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.size();

	pw_thread_loop_lock(parent->loop);

	inProcessing.filter = pw_filter_new_simple(
		pw_thread_loop_get_loop(parent->loop),
		filtername.c_str(),
		pw_properties_new(
			PW_KEY_MEDIA_TYPE, "Audio",
			PW_KEY_MEDIA_CATEGORY, "Filter",
			PW_KEY_MEDIA_ROLE, "DSP",
			NULL),
		&filter_events,
		this);

	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(inProcessing.in_ports, void*, inProcessing.numChansInMax);
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(inProcessing.out_ports, void*, inProcessing.numChansOutMax);

	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(inProcessing.in_data, float*, inProcessing.numChansInMax);
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(inProcessing.out_data, float*, inProcessing.numChansOutMax);

	auto elmSS = theDevicehandle->sources.begin();
   	oneNode *theNode = *elmSS;
   	auto elmP = theNode->out_ports.begin();       

	for(i = 0; i < inProcessing.numChansInMax; i++, elmP++)
	{
		std::string str = jvx_size2String((*elmP)->serial);
		/* make an audio DSP input port */
		inProcessing.in_ports[i] = pw_filter_add_port(inProcessing.filter,
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
	for(i = 0; i < inProcessing.numChansOutMax; i++)
	{
		std::string str = jvx_size2String((*elmP)->serial);
		/* make an audio DSP output port */
		inProcessing.out_ports[i] = pw_filter_add_port(inProcessing.filter,
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
										  &lat_info);

	// params[1] = pw_properties_new(PW_KEY_TARGET_OBJECT, theDevicehandle->serial);
	/* Now connect this filter. We ask that our process function is
	 * called in a realtime thread. */
	int conn_result = pw_filter_connect(inProcessing.filter, PW_FILTER_FLAG_RT_PROCESS, params, 1);
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
	
jvxErrorType 
CjvxAudioPWireDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}


 void 
 CjvxAudioPWireDevice::process_buffer(struct spa_io_position *position)
 {
	jvxSize i;
	 uint32_t n_samples = position->clock.duration;

	 pw_log_trace("do process %d", n_samples);

	for(i = 0; i < inProcessing.numChansInMax; i++)
	{
 		inProcessing.in_data[i] = (float *)pw_filter_get_dsp_buffer(inProcessing.in_ports[i], n_samples);
	}
	
	for(i = 0; i < inProcessing.numChansOutMax; i++)
	{
 		inProcessing.out_data[i] = (float *)pw_filter_get_dsp_buffer(inProcessing.out_ports[i], n_samples);
	}
	
	int nChansMax = JVX_MAX(inProcessing.numChansInMax, inProcessing.numChansOutMax);
	for(i = 0; i < nChansMax; i++)
	{
		jvxSize idxIn = i % inProcessing.numChansInMax;
		jvxSize idxOut = i % inProcessing.numChansOutMax;
		float* in = inProcessing.in_data[idxIn];
		float* out = inProcessing.out_data[idxOut];
		if(in && out)
		{
			memcpy(out, in, n_samples * sizeof(float));
		}
	}
 }