#ifndef __JVX_MEASURE_IR_REG_H__
#define __JVX_MEASURE_IR_REG_H__

#include "jvx_dsp_base.h"

struct jvx_measure_ir_reg_init
{
	jvxSize loopCnt;
	jvxSize fs;
	jvxCBool skipFirst;
	jvxData flowStart;
	jvxData flowStop;
	jvxData fhighStart;
	jvxData fhighStop;
	jvxSize llMax;
};

struct jvx_measure_ir_reg_async
{
	jvxData regulationCoeffMin;
	jvxData regulationCoeffMax;
	jvxCBool interpol_log;
	jvxData exponent_fac;
};

struct jvx_measure_ir_reg_derived
{
	jvxSize ll_oneLoop;
};

struct jvx_measure_ir_reg_ext
{
	const jvxData* gain_ptr;	
	const jvxData* freq_ptr;
	jvxSize sz;
};

struct jvx_measure_ir_reg
{	
	struct jvx_measure_ir_reg_init init;
	struct jvx_measure_ir_reg_async async;
	struct jvx_measure_ir_reg_derived derived;
	jvxHandle* prv;
	struct jvx_measure_ir_reg_ext ext;
};

void jvx_measure_ir_reg_cfg_init(struct jvx_measure_ir_reg* hdl);

jvxErrorType jvx_measure_ir_reg_init(struct jvx_measure_ir_reg* hdl);

jvxErrorType jvx_measure_ir_reg_terminate(struct jvx_measure_ir_reg* hdl);

jvxErrorType jvx_measure_ir_reg_process(struct jvx_measure_ir_reg* hdl,
	const jvxData* inMeasure_ptr, jvxSize inMeasure_sz,
	const jvxData* inTest_ptr, jvxSize inTest_sz,
	jvxData* outMeasureAvrg_ptr, jvxSize outMeasureAvrg_sz,
	jvxData* outTestAvrg_ptr, jvxSize outTestAvrg_sz,
	jvxData* outIr_ptr, jvxSize outIr_sz, 
	const jvxData* win_ptr, jvxSize win_sz,
	jvxData* dbgOut_ptr, jvxSize dbgOut_sz);

jvxErrorType jvx_measure_ir_reg_update(struct jvx_measure_ir_reg* hdl, jvxCBool do_set);

#endif
