#ifndef _AYF_CONNECTION_HOST_H__
#define _AYF_CONNECTION_HOST_H__
#include "jvx.h"

extern "C"
{
	jvxErrorType ayf_register_object_host(const char* nm, jvxApiString& nmAsRegistered, IjvxObject* regMe, 
		IjvxMinHost** hostOnReturn, IjvxConfigProcessor** confProcReturn);
	jvxErrorType ayf_unregister_object_host(IjvxObject* regMe);

	jvxErrorType ayf_load_config_content(IjvxObject* priObj, jvxConfigData** datOnReturn, const char* fName);
	jvxErrorType ayf_release_config_content(IjvxObject* priObj, jvxConfigData* datOnReturn);

	jvxErrorType ayf_attach_component_module(const char* nm, IjvxObject* priObj, IjvxObject* attachMe);
	jvxErrorType ayf_detach_component_module(const char* nm, IjvxObject* priObj);
	jvxErrorType ayf_forward_text_command(const char* command, IjvxObject* priObj, jvxApiString& astr);
};

#endif