#include "jvxLibHost.h"

jvxErrorType
jvxLibHost::process_access(jvxSize uId, jvxSize* catId, jvxApiString* astr, IjvxConnectionIterator** itRet)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	IjvxDataConnectionProcess* proc = nullptr;
	IjvxConnectionIterator* it = nullptr;
	if (synchronize_thread_enter())
	{
		IjvxDataConnections* datConnPtr = nullptr;
		if (involvedHost.hHost)
		{
			datConnPtr = reqInterface<IjvxDataConnections>(involvedHost.hHost);
			if (datConnPtr)
			{
				res = datConnPtr->reference_connection_process_uid(uId, &proc);
				if (proc)
				{
					proc->category_id(catId);
					proc->iterator_chain(itRet);
					proc->description(astr);
					datConnPtr->return_reference_connection_process(proc);
				}
				proc = nullptr;
				retInterface<IjvxDataConnections>(involvedHost.hHost, datConnPtr);
			}
		}
		synchronize_thread_leave();
	}
	return res;
}

jvxErrorType
jvxLibHost::process_decode_iterator(IjvxConnectionIterator* it, 
	jvxComponentIdentification* retCp, jvxSize* numBranch, jvxApiString* modName, jvxApiString* description, jvxApiString* lContext)
{
	it->reference_component(retCp, modName, description, lContext);
	if (numBranch)
	{
		it->number_next(numBranch);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::process_next_iterator(IjvxConnectionIterator* it, IjvxConnectionIterator** next, jvxSize id)
{
	return it->reference_next(id, next);
}


jvxErrorType 
jvxLibHost::process_system_ready(jvxApiString* astr)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	IjvxDataConnectionProcess* proc = nullptr;
	IjvxConnectionIterator* it = nullptr;
	if (synchronize_thread_enter())
	{
		IjvxDataConnections* datConnPtr = nullptr;
		if (involvedHost.hHost)
		{
			datConnPtr = reqInterface<IjvxDataConnections>(involvedHost.hHost);
			if (datConnPtr)
			{
				res = datConnPtr->ready_for_start(astr);
				retInterface<IjvxDataConnections>(involvedHost.hHost, datConnPtr);
			}
		}
		synchronize_thread_leave();
	}
	return res;
}