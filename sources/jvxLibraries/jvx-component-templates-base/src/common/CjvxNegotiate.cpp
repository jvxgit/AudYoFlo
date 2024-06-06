#include "jvx.h"
#include "common/CjvxNegotiate.h"
#include "common/CjvxObjectLog.h"

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
	jvxDataflow datflow,
	jvxSize segment_x,
	jvxSize segment_y)
{
	jvxConstraintSetResult res = JVX_NEGOTIATE_CONSTRAINT_NO_CHANGE;
	if (negBehavior == negBehaviorType::JVX_BEHAVIOR_AUDIO)
	{
		SET_BUFFERSIZE_DIMX(res, preferred, bsize);
	}
	else
	{
		SET_OTHER(res, preferred, buffersize, bsize);
		SET_OTHER(res, preferred, dimX, segment_x);
	}
	if (negBehavior == negBehaviorType::JVX_BEHAVIOR_AUDIO)
	{
		SET_OTHER(res, preferred, number_channels, num_channels);
		SET_OTHER(res, preferred, dimY, 1);
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
	SET_OTHER(res, preferred, data_flow, datflow);
}

jvxConstraintSetResult
CjvxNegotiate_common::_update_parameters_fixed(
	jvxSize num_channels,
	jvxSize bsize,
	jvxSize srate,
	jvxDataFormat format,
	jvxDataFormatGroup sub_format, 
	jvxDataflow datflow,
	jvxLinkDataDescriptor* datOut,
	jvxSize segment_x,
	jvxSize segment_y)
{
	jvxConstraintSetResult res = JVX_NEGOTIATE_CONSTRAINT_NO_CHANGE;
	if (negBehavior == negBehaviorType::JVX_BEHAVIOR_AUDIO)
	{
		if (JVX_CHECK_SIZE_SELECTED(bsize))
		{
			SET_BUFFERSIZE_DIMX(res, preferred, bsize);
			if (datOut)
			{
				datOut->con_params.buffersize = bsize;
				datOut->con_params.segmentation.x = bsize;
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
				datOut->con_params.segmentation.x = segment_x;
			}
		}
	}

	if (JVX_CHECK_SIZE_SELECTED(num_channels))
	{
		SET_OTHER(res, preferred, number_channels, num_channels);
		if (datOut)
		{
			datOut->con_params.number_channels = num_channels;
		}
	}
	if (negBehavior == negBehaviorType::JVX_BEHAVIOR_AUDIO)
	{
		SET_OTHER(res, preferred, dimY, 1);
	}
	else
	{
		if (JVX_CHECK_SIZE_SELECTED(segment_y))
		{
			SET_OTHER(res, preferred, dimY, segment_y);
			if (datOut)
			{
				datOut->con_params.segmentation.y = segment_y;
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

	if (datflow != jvxDataflow::JVX_DATAFLOW_DONT_CARE)
	{
		SET_OTHER(res, preferred, data_flow, datflow);
		if (datOut)
			datOut->con_params.data_flow = datflow;
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
	jvxBool segsizey,
	jvxBool datflow)
{
	jvxConstraintSetResult res = JVX_NEGOTIATE_CONSTRAINT_NO_CHANGE;
	if (bsizeset)
	{
		if (negBehavior == negBehaviorType::JVX_BEHAVIOR_AUDIO)
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
	
	if (negBehavior == negBehaviorType::JVX_BEHAVIOR_AUDIO)
	{
		if (numberchannelsset)
		{
			SET_OTHER(res, preferred, number_channels, JVX_SIZE_UNSELECTED);
		}
	}
	else
	{
		if (numberchannelsset)
		{
			SET_OTHER(res, preferred, number_channels, JVX_SIZE_UNSELECTED);
		}

		if (segsizey)
		{
			SET_OTHER(res, preferred, dimY, JVX_SIZE_UNSELECTED);
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
	if (datflow)
	{
		SET_OTHER(res, preferred, data_flow, jvxDataflow::JVX_DATAFLOW_DONT_CARE);
	}
	
	return res;
}

// ================================================================================

jvxErrorType 
CjvxNegotiate_common::derive(jvxLinkDataDescriptor_con_params* params)
{
	SET_DEFAULT_PARAM_SIZE(params, preferred, buffersize, buffersize, 1024);
	params->segmentation.x = params->buffersize;
	params->segmentation.y = 1;
	
	SET_DEFAULT_PARAM_SIZE(params, preferred, rate, samplerate, 48000);
	SET_DEFAULT_PARAM_SIZE(params, preferred, number_channels, number_channels, 1);

	SET_DEFAULT_PARAM_ENUM(params, preferred, format, format, JVX_DATAFORMAT_NONE, JVX_DATAFORMAT_DATA);
	SET_DEFAULT_PARAM_ENUM(params, preferred, format_group, subformat, JVX_DATAFORMAT_GROUP_NONE, JVX_DATAFORMAT_GROUP_AUDIO_PCM_INTERLEAVED);
	
	SET_DEFAULT_PARAM_ENUM(params, preferred, data_flow, data_flow, jvxDataflow::JVX_DATAFLOW_DONT_CARE,
		jvxDataflow::JVX_DATAFLOW_PUSH_ACTIVE);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxNegotiate_common::_push_constraints()
{
	if (!stack_pushed)
	{
		stack_preferred = preferred;
		stack_pushed = true;
		return JVX_NO_ERROR;
	}

	// I changed this condition from fatal to non-fatal: under special conditions, a push an another push may occur.
	// It happens if the test run returns "success" but there is a request to change from the end of the chain lateron.
	// This is possible if the component is part of a fixed-connection: Then, the fixed-connection is tested clear but
	// a request is generated from towards the end after the fixed-connection. It is not too bad to accept this condition I think but
	// a warning should be generated in the verbose output. This kind of warning should not happen at every test but at the first test where 
	// a modification request is accepted.
	JVX_START_LOCK_LOG_REF(logObj, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT)
	log << __FUNCTION__ << "Warning: stack has already been pushed but not popped afterwards." << std::endl;
	JVX_STOP_LOCK_LOG_REF(logObj)

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

	// I changed this condition from fatal to non-fatal: under special conditions, a push an another push may occur.
	// It happens if the test run returns "success" but there is a request to change from the end of the chain lateron.
	// This is possible if the component is part of a fixed-connection: Then, the fixed-connection is tested clear but
	// a request is generated from towards the end after the fixed-connection. It is not too bad to accept this condition I think but
	// a warning should be generated in the verbose output. This kind of warning should not happen at every test but at the first test where 
	// a modification request is accepted.
	JVX_START_LOCK_LOG_REF(logObj, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT)
	log << __FUNCTION__ << "Warning: stack has not been pushed before pop." << std::endl;
	JVX_STOP_LOCK_LOG_REF(logObj)

	return JVX_ERROR_WRONG_STATE;
}

jvxBool
CjvxNegotiate_common::_stack_pushed()
{
	return stack_pushed;
}

jvxErrorType
CjvxNegotiate_common::_negotiate_transfer_backward_ocon(
	jvxLinkDataDescriptor* ld,
	jvxLinkDataDescriptor* dataOut,
	IjvxObject* this_pointer,
	jvxCBitField* modFlagsRet
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	std::string txt;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool thereismismatch = false;
	jvxCBitField modFlagsLoc = 0;
	jvxLinkDataDescriptor ld_fixed = *ld;

	res = jvx_check_valid(ld->con_params, txt);
	if (res != JVX_NO_ERROR)
	{
		JVX_CONNECTION_FEEDBACK_ON_LEAVE_ERROR(fdb, res,
			("Invalid connector parameter on backward complain, reason in detail: " + txt).c_str());
		return res;
	}

	// compare_core(ld, ld, thereismismatch);
	compare_core(ld, &ld_fixed, thereismismatch); // Test this: the passed struct should NOT be modified

	if (this->negBehavior == negBehaviorType::JVX_BEHAVIOR_AUDIO)
	{
		if ((ld->con_params.segmentation.y != 1) ||
			(ld->con_params.segmentation.x != ld->con_params.buffersize))
		{
			std::cout << __FUNCTION__ << ": " << __LINE__ << "Warning: Setup of the segmentation is untypical for audio. Expected would be:" << std::endl;
			std::cout << " Seg X: " << ld->con_params.segmentation.x << " vs " << ld->con_params.buffersize << "." << std::endl;
			std::cout << " Seg Y: " << ld->con_params.segmentation.y << " vs 1." << std::endl;
		}
	}
	else
	{
		if ((ld->con_params.segmentation.y * ld->con_params.segmentation.x) != ld->con_params.buffersize)
		{
			std::cout << __FUNCTION__ << ": " << __LINE__ << "Warning: Setup of the segmentation is not correct. Expected would be:" << std::endl;
			std::cout << " Seg X x Seg Y = " << ld->con_params.segmentation.x << " X " <<
				ld->con_params.segmentation.x << " = " << ld->con_params.buffersize << "." << std::endl;
		}

	}

	// ===================================================================================
	if (thereismismatch)
	{
		res = JVX_ERROR_UNSUPPORTED;

		// We may accept the new parameters and modify them to return a compromise
		if (allowCompromiseOutput)
		{
			res = JVX_ERROR_COMPROMISE;
		}
	}
	else
	{
		res = JVX_NO_ERROR;
	}

	if (ld->con_params.buffersize != dataOut->con_params.buffersize)
	{
		jvx_bitSet(modFlagsLoc, (jvxCBitField)jvxAddressLinkDataEntry::JVX_ADDRESS_BUFFERSIZE_SHIFT);
	}
	if (ld->con_params.rate != dataOut->con_params.rate)
	{
		jvx_bitSet(modFlagsLoc, (jvxCBitField)jvxAddressLinkDataEntry::JVX_ADDRESS_SAMPLERATE_SHIFT);
	}
	if (ld->con_params.number_channels != dataOut->con_params.number_channels)
	{
		jvx_bitSet(modFlagsLoc, (jvxCBitField)jvxAddressLinkDataEntry::JVX_ADDRESS_NUM_CHANNELS_SHIFT);
	}
	if (ld->con_params.format != dataOut->con_params.format)
	{
		jvx_bitSet(modFlagsLoc, (jvxCBitField)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMAT_SHIFT);
	}
	if (ld->con_params.format_group != dataOut->con_params.format_group)
	{
		jvx_bitSet(modFlagsLoc, (jvxCBitField)jvxAddressLinkDataEntry::JVX_ADDRESS_SUBFORMAT_SHIFT);
	}
	if (ld->con_params.segmentation.x != dataOut->con_params.segmentation.x)
	{
		jvx_bitSet(modFlagsLoc, (jvxCBitField)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGX_SHIFT);
	}
	if (ld->con_params.segmentation.y != dataOut->con_params.segmentation.y)
	{
		jvx_bitSet(modFlagsLoc, (jvxCBitField)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGY_SHIFT);
	}
	if (ld->con_params.data_flow != dataOut->con_params.data_flow)
	{
		jvx_bitSet(modFlagsLoc, (jvxCBitField)jvxAddressLinkDataEntry::JVX_ADDRESS_DATAFLOW_SHIFT);
	}
	
	// Prepare for return
	if (modFlagsRet)
		*modFlagsRet = modFlagsLoc;

	switch (res)
	{
	case JVX_ERROR_UNSUPPORTED:
		// No chance to negotiate
		break;
	case JVX_NO_ERROR:			// Modifications in propsal have not been made. This may also be a modification in dataOut
	case JVX_ERROR_COMPROMISE:	// Modifications in propsal has been made. This will also be a modification in dataOut
		
		// In case of a compromise as well as in the case that the passed option is taken, copy new parameters to output arguments
		// This code was fixed 02.Aug 2023 - HK
		dataOut->con_params = ld_fixed.con_params;
		break;
	}

	_latest_results = dataOut->con_params;

	return res;
}

void
CjvxNegotiate_common::compare_core(jvxLinkDataDescriptor* ld_cp, jvxLinkDataDescriptor* ld_fix, jvxBool& thereismismatch)
{
	thereismismatch = false;

	// In the first part, check the data connection main topics.
	
	// ================================================================================
	// Dataflow
	// ================================================================================

	if (preferred.data_flow.min != jvxDataflow::JVX_DATAFLOW_DONT_CARE)
	{
		if (ld_cp->con_params.data_flow < preferred.data_flow.min)
		{
			thereismismatch = true;
			if (ld_fix)
			{
				ld_fix->con_params.data_flow = preferred.data_flow.min;
			}
		}
	}
	if (preferred.data_flow.max != jvxDataflow::JVX_DATAFLOW_DONT_CARE)
	{
		if (ld_cp->con_params.data_flow > preferred.data_flow.max)
		{
			thereismismatch = true;
			if (ld_fix)
			{
				ld_fix->con_params.data_flow = preferred.data_flow.max;
			}
		}
	}

	// ================================================================================
	// Subformat
	// ================================================================================

	if (preferred.subformat.min != JVX_DATAFORMAT_GROUP_NONE)
	{
		if (ld_cp->con_params.format_group < preferred.subformat.min)
		{
			thereismismatch = true;
			if (ld_fix)
			{
				ld_fix->con_params.format_group = preferred.subformat.min;
			}
		}
	}
	if (preferred.subformat.max != JVX_DATAFORMAT_GROUP_NONE)
	{
		if (ld_cp->con_params.format_group > preferred.subformat.max)
		{
			thereismismatch = true;
			if (ld_fix)
			{
				ld_fix->con_params.format_group = preferred.subformat.max;
			}
		}
	}

	// In the second part, check the data connection topics for "real" data
	
	// All other parameters are not relevant from here on if trigger only
	if (ld_cp->con_params.format_group != JVX_DATAFORMAT_GROUP_TRIGGER_ONLY)
	{
		// ================================================================================
		// Number output channels
		// ================================================================================
		if (JVX_CHECK_SIZE_SELECTED(preferred.number_channels.min))
		{
			if (ld_cp->con_params.number_channels < preferred.number_channels.min)
			{
				thereismismatch = true;
				if (ld_fix)
				{
					ld_fix->con_params.number_channels = preferred.number_channels.min;
				}
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.number_channels.max))
		{
			if (ld_cp->con_params.number_channels > preferred.number_channels.max)
			{
				thereismismatch = true;
				if (ld_fix)
				{
					ld_fix->con_params.number_channels = preferred.number_channels.max;
				}
			}
		}

		// ================================================================================
		// Buffersize
		// ================================================================================

		if (JVX_CHECK_SIZE_SELECTED(preferred.buffersize.min))
		{
			if (ld_cp->con_params.buffersize < preferred.buffersize.min)
			{
				thereismismatch = true;
				if (ld_fix)
				{
					ld_fix->con_params.buffersize = preferred.buffersize.min;
				}
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.buffersize.max))
		{
			if (ld_cp->con_params.buffersize > preferred.buffersize.max)
			{
				thereismismatch = true;
				if (ld_fix)
				{
					ld_fix->con_params.buffersize = preferred.buffersize.max;
				}
			}
		}

		// ================================================================================
		// Samplerate
		// ================================================================================
		if (JVX_CHECK_SIZE_SELECTED(preferred.samplerate.min))
		{
			if (ld_cp->con_params.rate < preferred.samplerate.min)
			{
				thereismismatch = true;
				if (ld_fix)
				{
					ld_fix->con_params.rate = preferred.samplerate.min;
				}
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(preferred.samplerate.max))
		{
			if (ld_cp->con_params.rate > preferred.samplerate.max)
			{
				thereismismatch = true;
				if (ld_fix)
				{
					ld_fix->con_params.rate = preferred.samplerate.max;
				}
			}
		}

		// ================================================================================
		// Format
		// ================================================================================
		if (preferred.format.min != JVX_DATAFORMAT_NONE)
		{
			if (ld_cp->con_params.format < preferred.format.min)
			{
				thereismismatch = true;
				if (ld_fix)
				{
					ld_fix->con_params.format = preferred.format.min;
				}
			}
		}
		if (preferred.format.max != JVX_DATAFORMAT_NONE)
		{
			if (ld_cp->con_params.format > preferred.format.max)
			{
				thereismismatch = true;
				if (ld_fix)
				{
					ld_fix->con_params.format = preferred.format.max;
				}
			}
		}

		// ================================================================================
		// DimX
		// ================================================================================
		if (JVX_CHECK_SIZE_SELECTED(ld_cp->con_params.segmentation.x))
		{
			if (JVX_CHECK_SIZE_SELECTED(preferred.dimX.min))
			{
				if (ld_cp->con_params.segmentation.x < preferred.dimX.min)
				{
					thereismismatch = true;
					if (ld_fix)
					{
						ld_fix->con_params.segmentation.x = preferred.dimX.min;
					}
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(preferred.dimX.max))
			{
				if (ld_cp->con_params.segmentation.x > preferred.dimX.max)
				{
					thereismismatch = true;
					if (ld_fix)
					{
						ld_fix->con_params.segmentation.x = preferred.dimX.max;
					}
				}
			}
		}

		// ================================================================================
		// DimY
		// ================================================================================
		if (JVX_CHECK_SIZE_SELECTED(ld_cp->con_params.segmentation.y))
		{
			if (JVX_CHECK_SIZE_SELECTED(preferred.dimY.min))
			{
				if (ld_cp->con_params.segmentation.y < preferred.dimY.min)
				{
					thereismismatch = true;
					if (ld_fix)
					{
						ld_fix->con_params.segmentation.y = preferred.dimY.min;
					}
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(preferred.dimY.max))
			{
				if (ld_cp->con_params.segmentation.y > preferred.dimY.max)
				{
					thereismismatch = true;
					if (ld_fix)
					{
						ld_fix->con_params.segmentation.y = preferred.dimY.max;
					}
				}
			}
		}
	}
}

void
CjvxNegotiate_common::_constrain_ldesc(jvxLinkDataDescriptor* theData) const
{

	// Prefer max buffersize 
	JVX_MIN_NEG_SET_LDAT(preferred.buffersize, theData->con_params.buffersize);
	JVX_MAX_NEG_SET_LDAT(preferred.buffersize, theData->con_params.buffersize);
	// ===================================================================

	// ===================================================================
	// Prefer min samplerate
	JVX_MAX_NEG_SET_LDAT(preferred.samplerate, theData->con_params.rate);
	JVX_MIN_NEG_SET_LDAT(preferred.samplerate, theData->con_params.rate);
	// ===================================================================

	// ===================================================================
	// Prefer min number channels
	JVX_MAX_NEG_SET_LDAT(preferred.number_channels, theData->con_params.number_channels);
	JVX_MIN_NEG_SET_LDAT(preferred.number_channels, theData->con_params.number_channels);
	// ===================================================================

	JVX_MAX_NEG_SET_LDAT(preferred.dimX, theData->con_params.segmentation.x);
	JVX_MIN_NEG_SET_LDAT(preferred.dimX, theData->con_params.segmentation.x);

	// ===================================================================

	JVX_MAX_NEG_SET_LDAT(preferred.dimY, theData->con_params.segmentation.y);
	JVX_MIN_NEG_SET_LDAT(preferred.dimY, theData->con_params.segmentation.y);

	// ===================================================================
	// Prefer min format
	JVX_MAX_NEG_SET_LDAT_CMP(preferred.format, theData->con_params.format, JVX_DATAFORMAT_NONE);
	JVX_MIN_NEG_SET_LDAT_CMP(preferred.format, theData->con_params.format, JVX_DATAFORMAT_NONE);
	// ===================================================================

	// ===================================================================
	// Prefer min format
	JVX_MAX_NEG_SET_LDAT_CMP(preferred.subformat, theData->con_params.format_group, JVX_DATAFORMAT_GROUP_NONE);
	JVX_MIN_NEG_SET_LDAT_CMP(preferred.subformat, theData->con_params.format_group, JVX_DATAFORMAT_GROUP_NONE);
	// ===================================================================

	// ===================================================================
	// Prefer min format
	JVX_MAX_NEG_SET_LDAT_CMP(preferred.data_flow, theData->con_params.data_flow, JVX_DATAFLOW_DONT_CARE);
	JVX_MIN_NEG_SET_LDAT_CMP(preferred.data_flow, theData->con_params.data_flow, JVX_DATAFLOW_DONT_CARE);
}

// ===================================================================
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

	resComplain = jvx_check_valid(theData_in->con_params, reason);
	if (resComplain != JVX_NO_ERROR)
	{
		JVX_CONNECTION_FEEDBACK_ON_LEAVE_ERROR(fdb, resComplain,
			("Invalid connector parameter on forward negotiation, reason in detail: " + reason).c_str());
		return resComplain;
	}

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
				(allowedSetup[i].dimX == theData_in->con_params.segmentation.x) &&
				(allowedSetup[i].dimY == theData_in->con_params.segmentation.y) &&
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
			ld_cp.con_params.data_flow = allowedSetup[i].dataflow;
			ld_cp.con_params.segmentation.x = allowedSetup[i].dimX;
			ld_cp.con_params.segmentation.y = allowedSetup[i].dimY;

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
		ld_cp.con_params.segmentation.x = theData_in->con_params.segmentation.x;
		ld_cp.con_params.segmentation.y = theData_in->con_params.segmentation.y;		
		ld_cp.con_params.data_flow = theData_in->con_params.data_flow;

		compare_core(theData_in, &ld_cp, thereismismatch);

		if (this->negBehavior == negBehaviorType::JVX_BEHAVIOR_AUDIO)
		{
			if ((ld_cp.con_params.segmentation.y != 1) ||
				(ld_cp.con_params.segmentation.x != ld_cp.con_params.buffersize))
			{
				std::cout << __FUNCTION__ << ": " << __LINE__ << ": Warning: Setup of the segmentation is untypical for audio. Expected would be:" << std::endl;
				std::cout << " Seg X: " << ld_cp.con_params.segmentation.x << " vs " << ld_cp.con_params.buffersize << "." << std::endl;
				std::cout << " Seg Y: " << ld_cp.con_params.segmentation.y << " vs 1." << std::endl;
			}
		}
		else
		{
			if ((ld_cp.con_params.segmentation.y * ld_cp.con_params.segmentation.x) != ld_cp.con_params.buffersize)
			{
				std::cout << __FUNCTION__ << ": " << __LINE__ << ": Warning: Setup of the segmentation is not correct. Expected would be:" << std::endl;
				std::cout << " Seg X x Seg Y = " << ld_cp.con_params.segmentation.x << " X " << 
					ld_cp.con_params.segmentation.x << " = " << ld_cp.con_params.buffersize << "." << std::endl;
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
				reason += jvxDataFormat_txt(ld_cp.con_params.format);
				reason += "> was desired but the input connector only delivers <";
				reason += jvxDataFormat_txt(theData_in->con_params.format);
				reason += ">.";
				resComplain = JVX_ERROR_COMPROMISE;
			}
			else if (ld_cp.con_params.format_group != theData_in->con_params.format_group)
			{
				reason = "A format group of <";
				reason += jvxDataFormatGroup_txt(ld_cp.con_params.format_group);
				reason += "> was desired but the input connector only delivers <";
				reason += jvxDataFormatGroup_txt(theData_in->con_params.format_group);
				reason += ">.";
				resComplain = JVX_ERROR_COMPROMISE;
			}
			else if (ld_cp.con_params.data_flow != theData_in->con_params.data_flow)
			{
				reason = "A dataflow type of <";
				reason += jvxDataflow_txt(ld_cp.con_params.data_flow);
				reason += "> was desired but the input connector only delivers <";
				reason += jvxDataflow_txt(theData_in->con_params.data_flow);
				reason += ">.";
				resComplain = JVX_ERROR_COMPROMISE;
			}
			else if (ld_cp.con_params.segmentation.x != theData_in->con_params.segmentation.x)
			{
				reason = "A segmentation_x of <";
				reason += jvx_size2String(ld_cp.con_params.segmentation.x);
				reason += "> was desired but the input connector only delivers <";
				reason += jvx_size2String(theData_in->con_params.segmentation.x);
				reason += ">.";
				resComplain = JVX_ERROR_COMPROMISE;
			}
			else if (ld_cp.con_params.segmentation.y != theData_in->con_params.segmentation.y)
			{
				reason = "A segmentation_y of <";
				reason += jvx_size2String(ld_cp.con_params.segmentation.y);
				reason += "> was desired but the input connector only delivers <";
				reason += jvx_size2String(theData_in->con_params.segmentation.y);
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


