#include "common/CjvxDataChainInterceptorGroup.h"
#include "jvxHosts/CjvxDataConnectionsGroup.h"
#include "development/jvx_define_constants_proc_group_logs.h"

#define JVX_ALPHA_DURATION 0.95
#define JVX_ALPHA_FORGET 0.995
#define JVX_INVOLVE_SHORTCUTS

#ifdef JVX_BUFFERING_LOGFILE
#define LOG_BUF_WRITE(COMPONENT_ID, CHAIN_THREAD_ID, OPER, SUC, PROC, IDF, IDT, TID) \
		 if (log_buf) \
		 { \
			JVX_LOCK_MUTEX(safeLogBuffer); \
			log_buf[0] = COMPONENT_ID; \
			log_buf[1] = CHAIN_THREAD_ID; \
			log_buf[2] = OPER; \
			log_buf[3] = SUC; \
			log_buf[4] = PROC; \
			log_buf[5] = IDF; \
			log_buf[6] = IDT; \
			log_buf[7] = JVX_THREAD_ID_UINT32(TID);	      \
			myGroupParent->write_single_instance(log_id); \
			JVX_UNLOCK_MUTEX(safeLogBuffer); \
		}
#else
#define LOG_BUF_WRITE(INSTANCE, OPER, SUC, PROC, IDF, IDT)
#endif

CjvxDataChainInterceptorGroup::CjvxDataChainInterceptorGroup(
	IjvxConnectorFactory* myParentRef, IjvxObject* objPar, 
		const std::string& name_connection,
		const std::string& name_bridge,
		jvxSize cnt_interceptor,
		IjvxDataChainInterceptorReport* repRef, jvxBool dedicatedThread, jvxBool boostThread,
		CjvxDataConnectionsGroup* parentRef):
	CjvxDataChainInterceptor(myParentRef, objPar,  name_connection, name_bridge, cnt_interceptor,repRef,
		dedicatedThread,  boostThread), myGroupParent(parentRef)
{
	isProcess = false;

	JVX_INITIALIZE_MUTEX(safeAccess);
	idxBufferList_read = JVX_SIZE_UNSELECTED;
	numBufferList_current = 0;
	numBufferList = 0;
	fldBufferList = NULL;
	theThreadObj = NULL;
	theThreadHandle = NULL;
	eff_number_pipelinestages = JVX_SIZE_UNSELECTED;
	eff_number_new_buffers = JVX_SIZE_UNSELECTED;

#ifdef JVX_BUFFERING_LOGFILE
	log_id = JVX_SIZE_UNSELECTED;
	log_buf = NULL;
	log_sz = 0;
	JVX_INITIALIZE_MUTEX(safeLogBuffer);
#endif

	my_thread_bf_position = JVX_SIZE_UNSELECTED;
	thread_id = JVX_NULLTHREAD;

	id_task_me = JVX_SIZE_UNSELECTED;
	strat_this_run = JVX_PROCESSING_STRATEGY_WAIT;	 
	not_connected = false;
}

CjvxDataChainInterceptorGroup::~CjvxDataChainInterceptorGroup()
{
	JVX_TERMINATE_MUTEX(safeAccess);
	JVX_INITIALIZE_MUTEX(safeLogBuffer);
}

void 
CjvxDataChainInterceptorGroup::set_output_connector(IjvxInputConnector* iconPar)
{
	icon = iconPar;
}

// =============================================================================

/*
jvxErrorType 
CjvxDataChainInterceptorGroup::connect_connect_icon(jvxLinkDataDescriptor* theData
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numThreads = 1; // Start with the input thread
	res = CjvxDataChainInterceptor::connect_connect_icon(theData
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
		{
			
			jvxErrorType resL = myGroupParent->connect_thread_pipeline(this, numThreads);
			assert(resL == JVX_NO_ERROR);
		}
	}
	return res;
}
*/

jvxErrorType 
CjvxDataChainInterceptorGroup::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	// Set local timer reference
	JVX_GET_TICKCOUNT_US_SETREF(&myTStampData);

	std::string reg_nm = "GROUP-";
	reg_nm += nm_process;
	reg_nm += "-";
	reg_nm += nm_ic;

#ifdef JVX_BUFFERING_LOGFILE
	log_id = JVX_SIZE_UNSELECTED;
	log_buf = NULL;
	log_sz = 0;
#endif

	thread_id = JVX_GET_CURRENT_THREAD_ID();
	not_connected = false;

	if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
	{
		jvxSize cnt = 0;

#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << "### ==========================================" << std::endl;
		std::cout << "### Entering Processing Group via Interceptor " << nm_ic << " -- " << __FUNCTION__ << std::endl;
		std::cout << "### ==========================================" << std::endl;
#endif
		// =======================================================================0
		// run local prepare
		// =======================================================================0

		// From here, prepare processing group
		myGroupParent->prepare_local();
		
		// =======================================================================0
		// Fill bitfield for all threads in chain
		// =======================================================================0

		// Pass thread bitfield to parent
		resL = myGroupParent->prepare_thread_pipeline(this, &thread_vars.conn_id);
		thread_vars.conn_id->thread_id_complete = thread_vars.conn_id->thread_id;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(thread_vars.conn_id->notify_complete, JVX_NOTIFY_HANDLE);
		JVX_INITIALIZE_NOTIFICATION(*thread_vars.conn_id->notify_complete);
		JVX_DSP_SAFE_ALLOCATE_OBJECT(thread_vars.conn_id->synchronized_complete, JVX_MUTEX_HANDLE);
		JVX_INITIALIZE_MUTEX(*thread_vars.conn_id->synchronized_complete );

		assert(resL == JVX_NO_ERROR);
	}
	else
	{
#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << "### Interceptor " << nm_ic << " -- " << __FUNCTION__ << std::endl;
#endif
	}

#ifdef JVX_BUFFERING_LOGFILE
	myGroupParent->register_instance(reg_nm, log_id);
#endif

	if (
		(dedicatedThread)||
		(my_tag == JVX_PROCESS_MARK_LEAVE_CONNECTOR))
	{
		thread_vars.idx_stage = JVX_SIZE_UNSELECTED;
		thread_vars.proc_id = JVX_SIZE_UNSELECTED;

		//hostRef->;
		
		assert(uniqueId);
		uniqueId->obtain_unique_id(&my_runtime_id, reg_nm.c_str());
		thread_vars.proc_id = my_runtime_id;

		idx_pipe_stage = theData_in->con_pipeline.num_additional_pipleline_stages;

/*
#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << " --- Buffer stage id in pipeline: " << idx_pipe_stage << std::endl;
#endif
*/
		// We need an additional pipeline stage due to new thread environment
		theData_in->con_pipeline.num_additional_pipleline_stages++;

		res = CjvxDataChainInterceptor::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			if (theData_in->con_data.buffers)
			{
				eff_number_pipelinestages = theData_in->con_pipeline.num_additional_pipleline_stages;
				eff_number_new_buffers = theData_in->con_data.number_buffers - theData_in->con_pipeline.num_additional_pipleline_stages;

				numBufferList = theData_in->con_data.number_buffers;

				// Do not set to zero, it is a class with constructors
				JVX_DSP_SAFE_ALLOCATE_FIELD(fldBufferList, oneEntryBufferList, numBufferList);
				/* Elements have a default configuration already, no need to set it manually
				for (i = 0; i < numBufferList; i++)
				{
					fldBufferList[i].idxPipeline = JVX_SIZE_UNSELECTED;
					fldBufferList[i].stat = JVX_CHAIN_BUFFER_NONE;
				}*/
				numBufferList_current = 0;
				idxBufferList_read = 0;

				thread_vars.toBeAddressedNext = (idx_pipe_stage + 1) % (theData_in->con_pipeline.num_additional_pipleline_stages + 1);
			}
			else
			{
				not_connected = true;
			}
		}

		theThreadObj = NULL;
		theThreadHandle = NULL;
		
		if (my_tag != JVX_PROCESS_MARK_LEAVE_CONNECTOR)
		{
			// A thread object is only required in case of the nodes which do not "leave"
			if (thread_vars.conn_id->myThreadMode == JVX_THREAD_PROC_GROUP_MODE_FULL_THREADS_COUPLED)
			{
				toolsHost->instance_tool(JVX_COMPONENT_THREADS, &theThreadObj, 0, NULL);
				assert(theThreadObj);

				theThreadObj->request_specialization(reinterpret_cast<jvxHandle**>(&theThreadHandle), NULL, NULL, NULL);
				assert(theThreadHandle);

				resL = theThreadHandle->initialize(static_cast<IjvxThreads_report*>(this));
				assert(resL == JVX_NO_ERROR);
			}
		}
	}
	else
	{
/*
#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << " --- No dedicated thread." << std::endl;
#endif
*/
		res = CjvxDataChainInterceptor::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (theData_in->con_data.buffers == NULL)
		{
			not_connected = true;
		}
	}

	if (res == JVX_NO_ERROR)
	{
		if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
		{
			//resL = myGroupParent->start_thread_pipeline(this);
			//assert(resL == JVX_NO_ERROR);
		}
	}
	return res;
}

jvxErrorType
CjvxDataChainInterceptorGroup::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType resL = JVX_NO_ERROR;
	if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
	{
		//resL = myGroupParent->stop_thread_pipeline(this);
		//assert(resL == JVX_NO_ERROR);
	}

	if (
		(dedicatedThread)||
		(my_tag == JVX_PROCESS_MARK_LEAVE_CONNECTOR))
	{
		log_id = JVX_SIZE_UNSELECTED;

		//if (my_tag != JVX_PROCESS_MARK_LEAVE_CONNECTOR) <- not needed since theThreadHandle is NULL
		//{ 
			if (theThreadHandle)
			{
				resL = theThreadHandle->terminate();
				assert(resL == JVX_NO_ERROR);

				toolsHost->return_instance_tool(JVX_COMPONENT_THREADS, theThreadObj, 0, NULL);
			}
		//}
		theThreadObj = NULL;
		theThreadHandle = NULL;

		// ======================================================================

		JVX_DSP_SAFE_DELETE_FIELD(fldBufferList);
		numBufferList_current = 0;
		idxBufferList_read = 0;
		numBufferList = 0;

		eff_number_pipelinestages = JVX_SIZE_UNSELECTED;
		eff_number_new_buffers = JVX_SIZE_UNSELECTED;

		// ======================================================================

		resL = CjvxDataChainInterceptor::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(resL == JVX_NO_ERROR);

		theData_in->con_pipeline.num_additional_pipleline_stages = 0;

		my_runtime_id = JVX_SIZE_UNSELECTED;

		thread_vars.idx_stage = JVX_SIZE_UNSELECTED;
		thread_vars.proc_id = JVX_SIZE_UNSELECTED;
	}
	else
	{
		resL = CjvxDataChainInterceptor::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(resL == JVX_NO_ERROR);
	}

	if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
	{
		jvxSize cnt = 0;

		thread_vars.conn_id->thread_id_complete = 0;
		JVX_TERMINATE_NOTIFICATION(*thread_vars.conn_id->notify_complete);
		JVX_DSP_SAFE_DELETE_OBJECT(thread_vars.conn_id->notify_complete);
		thread_vars.conn_id->notify_complete = NULL;
		
		JVX_TERMINATE_MUTEX(*thread_vars.conn_id->synchronized_complete);
		JVX_DSP_SAFE_DELETE_OBJECT(thread_vars.conn_id->synchronized_complete);
		thread_vars.conn_id->synchronized_complete = NULL;

		// Pass thread bitfield to parent
		resL = myGroupParent->postprocess_thread_pipeline(this);
		assert(resL == JVX_NO_ERROR);

		// From here, prepare processing group
		myGroupParent->postprocess_local();
	}
	return JVX_NO_ERROR;
}

// ====================================================================================

jvxErrorType
CjvxDataChainInterceptorGroup::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
	{
		jvxSize cnt = 0;

		// =======================================================================0
		// run local prepare
		// =======================================================================0

		// From here, prepare processing group
		myGroupParent->start_local();
	}

#ifdef JVX_BUFFERING_LOGFILE
	myGroupParent->memory_instance(&log_buf, &log_sz, log_id);
#endif

	if (dedicatedThread)
	{
		if (thread_vars.conn_id->myThreadMode == JVX_THREAD_PROC_GROUP_MODE_FULL_THREADS_COUPLED)
		{
			// start the thread for this "edge"
			jvxErrorType resL = theThreadHandle->start(JVX_SIZE_UNSELECTED, true, true);
			assert(resL == JVX_NO_ERROR);

			theThreadHandle->thread_id(&thread_id);
		}
	}
	res = CjvxDataChainInterceptor::start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxDataChainInterceptorGroup::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxDataChainInterceptor::stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (dedicatedThread)
	{
		log_buf = NULL;
		log_sz = 0;

		if (thread_vars.conn_id->myThreadMode == JVX_THREAD_PROC_GROUP_MODE_FULL_THREADS_COUPLED)
		{
			// start the thread for this "edge"
			jvxErrorType resL = theThreadHandle->stop(JVX_SIZE_UNSELECTED);
			assert(resL == JVX_NO_ERROR);
		}
	}

	if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
	{
		jvxSize cnt = 0;

		// =======================================================================0
		// run local prepare
		// =======================================================================0

		// From here, prepare processing group
		myGroupParent->stop_local();
	}
	return res;
}

//====================================================================================

jvxErrorType 
CjvxDataChainInterceptorGroup::process_start_icon(
	jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize numBufsAvail = SIZE_MAX;
	jvxCBitField trigger_threads_output = 0;
#ifdef JVX_INVOLVE_SHORTCUTS
	std::list< IjvxDataConnectionShortcut*>::iterator elmD;
#endif

#if 0
	LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_PROCESSING_START_ENTER,
		JVX_LOG_PROC_GROUPS_SUCCESS_OK, -1, -1, -1, thread_id);
#endif

	if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
	{
		strat_this_run = thread_vars.conn_id->thread_strat;
		lock_processing = false;

		if (strat_this_run == JVX_PROCESSING_STRATEGY_REJECT)
		{
			if (thread_vars.conn_id->thread_id_complete != thread_vars.conn_id->thread_id)
			{
				thread_vars.conn_id->numRejected++;
				thread_vars.conn_id->numMissedLastTime++;
				lock_processing = true;

				auto elmO = thread_vars.conn_id->trigger_pts_output.begin();
				for (; elmO != thread_vars.conn_id->trigger_pts_output.end(); elmO++)
				{
					// We instantansouly report that we missed a buffer here which we may use for better handling!
					resL = (*elmO)->trigger_report_missed();
					assert(resL == JVX_NO_ERROR);
				}

				return JVX_ERROR_COMPONENT_BUSY;
			}
			else
			{
				if (thread_vars.conn_id->numMissedLastTime)
				{
					resL = CjvxDataChainInterceptor::transfer_forward_ocon(
						JVX_LINKDATA_TRANSFER_TRIGGER_REPORT_MISSED_FRAMES, 
						&thread_vars.conn_id->numMissedLastTime
						JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
					assert(resL == JVX_NO_ERROR);
				}
				thread_vars.conn_id->numMissedLastTime = 0;
			}
		}

		thread_vars.conn_id->thread_id = 0;

		if (myThreadParameters.measureTiming)
		{
			tickEnterStartIn = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStampData);
		}

#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << "### ==========================================" << std::endl;
		std::cout << "### Entering Processing Group via Interceptor " << nm_ic << " -- " __FUNCTION__ << std::endl;
#endif
		// This code is executed BEFORE all functions for a single buffer are called.
		switch (thread_vars.conn_id->myThreadMode)
		{
		case JVX_THREAD_PROC_GROUP_MODE_FULL_THREADS_COUPLED:

#ifdef JVX_INVOLVE_SHORTCUTS
			elmD = thread_vars.conn_id->trigger_pts_dedicated.begin();
			for (; elmD != thread_vars.conn_id->trigger_pts_dedicated.end(); elmD++)
			{
				res = (*elmD)->trigger_prepare_dedicated();
				assert(res == JVX_NO_ERROR);
			}
#else

			// We prepare the pipeline stages here and trigger the secondary threads
			resL = CjvxDataChainInterceptor::transfer_forward_ocon(
				JVX_LINKDATA_TRANSFER_TRIGGER_FULL_THREADS_COUPLED, thread_vars.conn_id
				JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
			assert(resL == JVX_NO_ERROR);
#endif

			if (strat_this_run == JVX_PROCESSING_STRATEGY_WAIT)
			{
				// Start wait condition here already
				JVX_WAIT_FOR_NOTIFICATION_I(*thread_vars.conn_id->notify_complete);

#ifdef JVX_INVOLVE_SHORTCUTS
				elmD = thread_vars.conn_id->trigger_pts_dedicated.begin();
				for (; elmD != thread_vars.conn_id->trigger_pts_dedicated.end(); elmD++)
				{
					res = (*elmD)->trigger_wakeup_dedicated();
					assert(res == JVX_NO_ERROR);
				}
#endif
			}
			else
			{
#ifdef JVX_INVOLVE_SHORTCUTS

				auto elmO = thread_vars.conn_id->trigger_pts_output.begin();
				for (; elmO != thread_vars.conn_id->trigger_pts_output.end(); elmO++)
				{
					resL = (*elmO)->trigger_run_out(0x1);
					assert(resL == JVX_NO_ERROR);
				}
#else
				trigger_threads_output = 0x1;
				// Trigger all output connectors! Propagate through the chains
				resL = CjvxDataChainInterceptor::transfer_forward_ocon(
					JVX_LINKDATA_TRANSFER_COMPLETE_LEAVE_THREADS, &trigger_threads_output
					JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
				assert(resL == JVX_NO_ERROR);
#endif

#ifdef JVX_INVOLVE_SHORTCUTS
				elmD = thread_vars.conn_id->trigger_pts_dedicated.begin();
				for (; elmD != thread_vars.conn_id->trigger_pts_dedicated.end(); elmD++)
				{
					res = (*elmD)->trigger_wakeup_dedicated();
					assert(res == JVX_NO_ERROR);
				}
#endif
			}
			break;
		}
	}
	else
	{
#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << "### Interceptor " << nm_ic << " -- " << __FUNCTION__ << std::endl;
#endif
	}

	if (dedicatedThread)
	{
		// This chain run stops here
		jvx_shift_buffer_pipeline_idx_on_start(theData_in, my_runtime_id, pipeline_offset, 
			idx_stage, idx_pipe_stage, clbk, priv_ptr);
		
		// Check that everything allows to input data
		res = JVX_NO_ERROR;
	}
	else
	{
		res = CjvxDataChainInterceptor::process_start_icon(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	}

#if 0
	LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_PROCESSING_START_LEAVE,
		JVX_LOG_PROC_GROUPS_SUCCESS_OK, -1, -1, -1, thread_id);
#endif

	return res;
}

jvxErrorType 
CjvxDataChainInterceptorGroup::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxTick tickEnter = 0;
	jvxTick tickDoneProcess = 0;
#if 0
	LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_PROCESSING_ENTER,
		JVX_LOG_PROC_GROUPS_SUCCESS_OK, -1, -1, -1, thread_id);
#endif
	if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
	{
		if (lock_processing)
		{
			return JVX_ERROR_COMPONENT_BUSY;
		}

		if (myThreadParameters.measureTiming)
		{
			tickEnter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStampData);
		}
	}

	if (
		(dedicatedThread)||
		(my_tag == JVX_PROCESS_MARK_LEAVE_CONNECTOR))
	{
		// End pipeline here, data has been passed
		return JVX_NO_ERROR;
	}
	else
	{
		res = CjvxDataChainInterceptor::process_buffers_icon(mt_mask, idx_stage);
	}

	if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
	{
		if (myThreadParameters.measureTiming)
		{
			tickDoneProcess = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStampData);
		}

		jvxData time_msec = 0.001 * (tickDoneProcess - tickEnter);
		thread_vars.conn_id->duration_arvrg_task[1] = 
			(1-JVX_ALPHA_DURATION) * time_msec + (JVX_ALPHA_DURATION) * thread_vars.conn_id->duration_arvrg_task[1];
		if (time_msec > thread_vars.conn_id->duration_max_task[1])
		{
			thread_vars.conn_id->duration_max_task[1] = time_msec;
		}
		else
		{
			thread_vars.conn_id->duration_max_task[1] *= JVX_ALPHA_FORGET;
		}

#ifdef JVX_GROUP_INTERCEPTOR_TIMING
		std::cout << "Group Core processing" << nm_process << ": processing time = " << time_msec << " msecs." << std::endl;
#endif
	}

#if 0
	LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_PROCESSING_LEAVE,
		JVX_LOG_PROC_GROUPS_SUCCESS_OK, -1, -1, -1, thread_id);
#endif
	return res;
}

jvxErrorType 
CjvxDataChainInterceptorGroup::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxSize idx_complete = JVX_SIZE_UNSELECTED;
	jvxSize idxWrite = 0;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize toBeAddressedNext = 0;
	jvxCBitField trigger_threads_output = 0;

	if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
	{
		if (lock_processing)
		{
			return JVX_ERROR_COMPONENT_BUSY;
		}
	}

#if 0
	LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_PROCESSING_STOP_ENTER,
		JVX_LOG_PROC_GROUPS_SUCCESS_OK, -1, -1, -1, thread_id);
#endif

	if (
		(dedicatedThread) ||
		(my_tag == JVX_PROCESS_MARK_LEAVE_CONNECTOR)) 
	{
		if (!not_connected)
		{
			// Store completed pipeline stage id
			idx_complete = *theData_in->con_pipeline.idx_stage_ptr;

			toBeAddressedNext = (idx_pipe_stage + 1) % (theData_in->con_pipeline.num_additional_pipleline_stages + 1);

			// Release buffer and pass over to the next processing block
			res = jvx_shift_buffer_pipeline_idx_on_stop(theData_in, idx_stage, shift_fwd, toBeAddressedNext,
				st_callback_process_stop_in_lock, reinterpret_cast<jvxHandle*>(this));

			if (thread_vars.conn_id->myThreadMode == JVX_THREAD_PROC_GROUP_MODE_SEQ_CHAIN_THREADS)
			{
				// Run the dedicated threads here. Do not run the leave connectors
				if (my_tag != JVX_PROCESS_MARK_LEAVE_CONNECTOR)
				{
					resL = prepare_thread_processing();
					if (resL == JVX_NO_ERROR)
					{
						thread_vars.run_this_time = true;
						this->run_thread_triggered();
					}
				}
			}
		}
	}
	else
	{
		res = CjvxDataChainInterceptor::process_stop_icon(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
	}

	// If we entered chain here, we need to run all coupled elements
	if (my_tag == JVX_PROCESS_MARK_ENTER_CONNECTOR)
	{
		std::list<IjvxDataConnectionShortcut*>::iterator elmO;
		switch (thread_vars.conn_id->myThreadMode)
		{
		case JVX_THREAD_PROC_GROUP_MODE_NONE:
			assert(0);
			break;
		case JVX_THREAD_PROC_GROUP_MODE_SEQ_CHAIN_THREADS:

#ifdef JVX_INVOLVE_SHORTCUTS
			elmO = thread_vars.conn_id->trigger_pts_output.begin();
			for (; elmO != thread_vars.conn_id->trigger_pts_output.end(); elmO++)
			{
				resL = (*elmO)->trigger_run_out(0x3);
				assert(resL == JVX_NO_ERROR);
			}
#else
			trigger_threads_output = 0x3;
			// Trigger all output connectors! Propagate through the chains
			resL = CjvxDataChainInterceptor::transfer_forward_ocon(
				JVX_LINKDATA_TRANSFER_COMPLETE_LEAVE_THREADS, &trigger_threads_output
				JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
			assert(resL == JVX_NO_ERROR);
#endif
			break;

		case JVX_THREAD_PROC_GROUP_MODE_FULL_THREADS_COUPLED:
			
			if (strat_this_run == JVX_PROCESSING_STRATEGY_WAIT)
			{
				// Wait for parallel processing to complete
				JVX_WAIT_RESULT resW = JVX_WAIT_FOR_NOTIFICATION_II_MS(*thread_vars.conn_id->notify_complete, 
					myThreadParameters.max_wait_msec);
				if (resW != JVX_WAIT_SUCCESS)
				{
					std::cout << __FUNCTION__ << ": Timeout expired in processing group wait." << this->nm_process << std::endl;
					// JVX_ASSERT(0);
				}

#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
				std::cout << "### Wait for parallel threads completed." << std::endl;
#endif
				// From here, forward all output connectors
#ifdef JVX_INVOLVE_SHORTCUTS
				elmO = thread_vars.conn_id->trigger_pts_output.begin();
				for (; elmO != thread_vars.conn_id->trigger_pts_output.end(); elmO++)
				{
					resL = (*elmO)->trigger_run_out(0x3);
					assert(resL == JVX_NO_ERROR);
				}
#else
				trigger_threads_output = 0x3;
				// Trigger all output connectors! Propagate through the chains
				resL = CjvxDataChainInterceptor::transfer_forward_ocon(
					JVX_LINKDATA_TRANSFER_COMPLETE_LEAVE_THREADS, &trigger_threads_output
					JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
				assert(resL == JVX_NO_ERROR);
#endif
			}
			else
			{
				// From here, forward all output connectors
#ifdef JVX_INVOLVE_SHORTCUTS
				elmO = thread_vars.conn_id->trigger_pts_output.begin();
				for (; elmO != thread_vars.conn_id->trigger_pts_output.end(); elmO++)
				{
					resL = (*elmO)->trigger_run_out(0x2);
					assert(resL == JVX_NO_ERROR);
				}
#else
				trigger_threads_output = 0x2;
				// Trigger all output connectors! Propagate through the chains
				resL = CjvxDataChainInterceptor::transfer_forward_ocon(
					JVX_LINKDATA_TRANSFER_COMPLETE_LEAVE_THREADS, &trigger_threads_output
					JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
				assert(resL == JVX_NO_ERROR);
#endif
			}
			break;

		}
#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << "### Leaving Processing Group via Interceptor " << nm_ic << " -- " __FUNCTION__ << std::endl;
		std::cout << "### ==========================================" << std::endl;
#endif
		if (myThreadParameters.measureTiming)
		{
			tickEnterStopOut = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStampData);
		}
		jvxData time_msec = 0.001 * (tickEnterStopOut - tickEnterStartIn);
		thread_vars.conn_id->duration_arvrg_task[0] =
			(1 - JVX_ALPHA_DURATION) * time_msec + (JVX_ALPHA_DURATION)* thread_vars.conn_id->duration_arvrg_task[0];
		if (time_msec > thread_vars.conn_id->duration_max_task[0])
		{
			thread_vars.conn_id->duration_max_task[0] = time_msec;
		}
		else
		{
			thread_vars.conn_id->duration_max_task[0] *= JVX_ALPHA_FORGET;
		}


#ifdef JVX_GROUP_INTERCEPTOR_TIMING
		std::cout << "Group " << nm_process << ": processing time = " << time_msec << " msecs." << std::endl;
#endif
	}
	else
	{
#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << "### Interceptor " << nm_ic << " -- " << __FUNCTION__  << std::endl;
#endif
	}

#if 0
	LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_PROCESSING_STOP_LEAVE,
		JVX_LOG_PROC_GROUPS_SUCCESS_OK, -1, -1, -1, thread_id);
#endif

	return res;
}

// ======================================================================================================

jvxErrorType 
CjvxDataChainInterceptorGroup::try_run_processing(IjvxConnectionMaster* master_ref)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	if(theData_in)
	{
		if (master_ref == theData_in->con_link.master)
		{
			res = run_thread_triggered();
		}
	}
	return res;
}

jvxErrorType
CjvxDataChainInterceptorGroup::prepare_thread_processing()
{
	jvxTick tickEnter = 0;
	jvxTick tickLeave1 = 0;
	jvxErrorType res = JVX_NO_ERROR;
	thread_vars.idx_stage = JVX_SIZE_UNSELECTED;

	if (myThreadParameters.measureTiming)
	{
		tickEnter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStampData);
	}

	// Trigger start BUT 
	res = this->process_start_ocon(0, &thread_vars.idx_stage, 0,
		st_callback_process_start_in_lock,
		reinterpret_cast<jvxHandle*>(this));

	if (myThreadParameters.measureTiming)
	{
		tickLeave1 = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStampData);
	}

	return res;
}

jvxErrorType
CjvxDataChainInterceptorGroup::run_thread_triggered()
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxTick tickEnter = 0;
	jvxTick tickDoneProcess = 0;
	jvxTick tickDoneStop = 0;

	assert(
		(dedicatedThread) ||
		(my_tag == JVX_PROCESS_MARK_LEAVE_CONNECTOR));

	if (thread_vars.run_this_time)
	{
#if 0
		LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_THREAD_ENTER,
			JVX_LOG_PROC_GROUPS_SUCCESS_OK, -1, -1, -1, thread_id);
#endif
		if (myThreadParameters.measureTiming)
		{
			tickEnter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStampData);
		}

		resL = this->process_buffers_ocon(JVX_SIZE_UNSELECTED, thread_vars.idx_stage);
		assert(resL == JVX_NO_ERROR);

		if (myThreadParameters.measureTiming)
		{
			tickDoneProcess = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStampData);
		}

		// Very important: DO NOT forward the pipeline pointer since we did not write to the buffer
		resL = this->process_stop_ocon(thread_vars.idx_stage, false, thread_vars.toBeAddressedNext, NULL, NULL);
		assert(resL == JVX_NO_ERROR);

		if (myThreadParameters.measureTiming)
		{
			tickDoneStop = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStampData);
		}

#ifdef CJVXDATACHAININTERCEPTORGROUP_TIMING_VERBOSE
		std::cout << "Pure processing Time <" << nm_ic << "> = " << (tickLeave1 - tickEnter) << " -- " <<
			(tickLeave2 - tickLeave1) << "--" << (tickLeave3 - tickLeave2) << "  [usec]" << std::endl;
#endif
		jvxData time_msec = 0.001 * (tickDoneProcess - tickEnter);

		thread_vars.conn_id->duration_arvrg_task[id_task_me] = 
			(1 - JVX_ALPHA_DURATION) * time_msec + 
			(JVX_ALPHA_DURATION)* thread_vars.conn_id->duration_arvrg_task[id_task_me];
		if (time_msec > thread_vars.conn_id->duration_max_task[id_task_me])
		{
			thread_vars.conn_id->duration_max_task[id_task_me] = time_msec;
		}
		else
		{
			thread_vars.conn_id->duration_max_task[id_task_me] *= JVX_ALPHA_FORGET;
		}

#ifdef JVX_GROUP_INTERCEPTOR_TIMING
		std::cout << "Running from bridge" << nm_bridge << "( task #" << id_task_me << "): processing time = " << time_msec << "msecs." << std::endl;
#endif


	}
	thread_vars.run_this_time = false;
	return res;
}

jvxErrorType
CjvxDataChainInterceptorGroup::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* hdl
	JVX_CONNECTION_FEEDBACK_TYPE_A(var))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxDataConnectionsThreadIdentification* arg_thread = (jvxDataConnectionsThreadIdentification*) hdl;
	jvxSize* arg_sz = (jvxSize*)hdl;
	jvxDataConnectionsObjectIdentification* arg_bf = (jvxDataConnectionsObjectIdentification*)hdl;
	jvxDataConnectionsThreadParameters* myThreads = (jvxDataConnectionsThreadParameters*)hdl;
	jvxCBitField* trigger_bf = (jvxCBitField*)hdl;
	std::vector<std::string>* lst_tasknames = (std::vector<std::string>*)hdl;

	jvxBool fwd = false;
	jvxBool fwd_ingroup = false;
	jvxBool trigger_thread = false;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_TRIGGER_REPORT_MISSED_FRAMES:
		fwd_ingroup = true;
		break;

	case JVX_LINKDATA_TRANSFER_SPECIFY_TASKNAMES_CHAIN_IN_GROUP:
		
		if (JVX_CHECK_SIZE_SELECTED(id_task_me))
		{
			if (id_task_me < lst_tasknames->size())
			{
				(*lst_tasknames)[id_task_me] = nm_bridge;
			}
		}
		fwd_ingroup = true;
		break;
	case JVX_LINKDATA_TRANSFER_TRIGGER_FULL_THREADS_COUPLED:

		if (dedicatedThread)
		{
			trigger_thread = true;
			jvx_bitClear(arg_bf->thread_id, my_thread_bf_position);
			if (numBufferList_current > 0)
			{
#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
				std::cout << "### Prepare parallel thread: " << numBufferList_current << " buffers available for processing, interceptor " <<
					nm_ic << " -- " << __FUNCTION__ << std::endl;
#endif
				res = prepare_thread_processing();
				assert(res == JVX_NO_ERROR);
				thread_vars.run_this_time = true;
			}
			else
			{
#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
				std::cout << "### Prepare parallel thread: No buffer available for processing, interceptor " << 
					nm_ic << " -- " << __FUNCTION__ << std::endl;
#endif
				// Highlight this thread as "complete"
				thread_vars.run_this_time = false;
			}
		}
		fwd_ingroup = true;
		break;

	case JVX_LINKDATA_TRANSFER_TRIGGER_CHAIN_PROCESS_SEQ_IN_GROUP:

		if (dedicatedThread)
		{
			if (JVX_EVALUATE_BITFIELD(arg_thread->thread_trigger & my_thread_bf))
			{
				run_thread_triggered();
				assert(theThreadHandle);
			}
		}

		break;

	case JVX_LINKDATA_TRANSFER_ENUMERATE_THREADS_CHAIN_IN_GROUP:

		id_task_me = JVX_SIZE_UNSELECTED;
		if (dedicatedThread)
		{
			jvxSize cnt = 0;
			my_thread_bf.bit_fclear();
			while (1)
			{
				if (!jvx_bitTest(arg_bf->thread_id, cnt))
				{
					std::cout << " ---> Thread #" << cnt << " -- <" << nm_ic << ">." << std::endl;

					thread_vars.id_in_dedicated_list = cnt;

					jvx_bitSet(arg_bf->thread_id, cnt);
					jvx_bitSet(my_thread_bf, cnt);
					my_thread_bf_position = cnt;
					break;
				}
				cnt++;
			}
		}
		if(
			dedicatedThread ||
			(my_tag == JVX_PROCESS_MARK_LEAVE_CONNECTOR))
		{
			// deidicated thread but as well the leave connector are dedicated tasks
			id_task_me = arg_bf->num_tasks;
			arg_bf->num_tasks++;
			thread_vars.conn_id = arg_bf;
			if (dedicatedThread)
			{
				arg_bf->trigger_pts_dedicated.push_back(static_cast<IjvxDataConnectionShortcut*>(this));
			}
			if (my_tag == JVX_PROCESS_MARK_LEAVE_CONNECTOR)
			{
				arg_bf->trigger_pts_output.push_back(static_cast<IjvxDataConnectionShortcut*>(this));
			}
		}

		// object_id = *arg_bf;
		arg_bf->component_id++;

		fwd_ingroup = true;
		break;

	case JVX_LINKDATA_TRANSFER_BROADCAST_THREADING_PARAMETERS_IN_GROUP:

		myThreadParameters = *myThreads;
		fwd_ingroup = true;
		break;

	// Alternatively implemented a shortcut!
	case JVX_LINKDATA_TRANSFER_COMPLETE_LEAVE_THREADS:

		if (my_tag == JVX_PROCESS_MARK_LEAVE_CONNECTOR)
		{
			if (jvx_bitTest(*trigger_bf, 0))
			{
				thread_vars.run_this_time = false;
				jvxErrorType resL = prepare_thread_processing();
				if (resL == JVX_NO_ERROR)
				{
					thread_vars.run_this_time = true;
				}
			}
			if (jvx_bitTest(*trigger_bf, 1))
			{
				this->run_thread_triggered();
			}
		}
		else
		{
			fwd_ingroup = true;
		}
		break;

	default:

		// All unknown messages should pass by
		fwd = true;
		break;

	// ==================================================================
	// Deprecated Deprecated Deprecated Deprecated Deprecated Deprecated
	// ==================================================================
#if 0
	case JVX_LINKDATA_TRANSFER_COUNT_OVERALL_BUFFERS_ADD_PIPELINE:
		JVX_ASSERT(0);
		if (dedicatedThread)
		{
			*arg_sz = JVX_MIN(eff_number_new_buffers, *arg_sz);
		}
		fwd_ingroup = true;
		break;
	case JVX_LINKDATA_TRANSFER_COUNT_AVAIL_BUFFER_MIN_CHAIN_IN_GROUP:
		JVX_ASSERT(0);
		if (dedicatedThread)
		{
			*arg_sz = JVX_MIN(numBufferList - numBufferList_current, *arg_sz);
		}
		fwd_ingroup = true;
		break;
	case JVX_LINKDATA_TRANSFER_PREPARE_CHAIN_PROCESS_PARALLEL_IN_GROUP:
		JVX_ASSERT(0);
		if (dedicatedThread)
		{
			if (JVX_EVALUATE_BITFIELD(arg_thread->thread_trigger & my_thread_bf))
			{
				prepare_chain_buffer(arg_thread->processing_step_id, arg_thread->chain_hdl);
			}
		}
		fwd_ingroup = true;
		break;
	case JVX_LINKDATA_TRANSFER_TRIGGER_CHAIN_PROCESS_PARALLEL_IN_GROUP:
		JVX_ASSERT(0);
		if (dedicatedThread)
		{
			if (JVX_EVALUATE_BITFIELD(arg_thread->thread_trigger & my_thread_bf))
			{
				// Do the prepare here since processing is linear
				if (myThreadParameters.myThreadMode == JVX_THREAD_PROC_GROUP_MODE_SEQ_CHAIN_THREADS)
				{
					prepare_chain_buffer(arg_thread->processing_step_id, arg_thread->chain_hdl);
				}

				if (JVX_CHECK_SIZE_SELECTED(thread_vars.proc_id))
				{
					if (myThreadParameters.myThreadMode == JVX_THREAD_PROC_GROUP_MODE_FULL_THREADS_COUPLED)
					{
						theThreadHandle->trigger_wakeup();
					}
					else if (myThreadParameters.myThreadMode == JVX_THREAD_PROC_GROUP_MODE_SEQ_CHAIN_THREADS)
					{
						run_processing_core();
					}
					else
					{
						assert(0);
					}
				}
				else
				{
#if 0
					// There is no buffer available for processing, report that processing is already over
					LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_THREAD_COMPLETE_ENTER,
						JVX_LOG_PROC_GROUPS_SUCCESS_BUFFER_NOT_READY, -1, -1, -1, thread_id);
					myGroupParent->report_thread_pipeline(
						reinterpret_cast<CjvxDataChainInterceptorGroup*>(arg_thread->chain_hdl),
						my_thread_bf, arg_thread->processing_step_id);
					LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_THREAD_COMPLETE_LEAVE,
						JVX_LOG_PROC_GROUPS_SUCCESS_BUFFER_NOT_READY, -1, -1, -1, thread_id);
#endif
				}
			}
		}

		// This chain run should not leave group
		fwd_ingroup = true;
		break;
#endif
	}

	// This chain run should not leave group
	if (my_tag != JVX_PROCESS_MARK_LEAVE_CONNECTOR)
	{
		if (fwd_ingroup)
		{
			fwd = true;
		}
	}

	if (fwd)
	{
		res = transfer_forward_ocon(tp, hdl JVX_CONNECTION_FEEDBACK_CALL_A(var));
	}

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_TRIGGER_FULL_THREADS_COUPLED:

		if (dedicatedThread)
		{
			if (trigger_thread)
			{
				theThreadHandle->trigger_wakeup();
			}
		}
		break;
	}

	return res;
}

// ========================================================================

jvxErrorType 
CjvxDataChainInterceptorGroup::startup(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataChainInterceptorGroup::expired(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataChainInterceptorGroup::wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = run_processing_core();
	return res;
}

jvxErrorType 
CjvxDataChainInterceptorGroup::stopped(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}

// ========================================================================

jvxErrorType
CjvxDataChainInterceptorGroup::run_processing_core()
{
	jvxBool report_complete = false;
	jvxBitField bf;
	// Run the queue element if there is a valid proc id
	if (JVX_CHECK_SIZE_SELECTED(thread_vars.proc_id))
	{
#ifdef JVX_IC_GROUP_USE_ATOMIC_SYNC
		jvxCBitField modBf = (jvxCBitField)1 << thread_vars.id_in_dedicated_list;
		jvxCBitField valBefore = 0;
#endif
		// Run thread
		run_thread_triggered();

#ifdef JVX_IC_GROUP_USE_ATOMIC_SYNC
		// Critical area to synchronize
		valBefore = JVX_INTERLOCKED_EXCHANGE_ADD_64((jvxInt64*)&thread_vars.conn_id->thread_id, modBf);
		valBefore |= modBf;

		if (valBefore == thread_vars.conn_id->thread_id_complete)
		{
			JVX_SET_NOTIFICATION(*thread_vars.conn_id->notify_complete);
		}
#else
		JVX_LOCK_MUTEX(*thread_vars.conn_id->synchronized_complete);
		jvx_bitSet(thread_vars.conn_id->thread_id, thread_vars.id_in_dedicated_list);
		bf = thread_vars.conn_id->thread_id;
		JVX_UNLOCK_MUTEX(*thread_vars.conn_id->synchronized_complete);
	
		if (bf == thread_vars.conn_id->thread_id_complete)
		{
			JVX_SET_NOTIFICATION(*thread_vars.conn_id->notify_complete);
		}
#endif

#if 0
		LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_THREAD_COMPLETE_ENTER,
			JVX_LOG_PROC_GROUPS_SUCCESS_OK, -1, -1, -1, thread_id);

		// Report that this part is complete
		myGroupParent->report_thread_pipeline(
			reinterpret_cast<CjvxDataChainInterceptorGroup*>(thread_vars.thread_entry),
			my_thread_bf, thread_vars.proc_id);

		LOG_BUF_WRITE(object_id.component_id, myChainId, JVX_LOG_PROC_GROUPS_OPERATION_THREAD_COMPLETE_LEAVE,
			JVX_LOG_PROC_GROUPS_SUCCESS_OK, -1, -1, -1, thread_id);

		// Clear the index values for processing
		thread_vars.proc_id = JVX_SIZE_UNSELECTED;
		thread_vars.idx_stage = JVX_SIZE_UNSELECTED;
#endif
	}
	return JVX_NO_ERROR;
}

void
CjvxDataChainInterceptorGroup::prepare_chain_buffer(jvxSize proc_id, jvxHandle* chain_hdl)
{
	jvxSize idxRead;
	jvxBool foundBuffer = false;
	jvxSize i;

	// Associate process to buffer
	JVX_LOCK_MUTEX(safeAccess);
	if (JVX_CHECK_SIZE_UNSELECTED(thread_vars.idx_stage))
	{
		for (i = 0; i < numBufferList_current; i++)
		{
			idxRead = (idxBufferList_read + i) % numBufferList;
			if (fldBufferList[idxRead].stat == JVX_CHAIN_BUFFER_AVAILABLE)
			{
				fldBufferList[idxRead].stat = JVX_CHAIN_BUFFER_READY;
				fldBufferList[idxRead].proc_id = proc_id;

				// Move all info on this process to local data and release buffer from list
				thread_vars.idx_stage = fldBufferList[idxRead].idxPipeline;
				thread_vars.thread_entry = chain_hdl;
				thread_vars.proc_id = proc_id;

				numBufferList_current--;
				idxBufferList_read = (idxBufferList_read + i) % numBufferList;

				break;
			}
		}
	}
	else
	{
		// Currently, a process is ongoing. We will not prepare processing but wait for longer
	}
	JVX_UNLOCK_MUTEX(safeAccess);
}

// ================================================================================================
jvxErrorType 
CjvxDataChainInterceptorGroup::st_callback_process_start_in_lock(jvxSize* pipe_idx, jvxHandle* priv_ptr)
{
	CjvxDataChainInterceptorGroup* this_pointer = reinterpret_cast<CjvxDataChainInterceptorGroup*>(priv_ptr);
	return this_pointer->ic_callback_process_start_in_lock(pipe_idx);
}
jvxErrorType 
CjvxDataChainInterceptorGroup::ic_callback_process_start_in_lock(jvxSize* pipe_idx)
{
	if (numBufferList_current > 0)
	{
		*pipe_idx = fldBufferList[idxBufferList_read].idxPipeline;
		idxBufferList_read = (idxBufferList_read + 1) % numBufferList;
		numBufferList_current--;

#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << "### Removing buffer index " << *pipe_idx << " to run stage, interceptor " << nm_ic << " -- " << __FUNCTION__ << std::endl;
#endif
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_NOT_READY;
}

// ================================================================================================

jvxErrorType
CjvxDataChainInterceptorGroup::st_callback_process_stop_in_lock(jvxSize pipe_idx, jvxHandle* priv_ptr)
{
	CjvxDataChainInterceptorGroup* this_pointer = reinterpret_cast<CjvxDataChainInterceptorGroup*>(priv_ptr);
	return this_pointer->ic_callback_process_stop_in_lock(pipe_idx);
}

jvxErrorType
CjvxDataChainInterceptorGroup::ic_callback_process_stop_in_lock(jvxSize pipe_idx)
{
	jvxSize idxWrite = 0;
	assert(numBufferList_current < numBufferList);
	idxWrite = (idxBufferList_read + numBufferList_current) % numBufferList;
	fldBufferList[idxWrite].idxPipeline = pipe_idx;
	fldBufferList[idxWrite].stat = JVX_CHAIN_BUFFER_AVAILABLE;
	numBufferList_current++;

#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
	std::cout << "### Added buffer " << pipe_idx << " to next stage, interceptor " << nm_ic << " -- " << __FUNCTION__ << std::endl;
#endif
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataChainInterceptorGroup::trigger_report_missed()
{
	return this->transfer_forward_ocon(JVX_LINKDATA_TRANSFER_TRIGGER_REPORT_SINGLE_MISSED_FRAME, NULL JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
}

jvxErrorType 
CjvxDataChainInterceptorGroup::trigger_run_out(jvxCBitField trigger_bf)
{
	if (jvx_bitTest(trigger_bf, 0))
	{
		thread_vars.run_this_time = false;
		if (numBufferList_current > 0)
		{
			jvxErrorType resL = prepare_thread_processing();
			assert(resL == JVX_NO_ERROR);
			thread_vars.run_this_time = true;
		}
	}
	if (jvx_bitTest(trigger_bf, 1))
	{
		// Direct execution, may also NT run if thread_vars.run_this_time is not true;
		this->run_thread_triggered();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataChainInterceptorGroup::trigger_prepare_dedicated()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_bitClear(thread_vars.conn_id->thread_id, my_thread_bf_position);

	if (numBufferList_current > 0)
	{
#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << "### Prepare parallel thread: " << numBufferList_current << " buffers available for processing, interceptor " <<
			nm_ic << " -- " << __FUNCTION__ << std::endl;
#endif
		res = prepare_thread_processing();
		assert(res == JVX_NO_ERROR);
		thread_vars.run_this_time = true;
	}
	else
	{
#ifdef JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
		std::cout << "### Prepare parallel thread: No buffer available for processing, interceptor " <<
			nm_ic << " -- " << __FUNCTION__ << std::endl;
#endif
		// Highlight this thread as "complete"
		thread_vars.run_this_time = false;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataChainInterceptorGroup::trigger_wakeup_dedicated()
{
	theThreadHandle->trigger_wakeup();
	return JVX_NO_ERROR;
}

void 
CjvxDataChainInterceptorGroup::description_bridge(jvxApiString* astr)
{
	if (astr)
	{
		astr->assign(this->nm_bridge);
	}
}
