#ifndef __JVX_IIR_FB_H__
#define __JVX_IIR_FB_H__

#include "jvx_dsp_base.h"

typedef enum
{
	JVX_MBC_FILTER_DESIGN_NONE = 0,
	JVX_MBC_FILTER_DESIGN_BUTTERWORTH = 1
} jvxIirFbType;

typedef struct
{
	jvxSize* splitFrequencies;
	jvxSize* orders;	
	jvxSize numSplitFrequencies;
	jvxSize numChannels;
	jvxSize fs;
	jvxIirFbType fbType;
	jvxCBool verbose;
} jvx_iir_fb_prmInit;

//typedef struct
//{
//} jvx_iir_fb_prmAscync;

typedef struct
{
	jvxSize numSubNodes;
} jvx_iir_fb_prmDerived;

typedef struct
{
	jvx_iir_fb_prmInit prm_init;
	jvx_iir_fb_prmDerived prm_derived;

	//jvx_iir_fb_prmAscync prm_async;

	jvxHandle* prv;
} jvx_iir_fb;

JVX_DSP_LIB_BEGIN

jvxDspBaseErrorType jvx_iir_fb_initConfig(jvx_iir_fb* handle);

jvxDspBaseErrorType jvx_iir_fb_prepare(jvx_iir_fb* handleOnReturn, const char* prefix_verbose);

jvxDspBaseErrorType jvx_iir_fb_postprocess(jvx_iir_fb* handleOnReturn);

jvxDspBaseErrorType jvx_iir_fb_update(jvx_iir_fb* handle, jvxUInt16 whatToUpdate, jvxCBool do_set);

jvxDspBaseErrorType jvx_iir_fb_process(jvx_iir_fb* hhandle, jvxData** bufIn, jvxData*** bufOut, jvxSize bsize);

jvxDspBaseErrorType jvx_iir_fb_reset(jvx_iir_fb* handle);

jvxDspBaseErrorType jvx_iir_fb_prepare_direct(jvx_iir_fb* handleOnReturn, jvxSize* splitFrequencies, jvxSize* orders, jvxSize numSplitFrequencies, jvxSize numChannels, jvxSize fs, jvxIirFbType fbType, jvxCBool verbose, jvxData* coeffs_num, jvxData* coeffs_den, jvxSize* array_coeff_numbers, jvxData* state_buffers, jvxSize num_filters);

jvxDspBaseErrorType jvx_iir_fb_postprocess_direct(jvx_iir_fb* handleOnReturn);

JVX_DSP_LIB_END

#endif
