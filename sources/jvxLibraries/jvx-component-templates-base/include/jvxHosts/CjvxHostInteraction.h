#ifndef _CJVXHOSTINTERACTION_H__
#define _CJVXHOSTINTERACTION_H__

#include "common/CjvxObject.h"
#include <map>

template <class T> class oneObj
{
public:
	jvxOneComponentModuleWrap common;
	T* theHandle_single;// In case multiple objects are not allowed, always return this, otherwise, always return a new instance
	oneObj()
	{
		theHandle_single = NULL;
	};
};

template <class T> class oneExternalObj
{
public:
	std::string moduleName;
	T* theHandle_single;// In case multiple objects are not allowed, always return this, otherwise, always return a new instance
	oneExternalObj()
	{
		theHandle_single = NULL;
	};
};

template <class T> class oneSelectedObj
{
public:
	T* obj = nullptr;
	jvxSize idSel = JVX_SIZE_UNSELECTED;
	jvxSize uid = JVX_SIZE_UNSELECTED;
	std::string nmExternal;
	IjvxConnectorFactory* cfac = nullptr;
	IjvxConnectionMasterFactory* mfac = nullptr;
	oneSelectedObj()
	{
		obj = NULL;
		idSel = JVX_SIZE_UNSELECTED;
		cfac = NULL;
		mfac = NULL;
	};
};

template <class T> class objT
{
public:
	std::vector<oneObj<T>> availableEndpoints;
	std::vector<oneExternalObj<T>> externalEndpoints;
	std::vector<oneSelectedObj<T>> theHandle_shortcut;
	jvxSize numSlotsMax = 0;
	objT()
	{
		numSlotsMax = 0;
	};
};

template <typename T> class oneObjType
{
public:
	objT<T> instances;
	jvxComponentType selector[1];
	jvxComponentTypeClass classType;
	std::string description;
	std::string tokenInConfigFile;
};

class CjvxHostInteraction 
{
protected:

	class oneInterface
	{
	public:
		jvxHandle* theHdl = nullptr;
		jvxInterfaceType selector[1] = { JVX_INTERFACE_UNKNOWN };
	} ;

	class oneBlacklistEntry
	{
	public:
		jvxComponentType selector[1] = {JVX_COMPONENT_UNKNOWN};
	} ;

	class oneExternalModuleFactory
	{
	public:
		IjvxExternalModuleFactory* theFac;
		// What else??
	};

	class oneEntryReportStateSwitch
	{
	public:
		IjvxReportStateSwitch* ptrReport = nullptr;
		std::string tag;
	};

	struct
	{
		std::vector<oneInterface> externalInterfaces;

		struct
		{
			std::vector<jvxOneComponentModule> availableOtherComponents;
		} otherComponents;

		IjvxReport* reportUnit;
		IjvxReportOnConfig* reportOnConfig = nullptr;
		
		//IjvxReportStateSwitch* reportOnStateSwitch = nullptr;
		std::map<IjvxReportStateSwitch*, oneEntryReportStateSwitch> registeredStateSwitchHandlers;

		JVX_THREAD_ID threadId_registered;

		std::vector<oneBlacklistEntry> blacklist_load;
		std::vector<oneBlacklistEntry> inv_blacklist_load;
		std::vector<oneBlacklistEntry> blacklist_load_use;

		jvxBool doUnload;

		struct
		{
			jvxLoadModuleFilterCallback theCallback;
			jvxHandle* theCallback_priv;
		} loadFilter;

		std::map<std::string, std::string> storedConfigs;

		jvxBool bootComplete = false;

	} _common_set_host;

	struct
	{
		jvxBool minHostFunctionality_ = false;

		//! The host may be configured to load dynamic libraries or solely rely on static libraries.
		//! Also, a mix of both is possible with higher prio for the static libraries.
		jvxBool use_only_static_objects = false;
	} config;

	std::map<IjvxObject*, std::string> staticModules;

	std::map<IjvxExternalModuleFactory*, oneExternalModuleFactory> extModuleFactories;

	IjvxHiddenInterface* hostRefPass = nullptr;

public:

	CjvxHostInteraction();
	~CjvxHostInteraction();

	jvxErrorType _add_external_component(CjvxObject* meObj,
		IjvxObject* theObj, IjvxGlobalInstance* theGlob, const char* locationDescription, jvxBool allowMultipleInstance,
		jvxInitObject_tp funcInit, jvxTerminateObject_tp funcTerm);

	jvxErrorType _remove_external_component(CjvxObject* meObj, IjvxObject* theObj);

	jvxErrorType _add_external_factory(IjvxExternalModuleFactory* oneModFactory);
	jvxErrorType _remove_external_factory(IjvxExternalModuleFactory* oneModFactory);
	jvxErrorType _trigger_external_factory(IjvxExternalModuleFactory* oneModFactory, jvxBool isInvite);

	jvxErrorType _add_external_report_state_switch(IjvxReportStateSwitch* theHdl, const char* tag);
	jvxErrorType _remove_external_report_state_switch(IjvxReportStateSwitch* theHdl);

	jvxErrorType _set_external_report_target(CjvxObject* meObj, IjvxReport* theHdl);

	jvxErrorType _set_external_report_on_config(CjvxObject* meObj, IjvxReportOnConfig* theHdl);
	
	jvxErrorType _add_external_interface(CjvxObject* meObj, jvxHandle* theHdl, jvxInterfaceType theIFacetype);

	jvxErrorType _remove_external_interface(CjvxObject* meObj, jvxHandle* theHdl, jvxInterfaceType theIFacetype);

	jvxErrorType _add_component_load_blacklist(CjvxObject* meObj, jvxComponentType theTp, jvxBool targetBlackList);

	jvxErrorType _remove_component_load_blacklist(CjvxObject* meObj, jvxComponentType theTp, jvxBool targetBlackList);

	jvxErrorType _store_config(const char* token, const char* cfgToken, jvxBool);

	jvxErrorType _copy_config(const char* token, jvxApiString* strReturn);

	jvxErrorType _clear_config(const char* token);	

	jvxErrorType _request_id_main_thread(JVX_THREAD_ID* thread_id);

	jvxErrorType _report_boot_complete(jvxBool isComplete);
protected:

	// ================================================================================
	// Helper function
	// ================================================================================

	jvxErrorType loadAllComponents(jvxBool do_unload_dlls, std::vector<std::string>& component_path, jvxBool info_cout, jvxBool verbose_out);

	// This is a pre-run to the unload function. We need to release all package additional components here
	jvxErrorType pre_unloadAllComponents();
	jvxErrorType unloadAllComponents();

	//virtual jvxErrorType removeAllRemainingComponents();

	void reset();

	jvxErrorType request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl);
	jvxErrorType return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl);

	jvxErrorType _set_component_load_filter_function(jvxLoadModuleFilterCallback regme, jvxHandle* priv);

	// Try to forward call to add component. If return value if JVX_ERROR_ELEMENT_NOT_FOUND,
	// procede locally -> availableOtherComponents
	virtual jvxErrorType fwd_add_external_component(CjvxObject* meObj,
		IjvxObject* theObj, IjvxGlobalInstance* theGlob, const char* locationDescription, jvxBool allowMultipleInstance,
		jvxInitObject_tp funcInit, jvxTerminateObject_tp funcTerm, jvxComponentType tp);

	virtual void fwd_remove_external_component(CjvxObject* meObj, IjvxObject* theObj);

	// Try to forward call to load component. If return value if JVX_ERROR_ELEMENT_NOT_FOUND,
	// procede locally -> availableOtherComponents
	virtual jvxErrorType fwd_load_all_components(jvxComponentType tp, 
		jvxHandle* specCompAdd, jvxBool allowMultipleObjectsAdd,
		IjvxObject* newObjAdd,IjvxGlobalInstance* newGlobInstAdd,
		IjvxPackage* thePack, jvxSize thePackIdx,
		const std::string& fileName, 
		jvxInitObject_tp funcInitAdd, jvxTerminateObject_tp funcTermAdd,
		JVX_HMODULE dllHandle, jvxBitField dllProps, jvxSize& numAdded);

	virtual void fwd_pre_unload_all_components();

	virtual void fwd_unload_all_components();
};

#endif
