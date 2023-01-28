#ifndef __JVX_PDM_CIC__H__
#define __JVX_PDM_CIC__H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

typedef struct {
    jvxSize frame_length;
    jvxInt16 cic_i[3];
    jvxInt16 cic_c0[3];
    jvxInt16 cic_c1[3];
} jvx_cic;

void cic_decimation_filter_init(jvx_cic** hdlOnReturn, jvxSize frame_length);
void cic_decimation_filter_free(jvx_cic** hdl);
void cic_decimation_filter(jvx_cic* hdl, jvxInt16 * JVX_RESTRICT in_i, jvxInt16 * JVX_RESTRICT out_i);

JVX_DSP_LIB_END

#endif
