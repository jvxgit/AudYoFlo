#include "CjvxAuNBitstreamEncoder.h"
#include "jvx-helpers-cpp.h"

CjvxAuNBitstreamEncoder::CjvxAuNBitstreamEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io_rearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
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

#if 0
jvxErrorType 
CjvxAuNBitstreamEncoder::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxBareNode1io_rearrange::select(owner);
	if (res == JVX_NO_ERROR)
	{
		/*
		// Need to allocate properties on selected to allow modification of prefix
		genCaptureFile_node::init_all();
		genCaptureFile_node::allocate_all();
		genCaptureFile_node::register_all(static_cast<CjvxProperties*>(this));
		genCaptureFile_node::register_callbacks(static_cast<CjvxProperties*>(this),
			set_processing_props, get_processing_monitor,
			reinterpret_cast<jvxHandle*>(this), NULL);
			*/
	}
	return res;
}

jvxErrorType 
CjvxAuNBitstreamEncoder::unselect()
{
	jvxErrorType res = CjvxBareNode1io_rearrange::unselect();
	if (res == JVX_NO_ERROR)
	{
		/*
		genCaptureFile_node::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);
		genCaptureFile_node::unregister_all(static_cast<CjvxProperties*>(this));
		genCaptureFile_node::deallocate_all();
		*/
	}
	return res;
}
#endif

// ===================================================================

jvxErrorType
CjvxAuNBitstreamEncoder::activate()
{
	jvxErrorType res = CjvxBareNode1io_rearrange::activate();
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

		theMicroConnection = new HjvxMicroConnection(
			"Micro Connection Encoder",
			false,
			"Micro Connection Encoder",
			0, "local-temp-lane",
			JVX_COMPONENT_ACCESS_SUB_COMPONENT,
			JVX_COMPONENT_CUSTOM_DEVICE, "", NULL);
		theMicroConnection->set_location_info(jvxComponentIdentification(JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_UNSELECTED));
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamEncoder::deactivate()
{
	jvxErrorType res = CjvxBareNode1io_rearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{		
		// Close any existing connection
		release_encoder_connection();

		// Delete the microconnection
		delete theMicroConnection;
		theMicroConnection = nullptr;

		// Release all codecs
		for(std::pair<jvxSize, refComp<IjvxAudioCodec>> elm: lstCodecInstances)
		{
			retInstTool<IjvxAudioCodec>(_common_set.theToolsHost, JVX_COMPONENT_AUDIO_CODEC, elm.second, elm.first);
		}
		lstCodecInstances.clear();
		CjvxBareNode1io_rearrange::deactivate();
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
	jvxSize uId = JVX_SIZE_UNSELECTED;
	IjvxDataConnectionProcess* theProc = nullptr;

	if (theEncoder == nullptr)
	{
		assert(lstCodecInstances.size());
		id_selected = 0;

		if (_common_set_ldslave.theData_in->con_link.master)
		{
			_common_set_ldslave.theData_in->con_link.master->associated_process(&theProc);
			if (theProc)
			{
				theProc->unique_id_connections(&uId);
			}
		}
		this->activate_encoder_connection(uId);
		
		assert(theEncoder);
		config_token = theEncoder->get_configure_token(&astr);
		config_token = astr.std_str();
	}
	
	res = theMicroConnection->test_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));			
	if (res == JVX_NO_ERROR)
	{
		// Since we run this outside of the normal flow from CjvxBareNode1io_rearrange we need to
		// realize a dedicated functionality
		// CjvxBareNode1io_rearrange::update_input_params_on_test();


		// CjvxBareNode1io_rearrange::update_output_params_on_test();
		CjvxNodeBase1io::update_simple_params_from_ldesc();		
	}
	return res;
}

// ===================================================================

jvxErrorType
CjvxAuNBitstreamEncoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	prepare_autostart();
	return theMicroConnection->prepare_connection(false, true);
}

jvxErrorType
CjvxAuNBitstreamEncoder::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = theMicroConnection->postprocess_connection();
	postprocess_autostart();
	return res;
}

jvxErrorType
CjvxAuNBitstreamEncoder::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	start_autostart();
	return theMicroConnection->start_connection();
}

jvxErrorType
CjvxAuNBitstreamEncoder::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = theMicroConnection->stop_connection();
	stop_autostart();
	return res;
}

// ===========================================================================

jvxErrorType 
CjvxAuNBitstreamEncoder::process_start_icon(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	return theMicroConnection->prepare_process_connection(nullptr,
		pipeline_offset,
		idx_stage,
		tobeAccessedByStage,
		clbk,
		priv_ptr);
}

jvxErrorType
CjvxAuNBitstreamEncoder::process_stop_icon(
	jvxSize idx_stage,
	jvxBool operate_first_call,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock cb,
	jvxHandle* priv_ptr)
{
	return theMicroConnection->postprocess_process_connection(
		idx_stage,
		operate_first_call,
		tobeAccessedByStage,
		cb,
		priv_ptr);
}

jvxErrorType
CjvxAuNBitstreamEncoder::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	// Run the decode microconnection
	return theMicroConnection->process_connection(nullptr);
}

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
		res = CjvxBareNode1io_rearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
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

jvxErrorType
CjvxAuNBitstreamEncoder::init_microconnection()
{
	jvxErrorType res = JVX_NO_ERROR;
	

	

	res = theMicroConnection->test_chain_master(JVX_CONNECTION_FEEDBACK_CALL_NULL);
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNBitstreamEncoder, get_processing_monitor)
{
	return JVX_NO_ERROR;
}

// ==========================================================================================
// ==========================================================================================

jvxErrorType
CjvxAuNBitstreamEncoder::hook_test_negotiate(jvxLinkDataDescriptor* proposed JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = _common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(
		JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, proposed JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		config_token = _common_set_ldslave.theData_in->con_params.format_spec->std_str();

		// Configure the decoder
		//res = theDecoder->configure_token(config_token.c_str());
		assert(res == JVX_NO_ERROR);
	}
	return res;
}

jvxErrorType 
CjvxAuNBitstreamEncoder::hook_test_accept(jvxLinkDataDescriptor* dataIn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = hook_test_update(dataIn  JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	res = CjvxBareNode1io_rearrange::test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxAuNBitstreamEncoder::hook_test_update(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	// We end up here if the microconnection was tested with success
	// Forward the connection parameters to the bitsream output path
	_common_set_ldslave.theData_out.con_params = dataIn->con_params;
	if (_common_set_ldslave.theData_out.con_params.format_spec)
	{
		config_token = _common_set_ldslave.theData_out.con_params.format_spec->std_str();
	}
	return JVX_NO_ERROR;
}

	jvxErrorType 
CjvxAuNBitstreamEncoder::hook_check_is_ready(jvxBool* is_ready, jvxApiString* astr)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNBitstreamEncoder::hook_prepare(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{

	// Forward the chain	
	return _prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNBitstreamEncoder::hook_postprocess(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	return _postprocess_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNBitstreamEncoder::hook_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize idRuntime = JVX_SIZE_UNSELECTED;
	_common_set.theUniqueId->obtain_unique_id(&idRuntime, _common_set.theDescriptor.c_str());
	return _start_connect_icon(true, idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNBitstreamEncoder::hook_stop(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return _stop_connect_icon(true, nullptr JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNBitstreamEncoder::hook_process_start(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ldslave.ocon)
	{
		// Input / output connector implementations are never zerocopy
		// We start a regular output connector here
		res = _common_set_ldslave.ocon->process_start_ocon(
			pipeline_offset,
			idx_stage,
			tobeAccessedByStage,
			clbk,
			priv_ptr);
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamEncoder::hook_process(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ldslave.ocon)
	{
		return _common_set_ldslave.ocon->process_buffers_ocon(mt_mask, idx_stage);
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamEncoder::hook_process_stop(
	jvxSize idx_stage,
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ldslave.ocon)
	{
		// Input / output connector implementations are never zerocopy
		// We start a regular output connector here
		res = _common_set_ldslave.ocon->process_stop_ocon(
			idx_stage,
			shift_fwd,
			tobeAccessedByStage,
			clbk,
			priv_ptr);
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamEncoder::hook_forward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return CjvxBareNode1io_rearrange::_transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

// ==================================================================

jvxErrorType
CjvxAuNBitstreamEncoder::activate_encoder_connection(jvxSize uIdProc)
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
			"decoder");
		_common_set_properties.propSetRevision++;
	}

	_common_set.theUniqueId->obtain_unique_id(&uIdl, "Another bitstream encoder");
	theMicroConnection->activate_connection(
		_common_set_min.theHostRef,
		theEncoder,
		"default", "default", "default", "default",
		"jvxAuNBitstreamEncoder#" + jvx_size2String(uIdl),
		false, nullptr, uIdProc);

	res = theMicroConnection->connect_connection(_common_set_ldslave.theData_in,
		&_common_set_ldslave.theData_out,
		HjvxMicroConnection::jvxConnectionType::JVX_MICROCONNECTION_ENGAGE,
		static_cast<HjvxMicroConnection_hooks_simple*>(this),
		static_cast<HjvxMicroConnection_hooks_fwd*>(this));
	assert(res == JVX_NO_ERROR);

	return res;
}

jvxErrorType 
CjvxAuNBitstreamEncoder::release_encoder_connection()
{
	// =================================================================
	if (theEncoder && ptrCodec)
	{
		jvxErrorType res = theMicroConnection->disconnect_connection();
		res = theMicroConnection->deactivate_connection();
		res = _common_set.theUniqueId->release_unique_id(uIdl);

		if (theEncoderProps)
		{
			CjvxProperties::_unregister_sub_module("decoder");
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