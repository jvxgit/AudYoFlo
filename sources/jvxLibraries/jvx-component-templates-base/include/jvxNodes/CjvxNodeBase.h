#ifndef CjvxNodeBase_H__
#define CjvxNodeBase_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"

class CjvxNodeBase : public IjvxNode, public CjvxObject,
	public IjvxProperties, public CjvxProperties,
	public IjvxConfiguration, public IjvxManipulate
{
protected:

public:
	JVX_CALLINGCONVENTION CjvxNodeBase(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxNodeBase() {};
	
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"
	
#define JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS
#define JVX_STATE_MACHINE_DEFINE_STARTSTOP
#define JVX_STATE_MACHINE_DEFINE_ACTIVATEDEACTIVATE
#define JVX_STATE_MACHINE_DEFINE_READY
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_READY
#undef JVX_STATE_MACHINE_DEFINE_ACTIVATEDEACTIVATE
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP
#undef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS

#include "codeFragments/simplify/jvxObjects_simplify.h"

	// ============================================================================================
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_MANIPULATE
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_MANIPULATE
	// ============================================================================================

	// ===================================================================================================
	// Interface IjvxConfiguration
	// ===================================================================================================
	jvxErrorType put_configuration(
		jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename,
		jvxInt32 lineno)override;

	jvxErrorType get_configuration(
		jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================================
	// Interface IjvxManipulation
	// ===================================================================================================

	jvxErrorType set_manipulate_value(jvxSize id, jvxVariant* varray) override;
	jvxErrorType get_manipulate_value(jvxSize id, jvxVariant* varray) override;

};

#endif