#ifndef _JVX_REMOTE_CALL_CB_H__
#define _JVX_REMOTE_CALL_CB_H__

#include "jvx_remote_call.h"

extern jvx_remote_call hdlRemoteCall;
extern jvx_rc_operate_callbacks myCallbacks;

void jvx_remote_call_cb_init();

// callback receivers
jvxDspBaseErrorType jvx_rc_accept_init_request(jvxHandle* hdl, const char* description, jvxSize numInParams, jvxSize numOutParams, jvxSize processingId);
jvxDspBaseErrorType jvx_rc_unlock_receive_buffer(jvxHandle* hdl, jvxSize unique_id);
jvxDspBaseErrorType jvx_rc_allocate_memory(jvxHandle* hdl, jvx_rc_allocation_purpose purp, jvxHandle** ptrReturn, jvxSize* offset, jvxSize numElements, jvxSize* szFldAllocated, jvxDataFormat form);
jvxDspBaseErrorType jvx_rc_deallocate_memory(jvxHandle* hdl, jvx_rc_allocation_purpose purp, jvxHandle* ptrReturn, jvxSize szFldAllocated);
jvxDspBaseErrorType jvx_rc_send_buffer(jvxHandle* hdl, jvxByte* resp, jvxSize numBytes);
jvxDspBaseErrorType jvx_rc_process_request(jvxHandle* hdl, const char* description, jvxRCOneParameter* paramsIn, jvxSize numParamsIn, jvxRCOneParameter* paramsOut, jvxSize numParamsOut, jvxSize processingId, jvxCBool* immediateResponse);
jvxDspBaseErrorType jvx_rc_lock(jvxHandle* hdl);
jvxDspBaseErrorType jvx_rc_unlock(jvxHandle* hdl);
jvxDspBaseErrorType jvx_rc_report_local_error(jvxHandle* hdl, const char* text, jvxDspBaseErrorType errCode, jvxSize errId);
jvxDspBaseErrorType jvx_rc_report_transfer_terminated(jvxHandle* hdl, jvxSize processing_id, jvxSize processing_counter, jvxDspBaseErrorType resulTransfer, const char* err_description);


// the macros below help generating the remote call function mapping
#define RC_I(name) name##_paramsIn
#define RC_O(name) name##_paramsOut
// function parameter declaration macros
#define RC_I_DEF(name) const jvxRcProtocol_parameter_description RC_I(name)[]
#define RC_O_DEF(name) const jvxRcProtocol_parameter_description RC_O(name)[]
// number of argument macros
#define RC_NUM_I(name) (sizeof(RC_I(name))/sizeof(RC_I(name)[0]))
#define RC_NUM_O(name) (sizeof(RC_O(name))/sizeof(RC_O(name)[0]))
// function mapping macros
#define RC_MAP_IO(name) {#name, &name, RC_NUM_I(name), RC_I(name), RC_NUM_O(name), RC_O(name)}
#define RC_MAP_I(name) {#name, &name, RC_NUM_I(name), RC_I(name), 0, NULL}
#define RC_MAP_O(name) {#name, &name, 0, NULL, RC_NUM_O(name), RC_O(name)}
#define RC_MAP(name) {#name, &name, 0, NULL, 0, NULL}

typedef jvxDspBaseErrorType(*rc_function_wrapper)(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut);

typedef struct {
    const char* description;
    rc_function_wrapper function;
    jvxSize numParamsIn;
    const jvxRcProtocol_parameter_description *paramsInDesc;
    jvxSize numParamsOut;
    const jvxRcProtocol_parameter_description *paramsOutDesc;
} rc_function_mapping;

// these variables implement the actual remote calls
extern const rc_function_mapping function_mapping[];
extern const jvxSize rc_function_mapping_length;
extern volatile int rc_disable_hwi_on_exec;

#endif
