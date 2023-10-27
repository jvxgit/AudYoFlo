#ifndef __JVX_MATRIX_PRV_H__
#define __JVX_MATRIX_PRV_H__

typedef struct
{
	jvx_matrix_prmInit init;
	jvx_matrix_prmSync sync;
	jvxCBool inter_allocate;
} jvx_matrix_prv;

#endif