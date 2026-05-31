#include "CjvxAuNBitstreamEncoder.h"
#include "jvx-helpers-cpp.h"

CjvxAuNBitstreamEncoder::CjvxAuNBitstreamEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	JVX_LOCAL_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE);
	_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;

	allowZeroCopyOnCondition = false;
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

jvxErrorType
CjvxAuNBitstreamEncoder::select(IjvxObject* parent)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::select(parent);
	if (res == JVX_NO_ERROR)
	{
		genBitstreamEncoder_node_select::init_all();
		genBitstreamEncoder_node_select::allocate_all();
		genBitstreamEncoder_node_select::register_all(this);

		genBitstreamEncoder_node_select::ext_tools_host.ex_interface.ptr = static_cast<IjvxPropertyExtender*>(this);
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamEncoder::unselect()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		genBitstreamEncoder_node_select::ext_tools_host.ex_interface.ptr = nullptr;

		genBitstreamEncoder_node_select::unregister_all(this);
		genBitstreamEncoder_node_select::deallocate_all();

		res = CjvxBareNode1ioRearrange::unselect();
	}
	return res;
}

jvxErrorType 
CjvxAuNBitstreamEncoder::supports_prop_extender_type(jvxPropertyExtenderType tp) 
{
	if (tp == jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_SIMPLE_TOOLS_HOST)
	{
		return JVX_NO_ERROR;
	}
	return JVX_LOCAL_BASE_CLASS::supports_prop_extender_type(tp);
};

jvxErrorType 
CjvxAuNBitstreamEncoder::prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp) 
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (tp)
	{
	case jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_SIMPLE_TOOLS_HOST:
		if (prop_extender)
		{
			*prop_extender = static_cast<IjvxPropertyExtenderSimpleToolsHostInstall*>(this);
		}
		res = JVX_NO_ERROR;
		break;
	default:
		return JVX_LOCAL_BASE_CLASS::prop_extender_specialization(prop_extender, tp);
	}
	return res;
};

jvxErrorType 
CjvxAuNBitstreamEncoder::install(IjvxPropertyExtenderSimpleToolsHost* installThis)
{
	jvxErrorType res = JVX_ERROR_DUPLICATE_ENTRY;
	if (registeredToolsHost == nullptr)
	{
		registeredToolsHost = installThis;
		res = JVX_NO_ERROR;
	}
	return res;
}

jvxErrorType 
CjvxAuNBitstreamEncoder::uninstall(IjvxPropertyExtenderSimpleToolsHost* uninstallThis)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	if (registeredToolsHost == uninstallThis)
	{
		registeredToolsHost = nullptr;
		res = JVX_NO_ERROR;
	}
	return res;
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

		if (registeredToolsHost)
		{
			registeredToolsHost->number_tools(JVX_COMPONENT_AUDIO_CODEC, &num);
			for (i = 0; i < num; i++)
			{
				refComp<IjvxAudioCodec> retI;
				retI = reqInstTool<IjvxAudioCodec, IjvxPropertyExtenderSimpleToolsHost>(registeredToolsHost, JVX_COMPONENT_AUDIO_CODEC, i);
				if (retI.cpPtr)
				{
					lstCodecInstances[i] = retI;
				}
			}
		}
		else if (_common_set.theToolsHost)
		{
			_common_set.theToolsHost->number_tools(JVX_COMPONENT_AUDIO_CODEC, &num);
			for (i = 0; i < num; i++)
			{
				refComp<IjvxAudioCodec> retI;
				retI = reqInstTool<IjvxAudioCodec, IjvxToolsHost>(_common_set.theToolsHost, JVX_COMPONENT_AUDIO_CODEC, i);
				if (retI.cpPtr)
				{
					lstCodecInstances[i] = retI;
				}
			}
		}

		genBitstreamEncoder_node::init_all();
		genBitstreamEncoder_node::allocate_all();
		genBitstreamEncoder_node::register_all(this);

		jvxApiStringList lst;
		for (auto elm : lstCodecInstances)
		{
			jvxApiString astr;
			elm.second.objPtr->description(&astr);
			genBitstreamEncoder_node::info.codecs.value.push_back(astr.std_str());
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

		// Delete list of shown codecs
		genBitstreamEncoder_node::info.codecs.value.clear();
		genBitstreamEncoder_node::unregister_all(this);
		genBitstreamEncoder_node::deallocate_all();

		if (registeredToolsHost)
		{
			// Release all codecs
			for (std::pair<jvxSize, refComp<IjvxAudioCodec>> elm : lstCodecInstances)
			{
				retInstTool<IjvxAudioCodec, IjvxPropertyExtenderSimpleToolsHost>(registeredToolsHost, JVX_COMPONENT_AUDIO_CODEC, elm.second, elm.first);
			}
		}
		else if (_common_set.theToolsHost)
		{
			// Release all codecs
			for (std::pair<jvxSize, refComp<IjvxAudioCodec>> elm : lstCodecInstances)
			{
				retInstTool<IjvxAudioCodec, IjvxToolsHost>(_common_set.theToolsHost, JVX_COMPONENT_AUDIO_CODEC, elm.second, elm.first);
			}
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
