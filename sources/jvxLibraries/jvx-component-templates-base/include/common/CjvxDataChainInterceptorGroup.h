#ifndef __CJVXDATACHAININTERCEPTORGROUP_H__
#define __CJVXDATACHAININTERCEPTORGROUP_H__

#include "common/CjvxDataChainInterceptor.h"

#include "development/jvx_define_constants_proc_group_logs.h"

#define CJVXDATACHAININTERCEPTORGROUP_VERBOSE
// #define CJVXDATACHAININTERCEPTORGROUP_TIMING_VERBOSE
// #define JVX_GROUP_INTERCEPTOR_TIMING
// Use a MUTEX or an atmonic to synchronize complete threads
#define JVX_IC_GROUP_USE_ATOMIC_SYNC

class CjvxDataConnectionsGroup;

class CjvxDataChainInterceptorGroup: public CjvxDataChainInterceptor, public IjvxThreads_report, public IjvxDataConnectionShortcut
{
private:
	
	// One element to take from queues and then run in dedicated thread if desired
	struct inThread
	{
		// Processing id
		jvxSize proc_id; 

		// Input data index from pipeline (stage)
		jvxSize idx_stage;

		jvxSize toBeAddressedNext;

		// Reference to identify chain entry in CjvxDataConnectionGroup
		jvxHandle* thread_entry;

		jvxBool run_this_time; 

		jvxDataConnectionsObjectIdentification* conn_id;

		//! ID to identify this object in chain context
		jvxSize id_in_dedicated_list;

		inThread()
		{
			proc_id = JVX_SIZE_UNSELECTED;
			idx_stage = JVX_SIZE_UNSELECTED;
			toBeAddressedNext = JVX_SIZE_UNSELECTED;
			thread_entry = NULL;
			run_this_time = false;
			conn_id = NULL;
			id_in_dedicated_list = 0;
		}
	};

	//! States of buffers: from init to available to ready - processing not used any more
	typedef enum
	{
		JVX_CHAIN_BUFFER_NONE,
		JVX_CHAIN_BUFFER_AVAILABLE,
		JVX_CHAIN_BUFFER_READY,
		JVX_CHAIN_BUFFER_PROCESSING
	}jvxChainBufferState;

	// Store buffers for processing in this list. Typically there is nly one
	struct oneEntryBufferList
	{
		jvxSize proc_id;
		jvxSize idxPipeline;
		jvxChainBufferState stat;
		oneEntryBufferList()
		{
			idxPipeline = JVX_SIZE_UNSELECTED;
			stat = JVX_CHAIN_BUFFER_NONE;
			proc_id = JVX_SIZE_UNSELECTED;
		};
	};

	//! Mutex handle to protect list of buffers for processing
	JVX_MUTEX_HANDLE safeAccess;
	jvxSize idxBufferList_read;
	jvxSize numBufferList_current;
	jvxSize numBufferList;
	oneEntryBufferList* fldBufferList;

	// Runtime id to allocate pipeline. Is obtained when starting processing and received from tools host
	jvxSize my_runtime_id;

	// Reference to parent reference
	CjvxDataConnectionsGroup* myGroupParent;

	// Bitfield to identify "my thread"
	jvxBitField my_thread_bf;
	jvxSize my_thread_bf_position;

	// Bitfield to identify "all threads"
	jvxBitField all_threads_bf;

	// Parameters for threads, holds processing type and number additinal buffers
	jvxDataConnectionsThreadParameters myThreadParameters;
	//jvxDataConnectionsThreadIdentification myThreadIdentification;

	// jvxDataConnectionsObjectIdentification object_id;

	//! Object to realize a thread instance with member function callbacks
	IjvxObject* theThreadObj;

	//! Handle to realize a thread instance with member function callbacks
	IjvxThreads* theThreadHandle;

	//! Effective number of pipeline stages
	jvxSize eff_number_pipelinestages;

	//! Number of buffers that may be added before pipeline operates
	jvxSize eff_number_new_buffers;

	//! Local identification for processing events from one to another thread
	inThread thread_vars;

	JVX_THREAD_ID thread_id;

	//! Timestamp seed for measurements
	jvxTimeStampData myTStampData;

	jvxSize id_task_me;

	jvxTick tickEnterStartIn;
	jvxTick tickEnterStopOut;

	jvxThreadProcessingStrategy strat_this_run;

	jvxBool not_connected;
	jvxBool lock_processing;

#ifdef JVX_BUFFERING_LOGFILE
	jvxSize log_id;
	jvxUInt32* log_buf;
	jvxSize log_sz;
	JVX_MUTEX_HANDLE safeLogBuffer;
#endif

public:
	CjvxDataChainInterceptorGroup(
		IjvxConnectorFactory* myParentRef, IjvxObject* objPar,
		const std::string& name_connection,
		const std::string& name_bridge,
		jvxSize cnt_interceptor,
		IjvxDataChainInterceptorReport* repRef, jvxBool dedicatedThread, 
		jvxBool boostThread, CjvxDataConnectionsGroup* parentRef);
	~CjvxDataChainInterceptorGroup();

	// Forward test parameters
	void set_output_connector(IjvxInputConnector* icon);

	// ===========================================================================

	//! API: Prepare chain call
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	//! API: Starting chain call
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	//! API: Stop of chain call
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	//! API: Postprocess chain call
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	//! API: Run chain before buffer processing
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset, 
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr) override;

	//! API: Run chain for buffer processing
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;

	//! API: Run chain after buffer processing
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
		jvxSize idx_stage, 
		jvxBool shift_fwd, 
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr) override;

	// ===========================================================================

	//! Transfer an event through the chaining pipeline
	jvxErrorType transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* hdl JVX_CONNECTION_FEEDBACK_TYPE_A(var))override;

	// ===========================================================================
	jvxErrorType prepare_thread_processing();
	jvxErrorType run_thread_triggered();
	jvxErrorType try_run_processing(IjvxConnectionMaster* master_ref)override;
	jvxErrorType run_processing_core();
	void prepare_chain_buffer(jvxSize proc_id, jvxHandle* chain_hdl);

	static jvxErrorType st_callback_process_stop_in_lock(jvxSize pipe_idx, jvxHandle* priv_ptr);
	jvxErrorType ic_callback_process_stop_in_lock(jvxSize pipe_idx);

	static jvxErrorType st_callback_process_start_in_lock(jvxSize* pipe_idx, jvxHandle* priv_ptr);
	jvxErrorType ic_callback_process_start_in_lock(jvxSize* pipe_idx);

	// ===========================================================================

	//! API: Call this function on start of parallel thread
	virtual jvxErrorType JVX_CALLINGCONVENTION startup(jvxInt64 timestamp_us) override;

	//! API: Call this function on timer expired of parallel thread
	virtual jvxErrorType JVX_CALLINGCONVENTION expired(jvxInt64 timestamp_us, jvxSize* delta_ms) override;

	//! API: Call this function on wake up event of parallel thread
	virtual jvxErrorType JVX_CALLINGCONVENTION wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms) override;

	//! API: Call this function on stop of parallel thread
	virtual jvxErrorType JVX_CALLINGCONVENTION stopped(jvxInt64 timestamp_us) override;

	// ============================================================

	virtual jvxErrorType trigger_run_out(jvxCBitField what) override;
	virtual jvxErrorType trigger_report_missed() override;

	virtual jvxErrorType trigger_prepare_dedicated() override;
	virtual jvxErrorType trigger_wakeup_dedicated() override;
	virtual void description_bridge(jvxApiString* astr) override;
};

#endif
