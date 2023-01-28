#ifndef __CAYFGLOBALHOSTCLASS_H__
#define __CAYFGLOBALHOSTCLASS_H__

#include "jvx.h"
#include "jvxHCon.h"
#include "CjvxHostJsonCommandsProperties.h"
#include "CjvxHostJsonCommandsConfigShow.h"

class CayfGlobalHostClass: public CjvxHostJsonCommandsProperties
{
public:

	class oneEntryAttachedComponent
	{
	public:
		std::string nm;
		jvxBool includeConfig = true;
		IjvxObject* theObj = nullptr;
	};
		
	class oneEntryHostList
	{
	public:
		IjvxObject* obj;
		std::string nmRegistered;
		std::string cfgFName;
		std::list<std::string> cfgAddPaths;
		jvxConfigData* cfgHandleInUse = nullptr;
		jvxErrorType resConfig = JVX_ERROR_NOT_READY;
		std::string errTxt;
		std::map<std::string, oneEntryAttachedComponent> attachedModules;
	};

	std::map<IjvxObject*, oneEntryHostList> registeredModules;

	IjvxObject* globalHostObj = nullptr;
	IjvxMinHost* globalHostRef = nullptr;

	IjvxObject* confProcObj = nullptr;
	IjvxConfigProcessor* confProcHdl = nullptr;

	// This pointer is reserved for the functions in the <CjvxHostJsonCommandsProperties>
	// base class to address a component if we are not in the scheme with components located 
	// in the component grid.
	IjvxObject* cpOfInterest = nullptr;

	CjvxHostJsonCommandsConfigShow config_show;

	CayfGlobalHostClass(); 
	~CayfGlobalHostClass();

	jvxErrorType register_module_host(const char* nm, jvxApiString& nmAsRegistered, IjvxObject* regMe, IjvxMinHost** hostOnReturn, IjvxConfigProcessor** cfgOnReturn);
	jvxErrorType unregister_module_host(IjvxObject* regMe);

	jvxErrorType load_config_content(IjvxObject* priObj, jvxConfigData** datOnReturn, const char* fName);
	jvxErrorType release_config_content(IjvxObject* priObj, jvxConfigData* datOnReturn);

	jvxErrorType attach_component_module(const char* nm, IjvxObject* priObj, IjvxObject* attachMe);
	jvxErrorType detach_component_module(const char* nm, IjvxObject* priObj);
	jvxErrorType forward_text_command(const char* command, IjvxObject* priObj, jvxApiString& astr);

	std::string processTextMessage(std::string txt, oneEntryHostList& entry);// std::map<std::string, oneEntryAttachedComponent>& attachedModules);
	jvxConfigData* configOneObject(const std::string& nm, IjvxObject* theObj);

	virtual jvxErrorType requestReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp) override;
	virtual jvxErrorType returnReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp) override;
};

#endif
