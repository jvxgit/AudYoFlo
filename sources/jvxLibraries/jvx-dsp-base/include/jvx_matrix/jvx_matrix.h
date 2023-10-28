#ifndef __JVX_MATRIX_H__
#define __JVX_MATRIX_H__

#include "jvx_dsp_base.h"

#include <math.h>

JVX_DSP_LIB_BEGIN

typedef struct 
{
	jvxSize N;
	jvxSize M;
	jvxDataFormat format;
	jvxCBool intern_alloc_cont;
	jvxCBool lin_field_matrix;
} jvx_matrix_prmInit;

typedef struct 
{
	jvxHandle** theMat;
	jvxHandle* theMat_lin;
} jvx_matrix_prmSync;

typedef struct 
{
	jvx_matrix_prmInit prmInit;
	jvx_matrix_prmSync prmSync;
	jvxHandle* prv;
} jvx_matrix;

jvxDspBaseErrorType jvx_matrix_initConfig(jvx_matrix* theMatrix);
jvxDspBaseErrorType jvx_matrix_prepare(jvx_matrix* theMatrix);
jvxDspBaseErrorType jvx_matrix_postprocess(jvx_matrix* theMatrix);
jvxDspBaseErrorType jvx_matrix_update(jvx_matrix* theMatrix, jvxInt16 whatToUpdate, jvxCBool do_set);

JVX_DSP_LIB_END

#endif
