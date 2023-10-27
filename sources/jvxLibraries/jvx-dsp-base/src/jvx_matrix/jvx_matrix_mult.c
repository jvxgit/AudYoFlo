#include "jvx_dsp_base.h"
#include "jvx_matrix/jvx_matrix.h"
#include "jvx_matrix/jvx_matrix_prv.h"

jvxDspBaseErrorType jvx_matrix_process_mult_real(jvx_matrix* theMatrix, const void** in, void** out, jvxSize idx_start, jvxSize idx_stop_b4)
{
	jvxSize i,j,k;
	jvxData accuD = 0;

	const jvxData** inD = (const jvxData**)in;
	jvxData** outD = (jvxData**)out;

	if (theMatrix->prv)
	{
		jvx_matrix_prv* thePtr = (jvx_matrix_prv*)theMatrix->prv;		
		jvxData** fld = (jvxData**)thePtr->sync.theMat;
		switch (thePtr->init.format)
		{
		case JVX_DATAFORMAT_DATA:

			for (i = idx_start; i < idx_stop_b4; i++)
			{
				for (j = 0; j < thePtr->init.M; j++)
				{
					jvxData accuD = 0.0;
					jvxData* matPtr = fld[j];
					for (k = 0; k < thePtr->init.N; k++)
					{
						accuD += inD[k][i] * matPtr[k];
					}
					outD[j][i] = accuD;
				}
			}
			break;
		default:
			assert(0);
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_matrix_process_mult_cplx(jvx_matrix* theMatrix, const void** in, void** out, jvxSize idx_start, jvxSize idx_stop_b4)
{
	jvxSize i, j, k;

	const jvxDataCplx** inD = (const jvxDataCplx**)in;
	jvxDataCplx** outD = (jvxDataCplx**)out;

	if (theMatrix->prv)
	{
		jvx_matrix_prv* thePtr = (jvx_matrix_prv*)theMatrix->prv;
		jvxData** fld = (jvxData**)thePtr->sync.theMat;
		switch (thePtr->init.format)
		{
		case JVX_DATAFORMAT_DATA:

			for (i = idx_start; i < idx_stop_b4; i++)
			{
				for (j = 0; j < thePtr->init.M; j++)
				{
					jvxData accuD_real = 0;
					jvxData accuD_imag = 0;
					jvxData* matPtr = fld[j];
					for (k = 0; k < thePtr->init.N; k++)
					{
						accuD_real += inD[k][i].re * matPtr[k];
						accuD_imag += inD[k][i].im * matPtr[k];
					}
					outD[j][i].re = accuD_real;
					outD[j][i].im = accuD_imag;
				}
			}
			break;
		default:
			assert(0);
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}
