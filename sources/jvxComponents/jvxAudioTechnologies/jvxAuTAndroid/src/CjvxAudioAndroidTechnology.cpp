#include "CjvxAudioAndroidTechnology.h"
// =========================================================================================

CjvxAudioAndroidTechnology::CjvxAudioAndroidTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	LOCAL_TEMPLATE_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{	
	passThisInit = this;
}

CjvxAudioAndroidTechnology::~CjvxAudioAndroidTechnology()
{
}

// =========================================================================================

jvxErrorType
CjvxAudioAndroidTechnology::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genAndroid_technology::init_all();
		genAndroid_technology::allocate_all();
		genAndroid_technology::register_all(static_cast<CjvxProperties*>(this));
	}
	return res;
}

jvxErrorType
CjvxAudioAndroidTechnology::unselect()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		genAndroid_technology::unregister_all(static_cast<CjvxProperties*>(this));
		genAndroid_technology::deallocate_all();
		res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::unselect();
	}
	return res;
}

jvxErrorType
CjvxAudioAndroidTechnology::activate()
{
	// "Jump" over the base class implementation 
	jvxErrorType res = _pre_check_activate();
	if (res == JVX_NO_ERROR)
	{
		numberDevicesInit = genAndroid_technology::config_select.number_exposed.value;
		deviceNamePrefix = "SyncClockDevice";
		res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::activate();
	}
	return(res);
};

jvxErrorType
CjvxAudioAndroidTechnology::deactivate()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxTemplateTechnology<CjvxAudioAndroidDevice>::deactivate();
	}
	return res;
}

// =======================================================================================

jvxErrorType
CjvxAudioAndroidTechnology::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno )
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warns;
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		genAndroid_technology::put_configuration__config_select(callMan, processor, sectionToContainAllSubsectionsForMe);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioAndroidTechnology::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	genAndroid_technology::get_configuration__config_select(callMan, processor, sectionWhereToAddAllSubsections);
	return JVX_NO_ERROR;
}
