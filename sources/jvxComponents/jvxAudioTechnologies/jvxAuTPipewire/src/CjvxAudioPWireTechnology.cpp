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
