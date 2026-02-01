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

    void* init_ayf_starter(int nChannelsIn, int nChannelsOut, int fsize, int samplerate, int ayfIdentSlot_node, int ayfIdentSlot_dev, jvxHandle* priv)
    {
        jvxErrorType res = JVX_NO_ERROR;
        int slotIds[2];
        slotIds[0] = ayfIdentSlot_node;
        slotIds[1] = ayfIdentSlot_dev;

        void* retVal = nullptr;
        AYF_FUNCTIONNAME_REDEFINE(ayf_cc_initModule, AYF_PROJECT_POSTFIX)(&retVal, nChannelsIn,
            nChannelsOut, fsize, samplerate, ayfBufferInterleaveType::AYF_BUFFERS_NONINTERLEAVED, false, (jvxHandle*)init_ayf_starter, slotIds, 2, 
            NULL,NULL,
            cb_ayf_started, priv);

        return retVal;
    }

    int term_ayf_starter(void* hdlArg)
    {
        jvxErrorType res = JVX_NO_ERROR;

        AYF_FUNCTIONNAME_REDEFINE(ayf_cc_termModule, AYF_PROJECT_POSTFIX)(hdlArg);
        return JVX_NO_ERROR;
    }


    int run_frame_ayf_starter(void* hdlArg, int nChannelsIn, int nChannelsOut, int fSize, jvxData** ptrsIn, jvxData** ptrsOut)
    {
        // Always a non-interleaved call
        AYF_FUNCTIONNAME_REDEFINE(ayf_cc_process_nil, AYF_PROJECT_POSTFIX)(hdlArg, ptrsIn, fSize, nChannelsIn, ptrsOut, fSize, nChannelsOut);

        return JVX_NO_ERROR;
    }
};