#ifndef _CJVXPORTAUDIOTECHNOLOGY_H__
#define _CJVXPORTAUDIOTECHNOLOGY_H__

#include "jvx.h"
#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "CjvxPortAudioDevice.h"
#include "pcg_exports_technology.h"

class CjvxPortAudioTechnology: public CjvxAudioTechnology, 
	public IjvxSequencerControl, public CjvxSequencerControl, 
	public genPortAudio_technology
{
public:
	JVX_CALLINGCONVENTION CjvxPortAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxPortAudioTechnology();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

#include "codeFragments/simplify/jvxProperties_simplify.h"

#include "codeFragments/simplify/jvxSequencerControl_simplify.h"

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

	//bool initializePortAudio();
//#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_PROPERTIES 
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
//#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
};

#endif
