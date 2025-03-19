#ifndef CJVXAUDIONODEBASE_H__
#define CJVXAUDIONODEBASE_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"

// Important: Put this header INTO the name space, otherwise, we get name clashes
#include "pcg_CjvxAudioNode_pcg.h"

class CjvxAudioNodeBase : 
	public IjvxNode, public CjvxObject,
	public IjvxProperties, public CjvxProperties,
	public IjvxSequencerControl, public CjvxSequencerControl,
	public IjvxConnectorFactory, public CjvxConnectorFactory,
	public IjvxInputConnector, public IjvxOutputConnector, public CjvxInputOutputConnector,
	public IjvxConfiguration, public CjvxAudioNode_genpcg
{
protected:
	jvxCommonSetNodeParams _common_set_node_params_1io;

	jvxBool zeroCopyBuffering_rt;

public:
	JVX_CALLINGCONVENTION CjvxAudioNodeBase(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAudioNodeBase();

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
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

	// ===================================================================================================
	// Interface IjvxConfiguration
	// ===================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan, 
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan, 
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================================
	// Interface 
	// ===================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual void on_test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	
	jvxErrorType reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId);
};

#define JVX_OS_REACT_INTERFACE_PARAMETERS_DECLARE \
	jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))override; \
	jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override; \
	void update_interfaces();

#define JVX_OS_REACT_INTERFACE_PARAMETERS_DEFINE(classname, baseclass) \
jvxErrorType classname::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) \
{ \
	jvxErrorType res = JVX_NO_ERROR; \
	baseclass::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(var)); \
	if (res == JVX_NO_ERROR) \
	{ \
		update_interfaces(); \
	} \
	return res; \
} \
jvxErrorType classname::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) \
{ \
	jvxSize i; \
	jvxErrorType res = baseclass::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb)); \
	if (res == JVX_NO_ERROR) \
	{ \
		update_interfaces(); \
	} \
	return res; \
} \
void classname::update_interfaces() 

#endif