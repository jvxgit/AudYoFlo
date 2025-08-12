#ifndef __JVX_FXLMS_H__
#define __JVX_FXLMS_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

typedef enum
{
	RTP_FXLMS_UPDATE_STRATEGY_CONV = 0,
	RTP_FXLMS_UPDATE_STRATEGY_MODIFIED = 1
} jvxFxlmsFeedbackStrategy;

struct jvx_fxlms_init
{
	jvxFxlmsFeedbackStrategy stepsizeStrategy;

	jvxData freqSelect_omegaMin;
	jvxData freqSelect_omegaMax;

	jvxInt32 delay_realSecPath;
	jvxInt32 delay_estimSecPath;

	jvxSize length_realSecPath;
	jvxSize length_estimSecPath;
	jvxSize length_adapFilter;
};

struct jvx_fxlms_async
{
	jvxData mu;
	jvxData alpha;
	jvxData eps;
	jvxData effectiveDelaySecPath;
	jvxData alpha_smooth;
	jvxData energy_filter;
	jvxData maxValAmplitude;
	jvxData leakage;

	struct
	{

		jvxCBool active_fxh;
		jvxCBool active_fe;
	} frequencySelectionFilter;

	jvxFxlmsFeedbackStrategy stepsizeStrategy;
};


struct jvx_fxlms_sync
{
	jvxData* coeffs_adaptiveFilter;
	jvxData* coeffs_realSecPath;
	jvxData* coeffs_estimSecPath;

	struct
	{
		jvxData omega_freq;
		jvxData qfactor;
	} frequencySelectionFilter_fxh;

	struct
	{
		jvxData omega_freq;
		jvxData qfactor;
	} frequencySelectionFilter_fex;

};

struct jvx_fxlms
{
	struct jvx_fxlms_init init;
	struct jvx_fxlms_async async;
	struct jvx_fxlms_sync sync;

	jvxHandle* prv;
};

jvxErrorType jvx_fxlms_feedback_process(struct jvx_fxlms* hdlIn, jvxData input_e, jvxData* output_x,
	jvxCBool* limiter, jvxData* ret_energy_filter, jvxData overwrite_leakage, jvxData* output_xh_filtered);


JVX_DSP_LIB_END

#endif