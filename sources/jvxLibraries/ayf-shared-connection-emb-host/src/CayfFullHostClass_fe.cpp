#include "CayfFullHostClass.h"

jvxErrorType
CayfFullHostClass::report_register_fe_commandline(IjvxCommandLine* comLine) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CayfFullHostClass::report_readout_fe_commandline(IjvxCommandLine* comLine) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CayfFullHostClass::report_process_event(TjvxEventLoopElement* theQueueElement) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CayfFullHostClass::report_cancel_event(TjvxEventLoopElement* theQueueElement) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CayfFullHostClass::report_assign_output(TjvxEventLoopElement* theQueueElement, jvxErrorType sucOperation, jvxHandle* priv) 
{
	if (theQueueElement->origin_fe == static_cast<IjvxEventLoop_frontend*>(this))
	{
		assert(theQueueElement->paramType == JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS);
		CjvxJsonElementList* jlst = (CjvxJsonElementList*)theQueueElement->param;
		CjvxJsonElementList cLst;		

		std::string tmp;
		std::vector<std::string> errs;
		jlst->printString(tmp, JVX_JSON_PRINT_JSON, 0, "", "", "", false);
		/*
		if (CjvxJsonElementList::stringToRepresentation(tmp, cLst, errs) == JVX_NO_ERROR)
		{
			CjvxJsonElement addCtc;
			addCtc.makeAssignmentString("call_context", myPrivateMem->ctx_mthread);
			cLst.addConsumeElement(addCtc);
				jlst = &cLst;
			}
		}
		myPrivateMem->ret_mthread.clear();		
		jlst->printString(myPrivateMem->ret_mthread, JVX_JSON_PRINT_JSON, 0, "", "", "", false);		
		myPrivateMem->res_mthread = sucOperation;
		*/
		tmpOutputOnCommand = tmp;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CayfFullHostClass::report_special_event(TjvxEventLoopElement* theQueueElement, jvxHandle* priv) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CayfFullHostClass::report_want_to_shutdown_ext(jvxBool restart) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CayfFullHostClass::query_property(jvxFrontendSupportQueryType tp, jvxHandle* load) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CayfFullHostClass::trigger_sync(jvxFrontendTriggerType tp, jvxHandle* load, jvxBool blockedRun) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CayfFullHostClass::request_if_command_forward(IjvxReportSystemForward** fwdCalls) 
{
	return JVX_NO_ERROR;
}



jvxErrorType
CayfFullHostClass::add_reference_event_loop(IjvxEventLoopObject* eventLoop) 
{
	assert(evLoop == nullptr);
	JVX_LOCK_MUTEX(safeAccess);
	evLoop = eventLoop;	
	JVX_UNLOCK_MUTEX(safeAccess);

	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::clear_reference_event_loop(IjvxEventLoopObject* eventLoop) 
{
	assert(evLoop);
	JVX_LOCK_MUTEX(safeAccess);
	evLoop = nullptr;
	JVX_UNLOCK_MUTEX(safeAccess);
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::set_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend) 
{
	assert(priBeHandle == nullptr);
	JVX_LOCK_MUTEX(safeAccess);
	priBeHandle = theBackend;
	JVX_UNLOCK_MUTEX(safeAccess);
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::clear_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend) 
{
	assert(priBeHandle);
	JVX_LOCK_MUTEX(safeAccess);
	theBackend = nullptr;
	JVX_UNLOCK_MUTEX(safeAccess);
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::add_sec_reference_event_backend(IjvxEventLoop_backend* theBackend) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::clear_sec_reference_event_backend(IjvxEventLoop_backend* theBackend) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::returns_from_start(jvxBool* doesReturn) 
{
	if(doesReturn)
	{
		*doesReturn = true;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::start(int argc, char* argv[]) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::stop() 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::wants_restart(jvxBool* wantsRestart) 
{
	if(wantsRestart)
	{
		*wantsRestart = false;
	}
	return JVX_NO_ERROR;
}

