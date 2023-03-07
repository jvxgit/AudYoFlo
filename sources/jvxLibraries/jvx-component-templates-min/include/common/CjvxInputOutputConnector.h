#ifndef __CJVXINPUTCONNECTOR_H__
#define __CJVXINPUTCONNECTOR_H__

// #define JVX_VERBOSE_CJVXINPUTCONNECTOR_H
#include "CjvxJson.h"
#include "HjvxMisc.h"

#define JVX_TRIGGER_TEST_CHAIN_1IO_CONNECTOR() \
	{ \
		jvxSize procId = JVX_SIZE_UNSELECTED; \
		request_process_id_chain(&procId); \
		if (JVX_CHECK_SIZE_SELECTED(procId)) \
		{ \
			_request_test_chain_master(procId); \
		} \
	}

class CjvxInputOutputConnector
{
protected:

	struct
	{
		IjvxDataProcessor* data_processor;
		IjvxObject* object;
		IjvxInputConnector* icon;
		IjvxOutputConnector* ocon;
		std::string descriptor;

		IjvxConnectorBridge* conn_in;
		IjvxConnectorBridge* conn_out;

		IjvxConnectorFactory* myParent;

		jvxLinkDataDescriptor theData_out;
		jvxLinkDataDescriptor* theData_in;

		IjvxConnectionMaster* theMaster;

		IjvxDataConnectionCommon* theCommon_to;
		IjvxDataConnectionCommon* theCommon_from;

		jvxBool detectLoop;

		jvxSize num_min_buffers_in;
		jvxSize num_min_buffers_out;
		jvxSize num_additional_pipleline_stages;
		jvxBool zeroCopyBuffering_cfg;

		jvxSize myRuntimeId;

		jvxBool allows_termination;
		jvxBool setup_for_termination;

		// Copy the input timestamp to the output timestamp
		jvxBool copy_timestamp_inout;

		struct
		{
			jvxDataFormatGroup format_group;
			jvxDataflow data_flow;
		} caps_in;

		struct
		{
			jvxDataFormatGroup format_group;
			jvxDataflow data_flow;
		} caps_out;

#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		const char* dbg_hint;
#endif
	} _common_set_ldslave;


	CjvxInputOutputConnector();

	~CjvxInputOutputConnector();

	jvxErrorType _available_connector(jvxBool* isAvail);

 jvxErrorType _associated_common_icon(IjvxDataConnectionCommon** ref);

	jvxErrorType _associated_common_ocon(IjvxDataConnectionCommon** ref);

	 jvxErrorType _descriptor_connector(jvxApiString* str);

	 jvxErrorType lds_activate(IjvxDataProcessor* theDataProc, IjvxObject* theObjRef,
		IjvxConnectorFactory* myPar, IjvxConnectionMaster* master, std::string descror,
		IjvxInputConnector* icon, IjvxOutputConnector* ocon
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		 , const char* dbg_hint
#endif
	 );

	 jvxErrorType lds_deactivate();

	 jvxErrorType _set_parent_factory(IjvxConnectorFactory* my_parent);

	 jvxErrorType _parent_factory(IjvxConnectorFactory** my_parent);

	 jvxErrorType _supports_connector_class_ocon(
		 jvxDataFormatGroup format_group,
		jvxDataflow data_flow);
	 jvxErrorType _supports_connector_class_icon(
		 jvxDataFormatGroup format_group,
		jvxDataflow data_flow);

	// ==============================================================
	// SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT
	// ==============================================================

	 jvxErrorType _select_connect_icon(IjvxConnectorBridge* bri, 
		 IjvxConnectionMaster* master,
		 IjvxDataConnectionCommon* ass_connection_common,
		 IjvxInputConnector** replace_connector);

	 jvxErrorType _select_connect_ocon(IjvxConnectorBridge* bri, 
		 IjvxConnectionMaster* master, 
		 IjvxDataConnectionCommon* ass_connection_common,
		 IjvxOutputConnector** replace_connector);

	// ==============================================================
	// UNSELECT UNSELECT UNSELECT UNSELECT UNSELECT UNSELECT UNSELECT
	// ==============================================================

	 jvxErrorType _unselect_connect_icon(
		 IjvxConnectorBridge* bri,
		 IjvxInputConnector* replace_connector);

	jvxErrorType _unselect_connect_ocon(IjvxConnectorBridge* bri,
		IjvxOutputConnector* replace_connector);

	// ==============================================================
	// CONNECT CONNECT CONNECT CONNECT CONNECT CONNECT CONNECT CONNECT
	// ==============================================================

 jvxErrorType _connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb), jvxBool forward);

	 jvxErrorType _connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb), jvxDataProcessorHintDescriptor* add_me = NULL);

	// ==============================================================
	// DISCONNECT DISCONNECT DISCONNECT DISCONNECT DISCONNECT DISCONNECT
	// ==============================================================

	 jvxErrorType _disconnect_connect_icon(jvxLinkDataDescriptor* theData, jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	 jvxErrorType _disconnect_connect_ocon(const jvxChainConnectArguments& args, JVX_CONNECTION_FEEDBACK_TYPE_F(fdb) jvxDataProcessorHintDescriptor** pointer_remove = NULL);


	// ==============================================================
	// PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE
	// ==============================================================

	 jvxErrorType _prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	 jvxErrorType _prepare_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ==============================================================
	// POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS
	// ==============================================================

	 jvxErrorType _postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	 jvxErrorType _postprocess_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ==============================================================
	// START START START START START START START START START START START
	// ==============================================================

	 jvxErrorType _start_connect_icon(jvxBool forward, jvxSize myUniquePipelineId JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	 jvxErrorType _start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// ==============================================================
	// STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP
	// ==============================================================
	 jvxErrorType _stop_connect_icon(jvxBool forward, jvxSize * uId JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	 jvxErrorType _stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));


	// =========================================================================
	// =========================================================================
	// =========================================================================
	// =========================================================================

	// ==============================================================
	// PROCESSSTART PROCESSSTART PROCESSSTART PROCESSSTART PROCESSSTART
	// ==============================================================
	 jvxErrorType _process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage, jvxSize tobeAccessedByStage = 0,
		 callback_process_start_in_lock clbk = NULL,
		 jvxHandle* priv_ptr = NULL);


	 jvxErrorType _process_start_ocon(jvxSize pipeline_offset, 
		 jvxSize* idx_stage, jvxSize tobeAccessedByStage = 0,
		 callback_process_start_in_lock clbk = NULL,
		 jvxHandle* priv_ptr = NULL);

	// ==============================================================
	// PROCESSBUFFER PROCESSBUFFER PROCESSBUFFER PROCESSBUFFER PROCESSBUFFER
	// ==============================================================
	 jvxErrorType _process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage);

	 jvxErrorType _process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage );

	 // Declare a macro to make behavior more clear
#define fwd_process_buffers_icon _process_buffers_icon

	// ==============================================================
	// PROCESSSTOP PROCESSSTOP PROCESSSTOP PROCESSSTOP PROCESSSTOP PROCESSSTOP
	// ==============================================================
	 jvxErrorType _process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd, jvxSize tobeAccessed = 0,
		 callback_process_stop_in_lock clbk = NULL, jvxHandle* priv_ptr = NULL);

	 jvxErrorType _process_stop_ocon(jvxSize idx_stage, jvxBool shift_fwd, jvxSize tobeAccessed = 0,
		 callback_process_stop_in_lock clbk = NULL, jvxHandle* priv_ptr = NULL);

	// ============================================================================
	// ============================================================================
	// ============================================================================
	// ============================================================================

	// ==============================================================
	// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	// ==============================================================

	 jvxErrorType _test_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	 jvxErrorType _test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// ==============================================================
	// TRANSFER FORWARD
	// ==============================================================

	 jvxErrorType JVX_CALLINGCONVENTION _transfer_forward_icon(jvxBool forward, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	 jvxErrorType JVX_CALLINGCONVENTION _transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ==============================================================
	// TRANSFER FORWARD
	// ==============================================================

	 jvxErrorType JVX_CALLINGCONVENTION _transfer_backward_ocon(jvxBool forward, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	 jvxErrorType JVX_CALLINGCONVENTION _transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	 // ==============================================================

	 virtual jvxErrorType JVX_CALLINGCONVENTION _number_next(jvxSize* num);

	 virtual jvxErrorType JVX_CALLINGCONVENTION _reference_next(jvxSize idx, IjvxConnectionIterator** next);

	 virtual jvxErrorType JVX_CALLINGCONVENTION _reference_component(
		 jvxComponentIdentification* cpTp, 
		 jvxApiString* modName,
		 jvxApiString* lContext);

	 virtual jvxErrorType JVX_CALLINGCONVENTION _connected_icon(IjvxInputConnector** icon);

	 // ==============================================================

	// ==============================================================
	// HELPER FUNCTIONS
	// ==============================================================
	 jvxErrorType JVX_CALLINGCONVENTION shift_buffer_pipeline_idx_on_start(
		 jvxSize pipeline_offset , jvxSize* idx_stage,
		 jvxSize tobeAccessedByStage, callback_process_start_in_lock clbk,
		 jvxHandle* priv_ptr);

	 jvxErrorType JVX_CALLINGCONVENTION shift_buffer_pipeline_idx_on_stop(jvxSize idx_stage, 
		 jvxBool shift_fwd,
		 jvxSize tobeAccessedByStage,
		 callback_process_stop_in_lock clbk,
		 jvxHandle* priv_ptr);

	 jvxErrorType JVX_CALLINGCONVENTION allocate_pipeline_and_buffers_prepare_to();

	 jvxErrorType JVX_CALLINGCONVENTION allocate_pipeline_and_buffers_prepare_to_zerocopy();

	 jvxErrorType JVX_CALLINGCONVENTION deallocate_pipeline_and_buffers_postprocess_to();

	 jvxErrorType JVX_CALLINGCONVENTION deallocate_pipeline_and_buffers_postprocess_to_zerocopy();

	 //! (Simplifying) function to request to run the test function on the chain which is locally connected
	 void request_test_chain(IjvxReport* theRep);
	 jvxErrorType request_process_id_chain(jvxSize* procId);

};

#endif
