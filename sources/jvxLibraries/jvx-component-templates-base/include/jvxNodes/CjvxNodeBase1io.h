#ifndef CjvxNodeBase1io_H__
#define CjvxNodeBase1io_H__

#include "jvx.h"
#include "common/CjvxObject.h"
//#include "common/CjvxPropertiesP.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxNegotiate.h"

#include "common/CjvxSimplePropsPars.h"
#include "pcg_CjvxNode_pcg.h"

class CjvxNodeBase1io : public IjvxNode, public CjvxObject,
	public IjvxProperties, public CjvxProperties,
	public IjvxSequencerControl, public CjvxSequencerControl,
	public IjvxConnectorFactory, public CjvxConnectorFactory,
	public IjvxInputConnector, public IjvxOutputConnector, public CjvxInputOutputConnector,
	public IjvxConfiguration, public IjvxManipulate
{
protected:

	CjvxSimplePropsPars node_inout;

	// jvxCommonSetNodeParams _common_set_node_params_1io;
	jvxBool zeroCopyBuffering_rt = false;

	CjvxNegotiate_input neg_input;

	// Forward a complaint towards previous component
	jvxBool forward_complain;
	
	// This variable checks on the INPUT side if new parametzer settings were detected.
	// If so we try to change
	jvxBool newParamsOnTestInput = false;

	// This prefix will be used to identify data setting properties
	std::string prefix_descriptor_properties = "inout";
	std::string prefix_description_properties = "InOut";

public:
	JVX_CALLINGCONVENTION CjvxNodeBase1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxNodeBase1io() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override;

#include "codeFragments/simplify/jvxSystemStatus_simplify.h"
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

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================================
	// Interface 
	// ===================================================================================================

	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	virtual jvxErrorType accept_input_parameters_stop(jvxErrorType resTest);

	virtual void from_input_to_output();

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	/** 
	 * This function runs before the actual local negotiations of the local component. We may forward a
	 * negotiation towards the successor!
	 */ //==============================================================================================
	virtual jvxErrorType accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// jvxErrorType reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId);
	jvxErrorType reportRequestTestChainMaster();

	void update_ldesc_from_input_params_on_test();
	// This function is called from local test_connect_ocon function and shall be re-defined in derived class
	virtual void test_set_output_parameters();

	virtual jvxErrorType inform_chain_test(jvxBool oldMethod = true);

	// Some helpers

	// Update the input parameter properties from latest successful negotiations
	virtual void update_simple_params_from_neg_on_test();
	virtual void update_simple_params_from_ldesc(); // **

	jvxCBitField requires_reconfig(jvxLinkDataDescriptor* theData, jvxCBitField cmpWhat);

	virtual void constrain_ldesc_from_neg_params(const CjvxNegotiate_common& neg);

	virtual jvxErrorType JVX_CALLINGCONVENTION set_manipulate_value(jvxSize id, jvxVariant* varray) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_manipulate_value(jvxSize id, jvxVariant* varray) override;

};

#define JVX_OS_REACT_INTERFACE_PARAMETERS_DECLARE \
	jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))override; \
	jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override; \
	void update_interfaces();

#define JVX_OS_REACT_INTERFACE_PARAMETERS_DEFINE(classname) \
jvxErrorType classname::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) \
{ \
	jvxErrorType res = JVX_NO_ERROR; \
	CjvxNodeBase1io::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(var)); \
	if (res == JVX_NO_ERROR) \
	{ \
		update_interfaces(); \
	} \
	return res; \
} \
jvxErrorType classname::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) \
{ \
	jvxSize i; \
	jvxErrorType res = CjvxNodeBase1io::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb)); \
	if (res == JVX_NO_ERROR) \
	{ \
		update_interfaces(); \
	} \
	return res; \
} \
void classname::update_interfaces() 

#endif