#include "CjvxRequestCommandsHandler.h"

CjvxRequestCommandsHandler::CjvxRequestCommandsHandler()
{

}

void 
CjvxRequestCommandsHandler::initialize_fwd_link(CjvxHandleRequestCommands_callbacks* ptr)
{
	fwd_request = ptr;
	mainThreadId = JVX_GET_CURRENT_THREAD_ID();
	
}

jvxErrorType
CjvxRequestCommandsHandler::request_command(const CjvxReportCommandRequest& request, jvxBool verbose)
{
	jvxErrorType res = JVX_NO_ERROR;
	CjvxReportCommandRequest* ptr = NULL;
	const CjvxReportCommandRequest_uid* ptrReq = nullptr;
	jvxReportCommandRequest reqTpMod = request.request();;

	if (verbose)
	{
		jvxErrorType resLock = JVX_NO_ERROR;
		std::ostream* out_stream = &std::cout;

		if (log_stream.hdl)
		{
			resLock = log_stream.hdl->start_lock();
			if (resLock == JVX_NO_ERROR)
			{
				out_stream = log_stream.hdl->log_str();

				// The function still may return a NULL-pointer
				if (!out_stream)
				{
					out_stream = &std::cout;
				}
			}
		}
		if (out_stream)
		{
			jvx::helper::debug_out_command_request(request, *out_stream, "");
		}
		if (resLock == JVX_NO_ERROR)
		{			
			if (log_stream.hdl)
			{
				log_stream.hdl->stop_lock();
			}
		}
	}

	JVX_THREAD_ID locThreadId = JVX_GET_CURRENT_THREAD_ID();

	if (request.immediate())
	{
		switch (request.request())
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE:

			if (locThreadId != mainThreadId)
			{
				if (fwd_request)
				{
					fwd_request->report_error(JVX_ERROR_THREADING_MISMATCH, request);
				}
			}
			else
			{
				// We need to check the thread here - we are not allowed to run this outside of the main thread!!
				if (fwd_request)
				{
					fwd_request->trigger_immediate_sequencerStep();
				}
			}
			break;
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN:

			// This function may be called from outside the main thread!
			// This prevents forwarding this request to the postpone thread
			reqTpMod = jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC;

			ptrReq = castCommandRequest<CjvxReportCommandRequest_uid>(request);
			if (ptrReq)
			{
				IjvxDataConnections* theDataConnections = NULL;
				if (hostRef)
				{
					theDataConnections = reqInterfaceObj<IjvxDataConnections>(hostRef);
					if (theDataConnections)
					{
						jvxSize num = 0;
						jvxSize uId = JVX_SIZE_UNSELECTED;
						ptrReq->uid(&uId);

						// Try to add the current test to the test trigger queue
						res = theDataConnections->add_process_test(uId, &num, request.immediate());
						if (res == JVX_ERROR_POSTPONE)
						{
							// In case we receive a POSTPONE, this indicates that we need to change thread
							reqTpMod = jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN_RUN;
						}
						else
						{
							if (num)
							{
								// If any test was actually tested, there might be a change
								// We will end up here only if 
								// 1) immediate and
								// 2) we ended here in main thread
								if (fwd_request)
								{
									fwd_request->run_mainthread_triggerTestChainDone();
								}
							}
						}
						retInterfaceObj<IjvxDataConnections>(hostRef, theDataConnections);
					}
				}
			}
		}

		if (automationReport)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			jvxReportCommandBroadcastType broad = request.broadcast();
			switch (broad)
			{
			case jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION:
				resL = automationReport->request_command(request);
				break;
			}
		}

		if (fwd_request)
		{
			fwd_request->run_immediate_rescheduleRequest(request);
		}
		return res;
	}
	else
	{
		switch (request.request())
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN:
			// Special rules for the test chain
			ptrReq = castCommandRequest<CjvxReportCommandRequest_uid>(request);
			if (ptrReq)
			{
				IjvxDataConnections* theDataConnections = NULL;
				if (hostRef)
				{
					theDataConnections = reqInterfaceObj<IjvxDataConnections>(hostRef);
					if (theDataConnections)
					{
						jvxSize num = 0;
						jvxSize uId = JVX_SIZE_UNSELECTED;
						ptrReq->uid(&uId);

						// Try to add the current test to the test trigger queue
						res = theDataConnections->add_process_test(uId, &num, request.immediate());
						if (res == JVX_ERROR_POSTPONE)
						{
							// In case we receive a POSTPONE, this indicates that we need to change thread
							reqTpMod = jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN_RUN;
						}
						else
						{
							if (num)
							{
								// If any test was actually tested, there might be a change
								// We will end up here only if 
								// 1) immediate and
								// 2) we ended here in main thread
								if (fwd_request)
								{
									fwd_request->run_mainthread_triggerTestChainDone();
								}
							}
						}
						retInterfaceObj<IjvxDataConnections>(hostRef, theDataConnections);
					}
				}
			}
			break;
		}

		if (reqTpMod != jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC)
		{
			ptr = jvx_command_request_copy_alloc(request);
			ptr->set_request(reqTpMod);
			if (fwd_request)
			{
				fwd_request->trigger_threadChange_forward(ptr);
			}
		}
	}

	return res;
}

void
CjvxRequestCommandsHandler::request_command_in_main_thread(CjvxReportCommandRequest* request, jvxBool removeAfterHandle)
{
	jvxReportCommandDataType tp = request->datatype();
	const CjvxReportCommandRequest_rm* iface_rm = NULL;
	const CjvxReportCommandRequest_id* iface_id = NULL;
	const CjvxReportCommandRequest_uid* ptrReq = nullptr;
	jvxReportCommandRequest req = request->request();
	jvxComponentIdentification cp;
	jvxCBitField prio = 0;
	IjvxObject* theObj = NULL;
	IjvxInterfaceFactory* theFac = NULL;
	jvxComponentIdentification cpId = request->origin();
	jvxSize schedId = JVX_SIZE_UNSELECTED;
	jvxErrorType resL = JVX_NO_ERROR;

	// RESCHEDULED requests have been handled before and will be forwarded only!!
	if (request->broadcast() != jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_RESCHEDULED)
	{
		switch (tp)
		{
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_BASE:
			switch (req)
			{
			case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST:
				if (fwd_request)
				{
					fwd_request->run_mainthread_updateComponentList(cpId);
				}
				break;
			case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_ALL_PROPERTIES:

				if (fwd_request)
				{
					fwd_request->run_mainthread_updateProperties(cpId);
				}
				break;
			case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED:

				if (fwd_request)
				{
					fwd_request->run_mainthread_updateSystemStatus();
				}
				break;
			}
			break;
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE:
			request->specialization(reinterpret_cast<const jvxHandle**>(&iface_rm), tp);
			if (iface_rm)
			{
				jvxHandle* userData = request->user_data();
				schedId = iface_rm->schedule_id();
				if (hostRef)
				{
					hostRef->request_object_selected_component(cpId, &theObj);
					if (theObj)
					{
						theObj->interface_factory(&theFac);
						if (theFac)
						{
							IjvxSchedule* theSchedule = NULL;
							theFac->request_hidden_interface(JVX_INTERFACE_SCHEDULE, reinterpret_cast<jvxHandle**>(&theSchedule));
							if (theSchedule)
							{
								theSchedule->schedule_main_loop(schedId, userData);
								theFac->return_hidden_interface(JVX_INTERFACE_SCHEDULE, reinterpret_cast<jvxHandle*>(theSchedule));
							}
							else
							{
								jvxApiString astr;
								theObj->description(&astr);
								std::cout << __FUNCTION__ << " : Request to re-schedule in main thread failed for object <" << astr.std_str() << ">, reason: JVX_INTERFACE_SCHEDULE not supported." << std::endl;
							}
						}
						hostRef->return_object_selected_component(cpId, theObj);
					}
				}
			}
			break;
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID:
			switch (req)
			{
			case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN_RUN:
				// Special rules for the test chain
				ptrReq = castCommandRequest<CjvxReportCommandRequest_uid>(*request);
				if (ptrReq)
				{
					IjvxDataConnections* theDataConnections = NULL;
					if (hostRef)
					{
						theDataConnections = reqInterfaceObj<IjvxDataConnections>(hostRef);
						if (theDataConnections)
						{
							jvxSize num = 0;
							jvxSize uId = JVX_SIZE_UNSELECTED;
							ptrReq->uid(&uId);

							// Try to add the current test to the test trigger queue
							resL = theDataConnections->trigger_process_test_all(&num); // we trigger the test "immediately" since it is already the postponed version
							if (num)
							{
								// If any test was actually tested, there might be a change
								// We will end up here only if 
								// 1) immediate and
								// 2) we ended here in main thread
								if (fwd_request)
								{
									fwd_request->run_mainthread_triggerTestChainDone();
								}
							}
							retInterfaceObj<IjvxDataConnections>(hostRef, theDataConnections);
						}
					}
				}
				break;
			}
			break;

		default:
			break;
		}

		if (this->automationReport)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			jvxReportCommandBroadcastType broad = request->broadcast();
			switch (broad)
			{
			case jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION:
				resL = automationReport->request_command(*request);
				break;
			}
		}
	}

	if (removeAfterHandle)
	{
		//delete request;
		jvx_command_request_copy_dealloc(request);
	}
}

void
CjvxRequestCommandsHandler::run_all_tests()
{
	if (hostRef)
	{
		jvxCBitField requestStart = 0;
		jvxSize numTested = 0;
		IjvxDataConnections* theDataConnections = reqInterfaceObj<IjvxDataConnections>(hostRef);
		if (theDataConnections)
		{
			theDataConnections->trigger_process_test_all(&numTested);
			retInterfaceObj<IjvxDataConnections>(hostRef, theDataConnections);
		}
		if (numTested)
		{
			if (fwd_request)
			{
				fwd_request->run_mainthread_triggerTestChainDone();
			}
		}
	}
}