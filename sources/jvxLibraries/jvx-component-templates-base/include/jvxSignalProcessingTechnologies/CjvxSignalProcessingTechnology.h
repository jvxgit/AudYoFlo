#ifndef __CjvxSignalProcessingTechnology_H__
#define __CjvxSignalProcessingTechnology_H__

#include "jvx.h"

#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"
#include "common/CjvxTechnology.h"

class CjvxSignalProcessingTechnology: public IjvxTechnology, public CjvxTechnology, 
	public IjvxProperties, public CjvxProperties, 
	public IjvxSequencerControl, public CjvxSequencerControl
{
public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxSignalProcessingTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxSignalProcessingTechnology();
	

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

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

	virtual jvxErrorType JVX_CALLINGCONVENTION request_device(jvxSize idx, IjvxDevice** dev)override{return(CjvxTechnology::_request_device(idx, dev));};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_device(IjvxDevice* dev)override{return(CjvxTechnology::_return_device(dev));};

	virtual jvxErrorType JVX_CALLINGCONVENTION ident_device(jvxSize* idx, IjvxDevice* dev)override
	{
		return(CjvxTechnology::_ident_device(idx, dev));
	};

#include "codeFragments/simplify/jvxTechnologies_simplify.h"
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxSequencerControl_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL

};

#endif