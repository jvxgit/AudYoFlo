#include "CjvxVstPluginController.h"
#include "CjvxVstPluginParameter.h"

namespace Steinberg {
namespace Vst {

CjvxPluginVstController::CjvxPluginVstController() 
{
}

jvxErrorType 
CjvxPluginVstController::handle_message_from_processor(IMessage* message)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (!theController)
	{
		return JVX_ERROR_NOT_READY;
	}
	// This function is the lower level method
	if (!strcmp(message->getMessageID(), "BinaryMessage"))
	{
		const void* data = nullptr;
		uint32 size = 0;
		if (message->getAttributes()->getBinary("jvxrt-audioplugin", data, size) == kResultOk)
		{
			jvxSize i;
			jvxData normVal;
			jvxData* fld_ptr = nullptr;
			jvxSize fld_sz = 0;
			jvxApiStringList strl;
			jvxErrorType resL = JVX_NO_ERROR;
			jvxInt32 id = (jvxInt32)JVX_SIZE_UNSELECTED;
			jvxAudioPluginBinaryLinFieldHeader* lfhdr = (jvxAudioPluginBinaryLinFieldHeader*)data;
			jvxAudioPluginBinaryStateChangeHeader* shdr = (jvxAudioPluginBinaryStateChangeHeader*)data;
			jvxAudioPluginBinaryParamHeader* phdr = (jvxAudioPluginBinaryParamHeader*)data;
			jvxAudioPluginBinaryField_sync_norm* ptrParam = (jvxAudioPluginBinaryField_sync_norm*)((jvxByte*)data + sizeof(jvxAudioPluginBinaryParamHeader));
			jvxAudioPluginBinaryField_sync_norm_strlist* ptrParamStrLst = NULL;

			assert(size == phdr->hdr.sz);
			switch (phdr->hdr.command)
			{
			case JVX_AUDIOPLUGIN_SEND_LIN_FIELD:

				fld_ptr = (jvxData*)((jvxByte*)lfhdr + sizeof(jvxAudioPluginBinaryLinFieldHeader));
				fld_sz = (lfhdr->hdr.sz - sizeof(jvxAudioPluginBinaryLinFieldHeader))/sizeof(jvxData);
				process_linear_field(fld_ptr, fld_sz, lfhdr->id, lfhdr->subid, lfhdr->num, lfhdr->offset);

				break;
			case JVX_AUDIOPLUGIN_SEND_NORM_PARAM:
				for (i = 0; i < phdr->numFields; i++)
				{
					switch (ptrParam->hdr.type_entry)
					{
					case JVX_AUDIOPLUGIN_PARAM_NORMALIZED:
						resL = (jvxErrorType)ptrParam->hdr.err_entry;
						ptrParam = CjvxPluginCommon::decode_norm_param(ptrParam, id, normVal);
						if (resL == JVX_NO_ERROR)
						{
							theController->setParamNormalized(id, normVal);

						}
						break;
					case JVX_AUDIOPLUGIN_PARAM_NORM_STRLIST:
						ptrParamStrLst = (jvxAudioPluginBinaryField_sync_norm_strlist*)ptrParam;
						resL = (jvxErrorType)ptrParam->hdr.err_entry;
						ptrParam = (jvxAudioPluginBinaryField_sync_norm*)
							CjvxPluginCommon::decode_norm_strlist_param(ptrParamStrLst, id, normVal, &strl);

						// Modify the string list
						if (resL == JVX_NO_ERROR)
						{
							Parameter* p = theController->getParameterObject(id);
							if (p)
							{
								jvxVstStringListParameter* sp = dynamic_cast<jvxVstStringListParameter*>(p);
								if (sp)
								{
									sp->clear();
									for (i = 0; i < strl.ll(); i++)
									{
										sp->appendString(USTRING(strl.std_str_at(i).c_str()));
									}

									theController->setParamNormalized(id, normVal);
								}
								else
								{
									process_unmatched_strlist(id, normVal, &strl);
								}
							}
							else
							{
								// Here, a non-parameter value has been transferred
								process_unmatched_parameter(id, normVal, &strl);
							}
						}
						break;
					}
				}
				if (phdr->hdr.rep_process_id > 0)
				{
					// Send a response where required
					jvxAudioPluginBinaryHeader theRequest;
					CjvxPluginCommon::prepare_controller_processor_message_proc_complete(
					&theRequest, -1, phdr->hdr.rep_process_id);
					this->plugin_send_message((jvxAudioPluginBinaryHeader*)&theRequest);
				}
				break;
			case JVX_AUDIOPLUGIN_REPORT_STATE_CHANGED:
			case JVX_AUDIOPLUGIN_REPORT_CURRENT_STATE:

				process_state_update((jvxAudioPluginBinaryStateChange)shdr->stateId);
				if (phdr->hdr.rep_process_id > 0)
				{
					// Send a response where required
					jvxAudioPluginBinaryHeader theRequest;
					CjvxPluginCommon::prepare_controller_processor_message_proc_complete(
						&theRequest, -1, phdr->hdr.rep_process_id);
					this->plugin_send_message((jvxAudioPluginBinaryHeader*)&theRequest);
				}
				break;
			case JVX_AUDIOPLUGIN_REPORT_PROCESS_COMPLETE:
				process_process_complete(phdr->hdr.ass_process_id);
				break;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		return res;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

void
CjvxPluginVstController::trigger_update_params(jvxInt32* allIds, jvxSize numAllIds, const std::string& tag, jvxInt16 process_id)
{
	jvxAudioPluginBinaryParamHeader* theRequest = nullptr;
	CjvxPluginCommon::prepare_controller_processor_message_trigger_sync(
		numAllIds,
		allIds,
		&theRequest, tag.c_str(), process_id, -1);
	this->plugin_send_message((jvxAudioPluginBinaryHeader*)theRequest);
	CjvxPluginCommon::release_controller_processor_message((jvxAudioPluginBinaryHeader*)theRequest);	
}

void
CjvxPluginVstController::trigger_update_state(jvxInt16 proc_id)
{
	jvxAudioPluginBinaryHeader theRequest;
	theRequest.command = JVX_AUDIOPLUGIN_TRIGGER_SYNC_STATE;
	theRequest.sz = sizeof(jvxAudioPluginBinaryHeader);
	theRequest.rep_process_id = proc_id;
	theRequest.ass_process_id = -1;
	this->plugin_send_message(&theRequest);
}

jvxErrorType
CjvxPluginVstController::trigger_transfer_param_norm_strlist(
	jvxInt32 id, jvxData normv, jvxErrorType resL, jvxApiStringList* strl, const char* tag,
	jvxInt16 proc_id)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxAudioPluginBinaryParamHeader* hdr = nullptr;
	CjvxPluginCommon::prepare_controller_processor_message_param_strlist(
		&hdr, id, normv, resL, strl, tag, proc_id, -1);

	res = this->plugin_send_message((jvxAudioPluginBinaryHeader*)hdr);	
	CjvxPluginCommon::release_controller_processor_message((jvxAudioPluginBinaryHeader*)hdr);

	return res;
}

jvxErrorType
CjvxPluginVstController::trigger_transfer_param_norm(
	jvxInt32 id, jvxData normv, jvxErrorType resL, const char* tag, jvxInt16 process_id)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxAudioPluginBinaryParamHeader* hdr = nullptr;
	CjvxPluginCommon::prepare_controller_processor_message_param_norm(
		&hdr, id, normv, resL, tag, process_id, -1);
	
	res = this->plugin_send_message((jvxAudioPluginBinaryHeader*)hdr);

	CjvxPluginCommon::release_controller_processor_message((jvxAudioPluginBinaryHeader*)hdr);
	return res;
}

jvxErrorType
CjvxPluginVstController::plugin_send_message(jvxAudioPluginBinaryHeader* fld)
{
	if (theController)
	{
		if (IPtr<IMessage> message = owned(theController->allocateMessage()))
		{
			message->setMessageID("BinaryMessage");
			message->getAttributes()->setBinary("jvxrt-audioplugin", fld, fld->sz);
			tresult resStb = theController->sendMessage(message);
			if (resStb == kResultOk)
			{
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		}
		return JVX_ERROR_INTERNAL;
	}
	return JVX_ERROR_NOT_READY;
}

} // namespace Vst
} // namespace Steinberg


