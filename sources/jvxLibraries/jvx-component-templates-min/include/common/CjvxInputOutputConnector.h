#ifndef __CJVXINPUTOUTPUTCONNECTOR_H__
#define __CJVXINPUTOUTPUTCONNECTOR_H__

// #define JVX_VERBOSE_CJVXINPUTCONNECTOR_H
#include "common/CjvxInputConnectorCore.h"
#include "common/CjvxOutputConnectorCore.h"
#include "common/CjvxInputOutputConnectorCore.h"

#define JVX_TRIGGER_TEST_CHAIN_1IO_CONNECTOR() \
	{ \
		jvxSize procId = JVX_SIZE_UNSELECTED; \
		request_process_id_chain(&procId); \
		if (JVX_CHECK_SIZE_SELECTED(procId)) \
		{ \
			_request_test_chain_master(procId); \
		} \
	}

class CjvxInputOutputConnector: 
	public CjvxInputConnectorCore, 
	public CjvxOutputConnectorCore, 
	public CjvxInputOutputConnectorCore
{
protected:

	class common_set_ldslave_t
	{
	public:
		IjvxDataProcessor* data_processor = nullptr;

		jvxBool detectLoop = false;

		jvxSize num_min_buffers_in = 0;
		jvxSize num_min_buffers_out = 0;
		jvxSize num_additional_pipleline_stages = 0;
		jvxBool zeroCopyBuffering_cfg = false;
		jvxCBitField fwd_alloc_flags = JVX_LINKDATA_ALLOCATION_MASK_FORWARD_ELEMENT_TO_ELEMENT;

		// Copy the input timestamp to the output timestamp
		jvxBool copy_timestamp_inout = false;

#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		const char* dbg_hint = nullptr;
#endif
	};

	common_set_ldslave_t _common_set_ldslave;

	CjvxInputOutputConnector();
	~CjvxInputOutputConnector();

	// jvxErrorType _available_connector(jvxBool* isAvail);

	 jvxErrorType lds_activate(IjvxDataProcessor* theDataProc, IjvxObject* theObjRef,
		IjvxConnectorFactory* myPar, IjvxConnectionMaster* master, std::string descror,
		IjvxInputConnector* icon, IjvxOutputConnector* ocon
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		 , const char* dbg_hint
#endif
	 );

	 jvxErrorType lds_deactivate();

	// ==============================================================
	// CONNECT DISCONNECT  CONNECT DISCONNECT  CONNECT DISCONNECT 
	// ==============================================================

	 jvxErrorType _connect_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	 jvxErrorType _disconnect_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ==============================================================
	// PREPARE POSTPROCESS  PREPARE POSTPROCESS  PREPARE POSTPROCESS 
	// ==============================================================

	 jvxErrorType _prepare_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	 jvxErrorType _postprocess_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ==============================================================
	// START START START START START START START START START START START
	// ==============================================================

	 virtual void _start_connect_common(jvxSize myUniquePipelineId) override;
	 virtual jvxErrorType _start_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override; 
	 
	// ==============================================================
	// STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP
	// ==============================================================
	 virtual jvxErrorType _stop_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	 virtual void _stop_connect_common(jvxSize* myUniquePipelineId) override;

	// ==============================================================
	// PROCESSSTART PROCESSSTOP PROCESSBUFFER PROCESSSTART PROCESSSTOP PROCESSBUFFER
	// ==============================================================
	 jvxErrorType _process_start_forward() override;
	 jvxErrorType _process_buffers_forward(jvxSize mt_mask, jvxSize idx_stage) override;
	 jvxErrorType _process_stop_forward(jvxSize idx_stage, jvxBool shift_fwd,
		 jvxSize tobeAccessedByStage, callback_process_stop_in_lock clbk,
		 jvxHandle* priv_ptr) override;
	
	// ==============================================================
	// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	// ==============================================================
	 virtual jvxErrorType _test_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	 // ==============================================================
	 // TRANSFER FORWARD
	 // ==============================================================

	 jvxErrorType JVX_CALLINGCONVENTION _transfer_forward_forward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	 jvxErrorType JVX_CALLINGCONVENTION _transfer_backward_backward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	 // ==============================================================

	 virtual jvxErrorType JVX_CALLINGCONVENTION _number_next(jvxSize* num);

	 virtual jvxErrorType JVX_CALLINGCONVENTION _reference_next(jvxSize idx, IjvxConnectionIterator** next);

	 virtual jvxErrorType JVX_CALLINGCONVENTION _reference_component(
		 jvxComponentIdentification* cpTp, 
		 jvxApiString* modName,
		 jvxApiString* lContext);

	 // ==============================================================

	// ==============================================================
	// HELPER FUNCTIONS
	// ==============================================================

	 // Helper for derived classes
	 jvxErrorType allocate_pipeline_and_buffers_prepare_to_zerocopy();

	 //! (Simplifying) function to request to run the test function on the chain which is locally connected
	 void request_test_chain(IjvxReport* theRep);
	 jvxErrorType request_process_id_chain(jvxSize* procId);

	 jvxErrorType _check_common_icon(IjvxDataConnectionCommon* ass_connection_common, IjvxConnectionMaster* master) override;
	 jvxErrorType _check_common_ocon(IjvxDataConnectionCommon* ass_connection_common, IjvxConnectionMaster* master) override;
	 jvxErrorType _check_common(IjvxDataConnectionCommon* ass_connection_common, IjvxConnectionMaster* master);
};

// Declare a macro to make behavior more clear
#define fwd_process_buffers_icon _process_buffers_icon

#endif
