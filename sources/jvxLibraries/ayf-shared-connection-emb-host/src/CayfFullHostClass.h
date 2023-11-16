#ifndef __CAYFGLOBALHOSTCLASS_H__
#define __CAYFGLOBALHOSTCLASS_H__

#include "jvx.h"
#include "CjvxHostJsonCommandsProperties.h"
#include "CjvxHostJsonCommandsConfigShow.h"

class CayfFullHostClass : public CjvxHostJsonCommandsProperties, public IjvxExternalModuleFactory, public IjvxEventLoop_frontend_ctrl
{
private:

	class oneRegisteredFactory
	{
	public:
		std::string name;
		std::string nameAsRegistered;
		IjvxExternalModuleFactory* theFactory;
	};

	std::list<oneRegisteredFactory> factories_pending;
	std::list<oneRegisteredFactory> factories_active;
	std::list<oneRegisteredFactory> factories_remove;

	jvxBool firstRunExternalFacs = false;
	jvxBool shutDownProcess = false;
	JVX_THREAD_ID requestThread = JVX_THREAD_ID_INVALID;

	IjvxHiddenInterface* hHost = nullptr;
public:

	jvxBool hostStarted = false;
	JVX_THREAD_HANDLE hdlHostThread = JVX_INVALID_HANDLE_VALUE;
	JVX_THREAD_ID idHostThread = 0;

	jvxApiStringList astrlArgv;

	JVX_MUTEX_HANDLE safeAccess;

	/*
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
	*/
	CjvxHostJsonCommandsConfigShow config_show;

	CayfFullHostClass();
	~CayfFullHostClass();

	jvxErrorType register_factory_host(
		const char* nm, jvxApiString& nmAsRegistered, 
		IjvxExternalModuleFactory* regMe,
		int argc, const char* argv[]);
	jvxErrorType unregister_factory_host( IjvxExternalModuleFactory* regMe);
	int mainThreadLoop();

	virtual jvxErrorType requestReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp) override;
	virtual jvxErrorType returnReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp) override;

	jvxErrorType load_config_content(IjvxExternalModuleFactory* priIf, jvxConfigData** datOnReturn, const char* fName);
	jvxErrorType release_config_content(IjvxExternalModuleFactory* priIf, jvxConfigData* datOnReturn);

	jvxErrorType invite_external_components(IjvxHiddenInterface* hostRef, jvxBool isInvite) override;
	void setHost(IjvxHiddenInterface* hHostArg) { hHost = hHostArg; };

	jvxErrorType forward_text_command(const char* command, IjvxObject* priObj, jvxApiString& astr);

	// ===============================================================================================

	jvxErrorType report_register_fe_commandline(IjvxCommandLine* comLine) override;
	jvxErrorType report_readout_fe_commandline(IjvxCommandLine* comLine) override;
	jvxErrorType report_process_event(TjvxEventLoopElement* theQueueElement) override;
	jvxErrorType report_cancel_event(TjvxEventLoopElement* theQueueElement) override;
	jvxErrorType report_assign_output(TjvxEventLoopElement* theQueueElement, jvxErrorType sucOperation, jvxHandle* priv) override;
	jvxErrorType report_special_event(TjvxEventLoopElement* theQueueElement, jvxHandle* priv) override;
	jvxErrorType report_want_to_shutdown_ext(jvxBool restart) override;
	jvxErrorType query_property(jvxFrontendSupportQueryType tp, jvxHandle* load) override;
	jvxErrorType trigger_sync(jvxFrontendTriggerType tp, jvxHandle* load, jvxBool blockedRun) override;
	jvxErrorType request_if_command_forward(IjvxReportSystemForward** fwdCalls) override;

	jvxErrorType add_reference_event_loop(IjvxEventLoopObject* eventLoop) override;
	jvxErrorType clear_reference_event_loop(IjvxEventLoopObject* eventLoop) override;
	jvxErrorType set_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend) override;
	jvxErrorType clear_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend) override;
	jvxErrorType add_sec_reference_event_backend(IjvxEventLoop_backend* theBackend) override;
	jvxErrorType clear_sec_reference_event_backend(IjvxEventLoop_backend* theBackend) override;
	jvxErrorType returns_from_start(jvxBool* doesReturn) override;
	jvxErrorType start(int argc, char* argv[]) override;
	jvxErrorType stop() override;
	jvxErrorType wants_restart(jvxBool* wantsRestart) override;

};

#endif
