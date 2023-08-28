//#include "CayfGlobalHostClass.h"
//CayfGlobalHostClass globalHostInstance;
#include "jvx.h"

extern "C"
{
	jvxErrorType ayf_register_module_host(const char* nm, jvxApiString& nmAsRegistered, IjvxObject* regMe, IjvxMinHost** hostOnReturn, IjvxConfigProcessor** cfgOnReturn)
	{
		return JVX_NO_ERROR;// globalHostInstance.register_module_host(nm, nmAsRegistered, regMe, hostOnReturn, cfgOnReturn);
	}

	jvxErrorType ayf_unregister_module_host(IjvxObject* regMe)
	{
		return JVX_NO_ERROR;//globalHostInstance.unregister_module_host(regMe);
	}

	jvxErrorType ayf_load_config_content(IjvxObject* priObj, jvxConfigData** datOnReturn, const char* fName)
	{
		return JVX_NO_ERROR;//globalHostInstance.load_config_content(priObj, datOnReturn, fName);
	}

	jvxErrorType ayf_release_config_content(IjvxObject* priObj, jvxConfigData* datOnReturn)
	{
		return JVX_NO_ERROR;//globalHostInstance.release_config_content(priObj, datOnReturn);
	}

	jvxErrorType ayf_attach_component_module(const char* nm, IjvxObject* priObj, IjvxObject* attachMe)
	{
		return JVX_NO_ERROR;//globalHostInstance.attach_component_module(nm,  priObj,  attachMe);
	}

	jvxErrorType ayf_detach_component_module(const char* nm, IjvxObject* priObj)
	{
		return JVX_NO_ERROR;//globalHostInstance.detach_component_module(nm, priObj);
	}

	jvxErrorType ayf_forward_text_command(const char* command, IjvxObject* priObj, jvxApiString& astr)
	{
		return JVX_NO_ERROR;//globalHostInstance.forward_text_command(command, priObj, astr);
	}
}
