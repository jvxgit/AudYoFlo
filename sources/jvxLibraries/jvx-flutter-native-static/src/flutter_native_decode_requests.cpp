#include "flutter_native_local.h"

extern int ffi_host_allocate_char_array(const std::string& txt, char** txtOnReturn);
extern void ffi_host_allocate_sequencer_event__char_array(TjvxSequencerEvent* ev, struct one_sequencer_event** ptrRet);

int ffi_req_command_decode_uid(void* hdl_class, size_t* uid)
{
	jvxErrorType res = JVX_NO_ERROR;
	const CjvxReportCommandRequest_uid* uidptr = nullptr;
	const CjvxReportCommandRequest* req = (const CjvxReportCommandRequest*)hdl_class;
	if (uid)
	{
		*uid = JVX_SIZE_UNSELECTED;
	}
	if (req->datatype() == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID)
	{
		req->specialization(reinterpret_cast<const jvxHandle**>(&uidptr), jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID);
		if (uidptr)
		{
			uidptr->uid(uid);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return res;
}

int ffi_req_command_decode_sswitch(void* hdl_class, int* sswitchRet)
{
	jvxErrorType res = JVX_NO_ERROR;
	const CjvxReportCommandRequest_ss* ssptr = nullptr;
	const CjvxReportCommandRequest_ss_id* ssidptr = nullptr;
	const CjvxReportCommandRequest* req = (CjvxReportCommandRequest*)hdl_class;
	if (sswitchRet)
	{
		*sswitchRet = JVX_STATE_SWITCH_NONE;
	}
	if (req->datatype() == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS)
	{
		req->specialization(reinterpret_cast<const jvxHandle**>(&ssptr), jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS);
		if (ssptr)
		{
			jvxStateSwitch sswitch = JVX_STATE_SWITCH_NONE;
			ssptr->sswitch(&sswitch);
			if (sswitchRet)
				*sswitchRet = (int)sswitch;
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
	}
	else if (req->datatype() == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS_ID)
	{
		req->specialization(reinterpret_cast<const jvxHandle**>(&ssidptr), jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS_ID);
		if (ssidptr)
		{
			jvxStateSwitch sswitch = JVX_STATE_SWITCH_NONE;
			ssidptr->sswitch(&sswitch);
			if (sswitchRet)
				*sswitchRet = (int)sswitch;
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return res;
}

int ffi_req_command_decode_broadcast(void* hdl_class, int* broadcRet)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxReportCommandBroadcastType bcType = jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NONE;
	CjvxReportCommandRequest* req = (CjvxReportCommandRequest*)hdl_class;
	bcType = req->broadcast();

	if (broadcRet)
	{
		*broadcRet = (int)bcType;
	}

	return res;
}

int ffi_req_command_decode_request_type(void* hdl_class, int* reqRetPtr)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxReportCommandRequest reqType = jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC;
	CjvxReportCommandRequest* req = (CjvxReportCommandRequest*)hdl_class;
	reqType = req->request();

	if (reqRetPtr)
	{
		*reqRetPtr = (int)reqType;
	}

	return res;
}

int ffi_req_command_decode_origin(void* hdl_class, struct component_ident* retCp)
{
	jvxErrorType res = JVX_NO_ERROR;
	CjvxReportCommandRequest_uid* uidptr = nullptr;
	CjvxReportCommandRequest* req = (CjvxReportCommandRequest*)hdl_class;
	jvxComponentIdentification cpTyp = req->origin();
	if (retCp)
	{
		retCp->tp = cpTyp.tp;
		retCp->slotid = cpTyp.slotid;
		retCp->slotsubid = cpTyp.slotsubid;
		retCp->uId = cpTyp.uId;
	}
	return res;
}

char* ffi_req_command_decode_ident_allocate_char_array(void* hdl_class)
{
	char* ptrRet = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	const CjvxReportCommandRequest_id* idptr = nullptr;
	const CjvxReportCommandRequest_ss_id* ssidptr = nullptr;
	const CjvxReportCommandRequest* req = (const CjvxReportCommandRequest*)hdl_class;
	if (req->datatype() == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT)
	{
		req->specialization(reinterpret_cast<const jvxHandle**>(&idptr), jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT);
		if (idptr)
		{
			jvxApiString astr;
			idptr->ident(&astr);
			std::string txt = astr.std_str();
			ffi_host_allocate_char_array(txt, &ptrRet);
		}
	}
	else if (req->datatype() == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS_ID)
	{
		req->specialization(reinterpret_cast<const jvxHandle**>(&ssidptr), jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS_ID);
		if (ssidptr)
		{
			jvxApiString astr;
			ssidptr->ident(&astr);
			std::string txt = astr.std_str();
			ffi_host_allocate_char_array(txt, &ptrRet);
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return ptrRet;
}

struct one_sequencer_event* ffi_req_command_decode_sequencer_event(void* hdl_class)
{
	jvxErrorType res = JVX_NO_ERROR;
	struct one_sequencer_event* ptrRet = nullptr;
	const CjvxReportCommandRequest_seq* seqptr = nullptr;
	const CjvxReportCommandRequest* req = (CjvxReportCommandRequest*)hdl_class;
	if (req->datatype() == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SEQ)
	{
		req->specialization(reinterpret_cast<const jvxHandle**>(&seqptr), jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SEQ);
		if (seqptr)
		{
			TjvxSequencerEvent ev;
			seqptr->seq_event(&ev);
			ffi_host_allocate_sequencer_event__char_array(&ev, &ptrRet);
		}
	}
	return ptrRet;
}