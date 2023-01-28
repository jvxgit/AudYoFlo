#include "jvx_dsp_base.h"
#include "jvx_remote_call_helpers.h"

#ifdef COMPILE_FOR_OMAP_137
#define JVX_ASSERT_STRING_TOKEN "jvx_remote_call.c"
#define JVX_ASSERT_INT_ID __LINE__
#else
#define JVX_ASSERT_STRING_TOKEN __FUNCTION__
#define JVX_ASSERT_INT_ID __LINE__
#endif

// =============================================================================

#ifdef JVX_REMOTE_CALL_STATIC_OBJECT
static jvx_remote_call_private theRemoteCallHandle;
static jvxCBool is_initialized = false;
#endif

jvxDspBaseErrorType jvx_remote_call_initCfg(jvx_remote_call* hdl)
{
	hdl->prmInit.delayedSend = false;
	hdl->prmInit.operation_mode = JVX_RC_OPERATE_MASTER;

	hdl->prmInit.theCallbacks.callback_unlock_receive_buffer = NULL;
	hdl->prmInit.theCallbacks.callback_allocate_memory = NULL;
	hdl->prmInit.theCallbacks.callback_deallocate_memory = NULL;
	hdl->prmInit.theCallbacks.callback_accept_init_request = NULL;
	hdl->prmInit.theCallbacks.callback_send_buffer = NULL;
	hdl->prmInit.theCallbacks.callback_process_request = NULL;
	hdl->prmInit.theCallbacks.callback_lock = NULL;
	hdl->prmInit.theCallbacks.callback_unlock = NULL;
	hdl->prmInit.theCallbacks.callback_error_report = NULL;
	hdl->prmInit.theCallbacks.callback_transfer_terminated = NULL;
	hdl->prmInit.cb_priv = NULL;
	return JVX_DSP_NO_ERROR;
}
jvxDspBaseErrorType
jvx_remote_call_init(jvx_remote_call* hdl)
//jvx_remote_call_init(jvxHandle** hdlReturn, jvxRCOperationMode mode, jvx_rc_operate_callbacks* theCallbacks, jvxHandle* priv)
{
	jvx_remote_call_private* ptr = NULL;
	if (hdl->prv == NULL)
	{
#ifdef JVX_REMOTE_CALL_STATIC_OBJECT
		if (is_initialized)
		{
			return(JVX_DSP_ERROR_WRONG_STATE);
		}
		else
		{
			ptr = &theRemoteCallHandle;
			is_initialized = true;
		}
#else
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_remote_call_private);
#endif

		memset(ptr, 0, sizeof(jvx_remote_call_private));
		ptr->init_copy = hdl->prmInit;

		// Minimum constraints
		JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, ptr->init_copy.theCallbacks.callback_accept_init_request != NULL);
		JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, ptr->init_copy.theCallbacks.callback_allocate_memory != NULL);
		JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, ptr->init_copy.theCallbacks.callback_deallocate_memory != NULL);
		JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, ptr->init_copy.theCallbacks.callback_process_request != NULL);
		JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, ptr->init_copy.theCallbacks.callback_send_buffer != NULL);

		hdl->prv = ptr;
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_WRONG_STATE);
}


jvxDspBaseErrorType 
jvx_remote_call_terminate(jvx_remote_call* hdl)
//jvx_remote_call_terminate(jvxHandle* hdlReturn)
{
	jvx_remote_call_private* ptr = NULL;
	if (hdl->prv)
	{
		ptr = hdl->prv;
		
		// Set everything to 0
		memset(&ptr->init_copy, 0, sizeof(ptr->init_copy));

#ifdef JVX_REMOTE_CALL_STATIC_OBJECT
	
		if (is_initialized)
		{
			JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, ptr == &theRemoteCallHandle);
			is_initialized = false;
		}
		else
		{
			return(JVX_DSP_ERROR_WRONG_STATE);
		}

#else
		JVX_DSP_SAFE_DELETE_OBJECT(ptr);
#endif
		hdl->prv = NULL;
		return(JVX_DSP_NO_ERROR);
	}
	
	return(JVX_DSP_ERROR_WRONG_STATE);
}

jvxDspBaseErrorType jvx_remote_call_reset_local(jvx_remote_call_private* hdl)
{
	jvxSize i, j;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxSize mySize = 0;
	JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->init_copy.theCallbacks.callback_deallocate_memory != NULL);

	if (hdl->init_copy.theCallbacks.callback_lock)
	{
		hdl->init_copy.theCallbacks.callback_lock(hdl->init_copy.cb_priv);
	}

	if (hdl->init_copy.operation_mode == JVX_RC_OPERATE_SLAVE)
	{
		if (hdl->input_params)
		{
			for (i = 0; i < hdl->params_request.numInParams; i++)
			{
				if (hdl->input_params[i].content)
				{
					mySize = jvxDataFormat_getsize(hdl->input_params[i].description.format) * hdl->input_params[i].description.seg_length_x;
					for (j = 0; j < hdl->input_params[i].description.seg_length_y; j++)
					{
						res = hdl->init_copy.theCallbacks.callback_deallocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD,
							hdl->input_params[i].content[j], mySize);
					}
					mySize = sizeof(jvxHandle*) * hdl->input_params[i].description.seg_length_y;
					res = hdl->init_copy.theCallbacks.callback_deallocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD_PTR,
						hdl->input_params[i].content, mySize);
				}
			}
			mySize = hdl->params_request.numInParams * sizeof(jvxRCOneParameter);
			res = hdl->init_copy.theCallbacks.callback_deallocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_INPUT_FIELD, hdl->input_params, mySize);
			JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
			hdl->input_params = NULL;
		}
		if (hdl->output_params)
		{
			for (i = 0; i < hdl->params_request.numOutParams; i++)
			{
				if (hdl->output_params[i].content)
				{
					mySize = jvxDataFormat_getsize(hdl->output_params[i].description.format) * hdl->output_params[i].description.seg_length_x;
					for (j = 0; j < hdl->output_params[i].description.seg_length_y; j++)
					{
						res = hdl->init_copy.theCallbacks.callback_deallocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD,
							hdl->output_params[i].content[j], mySize);
					}
					mySize = sizeof(jvxHandle*) * hdl->output_params[i].description.seg_length_y;
					res = hdl->init_copy.theCallbacks.callback_deallocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD_PTR,
						hdl->output_params[i].content, mySize);
				}
			}
			mySize = hdl->params_request.numOutParams * sizeof(jvxRCOneParameter);
			res = hdl->init_copy.theCallbacks.callback_deallocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_OUTPUT_FIELD, hdl->output_params, mySize);
			JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
		}
	}
	hdl->input_params = NULL;
	hdl->output_params = NULL;

	hdl->params_request.numInParams = 0;
	hdl->params_request.numOutParams = 0;

	hdl->transfer_state = JVX_RC_STATE_NONE;
	return res;
}

jvxDspBaseErrorType
jvx_remote_call_master_switch_state(jvxByte** fldTransfer, jvxSize* numBytesData, jvxSize* numBytesTransfer, jvxCBool* requestTransfer,
	jvx_remote_call_private* hdl, jvxProtocolHeader* buf_api, jvxSize unique_id)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;

	//jvxRcProtocol_error_response_message* ptrErrorHeader = (jvxRcProtocol_error_response_message*)(buf_api );
	jvxRcProtocol_response_message* ptrResponseHeader = (jvxRcProtocol_response_message*)(buf_api);


	// Check the size of the data package
	if (buf_api->paketsize >= (sizeof(jvxRcProtocol_response_message)))
	{
		if (ptrResponseHeader->success)
		{
			if (buf_api->paketsize == (sizeof(jvxRcProtocol_response_message)))
			{
				while (1)
				{
					// In state started just shift state
					if (hdl->transfer_state == JVX_RC_STATE_STARTED)
					{
						hdl->transfer_state = JVX_RC_STATE_SENDING_PARAMETERS_IN_PRE;
						hdl->helpers.tmpCnt0 = 0;
						hdl->helpers.tmpCnt1 = 0;
						// continue..
					}

					if (hdl->transfer_state == JVX_RC_STATE_SENDING_PARAMETERS_IN_PRE)
					{
						if (hdl->helpers.tmpCnt0 < hdl->params_request.numInParams)
						{
							// Send info on next input channel
							res = produce_request_data_descriptor(fldTransfer, numBytesData, numBytesTransfer, hdl, true);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							*requestTransfer = c_true;
							hdl->helpers.tmpCnt0++;
							break;
						}
						else
						{
							hdl->transfer_state = JVX_RC_STATE_SENDING_PARAMETERS_IN;
							hdl->helpers.tmpCnt0 = 0;
							hdl->helpers.tmpCnt1 = 0;
							// continue ..
						}
					}

					if (hdl->transfer_state == JVX_RC_STATE_SENDING_PARAMETERS_IN)
					{
						if (hdl->helpers.tmpCnt0 < hdl->params_request.numInParams)
						{
							if (hdl->helpers.tmpCnt1 < hdl->input_params[hdl->helpers.tmpCnt0].description.seg_length_y)
							{
								// Send content of input channel
								res = produce_transfer_data_content(fldTransfer, numBytesData, numBytesTransfer, hdl, c_true);
								JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
								*requestTransfer = c_true;
								hdl->helpers.tmpCnt1++;
								break;
							}
							else
							{
								hdl->helpers.tmpCnt0++;
								hdl->helpers.tmpCnt1 = 0;
							}
						}
						else
						{
							if (hdl->params_request.numOutParams)
							{
								hdl->transfer_state = JVX_RC_STATE_SENDING_PARAMETERS_OUT_PRE;
								hdl->helpers.tmpCnt0 = 0;
							}
							else
							{
								hdl->transfer_state = JVX_RC_STATE_TRIGGER_ACTION;
							}
						}
					}

					if (hdl->transfer_state == JVX_RC_STATE_SENDING_PARAMETERS_OUT_PRE)
					{
						if (hdl->helpers.tmpCnt0 < hdl->params_request.numOutParams)
						{
							// Send info on next output channel
							res = produce_request_data_descriptor(fldTransfer, numBytesData, numBytesTransfer, hdl, false);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							*requestTransfer = c_true;
							hdl->helpers.tmpCnt0++;
							break;
						}
						else
						{
							hdl->transfer_state = JVX_RC_STATE_TRIGGER_ACTION;
							hdl->helpers.tmpCnt0 = 0;
							hdl->helpers.tmpCnt1 = 0;
						}
					}
					if (hdl->transfer_state == JVX_RC_STATE_TRIGGER_ACTION)
					{
						// Send info on next input channel
						// Send response message
						res = produce_request_simple(fldTransfer, numBytesData, numBytesTransfer, hdl, JVX_RC_TRIGGER_ACTION_REQUEST);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						*requestTransfer = c_true;
						hdl->helpers.tmpCnt0 = 0;
						hdl->helpers.tmpCnt1 = 0;
						break;
					}
				} // while(1)
			}
			else
			{
				res = JVX_DSP_ERROR_INTERNAL;
			}
		}
		else
		{
			if (buf_api->paketsize == (sizeof(jvxRcProtocol_error_response_message)))
			{
				res = JVX_DSP_ERROR_ABORT;
			}
			else
			{
				res = JVX_DSP_ERROR_INTERNAL;
			}
		}
	}
	else
	{
		res = JVX_DSP_ERROR_INTERNAL;
	}
	return(res);
}

jvxDspBaseErrorType jvx_remote_call_reset(jvx_remote_call* hdlIn)
{
	if (hdlIn)
	{
		jvx_remote_call_private* hdl = (jvx_remote_call_private*)hdlIn->prv;
		return jvx_remote_call_reset_local(hdl);
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

// Callback to report a complete new commData buffer
jvxDspBaseErrorType 
jvx_remote_call_incoming(jvx_remote_call* hdlIn, jvxProtocolHeader* buf_api, jvxSize unique_id
	, jvxByte** send_ptr, jvxSize* num_bytes)
{
	jvxSize i;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;

	jvxRcProtocol_simple_message* ptrSimpleMessage = (jvxRcProtocol_simple_message*)((jvxByte*)buf_api);
	jvxRcProtocol_start_message* ptrStartMessage = (jvxRcProtocol_start_message*)((jvxByte*)buf_api);
	jvxRcProtocol_response_message* ptrResponseMessage = (jvxRcProtocol_response_message*)((jvxByte*)buf_api);
	jvxRcProtocol_data_message* ptrDataMessage = (jvxRcProtocol_data_message*)((jvxByte*)buf_api);
	jvxRcProtocol_data_message_with_load* ptrDataMessageWithLoad = (jvxRcProtocol_data_message_with_load*)((jvxByte*)buf_api);
	jvxRcProtocol_data_description_message* ptrDataDescriptionMessage = (jvxRcProtocol_data_description_message*)((jvxByte*)buf_api);
	jvxRcProtocol_error_response_message* ptrErrorMessage = (jvxRcProtocol_error_response_message*)((jvxByte*)buf_api);
	jvxByte* ptrLoadCopy = (jvxByte*)(&ptrDataMessageWithLoad->firstByteLoad);

	jvxByte* resp_byte = NULL;
	//jvxRcProtocol_response_message* locresp = NULL;
	jvxSize offset;
	jvx_remote_call_private* hdl = NULL;

	jvxCBool requestTransfer = c_false;
	jvxByte* fldTransfer = NULL;
	//jvxSize numBytesTransfer = 0;
	jvxSize numBytesTransmit = 0;
	jvxSize numBytesData = 0;
	jvxCBool reportError = c_false;
	const char* errtext = NULL;
	jvxDspBaseErrorType errCode = JVX_DSP_NO_ERROR;
	jvxSize errId = 0;
	jvxSize mySize = 0;
	jvxCBool immediateResponse = c_true;

	jvxCBool reportTransferComplete = c_false;
	if (send_ptr)
	{
		*send_ptr = NULL;
	}
	if (num_bytes)
	{
		*num_bytes = 0;
	}

	if (hdlIn)
	{
		hdl = (jvx_remote_call_private*)hdlIn->prv;

		if (hdl->init_copy.theCallbacks.callback_lock)
		{
			hdl->init_copy.theCallbacks.callback_lock(hdl->init_copy.cb_priv);
		}

		switch (hdl->init_copy.operation_mode)
		{
		case JVX_RC_OPERATE_SLAVE:
			switch (buf_api->purpose)
			{
			case JVX_RC_ERROR_TRIGGER:

				// Report error, mainly switch to start condifition				
				jvx_remote_call_reset_local(hdl);

				reportError = c_true;
				errtext = "Error package received";
				errId = JVX_ASSERT_INT_ID;
				errCode = JVX_DSP_ERROR_ABORT;

				break;

			case JVX_RC_EXCHANGE_START_REQUEST:

				// Are we in the right state?
				if (hdl->transfer_state == JVX_RC_STATE_NONE)
				{

					// Check the size of the data package
					if (buf_api->paketsize == (sizeof(jvxRcProtocol_start_message)))
					{
						// Copy the important parameters from request
						hdl->params_request = *ptrStartMessage; // Copy also processCounter

						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->input_params == NULL);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->output_params == NULL);
						
						res = hdl->init_copy.theCallbacks.callback_accept_init_request(hdl->init_copy.cb_priv, hdl->params_request.description,
							hdl->params_request.numInParams, hdl->params_request.numOutParams, hdl->params_request.transfer.processingId);
						if (res == JVX_DSP_NO_ERROR)
						{
							hdl->helpers.tmpCnt0 = 0;
							if (hdl->params_request.numInParams > 0)
							{
								res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_INPUT_FIELD, (jvxHandle**)&hdl->input_params, &offset,
									hdl->params_request.numInParams * sizeof(jvxRCOneParameter), &numBytesTransmit, JVX_DATAFORMAT_BYTE);
								JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							}
							if (hdl->params_request.numOutParams > 0)
							{
								res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_OUTPUT_FIELD,
									(jvxHandle**)&hdl->output_params, &offset, hdl->params_request.numOutParams * sizeof(jvxRCOneParameter), 
									&numBytesTransmit, JVX_DATAFORMAT_BYTE);
								JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							}

							if (hdl->params_request.numInParams > 0)
							{
								hdl->transfer_state = JVX_RC_STATE_SENDING_PARAMETERS_IN_PRE;
							}
							else
							{
								if (hdl->params_request.numOutParams > 0)
								{
									hdl->transfer_state = JVX_RC_STATE_SENDING_PARAMETERS_OUT_PRE;
								}
								else
								{
									hdl->transfer_state = JVX_RC_STATE_TRIGGER_ACTION;
								}
							}

							// Send response message
							res = produce_response_general(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, JVX_RC_EXCHANGE_START_RESPONSE, c_true);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
						}
						else
						{
							reportError = c_true;
							errtext = "Request denied by peer";
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_INTERNAL;

							res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
							jvx_remote_call_reset_local(hdl);
						}
					}
					else
					{
						reportError = c_true;
						errtext = "Invalid package length";
						errId = JVX_ASSERT_INT_ID;
						errCode = JVX_DSP_ERROR_INTERNAL;

						res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						requestTransfer = c_true;
						jvx_remote_call_reset_local(hdl);
					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl,  errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);
				}
				break;

			case JVX_RC_DATA_TRANSFER_IN_PRE_REQUEST:
				// Are we in the right state?
				if (hdl->transfer_state == JVX_RC_STATE_SENDING_PARAMETERS_IN_PRE)
				{

					// Check the size of the data package
					if (buf_api->paketsize == sizeof(jvxRcProtocol_data_description_message))
					{
						if (ptrDataDescriptionMessage->param_id == hdl->helpers.tmpCnt0)
						{
							hdl->input_params[hdl->helpers.tmpCnt0].description = ptrDataDescriptionMessage->description;
							hdl->params_request.transfer.processingCounter = ptrDataDescriptionMessage->transfer.processingCounter;

							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->input_params[hdl->helpers.tmpCnt0].description.float_precision_id == JVX_FLOAT_DATAFORMAT_ID);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->input_params[hdl->helpers.tmpCnt0].description.seg_length_y);

							res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD_PTR,
								(jvxHandle**)&resp_byte, &offset,
								hdl->input_params[hdl->helpers.tmpCnt0].description.seg_length_y, &numBytesTransmit, 
								JVX_DATAFORMAT_POINTER);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);

							hdl->input_params[hdl->helpers.tmpCnt0].content = (jvxHandle**)resp_byte;

							for (i = 0; i < hdl->input_params[hdl->helpers.tmpCnt0].description.seg_length_y; i++)
							{
								res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD, (jvxHandle**)&resp_byte, &offset,
									hdl->input_params[hdl->helpers.tmpCnt0].description.seg_length_x, &numBytesTransmit,
									(jvxDataFormat)hdl->input_params[hdl->helpers.tmpCnt0].description.format);
								JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
								hdl->input_params[hdl->helpers.tmpCnt0].content[i] = resp_byte;
							}

							hdl->helpers.tmpCnt0++;
							if (hdl->helpers.tmpCnt0 == hdl->params_request.numInParams)
							{
								hdl->transfer_state = JVX_RC_STATE_SENDING_PARAMETERS_IN;
								hdl->helpers.tmpCnt0 = 0;
								hdl->helpers.tmpCnt1 = 0;
							}

							// Send response message
							res = produce_response_general(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, JVX_RC_DATA_TRANSFER_IN_PRE_RESPONSE, c_true);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
						}
						else
						{
							reportError = c_true;
							errtext = "Wrong parameter id input param";
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_INTERNAL;

							res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
							jvx_remote_call_reset_local(hdl);
						}
					}
					else
					{
						reportError = c_true;
						errtext = "Invalid package length";
						errId = JVX_ASSERT_INT_ID;
						errCode = JVX_DSP_ERROR_INTERNAL;

						res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						requestTransfer = c_true;
						jvx_remote_call_reset_local(hdl);

					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);

				}
				break;
			case JVX_RC_DATA_TRANSFER_IN_REQUEST:
				// Are we in the right state?
				if (hdl->transfer_state == JVX_RC_STATE_SENDING_PARAMETERS_IN)
				{

					// Check the size of the data package
					mySize = jvxDataFormat_size[hdl->input_params[hdl->helpers.tmpCnt0].description.format] *
						hdl->input_params[hdl->helpers.tmpCnt0].description.seg_length_x;
					if (buf_api->paketsize == (sizeof(jvxRcProtocol_data_message) + mySize))
					{
						hdl->params_request.transfer.processingCounter = ptrDataMessage->transfer.processingCounter;

						if ((ptrDataMessage->param_id == hdl->helpers.tmpCnt0) && (ptrDataMessage->segy_id == hdl->helpers.tmpCnt1))
						{
							if (mySize)
							{
								memcpy(hdl->input_params[hdl->helpers.tmpCnt0].content[hdl->helpers.tmpCnt1], ptrLoadCopy, mySize);
							}
							hdl->helpers.tmpCnt1++;

							if (hdl->helpers.tmpCnt1 == hdl->input_params[hdl->helpers.tmpCnt0].description.seg_length_y)
							{
								// Go to next in parameter and reset segment counter
								hdl->helpers.tmpCnt0++;
								hdl->helpers.tmpCnt1 = 0;

								if (hdl->helpers.tmpCnt0 == hdl->params_request.numInParams)
								{
									if (hdl->params_request.numOutParams > 0)
									{
										hdl->transfer_state = JVX_RC_STATE_SENDING_PARAMETERS_OUT_PRE;
										hdl->helpers.tmpCnt0 = 0;
										hdl->helpers.tmpCnt1 = 0;
									}
									else
									{
										hdl->transfer_state = JVX_RC_STATE_TRIGGER_ACTION;
									}
								}
							}

							// Send response message
							res = produce_response_general(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, JVX_RC_DATA_TRANSFER_IN_RESPONSE, c_true);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
						}
						else
						{
							reportError = c_true;
							errtext = "Wrong segy id input param";
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_INTERNAL;

							res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
							jvx_remote_call_reset_local(hdl);
						}
					}
					else
					{
						reportError = c_true;
						errtext = "Invalid package length";
						errId = JVX_ASSERT_INT_ID;
						errCode = JVX_DSP_ERROR_INTERNAL;

						res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						requestTransfer = c_true;
						jvx_remote_call_reset_local(hdl);

					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);

				}
				break;
			case JVX_RC_DATA_TRANSFER_OUT_PRE_REQUEST:
				// Are we in the right state?
				if (hdl->transfer_state == JVX_RC_STATE_SENDING_PARAMETERS_OUT_PRE)
				{
					// Check the size of the data package
					if (buf_api->paketsize == sizeof(jvxRcProtocol_data_description_message))
					{
						hdl->params_request.transfer.processingCounter = ptrDataDescriptionMessage->transfer.processingCounter;

						if (ptrDataDescriptionMessage->param_id == hdl->helpers.tmpCnt0)
						{
							hdl->output_params[hdl->helpers.tmpCnt0].description = ptrDataDescriptionMessage->description;

							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->output_params[hdl->helpers.tmpCnt0].description.float_precision_id == JVX_FLOAT_DATAFORMAT_ID);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->output_params[hdl->helpers.tmpCnt0].description.seg_length_y);

							res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD_PTR, (jvxHandle**)&resp_byte, &offset,
								hdl->output_params[hdl->helpers.tmpCnt0].description.seg_length_y, 
								&numBytesData, JVX_DATAFORMAT_POINTER);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							hdl->output_params[hdl->helpers.tmpCnt0].content = (jvxHandle**)resp_byte;

							for (i = 0; i < hdl->output_params[hdl->helpers.tmpCnt0].description.seg_length_y; i++)
							{
								res = hdl->init_copy.theCallbacks.callback_allocate_memory(hdl->init_copy.cb_priv, JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD, (jvxHandle**)&resp_byte, &offset,
									hdl->output_params[hdl->helpers.tmpCnt0].description.seg_length_x, 
									&numBytesData,
									(jvxDataFormat)hdl->output_params[hdl->helpers.tmpCnt0].description.format);
								JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
								hdl->output_params[hdl->helpers.tmpCnt0].content[i] = resp_byte;
							}

							hdl->helpers.tmpCnt0++;
							if (hdl->helpers.tmpCnt0 == hdl->params_request.numOutParams)
							{
								hdl->transfer_state = JVX_RC_STATE_TRIGGER_ACTION;
								hdl->helpers.tmpCnt0 = 0;
								hdl->helpers.tmpCnt1 = 0;
							}

							// Send response message
							res = produce_response_general(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, JVX_RC_DATA_TRANSFER_OUT_PRE_RESPONSE, c_true);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
						}
						else
						{
							reportError = c_true;
							errtext = "Wrong parameter id output param";
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_INTERNAL;

							res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl,  errtext, errCode, errId);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
							jvx_remote_call_reset_local(hdl);
						}
					}
					else
					{
						reportError = c_true;
						errtext = "Invalid package length";
						errId = JVX_ASSERT_INT_ID;
						errCode = JVX_DSP_ERROR_INTERNAL;

						res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						requestTransfer = c_true;
						jvx_remote_call_reset_local(hdl);

					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);

				}
				break;
			case JVX_RC_TRIGGER_ACTION_REQUEST:
				// Are we in the right state?
				if (hdl->transfer_state == JVX_RC_STATE_TRIGGER_ACTION)
				{
					// Check the size of the data package
					if (buf_api->paketsize == sizeof(jvxRcProtocol_simple_message))
					{
						hdl->params_request.transfer = *ptrSimpleMessage; // Copies also counter

						immediateResponse = c_true;
						res = hdl->init_copy.theCallbacks.callback_process_request(hdl->init_copy.cb_priv, hdl->params_request.description,
							hdl->input_params, hdl->params_request.numInParams, hdl->output_params, hdl->params_request.numOutParams, 
							hdl->params_request.transfer.processingId, &immediateResponse);
						if(immediateResponse == c_true)
						{
							if (res == JVX_DSP_NO_ERROR)
							{
								hdl->helpers.tmpCnt0 = 0;
								hdl->helpers.tmpCnt1 = 0;

								if (hdl->params_request.numOutParams)
								{
									hdl->transfer_state = JVX_RC_STATE_RESPONDING_PARAMETERS_OUT;
								}
								else
								{
									hdl->transfer_state = JVX_RC_STATE_TERMINATING_ACTION;
								}
								res = produce_response_general(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, JVX_RC_TRIGGER_ACTION_RESPONSE, c_true);
								JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
								requestTransfer = c_true;
							}
							else
							{

								reportError = c_true;
								errtext = "Process request failed";
								errId = JVX_ASSERT_INT_ID;
								errCode = res;

								res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl,  errtext, errCode, errId);
								JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
								requestTransfer = c_true;
								jvx_remote_call_reset_local(hdl);
							}
						}
						else
						{
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, 0);
						}
					}
					else
					{
						reportError = c_true;
						errtext = "Invalid package length";
						errId = JVX_ASSERT_INT_ID;
						errCode = JVX_DSP_ERROR_INTERNAL;

						res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl,  errtext, errCode, errId);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						requestTransfer = c_true;
						jvx_remote_call_reset_local(hdl);

					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);

				}
				break;
			case JVX_RC_DATA_RESPONSE_OUT_REQUEST:
				// Are we in the right state?
				if (hdl->transfer_state == JVX_RC_STATE_RESPONDING_PARAMETERS_OUT)
				{
					// Check the size of the data package
					//jvxSize mySize = jvxDataFormat_size[hdl->output_params[hdl->helpers.tmpCnt0].description.format] *
					//	hdl->output_params[hdl->helpers.tmpCnt0].description.seg_length_x;
					if (buf_api->paketsize == sizeof(jvxRcProtocol_data_message))
					{
						hdl->params_request.transfer = *ptrSimpleMessage;

						// Prepare response message
						res = produce_transfer_data_content(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, false);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						requestTransfer = c_true;

						// Let us see what will be the next state
						hdl->helpers.tmpCnt1++;
						if (hdl->helpers.tmpCnt1 == hdl->output_params[hdl->helpers.tmpCnt0].description.seg_length_y)
						{
							hdl->helpers.tmpCnt0++;
							if (hdl->helpers.tmpCnt0 == hdl->params_request.numOutParams)
							{
								hdl->transfer_state = JVX_RC_STATE_TERMINATING_ACTION;
							}
						}

					}
					else
					{
						reportError = c_true;
						errtext = "Invalid package length";
						errId = JVX_ASSERT_INT_ID;
						errCode = JVX_DSP_ERROR_INTERNAL;

						res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						requestTransfer = c_true;
						jvx_remote_call_reset_local(hdl);

					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);

				}
				break;
			case JVX_RC_EXCHANGE_COMPLETE_REQUEST:
				// Are we in the right state?
				if (hdl->transfer_state == JVX_RC_STATE_TERMINATING_ACTION)
				{
					if (buf_api->paketsize == sizeof(jvxRcProtocol_simple_message))
					{
						hdl->params_request.transfer = *ptrSimpleMessage;

						jvx_remote_call_reset_local(hdl);
						hdl->transfer_state = JVX_RC_STATE_NONE;
						res = produce_response_general(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, JVX_RC_EXCHANGE_COMPLETE_RESPONSE, c_true);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						requestTransfer = c_true;
					}
					else
					{
						reportError = c_true;
						errtext = "Invalid package length";
						errId = JVX_ASSERT_INT_ID;
						errCode = JVX_DSP_ERROR_INTERNAL;

						res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						requestTransfer = c_true;
						jvx_remote_call_reset_local(hdl);
					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);

				}
				break;
			}
			break;

		case JVX_RC_OPERATE_MASTER:
#ifdef JVX_RC_SLAVE_CODE_ONLY
			res = JVX_DSP_ERROR_UNSUPPORTED;
			break;
#else
			switch (buf_api->purpose)
			{
			case JVX_RC_ERROR_TRIGGER:

				// Report error, mainly switch to start condifition				
				jvx_remote_call_reset_local(hdl);

				reportError = c_true;
				errtext = "Error package received";
				errId = JVX_ASSERT_INT_ID;
				errCode = JVX_DSP_ERROR_ABORT;

				break;

			case JVX_RC_EXCHANGE_START_RESPONSE:
				if (hdl->transfer_state == JVX_RC_STATE_STARTED)
				{
					res = jvx_remote_call_master_switch_state(&fldTransfer, &numBytesData, &numBytesTransmit, &requestTransfer, hdl, buf_api, unique_id);
					if (res != JVX_DSP_NO_ERROR)
					{
						if (res == JVX_DSP_ERROR_ABORT)
						{
							reportError = c_true;
							errtext = ptrErrorMessage->error;
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_REMOTE;
							jvx_remote_call_reset_local(hdl);
						}
						else
						{
							reportError = c_true;
							errtext = "Internal state machine failed";
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_INTERNAL;

							res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
							jvx_remote_call_reset_local(hdl);
						}
					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);
				}
				break;
			case JVX_RC_DATA_TRANSFER_IN_PRE_RESPONSE:
				if (hdl->transfer_state == JVX_RC_STATE_SENDING_PARAMETERS_IN_PRE)
				{
					res = jvx_remote_call_master_switch_state(&fldTransfer, &numBytesData, &numBytesTransmit, &requestTransfer, hdl, buf_api, unique_id);
					if (res != JVX_DSP_NO_ERROR)
					{
						if (res == JVX_DSP_ERROR_ABORT)
						{
							reportError = c_true;
							errtext = ptrErrorMessage->error;
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_REMOTE;
							jvx_remote_call_reset_local(hdl);
						}
						else
						{
							reportError = c_true;
							errtext = "Internal state machine failed";
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_INTERNAL;

							res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
							jvx_remote_call_reset_local(hdl);
						}
					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);
				}
				break;
			case JVX_RC_DATA_TRANSFER_IN_RESPONSE:
				if (hdl->transfer_state == JVX_RC_STATE_SENDING_PARAMETERS_IN)
				{
					res = jvx_remote_call_master_switch_state(&fldTransfer, &numBytesData, &numBytesTransmit, &requestTransfer, hdl, buf_api, unique_id);
					if (res != JVX_DSP_NO_ERROR)
					{
						if (res == JVX_DSP_ERROR_ABORT)
						{
							reportError = c_true;
							errtext = ptrErrorMessage->error;
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_REMOTE;
							jvx_remote_call_reset_local(hdl);
						}
						else
						{
							reportError = c_true;
							errtext = "Internal state machine failed";
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_INTERNAL;

							res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
							jvx_remote_call_reset_local(hdl);
						}
					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);
				}
				break;			
			case JVX_RC_DATA_TRANSFER_OUT_PRE_RESPONSE:
				if (hdl->transfer_state == JVX_RC_STATE_SENDING_PARAMETERS_OUT_PRE)
				{
					res = jvx_remote_call_master_switch_state(&fldTransfer, &numBytesData, &numBytesTransmit, &requestTransfer, hdl, buf_api, unique_id);
					if (res != JVX_DSP_NO_ERROR)
					{
						if (res == JVX_DSP_ERROR_ABORT)
						{
							reportError = c_true;
							errtext = ptrErrorMessage->error;
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_REMOTE;
							jvx_remote_call_reset_local(hdl);
						}
						else
						{
							reportError = c_true;
							errtext = "Internal state machine failed";
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_INTERNAL;

							res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
							jvx_remote_call_reset_local(hdl);
						}
					}
				}
				else
				{
					reportError = c_true;
					errtext = "Unexpected packet in current state";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);
				}
				break;
	
			case JVX_RC_TRIGGER_ACTION_RESPONSE:
				if (buf_api->paketsize == sizeof(jvxRcProtocol_response_message))
				{
					// Are we in the right state?
					if (hdl->transfer_state == JVX_RC_STATE_TRIGGER_ACTION)
					{
						// Deal with the result:
						hdl->helpers.success = ptrResponseMessage->success;
						hdl->helpers.tmpCnt0 = 0;
						hdl->helpers.tmpCnt1 = 0;
						if (hdl->params_request.numOutParams)
						{
							hdl->transfer_state = JVX_RC_STATE_RESPONDING_PARAMETERS_OUT;

							// Request next output channel
							res = produce_request_data_content(&fldTransfer, &numBytesData, &numBytesTransmit, hdl);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;						
						}
						else
						{
							hdl->transfer_state = JVX_RC_STATE_TERMINATING_ACTION;

							res = produce_request_simple(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, JVX_RC_EXCHANGE_COMPLETE_REQUEST);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
							hdl->helpers.tmpCnt0 = 0;
							hdl->helpers.tmpCnt1 = 0;
							break;
						}
					}
					else
					{
						// ERROR
					}
				}
				break;
			case JVX_RC_DATA_RESPONSE_OUT_RESPONSE:
				mySize = jvxDataFormat_size[hdl->output_params[hdl->helpers.tmpCnt0].description.format] *
					hdl->output_params[hdl->helpers.tmpCnt0].description.seg_length_x;
				if (buf_api->paketsize == (sizeof(jvxRcProtocol_data_message) + mySize))
				{
					if ((ptrDataMessage->param_id == hdl->helpers.tmpCnt0) && (ptrDataMessage->segy_id == hdl->helpers.tmpCnt1))
					{
						if (mySize)
						{
							memcpy(hdl->output_params[hdl->helpers.tmpCnt0].content[hdl->helpers.tmpCnt1], ptrLoadCopy, mySize);
						}

						hdl->helpers.tmpCnt1++;

						if (hdl->helpers.tmpCnt1 == hdl->output_params[hdl->helpers.tmpCnt0].description.seg_length_y)
						{
							hdl->helpers.tmpCnt0++;
							if (hdl->helpers.tmpCnt0 == hdl->params_request.numOutParams)
							{
								hdl->transfer_state = JVX_RC_STATE_TERMINATING_ACTION;
							}
						}

						if (hdl->transfer_state == JVX_RC_STATE_RESPONDING_PARAMETERS_OUT)
						{
							// Request next output frame
							res = produce_request_data_content(&fldTransfer, &numBytesData, &numBytesTransmit, hdl);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
						}
						else
						{
							// Send response message
							res = produce_request_simple(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, JVX_RC_EXCHANGE_COMPLETE_REQUEST);
							JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
							requestTransfer = c_true;
						}
					}
					else
					{
						reportError = c_true;
						errtext = "Wrong segy id output param";
						errId = JVX_ASSERT_INT_ID;
						errCode = JVX_DSP_ERROR_INTERNAL;

						res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
						JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
						requestTransfer = c_true;
						jvx_remote_call_reset_local(hdl);
					}
				}
				else
				{
					reportError = c_true;
					errtext = "Invalid package length";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;

					res = produce_response_error(&fldTransfer, &numBytesData, &numBytesTransmit, hdl, errtext, errCode, errId);
					JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);
					requestTransfer = c_true;
					jvx_remote_call_reset_local(hdl);

				}
				break;
			case JVX_RC_EXCHANGE_COMPLETE_RESPONSE:
				if (buf_api->paketsize >= sizeof(jvxRcProtocol_response_message))
				{
					if (ptrResponseMessage->success)
					{
						if (buf_api->paketsize == sizeof(jvxRcProtocol_response_message))
						{
							reportTransferComplete = c_true;
							errCode = JVX_DSP_NO_ERROR;
							errtext = "";
							
						}
						else
						{
							reportError = c_true;
							errtext = "Invalid package length";
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_INTERNAL;
						}
					}
					else
					{
						if (buf_api->paketsize == sizeof(jvxRcProtocol_error_response_message))
						{
							reportTransferComplete = c_true;
							errCode = (jvxDspBaseErrorType)ptrErrorMessage->err_code;
							errtext = ptrErrorMessage->error;
						}
						else
						{
							reportError = c_true;
							errtext = "Invalid package length";
							errId = JVX_ASSERT_INT_ID;
							errCode = JVX_DSP_ERROR_INTERNAL;
						}
					}
				}
				else
				{
					reportError = c_true;
					errtext = "Invalid package length";
					errId = JVX_ASSERT_INT_ID;
					errCode = JVX_DSP_ERROR_INTERNAL;
				}

				// Here, we need to reset the current transfer in any case!!
				jvx_remote_call_reset_local(hdl);
				break;
			}
			break;
#endif

		default:
				JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, 0);

		}

		// I am not sure why I actually needed this in the previous implementation..
		if (hdl->init_copy.theCallbacks.callback_unlock_receive_buffer)
		{
			res = hdl->init_copy.theCallbacks.callback_unlock_receive_buffer(hdl->init_copy.cb_priv, unique_id);
		}

		if (hdl->init_copy.theCallbacks.callback_unlock)
		{
			hdl->init_copy.theCallbacks.callback_unlock(hdl->init_copy.cb_priv);
		}

		if (reportError)
		{
			if(hdl->init_copy.theCallbacks.callback_error_report)
			{ 
				hdl->init_copy.theCallbacks.callback_error_report(hdl->init_copy.cb_priv, errtext, errCode, errId);
			}
		}

		if (requestTransfer)
		{
			if (hdl->init_copy.delayedSend)
			{
				if (send_ptr)
					*send_ptr = fldTransfer;
				if (num_bytes)
					*num_bytes = numBytesTransmit;
			}
			else
			{
				res = transmit_data_fld_deallocate(fldTransfer, numBytesTransmit, hdl);
				if (res != JVX_DSP_NO_ERROR)
				{
					// Report error, mainly switch to start condifition				
					jvx_remote_call_reset_local(hdl);

					reportError = c_true;
					errtext = "Unable to transmit packet";
					errId = JVX_ASSERT_INT_ID;
					errCode = res;
				}
			}
		}

		if (reportError)
		{
			if (hdl->init_copy.theCallbacks.callback_error_report)
			{
				hdl->init_copy.theCallbacks.callback_error_report(hdl->init_copy.cb_priv, errtext, errCode, errId);
			}
		}

		if (reportTransferComplete)
		{
			if (hdl->init_copy.theCallbacks.callback_transfer_terminated)
			{
				hdl->init_copy.theCallbacks.callback_transfer_terminated(hdl->init_copy.cb_priv,
					hdl->params_request.transfer.processingId,
					hdl->params_request.transfer.processingCounter,
					errCode, errtext);
			}

		}

		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType 
jvx_remote_call_init_transfer(jvx_remote_call* hdlIn, const char * description,
	jvxRCOneParameter* input_params, jvxSize num_input_params, 
	jvxRCOneParameter* output_params, jvxSize num_output_params, 
	jvxSize processing_id, jvxSize processing_counter)
{
	jvx_remote_call_private* hdl;
	jvxSize strsz;
	jvxCBool requestTransfer = c_false;
	jvxByte* fldTransfer = NULL;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxSize numBytesTransmit = 0;
	jvxSize numBytesData = 0;
	if (hdlIn)
	{
		hdl = (jvx_remote_call_private*)hdlIn->prv;

		if (hdl->init_copy.theCallbacks.callback_lock)
		{
			hdl->init_copy.theCallbacks.callback_lock(hdl->init_copy.cb_priv);
		}

		if (hdl->transfer_state == JVX_RC_STATE_NONE)
		{
			JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->input_params == NULL);
			JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->output_params == NULL);
			JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->params_request.numInParams == 0);
			JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, hdl->params_request.numOutParams == 0);

			hdl->input_params = input_params;
			hdl->params_request.numInParams = JVX_SIZE_UINT16( num_input_params);
			hdl->output_params = output_params;
			hdl->params_request.numOutParams = JVX_SIZE_UINT16(num_output_params);
			memset(hdl->params_request.description, 0, JVX_RC_LENGTH_REQUEST_TEXTFIELD);
			strsz = JVX_MIN(strlen(description), JVX_RC_LENGTH_REQUEST_TEXTFIELD - 1);
			memcpy(hdl->params_request.description, description, strsz);
			hdl->params_request.transfer.processingId = JVX_SIZE_UINT16(processing_id);
			hdl->params_request.transfer.processingCounter = JVX_SIZE_UINT16(processing_counter);

			requestTransfer = true;
			res = produce_request_start(&fldTransfer, &numBytesData, &numBytesTransmit, hdl);
			JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, res == JVX_DSP_NO_ERROR);

			hdl->transfer_state = JVX_RC_STATE_STARTED;
			hdl->helpers.tmpCnt0 = 0;
			hdl->helpers.tmpCnt1 = 0;
		}
		else
		{
			res = JVX_DSP_ERROR_WRONG_STATE;
		}

		if (hdl->init_copy.theCallbacks.callback_unlock)
		{
			hdl->init_copy.theCallbacks.callback_unlock(hdl->init_copy.cb_priv);
		}

		if (requestTransfer)
		{
			res = transmit_data_fld_deallocate(fldTransfer, numBytesTransmit, hdl);
		}
		return(res);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType
jvx_remote_call_transfer_buffer(jvx_remote_call* hdlIn, jvxByte* fldTransfer, jvxSize numBytesTransmit)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	const char* errtext = NULL;
	jvxDspBaseErrorType errCode = JVX_DSP_NO_ERROR;
	jvxSize errId = 0;
	if (hdlIn)
	{
		jvx_remote_call_private* hdl = (jvx_remote_call_private*)hdlIn->prv;
		res = transmit_data_fld_deallocate(fldTransfer, numBytesTransmit, hdl);
		if (res != JVX_DSP_NO_ERROR)
		{
			// Report error, mainly switch to start condifition				
			jvx_remote_call_reset_local(hdl);

			errtext = "Unable to transmit packet";
			errId = JVX_ASSERT_INT_ID;
			if (hdl->init_copy.theCallbacks.callback_error_report)
			{
				hdl->init_copy.theCallbacks.callback_error_report(hdl->init_copy.cb_priv, errtext, errCode, errId);
			}
		}
	}
	return res;
}
