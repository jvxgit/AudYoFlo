#ifndef __CjvxBareNode1ioRearrange_H__
#define __CjvxBareNode1ioRearrange_H__

#include "jvxNodes/CjvxBareNode1io.h"

class CjvxBareNode1ioRearrange : public CjvxBareNode1io
{
protected:

	CjvxSimplePropsPars node_output;

	//! Negotiation struct - we need output since the input may accept variable settings but the output side is not
	CjvxNegotiate_output neg_output;

public:

	JVX_CALLINGCONVENTION CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	
	virtual jvxErrorType accept_input_parameters_stop(jvxErrorType resTest)override;
	virtual jvxErrorType accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual void from_input_to_output() override;
	virtual void test_set_output_parameters() override;
	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// Update the output parameter properties from the current input parameter properties
	void update_output_params_from_input_params();

	// Copy the link data descriptor parameters from the output parameter properties
	void update_ldesc_from_output_params_on_test();
	
	void update_output_params_from_ldesc_on_test(jvxLinkDataDescriptor* theData = nullptr);

	// I am not sure about these functions..
	// void update_output_params_on_test();
	void output_params_from_ldesc_on_test();
};

#endif