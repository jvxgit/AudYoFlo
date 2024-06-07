#include "CjvxSpNMixChainEnterLeave.h"

CjvxSpNMixChainEnterLeave::CjvxSpNMixChainEnterLeave(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	forward_complain = true;
	_common_set_ldslave.zeroCopyBuffering_cfg = true;

	CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::cbRef = this;
	CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::cbRef = this;
}

CjvxSpNMixChainEnterLeave::~CjvxSpNMixChainEnterLeave()
{
	

	assert((CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors).size() == 0);
	assert((CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::processingConnectors).size() == 0);
	assert((CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen) == nullptr);

	assert((CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::selectedConnectors).size() == 0);
	assert((CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::processingConnectors).size() == 0);
	assert((CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen) == nullptr);
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

		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genMixChain::config.number_channels_side);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genMixChain::config.operation_mode);

		switch (operationMode)
		{
		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT:
			CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::conName = "mix-in";
			JVX_SAFE_ALLOCATE_OBJECT((CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen), 
				CjvxSingleInputConnectorMulti(true));
			CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen->activate(this, this, 
				CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::conName, this, 0);
			elmIn.theConnector = CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen;

			// Functional part "CjvxConnectorFactory"
			_common_set_conn_factory.input_connectors[CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen] = elmIn;
			break;
		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:
			CjvxConnectorCollection < CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::conName = "mix-out";
			JVX_SAFE_ALLOCATE_OBJECT((CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen), CjvxSingleOutputConnectorMulti(true));
			CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen->activate(this, this, 
				CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::conName, this, 0);
			elmOut.theConnector = CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen;
			// Functional part "CjvxConnectorFactory"
			_common_set_conn_factory.output_connectors[CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen] = elmOut;
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
			elmI = _common_set_conn_factory.input_connectors.find(CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen);
			if (elmI != _common_set_conn_factory.input_connectors.end())
			{
				_common_set_conn_factory.input_connectors.erase(elmI);
			}

			CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen->deactivate();
			JVX_SAFE_DELETE_OBJECT((CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen));
			break;

		case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:
			
			// Functional part "CjvxConnectorFactory"
			elmO = _common_set_conn_factory.output_connectors.find(CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen);
			if (elmO != _common_set_conn_factory.output_connectors.end())
			{
				_common_set_conn_factory.output_connectors.erase(elmO);
			}

			CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen->deactivate();
			JVX_SAFE_DELETE_OBJECT((CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen));
			break;
		}
		_undo_update_property_access_type(genMixChain::config.number_channels_side);
		_undo_update_property_access_type(genMixChain::config.operation_mode);
		res = CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvxSize numChansMax = JVX_MAX(
			node_output._common_set_node_params_a_1io.number_channels,
			node_inout._common_set_node_params_a_1io.number_channels);
		if (numChansMax != szChannelRoutes)
		{
			jvxSize i;
			jvxSize* fldNew = nullptr;
			jvxSize fldNewSz = numChansMax;

			if (szChannelRoutes)
			{
				genMixChain::deassociate__config(this);
			}

			if (szChannelRoutes)
			{
				std::vector<jvxSize> lstNew = oldRouting;
				if (oldRouting.size() < szChannelRoutes)
				{
					oldRouting.resize(szChannelRoutes);
				}

				for (i = 0; i < szChannelRoutes; i++)
				{					
					oldRouting[i] = ptrChannelRoutes[i];
				}

				for (; i < lstNew.size(); i++)
				{
					oldRouting[i] = lstNew[i];
				}

				correct_order_channel_route(oldRouting.data(), oldRouting.size());

				JVX_SAFE_DELETE_FIELD(ptrChannelRoutes);
				szChannelRoutes = 0;
			}

			if (fldNewSz)
			{
				JVX_SAFE_ALLOCATE_FIELD_CPP_Z(fldNew, jvxSize, fldNewSz);
				for (i = 0; i < fldNewSz; i++)
				{
					fldNew[i] = i;
					if (i < oldRouting.size())
					{
						fldNew[i] = oldRouting[i];
					}
				}


			}

			szChannelRoutes = fldNewSz;
			ptrChannelRoutes = fldNew;
			correct_order_channel_route(ptrChannelRoutes, szChannelRoutes);

			oldRouting.resize(fldNewSz);
			if (fldNewSz)
			{
				memcpy(oldRouting.data(), ptrChannelRoutes, fldNewSz * sizeof(jvxSize));
			}
			genMixChain::associate__config(this, ptrChannelRoutes, szChannelRoutes);
		}

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
		node_output._common_set_node_params_a_1io.number_channels += (jvxInt32)genMixChain::config.number_channels_side.value;
		break;
	case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:
		node_output._common_set_node_params_a_1io.number_channels -= (jvxInt32)genMixChain::config.number_channels_side.value;
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

			jvxBool routeOutputToInput = true;

			switch (operationMode)
			{
			case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT:
				routeOutputToInput = false;
				break;
			case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:
				routeOutputToInput = true;
				break;
			}

			// Reallocate all buffers taken from the output side
			jvx_allocateDataLinkDescriptorRouteChannels(
				_common_set_icon.theData_in,
				&_common_set_ocon.theData_out,
				numChansCopy,
				&bufsToStore, ptrChannelRoutes, szChannelRoutes, routeOutputToInput);

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
						jvxSize writeFrom = j + _common_set_icon.theData_in->con_params.number_channels;
						if (writeFrom < szChannelRoutes)
						{
							writeFrom = ptrChannelRoutes[writeFrom];
						}
						writeFrom = JVX_MIN(writeFrom, _common_set_ocon.theData_out.con_params.number_channels - 1);
						bufsSideChannel[i][j] = _common_set_ocon.theData_out.con_data.buffers[i][writeFrom];
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
						jvxSize writeTo = j + _common_set_ocon.theData_out.con_params.number_channels;						
						if (writeTo < szChannelRoutes)
						{
							writeTo = ptrChannelRoutes[writeTo];
						}
						writeTo = JVX_MIN(writeTo, _common_set_icon.theData_in->con_params.number_channels - 1);
						_common_set_icon.theData_in->con_data.buffers[i][writeTo] = bufsSideChannel[i][j];
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
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(bufsSideChannel[i][j], jvxByte);
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

		lock();		
		for (auto& elm : CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::processingConnectors)
		{
			elm.second.ioconn->setLatestResultGetData(elm.second.ioconn->trigger_get_data());
		}
		unlock();
		break;
	case jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT:
		idxBufferProcess = idx_stage;
		if (JVX_CHECK_SIZE_UNSELECTED(idxBufferProcess))
		{
			idxBufferProcess = *_common_set_icon.theData_in->con_pipeline.idx_stage_ptr;
		}

		lock();
		for (auto& elm : CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::processingConnectors)
		{
			jvxErrorType resOther = elm.second.ioconn->resultFromInputConnector();
			if(
				(resOther == JVX_ERROR_UNKNOWN) || 
				resOther == JVX_NO_ERROR)
				elm.second.ioconn->trigger_put_data();
		}
		unlock();
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

	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genMixChain::config.channel_routing))
	{
		// Correct channel order
		correct_order_channel_route(ptrChannelRoutes, szChannelRoutes);
		
		// Update channel routings
		if (szChannelRoutes)
		{
			memcpy(oldRouting.data(), ptrChannelRoutes, szChannelRoutes * sizeof(jvxSize));
		}

		operationMode = genMixChain::translate__config__operation_mode_from();
		triggerTest = true;
	}

	if (triggerTest)
	{
		this->inform_chain_test();
	}

	return JVX_NO_ERROR;
}

void 
CjvxSpNMixChainEnterLeave::correct_order_channel_route(jvxSize* ptrChannelRoutes, jvxSize szChannelRoutes)
{
	jvxSize i;
	jvxCBitField chanSelCorrect = (1 << szChannelRoutes) - 1;
	for (i = 0; i < szChannelRoutes; i++)
	{
		jvxSize idx = ptrChannelRoutes[i];
		if (!jvx_bitTest(chanSelCorrect, idx))
		{
			// Here, we need to correct
			jvxSize j;
			idx = JVX_SIZE_UNSELECTED;
			for (j = 0; j < szChannelRoutes; j++)
			{
				if (jvx_bitTest(chanSelCorrect, j))
				{
					idx = j;
					break;
				}
			}
			assert(JVX_CHECK_SIZE_SELECTED(idx));
		}

		// CHeck this position
		jvx_bitClear(chanSelCorrect, idx);
		ptrChannelRoutes[i] = idx;
	}
}

jvxErrorType
CjvxSpNMixChainEnterLeave::set_manipulate_value(jvxSize id, jvxVariant* varray)
{
	jvxApiString* ptrString = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	if (!varray)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (id)
	{
	case JVX_MANIPULATE_DESCRIPTION:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			_common_set_min.theDescription = ptrString->std_str();
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	case JVX_MANIPULATE_DESCRIPTOR:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			_common_set_min.theDescription = ptrString->std_str();
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

jvxErrorType
CjvxSpNMixChainEnterLeave::get_manipulate_value(jvxSize id, jvxVariant* varray)
{
	jvxApiString* ptrString = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	if (!varray)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (id)
	{
	case JVX_MANIPULATE_DESCRIPTION:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			ptrString->assign(_common_set_min.theDescription);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	case JVX_MANIPULATE_DESCRIPTOR:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			ptrString->assign(_common_set_min.theDescription);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	switch (tp)
	{
	case JVX_INTERFACE_MANIPULATE:
		if (hdl) *hdl = static_cast<IjvxManipulate*>(this);
		return JVX_NO_ERROR;
	default:
		break;
	}
	return CjvxBareNode1ioRearrange::request_hidden_interface(tp, hdl);
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	switch (tp)
	{
	case JVX_INTERFACE_MANIPULATE:
		if (hdl == static_cast<IjvxManipulate*>(this))
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_ARGUMENT;
	default:
		break;
	}
	return CjvxBareNode1ioRearrange::return_hidden_interface(tp, hdl);
}

bool 
CjvxSpNMixChainEnterLeave::check_positive_zero_copy()
{
	return (
		(_common_set_ocon.theData_out.con_params.buffersize == node_inout._common_set_node_params_a_1io.buffersize) &&
		(_common_set_ocon.theData_out.con_params.format == node_inout._common_set_node_params_a_1io.format) &&
		(_common_set_ocon.theData_out.con_params.rate == node_inout._common_set_node_params_a_1io.samplerate));// &&
		//(_common_set_ocon.theData_out.con_params.number_channels == node_inout._common_set_node_params_a_1io.number_channels));
}
