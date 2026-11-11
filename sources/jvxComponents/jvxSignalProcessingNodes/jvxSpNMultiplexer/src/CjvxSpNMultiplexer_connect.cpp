#include "CjvxSpNMultiplexer.h"

// =====================================================================================
// Connect / disconnect: the calls arrive at the input connector and are forwarded to all
// output connectors which are part of a bridge in the same connection.
// =====================================================================================

jvxErrorType
CjvxSpNMultiplexer::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	// The base class forwards to the output connector "default" which accepts to be not connected
	jvxErrorType res = CjvxBareNode1ioRearrange::connect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		for (auto& elm : outputConnectors)
		{
			elm->inChain = false;
			if (
				(elm->_common_set_ocon.conn_out != nullptr) &&
				(elm->_common_set_ocon.theCommon_from == _common_set_icon.theCommon_to))
			{
				res = elm->connect_connect_ocon(jvxChainConnectArguments(theData->con_link.uIdConn,
					theData->con_link.master) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res != JVX_NO_ERROR)
				{
					break;
				}
				elm->inChain = true;
			}
		}

		if (res != JVX_NO_ERROR)
		{
			// Undo what has been connected so far
			for (auto& elm : outputConnectors)
			{
				if (elm->inChain)
				{
					elm->disconnect_connect_ocon(jvxChainConnectArguments(theData->con_link.uIdConn) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					elm->inChain = false;
				}
			}
			CjvxBareNode1ioRearrange::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		update_monitor();
	}
	return res;
}

jvxErrorType
CjvxSpNMultiplexer::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	for (auto& elm : outputConnectors)
	{
		if (elm->inChain)
		{
			elm->disconnect_connect_ocon(jvxChainConnectArguments(theData->con_link.uIdConn) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			elm->inChain = false;
		}
	}
	update_monitor();
	return CjvxBareNode1ioRearrange::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

// =====================================================================================
// Negotiations
// =====================================================================================

void
CjvxSpNMultiplexer::update_channel_assignment()
{
	jvxSize numChannelsIn = node_inout._common_set_node_params_a_1io.number_channels;
	jvxSize width = genMultiplexer_node::config.channels_per_connector.value;
	if (width == 0)
	{
		// Distribute the input channels evenly
		if (outputConnectors.size())
		{
			width = numChannelsIn / outputConnectors.size();
		}
	}

	for (auto& elm : outputConnectors)
	{
		elm->chanOffset = elm->idx * width;
		elm->chanNum = 0;
		if (elm->chanOffset < numChannelsIn)
		{
			elm->chanNum = JVX_MIN(width, numChannelsIn - elm->chanOffset);
		}
	}
}

void
CjvxSpNMultiplexer::from_input_to_output()
{
	// All output parameters are identical to the input parameters..
	CjvxBareNode1ioRearrange::from_input_to_output();

	// .. except for the number of channels. The output properties show the channel bundle of the first connector
	update_channel_assignment();
	if (outputConnectors.size())
	{
		node_output._common_set_node_params_a_1io.number_channels = outputConnectors[0]->chanNum;
	}
}

void
CjvxSpNMultiplexer::update_output_parameters(CjvxSpNMultiplexerOutputConnector* oconn)
{
	jvxLinkDataDescriptor_con_params& paramsOut = oconn->_common_set_ocon.theData_out.con_params;
	const jvxLinkDataDescriptor_con_params& paramsIn = _common_set_icon.theData_in->con_params;

	paramsOut.number_channels = oconn->chanNum;
	paramsOut.buffersize = paramsIn.buffersize;
	paramsOut.rate = paramsIn.rate;
	paramsOut.format = paramsIn.format;
	paramsOut.format_group = paramsIn.format_group;
	paramsOut.data_flow = paramsIn.data_flow;
	paramsOut.segmentation.x = paramsIn.segmentation.x;
	paramsOut.segmentation.y = paramsIn.segmentation.y;
	paramsOut.format_spec = paramsIn.format_spec;
	paramsOut.additional_flags = paramsIn.additional_flags;

	// There is no degree of freedom on the output connector
	oconn->neg_output._update_parameters_fixed(
		paramsOut.number_channels,
		paramsOut.buffersize,
		paramsOut.rate,
		paramsOut.format,
		paramsOut.format_group,
		paramsOut.data_flow);
}

jvxErrorType
CjvxSpNMultiplexer::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Negotiate the input side, the output connector "default" is not connected
	jvxErrorType res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvxSize cntRuns = 0;
		jvxBool runAgain = true;
		while (runAgain)
		{
			runAgain = false;
			renegotiatedOnTest = false;

			update_channel_assignment();
			for (auto& elm : outputConnectors)
			{
				if (elm->inChain)
				{
					res = test_one_output_connector(elm JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res != JVX_NO_ERROR)
					{
						break;
					}

					if (renegotiatedOnTest)
					{
						// The successor has requested a modification of the processing parameters which was
						// accepted by the previous component. All output connectors must be tested again.
						cntRuns++;
						runAgain = (cntRuns <= outputConnectors.size());
						break;
					}
				}
			}
		}
		renegotiatedOnTest = false;
	}
	return res;
}

jvxErrorType
CjvxSpNMultiplexer::test_one_output_connector(CjvxSpNMultiplexerOutputConnector* oconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (oconn->chanNum == 0)
	{
		std::string txt = "Output connector <out#" + jvx_size2String(oconn->idx) +
			"> is connected but there are no input channels left for it: number input channels = " +
			jvx_size2String(node_inout._common_set_node_params_a_1io.number_channels) + ".";
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), JVX_ERROR_INVALID_SETTING);
		return JVX_ERROR_INVALID_SETTING;
	}

	update_output_parameters(oconn);
	return oconn->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxSpNMultiplexer::negotiate_one_output_connector(CjvxSpNMultiplexerOutputConnector* oconn, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor_con_params& paramsOut = oconn->_common_set_ocon.theData_out.con_params;

	if (preferredByOutput == nullptr)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	// The number of channels is given by the channel bundle and can not be negotiated.
	// All other parameters must be modified on the input side.
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
		if (!forward_complain || (_common_set_icon.theData_in == nullptr))
		{
			return JVX_ERROR_UNSUPPORTED;
		}

		// Forward the request to the previous component, with the number of channels on the input side
		jvxLinkDataDescriptor ld_loc = *preferredByOutput;
		ld_loc.con_params.number_channels = _common_set_icon.theData_in->con_params.number_channels;
		res = transfer_backward_icon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &ld_loc JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (
			(res != JVX_NO_ERROR) &&
			(res != JVX_ERROR_COMPROMISE))
		{
			return res;
		}

		// The input parameters may have been modified, accept the new setting..
		res = accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			return res;
		}

		// .. and derive the parameters for this connector. The other connectors follow
		// since the test run is repeated
		update_channel_assignment();
		update_output_parameters(oconn);
		renegotiatedOnTest = true;
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

	// In case of a mismatch, the successor finds the best we can offer in the link data of the connector
	return (mismatch ? JVX_ERROR_COMPROMISE : JVX_NO_ERROR);
}

jvxErrorType
CjvxSpNMultiplexer::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		for (auto& elm : outputConnectors)
		{
			if (elm->inChain)
			{
				jvxErrorType resL = elm->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res == JVX_NO_ERROR)
				{
					res = resL;
				}
			}
		}
	}
	return res;
}

// =====================================================================================
// Chain iterator
// =====================================================================================

jvxErrorType
CjvxSpNMultiplexer::number_next(jvxSize* num)
{
	if (num)
	{
		*num = 0;
		for (auto& elm : outputConnectors)
		{
			if (elm->inChain)
			{
				(*num)++;
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNMultiplexer::reference_next_handle(jvxSize idx, IjvxConnectionIterator** next, jvxApiString* nmOcon, jvxApiString* nmIcon)
{
	jvxSize cnt = 0;
	for (auto& elm : outputConnectors)
	{
		if (elm->inChain)
		{
			if (cnt == idx)
			{
				return elm->reference_next_handle(0, next, nmOcon, nmIcon);
			}
			cnt++;
		}
	}
	if (next)
	{
		*next = nullptr;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

// =====================================================================================
// Callbacks output connectors
// =====================================================================================

jvxErrorType
CjvxSpNMultiplexer::report_selected_connector(CjvxSingleOutputConnector* oconn)
{
	// Only in use for multi-connectors
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNMultiplexer::report_unselected_connector(CjvxSingleOutputConnector* oconn)
{
	// Only in use for multi-connectors
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNMultiplexer::report_test_connector(CjvxSingleOutputConnector* oconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	// The output parameters have been setup before the test was started
	return JVX_NO_ERROR;
}

void
CjvxSpNMultiplexer::request_unique_id_start(CjvxSingleOutputConnector* oconn, jvxSize* uId)
{
	if (_common_set.theUniqueId)
	{
		CjvxSpNMultiplexerOutputConnector* oconnLoc = static_cast<CjvxSpNMultiplexerOutputConnector*>(oconn);
		std::string nm = _common_set.theDescriptor + "-out#" + jvx_size2String(oconnLoc->idx);
		_common_set.theUniqueId->obtain_unique_id(uId, nm.c_str());
	}
}

void
CjvxSpNMultiplexer::release_unique_id_stop(CjvxSingleOutputConnector* oconn, jvxSize uId)
{
	if (_common_set.theUniqueId)
	{
		_common_set.theUniqueId->release_unique_id(uId);
	}
}

jvxErrorType
CjvxSpNMultiplexer::report_started_connector(CjvxSingleOutputConnector* oconn)
{
	static_cast<CjvxSpNMultiplexerOutputConnector*>(oconn)->isStarted = true;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNMultiplexer::report_stopped_connector(CjvxSingleOutputConnector* oconn)
{
	static_cast<CjvxSpNMultiplexerOutputConnector*>(oconn)->isStarted = false;
	return JVX_NO_ERROR;
}

void
CjvxSpNMultiplexer::report_process_buffers(CjvxSingleOutputConnector* oconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage)
{
	// Not in use: the output buffers are filled in <process_buffers_icon>
}

jvxErrorType
CjvxSpNMultiplexer::report_transfer_connector(CjvxSingleOutputConnector* oconn, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		res = negotiate_one_output_connector(static_cast<CjvxSpNMultiplexerOutputConnector*>(oconn),
			(jvxLinkDataDescriptor*)data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;
	default:

		// All other requests are passed towards the previous component
		if (_common_set_icon.theData_in)
		{
			res = transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		break;
	}
	return res;
}
