#ifndef __JVX_REMOTE_CALL_HELPERS_H__
#define __JVX_REMOTE_CALL_HELPERS_H__

#include "jvx_remote_call.h"

// =========================================================================================================

#define JVX_RC_ERROR_TRIGGER (JVX_RC_ERROR | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER)

#define JVX_RC_EXCHANGE_START_REQUEST  (JVX_RC_EXCHANGE_START | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST)
#define JVX_RC_EXCHANGE_START_RESPONSE  (JVX_RC_EXCHANGE_START | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)

#define JVX_RC_DATA_TRANSFER_IN_PRE_REQUEST (JVX_RC_DATA_TRANSFER_IN_PRE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST)
#define JVX_RC_DATA_TRANSFER_IN_PRE_RESPONSE (JVX_RC_DATA_TRANSFER_IN_PRE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)

#define JVX_RC_DATA_TRANSFER_IN_REQUEST (JVX_RC_DATA_TRANSFER_IN | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST)
#define JVX_RC_DATA_TRANSFER_IN_RESPONSE (JVX_RC_DATA_TRANSFER_IN | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)

#define JVX_RC_DATA_TRANSFER_OUT_PRE_REQUEST (JVX_RC_DATA_TRANSFER_OUT_PRE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST)
#define JVX_RC_DATA_TRANSFER_OUT_PRE_RESPONSE (JVX_RC_DATA_TRANSFER_OUT_PRE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)

#define JVX_RC_TRIGGER_ACTION_REQUEST (JVX_RC_TRIGGER_ACTION | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST)
#define JVX_RC_TRIGGER_ACTION_RESPONSE (JVX_RC_TRIGGER_ACTION | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)

#define JVX_RC_DATA_RESPONSE_OUT_REQUEST (JVX_RC_DATA_RESPONSE_OUT | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST)
#define JVX_RC_DATA_RESPONSE_OUT_RESPONSE (JVX_RC_DATA_RESPONSE_OUT | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)

#define JVX_RC_EXCHANGE_COMPLETE_REQUEST (JVX_RC_EXCHANGE_COMPLETE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST)
#define JVX_RC_EXCHANGE_COMPLETE_RESPONSE (JVX_RC_EXCHANGE_COMPLETE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)

// =========================================================================================================

typedef struct
{
	jvx_remote_call_prmInit init_copy;
	//jvx_rc_operate_callbacks* callbacks;
	//jvxHandle* priv;
	//jvxRCOperationMode operation_mode;
	jvxRcProtocol_start_message params_request;
	jvx_remote_call_state transfer_state;
	jvxRCOneParameter* input_params;
	jvxRCOneParameter* output_params;
	struct
	{
		jvxSize tmpCnt0;
		jvxSize tmpCnt1;
		jvxCBool success;
	} helpers;
} jvx_remote_call_private;

// =========================================================================================================

jvxDspBaseErrorType transmit_data_fld_deallocate(jvxByte* resp_byte, jvxSize szFldTransmit, jvx_remote_call_private* hdl);
jvxDspBaseErrorType produce_request_start(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransmit, jvx_remote_call_private* hdl);
jvxDspBaseErrorType produce_request_simple(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransmit, jvx_remote_call_private* hdl, jvxUInt16 tp);
jvxDspBaseErrorType produce_request_data_content(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransmit, jvx_remote_call_private* hdl);
jvxDspBaseErrorType produce_request_data_descriptor(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransmit, jvx_remote_call_private* hdl, jvxCBool send_input);

// ===============================================================================

jvxDspBaseErrorType produce_transfer_data_content(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransmit, jvx_remote_call_private* hdl,
	jvxCBool send_input);

// ===============================================================================

jvxDspBaseErrorType produce_response_general(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransmit,
	jvx_remote_call_private* hdl, jvxUInt16 tp, jvxCBool suc);
jvxDspBaseErrorType produce_response_error(jvxByte** resp_byte, jvxSize* szFldData, jvxSize* szFldTransmit, 
	jvx_remote_call_private* hdl, const char* errdescr, jvxDspBaseErrorType errcode, jvxSize errid);

// ===============================================================================

#endif
