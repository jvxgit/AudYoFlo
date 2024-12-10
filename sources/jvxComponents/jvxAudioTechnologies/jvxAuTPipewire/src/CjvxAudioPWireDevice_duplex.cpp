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
		this_pointer-> process_buffer_duplex(position);
	}
};

static const struct pw_filter_events filter_events = {
        PW_VERSION_FILTER_EVENTS,
        .process = on_process,
};

jvxErrorType 
CjvxAudioPWireDevice::start_device_duplex()
{
	jvxSize i;
	std::string filtername = _common_set.theName + "-filter";
	const struct spa_pod *params[2];

	uint8_t buffer[1024];
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

	common.loop = parent->loop_tech;
	if(loopLocal && loop_dev)	
	{
		common.loop = loop_dev;
	}

	pw_thread_loop_lock(common.loop);

	duplex.inProcessing.filter = pw_filter_new_simple(
		pw_thread_loop_get_loop(common.loop),
		filtername.c_str(),
		pw_properties_new(
			PW_KEY_MEDIA_TYPE, "Audio",
			PW_KEY_MEDIA_CATEGORY, "Filter",
			PW_KEY_MEDIA_ROLE, "DSP",
			NULL),
		&filter_events,
		this);

	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(duplex.inProcessing.in_ports, void*, common.numChansInMax);
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(duplex.inProcessing.out_ports, void*,  common.numChansOutMax);
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(duplex.inProcessing.in_data, float*,  common.numChansInMax);
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(duplex.inProcessing.out_data, float*,  common.numChansOutMax);

	auto elmSS = theDevicehandle->sources.begin();
   	oneNode *theNode = *elmSS;
   	auto elmP = theNode->out_ports.begin();       

	for(i = 0; i <  common.numChansInMax; i++, elmP++)
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
	for(i = 0; i <  common.numChansOutMax; i++)
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
	pw_thread_loop_unlock(common.loop);
	if(conn_result < 0)
	{
		std::cout << __FUNCTION__ << "Error connecting the filter!" << std::endl;
		goto error;
	}
	return JVX_NO_ERROR;
error:
	return JVX_ERROR_INTERNAL;
}
	
void 
 CjvxAudioPWireDevice::process_buffer_duplex(struct spa_io_position *position)
 {
	jvxSize i;
	 uint32_t n_samples = position->clock.duration;

	 pw_log_trace("do process %d", n_samples);

	for(i = 0; i <  common.numChansInMax; i++)
	{
 		duplex.inProcessing.in_data[i] = (float *)pw_filter_get_dsp_buffer(
			duplex.inProcessing.in_ports[i], n_samples);
	}
	
	for(i = 0; i <  common.numChansOutMax; i++)
	{
 		duplex.inProcessing.out_data[i] = (float *)pw_filter_get_dsp_buffer(
			duplex.inProcessing.out_ports[i], n_samples);
	}
	
	int nChansMax = JVX_MAX(common.numChansInMax, common.numChansOutMax);

	for(i = 0; i < nChansMax; i++)
	{
		jvxSize idxIn = i %  common.numChansInMax;
		jvxSize idxOut = i %  common.numChansOutMax;
		float* in = duplex.inProcessing.in_data[idxIn];
		float* out = duplex.inProcessing.out_data[idxOut];
		if(in && out)
		{
			memcpy(out, in, n_samples * sizeof(float));
		}
	}
 }
