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
		genConvert_node::associate__resampling(this, &resampling.num, 1, &resampling.den, 1, &resampling.loopNum, 1);

		// genChannelRearrange_node::register_callbacks(this, get_level_pre, get_level_post, set_passthru, this);
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
			genConvert_node::resampling.real_rate.value = (jvxData)node_output._common_set_node_params_a_1io.samplerate * (jvxData)resampling.den / (jvxData)resampling.num;
			break;
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
			genConvert_node::resampling.real_rate.value = (jvxData)node_inout._common_set_node_params_a_1io.samplerate * (jvxData)resampling.num / (jvxData)resampling.den;
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
	node_output._common_set_node_params_a_1io.buffersize = resampling.loopNum *  node_inout._common_set_node_params_a_1io.buffersize * resampling.num / resampling.den;
	node_output._common_set_node_params_a_1io.samplerate = node_inout._common_set_node_params_a_1io.samplerate * resampling.num / resampling.den;
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
	while(1)
	{
		switch (res)
		{
		case JVX_NO_ERROR:
			runLoop = false;
			break;
		case JVX_ERROR_COMPROMISE:
			// What can we do here?
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
		gcdVal = jvx_gcd(resampling.loopNum *bSizeIn, bSizeOutFwd);
		resampling.num = bSizeOutFwd / gcdVal;
		resampling.den = resampling.loopNum * bSizeIn / gcdVal;

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
	if ((resampling.num != 1) || (resampling.den != 1))
	{
		_common_set_ldslave.zeroCopyBuffering_cfg = false;
		resampling.active = true;
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
				jvx_fixed_resampler_prepare(&resampling.fldResampler[i]);
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
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNConvert::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if(zeroCopyBuffering_rt)
	{ 
		return CjvxBareNode1ioRearrange::process_buffers_icon(mt_mask, idx_stage);
	}

	jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);

	for(i = 0; i < resampling.numResampler; i++)
	{ 
		jvx_fixed_resampler_process(&resampling.fldResampler[i], bufsIn[i], bufsOut[i]);
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