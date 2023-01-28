#include "jvxLibHost.h"

bool
jvxLibHost::thread_check_enter()
{
	JVX_THREAD_ID myThreadId = JVX_GET_CURRENT_THREAD_ID();

	if (myThreadId == mainThreadId)
	{
		return true;
	}
	// Hard constraint: check thread id of any access context!
	assert(0);

	return false;
}

void
jvxLibHost::thread_check_leave()
{
}

bool
jvxLibHost::synchronize_thread_enter(jvxBool forceSync)
{
	if (theHostFeatures.host_sync || forceSync)
	{
		JVX_LOCK_MUTEX(safeAccessEngineThreads);
		return true;
	}
	return thread_check_enter();
}

void 
jvxLibHost::synchronize_thread_leave(jvxBool forceSync)
{
	if (theHostFeatures.host_sync || forceSync)
	{
		JVX_UNLOCK_MUTEX(safeAccessEngineThreads);
		return;
	}
	return thread_check_leave();
}

void
jvxLibHost::transfer_command(const std::string& cmd, jvxApiString* returnStr, bool json_out)
{
	this->interpret_command(cmd.c_str(), returnStr, json_out);
}

#define JVX_DECODE_TYPE_STRING(type) \
	if (typeForm == #type) \
	{ \
		return (jvxSize)type ## _decode(typeSelect.c_str()); \
	}

jvxSize
jvxLibHost::translate_enum_string(const std::string& typeForm, const std::string& typeSelect)
{
	JVX_DECODE_TYPE_STRING(jvxDataFormat)
	JVX_DECODE_TYPE_STRING(jvxPropertyAccessType)	
	JVX_DECODE_TYPE_STRING(jvxPropertyCategoryType)
	JVX_DECODE_TYPE_STRING(jvxPropertyDecoderHintType)
	JVX_DECODE_TYPE_STRING(jvxPropertyContext)
	return JVX_SIZE_UNSELECTED;
}

jvxErrorType
jvxLibHost::lookup_component_class(const jvxComponentType& cpTp, jvxComponentTypeClass& cls)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (involvedHost.hHost)
	{
		res = involvedHost.hHost->component_class(cpTp, cls);
	}
	return res;
}