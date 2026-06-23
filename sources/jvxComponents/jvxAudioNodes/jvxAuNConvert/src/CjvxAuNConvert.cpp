#include "CjvxAuNConvert.h"
#include "jvx-helpers-cpp.h"
#include "jvx_math/jvx_math_extensions.h"

static jvxSize resamplerQualities[] =
{
	32, 64, 128
};

#define JVX_FORMAT_TABLE_SIZE 4
static jvxDataFormat formatTable[JVX_FORMAT_TABLE_SIZE] =
{
	JVX_DATAFORMAT_DATA,
	JVX_DATAFORMAT_32BIT_LE,
	JVX_DATAFORMAT_16BIT_LE,
	JVX_DATAFORMAT_64BIT_LE
};

// =====================================================================================
// =====================================================================================

CjvxAuNConvert::CjvxAuNConvert(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE);
	_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;
	// outputArgsFromOutputParams = true;

	jvx_bitSet(checkRequestUpdate, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_DATAFLOW_SHIFT);
	jvx_bitSet(checkRequestUpdate, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SUBFORMAT_SHIFT);
	jvx_bitSet(checkRequestUpdate, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMATSPEC_SHIFT);
	jvx_bitInvert(checkRequestUpdate);
}

CjvxAuNConvert::~CjvxAuNConvert()
{
	
}

jvxErrorType
CjvxAuNConvert::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		genConvert_node::init_all();
		genConvert_node::allocate_all();
		genConvert_node::register_all(this);
		genConvert_node::associate__resampling(this, &currNegoStat.resampling.cc.oversamplingFactor, 1, &currNegoStat.resampling.cc.downsamplingFactor, 1);
		genConvert_node::register_callbacks(this, set_config, this);

		// Initial read of functionality
		currNegoStat.fixedLocationMode = genConvert_node::translate__config__fixed_rate_location_mode_from(0);

		// genChannelRearrange_node::register_callbacks(this, get_level_pre, get_level_post, set_passthru, this);
		currNegoStat.resampling.cc.downsamplingFactor = 1;
		currNegoStat.resampling.cc.oversamplingFactor = 1;
	}
	return res;
}

jvxErrorType
CjvxAuNConvert::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genConvert_node::unregister_all(this);
		genConvert_node::deallocate_all();

		CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

jvxErrorType 
CjvxAuNConvert::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_ERROR_REQUEST_CALL_AGAIN;
	
	currNegoStat.resetOneTest(currNegoStat.fixedLocationMode);
	currNegoStat.passFromPredecessor(_common_set_icon.theData_in);	

	while (res == JVX_ERROR_REQUEST_CALL_AGAIN)
	{
		res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	if (res == JVX_NO_ERROR)
	{
		// Take over all results from submodule
		currNegoStat.copyInputArgs(node_inout._common_set_node_params_a_1io);
		currNegoStat.copyOutputArgs(node_output._common_set_node_params_a_1io);

		// Procede with derivation of processing quantities
		reChannel.numIn = node_inout._common_set_node_params_a_1io.number_channels;
		reType.formIn = (jvxDataFormat)node_inout._common_set_node_params_a_1io.format;

		reChannel.numOut = node_output._common_set_node_params_a_1io.number_channels;
		reType.formOut = (jvxDataFormat)node_output._common_set_node_params_a_1io.format;

		// auto mode = genConvert_node::translate__config__fixed_rate_location_mode_from();
		switch (currNegoStat.fixedLocationMode)
		{
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
			genConvert_node::resampling.real_rate.value = (jvxData)node_output._common_set_node_params_a_1io.samplerate *
				(jvxData)currNegoStat.resampling.cc.downsamplingFactor / (jvxData)currNegoStat.resampling.cc.oversamplingFactor;
			break;
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
			genConvert_node::resampling.real_rate.value = (jvxData)node_inout._common_set_node_params_a_1io.samplerate * 
				(jvxData)currNegoStat.resampling.cc.oversamplingFactor / (jvxData)currNegoStat.resampling.cc.downsamplingFactor;
			break;
		default: 
			break;
		}		
	}

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
	genConvert_node::feedback.protocolSummary.value = currNegoStat.protoNegotiate;
#endif

	return res;
}

jvxErrorType
CjvxAuNConvert::accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// ===============================================================================
	// Set the input parameters
	// ===============================================================================

	// Take over input parameters
	currNegoStat.acceptInputParams(_common_set_icon.theData_in, nullptr);
	currNegoStat.inputToOutput();
	
	return res;

}

void 
CjvxAuNConvert::test_set_output_parameters()
{
	currNegoStat.prepareForForward(_common_set_ocon.theData_out);	
}

void
CjvxAuNConvert::from_input_to_output()
{
	// Override to do not do anything here!!
}

jvxErrorType 
CjvxAuNConvert::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	// This function involves the default implementation but should end up in <accept_negotiate_output>
	// by inheritance
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		res = currNegoStat.passFromSuccessor((jvxLinkDataDescriptor*)data, _common_set_ocon.theData_out, _common_set_icon.theData_in JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;
	default:
		res = CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;
	}
	return res;
}

jvxErrorType
CjvxAuNConvert::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	return res;
}

jvxErrorType
CjvxAuNConvert::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	// Configure this module to be zerocopy
	_common_set_ldslave.zeroCopyBuffering_cfg = true;
	runtime.active_resampling = false;
	runtime.active_rechannel = false;
	runtime.requiresRebuffering = false;
	runtime.requiresRebufferHeadroom= false;
	runtime.lFieldRebuffer = 0;
	runtime.commonFormat = JVX_DATAFORMAT_DATA;

	// HINT FOR THIS PART:
	// We may redefine the common dataformat to prevent type connversion on the input or the output side
	// However, this would require some type specific code changes in the process functions. Up for now,
	// we always run the signal processing part in format jvxData. It is all prepared but currently, always falling back to 
	// jvxData should not be a significant disadvantage!!

#if 0
	
	// Find a common dataformat to avoid type conversions
	for (jvxSize ff = 0; ff < JVX_FORMAT_TABLE_SIZE; ff++)
	{
		jvxDataFormat cForm = formatTable[ff];
		if (
			(node_inout._common_set_node_params_a_1io.format == cForm) ||
			(node_output._common_set_node_params_a_1io.format == cForm))
		{
			runtime.commonFormat = cForm;
			break;
		}
	}
	// runtime.commonFormat = (jvxDataFormat)JVX_MIN(node_inout._common_set_node_params_a_1io.format, node_output._common_set_node_params_a_1io.format);
#endif

	switch (runtime.commonFormat)
	{
	case JVX_DATAFORMAT_DATA:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(runtime.bufsConvert_data, dataBaseProcessing<jvxData>);
		break;
	default: 
		assert(0);
		break;
	}

	if ((currNegoStat.resampling.cc.oversamplingFactor != 1) || 
		(currNegoStat.resampling.cc.downsamplingFactor != 1))
	{
		runtime.active_resampling = true;
	}
	if (reChannel.numIn != reChannel.numOut)
	{
		runtime.active_rechannel = true;
	}
	if (node_inout._common_set_node_params_a_1io.format != runtime.commonFormat)
	{
		runtime.active_retype_input = true;
	}

	if (node_output._common_set_node_params_a_1io.format != runtime.commonFormat)
	{
		runtime.active_retype_output = true;
	}

	if(runtime.active_resampling || runtime.active_rechannel)
	{
		_common_set_ldslave.zeroCopyBuffering_cfg = false;

		switch (currNegoStat.fixedLocationMode)
		{
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
			runtime.requiresRebuffering = jvx_bitTest(_common_set_icon.theData_in->con_data.alloc_flags,
				(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
			break;
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
			/*runtime.requiresRebuffering = jvx_bitTest(_common_set_icon.theData_in->con_data.alloc_flags,
				(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
				*/
			// std::cout << "Hier" << std::endl;
			break;
		}
		
		if (runtime.requiresRebuffering)
		{
			// In case of variable input size, add headroom space on input side
			runtime.requiresRebufferHeadroom = true;
		}
		else
		{			
			// If we have a fixed input / output relation, check if we need a variable framesize
			if (node_output._common_set_node_params_a_1io.buffersize % currNegoStat.resampling.cc.oversamplingFactor)
			{
				runtime.requiresRebufferHeadroom= true;
				runtime.requiresRebuffering = true;
			}
			if (node_inout._common_set_node_params_a_1io.buffersize % currNegoStat.resampling.cc.downsamplingFactor)
			{
				runtime.requiresRebufferHeadroom= true;
				runtime.requiresRebuffering = true;
			}
		}
		if (runtime.active_rechannel)
		{
			runtime.requiresRebuffering = true;
		}

		switch (currNegoStat.fixedLocationMode)
		{
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
			if (runtime.requiresRebufferHeadroom)
			{
				// This module may produce 
				jvx_bitSet(_common_set_ocon.theData_out.con_data.alloc_flags,
					(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
			}
			break;
		}
	}

	res = CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvxSize numBuffersIntermediate = 0;

		if (runtime.active_resampling)
		{
			switch (currNegoStat.fixedLocationMode)
			{
			case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
				runtime.numResampler = node_output._common_set_node_params_a_1io.number_channels;
				break;
			case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
				runtime.numResampler = node_inout._common_set_node_params_a_1io.number_channels;
				numBuffersIntermediate = runtime.numResampler;
				break;
			}

			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.fldResampler, jvx_fixed_resampler, runtime.numResampler);
			for (i = 0; i < runtime.numResampler; i++)
			{
				jvx_fixed_resampler_initConfig(&runtime.fldResampler[i]);
				runtime.fldResampler[i].prmInit.buffersizeIn = currNegoStat.resampling.bSizeInMax;
				runtime.fldResampler[i].prmInit.buffersizeOut = currNegoStat.resampling.bSizeOutMax;
				runtime.fldResampler[i].prmInit.format = runtime.commonFormat;
				runtime.fldResampler[i].prmInit.lFilter = resamplerQualities[(jvxSize)resamplerQuality];
				runtime.fldResampler[i].prmInit.optPtrConversion = &currNegoStat.resampling.cc; // Provide the resampling exact config
				jvx_fixed_resampler_prepare(&runtime.fldResampler[i]);
			}
		}

		if (runtime.requiresRebuffering)
		{
			if (runtime.requiresRebufferHeadroom)
			{
				switch (currNegoStat.fixedLocationMode)
				{
				case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
					runtime.lFieldRebuffer = node_inout._common_set_node_params_a_1io.buffersize + 
						currNegoStat.resampling.cc.downsamplingFactor;
					break;
				case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
					runtime.lFieldRebuffer = node_output._common_set_node_params_a_1io.buffersize + 
						currNegoStat.resampling.cc.oversamplingFactor;
					break;
				}
			}
			else
			{
				switch (currNegoStat.fixedLocationMode)
				{
				case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
					runtime.lFieldRebuffer = node_inout._common_set_node_params_a_1io.buffersize;
					break;
				case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
					runtime.lFieldRebuffer = node_output._common_set_node_params_a_1io.buffersize;
					break;
				}
			}

			// The number of resamplers may be higher than the number of output buffers if we need an addional rebuffering
			runtime.fFieldRebuffer = 0;
			runtime.nCFieldRebuffer = JVX_MAX(numBuffersIntermediate, node_output._common_set_node_params_a_1io.number_channels);
			runtime.lFieldRebufferChannel = runtime.lFieldRebuffer * jvxDataFormat_getsize(runtime.commonFormat);

			switch (runtime.commonFormat)
			{
			case JVX_DATAFORMAT_DATA:
				runtime.bufsConvert_data->ptrFieldBuffer = nullptr;
				JVX_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.bufsConvert_data->ptrFieldBuffer, jvxData*, runtime.nCFieldRebuffer);
				for (i = 0; i < runtime.nCFieldRebuffer; i++)
				{
					jvxByte* ptrByte = nullptr;
					JVX_SAFE_ALLOCATE_FIELD_CPP_Z(ptrByte, jvxByte, runtime.lFieldRebufferChannel);
					runtime.bufsConvert_data->ptrFieldBuffer[i] = (jvxData*)ptrByte;
				}
				break;
			default:
				assert(0);
			}
		}

		if (runtime.active_retype_input)
		{
			switch (runtime.commonFormat)
			{
			case JVX_DATAFORMAT_DATA:
				JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(runtime.bufsConvert_data->bufsInConvert,
					jvxData, node_inout._common_set_node_params_a_1io.number_channels,
					node_inout._common_set_node_params_a_1io.buffersize);
				break;
			default:
				assert(0);
			}
		}
		if (runtime.active_retype_output)
		{
			switch (runtime.commonFormat)
			{
			case JVX_DATAFORMAT_DATA:
				JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(runtime.bufsConvert_data->bufsOutConvert,
					jvxData, node_output._common_set_node_params_a_1io.number_channels,
					node_output._common_set_node_params_a_1io.buffersize);
				break;
			default:
				assert(0);
			}
		}
	}
	return res;
}

jvxErrorType
CjvxAuNConvert::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxBareNode1ioRearrange::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	if (runtime.active_retype_input)
	{
		switch (runtime.commonFormat)
		{
		case JVX_DATAFORMAT_DATA:
			JVX_SAFE_DELETE_2DFIELD(runtime.bufsConvert_data->bufsInConvert, node_inout._common_set_node_params_a_1io.number_channels);
			break;
		default:
			assert(0);
		}
	}

	if (runtime.active_retype_output)
	{
		switch (runtime.commonFormat)
		{
		case JVX_DATAFORMAT_DATA:
			JVX_SAFE_DELETE_2DFIELD(runtime.bufsConvert_data->bufsOutConvert, node_output._common_set_node_params_a_1io.number_channels);
			break;
		default:
			assert(0);
		}
	}

	if (runtime.active_resampling)
	{
		for (i = 0; i < runtime.numResampler; i++)
		{
			jvx_fixed_resampler_postprocess(&runtime.fldResampler[i]);
		}
		JVX_SAFE_DELETE_FIELD(runtime.fldResampler);
		runtime.numResampler = 0;
	}

	if (runtime.requiresRebuffering)
	{
		switch (runtime.commonFormat)
		{
		case JVX_DATAFORMAT_DATA:
			for (i = 0; i < runtime.nCFieldRebuffer; i++)
			{
				JVX_SAFE_DELETE_FIELD_TYPE(runtime.bufsConvert_data->ptrFieldBuffer[i], jvxByte);
			}
			JVX_SAFE_DELETE_FIELD(runtime.bufsConvert_data->ptrFieldBuffer);
			runtime.bufsConvert_data->ptrFieldBuffer = nullptr;
			break;
		default:
			assert(0);
		}

		runtime.fFieldRebuffer = 0;
		runtime.nCFieldRebuffer = 0;
		runtime.lFieldRebufferChannel = 0;
		runtime.lFieldRebuffer = 0;

	}

	runtime.requiresRebuffering = false;
	runtime.requiresRebufferHeadroom = false;
	
	runtime.active_resampling = false;
	runtime.active_retype_input = false;
	runtime.active_retype_output = false;
	runtime.active_rechannel = false;

	switch (runtime.commonFormat)
	{
	case JVX_DATAFORMAT_DATA:
		JVX_DSP_SAFE_DELETE_OBJECT(runtime.bufsConvert_data);
		runtime.bufsConvert_data = nullptr;
		break;
	default:
		assert(0);
		break;
	}

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNConvert::process_start_icon(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxSize idx_stage_local = JVX_SIZE_UNSELECTED;
	jvxSize space = 0;
	jvxErrorType res = CjvxBareNode1ioRearrange::process_start_icon(
		pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	if (res == JVX_NO_ERROR)
	{
		if (runtime.requiresRebuffering)
		{
			// If we run resampler, under certain circumstances, we need to request different numbers of 
			// samples - otherwise the buffersize can not be fulfilled in average
			switch (currNegoStat.fixedLocationMode)
			{
			case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:

				// Compute the number of samples we would need to fill the internal buffer
				idx_stage_local = jvx_process_icon_extract_stage(_common_set_icon.theData_in, JVX_SIZE_UNSELECTED);
				
				// If the successor does not support variable frame size this algorithm can not really work!!
				assert(_common_set_icon.theData_in->con_data.fHeights);

				// Compute the min or the max number of samples and provide for successor				
				space = runtime.lFieldRebuffer - runtime.fFieldRebuffer;

				// Depending on the space, request either the minimum or the maximum number of samples
				if (space >= currNegoStat.resampling.bSizeOutMax)
				{
					_common_set_icon.theData_in->con_data.fHeights[idx_stage_local].x = currNegoStat.resampling.bSizeInMax;
				}
				else
				{
					_common_set_icon.theData_in->con_data.fHeights[idx_stage_local].x = currNegoStat.resampling.bSizeInMin;
				}

				break;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxAuNConvert::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
		
	// If we do nothing, bypass with zerocopy - no data processing!
	if(zeroCopyBuffering_rt)
	{ 
		return CjvxBareNode1ioRearrange::process_buffers_icon(mt_mask, idx_stage);
	}

	// Options: 
	// 
	// --- runtime.active_resampling
	// --- runtime.active_retype_output
	// --- runtime.active_retype_input
	// --- runtime.requiresRebuffering
	//
	
	// ============================================================================
	// Extract input fields. We may accept different formats here!
	// ============================================================================

	jvxData** bufsInData = nullptr;
	jvxInt16** bufsInInt16 = nullptr;
	jvxInt32** bufsInInt32 = nullptr;

	switch (_common_set_icon.theData_in->con_params.format)
	{
	case JVX_DATAFORMAT_DATA:
		bufsInData = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		bufsInInt16 = jvx_process_icon_extract_input_buffers<jvxInt16>(_common_set_icon.theData_in, idx_stage);
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		bufsInInt32 = jvx_process_icon_extract_input_buffers<jvxInt32>(_common_set_icon.theData_in, idx_stage);
		break;
	}
	
	// ============================================================================
	// Extract output fields. We may accept different formats here!
	// ============================================================================

	jvxData** bufsOutData = nullptr;
	jvxInt16** bufsOutInt16 = nullptr;
	jvxInt32** bufsOutInt32 = nullptr;

	switch (_common_set_ocon.theData_out.con_params.format)
	{
	case JVX_DATAFORMAT_DATA:
		bufsOutData = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		bufsOutInt16 = jvx_process_icon_extract_output_buffers<jvxInt16>(_common_set_ocon.theData_out);
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		bufsOutInt32 = jvx_process_icon_extract_output_buffers<jvxInt32>(_common_set_ocon.theData_out);
		break;
	}

	jvxSize idx_stage_local_in = jvx_process_icon_extract_stage(_common_set_icon.theData_in, idx_stage);
	jvxSize idx_stage_local_out = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;

	// ============================================================================
	// ============================================================================
	// ============================================================================
	
	// ============================================================================
	// Compute number of input samples for rebuffering mode!!	
	// ============================================================================

	jvxSize numInputSamples = _common_set_icon.theData_in->con_params.buffersize;
	if (_common_set_icon.theData_in->con_data.fHeights && JVX_CHECK_SIZE_SELECTED(_common_set_icon.theData_in->con_data.fHeights[idx_stage_local_in].x))
	{
		numInputSamples = _common_set_icon.theData_in->con_data.fHeights[idx_stage_local_in].x;
	}
	assert(numInputSamples <= _common_set_icon.theData_in->con_params.buffersize);

	// Default case is that we out output the proposed number of samples
	jvxSize numOutSamples = node_output._common_set_node_params_a_1io.buffersize;

	// ============================================================================
	// In case a type conversion is required on the input side
	// ============================================================================

	if (runtime.active_retype_input)
	{
		jvxSize cnt = 0;
		jvxBool jumpToExit = false;
		
		// Output of converter may be convert buffer (default) or processing output buffer if nothing else happens
		jvxData** targetConvInData = (jvxData**)runtime.bufsConvert_data->bufsInConvert;

		// If we see no output conversion and no rebuffering, we will copy straight to the output
		if (!runtime.requiresRebuffering && !runtime.active_retype_output && !runtime.active_resampling)
		{
			targetConvInData = bufsOutData; // <- this is the component target output buffer!!
			jumpToExit = true; // <- in case we operatre quickly, use the jump shortcut!!
		}

		// Run the actual conversion
		switch (_common_set_icon.theData_in->con_params.format)
		{
		case JVX_DATAFORMAT_16BIT_LE:
			for (cnt = 0; cnt < _common_set_icon.theData_in->con_params.number_channels; cnt++)
			{
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, jvxData>(
					bufsInInt16[cnt], targetConvInData[cnt],
					numInputSamples, JVX_MAX_INT_16_DIV);
			}			
			bufsInData = (jvxData**)runtime.bufsConvert_data->bufsInConvert;

			break;
		case JVX_DATAFORMAT_32BIT_LE:
			for (cnt = 0; cnt < _common_set_icon.theData_in->con_params.number_channels; cnt++)
			{
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt32, jvxData>(
					bufsInInt32[cnt], targetConvInData[cnt],
					numInputSamples, JVX_MAX_INT_16_DIV);
			}
			bufsInData = (jvxData**)runtime.bufsConvert_data->bufsInConvert;

			break;
		default:
			assert(0);
		}

		// If shortcut makes sense, go ahead!!
		if (jumpToExit)
		{
			goto exit_fwd;
		}
	}


	// =======================================================================================
	// Convert from jvxData to specific format. OUTPUT CONVERSION!
	// Here, we only pre-set a variable for the later case
	// =======================================================================================	

	if (runtime.active_retype_output)
	{
		// We declare a new output buffer taregt: the intermediate buffer of type jvxData!
		// Later, the data within this buffer will be retyped on the output side
		switch (_common_set_ocon.theData_out.con_params.format)
		{
		case JVX_DATAFORMAT_16BIT_LE:

			bufsOutData = (jvxData**)runtime.bufsConvert_data->bufsOutConvert;
			break;

		case JVX_DATAFORMAT_32BIT_LE:
			bufsOutData = (jvxData**)runtime.bufsConvert_data->bufsOutConvert;
			break;

		default:
			assert(0);
		}
	}

	// The algorithm becomes a little complicated if the output buffersize does not fit into the 
	// provides buffer due to fractional relation
	if (runtime.requiresRebuffering)
	{
		jvxSize numCopyNextFrame = 0;
		jvxSize numResamplerOut = 0;
		jvxSize numResamplerIn = 0;		
		jvxSize cnt = 0;
		jvxSize splCnt = 0;
		jvxSize loopNum = 0; 
		jvxSize numOutChannels = _common_set_ocon.theData_out.con_params.number_channels;
		jvxData** bufsOutChannels = bufsOutData;
		jvxSize numInChannels = _common_set_icon.theData_in->con_params.number_channels;
		jvxData** bufsInChannelsData = bufsInData;

		switch (currNegoStat.fixedLocationMode)
		{
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:

			// This case: Converter forwards variable buffersizes!!

			// runtime.nCFieldRebuffer is allocated to hold the OUTPUT channels!
			// We will loop over max of input and output buffers but we will only
			// resample over the output channels

			// If we have a length constraint in the input, we need it as well in the output
			numOutSamples = numInputSamples;
			assert(numOutSamples <= node_output._common_set_node_params_a_1io.buffersize);

			// Channel conversion with different targets for resampling or no resampling			
			if (runtime.active_resampling)
			{
				numOutChannels = runtime.nCFieldRebuffer;
				bufsOutChannels = (jvxData**)runtime.bufsConvert_data->ptrFieldBuffer;
			}

			loopNum = JVX_MAX(numOutChannels,
				_common_set_icon.theData_in->con_params.number_channels);
			
			// ===========================================================
			// First, set the output buffers all to 0
			// ===========================================================
			for (cnt = 0; cnt < numOutChannels; cnt++)
			{
				jvxData* targetIn = (jvxData*)bufsOutChannels[cnt];
				targetIn += runtime.fFieldRebuffer;

				jvxSize splCnt = 0;
				for (; splCnt < numInputSamples; splCnt++)
				{
					targetIn[splCnt] = 0;
				}
			}

			// ===========================================================
			// Mix input channels into output buffers - either downmix or circular repetition
			// ===========================================================
			for (cnt = 0; cnt < loopNum; cnt++)
			{
				// Channel wrap around for in and out
				jvxSize inCnt = cnt % _common_set_icon.theData_in->con_params.number_channels;
				jvxSize outCnt = cnt % numOutChannels;

				// From input to rebuffer buffers
				jvxData* sourceInData = (jvxData*)bufsInData[inCnt];
				jvxData* targetIn = (jvxData*)bufsOutChannels[outCnt];
				
				// Target may have an offset due to stored samples
				targetIn += runtime.fFieldRebuffer;

				// Loop over all samples with circular extend or downmix
				for (splCnt = 0; splCnt < numInputSamples; splCnt++)
				{
					jvxData sampleIn = sourceInData[splCnt];
					targetIn[splCnt] += sampleIn;
				}
			}

			// ====================================================================

			if (runtime.active_resampling)
			{
				// Update the fillheight
				runtime.fFieldRebuffer += numInputSamples;

				// =============================================================
				// Approaching the resampling next!
				// =============================================================

				// Number samples on the input side - we trunc the samples according to downsampling factor to find the number of input samples that fit with the 
				numResamplerIn = runtime.fFieldRebuffer / currNegoStat.resampling.cc.downsamplingFactor * 
					currNegoStat.resampling.cc.downsamplingFactor; // Find multiples of input granularity

				// Compute the number of output samples
				numResamplerOut = numResamplerIn * currNegoStat.resampling.cc.oversamplingFactor / 
					currNegoStat.resampling.cc.downsamplingFactor;
				assert(numResamplerOut <= _common_set_ocon.theData_out.con_params.buffersize);

				// Run the actual resamplers, produce a variable output size
				for (i = 0; i < runtime.numResampler; i++)
				{
					fixed_resample_variable_out varOut;
					varOut.bsizeIn = numResamplerIn;
					varOut.numOut = numResamplerOut;

					// Important: use the "new" input buffer
					jvx_fixed_resampler_process(&runtime.fldResampler[i], runtime.bufsConvert_data->ptrFieldBuffer[i], bufsOutData[i], &varOut); // Full size resampling
				}
			
				// Take away the number of output samples as computed - we will need this on output conversion
				numOutSamples = numResamplerOut;

				// Housekeeping: update intermediate buffer by sample move. It is typically only very few samples to move.
				numCopyNextFrame = runtime.fFieldRebuffer - numResamplerIn;
				for (cnt = 0; cnt < runtime.nCFieldRebuffer; cnt++)
				{
					jvxData* cpyTo = (jvxData*)runtime.bufsConvert_data->ptrFieldBuffer[cnt];
					jvxData* cpyFrom = cpyTo + numResamplerIn;

					for (splCnt = 0; splCnt < numCopyNextFrame; splCnt++)
					{
						*cpyTo = *cpyFrom;
						cpyTo++;
						cpyFrom++;
					}
				}

				// Finally, update fill height for next frame
				runtime.fFieldRebuffer -= numResamplerIn;
			}

			// Accept number of samples on the output side
			assert(_common_set_ocon.theData_out.con_data.fHeights);
			// Now approach the output side and set the variable output buffersize			
			_common_set_ocon.theData_out.con_data.fHeights[idx_stage_local_out].x = numOutSamples;

			break;
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:

			// This case: Converter expects variable buffersizes on the input!!
			// Control of the number of samples is in process_icon_start!!

			// runtime.nCFieldRebuffer is allocated to hold the INPUT channels!
			// We will loop over max of input and output buffers but we will only
			// resample over the output channels
			if (runtime.active_resampling)
			{
				numInChannels = runtime.nCFieldRebuffer;
				bufsInChannelsData = (jvxData**)runtime.bufsConvert_data->ptrFieldBuffer;
			}

			loopNum = JVX_MAX(numInChannels,
				_common_set_ocon.theData_out.con_params.number_channels);
			
			// Compute the number of samples available
			numResamplerOut = numInputSamples * currNegoStat.resampling.cc.oversamplingFactor / 
				currNegoStat.resampling.cc.downsamplingFactor;
			
			// Output buffersize is fixed! Has been set on entry!!
			// numOutSamples_ = node_output._common_set_node_params_a_1io.buffersize;

			// Some checks to make sure the samples fit into our internal buffer
			assert(runtime.fFieldRebuffer + numResamplerOut <= runtime.lFieldRebuffer);

			if (runtime.active_resampling)
			{
				// Resample the input to the internal buffer. The buffer can be variably filled but we must
				// make sure that we will have enough samples to forward with the fixed buffersize!
				for (i = 0; i < runtime.numResampler; i++)
				{
					// Target field is the intermediate buffer
					jvxData* targetOut = (jvxData*)runtime.bufsConvert_data->ptrFieldBuffer[i];

					// But we will fill up from the fHeight offset to hiold old samples
					targetOut += runtime.fFieldRebuffer;

					// Core resampling
					fixed_resample_variable_out varOut;
					varOut.bsizeIn = numInputSamples;
					varOut.numOut = numResamplerOut;

					// Important: use the "new" input buffer with index shift (targetOut)
					jvx_fixed_resampler_process(&runtime.fldResampler[i], bufsInData[i], targetOut, &varOut); // Full size resampling
				}

				// Update fillheight
				runtime.fFieldRebuffer += numResamplerOut;
			}

			// Now, preset the output buffer to 0
			for (cnt = 0; cnt < node_output._common_set_node_params_a_1io.number_channels; cnt++)
			{
				jvxData* targetOut = bufsOutData[cnt];
				jvxSize splCnt = 0;
				for (; splCnt < numOutSamples; splCnt++)
				{
					targetOut[splCnt] = 0;
				}
			}

			// Here, take the output from the resampled input buffers - start from the beginning of the buffer
			// to forward oldest samples at first
			for (cnt = 0; cnt < loopNum; cnt++)
			{
				// Channel wrap around for downmix or circular extend
				jvxSize inCnt = cnt % numInChannels;
				jvxSize outCnt = cnt % _common_set_ocon.theData_out.con_params.number_channels;

				// Straight copy front to back
				jvxData* sourceOut = (jvxData*)bufsInChannelsData[inCnt];
				jvxData* targetOut = (jvxData*)bufsOutData[outCnt];

				// Add the output from channels in case of downmix
				for (splCnt = 0; splCnt < numOutSamples; splCnt++)
				{
					jvxData sampleOut = sourceOut[splCnt];
					targetOut[splCnt] += sampleOut;
				}
			}

			if (runtime.active_resampling)
			{
				// Check that we do not fail into wrong buffer part
				assert((runtime.fFieldRebuffer - numOutSamples) >= 0);

				// Copy the samples that are left over this frame to the first ones in the next frame!!
				for (cnt = 0; cnt < runtime.nCFieldRebuffer; cnt++)
				{
					jvxData* targetOut = (jvxData*)runtime.bufsConvert_data->ptrFieldBuffer[cnt];
					jvxData* sourceOut = (jvxData*)runtime.bufsConvert_data->ptrFieldBuffer[cnt];
					sourceOut += numOutSamples;
					for (splCnt = 0; splCnt < (runtime.fFieldRebuffer - numOutSamples); splCnt++)
					{
						jvxData sampleOut = sourceOut[splCnt];
						targetOut[splCnt] = sampleOut;
					}
				}

				// update the fillheight
				runtime.fFieldRebuffer -= numOutSamples;
				assert(runtime.fFieldRebuffer >= 0);
			}
			break;
		}
	}
	else
	{
		if (runtime.active_resampling)
		{
			// Simple case: just resample straight away
			for (i = 0; i < runtime.numResampler; i++)
			{
				jvx_fixed_resampler_process(&runtime.fldResampler[i], bufsInData[i], bufsOutData[i], nullptr); // Full size resampling
			}
		}
		else
		{
			if (runtime.active_retype_output)
			{
				// We can only reach this location if:
				// 1) no input conversion
				// 2) no rebuffering
				// 3) no resampling
				// Then, the only operation will be to convert on the output side!!
				
				// Relocate the inpout buffer for conversion on output side
				bufsOutData = bufsInData;
			}
			else 
			{
				// We can only reach this location if:
				// 1) no input conversion
				// 2) no rebuffering
				// 3) no resampling
				// 4) no output conversion
				// This may only happen if a zerocopy operation is desired!!

				assert(0); // <- this case should have been covered by zerocopy processing!!
			}
		}
	}

	// ===============================================================================
	// Output conversion: Change type of audio samples.
	// ===============================================================================

	if (runtime.active_retype_output)
	{
		jvxSize cnt = 0;

		// Note that <bufsOutData> might be different buffer locations depending on history!!
		switch (_common_set_ocon.theData_out.con_params.format)
		{
		case JVX_DATAFORMAT_16BIT_LE:
			for (cnt = 0; cnt < _common_set_ocon.theData_out.con_params.number_channels; cnt++)
			{
				jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt16>(
					bufsOutData[cnt],
					bufsOutInt16[cnt], numOutSamples, JVX_MAX_INT_16_DATA);
			}
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			for (cnt = 0; cnt < _common_set_ocon.theData_out.con_params.number_channels; cnt++)
			{
				jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt32>(
					bufsOutData[cnt],
					bufsOutInt32[cnt], numOutSamples, JVX_MAX_INT_32_DATA);
			}

			break;
		default:
			assert(0);
		}
	}	

exit_fwd:
	return fwd_process_buffers_icon(mt_mask, idx_stage);
	
}

// ====================================================================================
// ====================================================================================

jvxErrorType 
CjvxAuNConvert::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	jvxErrorType res = CjvxBareNode1ioRearrange::put_configuration(callMan, processor, sectionToContainAllSubsectionsForMe, filename, lineno);
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		genConvert_node::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe);
		currNegoStat.fixedLocationMode = genConvert_node::translate__config__fixed_rate_location_mode_from(0);
		resamplerQuality = genConvert_node::translate__config__resampler_quality_from(0);
	}
	return res;
}

jvxErrorType 
CjvxAuNConvert::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = JVX_NO_ERROR;

	genConvert_node::translate__config__fixed_rate_location_mode_to(currNegoStat.fixedLocationMode);
	genConvert_node::translate__config__resampler_quality_to(resamplerQuality);

	res = CjvxBareNode1ioRearrange::get_configuration(callMan, processor, sectionWhereToAddAllSubsections);
	genConvert_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	
	return res;
}


JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNConvert, set_config)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genConvert_node::config.fixed_rate_location_mode))
	{
		currNegoStat.fixedLocationMode = genConvert_node::translate__config__fixed_rate_location_mode_from(0);
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genConvert_node::config.resampler_quality))
	{
		resamplerQuality = genConvert_node::translate__config__resampler_quality_from(0);
	}

	return JVX_NO_ERROR;
}

