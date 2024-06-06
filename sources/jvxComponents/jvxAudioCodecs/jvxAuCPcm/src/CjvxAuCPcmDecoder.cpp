#include "CjvxAuCPcmDecoder.h"

CjvxAuCPcmDecoder::CjvxAuCPcmDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) : 
	CjvxAudioDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
}

jvxErrorType
CjvxAuCPcmDecoder::activate()
{
	jvxErrorType res = CjvxAudioDecoder::activate();
	if (res == JVX_NO_ERROR)
	{

		CjvxAudioCodec_genpcg::init_all();
		CjvxAudioCodec_genpcg::allocate_all();
		CjvxAudioCodec_genpcg::register_all(static_cast<CjvxProperties*>(this));
		//CjvxAudioCodec_genpcg::general_init_all();

		// Decoder is in slave mode for encoder!!
		_update_property_access_type_all(JVX_PROPERTY_ACCESS_READ_ONLY);
		_undo_update_property_access_type(
			CjvxAudioCodec_genpcg::general.audio_format);
		//CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, )
	}
	return res;
}

jvxErrorType
CjvxAuCPcmDecoder::deactivate()
{
	jvxErrorType res = CjvxAudioDecoder::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		_undo_update_property_access_type_all();
		CjvxAudioCodec_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxAudioCodec_genpcg::deallocate_all();

		res = CjvxAudioDecoder::deactivate();
	}
	return res;
}

jvxErrorType
CjvxAuCPcmDecoder::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxByte** bufsIn = jvx_process_icon_extract_input_buffers<jvxByte>(_common_set_icon.theData_in, idx_stage);
	jvxData** bufsOutData = nullptr;
	jvxSize idxBuf = JVX_CHECK_SIZE_UNSELECTED(idx_stage) ? *_common_set_icon.theData_in->con_pipeline.idx_stage_ptr : idx_stage;

	jvxSize bSizeIn = _common_set_icon.theData_in->con_params.buffersize;
	jvxSize bSizeOut = 0;
	if (_common_set_icon.theData_in->con_data.fHeights)
	{
		bSizeIn = JVX_MIN(bSizeIn, _common_set_icon.theData_in->con_data.fHeights[idxBuf].x);
	}

	switch (_common_set_ocon.theData_out.con_params.format)
	{
	case JVX_DATAFORMAT_DATA:
		bufsOutData = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
		for (i = 0; i < params.nchans; i++)
		{
			if (params.sample_type == audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FIXED_POINT)
			{
				switch (params.resolution)
				{
				case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT:
					bSizeOut = bSizeIn / params.nchans / sizeof(jvxInt16);

					jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, jvxData>(
						(jvxInt16*)bufsIn[0],
						(jvxData*)bufsOutData[i],
						bSizeOut,
						JVX_MAX_INT_16_DIV,
						i, params.nchans,
						0, 1);
					break;
				case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_24BIT:
					bSizeOut = bSizeIn / params.nchans / (sizeof(jvxInt8) * 3);
					jvx_convertSamples_from_bytes_shift_norm_to_flp<jvxInt32, jvxData>(
						(jvxByte*)bufsIn[0],
						(jvxData*)bufsOutData[i],
						bSizeOut, 8,
						JVX_MAX_INT_32_DIV, 3,
						i, params.nchans,
						0, 1);
					break;
				case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT:
					bSizeOut = bSizeIn / params.nchans / sizeof(jvxInt32);
					jvx_convertSamples_from_fxp_norm_to_flp<jvxInt32, jvxData>(
						(jvxInt32*)bufsIn[0],
						(jvxData*)bufsOutData[i],
						bSizeOut,
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
					bSizeOut = bSizeIn / params.nchans / sizeof(float);
					jvx_convertSamples_from_float_to_data<float>(
						(float*)bufsIn[0],
						(jvxData*)bufsOutData[i],
						_common_set_ocon.theData_out.con_params.buffersize,
						i, params.nchans,
						0, 1);
					break;
				case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_64BIT:
					bSizeOut = bSizeIn / params.nchans / sizeof(double);
					jvx_convertSamples_from_to<jvxData>(
						(jvxData*)bufsIn[0],
						(jvxData*)bufsOutData[i],
						_common_set_ocon.theData_out.con_params.buffersize,
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
	} // switch (_common_set_ocon.theData_out.con_params.format)

	// Pass forward variable buffersize - een if it is constant
	if (_common_set_ocon.theData_out.con_data.fHeights)
	{
		_common_set_ocon.theData_out.con_data.fHeights[*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr].x = bSizeOut;
	}

	return _process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType
CjvxAuCPcmDecoder::set_configure_token(const char* tokenArg)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = configure_decoder(tokenArg);
	if (res == JVX_NO_ERROR)
	{
		neg_input._set_parameters_fixed(
			1, CjvxAudioCodec_genpcg::general.max_number_bytes.value,
			JVX_SIZE_DONTCARE,
			JVX_DATAFORMAT_BYTE,
			JVX_DATAFORMAT_GROUP_AUDIO_CODED_GENERIC,
			JVX_DATAFLOW_DONT_CARE,
			JVX_SIZE_UNSELECTED, 1); // <- number_channels and segment are different arguments here

		neg_output._set_parameters_fixed(
			CjvxAudioCodec_genpcg::general.num_audio_channels.value,
			CjvxAudioCodec_genpcg::general.buffersize.value,
			CjvxAudioCodec_genpcg::general.samplerate.value,
			(jvxDataFormat)CjvxAudioCodec_genpcg::general.audio_format.value,
			JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);
	}
	return res;
}

jvxErrorType
CjvxAuCPcmDecoder::configure_decoder(const char* tokenArg)
{
	jvxApiString famToken;
	jvxErrorType res = jvx_wav_configtoken_2_values(tokenArg, &params, &famToken);
	if (res == JVX_NO_ERROR)
	{
		// This should have been clarified before!
		assert(famToken.std_str() == "jvx");

		CjvxAudioCodec_genpcg::general.max_number_bytes.value = JVX_SIZE_INT32(params.fsizemax);
		CjvxAudioCodec_genpcg::general.samplerate.value = JVX_SIZE_INT32(params.srate);
		CjvxAudioCodec_genpcg::general.num_audio_channels.value = params.nchans;
		CjvxAudioCodec_genpcg::general.buffersize.value = params.bsize;
		CjvxAudioCodec_genpcg::general.audio_format.value = JVX_DATAFORMAT_DATA;
	}
	return res;
}

void
CjvxAuCPcmDecoder::accept_input_parameters()
{

}

void
CjvxAuCPcmDecoder::test_set_output_parameters()
{
	// The output side is "audio"
	_common_set_ocon.theData_out.con_params.rate = CjvxAudioCodec_genpcg::general.samplerate.value;
	_common_set_ocon.theData_out.con_params.number_channels = CjvxAudioCodec_genpcg::general.num_audio_channels.value;
	_common_set_ocon.theData_out.con_params.format = (jvxDataFormat)CjvxAudioCodec_genpcg::general.audio_format.value;
	_common_set_ocon.theData_out.con_params.buffersize = CjvxAudioCodec_genpcg::general.buffersize.value;
	_common_set_ocon.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
	_common_set_ocon.theData_out.con_params.data_flow = _common_set_icon.theData_in->con_params.data_flow;
	_common_set_ocon.theData_out.con_params.segmentation.x = _common_set_ocon.theData_out.con_params.buffersize;
	_common_set_ocon.theData_out.con_params.segmentation.y = 1;
}

jvxErrorType
CjvxAuCPcmDecoder::updated_backward_format_spec(jvxLinkDataDescriptor& forward, jvxLinkDataDescriptor* ld_cp)
{
	wav_params nParams = params;
	nParams.bsize = ld_cp->con_params.buffersize;
	forward.con_params.buffersize = jvx_wav_compute_bsize_bytes_pcm(&nParams, nParams.bsize);
	forward.con_params.format_spec = jvx_wav_produce_codec_token(&nParams);
	return JVX_NO_ERROR;
}