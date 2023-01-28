#include "jvxNodes/CjvxBareNode1io_zerocopy.h"

CjvxBareNode1io_zerocopy::CjvxBareNode1io_zerocopy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_ldslave.zeroCopyBuffering_cfg = true;

	_common_set_node_base_1io.prepareOnChainPrepare = true;
	_common_set_node_base_1io.startOnChainStart = true;
	_common_set_node_base_1io.impPrepareOnChainPrepare = true;
	_common_set_node_base_1io.impStartOnChainStart = true;

	_common_set_1io_zerocopy.num_iochans = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.bsize_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.rate_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.form_io = JVX_DATAFORMAT_NONE;
	_common_set_1io_zerocopy.subform_io = JVX_DATAFORMAT_GROUP_NONE;

	_common_set_1io_zerocopy.forward_complain = true;
}

jvxErrorType 
CjvxBareNode1io_zerocopy::activate()
{
	jvxErrorType res = CjvxBareNode1io::activate();
	if (res == JVX_NO_ERROR)
	{
		// Set the codec parameters based on the properties
		if (!newParamProps)
		{
			neg_input._set_parameters_fixed(
				_common_set_1io_zerocopy.num_iochans,
				_common_set_1io_zerocopy.bsize_io,
				_common_set_1io_zerocopy.rate_io,
				_common_set_1io_zerocopy.form_io,
				_common_set_1io_zerocopy.subform_io);
			neg_output._set_parameters_fixed(
				_common_set_1io_zerocopy.num_iochans,
				_common_set_1io_zerocopy.bsize_io,
				_common_set_1io_zerocopy.rate_io,
				_common_set_1io_zerocopy.form_io,
				_common_set_1io_zerocopy.subform_io);
		}
	}
	return res;
}

jvxErrorType
CjvxBareNode1io_zerocopy::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (_common_set_1io_zerocopy.forward_complain)
	{
		jvxLinkDataDescriptor ld_loc = *preferredByOutput;
		jvxErrorType resL = transfer_backward_icon(tp, &ld_loc  JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (
			(resL == JVX_NO_ERROR) ||
			(resL == JVX_ERROR_COMPROMISE))
		{
			accept_input_parameters_stop(resL);
			accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBareNode1io_zerocopy::accept_input_parameters_stop(jvxErrorType resTest)
{
	if (resTest == JVX_NO_ERROR)
	{
		update_simple_params_from_ldesc();
	}
	else
	{
		// We might try to negotiate to the successor!
	}
	neg_output._pop_constraints();
	return resTest;
}

// Code fragment to copy input arguments towards output
void
CjvxBareNode1io_zerocopy::from_input_to_output()
{
	// Since we have a zero copy, outputand input number of channels must be identical
	if (newParamProps)
	{
		// CjvxSimplePropsPars::node.numberoutputchannels = CjvxSimplePropsPars::node.numberinputchannels;
		neg_output._push_constraints();

		// Update also the data processing parameters to hold the right value
		neg_output._update_parameters_fixed(_common_set_node_params_a_1io.number_input_channels,
			_common_set_node_params_a_1io.buffersize,
			_common_set_node_params_a_1io.samplerate,
			(jvxDataFormat)_common_set_node_params_a_1io.format,
			(jvxDataFormatGroup)_common_set_node_params_a_1io.subformat,
			&_common_set_ldslave.theData_out);
	}
	else
	{
		CjvxNode_genpcg::node.numberoutputchannels.value = CjvxNode_genpcg::node.numberinputchannels.value;
		neg_output._push_constraints();

		// Update also the data processing parameters to hold the right value
		neg_output._update_parameters_fixed(CjvxNode_genpcg::node.numberoutputchannels.value,
			CjvxNode_genpcg::node.buffersize.value,
			CjvxNode_genpcg::node.samplerate.value,
			(jvxDataFormat)CjvxNode_genpcg::node.format.value,
			(jvxDataFormatGroup)CjvxNode_genpcg::node.subformat.value,
			&_common_set_ldslave.theData_out);
	}
}
