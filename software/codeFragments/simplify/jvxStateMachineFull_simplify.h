#include "jvxStateMachine_simplify.h"

#ifndef JVX_STATEMACHINE_FULL_SKIP_ACTIVATE_DEACTIVATE
virtual jvxErrorType JVX_CALLINGCONVENTION activate()override
{return(CjvxObject::_activate());};
		
virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override
{return(CjvxObject::_deactivate());};
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override
{return(CjvxObject::_is_ready(suc, reasonIfNot));};

#ifndef JVX_STATEMACHINE_FULL_SKIP_PREPARE_POSTPROCESS
virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override
{
	return(CjvxObject::_prepare());
};

virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override
{
	return(CjvxObject::_postprocess());
};
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION start()override
{return(CjvxObject::_start());};

virtual jvxErrorType JVX_CALLINGCONVENTION stop()override
{return(CjvxObject::_stop());};

