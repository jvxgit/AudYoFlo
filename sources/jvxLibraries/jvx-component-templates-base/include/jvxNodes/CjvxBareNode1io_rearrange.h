#ifndef __CJVXBARENODE1IO_REARRANGE_H__
#define __CJVXBARENODE1IO_REARRANGE_H__

#include "jvxNodes/CjvxBareNode1io.h"
#include "pcg_CjvxNode_rearrange_pcg.h"

class CjvxBareNode1io_rearrange : public CjvxBareNode1io, public CjvxNode_rearrange_genpcg
{
protected:
	
	jvxBool outputArgsFromInputOnInit = true;
	jvxBool outputArgsFromInputOnInit_set = false;

	jvxBool outputArgsFromOutputParams = false;
public:

	JVX_CALLINGCONVENTION CjvxBareNode1io_rearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	
	virtual jvxErrorType accept_input_parameters_stop(jvxErrorType resTest)override;
	virtual void from_input_to_output() override;
	
	// Update the output parameter properties from the current input parameter properties
	void update_output_params_from_input_params();

	// Copy the link data descriptor parameters from the output parameter properties
	void update_ldesc_from_output_params_on_test();
	
	// I am not sure about these functions..
	void update_output_params_on_test();
	void output_props_from_ldesc_on_test();

	void update_input_params_on_test();
	void input_props_from_ldesc_on_test();
};

#endif