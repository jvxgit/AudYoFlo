#include "CjvxAuNConvert.h"
#include "jvx-helpers-cpp.h"
#include "jvx_math/jvx_math_extensions.h"

static jvxSize resamplerQualities[] =
{
	32, 64, 128
};

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
		genConvert_node::associate__resampling(this, &resampling.cc.oversamplingFactor, 1, &resampling.cc.downsamplingFactor, 1);
		genConvert_node::register_callbacks(this, set_config, this);

		// Initial read of functionality
		fixedLocationMode = genConvert_node::translate__config__fixed_rate_location_mode_from(0);

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
	
	whatChanged = CjvxNodeBase1io::requires_reconfig(_common_set_icon.theData_in, checkRequestUpdate);
	res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		reChannel.numIn = node_inout._common_set_node_params_a_1io.number_channels;
		reType.formIn = (jvxDataFormat)node_inout._common_set_node_params_a_1io.format;

		reChannel.numOut = node_output._common_set_node_params_a_1io.number_channels;
		reType.formOut = (jvxDataFormat)node_output._common_set_node_params_a_1io.format;

		// auto mode = genConvert_node::translate__config__fixed_rate_location_mode_from();
		switch (fixedLocationMode)
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

void
CjvxAuNConvert::from_input_to_output()
{
	// If the input arguments have changed we need to reconfigure
	if (whatChanged)
	{
		CjvxBareNode1ioRearrange::from_input_to_output();

		// Check the relation input/output
		adapt_output_parameters_forward();
	}
}

void
CjvxAuNConvert::compute_buffer_relations(jvxBool fromInput, jvxSize* bsizeArgOpt)
{
	jvxSize bSize = 0;
	if (fromInput)
	{
		bSize = node_inout._common_set_node_params_a_1io.buffersize;
		if (bsizeArgOpt)
		{
			bSize = *bsizeArgOpt;
		}
		resampling.bSizeInMin = bSize;
		resampling.bSizeInMax = bSize;
		if (bSize % resampling.cc.downsamplingFactor)
		{
			resampling.bSizeInMin = bSize / resampling.cc.downsamplingFactor;
			resampling.bSizeInMin *= resampling.cc.downsamplingFactor;
			resampling.bSizeInMax = resampling.bSizeInMin + resampling.cc.downsamplingFactor;
		}
		resampling.bSizeOutMin = resampling.bSizeInMin * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
		resampling.bSizeOutMax = resampling.bSizeInMax * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
	}
	else
	{
		bSize = node_output._common_set_node_params_a_1io.buffersize;
		if (bsizeArgOpt)
		{
			bSize = *bsizeArgOpt;
		}
		resampling.bSizeOutMin = bSize;
		resampling.bSizeOutMax = bSize;
		if (bSize % resampling.cc.oversamplingFactor)
		{
			resampling.bSizeOutMin = bSize / resampling.cc.oversamplingFactor;
			resampling.bSizeOutMin *= resampling.cc.oversamplingFactor;
			resampling.bSizeOutMax = resampling.bSizeOutMin + resampling.cc.oversamplingFactor;
		}
		resampling.bSizeInMin = resampling.bSizeOutMin * resampling.cc.downsamplingFactor / resampling.cc.oversamplingFactor;
		resampling.bSizeInMax = resampling.bSizeOutMax * resampling.cc.downsamplingFactor / resampling.cc.oversamplingFactor;
	}
}

void
CjvxAuNConvert::adapt_output_parameters_forward()
{
	compute_buffer_relations(true);

	// Check if there is a new value in rate or buffersize - if so, reset num/den
	// node_output._common_set_node_params_a_1io.buffersize = ceil((jvxData)node_inout._common_set_node_params_a_1io.buffersize * (jvxData) resampling.cc.oversamplingFactor / (jvxData)resampling.cc.downsamplingFactor);
	switch (fixedLocationMode)
	{
	case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:

		// If we forward output data with a variable framesize, we need to allocate the maximum buffersize towards the output
		node_output._common_set_node_params_a_1io.buffersize = resampling.bSizeOutMax;
		break;
	}

	node_output._common_set_node_params_a_1io.samplerate = node_inout._common_set_node_params_a_1io.samplerate * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
	
	neg_output._update_parameters_fixed(
		JVX_SIZE_UNSELECTED,
		node_output._common_set_node_params_a_1io.buffersize,
		node_output._common_set_node_params_a_1io.samplerate,
		(jvxDataFormat)node_output._common_set_node_params_a_1io.format, 
		(jvxDataFormatGroup)node_output._common_set_node_params_a_1io.subformat,
		(jvxDataflow)node_output._common_set_node_params_a_1io.data_flow);
}

void
CjvxAuNConvert::adapt_output_parameters_backward(jvxSize numChannelsOutDesired, jvxDataFormat formOutDesired)
{
	// Recompute the input parameters
	update_simple_params_from_ldesc();

	// Only the output samplerate should have changed!
	jvxSize desiredSamplerate = node_inout._common_set_node_params_a_1io.samplerate * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;

	neg_output._update_parameters_fixed(
		numChannelsOutDesired,
		resampling.bSizeOutMax,
		desiredSamplerate,
		formOutDesired);
}

jvxErrorType 
CjvxAuNConvert::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	// This function involves the default implementation but should end up in <accept_negotiate_output>
	// by inheritance
	jvxErrorType res = CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res != JVX_NO_ERROR)
	{
		
	}
	return res;
}

jvxErrorType
CjvxAuNConvert::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// First, try to activate the setting as is:
	jvxLinkDataDescriptor tryThis = *_common_set_icon.theData_in;

	JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
	log << "Entering function " << __FUNCTION__  << " to negotiate processing parameters." << std::endl;
	JVX_STOP_LOCK_LOG;

	// First, try to just activate the output desire - which typically does not really work
	JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
	log << "Trying proposed settings being applied directly to successor." << std::endl;
	JVX_STOP_LOCK_LOG;

	tryThis = *preferredByOutput;
	res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryThis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	switch(res)
	{
		case JVX_NO_ERROR:

			JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Successor accepted proposed configuration." << std::endl;
			JVX_STOP_LOCK_LOG;

			// Reset the resampler to NOT resample
			resampling.cc.downsamplingFactor = 1;
			resampling.cc.oversamplingFactor = 1;
			resampling.cc.gcd = 1;

			// We need to run from the beginning..
			whatChanged = 0;
			jvx_bitInvert(whatChanged);

			update_simple_params_from_ldesc();
			update_output_params_from_input_params();
			adapt_output_parameters_forward();
			test_set_output_parameters();
			
			break;
		case JVX_ERROR_COMPROMISE:

			JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Successor returned compromise - returned JVX_ERROR_POSTPONE to test the connection again." << std::endl;
			JVX_STOP_LOCK_LOG;

			// Too much complexity, we better run the test again!
			res = JVX_ERROR_POSTPONE;
			break;
		default:

			JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Successor returned error. We will adapt input/output settings involving resampler." << std::endl;
			JVX_STOP_LOCK_LOG;

			// Simple forwarding did not work out, hence, let us find out step by step
			whatChanged = CjvxNodeBase1io::requires_reconfig(preferredByOutput, checkRequestUpdate);
			res = JVX_NO_ERROR;

			switch (fixedLocationMode)
			{
			case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
			
				// ===========================================================================================
				// Try to align number channels
				// ===========================================================================================
				if (jvx_bitTest(whatChanged, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_NUM_CHANNELS_SHIFT))
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
					log << "Asking successor to adapt number of channels." << std::endl;
					JVX_STOP_LOCK_LOG;

					tryThis.con_params = _common_set_icon.theData_in->con_params;
					tryThis.con_params.number_channels = preferredByOutput->con_params.number_channels;
					switch (res)
					{
					case JVX_NO_ERROR:
						node_inout._common_set_node_params_a_1io.number_channels = _common_set_icon.theData_in->con_params.number_channels;
						break;
					case JVX_ERROR_COMPROMISE:
						res = JVX_ERROR_UNSUPPORTED;
						break;
					default:

						JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
						log << "Successor could not fulfill requirements." << std::endl;
						JVX_STOP_LOCK_LOG;

						// Override error message - can deal with it
						res = JVX_NO_ERROR;
					}

					// Modifify the number of output channels as requested
					node_output._common_set_node_params_a_1io.number_channels = preferredByOutput->con_params.number_channels;
					if (res != JVX_NO_ERROR)
					{
						return res;
					}
				}

				// ===========================================================================================
				// Try to align processing
				// ===========================================================================================
				if (jvx_bitTest(whatChanged, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMAT_SHIFT))
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
					log << "Asking successor to adapt processing format." << std::endl;
					JVX_STOP_LOCK_LOG;

					tryThis.con_params = _common_set_icon.theData_in->con_params;
					tryThis.con_params.format = preferredByOutput->con_params.format;
					res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryThis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

					switch (res)
					{
					case JVX_NO_ERROR:
						node_inout._common_set_node_params_a_1io.format = _common_set_icon.theData_in->con_params.format;
						break;
					case JVX_ERROR_COMPROMISE:
						res = JVX_ERROR_UNSUPPORTED;
						break;
					default:

						JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
						log << "Successor could not fulfill requirements." << std::endl;
						JVX_STOP_LOCK_LOG;

						// Override error message - can deal with it
						res = JVX_NO_ERROR;
					}
					// Modifify the number of output format as requested
					node_output._common_set_node_params_a_1io.format = preferredByOutput->con_params.format;
					if (res != JVX_NO_ERROR)
					{
						return res;
					}
				}
				
				// ===========================================================================================
				// Next we deal with buffersize/samperates
				// ===========================================================================================
				if (
					(jvx_bitTest(whatChanged, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SAMPLERATE_SHIFT)) ||
					(jvx_bitTest(whatChanged, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_BUFFERSIZE_SHIFT)))
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
					log << "Adapting conversion parameters to convert samplerate/buffersize." << std::endl;
					JVX_STOP_LOCK_LOG;


					// Final attempt without any other components involved: convert resampling options
					jvx_fixed_resampler_init_conversion(
						&resampling.cc,
						node_inout._common_set_node_params_a_1io.samplerate,
						preferredByOutput->con_params.rate);

					// We need to deviate from the proposed procedure since this segment returns a "COMPROMISE"
					// The test_set_output
					adapt_output_parameters_forward();
					// test_set_output_parameters(); <- will be called in calling function

					res = JVX_ERROR_COMPROMISE;
					break;
				}
				break;
			case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
				
				// Final attempt without any other components involved: convert resampling options
				tryThis = *_common_set_icon.theData_in;
				jvx_fixed_resampler_init_conversion(&resampling.cc, node_inout._common_set_node_params_a_1io.samplerate,
					preferredByOutput->con_params.rate);

				// Adapt output buffersize as requested since compute_buffer_relations requires 				
				compute_buffer_relations(false, &preferredByOutput->con_params.buffersize);
				tryThis.con_params.buffersize = resampling.bSizeInMax;
				tryThis.con_params.segmentation.x = resampling.bSizeInMax;

				// Here, we must rely on a buffersize update in the previous component - this is the degree of freedom that we have
				res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryThis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res == JVX_NO_ERROR)
				{
					adapt_output_parameters_backward(preferredByOutput->con_params.number_channels, preferredByOutput->con_params.format);
				}
				break;			
			}		
	}
	return res;
		//CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, preferredByOutput JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
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

	if ((resampling.cc.oversamplingFactor != 1) || (resampling.cc.downsamplingFactor != 1))
	{
		runtime.active_resampling = true;
	}
	if (reChannel.numIn != reChannel.numOut)
	{
		runtime.active_rechannel = true;
	}
	if (node_inout._common_set_node_params_a_1io.format != node_output._common_set_node_params_a_1io.format)
	{
		runtime.active_retype = true;
	}

	if(runtime.active_resampling || runtime.active_rechannel || runtime.active_retype)
	{
		_common_set_ldslave.zeroCopyBuffering_cfg = false;

		switch (fixedLocationMode)
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
			if (node_output._common_set_node_params_a_1io.buffersize % resampling.cc.oversamplingFactor)
			{
				runtime.requiresRebufferHeadroom= true;
				runtime.requiresRebuffering = true;
			}
			if (node_inout._common_set_node_params_a_1io.buffersize % resampling.cc.downsamplingFactor)
			{
				runtime.requiresRebufferHeadroom= true;
				runtime.requiresRebuffering = true;
			}
		}
		if (runtime.active_rechannel || runtime.active_retype)
		{
			runtime.requiresRebuffering = true;
		}

		switch (fixedLocationMode)
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
			jvxDataFormat formOut = (jvxDataFormat)node_inout._common_set_node_params_a_1io.format;

			switch (fixedLocationMode)
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
				runtime.fldResampler[i].prmInit.buffersizeIn = resampling.bSizeInMax;
				runtime.fldResampler[i].prmInit.buffersizeOut = resampling.bSizeOutMax;
				runtime.fldResampler[i].prmInit.format = formOut;
				runtime.fldResampler[i].prmInit.lFilter = resamplerQualities[(jvxSize)resamplerQuality];
				runtime.fldResampler[i].prmInit.optPtrConversion = &resampling.cc; // Provide the resampling exact config
				jvx_fixed_resampler_prepare(&runtime.fldResampler[i]);
			}
		}

		if (runtime.requiresRebuffering)
		{
			if (runtime.requiresRebufferHeadroom)
			{
				switch (fixedLocationMode)
				{
				case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
					runtime.lFieldRebuffer = node_inout._common_set_node_params_a_1io.buffersize + resampling.cc.downsamplingFactor;
					break;
				case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
					runtime.lFieldRebuffer = node_output._common_set_node_params_a_1io.buffersize + resampling.cc.oversamplingFactor;
					break;
				}
			}
			else
			{
				switch (fixedLocationMode)
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
			runtime.ptrFieldBuffer = nullptr;
			runtime.lFieldRebufferChannel = runtime.lFieldRebuffer * jvxDataFormat_getsize(node_output._common_set_node_params_a_1io.format);
			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.ptrFieldBuffer, jvxHandle*, runtime.nCFieldRebuffer);
			for (i = 0; i < runtime.nCFieldRebuffer; i++)
			{
				JVX_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.ptrFieldBuffer[i], jvxByte, runtime.lFieldRebufferChannel);
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
		for (i = 0; i < runtime.nCFieldRebuffer; i++)
		{
			JVX_SAFE_DELETE_FIELD_TYPE(runtime.ptrFieldBuffer[i], jvxByte);
		}
		JVX_SAFE_DELETE_FIELD(runtime.ptrFieldBuffer);

		runtime.fFieldRebuffer = 0;
		runtime.nCFieldRebuffer = 0;
		runtime.ptrFieldBuffer = nullptr;
		runtime.lFieldRebufferChannel = 0;
		runtime.lFieldRebuffer = 0;

	}

	runtime.requiresRebuffering = false;
	runtime.requiresRebufferHeadroom = false;
	
	runtime.active_resampling = false;
	runtime.active_retype = false;
	runtime.active_rechannel = false;

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
			switch (fixedLocationMode)
			{
			case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:

				// Compute the number of samples we would need to fill the internal buffer
				idx_stage_local = jvx_process_icon_extract_stage(_common_set_icon.theData_in, JVX_SIZE_UNSELECTED);
				
				// If the successor does not support variable frame size this algorithm can not really work!!
				assert(_common_set_icon.theData_in->con_data.fHeights);

				// Compute the min or the max number of samples and provide for successor				
				space = runtime.lFieldRebuffer - runtime.fFieldRebuffer;

				// Depending on the space, request either the minimum or the maximum number of samples
				if (space >= resampling.bSizeOutMax)
				{
					_common_set_icon.theData_in->con_data.fHeights[idx_stage_local].x = resampling.bSizeInMax;
				}
				else
				{
					_common_set_icon.theData_in->con_data.fHeights[idx_stage_local].x = resampling.bSizeInMin;
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
		
	// If we do nothing, bypass with zerocopy
	if(zeroCopyBuffering_rt)
	{ 
		return CjvxBareNode1ioRearrange::process_buffers_icon(mt_mask, idx_stage);
	}

	// Extract input and output fields
	jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
	jvxSize idx_stage_local = jvx_process_icon_extract_stage(_common_set_icon.theData_in, idx_stage);


	// The algorithm becomes a little complicated if the output buffersize does not fit into the 
	// provides buffer due to fractional relation
	if (runtime.requiresRebuffering)
	{
		jvxSize numInputSamples = _common_set_icon.theData_in->con_params.buffersize;		
		jvxSize numCopyNextFrame = 0;
		jvxSize numResamplerOut = 0;
		jvxSize numResamplerIn = 0;
		jvxSize numOutSamples = 0;
		jvxSize cnt = 0;
		jvxSize splCnt = 0;
		jvxSize loopNum = 0; 
		jvxSize numOutChannels = _common_set_ocon.theData_out.con_params.number_channels;
		jvxData** bufsOutChannels = bufsOut;
		jvxSize numInChannels = _common_set_icon.theData_in->con_params.number_channels;
		jvxData** bufsInChannels = bufsIn;

		// Obtain the (optional) number of input samples. Buffersize may only be lower!!
		if (JVX_CHECK_SIZE_SELECTED(_common_set_icon.theData_in->con_data.fHeights[idx_stage_local].x))
		{
			numInputSamples = _common_set_icon.theData_in->con_data.fHeights[idx_stage_local].x;
		}
		assert(numInputSamples <= _common_set_icon.theData_in->con_params.buffersize);

		switch (fixedLocationMode)
		{
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:

			// This case: Converter forwards variable buffersizes!!

			// runtime.nCFieldRebuffer is allocated to hold the OUTPUT channels!
			// We will loop over max of input and output buffers but we will only
			// resample over the output channels

			// Channel conversion with different targets for resampling or no resampling			
			if (runtime.active_resampling)
			{
				numOutChannels = runtime.nCFieldRebuffer;
				bufsOutChannels = (jvxData**)runtime.ptrFieldBuffer;
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
				jvxData* sourceIn = (jvxData*)bufsIn[inCnt];
				jvxData* targetIn = (jvxData*)bufsOutChannels[outCnt];
				
				// Target may have an offset due to stored samples
				targetIn += runtime.fFieldRebuffer;

				// Loop over all samples with circular extend or downmix
				for (splCnt = 0; splCnt < numInputSamples; splCnt++)
				{
					jvxData sampleIn = sourceIn[splCnt];
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
				numResamplerIn = runtime.fFieldRebuffer / resampling.cc.downsamplingFactor * resampling.cc.downsamplingFactor; // Find multiples of input granularity

				// Compute the number of output samples
				numResamplerOut = numResamplerIn * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
				assert(numResamplerOut <= _common_set_ocon.theData_out.con_params.buffersize);

				// Run the actual resamplers, produce a variable output size
				for (i = 0; i < runtime.numResampler; i++)
				{
					fixed_resample_variable_out varOut;
					varOut.bsizeIn = numResamplerIn;
					varOut.numOut = numResamplerOut;

					// Important: use the "new" input buffer
					jvx_fixed_resampler_process(&runtime.fldResampler[i], runtime.ptrFieldBuffer[i], bufsOut[i], &varOut); // Full size resampling
				}

				// Now approach the output side and set the variable output buffersize
				idx_stage_local = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;
				_common_set_ocon.theData_out.con_data.fHeights[idx_stage_local].x = numResamplerOut;

				// Housekeeping: update intermediate buffer by sample move. It is typically only very few samples to move.
				numCopyNextFrame = runtime.fFieldRebuffer - numResamplerIn;
				for (cnt = 0; cnt < runtime.nCFieldRebuffer; cnt++)
				{
					jvxData* cpyTo = (jvxData*)runtime.ptrFieldBuffer[cnt];
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
				bufsInChannels = (jvxData**)runtime.ptrFieldBuffer;
			}

			loopNum = JVX_MAX(numInChannels,
				_common_set_ocon.theData_out.con_params.number_channels);
			
			// Compute the number of samples available
			numResamplerOut = numInputSamples * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
			
			// Output buffersize is fixed!
			numOutSamples = node_output._common_set_node_params_a_1io.buffersize;

			// Some checks to make sure the samples fit into our internal buffer
			assert(runtime.fFieldRebuffer + numResamplerOut <= runtime.lFieldRebuffer);

			if (runtime.active_resampling)
			{
				// Resample the input to the internal buffer. The buffer can be variably filled but we must
				// make sure that we will have enough samples to forward with the fixed buffersize!
				for (i = 0; i < runtime.numResampler; i++)
				{
					// Target field is the intermediate buffer
					jvxData* targetOut = (jvxData*)runtime.ptrFieldBuffer[i];

					// But we will fill up from the fHeight offset to hiold old samples
					targetOut += runtime.fFieldRebuffer;

					// Core resampling
					fixed_resample_variable_out varOut;
					varOut.bsizeIn = numInputSamples;
					varOut.numOut = numResamplerOut;

					// Important: use the "new" input buffer with index shift (targetOut)
					jvx_fixed_resampler_process(&runtime.fldResampler[i], bufsIn[i], targetOut, &varOut); // Full size resampling
				}

				// Update fillheight
				runtime.fFieldRebuffer += numResamplerOut;
			}

			// Now, preset the output buffer to 0
			for (cnt = 0; cnt < node_output._common_set_node_params_a_1io.number_channels; cnt++)
			{
				jvxData* targetOut = bufsOut[cnt];
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
				jvxData* sourceOut = (jvxData*)bufsInChannels[inCnt];
				jvxData* targetOut = (jvxData*)bufsOut[outCnt];

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
					jvxData* targetOut = (jvxData*)runtime.ptrFieldBuffer[cnt];
					jvxData* sourceOut = (jvxData*)runtime.ptrFieldBuffer[cnt];
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
		// Simple case: just resample straight away
		for (i = 0; i < runtime.numResampler; i++)
		{
			jvx_fixed_resampler_process(&runtime.fldResampler[i], bufsIn[i], bufsOut[i], nullptr); // Full size resampling
		}
	}
	return fwd_process_buffers_icon(mt_mask, idx_stage);
	
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNConvert, set_config)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genConvert_node::config.fixed_rate_location_mode))
	{
		fixedLocationMode = genConvert_node::translate__config__fixed_rate_location_mode_from(0);
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genConvert_node::config.resampler_quality))
	{
		resamplerQuality = genConvert_node::translate__config__resampler_quality_from(0);
	}

	return JVX_NO_ERROR;
}

