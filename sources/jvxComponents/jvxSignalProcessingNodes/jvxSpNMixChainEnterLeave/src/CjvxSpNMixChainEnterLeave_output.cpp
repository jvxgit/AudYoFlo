#include "CjvxSpNMixChainEnterLeave.h"

jvxErrorType
CjvxSpNMixChainEnterLeave::report_selected_connector(CjvxSingleOutputConnector* oconn)
{
	oneConnectorPlusName<CjvxSingleOutputConnector> addme;
	addme.ioconn = oconn;
	addme.nmUnique = _common_set_min.theDescription + "-" + CjvxConnectorCollection < CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::conName + "_" + jvx_size2String(uIdGen++);

	CjvxConnectorCollection < CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::selectedConnectors[oconn] = addme;
	if (_common_set_icon.theData_in)
	{
		// All parameters as desired
		jvxLinkDataDescriptor_con_params params = _common_set_icon.theData_in->con_params;

		// Number of channels differs
		params.number_channels = genMixChain::config.number_channels_side.value;
		params.data_flow = JVX_DATAFLOW_PUSH_ON_PULL;

		// Pass it to the connector but do not request a test of the chain - it is in build status anyway
		oconn->updateFixedProcessingArgs(params);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNMixChainEnterLeave::report_test_connector(CjvxSingleOutputConnector* oconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxComponentIdentification cpId;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	if (oconn->_common_set_ocon.theData_out.con_link.connect_to)
	{
		if (oconn->_common_set_ocon.theData_out.con_link.connect_to)
		{
			if (oconn->_common_set_ocon.theData_out.con_link.connect_to->transfer_forward_icon(jvxLinkDataTransferType::JVX_LINKDATA_TRANSFER_REQUEST_REAL_MASTER, &cpId  JVX_CONNECTION_FEEDBACK_CALL_A(fdb)) == JVX_NO_ERROR)
			{
				res = check_preset_channels(oconn->chanSetting, oconn->linkageIoActive, cpId);
			}
		}
	}
	return res;
}

// This function is called for each single connection. We may search for the entry in the map.
// The output field is passed in bufferPtrs and the corresponding parameters in params.
void
CjvxSpNMixChainEnterLeave::report_process_buffers(CjvxSingleOutputConnector* oconn, jvxLinkDataDescriptor& datOutThisConnector, jvxSize idxStage)
{
	jvxSize i;

	jvxHandle** bufferPtrs = nullptr;
	// const jvxLinkDataDescriptor_con_params& params

	jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(datOutThisConnector);

	assert(JVX_CHECK_SIZE_SELECTED(idxBufferProcess));

	// Check that the buffersize matches: secondary output connector and primary output buffers
	assert(datOutThisConnector.con_params.buffersize == _common_set_ocon.theData_out.con_params.buffersize);
	// assert(params.number_channels == szExtraBuffersChannels);
	
	// Check that the format matches, secondary output connector and primary output buffers
	assert(datOutThisConnector.con_params.format == _common_set_ocon.theData_out.con_params.format);

	// Check that the rate matches, secondary output connector and primary output buffers
	assert(datOutThisConnector.con_params.rate == _common_set_ocon.theData_out.con_params.rate);

	// The pointers are a shortcut!!
	jvxData** bufsIn= (jvxData**)bufsSideChannel[idxBufferProcess];

	jvxSize numChannels = szExtraBuffersChannels - oconn->chanSetting.idxOffset;
	numChannels = JVX_MIN(numChannels, oconn->chanSetting.channel_num);
	numChannels = JVX_MIN(numChannels, datOutThisConnector.con_params.number_channels);
	for (i = 0; i < numChannels; i++)
	{
		// bufsOut: Channel selection in mix channel space
		// bufferPtrs: Target buffers as 
		memcpy(bufsOut[i], bufsIn[i + oconn->chanSetting.idxOffset], jvxDataFormat_getsize(datOutThisConnector.con_params.format) * datOutThisConnector.con_params.buffersize);
	}
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{	
	if (
		(tp == JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES) && 
		(operationMode == jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT))
	{
		if (!jvx::helper::translate_transfer_chain_get_properties((jvx::propertyCallCompactRefList*)data, this))
		{
			return JVX_NO_ERROR;
		}
	}
	return CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (
		(tp == JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES) &&
		(operationMode == jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT))
	{
		if (!jvx::helper::translate_transfer_chain_get_properties((jvx::propertyCallCompactRefList*)data, this))
		{
			return JVX_NO_ERROR;
		}
	}
	return CjvxBareNode1ioRearrange::transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}
