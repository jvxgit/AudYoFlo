#include "CjvxAudioSyncClockTechnology.h"
// =========================================================================================

CjvxAudioSyncClockTechnology::CjvxAudioSyncClockTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	LOCAL_TEMPLATE_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{	
	passThisInit = this;
}

CjvxAudioSyncClockTechnology::~CjvxAudioSyncClockTechnology()
{
}

// =========================================================================================

jvxErrorType
CjvxAudioSyncClockTechnology::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioSyncClockDevice>::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genSyncClock_technology::init_all();
		genSyncClock_technology::allocate_all();
		genSyncClock_technology::register_all(static_cast<CjvxProperties*>(this));
	}
	return res;
}

jvxErrorType
CjvxAudioSyncClockTechnology::unselect()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioSyncClockDevice>::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		genSyncClock_technology::unregister_all(static_cast<CjvxProperties*>(this));
		genSyncClock_technology::deallocate_all();
		res = CjvxTemplateTechnology<CjvxAudioSyncClockDevice>::unselect();
	}
	return res;
}

jvxErrorType
CjvxAudioSyncClockTechnology::activate()
{
	// "Jump" over the base class implementation 
	jvxErrorType res = _pre_check_activate();
	if (res == JVX_NO_ERROR)
	{
		numberDevicesInit = genSyncClock_technology::config_select.number_exposed.value;
		deviceNamePrefix = "SyncClockDevice";
		res = CjvxTemplateTechnology<CjvxAudioSyncClockDevice>::activate();
	}
	return(res);
};

jvxErrorType
CjvxAudioSyncClockTechnology::deactivate()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioSyncClockDevice>::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxTemplateTechnology<CjvxAudioSyncClockDevice>::deactivate();
	}
	return res;
}

// =======================================================================================

jvxErrorType
CjvxAudioSyncClockTechnology::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno )
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warns;
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		genSyncClock_technology::put_configuration__config_select(callMan, processor, sectionToContainAllSubsectionsForMe);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioSyncClockTechnology::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	genSyncClock_technology::get_configuration__config_select(callMan, processor, sectionWhereToAddAllSubsections);
	return JVX_NO_ERROR;
}
