/*
 * ==============================================
 * Filename: user_GenericDebugger.c
 * ==============================================
 * Purpose: User entry point for generic debugging.
 *          Every remote call which is initiated from the
 *			RS 232 connected PC at first causes a call of
 *			the function reportActionRequest_uart. If the
 * 			actionName is unexpected, the remote function can be
 * 			terminated here already by returning a "0". However,
 *			returning 1 is always the better idea.
 *			In the second step the reportActionProcess_uart
 *			function is called. Here, the user will find the
 * 			actionName to address a specific functionality as
 *			well as the input and output fields.
 *
 * 			NOTE: Dataformat "RTP_DATAFORMAT_DOUBLE" does
 * 			not work since the 64 bit values require a certain alignment
 *			which is not considered. However, float will do the job for you!
 * 			COPYRIGHT HK, Javox, 2011
 */

#include <log.h>
#include <string.h>
#include <c6x.h>

#include "user_remoteCall.h"
#include "jvx_remote_call_cb.h"
#include "jvx_platform.h"

// ================================================================================
// remote call function includes
#include "projectAcfg.h"
#include "rtp_fft_core.h"
#include "nrMotor_dsp.h"
#include "nrMotor_dsp_intern.h"
#include "rtp_general_dsp_gen.h"
#include "rtp_fft_nr_dsp_gen.h"
#include "rtp_fft_rpm_dsp_gen.h"
#include "rtp_ifft_nr_dsp_gen.h"
#include "rtp_general_dsp_gen.h"

extern float FFT_NR_WINDOW[FFT_NR_LBUF];
extern float FFT_NR_BUFFER_0[FFT_NR_LBUF];
extern float FFT_NR_BUFFER_1[FFT_NR_LBUF];
extern int FFT_NR_IDXWRITE0;
extern int FFT_NR_IDXWRITE1;
extern float FFT_RPM_WINDOW[FFT_RPM_LBUF];
extern float FFT_RPM_BUFFER_0[FFT_RPM_LBUF];
extern int FFT_RPM_IDXWRITE0;
extern float IFFT_NR_BUFFER_0[IFFT_NR_LBUF];
extern float IFFT_NR_WINDOW[IFFT_NR_LBUF];
extern int IFFT_NR_IDXWRITE0;
extern float NRMOTOR_FFT_RPM_BUFFER[NRMOTOR_FFTLENGTH];
extern float NRMOTOR_IFFT_NR_BUFFER[NRMOTOR_FFTLENGTH];
extern float NRMOTOR_FFT_NR_BUFFER[NRMOTOR_FFTLENGTH];
extern float NRMOTOR_CIRC_BUFFER_DELAY_SIGNAL[NRMOTOR_L_CIRCBUFFER_DELAY_AUDIO_NR * NRMOTOR_FRAMESHIFT];
extern int NRMOTOR_CIRCBUFFER_NR_INDEX;

//float buf_40_1_intern[40];
#ifdef NRMOTOR_TARGET_OMAPL137
#pragma DATA_SECTION(buf_513_1_intern, ".userDefinedDataTC")
#endif
jvxData buf_513_1_intern[513];

#ifdef NRMOTOR_TARGET_OMAPL137
#pragma DATA_SECTION(buf_513_2_intern, ".userDefinedDataTC")
#endif
jvxData buf_513_2_intern[513];

#ifdef NRMOTOR_TARGET_OMAPL137
#pragma DATA_SECTION(buf_513_3_intern,".userDefinedDataTC")
#endif
jvxData buf_513_3_intern[513];

volatile int rc_disable_hwi_on_exec = 0;

static jvxTimeStampData refClk;
static jvxInt64 lastMeasuredClock = 0;

/* ***************************************************************************************************** *
 * ****************************** how to add a new remote call function ******************************** *
 * ***************************************************************************************************** */

/*
 * 1) implement wrapper function as shown in this example for a function named new_func:
-----------------------------------------------------------------------------------------------------------------------
RC_I_DEF(new_func) = {{.., .., .., ..}, ..}; // entries are of type jvxRcProtocol_parameter_description
RC_O_DEF(new_func) = {{.., .., .., ..}, ..}; // which is {format, float_precision_id, seg_length_y, seg_length_x}
static jvxDspBaseErrorType new_func(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    // use paramsIn[0].content to access the first input parameter's content
    // use paramsOut[1].description to access the second output parameter's description
    return JVX_DSP_NO_ERROR;
}
-----------------------------------------------------------------------------------------------------------------------
 * 2) add function mapping to function_mapping array at the bottom of the file
 *    RC_MAP_IO(new_func) for functions with input and output parameters
 *    RC_MAP_I(new_func)  for input only
 *    RC_MAP_O(new_func)  for output only
 *    RC_MAP(new_func)    neither input nor output
 */

/* ***************************************************************************************************** *
 * ********************************** remote call function wrappers ************************************ *
 * ***************************************************************************************************** */

RC_I_DEF(fft_test) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 1024}};
RC_O_DEF(fft_test) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 2, 513}};
static jvxDspBaseErrorType fft_test(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    float* in = paramsIn[0].content[0];
    float* out_re = paramsOut[0].content[0];
    float* out_im = paramsOut[0].content[1];

    JVX_GET_TICKCOUNT_CLK_SETREF(&refClk);
    rtp_execute_fft_1024_dsp(in, out_re, out_im);
    lastMeasuredClock = JVX_GET_TICKCOUNT_CLK_GET(&refClk);

    return JVX_DSP_NO_ERROR;
}

RC_I_DEF(fft_fw_rpm) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 40}};
RC_O_DEF(fft_fw_rpm) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 2, 513}};
static jvxDspBaseErrorType fft_fw_rpm(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    float* in = paramsIn[0].content[0];
    float* out_re = paramsOut[0].content[0];
    float* out_im = paramsOut[0].content[1];

    // Copy to internal memory for higher processing speed
    memcpy(buf_513_3_intern, in, jvxDataFormat_size[paramsIn[0].description.format]*paramsIn[0].description.seg_length_x);

    JVX_GET_TICKCOUNT_CLK_SETREF(&refClk);
    rtp_fft_framework_process_dsp(buf_513_3_intern, buf_513_1_intern, buf_513_2_intern, NRMOTOR_FFT_RPM_BUFFER, FFT_RPM_BUFFER_0, FFT_RPM_WINDOW, &FFT_RPM_IDXWRITE0, FFT_RPM_LBUF);
    lastMeasuredClock = JVX_GET_TICKCOUNT_CLK_GET(&refClk);

    memcpy(out_re, buf_513_1_intern, jvxDataFormat_size[paramsOut[0].description.format]*paramsOut[0].description.seg_length_x);
    memcpy(out_im, buf_513_2_intern, jvxDataFormat_size[paramsOut[0].description.format]*paramsOut[0].description.seg_length_x);

    return JVX_DSP_NO_ERROR;
}

RC_I_DEF(fft_fw_nr) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 40}};
RC_O_DEF(fft_fw_nr) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 2, 513}};
static jvxDspBaseErrorType fft_fw_nr(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    float* in = paramsIn[0].content[0];
    float* out_re = paramsOut[0].content[0];
    float* out_im = paramsOut[0].content[1];

    // Copy to internal memory for higher processing speed
    memcpy(buf_513_3_intern, in, jvxDataFormat_size[paramsIn[0].description.format]*paramsIn[0].description.seg_length_x);

    JVX_GET_TICKCOUNT_CLK_SETREF(&refClk);
    rtp_fft_framework_process_dsp(buf_513_3_intern,  buf_513_1_intern, buf_513_2_intern, NRMOTOR_FFT_NR_BUFFER, FFT_NR_BUFFER_0, FFT_NR_WINDOW, &FFT_NR_IDXWRITE0, FFT_NR_LBUF);
    lastMeasuredClock = JVX_GET_TICKCOUNT_CLK_GET(&refClk);

    memcpy(out_re, buf_513_1_intern, jvxDataFormat_size[paramsOut[0].description.format]*paramsOut[0].description.seg_length_x);
    memcpy(out_im, buf_513_2_intern, jvxDataFormat_size[paramsOut[0].description.format]*paramsOut[0].description.seg_length_x);

    return JVX_DSP_NO_ERROR;
}

RC_I_DEF(estimate_rpm) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 2, 513}};
RC_O_DEF(estimate_rpm) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 2, 241}};
static jvxDspBaseErrorType estimate_rpm(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    float* in_re = paramsIn[0].content[0];
    float* in_im = paramsIn[0].content[1];
    float* out_sign = paramsOut[0].content[0];
    float* out_probs = paramsOut[0].content[1];
    float f0Out = 0;
    int idxOut = 0;

    // Copy input samples to internal memory buffers..
    memcpy(buf_513_1_intern, in_re, jvxDataFormat_size[paramsIn[0].description.format]*paramsIn[0].description.seg_length_x);
    memcpy(buf_513_2_intern, in_im, jvxDataFormat_size[paramsIn[0].description.format]*paramsIn[0].description.seg_length_x);

    JVX_GET_TICKCOUNT_CLK_SETREF(&refClk);
    rtp_estimate_rpm_process_dsp(&f0Out, &idxOut, buf_513_1_intern, buf_513_2_intern, out_sign, NULL, out_probs);
    lastMeasuredClock = JVX_GET_TICKCOUNT_CLK_GET(&refClk);

    return JVX_DSP_NO_ERROR;
}

RC_I_DEF(td_2_fft) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 40}};
RC_O_DEF(td_2_fft) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 513}};
static jvxDspBaseErrorType td_2_fft(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    memset(paramsOut[0].content[0], 0, jvxDataFormat_size[paramsOut[0].description.format]*paramsOut[0].description.seg_length_x);
    return JVX_DSP_NO_ERROR;
}

RC_I_DEF(wspp_noise_estimation) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 2, 513}, {JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 1}};
RC_O_DEF(wspp_noise_estimation) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 513}};
static jvxDspBaseErrorType wspp_noise_estimation(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    float* in_re = paramsIn[0].content[0];
    float* in_im = paramsIn[0].content[1];
    float* out_psd = paramsOut[0].content[0];
    float* f0 = paramsIn[1].content[0];

    memcpy(buf_513_1_intern, in_re, jvxDataFormat_size[paramsIn[0].description.format]*paramsIn[0].description.seg_length_x);
    memcpy(buf_513_2_intern, in_im, jvxDataFormat_size[paramsIn[0].description.format]*paramsIn[0].description.seg_length_x);

    JVX_GET_TICKCOUNT_CLK_SETREF(&refClk);
    rtp_ne_wspp_process_dsp(buf_513_1_intern, buf_513_2_intern, *f0, buf_513_3_intern);
    lastMeasuredClock = JVX_GET_TICKCOUNT_CLK_GET(&refClk);

    memcpy(out_psd, buf_513_3_intern, jvxDataFormat_size[paramsOut[0].description.format]*paramsOut[0].description.seg_length_x);
    return JVX_DSP_NO_ERROR;
}

RC_I_DEF(ifft_fw_nr) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 2, 513}};
RC_O_DEF(ifft_fw_nr) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 40}};
static jvxDspBaseErrorType ifft_fw_nr(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    float* in_re = paramsIn[0].content[0];
    float* in_im = paramsIn[0].content[1];
    float* out_td = paramsOut[0].content[0];

    memcpy(buf_513_1_intern, in_re, jvxDataFormat_size[paramsIn[0].description.format]*paramsIn[0].description.seg_length_x);
    memcpy(buf_513_2_intern, in_im, jvxDataFormat_size[paramsIn[0].description.format]*paramsIn[0].description.seg_length_x);

    JVX_GET_TICKCOUNT_CLK_SETREF(&refClk);
    rtp_ifft_framework_process_dsp(buf_513_1_intern, buf_513_2_intern, buf_513_3_intern, NRMOTOR_IFFT_NR_BUFFER, IFFT_NR_BUFFER_0,  IFFT_NR_WINDOW, &IFFT_NR_IDXWRITE0,
            IFFT_NR_LBUF, IFFT_NR_NORMFAC_OVERLAPADD);
    lastMeasuredClock = JVX_GET_TICKCOUNT_CLK_GET(&refClk);

    memcpy(out_td, buf_513_3_intern, jvxDataFormat_size[paramsOut[0].description.format]*paramsOut[0].description.seg_length_x);
    return JVX_DSP_NO_ERROR;
}

RC_I_DEF(nrMotor) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 2, 40}};
RC_O_DEF(nrMotor) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 40}};
static jvxDspBaseErrorType nrMotor(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    float* in_voice = paramsIn[0].content[0];
    float* in_chassis = paramsIn[0].content[1];
    float* out_enhanced = paramsOut[0].content[0];

    memcpy(buf_513_1_intern, in_voice, jvxDataFormat_size[paramsIn[0].description.format]*paramsIn[0].description.seg_length_x);
    memcpy(buf_513_2_intern, in_chassis, jvxDataFormat_size[paramsIn[0].description.format]*paramsIn[0].description.seg_length_x);

    JVX_GET_TICKCOUNT_CLK_SETREF(&refClk);
    rtp_nrMotor_process(buf_513_1_intern, buf_513_2_intern, buf_513_3_intern, NULL);
    lastMeasuredClock = JVX_GET_TICKCOUNT_CLK_GET(&refClk);

    memcpy(out_enhanced, buf_513_3_intern, jvxDataFormat_size[paramsOut[0].description.format]*paramsOut[0].description.seg_length_x);
    return JVX_DSP_NO_ERROR;
}

RC_O_DEF(lastclock) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 1}};
static jvxDspBaseErrorType lastclock(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    float* out_lastclock = paramsOut[0].content[0];
    *out_lastclock = (float)lastMeasuredClock;
    return JVX_DSP_NO_ERROR;
}

static jvxDspBaseErrorType system_disable_irq(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    rc_disable_hwi_on_exec = 1;
    return JVX_DSP_NO_ERROR;
}

static jvxDspBaseErrorType system_enable_irq(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    rc_disable_hwi_on_exec = 0;
    return JVX_DSP_NO_ERROR;
}

RC_I_DEF(loop_back) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 128}};
RC_O_DEF(loop_back) = {{JVX_DATAFORMAT_DATA, JVX_FLOAT_DATAFORMAT_ID, 1, 128}};
static jvxDspBaseErrorType loop_back(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut)
{
    memcpy(paramsOut[0].content[0], paramsIn[0].content[0], paramsOut[0].description.seg_length_x * jvxDataFormat_size[paramsOut[0].description.format]);
    return JVX_DSP_NO_ERROR;
}


/* ***************************************************************************************************** *
 * ********************************** remote call function mapping ************************************* *
 * ***************************************************************************************************** */

const rc_function_mapping function_mapping[] = {RC_MAP_IO(fft_test), RC_MAP_IO(fft_fw_rpm), RC_MAP_IO(fft_fw_nr), RC_MAP_IO(estimate_rpm), RC_MAP_IO(td_2_fft),
        RC_MAP_IO(wspp_noise_estimation), RC_MAP_IO(ifft_fw_nr), RC_MAP_IO(nrMotor), RC_MAP_O(lastclock), RC_MAP(system_disable_irq), RC_MAP(system_enable_irq),
        RC_MAP_IO(loop_back)};

const jvxSize rc_function_mapping_length = (sizeof(function_mapping)/sizeof(function_mapping[0]));
