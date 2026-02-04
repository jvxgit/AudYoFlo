#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "jvx.h"
#include "ayf-component-connect.h"

// ===========================================================================

extern "C"
{
    void cb_ayf_started(ayfVoidPvoidDefinition id, jvxHandle* priv, jvxHandle* purp)
    {
    }

    void* init_ayf_starter(int nChannelsIn, int nChannelsOut, int fsize, int samplerate, int ayfIdentSlot_node, int ayfIdentSlot_dev, const char* nmIniFile)
    {
        jvxErrorType res = JVX_NO_ERROR;

        struct ayfInitConnectStruct initStr;
        ayfInitConnectStruct_init(&initStr, ayfIdentSlot_node, ayfIdentSlot_dev);
        initStr.fptr = cb_ayf_started;
        initStr.priv = nullptr;
        initStr.fNameIniPtr = nmIniFile;

        struct ayfInitParamStruct  paramStr;
        ayfInitParamStruct_init(&paramStr, nChannelsIn,
            nChannelsOut, fsize, samplerate, ayfBufferInterleaveType::AYF_BUFFERS_NONINTERLEAVED, false);

        void* retVal = nullptr;
        AYF_FUNCTIONNAME_REDEFINE(ayf_cc_initModule, AYF_PROJECT_POSTFIX)(&retVal, &paramStr, (jvxHandle*)init_ayf_starter, &initStr);
            

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