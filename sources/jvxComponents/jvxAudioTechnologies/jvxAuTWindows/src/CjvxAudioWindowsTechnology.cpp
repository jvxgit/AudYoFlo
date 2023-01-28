#include "CjvxAudioWindowsTechnology.h"


// =========================================================================================

CjvxAudioWindowsTechnology::CjvxAudioWindowsTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxTemplateTechnology<CjvxAudioWindowsDevice>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	this->numberDevicesInit = 0;
	this->deviceNamePrefix = "AudioDevice";
	JVX_INITIALIZE_MUTEX(safeAccessConnections);
	
}

CjvxAudioWindowsTechnology::~CjvxAudioWindowsTechnology()
{
	JVX_TERMINATE_MUTEX(safeAccessConnections);
}

jvxErrorType
CjvxAudioWindowsTechnology::activate()
{
	// "Jump" over the base class implementation 
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioWindowsDevice>::activate();
	if (res == JVX_NO_ERROR)
	{
		genWindows_technology::init_all();
		genWindows_technology::allocate_all();
		genWindows_technology::register_all(static_cast<CjvxProperties*>(this));

		activate_windows_audio_technology();

		//update_capabilities(); 
	}
	return(res);
};

jvxErrorType
CjvxAudioWindowsTechnology::deactivate()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioWindowsDevice>::deactivate();
	deactivate_windows_audio_technology();
	genWindows_technology::unregister_all(static_cast<CjvxProperties*>(this)); 
	genWindows_technology::deallocate_all();	
	return res;
}

jvxErrorType 
CjvxAudioWindowsTechnology::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename, jvxInt32 lineno)
{
	std::vector<std::string> warnings;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		genWindows_technology::put_configuration_all(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		if (!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for (i = 0; i < warnings.size(); i++)
			{
				txt = __FUNCTION__;
				txt += ": Failed to read property " + warnings[i];
				_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioWindowsTechnology::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	genWindows_technology::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);
	return JVX_NO_ERROR;
}