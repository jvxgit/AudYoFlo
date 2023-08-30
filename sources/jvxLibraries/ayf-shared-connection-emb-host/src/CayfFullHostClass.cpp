#include "CayfFullHostClass.h"
#include "CjvxHostJsonDefines.h"
#include "jvxMHAppMinHostCon.h"

#include "jvxTconfigProcessor.h"

#include "jvx-core-host-loop.h"

static std::string combineWithCwd(const std::string& fName)
{
	std::string fNameRet = ".";
	char wDir[JVX_MAXSTRING] = { 0 };
	if (JVX_GETCURRENTDIRECTORY(wDir, JVX_MAXSTRING))
	{
		fNameRet = wDir;
		fNameRet += JVX_SEPARATOR_DIR + fName;
	}
	return fNameRet;
}

CayfFullHostClass::CayfFullHostClass() : CjvxHostJsonCommandsProperties(config_show)
{
	
}

CayfFullHostClass::~CayfFullHostClass()
{
	
}

JVX_THREAD_ENTRY_FUNCTION(runHost, param)
{
	// void parameter param
	CayfFullHostClass* this_pointer = (CayfFullHostClass*)param;
	if (this_pointer)
	{
		return this_pointer->mainThreadLoop();
	}
	return JVX_THREAD_EXIT_ERROR;
}

jvxErrorType 
CayfFullHostClass::register_factory_host(const char* nm, jvxApiString& nmAsRegistered, IjvxExternalModuleFactory* regMe, int argc, const char* argv[])
{
	if (hostStarted == false)
	{
		jvxSize i;
		for (i = 0; i < argc; i++)
		{
			astrlArgv.add(argv[i]);
		}
		JVX_CREATE_THREAD(hdlHostThread, runHost, this, idHostThread);
		hostStarted = true;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::unregister_factory_host(IjvxExternalModuleFactory* regMe)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hostStarted)
	{
		jvxErrorType res = jvx_core_host_loop_stop();
		assert(res == JVX_NO_ERROR);
		JVX_WAIT_FOR_THREAD_TERMINATE_INF(hdlHostThread);
	}
	
	return res;
}

int
CayfFullHostClass::mainThreadLoop()
{
	const char** argv = nullptr;
	int argc = 1 + astrlArgv.ll();
	if (argc)
	{
		jvxSize i;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(argv, const char*, argc);
		argv[0] = "CayfFullHostClass";
		for (i = 1; i < argc; i++)
		{
			argv[i] = astrlArgv.c_str_at(i - 1);
		}
	}

	jvx_core_host_loop(argc, (char**)argv);
	return JVX_THREAD_EXIT_NORMAL;
}

/*
jvxErrorType 
CayfFullHostClass::load_config_content(IjvxObject* priObj, jvxConfigData** datOnReturn, const char* fName)
{
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CayfFullHostClass::release_config_content(IjvxObject* priObj, jvxConfigData* datOnReturn)
{
	
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CayfFullHostClass::attach_component_module(const char* nm, IjvxObject* priObj, IjvxObject* attachMe)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	
	return res;
}

jvxErrorType 
CayfFullHostClass::detach_component_module(const char* nm, IjvxObject* priObj)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	
	return res;
}

jvxErrorType 
CayfFullHostClass::forward_text_command(const char* command, IjvxObject* priObj, jvxApiString& astr)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	
	return res;
}

/*
 * 
 * This is the base class callback to obtain a pointer to the properties object if we are running without
 * component location. The pointer <cpOfInterest> must be set before the actual call to 
 * any of the functions
 * - show_property_list
 * - act_get_property_component_core
 * - act_set_property_component_core
 * If the pointer is nullptr, the returned field should always be like "no pproperties"
 */
jvxErrorType 
CayfFullHostClass::requestReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp)
{
	
	return JVX_NO_ERROR;

}

jvxErrorType 
CayfFullHostClass::returnReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp)
{
	
	return JVX_NO_ERROR;
}

// ================================================================================
// ================================================================================

jvxErrorType 
CayfFullHostClass::invite_load_components_active(IjvxHost* hostRef)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::finalize_unload_components_active(IjvxHost* hostRef)
{
	return JVX_NO_ERROR;
}

/*
std::string
CayfFullHostClass::processTextMessage(std::string txt, oneEntryHostList& entry)
{
	std::string response;
	return response;
}

jvxConfigData* 
CayfFullHostClass::configOneObject(const std::string& nm, IjvxObject* theObj)
{
	jvxConfigData* retPtr = nullptr;
	return retPtr;
}
*/
