#ifndef __CJVXAPPLICATIONCONTROLLERTECHNOLOGY_H__
#define __CJVXAPPLICATIONCONTROLLERTECHNOLOGY_H__

#include "jvx.h"

#include "common/CjvxProperties.h"
#include "common/CjvxTechnology.h"

class CjvxApplicationControllerTechnology: public IjvxTechnology, public CjvxTechnology
	//, public CjvxProperties
{
protected:


public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxApplicationControllerTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxApplicationControllerTechnology();
	
	// ======================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// ======================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION request_device(jvxSize idx, IjvxDevice** dev, jvxCBitField requestReferenceProxy)override
  {
    return(CjvxTechnology::_request_device(idx, dev, requestReferenceProxy));
  };
	virtual jvxErrorType JVX_CALLINGCONVENTION return_device(IjvxDevice* dev)override
  {
    return(CjvxTechnology::_return_device(dev));
  };

	virtual jvxErrorType JVX_CALLINGCONVENTION ident_device(jvxSize* idx, IjvxDevice* dev)override
	{
		return(CjvxTechnology::_ident_device(idx, dev));
	};

#define JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS_UNSUPPORTED
#define JVX_STATE_MACHINE_DEFINE_STARTSTOP_UNSUPPORTED
#define JVX_STATE_MACHINE_DEFINE_READY_UNSUPPORTED
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP_UNSUPPORTED
#undef JVX_STATE_MACHINE_DEFINE_READY_UNSUPPORTED
#undef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS_UNSUPPORTED

#include "codeFragments/simplify/jvxTechnologies_simplify.h"
#include "codeFragments/simplify/jvxObjects_simplify.h"
// #include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxHiddenInterfaceFwd_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"
	//#include "codeFragments/simplify/jvxProperties_simplify.h"

};

#endif
