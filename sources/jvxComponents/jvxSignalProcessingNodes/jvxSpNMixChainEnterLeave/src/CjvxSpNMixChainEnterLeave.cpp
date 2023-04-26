#include "CjvxSpNMixChainEnterLeave.h"

CjvxSpNMixChainEnterLeave::CjvxSpNMixChainEnterLeave(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	forward_complain = true;
	_common_set_ldslave.zeroCopyBuffering_cfg = true;

	JVX_INITIALIZE_MUTEX(safeAccessInputsOutputs);
}

CjvxSpNMixChainEnterLeave::~CjvxSpNMixChainEnterLeave()
{
	JVX_TERMINATE_MUTEX(safeAccessInputsOutputs);

	assert(inputConnectors.selectedInputConnectors.size() == 0);
	assert(inputConnectors.processingInputConnectors.size() == 0);
	assert(inputConnectors.extra_icon_gen == nullptr);
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::select(IjvxObject* owner)
{
	auto res = CjvxBareNode1ioRearrange::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genMixChain::init__config();
		genMixChain::allocate__config();
		genMixChain::register__config(this);

		genMixChain::register_callbacks(this, set_on_config, this);
		
		operationMode = genMixChain::translate__config__operation_mode_from();
	}
	return res;
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::unselect()
{
	auto res = CjvxBareNode1ioRearrange::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		genMixChain::unregister_callbacks(this);
		genMixChain::unregister__config(this); 
		genMixChain::deallocate__config();
		
		CjvxBareNode1ioRearrange::unselect();
	}
	return res;
}


jvxErrorType
CjvxSpNMixChainEnterLeave::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		CjvxConnectorFactory::oneInputConnectorElement elmIn;
		CjvxConnectorFactory::oneOutputConnectorElement elmOut;

		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genMixChain::config.nickname);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genMixChain::config.number_channels_side);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genMixChain::config.operation_mode);

		switch (operationMode)
		{
		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT:
			inputConnectors.inConName = "mix-in";
			JVX_SAFE_ALLOCATE_OBJECT(inputConnectors.extra_icon_gen, CjvxSingleInputConnectorMulti);
			inputConnectors.extra_icon_gen->activate(this, this, inputConnectors.inConName, this);
			elmIn.theConnector = inputConnectors.extra_icon_gen;

			// Functional part "CjvxConnectorFactory"
			_common_set_conn_factory.input_connectors[inputConnectors.extra_icon_gen] = elmIn;
			break;
		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:
			outputConnectors.outConName = "mix-out";
			JVX_SAFE_ALLOCATE_OBJECT(outputConnectors.extra_ocon_gen, CjvxSingleOutputConnector);
			outputConnectors.extra_ocon_gen->activate(this, this, outputConnectors.outConName, this);
			elmOut.theConnector = outputConnectors.extra_ocon_gen;
			// Functional part "CjvxConnectorFactory"
			_common_set_conn_factory.output_connectors[outputConnectors.extra_ocon_gen] = elmOut;
			break;
		}
	}
	return res;
}

jvxErrorType
CjvxSpNMixChainEnterLeave::deactivate()
{
	std::map<IjvxInputConnectorSelect*, oneInputConnectorElement>::iterator elmI;
	std::map<IjvxOutputConnectorSelect*, oneOutputConnectorElement>::iterator elmO;
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		switch (operationMode)
		{
		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT:
			
			// Functional part "CjvxConnectorFactory"
			elmI = _common_set_conn_factory.input_connectors.find(inputConnectors.extra_icon_gen);
			if (elmI != _common_set_conn_factory.input_connectors.end())
			{
				_common_set_conn_factory.input_connectors.erase(elmI);
			}

			inputConnectors.extra_icon_gen->deactivate();
			JVX_SAFE_DELETE_OBJECT(inputConnectors.extra_icon_gen);
			break;

		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:
			
			// Functional part "CjvxConnectorFactory"
			elmO = _common_set_conn_factory.output_connectors.find(outputConnectors.extra_ocon_gen);
			if (elmO != _common_set_conn_factory.output_connectors.end())
			{
				_common_set_conn_factory.output_connectors.erase(elmO);
			}

			outputConnectors.extra_ocon_gen->deactivate();
			JVX_SAFE_DELETE_OBJECT(outputConnectors.extra_ocon_gen);
			break;
		}
		_undo_update_property_access_type(genMixChain::config.nickname);
		_undo_update_property_access_type(genMixChain::config.number_channels_side);
		_undo_update_property_access_type(genMixChain::config.operation_mode);
		res = CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

void
CjvxSpNMixChainEnterLeave::from_input_to_output()
{
	CjvxBareNode1ioRearrange::from_input_to_output();

	switch (operationMode)
	{
	case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT:
		node_output._common_set_node_params_a_1io.number_channels += genMixChain::config.number_channels_side.value;
		break;
	case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:
		node_output._common_set_node_params_a_1io.number_channels -= genMixChain::config.number_channels_side.value;
		break;
	default:

		// Talkthrough
		// node_output._common_set_node_params_a_1io.number_channels += 0;
		break;
	}
}

jvxErrorType
CjvxSpNMixChainEnterLeave::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	// Forward the complaint
	// Check what comes in from successor

	jvxLinkDataDescriptor ld_loc = *preferredByOutput;

	// Froward without side channels
	if (ld_loc.con_params.number_channels >= genMixChain::config.number_channels_side.value)
	{
		switch (operationMode)
		{
		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT:
			ld_loc.con_params.number_channels -= genMixChain::config.number_channels_side.value;
			break;
		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:
			ld_loc.con_params.number_channels += genMixChain::config.number_channels_side.value;
			break;
		default:
			// Talkthrough
			// ld_loc.con_params.number_channels += 0;
			break;
		}

		// Use default implementation
		return CjvxNodeBase1io::accept_negotiate_output(tp, &ld_loc JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}

	// If the number of output channels is too small, try to insist on the previous number and return it as a "compromise"
	return JVX_ERROR_COMPROMISE;
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i,j;
	auto res = CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Lock some of the properties
		_update_properties_on_start();

		if (operationMode != jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_TALKTHROUGH)
		{
			// Here, we have seen the following copy before:
			// _common_set_icon.theData_in->con_data = _common_set_ocon.theData_out.con_data;
			// We need to create out own container here, however, with non filled buffers
			// We need to override the buffer values which are currently there since the allocator checks
			// for nullptr
			jvxSize numChansCopy = JVX_MIN(
				_common_set_icon.theData_in->con_params.number_channels,
				_common_set_ocon.theData_out.con_params.number_channels);

			// Reallocate all buffers taken from the output side
			jvx_allocateDataLinkDescriptorRouteChannels(
				_common_set_icon.theData_in,
				&_common_set_ocon.theData_out,
				numChansCopy,
				&bufsToStore);

			switch (operationMode)
			{
			case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT:

				assert(_common_set_icon.theData_in->con_params.number_channels <= _common_set_ocon.theData_out.con_params.number_channels);

				szExtraBuffersChannels = _common_set_ocon.theData_out.con_params.number_channels - _common_set_icon.theData_in->con_params.number_channels;
				if (_common_set_icon.theData_in->con_data.number_buffers)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(bufsSideChannel, jvxHandle**, _common_set_icon.theData_in->con_data.number_buffers);
				}

				// Check the other parameters if you want to..

				// Now, we have to copy the channel buffers
				for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(bufsSideChannel[i], jvxHandle*, szExtraBuffersChannels);
					assert(bufsSideChannel[i]);
					for (j = 0; j < szExtraBuffersChannels; j++)
					{
						bufsSideChannel[i][j] = _common_set_ocon.theData_out.con_data.buffers[i][j + _common_set_icon.theData_in->con_params.number_channels];
					}
				}
				break;

			case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:

				assert(_common_set_icon.theData_in->con_params.number_channels >= _common_set_ocon.theData_out.con_params.number_channels);

				szExtraBuffersChannels = _common_set_icon.theData_in->con_params.number_channels - _common_set_ocon.theData_out.con_params.number_channels;
				if (_common_set_icon.theData_in->con_data.number_buffers)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(bufsSideChannel, jvxHandle**, _common_set_icon.theData_in->con_data.number_buffers);
				}

				// Check the other parameters if you want to..

				// Now, we have to copy the channel buffers to be available in the previous component
				for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(bufsSideChannel[i], jvxHandle*, szExtraBuffersChannels);
					assert(bufsSideChannel[i]);
					for (j = 0; j < szExtraBuffersChannels; j++)
					{
						JVX_DSP_SAFE_ALLOCATE_FIELD(bufsSideChannel[i][j], jvxByte, jvxDataFormat_getsize(_common_set_icon.theData_in->con_params.format) * _common_set_icon.theData_in->con_params.buffersize);
						_common_set_icon.theData_in->con_data.buffers[i][j + _common_set_ocon.theData_out.con_params.number_channels] = bufsSideChannel[i][j];
					}
				}
				break;
			}
		}
	}
	return res;
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i, j;

	if (operationMode != jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_TALKTHROUGH)
	{
		switch (operationMode)
		{
		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT:

			// Now, we have to copy the channel buffers
			for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
			{
				for (j = 0; j < szExtraBuffersChannels; j++)
				{
					bufsSideChannel[i][j] = nullptr;
				}
				JVX_DSP_SAFE_DELETE_FIELD(bufsSideChannel[i]);
			}

			if (_common_set_icon.theData_in->con_data.number_buffers)
			{
				JVX_DSP_SAFE_DELETE_FIELD(bufsSideChannel);
			}
			szExtraBuffersChannels = 0;
			break;
			// Check the other parameters if you want to..

		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:

			// Now, we have to copy the channel buffers
			for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
			{
				for (j = 0; j < szExtraBuffersChannels; j++)
				{
					_common_set_icon.theData_in->con_data.buffers[i][j + _common_set_ocon.theData_out.con_params.number_channels] = nullptr;
					JVX_DSP_SAFE_DELETE_FIELD(bufsSideChannel[i][j]);					
				}
				JVX_DSP_SAFE_DELETE_FIELD(bufsSideChannel[i]);
			}

			if (_common_set_icon.theData_in->con_data.number_buffers)
			{
				JVX_DSP_SAFE_DELETE_FIELD(bufsSideChannel);
			}
			szExtraBuffersChannels = 0;
			break;

		}

		// Delete the buffers
		jvx_deallocateDataLinkDescriptorRouteChannels(_common_set_icon.theData_in, bufsToStore);
	}

	// Unlock some of the properties
	_update_properties_on_stop();

	res = CjvxBareNode1ioRearrange::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxHandle** bufsOut = nullptr;
	// Browse through the list of the active streams and request data
	switch (operationMode)
	{
	case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT:

		// Prepare buffer on output side. Set overall values to 0!!
		idxBufferProcess = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;
		bufsOut = bufsSideChannel[idxBufferProcess];
		for (i = 0; i < szExtraBuffersChannels; i++)
		{
			memset(bufsOut[i], 0, jvxDataFormat_getsize(_common_set_ocon.theData_out.con_params.format) * _common_set_ocon.theData_out.con_params.buffersize);
		}

		JVX_LOCK_MUTEX(safeAccessInputsOutputs);
		for (auto& elm : inputConnectors.processingInputConnectors)
		{
			elm.second.ioconn->trigger_get_data();
		}
		JVX_UNLOCK_MUTEX(safeAccessInputsOutputs);
		break;
	case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:
		break;
	}

	auto res = CjvxBareNode1ioRearrange::process_buffers_icon(mt_mask, idx_stage);
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMixChainEnterLeave, set_on_config)
{
	jvxBool triggerTest = false;
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genMixChain::config.number_channels_side))
	{
		// New number of channels, run the test function
		triggerTest = true;
	}
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genMixChain::config.operation_mode))
	{
		operationMode = genMixChain::translate__config__operation_mode_from();
		triggerTest = true;
	}

	if (triggerTest)
	{
		this->inform_chain_test();
	}

	return JVX_NO_ERROR;
}

