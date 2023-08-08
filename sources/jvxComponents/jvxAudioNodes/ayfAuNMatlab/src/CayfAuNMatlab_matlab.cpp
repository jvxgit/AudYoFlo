#include "CayfAuNMatlab.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#ifdef JVX_EXTERNAL_CALL_ENABLED

	jvxErrorType
		CayfAuNMatlab::myFirstAlgorithm( /* Input Signal->*/ jvxData* paramIn0, jvxInt32 dimInX0,
			/* Output Signal->*/ jvxData* paramOut0, jvxInt32 dimOutX0)
	{
		jvxSize dimInOut = JVX_MIN(dimInX0, dimOutX0);
		memcpy(paramOut0, paramIn0, sizeof(jvxData) * dimInOut);
		return JVX_NO_ERROR;
	}

#endif

#ifdef JVX_PROJECT_NAMESPACE
}
#endif