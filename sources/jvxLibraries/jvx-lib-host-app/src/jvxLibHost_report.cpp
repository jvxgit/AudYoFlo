#include "jvxLibHost.h"

// =============================================================================
// IjvxReport IjvxReport IjvxReport IjvxReport IjvxReport IjvxReport IjvxReport
// =============================================================================
jvxErrorType
jvxLibHost::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;

	// Make a copy of the Text field and transfer via callback
	if(cbks_api.async_report_simple_text_message)
	{ 
		res = cbks_api.async_report_simple_text_message(txt, prio);
	}
	return res;
}

jvxErrorType
jvxLibHost::report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offset_start, jvxSize numElements,
	const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose )
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;

	// Make a copy of the Text field and transfer via callback
	if (cbks_api.sync_report_internals_have_changed)
	{
		if (oConfig.reportSyncDirect)
		{
			res = cbks_api.sync_report_internals_have_changed(
				thisismytype,
				thisisme,
				cat, propId, onlyContent, offset_start, numElements,
				tpTo, callpurpose, this);
		}
		else
		{
			res = cbks_api.sync_report_internals_have_changed(
				thisismytype,
				thisisme,
				cat, propId, onlyContent, offset_start, numElements,
				tpTo, callpurpose, nullptr);
		}
	}
	return res;
}

jvxErrorType
jvxLibHost::report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
	jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo,
	jvxPropertyCallPurpose callpurpose )
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
jvxLibHost::report_command_request(jvxCBitField request,
	jvxHandle* caseSpecificData,
	jvxSize szSpecificData)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;

	// The test_chain function is directly moved to the new request_command principle 
	// since it is much better :-)
	if (jvx_bitTest(request, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT))
	{
		jvxSize uid = (intptr_t)caseSpecificData;
		CjvxReportCommandRequest_uid new_command(
			jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN,
			JVX_COMPONENT_UNKNOWN, uid);
		res = this->request_command(new_command);
		jvx_bitClear(request, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT);
	}

	if (jvx_bitTest(request, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT))
	{
		CjvxReportCommandRequest new_command(
			jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTIES,
			JVX_COMPONENT_UNKNOWN);
		res = this->request_command(new_command);
		jvx_bitClear(request, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	}

	if (request)
	{
		// Make a copy of the Text field and transfer via callback
		if (cbks_api.async_report_command_request)
		{
			res = cbks_api.async_report_command_request(
				request,
				caseSpecificData,
				szSpecificData);
		}
	}
	return res;	
}


jvxErrorType
jvxLibHost::report_os_specific(jvxSize commandId, void* context)
{
	return JVX_ERROR_UNSUPPORTED;
}

// ========================================================================
jvxErrorType 
jvxLibHost::request_command(const CjvxReportCommandRequest& request)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;

	// Make a copy of the Text field and transfer via callback
	if (cbks_api.request_command)
	{
		if (oConfig.reportSyncDirect)
		{
			res = cbks_api.request_command(request, false, this);
		}
		else
		{
			res = cbks_api.request_command(request, false, nullptr);
		}
	}

	// sub reports are in a mutex since they may also deal with commands outside the main thread
	/*
	JVX_LOCK_MUTEX(safeAccessSubReports);
	for (auto& elm : subReports)
	{
		if (elm)
		{
			// Forward the request
			elm->request_command(request);
		}
	}
	JVX_UNLOCK_MUTEX(safeAccessSubReports);
	*/

	return res;
}

//virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;
jvxErrorType
jvxLibHost::interface_sub_report(IjvxSubReport** subReport)
{
	if (subReport)
	{
		*subReport = static_cast<IjvxSubReport*>(this);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

//JVX_MUTEX_HANDLE safeAccessSubReports;
//std::list< IjvxSubReport*> subReports;

jvxErrorType
jvxLibHost::register_sub_report(IjvxReportSystem* request)
{
	jvxErrorType res = JVX_ERROR_ALREADY_IN_USE;
	JVX_LOCK_MUTEX(safeAccessSubReports);
	auto fIt = std::find(subReports.begin(), subReports.end(), request);
	if (fIt == subReports.end())
	{
		subReports.push_back(request);
		res = JVX_NO_ERROR;
	}
	JVX_UNLOCK_MUTEX(safeAccessSubReports);
	return res;
}

jvxErrorType 
jvxLibHost::unregister_sub_report(IjvxReportSystem* request)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	JVX_LOCK_MUTEX(safeAccessSubReports);
	auto fIt = std::find(subReports.begin(), subReports.end(), request);
	if (fIt != subReports.end())
	{
		subReports.erase(fIt);
		res = JVX_NO_ERROR;
	}
	JVX_UNLOCK_MUTEX(safeAccessSubReports);
	return res;
}

