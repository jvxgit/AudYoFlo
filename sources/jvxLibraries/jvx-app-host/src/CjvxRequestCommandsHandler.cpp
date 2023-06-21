#include "CjvxRequestCommandsHandler.h"

CjvxRequestCommandsHandler::CjvxRequestCommandsHandler()
{

}

jvxErrorType
CjvxRequestCommandsHandler::_request_command(const CjvxReportCommandRequest& request, jvxBool verbose)
{
	jvxErrorType res = JVX_NO_ERROR;
	CjvxReportCommandRequest* ptr = NULL;
	const CjvxReportCommandRequest_uid* ptrReq = nullptr;
	jvxReportCommandRequest reqTpMod = request.request();;

	if (verbose)
	{
		std::cout << "--Incoming command request --" << std::endl;
		if (request.immediate())
		{
			std::cout << "- immediate" << std::endl;
		}
		else
		{
			std::cout << "- not immediate" << std::endl;
		}
		std::cout << "- request = " << jvxReportCommandRequest_txt(request.request()) << std::endl;
		std::cout << "- type = " << jvxReportCommandDataType_txt(request.datatype()) << std::endl;
		std::cout << "- origin = " << jvxComponentIdentification_txt(request.origin()) << std::endl;
		std::cout << "- - " << jvxReportCommandDataType_txt(request.datatype()) << std::endl;

		switch (request.datatype())
		{
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_BASE:
			break;
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID:
			{
				auto ptrH = castCommandRequest<CjvxReportCommandRequest_uid>(request);
				if (ptrH)
				{
					jvxSize uId = 0;
					ptrH->uid(&uId);
					std::cout << "- - - uid = " << uId << std::endl;
				}
			}
			break;
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE:
			{
				auto ptrH = castCommandRequest<CjvxReportCommandRequest_rm>(request);
				if (ptrH)
				{
					jvxSize sId = ptrH->schedule_id();
					std::cout << "- - - sid = " << sId << std::endl;
				}
			}
			break;
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SEQ:
		{
			auto ptrH = castCommandRequest<CjvxReportCommandRequest_seq>(request);
			if (ptrH)
			{
				TjvxSequencerEvent ev;	
				ptrH->seq_event(&ev);
				std::cout << "- - - ev = " << jvxSequencerEventType_txt(ev.tp) << std::endl;
			}
		}
		break;
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS:
		{
			auto ptrH = castCommandRequest<CjvxReportCommandRequest_ss>(request);
			if (ptrH)
			{

				jvxStateSwitch ss;
				ptrH->sswitch(&ss);
				std::cout << "- - - ss = " << jvxStateSwitch_txt(ss) << std::endl;
			}
		}
		break;
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT:
		{
			auto ptrH = castCommandRequest<CjvxReportCommandRequest_id>(request);
			if (ptrH)
			{

				jvxApiString id;
				ptrH->ident(&id);
				std::cout << "- - - id = " << id.std_str()  << std::endl;
			}
		}
		break;
		}

		std::cout << "---------------------------" << std::endl << std::endl;
	}
	if (request.immediate())
	{
		switch (request.request())
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE:

			// We need to check the thread here - we are not allowed to run this outside of the main thread!!
			if (reportRef)
			{
				reportRef->trigger_immediate_sequencerStep();				
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
								if (reportRef)
								{
									reportRef->run_mainthread_triggerTestChainDone();
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
								if (reportRef)
								{
									reportRef->run_mainthread_triggerTestChainDone();
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
			if (reportRef)
			{
				reportRef->trigger_threadChange_forward(ptr);
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

	switch (tp)
	{
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_BASE:
		switch (req)
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST:
			if (reportRef)
			{
				reportRef->run_mainthread_updateComponentList(cpId);
			}
			break;
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_ALL_PROPERTIES:

			if (reportRef)
			{
				reportRef->run_mainthread_updateProperties(cpId);
			}
			break;
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED:

			if (reportRef)
			{
				reportRef->run_mainthread_updateSystemStatus();
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
							if (reportRef)
							{
								reportRef->run_mainthread_triggerTestChainDone();
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

	if (removeAfterHandle)
	{
		//delete request;
		jvx_command_request_copy_dealloc(request);
	}
}

void
CjvxRequestCommandsHandler::_run_all_tests()
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
			if (reportRef)
			{
				reportRef->run_mainthread_triggerTestChainDone();
			}
		}
	}
}