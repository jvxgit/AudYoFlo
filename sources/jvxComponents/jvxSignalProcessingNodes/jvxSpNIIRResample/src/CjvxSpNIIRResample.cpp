#include "CjvxSpNIIRResample.h"
#include "jvx-helpers-cpp.h"

#include <cstring>

// =====================================================================================
// =====================================================================================

CjvxSpNIIRResample::CjvxSpNIIRResample(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE);
	_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;
}

CjvxSpNIIRResample::~CjvxSpNIIRResample()
{
}

jvxErrorType
CjvxSpNIIRResample::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		genIIRResample_node::init_all();
		genIIRResample_node::allocate_all();
		genIIRResample_node::register_all(this);
		genIIRResample_node::associate__resampling(this,
			&currNegoStat.resampling.cc.oversamplingFactor, 1,
			&currNegoStat.resampling.cc.downsamplingFactor, 1);
		genIIRResample_node::register_callbacks(this, set_config, this);

		currNegoStat.fixedLocationMode = genIIRResample_node::translate__config__fixed_rate_location_mode_from(0);
		filterTechnology = genIIRResample_node::translate__config__filter_technology_from(0);

		currNegoStat.resampling.cc.oversamplingFactor = 1;
		currNegoStat.resampling.cc.downsamplingFactor = 1;
	}
	return res;
}

jvxErrorType
CjvxSpNIIRResample::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genIIRResample_node::unregister_all(this);
		genIIRResample_node::deallocate_all();

		CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

// =====================================================================================
// Negotiation: delegate the generic rate/buffersize/format/channel matching to
// CjvxInputOutputSettings, exactly as done by other converter-style nodes.
// =====================================================================================

jvxErrorType
CjvxSpNIIRResample::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_ERROR_REQUEST_CALL_AGAIN;

	currNegoStat.resetOneTest(currNegoStat.fixedLocationMode);
	currNegoStat.passFromPredecessor(_common_set_icon.theData_in);

	while (res == JVX_ERROR_REQUEST_CALL_AGAIN)
	{
		res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

jvxErrorType
CjvxSpNIIRResample::accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	currNegoStat.acceptInputParams(_common_set_icon.theData_in, nullptr);
	currNegoStat.inputToOutput();
	return JVX_NO_ERROR;
}

void
CjvxSpNIIRResample::test_set_output_parameters()
{
	currNegoStat.prepareForForward(_common_set_ocon.theData_out);
}

void
CjvxSpNIIRResample::from_input_to_output()
{
	// Override to do nothing: samplerate/buffersize on the output side are derived from
	// currNegoStat (which reflects what has actually been negotiated with predecessor and
	// successor), not a plain copy of the input parameters.
}

jvxErrorType
CjvxSpNIIRResample::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
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
CjvxSpNIIRResample::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return JVX_NO_ERROR;
}

// =====================================================================================
// Connect: determine decimation / interpolation mode from the negotiated rate ratio and
// design the IIR lowpass filter accordingly.
// =====================================================================================

jvxErrorType
CjvxSpNIIRResample::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	runtime.active_resampling = false;
	runtime.mode_decimate = false;
	runtime.mode_interpolate = false;
	runtime.bufIntermediate = nullptr;
	runtime.lenIntermediate = 0;
	runtime.numChannels = 0;

	// This is a pure signal processing node: it operates on already-converted jvxData
	// samples with a fixed number of channels. Channel rearrangement or sample type
	// conversion is expected to be handled by a neighboring converter node.
	_common_set_ldslave.zeroCopyBuffering_cfg = false;

	if (node_inout._common_set_node_params_a_1io.number_channels != node_output._common_set_node_params_a_1io.number_channels)
	{
		res = JVX_ERROR_INVALID_SETTING;
	}
	if ((res == JVX_NO_ERROR) &&
		((node_inout._common_set_node_params_a_1io.format != JVX_DATAFORMAT_DATA) ||
		 (node_output._common_set_node_params_a_1io.format != JVX_DATAFORMAT_DATA)))
	{
		res = JVX_ERROR_INVALID_FORMAT;
	}

	if (res == JVX_NO_ERROR)
	{
		res = CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	if (res == JVX_NO_ERROR)
	{
		runtime.numChannels = node_output._common_set_node_params_a_1io.number_channels;

		jvxSize L = currNegoStat.resampling.cc.oversamplingFactor;
		jvxSize M = currNegoStat.resampling.cc.downsamplingFactor;
		jvxSize bufIn = node_inout._common_set_node_params_a_1io.buffersize;
		jvxSize bufOut = node_output._common_set_node_params_a_1io.buffersize;

		// This component assumes a fixed integer sample relation between input and output
		// (the caller is responsible for ensuring only integer conversion ratios are used) -
		// no variable-framesize rebuffering for fractional remainders is implemented.
		if ((bufIn * L) != (bufOut * M))
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		else if ((M == 1) && (L == 1))
		{
			// Identical rate: plain passthrough, no filter required.
		}
		else if (M == 1)
		{
			runtime.mode_interpolate = true;
			runtime.active_resampling = true;
		}
		else if (L == 1)
		{
			runtime.mode_decimate = true;
			runtime.active_resampling = true;
		}
		else
		{
			// A combined rational conversion (interpolation and decimation at once, e.g.
			// 44.1 kHz <-> 48 kHz) is out of scope for this low-latency resampler, which
			// targets simple integer up-/downsampling such as 48 kHz <-> 16 kHz.
			res = JVX_ERROR_INVALID_SETTING;
		}
	}

	if ((res == JVX_NO_ERROR) && runtime.active_resampling)
	{
		res = setup_filter();
	}

	return res;
}

jvxErrorType
CjvxSpNIIRResample::setup_filter()
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxSize rateIn = node_inout._common_set_node_params_a_1io.samplerate;
	jvxSize rateOut = node_output._common_set_node_params_a_1io.samplerate;

	// Run the filter at the higher of the two rates, i.e. before decimating or after
	// interpolating, and place the cutoff below the Nyquist frequency of the LOWER rate
	// to suppress aliasing (decimation) or imaging (interpolation) artifacts.
	jvxSize rateDesign = runtime.mode_decimate ? rateIn : rateOut;
	jvxData nyquistNew = (jvxData)JVX_MIN(rateIn, rateOut) / 2.0;
	jvxData fc = genIIRResample_node::config.filter_cutoff_scale.value * nyquistNew;

	std::string tokenTech;
	switch (filterTechnology)
	{
	case jvxIIRResamplerFilterTechnology::JVX_IIR_RESAMPLER_FILTER_CHEBYSHEV1:
		tokenTech = "ChebyshevI";
		break;
	case jvxIIRResamplerFilterTechnology::JVX_IIR_RESAMPLER_FILTER_CHEBYSHEV2:
		tokenTech = "ChebyshevII";
		break;
	case jvxIIRResamplerFilterTechnology::JVX_IIR_RESAMPLER_FILTER_ELLIPTIC:
		tokenTech = "Elliptic";
		break;
	case jvxIIRResamplerFilterTechnology::JVX_IIR_RESAMPLER_FILTER_BUTTERWORTH:
	default:
		tokenTech = "Butterworth";
		break;
	}

	res = runtime.filterObj.initialize(tokenTech, "LowPass",
		genIIRResample_node::config.filter_order.value,
		rateDesign, fc,
		genIIRResample_node::config.filter_ripple_db.value,
		genIIRResample_node::config.filter_stopband_db.value,
		0.0, false);

	if (res == JVX_NO_ERROR)
	{
		runtime.lenIntermediate = runtime.mode_decimate ?
			node_inout._common_set_node_params_a_1io.buffersize :
			node_output._common_set_node_params_a_1io.buffersize;

		res = runtime.filterObj.prepare(runtime.numChannels, runtime.lenIntermediate);
	}

	if (res == JVX_NO_ERROR)
	{
		JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(runtime.bufIntermediate, jvxData, runtime.numChannels, runtime.lenIntermediate);
	}

	return res;
}

jvxErrorType
CjvxSpNIIRResample::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	if (runtime.active_resampling)
	{
		JVX_SAFE_DELETE_2DFIELD(runtime.bufIntermediate, runtime.numChannels);
		runtime.filterObj.postprocess();
		runtime.filterObj.terminate();
	}

	runtime.active_resampling = false;
	runtime.mode_decimate = false;
	runtime.mode_interpolate = false;
	runtime.lenIntermediate = 0;
	runtime.numChannels = 0;

	return res;
}

// =====================================================================================
// Processing
// =====================================================================================

jvxErrorType
CjvxSpNIIRResample::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize c, i;

	jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);

	jvxSize numIn = node_inout._common_set_node_params_a_1io.buffersize;
	jvxSize numOut = node_output._common_set_node_params_a_1io.buffersize;

	if (!runtime.active_resampling)
	{
		// Identical samplerate: plain passthrough
		for (c = 0; c < runtime.numChannels; c++)
		{
			memcpy(bufsOut[c], bufsIn[c], numOut * sizeof(jvxData));
		}
	}
	else if (runtime.mode_decimate)
	{
		jvxSize M = currNegoStat.resampling.cc.downsamplingFactor;

		// Lowpass filter at the full input rate, then decimate by picking every M-th sample.
		// As buffersizes are constrained to be exact multiples of M, the decimation phase is
		// aligned to 0 at every frame boundary and no cross-frame phase state is required.
		runtime.filterObj.process((const jvxData**)bufsIn, runtime.bufIntermediate, runtime.numChannels, numIn);

		for (c = 0; c < runtime.numChannels; c++)
		{
			jvxData* src = runtime.bufIntermediate[c];
			jvxData* dst = bufsOut[c];
			for (i = 0; i < numOut; i++)
			{
				dst[i] = src[i * M];
			}
		}
	}
	else // runtime.mode_interpolate
	{
		jvxSize L = currNegoStat.resampling.cc.oversamplingFactor;
		jvxData gain = (jvxData)L;

		// Zero-stuff by L (scaling the inserted samples by L to restore the passband gain),
		// then lowpass filter at the full output rate to suppress spectral images. The IIR
		// filter carries its state across frames, so interpolation phase stays continuous.
		for (c = 0; c < runtime.numChannels; c++)
		{
			jvxData* src = bufsIn[c];
			jvxData* dst = runtime.bufIntermediate[c];
			memset(dst, 0, runtime.lenIntermediate * sizeof(jvxData));
			for (i = 0; i < numIn; i++)
			{
				dst[i * L] = src[i] * gain;
			}
		}

		runtime.filterObj.process((const jvxData**)runtime.bufIntermediate, bufsOut, runtime.numChannels, numOut);
	}

	return _process_buffers_icon(mt_mask, idx_stage);
}

// =====================================================================================
// Configuration
// =====================================================================================

jvxErrorType
CjvxSpNIIRResample::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::put_configuration(callMan, processor, sectionToContainAllSubsectionsForMe, filename, lineno);
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		genIIRResample_node::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe);
		currNegoStat.fixedLocationMode = genIIRResample_node::translate__config__fixed_rate_location_mode_from(0);
		filterTechnology = genIIRResample_node::translate__config__filter_technology_from(0);
	}
	return res;
}

jvxErrorType
CjvxSpNIIRResample::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = JVX_NO_ERROR;

	genIIRResample_node::translate__config__fixed_rate_location_mode_to(currNegoStat.fixedLocationMode);
	genIIRResample_node::translate__config__filter_technology_to(filterTechnology);

	res = CjvxBareNode1ioRearrange::get_configuration(callMan, processor, sectionWhereToAddAllSubsections);
	genIIRResample_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);

	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNIIRResample, set_config)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genIIRResample_node::config.fixed_rate_location_mode))
	{
		currNegoStat.fixedLocationMode = genIIRResample_node::translate__config__fixed_rate_location_mode_from(0);
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genIIRResample_node::config.filter_technology))
	{
		filterTechnology = genIIRResample_node::translate__config__filter_technology_from(0);
	}

	return JVX_NO_ERROR;
}
