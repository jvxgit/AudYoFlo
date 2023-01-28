#ifndef __CJVXCUSTOMDEVICE_H__
#define __CJVXCUSTOMDEVICE_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxDevice.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxConnectionMaster.h"
#include "common/CjvxConnectorMasterFactory.h"

#include "pcg_CjvxCustomDevice_pcg.h"


#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxCustomDevice: public IjvxDevice, public CjvxDevice, public CjvxObject, 
	public IjvxProperties, public CjvxProperties,
	public IjvxSequencerControl, public CjvxSequencerControl,
	public IjvxConfiguration,
	public IjvxConnectorFactory, public CjvxConnectorFactory,
	public IjvxInputConnector, public IjvxOutputConnector, public CjvxInputOutputConnector,
	public IjvxConnectionMaster, public CjvxConnectionMaster,
	public IjvxConnectionMasterFactory, public CjvxConnectionMasterFactory,
	public CjvxCustomDevice_genpcg
{
protected:

	struct
	{
		int dummy;
	} _common_set_custom_device;

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	JVX_CALLINGCONVENTION CjvxCustomDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxCustomDevice();

	// =====================================================================
	// Default implementations state machine
	// =====================================================================

#define  JVX_STATE_MACHINE_DEFINE_STARTSTOP
#define  JVX_STATE_MACHINE_DEFINE_READY
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP
#undef JVX_STATE_MACHINE_DEFINE_READY

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	// =====================================================================
	// =====================================================================

	// =======================================================================
	// Interface properties
	// =======================================================================
#include "codeFragments/simplify/jvxProperties_simplify.h"

	// Interface IjvxConfiguration

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "",
		jvxInt32 lineno = -1)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// All other interfaces

#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxSequencerControl_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Link to default input/output connector factory and input/output connector implementations
	// =====================================================================
#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// =====================================================================
#define JVX_INPUT_OUTPUT_CONNECTOR_MASTER
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
#undef JVX_INPUT_OUTPUT_CONNECTOR_MASTER

	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Link to default master factory and master implementations
	// =====================================================================
#include "codeFragments/simplify/jvxConnectorMasterFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Master interface: Default implementation requires to find current settings
	// =====================================================================
#include "codeFragments/simplify/jvxConnectionMaster_simplify.h"

	// =====================================================================
	// =====================================================================

	// =====================================================================
	// For all non-object interfaces, return object reference
	// =====================================================================
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Activate the relevant hidden interfaces
	// =====================================================================

#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#define JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL

#include "codeFragments/simplify/jvxDevice_simplify.h"
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
