#include "jvxAudioTechnologies/CjvxAudioMasterDevice.h"

CjvxAudioMasterDevice::CjvxAudioMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxSimpleMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_DEVICE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);

	last_user_interaction.last_selection_user_input_channels = JVX_SIZE_UNSELECTED;
	last_user_interaction.last_selection_user_output_channels = JVX_SIZE_UNSELECTED;
}

CjvxAudioMasterDevice::~CjvxAudioMasterDevice()
{
	this->terminate();
}

jvxErrorType
CjvxAudioMasterDevice::select(IjvxObject* owner)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _select(owner);
	if (res == JVX_NO_ERROR)
	{
		// CjvxDeviceCaps_genpcg::device_caps.dev_avail.value = 0;
	}
	return res;
}

jvxErrorType
CjvxAudioMasterDevice::unselect()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _unselect();
	if (res == JVX_NO_ERROR)
	{
		// CjvxDeviceCaps_genpcg::device_caps.dev_avail.value = 1;
	}
	return res;
}

jvxErrorType
CjvxAudioMasterDevice::activate()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	res = _activate();
	if (res == JVX_NO_ERROR)
	{
		last_user_interaction.last_selection_user_input_channels = JVX_SIZE_UNSELECTED;
		last_user_interaction.last_selection_user_output_channels = JVX_SIZE_UNSELECTED;

		activate_connectors_master();

		init__properties_active();
		allocate__properties_active();

		CjvxAudioDevice_genpcg::properties_active.sourceName.value = _common_set.theName;
		CjvxAudioDevice_genpcg::properties_active.sinkName.value = _common_set.theName;

		register__properties_active(static_cast<CjvxProperties*>(this));

		//_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, JVX_PROPERTY_CATEGORY_PREDEFINED, CjvxAudioMasterDevice::properties_active.format.globalIdx);

		for (i = 0; i < _common_set_audio_device.formats.size(); i++)
		{
			CjvxAudioMasterDevice::properties_active.formatselection.value.entries.push_back(jvxDataFormat_txt(_common_set_audio_device.formats[i]));
		}
		if (_common_set_audio_device.formats.size() > 0)
		{
			CjvxAudioMasterDevice::properties_active.format.value = (jvxInt16)_common_set_audio_device.formats[0];
			CjvxAudioMasterDevice::properties_active.formatselection.value.selection() = ((jvxBitField)1 << 0);
			CjvxAudioMasterDevice::properties_active.formatselection.value.exclusive = (jvxBitField)-1;
		}
		else
		{
			CjvxAudioMasterDevice::properties_active.format.value = (jvxInt16)JVX_DATAFORMAT_NONE;
		}

		this->updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);
	}
	return(res);
}

jvxErrorType
CjvxAudioMasterDevice::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _deactivate();
	if (res == JVX_NO_ERROR)
	{

		unregister__properties_active(static_cast<CjvxProperties*>(this));
		//deassociate__properties_active(static_cast<CjvxProperties*>(this));

		deallocate__properties_active();

		CjvxAudioMasterDevice::properties_active.inputchannelselection.value.entries.clear();
		CjvxAudioMasterDevice::properties_active.inputchannelselection.value.selection() = 0;
		CjvxAudioMasterDevice::properties_active.inputchannelselection.value.exclusive = 0;

		CjvxAudioMasterDevice::properties_active.outputchannelselection.value.entries.clear();
		CjvxAudioMasterDevice::properties_active.outputchannelselection.value.selection() = 0;
		CjvxAudioMasterDevice::properties_active.outputchannelselection.value.exclusive = 0;

		CjvxAudioMasterDevice::properties_active.buffersize.value = 0;
		CjvxAudioMasterDevice::properties_active.format.value = (jvxInt16)JVX_DATAFORMAT_NONE;

		CjvxAudioMasterDevice::properties_active.formatselection.value.entries.clear();
		CjvxAudioMasterDevice::properties_active.formatselection.value.selection() = 0;
		CjvxAudioMasterDevice::properties_active.formatselection.value.exclusive = 0;
		CjvxAudioMasterDevice::properties_active.format.value = (jvxInt16)JVX_DATAFORMAT_NONE;

		CjvxAudioMasterDevice::properties_active.numberinputchannels.value = 0;
		CjvxAudioMasterDevice::properties_active.numberoutputchannels.value = 0;

		CjvxAudioMasterDevice::properties_active.samplerate.value = 0;

		JVX_DEACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER();
		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
}

jvxErrorType
CjvxAudioMasterDevice::set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& detail)
{
	jvxBool report_update = false;
	jvxErrorType res = JVX_NO_ERROR;
	res = _set_property(
		callGate,
		rawPtr,
		ident, detail);
	if (res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		this->updateDependentVariables(propId, category, false, callGate.call_purpose);
	}
	return(res);
}

jvxErrorType
CjvxAudioMasterDevice::stop_property_group(jvxCallManagerProperties& callGate)
{
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxErrorType res = _stop_property_group(callGate);
	assert(res == JVX_NO_ERROR);
	if (_common_set_properties.inPropertyGroupCnt == 0)
	{
		if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)
		{
			// inform all other devices that property has changed
			if (_common_set_ld_master.refProc)
			{
				_common_set_ld_master.refProc->unique_id_connections(&uId);
				this->_request_test_chain_master(uId);
			}
		}
	}
	return res;
}

jvxErrorType
CjvxAudioMasterDevice::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

	if (this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		jvxBitField selFormatOld = CjvxAudioDevice_genpcg::properties_active.formatselection.value.selection();
		// Read all parameters from this class
		CjvxAudioMasterDevice::put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		if (!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for (i = 0; i < warnings.size(); i++)
			{
				txt = "Component <";
				txt += _common_set_min.theDescription;
				txt += ">: " + warnings[i];
				_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}
		}

		// If something goes wrong in the configuration let us define a minimum in selection here!!
		if (JVX_EVALUATE_BITFIELD(CjvxAudioDevice_genpcg::properties_active.formatselection.value.selection()) == false)
		{
			// If there is a wrong format in the config file it might be good to retun to the old value
			CjvxAudioDevice_genpcg::properties_active.formatselection.value.selection() = selFormatOld;
		}

		// Update all dependant props
		this->updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

	}
	return(res);
}

jvxErrorType
CjvxAudioMasterDevice::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{

	// Write all parameters from this class
	if (_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		CjvxAudioMasterDevice::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);
	}
	return(JVX_NO_ERROR);
}

void
CjvxAudioMasterDevice::updateDependentVariables(
	jvxSize propId, jvxPropertyCategoryType category,
	jvxBool updateAll, jvxPropertyCallPurpose callPurpose,
	jvxBool suppress_update_chain)
{
	jvxSize i;
	jvxInt32 newValue = 0;
	jvxBool updateChain = false;
	if (category == JVX_PROPERTY_CATEGORY_PREDEFINED)
	{
		if (
			(propId == CjvxAudioMasterDevice::properties_active.inputchannelselection.globalIdx) ||
			(propId == CjvxAudioMasterDevice::properties_active.numberinputchannels.globalIdx) ||
			updateAll)
		{
			for (i = 0; i < (int)CjvxAudioMasterDevice::properties_active.inputchannelselection.value.entries.size(); i++)
			{
				if (jvx_bitTest(CjvxAudioMasterDevice::properties_active.inputchannelselection.value.selection(), i))
				{
					newValue++;
				}
			}
			CjvxAudioMasterDevice::properties_active.numberinputchannels.value = newValue;
			updateChain = true;
		}

		// =================================================================================================

		if (
			(propId == CjvxAudioMasterDevice::properties_active.outputchannelselection.globalIdx) ||
			(propId == CjvxAudioMasterDevice::properties_active.numberoutputchannels.globalIdx) ||
			updateAll)
		{
			newValue = 0;
			for (i = 0; i < (int)CjvxAudioMasterDevice::properties_active.outputchannelselection.value.entries.size(); i++)
			{
				if (jvx_bitTest(CjvxAudioMasterDevice::properties_active.outputchannelselection.value.selection(), i))
				{
					newValue++;
				}
			}
			CjvxAudioMasterDevice::properties_active.numberoutputchannels.value = newValue;
			updateChain = true;
		}

		// =================================================================================================

		jvxBool formatFromSelectionList = false;
		if (
			(propId == CjvxAudioMasterDevice::properties_active.format.globalIdx) && (!updateAll))
		{
			std::string selStr = jvxDataFormat_txt(CjvxAudioMasterDevice::properties_active.format.value);
			jvxInt32 idxSel = -1;

			for (i = 0; i < CjvxAudioMasterDevice::properties_active.formatselection.value.entries.size(); i++)
			{
				if (CjvxAudioMasterDevice::properties_active.formatselection.value.entries[i] == selStr)
				{
					idxSel = (jvxInt32)i;
					break;
				}
			}

			if (idxSel >= 0)
			{
				CjvxAudioMasterDevice::properties_active.formatselection.value.selection() = (jvxBitField)1 << idxSel;
				formatFromSelectionList = true;
			}

			if (formatFromSelectionList)
			{
				updateChain = true;
			}
			else
			{
				/*_report_text_message(("Warning: request to activate format " +
					selStr + " which is not provided by device.").c_str(), JVX_REPORT_PRIORITY_WARNING);
					*/
			}
		}

		if (
			(propId == CjvxAudioMasterDevice::properties_active.formatselection.globalIdx) ||
			updateAll || formatFromSelectionList)
		{
			std::string selStr = jvxDataFormat_txt(JVX_DATAFORMAT_NONE);
			for (i = 0; i < CjvxAudioMasterDevice::properties_active.formatselection.value.entries.size(); i++)
			{
				if (i < _common_set_audio_device.formats.size())
				{
					if (jvx_bitTest(CjvxAudioMasterDevice::properties_active.formatselection.value.selection(), i))
					{
						selStr = CjvxAudioMasterDevice::properties_active.formatselection.value.entries[i];
						break;
					}
				}
			}
			CjvxAudioMasterDevice::properties_active.format.value = jvxDataFormat_decode(selStr.c_str());			
			updateChain = true;
		}

		if (
			(propId == CjvxAudioMasterDevice::properties_active.buffersize.globalIdx) || updateAll)
		{
			updateChain = true;
		}

		if (
			(propId == CjvxAudioMasterDevice::properties_active.samplerate.globalIdx) || updateAll)
		{
			updateChain = true;
		}

		if (
			(propId == CjvxAudioMasterDevice::properties_active.activateSilenceStop.globalIdx) || updateAll)
		{
			updateChain = true;
		}

		if (
			(propId == CjvxAudioMasterDevice::properties_active.periodSilenceStop.globalIdx) || updateAll)
		{
			updateChain = true;
		}
	}

	// ============================================================================================
	if (!suppress_update_chain && updateChain && (_common_set_properties.inPropertyGroupCnt == 0))
	{
		if (callPurpose != JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)
		{
			jvxSize uId = JVX_SIZE_UNSELECTED;

			// inform all other devices that property has changed
			if (_common_set_ld_master.refProc)
			{
				_common_set_ld_master.refProc->unique_id_connections(&uId);
				this->_request_test_chain_master(uId);
			}
		}
	}
}

jvxErrorType
CjvxAudioMasterDevice::prepare()
{
	jvxErrorType res = _prepare();
	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_update_properties_on_start();

		currentSetupAudioParams(_inproc);
	}
	return(res);
}

jvxErrorType
CjvxAudioMasterDevice::postprocess()
{
	jvxErrorType res = _postprocess();
	if (res == JVX_NO_ERROR)
	{
		_inproc.samplerate = -1;
		_inproc.buffersize = -1;
		_inproc.format = JVX_DATAFORMAT_NONE;

		_inproc.numInputs = 0;
		_inproc.numOutputs = 0;

		CjvxProperties::_update_properties_on_stop();

	}
	return(res);
}

jvxErrorType
CjvxAudioMasterDevice::currentSetupAudioParams(jvxAudioParams& params)
{
	jvxSize i;

	// Determine all relevant processing parameters
	params.samplerate = CjvxAudioMasterDevice::properties_active.samplerate.value;
	params.buffersize = CjvxAudioMasterDevice::properties_active.buffersize.value;
	params.format = (jvxDataFormat)CjvxAudioMasterDevice::properties_active.format.value;

	params.numInputs = 0;
	params.numOutputs = 0;

	params.minChanCntInput = 0;
	params.minChanCntOutput = 0;

	params.maxChanCntInput = CjvxAudioMasterDevice::properties_active.inputchannelselection.value.entries.size();
	params.maxChanCntOutput = CjvxAudioMasterDevice::properties_active.outputchannelselection.value.entries.size();

	params.inChannelsSelectionMask = CjvxAudioMasterDevice::properties_active.inputchannelselection.value.selection();
	params.outChannelsSelectionMask = CjvxAudioMasterDevice::properties_active.outputchannelselection.value.selection();

	for (i = 0; i < params.maxChanCntInput; i++)
	{
		if (jvx_bitTest(params.inChannelsSelectionMask, i))
		{
			params.numInputs++;
			params.minChanCntInput = (jvxInt32)i;
		}
	}

	for (i = 0; i < params.maxChanCntOutput; i++)
	{
		if (jvx_bitTest(params.outChannelsSelectionMask, i))
		{
			params.numOutputs++;
			params.minChanCntOutput = (jvxInt32)i;
		}
	}
	return JVX_NO_ERROR;
}

void
CjvxAudioMasterDevice::updateChainOutputParameter()
{
	jvxAudioParams params;
	currentSetupAudioParams(params);

	// Do some local stuff, fill output struct
	_common_set_ocon.theData_out.con_params.buffersize = params.buffersize;
	_common_set_ocon.theData_out.con_params.rate = params.samplerate;
	_common_set_ocon.theData_out.con_params.format = params.format;
	_common_set_ocon.theData_out.con_params.number_channels = params.numInputs;

	// Default setting for 
	_common_set_ocon.theData_out.con_params.segmentation.x = _common_set_ocon.theData_out.con_params.buffersize;
	_common_set_ocon.theData_out.con_params.segmentation.y = 1;
	_common_set_ocon.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
	jvx_bitFClear(_common_set_ocon.theData_out.con_params.additional_flags);
}

jvxErrorType
CjvxAudioMasterDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));

	updateChainOutputParameter();
	res = _test_chain_master(fdb);

	return res;
}

jvxErrorType
CjvxAudioMasterDevice::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxBool forward = false;
	jvxAudioParams params;
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor ld_con;
	std::list<std::string> modProps;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this),
		_common_set_io_common.descriptor.c_str(), "Entering CjvxAudioDevice default input connector");

	currentSetupAudioParams(params);

	ld_con.con_params.buffersize = params.buffersize;
	ld_con.con_params.rate = params.samplerate;
	ld_con.con_params.format = params.format;
	ld_con.con_params.number_channels = params.numOutputs;

	jvx_bitFClear(ld_con.con_params.additional_flags);
	ld_con.con_params.segmentation.x = params.buffersize;
	ld_con.con_params.segmentation.y = 1;
	ld_con.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;

	if (
		(_common_set_icon.theData_in->con_params.buffersize != params.buffersize) ||
		(_common_set_icon.theData_in->con_params.rate != params.samplerate) ||
		(_common_set_icon.theData_in->con_params.format != params.format) ||
		(_common_set_icon.theData_in->con_params.number_channels != params.numOutputs) ||

		(_common_set_icon.theData_in->con_params.segmentation.x != params.buffersize) ||
		(_common_set_icon.theData_in->con_params.segmentation.y != 1) ||
		(_common_set_icon.theData_in->con_params.format_group != JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED))
	{
		res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS,
			&ld_con JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}

	switch (res)
	{
	case JVX_NO_ERROR:
		res = _test_connect_icon(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;

	case JVX_ERROR_COMPROMISE:
		if (
			(_common_set_icon.theData_in->con_params.buffersize != params.buffersize) ||
			(_common_set_icon.theData_in->con_params.rate != params.samplerate) ||
			(_common_set_icon.theData_in->con_params.segmentation.x != params.buffersize) ||
			(_common_set_icon.theData_in->con_params.segmentation.y != 1) ||
			(_common_set_icon.theData_in->con_params.format_group != JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED) ||
			(_common_set_icon.theData_in->con_params.format != params.format))
		{
			std::string err = "";
			if (_common_set_icon.theData_in->con_params.buffersize != params.buffersize)
			{
				err = "Buffersize: " + jvx_size2String(_common_set_icon.theData_in->con_params.buffersize) + " vs " +
					jvx_size2String(params.buffersize);
			}
			if (_common_set_icon.theData_in->con_params.rate != params.samplerate)
			{
				err = "Samplerate: " + jvx_size2String(_common_set_icon.theData_in->con_params.rate) + " vs " +
					jvx_size2String(params.samplerate);
			}
			if (_common_set_icon.theData_in->con_params.format != params.format)
			{
				err = "Format: ";
				err += jvxDataFormat_txt(_common_set_icon.theData_in->con_params.format);
				err += " vs ";
				err += jvxDataFormat_txt(params.format);
			}
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, ("Could not find compromise with previous audio node: " + err).c_str(),
				JVX_ERROR_INVALID_SETTING);
			res = JVX_ERROR_INVALID_SETTING;
		}
		else
		{
			if (ld_con.con_params.number_channels != params.numOutputs)
			{
				if (ld_con.con_params.number_channels <= CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size())
				{
					int delta = ld_con.con_params.number_channels - params.numOutputs;

					if (delta != 0)
					{
						modProps.push_back(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.descriptor.c_str());
					}

					if (delta < 0)
					{
						int ii;
						for (ii = (int)CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size() - 1; ii >= 0; ii--)
						{
							if (jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), ii))
							{
								jvx_bitClear(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), ii);
								delta++;
							}
							if (delta == 0)
							{
								break;
							}
						}
					}
					if (delta > 0)
					{
						int ii;
						for (ii = 0; ii < CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size(); ii++)
						{
							if (!jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), ii))
							{
								jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), ii);
								delta--;
							}
							if (delta == 0)
							{
								break;
							}
						}
					}
					assert(delta == 0);
					res = _test_connect_icon(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));



				}
				else
				{
					JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb,
						"Could not find compromise with previous audio node - not suffiecent channels available.",
						JVX_ERROR_INVALID_SETTING);
					res = JVX_ERROR_INVALID_SETTING;
				}
			}
			else
			{
				res = _test_connect_icon(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
		}

		// If we have changed any processing parameter, trigger a report!
		if (!modProps.empty())
		{
			// Report modified parameters to host
			//CjvxReportCommandRequest_id reportModifiedProps(;	
			//_request_command(&reportModifiedProps)
			std::string props_set = jvx::helper::properties::collectedPropsToString(modProps);
			if (_common_set_property_report.reportRef)
			{
				_common_set_property_report.reportRef->report_properties_modified(props_set.c_str());
			}
		}
		break;
	default:
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb,
			"Unable to find a compromise.",
			JVX_ERROR_INVALID_SETTING);
		res = JVX_ERROR_INVALID_SETTING;
		break;
	}
	return res;

}

jvxErrorType
CjvxAudioMasterDevice::transfer_backward_ocon_match_setting(jvxLinkDataTransferType tp, jvxHandle* data, jvxPropertyContainerSingle<jvxSelectionList_cpp>* rateselection,
	jvxPropertyContainerSingle<jvxSelectionList_cpp>* sizeselection JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld_con = NULL;
	jvxBool reportCompromise = false;
	jvxSize i;
	jvxBool runAlgoBestMatch = false;
	jvxAudioParams paramsDevice;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool rateselectionvalid = false;
	jvxBool sizeselectionvalid = false;
	jvxSize idxB = JVX_SIZE_UNSELECTED;
	jvxSize idxR = JVX_SIZE_UNSELECTED;
	jvxSize idxF = JVX_SIZE_UNSELECTED;
	std::list<std::string> modProps;

	// Only in this case!!
	assert(tp == JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS);

	if (rateselection)
	{
		rateselectionvalid = rateselection->isValid;
	}
	if (sizeselection)
	{
		sizeselectionvalid = sizeselection->isValid;
	}

	ld_con = (jvxLinkDataDescriptor*)data;
	currentSetupAudioParams(paramsDevice);

	if ((ld_con->con_params.rate != paramsDevice.samplerate) || (ld_con->con_params.buffersize != paramsDevice.buffersize))
	{
		jvxInt32* srate = NULL;
		jvxInt32* bsize = NULL;
		std::vector<std::string>* entries_sz = nullptr;
		std::vector<std::string>* entries_ra = nullptr;

		if (rateselectionvalid)
		{
			entries_ra = &rateselection->value.entries;
			modProps.push_back(rateselection->descriptor.c_str());
		}
		else
		{
			srate = &paramsDevice.samplerate;
		}
		if (sizeselectionvalid)
		{
			modProps.push_back(sizeselection->descriptor.c_str());
			entries_sz = &sizeselection->value.entries;
		}
		else
		{
			bsize = &paramsDevice.buffersize;
		}
		jvx_find_best_match_bsize_srate(
			ld_con->con_params.buffersize,
			ld_con->con_params.rate,
			entries_sz,
			idxB,
			entries_ra,
			idxR,
			bsize, srate
		);
	}

	if (ld_con->con_params.format != paramsDevice.format)
	{
		if (CjvxAudioDevice_genpcg::properties_active.formatselection.isValid)
		{
			modProps.push_back(CjvxAudioDevice_genpcg::properties_active.formatselection.descriptor.c_str());
			for (i = 0; i < CjvxAudioDevice_genpcg::properties_active.formatselection.value.entries.size(); i++)
			{
				jvxBool err = false;
				jvxDataFormat form = JVX_DATAFORMAT_NONE;
				form = jvxDataFormat_decode(CjvxAudioDevice_genpcg::properties_active.formatselection.value.entries[i].c_str());
				assert(!err);

				if (form == ld_con->con_params.format)
				{
					jvx_bitZSet(CjvxAudioDevice_genpcg::properties_active.formatselection.value.selection(), i);
					paramsDevice.format = form;
					idxF = i;
					break;
				}
			}
		}
		else
		{
			paramsDevice.format = ld_con->con_params.format;
		}
	}

	if (res == JVX_NO_ERROR)
	{
		CjvxAudioDevice_genpcg::properties_active.samplerate.value = paramsDevice.samplerate;
		CjvxAudioDevice_genpcg::properties_active.buffersize.value = paramsDevice.buffersize;
		CjvxAudioDevice_genpcg::properties_active.format.value = paramsDevice.format;

		modProps.push_back(CjvxAudioDevice_genpcg::properties_active.samplerate.descriptor.c_str());
		modProps.push_back(CjvxAudioDevice_genpcg::properties_active.buffersize.descriptor.c_str());
		modProps.push_back(CjvxAudioDevice_genpcg::properties_active.format.descriptor.c_str());

		if (JVX_CHECK_SIZE_SELECTED(idxF))
		{
			jvxBool err = false;
			jvx_bitZSet(CjvxAudioDevice_genpcg::properties_active.formatselection.value.selection(), idxF);
			CjvxAudioDevice_genpcg::properties_active.format.value = jvxDataFormat_decode(CjvxAudioDevice_genpcg::properties_active.formatselection.value.entries[idxF].c_str());
		}
		if (JVX_CHECK_SIZE_SELECTED(idxR))
		{
			jvxBool err = false;
			jvx_bitZSet(rateselection->value.selection(), idxR);
			CjvxAudioDevice_genpcg::properties_active.samplerate.value = JVX_SIZE_INT32(jvx_string2Size(rateselection->value.entries[idxR], err));
		}
		if (JVX_CHECK_SIZE_SELECTED(idxB))
		{
			jvxBool err = false;
			jvx_bitZSet(sizeselection->value.selection(), idxB);
			CjvxAudioDevice_genpcg::properties_active.buffersize.value = JVX_SIZE_INT32(jvx_string2Size(sizeselection->value.entries[idxB], err));
		}

		_common_set_ocon.theData_out.con_params.buffersize = CjvxAudioDevice_genpcg::properties_active.buffersize.value;
		_common_set_ocon.theData_out.con_params.rate = CjvxAudioDevice_genpcg::properties_active.samplerate.value;
		_common_set_ocon.theData_out.con_params.format = (jvxDataFormat)CjvxAudioDevice_genpcg::properties_active.format.value;

	}
	else
	{
		modProps.clear();
	}

	if (ld_con->con_params.number_channels <= paramsDevice.maxChanCntInput)
	{
		// Here I just corrected the channel count today (24.9.2022)
		// The idea is: ld_con->con_params.number_channels is the DESIRED number of channels from 
		// the following input connector, paramsDevice.numInputs is the current number of input channels
		// to be forwarded. Then, delta is the required modification to reach the DESIRED number of 
		// channels, hence, a pos number means "more channels".
		// Somehow, this was just the other way around before my modification.
		jvxInt32 delta = ld_con->con_params.number_channels - paramsDevice.numInputs;
		if (delta != 0)
		{
			modProps.push_back(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.descriptor.c_str());
		}

		jvxSize idx = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size() - 1;
		std::vector<std::string>::reverse_iterator elmbwd = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.rbegin();
		while (delta < 0 && (elmbwd != CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.rend()))
		{
			if (idx != last_user_interaction.last_selection_user_input_channels)
			{
				if (jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx))
				{
					jvx_bitClear(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx);
					delta++;
				}
			}
			elmbwd++;
			idx--;
		}

		idx = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size() - 1;
		elmbwd = elmbwd = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.rbegin();
		while (delta < 0 && (elmbwd != CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.rend()))
		{
			if (jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx))
			{
				jvx_bitClear(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx);
				delta++;
			}
			elmbwd++;
		}

		// Add channels in forward manner!!
		idx = 0;
		std::vector<std::string>::iterator elmfwd = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.begin();
		while (delta > 0 && (elmfwd != CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.end()))
		{
			if (idx != last_user_interaction.last_selection_user_input_channels)
			{
				if (!jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx))
				{
					jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx);
					delta--;
				}
			}
			elmfwd++;
			idx++;
		}

		idx = 0;
		elmfwd = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.begin();
		while (delta > 0 && (elmfwd != CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.end()))
		{
			if (!jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx))
			{
				jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx);
				delta--;
			}
			elmfwd++;
		}
	}
	else
	{
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb,
			"Could not find compromise with previous audio node - sufficient number of channels is not available.",
			JVX_ERROR_INVALID_SETTING);
		res = JVX_ERROR_INVALID_SETTING;
	}

	// If we have changed any processing parameter, trigger a report!
	if (!modProps.empty())
	{
		// Report modified parameters to host
		//CjvxReportCommandRequest_id reportModifiedProps(;	
		//_request_command(&reportModifiedProps)
		std::string props_set = jvx::helper::properties::collectedPropsToString(modProps);
		if (_common_set_property_report.reportRef)
		{
			_common_set_property_report.reportRef->report_properties_modified(props_set.c_str());
		}
	}
	return res;
}

jvxErrorType
CjvxAudioMasterDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor* tryThis = reinterpret_cast<jvxLinkDataDescriptor*>(data);
	jvxComponentIdentification* cpId = reinterpret_cast<jvxComponentIdentification*>(data);

	jvx::propertyCallCompactRefList* propCallCompact = nullptr;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES:
		propCallCompact = (jvx::propertyCallCompactRefList*)data;
		if (propCallCompact)
		{
			jvxBool forwardChain = false;
			for (auto& elm : propCallCompact->propReqs)
			{
				if (elm.resCall == JVX_ERROR_ELEMENT_NOT_FOUND)
				{
					elm.resCall = this->get_property((elm.callMan), elm.rawPtr,
						elm.ident, elm.detail);
				}
			}
			return JVX_NO_ERROR;
		}
		break;
	case JVX_LINKDATA_TRANSFER_REQUEST_REAL_MASTER:
		if (cpId)
		{
			*cpId = _common_set.theComponentType;
		}
		return JVX_NO_ERROR;
		break;
	}
	return CjvxSimpleMasterDevice::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAudioMasterDevice::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx::propertyCallCompactRefList* propCallCompact = nullptr;
	jvxComponentIdentification* cpId = reinterpret_cast<jvxComponentIdentification*>(data);

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES:
		propCallCompact = (jvx::propertyCallCompactRefList*)data;
		if (propCallCompact)
		{
			jvxBool forwardChain = false;
			for (auto& elm : propCallCompact->propReqs)
			{
				if (elm.resCall == JVX_ERROR_ELEMENT_NOT_FOUND)
				{
					elm.resCall = this->get_property((elm.callMan), elm.rawPtr,
						elm.ident, elm.detail);
				}
			}
			return JVX_NO_ERROR;
		}
		break;
	case JVX_LINKDATA_TRANSFER_REQUEST_REAL_MASTER:
		if (cpId)
		{
			*cpId = _common_set.theComponentType;
		}
		return JVX_NO_ERROR;
		break;
	}
	return CjvxSimpleMasterDevice::transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

void
CjvxAudioMasterDevice::activate_connectors_master()
{
	JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), "default",
		static_cast<IjvxConnectionMaster*>(this), _common_set.theModuleName.c_str());
	JVX_ACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER(static_cast<CjvxObject*>(this), "default");
}

void
CjvxAudioMasterDevice::deactivate_connectors_master()
{
	JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), "default",
		static_cast<IjvxConnectionMaster*>(this), _common_set.theModuleName.c_str());
	JVX_ACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER(static_cast<CjvxObject*>(this), "default");
}

/*
jvxErrorType
CjvxAudioMasterDevice::adaptNumberInputChannels(jvxSize numChannels)
{
	jvxBool modified = false;
	jvxSize i;
	if (numChannels  < CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size())
{
	jvxSize selected = jvx_bitCountFilled(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(0));
	int deltaChannels = ((int)tryThis->con_params.number_channels - (int)selected);
	if (deltaChannels > 0)
	{
		for (i = 0; i < CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size(); i++)
		{
			if (!jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(0), i))
			{
				jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(0), i);
				modified = true;
				deltaChannels--;
			}
			if (deltaChannels == 0)
			{
				break;
			}
		}
	}

	return JVX_NO_ERROR;
}
return JVX_ERROR_NOT_READY;
}
*/

#if 0
jvxErrorType
CjvxAudioMasterDevice::activate_lock()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	res = _activate_lock();
	if (res == JVX_NO_ERROR)
	{
		deactivate_connectors_master();

		init__properties_active();
		allocate__properties_active();
		register__properties_active(static_cast<CjvxProperties*>(this));

		CjvxAudioDevice_genpcg::properties_active.sourceName.value = _common_set.theName;
		CjvxAudioDevice_genpcg::properties_active.sinkName.value = _common_set.theName;

		//_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, JVX_PROPERTY_CATEGORY_PREDEFINED, CjvxAudioMasterDevice::properties_active.format.globalIdx);

		for (i = 0; i < _common_set_audio_device.formats.size(); i++)
		{
			CjvxAudioMasterDevice::properties_active.formatselection.value.entries.push_back(jvxDataFormat_txt(_common_set_audio_device.formats[i]));
		}
		if (_common_set_audio_device.formats.size() > 0)
		{
			CjvxAudioMasterDevice::properties_active.format.value = (jvxInt16)_common_set_audio_device.formats[0];
			CjvxAudioMasterDevice::properties_active.formatselection.value.selection() = ((jvxBitField)1 << 0);
			CjvxAudioMasterDevice::properties_active.formatselection.value.exclusive = (jvxBitField)-1;
		}
		else
		{
			CjvxAudioMasterDevice::properties_active.format.value = (jvxInt16)JVX_DATAFORMAT_NONE;
		}
		this->updateDependentVariables_lock(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);
	}
	return(res);
}
#endif


#if 0
jvxErrorType
CjvxAudioMasterDevice::deactivate_lock()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _deactivate_lock();
	if (res == JVX_NO_ERROR)
	{

		unregister__properties_active(static_cast<CjvxProperties*>(this));
		//deassociate__properties_active(static_cast<CjvxProperties*>(this));

		deallocate__properties_active();

		CjvxAudioMasterDevice::properties_active.inputchannelselection.value.entries.clear();
		CjvxAudioMasterDevice::properties_active.inputchannelselection.value.selection() = 0;
		CjvxAudioMasterDevice::properties_active.inputchannelselection.value.exclusive = 0;

		CjvxAudioMasterDevice::properties_active.outputchannelselection.value.entries.clear();
		CjvxAudioMasterDevice::properties_active.outputchannelselection.value.selection() = 0;
		CjvxAudioMasterDevice::properties_active.outputchannelselection.value.exclusive = 0;

		CjvxAudioMasterDevice::properties_active.buffersize.value = 0;
		CjvxAudioMasterDevice::properties_active.format.value = (jvxInt16)JVX_DATAFORMAT_NONE;

		CjvxAudioMasterDevice::properties_active.formatselection.value.entries.clear();
		CjvxAudioMasterDevice::properties_active.formatselection.value.selection() = 0;
		CjvxAudioMasterDevice::properties_active.formatselection.value.exclusive = 0;
		CjvxAudioMasterDevice::properties_active.format.value = (jvxInt16)JVX_DATAFORMAT_NONE;

		CjvxAudioMasterDevice::properties_active.numberinputchannels.value = 0;
		CjvxAudioMasterDevice::properties_active.numberoutputchannels.value = 0;

		CjvxAudioMasterDevice::properties_active.samplerate.value = 0;

		JVX_DEACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER();
		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
}
#endif
