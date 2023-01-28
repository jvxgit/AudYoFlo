#include "CjvxAuNPlayChannelId.h"

CjvxAuNPlayChannelId::CjvxAuNPlayChannelId(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io_zerocopy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	// Following already set in componentEntry
	//_common_set.theComponentType.tp = JVX_NODE_TYPE_SPECIFIER_TYPE;
	//_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;
	
	_common_set_1io_zerocopy.num_iochans = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.bsize_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.rate_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.form_io = JVX_DATAFORMAT_DATA;
	_common_set_1io_zerocopy.subform_io = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;

	// Get the additional tools
	theText2Speech_obj = NULL;
	theText2Speech_hdl = NULL;
	
	playback_status = JVX_CHANNEL_OUTPUT_STATE_READY;
	mixT2S.initialize();
}

CjvxAuNPlayChannelId::~CjvxAuNPlayChannelId()
{
	mixT2S.terminate();
}

// ===================================================================

jvxErrorType
CjvxAuNPlayChannelId::activate()
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::activate();
	if (res == JVX_NO_ERROR)
	{
		genChannelPlayback_node::init_all();
		genChannelPlayback_node::allocate_all();
		genChannelPlayback_node::register_all(static_cast<CjvxProperties*>(this));
		genChannelPlayback_node::register_callbacks(static_cast<CjvxProperties*>(this), 
			set_processing_props,
			reinterpret_cast<jvxHandle*>(this), NULL);

		playback_status = JVX_CHANNEL_OUTPUT_STATE_READY;
		jvx_bitZSet(genChannelPlayback_node::playback.control_status.current_status.value.selection(), 0);
		genChannelPlayback_node::playback.control_status.current_channel_number.value = 0;
	}
	return res;
}

jvxErrorType
CjvxAuNPlayChannelId::deactivate()
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::deactivate();
	if (res == JVX_NO_ERROR)
	{
		genChannelPlayback_node::unregister_all(static_cast<CjvxProperties*>(this));
		genChannelPlayback_node::deallocate_all();
	}
	return res;
}

// ===================================================================

jvxErrorType
CjvxAuNPlayChannelId::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1io::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	jvxErrorType resL = JVX_NO_ERROR;
	if (res == JVX_NO_ERROR)
	{
		// Get the additional tools
		theText2Speech_obj = NULL;
		theText2Speech_hdl = NULL;
		/*
		extensions.theResamplerUp_obj = NULL;
		extensions.theResamplerDown_obj = NULL;
		extensions.theResamplerUp_hdl = NULL;
		extensions.theResamplerDown_hdl = NULL;
		*/

		if (_common_set.theToolsHost)
		{
			_common_set.theToolsHost->reference_tool(jvxComponentIdentification(JVX_COMPONENT_TEXT2SPEECH, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED), 
				&theText2Speech_obj,
				0, NULL);
			if (theText2Speech_obj)
			{
				resL = theText2Speech_obj->request_specialization(reinterpret_cast<jvxHandle**>(&theText2Speech_hdl), NULL, NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = theText2Speech_hdl->initialize(_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = theText2Speech_hdl->prepare();
				assert(resL == JVX_NO_ERROR);
			}

			mixT2S.prepare(
				theText2Speech_hdl,
				genChannelPlayback_node::playback.control_status.pre_text_announce.value,
				_common_set_ldslave.theData_in->con_params.rate,
				_common_set_ldslave.theData_in->con_params.buffersize,
				_common_set_ldslave.theData_in->con_params.number_channels,
				genChannelPlayback_node::playback.control_status.channel_number_start_offset.value);

			playback_status = JVX_CHANNEL_OUTPUT_STATE_READY;
			jvx_bitZSet(genChannelPlayback_node::playback.control_status.current_status.value.selection(), 0);
			genChannelPlayback_node::playback.control_status.current_channel_number.value = 0;
		}
	}
	return res;
}

jvxErrorType
CjvxAuNPlayChannelId::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = CjvxBareNode1io::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		mixT2S.trigger_stop();

		// If we are here, no more buffer switches occur
		playback_status = JVX_CHANNEL_OUTPUT_STATE_READY;
		jvx_bitZSet(genChannelPlayback_node::playback.control_status.current_status.value.selection(), 0);

		mixT2S.postprocess();

		if (theText2Speech_obj)
		{
			resL = theText2Speech_hdl->postprocess();
			assert(resL == JVX_NO_ERROR);
			resL = theText2Speech_hdl->terminate();
			assert(resL == JVX_NO_ERROR);

			_common_set.theToolsHost->return_reference_tool(jvxComponentIdentification(JVX_COMPONENT_TEXT2SPEECH, 
				JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED), theText2Speech_obj);
		}

		theText2Speech_obj = NULL;
		theText2Speech_hdl = NULL;
	}
	return res;
}

jvxErrorType
CjvxAuNPlayChannelId::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idx_play = JVX_SIZE_UNSELECTED;
	if (playback_status == JVX_CHANNEL_OUTPUT_STATE_START_PENDING)
	{
		jvx_bitZSet(genChannelPlayback_node::playback.control_status.current_status.value.selection(), 1);
		mixT2S.trigger_start();
		playback_status = JVX_CHANNEL_OUTPUT_STATE_PROCESSING;
		jvx_bitZSet(genChannelPlayback_node::playback.control_status.current_status.value.selection(), 2);

	}

	if (playback_status == JVX_CHANNEL_OUTPUT_STATE_PROCESSING)
	{
		jvxSize idx = idx_stage;
		if (JVX_CHECK_SIZE_UNSELECTED(idx))
		{
			idx = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
		}
		mixT2S.process((jvxData**)_common_set_ldslave.theData_out.con_data.buffers[idx],
			genChannelPlayback_node::playback.control_status.output_volume.value.val(),
			true, &idx_play);
		if (JVX_CHECK_SIZE_UNSELECTED(idx_play))
		{
			playback_status = JVX_CHANNEL_OUTPUT_STATE_COMPLETE;
		}
		genChannelPlayback_node::playback.control_status.current_channel_number.value = idx_play;
	}
	if (playback_status == JVX_CHANNEL_OUTPUT_STATE_STOP_PENDING)
	{
		mixT2S.trigger_stop();
		jvx_bitZSet(genChannelPlayback_node::playback.control_status.current_status.value.selection(), 3);
		playback_status = JVX_CHANNEL_OUTPUT_STATE_COMPLETE;
	}
	if (playback_status == JVX_CHANNEL_OUTPUT_STATE_COMPLETE)
	{
		// This module stops by itself
		playback_status = JVX_CHANNEL_OUTPUT_STATE_READY;
		jvx_bitZSet(genChannelPlayback_node::playback.control_status.current_status.value.selection(), 0);
	}

	// Forward the processed data to next chain element
	res = CjvxBareNode1io::process_buffers_icon(mt_mask, idx_stage);
	return res;
}

/*
void 
CjvxAuNPlayChannelId::test_set_output_parameters()
{
	
}
*/

// ===================================================================
// ===================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNPlayChannelId, set_processing_props)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genChannelPlayback_node::playback.control_status.trigger_start))
	{
		if (playback_status == JVX_CHANNEL_OUTPUT_STATE_READY)
		{
			playback_status = JVX_CHANNEL_OUTPUT_STATE_START_PENDING;
		}
		if (playback_status == JVX_CHANNEL_OUTPUT_STATE_PROCESSING)
		{
			playback_status = JVX_CHANNEL_OUTPUT_STATE_STOP_PENDING;
		}
		genChannelPlayback_node::playback.control_status.trigger_start.value = c_false;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNPlayChannelId::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1io_zerocopy::put_configuration(callMan,
		processor, sectionToContainAllSubsectionsForMe,
		filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			genChannelPlayback_node::put_configuration_all(callMan,
				processor, sectionToContainAllSubsectionsForMe,
				&warns);
			for (i = 0; i < warns.size(); i++)
			{
				std::cout << __FUNCTION__ << ": Warning: " << warns[i] << std::endl;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxAuNPlayChannelId::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1io_zerocopy::get_configuration(callMan,
		processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genChannelPlayback_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	}
	return res;
}

jvxErrorType 
CjvxAuNPlayChannelId::get_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	return CjvxBareNode1io_zerocopy::get_property(callGate,
		rawPtr, ident, trans);
}