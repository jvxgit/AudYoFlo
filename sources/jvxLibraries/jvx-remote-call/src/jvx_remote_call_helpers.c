#include "jvx_remote_call_helpers.h"

jvxDspBaseErrorType transmit_data_fld_deallocate(jvxByte* resp_byte, jvxSize szFldTransfer, jvx_remote_call_private* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	res = hdl->init_copy.theCallbacks.callback_send_buffer(hdl->init_copy.cb_priv, resp_byte, szFldTransfer);
	resL = hdl->init_copy.theCallbacks.callback_deallocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD, (jvxHandle*)resp_byte, szFldTransfer);
	assert(resL == JVX_DSP_NO_ERROR);
	return(res);
}

jvxDspBaseErrorType produce_request_start(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransfer, jvx_remote_call_private* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxByte* resp_offset = NULL;
	jvxRcProtocol_start_message* locreq = NULL;
	jvxSize offset = 0;
	*szFldData = sizeof(jvxRcProtocol_start_message);

	res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD, (jvxHandle**)resp_byte, &offset, 
		*szFldData, szFldTransfer, JVX_DATAFORMAT_BYTE);
	if (res != JVX_DSP_NO_ERROR)
	{
		return res;
	}
	resp_offset = *resp_byte + offset;
	locreq = (jvxRcProtocol_start_message*)(resp_offset);

	hdl->params_request.transfer.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_REMOTE_CALL;
	hdl->params_request.transfer.loc_header.purpose = JVX_RC_EXCHANGE_START_REQUEST;
	hdl->params_request.transfer.loc_header.paketsize = JVX_SIZE_UINT32(*szFldData);

	*locreq = hdl->params_request;

	return res;
}

jvxDspBaseErrorType produce_request_simple(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransfer, jvx_remote_call_private* hdl, jvxUInt16 tp)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxByte* resp_offset = NULL;
	jvxRcProtocol_simple_message* locreq = NULL;
	jvxSize offset = 0;
	*szFldData = sizeof(jvxRcProtocol_simple_message);

	res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD, (jvxHandle**)resp_byte, &offset, 
		*szFldData, szFldTransfer, JVX_DATAFORMAT_BYTE);
	if (res != JVX_DSP_NO_ERROR)
	{
		return res;
	}
	resp_offset = *resp_byte + offset;
	locreq = (jvxRcProtocol_simple_message*)(resp_offset);
	
	hdl->params_request.transfer.processingCounter++;
	hdl->params_request.transfer.loc_header.purpose = tp;

	locreq->loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_REMOTE_CALL;
	locreq->loc_header.purpose = tp;
	locreq->loc_header.paketsize = JVX_SIZE_UINT32(*szFldData);

	locreq->processingCounter = hdl->params_request.transfer.processingCounter;
	locreq->processingId = hdl->params_request.transfer.processingId;
	return res;
}

jvxDspBaseErrorType produce_request_data_content(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransfer, jvx_remote_call_private* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxByte* resp_offset = NULL;
	jvxRcProtocol_data_message* locreq = NULL;
	jvxSize offset = 0;
	*szFldData = sizeof(jvxRcProtocol_data_message);

	res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD, (jvxHandle**)resp_byte, &offset, 
		*szFldData, szFldTransfer, JVX_DATAFORMAT_BYTE);
	if (res != JVX_DSP_NO_ERROR)
	{
		return res;
	}
	resp_offset = *resp_byte + offset;
	locreq = (jvxRcProtocol_data_message*)(resp_offset);

	locreq->transfer.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_REMOTE_CALL;
	locreq->transfer.loc_header.purpose = JVX_RC_DATA_RESPONSE_OUT_REQUEST;
	locreq->transfer.loc_header.paketsize = JVX_SIZE_UINT32(*szFldData);

	locreq->param_id = JVX_SIZE_UINT16(hdl->helpers.tmpCnt0);
	locreq->segy_id = JVX_SIZE_UINT16(hdl->helpers.tmpCnt1);
	
	hdl->params_request.transfer.processingCounter++;
	hdl->params_request.transfer.loc_header.purpose = JVX_RC_DATA_RESPONSE_OUT_REQUEST;
	
	locreq->transfer.processingCounter= hdl->params_request.transfer.processingCounter;
	locreq->transfer.processingId = hdl->params_request.transfer.processingId;
	return res;
}

jvxDspBaseErrorType produce_request_data_descriptor(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransfer, jvx_remote_call_private* hdl, 
	jvxCBool send_input)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxByte* resp_offset = NULL;
	jvxRcProtocol_data_description_message* locreq = NULL;
	jvxSize offset = 0;
	*szFldData = sizeof(jvxRcProtocol_data_description_message);

	res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD, (jvxHandle**)resp_byte, &offset, *szFldData, 
		szFldTransfer, JVX_DATAFORMAT_BYTE);
	if (res != JVX_DSP_NO_ERROR)
	{
		return res;
	}
	resp_offset = *resp_byte + offset;
	locreq = (jvxRcProtocol_data_description_message*)(resp_offset);

	locreq->transfer.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_REMOTE_CALL;
	locreq->transfer.loc_header.paketsize = JVX_SIZE_UINT32(*szFldData);

	locreq->param_id = JVX_SIZE_UINT16(hdl->helpers.tmpCnt0);
	
	hdl->params_request.transfer.processingCounter++;

	locreq->transfer.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_REMOTE_CALL;
	locreq->transfer.processingCounter = hdl->params_request.transfer.processingCounter;
	locreq->transfer.processingId = hdl->params_request.transfer.processingId;

	if (send_input)
	{
		locreq->description = hdl->input_params[hdl->helpers.tmpCnt0].description;
		locreq->transfer.loc_header.purpose = JVX_RC_DATA_TRANSFER_IN_PRE_REQUEST;
	}
	else
	{
		locreq->description = hdl->output_params[hdl->helpers.tmpCnt0].description;
		locreq->transfer.loc_header.purpose = JVX_RC_DATA_TRANSFER_OUT_PRE_REQUEST;
	}
	hdl->params_request.transfer.loc_header.purpose = locreq->transfer.loc_header.purpose;
	return res;
}

// ===============================================================================
// ===============================================================================

jvxDspBaseErrorType produce_transfer_data_content(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransfer, jvx_remote_call_private* hdl, 
		jvxCBool send_input)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxByte* resp_offset = NULL;
	jvxRcProtocol_data_message_with_load* locreq = NULL;
	jvxSize mySize = 0;
	jvxSize offset = 0;
	
	if (send_input)
	{
		mySize = jvxDataFormat_size[hdl->input_params[hdl->helpers.tmpCnt0].description.format] *
			hdl->input_params[hdl->helpers.tmpCnt0].description.seg_length_x;
	}
	else
	{
		mySize = jvxDataFormat_size[hdl->output_params[hdl->helpers.tmpCnt0].description.format] *
			hdl->output_params[hdl->helpers.tmpCnt0].description.seg_length_x;
	}

	*szFldData = sizeof(jvxRcProtocol_data_message) + mySize;

	res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD, (jvxHandle**)resp_byte, &offset, *szFldData, 
		szFldTransfer, JVX_DATAFORMAT_BYTE);
	if (res != JVX_DSP_NO_ERROR)
	{
		return res;
	}
	resp_offset = *resp_byte + offset;
	locreq = (jvxRcProtocol_data_message_with_load*)(resp_offset);

	hdl->params_request.transfer.processingCounter++;

	locreq->message.transfer.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_REMOTE_CALL;
	locreq->message.transfer.loc_header.paketsize = JVX_SIZE_UINT32(*szFldData);
	
	if (send_input)
	{
		locreq->message.transfer.loc_header.purpose = JVX_RC_DATA_TRANSFER_IN_REQUEST;
		locreq->message.param_id = JVX_SIZE_UINT16(hdl->helpers.tmpCnt0);
		locreq->message.segy_id = JVX_SIZE_UINT16(hdl->helpers.tmpCnt1);
		
		hdl->params_request.transfer.loc_header.purpose = JVX_RC_DATA_TRANSFER_IN_REQUEST;

		locreq->message.transfer.processingCounter = hdl->params_request.transfer.processingCounter;
		locreq->message.transfer.processingId = hdl->params_request.transfer.processingId;

		memcpy(&locreq->firstByteLoad, hdl->input_params[hdl->helpers.tmpCnt0].content[hdl->helpers.tmpCnt1],
			mySize);
	}
	else
	{
		locreq->message.transfer.loc_header.purpose = JVX_RC_DATA_RESPONSE_OUT_RESPONSE;
		locreq->message.param_id = JVX_SIZE_UINT16(hdl->helpers.tmpCnt0);
		locreq->message.segy_id = JVX_SIZE_UINT16(hdl->helpers.tmpCnt1);

		hdl->params_request.transfer.loc_header.purpose = JVX_RC_DATA_RESPONSE_OUT_RESPONSE;

		locreq->message.transfer.processingCounter = hdl->params_request.transfer.processingCounter;
		locreq->message.transfer.processingId = hdl->params_request.transfer.processingId;

		memcpy(&locreq->firstByteLoad, hdl->output_params[hdl->helpers.tmpCnt0].content[hdl->helpers.tmpCnt1],
			mySize);
	}
	return res;
}

// ===============================================================================
// ===============================================================================

jvxDspBaseErrorType produce_response_general(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransfer,
		jvx_remote_call_private* hdl, jvxUInt16 tp, jvxCBool suc)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxByte* resp_offset = NULL;
	jvxRcProtocol_response_message* locresp = NULL;
	jvxSize offset = 0;
	*szFldData = sizeof(jvxRcProtocol_response_message);

	res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD, (jvxHandle**)resp_byte, &offset, *szFldData, szFldTransfer, JVX_DATAFORMAT_BYTE);
	if (res != JVX_DSP_NO_ERROR)
	{
		return res;
	}
	resp_offset = *resp_byte + offset;
	locresp = (jvxRcProtocol_response_message*)(resp_offset);

	locresp->transfer.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_REMOTE_CALL;
	locresp->transfer.loc_header.purpose = tp;
	locresp->transfer.loc_header.paketsize = JVX_SIZE_UINT32(*szFldData);


	hdl->params_request.transfer.loc_header.purpose = tp;
	hdl->params_request.transfer.processingCounter++;

	locresp->transfer.processingCounter = hdl->params_request.transfer.processingCounter;
	locresp->transfer.processingId = hdl->params_request.transfer.processingId;
	
	locresp->success = (jvxUInt16)suc;
	return res;
}

jvxDspBaseErrorType produce_response_error(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransfer, jvx_remote_call_private* hdl, const char* errdescr, jvxDspBaseErrorType errcode, jvxSize errid)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxByte* resp_offset = NULL;
	jvxRcProtocol_error_response_message* locresp = NULL;
	jvxSize offset = 0;
	jvxSize strsz;
	*szFldData = sizeof(jvxRcProtocol_error_response_message);

	res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD, (jvxHandle**)resp_byte, &offset, 
		*szFldData, szFldTransfer, JVX_DATAFORMAT_BYTE);
	if (res != JVX_DSP_NO_ERROR)
	{
		return res;
	}
	resp_offset = *resp_byte + offset;
	locresp = (jvxRcProtocol_error_response_message*)(resp_offset);

	locresp->response.transfer.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_REMOTE_CALL;
	locresp->response.transfer.loc_header.purpose = JVX_RC_ERROR_TRIGGER;
	locresp->response.transfer.loc_header.paketsize = JVX_SIZE_UINT32(*szFldData);

	hdl->params_request.transfer.processingCounter++;
	hdl->params_request.transfer.loc_header.purpose = JVX_RC_ERROR_TRIGGER;

	locresp->response.transfer.processingCounter = hdl->params_request.transfer.processingCounter;
	locresp->response.transfer.processingId = hdl->params_request.transfer.processingId;

	locresp->response.success = c_false;
	locresp->err_code = JVX_SIZE_UINT16(errcode);
	locresp->err_id = JVX_SIZE_UINT16(errid);
	memset(locresp->error, 0, JVX_RC_LENGTH_REQUEST_TEXTFIELD);
	strsz = JVX_MIN(strlen(errdescr), JVX_RC_LENGTH_REQUEST_TEXTFIELD - 1);
	memcpy(locresp->error, errdescr, strsz);
	return(res);
}

