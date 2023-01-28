#include <stdlib.h>

#include "jvx_project_local_constraints.h"
#include "jvx_remote_call_cb.h"
#include "uart.h"

#define JVX_ASSERT_STRING_TOKEN "jvx_remote_call_cb.c"
#define JVX_ASSERT_INT_ID __LINE__

extern swi_hwi_disable();
extern swi_hwi_enable();

typedef struct {
    jvxSize bytesPrepend;
    jvxSize fldMinSize;
    jvxSize fldMaxSize;
} jvxFrameConstraintType;

#define BUF0_NUM 16
#define BUF0_SIZE 32
#define BUF1_NUM 8
#define BUF1_SIZE 128

// We need all possible buffers + one additional for response
#define BUF2_NUM MAX_NUM_INOUT_BUFFERS + 1
#define BUF2_SIZE MAX_LENGTH_SINGLE_PACKET

#ifdef NRMOTOR_TARGET_OMAPL137
		#pragma DATA_SECTION(mem_pool, ".userDefinedData")
#endif
struct {
        jvxByte mp_0[BUF0_NUM][BUF0_SIZE]; // actual buffer memory
        jvxByte flag_0[BUF0_NUM]; // usage flag table (1 means buffer is in use)
        jvxByte mp_1[BUF1_NUM][BUF1_SIZE];
        jvxByte flag_1[BUF1_NUM];
        jvxByte mp_2[BUF2_NUM][BUF2_SIZE];
        jvxByte flag_2[BUF2_NUM];
} mem_pool;


void mem_pool_init()
{
    memset(mem_pool.flag_0, 0, sizeof(jvxByte)*BUF0_NUM);
    memset(mem_pool.flag_1, 0, sizeof(jvxByte)*BUF1_NUM);
    memset(mem_pool.flag_2, 0, sizeof(jvxByte)*BUF2_NUM);
}

jvxByte* mem_pool_alloc(jvxSize size)
{
    jvxSize i;
    if (size <= BUF0_SIZE) {
        // look for free buffer
        for (i = 0; i < BUF0_NUM; i++) {
            if (!mem_pool.flag_0[i]) {
                mem_pool.flag_0[i] = 1;
                return mem_pool.mp_0[i];
            }
        }
    }
    if (size <= BUF1_SIZE) {
        // look for free buffer
        for (i = 0; i < BUF1_NUM; i++) {
            if (!mem_pool.flag_1[i]) {
                mem_pool.flag_1[i] = 1;
                return mem_pool.mp_1[i];
            }
        }
    }
    if (size <= BUF2_SIZE) {
        // look for free buffer
        for (i = 0; i < BUF2_NUM; i++) {
            if (!mem_pool.flag_2[i]) {
                mem_pool.flag_2[i] = 1;
                return mem_pool.mp_2[i];
            }
        }
    }
    return 0; // error
}

void mem_pool_free(jvxByte* buf)
{
    jvxSize i;
    for (i = 0; i < BUF0_NUM; i++) {
        if (mem_pool.mp_0[i] == buf) {
            mem_pool.flag_0[i] = 0;
            return;
        }
    }
    for (i = 0; i < BUF1_NUM; i++) {
        if (mem_pool.mp_1[i] == buf) {
            mem_pool.flag_1[i] = 0;
            return;
        }
    }
    for (i = 0; i < BUF2_NUM; i++) {
        if (mem_pool.mp_2[i] == buf) {
            mem_pool.flag_2[i] = 0;
            return;
        }
    }
}

// compares two jvxRcProtocol_parameter_description
#define COMPARE_RC_PARAMS(a, b) (a.description.format != b.format && \
                                 a.description.float_precision_id == b.float_precision_id && \
                                 a.description.seg_length_y == b.seg_length_y && \
                                 a.description.seg_length_x == b.seg_length_x)

jvx_remote_call hdlRemoteCall;
jvxFrameConstraintType myFrameConstraint;

void jvx_remote_call_cb_init()
{
    // init memory pool
    mem_pool_init();

    hdlRemoteCall.prmInit.cb_priv = NULL;
    hdlRemoteCall.prmInit.operation_mode = JVX_RC_OPERATE_SLAVE;
    hdlRemoteCall.prmInit.delayedSend = false;

    // setup remote call callbacks
    hdlRemoteCall.prmInit.theCallbacks.callback_unlock_receive_buffer = jvx_rc_unlock_receive_buffer;
    hdlRemoteCall.prmInit.theCallbacks.callback_allocate_memory = jvx_rc_allocate_memory;
    hdlRemoteCall.prmInit.theCallbacks.callback_deallocate_memory = jvx_rc_deallocate_memory;
    hdlRemoteCall.prmInit.theCallbacks.callback_accept_init_request = jvx_rc_accept_init_request;
    hdlRemoteCall.prmInit.theCallbacks.callback_send_buffer = jvx_rc_send_buffer;
    hdlRemoteCall.prmInit.theCallbacks.callback_process_request = jvx_rc_process_request;
    hdlRemoteCall.prmInit.theCallbacks.callback_lock = jvx_rc_lock;
    hdlRemoteCall.prmInit.theCallbacks.callback_unlock = jvx_rc_unlock;
    hdlRemoteCall.prmInit.theCallbacks.callback_error_report = jvx_rc_report_local_error;
    hdlRemoteCall.prmInit.theCallbacks.callback_transfer_terminated = jvx_rc_report_transfer_terminated;

    myFrameConstraint.bytesPrepend = 0;
    myFrameConstraint.fldMinSize = 0;
    myFrameConstraint.fldMaxSize = BUF2_SIZE;
}

jvxDspBaseErrorType jvx_rc_accept_init_request(jvxHandle* hdl, const char* description, jvxSize numInParams, jvxSize numOutParams, jvxSize processingId)
{
    jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
    jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
    //resL = reportActionRequest(description, numInParams, numOutParams, processingId);
    // TODO implement
    if (resL != JVX_DSP_NO_ERROR) {
        res = JVX_DSP_ERROR_UNSUPPORTED;
    }
    return res;
}

jvxDspBaseErrorType jvx_rc_unlock_receive_buffer(jvxHandle* hdl, jvxSize unique_id)
{
    uart_unlockReceiverBuffer();
    return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_rc_allocate_memory(jvxHandle* hdl, jvx_rc_allocation_purpose purp, jvxHandle** ptrReturn, jvxSize* offset,
		jvxSize numElements, jvxSize* szFldAllocated, jvxDataFormat form)
{
    jvxSize sz = 0;
    jvxSize szElm = 0;
    jvxSize szFld = 0;
    if (ptrReturn && offset) {
        switch (purp) {
            case JVX_RC_ALLOCATE_PURPOSE_INPUT_FIELD:
            case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_FIELD:
            case JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD_PTR:
            case JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD:
            case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD_PTR:
            case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD:

            	if(offset)
            		*offset = 0;
            	szElm = jvxDataFormat_size[form];
            	JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, szElm != 0);
            	szFld = szElm * numElements;

                *ptrReturn = (jvxHandle*)mem_pool_alloc(sizeof(jvxByte)*szFld);
                JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, *ptrReturn != 0);
                memset((void*) *ptrReturn, (int) 0, (size_t) (sizeof(jvxByte) * szFld));

                break;

            case JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD:

            	sz = myFrameConstraint.bytesPrepend;
            	if (offset)
            		*offset = sz;

            	szElm = jvxDataFormat_size[form];
            	JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, szElm != 0);
            	szFld = szElm * numElements + sz;

                if (JVX_CHECK_SIZE_SELECTED(myFrameConstraint.fldMinSize)) {
                    szFld = JVX_MAX(myFrameConstraint.fldMinSize, szFld);
                }
                if (JVX_CHECK_SIZE_SELECTED(myFrameConstraint.fldMaxSize)) {
                    JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, szFld <= myFrameConstraint.fldMaxSize);
                }
                *ptrReturn = (jvxHandle*)mem_pool_alloc(sizeof(jvxByte)*szFld);
                JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, *ptrReturn != 0);
                memset(*ptrReturn, 0, sizeof(jvxByte) * szFld);

                break;

            default:
                JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, 0);
        }

		if(szFldAllocated)
		{
			* szFldAllocated = szFld;
		}
        return JVX_DSP_NO_ERROR;
    }
    return (JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_rc_deallocate_memory(jvxHandle* hdl, jvx_rc_allocation_purpose purp, jvxHandle* ptrReturn, jvxSize szFldAllocated)
{
    if (ptrReturn) {
        switch (purp) {
            case JVX_RC_ALLOCATE_PURPOSE_INPUT_FIELD:
            case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_FIELD:
            case JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD:
            case JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD_PTR:
            case JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD:
            case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD_PTR:
            case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD:
                if (ptrReturn)
                    mem_pool_free(ptrReturn);
                break;

            default:
                JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, 0);
        }

        return JVX_DSP_NO_ERROR;
    }
    return (JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_rc_send_buffer(jvxHandle* hdl, jvxByte* resp, jvxSize numBytes)
{
    uart_initializeTransfer( resp, numBytes);
    return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_rc_process_request(jvxHandle* hdl, const char* description, jvxRCOneParameter* paramsIn, jvxSize numParamsIn, jvxRCOneParameter* paramsOut,
		jvxSize numParamsOut, jvxSize processingId, jvxCBool* immediateResponse)
{
    jvxSize i, j;
    jvxDspBaseErrorType res = JVX_DSP_ERROR_INVALID_ARGUMENT;
    int localHwiDisableOnExec;

    // search remote call function wrapper
    for (i = 0; i < rc_function_mapping_length; i++)
    {
        const rc_function_mapping *fm = &function_mapping[i];
        const jvxRcProtocol_parameter_description *fm_paramsIn  = fm->paramsInDesc;
        const jvxRcProtocol_parameter_description *fm_paramsOut = fm->paramsOutDesc;
        if (strcmp(description, fm->description) == 0)
        {
            // check for matching numbers of input and output parameters
            if (numParamsIn != fm->numParamsIn || numParamsOut != fm->numParamsOut)
                break;

            // check for matching input parameter types
            for (j = 0; j < numParamsIn; j++)
            {
                if (!COMPARE_RC_PARAMS(paramsIn[j], fm_paramsIn[j]))
                    break;
            }

            // check for matching output parameter types
            for (j = 0; j < numParamsOut; j++)
            {
                if (!COMPARE_RC_PARAMS(paramsOut[j], fm_paramsOut[j]))
                    break;
            }

            // disable HWI/SWI if requested
            localHwiDisableOnExec = rc_disable_hwi_on_exec;

            if(localHwiDisableOnExec)
            {
            	swi_hwi_disable();
            }

            // call the function wrapper
            res = fm->function(paramsIn, paramsOut);
            // stop time measurement

            // re-enable HWI/SWI
            if(localHwiDisableOnExec)
            {
            	swi_hwi_enable();
            }

            break;
        }
    }

    return res;
}

jvxDspBaseErrorType jvx_rc_lock(jvxHandle* hdl)
{
    return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_rc_unlock(jvxHandle* hdl)
{
    return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_rc_report_local_error(jvxHandle* hdl, const char* text, jvxDspBaseErrorType errCode, jvxSize errId)
{
    return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_rc_report_transfer_terminated(jvxHandle* hdl, jvxSize processing_id, jvxSize processing_counter, jvxDspBaseErrorType resulTransfer, const char* err_description)
{
    return JVX_DSP_NO_ERROR;
}
