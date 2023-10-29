#ifndef __MEXJVXHOST_H__
#define __MEXJVXHOST_H__

#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#define __STDC_UTF_16__
#endif
#endif
#include <mex.h>

#include <vector>
#include "jvx.h"
#include "codeFragments/matlab_c/CjvxCToMatlabConverter.h"
#include "codeFragments/matlab_c/CjvxPropertiesToMatlabConverter.h"
#include "commandline/CjvxCommandLine.h"
#include "common/CjvxRealtimeViewer.h"
#include "typedefs/tools/TjvxExternalCall.h"
#include "allHostsStatic_common.h"
#include "common/CjvxReportMessageQueue.h"

#include "CjvxAppHostBase.h"
#include "CjvxAppHostProduct.h"

#include "interfaces/all-hosts/configHostFeatures_common.h"

#define MAXSTRING 256

#if _MATLAB_MEXVERSION >= 100
#define SZ_MAT_TYPE mwSize
#else
#define SZ_MAT_TYPE int
#endif

#define ERROR_MESSAGE_REPORT(fct) ((std::string)"Internal error when using jvxJvxHost, member function " + fct + (std::string)", reason: ")
#define MEX_PARAMETER_ERROR(p, id, exp) this->report_simple_text_message(((std::string)"Parameter <" + p + ", #" + jvx_int2String(id) + "> is not of correct type. A parameter of type <" + exp + "> is expected.").c_str(), JVX_REPORT_PRIORITY_ERROR);
#define MEX_PARAMETER_UNKNOWN_ERROR(p, id, exp) this->report_simple_text_message(((std::string)"Value <" + exp + "> for parameter <" + p + ", #" + jvx_int2String(id) + "> is not a valid option.").c_str(), JVX_REPORT_PRIORITY_ERROR);
#define MEX_PARAMETER_OBJ_NOT_FOUND(p, id, exp) this->report_simple_text_message(((std::string)"Parameter <" + p + ", #" + jvx_int2String(id) + "> is not a valid option for the specification of an object of type " + exp +".").c_str(), JVX_REPORT_PRIORITY_ERROR);
#define MEX_PARAMETER_INVALID_VALUE_ERROR(p, id, idT, idLim) this->report_simple_text_message(((std::string)"Parameter <" + p + ", #" + jvx_int2String(id) + "> is not related to any valid entry. The specified type-id is " + jvx_int2String(idT) + " but the limit for this type is " + jvx_int2String(idLim)).c_str(), JVX_REPORT_PRIORITY_ERROR);
#define MEX_PARAMETER_ERROR_STRING_MATCH(p, id) this->report_simple_text_message(((std::string)"Parameter <" + p + ", #" + jvx_int2String(id) + "> does not match any of the allowed options.").c_str(), JVX_REPORT_PRIORITY_ERROR);
#define TIMEOUT_WAIT_MAX_PROCESS_COMPLETE_MS 5000

class mexJvxHost: public CjvxCToMatlabConverter, public CjvxPropertiesToMatlabConverter,
	public IjvxReport,
	public IjvxSequencer_report, public CjvxReportMessageQueue, 
	public IjvxPrintf , public JVX_APPHOST_PRODUCT_CLASSNAME
{
	//! Reference to underlying fileReader functionality module
	
	/*
	struct
	{
		IjvxHost* theHdl;
		IjvxObject* theObj;
		IjvxToolsHost* hTools;
	} jvxhost;
	*/
	union numTypeConvert 
	{
		jvxData singleDat;
		jvxInt8 singleInt8;
		jvxInt16 singleInt16;
		jvxInt32 singleInt32;
		jvxInt64 singleInt64;
		jvxInt8 singleUInt8;
		jvxUInt16 singleUInt16;
		jvxUInt32 singleUInt32;
		jvxUInt64 singleUInt64;
	};

	struct
	{
		IjvxObject* theObj;
		IjvxGlobalInstance* theGLobal;
		struct
		{
			IjvxObject* theObj;
			IjvxExternalCall* theHdl;
		} fromHost;
	} externalCall;

	struct
	{
		std::string callback_prefix;
	} msg_queue;

	struct
	{
		JVX_THREAD_ID theMatlabThread;

		int numConfigureTokens;
		char** ptrTokens_cchar;
	} runtime;

	CjvxRealtimeViewer viewer_plots;
	CjvxRealtimeViewer viewer_props;

	jvxComponentIdentification tpAll[JVX_COMPONENT_ALL_LIMIT];
	CjvxCommandLine commLine;

	configureHost_features theHostFeatures;

	std::string cfg_filename_in_use;
	std::string cfg_filename_olay_in_use;

public:

	//! Constructor
	mexJvxHost(void);

	//! Destructor
	~mexJvxHost(void);

	// Function to specialize this app
	jvxErrorType initialize_specific();
	jvxErrorType terminate_specific();
	jvxErrorType lookup_type_id__name_specific(std::vector<std::string>& strLst, std::string& category,
		std::string tp, int nlhs, mxArray* plhs[], bool return_options_lookup, bool return_options_names);
	jvxErrorType lookup_const__name_specific(std::vector<std::string>& strLst, std::string& category, int nlhs, 
		mxArray* plhs[], bool return_options_lookup);

	//! Main wrapper function to scan for MEX input arguments
	bool parseInput(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]);
	void printText(std::string txt);

	/* Initialize the underlying host program*/

	// === BOOTUP ===
	jvxErrorType initialize(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType activate(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType read_config_file(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	// === SHUTDOWN ===
	jvxErrorType deactivate(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType terminate(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType lookup_type_id__name(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType lookup_type_name__value(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType lookup_const__name(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType export_config_string(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType export_config_struct(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType convert_string_config_struct(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType info(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType set_config_entry(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType get_config_entry(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType write_config(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	// === COMPONENTS ===
	jvxErrorType select_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType activate_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType status_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType commands_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType conditions_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType rel_jumps_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType isready_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType deactivate_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType unselect_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	// === SEQUENER ===
	jvxErrorType info_sequencer(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType add_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType mark_sequence_default(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType remove_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType reset_sequences(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType insert_step_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType edit_step_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType remove_step_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType switch_step_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType set_active_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType start_process_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType trigger_stop_process(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType trigger_stop_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType wait_completion_process(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType status_process_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType trigger_external_process(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	// === PROPERTIES ===

	jvxErrorType info_properties(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType list_property_tags(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType get_property_idx(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset);

	jvxErrorType get_property_uniqueid(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset);

	jvxErrorType get_property_descriptor(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset);

	jvxErrorType set_property_idx(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset);

	jvxErrorType set_property_uniqueid(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset);

	jvxErrorType set_property_descriptor(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset);

	jvxErrorType start_property_group(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset);

	jvxErrorType stop_property_group(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset);

	jvxErrorType status_property_group(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset);

	// === Connection RUles ===

	jvxErrorType list_connection_rules(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType list_connections(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType add_connection_rule(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType rem_connection_rule(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType test_chain_master(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType return_result_chain_master(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType check_ready_for_start(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	// === MESSAGE QUEUE ===

	jvxErrorType set_callback_msgq(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType get_callback_msgq(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType trigger_next_msgq(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType add_message_msgq(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	// === MATLAB CALL ===
	jvxErrorType external_call(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);


	//! Print usage of MATLAB module
	void printUsage();

	void mexCellContentToStringList(const mxArray* arr, std::vector<std::string>& strList);

	//! Return data in MEX format
	void mexReturnComponentIdentification(mxArray*& plhs, const jvxComponentIdentification& cpTp);


	void mexReturnStructSetup(mxArray*& plhs);
	void mexReturnComponents(mxArray*& plhs);
	void mexReturnStructOneComponent(mxArray*& plhs, const jvxComponentIdentification& tp);
	void mexReturnStructContentOneComponent(mxArray*& plhs, const jvxComponentIdentification& tp,jvxSize idx);
	void mexReturnStructComponentReady(mxArray*& plhs, const jvxComponentIdentification& tp);
	void mexReturnStructReadyForStartResult(mxArray*& plhs, jvxBool isReady, const std::string& reasonIfNot);
	void mexReturnStructComponentStatus(mxArray*& plhs, const jvxComponentIdentification& tp);
	
	void mexReturnStructCommandsComponents(mxArray*& plhs, const jvxComponentIdentification& tp);
	void mexReturnStructOneCommand(mxArray*& plhs, IjvxSequencerControl* theControl, IjvxObject* theObj, jvxSize idx);

	void mexReturnStructConditionsComponents(mxArray*& plhs, const jvxComponentIdentification& tp);
	void mexReturnStructOneCondition(mxArray*& plhs, IjvxSequencerControl* theControl, IjvxObject* theObj, jvxSize idx);

	void mexReturnStructRelativeJumpsComponents(mxArray*& plhs, const jvxComponentIdentification& tp);
	void mexReturnStructOneRelJump(mxArray*& plhs, IjvxSequencerControl* theControl, IjvxObject* theObj, jvxSize idx);

	void mexReturnStructSequencer(mxArray*& plhs);
	void mexReturnStructOneSequence(mxArray*& plhs, jvxSize idxSequence, IjvxSequencer* theSequencer, IjvxObject* theObj);
	void mexReturnStructOneSequenceStepQueue(mxArray*& plhs, jvxSize idxSequence, jvxSequencerQueueType qtp, jvxSize num,  IjvxSequencer* theSequencer, IjvxObject* theObj);
	void mexReturnStructOneSequenceStep(mxArray*& plhs, jvxSize idxSequence, jvxSequencerQueueType qtp, jvxSize stepId, IjvxSequencer* theSequencer, IjvxObject* theObj);

	void mexReturnStructConnectionRules(mxArray*& plhs, const std::string& nmRule, jvxSize idRule);
	void mexReturnStructOneConnectionRule(mxArray*& plhs, jvxSize mxId, IjvxDataConnectionRule* theDataConnectionRule);
	void mexReturnConnectionRuleMaster(mxArray*& plhs, const jvxComponentIdentification& cpId,
		const std::string& selFac, const std::string& selMasCon);
	void mexReturnConnectionRuleBridges(mxArray*& plhs, IjvxDataConnectionRule* theConRul);
	void mexReturnStructOneConnectionRuleBridge(mxArray*& plhs, jvxSize id, IjvxDataConnectionRule* theConRul);

	void mexReturnStructConnectionProcesses(mxArray*& plhs, const std::string& nmConnection, jvxSize idConnection);
	void mexReturnStructOneConnectionProcess(mxArray*& plhs, jvxSize mxId, IjvxDataConnectionProcess* theDataConnectionProc);
	void mexReturnConnectionAssociatedMaster(mxArray*& plhs, IjvxConnectionMaster* master);
	void mexReturnConnectionProcessBridges(mxArray*& plhs, IjvxDataConnectionProcess* theDataConnectionProc);
	void mexReturnStructOneConnectionProcessBridge(mxArray*& plhs, IjvxConnectorBridge* theBridge);
	void mexReturnOutputConnector(mxArray*& plhs, IjvxOutputConnector* theOCon);
	void mexReturnInputConnector(mxArray*& plhs, IjvxInputConnector* theICon);

	jvxErrorType convertSingleNumericalUnion(jvxDataFormat format, numTypeConvert& inputConvert, const mxArray* prhs);

	/*void mexReturnConnectionRuleMaster(mxArray*& plhs, const jvxComponentIdentification& cpId,
		const std::string& selFac, const std::string& selMasCon);
	void mexReturnConnectionRuleBridges(mxArray*& plhs, IjvxDataConnectionRule* theConRul);
	void mexReturnStructOneConnectionRuleBridge(mxArray*& plhs, jvxSize id, IjvxDataConnectionRule* theConRul);*/


	void mexReturnSequencerStatus(mxArray*& plhs, jvxSequencerStatus status, jvxSize idxSeq, jvxSequencerQueueType qTp, jvxSize idxStep, jvxBool loopEn);

	jvxErrorType mexReturnPropertyNumerical(mxArray*& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format, jvxSize numElms, 
		jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset);
	jvxErrorType mexReturnPropertyNumericalSize(mxArray*& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format, jvxSize numElms, jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset);
	jvxErrorType mexReturnPropertyNumerical(mxArray*& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset);
	jvxErrorType mexReturnPropertyNumericalSize(mxArray*& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset);

	jvxErrorType mexReturnPropertyOthers(mxArray*& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format, jvxSize numElms, jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset);
	jvxErrorType mexReturnPropertyOthers(mxArray*& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset);

	jvxErrorType copyDataToComponentNumerical(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, 
		jvxPropertyCategoryType cat, jvxDataFormat format, jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt);
	jvxErrorType copyDataToComponentNumericalSize(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, jvxPropertyCategoryType cat, 
		jvxDataFormat format, jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt);
	jvxErrorType copyDataToComponentNumerical(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, jvxDataFormat format, jvxSize numElms, 
		const char* descr, jvxSize offset, jvxAccessProtocol* accProt);
	jvxErrorType copyDataToComponentNumericalSize(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, 
		jvxDataFormat format, jvxSize numElms, const char* descr, jvxSize offset, jvxAccessProtocol* accProt);

	jvxErrorType copyDataToComponentOthers(const mxArray** prhs, int nrhs, jvx_propertyReferenceTriple& theTriple, jvxPropertyCategoryType cat, 
		jvxDataFormat format, jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt);
	jvxErrorType copyDataToComponentOthers(const mxArray** prhs, int nrhs, jvx_propertyReferenceTriple& theTriple,  jvxDataFormat format, 
		jvxSize numElms, const char* descr, jvxSize offset, jvxAccessProtocol* accProt);

	// Read and write configfiles
	jvxErrorType configurationFromFile(std::string fName, IjvxHost* theHost,IjvxToolsHost* theTools, IjvxReport* theReport);
	jvxErrorType configurationToFile(std::string& fName, IjvxHost* theHost, IjvxToolsHost* theTools, IjvxReport* theReport);

	jvxErrorType convert_mat_buf_c_buf_1_x_N(jvxHandle* data_setprops, jvxDataFormat format, jvxSize numElms, const mxArray* prhs);

	/* INTERFACE IjvxReport */
	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, 
		jvxSize offs, jvxSize num, const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(
		jvxCBitField request,
		jvxHandle* caseSpecificData, 
		jvxSize szSpecificData)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;

  virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context)override;

	/* INTERFACE IjvxSequencer_report */
	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxSequencerStatus stat, jvxCBitField event_mask, const char* description, jvxSize sequenceId, jvxSize stepId,
		jvxSequencerQueueType tp, jvxSequencerElementType stp, jvxSize fId, jvxSize current_state,
		jvxBool indicateFirstError)override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_callback(jvxSize sequenceId, jvxSize stepId,
								      jvxSequencerQueueType tp, jvxSize functionId)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc) override;

	static jvxErrorType st_process_msg_callback(jvxHandle* theField, jvxSize szField, jvxCBitField elementType, jvxHandle* theContext);
	virtual jvxErrorType ic_process_msg_callback(jvxHandle* theField, jvxSize szField, jvxCBitField elementType);

	static jvxErrorType st_remove_msg_callback(jvxHandle* theField, jvxSize szField, jvxCBitField elementType, jvxHandle* theContext);
	virtual jvxErrorType ic_remove_msg_callback(jvxHandle* theField, jvxSize szField, jvxCBitField elementType);

	jvxErrorType add_element_to_queue(const mxArray* prhs[], int nrhs, std::string& error);

	static int myMexPrintfC(jvxHandle* priv, const char* txt);
	int ic_myMexPrintfC(const char* txt);
	jvxErrorType request_printf(the_jvx_printf* fPtr)override;

	// =================================================================================00
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

	virtual void get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext) override;
	virtual void get_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data) override;
	virtual void put_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName) override;
	virtual void put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext) override;

	virtual void report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName) override;
};

#endif
