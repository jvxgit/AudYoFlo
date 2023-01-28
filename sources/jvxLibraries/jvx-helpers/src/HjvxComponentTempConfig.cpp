#include "jvx-helpers.h"

CjvxComponentTempConfig::CjvxComponentTempConfig()
{
	theRealHost = NULL;
	theConfigProc_obj = NULL;
	theConfigProc = NULL;
	theHostRef = NULL;
}

jvxErrorType
CjvxComponentTempConfig::init_tc(IjvxHiddenInterface* theHost)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification cpTp = JVX_COMPONENT_CONFIG_PROCESSOR;
	theHostRef = theHost;

	theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theToolsHost));
	if (theToolsHost)
	{
		res = theToolsHost->reference_tool(cpTp, &theConfigProc_obj, 0, NULL);
		if (res == JVX_NO_ERROR)
		{
			theConfigProc_obj->request_specialization(reinterpret_cast<jvxHandle**>(&theConfigProc), NULL, NULL);

			res = theHostRef->request_hidden_interface(JVX_INTERFACE_HOST,
				reinterpret_cast<jvxHandle**>(&theRealHost));
			if (res != JVX_NO_ERROR)
			{
				terminate_tc();
			}
		}
		else
		{
			terminate_tc();
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_SETTING;
	}
	return res;
}

jvxErrorType 
CjvxComponentTempConfig::put_configuration_tc(IjvxConfiguration* cfgComp, const char* description_comp)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString fldStr;
	jvxConfigData* cfg = NULL;
	std::string token = "stored-locally";
	jvxCallManagerConfiguration confCall;
	if (theRealHost && theConfigProc && cfgComp)
	{
		res = theRealHost->copy_config(description_comp, &fldStr);
		if (res == JVX_NO_ERROR)
		{
			theConfigProc->parseTextField(fldStr.c_str(), token.c_str(), 0);
			theConfigProc->getConfigurationHandle(&cfg);
			if (cfg)
			{
				confCall.call_purpose = JVX_CONFIGURATION_PURPOSE_CONFIG_ON_COMPONENT_STARTSTOP;
				res = cfgComp->put_configuration(&confCall, 
					theConfigProc, cfg, token.c_str(), 0);
			}
			theConfigProc->removeHandle(cfg);
			// Read in configuration
		}
	}
	else
	{
		res = JVX_ERROR_NOT_READY;
	}
	return res;
}

jvxErrorType
CjvxComponentTempConfig::get_configuration_tc(IjvxConfiguration* cfgComp, const char* description_comp)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerConfiguration confCall;
	if (theConfigProc && theRealHost)
	{
		jvxConfigData* cfgData = NULL;
		jvxApiString fldStr;
		theConfigProc->createEmptySection(&cfgData, "MAIN_SECTION");
		if (cfgData)
		{
			res = cfgComp->get_configuration(&confCall, theConfigProc, cfgData);
			if (res == JVX_NO_ERROR)
			{
				confCall.call_purpose = JVX_CONFIGURATION_PURPOSE_CONFIG_ON_COMPONENT_STARTSTOP;
				theConfigProc->printConfiguration(cfgData, &fldStr, true);
				if (theRealHost)
				{
					res = theRealHost->store_config(description_comp, fldStr.c_str(), true);
				}
				theConfigProc->removeHandle(cfgData);
			}
		}
	}
	else
	{
		res = JVX_ERROR_NOT_READY;
	}
	return res;
}

jvxErrorType
CjvxComponentTempConfig::terminate_tc()
{
	if (theConfigProc_obj)
	{
		jvxComponentIdentification cpTp = JVX_COMPONENT_CONFIG_PROCESSOR;
		theToolsHost->return_reference_tool(cpTp, theConfigProc_obj);
	}
	theConfigProc_obj = NULL;
	theConfigProc = NULL;

	if (theRealHost)
	{
		theHostRef->return_hidden_interface(JVX_INTERFACE_HOST,
			reinterpret_cast<jvxHandle*>(theRealHost));
	}
	theRealHost = NULL;

	if (theToolsHost)
	{
		theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST,
			reinterpret_cast<jvxHandle*>(theToolsHost));
	}
	theToolsHost = NULL;

	theHostRef = NULL;

	return JVX_NO_ERROR;
}