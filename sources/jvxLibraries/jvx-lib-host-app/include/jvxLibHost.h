#include "jvx.h"
#include "jvx-helpers.h"
#include "CjvxAppHostProduct.h"
#include "common/CjvxProperties.h"
#include "pcg_exports_lib.h"

#define JVX_NEW_COMMAND_HANDLING

class CjvxHostJsonCommandsActShow;
class jvxLibHost;

class callbacks_capi
{
public:
	jvxErrorType(*async_report_simple_text_message)(const char* txt, jvxReportPriority prio) = nullptr;
	
	jvxErrorType(*sync_report_internals_have_changed)(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offset_start, jvxSize numElements,
		const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose, jvxLibHost* backref) = nullptr;
	
	jvxErrorType(*async_report_command_request)(
		jvxCBitField request,
		jvxHandle* caseSpecificData,
		jvxSize szSpecificData) = nullptr;

	// jvxErrorType(*request_command)(const CjvxReportCommandRequest& request, jvxBool forceAsync, jvxLibHost* backRef) = nullptr;
};

// ==============================================================================

class jvxLibHost : public JVX_APPHOST_PRODUCT_CLASSNAME,
	public IjvxReport, 
	public IjvxReportOnConfig,
	public IjvxSequencer_report,
	public IjvxCommandInterpreter,
	public IjvxSubReport,
	public IjvxProperties, public CjvxProperties, public CjvxObjectMin,
	public IjvxConfiguration,
	public genLibHost
{
private:
	
	CjvxCommandLine commLine;
	configureLibHost_features theHostFeatures;
	
	// Main thread id preset on initSystem!
	JVX_THREAD_ID mainThreadId = JVX_THREAD_ID_INVALID;
	JVX_MUTEX_HANDLE safeAccessEngineThreads;
	jvxBool init_done = false;
	callbacks_capi cbks_api;

	jvxHandle* showHandle = nullptr;

	jvxTimeStampData seq_timestap_data;
	
	class oneCheckoutPropReference
	{
	public:
		IjvxProperties* ptrProp = nullptr;
		IjvxObject* ptrObj = nullptr;
		jvxSize refCnt = 0;
	};
	std::map<jvxComponentIdentification, oneCheckoutPropReference> checkedOutPropReferences;

	class operationConfig
	{
	public:
		jvxBool reportOnSet = false;
		jvxBool reportSyncDirect = true;
	} oConfig;

	JVX_MUTEX_HANDLE safeAccessSubReports;
	std::list< IjvxReportSystem*> subReports;

	// =====================================================================

	/*
	class systemParamsType
	{
	public:
		jvxBool auto_start = false;
		jvxBool auto_stop = false;		
	};
	systemParamsType systemParams;
	*/

	// Extending host properties
	jvxBool supports_shutdown;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_set_options);

public:
	jvxLibHost();

	~jvxLibHost();

	// =====================================================================================
#ifdef JVX_BINARY_API
	static void verify_binary_build() { std::cout << "jvxLibHost with binary api" << std::endl; };
#elif JVX_PLUGIN_API
	static void verify_plugin_build() { std::cout << "jvxLibHost with binary api" << std::endl; };
#else
#error Libhost App without API specification is not allowed!!
#endif
	// =====================================================================================

	jvxErrorType initSystem(const char* [], int, callbacks_capi* cbks = nullptr, bool executableInBinFolder = true);
	jvxErrorType terminateSystem();

	// Asynchronous forward functions to switch threads
	// ======================================================================
	jvxErrorType request_command_fwd(const CjvxReportCommandRequest& request, jvxBool async);
	jvxErrorType request_test_chain(const CjvxReportCommandRequest_uid& req);

	// ======================================================================
	jvxErrorType process_access(jvxSize uId, jvxSize* catId, jvxApiString* astr, IjvxConnectionIterator** itRet);
	jvxErrorType process_decode_iterator(IjvxConnectionIterator* it,
		jvxComponentIdentification* retCp, jvxSize* numBranch, jvxApiString* modName, jvxApiString* description, jvxApiString* lContext);
	jvxErrorType process_next_iterator(IjvxConnectionIterator* it, IjvxConnectionIterator** next, jvxSize id);
	jvxErrorType process_system_ready(jvxApiString* astr);

	// ======================================================================
	jvxErrorType sequencer_status(jvxSequencerStatus* stat);
	jvxErrorType sequencer_start(jvxSize granularity_state_report);
	jvxErrorType sequencer_stop();
	jvxErrorType sequencer_ack_stop();
	jvxErrorType sequencer_trigger();

	// ======================================================================
	jvxErrorType description_selected_component(jvxComponentIdentification cpTp, jvxApiString* astr);
	jvxErrorType number_components_system(jvxComponentIdentification cpTp, jvxSize* num);
	jvxErrorType number_devices_selected_technology(jvxComponentIdentification cpTp, jvxSize* num);
	jvxErrorType selection_component(const jvxComponentIdentification& tpGet, jvxSize* idx);
	jvxErrorType number_slots_component_system(const jvxComponentIdentification& tpGet, jvxSize* szSlots, jvxSize* szSubslots);
	
	jvxErrorType desciptor_device_selected_technology(jvxComponentIdentification cpTp, jvxSize idx, jvxApiString* descror);
	jvxErrorType desciption_device_selected_technology(jvxComponentIdentification cpTp, jvxSize idx, jvxApiString* description);
	jvxErrorType capability_device_selected_technology(jvxComponentIdentification cpTp, jvxSize idx, jvxDeviceCapabilities& caps, jvxComponentIdentification& tpId);
	jvxErrorType state_device_selected_technology(jvxComponentIdentification cpTp, jvxSize idx, jvxState* stat);

	jvxErrorType descriptor_component_system(jvxComponentIdentification cpTp, jvxSize idx, jvxApiString* descror);
	jvxErrorType description_component_system(jvxComponentIdentification cpTp, jvxSize idx, jvxApiString* description);

	jvxErrorType save_config();
	
	jvxErrorType set_operation_flags(bool reportOnSet, bool reportSyncDirect);
	jvxErrorType get_compile_flags(jvxDataTypeSpec& dataTpFormat, jvxSize& num32BitWordsSelection);

	// ======================================================================
	
	jvxErrorType periodic_trigger();

	// ======================================================================
	void transfer_command(const std::string& cmd, jvxApiString* returnStr, bool json_out);

	// ======================================================================
	jvxSize translate_enum_string(const std::string& typeForm, const std::string& typeSelect);

	// ======================================================================
	jvxErrorType lookup_component_class(const jvxComponentType& cpTp, jvxComponentTypeClass& cls);

	// =====================================================================
	jvxErrorType select_component(jvxComponentIdentification& cpTp, jvxSize idx);
	jvxErrorType activate_selected_component(const jvxComponentIdentification& cpTp);
	jvxErrorType deactivate_selected_component(const jvxComponentIdentification& cpTp);
	jvxErrorType unselect_selected_component(jvxComponentIdentification& cpTp);

	// ======================================================================
	jvxErrorType property_request_handle(jvxComponentIdentification cpId,
		IjvxObject** obj_ptr,
		IjvxProperties** prop_ptr);
	jvxErrorType property_return_handle(jvxComponentIdentification cpId,
		IjvxObject* obj_ptr,
		IjvxProperties* prop_ptr);
	jvxErrorType verify_property_handle(jvxComponentIdentification cpId,
		IjvxObject* obj_ptr,
		IjvxProperties* prop_ptr);

	// ======================================================================
	// JVX_APP_FACTORY_HOST_CLASSNAME
	// ======================================================================
	virtual jvxErrorType boot_initialize_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_specify_slots_specific() override;
	virtual jvxErrorType boot_prepare_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_start_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_activate_specific(jvxApiString* errloc) override;

	virtual jvxErrorType shutdown_terminate_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_deactivate_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_postprocess_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_stop_specific(jvxApiString* errloc) override;
	// ======================================================================
	// ======================================================================

	jvxErrorType set_property(
		jvxCallManagerProperties& callGate, jvxComponentIdentification cpId,
		IjvxObject* objPtr, IjvxProperties* propPtr,		
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans,
		AyfBackendReportPropertySet propSetType);
	
	jvxErrorType start_property_set_collect(jvxCallManagerProperties& callGate, jvxComponentIdentification cpId,
			IjvxObject* objPtr, IjvxProperties* propPtr);
	
	jvxErrorType stop_property_set_collect(jvxCallManagerProperties& callGate, jvxComponentIdentification cpId,
			IjvxObject* objPtr, IjvxProperties* propPtr);

	// ======================================================================

	// =============================================================================
	virtual void report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context) override;
	virtual void inform_config_read_start(const char* nmfile, jvxHandle* context) override;
	virtual void inform_config_read_stop(jvxErrorType resRead, const char* nmfile, jvxHandle* context) override;

	virtual void report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName) override;

	virtual void get_configuration_specific_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigurationExtender* ext) override;
	virtual void get_configuration_specific(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* cfg, jvxConfigData* data) override;
	virtual void put_configuration_specific(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName) override;
	virtual void put_configuration_specific_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigurationExtender* ext) override;

	// =============================================================================
	// Interface IjvxReportStateSwitch
	// =============================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc) override;
	// =============================================================================
	// =============================================================================

	// =============================================================================
	//	IjvxReport	IjvxReport	IjvxReport	IjvxReport	IjvxReport	IjvxReport	IjvxReport
	// =============================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offset_start, jvxSize numElements,
		const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, 
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(jvxCBitField request,
		jvxHandle* caseSpecificData = NULL,
		jvxSize szSpecificData = 0) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;

	// =============================================================================
	// =============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION register_sub_report(IjvxReportSystem* request) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_sub_report(IjvxReportSystem* request) override;

	// =============================================================================
	// =============================================================================

	// =============================================================================
	// IjvxReportOnConfig IjvxReportOnConfig IjvxReportOnConfig IjvxReportOnConfig IjvxReportOnConfig
	// =============================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_select_on_config(const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_active_on_config(const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_inactive_on_config(const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_unselect_on_config(const jvxComponentIdentification& tp) override;
	// =============================================================================
	// =============================================================================

	// =============================================================================
	// IjvxSequencer_report IjvxSequencer_report IjvxSequencer_report IjvxSequencer_report
	// =============================================================================	
	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxCBitField event_mask, const char* description, jvxSize sequenceId, jvxSize stepId,
		jvxSequencerQueueType tp, jvxSequencerElementType stp, jvxSize fId, jvxSize current_state, jvxBool indicateFirstError) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId) override;
	// =============================================================================
	// =============================================================================

	// =============================================================================
	// IjvxReportStateSwitch IjvxReportStateSwitch IjvxReportStateSwitch IjvxReportStateSwitch
	// =============================================================================
	// virtual jvxErrorType JVX_CALLINGCONVENTION pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp) override;
	// virtual jvxErrorType JVX_CALLINGCONVENTION post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc) override;
	// =============================================================================
	// =============================================================================
	bool thread_check_enter();
	void thread_check_leave();

	bool synchronize_thread_enter(jvxBool forceSync = false);
	void synchronize_thread_leave(jvxBool forceSync = false);

	/* These function require the preprocessor macro JVX_LIBHOST_WITH_JSON_SHOW to be set.
	 * If the macro is not set, the functions return errors and do not produce any reasonable 
	 * output as the command is ignored (empty implementation)
	 */
	jvxErrorType allocate_json_show_handle();
	jvxErrorType deallocate_json_show_handle();

	jvxErrorType set_request_command_handler(CjvxHandleRequestCommands_callbacks* refFwdReqCommands);

	jvxErrorType interpret_command(const char* command, jvxApiString* result, jvxBool jsonOut) override;
	// void run_direct_show_command(const std::string& command, jvxApiString* retVal = nullptr, jvxBool jsonOut = false);

	// ================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

	// ================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION report_properties_modified(const char* props_set) override;

#define JVX_OBJECT_ZERO_REFERENCE
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#undef JVX_OBJECT_ZERO_REFERENCE

#include "codeFragments/simplify/jvxProperties_simplify.h"


};