#include "CjvxSpNSynchronize_sec.h"
#include "CjvxSpNSynchronize.h"

CjvxSpNSynchronize_sec::CjvxSpNSynchronize_sec(CjvxSpNSynchronize* reference) :
	CjvxSingleInputConnector(false),
	CjvxSingleOutputConnector(false)
{
	CjvxSingleInputConnector::report = this;
	CjvxSingleOutputConnector::report = this;

	CjvxSingleConnector_report<CjvxSingleInputConnector>* reportRefIn;
	referencePtr = reference;
	safeAccessRWConnectionStatus.v = 0;
	safeAccessConnectionBuffering.v = 0;

	// Initialize this module
	jvx_audio_stack_sample_dispenser_cont_initCfg(&inProcessing.crossThreadTransfer.myAudioDispenser);
	inProcessing.fHeight.numberEventsConsidered_perMinMaxSection = 10;
	inProcessing.fHeight.num_MinMaxSections = 5;
	inProcessing.fHeight.recSmoothFactor = 0.95;
	inProcessing.fHeight.numOperations = 0;
}

void
CjvxSpNSynchronize_sec::activate()
{
	// Activate the input connector
	CjvxSingleInputConnector::activate(
		static_cast<IjvxObject*>(referencePtr),
		static_cast<IjvxConnectorFactory*>(referencePtr),
		"sec-audio-in", 
		static_cast<CjvxSingleConnector_report<CjvxSingleInputConnector>*>(this), 
		0);
	
	// Activate the output connector
	CjvxSingleOutputConnector::activate(
		static_cast<IjvxObject*>(referencePtr), 
		static_cast<IjvxConnectorFactory*>(referencePtr),
		"sec-audio-out", 
		static_cast<CjvxSingleConnector_report<CjvxSingleOutputConnector>*>(this), 
		0);
	
	// Activate the master function
	CjvxConnectionMaster::_init_master(static_cast<CjvxObject*>(referencePtr), 
		"sec-audio-master", static_cast<IjvxConnectionMasterFactory*>(this)); 
	oneMasterElement newElm; 
	newElm.descror = "sec-audio-master";
	newElm.theMaster = static_cast<IjvxConnectionMaster*>(this); 
	_common_set_conn_master_factory.masters[newElm.theMaster] = newElm; 
	CjvxConnectionMasterFactory::_activate_master_factory(referencePtr); 

	// Note: the connection between master and output connector will be established on first connect

	// Register properties on the synchronous side. Add prefix AND register as non-system properties
	node_input.initialize(
		static_cast<CjvxProperties*>(referencePtr), "sec-in", true);
	node_output.initialize(
		static_cast<CjvxProperties*>(referencePtr), "sec-out", true);

}

void
CjvxSpNSynchronize_sec::deactivate()
{
	node_input.terminate();
	node_output.terminate();

	CjvxConnectionMasterFactory::_deactivate_master_factory();
	_common_set_conn_master_factory.masters.erase(this);
	CjvxConnectionMaster::_terminate_master();

	CjvxSingleInputConnector::deactivate();
	CjvxSingleOutputConnector::deactivate();
}

jvxErrorType 
CjvxSpNSynchronize_sec::connect_chain_master(const jvxChainConnectArguments& args,
	const jvxLinkDataDescriptor_con_params* init_params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _activate_master(this, static_cast<IjvxConnectionMaster*>(this));
	assert(res == JVX_NO_ERROR);

	res = _connect_chain_master(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res != JVX_NO_ERROR)
	{
		jvxErrorType resL = _deactivate_master(this, static_cast<IjvxConnectionMaster*>(this));
		assert(resL == JVX_NO_ERROR);
	}
	else
	{
		outputConnectorConnected = true;
	}
	return res;
}

jvxErrorType
CjvxSpNSynchronize_sec::disconnect_chain_master(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _disconnect_chain_master(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	res = _deactivate_master(this, static_cast<IjvxConnectionMaster*>(this));
	outputConnectorConnected = false;
	return res;
}

jvxErrorType 
CjvxSpNSynchronize_sec::transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	if (tp == JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY)
	{
		jvxBool is_ready = true;
		jvxApiString* astr = (jvxApiString*)data;
		res = referencePtr->is_ready(&is_ready, astr);
		if (is_ready == false)
		{
			res = JVX_ERROR_NOT_READY;
		}
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
	}
	return _transfer_chain_forward_master(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}



jvxErrorType 
CjvxSpNSynchronize_sec::request_reference_object(IjvxObject** obj)
{
	if (obj)
	{
		* obj = static_cast<IjvxObject*>(referencePtr);
	}
	return JVX_NO_ERROR;
};

jvxErrorType
CjvxSpNSynchronize_sec::return_reference_object(IjvxObject* obj) 
{
	return JVX_NO_ERROR;
};

void
CjvxSpNSynchronize_sec::trigger_request_secondary_chain()
{
	if (JVX_CHECK_SIZE_SELECTED(_common_set_ocon.theData_out.con_link.uIdConn))
	{
		referencePtr->_request_test_chain_master(_common_set_ocon.theData_out.con_link.uIdConn);
	}
}

jvxErrorType 
CjvxSpNSynchronize_sec::available_to_connect_icon()
{
	if (!inputConnectorConnected)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_NOT_READY;
}


jvxErrorType
CjvxSpNSynchronize_sec::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxSingleInputConnector::connect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		inputConnectorConnected = true;
	}
	return res;
}

jvxErrorType
CjvxSpNSynchronize_sec::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxSingleInputConnector::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		inputConnectorConnected = false;
	}
	return res;
}

// ================================================================================
// Callbacks for output connector
// ================================================================================
jvxErrorType
CjvxSpNSynchronize_sec::available_to_connect_ocon()
{
	if (!outputConnectorConnected)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_NOT_READY;
}

jvxErrorType
CjvxSpNSynchronize_sec::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Copy the current parameter from primary chain
	switch (referencePtr->bufferMode)
	{
	case jvxSynchronizeBufferMode::JVX_SYNCHRONIZE_UNBUFFERED_PUSH:
		JVX_LINKDATA_LOAD_FROM(&_common_set_ocon.theData_out, referencePtr->node_inout._common_set_node_params_a_1io, JVX_DATAFLOW_PUSH_ACTIVE);
		break;
	case jvxSynchronizeBufferMode::JVX_SYNCHRONIZE_BUFFERED_PULL:
		JVX_LINKDATA_LOAD_FROM(&_common_set_ocon.theData_out, referencePtr->node_inout._common_set_node_params_a_1io, JVX_DATAFLOW_PUSH_ON_PULL);
		break;
	default:
		assert(0);
		break;
	}
	res = _test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	if (res == JVX_NO_ERROR)
	{
		JVX_LINKDATA_LOAD_TO(node_output._common_set_node_params_a_1io, &_common_set_ocon.theData_out);
		JVX_LINKDATA_LOAD_TO(node_input._common_set_node_params_a_1io, _common_set_icon.theData_in);

	}
	return res;
}

jvxErrorType
CjvxSpNSynchronize_sec::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxSingleInputConnector::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));	
	return res;
}

void 
CjvxSpNSynchronize_sec::updateFixedProcessingArgsInOut(jvxBool triggerTestChain)
{
	CjvxSingleInputConnector::updateFixedProcessingArgs(referencePtr->_common_set_ocon.theData_out.con_params, false);
	CjvxSingleOutputConnector::updateFixedProcessingArgs(referencePtr->_common_set_icon.theData_in->con_params); 
	if (triggerTestChain)
	{
		trigger_request_secondary_chain();
	}
}
