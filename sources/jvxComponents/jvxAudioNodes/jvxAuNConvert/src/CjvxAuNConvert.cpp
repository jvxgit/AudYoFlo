#include "CjvxAuNConvert.h"
#include "jvx-helpers-cpp.h"
#include "jvx_math/jvx_math_extensions.h"

CjvxAuNConvert::CjvxAuNConvert(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE);
	_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;
	// outputArgsFromOutputParams = true;
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
		genConvert_node::associate__resampling(this, &resampling.cc.oversamplingFactor, 1, &resampling.cc.downsamplingFactor, 1, &resampling.loopNum, 1);

		// genChannelRearrange_node::register_callbacks(this, get_level_pre, get_level_post, set_passthru, this);
		resampling.cc.downsamplingFactor = 1;
		resampling.cc.oversamplingFactor = 1;
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
	jvxErrorType res = JVX_NO_ERROR;
	
	// If we change the number of input channels we need to also adapt the number of outputchannels to let the output side know that we could provide MORE
	if (_common_set_icon.theData_in->con_params.number_channels != reChannel.numIn)
	{
		reChannel.numIn = _common_set_icon.theData_in->con_params.number_channels;
		reChannel.numOut = reChannel.numIn;
	}
	res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		auto mode = genConvert_node::translate__config__fixed_rate_location_mode_from();
		switch (mode)
		{
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
			genConvert_node::resampling.real_rate.value = (jvxData)node_output._common_set_node_params_a_1io.samplerate * (jvxData)resampling.cc.downsamplingFactor / (jvxData)resampling.cc.oversamplingFactor;
			break;
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
			genConvert_node::resampling.real_rate.value = (jvxData)node_inout._common_set_node_params_a_1io.samplerate * (jvxData)resampling.cc.oversamplingFactor / (jvxData)resampling.cc.downsamplingFactor;
			break;
		default: 
			break;
		}		
	}
	return res;
}

static jvxSize commonSamplerates[] =
{
	8000,
	11025,
	16000,
	22050,
	32000,
	44100,
	48000,
	0
};

jvxSize jvx_roundSamplerate(jvxData srateFracIn)
{
	jvxSize retVal = JVX_SIZE_UNSELECTED;
	int cnt = 0;
	jvxData ddelta = JVX_DATA_MAX_POS;
	while (1)
	{
		if (commonSamplerates[cnt] == 0)
		{
			break;
		}

		jvxData diff = JVX_ABS(srateFracIn - commonSamplerates[cnt]);
		if (diff < ddelta)
		{
			retVal = commonSamplerates[cnt];
			ddelta = diff;
		}
		cnt++;
	}
	return retVal;
}

void
CjvxAuNConvert::from_input_to_output()
{
	CjvxBareNode1ioRearrange::from_input_to_output();
	
	// Check the relation input/output
	adapt_output_parameters();
}

void
CjvxAuNConvert::adapt_output_parameters()
{
	// Check if there is a new value in rate or buffersize - if so, reset num/den
	node_output._common_set_node_params_a_1io.buffersize = ceil((jvxData)node_inout._common_set_node_params_a_1io.buffersize * (jvxData) resampling.cc.oversamplingFactor / (jvxData)resampling.cc.downsamplingFactor);
	node_output._common_set_node_params_a_1io.samplerate = node_inout._common_set_node_params_a_1io.samplerate * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
	node_output._common_set_node_params_a_1io.number_channels = reChannel.numOut;

	neg_output._update_parameters_fixed(
		JVX_SIZE_UNSELECTED,
		node_output._common_set_node_params_a_1io.buffersize,
		node_output._common_set_node_params_a_1io.samplerate);
}

jvxErrorType 
CjvxAuNConvert::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNConvert::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// First, try to activate the setting as is:
	jvxLinkDataDescriptor tryThis = *_common_set_icon.theData_in;

	// Try to align number channels
	reChannel.numIn = tryThis.con_params.number_channels;
	reChannel.numOut = preferredByOutput->con_params.number_channels;

	if (tryThis.con_params.number_channels != preferredByOutput->con_params.number_channels)
	{
		tryThis.con_params.number_channels = preferredByOutput->con_params.number_channels;
		res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryThis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res == JVX_NO_ERROR)
		{
			reChannel.numIn = _common_set_icon.theData_in->con_params.number_channels;
		}
	}

	// Next we deal with buffersize/samperates

	// First, try to just activate the output desire - which typically does not really work
	tryThis = *preferredByOutput;
	res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryThis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	jvxSize cnt = 0;
	jvxBool runLoop = true;
	while(runLoop)
	{
		switch (res)
		{
		case JVX_NO_ERROR:
			runLoop = false;
			break;
		case JVX_ERROR_COMPROMISE:
			// What can we do here?
			assert(0);
			runLoop = false;
			break;
		default:

			// Plan A did fail, now let us try plan B
			tryThis.con_params = _common_set_icon.theData_in->con_params;
			res = computeResamplerOperation(
				node_inout._common_set_node_params_a_1io.samplerate,
				preferredByOutput->con_params.rate,
				node_inout._common_set_node_params_a_1io.buffersize,
				preferredByOutput->con_params.buffersize, cnt++, tryThis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			break;
		}

		if (cnt == 2)
		{
			break;
		}
	}

	return res;
		//CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, preferredByOutput JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNConvert::computeResamplerOperation(jvxSize sRateIn, jvxSize sRateOut,
	jvxSize bSizeIn, jvxSize bSizeOut, jvxSize runCnt, 
	jvxLinkDataDescriptor& startRequestSuccessorWiththis
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxData resamplingFacInOut = (jvxData)sRateOut / (jvxData)sRateIn;
	jvxData bSizeInFrac = (jvxData) bSizeOut / resamplingFacInOut;
	jvxSize bSizeInFwd = JVX_DATA2SIZE(bSizeInFrac);
	jvxData bsizeOutFrac = 0;
	jvxInt32 gcdVal = 1;	
	jvxSize bSizeOutFwd = 0;

	/*
	 * Optimize the resampler settings
	 */

	switch (runCnt)
	{
	case 0:
		// Try to force the successor to provide another buffersize
		gcdVal = jvx_gcd(bSizeInFwd, bSizeOut);
		startRequestSuccessorWiththis.con_params.buffersize = bSizeInFwd;
		startRequestSuccessorWiththis.con_params.segmentation.x = startRequestSuccessorWiththis.con_params.buffersize;
		res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &startRequestSuccessorWiththis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{

		}
		break;
	case 1:

		resampling.loopNum = 1;
		bsizeOutFrac = (jvxData)bSizeIn * (jvxData)resampling.loopNum * resamplingFacInOut;
		while (bsizeOutFrac - floor(bsizeOutFrac) != 0.0)
		{
			resampling.loopNum++;
			bsizeOutFrac = (jvxData)bSizeIn * (jvxData)resampling.loopNum * resamplingFacInOut;
		}
		bSizeOutFwd = JVX_DATA2SIZE(bsizeOutFrac);		
		jvx_fixed_resampler_init_conversion(&resampling.cc, sRateIn, sRateOut);

		resampling.granularityIn = resampling.cc.downsamplingFactor;
		resampling.granularityOut = resampling.cc.oversamplingFactor;
		
		// Update the output side
		adapt_output_parameters();
		test_set_output_parameters();

		// Keep input as it is and adapt the output arguments
		startRequestSuccessorWiththis.con_params.buffersize = bSizeOutFwd;
		startRequestSuccessorWiththis.con_params.segmentation.x = startRequestSuccessorWiththis.con_params.buffersize;
		res = JVX_ERROR_COMPROMISE;
		break;
	}
	
	if (
		(res == JVX_NO_ERROR) ||
		(res == JVX_ERROR_COMPROMISE))
	{
	}
	return res;
}

jvxErrorType
CjvxAuNConvert::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	// Configure this module to be zerocopy
	_common_set_ldslave.zeroCopyBuffering_cfg = true;
	resampling.active = false;
	reChannel.active = false;
	output.requiresRebuffering = false;
	output.requiresInputGranularityHeadroom = false;
	output.lFieldRebuffer = 0;

	if ((resampling.cc.oversamplingFactor != 1) || (resampling.cc.downsamplingFactor != 1))
	{
		resampling.active = true;
	}
	if (reChannel.numIn != reChannel.numOut)
	{
		reChannel.active = true;
	}
	if (node_inout._common_set_node_params_a_1io.format != node_output._common_set_node_params_a_1io.format)
	{
		reType.active = true;
	}

	if(resampling.active || reChannel.active || reType.active)
	{
		_common_set_ldslave.zeroCopyBuffering_cfg = false;

		output.requiresRebuffering = jvx_bitTest(_common_set_ocon.theData_out.con_data.alloc_flags,
			(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
		
		if (output.requiresRebuffering)
		{
			// In case of variable input size, add headroom space on input side
			output.requiresInputGranularityHeadroom = true;
		}
		else
		{
			// If we have a fixed input / output relation, check if we need a variable framesize
			if (node_output._common_set_node_params_a_1io.buffersize % resampling.cc.oversamplingFactor)
			{
				output.requiresInputGranularityHeadroom = true;
				output.requiresRebuffering = true;
			}
			if (node_inout._common_set_node_params_a_1io.buffersize % resampling.cc.downsamplingFactor)
			{
				output.requiresInputGranularityHeadroom = true;
				output.requiresRebuffering = true;
			}
		}
		if (reChannel.active || reType.active)
		{
			output.requiresRebuffering = true;
		}

		if (output.requiresInputGranularityHeadroom)
		{
			// This module may produce 
			jvx_bitSet(_common_set_ocon.theData_out.con_data.alloc_flags,
				(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
		}
	}

	res = CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (resampling.active)
		{
			jvxDataFormat formOut = (jvxDataFormat)node_inout._common_set_node_params_a_1io.format;
			resampling.numResampler = node_output._common_set_node_params_a_1io.number_channels;

			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(resampling.fldResampler, jvx_fixed_resampler, resampling.numResampler);
			for (i = 0; i < resampling.numResampler; i++)
			{
				jvx_fixed_resampler_initConfig(&resampling.fldResampler[i]);
				resampling.fldResampler[i].prmInit.buffersizeIn = node_inout._common_set_node_params_a_1io.buffersize;
				resampling.fldResampler[i].prmInit.buffersizeOut = node_output._common_set_node_params_a_1io.buffersize;
				resampling.fldResampler[i].prmInit.format = formOut;
				resampling.fldResampler[i].prmInit.lFilter = 32;
				resampling.fldResampler[i].prmInit.optPtrConversion = &resampling.cc; // Provide the resampling exact config
				jvx_fixed_resampler_prepare(&resampling.fldResampler[i]);
			}
		}

		if (output.requiresRebuffering)
		{
			if (output.requiresInputGranularityHeadroom)
			{
				output.lFieldRebuffer = node_inout._common_set_node_params_a_1io.buffersize + resampling.granularityIn;
			}
			else
			{
				output.lFieldRebuffer = node_inout._common_set_node_params_a_1io.buffersize;
			}

			output.fFieldRebuffer = 0;
			output.nCFieldRebuffer = node_output._common_set_node_params_a_1io.number_channels;
			output.ptrFieldBuffer = nullptr;
			output.lFieldRebufferChannel = output.lFieldRebuffer * jvxDataFormat_getsize(node_output._common_set_node_params_a_1io.format);
			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(output.ptrFieldBuffer, jvxHandle*, output.nCFieldRebuffer);
			for (i = 0; i < output.nCFieldRebuffer; i++)
			{
				JVX_SAFE_ALLOCATE_FIELD_CPP_Z(output.ptrFieldBuffer[i], jvxByte, output.lFieldRebufferChannel);
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

	if (resampling.active)
	{
		for (i = 0; i < resampling.numResampler; i++)
		{
			jvx_fixed_resampler_postprocess(&resampling.fldResampler[i]);
		}
		JVX_SAFE_DELETE_FIELD(resampling.fldResampler);
		resampling.numResampler = 0;
	}

	if (output.requiresRebuffering)
	{
		for (i = 0; i < output.nCFieldRebuffer; i++)
		{
			JVX_SAFE_DELETE_FIELD(output.ptrFieldBuffer[i]);
		}
		JVX_SAFE_DELETE_FIELD(output.ptrFieldBuffer);

		output.fFieldRebuffer = 0;
		output.nCFieldRebuffer = 0;
		output.ptrFieldBuffer = nullptr;
		output.lFieldRebufferChannel = 0;
		output.lFieldRebuffer = 0;

	}

	output.requiresRebuffering = false;
	output.requiresInputGranularityHeadroom = false;
	
	resampling.active = false;
	reType.active = false;
	reChannel.active = false;

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNConvert::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numInputSamples = _common_set_icon.theData_in->con_params.buffersize;
	jvxSize numOutputSamples = 0;

	if(zeroCopyBuffering_rt)
	{ 
		return CjvxBareNode1ioRearrange::process_buffers_icon(mt_mask, idx_stage);
	}

	jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
	jvxSize idx_stage_local = jvx_process_icon_extract_stage(_common_set_icon.theData_in, idx_stage);

	if (JVX_CHECK_SIZE_SELECTED(_common_set_icon.theData_in->con_data.fHeights[idx_stage_local].x))
	{
		numInputSamples = _common_set_icon.theData_in->con_data.fHeights[idx_stage_local].x;
	}
	
	if (output.requiresRebuffering)
	{
		jvxSize cnt = 0;
		jvxSize splCnt = 0;
		if (reChannel.active || reType.active)
		{
			jvxSize loopNum = JVX_MAX(output.nCFieldRebuffer,
				_common_set_icon.theData_in->con_params.number_channels);

			// Set the output to 0 first
			for (cnt = 0; cnt < output.nCFieldRebuffer; cnt++)
			{
				jvxData* targetIn = (jvxData*)output.ptrFieldBuffer[cnt];
				targetIn += output.fFieldRebuffer;

				jvxSize splCnt = 0;
				for (; splCnt < numInputSamples; splCnt++)
				{
					targetIn[splCnt] = 0;
				}
			}

			for (cnt = 0; cnt < loopNum; cnt++)
			{
				jvxSize inCnt = cnt % _common_set_icon.theData_in->con_params.number_channels;
				jvxSize outCnt = cnt % output.nCFieldRebuffer;

				jvxData* sourceIn = (jvxData*)bufsIn[inCnt];

				jvxData* targetIn = (jvxData*)output.ptrFieldBuffer[outCnt];
				targetIn += output.fFieldRebuffer;

				
				for (splCnt = 0; splCnt < numInputSamples; splCnt++)
				{
					jvxData sampleIn = sourceIn[splCnt];
					targetIn[splCnt] += sampleIn;
				}
			}
		}
		output.fFieldRebuffer += numInputSamples;

		jvxSize numResamplerIn = output.fFieldRebuffer / resampling.cc.downsamplingFactor * resampling.cc.downsamplingFactor; // Find multiples of input granularity
		jvxSize numResamplerOut = numResamplerIn * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
		assert(numResamplerOut <= _common_set_ocon.theData_out.con_params.buffersize);

		for (i = 0; i < resampling.numResampler; i++)
		{
			fixed_resample_variable_out varOut;
			varOut.bsizeIn = numResamplerIn;
			varOut.numOut = numResamplerOut;

			// Important: use the "new" input buffer
			jvx_fixed_resampler_process(&resampling.fldResampler[i], output.ptrFieldBuffer[i], bufsOut[i], &varOut); // Full size resampling
		}
		idx_stage_local = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;

		_common_set_ocon.theData_out.con_data.fHeights[idx_stage_local].x = numResamplerOut;

		jvxSize numCopyNextFrame = output.fFieldRebuffer - numResamplerIn;
		for (cnt = 0; cnt < output.nCFieldRebuffer; cnt++)
		{
			jvxData* cpyTo = (jvxData*)output.ptrFieldBuffer[cnt];
			jvxData* cpyFrom = cpyTo + numResamplerIn;

			for (splCnt = 0; splCnt < numCopyNextFrame; splCnt++)
			{
				*cpyTo = *cpyFrom;
				cpyTo++;
				cpyFrom++;
			}
		}
		output.fFieldRebuffer -= numResamplerIn;
	}
	else
	{
		// Simple case: just resample straight away
		for (i = 0; i < resampling.numResampler; i++)
		{
			jvx_fixed_resampler_process(&resampling.fldResampler[i], bufsIn[i], bufsOut[i], nullptr); // Full size resampling
		}
	}
	return fwd_process_buffers_icon(mt_mask, idx_stage);
	
}

/*


// ===================================================================
jvxErrorType
CjvxAuNChannelRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res == JVX_NO_ERROR)
	{
	}
	return res;
}

// ===================================================================
// ===========================================================================

jvxErrorType
CjvxAuNChannelRearrange::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{

	if (!_common_set_ocon.setup_for_termination)
	{
		jvxData** fieldInput = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
		jvxData** fieldOutput = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
		jvxBool passThrough = (genChannelRearrange_node::passthrough.active.value == c_true);
		// Talkthrough
		jvxSize ii;
		jvxSize inChans = _common_set_icon.theData_in->con_params.number_channels;
		jvxSize outChans = _common_set_ocon.theData_out.con_params.number_channels;
		if (passThrough)
		{
			inChans = JVX_MAX((int)inChans - 1, 0);
			outChans = JVX_MAX((int)outChans - 1, 0);
		}
		jvxSize maxChans = JVX_MAX(inChans, outChans);
		jvxSize minChans = JVX_MIN(inChans, outChans);

		// This default function does not tolerate a lot of unexpected settings
		assert(_common_set_icon.theData_in->con_params.format == _common_set_ocon.theData_out.con_params.format);
		assert(_common_set_icon.theData_in->con_params.buffersize == _common_set_ocon.theData_out.con_params.buffersize);

		if (minChans)
		{
			for (ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % inChans;
				jvxSize idxOut = ii % outChans;

				assert(_common_set_icon.theData_in->con_params.format == _common_set_ocon.theData_out.con_params.format);
				jvx_convertSamples_memcpy(
					_common_set_icon.theData_in->con_data.buffers[
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][idxIn],
							_common_set_ocon.theData_out.con_data.buffers[
								*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][idxOut],
									jvxDataFormat_size[_common_set_icon.theData_in->con_params.format],
									_common_set_icon.theData_in->con_params.buffersize);
			}
		}
		if (passThrough)
		{
			if ((inChans > 0) && (outChans > 0))
			{
				memcpy(fieldOutput[outChans], fieldInput[inChans], jvxDataFormat_getsize(_common_set_icon.theData_in->con_params.format) * _common_set_icon.theData_in->con_params.buffersize);
			}
		}
		return fwd_process_buffers_icon(mt_mask, idx_stage);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNChannelRearrange::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		// Forward call to base class
		return CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;

	default:
		res = CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxAuNChannelRearrange::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1ioRearrange::put_configuration(callMan,
		processor, sectionToContainAllSubsectionsForMe,
		filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			genChannelRearrange_node::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe);
		}
	}
	return res;
}

jvxErrorType
CjvxAuNChannelRearrange::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1ioRearrange::get_configuration(callMan, processor, sectionWhereToAddAllSubsections);;
	if (res == JVX_NO_ERROR)
	{
		genChannelRearrange_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNChannelRearrange, get_level_pre)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNChannelRearrange, get_level_post)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNChannelRearrange, set_passthru)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genChannelRearrange_node::passthrough.active))
	{
		// Actually, we need not do anything here, this macro is considered only in the procesing function
	}
	return JVX_NO_ERROR;
}
*/