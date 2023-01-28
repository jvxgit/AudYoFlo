
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

#include "codeFragments/simplify/jvxStateMachine_simplify.h"

	// ======================================================
	// Unsupported state transitions
	// ======================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override
	{
		return(JVX_ERROR_UNSUPPORTED);
	};


	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override
	{
		return(JVX_ERROR_UNSUPPORTED);
	};


	virtual jvxErrorType JVX_CALLINGCONVENTION start()override
	{
		return(JVX_ERROR_UNSUPPORTED);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override
	{
		return(JVX_ERROR_UNSUPPORTED);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override
	{
		return(JVX_ERROR_UNSUPPORTED);
	};

	// ======================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION request_device(jvxSize idx, IjvxDevice** dev)override 
	{
		return(CjvxTechnology::_request_device(idx, dev));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_device(IjvxDevice* dev)override 
	{
		return(CjvxTechnology::_return_device(dev));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION ident_device(jvxSize* idx, IjvxDevice* dev)override
	{
		return(CjvxTechnology::_ident_device(idx, dev));
	};