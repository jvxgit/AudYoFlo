#include "CjvxSpNDemultiplexer.h"

// =====================================================================================
// Connect / disconnect: the chain towards the output is connected when the first input
// connector is connected and disconnected when the last input connector is disconnected.
// =====================================================================================

jvxErrorType
CjvxSpNDemultiplexer::icon_connect(CjvxSpNDemultiplexerInputConnector* iconn, jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = iconn->CjvxSingleInputConnector::connect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (!output.isConnected)
		{
			res = connect_connect_ocon(jvxChainConnectArguments(theData->con_link.uIdConn,
				theData->con_link.master) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res != JVX_NO_ERROR)
			{
				iconn->CjvxSingleInputConnector::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				return res;
			}
			output.isConnected = true;
		}
		iconn->inChain = true;
		iconn->seenTest = false;
		update_monitor();
	}
	return res;
}

jvxErrorType
CjvxSpNDemultiplexer::icon_disconnect(CjvxSpNDemultiplexerInputConnector* iconn, jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxBool anyInChain = false;

	iconn->inChain = false;
	iconn->seenTest = false;
	for (auto& elm : inputConnectors)
	{
		anyInChain = anyInChain || elm->inChain;
	}

	if (!anyInChain && output.isConnected)
	{
		disconnect_connect_ocon(jvxChainConnectArguments(theData->con_link.uIdConn) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		output.isConnected = false;

		node_inout.reset();
		node_output.reset();
	}
	update_monitor();

	return iconn->CjvxSingleInputConnector::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

// =====================================================================================
// Negotiations
// =====================================================================================

jvxErrorType
CjvxSpNDemultiplexer::icon_test(CjvxSpNDemultiplexerInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isFirst = true;
	jvxBool allSeen = true;
	CjvxNegotiate_input negNeutral;

	if (iconn->seenTest)
	{
		// The connector has already been tested but the test on the output side was not reached:
		// the previous test run was not complete, a new run starts here.
		for (auto& elm : inputConnectors)
		{
			elm->seenTest = false;
		}
	}

	for (auto& elm : inputConnectors)
	{
		if (elm->inChain && elm->seenTest)
		{
			isFirst = false;
			break;
		}
	}

	// The first connector in a test run defines the processing parameters, all others need to follow.
	// The number of channels is not constrained.
	iconn->neg_input.preferred = negNeutral.preferred;
	if (!isFirst)
	{
		iconn->neg_input._update_parameters_fixed(
			JVX_SIZE_UNSELECTED,
			paramsReference.buffersize,
			paramsReference.rate,
			paramsReference.format,
			paramsReference.format_group,
			paramsReference.data_flow);
	}

	// This involves the negotiation with the previous component
	res = iconn->CjvxSingleInputConnector::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		for (auto& elm : inputConnectors)
		{
			elm->seenTest = false;
		}
		return res;
	}

	if (isFirst)
	{
		const jvxLinkDataDescriptor_con_params& paramsIn = iconn->_common_set_icon.theData_in->con_params;
		paramsReference.buffersize = paramsIn.buffersize;
		paramsReference.rate = paramsIn.rate;
		paramsReference.format = paramsIn.format;
		paramsReference.format_group = paramsIn.format_group;
		paramsReference.data_flow = paramsIn.data_flow;
		paramsReference.segmentation.x = paramsIn.segmentation.x;
		paramsReference.segmentation.y = paramsIn.segmentation.y;
		paramsReference.format_spec = paramsIn.format_spec;
		paramsReference.additional_flags = paramsIn.additional_flags;
	}
	iconn->seenTest = true;

	for (auto& elm : inputConnectors)
	{
		if (elm->inChain && !elm->seenTest)
		{
			allSeen = false;
			break;
		}
	}

	if (allSeen)
	{
		// All input connectors are involved, continue the test on the output side
		for (auto& elm : inputConnectors)
		{
			elm->seenTest = false;
		}
		res = test_output(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

void
CjvxSpNDemultiplexer::update_output_parameters()
{
	jvxSize numChannels = 0;
	jvxLinkDataDescriptor_con_params& paramsOut = _common_set_ocon.theData_out.con_params;

	// The output channels are composed of the channels of all connected input connectors
	for (auto& elm : inputConnectors)
	{
		if (elm->inChain && elm->_common_set_icon.theData_in)
		{
			elm->chanOffset = numChannels;
			numChannels += elm->_common_set_icon.theData_in->con_params.number_channels;
		}
	}

	paramsOut.number_channels = numChannels;
	paramsOut.buffersize = paramsReference.buffersize;
	paramsOut.rate = paramsReference.rate;
	paramsOut.format = paramsReference.format;
	paramsOut.format_group = paramsReference.format_group;
	paramsOut.data_flow = paramsReference.data_flow;
	paramsOut.segmentation.x = paramsReference.segmentation.x;
	paramsOut.segmentation.y = paramsReference.segmentation.y;
	paramsOut.format_spec = paramsReference.format_spec;
	paramsOut.additional_flags = paramsReference.additional_flags;

	// Derive the output properties, these are used in the base class when testing the output connector
	output_params_from_ldesc_on_test();

	// The input properties show the overall number of channels on the input side
	node_inout._common_set_node_params_a_1io = node_output._common_set_node_params_a_1io;

	// There is no degree of freedom on the output connector
	neg_output._update_parameters_fixed(
		paramsOut.number_channels,
		paramsOut.buffersize,
		paramsOut.rate,
		paramsOut.format,
		paramsOut.format_group,
		paramsOut.data_flow);
}

jvxErrorType
CjvxSpNDemultiplexer::test_output(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	update_output_parameters();

	// Function of the base class: constrains the output parameters and forwards the test
	return test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxSpNDemultiplexer::negotiate_output(jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor_con_params& paramsOut = _common_set_ocon.theData_out.con_params;

	if (preferredByOutput == nullptr)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	// The number of channels is the sum of all input channels and can not be negotiated.
	// All other parameters must be modified in all input streams.
	jvxBool requiresInputChange =
		(preferredByOutput->con_params.buffersize != paramsOut.buffersize) ||
		(preferredByOutput->con_params.rate != paramsOut.rate) ||
		(preferredByOutput->con_params.format != paramsOut.format) ||
		(preferredByOutput->con_params.format_group != paramsOut.format_group) ||
		(preferredByOutput->con_params.data_flow != paramsOut.data_flow) ||
		(preferredByOutput->con_params.segmentation.x != paramsOut.segmentation.x) ||
		(preferredByOutput->con_params.segmentation.y != paramsOut.segmentation.y);

	if (requiresInputChange)
	{
		jvxBool isFirst = true;
		if (!forward_complain)
		{
			return JVX_ERROR_UNSUPPORTED;
		}

		for (auto& elm : inputConnectors)
		{
			if (elm->inChain && elm->_common_set_icon.theData_in)
			{
				jvxLinkDataDescriptor_con_params& paramsIn = elm->_common_set_icon.theData_in->con_params;

				// Forward the request to the previous component, with the number of channels of this connector
				jvxLinkDataDescriptor ld_loc = *preferredByOutput;
				ld_loc.con_params.number_channels = paramsIn.number_channels;
				jvxErrorType resL = elm->transfer_backward_icon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &ld_loc JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (
					(resL != JVX_NO_ERROR) &&
					(resL != JVX_ERROR_COMPROMISE))
				{
					return resL;
				}

				if (isFirst)
				{
					// The first connector defines the processing parameters as in the test run
					paramsReference.buffersize = paramsIn.buffersize;
					paramsReference.rate = paramsIn.rate;
					paramsReference.format = paramsIn.format;
					paramsReference.format_group = paramsIn.format_group;
					paramsReference.data_flow = paramsIn.data_flow;
					paramsReference.segmentation.x = paramsIn.segmentation.x;
					paramsReference.segmentation.y = paramsIn.segmentation.y;
					paramsReference.format_spec = paramsIn.format_spec;
					paramsReference.additional_flags = paramsIn.additional_flags;
					isFirst = false;
				}
				else
				{
					// All input streams must end up with the same parameters
					if (
						(paramsReference.buffersize != paramsIn.buffersize) ||
						(paramsReference.rate != paramsIn.rate) ||
						(paramsReference.format != paramsIn.format) ||
						(paramsReference.format_group != paramsIn.format_group) ||
						(paramsReference.data_flow != paramsIn.data_flow))
					{
						return JVX_ERROR_UNSUPPORTED;
					}
				}
			}
		}

		// The input parameters may have been modified, derive the new output parameters
		update_output_parameters();
	}

	jvxBool mismatch =
		(preferredByOutput->con_params.number_channels != paramsOut.number_channels) ||
		(preferredByOutput->con_params.buffersize != paramsOut.buffersize) ||
		(preferredByOutput->con_params.rate != paramsOut.rate) ||
		(preferredByOutput->con_params.format != paramsOut.format) ||
		(preferredByOutput->con_params.format_group != paramsOut.format_group) ||
		(preferredByOutput->con_params.data_flow != paramsOut.data_flow) ||
		(preferredByOutput->con_params.segmentation.x != paramsOut.segmentation.x) ||
		(preferredByOutput->con_params.segmentation.y != paramsOut.segmentation.y);

	// In case of a mismatch, the successor finds the best we can offer in the link data of the output connector
	return (mismatch ? JVX_ERROR_COMPROMISE : JVX_NO_ERROR);
}

jvxErrorType
CjvxSpNDemultiplexer::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		res = negotiate_output((jvxLinkDataDescriptor*)data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;
	default:

		// All other requests are passed towards the previous components. The connector "default" on
		// the input side is not connected, hence, the implementation of the base class must not be used.
		// The request is passed along the input connectors until the first component accepts it.
		for (auto& elm : inputConnectors)
		{
			if (elm->inChain && elm->_common_set_icon.theData_in)
			{
				res = elm->transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res == JVX_NO_ERROR)
				{
					break;
				}
			}
		}
		break;
	}
	return res;
}

// =====================================================================================
// Chain iterator
// =====================================================================================

jvxErrorType
CjvxSpNDemultiplexer::icon_reference_next_handle(jvxSize idx, IjvxConnectionIterator** next, jvxApiString* nmOcon, jvxApiString* nmIcon)
{
	// The next element in the chain is the one connected to the output connector "default"
	return _reference_next_handle(idx, next, nmOcon, nmIcon);
}

// =====================================================================================
// Callbacks input connectors
// =====================================================================================

jvxErrorType
CjvxSpNDemultiplexer::report_selected_connector(CjvxSingleInputConnector* iconn)
{
	// Only in use for multi-connectors
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNDemultiplexer::report_unselected_connector(CjvxSingleInputConnector* iconn)
{
	// Only in use for multi-connectors
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNDemultiplexer::report_test_connector(CjvxSingleInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	// The constraints for the negotiation have been setup before the test was started
	return JVX_NO_ERROR;
}

void
CjvxSpNDemultiplexer::request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId)
{
	if (_common_set.theUniqueId)
	{
		CjvxSpNDemultiplexerInputConnector* iconnLoc = static_cast<CjvxSpNDemultiplexerInputConnector*>(iconn);
		std::string nm = _common_set.theDescriptor + "-in#" + jvx_size2String(iconnLoc->idx);
		_common_set.theUniqueId->obtain_unique_id(uId, nm.c_str());
	}
}

void
CjvxSpNDemultiplexer::release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId)
{
	if (_common_set.theUniqueId)
	{
		_common_set.theUniqueId->release_unique_id(uId);
	}
}

jvxErrorType
CjvxSpNDemultiplexer::report_started_connector(CjvxSingleInputConnector* iconn)
{
	static_cast<CjvxSpNDemultiplexerInputConnector*>(iconn)->isStarted = true;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNDemultiplexer::report_stopped_connector(CjvxSingleInputConnector* iconn)
{
	static_cast<CjvxSpNDemultiplexerInputConnector*>(iconn)->isStarted = false;
	return JVX_NO_ERROR;
}

void
CjvxSpNDemultiplexer::report_process_buffers(CjvxSingleInputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage)
{
	// Not in use: the input buffers are processed in <icon_process_buffers>
}

jvxErrorType
CjvxSpNDemultiplexer::report_transfer_connector(CjvxSingleInputConnector* iconn, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (tp == JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY)
	{
		jvxBool isReady = true;
		jvxErrorType res = this->is_ready(&isReady, (jvxApiString*)data);
		if ((res == JVX_NO_ERROR) && !isReady)
		{
			return JVX_ERROR_NOT_READY;
		}
	}

	// Continue on the output side. The call arrives for each connected input connector
	return transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}
