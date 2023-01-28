#ifndef __CJVXPLUGINCOMMON_H__
#define __CJVXPLUGINCOMMON_H__

#include "jvx.h"
#include "TjvxPluginCommon.h"

class CjvxPluginCommon
{
public:

	static void prepare_controller_processor_message_proc_complete(
			jvxAudioPluginBinaryHeader* ptr,
		jvxInt16 rep_process_id, jvxInt16 ass_process_id);

	static void prepare_controller_processor_message_trigger_sync(
		jvxSize num, jvxInt32* req_ids,
		jvxAudioPluginBinaryParamHeader** ptr,
		const char* tag, 
		jvxInt16 rep_process_id, jvxInt16 ass_process_id);

	static void prepare_controller_processor_message_trigger_fld(
		jvxInt32 req_id, jvxInt32 sub_id, jvxUInt32 num, jvxUInt32 off,
		jvxAudioPluginBinaryLinFieldHeader** ptr,
		const char* tag,
		jvxInt16 rep_process_id, jvxInt16 ass_process_id);

	static void prepare_controller_processor_message_send_fld(
		jvxData** fld_ptr, jvxSize* fld_sz, jvxInt32 req_id,
		jvxInt32 sub_id, jvxUInt32 num, jvxUInt32 off,
		jvxAudioPluginBinaryLinFieldHeader** ptr,
		jvxInt16 rep_process_id, jvxInt16 ass_process_id);

	static void prepare_controller_processor_message_param_norm(
		jvxAudioPluginBinaryParamHeader** hdr,
		jvxInt32 id, jvxData normVal,
		jvxErrorType resAcc,
		const char* tag,
		jvxInt16 rep_process_id, jvxInt16 ass_process_id);

	static void prepare_controller_processor_message_param_strlist(
		jvxAudioPluginBinaryParamHeader** hdr,
		jvxInt32 id, jvxData normVal,
		jvxErrorType resAcc,
		jvxApiStringList* strl,
		const char* tag,
		jvxInt16 rep_process_id, jvxInt16 ass_process_id);

	static void release_controller_processor_message(jvxAudioPluginBinaryHeader* ptr);

	static jvxAudioPluginBinaryField_sync_norm* decode_norm_param(jvxAudioPluginBinaryField_sync_norm* ptrParam, jvxInt32& id, jvxData& normVal);

	static jvxAudioPluginBinaryField_sync_norm_strlist* decode_norm_strlist_param(jvxAudioPluginBinaryField_sync_norm_strlist* ptrParam,
		jvxInt32& id, jvxData& normVal, jvxApiStringList* strl);
};

#endif
