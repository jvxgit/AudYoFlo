#ifndef __CjvxSignalProcessingDevice_H__
#define __CjvxSignalProcessingDevice_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"

#include "pcg_CjvxSignalProcessingDevice_pcg.h"

class CjvxSignalProcessingDevice: public IjvxDevice, public CjvxObject, public IjvxProperties, 
	public CjvxProperties, public IjvxSequencerControl, public CjvxSequencerControl, 
	public CjvxSignalProcessingDevice_genpcg
{

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxSignalProcessingDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxSignalProcessingDevice();
	

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override
	{
		return(_is_ready(suc, reasonIfNot));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override
	{
		return(_prepare());
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION start()override
	{
		return(_start());
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override
	{
		return(_stop());
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override
	{
		return(_postprocess());
	};
	
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxSequencerControl_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL

};

#endif