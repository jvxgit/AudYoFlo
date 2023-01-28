#ifndef __CJVXAPPLICATIONCONTROLLERDEVICE_H__
#define __CJVXAPPLICATIONCONTROLLERDEVICE_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxDevice.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"

#include "pcg_CjvxAppCtrDevice_pcg.h"

class CjvxApplicationControllerDevice: public IjvxDevice, public CjvxDevice, public CjvxObject, 
	public IjvxProperties, public CjvxProperties, 
	public IjvxSequencerControl, public CjvxSequencerControl, 
	public IjvxConfiguration, public CjvxApplicationControllerDevice_genpcg
{
protected:

	struct
	{
		bool is_connected;
	} _common_set_external_controller;

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxApplicationControllerDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxApplicationControllerDevice();
	
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override{return(_is_ready(suc, reasonIfNot));};

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override{return(_prepare());};

	virtual jvxErrorType JVX_CALLINGCONVENTION start()override{return(_start());};

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override{return(_stop());};

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override{return(_postprocess());};

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;


	virtual jvxErrorType JVX_CALLINGCONVENTION  put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename, jvxInt32 lineno )override
	{
		return(JVX_ERROR_UNSUPPORTED);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION  get_configuration(jvxCallManagerConfiguration* callMan, 
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override
	{
		return(JVX_ERROR_UNSUPPORTED);
	};

#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxSequencerControl_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION

#include "codeFragments/simplify/jvxDevice_simplify.h"

};

#endif