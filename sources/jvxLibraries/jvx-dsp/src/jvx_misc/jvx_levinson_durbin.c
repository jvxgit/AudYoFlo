#include "jvx_dsp_base.h"
#include "jvx_misc/jvx_levinson_durbin.h"

//#define EPS_RXX_INIT (jvxData)0.0000001
#define EPS_RXX_INIT 0.0


typedef struct
{
	jvxSize maxOrderLPC;
	jvxSize maxNumCoeffsLPCVec;
	jvxData* alpha_temp;
} jvx_levinson_durbin_prv;


jvxDspBaseErrorType
jvx_levinson_durbin_initialize(jvxHandle** handleOnReturn, jvxSize lpcOrderMax)
{
	if(handleOnReturn)
	{
		jvx_levinson_durbin_prv* H = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(H, jvx_levinson_durbin_prv);
		H->maxOrderLPC = lpcOrderMax;
		H->maxNumCoeffsLPCVec = H->maxOrderLPC + 1;

		H->alpha_temp = NULL;
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(H->alpha_temp, jvxData, H->maxNumCoeffsLPCVec);
		memset(H->alpha_temp, 0, sizeof(jvxData)*(H->maxNumCoeffsLPCVec));
		*handleOnReturn = H;
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType
jvx_levinson_durbin_terminate(jvxHandle* handle)
{
	if(handle)
	{
		jvx_levinson_durbin_prv* H = (jvx_levinson_durbin_prv*)handle;
		JVX_DSP_SAFE_DELETE_FIELD(H->alpha_temp);
		H->alpha_temp = NULL;
		JVX_DSP_SAFE_DELETE_OBJECT(H);
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType
jvx_levinson_durbin_compute(jvxHandle* handle, jvxData* Rxx, jvxData* alpha, jvxData* eStart, jvxData* eStop)
{
	jvxSize ii, iii;
	jvxData E;
	jvxData q;
	jvxData kp;
	jvxData* pointerPos;
	jvxData* pointerNeg;

	if(handle)
	{
		jvx_levinson_durbin_prv* H = (jvx_levinson_durbin_prv*) handle;

		memset(&alpha[1], 0, sizeof(jvxData)*(H->maxOrderLPC));

		alpha[0] = 1.0;
		
		E = Rxx[0] + EPS_RXX_INIT;
		if(eStart)
			*eStart = E;
		q = 0;
		kp = 0;

		pointerPos = NULL;
		pointerNeg = NULL;

		// Complexity: orderLPC (CPLX0)
		for(ii = 1; ii < (H->maxNumCoeffsLPCVec); ii++)
		{
			q = 0;
	
			pointerPos = alpha;
			pointerNeg = Rxx+ii;

			// Inner loop: ii = 1.. Nc = ii (CPLX1)
			for(iii = 0; iii < ii; iii++)
			{
				q += *pointerPos++ * *pointerNeg--;
			}

			kp = -q/E;

			memcpy(H->alpha_temp, alpha, sizeof(jvxData)*(ii+1));

			pointerPos = alpha;
			pointerNeg = H->alpha_temp+ii;

			for(iii = 0; iii <= ii; iii++)
			{
				*pointerPos++ += kp * *pointerNeg--;
				//alphaLong[iii] = alphaLong[iii] + kp * alpha_temp[ii-iii];
			}

			// Update residual energy (CPLX5)
			E = E*(1-kp*kp);
		}
		if(eStop)
		{
			*eStop = E;
		}
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

