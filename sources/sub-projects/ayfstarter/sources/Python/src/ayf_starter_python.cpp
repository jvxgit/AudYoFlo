#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "jvx.h"
#include "ayf-component-connect.h"

// ===========================================================================
struct ayfLocalDataContext
{
    JVX_THREAD_ID threadIdStarted = JVX_SIZE_UNSELECTED;
    std::function<void(int, void*)> cbPython;
    void* hdl;
};

extern "C"
{
    void cb_ayf_multipurpose(ayfVoidPvoidDefinition purp, jvxHandle* priv, jvxHandle* purpSpecific)
    {
        ayfLocalDataContext* localCtxt = (ayfLocalDataContext*)priv;
        IjvxDataConnectionProcess* proc = (IjvxDataConnectionProcess*)purpSpecific;
        JVX_THREAD_ID localThreadId = JVX_GET_CURRENT_THREAD_ID();

        switch (purp)
        {
        case ayfVoidPvoidDefinition::AYF_VOID_PVOID_ID_REPORT_NO_BINDING:
            break;

        case ayfVoidPvoidDefinition::AYF_VOID_PVOID_ID_BEFORE_START:

            // Run the Python function if in the same thread            
            if (localCtxt)
            {
                if (localCtxt->threadIdStarted == localThreadId)
                {
                    if (localCtxt->cbPython)
                    {
                        localCtxt->cbPython((int)purp, purpSpecific);
                    }
                }
                else
                {
                    // We are not alowed to trigger a python function as we come in a side thread
                }
            }
            // Use the pointer IjvxDataConnectionProces to access all involved components if desired
            break;

        case ayfVoidPvoidDefinition::AYF_VOID_PVOID_ID_STARTED:
            break;

        case ayfVoidPvoidDefinition::AYF_VOID_PVOID_ID_TERMINATE:

            // From here on, the private memory is not referenced anymore
            JVX_SAFE_DELETE_OBJ(localCtxt);
            break;
        }
    }
    
    void* init_ayf_starter(int nChannelsIn, int nChannelsOut, int fsize, int samplerate, int ayfIdentSlot_node, int ayfIdentSlot_dev, const char* nmIniFile, std::function<void(int, void*)> cb)
    {
        jvxErrorType res = JVX_NO_ERROR;

        struct ayfInitConnectStruct initStr;
        ayfInitConnectStruct_init(&initStr, ayfIdentSlot_node, ayfIdentSlot_dev);
        initStr.fptr_multi_purpose = cb_ayf_multipurpose;
        initStr.priv = nullptr;        
        initStr.fNameIniPtr = nmIniFile;
        initStr.doNotLoadProxy = true;
       
        ayfLocalDataContext* privCtxt = nullptr;
        JVX_DSP_SAFE_ALLOCATE_OBJECT(privCtxt, ayfLocalDataContext);
        privCtxt->threadIdStarted = JVX_GET_CURRENT_THREAD_ID();
        privCtxt->cbPython = std::move(cb);
        privCtxt->hdl = nullptr;

        initStr.priv = privCtxt;

        struct ayfInitParamStruct  paramStr;
        ayfInitParamStruct_init(&paramStr, nChannelsIn,
            nChannelsOut, fsize, samplerate, ayfBufferInterleaveType::AYF_BUFFERS_NONINTERLEAVED, false);

        void* retVal = nullptr;
        AYF_FUNCTIONNAME_REDEFINE(ayf_cc_initModule, AYF_PROJECT_POSTFIX)(&retVal, &paramStr, (jvxHandle*)init_ayf_starter, &initStr);
        privCtxt->hdl = retVal;

        return retVal;
    }

    int term_ayf_starter(void* hdlArg)
    {
        jvxErrorType res = JVX_NO_ERROR;

        AYF_FUNCTIONNAME_REDEFINE(ayf_cc_termModule, AYF_PROJECT_POSTFIX)(hdlArg);
        return JVX_NO_ERROR;
    }


    int run_frame_ayf_starter(void* hdlArg, int nChannelsIn, int nChannelsOut, int fSize, 
        pybind11::array_t<jvxData> ptrIn, pybind11::array_t<jvxData> ptrOut)
    {
        jvxData* buf_in = ptrIn.mutable_data();
        jvxData* buf_out = ptrOut.mutable_data();
        // Always a non-interleaved call
        AYF_FUNCTIONNAME_REDEFINE(ayf_cc_process_il, AYF_PROJECT_POSTFIX)(hdlArg, buf_in, fSize, nChannelsIn, buf_out, fSize, nChannelsOut);

        return JVX_NO_ERROR;
    }
};