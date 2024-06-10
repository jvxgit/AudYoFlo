#ifndef __TJVXDATACONNECTIONS_H__
#define __TJVXDATACONNECTIONS_H__

typedef enum
{
	JVX_MASTER_UPDATE_EVENT_NONE,
	JVX_MASTER_UPDATE_EVENT_STARTED,
	JVX_MASTER_UPDATE_EVENT_ABOUT_TO_STOP,
	JVX_MASTER_UPDATE_EVENT_LAYOUT_CHANGED
} jvxMasterChangedEventType;

typedef enum
{
	JVX_CONNECTION_EVENT_LAYOUT_CHANGED
} jvxConnectionEventType;

typedef enum
{
	JVX_THREAD_PROC_GROUP_MODE_NONE,
	JVX_THREAD_PROC_GROUP_MODE_SEQ_CHAIN_THREADS, // This runs all processing stages in a sequential approach with chaining
	JVX_THREAD_PROC_GROUP_MODE_FULL_THREADS_COUPLED // This runs the stages in different threads, waiting for completion after each step
} jvxProcessingGroupThreadMode;

typedef enum
{
	JVX_LINKDATA_TRANSFER_NONE = 0,
	JVX_LINKDATA_TRANSFER_REQUEST_DATA, /* optional: a number of desired sample count as <jvxSize*> */
	JVX_LINKDATA_TRANSFER_COLLECT_LINK_STRING,
	JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON, /* requires CjvxJsonElementList*/
	JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS,
	JVX_LINKDATA_TRANSFER_REQUEST_PROCESS_ID,
	JVX_LINKDATA_TRANSFER_SPECIFY_THREAD_ID_IN_GROUP, /* arg: jvxSize with thread_id */
	JVX_LINKDATA_TRANSFER_TRIGGER_CHAIN_PROCESS_SEQ_IN_GROUP, /* no arg, chain buffer pipe in non-parallel mode*/
	//JVX_LINKDATA_TRANSFER_PREPARE_CHAIN_PROCESS_PARALLEL_IN_GROUP, /* arg: jvxDataConnectionsThreadIdentification to hold the flags for the threads and a process_step id */
	//JVX_LINKDATA_TRANSFER_TRIGGER_CHAIN_PROCESS_PARALLEL_IN_GROUP, /* no arg, should only run buffer processing*/
	JVX_LINKDATA_TRANSFER_ENUMERATE_THREADS_CHAIN_IN_GROUP, /* arg: jvxDataConnectionsObjectIdentification, add flag on each thread context change */
	JVX_LINKDATA_TRANSFER_BROADCAST_THREADING_PARAMETERS_IN_GROUP, /* arg: jvxDataConnectionsThreadParameters, pass threading model on init */
	//JVX_LINKDATA_TRANSFER_COUNT_AVAIL_BUFFER_MIN_CHAIN_IN_GROUP, /* arg: single jvxSize, count minimum avail buffers  */
	//JVX_LINKDATA_TRANSFER_COUNT_OVERALL_BUFFERS_ADD_PIPELINE, /* arg: single jvxSize, count minimum avail buffers  */
	JVX_LINKDATA_TRANSFER_REQUEST_CHAIN_SPECIFIC_DATA_HINT, /* arg: jvxDataProcessorHintDescriptorSearch */
	JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY, /* arg: jvxApiString to hold simple description for "not ready" */
	JVX_LINKDATA_TRANSFER_TRIGGER_FULL_THREADS_COUPLED, /* ARG?? */
	JVX_LINKDATA_TRANSFER_COMPLETE_LEAVE_THREADS, /* Arg: jvxCBitField */
	JVX_LINKDATA_TRANSFER_SPECIFY_TASKNAMES_CHAIN_IN_GROUP, /* std::vector<std::string> */
	JVX_LINKDATA_TRANSFER_TRIGGER_REPORT_MISSED_FRAMES, /* jvxSize* to hold number frames missed since last successful */
	JVX_LINKDATA_TRANSFER_TRIGGER_REPORT_SINGLE_MISSED_FRAME, /* No argument, always one missed frame reported at the moment of the arrival! */
	JVX_LINKDATA_TRANSFER_CONFIG_TOKEN, /* jvxDataConnectionsTransferConfigToken* */
	JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES, /* jvx::propertyCallCompactList* */
	JVX_LINKDATA_TRANSFER_REQUEST_REAL_MASTER, /* jvxComponentIdentification* */

	/**
	 * The INIT_PRERUN and the TERM_POSTRUN runs must be within the processing loop BUT BEFORE and after all buffers to be processed.
	 * The reason is that some functions need to be allocated in the same thread as the processing itself to work (windows MF),
	 *///=====================================================================================================================
	JVX_LINKDATA_TRANSFER_REQUEST_THREAD_INIT_PRERUN, /* No parameter, indicates that we may init parts in the processing loop */
	JVX_LINKDATA_TRANSFER_REQUEST_THREAD_TERM_POSTRUN, /* No parameter, indicates that we may term parts in the processing loop */
	JVX_LINKDATA_TRANSFER_LIMIT
} jvxLinkDataTransferType;

typedef enum
{
	JVX_PROCESSING_STRATEGY_WAIT,
	JVX_PROCESSING_STRATEGY_REJECT
} jvxThreadProcessingStrategy;

class jvxDataConnectionsThreadParameters
{
public:
	// jvxProcessingGroupThreadMode myThreadMode;
	jvxSize numExtraBuffers;
	jvxBool measureTiming;
	jvxSize max_wait_msec;
	//jvxHandle* chain_hdl;

	jvxDataConnectionsThreadParameters()
	{
		//chain_hdl = NULL;
		// myThreadMode = JVX_THREAD_PROC_GROUP_MODE_FULL_THREADS_COUPLED;
		numExtraBuffers = 0;
		max_wait_msec = 1000;
		measureTiming = true;
	};
};

class jvxDataConnectionsThreadIdentification
{
public:
	
	jvxSize processing_step_id;
	jvxBitField thread_trigger;
	jvxBitField thread_complete;
	jvxHandle* chain_hdl;
	jvxBool in_use;

	jvxDataConnectionsThreadIdentification()
	{
		processing_step_id = JVX_SIZE_UNSELECTED;
		chain_hdl = NULL;
		in_use = false;
	};
};

class jvxDataConnectionsObjectIdentification;

// Forward declaation
JVX_INTERFACE IjvxDataConnectionShortcut
{
public:
	virtual ~IjvxDataConnectionShortcut() {};
	virtual jvxErrorType trigger_report_missed() = 0;
	virtual jvxErrorType trigger_run_out(jvxCBitField what) = 0;
	virtual jvxErrorType trigger_prepare_dedicated() = 0;
	virtual jvxErrorType trigger_wakeup_dedicated() = 0;
	virtual void description_bridge(jvxApiString* astr) = 0;
};

class jvxDataConnectionsObjectIdentification
{
public:

	// Collect all complete threads here
	jvxCBitField thread_id;

	jvxProcessingGroupThreadMode myThreadMode;
	//jvxSize numExtraBuffers;
	//jvxBool measureTiming;
	//jvxSize max_wait_msec;

	// Id in thread
	jvxSize component_id;
	jvxCBitField at_thread_bf;

	// Bit field to indicate "done" with all threads
	jvxCBitField thread_id_complete;

	jvxSize num_tasks;
	jvxData* duration_arvrg_task;
	jvxData* duration_max_task;

	JVX_NOTIFY_HANDLE* notify_complete;
	JVX_MUTEX_HANDLE* synchronized_complete;

	jvxSize numRejected;
	jvxSize numMissedLastTime;

	jvxThreadProcessingStrategy thread_strat;

	std::list<IjvxDataConnectionShortcut*> trigger_pts_dedicated;
	std::list<IjvxDataConnectionShortcut*> trigger_pts_output;

	jvxDataConnectionsObjectIdentification()
	{
		myThreadMode = JVX_THREAD_PROC_GROUP_MODE_SEQ_CHAIN_THREADS;
		component_id = JVX_SIZE_UNSELECTED;
		notify_complete = NULL;
		synchronized_complete = NULL;
		num_tasks = 0;
		duration_arvrg_task = NULL;
		duration_max_task = NULL;
		numRejected = 0;
		numMissedLastTime = 0;
		thread_strat = JVX_PROCESSING_STRATEGY_WAIT;
	};
};

class jvxDataConnectionsTransferConfigToken
{
public:
	const char* token_descr;
	const char* token_content;
	jvxDataConnectionsTransferConfigToken()
	{
		token_descr = NULL;
		token_content = NULL;
	};
};

/* Previous default parameters
   jvxBool interceptors, jvxBool essential, jvxBool connect_process = true, jvxBool dbg_output = false, const char* descr_object = ""
 */

class jvxDataConnectionRuleParameters
{
public:
	jvxBool interceptors = false;
	jvxBool essential = true;
	jvxBool connect_process = true;
	jvxBool dbg_output = false;
	jvxBool report_automation = false;
	const char* descr_object = "";
	jvxBool proper_format_groups_only = false;
	jvxSize minNumConnectionsReady = 0;
	jvxBool preventStoreConfig = true;

	jvxDataConnectionRuleParameters(jvxBool interceptors_arg = false,
		jvxBool essential_arg = true,
		jvxBool connect_process_arg = true,
		jvxBool dbg_output_arg = false,
		jvxBool report_automation_arg = false,
		jvxBool proper_format_groups_only_arg = false,
		const char* descr_object_arg = "",
		jvxSize minNumConnectionsReadyArg = 0) :
		interceptors(interceptors_arg),
		essential(essential_arg),
		connect_process(connect_process_arg),
		dbg_output(dbg_output_arg),
		report_automation(report_automation_arg),
		descr_object(descr_object_arg),
		proper_format_groups_only(proper_format_groups_only_arg),
		minNumConnectionsReady(minNumConnectionsReadyArg)
	{
	}
};

class jvxChainConnectArguments
{
public:
	jvxSize uIdConnection = JVX_SIZE_UNSELECTED;
	IjvxConnectionMaster* theMaster = nullptr;
	// const jvxLinkDataDescriptor_con_params* init_params;
	jvxChainConnectArguments(
		/*const jvxLinkDataDescriptor_con_params* init_ps, */
		jvxSize uIdConn = JVX_SIZE_UNSELECTED, IjvxConnectionMaster* mas = nullptr) :
		// init_params(init_ps), 
		uIdConnection(uIdConn), theMaster(mas){};
};

#endif