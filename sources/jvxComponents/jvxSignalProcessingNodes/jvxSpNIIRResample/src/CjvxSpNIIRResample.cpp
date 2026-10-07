#include "CjvxSpNIIRResample.h"
#include "jvx-helpers-cpp.h"

#include <cstring>

struct oneDesign
{
	jvxSize order = 12;
	std::string design = "Elliptic";
	jvxData stopDbRoll = -3;
	jvxData freqFac = 0.9313;
	jvxData rip = 0.05;
	jvxData freqWidth = 0;
};

oneDesign designUseCase[(int)jvxIIRResamplerFilterPresets::JVX_IIR_RESAMPLER_PRESET_LIMIT] =
{
	{6, "Butterworth", 0, 0.6, 0.0, 0.0} /*JVX_IIR_RESAMPLER_PRESET_BUTTER_LOW_QUALITY*/,
	{9, "Butterworth", 0, 0.68, 0.0, 0.0} /*JVX_IIR_RESAMPLER_PRESET_BUTTER_MEDIUM_QUALITY*/,
	{12, "Butterworth", 0, 0.75, 0.0, 0.0} /*JVX_IIR_RESAMPLER_PRESET_BUTTER_HIGH_QUALITY*/,
	
	{6, "Elliptic", -3, 0.9313, 0.05, 0.0} /*JVX_IIR_RESAMPLER_PRESET_ELLIPTIC_LOW_QUALITY*/,
	{8, "Elliptic", -3, 0.9313, 0.05, 0.0} /*JVX_IIR_RESAMPLER_PRESET_ELLIPTIC_MEDIUM_QUALITY*/,
	{12, "Elliptic", -3, 0.9313, 0.05, 0.0} /*JVX_IIR_RESAMPLER_PRESET_ELLIPTIC_HIGH_QUALITY*/,
	
	{6, "ChebyshevI", 40, 0.75, 0.25, 0.0} /*JVX_IIR_RESAMPLER_PRESET_CHEBYI_LOW_QUALITY*/,
	{9, "ChebyshevI", 40, 0.83, 0.05, 0.0} /*JVX_IIR_RESAMPLER_PRESET_CHEBYI_MEDIUM_QUALITY*/,
	{12, "ChebyshevI", 50, 0.89, 0.05, 0.0} /*JVX_IIR_RESAMPLER_PRESET_CHEBYI_HIGH_QUALITY*/,

	{6, "ChebyshevII", 30, 01, 0.05, 0.0} /*JVX_IIR_RESAMPLER_PRESET_CHEBYII_LOW_QUALITY*/,
	{9, "ChebyshevII", 40, 1, 0.05, 0.0} /*JVX_IIR_RESAMPLER_PRESET_CHEBYII_MEDIUM_QUALITY*/,
	{12, "ChebyshevII", 50, 1, 0.05, 0.0} /*JVX_IIR_RESAMPLER_PRESET_CHEBYII_HIGH_QUALITY*/
};

std::string designFilters[(int)jvxIIRResamplerFilterTechnology::JVX_IIR_RESAMPLER_FILTER_LIMIT] =
{
	"Butterworth",
	"Elliptic",
	"ChebyshevI",
	"ChebyshevII"
};

// =====================================================================================
// =====================================================================================

CjvxSpNIIRResample::CjvxSpNIIRResample(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE);
	_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;

	// Force the numbers of channels input/output to MATCH!!
	currNegoStat.force.channelsIdenticalInOut = true;
	currNegoStat.force.formatIdenticalInOut = true;
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
		//
		genIIRResample_node::associate__resampling(this,
			&currNegoStat.resampling.cc.oversamplingFactor, 1,
			&currNegoStat.resampling.cc.downsamplingFactor, 1);
		genIIRResample_node::register_callbacks(this, set_mode, set_config, set_preset, this);

		currNegoStat.fixedLocationMode = genIIRResample_node::translate__config__fixed_rate_location_mode_from(0);
		activate_preset(genIIRResample_node::translate__config__filter_presets_from(), false);

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
	jvxRateLocationMode locMode = genIIRResample_node::translate__config__fixed_rate_location_mode_from();
	if (locMode == jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT)
	{
		// We force the samplerate to a specific value - to prevent that a previous module does the resampling
		//neg_input._update_parameters_fixed(JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, genIIRResample_node::config.force_samplerate.value);
	}
	else
	{
		//neg_output._update_parameters_fixed(JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, genIIRResample_node::config.force_samplerate.value);
	}

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

		// Rate to run resampling filter at
		runtime.srateFilter = currNegoStat.in.rate * currNegoStat.resampling.cc.oversamplingFactor;
		runtime.bsizeFilter = currNegoStat.in.bSize * currNegoStat.resampling.cc.oversamplingFactor;

		configAccepted = true;

		activate_preset(genIIRResample_node::translate__config__filter_presets_from(), false);
	}
	else
	{
		configAccepted = false;
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
	neg_output._update_parameters_fixed(neg_input._latest_results.number_channels,
		JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, neg_input._latest_results.format);

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

	runtime.bufIntermediate = nullptr;
	runtime.lenIntermediate = 0;
	runtime.resampler_involved = false;

	// This is a pure signal processing node: it operates on already-converted jvxData
	// samples with a fixed number of channels. Channel rearrangement or sample type
	// conversion is expected to be handled by a neighboring converter node.
	_common_set_ldslave.zeroCopyBuffering_cfg = false;

	// This should have been solved in the test function!!
	assert(currNegoStat.in.nChans == currNegoStat.out.nChans);
	assert(currNegoStat.in.form == currNegoStat.out.form);
	assert(currNegoStat.in.form == JVX_DATAFORMAT_DATA);

	// assert(_common_set_icon.theData_in->con_params.number_channels == _common_set_ocon.theData_out.con_params.number_channels);
	// assert(_common_set_icon.theData_in->con_params.format == JVX_DATAFORMAT_DATA);
	// assert(_common_set_ocon.theData_out.con_params.format != JVX_DATAFORMAT_DATA);
			
	if ((currNegoStat.resampling.cc.oversamplingFactor == 1) && (currNegoStat.resampling.cc.downsamplingFactor == 1))
	{
		// Setup this component for zero-copy NO overhead
		runtime.resampler_involved = false;
		_common_set_ldslave.zeroCopyBuffering_cfg = true;
	}
	else 
	{
		// If we do any kind of resampling, we need to operate "inplace" and then change buffer
		runtime.resampler_involved = true;

		// At least input and output buffers
		_common_set_ldslave.zeroCopyBuffering_cfg = false;

		// Option 1: in -> filter -> out				|-> Up = 1; Down > 1
		// Option 2: in -> im -> filter -> out 			|-> Up > 1; Down > 1
		// Option 3: in -> out -> filter -> out 		|-> Up > 1; Down = 1	
		jvxBool allocateIntermediate = false;
		if (currNegoStat.resampling.cc.oversamplingFactor == 1)
		{
			runtime.useCase = ayfProcUseCase::AYF_PROCUSE_CASE_1_G1;
		}
		else
		{
			if (currNegoStat.resampling.cc.downsamplingFactor == 1)
			{
				runtime.useCase = ayfProcUseCase::AYF_PROCUSE_CASE_G1_1;
			}
			else
			{
				runtime.useCase = ayfProcUseCase::AYF_PROCUSE_CASE_G1_G1;
				allocateIntermediate = true;
			}
		}

		if (allocateIntermediate)
		{
			runtime.lenIntermediate = currNegoStat.in.bSize * currNegoStat.resampling.cc.oversamplingFactor;
			JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(runtime.bufIntermediate, jvxData, currNegoStat.in.nChans, runtime.lenIntermediate);
		}
	}

	if (res == JVX_NO_ERROR)
	{
		res = CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	if (res == JVX_NO_ERROR)
	{
		if (runtime.resampler_involved)
		{
			res = setup_filter(runtime.filterObj, true);
		}
	}

	return res;
}

jvxErrorType
CjvxSpNIIRResample::setup_filter(CjvxDigitalFilter& filt, jvxBool prepareAlso)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;

	if (configAccepted)
	{
		res = JVX_NO_ERROR;

		jvxData inRate = currNegoStat.in.rate;
		jvxData outRate = currNegoStat.out.rate;
		jvxData edgeFreq = JVX_MIN(inRate, outRate); // runtime.srateFilter;
		edgeFreq /= 2.0;

		// Run the filter at the higher of the two rates, i.e. before decimating or after
		// interpolating, and place the cutoff below the Nyquist frequency of the LOWER rate
		// to suppress aliasing (decimation) or imaging (interpolation) artifacts.
		edgeFreq = genIIRResample_node::config.filter_cutoff_scale.value * edgeFreq;

		jvxIIRResamplerFilterTechnology filterTechnology = genIIRResample_node::translate__config__filter_technology_from();
		std::string tokenTech = "Butterworth";
		if ((int)filterTechnology < (int)jvxIIRResamplerFilterTechnology::JVX_IIR_RESAMPLER_FILTER_LIMIT)
		{
			tokenTech = designFilters[(int)filterTechnology];
		}

		res = filt.initialize(tokenTech, "LowPass",
			genIIRResample_node::config.filter_order.value,
			runtime.srateFilter, edgeFreq,
			genIIRResample_node::config.filter_ripple_db.value,
			genIIRResample_node::config.filter_stopdb_roll.value,
			genIIRResample_node::config.filter_freqwidth.value,
			true);

		if (res == JVX_NO_ERROR)
		{
			if (prepareAlso)
			{
				/*
				runtime.lenIntermediate = runtime.mode_decimate ?
					node_inout._common_set_node_params_a_1io.buffersize :
					node_output._common_set_node_params_a_1io.buffersize;
				*/
				res = runtime.filterObj.prepare(currNegoStat.in.nChans, runtime.bsizeFilter);
			}
		}
	}

	return res;
}

jvxErrorType 
CjvxSpNIIRResample::destroy_filter(CjvxDigitalFilter& filt, jvxBool postprocessAlso)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (postprocessAlso)
	{
		filt.postprocess();
	}
	filt.terminate();
	return res;
}

jvxErrorType
CjvxSpNIIRResample::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	if (runtime.resampler_involved)
	{
		res = destroy_filter(runtime.filterObj, true);
		if (runtime.useCase == ayfProcUseCase::AYF_PROCUSE_CASE_G1_G1)
		{
			JVX_SAFE_DELETE_2DFIELD(runtime.bufIntermediate, currNegoStat.in.nChans);
			runtime.lenIntermediate = 0;
		}		
	}

	runtime.resampler_involved = false;	

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

	if (!runtime.resampler_involved)
	{
		// Do nothing, component is setup for zero-copy mode
	}
	else 
	{
		// Option 1: in -> filter -> out				|-> Up = 1; Down > 1
		// Option 2: in -> im -> filter -> out 			|-> Up > 1; Down > 1
		// Option 3: in -> out -> filter -> out 		|-> Up > 1; Down = 1	
		
		jvxData fac = currNegoStat.resampling.cc.oversamplingFactor;

		switch (runtime.useCase)
		{
		case ayfProcUseCase::AYF_PROCUSE_CASE_1_G1:

			// filter in place -> extract samples
			runtime.filterObj.process_ip(bufsIn, currNegoStat.in.nChans, currNegoStat.in.bSize);

			for (c = 0; c < currNegoStat.out.nChans; c++)
			{
				jvxSize cnt = currNegoStat.resampling.cc.downsamplingFactor - 1;
				for (i = 0; i < currNegoStat.out.bSize; i++)
				{
					bufsOut[c][i] = bufsIn[c][cnt];
					cnt += currNegoStat.resampling.cc.downsamplingFactor;
				}
			}
			break;

		case ayfProcUseCase::AYF_PROCUSE_CASE_G1_G1:

			// in expand to intermediate -> filter inplace intermediate -> extract samples

			// Expand
			for (c = 0; c < currNegoStat.in.nChans; c++)
			{
				jvxSize cnt = 0;
				memset(runtime.bufIntermediate[c], 0, sizeof(jvxData) * runtime.lenIntermediate);
				for (i = 0; i < currNegoStat.in.bSize; i++)
				{
					runtime.bufIntermediate[c][cnt] = bufsIn[c][i] * fac;
					cnt += currNegoStat.resampling.cc.oversamplingFactor;
				}
			}

			// Inplace filter
			runtime.filterObj.process_ip(runtime.bufIntermediate, currNegoStat.in.nChans, runtime.lenIntermediate);

			// To output buffer
			for (c = 0; c < currNegoStat.in.nChans; c++)
			{
				jvxSize cnt = currNegoStat.resampling.cc.downsamplingFactor - 1;
				for (i = 0; i < currNegoStat.out.bSize; i++)
				{
					bufsOut[c][i] = runtime.bufIntermediate[c][cnt];
					cnt += currNegoStat.resampling.cc.downsamplingFactor;
				}
			}
			break;

		case ayfProcUseCase::AYF_PROCUSE_CASE_G1_1:

			// in expand to bufsOut -> filter inplace out 
			// Expand
			for (c = 0; c < currNegoStat.in.nChans; c++)
			{
				jvxSize cnt = 0;
				memset(bufsOut[c], 0, sizeof(jvxData) * currNegoStat.out.bSize);
				for (i = 0; i < currNegoStat.in.bSize; i++)
				{
					bufsOut[c][cnt] = bufsIn[c][i] * fac;
					cnt += currNegoStat.resampling.cc.oversamplingFactor;
				}
			}

			// Inplace filter
			runtime.filterObj.process_ip(bufsOut, currNegoStat.out.nChans, currNegoStat.out.bSize);
			break;
		}
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
		// currNegoStat.fixedLocationMode = genIIRResample_node::translate__config__fixed_rate_location_mode_from(0);
		
		activate_preset(genIIRResample_node::translate__config__filter_presets_from(), false);
	}
	return res;
}

jvxErrorType
CjvxSpNIIRResample::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = JVX_NO_ERROR;

	// genIIRResample_node::translate__config__fixed_rate_location_mode_to(currNegoStat.fixedLocationMode);
	// genIIRResample_node::translate__config__filter_technology_to(filterTechnology);

	res = CjvxBareNode1ioRearrange::get_configuration(callMan, processor, sectionWhereToAddAllSubsections);
	genIIRResample_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);

	return res;
}

jvxErrorType 
CjvxSpNIIRResample::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (suc)
	{
		*suc = true;

		// if(currNegoStat.resampling.cc.downsamplingFactor)
		jvxSize L = currNegoStat.resampling.cc.oversamplingFactor;
		jvxSize M = currNegoStat.resampling.cc.downsamplingFactor;
		if ((currNegoStat.in.bSize * L) != (currNegoStat.out.bSize * M))
		{
			*suc = false;			
		}
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNIIRResample, set_mode)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genIIRResample_node::config.fixed_rate_location_mode))
	{
		currNegoStat.fixedLocationMode = genIIRResample_node::translate__config__fixed_rate_location_mode_from(0);
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNIIRResample, set_config)
{
	activate_preset(jvxIIRResamplerFilterPresets::JVX_IIR_RESAMPLER_PRESET_LIMIT, true);

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNIIRResample, set_preset)
{
	activate_preset(genIIRResample_node::translate__config__filter_presets_from(), true);
	return JVX_NO_ERROR;
}

void
CjvxSpNIIRResample::activate_preset(jvxIIRResamplerFilterPresets presetSel, jvxBool reportProps)
{
	if (presetSel != jvxIIRResamplerFilterPresets::JVX_IIR_RESAMPLER_PRESET_LIMIT)
	{
		oneDesign theDesign = designUseCase[(int)presetSel];

		genIIRResample_node::config.filter_cutoff_scale.value = theDesign.freqFac;
		genIIRResample_node::config.filter_freqwidth.value = theDesign.freqWidth;

		jvxSize cnt = JVX_SIZE_UNSELECTED;
		for (cnt = 0; cnt < (int)jvxIIRResamplerFilterTechnology::JVX_IIR_RESAMPLER_FILTER_LIMIT; cnt++)
		{
			if (theDesign.design == designFilters[cnt])
			{
				break;
			}
		}
		assert(cnt < (int)jvxIIRResamplerFilterTechnology::JVX_IIR_RESAMPLER_FILTER_LIMIT);
		genIIRResample_node::config.filter_order.value = theDesign.order;
		genIIRResample_node::config.filter_ripple_db.value = theDesign.rip;
		genIIRResample_node::config.filter_stopdb_roll.value = theDesign.stopDbRoll;		
		jvxIIRResamplerFilterTechnology filterTech = (jvxIIRResamplerFilterTechnology)cnt;
		genIIRResample_node::translate__config__filter_technology_to(filterTech);
	}

	CjvxDigitalFilter filterObj;

	setup_filter(filterObj, false);
	auto sosCoeffs = filterObj.coefficients_sos();
	auto sosGain = filterObj.gain();

	genIIRResample_node::design.gain.value = "[" + jvx_data2String(sosGain, JVX_DATA_2STRING_CONST_FORMAT_G) +"]";
	genIIRResample_node::design.sos.value = jvx_data2NumericExpressionString(sosCoeffs, JVX_DATA_2STRING_CONST_FORMAT_G);
	genIIRResample_node::design.fs.value = "[" + jvx_data2String(runtime.srateFilter, JVX_DATA_2STRING_CONST_FORMAT_G) + "]";

	destroy_filter(filterObj, false);

	if (reportProps)
	{
		add_property_report_collect(genIIRResample_node::config.filter_order.descriptor.std_str());
		add_property_report_collect(genIIRResample_node::config.filter_ripple_db.descriptor.std_str());
		add_property_report_collect(genIIRResample_node::config.filter_stopdb_roll.descriptor.std_str());
		add_property_report_collect(genIIRResample_node::config.filter_cutoff_scale.descriptor.std_str());
		add_property_report_collect(genIIRResample_node::config.filter_freqwidth.descriptor.std_str());
		add_property_report_collect(genIIRResample_node::config.filter_technology.descriptor.std_str());
	}
}