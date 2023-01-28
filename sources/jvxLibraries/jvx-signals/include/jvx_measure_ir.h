
#ifndef __JVX_MEASURE_IR_H__
#define __JVX_MEASURE_IR_H__

#include "jvx_dsp_base.h"

typedef struct
{
	jvxData* fldBufferTestSignal;
	jvxData* fldBufferMeasureSignal; 
	jvxSize lRecording;
	jvxData* fldIResponse;
	jvxData* fldSPosedMeasure;
	jvxData* fldSPosedTest;
	jvxSize lIResponse;
	jvxData* fldXCorr;
	jvxSize lXCorr;
	jvxSize offsetXCorr;
	struct
	{
		jvxData* freq_buf;
		jvxData* gain_buf;
		jvxData sRate;
		jvxSize freq_gain_len;
	} ext;
} jvxMeasureIr_sync;

typedef struct
{
	jvxSize szTestsignal_onerep;
	jvxSize numReps;
	jvxSize skipNumber;
	jvxCBool computeIr;
	jvxCBool computeXc;
	jvxCBool storeSuperPosed;	
} jvxMeasureIr_init;

typedef struct
{
	jvxMeasureIr_init prm_init;

	jvxMeasureIr_sync prm_sync;

	jvxHandle* prv;
} jvxMeasureIr;

// ============================================================================

jvxDspBaseErrorType jvx_measure_ir_initConfig(jvxMeasureIr* handle);

jvxDspBaseErrorType jvx_measure_ir_prepare(jvxMeasureIr* handleOnReturn);

jvxDspBaseErrorType jvx_measure_ir_update(jvxMeasureIr* handle, jvxUInt16 whatToUpdate, jvxCBool do_set);

jvxDspBaseErrorType jvx_measure_ir_process(jvxMeasureIr* handle);

jvxDspBaseErrorType jvx_measure_ir_postprocess(jvxMeasureIr* handleOnReturn);


#endif
