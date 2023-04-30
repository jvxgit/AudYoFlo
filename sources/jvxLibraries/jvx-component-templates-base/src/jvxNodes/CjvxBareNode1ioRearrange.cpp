#include "jvxNodes/CjvxBareNode1ioRearrange.h"

CjvxBareNode1ioRearrange::CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_ldslave.zeroCopyBuffering_cfg = false;

	_common_set_node_base_1io.prepareOnChainPrepare = true;
	_common_set_node_base_1io.startOnChainStart = true;

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
	
	forward_complain = false;
	neg_output.logObj = this;
}

jvxErrorType 
CjvxBareNode1ioRearrange::activate()
{
	jvxErrorType res = CjvxBareNode1io::activate();
	if (res == JVX_NO_ERROR)
	{
		node_output.initialize(this, "output", true);
		//outputArgsFromInputOnInit_set = false;

		node_inout.updtag_all("Input");
		node_output.updtag_all("Output");
	}
	return res;
}

jvxErrorType
CjvxBareNode1ioRearrange::deactivate()
{
	jvxErrorType res = CjvxBareNode1io::deactivate();
	if (res == JVX_NO_ERROR)
	{
		// No need to clear CjvxNegotiate ..
		node_output.terminate();
	}
	return res;
}

// =====================================================================================

jvxErrorType 
CjvxBareNode1ioRearrange::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxBareNode1io::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		node_output.reset();
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
CjvxBareNode1ioRearrange::accept_input_parameters_stop(jvxErrorType resTest)
{
	if (resTest == JVX_NO_ERROR)
	{
		output_params_from_ldesc_on_test();
	}
	return resTest;
}

jvxErrorType
CjvxBareNode1ioRearrange::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return JVX_NO_ERROR;
}

void
CjvxBareNode1ioRearrange::test_set_output_parameters()
{
	// Set the appropriate link descriptor values according to output params
	update_ldesc_from_output_params_on_test();
	constrain_ldesc_from_neg_params(neg_output);
}

jvxErrorType
CjvxBareNode1ioRearrange::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld = (jvxLinkDataDescriptor*)data;
	jvxBool thereismismatch = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resCheck = JVX_NO_ERROR;
	std::string txt;
	jvxLinkDataDescriptor ldStartStop;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		// The local function at this place does nothing - forward is not foreseen in the default implementation
		res = accept_negotiate_output(tp, ld JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		switch(res)
		{
		case JVX_NO_ERROR:

			// There is only room to negotiate 
			res = neg_output._negotiate_transfer_backward_ocon(ld,
				&_common_set_ocon.theData_out, static_cast<IjvxObject*>(this), nullptr
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res == JVX_NO_ERROR)
			{
				// Derive the output parameters from the "modified" data params
				output_params_from_ldesc_on_test();
				test_set_output_parameters();
			}
			break;
		case JVX_ERROR_COMPROMISE:

			// If we have setup a compromise, the check is obsolete and would undo the found compromise:
			// If we are at this point, ld which may not be changed will hold outdated information!!
			/*
			res = neg_output._negotiate_transfer_backward_ocon(ld,
				&_common_set_ocon.theData_out, static_cast<IjvxObject*>(this), nullptr
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res == JVX_NO_ERROR)
			{
				// Derive the output parameters from the "modified" data params
				output_params_from_ldesc_on_test();
			
				// Reset the result to COMPROMISE as this is a stronger requirement
				res = JVX_ERROR_COMPROMISE;
			}
			*/
			test_set_output_parameters();
			break;
		default:
			break;
		}
		break;
	default:
		res = _transfer_backward_ocon(true, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

void
CjvxBareNode1ioRearrange::update_output_params_from_input_params()
{
	node_output._common_set_node_params_a_1io.buffersize = node_inout._common_set_node_params_a_1io.buffersize;
	node_output._common_set_node_params_a_1io.samplerate = node_inout._common_set_node_params_a_1io.samplerate;
	node_output._common_set_node_params_a_1io.format = node_inout._common_set_node_params_a_1io.format;
	node_output._common_set_node_params_a_1io.subformat = node_inout._common_set_node_params_a_1io.subformat;
	node_output._common_set_node_params_a_1io.segmentation.x = node_inout._common_set_node_params_a_1io.segmentation.x;
	node_output._common_set_node_params_a_1io.segmentation.y = node_inout._common_set_node_params_a_1io.segmentation.y;
	node_output._common_set_node_params_a_1io.number_channels = node_inout._common_set_node_params_a_1io.number_channels;	
	node_output._common_set_node_params_a_1io.data_flow = node_inout._common_set_node_params_a_1io.data_flow;
}

void
CjvxBareNode1ioRearrange::update_ldesc_from_output_params_on_test()
{
	_common_set_ocon.theData_out.con_params.number_channels = node_output._common_set_node_params_a_1io.number_channels;
	_common_set_ocon.theData_out.con_params.buffersize = node_output._common_set_node_params_a_1io.buffersize;
	_common_set_ocon.theData_out.con_params.rate = node_output._common_set_node_params_a_1io.samplerate;
	_common_set_ocon.theData_out.con_params.format = (jvxDataFormat)node_output._common_set_node_params_a_1io.format;
	_common_set_ocon.theData_out.con_params.format_group = (jvxDataFormatGroup)node_output._common_set_node_params_a_1io.subformat;
	_common_set_ocon.theData_out.con_params.segmentation.x = node_output._common_set_node_params_a_1io.segmentation.x;
	_common_set_ocon.theData_out.con_params.segmentation.y = node_output._common_set_node_params_a_1io.segmentation.y;
	_common_set_ocon.theData_out.con_params.data_flow = (jvxDataflow)node_output._common_set_node_params_a_1io.data_flow;
}

void
CjvxBareNode1ioRearrange::output_params_from_ldesc_on_test()
{
	node_output._common_set_node_params_a_1io.number_channels = JVX_SIZE_INT32(_common_set_ocon.theData_out.con_params.number_channels);
	node_output._common_set_node_params_a_1io.buffersize = JVX_SIZE_INT32(_common_set_ocon.theData_out.con_params.buffersize);
	node_output._common_set_node_params_a_1io.samplerate = JVX_SIZE_INT32(_common_set_ocon.theData_out.con_params.rate);
	node_output._common_set_node_params_a_1io.format = JVX_SIZE_INT16(_common_set_ocon.theData_out.con_params.format);
	node_output._common_set_node_params_a_1io.subformat = JVX_SIZE_INT16(_common_set_ocon.theData_out.con_params.format_group);
	node_output._common_set_node_params_a_1io.segmentation.x = JVX_SIZE_INT32(_common_set_ocon.theData_out.con_params.segmentation.x);
	node_output._common_set_node_params_a_1io.segmentation.y = JVX_SIZE_INT32(_common_set_ocon.theData_out.con_params.segmentation.y);
	node_output._common_set_node_params_a_1io.format_spec = _common_set_ocon.theData_out.con_params.format_spec.std_str();
	node_output._common_set_node_params_a_1io.data_flow = _common_set_ocon.theData_out.con_params.data_flow;
}

void
CjvxBareNode1ioRearrange::from_input_to_output()
{

	// Copy output properties from input properties
	update_output_params_from_input_params();
}