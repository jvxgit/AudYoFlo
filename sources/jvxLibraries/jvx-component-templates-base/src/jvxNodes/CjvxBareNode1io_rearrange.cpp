#include "jvxNodes/CjvxBareNode1io_rearrange.h"

CjvxBareNode1io_rearrange::CjvxBareNode1io_rearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_ldslave.zeroCopyBuffering_cfg = false;

	_common_set_node_base_1io.prepareOnChainPrepare = true;
	_common_set_node_base_1io.startOnChainStart = true;
	_common_set_node_base_1io.impPrepareOnChainPrepare = true;
	_common_set_node_base_1io.impStartOnChainStart = true;

	neg_input._set_parameters_fixed(
		JVX_SIZE_UNSELECTED, 
		JVX_SIZE_UNSELECTED,
		JVX_SIZE_UNSELECTED, 
		JVX_DATAFORMAT_NONE,
		JVX_DATAFORMAT_GROUP_NONE);

	neg_output._set_parameters_fixed(
		JVX_SIZE_UNSELECTED,
		JVX_SIZE_UNSELECTED,
		JVX_SIZE_UNSELECTED,
		JVX_DATAFORMAT_NONE,
		JVX_DATAFORMAT_GROUP_NONE);
}

jvxErrorType 
CjvxBareNode1io_rearrange::activate()
{
	jvxErrorType res = CjvxBareNode1io::activate();
	if (res == JVX_NO_ERROR)
	{
		CjvxNode_rearrange_genpcg::init_all();
		CjvxNode_rearrange_genpcg::allocate_all();
		CjvxNode_rearrange_genpcg::register_all(static_cast<CjvxProperties*>(this));
		outputArgsFromInputOnInit_set = false;
	}
	return res;
}

jvxErrorType
CjvxBareNode1io_rearrange::deactivate()
{
	jvxErrorType res = CjvxBareNode1io::deactivate();
	if (res == JVX_NO_ERROR)
	{
		// No need to clear CjvxNegotiate ..
		CjvxNode_rearrange_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxNode_rearrange_genpcg::deallocate_all();
	}
	return res;
}

// =====================================================================================

/*
 * Step 1: check input parameters with neg_input
 * Step 2: If accepted, copy resulting parameters from negotiation to internal parameters (and properties)
 * Step 3: Call from_input_to_output:
 *			a) Function update_output_params_from_input_params: Copy output property parameters from input internal parameter. This does NOT include the number of channels
 *			b) Function update_ldesc_from_output_params_on_test: Copy the output properties to the output data struct
 * Step 4: Forward the chain towards next connector
 * Step 5: Function test_set_output_parameters: Constrain the output data struct according to the neg_output specification
 * Step 6: If next input connector requires different setting, we negotiate according to neg_output 
 * Step 6: If 4 was successful, copy the resulting output parameters to the internal properties
 *
 * update_output_params_from_input_params
 */


jvxErrorType
CjvxBareNode1io_rearrange::accept_input_parameters_stop(jvxErrorType resTest)
{
	if (resTest == JVX_NO_ERROR)
	{
		update_output_params_on_test();
	}
	return resTest;
}

void
CjvxBareNode1io_rearrange::update_output_params_from_input_params()
{
	if (newParamProps)
	{
		CjvxNode_rearrange_genpcg::node_output.buffersize.value = CjvxSimplePropsPars::_common_set_node_params_a_1io.buffersize;
		CjvxNode_rearrange_genpcg::node_output.samplerate.value = CjvxSimplePropsPars::_common_set_node_params_a_1io.samplerate;
		CjvxNode_rearrange_genpcg::node_output.format.value = CjvxSimplePropsPars::_common_set_node_params_a_1io.format;
		CjvxNode_rearrange_genpcg::node_output.subformat.value = CjvxSimplePropsPars::_common_set_node_params_a_1io.subformat;
		CjvxNode_rearrange_genpcg::node_output.segmentsize_x.value = CjvxSimplePropsPars::_common_set_node_params_a_1io.dimX;
		CjvxNode_rearrange_genpcg::node_output.segmentsize_y.value = CjvxSimplePropsPars::_common_set_node_params_a_1io.dimY;
	}
	else
	{
		CjvxNode_rearrange_genpcg::node_output.buffersize.value = CjvxNode_genpcg::node.buffersize.value;
		CjvxNode_rearrange_genpcg::node_output.samplerate.value = CjvxNode_genpcg::node.samplerate.value;
		CjvxNode_rearrange_genpcg::node_output.format.value = CjvxNode_genpcg::node.format.value;
		CjvxNode_rearrange_genpcg::node_output.subformat.value = CjvxNode_genpcg::node.subformat.value;
		CjvxNode_rearrange_genpcg::node_output.segmentsize_x.value = CjvxNode_genpcg::node.segmentsize_x.value;
		CjvxNode_rearrange_genpcg::node_output.segmentsize_y.value = CjvxNode_genpcg::node.segmentsize_y.value;
	}
}

void
CjvxBareNode1io_rearrange::update_ldesc_from_output_params_on_test()
{
	_common_set_ldslave.theData_out.con_params.buffersize = CjvxNode_rearrange_genpcg::node_output.buffersize.value;
	_common_set_ldslave.theData_out.con_params.rate = CjvxNode_rearrange_genpcg::node_output.samplerate.value;
	_common_set_ldslave.theData_out.con_params.format = (jvxDataFormat)CjvxNode_rearrange_genpcg::node_output.format.value;
	_common_set_ldslave.theData_out.con_params.format_group = (jvxDataFormatGroup)CjvxNode_rearrange_genpcg::node_output.subformat.value;
	_common_set_ldslave.theData_out.con_params.segmentation_x = CjvxNode_rearrange_genpcg::node_output.segmentsize_x.value;
	_common_set_ldslave.theData_out.con_params.segmentation_y = CjvxNode_rearrange_genpcg::node_output.segmentsize_y.value;
	_common_set_ldslave.theData_out.con_params.number_channels = CjvxNode_genpcg::node.numberoutputchannels.value;
}

void
CjvxBareNode1io_rearrange::update_output_params_on_test()
{
	// Update all output parameters
	if (!newParamProps)
	{
		CjvxNode_genpcg::node.numberoutputchannels.value = JVX_SIZE_INT32(_common_set_ldslave.theData_out.con_params.number_channels);
	}
	output_props_from_ldesc_on_test();
	
}

void
CjvxBareNode1io_rearrange::output_props_from_ldesc_on_test()
{
	CjvxNode_rearrange_genpcg::node_output.buffersize.value = JVX_SIZE_INT32(_common_set_ldslave.theData_out.con_params.buffersize);
	CjvxNode_rearrange_genpcg::node_output.samplerate.value = JVX_SIZE_INT32(_common_set_ldslave.theData_out.con_params.rate);
	CjvxNode_rearrange_genpcg::node_output.format.value = JVX_SIZE_INT16(_common_set_ldslave.theData_out.con_params.format);
	CjvxNode_rearrange_genpcg::node_output.subformat.value = JVX_SIZE_INT16(_common_set_ldslave.theData_out.con_params.format_group);
	CjvxNode_rearrange_genpcg::node_output.segmentsize_x.value = JVX_SIZE_INT32(_common_set_ldslave.theData_out.con_params.segmentation_x);
	CjvxNode_rearrange_genpcg::node_output.segmentsize_y.value = JVX_SIZE_INT32(_common_set_ldslave.theData_out.con_params.segmentation_y);
}

void
CjvxBareNode1io_rearrange::update_input_params_on_test()
{
	// Update all output parameters
	CjvxNode_genpcg::node.numberinputchannels.value = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.number_channels);
	input_props_from_ldesc_on_test();

}

void
CjvxBareNode1io_rearrange::input_props_from_ldesc_on_test()
{
	CjvxNode_genpcg::node.buffersize.value = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.buffersize);
	CjvxNode_genpcg::node.samplerate.value = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.rate);
	CjvxNode_genpcg::node.format.value = JVX_SIZE_INT16(_common_set_ldslave.theData_in->con_params.format);
	CjvxNode_genpcg::node.subformat.value = JVX_SIZE_INT16(_common_set_ldslave.theData_in->con_params.format_group);
	CjvxNode_genpcg::node.segmentsize_x.value = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.segmentation_x);
	CjvxNode_genpcg::node.segmentsize_y.value = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.segmentation_y);
}

void
CjvxBareNode1io_rearrange::from_input_to_output()
{

	// Copy output properties from input properties
	update_output_params_from_input_params();

	if (outputArgsFromOutputParams)
	{
		// Copy output params (properties) to dataOut field
		update_ldesc_from_output_params_on_test();
	}
}