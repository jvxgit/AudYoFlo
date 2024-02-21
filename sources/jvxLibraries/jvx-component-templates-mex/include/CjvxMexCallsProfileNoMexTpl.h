#ifndef __CJVXMEXCALLSPROFILENOMEXTPL_H__
#define __CJVXMEXCALLSPROFILENOMEXTPL_H__

#include "CjvxMexCallsProfileInterfacing.h"

template <class T>
class CjvxMexCallsProfileNoMexTpl : public T, public CjvxMexCallsProfileInterfacing
{
protected:

public:
	JVX_CALLINGCONVENTION CjvxMexCallsProfileNoMexTpl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		T(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL) {};

	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = T::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			res = local_prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		return res;
	};

	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = T::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			res = local_postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		return res;
	};

	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Run core algorithm in C
		res = local_process_buffers_icon(mt_mask, idx_stage);

		// Forward calling chain
		return fwd_process_buffers_icon(mt_mask, idx_stage);
	};
};

#endif
