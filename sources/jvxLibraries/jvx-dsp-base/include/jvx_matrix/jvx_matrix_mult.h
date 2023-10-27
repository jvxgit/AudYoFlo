#ifndef __JVX_MATRIX_MULT_H__
#define __JVX_MATRIX_MULT_H__

#include "jvx_dsp_base.h"
#include "jvx_matrix/jvx_matrix.h"

#include <math.h>

JVX_DSP_LIB_BEGIN

jvxDspBaseErrorType jvx_matrix_process_mult_real(jvx_matrix* theMatrix, const void** in, void** out, jvxSize offset, jvxSize num);
jvxDspBaseErrorType jvx_matrix_process_mult_cplx(jvx_matrix* theMatrix, const void** in, void** out, jvxSize offset, jvxSize num);

JVX_DSP_LIB_END

#endif
