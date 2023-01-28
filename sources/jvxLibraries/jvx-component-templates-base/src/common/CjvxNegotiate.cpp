#include "jvx.h"
#include "common/CjvxNegotiate.h"

#define SET_BUFFERSIZE_DIMX(res, str, bsize) \
	if ( \
		(str.buffersize.min != bsize) || \
		(str.buffersize.max != bsize)) \
	{ \
		res = JVX_NEGOTIATE_CONSTRAINT_CHANGE; \
		str.buffersize.min = bsize; \
		str.buffersize.max = bsize; \
		str.dimX.min = bsize; \
		str.dimX.max = bsize; \
	}

#define SET_CHANNELNUM_DIMY(res, str, numchans) \
	if ( \
		(str.number_channels.min != numchans) || \
		(str.number_channels.max != numchans) || \
		(str.dimY.min != numchans) || \
		(str.dimY.max != numchans)) \
	{ \
		res = JVX_NEGOTIATE_CONSTRAINT_CHANGE; \
		str.number_channels.min = numchans; \
		str.number_channels.max = numchans; \
		str.dimY.min = numchans; \
		str.dimY.max = numchans; \
	}

#define SET_OTHER(res, str, param, val) \
	if ( \
		(str.param.min != val) || \
		(str.param.max != val)) \
	{ \
		res = JVX_NEGOTIATE_CONSTRAINT_CHANGE; \
		str.param.min = val; \
	str.param.max = val; \
	}

#define SET_DEFAULT_PARAM_SIZE(params, preferred, varpar, varpre, defval) \
	/* 
	if (JVX_CHECK_SIZE_UNSELECTED(params->varpar)) 
	*/ \
	{ \
		if (JVX_CHECK_SIZE_SELECTED(preferred.varpre.min)) \
		{ \
			params->varpar = preferred.varpre.min; \
		} \
		else \
		{ \
			if (JVX_CHECK_SIZE_SELECTED(preferred.varpre.max)) \
			{ \
				params->varpar = preferred.varpre.max; \
			} \
			else \
			{ \
				params->varpar = defval; \
			} \
		} \
	} 

#define SET_DEFAULT_PARAM_ENUM(params, preferred, varpar, varpre, compval, defval) \
	/*
	if (params->varpar != compval) 
	*/ \
	{ \
		if (preferred.varpre.min != compval) \
		{ \
			params->varpar = preferred.varpre.min; \
		} \
		else \
		{ \
			if (preferred.varpre.max != compval) \
			{ \
				params->varpar = preferred.varpre.max; \
			} \
			else \
			{ \
				params->varpar = defval; \
			} \
		} \
	} 

CjvxNegotiate_common::CjvxNegotiate_common()
{
	//_common_set_node_params_neg.acceptOnlyExactMatchLinkDataInput = false;
	//_common_set_node_params_neg.acceptOnlyExactMatchLinkDataOutput = false;
	stack_pushed = false;
	
	// This variable has its own constructor -> _latest_results
}

void
CjvxNegotiate_common::_set_parameters_fixed(
	jvxSize num_channels,
	jvxSize bsize,
	jvxSize srate,
	jvxDataFormat format,
	jvxDataFormatGroup sub_format,
	jvxSize segment_x,
	jvxSize segment_y)
{
	jvxConstraintSetResult res = JVX_NEGOTIATE_CONSTRAINT_NO_CHANGE;
	if (coupleBsizeDimX)
	{
		SET_BUFFERSIZE_DIMX(res, preferred, bsize);
	}
	else
	{
		SET_OTHER(res, preferred, buffersize, bsize);
		SET_OTHER(res, preferred, dimX, segment_x);
	}
	if (coupleChannelsDimY)
	{
		SET_CHANNELNUM_DIMY(res, preferred, num_channels);
	}
	else
	{
		SET_OTHER(res, preferred, number_channels, num_channels);
		SET_OTHER(res, preferred, dimY, segment_y);
	}
	SET_OTHER(res, preferred, samplerate, srate);
	SET_OTHER(res, preferred, format, format);
	SET_OTHER(res, preferred, subformat, sub_format);
	SET_OTHER(res, preferred, samplerate, srate);
}

jvxConstraintSetResult
CjvxNegotiate_common::_update_parameters_fixed(
	jvxSize num_channels,
	jvxSize bsize,
	jvxSize srate,
	jvxDataFormat format,
	jvxDataFormatGroup sub_format, 
	jvxLinkDataDescriptor* datOut,
	jvxSize segment_x,
	jvxSize segment_y)
{
	jvxConstraintSetResult res = JVX_NEGOTIATE_CONSTRAINT_NO_CHANGE;
	if (coupleBsizeDimX)
	{
		if (JVX_CHECK_SIZE_SELECTED(bsize))
		{
			SET_BUFFERSIZE_DIMX(res, preferred, bsize);
			if (datOut)
			{
				datOut->con_params.buffersize = bsize;
				datOut->con_params.segmentation_x = bsize;
			}
		}
	}
	else
	{
		if (JVX_CHECK_SIZE_SELECTED(bsize))
		{
			SET_OTHER(res, preferred, buffersize, bsize);
			if (datOut)
			{
				datOut->con_params.buffersize = bsize;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(segment_x))
		{
			SET_OTHER(res, preferred, dimX, segment_x);
			if (datOut)
			{
				datOut->con_params.segmentation_x = segment_x;
			}
		}
	}

	if (coupleChannelsDimY)
	{
		if (JVX_CHECK_SIZE_SELECTED(num_channels))
		{
			SET_CHANNELNUM_DIMY(res, preferred, num_channels);
			if (datOut)
			{
				datOut->con_params.number_channels = num_channels;
				datOut->con_params.segmentation_y = num_channels;
			}
		}
	}
	else
	{
		if (JVX_CHECK_SIZE_SELECTED(num_channels))
		{
			SET_OTHER(res, preferred, number_channels, num_channels);
			if (datOut)
			{
				datOut->con_params.number_channels = num_channels;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(segment_y))
		{
			SET_OTHER(res, preferred, dimY, segment_y);
			if (datOut)
			{
				datOut->con_params.segmentation_y = segment_y;
			}
		}
	}

	if (JVX_CHECK_SIZE_SELECTED(srate))
	{
		SET_OTHER(res, preferred, samplerate, srate);
		if (datOut)
			datOut->con_params.rate = srate;
	}
	if (format != JVX_DATAFORMAT_NONE)
	{
		SET_OTHER(res, preferred, format, format);
		if (datOut)
			datOut->con_params.format = format;

	}
	if (sub_format != JVX_DATAFORMAT_GROUP_NONE)
	{
		SET_OTHER(res, preferred, subformat, sub_format);
		if (datOut)
			datOut->con_params.format_group = sub_format;
	}	

	return res;
}

jvxConstraintSetResult
CjvxNegotiate_common::_clear_parameters_fixed(
	jvxBool numberchannelsset,
	jvxBool bsizeset,
	jvxBool srateset,
	jvxBool formatset,
	jvxBool sub_formatset,
	jvxBool segsizex,
	jvxBool segsizey)
{
	jvxConstraintSetResult res = JVX_NEGOTIATE_CONSTRAINT_NO_CHANGE;
	if (bsizeset)
	{
		if (coupleBsizeDimX)
		{
			SET_BUFFERSIZE_DIMX(res, preferred,  JVX_SIZE_UNSELECTED);
		}
		else
		{
			SET_OTHER(res, preferred, buffersize, JVX_SIZE_UNSELECTED);
			if (segsizex)
			{
				SET_OTHER(res, preferred, dimX, JVX_SIZE_UNSELECTED);
			}
		}
	}
	
	if (numberchannelsset)
	{
		if (coupleChannelsDimY)
		{
			SET_CHANNELNUM_DIMY(res, preferred, JVX_SIZE_UNSELECTED);
		}
		else
		{
			SET_OTHER(res, preferred, number_channels, JVX_SIZE_UNSELECTED);
			if (segsizey)
			{
				SET_OTHER(res, preferred, dimY, JVX_SIZE_UNSELECTED);
			}
		}
	}
	if (srateset)
	{
		SET_OTHER(res, preferred, samplerate, JVX_SIZE_UNSELECTED);
	}
	if (formatset)
	{
		SET_OTHER(res, preferred, format, JVX_DATAFORMAT_NONE);
	}
	if (sub_formatset)
	{
		SET_OTHER(res, preferred, subformat, JVX_DATAFORMAT_GROUP_NONE);
	}
	
	return res;
}

// ================================================================================

jvxErrorType 
CjvxNegotiate_common::derive(jvxLinkDataDescriptor_con_params* params)
{
	SET_DEFAULT_PARAM_SIZE(params, preferred, buffersize, buffersize, 1024);
	params->segmentation_x = params->buffersize;
	params->segmentation_y = 1;
	
	SET_DEFAULT_PARAM_SIZE(params, preferred, rate, samplerate, 48000);
	SET_DEFAULT_PARAM_SIZE(params, preferred, number_channels, number_channels, 1);

	SET_DEFAULT_PARAM_ENUM(params, preferred, format, format, JVX_DATAFORMAT_NONE, JVX_DATAFORMAT_DATA);
	SET_DEFAULT_PARAM_ENUM(params, preferred, format_group, subformat, JVX_DATAFORMAT_GROUP_NONE, JVX_DATAFORMAT_GROUP_AUDIO_PCM_INTERLEAVED);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxNegotiate_common::_push_constraints()
{
	if (!stack_pushed)
	{
		stack_preferred = preferred;
		stack_pushed = false;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxNegotiate_common::_pop_constraints()
{
	if (stack_pushed)
	{
		preferred = stack_preferred;
		stack_pushed = false;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxBool
CjvxNegotiate_common::_stack_pushed()
{
	return stack_pushed;
}

// ====================================================================================

CjvxNegotiate_input::CjvxNegotiate_input()
{
}

jvxErrorType 
CjvxNegotiate_input::_negotiate_connect_icon(jvxLinkDataDescriptor* theData_in,
	IjvxObject* this_pointer,
	const char*descror
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;
	jvxLinkDataDescriptor ld_cp;
	jvxBool thereismismatch = false;
	jvxSize num = allowedSetup.size();
	jvxErrorType resComplain = JVX_NO_ERROR;
	std::string txt;
	std::string reason;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, this_pointer,
		descror,
		"Entering CjvxNegotiate default input connector");

	_latest_results = theData_in->con_params;

	if (num)
	{
		thereismismatch = true;
		for (i = 0; i < num; i++)
		{
			if (
				(allowedSetup[i].buffersize == theData_in->con_params.buffersize) &&
				(allowedSetup[i].samplerate == theData_in->con_params.rate) &&
				(allowedSetup[i].format == theData_in->con_params.format) &&
				(allowedSetup[i].subformat == theData_in->con_params.format_group) &&
				(allowedSetup[i].dimX == theData_in->con_params.segmentation_x) &&
				(allowedSetup[i].dimY == theData_in->con_params.segmentation_y) &&
				(allowedSetup[i].number_input_channels == theData_in->con_params.number_channels))
			{
				thereismismatch = false;
				break;
			}
		}
		if (thereismismatch)
		{
			ld_cp.con_params.buffersize = allowedSetup[i].buffersize;
			ld_cp.con_params.rate = allowedSetup[i].samplerate;
			ld_cp.con_params.format = allowedSetup[i].format;
			ld_cp.con_params.number_channels = allowedSetup[i].number_input_channels;
			ld_cp.con_params.format_group = allowedSetup[i].subformat;
			ld_cp.con_params.segmentation_x = allowedSetup[i].dimX;
			ld_cp.con_params.segmentation_y = allowedSetup[i].dimY;

			resComplain = theData_in->con_link.connect_from->transfer_backward_ocon(
				JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &ld_cp JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		ld_cp.con_params.buffersize = theData_in->con_params.buffersize;
		ld_cp.con_params.rate = theData_in->con_params.rate;
		ld_cp.con_params.format = theData_in->con_params.format;
		ld_cp.con_params.number_channels = theData_in->con_params.number_channels;
		ld_cp.con_params.format_group = theData_in->con_params.format_group;
		ld_cp.con_params.segmentation_x = theData_in->con_params.segmentation_x;
		ld_cp.con_params.segmentation_y = theData_in->con_params.segmentation_y;		

		// ================================================================================
		// Buffersize
		// ================================================================================
		if (JVX_CHECK_SIZE_SELECTED(preferred.buffersize.min))
		{
			if (theData_in->con_params.buffersize < preferred.buffersize.min)
			{
				thereismismatch = true;
				ld_cp.con_params.buffersize = preferred.buffersize.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.buffersize.max))
		{
			if (theData_in->con_params.buffersize > preferred.buffersize.max)
			{
				thereismismatch = true;
				ld_cp.con_params.buffersize = preferred.buffersize.max;
			}
		}

		// ================================================================================
		// Samplerate
		// ================================================================================
		if (JVX_CHECK_SIZE_SELECTED(preferred.samplerate.min))
		{
			if (theData_in->con_params.rate < preferred.samplerate.min)
			{
				thereismismatch = true;
				ld_cp.con_params.rate = preferred.samplerate.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.samplerate.max))
		{
			if (theData_in->con_params.rate > preferred.samplerate.max)
			{
				thereismismatch = true;
				ld_cp.con_params.rate = preferred.samplerate.max;
			}
		}

		// ================================================================================
		// Number input channels
		// ================================================================================
		if (JVX_CHECK_SIZE_SELECTED(preferred.number_channels.min))
		{
			if (theData_in->con_params.number_channels < preferred.number_channels.min)
			{
				thereismismatch = true;
				ld_cp.con_params.number_channels = preferred.number_channels.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.number_channels.max))
		{
			if (theData_in->con_params.number_channels > preferred.number_channels.max)
			{
				thereismismatch = true;
				ld_cp.con_params.number_channels = preferred.number_channels.max;
			}
		}

		// ================================================================================
		// Format
		// ================================================================================
		if (preferred.format.min != JVX_DATAFORMAT_NONE)
		{
			if (theData_in->con_params.format < preferred.format.min)
			{
				thereismismatch = true;
				ld_cp.con_params.format = preferred.format.min;
			}
		}
		if (preferred.format.max != JVX_DATAFORMAT_NONE)
		{
			if (theData_in->con_params.format > preferred.format.max)
			{
				thereismismatch = true;
				ld_cp.con_params.format = preferred.format.max;
			}
		}

		// ================================================================================
		// Subformat
		// ================================================================================

		if (preferred.subformat.min != JVX_DATAFORMAT_GROUP_NONE)
		{
			if (theData_in->con_params.format_group < preferred.subformat.min)
			{
				thereismismatch = true;
				ld_cp.con_params.format_group = preferred.subformat.min;
			}
		}
		if (preferred.subformat.max != JVX_DATAFORMAT_GROUP_NONE)
		{
			if (theData_in->con_params.format_group > preferred.subformat.max)
			{
				thereismismatch = true;
				ld_cp.con_params.format_group = preferred.subformat.max;
			}
		}

		// ================================================================================
		// DimX
		// ================================================================================
		jvxSize cpX = theData_in->con_params.segmentation_x;
		if (JVX_CHECK_SIZE_UNSELECTED(cpX))
		{
			cpX = theData_in->con_params.buffersize;
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.dimX.min))
		{
			
			if (cpX < preferred.dimX.min)
			{
				thereismismatch = true;
				ld_cp.con_params.segmentation_x = preferred.dimX.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.dimX.max))
		{
			if (cpX > preferred.dimX.max)
			{
				thereismismatch = true;
				ld_cp.con_params.segmentation_x = preferred.dimX.max;
			}
		}

		// ================================================================================
		// DimY
		// ================================================================================
		jvxSize cpY = theData_in->con_params.segmentation_y;
		if (JVX_CHECK_SIZE_UNSELECTED(cpY))
		{
			cpY = theData_in->con_params.number_channels;
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.dimY.min))
		{
			if (cpY < preferred.dimY.min)
			{
				thereismismatch = true;
				ld_cp.con_params.segmentation_y = preferred.dimY.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.dimY.max))
		{
			if (cpY > preferred.dimY.max)
			{
				thereismismatch = true;
				ld_cp.con_params.segmentation_y = preferred.dimY.max;
			}
		}

		// ===================================================================================
		if (thereismismatch)
		{
			resComplain = theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS,
				&ld_cp JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

			if (resComplain == JVX_ERROR_REQUEST_CALL_AGAIN)
			{
				return resComplain;
			}

			// The previous modue may have modified the parameters according to my wish or may have not 
			// We need to make sure the return value was right.
			// This is only due to modules that do not act as specified at the moment :-(
			if (ld_cp.con_params.buffersize != theData_in->con_params.buffersize)
			{
				reason = "A buffersize of <";
				reason += jvx_size2String(ld_cp.con_params.buffersize);
				reason += "> was desired but the input connector only delivers <";
				reason += jvx_size2String(theData_in->con_params.buffersize);
				reason += ">.";

				resComplain = JVX_ERROR_COMPROMISE;
			}
			else if (ld_cp.con_params.rate != theData_in->con_params.rate)
			{
				reason = "A samplerate of <";
				reason += jvx_size2String(ld_cp.con_params.rate);
				reason += "> was desired but the input connector only delivers <";
				reason += jvx_size2String(theData_in->con_params.rate);
				reason += ">.";
				resComplain = JVX_ERROR_COMPROMISE;
			}
			else if (ld_cp.con_params.format != theData_in->con_params.format)
			{
				reason = "A format of <";
				reason += jvxDataFormatGroup_txt(ld_cp.con_params.format);
				reason += "> was desired but the input connector only delivers <";
				reason += jvxDataFormatGroup_txt(theData_in->con_params.format);
				reason += ">.";
				resComplain = JVX_ERROR_COMPROMISE;
			}
			else if (ld_cp.con_params.format_group != theData_in->con_params.format_group)
			{
				reason = "A subformat of <";
				reason += jvxDataFormatGroup_txt(ld_cp.con_params.format_group);
				reason += "> was desired but the input connector only delivers <";
				reason += jvxDataFormatGroup_txt(theData_in->con_params.format_group);
				reason += ">.";
				resComplain = JVX_ERROR_COMPROMISE;
			}
			else if (ld_cp.con_params.segmentation_x != theData_in->con_params.segmentation_x)
			{
				reason = "A segmentation_x of <";
				reason += jvx_size2String(ld_cp.con_params.segmentation_x);
				reason += "> was desired but the input connector only delivers <";
				reason += jvx_size2String(theData_in->con_params.segmentation_x);
				reason += ">.";
				resComplain = JVX_ERROR_COMPROMISE;
			}
			else if (ld_cp.con_params.segmentation_y != theData_in->con_params.segmentation_y)
			{
				reason = "A segmentation_y of <";
				reason += jvx_size2String(ld_cp.con_params.segmentation_y);
				reason += "> was desired but the input connector only delivers <";
				reason += jvx_size2String(theData_in->con_params.segmentation_y);
				reason += ">.";
				resComplain = JVX_ERROR_COMPROMISE;
			}
		}
	}

	switch (resComplain)
	{
	case JVX_NO_ERROR:

		// Update parameters
		_latest_results = theData_in->con_params;

		return JVX_NO_ERROR;
	case JVX_ERROR_COMPROMISE:

		// Could not find compromise
		JVX_CONNECTION_FEEDBACK_ON_LEAVE_ERROR(fdb, resComplain,
			("Previous connector did not accept current setting, reason: " + reason).c_str());
		break;

	case JVX_ERROR_POSTPONE:
	case JVX_ERROR_REQUEST_CALL_AGAIN:
		// Something has changed in the component but it requires to restart a test
		return resComplain;
		break;
	default:

		JVX_CONNECTION_FEEDBACK_ON_LEAVE_ERROR(fdb, resComplain,
			"Previous connector did not accept current setting.");

		txt = __FUNCTION__;
		txt += ": ";
		txt += "When trying to negotiate parameters with connected output connector, negotiations failed.";
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), resComplain);
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

// =================================================================================

CjvxNegotiate_output::CjvxNegotiate_output()
{
}

jvxErrorType
CjvxNegotiate_output::_negotiate_transfer_backward_ocon(
	jvxLinkDataDescriptor* ld,
	jvxLinkDataDescriptor* dataOut,
	IjvxObject* this_pointer,
	jvxCBitField* modFlags
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	std::string txt;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool thereismismatch = false;

#if 0
	if (_negotiate_accept_input_only && (!master_mode))
	{
		if (
			(ld->con_params.rate != _params_input_on_connect_icon.rate) ||
			(ld->con_params.buffersize != _params_input_on_connect_icon.buffersize) ||
			(ld->con_params.format != _params_input_on_connect_icon.format) ||
			(ld->con_params.format_group != _params_input_on_connect_icon.subformat) ||
			(ld->con_params.segmentation_x != _params_input_on_connect_icon.segmentation_x) ||
			(ld->con_params.segmentation_y != _params_input_on_connect_icon.segmentation_y))
		{
			txt = __FUNCTION__;
			txt += ": ";
			jvxBool thereisone = false;
			if (ld->con_params.rate != _params_input_on_connect_icon.rate)
			{
				txt += "Input and output rate do not match: ";
				txt += jvx_size2String(_params_input_on_connect_icon.rate);
				txt += " on input side whereas ";
				txt += jvx_size2String(ld->con_params.rate);
				txt += " on output side";
				thereisone = true;
			}
			if (ld->con_params.buffersize != _params_input_on_connect_icon.buffersize)
			{
				if (thereisone)
				{
					txt += ", ";
				}
				txt += ": Input and output buffersize do not match: ";
				txt += jvx_size2String(_params_input_on_connect_icon.buffersize);
				txt += " on input side whereas ";
				txt += jvx_size2String(ld->con_params.buffersize);
				txt += " on output side.";
				thereisone = true;
			}
			if (ld->con_params.format != _params_input_on_connect_icon.format)
			{
				if (thereisone)
				{
					txt += ", ";
				}
				txt += ": Input and output format do not match: ";
				txt += jvxDataFormat_txt(_params_input_on_connect_icon.format);
				txt += " on input side whereas ";
				txt += jvxDataFormat_txt(ld->con_params.format);
				txt += " on output side";
			}
			if (ld->con_params.format_group != _params_input_on_connect_icon.subformat)
			{
				if (thereisone)
				{
					txt += ", ";
				}
				txt += ": Input and output format do not match: ";
				txt += jvxDataFormatGroup_txt(_params_input_on_connect_icon.subformat);
				txt += " on input side whereas ";
				txt += jvxDataFormatGroup_txt(ld->con_params.format_group);
				txt += " on output side";
			}
			if (ld->con_params.segmentation_x != _params_input_on_connect_icon.segmentation_x)
			{
				if (thereisone)
				{
					txt += ", ";
				}
				txt += ": Input and output segmentation_x do not match: ";
				txt += jvx_size2String(_params_input_on_connect_icon.segmentation_x);
				txt += " on input side whereas ";
				txt += jvx_size2String(ld->con_params.segmentation_x);
				txt += " on output side.";
				thereisone = true;
			}
			if (ld->con_params.segmentation_y != _params_input_on_connect_icon.segmentation_y)
			{
				if (thereisone)
				{
					txt += ", ";
				}
				txt += ": Input and output segmentation_y do not match: ";
				txt += jvx_size2String(_params_input_on_connect_icon.segmentation_y);
				txt += " on input side whereas ";
				txt += jvx_size2String(ld->con_params.segmentation_y);
				txt += " on output side.";
				thereisone = true;
			}
			txt += ".";
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), JVX_ERROR_INVALID_SETTING);
			res = JVX_ERROR_UNSUPPORTED;
		}
	}
	else
	{
#endif
	// ================================================================================
	// Number output channels
	// ================================================================================
	if (JVX_CHECK_SIZE_SELECTED(preferred.number_channels.min))
	{
		if (ld->con_params.number_channels < preferred.number_channels.min)
		{
			thereismismatch = true;
			ld->con_params.number_channels = preferred.number_channels.min;
		}
	}
	if (JVX_CHECK_SIZE_SELECTED(preferred.number_channels.max))
	{
		if (ld->con_params.number_channels > preferred.number_channels.max)
		{
			thereismismatch = true;
			ld->con_params.number_channels = preferred.number_channels.max;
		}
	}

	// ================================================================================
	// Buffersize
	// ================================================================================

	if (JVX_CHECK_SIZE_SELECTED(preferred.buffersize.min))
	{
		if (ld->con_params.buffersize < preferred.buffersize.min)
		{
			thereismismatch = true;
			ld->con_params.buffersize = preferred.buffersize.min;
		}
	}
	if (JVX_CHECK_SIZE_SELECTED(preferred.buffersize.max))
	{
		if (ld->con_params.buffersize > preferred.buffersize.max)
		{
			thereismismatch = true;
			ld->con_params.buffersize = preferred.buffersize.max;
		}
	}

	// ================================================================================
	// Samplerate
	// ================================================================================
	if (JVX_CHECK_SIZE_SELECTED(preferred.samplerate.min))
	{
		if (ld->con_params.rate < preferred.samplerate.min)
		{
			thereismismatch = true;
			ld->con_params.rate = preferred.samplerate.min;
		}
	}
	if (JVX_CHECK_SIZE_SELECTED(preferred.samplerate.max))
	{
		if (ld->con_params.rate > preferred.samplerate.max)
		{
			thereismismatch = true;
			ld->con_params.rate = preferred.samplerate.max;
		}
	}

	// ================================================================================
	// Format
	// ================================================================================
	if (preferred.format.min != JVX_DATAFORMAT_NONE)
	{
		if (ld->con_params.format < preferred.format.min)
		{
			thereismismatch = true;
			ld->con_params.format = preferred.format.min;
		}
	}
	if (preferred.format.max != JVX_DATAFORMAT_NONE)
	{
		if (ld->con_params.format > preferred.format.max)
		{
			thereismismatch = true;
			ld->con_params.format = preferred.format.max;
		}
	}

	// ================================================================================
	// Subformat
	// ================================================================================

	if (preferred.subformat.min != JVX_DATAFORMAT_GROUP_NONE)
	{
		if (ld->con_params.format_group < preferred.subformat.min)
		{
			thereismismatch = true;
			ld->con_params.format_group = preferred.subformat.min;
		}
	}
	if (preferred.subformat.max != JVX_DATAFORMAT_GROUP_NONE)
	{
		if (ld->con_params.format_group > preferred.subformat.max)
		{
			thereismismatch = true;
			ld->con_params.format_group = preferred.subformat.max;
		}
	}

	// ================================================================================
	// DimX
	// ================================================================================
	if (JVX_CHECK_SIZE_UNSELECTED(ld->con_params.segmentation_x))
	{
		if (JVX_CHECK_SIZE_SELECTED(preferred.dimX.min))
		{
			if (ld->con_params.segmentation_x < preferred.dimX.min)
			{
				thereismismatch = true;
				ld->con_params.segmentation_x = preferred.dimX.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.dimX.max))
		{
			if (ld->con_params.segmentation_x > preferred.dimX.max)
			{
				thereismismatch = true;
				ld->con_params.segmentation_x = preferred.dimX.max;
			}
		}
	}

	// ================================================================================
	// DimY
	// ================================================================================
	if (JVX_CHECK_SIZE_UNSELECTED(ld->con_params.segmentation_y))
	{
		if (JVX_CHECK_SIZE_SELECTED(preferred.dimY.min))
		{
			if (ld->con_params.segmentation_y < preferred.dimY.min)
			{
				thereismismatch = true;
				ld->con_params.segmentation_y = preferred.dimY.min;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.dimY.max))
		{
			if (ld->con_params.segmentation_y > preferred.dimY.max)
			{
				thereismismatch = true;
				ld->con_params.segmentation_y = preferred.dimY.max;
			}
		}
	}

	// ===================================================================================
	if (thereismismatch)
	{
		res = JVX_ERROR_COMPROMISE;
	}
	else
	{
		res = JVX_NO_ERROR;
	}

	if (modFlags)
	{
		*modFlags = 0;
		if (ld->con_params.buffersize != dataOut->con_params.buffersize)
		{
			jvx_bitSet(*modFlags, JVX_MODIFICATION_BUFFERSIZE_SHIFT);
		}
		if (ld->con_params.rate != dataOut->con_params.rate)
		{
			jvx_bitSet(*modFlags, JVX_MODIFICATION_SAMPLERATE_SHIFT);
		}
		if (ld->con_params.number_channels != dataOut->con_params.number_channels)
		{
			jvx_bitSet(*modFlags, JVX_MODIFICATION_NUM_CHANNELS_SHIFT);
		}
		if (ld->con_params.format != dataOut->con_params.format)
		{
			jvx_bitSet(*modFlags, JVX_MODIFICATION_FORMAT_SHIFT);
		}
		if (ld->con_params.format_group != dataOut->con_params.format_group)
		{
			jvx_bitSet(*modFlags, JVX_MODIFICATION_SUBFORMAT_SHIFT);
		}
		if (ld->con_params.segmentation_x != dataOut->con_params.segmentation_x)
		{
			jvx_bitSet(*modFlags, JVX_MODIFICATION_SEGX_SHIFT);
		}
		if (ld->con_params.segmentation_y != dataOut->con_params.segmentation_y)
		{
			jvx_bitSet(*modFlags, JVX_MODIFICATION_SEGY_SHIFT);
		}
	}

	switch (res)
	{
	case JVX_ERROR_UNSUPPORTED:
		// No chance
		break;
	case JVX_NO_ERROR:			// Modifications in propsal have not been made. This may also be a modification in dataOut
	case JVX_ERROR_COMPROMISE:	// Modifications in propsal has been made. This will also be a modification in dataOut
		// No modification needed to be done, just use the propsed settings
		if(modFlags == NULL)
		{
			dataOut->con_params = ld->con_params;
		}
		break;
	}

	_latest_results = dataOut->con_params;

	return res;
}


