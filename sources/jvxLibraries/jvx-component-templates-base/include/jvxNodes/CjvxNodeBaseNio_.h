#ifndef CjvxNodeBaseNio_H__
#define CjvxNodeBaseNio_H__

#include "jvx.h"
#include "templates/common/CjvxObject.h"
#include "templates/common/CjvxProperties.h"
#include "templates/common/CjvxSequencerControl.h"
//#include "templates/common/CjvxInputOutputConnector.h"
#include "templates/common/CjvxConnectorFactory.h"

#include "CjvxInputOutputConnector_Nio.h"

#include "pcg_CjvxNode_pcg.h"

#ifndef JVX_NODE_TYPE_SPECIFIER_TYPE
	#define JVX_NODE_TYPE_SPECIFIER_TYPE JVX_COMPONENT_AUDIO_NODE
#endif

#ifndef JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "signal_processing_node/audio_node"
#endif

class CjvxNodeBaseNio : public IjvxNode, public CjvxObject,
	public IjvxProperties, public CjvxProperties,
	public IjvxSequencerControl, public CjvxSequencerControl,
	public IjvxConnectorFactory, public CjvxConnectorFactory,
	public IjvxConfiguration, 
	public CjvxNode_genpcg
{
protected:

public:
	JVX_CALLINGCONVENTION CjvxNodeBaseNio(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
		_common_set.thisisme = static_cast<IjvxObject*>(this);
	};


	virtual JVX_CALLINGCONVENTION ~CjvxNodeBaseNio() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override;


#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxSequencerControl_simplify.h"
	
#define JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS
#define JVX_STATE_MACHINE_DEFINE_STARTSTOP
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP
#undef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS

#include "codeFragments/simplify/jvxObjects_simplify.h"

	// Interfaces and default implementations for connections
#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"

	// ===================================================================================================
	// Interface IjvxConfiguration
	// ===================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, jvxFlagTag flagtag,
		const char* filename,
		jvxInt32 lineno, jvxConfigurationPurpose = JVX_CONFIGURATION_PURPOSE_CONFIG_ON_SYSTEM_STARTSTOP)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections, jvxFlagTag flagtag, jvxConfigurationPurpose = JVX_CONFIGURATION_PURPOSE_CONFIG_ON_SYSTEM_STARTSTOP)override;

	// ===================================================================================================
	// Interface 
	// ===================================================================================================
#if 0
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
#endif

	jvxErrorType reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId);
};


#endif