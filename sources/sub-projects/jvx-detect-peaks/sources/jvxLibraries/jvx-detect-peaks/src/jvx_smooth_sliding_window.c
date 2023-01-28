#include "jvx_dsp_base.h"
#include "jvx_smooth_sliding_window.h"

typedef struct
{
	struct
	{
		jvxSize dimVector;
		jvxSize slidingWinHalfLength;
	} config;

	struct
	{
		jvxData divideConst;
		jvxData* tableDivision;
	} runtime;
} jvx_smooth_sliding_window_private;

jvxDspBaseErrorType
jvx_smooth_sliding_window_init(jvxHandle** hdl, jvxSize dimVector, jvxSize halfWinLength)
{
	jvxSize i;
	if(hdl)
	{
		jvx_smooth_sliding_window_private* newHdl = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvx_smooth_sliding_window_private);
		memset(newHdl, 0, sizeof(jvx_smooth_sliding_window_private));

		newHdl->config.dimVector = dimVector;
		newHdl->config.slidingWinHalfLength = halfWinLength;

		newHdl->runtime.divideConst = (1.0/(jvxData)(2*newHdl->config.slidingWinHalfLength +1));
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newHdl->runtime.tableDivision, jvxData, (newHdl->config.slidingWinHalfLength+1));
		memset(newHdl->runtime.tableDivision, 0, sizeof(jvxData) * (newHdl->config.slidingWinHalfLength+1));

		for(i = 0; i <= newHdl->config.slidingWinHalfLength; i++)
		{
			newHdl->runtime.tableDivision[i] = (jvxData)1.0/(jvxData)(newHdl->config.slidingWinHalfLength + 1 + i);
		}

		*hdl = newHdl;
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType
jvx_smooth_sliding_window_terminate(jvxHandle* hdl)
{
	//jvxSize i;
	if(hdl)
	{
		jvx_smooth_sliding_window_private* theHdl = (jvx_smooth_sliding_window_private*)hdl;
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theHdl->runtime.tableDivision, jvxData);
		JVX_DSP_SAFE_DELETE_OBJECT(theHdl);
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType
jvx_smooth_sliding_window_process(jvxHandle* hdl, jvxData* bufIn, jvxData* bufOut)
{
	jvxSize i;
	jvxData accu1 = 0;
	jvxSize cntReadTable;
	jvxSize cntReadValue;
	jvxSize cntRemove;
	jvxSize tmp;

	if(hdl)
	{
		jvx_smooth_sliding_window_private* theHdl = (jvx_smooth_sliding_window_private*)hdl;

		accu1 = 0;
		for(i = 0; i < theHdl->config.slidingWinHalfLength; i++)
		{
			accu1 += bufIn[i];//frameSearch_log[i];
		}

		cntReadTable = 0;
		cntReadValue = theHdl->config.slidingWinHalfLength;
		for(i=0; i <= theHdl->config.slidingWinHalfLength; i++, cntReadValue++)
		{
			accu1 += bufIn[cntReadValue];//frameSearch_log[cntReadValue];
			*bufOut++ = accu1 * theHdl->runtime.tableDivision[cntReadTable++];
		}

		cntRemove = 0;
		tmp = theHdl->config.dimVector - 2 * theHdl->config.slidingWinHalfLength - 2;
		for(i=0; i <= tmp; i++)
		{
			accu1 +=  bufIn[cntReadValue++];
			accu1 -=  bufIn[cntRemove++];
			*bufOut++ = accu1 * theHdl->runtime.divideConst;
		}

		for(i=0; i < theHdl->config.slidingWinHalfLength; i++)
		{
			*bufOut++ = accu1 * theHdl->runtime.tableDivision[--cntReadTable];
			accu1 -=  bufIn[cntRemove++];
		}		
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

