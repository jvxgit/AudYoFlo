#include "CjvxNodeBaseNio.h"

jvxErrorType
CjvxNodeBaseNio::activate()
{
	jvxErrorType res = _activate();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		CjvxNode_genpcg::init_all();
		CjvxNode_genpcg::allocate_all();
		CjvxNode_genpcg::register_all(static_cast<CjvxProperties*>(this));
		//JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), "default", NULL);
	}
	return(res);
};

jvxErrorType
CjvxNodeBaseNio::deactivate()
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = _deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxNode_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxNode_genpcg::deallocate_all();

		//JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
};


#if 0
jvxErrorType
CjvxNodeBaseNio::reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId)
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
#endif


jvxErrorType
CjvxNodeBaseNio::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	std::string txt;
	jvxErrorType res = _is_ready(suc, reasonIfNot);
	jvxBool Iamready = true;
	jvxBool isAProblem = false;

	if (res == JVX_NO_ERROR)
	{
		// 
	}

#if 0		
		/*
		 * CHECK BUFFERSIZE
		 */
		isAProblem = false;
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.buffersize.min))
		{
			if (CjvxNode_genpcg::node.buffersize.value < _common_set_node_params_1io.preferred.buffersize.min)
			{
				isAProblem = true;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.buffersize.max))
		{
			if (CjvxNode_genpcg::node.buffersize.value > _common_set_node_params_1io.preferred.buffersize.max)
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
			txt += jvx_int2String(CjvxNode_genpcg::node.buffersize.value);
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
			if (CjvxNode_genpcg::node.samplerate.value < _common_set_node_params_1io.preferred.samplerate.min)
			{
				isAProblem = true;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.samplerate.max))
		{
			if (CjvxNode_genpcg::node.samplerate.value > _common_set_node_params_1io.preferred.samplerate.max)
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
			txt += jvx_int2String(CjvxNode_genpcg::node.samplerate.value);
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
			if (CjvxNode_genpcg::node.numberinputchannels.value < _common_set_node_params_1io.preferred.number_input_channels.min)
			{
				isAProblem = true;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_input_channels.max))
		{
			if (CjvxNode_genpcg::node.numberinputchannels.value > _common_set_node_params_1io.preferred.number_input_channels.max)
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
			txt += jvx_int2String(CjvxNode_genpcg::node.numberinputchannels.value);
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
			if (CjvxNode_genpcg::node.numberoutputchannels.value < _common_set_node_params_1io.preferred.number_output_channels.min)
			{
				isAProblem = true;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_output_channels.max))
		{
			if (CjvxNode_genpcg::node.numberoutputchannels.value > _common_set_node_params_1io.preferred.number_output_channels.max)
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
			txt += jvx_int2String(CjvxNode_genpcg::node.numberoutputchannels.value);

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
			if (CjvxNode_genpcg::node.format.value < _common_set_node_params_1io.preferred.format.min)
			{
				isAProblem = true;
			}
		}
		if (_common_set_node_params_1io.preferred.format.max > JVX_DATAFORMAT_NONE)
		{
			if (CjvxNode_genpcg::node.format.value > _common_set_node_params_1io.preferred.format.max)
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
			txt += jvxDataFormat_txt(CjvxNode_genpcg::node.format.value);
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
#endif
	return(res);
};


jvxErrorType
CjvxNodeBaseNio::put_configuration(IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, jvxFlagTag flagtag,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		CjvxNode_genpcg::put_configuration_all(processor,
			sectionToContainAllSubsectionsForMe, flagtag, warns);
		for (i = 0; i < warns.size(); i++)
		{
			_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
	}
	return JVX_NO_ERROR;
}
jvxErrorType
CjvxNodeBaseNio::get_configuration(IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections, jvxFlagTag flagtag)
{
	CjvxNode_genpcg::get_configuration_all(processor, sectionWhereToAddAllSubsections, flagtag);

	return JVX_NO_ERROR;
}

#if 0
jvxErrorType
CjvxNodeBaseNio::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxLinkDataDescriptor ld_cp;
	jvxBool thereismismatch = false;
	jvxSize num = _common_set_node_params_1io.allowedSetup.size();
	jvxErrorType resComplain = JVX_NO_ERROR;
	std::string txt;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);

	if (num)
	{
		thereismismatch = true;
		for (i = 0; i < num; i++)
		{
			if (
				(_common_set_node_params_1io.allowedSetup[i].buffersize == _common_set_ldslave.theData_in->con_params.buffersize) &&
				(_common_set_node_params_1io.allowedSetup[i].samplerate == _common_set_ldslave.theData_in->con_params.rate) &&
				(_common_set_node_params_1io.allowedSetup[i].format == _common_set_ldslave.theData_in->con_params.format) &&
				(_common_set_node_params_1io.allowedSetup[i].number_input_channels == _common_set_ldslave.theData_in->con_params.number_channels)&&
				(_common_set_node_params_1io.allowedSetup[i].subformat == _common_set_ldslave.theData_in->con_params.format_group) && 
				(_common_set_node_params_1io.allowedSetup[i].dimX == _common_set_ldslave.theData_in->con_params.segmentation_x) && 
				(_common_set_node_params_1io.allowedSetup[i].dimY == _common_set_ldslave.theData_in->con_params.segmentation_y))
			{
				thereismismatch = false;
				CjvxNode_genpcg::node.numberoutputchannels.value = JVX_SIZE_INT32(_common_set_node_params_1io.allowedSetup[i].number_output_channels);
				break;
			}
		}
		if (thereismismatch)
		{
			ld_cp.con_params.buffersize = _common_set_node_params_1io.allowedSetup[i].buffersize;
			ld_cp.con_params.rate = _common_set_node_params_1io.allowedSetup[i].samplerate;
			ld_cp.con_params.format = _common_set_node_params_1io.allowedSetup[i].format;
			ld_cp.con_params.number_channels = _common_set_node_params_1io.allowedSetup[i].number_input_channels;
			ld_cp.con_params.segmentation_x = _common_set_node_params_1io.allowedSetup[i].dimX;
			ld_cp.con_params.segmentation_y = _common_set_node_params_1io.allowedSetup[i].dimY;
			ld_cp.con_params.format_group = _common_set_node_params_1io.allowedSetup[i].subformat;
			

			resComplain = _common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &ld_cp JVX_CONNECTION_FEEDBACK_CALL_A(fdb_loc));
		}
	}
	else
	{
		ld_cp.con_params.buffersize = _common_set_ldslave.theData_in->con_params.buffersize;
		ld_cp.con_params.rate = _common_set_ldslave.theData_in->con_params.rate;
		ld_cp.con_params.format = _common_set_ldslave.theData_in->con_params.format;
		ld_cp.con_params.number_channels = _common_set_ldslave.theData_in->con_params.number_channels;

		ld_cp.con_params.segmentation_x = _common_set_ldslave.theData_in->con_params.segmentation_x;
		ld_cp.con_params.segmentation_y = _common_set_ldslave.theData_in->con_params.segmentation_y;
		ld_cp.con_params.format_group = _common_set_ldslave.theData_in->con_params.format_group;
		ld_cp.con_data.ext.hints = _common_set_ldslave.theData_in->con_data.ext.hints;

		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.buffersize.min))
		{
			if (_common_set_ldslave.theData_in->con_params.buffersize < _common_set_node_params_1io.preferred.buffersize.min)
			{
				thereismismatch = true;
				ld_cp.con_params.buffersize = _common_set_node_params_1io.preferred.buffersize.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.buffersize.max))
		{
			if (_common_set_ldslave.theData_in->con_params.buffersize > _common_set_node_params_1io.preferred.buffersize.max)
			{
				thereismismatch = true;
				ld_cp.con_params.buffersize = _common_set_node_params_1io.preferred.buffersize.max;
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.samplerate.min))
		{
			if (_common_set_ldslave.theData_in->con_params.rate < _common_set_node_params_1io.preferred.samplerate.min)
			{
				thereismismatch = true;
				ld_cp.con_params.rate = _common_set_node_params_1io.preferred.samplerate.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.samplerate.max))
		{
			if (_common_set_ldslave.theData_in->con_params.rate > _common_set_node_params_1io.preferred.samplerate.max)
			{
				thereismismatch = true;
				ld_cp.con_params.rate = _common_set_node_params_1io.preferred.samplerate.max;
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_input_channels.min))
		{
			if (_common_set_ldslave.theData_in->con_params.number_channels < _common_set_node_params_1io.preferred.number_input_channels.min)
			{
				thereismismatch = true;
				ld_cp.con_params.number_channels = _common_set_node_params_1io.preferred.number_input_channels.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_input_channels.max))
		{
			if (_common_set_ldslave.theData_in->con_params.number_channels > _common_set_node_params_1io.preferred.number_input_channels.max)
			{
				thereismismatch = true;
				ld_cp.con_params.number_channels = _common_set_node_params_1io.preferred.number_input_channels.max;
			}
		}

		if (_common_set_node_params_1io.preferred.format.min != JVX_DATAFORMAT_NONE)
		{
			if (_common_set_ldslave.theData_in->con_params.format < _common_set_node_params_1io.preferred.format.min)
			{
				thereismismatch = true;
				ld_cp.con_params.format = _common_set_node_params_1io.preferred.format.min;
			}
		}
		if (_common_set_node_params_1io.preferred.format.max != JVX_DATAFORMAT_NONE)
		{
			if (_common_set_ldslave.theData_in->con_params.format > _common_set_node_params_1io.preferred.format.max)
			{
				thereismismatch = true;
				ld_cp.con_params.format = _common_set_node_params_1io.preferred.format.max;
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.dimX.min))
		{
			if (_common_set_ldslave.theData_in->con_params.segmentation_x < _common_set_node_params_1io.preferred.dimX.min)
			{
				thereismismatch = true;
				ld_cp.con_params.segmentation_x = _common_set_node_params_1io.preferred.dimX.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.dimX.max))
		{
			if (_common_set_ldslave.theData_in->con_params.segmentation_x > _common_set_node_params_1io.preferred.dimX.max)
			{
				thereismismatch = true;
				ld_cp.con_params.segmentation_x = _common_set_node_params_1io.preferred.dimX.max;
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.dimY.min))
		{
			if (_common_set_ldslave.theData_in->con_params.segmentation_y < _common_set_node_params_1io.preferred.dimY.min)
			{
				thereismismatch = true;
				ld_cp.con_params.segmentation_y = _common_set_node_params_1io.preferred.dimY.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.dimY.max))
		{
			if (_common_set_ldslave.theData_in->con_params.segmentation_y > _common_set_node_params_1io.preferred.dimY.max)
			{
				thereismismatch = true;
				ld_cp.con_params.segmentation_y = _common_set_node_params_1io.preferred.dimY.max;
			}
		}

		if (_common_set_node_params_1io.preferred.subformat.min != JVX_DATAFORMAT_GROUP_NONE)
		{
			if (_common_set_ldslave.theData_in->con_params.format_group < _common_set_node_params_1io.preferred.subformat.min)
			{
				thereismismatch = true;
				ld_cp.con_params.format_group = _common_set_node_params_1io.preferred.subformat.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.subformat.max))
		{
			if (_common_set_ldslave.theData_in->con_params.format_group > _common_set_node_params_1io.preferred.subformat.max)
			{
				thereismismatch = true;
				ld_cp.con_params.format_group = _common_set_node_params_1io.preferred.subformat.max;
			}
		}
		if (thereismismatch)
		{
			resComplain = _common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &ld_cp JVX_CONNECTION_FEEDBACK_CALL_A(fdb_loc));
		}
	}

	switch (resComplain)
	{
	case JVX_NO_ERROR:

		// Accept the new settings
		CjvxNode_genpcg::node.buffersize.value = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.buffersize);
		CjvxNode_genpcg::node.samplerate.value = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.rate);
		CjvxNode_genpcg::node.numberinputchannels.value = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.number_channels);
		CjvxNode_genpcg::node.format.value = _common_set_ldslave.theData_in->con_params.format;
		CjvxNode_genpcg::node.segmentsize_x.value = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.segmentation_x);
		CjvxNode_genpcg::node.segmentsize_y.value = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.segmentation_y);
		CjvxNode_genpcg::node.subformat.value = _common_set_ldslave.theData_in->con_params.format_group;
		return _test_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
	case JVX_ERROR_COMPROMISE:

		/*
		// We might accept a compromies
		CjvxNode_genpcg::node.buffersize.value = _common_set_ldslave.theData_in->con_params.buffersize;
		CjvxNode_genpcg::node.samplerate.value = _common_set_ldslave.theData_in->con_params.rate;
		CjvxNode_genpcg::node.numberinputchannels.value = _common_set_ldslave.theData_in->con_params.number_channels;
		CjvxNode_genpcg::node.format.value = _common_set_ldslave.theData_in->con_params.format;

		return JVX_NO_ERROR;*/
		break;
	default:

		txt = __FUNCTION__;
		txt += ": ";
		txt += "When trying to negotiate parameters with connected output connector, negotiations failed, reason: ";
		txt += fdb_loc->std_str();
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt);
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxNodeBaseNio::test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(var))
{
	_common_set_ldslave.theData_out.con_params.number_channels = CjvxNode_genpcg::node.numberoutputchannels.value;
	_common_set_ldslave.theData_out.con_params.buffersize = CjvxNode_genpcg::node.buffersize.value;
	_common_set_ldslave.theData_out.con_params.rate = CjvxNode_genpcg::node.samplerate.value;
	_common_set_ldslave.theData_out.con_params.format = (jvxDataFormat)CjvxNode_genpcg::node.format.value;

	return _test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(var));
}

jvxErrorType
CjvxNodeBaseNio::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	const jvxLinkDataDescriptor* ld = (const jvxLinkDataDescriptor*)data;
	jvxBool thereismismatch = false;
	jvxErrorType res = JVX_NO_ERROR;
	std::string txt;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		if (
			(ld->con_params.rate != CjvxNode_genpcg::node.samplerate.value) ||
			(ld->con_params.buffersize != CjvxNode_genpcg::node.buffersize.value) ||
			(ld->con_params.format != CjvxNode_genpcg::node.format.value))
		{
			txt = __FUNCTION__;
			txt += ": ";
			jvxBool thereisone = false;
			if (ld->con_params.rate != CjvxNode_genpcg::node.samplerate.value)
			{
				txt += "Input and output rate do not match: ";
				txt += jvx_size2String(CjvxNode_genpcg::node.samplerate.value);
				txt += " on input side whereas ";
				txt += jvx_size2String(ld->con_params.rate);
				txt += " on output side";
				thereisone = true;
			}
			if (ld->con_params.buffersize != CjvxNode_genpcg::node.buffersize.value)
			{
				if (thereisone)
				{
					txt += ", ";
				}
				txt += ": Input and output buffersize do not match: ";
				txt += jvx_size2String(CjvxNode_genpcg::node.buffersize.value);
				txt += " on input side whereas ";
				txt += jvx_size2String(ld->con_params.buffersize);
				txt += " on output side.";
				thereisone = true;
			}
			if (ld->con_params.format != CjvxNode_genpcg::node.format.value)
			{
				if (thereisone)
				{
					txt += ", ";
				}
				txt += ": Input and output format do not match: ";
				txt += jvxDataFormat_txt(CjvxNode_genpcg::node.format.value);
				txt += " on input side whereas ";
				txt += jvxDataFormat_txt(ld->con_params.format);
				txt += " on output side";
			}
			txt += ".";
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt);
			res = JVX_ERROR_UNSUPPORTED;
		}
		else
		{
			// Accept what comes from the output side in terms of channels
			CjvxNode_genpcg::node.numberoutputchannels.value = JVX_SIZE_INT32(ld->con_params.number_channels);

			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_output_channels.min))
			{

				if (CjvxNode_genpcg::node.numberoutputchannels.value < _common_set_node_params_1io.preferred.number_output_channels.min)
				{
					thereismismatch = true;
					CjvxNode_genpcg::node.numberoutputchannels.value =
						JVX_SIZE_INT32(_common_set_node_params_1io.preferred.number_output_channels.min);
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_output_channels.max))
			{
				if (CjvxNode_genpcg::node.numberoutputchannels.value > _common_set_node_params_1io.preferred.number_output_channels.max)
				{
					thereismismatch = true;
					CjvxNode_genpcg::node.numberoutputchannels.value =
						JVX_SIZE_INT32(_common_set_node_params_1io.preferred.number_output_channels.max);
				}
			}

			// Allow overwrite of output channels
			_common_set_ldslave.theData_out.con_params.number_channels = CjvxNode_genpcg::node.numberoutputchannels.value;

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

#endif