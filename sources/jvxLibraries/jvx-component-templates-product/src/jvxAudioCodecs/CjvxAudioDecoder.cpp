#include "jvx.h"
#include "jvxAudioCodecs/CjvxAudioDecoder.h"
#include "jvx-helpers-cpp.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxAudioDecoder::CjvxAudioDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_DECODER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxAudioDecoder*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);
	myParent = NULL;
	neg_input.coupleChannelsDimY = false;
}

CjvxAudioDecoder::~CjvxAudioDecoder()
{
}

// ==============================================================================

jvxErrorType
CjvxAudioDecoder::activate()
{
	jvxErrorType res = CjvxObject::_activate();
	if (res == JVX_NO_ERROR)
	{
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(
			NULL, static_cast<IjvxObject*>(this), "default", NULL,
			_common_set.theModuleName);

		CjvxAudioCodec_genpcg::init_all();
		CjvxAudioCodec_genpcg::allocate_all();
		CjvxAudioCodec_genpcg::register_all(static_cast<CjvxProperties*>(this));
		//CjvxAudioCodec_genpcg::general_init_all();

		// Decoder is in slave mode for encoder!!
		_update_property_access_type_all(JVX_PROPERTY_ACCESS_READ_ONLY);
		_undo_update_property_access_type(
			CjvxAudioCodec_genpcg::general.audio_format.category,
			CjvxAudioCodec_genpcg::general.audio_format.globalIdx);		
		//CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, )

		// Specific function
		this->activate_decoder();

		// Set the codec parameters based on the properties
		neg_input._set_parameters_fixed(1,
			CjvxAudioCodec_genpcg::general.max_number_bytes.value,
			CjvxAudioCodec_genpcg::general.samplerate.value,
			(jvxDataFormat)JVX_DATAFORMAT_BYTE,
			JVX_DATAFORMAT_GROUP_AUDIO_CODED_GENERIC,
			JVX_SIZE_UNSELECTED, 1); // <- number_channels and segment are different arguments here

		neg_output._set_parameters_fixed(
			CjvxAudioCodec_genpcg::general.num_audio_channels.value,
			CjvxAudioCodec_genpcg::general.buffersize.value,
			CjvxAudioCodec_genpcg::general.samplerate.value,
			(jvxDataFormat)CjvxAudioCodec_genpcg::general.audio_format.value,
			JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);
	}
	return(res);
}

jvxErrorType
CjvxAudioDecoder::activate_decoder()
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioDecoder::set_configure_token(const char* tokenArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	jvx_wav_configtoken_2_values(tokenArg, &params);

	CjvxAudioCodec_genpcg::general.max_number_bytes.value = params.fsizemax;
	CjvxAudioCodec_genpcg::general.samplerate.value = params.srate;

	CjvxAudioCodec_genpcg::general.num_audio_channels.value = params.nchans;
	CjvxAudioCodec_genpcg::general.buffersize.value = params.bsize;
	CjvxAudioCodec_genpcg::general.audio_format.value = JVX_DATAFORMAT_DATA;

	neg_input._set_parameters_fixed(
		CjvxAudioCodec_genpcg::general.num_audio_channels.value,
		CjvxAudioCodec_genpcg::general.max_number_bytes.value,
		CjvxAudioCodec_genpcg::general.samplerate.value,
		(jvxDataFormat)JVX_DATAFORMAT_BYTE,
		JVX_DATAFORMAT_GROUP_AUDIO_CODED_GENERIC,		
		JVX_SIZE_UNSELECTED, 1); // <- number_channels and segment are different arguments here

	neg_output._set_parameters_fixed(
		CjvxAudioCodec_genpcg::general.num_audio_channels.value,
		CjvxAudioCodec_genpcg::general.buffersize.value,
		CjvxAudioCodec_genpcg::general.samplerate.value,
		(jvxDataFormat)CjvxAudioCodec_genpcg::general.audio_format.value,
		JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);

	return JVX_NO_ERROR;
}
// ============================================================================================

jvxErrorType
CjvxAudioDecoder::deactivate()
{
	jvxErrorType res = CjvxObject::_deactivate();
	if (res == JVX_NO_ERROR)
	{
		_undo_update_property_access_type_all();
		CjvxAudioCodec_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxAudioCodec_genpcg::deallocate_all();

		this->deactivate_decoder();

		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
}

jvxErrorType
CjvxAudioDecoder::deactivate_decoder()
{
	return JVX_NO_ERROR;
}

// ============================================================================================

jvxErrorType
CjvxAudioDecoder::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	jvxErrorType res = CjvxObject::_is_ready(suc, reasonIfNot);
	if (res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
CjvxAudioDecoder::prepare()
{
	jvxErrorType res = CjvxObject::_prepare();
	if (res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
CjvxAudioDecoder::start()
{
	jvxErrorType res = CjvxObject::_start();
	if (res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
CjvxAudioDecoder::stop()
{
	jvxErrorType res = CjvxObject::_stop();
	if (res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
CjvxAudioDecoder::postprocess()
{
	jvxErrorType res = CjvxObject::_postprocess();
	if (res == JVX_NO_ERROR)
	{
	}
	return(res);
}

// ===========================================================================
// ===================================================================

jvxErrorType
CjvxAudioDecoder::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = neg_input._negotiate_connect_icon(_common_set_ldslave.theData_in,
		static_cast<IjvxObject*>(this),
		_common_set_ldslave.descriptor.c_str()
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Here would be the place to make sure that the config token is consistent to setting
		res = _test_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

// ===================================================================

void 
CjvxAudioDecoder::test_set_output_parameters()
{
	/*
	 * This is old code which is too complex. Use the config_decoder string mechanism!!!
	 * 
	jvxDataProcessorHintDescriptorSearch search_me;
	search_me.assocHint.tp = JVX_COMPONENT_AUDIO_ENCODER;
	search_me.assocHint.slotid = 2;// JVX_SIZE_DONTCARE;
	search_me.assocHint.slotsubid = JVX_SIZE_DONTCARE;
	search_me.hintDescriptorId = JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_CONFIGURATION_TEXT_DESCRIPTION;

	jvxErrorType res = this->transfer_backward_icon(JVX_LINKDATA_TRANSFER_REQUEST_CHAIN_SPECIFIC_DATA_HINT,
		&search_me JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
	if (res == JVX_NO_ERROR)
	{
		if (search_me.reference)
		{
			encode_hint_transferred = (const char*)search_me.reference;
		}
	}
	*/
	_common_set_ldslave.theData_out.con_params.rate = CjvxAudioCodec_genpcg::general.samplerate.value;
	_common_set_ldslave.theData_out.con_params.number_channels = CjvxAudioCodec_genpcg::general.num_audio_channels.value;
	_common_set_ldslave.theData_out.con_params.format = (jvxDataFormat)CjvxAudioCodec_genpcg::general.audio_format.value;
	_common_set_ldslave.theData_out.con_params.buffersize = CjvxAudioCodec_genpcg::general.buffersize.value;
	_common_set_ldslave.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
	_common_set_ldslave.theData_out.con_params.segmentation_x = _common_set_ldslave.theData_out.con_params.buffersize;
	_common_set_ldslave.theData_out.con_params.segmentation_y = 1;
}

/*
jvxErrorType
CjvxAudioDecoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState >= JVX_STATE_PREPARED)
	{
		res = jvx_allocate_pipeline_and_buffers_prepare_to(_common_set_ldslave.theData_in);
	}

	if (res == JVX_NO_ERROR)
	{
		_common_set_ldslave.theData_out.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
		_common_set_ldslave.theData_out.con_pipeline.num_additional_pipleline_stages = 0;
		_common_set_ldslave.theData_out.con_sync.type_timestamp = _common_set_ldslave.theData_in->con_sync.type_timestamp;
		_common_set_ldslave.theData_out.con_data.user_hints = _common_set_ldslave.theData_in->con_data.user_hints;

		res = _prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{
			goto exit_error;
		}
	}

	return res;
exit_error:
	jvx_deallocate_pipeline_and_buffers_postprocess_to(_common_set_ldslave.theData_in);
	return res;
}

jvxErrorType
CjvxAudioDecoder::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState >= JVX_STATE_PREPARED)
	{
		res = jvx_deallocate_pipeline_and_buffers_postprocess_to(_common_set_ldslave.theData_in);
	}

	if (res == JVX_NO_ERROR)
	{
		res = _postprocess_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}

	return res;
}
*/

jvxErrorType
CjvxAudioDecoder::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxByte** bufsIn = jvx_process_icon_extract_input_buffers<jvxByte>(_common_set_ldslave.theData_in, idx_stage);
	jvxData** bufsOutData = nullptr;

	switch (_common_set_ldslave.theData_out.con_params.format)
	{
	case JVX_DATAFORMAT_DATA:
		bufsOutData = jvx_process_icon_extract_output_buffers<jvxData>(&_common_set_ldslave.theData_out);
		for (i = 0; i < params.nchans; i++)
		{
			if (params.sample_type == audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FIXED_POINT)
			{
				switch (params.resolution)
				{
				case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT:
					jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, jvxData>(
						(jvxInt16*)bufsIn[0],
						(jvxData*)bufsOutData[i],
						_common_set_ldslave.theData_out.con_params.buffersize,
						JVX_MAX_INT_16_DIV,
						i, params.nchans,
						0, 1);
					break;
				case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_24BIT:
					jvx_convertSamples_from_bytes_shift_norm_to_flp<jvxInt32, jvxData>(
						(jvxByte*)bufsIn[0],
						(jvxData*)bufsOutData[i],
						_common_set_ldslave.theData_out.con_params.buffersize, 8,
						JVX_MAX_INT_32_DIV, 3,
						i, params.nchans,
						0, 1);
					break;
				case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT:
					jvx_convertSamples_from_fxp_norm_to_flp<jvxInt32, jvxData>(
						(jvxInt32*)bufsIn[0],
						(jvxData*)bufsOutData[i],
						_common_set_ldslave.theData_out.con_params.buffersize,
						JVX_MAX_INT_32_DIV,
						i, params.nchans,
						0, 1);
					break;
				default:
					assert(0);
				}
			} // if (params.fixedpt)
			else
			{
				switch (params.resolution)
				{
				case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT:
					jvx_convertSamples_from_float_to_data<float>(
						(float*)bufsIn[0],
						(jvxData*)bufsOutData[i],
						_common_set_ldslave.theData_out.con_params.buffersize,
						i, params.nchans,
						0, 1);
					break;
				case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_64BIT:
					jvx_convertSamples_from_to<jvxData>(
						(jvxData*)bufsIn[0],
						(jvxData*)bufsOutData[i],
						_common_set_ldslave.theData_out.con_params.buffersize,
						i, params.nchans,
						0, 1);
					break;
				default:
					assert(0);
				}
			}
		} // for (i = 0; i < params.nchans; i++)
		break;
	case JVX_DATAFORMAT_8BIT:
		assert(0);
		/*
		switch (params_file.fformat)
		{
		case JVX_FILEFORMAT_PCM_FIXED:
			switch(params_file.dformat)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt16, jvxInt8, jvxInt16>(
					(jvxInt16*)runtime.oneChunk,
					(jvxInt8*)fld_out[i],
					read, 8,
					i, params_file.channels,
					inOutCnt, 1);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt8, jvxInt32>(
					(jvxInt32*)runtime.oneChunk,
					(jvxInt8*)fld_out[i],
					read, 24,
					i, params_file.channels,
					inOutCnt, 1);
				break;
			default:
				assert(0);
			}
			break;
		case JVX_FILEFORMAT_IEEE_FLOAT:
			switch(params_file.dformat)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				assert(0);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				jvx_convertSamples_from_flp_norm_to_fxp<float, jvxInt8>(
					(float*)runtime.oneChunk,
					(jvxInt8*)fld_out[i],
					read, JVX_MAX_INT_8_DATA,
					i, params_file.channels,
					inOutCnt, 1);
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}*/
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		assert(0);
		/*
		switch (params_file.fformat)
			{
			case JVX_FILEFORMAT_PCM_FIXED:
				switch (params_file.bits_per_sample)
				{
				case 16:
					jvx_convertSamples_from_fxp_noshift_to_fxp<jvxInt16, jvxInt16, jvxInt16>(
						(jvxInt16*)runtime.oneChunk,
						(jvxInt16*)fld_out[i],
						read,
						i, params_file.channels,
						inOutCnt, 1);
					break;
				case 24:
					jvx_convertSamples_from_bytes_shiftright_to_fxp<jvxInt16, jvxInt32>(
						(jvxByte*)runtime.oneChunk,
						(jvxInt16*)fld_out[i],
						read, 3, 8,
						i, params_file.channels,
						inOutCnt, 1);
					break;
				case 32:
					jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt16, jvxInt32>(
						(jvxInt32*)runtime.oneChunk,
						(jvxInt16*)fld_out[i],
						read, 16,
						i, params_file.channels,
						inOutCnt, 1);
					break;
				default:
					assert(0);
				}
				break;
			case JVX_FILEFORMAT_IEEE_FLOAT:
				switch (params_file.bits_per_sample)
				{
				case 32:
					jvx_convertSamples_from_flp_norm_to_fxp<float, jvxInt16>(
						(float*)runtime.oneChunk,
						(jvxInt16*)fld_out[i],
						read, JVX_MAX_INT_16_DATA,
						i, params_file.channels,
						inOutCnt, 1);
					break;
				default:
					assert(0);
				}
				break;
			default:
				assert(0);
			}*/
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		assert(0);
		/*
		switch (params_file.fformat)
		{
		case JVX_FILEFORMAT_PCM_FIXED:
			switch (params_file.bits_per_sample)
			{
			case 16:
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt32, jvxInt32>(
					(jvxInt16*)runtime.oneChunk,
					(jvxInt32*)fld_out[i],
					read, 16,
					i, params_file.channels,
					inOutCnt, 1);
				break;
			case 24:
				jvx_convertSamples_from_bytes_shiftleft_to_fxp<jvxInt32, jvxInt32>(
					(jvxByte*)runtime.oneChunk,
					(jvxInt32*)fld_out[i],
					read, 3, 8,
					i, params_file.channels,
					inOutCnt, 1);
				break;
			case 32:
				jvx_convertSamples_from_fxp_noshift_to_fxp<jvxInt32, jvxInt32, jvxInt32>(
					(jvxInt32*)runtime.oneChunk,
					(jvxInt32*)fld_out[i],
					read,
					i, params_file.channels,
					inOutCnt, 1);
				break;
			default:
				assert(0);
			}
			break;
		case JVX_FILEFORMAT_IEEE_FLOAT:
			switch (params_file.bits_per_sample)
			{
			case 32:
				jvx_convertSamples_from_flp_norm_to_fxp<float, jvxInt32>(
					(float*)runtime.oneChunk,
					(jvxInt32*)fld_out[i],
					read, (float)JVX_MAX_INT_32_DATA,
					i, params_file.channels,
					inOutCnt, 1);
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}
		*/
		break;

	case JVX_DATAFORMAT_64BIT_LE:
		assert(0);
		/*
		switch(params_file.fformat)
		{
		case JVX_FILEFORMAT_PCM_FIXED:
			switch(params_file.dformat)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt64, jvxInt64>(
					(jvxInt16*)runtime.oneChunk,
					(jvxInt64*)fld_out[i],
					read, 48,
					i, params_file.channels,
					inOutCnt, 1);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt32, jvxInt64, jvxInt64>(
					(jvxInt32*)runtime.oneChunk,
					(jvxInt64*)fld_out[i],
					read, 32,
					i, params_file.channels,
					inOutCnt, 1);
				break;
			default:
				assert(0);
			}
			break;
		case JVX_FILEFORMAT_IEEE_FLOAT:
			switch(params_file.dformat)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				assert(0);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				jvx_convertSamples_from_flp_norm_to_fxp<float, jvxInt64>(
					(float*)runtime.oneChunk,
					(jvxInt64*)fld_out[i],
					read, JVX_MAX_INT_64_DATA,
					i, params_file.channels,
					inOutCnt, 1);
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}
		*/
		break;
	default:
		assert(0);
	} // switch (_common_set_ldslave.theData_out.con_params.format)

	return _process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType
CjvxAudioDecoder::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	jvxLinkDataDescriptor* ld_cp = nullptr;
	jvxLinkDataDescriptor forward;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		ld_cp = (jvxLinkDataDescriptor*)data;
		if (ld_cp)
		{
			if (
				(ld_cp->con_params.rate == _common_set_ldslave.theData_out.con_params.rate) &&
				(ld_cp->con_params.number_channels == _common_set_ldslave.theData_out.con_params.number_channels) &&
				(ld_cp->con_params.format == _common_set_ldslave.theData_out.con_params.format) &&
				(ld_cp->con_params.format_group == _common_set_ldslave.theData_out.con_params.format_group))
			{
				if (
					(ld_cp->con_params.buffersize != _common_set_ldslave.theData_out.con_params.buffersize))
				{
					forward = *_common_set_ldslave.theData_in;
					forward.con_params.buffersize = jvx_wav_compute_bsize_bytes_pcm(& params, ld_cp->con_params.buffersize);
					res = _transfer_backward_ocon(true, tp, &forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res == JVX_NO_ERROR)
					{
						// 
						_common_set_ldslave.theData_out.con_params.buffersize = params.bsize;
						_common_set_ldslave.theData_out.con_params.segmentation_x = params.bsize;
						CjvxAudioCodec_genpcg::general.buffersize.value = params.bsize;
					}
				}
				else
				{
					res = JVX_NO_ERROR;
				}
			}
			else
			{
				res = JVX_ERROR_UNSUPPORTED;
			}
		}
		break;
	default:
		res = _transfer_backward_ocon(true, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
	
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
