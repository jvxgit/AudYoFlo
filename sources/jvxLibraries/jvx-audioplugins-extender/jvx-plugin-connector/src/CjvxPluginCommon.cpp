#include "CjvxPluginCommon.h"

void
CjvxPluginCommon::prepare_controller_processor_message_proc_complete(
	jvxAudioPluginBinaryHeader* ptr, 
	jvxInt16 rep_process_id, jvxInt16 ass_process_id)
{
	ptr->command = JVX_AUDIOPLUGIN_REPORT_PROCESS_COMPLETE;
	ptr->rep_process_id = rep_process_id;
	ptr->ass_process_id = ass_process_id;
	ptr->sz = sizeof(jvxAudioPluginBinaryHeader);
}

void
CjvxPluginCommon::prepare_controller_processor_message_send_fld(
	jvxData** fld_ptr, jvxSize* fld_sz, jvxInt32 req_id,
	jvxInt32 sub_id, jvxUInt32 num, jvxUInt32 off,
	jvxAudioPluginBinaryLinFieldHeader** ptr,
	jvxInt16 rep_process_id, jvxInt16 ass_process_id)
{
	jvxByte* fld = nullptr;
	jvxSize szMessage = sizeof(jvxAudioPluginBinaryLinFieldHeader);
	jvxSize numVals = num - off;
	szMessage += numVals * sizeof(jvxData);
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, jvxByte, szMessage);
	*ptr = (jvxAudioPluginBinaryLinFieldHeader*)fld;
	*fld_ptr = (jvxData*)(fld + sizeof(jvxAudioPluginBinaryLinFieldHeader));
	*fld_sz = numVals;
	(*ptr)->hdr.command = JVX_AUDIOPLUGIN_SEND_LIN_FIELD;
	(*ptr)->hdr.sz = (jvxUInt32)szMessage;
	(*ptr)->hdr.rep_process_id = rep_process_id;
	(*ptr)->hdr.ass_process_id = ass_process_id;
	(*ptr)->numChar = 0;
	(*ptr)->id = req_id;
	(*ptr)->subid = sub_id;
	(*ptr)->num = num;
	(*ptr)->offset = off;
}

void 
CjvxPluginCommon::prepare_controller_processor_message_trigger_fld(
	jvxInt32 req_id, jvxInt32 sub_id, jvxUInt32 num, jvxUInt32 off,
	jvxAudioPluginBinaryLinFieldHeader** ptr,
	const char* tag,
	jvxInt16 rep_process_id, jvxInt16 ass_process_id)
{
	
	std::string origin = tag;
	jvxSize szMessage = origin.size();
	szMessage += sizeof(jvxAudioPluginBinaryLinFieldHeader);
	jvxByte* fld = nullptr;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, jvxByte, szMessage);
	*ptr = (jvxAudioPluginBinaryLinFieldHeader*)fld;
	jvxByte* ptrChar = (fld + sizeof(jvxAudioPluginBinaryLinFieldHeader));
	memcpy(ptrChar, origin.c_str(), origin.size());
	(*ptr)->hdr.command = JVX_AUDIOPLUGIN_TRIGGER_REQ_LIN_FIELD;
	(*ptr)->hdr.sz = (jvxUInt32)szMessage;
	(*ptr)->hdr.rep_process_id = rep_process_id;
	(*ptr)->hdr.ass_process_id = ass_process_id;
	(*ptr)->numChar = (jvxUInt16)origin.size();
	(*ptr)->id = req_id;
	(*ptr)->subid = sub_id;
	(*ptr)->num = num;
	(*ptr)->offset = off;
}

void
CjvxPluginCommon::prepare_controller_processor_message_trigger_sync(
	jvxSize num, jvxInt32* req_ids,
	jvxAudioPluginBinaryParamHeader** ptr,
	const char* tag,
	jvxInt16 rep_process_id, jvxInt16 ass_process_id)
{
	jvxSize i;
	std::string origin = tag;
	jvxSize szMessage = num * sizeof(jvxAudioPluginBinaryField_trigger);
	szMessage += origin.size();
	szMessage += sizeof(jvxAudioPluginBinaryParamHeader);
	jvxByte* fld = nullptr;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, jvxByte, szMessage);
	*ptr = (jvxAudioPluginBinaryParamHeader*)fld;
	jvxByte* ptrChar = (fld + sizeof(jvxAudioPluginBinaryParamHeader));
	memcpy(ptrChar, origin.c_str(), origin.size());
	jvxAudioPluginBinaryField_trigger* trigger = (jvxAudioPluginBinaryField_trigger*)(ptrChar + origin.size());
	(*ptr)->hdr.command = JVX_AUDIOPLUGIN_TRIGGER_SYNC_PARAM;
	(*ptr)->hdr.sz = (jvxUInt32)szMessage;
	(*ptr)->hdr.rep_process_id = rep_process_id;
	(*ptr)->hdr.ass_process_id = ass_process_id;
	(*ptr)->numFields = (jvxUInt16)num;
	(*ptr)->numChar = (jvxUInt16)origin.size();
	for (i = 0; i < (*ptr)->numFields; i++)
	{
		trigger[i].id = req_ids[i];
	}
}

void
CjvxPluginCommon::prepare_controller_processor_message_param_norm(
	jvxAudioPluginBinaryParamHeader** hdr,
	jvxInt32 id, jvxData normVal, jvxErrorType resAcc,
	const char* tag, jvxInt16 rep_process_id, jvxInt16 ass_process_id)
{
	std::string token;
	if (tag)
	{
		token = tag;
	}
	jvxSize szMess = sizeof(jvxAudioPluginBinaryParamHeader) + sizeof(jvxAudioPluginBinaryField_sync_norm);
	szMess += token.size();
	jvxByte* fld = nullptr;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, jvxByte, szMess);
	*hdr = (jvxAudioPluginBinaryParamHeader*)fld;
	(*hdr)->hdr.command = JVX_AUDIOPLUGIN_SEND_NORM_PARAM;
	(*hdr)->hdr.rep_process_id = rep_process_id;
	(*hdr)->hdr.ass_process_id = ass_process_id;
	(*hdr)->numChar = (jvxUInt16)token.size();
	(*hdr)->numFields = 1;
	(*hdr)->hdr.sz = (jvxUInt32)szMess;
	fld += sizeof(jvxAudioPluginBinaryParamHeader);
	if (token.size())
	{
		memcpy(fld, token.c_str(), token.size());
	}
	jvxAudioPluginBinaryField_sync_norm* fldN =
		(jvxAudioPluginBinaryField_sync_norm*)(fld + token.size());
	fldN->hdr.id = id;
	fldN->hdr.err_entry = (jvxUInt16)resAcc;
	fldN->hdr.type_entry = JVX_AUDIOPLUGIN_PARAM_NORMALIZED;
	fldN->val = normVal;
}

void 
CjvxPluginCommon::prepare_controller_processor_message_param_strlist(
	jvxAudioPluginBinaryParamHeader** hdr,
	jvxInt32 id, jvxData normVal,
	jvxErrorType resAcc,
	jvxApiStringList* strl,
	const char* tag, 
	jvxInt16 rep_process_id, jvxInt16 ass_process_id)
{
	jvxByte* fld = nullptr;
	jvxSize i;
	std::string token;
	if (tag)
	{
		token = tag;
	}
	jvxSize szMess = sizeof(jvxAudioPluginBinaryParamHeader) + sizeof(jvxAudioPluginBinaryField_sync_norm_strlist);
	szMess += token.size();
	for (i = 0; i < strl->ll(); i++)
	{
		std::string tt = strl->std_str_at(i);
		szMess += sizeof(jvxAudioPluginBinaryField_sync_norm_strlist_oneentry);
		szMess += tt.size();
	}
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, jvxByte, szMess);
	*hdr = (jvxAudioPluginBinaryParamHeader*)fld;
	(*hdr)->hdr.command = JVX_AUDIOPLUGIN_SEND_NORM_PARAM;
	(*hdr)->hdr.rep_process_id = rep_process_id;
	(*hdr)->hdr.ass_process_id = ass_process_id;
	(*hdr)->numChar = (jvxUInt16)token.size();
	(*hdr)->numFields = 1;
	(*hdr)->hdr.sz = (jvxUInt32)szMess;
	fld += sizeof(jvxAudioPluginBinaryParamHeader);
	if (token.size())
	{
		memcpy(fld, token.c_str(), token.size());
	}
	fld += token.size();
	jvxAudioPluginBinaryField_sync_norm_strlist* fldN =
		(jvxAudioPluginBinaryField_sync_norm_strlist*)fld;

	fldN->norm.hdr.id = id;
	fldN->norm.hdr.err_entry = (jvxUInt16)resAcc;
	fldN->norm.hdr.type_entry = JVX_AUDIOPLUGIN_PARAM_NORM_STRLIST;
	fldN->norm.val = normVal;
	fldN->num = strl->ll();
	fld += sizeof(jvxAudioPluginBinaryField_sync_norm_strlist);
	
	for(i = 0; i < fldN->num; i++)
	{
		jvxAudioPluginBinaryField_sync_norm_strlist_oneentry* fldE = (jvxAudioPluginBinaryField_sync_norm_strlist_oneentry*)fld;
		std::string tt = strl->std_str_at(i);
		char* ptrTxt = fld + sizeof(jvxAudioPluginBinaryField_sync_norm_strlist_oneentry);
		fldE->numchar = tt.size();
		memcpy(ptrTxt, tt.c_str(), tt.size());
		fld += sizeof(jvxAudioPluginBinaryField_sync_norm_strlist_oneentry);
		fld += tt.size();
	}
}

void
CjvxPluginCommon::release_controller_processor_message(jvxAudioPluginBinaryHeader* ptr)
{
	jvxByte* fld = (jvxByte*)ptr;
	JVX_DSP_SAFE_DELETE_FIELD(fld);
}

jvxAudioPluginBinaryField_sync_norm* 
CjvxPluginCommon::decode_norm_param(jvxAudioPluginBinaryField_sync_norm* ptrParam, jvxInt32& id, jvxData& normVal)
{
	normVal = ptrParam->val;
	id = ptrParam->hdr.id;
	ptrParam = (jvxAudioPluginBinaryField_sync_norm*)((jvxByte*)ptrParam + sizeof(jvxAudioPluginBinaryField_sync_norm));
	return ptrParam;
}

jvxAudioPluginBinaryField_sync_norm_strlist* 
CjvxPluginCommon::decode_norm_strlist_param(jvxAudioPluginBinaryField_sync_norm_strlist* ptrParam,
	jvxInt32& id, jvxData& normVal, jvxApiStringList* strl)
{
	jvxSize j;
	// What to with the string lst?
	normVal = ptrParam->norm.val;
	id = ptrParam->norm.hdr.id;
	jvxAudioPluginBinaryField_sync_norm_strlist_oneentry* entry =
		(jvxAudioPluginBinaryField_sync_norm_strlist_oneentry*)((jvxByte*)ptrParam + sizeof(jvxAudioPluginBinaryField_sync_norm_strlist));
	for (j = 0; j < ptrParam->num; j++)
	{
		std::string txt;
		char* ptr = (char*)entry + sizeof(jvxAudioPluginBinaryField_sync_norm_strlist_oneentry);
		txt.assign(ptr, entry->numchar);
		strl->add(txt);
		entry = (jvxAudioPluginBinaryField_sync_norm_strlist_oneentry*)
			((jvxByte*)entry + sizeof(jvxAudioPluginBinaryField_sync_norm_strlist_oneentry) + entry->numchar);
	}
	return (jvxAudioPluginBinaryField_sync_norm_strlist*)entry;
}