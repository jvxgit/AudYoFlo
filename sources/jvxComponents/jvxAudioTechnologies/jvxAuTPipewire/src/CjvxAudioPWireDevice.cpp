#include "CjvxAudioPWireDevice.h"
#include "CjvxAudioPWireTechnology.h"

#include <spa/pod/builder.h>
#include <pipewire/filter.h>

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
	std::string str = _common_set.theName + "-loop";
	loop_dev = pw_thread_loop_new(str.c_str(), NULL /* properties */);
	pw_thread_loop_start(loop_dev);
	_common_set_ocon.theData_out.con_data.number_buffers = 1;
	return CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}
	
jvxErrorType 
CjvxAudioPWireDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	_common_set_ocon.theData_out.con_data.number_buffers = 0;
	pw_thread_loop_stop(loop_dev);
	pw_thread_loop_destroy(loop_dev);
}

jvxErrorType 
CjvxAudioPWireDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		
	// Limitation: I can not provide a samplerate on the filter object as it seems!
	// https://gitlab.freedesktop.org/pipewire/pipewire/-/issues/4410

	common.numChansInMax = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.size();
	common.numChansOutMax = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.size();

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

// ===========================================================================
// ===========================================================================

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
