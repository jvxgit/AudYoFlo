#include "CjvxSpNSpeaker2Binaural.h"

CjvxSpNSpeaker2Binaural::CjvxSpNSpeaker2Binaural(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	/*
	 * Configurations:
	 * 
	 * Enable zero copy
	_common_set_ldslave.zeroCopyBuffering_cfg = true;

	 * Automatic prepare
	_common_set_node_base_1io.prepareOnChainPrepare = true;
	_common_set_node_base_1io.startOnChainStart = true;
	_common_set_node_base_1io.impPrepareOnChainPrepare = true;
	_common_set_node_base_1io.impStartOnChainStart = true;

	 * Default values for data parameters
	_common_set_1io_zerocopy.num_iochans = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.bsize_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.rate_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.form_io = JVX_DATAFORMAT_NONE;
	_common_set_1io_zerocopy.subform_io = JVX_DATAFORMAT_GROUP_NONE;
	
	 * Complaints about data prameters are directly forwarded
	forward_complain = true;
	 */
	if (force2OutputChannels)
	{
		neg_output._update_parameters_fixed(2);
	}
}

CjvxSpNSpeaker2Binaural::~CjvxSpNSpeaker2Binaural()
{	
} 

jvxErrorType
CjvxSpNSpeaker2Binaural::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::select(owner);
	if (res == JVX_NO_ERROR)
	{
		/*
		 * Activate all properties
		 */
		genSpeaker2Binaural_node::init_all();
		genSpeaker2Binaural_node::allocate_all();
		genSpeaker2Binaural_node::register_all(static_cast<CjvxProperties*>(this));
		genSpeaker2Binaural_node::register_callbacks(static_cast<CjvxProperties*>(this),
			set_bypass_mode,
			select_hrtf_file,
			set_speaker_posthook,
			set_parameter_speaker,
			get_parameter_speaker,
			static_cast<jvxHandle*>(this), NULL);

		update_bypass_mode();
	}
	return res;
}

jvxErrorType
CjvxSpNSpeaker2Binaural::unselect()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::unselect();
	if (res == JVX_NO_ERROR)
	{
		genSpeaker2Binaural_node::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);
		genSpeaker2Binaural_node::unregister_all(static_cast<CjvxProperties*>(this));
		genSpeaker2Binaural_node::deallocate_all();
	}
	return res;
}


jvxErrorType
CjvxSpNSpeaker2Binaural::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genSpeaker2Binaural_node::mode.slave_mode);

		update_properties(false);
	}
	return res;
}

jvxErrorType
CjvxSpNSpeaker2Binaural::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		_undo_update_property_access_type(
			genSpeaker2Binaural_node::mode.slave_mode);

		CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}


// =====================================================================================

#include "jvx-helpers-cpp.h"
jvxErrorType
CjvxSpNSpeaker2Binaural::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (!genSpeaker2Binaural_node::config.bypass.value)
	{
		assert(zeroCopyBuffering_rt == false);
		jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_ldslave.theData_in, idx_stage);
		jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(&_common_set_ldslave.theData_out);

		// Copy input to output. We need two buffers since we do not apply zero copy processing!
		if (engageRendererProc)
		{
			filtLeft.process_N1Filter(bufsIn, bufsOut[0], _common_set_ldslave.theData_in->con_params.number_channels,
				_common_set_ldslave.theData_in->con_params.buffersize);
			filtRight.process_N1Filter(bufsIn, bufsOut[1], _common_set_ldslave.theData_in->con_params.number_channels,
				_common_set_ldslave.theData_in->con_params.buffersize);
		}
	}
	else
	{
		// Nothing to do here, just bypass processing
	}
	

	// Forward the processed data to next chain element
	res = _process_buffers_icon(mt_mask, idx_stage);
	return res;
}

// =====================================================================================

jvxErrorType
CjvxSpNSpeaker2Binaural::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::put_configuration(
		callMan,
		processor,
		sectionToContainAllSubsectionsForMe,
		filename,
		lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			std::vector<std::string> warns;
			genSpeaker2Binaural_node::put_configuration_all(
				callMan,
				processor,
				sectionToContainAllSubsectionsForMe,
				&warns);
			JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_STD_COUT(warns);

			update_bypass_mode(false);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNSpeaker2Binaural::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::get_configuration(
		callMan,
		processor,
		sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genSpeaker2Binaural_node::get_configuration_all(
			callMan,
			processor,
			sectionWhereToAddAllSubsections);
	}
	return JVX_NO_ERROR;
}

// =====================================================================================

jvxErrorType
CjvxSpNSpeaker2Binaural::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxSpNSpeaker2Binaural::accept_input_parameters_stop(jvxErrorType resTest)
{
	if (force2OutputChannels)
	{
		return CjvxBareNode1ioRearrange::accept_input_parameters_stop(resTest);
	}
	return CjvxBareNode1io::accept_input_parameters_stop(resTest);
}

void
CjvxSpNSpeaker2Binaural::from_input_to_output()
{
	if (force2OutputChannels)
	{
		return CjvxBareNode1ioRearrange::from_input_to_output();
	}
	return CjvxBareNode1io::from_input_to_output();
}

jvxErrorType
CjvxSpNSpeaker2Binaural::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (force2OutputChannels)
	{
		return CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return CjvxBareNode1io::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

void
CjvxSpNSpeaker2Binaural::test_set_output_parameters()
{
	if (force2OutputChannels)
	{
		return CjvxBareNode1ioRearrange::test_set_output_parameters();
	}
	return CjvxBareNode1io::test_set_output_parameters();
}

// =====================================================================================

jvxErrorType
CjvxSpNSpeaker2Binaural::prepare()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::prepare();
	if (res == JVX_NO_ERROR)
	{
		// Lock the bypass property
		CjvxProperties::_update_properties_on_start();
	}
	return res;
}

jvxErrorType
CjvxSpNSpeaker2Binaural::postprocess()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::postprocess();
	if (res == JVX_NO_ERROR)
	{
		// Unlock the bypass property
		CjvxProperties::_update_properties_on_stop();
	}
	return res;
}

// =====================================================================================

jvxErrorType 
CjvxSpNSpeaker2Binaural::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		engageRendererProc = false;
		if (genSpeaker2Binaural_node::config.bypass.value == false)
		{
			engageRendererProc = true;
		}

		if (engageRendererProc)
		{
			filtLeft.prepare_N1Filter(_common_set_ldslave.theData_in->con_params.buffersize);
			filtRight.prepare_N1Filter(_common_set_ldslave.theData_in->con_params.buffersize);
		}
	}
	return res;
}

jvxErrorType 
CjvxSpNSpeaker2Binaural::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (engageRendererProc)
		{
			filtLeft.postprocess_N1Filter();
			filtRight.postprocess_N1Filter();
		}
		engageRendererProc = false;
	}
	return res;
}

// =========================================================================================

jvxErrorType
CjvxSpNSpeaker2Binaural::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	std::string conf_token;
	std::string conf_txt;
	jvxErrorType resP = JVX_ERROR_INVALID_FORMAT;

	jvxDataConnectionsTransferConfigToken* cfg = (jvxDataConnectionsTransferConfigToken*)data;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_CONFIG_TOKEN:
		conf_token = cfg->token_descr;
		conf_txt = cfg->token_content;

		if (conf_token == "binaural_render") 
		{
			if (genSpeaker2Binaural_node::mode.slave_mode.value)
			{
				resP = read_hrtf_settings(conf_token, conf_txt, "<passed-conf-token>");
				if (resP == JVX_NO_ERROR)
				{
					return resP;
				}
			}
			else
			{
				std::cout << __FUNCTION__ << ": Not accepting transferred configuration: Component is not in slave mode." << std::endl;
			}
		}
		break;
	}
	return CjvxBareNode1ioRearrange::transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}
