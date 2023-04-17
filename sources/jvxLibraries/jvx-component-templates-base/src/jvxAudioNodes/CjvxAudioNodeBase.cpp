#include "jvxAudioNodes/CjvxAudioNodeBase.h"

CjvxAudioNodeBase::CjvxAudioNodeBase(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected( JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	_common_set_node_params_1io.acceptOnlyExactMatchLinkDataInput = false;
	_common_set_node_params_1io.acceptOnlyExactMatchLinkDataOutput = false;

	_common_set_node_params_1io.preferred.buffersize.min = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.buffersize.max = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.number_input_channels.min = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.number_input_channels.max = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.number_output_channels.min = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.number_output_channels.max = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.samplerate.min = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.samplerate.max = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.format.min = JVX_DATAFORMAT_DATA;
	_common_set_node_params_1io.preferred.format.max = JVX_DATAFORMAT_32BIT_LE;

	_common_set_node_params_1io.processing.buffersize = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.processing.number_input_channels = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.processing.number_output_channels = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.processing.samplerate = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.processing.format = JVX_DATAFORMAT_NONE;

	_common_set.theComponentSubTypeDescriptor = "signal_processing_node/audio_node";

	// Avoid infinite loops
	//_common_set_node_params_1io.report_take_over_started = false;

	_common_set_node_params_1io.settingReadyToStart = true;
}

CjvxAudioNodeBase::~CjvxAudioNodeBase()
{
}

jvxErrorType
CjvxAudioNodeBase::activate()
{
	jvxErrorType res = _activate();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		CjvxAudioNode_genpcg::init__properties_parameters();
		CjvxAudioNode_genpcg::allocate__properties_parameters();
		CjvxAudioNode_genpcg::register__properties_parameters(static_cast<CjvxProperties*>(this));

		CjvxAudioNode_genpcg::init__properties_active_higher();
		CjvxAudioNode_genpcg::allocate__properties_active_higher();
		CjvxAudioNode_genpcg::register__properties_active_higher(static_cast<CjvxProperties*>(this));

		// This call can not be executed here since we hae not defined the DataProcessor interface at this point.
		// It is typically called in CjvxAudioNode
		// JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(static_cast<IjvxDataProcessor*>(this), static_cast<IjvxObject*>(this), "default", NULL);
	}
	return(res);
};

jvxErrorType
CjvxAudioNodeBase::deactivate()
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = _deactivate();
	if (res == JVX_NO_ERROR)
	{

		CjvxAudioNode_genpcg::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		CjvxAudioNode_genpcg::deallocate__properties_active_higher();

		CjvxAudioNode_genpcg::unregister__properties_parameters(static_cast<CjvxProperties*>(this));
		CjvxAudioNode_genpcg::deallocate__properties_parameters();

		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
};


jvxErrorType
CjvxAudioNodeBase::reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId)
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool requiresCorrection = false;
	jvxBool transferAll = false;

	IjvxDataConnectionProcess* theProc = NULL;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxErrorType res = JVX_ERROR_NOT_READY;

	if (_common_set_icon.theData_in)
	{
		if (_common_set_icon.theData_in->con_link.master)
		{
			_common_set_icon.theData_in->con_link.master->associated_process(&theProc);
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
CjvxAudioNodeBase::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	std::string txt;
	jvxErrorType res = _is_ready(suc, reasonIfNot);
	jvxBool Iamready = true;
	jvxBool isAProblem = false;

	if (res == JVX_NO_ERROR)
	{
		/*
		 * CHECK BUFFERSIZE
		 */
		isAProblem = false;
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.buffersize.min))
		{
			if (CjvxAudioNode_genpcg::properties_parameters.buffersize.value < _common_set_node_params_1io.preferred.buffersize.min)
			{
				isAProblem = true;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.buffersize.max))
		{
			if (CjvxAudioNode_genpcg::properties_parameters.buffersize.value > _common_set_node_params_1io.preferred.buffersize.max)
			{
				isAProblem = true;
			}
		}
		if (isAProblem)
		{
			Iamready = false;
			txt = "Buffersize mismatch, constraint: ";
			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.buffersize.min))
			{
				txt += jvx_size2String(_common_set_node_params_1io.preferred.buffersize.min);
				txt += "<=";
			}
			txt += jvx_int2String(CjvxAudioNode_genpcg::properties_parameters.buffersize.value);
			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.buffersize.max))
			{
				txt += "<=";
				txt += jvx_size2String(_common_set_node_params_1io.preferred.buffersize.max);
			}
		}

		/*
		 * CHECK SAMPLERATE
		 */
		isAProblem = false;
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.samplerate.min))
		{
			if (CjvxAudioNode_genpcg::properties_parameters.samplerate.value < _common_set_node_params_1io.preferred.samplerate.min)
			{
				isAProblem = true;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.samplerate.max))
		{
			if (CjvxAudioNode_genpcg::properties_parameters.samplerate.value > _common_set_node_params_1io.preferred.samplerate.max)
			{
				isAProblem = true;
			}
		}
		if (isAProblem)
		{
			Iamready = false;
			txt = "Samplerate mismatch, constraint: ";
			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.samplerate.min))
			{
				txt += jvx_size2String(_common_set_node_params_1io.preferred.samplerate.min);
				txt += "<=";
			}
			txt += jvx_int2String(CjvxAudioNode_genpcg::properties_parameters.samplerate.value);
			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.samplerate.max))
			{
				txt += "<=";
				txt += jvx_size2String(_common_set_node_params_1io.preferred.samplerate.max);
			}
		}

		/*
		 * CHECK NUMBER INPUT CHANNELS
		 */
		isAProblem = false;
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_input_channels.min))
		{
			if (CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value < _common_set_node_params_1io.preferred.number_input_channels.min)
			{
				isAProblem = true;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_input_channels.max))
		{
			if (CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value > _common_set_node_params_1io.preferred.number_input_channels.max)
			{
				isAProblem = true;
			}
		}
		if (isAProblem)
		{
			Iamready = false;
			txt = "Input Channel mismatch, constraint: ";
			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_input_channels.min))
			{
				txt += jvx_size2String(_common_set_node_params_1io.preferred.number_input_channels.min, true);
				txt += "<=";
			}
			txt += jvx_int2String(CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value);
			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_input_channels.max))
			{
				txt += "<=";
				txt += jvx_size2String(_common_set_node_params_1io.preferred.number_input_channels.max, true);
			}
		}


		/*
		 * CHECK NUMBER OUTPUT CHANNELS
		 */
		isAProblem = false;

		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_output_channels.min))
		{
			if (CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value < _common_set_node_params_1io.preferred.number_output_channels.min)
			{
				isAProblem = true;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_output_channels.max))
		{
			if (CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value > _common_set_node_params_1io.preferred.number_output_channels.max)
			{
				isAProblem = true;
			}
		}


		if (isAProblem)
		{
			Iamready = false;
			txt = "Output Channel mismatch, constraint: ";
			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_output_channels.min))
			{
				txt += jvx_size2String(_common_set_node_params_1io.preferred.number_output_channels.min);
				txt += "<=";
			}
			txt += jvx_int2String(CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value);

			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_output_channels.max))
			{
				txt += "<=";
				txt += jvx_size2String(_common_set_node_params_1io.preferred.number_output_channels.max, true);
			}
		}

		/*
		 * CHECK PROCESSING FORMAT
		 */
		isAProblem = false;

		if (_common_set_node_params_1io.preferred.format.min > JVX_DATAFORMAT_NONE)
		{
			if (CjvxAudioNode_genpcg::properties_parameters.format.value < _common_set_node_params_1io.preferred.format.min)
			{
				isAProblem = true;
			}
		}
		if (_common_set_node_params_1io.preferred.format.max > JVX_DATAFORMAT_NONE)
		{
			if (CjvxAudioNode_genpcg::properties_parameters.format.value > _common_set_node_params_1io.preferred.format.max)
			{
				isAProblem = true;
			}
		}


		if (isAProblem)
		{
			Iamready = false;
			txt = "Processing Format mismatch, constraint: ";
			if (_common_set_node_params_1io.preferred.format.min != JVX_DATAFORMAT_NONE)
			{
				txt += jvxDataFormat_txt(_common_set_node_params_1io.preferred.format.min);
				txt += "<=";
			}
			txt += jvxDataFormat_txt(CjvxAudioNode_genpcg::properties_parameters.format.value);
			if (_common_set_node_params_1io.preferred.format.max != JVX_DATAFORMAT_NONE)
			{
				txt += "<=";
				txt += jvxDataFormat_txt(_common_set_node_params_1io.preferred.format.max);
			}
		}

		/*
		* CHECK SAMPLERATE
		*/
		if (!_common_set_node_params_1io.settingReadyToStart)
		{
			Iamready = false;
			txt = "Algorithm is not propperly configured: ";
			txt += _common_set_node_params_1io.settingReadyFailReason;
		}

		if (!txt.empty())
		{
			if (reasonIfNot)
			{
				reasonIfNot->assign(txt);
			}
		}
		if (suc)
		{
			*suc = Iamready;
		}
	}

	return(res);
};

jvxErrorType
CjvxAudioNodeBase::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		CjvxAudioNode_genpcg::put_configuration_all(callConf, processor,
			sectionToContainAllSubsectionsForMe, &warns);
		for (i = 0; i < warns.size(); i++)
		{
			_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
	}
	return JVX_NO_ERROR;
}
jvxErrorType
CjvxAudioNodeBase::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	CjvxAudioNode_genpcg::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioNodeBase::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxLinkDataDescriptor ld_cp;
	jvxBool thereismismatch = false;
	jvxSize num = _common_set_node_params_1io.allowedSetup.size();
	jvxErrorType resComplain = JVX_NO_ERROR;
	std::string txt;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this), 
		CjvxInputOutputConnector::_common_set_ldslave.descriptor.c_str(),
		"Entering CjvxAudioNodeBase default input connector");
	if (num)
	{
		thereismismatch = true;
		for (i = 0; i < num; i++)
		{
			if (
				(_common_set_node_params_1io.allowedSetup[i].buffersize == _common_set_icon.theData_in->con_params.buffersize) &&
				(_common_set_node_params_1io.allowedSetup[i].samplerate == _common_set_icon.theData_in->con_params.rate) &&
				(_common_set_node_params_1io.allowedSetup[i].format == _common_set_icon.theData_in->con_params.format) &&
				(_common_set_node_params_1io.allowedSetup[i].number_input_channels == _common_set_icon.theData_in->con_params.number_channels))
			{
				thereismismatch = false;
				CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value = JVX_SIZE_INT32(_common_set_node_params_1io.allowedSetup[i].number_output_channels);
				break;
			}
		}
		if (thereismismatch)
		{
			ld_cp.con_params.buffersize = _common_set_node_params_1io.allowedSetup[i].buffersize;
			ld_cp.con_params.rate = _common_set_node_params_1io.allowedSetup[i].samplerate;
			ld_cp.con_params.format = _common_set_node_params_1io.allowedSetup[i].format;
			ld_cp.con_params.number_channels = _common_set_node_params_1io.allowedSetup[i].number_input_channels;

			resComplain = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(
				JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &ld_cp JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		ld_cp.con_params.buffersize = _common_set_icon.theData_in->con_params.buffersize;
		ld_cp.con_params.rate = _common_set_icon.theData_in->con_params.rate;
		ld_cp.con_params.format = _common_set_icon.theData_in->con_params.format;
		ld_cp.con_params.number_channels = _common_set_icon.theData_in->con_params.number_channels;

		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.buffersize.min))
		{
			if (_common_set_icon.theData_in->con_params.buffersize < _common_set_node_params_1io.preferred.buffersize.min)
			{
				thereismismatch = true;
				ld_cp.con_params.buffersize = _common_set_node_params_1io.preferred.buffersize.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.buffersize.max))
		{
			if (_common_set_icon.theData_in->con_params.buffersize > _common_set_node_params_1io.preferred.buffersize.max)
			{
				thereismismatch = true;
				ld_cp.con_params.buffersize = _common_set_node_params_1io.preferred.buffersize.max;
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.samplerate.min))
		{
			if (_common_set_icon.theData_in->con_params.rate < _common_set_node_params_1io.preferred.samplerate.min)
			{
				thereismismatch = true;
				ld_cp.con_params.rate = _common_set_node_params_1io.preferred.samplerate.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.samplerate.max))
		{
			if (_common_set_icon.theData_in->con_params.rate > _common_set_node_params_1io.preferred.samplerate.max)
			{
				thereismismatch = true;
				ld_cp.con_params.rate = _common_set_node_params_1io.preferred.samplerate.max;
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_input_channels.min))
		{
			if (_common_set_icon.theData_in->con_params.number_channels < _common_set_node_params_1io.preferred.number_input_channels.min)
			{
				thereismismatch = true;
				ld_cp.con_params.number_channels = _common_set_node_params_1io.preferred.number_input_channels.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_input_channels.max))
		{
			if (_common_set_icon.theData_in->con_params.number_channels > _common_set_node_params_1io.preferred.number_input_channels.max)
			{
				thereismismatch = true;
				ld_cp.con_params.number_channels = _common_set_node_params_1io.preferred.number_input_channels.max;
			}
		}

		if (_common_set_node_params_1io.preferred.format.min != JVX_DATAFORMAT_NONE)
		{
			if (_common_set_icon.theData_in->con_params.format < _common_set_node_params_1io.preferred.format.min)
			{
				thereismismatch = true;
				ld_cp.con_params.format = _common_set_node_params_1io.preferred.format.min;
			}
		}
		if (_common_set_node_params_1io.preferred.format.max != JVX_DATAFORMAT_NONE)
		{
			if (_common_set_icon.theData_in->con_params.format > _common_set_node_params_1io.preferred.format.max)
			{
				thereismismatch = true;
				ld_cp.con_params.format = _common_set_node_params_1io.preferred.format.max;
			}
		}
		if (thereismismatch)
		{
			resComplain = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, 
				&ld_cp JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}

	switch (resComplain)
	{
	case JVX_NO_ERROR:

		// Accept the new settings
		CjvxAudioNode_genpcg::properties_parameters.buffersize.value = JVX_SIZE_INT32(_common_set_icon.theData_in->con_params.buffersize);
		CjvxAudioNode_genpcg::properties_parameters.samplerate.value = JVX_SIZE_INT32(_common_set_icon.theData_in->con_params.rate);
		CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value = JVX_SIZE_INT32(_common_set_icon.theData_in->con_params.number_channels);
		CjvxAudioNode_genpcg::properties_parameters.format.value = _common_set_icon.theData_in->con_params.format;
		return _test_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
	case JVX_ERROR_COMPROMISE:

		/*
		// We might accept a compromies
		CjvxAudioNode_genpcg::properties_parameters.buffersize.value = _common_set_icon.theData_in->con_params.buffersize;
		CjvxAudioNode_genpcg::properties_parameters.samplerate.value = _common_set_icon.theData_in->con_params.rate;
		CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value = _common_set_icon.theData_in->con_params.number_channels;
		CjvxAudioNode_genpcg::properties_parameters.format.value = _common_set_icon.theData_in->con_params.format;

		return JVX_NO_ERROR;*/
		break;
	default:

		txt = __FUNCTION__;
		txt += ": ";
		txt += "When trying to negotiate parameters with connected output connector, negotiations failed.";
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), resComplain);
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxAudioNodeBase::test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));
	_common_set_ocon.theData_out.con_params.number_channels = CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value;
	_common_set_ocon.theData_out.con_params.buffersize = CjvxAudioNode_genpcg::properties_parameters.buffersize.value;
	_common_set_ocon.theData_out.con_params.rate = CjvxAudioNode_genpcg::properties_parameters.samplerate.value;
	_common_set_ocon.theData_out.con_params.format = (jvxDataFormat)CjvxAudioNode_genpcg::properties_parameters.format.value;

	return _test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxAudioNodeBase::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	const jvxLinkDataDescriptor* ld = (const jvxLinkDataDescriptor*)data;
	jvxBool thereismismatch = false;
	jvxErrorType res = JVX_NO_ERROR;
	std::string txt;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		if (
			(ld->con_params.rate != CjvxAudioNode_genpcg::properties_parameters.samplerate.value) ||
			(ld->con_params.buffersize != CjvxAudioNode_genpcg::properties_parameters.buffersize.value) ||
			(ld->con_params.format != CjvxAudioNode_genpcg::properties_parameters.format.value))
		{
			txt = __FUNCTION__;
			txt += ": ";
			jvxBool thereisone = false;
			if (ld->con_params.rate != CjvxAudioNode_genpcg::properties_parameters.samplerate.value)
			{
				txt += "Input and output rate do not match: ";
				txt += jvx_size2String(CjvxAudioNode_genpcg::properties_parameters.samplerate.value);
				txt += " on input side whereas ";
				txt += jvx_size2String(ld->con_params.rate);
				txt += " on output side";
				thereisone = true;
			}
			if (ld->con_params.buffersize != CjvxAudioNode_genpcg::properties_parameters.buffersize.value)
			{
				if (thereisone)
				{
					txt += ", ";
				}
				txt += ": Input and output buffersize do not match: ";
				txt += jvx_size2String(CjvxAudioNode_genpcg::properties_parameters.buffersize.value);
				txt += " on input side whereas ";
				txt += jvx_size2String(ld->con_params.buffersize);
				txt += " on output side.";
				thereisone = true;
			}
			if (ld->con_params.format != CjvxAudioNode_genpcg::properties_parameters.format.value)
			{
				if (thereisone)
				{
					txt += ", ";
				}
				txt += ": Input and output format do not match: ";
				txt += jvxDataFormat_txt(CjvxAudioNode_genpcg::properties_parameters.format.value);
				txt += " on input side whereas ";
				txt += jvxDataFormat_txt(ld->con_params.format);
				txt += " on output side";
			}
			txt += ".";
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), JVX_ERROR_INVALID_SETTING);
			res = JVX_ERROR_UNSUPPORTED;
		}
		else
		{
			// Accept what comes from the output side in terms of channels
			CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value = JVX_SIZE_INT32(ld->con_params.number_channels);

			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_output_channels.min))
			{

				if (CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value < _common_set_node_params_1io.preferred.number_output_channels.min)
				{
					thereismismatch = true;
					CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value =
						JVX_SIZE_INT32(_common_set_node_params_1io.preferred.number_output_channels.min);
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_output_channels.max))
			{
				if (CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value > _common_set_node_params_1io.preferred.number_output_channels.max)
				{
					thereismismatch = true;
					CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value =
						JVX_SIZE_INT32(_common_set_node_params_1io.preferred.number_output_channels.max);
				}
			}

			// Allow overwrite of output channels
			_common_set_ocon.theData_out.con_params.number_channels = CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value;

			if (thereismismatch)
			{
				res = JVX_ERROR_COMPROMISE;
			}
			else
			{
				res = JVX_NO_ERROR;
			}
		}
		break;
	default:
		res = _transfer_backward_ocon(true, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
};

