#include "CjvxSpNMixChainEnterLeave.h"

jvxErrorType
CjvxSpNMixChainEnterLeave::report_selected_connector(CjvxSingleInputConnector* iconn)
{
	oneConnectorPlusName<CjvxSingleInputConnector> addme;
	addme.ioconn = iconn;
	addme.nmUnique = _common_set_min.theDescription + "-" + inputConnectors.inConName + "_" + jvx_size2String(uIdGen++);

	inputConnectors.selectedInputConnectors[iconn] = addme;
	if (_common_set_icon.theData_in)
	{
		// All parameters as desired
		jvxLinkDataDescriptor_con_params params = _common_set_icon.theData_in->con_params;

		// Number of channels differs
		params.number_channels = genMixChain::config.number_channels_side.value;
		params.data_flow = JVX_DATAFLOW_PUSH_ON_PULL;

		// Pass it to the connector but do not request a test of the chain - it is in build status anyway
		iconn->updateFixedProcessingArgs(params, false);
	}
	return JVX_NO_ERROR;
}

void
CjvxSpNMixChainEnterLeave::request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId)
{
	auto elm = inputConnectors.selectedInputConnectors.find(iconn);
	if (elm != inputConnectors.selectedInputConnectors.end())
	{
		if (_common_set.theUniqueId)
		{
			_common_set.theUniqueId->obtain_unique_id(uId, elm->second.nmUnique.c_str());
		}
	}
}

jvxErrorType
CjvxSpNMixChainEnterLeave::report_started_connector(CjvxSingleInputConnector* iconn)
{
	auto elm = inputConnectors.selectedInputConnectors.find(iconn);
	assert(elm != inputConnectors.selectedInputConnectors.end());
	elm->second.inProcessing = true;

	// The list of processing connectors is to be protected as this list is operated in different threads
	JVX_LOCK_MUTEX(safeAccessInputsOutputs);
	// Copy to the list of active connections
	inputConnectors.processingInputConnectors[iconn] = elm->second;
	JVX_UNLOCK_MUTEX(safeAccessInputsOutputs);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNMixChainEnterLeave::report_stopped_connector(CjvxSingleInputConnector* iconn)
{
	// This list must be protected
	JVX_LOCK_MUTEX(safeAccessInputsOutputs);
	auto elm = inputConnectors.processingInputConnectors.find(iconn);
	assert(elm != inputConnectors.processingInputConnectors.end());
	inputConnectors.processingInputConnectors.erase(elm);
	JVX_UNLOCK_MUTEX(safeAccessInputsOutputs);

	auto elmS = inputConnectors.selectedInputConnectors.find(iconn);
	assert(elmS != inputConnectors.selectedInputConnectors.end());
	elmS->second.inProcessing = false;

	return JVX_NO_ERROR;
}

void
CjvxSpNMixChainEnterLeave::release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId)
{
	auto elm = inputConnectors.selectedInputConnectors.find(iconn);
	if (elm != inputConnectors.selectedInputConnectors.end())
	{
		if (_common_set.theUniqueId)
		{
			_common_set.theUniqueId->release_unique_id(uId);
		}
	}
}


jvxErrorType
CjvxSpNMixChainEnterLeave::report_unselected_connector(CjvxSingleInputConnector* iconn)
{
	auto elm = inputConnectors.selectedInputConnectors.find(iconn);
	if (elm != inputConnectors.selectedInputConnectors.end())
	{
		inputConnectors.selectedInputConnectors.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

// This function is called for each single connection. We may search for the entry in the map
void 
CjvxSpNMixChainEnterLeave::report_process_buffers(CjvxSingleInputConnector* iconn, jvxHandle** bufferPtrs, const jvxLinkDataDescriptor_con_params& params)
{
	jvxSize i;
	
	assert(JVX_CHECK_SIZE_SELECTED(idxBufferProcess));
	assert(params.buffersize == _common_set_ocon.theData_out.con_params.buffersize);
	assert(params.number_channels == szExtraBuffersChannels);
	assert(params.format == _common_set_ocon.theData_out.con_params.format);
	
	// The pointers are a shortcut!!
	jvxData** bufsOut = (jvxData**)bufsSideChannel[idxBufferProcess];

	for (i = 0; i < szExtraBuffersChannels; i++)
	{
		memcpy(bufsOut[i], bufferPtrs[i], jvxDataFormat_getsize(params.format) * params.buffersize);
	}
}