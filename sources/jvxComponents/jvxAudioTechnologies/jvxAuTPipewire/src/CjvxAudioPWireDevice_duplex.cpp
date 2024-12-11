#include "CjvxAudioPWireDevice.h"
#include "CjvxAudioPWireTechnology.h"

#include <spa/pod/builder.h>
#include <pipewire/filter.h>

struct port 
{
    CjvxAudioPWireDevice *this_pointer;
};

static void on_process_duplex(void *userdata, struct spa_io_position *position)
{
	CjvxAudioPWireDevice* this_pointer = (CjvxAudioPWireDevice*)userdata;
	if(this_pointer)
	{
		this_pointer-> process_buffer_duplex_pw(position);
	}
};

static const struct pw_filter_events filter_events = {
        PW_VERSION_FILTER_EVENTS,
        .process = on_process_duplex,
};

jvxErrorType 
CjvxAudioPWireDevice::start_device_duplex_pw()
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

	// Hint: It seems that the dataformat is alwas just 32 bit float. I was not able to find any logic
	// which would allow other formats. Hence, I convert from and to float ALWAYS!!
	
	for(i = 0; i <  common.numChansInUsed; i++, elmP++)
	{
		// std::string str = jvx_size2String((*elmP)->serial);
		std::string chanName = filtername + "-input-" + jvx_size2String(i);

		/* make an audio DSP input port */
		duplex.inProcessing.in_ports[i] = pw_filter_add_port(duplex.inProcessing.filter,
								 PW_DIRECTION_INPUT,
								 PW_FILTER_PORT_FLAG_MAP_BUFFERS,
								 sizeof(struct port),
								 pw_properties_new(
									 PW_KEY_FORMAT_DSP, "32 bit float mono audio", //"32 bit float mono audio",
									 PW_KEY_PORT_NAME, chanName.c_str(),
									 //PW_KEY_NODE_AUTOCONNECT, "true",
									 //PW_KEY_TARGET_OBJECT, str.c_str(),
									 NULL),
								 NULL, 0);
	}

	elmSS = theDevicehandle->sinks.begin();
	theNode = *elmSS;
	elmP = theNode->in_ports.begin();   
	for(i = 0; i <  common.numChansOutUsed; i++)
	{
		// std::string str = jvx_size2String((*elmP)->serial);
		std::string chanName = filtername + "-output-" + jvx_size2String(i);

		/* make an audio DSP output port */
		duplex.inProcessing.out_ports[i] = pw_filter_add_port(duplex.inProcessing.filter,
								  PW_DIRECTION_OUTPUT,
								  PW_FILTER_PORT_FLAG_MAP_BUFFERS,
								  sizeof(struct port),
								  pw_properties_new(
									  PW_KEY_FORMAT_DSP, "32 bit float mono audio",
									  PW_KEY_PORT_NAME, chanName.c_str(),									      
									  NULL),
								  NULL, 0);
	}

	// Auto connect is not supported here. However, we might use the logic of the pw-link tool to copy 
	// and establish an auto link functionality. Check the code here:
	// 
	// https://github.com/PipeWire/pipewire/blob/master/src/tools/pw-link.c
	//

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
	
jvxErrorType 
CjvxAudioPWireDevice::stop_device_duplex_pw()
{
	pw_thread_loop_lock(common.loop);
	pw_filter_destroy(duplex.inProcessing.filter);
	pw_thread_loop_unlock(common.loop);
	return JVX_NO_ERROR;
}

void 
 CjvxAudioPWireDevice::process_buffer_duplex_pw(struct spa_io_position *position)
 {
	jvxSize i;
	 uint32_t n_samples = position->clock.duration;

	if(n_samples != common.bsize)
	{
		// std::cout << __FUNCTION__ << ": Framesize mismatch!" << std::endl;
		genPWire_device::properties_active_higher.num_lost_buffers.value++;
		return;
	}

	 // pw_log_trace("do process %d", n_samples);

	for(i = 0; i <  common.numChansInUsed; i++)
	{
 		duplex.inProcessing.in_data[i] = (float *)pw_filter_get_dsp_buffer(
			duplex.inProcessing.in_ports[i], n_samples);
	}
	
	for(i = 0; i <  common.numChansOutUsed; i++)
	{
 		duplex.inProcessing.out_data[i] = (float *)pw_filter_get_dsp_buffer(
			duplex.inProcessing.out_ports[i], n_samples);
	}
	
	// I am the master. I will start the chain here!
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		_common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
	}

	jvxData **bufsOut = (jvxData **)jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
	assert(common.numChansInUsed == _common_set_ocon.theData_out.con_params.number_channels);
	for (i = 0; i < common.numChansInUsed; i++)
	{
		if (jvx_bitTest(common.maskInput, i))
		{
			if (duplex.inProcessing.in_data[i])
			{
#ifdef JVX_DATA_FORMAT_FLOAT
				jvx_convertSamples_memcpy(
					duplex.inProcessing.in_data[i],
					bufsOut[cnt],
					sizeof(float),
					common.bsize);
#else
				jvx_convertSamples_from_float_to_data<float>(
					duplex.inProcessing.in_data[i],
					bufsOut[i],
					common.bsize,
					0, 1,
					0, 1);
#endif
			}
			else
			{
				memset(bufsOut[i], 0, sizeof(float) * common.bsize);
			}
		}
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

	for (i = 0; i < common.numChansOutMax; i++)
	{
		duplex.inProcessing.out_data[i] = nullptr;
	}	
 }


jvxErrorType 
CjvxAudioPWireDevice::process_buffer_icon_duplex(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;

	// This is the endpoint on the processing chain. We take the data that comes in and copy that to the
	// pw buffer reference!
	jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	assert(common.numChansOutUsed == _common_set_icon.theData_in->con_params.number_channels); 
	
	for (i = 0; i < common.numChansOutUsed; i++)
	{
		if (jvx_bitTest(common.maskOutput, i))
		{			
			if(duplex.inProcessing.out_data[i])
			{
#ifdef JVX_DATA_FORMAT_FLOAT
			jvx_convertSamples_memcpy(
				bufsIn[cnt],
				duplex.inProcessing.out_data[i],
				sizeof(float),
				common.bsize);
#else
			jvx_convertSamples_from_data_to_float<float>(
				bufsIn[i],
				duplex.inProcessing.out_data[i],
				common.bsize,
				0, 1,
				0, 1);			
#endif
			}
		}
	}
	return res;
}
	
