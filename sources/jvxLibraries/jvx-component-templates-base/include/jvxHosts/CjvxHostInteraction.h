#ifndef _CJVXHOSTINTERACTION_H__
#define _CJVXHOSTINTERACTION_H__

#include "common/CjvxObject.h"
#include "CjvxHostTypeHandler.h"

class CjvxHostInteraction : public CjvxHostTypeHandler
{
protected:

	typedef struct
	{
		jvxHandle* theHdl;
		jvxInterfaceType selector[1];
	} oneInterface;

	typedef struct
	{
		jvxComponentType selector[1];
	} oneBlacklistEntry;

	struct
	{
		std::vector<oneInterface> externalInterfaces;

		struct
		{
			std::vector<jvxOneComponentModule> availableOtherComponents;
		} otherComponents;

		IjvxReport* reportUnit;
		IjvxReportOnConfig* reportOnConfig;
		IjvxReportStateSwitch* reportOnStateSwitch;

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
		jvxBool minHostFunctionality;
	} config;

	std::map<IjvxObject*, std::string> staticModules;
public:

	CjvxHostInteraction();

	jvxErrorType _add_external_component(CjvxObject* meObj,
		IjvxObject* theObj, IjvxGlobalInstance* theGlob, const char* locationDescription, jvxBool allowMultipleInstance,
		jvxInitObject_tp funcInit, jvxTerminateObject_tp funcTerm);

	jvxErrorType _remove_external_component(CjvxObject* meObj, IjvxObject* theObj);

	jvxErrorType _set_external_report_state_switch(CjvxObject* meObj, IjvxReportStateSwitch* theHdl);

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

	jvxErrorType unloadAllComponents();

	//virtual jvxErrorType removeAllRemainingComponents();

	void reset();

	jvxErrorType request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl);
	jvxErrorType return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl);

	jvxErrorType _set_component_load_filter_function(jvxLoadModuleFilterCallback regme, jvxHandle* priv);

	// ==========================================================================================
private:

	// The following are template functions as included by CvxHostInteraction-tpl.h
	template <class T> jvxErrorType t_add_external_component(IjvxObject* theObj, 
		IjvxGlobalInstance* theGlob, const char* locationDescription, jvxBool allowMultipleInstance,
		jvxInitObject_tp funcInit, jvxTerminateObject_tp funcTerm,
		std::vector<oneObjType<T>>& registeredObj, const jvxComponentIdentification& tp);

	template <class T> void t_remove_external_component(IjvxObject* theObj, 
		std::vector<oneObjType<T>>& registeredObj);

	template <class T> void t_unload_dlls(std::vector<oneObjType<T>>& registeredObj);
};

#endif
