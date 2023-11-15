#ifndef __AYF_EMBEDDING_PROXY_H__
#define __AYF_EMBEDDING_PROXY_H__

#include "jvx.h"

enum class ayfHostBindingType
{
	// There is no host at all!!
	AYF_HOST_BINDING_NONE,

	// There is a very minor host binding
	AYF_HOST_BINDING_MIN_HOST,

	// There is a full host binding but embedded somewhere
	AYF_HOST_BINDING_EMBEDDED_HOST,

	// There is a full host binding
	// AYF_HOST_BINDING_FULL_HOST,
};

typedef jvxErrorType(*ayf_register_object_host)(const char* nm, jvxApiString& nmAsRegistered, IjvxObject* regMe,
	IjvxMinHost** hostOnReturn, IjvxConfigProcessor** cfgOnReturn);
typedef jvxErrorType(*ayf_unregister_object_host)(IjvxObject* regMe);
typedef jvxErrorType(*ayf_load_config_content)(IjvxObject* priObj, jvxConfigData** datOnReturn, const char* fName);
typedef jvxErrorType(*ayf_release_config_content)(IjvxObject* priObj, jvxConfigData* datOnReturn);
typedef jvxErrorType(*ayf_attach_component_module)(const char* nm, IjvxObject* priObj, IjvxObject* attachMe);
typedef jvxErrorType(*ayf_detach_component_module)(const char* nm, IjvxObject* priObj);
typedef jvxErrorType(*ayf_forward_text_command)(const char* command, IjvxObject* priObj, jvxApiString& astr);

typedef jvxErrorType(*ayf_register_factory_host)(const char* nm, jvxApiString& nmAsRegistered,
	IjvxExternalModuleFactory* regMe, int argc, const char* argv[]);
typedef jvxErrorType(*ayf_unregister_factory_host)(IjvxExternalModuleFactory* regMe);
typedef jvxErrorType(*ayf_load_config_content_factory_host)(IjvxExternalModuleFactory* regObj, jvxConfigData** datOnReturn, const char* fName);
typedef jvxErrorType(*ayf_release_config_content_factory_host)(IjvxExternalModuleFactory* regObj, jvxConfigData* datOnReturn);

class ayfHostBindingReferences
{
public:
	ayfHostBindingType bindType = ayfHostBindingType::AYF_HOST_BINDING_NONE;
	virtual jvxErrorType request_specialization(jvxHandle** ptrOnReturn, ayfHostBindingType checkMe)
	{
		if (ptrOnReturn)
		{
			*ptrOnReturn = nullptr;
		}
		if (checkMe != bindType)
		{
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		if (ptrOnReturn)
		{
			*ptrOnReturn = reinterpret_cast<jvxHandle*>(this);
		}
		return JVX_NO_ERROR;
	};
};

class ayfHostBindingReferencesTxtCommand
{
public:
	// Forward a text token to the host from where the appropriate action is taken
	ayf_forward_text_command ayf_forward_text_command_call = nullptr;
};

class ayfHostBindingReferencesMinHost: public ayfHostBindingReferences, public ayfHostBindingReferencesTxtCommand
{
public:
	// ==============================================================
	// These functions for the min host configuration
	// ==============================================================

	// An external module is added to host if allowed
	ayf_register_object_host ayf_register_object_host_call = nullptr;

	// Unregister external module
	ayf_unregister_object_host ayf_unregister_object_host_call = nullptr;

	// Load config content from host - in most cases due to a postponed load of the module/component
	ayf_load_config_content ayf_load_config_content_call = nullptr;

	// Release the config content
	ayf_release_config_content ayf_release_config_content_call = nullptr;

	// Function to attach another component - the secondary component is associated to the first object
	ayf_attach_component_module ayf_attach_component_module_call = nullptr;

	// Detach the previously attached component
	ayf_detach_component_module ayf_detach_component_module_call = nullptr;

	virtual jvxErrorType request_specialization(jvxHandle** ptrOnReturn, ayfHostBindingType checkMe) override
	{
		if (ptrOnReturn)
		{
			*ptrOnReturn = nullptr;
		}

		if (checkMe != bindType)
		{
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		if (ptrOnReturn)
		{
			*ptrOnReturn = reinterpret_cast<jvxHandle*>(this);
		}
		return JVX_NO_ERROR;
	}
};

class ayfHostBindingReferencesEmbHost: public ayfHostBindingReferences, public ayfHostBindingReferencesTxtCommand
{
public:
	jvxApiStringList argsFullHost;

	// ==============================================================
	// These functions for the embedded host configuration
	// ==============================================================

	// Add an interface to register a factory for component integration
	ayf_register_factory_host ayf_register_factory_host_call = nullptr;

	// Add an interface to register a factory for component integration
	ayf_unregister_factory_host ayf_unregister_factory_host_call = nullptr;

	// Load config content from host - in most cases due to a postponed load of the module/component
	ayf_load_config_content_factory_host ayf_load_config_content_factory_host_call = nullptr;

	// Release the config content
	ayf_release_config_content_factory_host ayf_release_config_content_factory_host_call = nullptr;

	virtual jvxErrorType request_specialization(jvxHandle** ptrOnReturn, ayfHostBindingType checkMe) override
	{
		if (ptrOnReturn)
		{
			*ptrOnReturn = nullptr;
		}
		if (checkMe != bindType)
		{
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		if (ptrOnReturn)
		{
			*ptrOnReturn = reinterpret_cast<jvxHandle*>(this);
		}
		return JVX_NO_ERROR;
	}
};


#endif