#include "CjvxAuNBitstreamDecoder.h"
#include "jvx-helpers-cpp.h"

CjvxAuNBitstreamDecoder::CjvxAuNBitstreamDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
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

CjvxAuNBitstreamDecoder::~CjvxAuNBitstreamDecoder()
{
	
}

// ===================================================================

#if 0
jvxErrorType 
CjvxAuNBitstreamDecoder::select(IjvxObject* owner)
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
CjvxAuNBitstreamDecoder::unselect()
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
CjvxAuNBitstreamDecoder::activate()
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
			"Micro Connection Decoder",
			false,
			"Micro Connection Decoder",
			0, "local-temp-lane",
			JVX_COMPONENT_ACCESS_SUB_COMPONENT,
			JVX_COMPONENT_CUSTOM_DEVICE, "", NULL);
		theMicroConnection->set_location_info(jvxComponentIdentification(JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_UNSELECTED));
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamDecoder::deactivate()
{
	jvxErrorType res = CjvxBareNode1io_rearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{		
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
CjvxAuNBitstreamDecoder::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxBareNode1io_rearrange::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Close any existing connection
		release_decoder_connection();
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamDecoder::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	std::string token;
	std::string tokenDecoder;
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ldslave.theData_in->con_params.format_spec)
	{
		jvxBool err = false;
		jvxBool runloop = true;
		config_token = _common_set_ldslave.theData_in->con_params.format_spec->std_str();

		while (runloop)
		{
			if (theDecoder == nullptr)
			{
				id_selected = JVX_SIZE_UNSELECTED;
				for (std::pair<jvxSize, refComp<IjvxAudioCodec> > elm : lstCodecInstances)
				{
					jvxApiString astr;
					jvxBool compr = false;
					jvxErrorType resCompat = elm.second.cpPtr->accept_for_decoding(config_token.c_str(), &compr, &astr);
					if (resCompat == JVX_NO_ERROR)
					{
						id_selected = elm.first;
						break;
					}
				}

				if (JVX_CHECK_SIZE_SELECTED(id_selected))
				{			
					IjvxDataConnectionProcess* theProc = nullptr;
					jvxSize uId = JVX_SIZE_UNSELECTED;
					if (_common_set_ldslave.theData_in->con_link.master)
					{
						_common_set_ldslave.theData_in->con_link.master->associated_process(&theProc);
						if (theProc)
						{
							theProc->unique_id_connections(&uId);
						}
					}
					res = activate_decoder_connection(uId);

					runloop = false;

				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
			else
			{
				jvxApiString astr;
				jvxBool compr = false;
				jvxErrorType resCompat = lstCodecInstances[id_selected].cpPtr->accept_for_decoding(config_token.c_str(), &compr, &astr);
				if (resCompat != JVX_NO_ERROR)
				{
					release_decoder_connection();
				}
				else
				{
					runloop = false;
				}
			}
		}

		if (res == JVX_NO_ERROR)
		{
			// Configure the decoder
			res = theDecoder->set_configure_token(config_token.c_str());
			assert(res == JVX_NO_ERROR);

			// Test the connection. This will forward the request to the next element in the chain 
			// via the test_accept callback
			res = theMicroConnection->test_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));			
		}

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
	return JVX_ERROR_INVALID_FORMAT;
}

// ===================================================================

jvxErrorType
CjvxAuNBitstreamDecoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	prepare_autostart();
	return theMicroConnection->prepare_connection(false, true);
}

jvxErrorType
CjvxAuNBitstreamDecoder::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = theMicroConnection->postprocess_connection();
	postprocess_autostart();
	return res;
}

jvxErrorType
CjvxAuNBitstreamDecoder::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	start_autostart();
	return theMicroConnection->start_connection();
}

jvxErrorType
CjvxAuNBitstreamDecoder::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = theMicroConnection->stop_connection();
	stop_autostart();
	return res;
}

// ===========================================================================

jvxErrorType 
CjvxAuNBitstreamDecoder::process_start_icon(
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
CjvxAuNBitstreamDecoder::process_stop_icon(
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
CjvxAuNBitstreamDecoder::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	// Run the decode microconnection
	return theMicroConnection->process_connection(nullptr);
}

jvxErrorType 
CjvxAuNBitstreamDecoder::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
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

jvxErrorType 
CjvxAuNBitstreamDecoder::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return theMicroConnection->transfer_forward_connection(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxAuNBitstreamDecoder::put_configuration(jvxCallManagerConfiguration* callMan,
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
CjvxAuNBitstreamDecoder::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warns;
	jvxErrorType res = JVX_NO_ERROR;
	
	return res;
}

jvxErrorType
CjvxAuNBitstreamDecoder::init_microconnection()
{
	jvxErrorType res = JVX_NO_ERROR;
	

	

	res = theMicroConnection->test_chain_master(JVX_CONNECTION_FEEDBACK_CALL_NULL);
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNBitstreamDecoder, get_processing_monitor)
{
	return JVX_NO_ERROR;
}

// ==========================================================================================
// ==========================================================================================



// ==================================================================

jvxErrorType
CjvxAuNBitstreamDecoder::activate_decoder_connection(jvxSize idProc)
{
	jvxErrorType res = JVX_NO_ERROR;
	ptrCodec = lstCodecInstances[id_selected].cpPtr;

	res = ptrCodec->initialize(_common_set_min.theHostRef);
	assert(res == JVX_NO_ERROR);

	res = ptrCodec->select(nullptr);
	assert(res == JVX_NO_ERROR);

	res = ptrCodec->activate();
	assert(res == JVX_NO_ERROR);

	res = lstCodecInstances[id_selected].cpPtr->request_decoder(&theDecoder);
	assert(res == JVX_NO_ERROR);

	theDecoder->initialize(_common_set_min.theHostRef);
	assert(res == JVX_NO_ERROR);

	theDecoder->select();
	assert(res == JVX_NO_ERROR);

	theDecoder->activate();
	assert(res == JVX_NO_ERROR);

	// Integrate the decoder to become part of the bitstream component
	theDecoderProps = reqInterface<IjvxProperties>(theDecoder);
	if (theDecoderProps)
	{
		CjvxProperties::_register_sub_module(
			theDecoderProps,
			1024,
			"decoder",
			"decoder");
		_common_set_properties.propSetRevision++;
	}

	_common_set.theUniqueId->obtain_unique_id(&uIdl, "Another bitstream decoder");

	theMicroConnection->activate_connection(
		_common_set_min.theHostRef,
		theDecoder,
		"default", "default", "default", "default",
		"jvxAuNBitstreamDecoder#" + jvx_size2String(uIdl),
		false, nullptr, idProc);

	res = theMicroConnection->connect_connection(_common_set_ldslave.theData_in,
		&_common_set_ldslave.theData_out,
		HjvxMicroConnection::jvxConnectionType::JVX_MICROCONNECTION_ENGAGE,
		static_cast<HjvxMicroConnection_hooks_simple*>(this),
		static_cast<HjvxMicroConnection_hooks_fwd*>(this));
	assert(res == JVX_NO_ERROR);

	return res;
}

jvxErrorType 
CjvxAuNBitstreamDecoder::release_decoder_connection()
{
	// =================================================================
	if (theDecoder && ptrCodec)
	{
		jvxErrorType res = theMicroConnection->disconnect_connection();
		res = theMicroConnection->deactivate_connection();
		res = _common_set.theUniqueId->release_unique_id(uIdl);

		if (theDecoderProps)
		{
			CjvxProperties::_unregister_sub_module("decoder");
			retInterface<IjvxProperties>(theDecoder, theDecoderProps);
		}
		
		res = theDecoder->deactivate();
		res = theDecoder->unselect();
		res = theDecoder->terminate();

		res = ptrCodec->return_decoder(theDecoder);
		theDecoder = nullptr;

		res = ptrCodec->deactivate();
		res = ptrCodec->unselect();
		res = ptrCodec->terminate();

		ptrCodec = nullptr;
	}
	return JVX_NO_ERROR;
}