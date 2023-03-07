#include "CjvxAuNBitstreamDecoder.h"
#include "jvx-helpers-cpp.h"

CjvxAuNBitstreamDecoder::CjvxAuNBitstreamDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
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

CjvxAuNBitstreamDecoder::~CjvxAuNBitstreamDecoder()
{
	
}

// ===================================================================

#if 0
jvxErrorType 
CjvxAuNBitstreamDecoder::select(IjvxObject* owner)
{
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::select(owner);
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
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::unselect();
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
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::activate();
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
CjvxAuNBitstreamDecoder::deactivate()
{
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{		
		// Delete the microconnection

		// Release all codecs
		for(std::pair<jvxSize, refComp<IjvxAudioCodec>> elm: lstCodecInstances)
		{
			retInstTool<IjvxAudioCodec>(_common_set.theToolsHost, JVX_COMPONENT_AUDIO_CODEC, elm.second, elm.first);
		}
		lstCodecInstances.clear();
		JVX_LOCAL_BASE_CLASS::deactivate();
	}
	return res;
}

// ===================================================================
jvxErrorType 
CjvxAuNBitstreamDecoder::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Close any existing connection
		deactivate_decoder();
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
					activate_decoder();
					assert(theDecoder);

					// Specify a useful name 
					nmProcess = _common_set.theName + " MC";
					descrProcess = _common_set.theDescriptor + "_MC";
					descrorProcess= _common_set.theDescriptor + " MC";
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
					// This will disenage the "local" connection 
					shutdown_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
					deactivate_decoder();
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
			res = JVX_LOCAL_BASE_CLASS::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));			
		}

		if (res == JVX_NO_ERROR)
		{
			// Update the parameters if test was successful			
			output_params_from_ldesc_on_test();
		}
		return res;
	}
	return JVX_ERROR_INVALID_FORMAT;
}

// ===================================================================
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

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNBitstreamDecoder, get_processing_monitor)
{
	return JVX_NO_ERROR;
}

// ==========================================================================================
// ==========================================================================================



// ==================================================================

jvxErrorType
CjvxAuNBitstreamDecoder::activate_decoder()
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

	attach_component_chain("Decoder", theDecoder, nullptr);

	return res;
}

jvxErrorType 
CjvxAuNBitstreamDecoder::deactivate_decoder()
{
	jvxErrorType res = JVX_NO_ERROR;

	// =================================================================
	if (theDecoder && ptrCodec)
	{
		detach_component_chain("Decoder");

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

// ===================================================================================================

jvxErrorType 
CjvxAuNBitstreamDecoder::runStateSwitch(jvxStateSwitch ss, IjvxSimpleNode* node, 
	const char* moduleName, IjvxObject* theOwner)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxAuNBitstreamDecoder::componentsAboutToConnect()
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxAuNBitstreamDecoder::runTestChainComplete(jvxErrorType lastResult, IjvxSimpleNode* node, const char* moduleName, jvxSize uniqueId)
{
	return JVX_NO_ERROR;
}
