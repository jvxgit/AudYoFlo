#ifndef __CJVXCUSTOMTECHNOLOGY_H__
#define __CJVXCUSTOMTECHNOLOGY_H__

#include "jvx.h"

#include "common/CjvxProperties.h"
#include "common/CjvxTechnology.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxCustomTechnology: public IjvxTechnology, public CjvxTechnology,
	public IjvxProperties, public CjvxProperties
{
protected:


public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxCustomTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxCustomTechnology();
	
	// ======================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

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

#include "codeFragments/simplify/jvxProperties_simplify.h"

#define JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS_UNSUPPORTED
#define JVX_STATE_MACHINE_DEFINE_STARTSTOP_UNSUPPORTED
#define JVX_STATE_MACHINE_DEFINE_READY_UNSUPPORTED
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP_UNSUPPORTED
#undef JVX_STATE_MACHINE_DEFINE_READY_UNSUPPORTED
#undef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS_UNSUPPORTED

#include "codeFragments/simplify/jvxTechnologies_simplify.h"
#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxHiddenInterfaceFwd_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
