#ifndef __JVX_REMOTE_CALL_H__
#define __JVX_REMOTE_CALL_H__

#include "jvx_dsp_base.h"
#include "typedefs/network/TjvxProtocol.h"
#include "typedefs/tools/TjvxRemoteCall.h"

typedef enum
{
	JVX_RC_STATE_NONE = 0,
	JVX_RC_STATE_STARTED = 1,
	JVX_RC_STATE_SENDING_PARAMETERS_IN_PRE = 2,
	JVX_RC_STATE_SENDING_PARAMETERS_IN = 3,
	JVX_RC_STATE_SENDING_PARAMETERS_OUT_PRE = 4,
	JVX_RC_STATE_TRIGGER_ACTION = 5,
	JVX_RC_STATE_RESPONDING_PARAMETERS_OUT = 6,
	JVX_RC_STATE_TERMINATING_ACTION = 7,
	JVX_RC_STATE_WAIT_TO_COMPLETE = 8,

	JVX_RC_STATE_OUTGOING_TIMEOUT = 9
} jvx_remote_call_state;

typedef enum
{
	JVX_RC_ALLOCATE_PURPOSE_NONE = 0,
	JVX_RC_ALLOCATE_PURPOSE_INPUT_FIELD,
	JVX_RC_ALLOCATE_PURPOSE_OUTPUT_FIELD,
	JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD,
	JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD_PTR,
	JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD,
	JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD_PTR,
	JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD
} jvx_rc_allocation_purpose;

typedef jvxDspBaseErrorType(*jvx_rc_accept_init_request_fptr)(jvxHandle* hdl, const char* description, jvxSize numInParams, jvxSize numOutParams, jvxSize processingId);
typedef jvxDspBaseErrorType(*jvx_rc_unlock_receive_buffer_fptr)(jvxHandle* hdl, jvxSize unique_id);
typedef jvxDspBaseErrorType(*jvx_rc_allocate_memory_fptr)(jvxHandle* hdl, jvx_rc_allocation_purpose, jvxHandle** ptrReturn, jvxSize* offset, jvxSize numElements, jvxSize* numBytesTransfer, jvxDataFormat form);
typedef jvxDspBaseErrorType(*jvx_rc_deallocate_memory_fptr)(jvxHandle* hdl, jvx_rc_allocation_purpose, jvxHandle* ptrReturn, jvxSize szFldAllocated);
typedef jvxDspBaseErrorType(*jvx_rc_send_buffer_fptr)(jvxHandle* hdl, jvxByte* resp, jvxSize szFld);
typedef jvxDspBaseErrorType(*jvx_rc_process_request_fptr)(jvxHandle* hdl, const char* description, jvxRCOneParameter* paramsIn, jvxSize numParamsIn, 
														  jvxRCOneParameter* paramsOut, jvxSize numParamsOut, jvxSize processingId,
														  jvxCBool* immediateResponse);
typedef jvxDspBaseErrorType(*jvx_rc_lock_fptr)(jvxHandle* hdl);
typedef jvxDspBaseErrorType(*jvx_rc_unlock_fptr)(jvxHandle* hdl);
typedef jvxDspBaseErrorType(*jvx_rc_report_local_error_fptr)(jvxHandle* hdl, const char* text, jvxDspBaseErrorType errCode, jvxSize errId);
typedef jvxDspBaseErrorType(*jvx_rc_report_transfer_terminated_fptr)(jvxHandle* hdl, jvxSize processing_id, jvxSize processing_counter, jvxDspBaseErrorType resulTransfer, const char* err_description);

typedef struct
{
	jvx_rc_unlock_receive_buffer_fptr callback_unlock_receive_buffer;
	jvx_rc_allocate_memory_fptr callback_allocate_memory;
	jvx_rc_deallocate_memory_fptr callback_deallocate_memory;
	jvx_rc_accept_init_request_fptr callback_accept_init_request;
	jvx_rc_send_buffer_fptr callback_send_buffer;
	jvx_rc_process_request_fptr callback_process_request;
	jvx_rc_lock_fptr callback_lock;
	jvx_rc_unlock_fptr callback_unlock;
	jvx_rc_report_local_error_fptr callback_error_report;
	jvx_rc_report_transfer_terminated_fptr callback_transfer_terminated;
} jvx_rc_operate_callbacks;

typedef struct
{
	jvxCBool delayedSend;
	jvxRCOperationMode operation_mode;
	jvx_rc_operate_callbacks theCallbacks;
	jvxHandle* cb_priv;
} jvx_remote_call_prmInit;

// main struct
typedef struct
{

	jvxHandle* prv;
	jvx_remote_call_prmInit prmInit;
} jvx_remote_call;

jvxDspBaseErrorType jvx_remote_call_initCfg(jvx_remote_call* hdl);
jvxDspBaseErrorType jvx_remote_call_init(jvx_remote_call* hdl);
jvxDspBaseErrorType jvx_remote_call_incoming(jvx_remote_call* hdl, jvxProtocolHeader* buf_api, jvxSize unique_id, jvxByte** send_ptr, jvxSize* numBytes);
jvxDspBaseErrorType jvx_remote_call_init_transfer(jvx_remote_call* hdl, const char * description,
	jvxRCOneParameter* input_params, jvxSize num_input_params,
	jvxRCOneParameter* output_params, jvxSize num_output_params,
	jvxSize processing_id, jvxSize processing_counter);
jvxDspBaseErrorType jvx_remote_call_transfer_buffer(jvx_remote_call* hdlIn, jvxByte* fldTransfer, jvxSize numBytesTransmit);
jvxDspBaseErrorType jvx_remote_call_terminate(jvx_remote_call* hdl);
jvxDspBaseErrorType jvx_remote_call_reset(jvx_remote_call* hdl);


#endif
