#ifndef __JVX_PDM_FIR_LUT__H__
#define __JVX_PDM_FIR_LUT__H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

typedef struct
{
	jvxSize expSz;
} jvx_pdm_lut_prmInit;

typedef struct
{
	jvxUInt16 *circ_buf; // should be aligned to expSz*2
	jvxInt16* ptr_lookup;
	jvxUInt16* ptr_revAddr;

} jvx_pdm_lut_prmSync;

typedef struct
{
	jvx_pdm_lut_prmInit prm_init;
	jvx_pdm_lut_prmSync prm_sync;

	struct
	{
		jvxSize lCirc16Bit;
		jvxSize lLookup16Bit;
	} derived;

	jvxHandle* prv;
} jvx_fir_lut;

jvxDspBaseErrorType jvx_pdm_fir_lut_initConfig(jvx_fir_lut* hdlOnReturn);
jvxDspBaseErrorType jvx_pdm_fir_lut_terminate(jvx_fir_lut* hdlOnReturn);

jvxDspBaseErrorType jvx_pdm_fir_lut_prepare(jvx_fir_lut* hdlOnReturn);
jvxDspBaseErrorType jvx_pdm_fir_lut_postprocess(jvx_fir_lut* hdlOnReturn);

jvxDspBaseErrorType jvx_pdm_fir_lut_process(jvx_fir_lut* hdl, jvxInt16 * JVX_RESTRICT in_i, jvxInt16 * JVX_RESTRICT out_i, jvxSize fsize);
jvxDspBaseErrorType jvx_pdm_fir_lut_process_ip(jvx_fir_lut* hdl, jvxInt16 * JVX_RESTRICT inout, jvxSize fsize);
jvxDspBaseErrorType jvx_pdm_fir_lut_process_ip_lwords(jvx_fir_lut* hdl, jvxInt16 * JVX_RESTRICT inout, jvxSize fsize);

// jvxDspBaseErrorType jvx_pdm_fir_lut_process_ip_32swap(jvx_fir_lut* hdl, jvxUInt32 * JVX_RESTRICT inout, jvxSize fsize);
jvxDspBaseErrorType jvx_pdm_fir_lut_process_32swap_lr(jvx_fir_lut* hdl, jvxUInt32 * JVX_RESTRICT inl, 
	jvxUInt32 * JVX_RESTRICT inr, jvxInt16 * JVX_RESTRICT out, jvxSize fsizelr);

/*
jvxDspBaseErrorType jvx_pdm_fir_lut_process_rbit_32swap(jvx_fir_lut* hdl, jvxInt16 * JVX_RESTRICT inout, jvxSize fsize);
*/
jvxDspBaseErrorType jvx_pdm_fir_lut_process_rbit_32swap_lr(jvx_fir_lut* hdl, jvxUInt32 * JVX_RESTRICT inl,
	jvxUInt32 * JVX_RESTRICT inr, jvxInt16 * JVX_RESTRICT out, jvxSize fsizelr);

JVX_DSP_LIB_END

#endif
