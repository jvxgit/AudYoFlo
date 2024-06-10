#ifndef __CJVXSPNSYNCHRONIZE_SEC_H__
#define __CJVXSPNSYNCHRONIZE_SEC_H__

#include "common/CjvxSingleInputConnector.h"
#include "common/CjvxSingleOutputConnector.h"

#include "common/CjvxConnectionMaster.h"
#include "common/CjvxConnectorMasterFactory.h"

#include "jvxNodes/CjvxBareNode1ioRearrange.h"

extern "C"
{
#include "jvx_audio_stack_sample_dispenser_cont.h"
#include "jvx_buffer_estimation/jvx_estimate_buffer_fillheight.h"
}

enum class jvxSynchronizeBufferMode
{
	JVX_SYNCHRONIZE_UNBUFFERED_PUSH,
	JVX_SYNCHRONIZE_BUFFERED_PULL
};

// Forward declaration
class CjvxSpNSynchronize;

class CjvxSpNSynchronize_sec: 
	public CjvxSingleInputConnector, public CjvxSingleOutputConnector,
	public CjvxSingleConnector_report<CjvxSingleInputConnector>,
	public CjvxSingleConnector_report<CjvxSingleOutputConnector>,
	public IjvxConnectionMaster, public CjvxConnectionMaster,
	public IjvxConnectionMasterFactory, public CjvxConnectionMasterFactory
{
	friend class CjvxSpNSynchronize;
protected:
	CjvxSpNSynchronize* referencePtr = nullptr;

	CjvxSimplePropsPars node_output;
	CjvxSimplePropsPars node_input;

	jvxBool outputConnectorConnected = false;
	jvxBool inputConnectorConnected = false;

	jvxRWLockWithVariable<jvxCBitField> safeAccessRWConnectionStatus;

	jvxLockWithVariable<jvxCBitField> safeAccessConnectionBuffering;
	// Hold the structure to synchronize the two threads
	struct
	{
		struct
		{
			//jvxCBitField stateFlags;
			jvxSize numChannelsPrimary;
			jvxSize numChannelsSecondary;
			jvxSize buffersizePrimary;
			jvxSize buffersizeSecondary;
			jvxSize ratePrimary;
			jvxSize rateSecondary;
			jvxDataFormat formatPrimary;
			jvxDataFormat formatSecondary;

			jvxBool bothSidesStarted;
			jvxAudioStackDispenserCont myAudioDispenser;

			JVX_MUTEX_HANDLE safeAccess_lock; // <- lock used in audio sample disposer
			jvxHandle** work_buffers_from_hw;
			jvxHandle** work_buffers_to_hw;
		} crossThreadTransfer;

		struct
		{
			jvxHandle* fHeightEstimator = nullptr;
			jvxSize numberEventsConsidered_perMinMaxSection = 0;
			jvxSize num_MinMaxSections = 0;
			jvxData recSmoothFactor = 0.95;
			jvxSize numOperations = 10;
		} fHeight;

		jvxAudioStackDispenserProfile prof_prim;
		jvxAudioStackDispenserProfile prof_sec;
		jvxTimeStampData tstamp;
		jvxBool tstamp_init = false;

		std::list<jvxLinkDataAttached_oneFrameLostElement> attachedData;
		jvxSize cntLost;
	} inProcessing;

public:
	CjvxSpNSynchronize_sec(CjvxSpNSynchronize* reference);

	// Activate component. In there, register input and output connector as well as master functionality
	void activate();
	void deactivate();

	// Once the primary chain has seen a configuration modification, we need to trigger the secondary chain
	void trigger_request_secondary_chain();

	// UNBUFFERED MODES
	void trigger_process_immediate(jvxLinkDataDescriptor* datIn, jvxSize idx_stage, jvxLinkDataDescriptor* datOut);
	
	// UNBUFFERED WITH PULL
	void trigger_process_pull();

	// Allocate and deallocate exchange data structures
	void try_allocate_runtime_sync_data(jvxSize operationFlagPosition);
	void try_deallocate_runtime_sync_data(jvxSize posId);
	jvxErrorType audio_primary(jvxLinkDataDescriptor* datIn, jvxSize idx_stage, jvxLinkDataDescriptor* datOut);

	jvxErrorType audio_secondary(jvxLinkDataDescriptor* datIn, jvxSize idx_stage, jvxLinkDataDescriptor* datOut);
	jvxErrorType audio_secondary_get(jvxLinkDataDescriptor* datIn, jvxSize idx_stage);
	jvxErrorType audio_secondary_put(jvxLinkDataDescriptor* datOut);

	// ================================================================================
	// Redefined for input connector base class
	// ================================================================================
	jvxErrorType available_to_connect_icon()override;
	jvxErrorType connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	jvxErrorType disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	
	// ================================================================================
	// Redefined for output connector base class
	// ================================================================================
	jvxErrorType available_to_connect_ocon()override; 
	jvxErrorType prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ================================================================================
	// Callbacks for input connector
	// ================================================================================

	jvxErrorType report_selected_connector(CjvxSingleInputConnector* iconn) override;
	void request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId) override;
	jvxErrorType report_started_connector(CjvxSingleInputConnector* iconn) override;

	jvxErrorType report_stopped_connector(CjvxSingleInputConnector* iconn) override;
	void release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId) override;
	jvxErrorType report_unselected_connector(CjvxSingleInputConnector* iconn) override;

	jvxErrorType report_test_connector(CjvxSingleOutputConnector* iconn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	jvxErrorType report_test_connector(CjvxSingleInputConnector* iconn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	void report_process_buffers(CjvxSingleInputConnector* iconn, jvxHandle** bufferPtrs, const jvxLinkDataDescriptor_con_params& params) override;

	// ================================================================================
	// Callbacks for output connector
	// ================================================================================

	jvxErrorType report_selected_connector(CjvxSingleOutputConnector* oconn) override;
	void request_unique_id_start(CjvxSingleOutputConnector* oconn, jvxSize* uId) override;
	jvxErrorType report_started_connector(CjvxSingleOutputConnector* oconn) override;

	jvxErrorType report_stopped_connector(CjvxSingleOutputConnector* ioconn) override;
	void release_unique_id_stop(CjvxSingleOutputConnector* oconn, jvxSize uId) override;
	jvxErrorType report_unselected_connector(CjvxSingleOutputConnector* oconn) override;

	void report_process_buffers(CjvxSingleOutputConnector* oconn, jvxHandle** bufferPtrs, const jvxLinkDataDescriptor_con_params& params) override;

	// =====================================================================
	// Link to default master factory and master implementations
	// =====================================================================
#include "codeFragments/simplify/jvxConnectorMasterFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Master interface: Default implementation requires to find current settings
	// =====================================================================
// #define JVX_CONNECTION_MASTER_SKIP_TEST
#define JVX_SUPPRESS_AUTO_READY_CHECK_MASTER
#define JVX_CONNECTION_MASTER_SKIP_TEST
#define JVX_CONNECTION_MASTER_SKIP_CONNECT
#define JVX_CONNECTION_MASTER_SKIP_DISCONNECT
#define JVX_CONNECTION_MASTER_SKIP_PREPARE
#include "codeFragments/simplify/jvxConnectionMaster_simplify.h"
#undef JVX_CONNECTION_MASTER_SKIP_PREPARE
#undef JVX_CONNECTION_MASTER_SKIP_DISCONNECT
#undef JVX_CONNECTION_MASTER_SKIP_CONNECT
#undef JVX_CONNECTION_MASTER_SKIP_TEST
#undef JVX_SUPPRESS_AUTO_READY_CHECK_MASTER
// #undef JVX_CONNECTION_MASTER_SKIP_TEST

	// Re-define some of the functions
	jvxErrorType connect_chain_master(const jvxChainConnectArguments& args,
		const jvxLinkDataDescriptor_con_params* init_params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	jvxErrorType disconnect_chain_master(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	jvxErrorType transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;	
	jvxErrorType test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;


	// ===============================================================================================
	virtual jvxErrorType process_start_icon(
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL) override;

	virtual jvxErrorType process_stop_icon(jvxSize idx_stage,
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr)override;

	// ===============================================================================================
	// The reference request interface
	// ===============================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION request_reference_object(IjvxObject** obj)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_object(IjvxObject* obj) override;

	// ===============================================================================================
	// ===============================================================================================
	static jvxErrorType lock_sync_threads_cb(jvxHandle* prv);
	static jvxErrorType unlock_sync_threads_cb(jvxHandle* prv);
	jvxErrorType lock_sync_threads_ic();
	jvxErrorType unlock_sync_threads_ic();

	void updateFixedProcessingArgsInOut(jvxBool triggerTestChain);
};

#endif
