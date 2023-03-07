#include "CjvxAuNBitstreamEncoder.h"
#include "jvx-helpers-cpp.h"

CjvxAuNBitstreamEncoder::CjvxAuNBitstreamEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	JVX_LOCAL_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE);
	_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;

	checkInputOutputMostlyIdentical = false;
	/*
	_common_set_node_base_1io.num_iochans = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.bsize_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.rate_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.form_io = JVX_DATAFORMAT_DATA;
	_common_set_1io_zerocopy.subform_io = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
	*/
}

CjvxAuNBitstreamEncoder::~CjvxAuNBitstreamEncoder()
{
	
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxAuNBitstreamEncoder::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		jvxSize num = 0;
		jvxSize i;
		_common_set.theToolsHost->number_tools(JVX_COMPONENT_AUDIO_CODEC, &num);
		for (i = 0; i < num; i++)
		{
			refComp<IjvxAudioCodec> retI;
			retI = reqInstTool<IjvxAudioCodec>(_common_set.theToolsHost, JVX_COMPONENT_AUDIO_CODEC, i);
			if (retI.cpPtr)
			{
				lstCodecInstances[i] = retI;
			}
		}		
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamEncoder::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{		
		// Close any existing connection
		deactivate_encoder();

		// Delete the microconnection
		delete theMicroConnection;
		theMicroConnection = nullptr;

		// Release all codecs
		for(std::pair<jvxSize, refComp<IjvxAudioCodec>> elm: lstCodecInstances)
		{
			retInstTool<IjvxAudioCodec>(_common_set.theToolsHost, JVX_COMPONENT_AUDIO_CODEC, elm.second, elm.first);
		}
		lstCodecInstances.clear();
		CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamEncoder::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Close any existing connection
		deactivate_encoder();
	}
	return res;
}
// ===================================================================
jvxErrorType
CjvxAuNBitstreamEncoder::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	std::string token;
	jvxApiString astr;
	jvxErrorType res = JVX_NO_ERROR;
	IjvxDataConnectionProcess* theProc = nullptr;

	if (theEncoder == nullptr)
	{
		// If the encoder is not yet configured
		assert(lstCodecInstances.size());
		id_selected = 0;		
		this->activate_encoder();
		
		assert(theEncoder);
		theEncoder->get_configure_token(&astr);
		config_token = astr.std_str();

		// Specify a useful name 
		nmProcess = _common_set.theName + " MC";
		descrProcess = _common_set.theDescriptor + "_MC";
		descrorProcess = _common_set.theDescriptor + " MC";
	}
	
	res = JVX_LOCAL_BASE_CLASS::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	if (res == JVX_NO_ERROR)
	{
		output_params_from_ldesc_on_test();
	}
	return res;
}

// ===================================================================

jvxErrorType 
CjvxAuNBitstreamEncoder::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		// To do at this position
		return theMicroConnection->transfer_backward_connection(tp, data
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;

	case JVX_LINKDATA_TRANSFER_REQUEST_DATA:
		if (_common_set_ldslave.theData_in->con_link.connect_from)
		{
			res = _common_set_ldslave.theData_in->con_link.connect_from->
				transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		break;
	default:
		res = CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxAuNBitstreamEncoder::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = JVX_NO_ERROR;
	
	return res;
}

jvxErrorType
CjvxAuNBitstreamEncoder::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warns;
	jvxErrorType res = JVX_NO_ERROR;
	
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNBitstreamEncoder, get_processing_monitor)
{
	return JVX_NO_ERROR;
}

// ==================================================================

jvxErrorType
CjvxAuNBitstreamEncoder::activate_encoder()
{
	jvxErrorType res = JVX_NO_ERROR;
	ptrCodec = lstCodecInstances[id_selected].cpPtr;

	res = ptrCodec->initialize(_common_set_min.theHostRef);
	assert(res == JVX_NO_ERROR);

	res = ptrCodec->select(nullptr);
	assert(res == JVX_NO_ERROR);

	res = ptrCodec->activate();
	assert(res == JVX_NO_ERROR);

	res = lstCodecInstances[id_selected].cpPtr->request_encoder(&theEncoder);
	assert(res == JVX_NO_ERROR);

	theEncoder->initialize(_common_set_min.theHostRef);
	assert(res == JVX_NO_ERROR);

	theEncoder->select();
	assert(res == JVX_NO_ERROR);

	theEncoder->activate();
	assert(res == JVX_NO_ERROR);

	// Integrate the decoder to become part of the bitstream component
	theEncoderProps = reqInterface<IjvxProperties>(theEncoder);
	if (theEncoderProps)
	{
		CjvxProperties::_register_sub_module(
			theEncoderProps,
			1024,
			"encoder",
			"encoder");
		_common_set_properties.propSetRevision++;
	}

	attach_component_chain("Encoder", theEncoder, nullptr);

	return res;
}

jvxErrorType 
CjvxAuNBitstreamEncoder::deactivate_encoder()
{
	// =================================================================
	if (theEncoder && ptrCodec)
	{
		jvxErrorType res = JVX_NO_ERROR;
		
		detach_component_chain("Encoder");
		if (theEncoderProps)
		{
			CjvxProperties::_unregister_sub_module("encoder");
			retInterface<IjvxProperties>(theEncoder, theEncoderProps);
		}
		
		res = theEncoder->deactivate();
		res = theEncoder->unselect();
		res = theEncoder->terminate();

		res = ptrCodec->return_encoder(theEncoder);
		theEncoder = nullptr;

		res = ptrCodec->deactivate();
		res = ptrCodec->unselect();
		res = ptrCodec->terminate();

		ptrCodec = nullptr;
	}

	return JVX_NO_ERROR;
}

// ==========================================================================================
// ==========================================================================================

jvxErrorType
CjvxAuNBitstreamEncoder::runStateSwitch(jvxStateSwitch ss, IjvxSimpleNode* node,
	const char* moduleName, IjvxObject* theOwner)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxAuNBitstreamEncoder::componentsAboutToConnect()
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxAuNBitstreamEncoder::runTestChainComplete(jvxErrorType lastResult, IjvxSimpleNode* node, const char* moduleName, jvxSize uniqueId)
{
	return JVX_NO_ERROR;
}
