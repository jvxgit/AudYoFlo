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
CjvxAudioMasterDevice::activate()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	res = _activate();
	if (res == JVX_NO_ERROR)
	{
		inout_params.initialize(static_cast<CjvxProperties*>(this));

		// No direct modification of the channel number is allowed
		CjvxProperties::_update_property_access_type(
			jvxPropertyAccessType::JVX_PROPERTY_ACCESS_READ_ONLY, 
			inout_params.node.numberchannels);
		CjvxProperties::_update_property_access_type(
			jvxPropertyAccessType::JVX_PROPERTY_ACCESS_READ_ONLY, 
			inout_params.node_plus.numberchannels_out);

		last_user_interaction.last_selection_user_input_channels = JVX_SIZE_UNSELECTED;
		last_user_interaction.last_selection_user_output_channels = JVX_SIZE_UNSELECTED;

		activate_connectors_master();

		init__properties_active();
		allocate__properties_active();

		CjvxAudioMasterDevice_genpcg::properties_active.sourceName.value = _common_set.theName;
		CjvxAudioMasterDevice_genpcg::properties_active.sinkName.value = _common_set.theName;

		register__properties_active(static_cast<CjvxProperties*>(this));
		CjvxAudioMasterDevice_genpcg::register_callbacks(static_cast<CjvxProperties*>(this), base_params_set_update, this);

		assert(_common_set_audio_device.formats.size());
		for (i = 0; i < _common_set_audio_device.formats.size(); i++)
		{
			CjvxAudioMasterDevice::properties_active.formatselection.value.entries.push_back(jvxDataFormat_txt(_common_set_audio_device.formats[i]));
		}
		jvx_bitZSet(CjvxAudioMasterDevice::properties_active.formatselection.value.selection(), 0); // <- select first option by default
		this->updateDependentVariables(true); // <- do not cause a chain update as we are in call active - which is followed by an update anyway
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

		CjvxAudioMasterDevice::properties_active.formatselection.value.entries.clear();
		CjvxAudioMasterDevice::properties_active.formatselection.value.selection() = 0;
		CjvxAudioMasterDevice::properties_active.formatselection.value.exclusive = 0;

		JVX_DEACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER();
		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();

		inout_params.terminate();
		inout_params.reset();
	}
	return(res);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioMasterDevice, base_params_set_update)
{
	// Do not report update of chain in case of INTERNAL_PASS
	updateDependentVariables(callGate.call_purpose == JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS);
	return JVX_NO_ERROR;
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
		jvxBitField selFormatOld = CjvxAudioMasterDevice_genpcg::properties_active.formatselection.value.selection();
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
		if (JVX_EVALUATE_BITFIELD(CjvxAudioMasterDevice_genpcg::properties_active.formatselection.value.selection()) == false)
		{
			// If there is a wrong format in the config file it might be good to retun to the old value
			CjvxAudioMasterDevice_genpcg::properties_active.formatselection.value.selection() = selFormatOld;
		}

		// Update all dependant props
		this->updateDependentVariables(true);  // <- do not cause a chain update as we are in call put_configuration - which is followed by an update anyway

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
CjvxAudioMasterDevice::updateDependentVariables(jvxBool suppress_update_chain)
{
	jvxSize i;
	jvxInt32 newValue = 0;
	jvxBool updateChain = false;

	CjvxSimplePropsPars::_common_set_node_params_a_1io_t params_simple_on_enter = inout_params._common_set_node_params_a_1io;
	CjvxSimplePropsParsPlusOutChannel::_common_set_device_params_t params_simple_channel_on_enter = inout_params._common_set_device_params;

	// The property <CjvxAudioMasterDevice::properties_active.inputchannelselection> describes the input side, that is,
	// the microphones.  The derived property <_common_set_node_params_a_1io.number_channels > is a shortcut to this number. 

	// inout_params
	newValue = 0;
	for (i = 0; i < (int)CjvxAudioMasterDevice::properties_active.inputchannelselection.value.entries.size(); i++)
	{
		{
			if (jvx_bitTest(CjvxAudioMasterDevice::properties_active.inputchannelselection.value.selection(), i))
			{
				newValue++;
			}
		}
	}
	inout_params._common_set_node_params_a_1io.number_channels = newValue;
		
	// The property <CjvxAudioMasterDevice::properties_active.outputchannelselection> describes the output side, that is,
	// the microphones.  The derived property <inout_params._common_set_device_params.number_channels_out> is a shortcut to this number. 

	newValue = 0;
	for (i = 0; i < (int)CjvxAudioMasterDevice::properties_active.outputchannelselection.value.entries.size(); i++)
	{
		if (jvx_bitTest(CjvxAudioMasterDevice::properties_active.outputchannelselection.value.selection(), i))
		{
			newValue++;
		}
	}
	inout_params._common_set_device_params.number_channels_out = newValue;

	// =================================================================================================
	
	jvxSize idSel = jvx_bitfieldSelection2Id(CjvxAudioMasterDevice::properties_active.formatselection);
	if (JVX_CHECK_SIZE_SELECTED(idSel))
	{
		assert(idSel < _common_set_audio_device.formats.size());
		inout_params._common_set_node_params_a_1io.format = _common_set_audio_device.formats[idSel];
	}
	
	// =================================================================================================
	// =================================================================================================
	
	updateChain = inout_params.check_difference(&params_simple_on_enter, &params_simple_channel_on_enter);
	
	// ============================================================================================
	// ============================================================================================

	if (!suppress_update_chain && updateChain)
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

jvxErrorType
CjvxAudioMasterDevice::prepare()
{
	jvxErrorType res = _prepare();
	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_update_properties_on_start();

		// currentSetupAudioParams(_inproc);
	}
	return(res);
}

jvxErrorType
CjvxAudioMasterDevice::postprocess()
{
	jvxErrorType res = _postprocess();
	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_update_properties_on_stop();

	}
	return(res);
}

jvxErrorType
CjvxAudioMasterDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;	

	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));

	// Prepare the component-to-component data parameters
	
	// What is the number of (physical) output channels of the device is the number of input channels
	// on the software side from and towards the connected components
	inout_params.prepare_negotiate(neg_input);// <- copies number_channels_out
		
	CjvxSimplePropsPars* in_params = static_cast<CjvxSimplePropsPars*>(&inout_params); // Copies number_of_channels
	in_params->prepare_negotiate(neg_output);

	// Allow all numbers of channels!
	neg_output.preferred.number_channels.min = 0;
	neg_output.preferred.number_channels.max = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.size();

	neg_input.preferred.number_channels.min = 0;
	neg_input.preferred.number_channels.max = CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.entries.size();
	
	// Start with the current number of channels
	this->_common_set_ocon.theData_out.con_params.number_channels = inout_params._common_set_node_params_a_1io.number_channels;

	// Constrain the parameters
	neg_output._constrain_ldesc(&this->_common_set_ocon.theData_out);

	// Leave towards chain
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
		_common_set_io_common.descriptor.c_str(), "Entering CjvxAudioMasterDevice default input connector");

	res = neg_input._negotiate_connect_icon(_common_set_icon.theData_in,
		static_cast<IjvxObject*>(this), _common_set.theDescriptor.c_str()
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if(res == JVX_NO_ERROR)
	{
		// If the number of channels deviates from software to hardware - dont mid: we will activate the required number of channels
		jvxSize numMaxChans = CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.entries.size();
		if (_common_set_icon.theData_in->con_params.number_channels != inout_params._common_set_device_params.number_channels_out)
		{
			if (_common_set_icon.theData_in->con_params.number_channels <= numMaxChans)
			{
				int delta = _common_set_icon.theData_in->con_params.number_channels - inout_params._common_set_device_params.number_channels_out;

				if (delta != 0)
				{
					modProps.push_back(CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.descriptor.c_str());
					modProps.push_back(this->inout_params.node_plus.numberchannels_out.descriptor.c_str());
				}

				if (delta < 0)
				{
					int ii;
					for (ii = (int)CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.entries.size() - 1; ii >= 0; ii--)
					{
						if (jvx_bitTest(CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.selection(), ii))
						{
							jvx_bitClear(CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.selection(), ii);
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
					for (ii = 0; ii < CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.entries.size(); ii++)
					{
						if (!jvx_bitTest(CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.selection(), ii))
						{
							jvx_bitSet(CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.selection(), ii);
							delta--;
						}
						if (delta == 0)
						{
							break;
						}
					}
				}
				
				// Take over the new number of active output channels
				inout_params._common_set_device_params.number_channels_out = _common_set_icon.theData_in->con_params.number_channels;
			}
		}
		if (!modProps.empty())
		{
			// Report modified parameters to host
			// CjvxReportCommandRequest_id reportModifiedProps(;
			//_request_command(&reportModifiedProps)
			std::string props_set = jvx::helper::properties::collectedPropsToString(modProps);
			if (_common_set_property_report.reportRef)
			{
				_common_set_property_report.reportRef->report_properties_modified(props_set.c_str());
			}
		}
	}	
	return res;
}

jvxErrorType
CjvxAudioMasterDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor* tryThis = reinterpret_cast<jvxLinkDataDescriptor*>(data);
	jvxComponentIdentification* cpId = reinterpret_cast<jvxComponentIdentification*>(data);

	jvx::propertyCallCompactRefList* propCallCompact = nullptr;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		// This maps the call to a function that may be overloaded
		return try_match_settings_backward_ocon(tryThis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;

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

jvxErrorType
CjvxAudioMasterDevice::try_match_settings_backward_ocon(jvxLinkDataDescriptor* ld_con JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<std::string> modProps;
	jvxCBitField whatWasModified = 0;

	// Here, we may repair the following cases:
	// 1) Number of channels is not correct
	// 2) Dataformat is incorrect

	res = neg_output._negotiate_transfer_backward_ocon(ld_con,
		&_common_set_ocon.theData_out,
		static_cast<IjvxObject*>(this), &whatWasModified
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if(res == JVX_NO_ERROR)
	{
		// Case 1: Channel number may have been modified in allowed limits
		if(ld_con->con_params.number_channels != inout_params._common_set_node_params_a_1io.number_channels)
		{
			jvxSize numChansMax = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.size();
			if (ld_con->con_params.number_channels <= numChansMax)
			{
				// The idea is: ld_con->con_params.number_channels is the DESIRED number of channels from
				// the following input connector, paramsDevice.numInputs is the current number of input channels
				// to be forwarded. Then, delta is the required modification to reach the DESIRED number of
				// channels, hence, a pos number means "more channels".
				// Somehow, this was just the other way around before my modification.
				jvxInt32 delta = ld_con->con_params.number_channels - inout_params._common_set_node_params_a_1io.number_channels;
				if (delta != 0)
				{
					// Two properties will be changed!!
					modProps.push_back(CjvxAudioMasterDevice::properties_active.inputchannelselection.descriptor.c_str());
					modProps.push_back(inout_params.node.numberchannels.descriptor.c_str());
				}

				jvxSize idx = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.size() - 1;
				std::vector<std::string>::reverse_iterator elmbwd = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.rbegin();
				while (delta < 0 && (elmbwd != CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.rend()))
				{
					if (idx != last_user_interaction.last_selection_user_input_channels)
					{
						if (jvx_bitTest(CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx))
						{
							jvx_bitClear(CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx);
							delta++;
						}
					}
					elmbwd++;
					idx--;
				}

				idx = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.size() - 1;
				elmbwd = elmbwd = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.rbegin();
				while (delta < 0 && (elmbwd != CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.rend()))
				{
					if (jvx_bitTest(CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx))
					{
						jvx_bitClear(CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx);
						delta++;
					}
					elmbwd++;
				}

				// Add channels in forward manner!!
				idx = 0;
				std::vector<std::string>::iterator elmfwd = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.begin();
				while (delta > 0 && (elmfwd != CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.end()))
				{
					if (idx != last_user_interaction.last_selection_user_input_channels)
					{
						if (!jvx_bitTest(CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx))
						{
							jvx_bitSet(CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx);
							delta--;
						}
					}
					elmfwd++;
					idx++;
				}

				idx = 0;
				elmfwd = CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.begin();
				while (delta > 0 && (elmfwd != CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.end()))
				{
					if (!jvx_bitTest(CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx))
					{
						jvx_bitSet(CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx);
						delta--;
					}
					elmfwd++;
				}

				// Update the
				inout_params._common_set_node_params_a_1io.number_channels = ld_con->con_params.number_channels;
				_common_set_ocon.theData_out.con_params.number_channels = inout_params._common_set_node_params_a_1io.number_channels;
			}
			else
			{
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb,
															  "Could not find compromise with previous audio node - sufficient number of channels is not available.",
															  JVX_ERROR_INVALID_SETTING);
				res = JVX_ERROR_INVALID_SETTING;
			}
		}
	}
	else
	{
		if (ld_con->con_params.format != _common_set_ocon.theData_out.con_params.format)
		{			
			jvxSize idSel = 0;
			for (auto elm : _common_set_audio_device.formats)
			{
				if (elm == ld_con->con_params.format)
				{					
					break;
				}
				idSel++;
			}
			if (idSel < CjvxAudioMasterDevice::properties_active.formatselection.value.entries.size())
			{
				jvx_bitZSet(CjvxAudioMasterDevice::properties_active.formatselection.value.selection(), idSel);
				modProps.push_back(CjvxAudioMasterDevice::properties_active.formatselection.descriptor.c_str());
				
				neg_input._update_parameter_fixed(
					jvxAddressLinkDataEntry::JVX_ADDRESS_FORMAT_SHIFT, 
					(jvxSize) ld_con->con_params.format,
					&_common_set_ocon.theData_out);

				res = JVX_NO_ERROR;
			}
			else
			{
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb,
															  "Could not find compromise with previous audio node - requested format is not supported.",
															  JVX_ERROR_INVALID_SETTING);
				res = JVX_ERROR_INVALID_SETTING;
			}
		}
	}

	// If we have changed any processing parameter, trigger a report!
	if (!modProps.empty())
	{
		// Report modified parameters to host
		// CjvxReportCommandRequest_id reportModifiedProps(;
		//_request_command(&reportModifiedProps)
		std::string props_set = jvx::helper::properties::collectedPropsToString(modProps);
		if (_common_set_property_report.reportRef)
		{
			_common_set_property_report.reportRef->report_properties_modified(props_set.c_str());
		}
	}
	return res;
}
