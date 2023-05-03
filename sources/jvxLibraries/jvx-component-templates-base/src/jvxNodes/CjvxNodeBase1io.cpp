#include "jvxNodes/CjvxNodeBase1io.h"

#define JVX_MIN_NEG_SET_LDAT(neg, ldat) \
	if (JVX_CHECK_SIZE_SELECTED(neg.min)) \
	{ \
		ldat = JVX_MAX(neg.min, ldat); \
	}

#define JVX_MAX_NEG_SET_LDAT(neg, ldat) \
	if (JVX_CHECK_SIZE_SELECTED(neg.max)) \
	{ \
		ldat = JVX_MIN(neg.max, ldat); \
	}

#define JVX_MIN_NEG_SET_LDAT_CMP(neg, ldat, cmp) \
	if (neg.min != cmp) \
	{ \
		ldat = JVX_MAX(neg.min, ldat); \
	}

#define JVX_MAX_NEG_SET_LDAT_CMP(neg, ldat, cmp) \
	if (neg.max != cmp) \
	{ \
		ldat = JVX_MIN(neg.max, ldat); \
	}

CjvxNodeBase1io::CjvxNodeBase1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) : 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);
	forward_complain = true;
	_common_set_ldslave.zeroCopyBuffering_cfg = true;
	// jvx_test_settings::init_connect_icon_flex(&_common_set_node_params_1io);
	
	neg_input.logObj = this;
}; 

jvxErrorType
CjvxNodeBase1io::activate()
{
	jvxErrorType res = _activate();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		node_inout.initialize(this);
		node_inout.updtag_all("InOut");
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this),
			"default", NULL, _common_set.theModuleName.c_str());
	}
	return(res);
};

jvxErrorType
CjvxNodeBase1io::deactivate()
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = _deactivate();
	if (res == JVX_NO_ERROR)
	{
		node_inout.terminate();
		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
};

jvxErrorType
CjvxNodeBase1io::reportRequestTestChainMaster()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize processId = JVX_SIZE_UNSELECTED;
	IjvxDataConnectionCommon* dataConn = NULL;

	// Inform the secondary chain to be updated
	this->associated_common_icon(&dataConn);

	if (dataConn)
	{
		dataConn->unique_id_connections(&processId);
	}

	if (JVX_CHECK_SIZE_SELECTED(processId))
	{
		res = this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT, (jvxHandle*)(intptr_t)processId);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_SETTING;
}

jvxErrorType
CjvxNodeBase1io::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	std::string txt;
	jvxErrorType res = _is_ready(suc, reasonIfNot);
	jvxBool Iamready = true;
	jvxBool isAProblem = false;

	if (res == JVX_NO_ERROR)
	{

	}

	return(res);
};

jvxErrorType
CjvxNodeBase1io::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		node_inout.put_configuration_all(callConf, processor,
			sectionToContainAllSubsectionsForMe, &warns);
		for (i = 0; i < warns.size(); i++)
		{
			_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
		if (CjvxNode_genpcg::audiologtofile.properties_logtofile.activateLogToFile.value)
		{
			theLogger.activate_logging(CjvxNode_genpcg::audiologtofile.properties_logtofile.logFileFolder.value, CjvxNode_genpcg::audiologtofile.properties_logtofile.logFilePrefix.value);
		}
#endif
	}
	return JVX_NO_ERROR;
}
jvxErrorType
CjvxNodeBase1io::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	node_inout.get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxNodeBase1io::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Forward the call to next module
		res = _test_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
		res = this->accept_input_parameters_stop(res);
	}
	return res;
}

jvxErrorType
CjvxNodeBase1io::accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = neg_input._negotiate_connect_icon(_common_set_icon.theData_in,
		static_cast<IjvxObject*>(this),
		_common_set_io_common.descriptor.c_str()
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{		
		// Copy the parameters from the negotiation result to the internal parameter (and properties)
		update_simple_params_from_neg_on_test();

		// Function to derive output parameters from input arguments
		from_input_to_output();
	}
	return res;
}

jvxErrorType
CjvxNodeBase1io::accept_input_parameters_stop(jvxErrorType resTest)
{
	if (resTest == JVX_NO_ERROR)
	{
		update_simple_params_from_ldesc();
	}
	else
	{
		// We might try to negotiate to the successor!
	}
	return neg_input._pop_constraints();
}

void
CjvxNodeBase1io::from_input_to_output()
{
	// Since we have a zero copy, outputand input number of channels must be identical
	// CjvxSimplePropsPars::node.numberoutputchannels = CjvxSimplePropsPars::node.numberinputchannels;
	neg_input._push_constraints();

	// Update also the data processing parameters to hold the right value
	neg_input._update_parameters_fixed(
		node_inout._common_set_node_params_a_1io.number_channels,
		node_inout._common_set_node_params_a_1io.buffersize,
		node_inout._common_set_node_params_a_1io.samplerate,
		(jvxDataFormat)node_inout._common_set_node_params_a_1io.format,
		(jvxDataFormatGroup)node_inout._common_set_node_params_a_1io.subformat,
		(jvxDataflow)node_inout._common_set_node_params_a_1io.data_flow,
		&_common_set_ocon.theData_out);
}

jvxErrorType
CjvxNodeBase1io::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld = (jvxLinkDataDescriptor*)data;
	jvxBool thereismismatch = false;
	jvxErrorType res = JVX_NO_ERROR;
	std::string txt;

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		res = accept_negotiate_output(tp, ld JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		if (res == JVX_NO_ERROR)
		{
			// There is only room to negotiate 
			res = neg_input._negotiate_transfer_backward_ocon(ld,
				&_common_set_ocon.theData_out, static_cast<IjvxObject*>(this), NULL
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		break;
	default:
		res = _transfer_backward_ocon(true, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType
CjvxNodeBase1io::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (forward_complain)
	{
		jvxLinkDataDescriptor ld_loc = *preferredByOutput;
		res = transfer_backward_icon(tp, &ld_loc  JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (
			(res == JVX_NO_ERROR) ||
			(res == JVX_ERROR_COMPROMISE))
		{			
			// This function may also return an error - if the stack is in a "wrong" state. We may reach
			// this wrong state if we try to change the parameters from the "wrong end" - after parameters were
			// accepted in a forward test run but are requested to be changed on the backward complain. The 
			// problem occurs in the case of a fixed connection test. 
			jvxErrorType resL = accept_input_parameters_stop(res);
			
			// If this happens, however, at the moment I would propose to
			// accept this condition - just do not pop the stack as it is empty.
			/*
			if (resL != JVX_NO_ERROR)
			{
				return resL;
			}
			*/

			// Here is the place where we can reject the backwrd settings if it does not fit!!
			accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}

	}
	return res;
}

void
CjvxNodeBase1io::test_set_output_parameters()
{
	// Update the output link descriptor settings from input parameters
	update_ldesc_from_input_params_on_test();

	// Apply output negotiation constraints
	constrain_ldesc_from_neg_params(neg_input);
}

jvxErrorType
CjvxNodeBase1io::inform_chain_test(jvxBool oldMethod)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize processId = JVX_SIZE_UNSELECTED;
	IjvxDataConnectionCommon* dataConn = NULL;

	// Inform the secondary chain to be updated
	associated_common_icon(&dataConn);
	if (dataConn)
	{
		dataConn->unique_id_connections(&processId);
	}
	if (JVX_CHECK_SIZE_SELECTED(processId))
	{
		if (oldMethod)
		{
			res = this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT, (jvxHandle*)(intptr_t)processId);
		}
		else
		{
			res = this->_request_command(CjvxReportCommandRequest_uid(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN,
				_common_set.theComponentType, processId));
		}
	}
	return res;
}

void
CjvxNodeBase1io::update_simple_params_from_neg_on_test()
{
	newParamsOnTestInput = false;
	if (
		(node_inout._common_set_node_params_a_1io.number_channels != JVX_SIZE_INT32(neg_input._latest_results.number_channels)) ||
		(node_inout._common_set_node_params_a_1io.buffersize != JVX_SIZE_INT32(neg_input._latest_results.buffersize)) ||
		(node_inout._common_set_node_params_a_1io.samplerate != JVX_SIZE_INT32(neg_input._latest_results.rate)) ||
		(node_inout._common_set_node_params_a_1io.format != JVX_SIZE_INT32(neg_input._latest_results.format)) ||
		(node_inout._common_set_node_params_a_1io.subformat != JVX_SIZE_INT32(neg_input._latest_results.format_group)) ||
		(node_inout._common_set_node_params_a_1io.segmentation.x != JVX_SIZE_INT32(neg_input._latest_results.segmentation.x)) ||
		(node_inout._common_set_node_params_a_1io.segmentation.y != JVX_SIZE_INT32(neg_input._latest_results.segmentation.y)) ||
		(node_inout._common_set_node_params_a_1io.data_flow != JVX_SIZE_INT32(neg_input._latest_results.data_flow)))
	{
		newParamsOnTestInput = true;

		// Copy the values
		node_inout._common_set_node_params_a_1io.number_channels = JVX_SIZE_INT32(neg_input._latest_results.number_channels);
		node_inout._common_set_node_params_a_1io.buffersize = JVX_SIZE_INT32(neg_input._latest_results.buffersize);
		node_inout._common_set_node_params_a_1io.samplerate = JVX_SIZE_INT32(neg_input._latest_results.rate);
		node_inout._common_set_node_params_a_1io.format = JVX_SIZE_INT32(neg_input._latest_results.format);
		node_inout._common_set_node_params_a_1io.subformat = JVX_SIZE_INT32(neg_input._latest_results.format_group);
		node_inout._common_set_node_params_a_1io.segmentation.x = JVX_SIZE_INT32(neg_input._latest_results.segmentation.x);
		node_inout._common_set_node_params_a_1io.segmentation.y = JVX_SIZE_INT32(neg_input._latest_results.segmentation.y);
		node_inout._common_set_node_params_a_1io.data_flow = JVX_SIZE_INT32(neg_input._latest_results.data_flow);
	}
}

void
CjvxNodeBase1io::update_simple_params_from_ldesc()
{
	node_inout._common_set_node_params_a_1io.buffersize = _common_set_icon.theData_in->con_params.buffersize;
	node_inout._common_set_node_params_a_1io.samplerate = _common_set_icon.theData_in->con_params.rate;
	node_inout._common_set_node_params_a_1io.number_channels = _common_set_icon.theData_in->con_params.number_channels;
	node_inout._common_set_node_params_a_1io.format = _common_set_icon.theData_in->con_params.format;
	node_inout._common_set_node_params_a_1io.subformat = _common_set_icon.theData_in->con_params.format_group;
	node_inout._common_set_node_params_a_1io.segmentation.x = _common_set_icon.theData_in->con_params.segmentation.x;
	node_inout._common_set_node_params_a_1io.segmentation.y = _common_set_icon.theData_in->con_params.segmentation.y;
	node_inout._common_set_node_params_a_1io.data_flow = _common_set_icon.theData_in->con_params.data_flow;
	node_inout._common_set_node_params_a_1io.format_spec = _common_set_icon.theData_in->con_params.format_spec.std_str();
}

jvxCBitField
CjvxNodeBase1io::requires_reconfig(jvxLinkDataDescriptor* theData, jvxCBitField cmpWhat)
{
	jvxCBitField what = 0;
	if (jvx_bitTest(cmpWhat, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_BUFFERSIZE_SHIFT))
	{
		if (node_inout._common_set_node_params_a_1io.buffersize != theData->con_params.buffersize)
		{
			jvx_bitSet(what, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_BUFFERSIZE_SHIFT);
		}
	}
	
	if (jvx_bitTest(cmpWhat, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SAMPLERATE_SHIFT))
	{
		if (node_inout._common_set_node_params_a_1io.samplerate != theData->con_params.rate)
		{
			jvx_bitSet(what, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SAMPLERATE_SHIFT);
		}
	}	
	
	if (jvx_bitTest(cmpWhat, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_NUM_CHANNELS_SHIFT))
	{
		if (node_inout._common_set_node_params_a_1io.number_channels != theData->con_params.number_channels)
		{
			jvx_bitSet(what, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_NUM_CHANNELS_SHIFT);
		}
	}
		
	if (jvx_bitTest(cmpWhat, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMAT_SHIFT))
	{
		if (node_inout._common_set_node_params_a_1io.format != theData->con_params.format)
		{
			jvx_bitSet(what, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMAT_SHIFT);
		}
	}

	if (jvx_bitTest(cmpWhat, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SUBFORMAT_SHIFT))
	{
		if (node_inout._common_set_node_params_a_1io.subformat != theData->con_params.format_group)
		{
			jvx_bitSet(what, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SUBFORMAT_SHIFT);
		}
	}

	if (jvx_bitTest(cmpWhat, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGX_SHIFT))
	{
		if (node_inout._common_set_node_params_a_1io.segmentation.x != theData->con_params.segmentation.x)
		{
			jvx_bitSet(what, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGX_SHIFT);
		}
	}
	
	if (jvx_bitTest(cmpWhat, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGY_SHIFT))
	{
		if (node_inout._common_set_node_params_a_1io.segmentation.y != theData->con_params.segmentation.y)
		{
			jvx_bitSet(what, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGY_SHIFT);
		}
	}

	if (jvx_bitTest(cmpWhat, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_DATAFLOW_SHIFT))
	{
		if (node_inout._common_set_node_params_a_1io.data_flow != theData->con_params.data_flow)
		{
			jvx_bitSet(what, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_DATAFLOW_SHIFT);
		}
	}

	if (jvx_bitTest(cmpWhat, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMATSPEC_SHIFT))
	{
		if (node_inout._common_set_node_params_a_1io.format_spec != theData->con_params.format_spec.std_str())
		{
			jvx_bitSet(what, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMATSPEC_SHIFT);
		}
	}
	return what;
}

void
CjvxNodeBase1io::update_ldesc_from_input_params_on_test()
{
	_common_set_ocon.theData_out.con_params.number_channels = node_inout._common_set_node_params_a_1io.number_channels;
	_common_set_ocon.theData_out.con_params.buffersize = node_inout._common_set_node_params_a_1io.buffersize;
	_common_set_ocon.theData_out.con_params.rate = node_inout._common_set_node_params_a_1io.samplerate;
	_common_set_ocon.theData_out.con_params.format = (jvxDataFormat)node_inout._common_set_node_params_a_1io.format;
	_common_set_ocon.theData_out.con_params.format_group = (jvxDataFormatGroup)node_inout._common_set_node_params_a_1io.subformat;
	_common_set_ocon.theData_out.con_params.segmentation.x = node_inout._common_set_node_params_a_1io.segmentation.x;
	_common_set_ocon.theData_out.con_params.segmentation.y = node_inout._common_set_node_params_a_1io.segmentation.y;
	_common_set_ocon.theData_out.con_params.number_channels = node_inout._common_set_node_params_a_1io.number_channels;
	_common_set_ocon.theData_out.con_params.data_flow = (jvxDataflow)node_inout._common_set_node_params_a_1io.data_flow;
}

void
CjvxNodeBase1io::constrain_ldesc_from_neg_params(const CjvxNegotiate_common& neg)
{
	// ===================================================================
	// Prefer max buffersize 
	JVX_MIN_NEG_SET_LDAT(neg.preferred.buffersize, _common_set_ocon.theData_out.con_params.buffersize);
	JVX_MAX_NEG_SET_LDAT(neg.preferred.buffersize, _common_set_ocon.theData_out.con_params.buffersize);
	// ===================================================================

	// ===================================================================
	// Prefer min samplerate
	JVX_MAX_NEG_SET_LDAT(neg.preferred.samplerate, _common_set_ocon.theData_out.con_params.rate);
	JVX_MIN_NEG_SET_LDAT(neg.preferred.buffersize, _common_set_ocon.theData_out.con_params.rate);
	// ===================================================================

	// ===================================================================
	// Prefer min number channels
	JVX_MAX_NEG_SET_LDAT(neg.preferred.number_channels, _common_set_ocon.theData_out.con_params.number_channels);
	JVX_MIN_NEG_SET_LDAT(neg.preferred.number_channels, _common_set_ocon.theData_out.con_params.number_channels);
	// ===================================================================

	JVX_MAX_NEG_SET_LDAT(neg.preferred.dimX, _common_set_ocon.theData_out.con_params.segmentation.x);
	JVX_MIN_NEG_SET_LDAT(neg.preferred.dimX, _common_set_ocon.theData_out.con_params.segmentation.x);

	// ===================================================================

	JVX_MAX_NEG_SET_LDAT(neg.preferred.dimY, _common_set_ocon.theData_out.con_params.segmentation.y);
	JVX_MIN_NEG_SET_LDAT(neg.preferred.dimY, _common_set_ocon.theData_out.con_params.segmentation.y);

	// ===================================================================
	// Prefer min format
	JVX_MAX_NEG_SET_LDAT_CMP(neg.preferred.format, _common_set_ocon.theData_out.con_params.format, JVX_DATAFORMAT_NONE);
	JVX_MIN_NEG_SET_LDAT_CMP(neg.preferred.format, _common_set_ocon.theData_out.con_params.format, JVX_DATAFORMAT_NONE);
	// ===================================================================

	// ===================================================================
	// Prefer min format
	JVX_MAX_NEG_SET_LDAT_CMP(neg.preferred.subformat, _common_set_ocon.theData_out.con_params.format_group, JVX_DATAFORMAT_GROUP_NONE);
	JVX_MIN_NEG_SET_LDAT_CMP(neg.preferred.subformat, _common_set_ocon.theData_out.con_params.format_group, JVX_DATAFORMAT_GROUP_NONE);
	// ===================================================================

	// ===================================================================
	// Prefer min format
	JVX_MAX_NEG_SET_LDAT_CMP(neg.preferred.data_flow, _common_set_ocon.theData_out.con_params.data_flow, JVX_DATAFLOW_NONE);
	JVX_MIN_NEG_SET_LDAT_CMP(neg.preferred.data_flow, _common_set_ocon.theData_out.con_params.data_flow, JVX_DATAFLOW_NONE);
	// ===================================================================
}

jvxErrorType
CjvxNodeBase1io::set_manipulate_value(jvxSize id, jvxVariant* varray)
{
	jvxApiString* ptrString = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	if (!varray)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (id)
	{
	case JVX_MANIPULATE_DESCRIPTION:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			_common_set_min.theDescription = ptrString->std_str();
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	case JVX_MANIPULATE_DESCRIPTOR:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			_common_set_min.theDescription = ptrString->std_str();
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

jvxErrorType
CjvxNodeBase1io::get_manipulate_value(jvxSize id, jvxVariant* varray)
{
	jvxApiString* ptrString = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	if (!varray)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (id)
	{
	case JVX_MANIPULATE_DESCRIPTION:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			ptrString->assign(_common_set_min.theDescription);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	case JVX_MANIPULATE_DESCRIPTOR:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			ptrString->assign(_common_set_min.theDescription);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}