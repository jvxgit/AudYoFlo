#include "CjvxSpNSpeaker2Binaural.h"
#include "jvx-helpers-cpp.h"

CjvxSpNSpeaker2Binaural::CjvxSpNSpeaker2Binaural(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io_zerocopy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	idSelectChannel = JVX_SIZE_UNSELECTED;
	forward_complain = true;
}

CjvxSpNSpeaker2Binaural::~CjvxSpNSpeaker2Binaural()
{	
}

// ===================================================================
jvxErrorType
CjvxSpNSpeaker2Binaural::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genSpeakerEqualizer_node::init_all();
		genSpeakerEqualizer_node::allocate_all();
		genSpeakerEqualizer_node::register_all(static_cast<CjvxProperties*>(this));
		genSpeakerEqualizer_node::register_callbacks(static_cast<CjvxProperties*>(this),
			set_channel_posthook,
			select_eq_file_posthook,
			set_parameter_channel_posthook,
			get_parameter_channel_prehook,
			reinterpret_cast<jvxHandle*>(this), NULL);
	}
	return res;
}

jvxErrorType
CjvxSpNSpeaker2Binaural::unselect()
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::unselect();
	if (res == JVX_NO_ERROR)
	{
		genSpeakerEqualizer_node::unregister_all(static_cast<CjvxProperties*>(this));
		genSpeakerEqualizer_node::deallocate_all();
	}
	return res;
}


jvxErrorType
CjvxSpNSpeaker2Binaural::activate()
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::activate();
	if (res == JVX_NO_ERROR)
	{	
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genSpeakerEqualizer_node::mode.slave_mode.cat,
			genSpeakerEqualizer_node::mode.slave_mode.id);

		update_properties();
		
		// no data parameter constraints on init. The constraint will be set on 
		// Selection of eq
	}
	return res;
}

jvxErrorType
CjvxSpNSpeaker2Binaural::deactivate()
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		_undo_update_property_access_type(
			genSpeakerEqualizer_node::mode.slave_mode.cat,
			genSpeakerEqualizer_node::mode.slave_mode.id);

		CjvxBareNode1io_zerocopy::deactivate();
	}
	return res;
}

// ===================================================================

jvxErrorType 
CjvxSpNSpeaker2Binaural::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	/*
	if (_common_set_ldslave.theData_in->con_params.number_channels != CjvxNode_genpcg::node.numberinputchannels.value)
	{
		// genMeasureIr_node::config.number_input_channels_max.value = _common_set_ldslave.theData_in->con_params.number_channels;
		neg_input._update_parameters_fixed(_common_set_ldslave.theData_in->con_params.number_channels);
	}
	*/
	res = CjvxBareNode1io_zerocopy::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		/*
		CjvxNode_genpcg::node.numberoutputchannels.value = (jvxInt32)_common_set_ldslave.theData_out.con_params.number_channels;
		if (
			(genMeasureIr_node::measurements.measurement_out_channels.value.entries.size() != CjvxNode_genpcg::node.numberoutputchannels.value) ||
			(genMeasureIr_node::measurements.measurement_in_channels.value.entries.size() != CjvxNode_genpcg::node.numberinputchannels.value))
		{
			reconstruct_properties(selMeasures, false);
		}
		*/
	}

	return res;
}

void
CjvxSpNSpeaker2Binaural::test_set_output_parameters()
{
	// Leave number channels open
	neg_output._update_parameters_fixed(
		CjvxNode_genpcg::node.numberoutputchannels.value,
		CjvxNode_genpcg::node.buffersize.value,
		CjvxNode_genpcg::node.samplerate.value,
		(jvxDataFormat)CjvxNode_genpcg::node.format.value,
		(jvxDataFormatGroup)CjvxNode_genpcg::node.subformat.value,
		&_common_set_ldslave.theData_out);
}

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

		if (genSpeakerEqualizer_node::mode.slave_mode.value)
		{
			resP = read_eq_settings(conf_token, conf_txt, "<passed-conf-token>");
			if (resP == JVX_NO_ERROR)
			{
				return resP;
			}
		}
		else
		{
			std::cout << __FUNCTION__ << ": Not accepting transferred configuration: Component is not in slave mode." << std::endl;
		}
		break;
	}
	return CjvxBareNode1io_zerocopy::transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType 
CjvxSpNSpeaker2Binaural::read_eq_settings(const std::string& token,
	const std::string& txt, const std::string& fName)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConfigProcessor* confProc = NULL;
	IjvxObject* cfgObj = NULL;

	if ((std::string)token == "speaker_eq")
	{
		std::cout << __FUNCTION__ << ": Accepting configuration passed via chain." << std::endl;
		_common_set.theToolsHost->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &cfgObj, 0, NULL);
		if (cfgObj)
		{
			cfgObj->request_specialization(reinterpret_cast<jvxHandle**>(&confProc), NULL, NULL);
			if (confProc)
			{
				clear_config();
				neg_input._clear_parameters_fixed(true);
				forward_complain = true;
				jvx_bitFClear(genSpeakerEqualizer_node::mode.function.value.selection);
				genSpeakerEqualizer_node::monitor.last_error.value.clear();

				res = confProc->parseTextField(txt.c_str(), fName.c_str(), 0);

				if (res == JVX_NO_ERROR)
				{
					jvxConfigData* cfgData = NULL;
					confProc->getConfigurationHandle(&cfgData);
					if (cfgData)
					{
						jvxApiString astr;
						confProc->getNameCurrentEntry(cfgData, &astr);

						if (astr.std_str() == "speaker_eq")
						{
							std::string err;

							res = allocate_config(
								confProc, cfgData,
								err, JVX_SIZE_UNSELECTED);
							if (res == JVX_NO_ERROR)
							{
								std::cout << __FUNCTION__ << ": Configuration successfully processed." << std::endl;
								forward_complain = false;
								if (proc_data.delayEng)
								{
									jvx_bitSet(genSpeakerEqualizer_node::mode.function.value.selection, 0);
								}
								if (proc_data.gainEng)
								{
									jvx_bitSet(genSpeakerEqualizer_node::mode.function.value.selection, 1);
								}
								if (proc_data.firEng)
								{
									jvx_bitSet(genSpeakerEqualizer_node::mode.function.value.selection, 2);
								}
								genSpeakerEqualizer_node::monitor.last_error.value.clear();
							}
							else
							{
								std::cout << __FUNCTION__ << ": Configuration unsuccessfully, reason: " << err << "." << std::endl;
								genSpeakerEqualizer_node::monitor.last_error.value = err;
							}
						}
						confProc->removeHandle(cfgData);
					}
				}
				else
				{
					jvxApiError err;
					confProc->getParseError(&err);
					std::cout << __FUNCTION__ << ": In file " << fName << ": parse error: " << err.errorDescription.std_str() << std::endl;
					genSpeakerEqualizer_node::monitor.last_error.value = err.errorDescription.std_str();
					res = JVX_ERROR_INVALID_ARGUMENT;
				}

				neg_input._update_parameters_fixed(numChannels);
				update_properties();

				_common_set.theToolsHost->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, cfgObj);
			}
		}
	}
	return res;
}


jvxErrorType
CjvxSpNSpeaker2Binaural::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor* ld = (jvxLinkDataDescriptor*)data;

	// Accept the proposed number of output channels
	/*
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		res = neg_output._negotiate_transfer_backward_ocon(
			ld, &_common_set_ldslave.theData_out,
			this, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res == JVX_NO_ERROR)
		{
			CjvxNode_genpcg::node.numberoutputchannels.value = (jvxInt32)ld->con_params.number_channels;
			neg_output._update_parameters_fixed(CjvxNode_genpcg::node.numberoutputchannels.value,
				JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED,
				JVX_DATAFORMAT_NONE, JVX_DATAFORMAT_GROUP_NONE,
				&_common_set_ldslave.theData_out);
		}
		return res;
		break;
	}*/
	res = CjvxBareNode1io_zerocopy::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return res;
}

// ===================================================================

jvxErrorType
CjvxSpNSpeaker2Binaural::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		proc_eq_involved = false;
		if (JVX_CHECK_SIZE_SELECTED(numChannels))
		{
			prepare_eq(_common_set_ldslave.theData_in->con_params.buffersize);
			proc_eq_involved = true;
		}
	}
	return res;
}

// void copy_measurement(jvxSize cnt, jvxData* ptr);

jvxErrorType
CjvxSpNSpeaker2Binaural::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxBareNode1io_zerocopy::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (proc_eq_involved)
		{
			postprocess_eq();
		}
	}
	return res;
}

jvxErrorType
CjvxSpNSpeaker2Binaural::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (proc_eq_involved)
	{
		if (!genSpeakerEqualizer_node::config.bypass.value)
		{
			jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_ldslave.theData_in, idx_stage);
			jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(&_common_set_ldslave.theData_out);

			process_eq_iplace(bufsIn, _common_set_ldslave.theData_out.con_params.number_channels, _common_set_ldslave.theData_out.con_params.buffersize);
		}
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
	jvxErrorType res = CjvxBareNode1io_zerocopy::put_configuration(
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
			genSpeakerEqualizer_node::put_configuration_all(
				callMan,
				processor,
				sectionToContainAllSubsectionsForMe,
				warns);
			JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_STD_COUT(warns);

			if (!genSpeakerEqualizer_node::config.eq_definition_file.value.empty())
			{
				jvxErrorType resL = try_read_eq_file(genSpeakerEqualizer_node::config.eq_definition_file.value);
				if (resL != JVX_NO_ERROR)
				{
					genSpeakerEqualizer_node::config.eq_definition_file.value.clear();
				}
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSpNSpeaker2Binaural::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::get_configuration(
		callMan,
		processor,
		sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genSpeakerEqualizer_node::get_configuration_all(
			callMan,
			processor,
			sectionWhereToAddAllSubsections);
	}
	return JVX_NO_ERROR;
}

void 
CjvxSpNSpeaker2Binaural::update_properties()
{
	jvxSize i;
	jvxSize oldSelect = JVX_SIZE_UNSELECTED;
	
	CjvxSpNSpeaker2Binaural::config.select_channel.value.entries.clear(); 
	if (JVX_CHECK_SIZE_UNSELECTED(numChannels))
	{
		CjvxSpNSpeaker2Binaural::config.select_channel.valid = false;		
		CjvxSpNSpeaker2Binaural::mode.involved.valid = false;
		CjvxSpNSpeaker2Binaural::parameter.delay.valid = false;
		CjvxSpNSpeaker2Binaural::parameter.fir_eq.valid = false;
		CjvxSpNSpeaker2Binaural::parameter.gain.valid = false;
	}
	else
	{
		for (i = 0; i < numChannels; i++)
		{
			CjvxSpNSpeaker2Binaural::config.select_channel.value.entries.push_back(("CHANNEL #" + jvx_size2String(i)).c_str());
		}
		jvx_bitFClear(CjvxSpNSpeaker2Binaural::config.select_channel.value.selection);

		if (oldSelect >= CjvxSpNSpeaker2Binaural::config.select_channel.value.entries.size())
		{
			if (CjvxSpNSpeaker2Binaural::config.select_channel.value.entries.size())
			{
				oldSelect = 0;
				CjvxSpNSpeaker2Binaural::config.select_channel.valid = true;
				CjvxSpNSpeaker2Binaural::mode.involved.valid = true;
				CjvxSpNSpeaker2Binaural::parameter.delay.valid = true;
				CjvxSpNSpeaker2Binaural::parameter.fir_eq.valid = true;
				CjvxSpNSpeaker2Binaural::parameter.gain.valid = true;
			}
			else
			{
				oldSelect = JVX_SIZE_UNSELECTED;

			}
		}
		if (JVX_CHECK_SIZE_SELECTED(oldSelect))
		{
			idSelectChannel = oldSelect;
			jvx_bitZSet(CjvxSpNSpeaker2Binaural::config.select_channel.value.selection,
				idSelectChannel);
		}
	}

	if( genSpeakerEqualizer_node::mode.slave_mode.value)
	{
		CjvxSpNSpeaker2Binaural::config.eq_definition_file.valid = false;
		CjvxSpNSpeaker2Binaural::config.clear_definition_file.valid = false;
	}
}

// =======================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNSpeaker2Binaural, set_channel_posthook)
{
	jvxSize newSelect = jvx_bitfieldSelection2Id(CjvxSpNSpeaker2Binaural::config.select_channel.value.selection,
		CjvxSpNSpeaker2Binaural::config.select_channel.value.entries.size());
		
	if (newSelect != idSelectChannel)
	{
		jvxCBitField prio = 0;
		jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);

		idSelectChannel = newSelect;
		CjvxObject::_report_command_request(prio);
	}
	
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNSpeaker2Binaural, set_parameter_channel_posthook)
{
	if (JVX_CHECK_SIZE_SELECTED(numChannels))
	{
		if (idSelectChannel < numChannels)
		{
			if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, genSpeakerEqualizer_node::parameter.delay))
			{
				proc_data.delayChan[idSelectChannel] = genSpeakerEqualizer_node::parameter.delay.value;
			}
			else if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, genSpeakerEqualizer_node::parameter.gain))
			{
				proc_data.gainFac[idSelectChannel] = genSpeakerEqualizer_node::parameter.gain.value;
			}
			else if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, genSpeakerEqualizer_node::parameter.fir_eq))
			{
				jvxSize nUsed = 0;
				std::string txt = genSpeakerEqualizer_node::parameter.fir_eq.value;
				jvx_string2ValueList(txt, proc_data.fir_eq[idSelectChannel], JVX_DATAFORMAT_DATA, firEqSize, &nUsed);
			}
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNSpeaker2Binaural, get_parameter_channel_prehook)
{
	if (JVX_CHECK_SIZE_SELECTED(numChannels))
	{
		if (idSelectChannel < numChannels)
		{
			if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, genSpeakerEqualizer_node::parameter.delay))
			{
				genSpeakerEqualizer_node::parameter.delay.value = proc_data.delayChan[idSelectChannel];
			}
			else if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, genSpeakerEqualizer_node::parameter.gain))
			{
				genSpeakerEqualizer_node::parameter.gain.value = proc_data.gainFac[idSelectChannel];
			}
			else if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, genSpeakerEqualizer_node::parameter.fir_eq))
			{
				std::string txt;
				current_fir_eq(idSelectChannel, txt);
				genSpeakerEqualizer_node::parameter.fir_eq.value = txt;
			}
		}
	}
	return JVX_NO_ERROR;
}


JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNSpeaker2Binaural, select_eq_file_posthook)
{
	if (!genSpeakerEqualizer_node::mode.slave_mode.value)
	{
		if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, genSpeakerEqualizer_node::config.eq_definition_file))
		{
			
			std::string fName = genSpeakerEqualizer_node::config.eq_definition_file.value;
			try_read_eq_file(fName);
		} // if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, genSpeakerEqualizer_node::config.eq_definition_file))

		if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, genSpeakerEqualizer_node::config.clear_definition_file))
		{
			this->read_eq_settings("speaker_eq", "", "");
			genSpeakerEqualizer_node::config.eq_definition_file.value = "";
			reportRequestTestChainMaster();
			genSpeakerEqualizer_node::config.clear_definition_file.value = c_false;
		}
	}
	else
	{
		// Module is in slave mode! 
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNSpeaker2Binaural::try_read_eq_file(const std::string& fName)
{
	std::string content;
	std::string eq_settings;
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;

	jvxErrorType resL = jvx_readContentFromFile(fName, content);
	if (resL == JVX_NO_ERROR)
	{
		IjvxObject* cfgObj = NULL;
		IjvxConfigProcessor* confProc = NULL;

		_common_set.theToolsHost->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &cfgObj, 0, NULL);
		if (cfgObj)
		{
			cfgObj->request_specialization(reinterpret_cast<jvxHandle**>(&confProc), NULL, NULL);
			if (confProc)
			{
				resL = confProc->parseTextField(content.c_str(), fName.c_str(), 0);
				if (resL == JVX_NO_ERROR)
				{
					jvxConfigData* cfgData = NULL;
					confProc->getConfigurationHandle(&cfgData);
					if (cfgData)
					{
						jvxSize sz, i;
						confProc->getNumberEntriesCurrentSection(cfgData, &sz);
						for (i = 0; i < sz; i++)
						{
							jvxConfigData* subSection = NULL;
							confProc->getReferenceEntryCurrentSection_id(cfgData, &subSection, i);
							if (subSection)
							{
								jvxConfigData* subSubSection = NULL;
								confProc->getReferenceEntryCurrentSection_name(subSection, &subSubSection, "speaker_eq");
								if (subSubSection)
								{
									jvxApiString astr;
									confProc->printConfiguration(subSubSection, &astr, true);
									eq_settings = astr.std_str();
									break;
								}
							}
						}
						confProc->removeHandle(cfgData);
					}
				}
				else
				{
					jvxApiError err;
					confProc->getParseError(&err);
					std::cout << "In file " << fName << ": parse error: " << err.errorDescription.std_str() << std::endl;
					res = JVX_ERROR_PARSE_ERROR;
				}
			}
		}
		_common_set.theToolsHost->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, cfgObj);
		if (!eq_settings.empty())
		{
			res = JVX_NO_ERROR;
			this->read_eq_settings("speaker_eq", eq_settings, fName);
			reportRequestTestChainMaster();
		}
	} // if (resL == JVX_NO_ERROR)
	else
	{
		if (fName.empty())
		{
			clear_config();
			neg_input._clear_parameters_fixed(true);
			forward_complain = true;
			jvx_bitFClear(genSpeakerEqualizer_node::mode.function.value.selection);
			genSpeakerEqualizer_node::monitor.last_error.value.clear();
			update_properties();
			reportRequestTestChainMaster();
		}
		std::cout << ": Failed to open file <" << fName << ">." << std::endl;
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return res;
}
