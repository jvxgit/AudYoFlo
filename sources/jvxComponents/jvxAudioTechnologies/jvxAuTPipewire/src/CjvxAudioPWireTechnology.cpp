#include "CjvxAudioPWireTechnology.h"

CjvxAudioPWireTechnology::CjvxAudioPWireTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	// Add one proxy device
	numberDevicesInit = 1;
	deviceNamePrefix = "generic";
	JVX_INITIALIZE_NOTIFICATION(async_run.notWait);
}

CjvxAudioPWireTechnology::~CjvxAudioPWireTechnology()
{
	JVX_TERMINATE_NOTIFICATION(async_run.notWait);
}

// =============================================================================================

jvxErrorType 
CjvxAudioPWireTechnology::activate()
{
	jvxErrorType res = CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>::activate();
	if (res == JVX_NO_ERROR)
	{
		genPWire_technology::init_all();
		genPWire_technology::allocate_all();
		genPWire_technology::register_all(static_cast<CjvxProperties*>(this));
	}
	return res;
}

jvxErrorType 
CjvxAudioPWireTechnology::deactivate()
{
	jvxErrorType res = CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genPWire_technology::unregister_all(static_cast<CjvxProperties*>(this));
		genPWire_technology::deallocate_all();
		
		CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>::deactivate();
	}
	return res;
}

jvxErrorType 
CjvxAudioPWireTechnology::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>::put_configuration(callMan,
		processor, sectionToContainAllSubsectionsForMe, filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		genPWire_technology::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe);	
	}
	return res;
}

jvxErrorType 
CjvxAudioPWireTechnology::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>::get_configuration(callMan, processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genPWire_technology::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	}
	return res;
}

CjvxAudioPWireDevice* 
CjvxAudioPWireTechnology::local_allocate_device(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxSize idx, jvxBool actAsProxy_init, jvxHandle* fwd_arg)
{
	CjvxAudioPWireDevice* newDevice = CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>::local_allocate_device(
		JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL,  idx, actAsProxy_init,  fwd_arg);
	
	oneDevice* devRef = nullptr;
	 if(fwd_arg)
	{
		initializeDeviceParams* args = (initializeDeviceParams*)fwd_arg;		
		devRef = args->theDevicehandle;
	}
	else
	{
		if(!actAsProxy_init)
		{
			jvxSize i;
			oneDevice* theDev = nullptr;
			JVX_SAFE_ALLOCATE_OBJECT(theDev, oneDevice);
			theDev->name = description;
			theDev->id = JVX_SIZE_UNSELECTED;
			theDev->opMode == operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_DUPLEX;
			oneNode* theNode = nullptr;
			JVX_SAFE_ALLOCATE_OBJECT(theNode, oneNode);
			theNode->isSink = false;
			theNode->description = description;
			theNode->description += "-in";
			
			for(i = 0; i < genPWire_technology::properties_active.num_input_channels_generic_device.value; i++)
			{
				onePort* thePort = nullptr;
				JVX_SAFE_ALLOCATE_OBJECT(thePort, onePort);
				thePort->direction = "input";
				thePort->physical = "true";
				thePort->name = description;
				thePort->name += "-in" + jvx_size2String(i);
				thePort->port_id = i;
				theNode->out_ports.push_back(thePort);				
			}
			theDev->sources.push_back(theNode);
			
			theNode = nullptr;
			JVX_SAFE_ALLOCATE_OBJECT(theNode, oneNode);
			theNode->isSink = true;
			theNode->description = description;
			theNode->description += "-output";
			for(i = 0; i < genPWire_technology::properties_active.num_output_channels_generic_device.value; i++)
			{
				onePort* thePort = nullptr;
				JVX_SAFE_ALLOCATE_OBJECT(thePort, onePort);
				thePort->direction = "output";
				thePort->physical = "true";
				thePort->name = description;
				thePort->name += "-out" + jvx_size2String(i);
				thePort->port_id = i;
				theNode->in_ports.push_back(thePort);				
			}
			theDev->sinks.push_back(theNode);
			devRef = theDev;
		}
	}
	newDevice->set_references_api(devRef, this);
	return newDevice;
}

jvxErrorType 
CjvxAudioPWireTechnology::local_deallocate_device(CjvxAudioPWireDevice** elmDev, jvxHandle* fwd_arg)
{
	oneDevice* theDevice = (*elmDev)->references_api();
	if(JVX_CHECK_SIZE_UNSELECTED(theDevice->id))
	{
		JVX_SAFE_DELETE_OBJ(theDevice);
	}
	return CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>::local_deallocate_device(elmDev, fwd_arg);
}
