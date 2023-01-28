#include "flutter_native_local.h"

int ffi_host_request_command_fwd(void* opaque_hdl, void* request_command, int async_call)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	jvxLibHost* ll = nullptr;
	if (opaque_hdl)
	{
		ll = (jvxLibHost*)opaque_hdl;
		CjvxReportCommandRequest* ptr = (CjvxReportCommandRequest*)request_command;
		res = ll->request_command_fwd(*ptr, (async_call != 0));
	}
	return res;
}

extern jvxErrorType request_command(const CjvxReportCommandRequest& request, jvxBool forceAsync, jvxLibHost* backRef);

int ffi_host_request_command_reschedule(void* request)
{
	const CjvxReportCommandRequest* ptr = (const CjvxReportCommandRequest*)request;
	if (ptr->immediate())
	{
		CjvxReportCommandRequest* copy = jvx_command_request_copy_alloc(*ptr);
		copy->modify_broadcast(jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_RESCHEDULED);
		return request_command(*copy, true, nullptr);
		jvx_command_request_copy_dealloc(copy);
	}
	return JVX_ERROR_INVALID_SETTING;
}

    