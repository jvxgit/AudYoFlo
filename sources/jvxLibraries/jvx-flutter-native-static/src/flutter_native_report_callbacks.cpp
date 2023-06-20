#include "flutter_native_local.h"

// =========================================================================
// =========================================================================

jvxErrorType async_report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	std::string txtStr = txt;
	char* fldChar = nullptr;
	report_callback* cbk = nullptr;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(cbk, struct report_callback);
	cbk->callback_id = JVX_FFI_CALLBACK_ASYNC_REPORT_TEXT;
	cbk->callback_subid = prio;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fldChar, char, (txtStr.size() + 1));
	memcpy(fldChar, txtStr.c_str(), txtStr.size());
	cbk->load_fld = fldChar;
	cbk->sz_fld = txtStr.size() + 1;
	bool resultSend = send_async_dart(cbk);
	if (resultSend == false)
	{
		goto exit_fail;
	}
	return JVX_NO_ERROR;
exit_fail:
	JVX_DSP_SAFE_DELETE_FIELD(fldChar);
	JVX_DSP_SAFE_DELETE_OBJECT(cbk);
	return JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
}

jvxErrorType sync_report_internals_have_changed(
	const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offset_start, jvxSize numElements,
	const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose, jvxLibHost* backref)
{
	report_internal_changed rep_change;
	report_callback cbk;
	cbk.callback_id = JVX_FFI_CALLBACK_SYNC_REPORT_INTERNALS_CHANGED;
	cbk.callback_subid = 0; // not used

	rep_change.from.tp = thisismytype.tp;
	rep_change.from.slotid = thisismytype.slotid;
	rep_change.from.slotsubid = thisismytype.slotsubid;
	rep_change.from.uId = thisismytype.uId;

	rep_change.to.tp = tpTo.tp;
	rep_change.to.slotid = tpTo.slotid;
	rep_change.to.slotsubid = tpTo.slotsubid;
	rep_change.to.uId = tpTo.uId;

	rep_change.callpurpose = callpurpose;
	rep_change.cat = cat;
	rep_change.numElements = numElements;
	rep_change.offset_start = offset_start;
	rep_change.onlyContent = onlyContent;
	rep_change.propId = propId;
	
	if (backref == nullptr)
	{
		cbk.load_fld = &rep_change;
		cbk.sz_fld = sizeof(report_internal_changed);
		bool resultSend = send_sync_dart(&cbk); // <- sync call, hence, no dynamic allocation is required!
		if (resultSend == false)
		{
			goto exit_fail;
		}
	}
	else
	{
		// This message is just skipped..
	}
	return JVX_NO_ERROR;
exit_fail:
	return JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
}

jvxErrorType async_report_command_request(
	jvxCBitField request,
	jvxHandle* caseSpecificData,
	jvxSize szSpecificData)
{
	report_callback* cbk = nullptr;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(cbk, struct report_callback);
	cbk->callback_id = JVX_FFI_CALLBACK_ASYNC_REPORT_COMMAND_REQUEST;
	cbk->callback_subid = (int)request;
	cbk->load_fld = caseSpecificData;
	cbk->sz_fld = szSpecificData;
	bool resultSend = send_async_dart(cbk);
	if (resultSend == false)
	{
		goto exit_fail;
	}
	return JVX_NO_ERROR;
exit_fail:
	JVX_DSP_SAFE_DELETE_OBJECT(cbk);
	return JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
}

jvxErrorType request_command(const CjvxReportCommandRequest& request, jvxBool forceAsync, jvxLibHost* backRef)
{
	jvxErrorType res = JVX_NO_ERROR;
	bool immediatecall = request.immediate();
	if (forceAsync)
	{
		immediatecall = false;
	}

	/*
	if (request.request() == jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN)
	{
		const CjvxReportCommandRequest_uid* ptrReq = castCommandRequest<CjvxReportCommandRequest_uid>(request);
		if (ptrReq) 
		{
			res = backRef->request_test_chain(*ptrReq);
			if (res == JVX_ERROR_POSTPONE)
			{
				CjvxReportCommandRequest* ptr = jvx_command_request_copy_alloc(request);				
				ptr->set_immediate(false);
				// res = request_command(*ptr, true, backRef);
				jvx_command_request_copy_dealloc(ptr);
			}
		}
		return res;
	}
	*/

	jvxReportCommandRequest req = request.request();
	if (immediatecall)
	{	
		if (backRef)
		{
			// forward the immediate requests to the automation system
			jvxErrorType res = backRef->request_command_fwd(request, false);

			// From here on check which requests must be rescheduled in a different way
			jvxReportCommandRequest req = request.request();

			// The following are tasks that must be run from within the
			// UI host and within the main thread!

			// Use this function here to synchonize to main thread but enter the scheduler in a specific function
			if (req == jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE)
			{
				// Here, we need to run the sequener step IMMEDIATELY!
				res = backRef->sequencer_trigger();
			}

			//natLib.ffi_comm

			if (res == JVX_NO_ERROR) {

				// Sync calls must be re-scheduled to enter the UI to keep everything in order
				if (request.immediate())
				{
					// we must highlight these events as rescheduled to filter those events out instead of forwarding the
					// requests towards automation
					CjvxReportCommandRequest* ptr = jvx_command_request_copy_alloc(request);
					// assert(0); // need to fix this
					ptr->modify_broadcast(jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_RESCHEDULED);
					res = request_command(*ptr, true, backRef);
					jvx_command_request_copy_dealloc(ptr);
					return res;
				}
			}
		}
		else
		{
			report_callback cbk;
			cbk.callback_id = JVX_FFI_CALLBACK_REQUEST_COMMAND;
			cbk.callback_subid = (int)req;
			cbk.load_fld = (void*)&request;
			cbk.sz_fld = sizeof(request);

			// This code to report via dart/ffi
			return send_sync_dart(&cbk);
		}
		return res;
	}

	// We end up here in case of a non-immediate call
	report_callback* cbk = nullptr;
	CjvxReportCommandRequest* ptr = nullptr;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(cbk, struct report_callback);
	cbk->callback_id = JVX_FFI_CALLBACK_REQUEST_COMMAND;
	cbk->callback_subid = (int)req;

	// We allocate a new request and pass it to dart. It must be deallocated by the dart code since we loose control at this point
	ptr = jvx_command_request_copy_alloc(request);
	/*
	switch (request.datatype())
	{
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ptr, CjvxReportCommandRequest_rm(request));
		break;
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ptr, CjvxReportCommandRequest_id(request));
		break;
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ptr, CjvxReportCommandRequest_uid(request));
		break;
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ptr, CjvxReportCommandRequest_ss(request));
		break;
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SEQ:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ptr, CjvxReportCommandRequest_seq(request));
		break;
	default:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ptr, CjvxReportCommandRequest(request));
		break;
	}
	*/
	cbk->load_fld = ptr;
	cbk->sz_fld = sizeof(request);
	bool resultSend = send_async_dart(cbk);
	if (resultSend == false)
	{
		JVX_DSP_SAFE_DELETE_OBJECT(cbk);
		JVX_DSP_SAFE_DELETE_OBJECT(ptr);
		return JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
	}
	return JVX_NO_ERROR;
}

void assign_report_functions(callbacks_capi* capi)
{
	capi->async_report_simple_text_message = async_report_simple_text_message;
	capi->sync_report_internals_have_changed = sync_report_internals_have_changed;
	capi->async_report_command_request = async_report_command_request;
	capi->request_command = request_command;
}
