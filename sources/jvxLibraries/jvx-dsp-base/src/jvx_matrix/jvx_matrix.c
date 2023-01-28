#include "jvx_dsp_base.h"
#include "jvx_matrix/jvx_matrix.h"

typedef struct
{
	jvx_matrix_prmInit init;
	jvx_matrix_prmSync sync;
	jvxCBool inter_allocate;
} jvx_matrix_prv;

jvxDspBaseErrorType jvx_matrix_initConfig(jvx_matrix* theMatrix)
{
	theMatrix->prmInit.format = JVX_DATAFORMAT_DATA;
	theMatrix->prmInit.N = 8;
	theMatrix->prmInit.M = 8;
	theMatrix->prmInit.intern_alloc_cont = true;
	theMatrix->prmInit.lin_field_matrix = false;

	theMatrix->prmSync.theMat = NULL;
	theMatrix->prmSync.theMat_lin = NULL;

	theMatrix->prv = NULL;

	return JVX_DSP_NO_ERROR;
}
jvxDspBaseErrorType jvx_matrix_prepare(jvx_matrix* theMatrix)
{
	jvxSize i;
	if (theMatrix->prv == NULL)
	{
		jvx_matrix_prv* thePtr = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(thePtr, jvx_matrix_prv);

		thePtr->init = theMatrix->prmInit;
		thePtr->sync = theMatrix->prmSync;

		assert(thePtr->sync.theMat == NULL);
		assert(thePtr->sync.theMat_lin == NULL);

		if (thePtr->init.intern_alloc_cont)
		{
			thePtr->inter_allocate = true;
			if (thePtr->init.lin_field_matrix)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(thePtr->sync.theMat_lin, jvxByte, (thePtr->init.N * thePtr->init.M) * jvxDataFormat_getsize(thePtr->init.format));
			}
			else
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(thePtr->sync.theMat, jvxHandle*, thePtr->init.M);
				for (i = 0; i < thePtr->init.M; i++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD_Z(thePtr->sync.theMat[i], jvxByte, thePtr->init.N * jvxDataFormat_getsize(thePtr->init.format));
				}
			}
		}

		theMatrix->prv = thePtr;

		// Provide sync pointers to outside world
		jvx_matrix_update(theMatrix, JVX_DSP_UPDATE_SYNC, false);

		return JVX_DSP_NO_ERROR;

	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_matrix_postprocess(jvx_matrix* theMatrix)
{
	jvxSize i;
	if (theMatrix->prv)
	{
		jvx_matrix_prv* thePtr = (jvx_matrix_prv*)theMatrix->prv;
		if (thePtr->inter_allocate)
		{
			if (thePtr->sync.theMat)
			{
				for (i = 0; i < thePtr->init.M; i++)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(thePtr->sync.theMat[i], jvxByte);
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(thePtr->sync.theMat, jvxHandle*);
			}
			thePtr->sync.theMat = NULL;

			if (thePtr->sync.theMat_lin)
			{
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(thePtr->sync.theMat_lin, jvxByte);
			}
			thePtr->sync.theMat_lin = NULL;
		}
		theMatrix->prmSync = thePtr->sync;

		JVX_DSP_SAFE_DELETE_OBJECT(thePtr);
		theMatrix->prv = NULL;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_matrix_process_real(jvx_matrix* theMatrix, const void** in, void** out, jvxSize idx_start, jvxSize idx_stop_b4)
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

jvxDspBaseErrorType jvx_matrix_process_cplx(jvx_matrix* theMatrix, const void** in, void** out, jvxSize idx_start, jvxSize idx_stop_b4)
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
jvxDspBaseErrorType jvx_matrix_update(jvx_matrix* theMatrix, jvxInt16 whatToUpdate, jvxCBool do_set)
{
	jvx_matrix_prv* prv = NULL;
	
	if (!theMatrix)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_matrix_prv*)theMatrix->prv;

	if (!prv)
	{
		return JVX_DSP_ERROR_WRONG_STATE;
	}

	// ======================================================

	switch (whatToUpdate)
	{
	case JVX_DSP_UPDATE_ASYNC:
		if (do_set)
		{
			//prv->async = theMatrix->prmAsync;
		}
		else
		{
			// theMatrix->prmAsync = prv->async;
		}
		break;
	case JVX_DSP_UPDATE_SYNC:
		if (do_set)
		{
			prv->sync = theMatrix->prmSync;
		}
		else
		{
			theMatrix->prmSync = prv->sync;
		}
		break;
	case JVX_DSP_UPDATE_INIT:
		break;
	}
	return(JVX_DSP_NO_ERROR);
}