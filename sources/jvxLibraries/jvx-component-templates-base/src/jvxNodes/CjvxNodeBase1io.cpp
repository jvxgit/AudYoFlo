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

	jvx_test_settings::init_connect_icon_flex(&_common_set_node_params_1io);
}; 

jvxErrorType
CjvxNodeBase1io::activate()
{
	jvxErrorType res = _activate();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		if (newParamProps)
		{
			CjvxSimplePropsPars::initialize(this);
		}
		else
		{
			CjvxNode_genpcg::init_all();
			CjvxNode_genpcg::allocate_all();
			CjvxNode_genpcg::register_all(static_cast<CjvxProperties*>(this));
		}
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
		if (newParamProps)
		{
			CjvxSimplePropsPars::terminate();
		}
		else
		{
			CjvxNode_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
			CjvxNode_genpcg::deallocate_all();
		}

		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
};


jvxErrorType
CjvxNodeBase1io::reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId)
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool requiresCorrection = false;
	jvxBool transferAll = false;

	IjvxDataConnectionProcess* theProc = NULL;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxErrorType res = JVX_ERROR_NOT_READY;

	if (_common_set_ldslave.theData_in)
	{
		if (_common_set_ldslave.theData_in->con_link.master)
		{
			_common_set_ldslave.theData_in->con_link.master->associated_process(&theProc);
			if (theProc)
			{
				theProc->unique_id_connections(&uId);
				res = this->_request_test_chain_master(uId);
			}
		}
	}
	return res;

}

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
		CjvxNode_genpcg::put_configuration_all(callConf, processor,
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
	CjvxNode_genpcg::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);

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

	res = neg_input._negotiate_connect_icon(_common_set_ldslave.theData_in,
		static_cast<IjvxObject*>(this),
		_common_set_ldslave.descriptor.c_str()
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{		
		// Copy the parameters from the negotiation result to the internal parameter (and properties)
		update_input_params_from_neg_on_test();

		// Function to derive output parameters from input arguments
		from_input_to_output();
	}
	return res;
}

jvxErrorType
CjvxNodeBase1io::accept_input_parameters_stop(jvxErrorType resTest)
{
	assert(0);
	return JVX_NO_ERROR;
}

/*
void
CjvxNodeBase1io::from_input_to_output()
{
	// To be set to zero later, still needed in code recinstruction, however.
	assert(0);
}
*/ 

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
			res = neg_output._negotiate_transfer_backward_ocon(ld,
				&_common_set_ldslave.theData_out, static_cast<IjvxObject*>(this), NULL
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
	return JVX_NO_ERROR;
}

void
CjvxNodeBase1io::test_set_output_parameters()
{
	// The default behavior is to derive output link descriptor values from the output paramaters
	// in function test_connect_icon. Here, we constrain the link descriptor values to match the 
	// negotiation parameters. The output parameters are again updated in the call to
	// update_output_params_on_test(); in function test_connect_icon
	constrain_ldesc_from_neg_output_params();
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
CjvxNodeBase1io::update_input_params_from_neg_on_test()
{
	if (newParamProps)
	{
		CjvxSimplePropsPars::_common_set_node_params_a_1io.number_input_channels = JVX_SIZE_INT32(neg_input._latest_results.number_channels);
		CjvxSimplePropsPars::_common_set_node_params_a_1io.buffersize = JVX_SIZE_INT32(neg_input._latest_results.buffersize);
		CjvxSimplePropsPars::_common_set_node_params_a_1io.samplerate = JVX_SIZE_INT32(neg_input._latest_results.rate);
		CjvxSimplePropsPars::_common_set_node_params_a_1io.format = JVX_SIZE_INT32(neg_input._latest_results.format);
		CjvxSimplePropsPars::_common_set_node_params_a_1io.subformat = JVX_SIZE_INT32(neg_input._latest_results.format_group);
		CjvxSimplePropsPars::_common_set_node_params_a_1io.dimX = JVX_SIZE_INT32(neg_input._latest_results.segmentation_x);
		CjvxSimplePropsPars::_common_set_node_params_a_1io.dimY = JVX_SIZE_INT32(neg_input._latest_results.segmentation_y);
	}
	else
	{
		CjvxNode_genpcg::node.numberinputchannels.value = JVX_SIZE_INT32(neg_input._latest_results.number_channels);
		CjvxNode_genpcg::node.buffersize.value = JVX_SIZE_INT32(neg_input._latest_results.buffersize);
		CjvxNode_genpcg::node.samplerate.value = JVX_SIZE_INT32(neg_input._latest_results.rate);
		CjvxNode_genpcg::node.format.value = JVX_SIZE_INT32(neg_input._latest_results.format);
		CjvxNode_genpcg::node.subformat.value = JVX_SIZE_INT32(neg_input._latest_results.format_group);
		CjvxNode_genpcg::node.segmentsize_x.value = JVX_SIZE_INT32(neg_input._latest_results.segmentation_x);
		CjvxNode_genpcg::node.segmentsize_y.value = JVX_SIZE_INT32(neg_input._latest_results.segmentation_y);
	}
}

void
CjvxNodeBase1io::update_simple_params_from_ldesc()
{
	if (newParamProps)
	{
		CjvxSimplePropsPars::_common_set_node_params_a_1io.buffersize = _common_set_ldslave.theData_in->con_params.buffersize;
		CjvxSimplePropsPars::_common_set_node_params_a_1io.samplerate = _common_set_ldslave.theData_in->con_params.rate;
		CjvxSimplePropsPars::_common_set_node_params_a_1io.number_input_channels = _common_set_ldslave.theData_in->con_params.number_channels;
		CjvxSimplePropsPars::_common_set_node_params_a_1io.format = _common_set_ldslave.theData_in->con_params.format;
		CjvxSimplePropsPars::_common_set_node_params_a_1io.subformat = _common_set_ldslave.theData_in->con_params.format_group;
		CjvxSimplePropsPars::_common_set_node_params_a_1io.dimX = _common_set_ldslave.theData_in->con_params.segmentation_x;
		CjvxSimplePropsPars::_common_set_node_params_a_1io.dimY = _common_set_ldslave.theData_in->con_params.segmentation_y;
	}
	else
	{
		CjvxNode_genpcg::node.buffersize.value = _common_set_ldslave.theData_in->con_params.buffersize;
		CjvxNode_genpcg::node.samplerate.value = _common_set_ldslave.theData_in->con_params.rate;
		CjvxNode_genpcg::node.numberinputchannels.value = _common_set_ldslave.theData_in->con_params.number_channels;
		CjvxNode_genpcg::node.numberoutputchannels.value = _common_set_ldslave.theData_out.con_params.number_channels;
		CjvxNode_genpcg::node.format.value = _common_set_ldslave.theData_in->con_params.format;
		CjvxNode_genpcg::node.subformat.value = _common_set_ldslave.theData_in->con_params.format_group;
		CjvxNode_genpcg::node.segmentsize_x.value = _common_set_ldslave.theData_in->con_params.segmentation_x;
		CjvxNode_genpcg::node.segmentsize_y.value = _common_set_ldslave.theData_in->con_params.segmentation_y;
	}
}

void
CjvxNodeBase1io::constrain_ldesc_from_neg_output_params()
{
	// ===================================================================
	// Prefer max buffersize 
	JVX_MIN_NEG_SET_LDAT(neg_output.preferred.buffersize, _common_set_ldslave.theData_out.con_params.buffersize);
	JVX_MAX_NEG_SET_LDAT(neg_output.preferred.buffersize, _common_set_ldslave.theData_out.con_params.buffersize);
	// ===================================================================

	// ===================================================================
	// Prefer min samplerate
	JVX_MAX_NEG_SET_LDAT(neg_output.preferred.samplerate, _common_set_ldslave.theData_out.con_params.rate);
	JVX_MIN_NEG_SET_LDAT(neg_output.preferred.buffersize, _common_set_ldslave.theData_out.con_params.rate);
	// ===================================================================

	// ===================================================================
	// Prefer min number channels
	JVX_MAX_NEG_SET_LDAT(neg_output.preferred.number_channels, _common_set_ldslave.theData_out.con_params.number_channels);
	JVX_MIN_NEG_SET_LDAT(neg_output.preferred.number_channels, _common_set_ldslave.theData_out.con_params.number_channels);
	// ===================================================================

	_common_set_ldslave.theData_out.con_params.segmentation_x = _common_set_ldslave.theData_out.con_params.buffersize;
	_common_set_ldslave.theData_out.con_params.segmentation_y = _common_set_ldslave.theData_out.con_params.number_channels;

	// ===================================================================
	// Prefer min format
	JVX_MAX_NEG_SET_LDAT_CMP(neg_output.preferred.format, _common_set_ldslave.theData_out.con_params.format, JVX_DATAFORMAT_NONE);
	JVX_MIN_NEG_SET_LDAT_CMP(neg_output.preferred.format, _common_set_ldslave.theData_out.con_params.format, JVX_DATAFORMAT_NONE);
	// ===================================================================

	// ===================================================================
	// Prefer min format
	JVX_MAX_NEG_SET_LDAT_CMP(neg_output.preferred.subformat, _common_set_ldslave.theData_out.con_params.format_group, JVX_DATAFORMAT_GROUP_NONE);
	JVX_MIN_NEG_SET_LDAT_CMP(neg_output.preferred.subformat, _common_set_ldslave.theData_out.con_params.format_group, JVX_DATAFORMAT_GROUP_NONE);
	// ===================================================================
}