#ifndef _CJVXCONSOLEHOST_BE_DRIVEHOST_H__
#define _CJVXCONSOLEHOST_BE_DRIVEHOST_H__

#include "CjvxConsoleHost_be_print.h"
#include "allHostsStatic_common.h"
#include "jvxHost_config.h"
#include "commandline/CjvxCommandLine.h"
#include "CjvxAppHostProduct.h"
#include "interfaces/all-hosts/configHostFeatures_common.h"
#include "CjvxHostJsonCommandsActShow.h"

// ==========================================================================================


/*
class CjvxEventLoop_memory
{
	typedef struct
	{
		jvxByte* buf;
		jvxSize selector;
	} oneAllocatedField;

	std::vector<oneAllocatedField> flds;
	JVX_MUTEX_HANDLE safeAccessMemList;
public:

	CjvxEventLoop_memory() {};
	~CjvxEventLoop_memory() {};

	jvxErrorType cleanup(
			jvxSize message_id,
			jvxHandle* param, jvxSize paramType,
			jvxSize event_type, jvxOneEventClass event_class, jvxOneEventPriority event_priority)
	{
		std::string* newStr = NULL;
		jvxErrorType res = JVX_NO_ERROR;
		JVX_LOCK_MUTEX(safeAccessMemList);
		std::vector<oneAllocatedField>::iterator elm = flds.end();
		elm = jvx_findItemSelectorInList < oneAllocatedField, jvxSize>(flds, message_id);
		if (elm != flds.end())
		{
			switch (paramType)
			{
			case JVX_EVENTLOOP_DATAFORMAT_STDSTRING:
				newStr = (std::string*)elm->buf;
				JVX_DSP_SAFE_DELETE_OBJECT(newStr);
				break;
			default:
				res = JVX_ERROR_INVALID_SETTING;
				break;
			}
			flds.erase(elm);
		}
		JVX_UNLOCK_MUTEX(safeAccessMemList);
		return res;
	}
};
*/

typedef struct
{
	jvxComponentIdentification origin;
	jvxPropertyCategoryType cat;
	jvxSize propId;
	bool onlyContent;
	jvxSize offs_start;
	jvxSize numE;
} report_property_changed;
	
class CjvxConsoleHost_be_drivehost : 
	public CjvxConsoleHost_be_print,
	public CjvxHostJsonCommandsActShow,
	public IjvxReport,
	public IjvxReportOnConfig,
	public IjvxSequencer_report,
	public IjvxEventLoop_frontend, 
	public JVX_APPHOST_PRODUCT_CLASSNAME, public CjvxHandleRequestCommands_callbacks,
	public IjvxConfigurationExtender_report
{
private:

	// jvxComponentIdentification tpAll[JVX_COMPONENT_LIMIT];

	struct
	{
		jvxState myState;
		jvxTimeStampData myTimerRef;
		std::string firstError;
		//jvxBool acceptNewTimerEvents;
		jvxSize timeout_msec;
		struct
		{
			jvxSize currentTimerId;
		} sequencer;
	} runtime;

	struct
	{
		jvxSize timeout_seq_msec;
		IjvxConfigurationExtender* hostRefConfigExtender;
		jvxBool auto_start;
	} config;

	configureHost_features theHostFeatures;	

	struct
	{
		jvxErrorType res;
		std::string txt;
	} rem_control;

	jvxCallbackPrivate theCallback_saveconfig;
	JVX_PROPERTIES_CALLBACK_DECLARE(cb_save_config);

	jvxBool supports_shutdown;
	
	jvxCallbackPrivate theCallback_shutdown;
	JVX_PROPERTIES_CALLBACK_DECLARE(cb_shutdown);

	jvxCallbackPrivate theCallback_restart;
	JVX_PROPERTIES_CALLBACK_DECLARE(cb_restart);
	
	jvxCallbackPrivate theCallback_loadconfig;
	JVX_PROPERTIES_CALLBACK_DECLARE(cb_load_config);

	jvxCallbackPrivate theCallback_exchg_property;
	JVX_PROPERTIES_CALLBACK_DECLARE(cb_xchg_property);
	

public:
	CjvxConsoleHost_be_drivehost();
	~CjvxConsoleHost_be_drivehost();

	virtual void process_init(IjvxCommandLine* comLine)override;
	virtual void process_help_command(IjvxEventLoop_frontend* origin)override;
	virtual void process_full_command(const std::string& command, 
		jvxSize message_id, jvxHandle* param,
		jvxSize paramType, jvxOneEventClass event_class, 
		jvxOneEventPriority event_priority,
		IjvxEventLoop_frontend* origin,
		jvxHandle* privBlock, jvxCBitField* extFlags) override;
	virtual jvxErrorType process_shutdown()override;

	// =======================================================================================
	// Interface IjvxReport
	// =======================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, 
		jvxSize offsetStart, jvxSize numElements, const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(
		jvxCBitField request, jvxHandle* caseSpecificData = NULL,
		jvxSize szSpecificData = 0)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_want_to_shutdown_ext(jvxBool restart) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION query_property(jvxFrontendSupportQueryType tp, jvxHandle* load)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_sync(jvxFrontendTriggerType tp, jvxHandle* load, jvxBool blockedRun)override;

	// =======================================================================================
	// Interface IjvxReportOnConfig
	// =======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_select_on_config(const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_unselect_on_config(const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_active_on_config(const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_inactive_on_config(const jvxComponentIdentification& tp) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION pre_hook_stateswitch(
		jvxStateSwitch ss, 
		const jvxComponentIdentification& tp) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION post_hook_stateswitch(
		jvxStateSwitch ss, 
		const jvxComponentIdentification& tp, 
		jvxErrorType suc) override;

	// =======================================================================================
	// Interface IjvxEventLoop_frontend
	// =======================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION report_process_event(TjvxEventLoopElement* theQueueElement)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_cancel_event(TjvxEventLoopElement* theQueueElement)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_assign_output(TjvxEventLoopElement* theQueueElement, jvxErrorType sucOperation, jvxHandle* priv)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_special_event(TjvxEventLoopElement* theQueueElement, jvxHandle* priv)override 
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_register_fe_commandline(IjvxCommandLine* comLine)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_readout_fe_commandline(IjvxCommandLine* comLine)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_register_be_commandline(IjvxCommandLine* comLine)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_readout_be_commandline(IjvxCommandLine* comLine)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_if_command_forward(IjvxReportSystemForward** fwdCalls) override;

	// =======================================================================================
	// Interface IjvxEventLoop_backend
	// =======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_event(TjvxEventLoopElement* ev)override;
		/*
		jvxSize message_id, IjvxEventLoop_frontend* origin, jvxHandle* priv_fe,
		jvxHandle* param, jvxSize paramType,
		jvxSize event_type, jvxOneEventClass event_class,
		jvxOneEventPriority event_priority, jvxCBool deleteOnProcess,
		jvxHandle* privBlock, jvxBool* reschedule)*/

	// =======================================================================================
	// Interface IjvxSequencer_report
	// =======================================================================================
	// Report the status of the sequencer if desired
	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxCBitField event_mask, const char* description, jvxSize sequenceId, 
		jvxSize stepId, jvxSequencerQueueType tp, jvxSequencerElementType stp, jvxSize fId, jvxSize oper_state, jvxBool inMainLoop) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId)override;

	// =======================================================================================
	// Interface IjvxConfigurationExtender_report
	// =======================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename, jvxInt32 lineno)override;

	// =======================================================================================
	// =======================================================================================
	// =======================================================================================

	
	void postMessage_inThread(std::string message);
	void postMessage_outThread(std::string message);
	void fatalStop(std::string message1, std::string message2);

	void addDefaultSequence(bool	onlyIfNoSequence, jvxOneSequencerStepDefinition* defaultSteps_run, jvxSize numSteps_run,
			jvxOneSequencerStepDefinition* defaultSteps_leave, jvxSize numSteps_leave, const std::string& nm_def);

	jvxErrorType try_reopen_config();
	//void initializeHost();
	//void initConfigureHost();
	//void terminateHost();

	// =========================================================================
	// Callback from lower class hierarchies
	// =========================================================================
	virtual jvxErrorType boot_initialize_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_specify_slots_specific() override;
	virtual jvxErrorType boot_prepare_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_start_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_activate_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_terminate_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_deactivate_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_postprocess_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_stop_specific(jvxApiString* errloc) override;
	virtual void report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context) override;
	virtual void inform_config_read_start(const char* nmfile, jvxHandle* context) override;
	virtual void inform_config_read_stop(jvxErrorType resRead, const char* nmFile, jvxHandle* context) override;
	virtual void get_configuration_specific_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigurationExtender* ext) override;
	virtual void get_configuration_specific(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* cfg, jvxConfigData* data) override;
	virtual void put_configuration_specific(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName) override;
	void put_configuration_specific_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigurationExtender* ext) override;
	void report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName) override;

	jvxErrorType shutdown_specific();

	virtual jvxErrorType boot_initialize_product()override;

#ifndef JVX_CONFIGURE_HOST_STATIC_NODE

	static void myComponentFilterCallback(jvxBool* doNotLoad, const char* module_name, jvxComponentType tp, jvxHandle* priv);
#endif

#if 0
	void saveConfigFile(jvxBool allowOverlayOnly_config);
	void openConfigFile_core(const char* fName, jvxBool allowOverlayOnly_config);
	jvxErrorType configureFromFile(std::string fName, jvxFlagTag flagtag);
	jvxErrorType configurationToFile(std::string& fName, jvxFlagTag flagtag);
#endif

	jvxErrorType process_command_abstraction(const oneDrivehostCommand& dh_command, 
		const std::vector<std::string>& args, 
		const std::string& addArg,
		jvxSize off, CjvxJsonElementList& jsec, 
		jvxCBitField* extFlags, jvxBool* systemUpdate);

	void outputResult(std::string printthis, std::string startline, char newline, char tab);

	jvxErrorType show_dropzone(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsecg);

	jvxErrorType show_current_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmlst_ret) override;
	jvxErrorType act_edit_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec) override;

	// ===================================================================================================================

	jvxErrorType act_start_sequencer(CjvxJsonElementList& jelmret) override;
	jvxErrorType act_stop_sequencer(CjvxJsonElementList& jelmret, jvxBool full_stop) override;

	jvxErrorType file_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret, jvxCBitField* extFlags);

	void report_command_request_inThread(jvxCBitField request, jvxHandle* caseSpecificData, jvxSize szSpecificData);
	
	jvxErrorType one_step_sequencer(IjvxSequencer* sequencer, jvxSize timeout_msec);
	void handle_step_process_non_usual_return(IjvxSequencer* sequencer, jvxErrorType resIn);

	//! This function is triggered if a command shall be involved immediately
	jvxErrorType run_prop_command(const std::string command);

	//! This function is triggered if the host is to be switched off via property control
	jvxErrorType run_quit_command(jvxBool restart);

	// ===============================================================================
	/**
	 * If the request to run a sequencer step immediately, this callback is triggered.
	 */
	virtual void trigger_immediate_sequencerStep() override;

	/**
	 * For the remaining command requests, the trigger is stored in the request queue and will
	 * be postponed. This way, the request always comes out in a delayed fashion - even if the
	 * request was triggered from within the main thread. */
	virtual jvxErrorType trigger_threadChange_forward(CjvxReportCommandRequest* ptr) override;

	/**
	 * If a test request was attached to the queue and all tests runs were completed, the succesful test is
	 * reported to all connected listeners.
	 */
	virtual void run_mainthread_triggerTestChainDone() override;

	/*
	 * This callback is called if the component list of a technology has changed. The technology is passed as cpId.
	 * Typically, we end up here since the the request is delayed into the request event queue.
	 */
	virtual void run_mainthread_updateComponentList(jvxComponentIdentification cpId) override;

	/*
	 * This callback is called if the properties have changed.
	 * Typically, we end up here since the the request is delayed into the request event queue.
	 */
	virtual void run_mainthread_updateProperties(jvxComponentIdentification cpId) override;

	/*
	 * This callback is called if the system state has changed.
	 * Typically, we end up here since the the request is delayed into the request event queue.
	 */
	virtual void run_mainthread_updateSystemStatus() override;

	virtual void run_immediate_rescheduleRequest(const CjvxReportCommandRequest& request) override;
	virtual void report_error(jvxErrorType resError, const CjvxReportCommandRequest& request) override;
};

#endif

