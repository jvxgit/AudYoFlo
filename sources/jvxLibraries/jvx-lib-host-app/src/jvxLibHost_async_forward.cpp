#include "jvxLibHost.h"

jvxErrorType
jvxLibHost::request_command_fwd(const CjvxReportCommandRequest& request, jvxBool async_call)
{
	jvxBool mayEnter = true;

	// We need to distinguish between sync and async entry here: in case we enter within a sync
	// call, the lock is already held by the first entry to the host lib. If we
	// re-appear within a async call, the lock must be used to grant access
	if (async_call)
	{
		mayEnter = false;

		// The synchromnization is forced here. The force option is 
		// required only at very view places. One is due to the 
		// fact that the terminate function may arise in a different thread
		// due to the insufficiencies of, e.g., flutter 
		if (synchronize_thread_enter(true))
		{
			mayEnter = true;
		}
	}

	if (mayEnter)
	{
		jvxReportCommandDataType tp = request.datatype();
		const CjvxReportCommandRequest_rm* iface_rm = NULL;
		const CjvxReportCommandRequest_uid* iface_uid = NULL;
		jvxReportCommandRequest req = request.request();
		jvxReportCommandBroadcastType broadcastType = request.broadcast();
		jvxComponentIdentification cp;
		jvxCBitField prio = 0;
		IjvxObject* theObj = NULL;
		IjvxInterfaceFactory* theFac = NULL;
		jvxComponentIdentification cpId = request.origin();
		jvxSize schedId = JVX_SIZE_UNSELECTED;
		jvxErrorType resL = JVX_NO_ERROR;
		IjvxDataConnections* theDataConnections = NULL;
		IjvxDataConnectionProcess* theRef = NULL;
		IjvxConnectionMaster* theMas = NULL;
		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
		jvxBool testOk = false;

		// Rescheduled events have entered this function before and have already been processed.
		// Therefore they should not be forwarded to the automation system!
		// However, rescheduled requests are forwarded to sub-listener since the events have NOT been forwarded
		// when they passed by in "immediate" mode
		if (broadcastType != jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_RESCHEDULED)
		{
			switch (tp)
			{
			case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_BASE:

				// Functions handled from within the dart code:
				// 1) Sequencer Immediate Step!
				//	  jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE:


				/*
				 * These cases should be covered by the host itself

				switch (req)
				{
				case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST:
					cp = request->origin();
					if (subWidgets.bridgeMenuComponents)
					{
						subWidgets.bridgeMenuComponents->updateWindowSingle(cpId);
					}
					break;
				case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED:

					jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
					this->updateWindow(prio);
					break;
				}
				*/
				break;
			case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE:
				request.specialization(reinterpret_cast<const jvxHandle**>(&iface_rm), tp);
				if (iface_rm)
				{
					jvxHandle* userData = request.user_data();
					schedId = iface_rm->schedule_id();
					if (involvedHost.hHost)
					{
						involvedHost.hHost->request_object_selected_component(cpId, &theObj);
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
							}
							involvedHost.hHost->return_object_selected_component(cpId, theObj);
						}
					}
				}
				break;

			case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID:
				request.specialization(reinterpret_cast<const jvxHandle**>(&iface_uid), tp);
				switch (req)
				{
				case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN:

					if (involvedComponents.theHost.hFHost)
					{
						jvxErrorType res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));
						if ((res == JVX_NO_ERROR) && theDataConnections)
						{
							jvxSize uId = JVX_SIZE_UNSELECTED;
							iface_uid->uid(&uId);
							jvxApiString nmChain;
							JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

							theDataConnections->reference_connection_process_uid(uId, &theRef);
							if (theRef)
							{
								// Run test on chain, the result is stored internally and will be shown in next "updateWindow"
								jvxErrorType resL = theRef->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A_NULL);

								theRef->status_chain(&testOk JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
								if (!testOk)
								{
									theRef->descriptor_connection(&nmChain);
									this->report_simple_text_message(("Problem testing chain " + nmChain.std_str() + ", please review connection manager.").c_str(), JVX_REPORT_PRIORITY_ERROR);
								}
								theDataConnections->return_reference_connection_process(theRef);
							}
							else
							{
								std::cout << "Failed to find connection process with ID = " << uId << std::endl;
							}
							involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theDataConnections));
							theDataConnections = NULL;
						}
					}

					break;
				}
				break;

			default:
				break;
			}

			if (this->theHostFeatures.automation.if_report_automate)
			{
				jvxErrorType resL = JVX_NO_ERROR;
				jvxReportCommandBroadcastType broad = request.broadcast();
				switch (broad)
				{
				case jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION:
					resL = this->theHostFeatures.automation.if_report_automate->request_command(request);
					break;
				}
			}
		}
	}

	// Forward all async calls towards. This includes also the rescheduled immediate messages!! 
	if (async_call)
	{
		// We have to put this part into a critical section: new websockets can pop up at anytime since the 
		// web server "lifes" in his own thread
		JVX_LOCK_MUTEX(safeAccessSubReports);
		for (auto elm : subReports)
		{
			elm->request_command(request);
		}
		JVX_UNLOCK_MUTEX(safeAccessSubReports);
	}

	if (async_call)
	{
		if (mayEnter)
		{
			synchronize_thread_leave(true);
		}
	}

	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::request_test_chain(const CjvxReportCommandRequest_uid& req)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	IjvxDataConnections* theDataConnections = NULL;
	if (involvedComponents.theHost.hFHost)
	{
		res = JVX_NO_ERROR;
		theDataConnections = reqInterfaceObj<IjvxDataConnections>(involvedComponents.theHost.hFHost);
		if (theDataConnections)
		{
			jvxSize num = 0;
			jvxSize uId = JVX_SIZE_UNSELECTED;
			req.uid(&uId);

			res = theDataConnections->add_process_test(uId, &num, req.immediate());
			retInterfaceObj<IjvxDataConnections>(involvedComponents.theHost.hFHost, theDataConnections);
		}
	}
	return res;
}

