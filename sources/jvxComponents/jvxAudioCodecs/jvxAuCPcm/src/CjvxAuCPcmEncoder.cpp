#include "CjvxAuCPcmEncoder.h"
#include "CjvxAuCPcm.h"

CjvxAuCPcmEncoder::CjvxAuCPcmEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) : CjvxAudioEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	// params default is good
	params.bsize = 1024;
	params.nchans = 2;
	params.endian = audio_codec_wav_endian::AUDIO_CODEC_WAV_LITTLE_ENDIAN;
	params.resolution = audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT;
	params.sample_type = audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FIXED_POINT;
	params.sub_type = audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_32;
	params.fsizemax = jvx_wav_compute_bsize_bytes_pcm(&params, params.bsize);
	params.srate = 48000;
}

jvxErrorType
CjvxAuCPcmEncoder::activate()
{
	jvxErrorType res = CjvxAudioEncoder::activate();
	if (res == JVX_NO_ERROR)
	{
		CjvxAudioCodec_genpcg::init_all();
		CjvxAudioCodec_genpcg::allocate_all();
		CjvxAudioCodec_genpcg::register_all(static_cast<CjvxProperties*>(this));
		//CjvxAudioCodec_genpcg::general_init_all();
	}
	return res;
}

jvxErrorType
CjvxAuCPcmEncoder::deactivate()
{
	jvxErrorType res = CjvxAudioEncoder::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxAudioCodec_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxAudioCodec_genpcg::deallocate_all();
	}
	return res;
}

jvxErrorType
CjvxAuCPcmEncoder::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = _set_property(
		callGate,
		rawPtr,
		ident, trans);
	if (res == JVX_NO_ERROR)
	{
		if (myParent)
		{
			myParent->computeRateAndOutBuffersize(static_cast<CjvxAudioCodec_genpcg*>(this));
		}
	}
	return res;
}

jvxErrorType
CjvxAuCPcmEncoder::check_backward_parameters(jvxLinkDataDescriptor* ld_cp, jvxLinkDataDescriptor& forward, jvxBool& forwardRequest)
{
	forwardRequest = false;
	wav_params params_check;
	std::string config_token_complain = ld_cp->con_params.format_spec.std_str();
	jvxErrorType res = jvx_wav_configtoken_2_values(config_token_complain.c_str(), &params_check, nullptr);
	if (res != JVX_NO_ERROR)
	{
		forwardRequest = true;
	}
	else
	{
		if (
			(params.bsize == params_check.bsize) &&
			(params.srate == params_check.srate)
			)
		{
			// Accept the new parameters and report positive to caller
			params = params_check;
		}
		else
		{
			// Derive the parameters from input side for  the next forward step
			forward.con_params = _common_set_icon.theData_in->con_params;

			// This is the next level of modification: requires new buffersize and/or samplerate from previous module
			forward.con_params.buffersize = params_check.bsize;
			forward.con_params.rate = params_check.srate;
			forward.con_params.number_channels = params_check.nchans;

			forward.con_params.segmentation.x = forward.con_params.buffersize;
			forwardRequest = true;
		}
	}
	return res;
}

void
CjvxAuCPcmEncoder::accept_input_parameters()
{
	// Accept the input settings
	params.bsize = neg_input._latest_results.buffersize;
	params.srate = neg_input._latest_results.rate;
	params.nchans = neg_input._latest_results.number_channels;
	params.fsizemax = jvx_wav_compute_bsize_bytes_pcm(&params, params.bsize);
}

void
CjvxAuCPcmEncoder::test_set_output_parameters()
{
	std::string propstring;

	_common_set_ocon.theData_out.con_params.rate = params.srate / params.bsize;
	_common_set_ocon.theData_out.con_params.number_channels = 1;
	_common_set_ocon.theData_out.con_params.format = JVX_DATAFORMAT_BYTE;
	_common_set_ocon.theData_out.con_params.buffersize = params.fsizemax;
	_common_set_ocon.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_CODED_GENERIC;
	_common_set_ocon.theData_out.con_params.segmentation.x = _common_set_ocon.theData_out.con_params.buffersize;
	_common_set_ocon.theData_out.con_params.segmentation.y = 1;

	propstring = jvx_wav_produce_codec_token(&params);
	params.fsizemax = jvx_wav_compute_bsize_bytes_pcm(&params, params.bsize);

	_common_set_ocon.theData_out.con_params.buffersize = params.fsizemax;
	_common_set_ocon.theData_out.con_params.format_spec = propstring;
}

jvxErrorType
CjvxAuCPcmEncoder::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxByte** bufsOut = jvx_process_icon_extract_output_buffers<jvxByte>(_common_set_ocon.theData_out);
	jvxData** bufsInData = nullptr;

	assert(params.nchans == _common_set_icon.theData_in->con_params.number_channels);
	assert(params.bsize == _common_set_icon.theData_in->con_params.buffersize);

	switch (_common_set_icon.theData_in->con_params.format)
	{
	case JVX_DATAFORMAT_DATA:
		bufsInData = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);

		switch (params.sample_type)
		{
		case audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FIXED_POINT:
			switch (params.resolution)
			{
			case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT:
				for (i = 0; i < params.nchans; i++)
				{
					jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt16>(
						(jvxData*)bufsInData[i],
						(jvxInt16*)bufsOut[0],
						_common_set_icon.theData_in->con_params.buffersize,
						JVX_MAX_INT_16_DATA,
						0, 1,
						i, params.nchans);
				}
				break;
			case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_24BIT:
				for (i = 0; i < params.nchans; i++)
				{
					jvx_convertSamples_from_flp_norm_to_bytes<jvxData, jvxInt32>(
						(jvxData*)bufsInData[i],
						(jvxByte*)bufsOut[0],
						_common_set_icon.theData_in->con_params.buffersize,
						JVX_MAX_INT_24_DATA, 3,
						0, 1,
						i, params.nchans);
				}
				break;
			case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT:
				for (i = 0; i < params.nchans; i++)
				{
					jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt32>(
						(jvxData*)bufsInData[i],
						(jvxInt32*)bufsOut[0],
						_common_set_icon.theData_in->con_params.buffersize,
						JVX_MAX_INT_32_DATA,
						0, 1,
						i, params.nchans);
				}
				break;
			default:
				assert(0);
			}
			break;
		case audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FLOATING_POINT:
			switch (params.resolution)
			{
			case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT:
				for (i = 0; i < params.nchans; i++)
				{
					jvx_convertSamples_from_data_to_float<float>(
						(jvxData*)bufsInData[i],
						(float*)bufsOut[0],
						_common_set_icon.theData_in->con_params.buffersize,
						0, 1,
						i, params.nchans);
				}
				break;
			case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_64BIT:
				for (i = 0; i < params.nchans; i++)
				{
					jvx_convertSamples_from_data_to_float<double>(
						(jvxData*)bufsInData[i],
						(double*)bufsOut[0],
						_common_set_icon.theData_in->con_params.buffersize,
						0, 1,
						i, params.nchans);
				}
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		assert(0);
		/*
		switch(params_file.fformat)
		{
		case JVX_FILEFORMAT_PCM_FIXED:
			switch(params_file.dformat)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt8, jvxInt16, jvxInt16>(
					(jvxInt8*)fld_in[i],
					(jvxInt16*)runtime.oneChunk,
					writeSize, 8,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt8, jvxInt32, jvxInt32>(
					(jvxInt8*)fld_in[i],
					(jvxInt32*)runtime.oneChunk,
					writeSize, 24,
					inOutCnt, 1,
					i, params_file.channels);
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
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt8, float>(
					(jvxInt8*)fld_in[i],
					(float*)runtime.oneChunk,
					writeSize,
					(float)JVX_MAX_INT_8_DIV,
					inOutCnt, 1,
					i, params_file.channels);
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
					(jvxInt16*)fld_in[i],
					(jvxInt16*)runtime.oneChunk,
					writeSize,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case 24:
				jvx_convertSamples_from_fxp_shiftleft_to_bytes<jvxInt16, jvxInt32>(
					(jvxInt16*)fld_in[i],
					(jvxByte*)runtime.oneChunk,
					writeSize, 3, 8,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case 32:
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt32, jvxInt32>(
					(jvxInt16*)fld_in[i],
					(jvxInt32*)runtime.oneChunk,
					writeSize, 16,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		case JVX_FILEFORMAT_IEEE_FLOAT:
			switch (params_file.bits_per_sample)
			{
			case 32:
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, float>(
					(jvxInt16*)fld_in[i],
					(float*)runtime.oneChunk,
					writeSize,
					(float)JVX_MAX_INT_16_DIV,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		switch (params_file.fformat)
		{
		case JVX_FILEFORMAT_PCM_FIXED:
			switch (params_file.bits_per_sample)
			{
			case 16:
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt16, jvxInt32>(
					(jvxInt32*)fld_in[i],
					(jvxInt16*)runtime.oneChunk,
					writeSize, 16,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case 24:
				jvx_convertSamples_from_fxp_shiftright_to_bytes<jvxInt32, jvxInt32>(
					(jvxInt32*)fld_in[i],
					(jvxByte*)runtime.oneChunk,
					writeSize, 3, 8,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case 32:
				jvx_convertSamples_from_fxp_noshift_to_fxp<jvxInt32, jvxInt32, jvxInt32>(
					(jvxInt32*)fld_in[i],
					(jvxInt32*)runtime.oneChunk,
					writeSize,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		case JVX_FILEFORMAT_IEEE_FLOAT:
			switch (params_file.bits_per_sample)
			{
			case 32:
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt32, float>(
					(jvxInt32*)fld_in[i],
					(float*)runtime.oneChunk,
					writeSize,
					(float)JVX_MAX_INT_32_DIV,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}
		break;
		*/
	case JVX_DATAFORMAT_64BIT_LE:
		assert(0);
		/*
		switch(params_file.fformat)
		{
		case JVX_FILEFORMAT_PCM_FIXED:
			switch(params_file.dformat)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt16, jvxInt64>(
					(jvxInt64*)fld_in[i],
					(jvxInt16*)runtime.oneChunk,
					writeSize, 48,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt32, jvxInt64>(
					(jvxInt64*)fld_in[i],
					(jvxInt32*)runtime.oneChunk,
					writeSize, 32,
					inOutCnt, 1,
					i, params_file.channels);
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
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt64, float>(
					(jvxInt64*)fld_in[i],
					(float*)runtime.oneChunk,
					writeSize,
					JVX_MAX_INT_64_DIV,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}*/
		break;
	default:
		assert(0);
	}
	return _process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType
CjvxAuCPcmEncoder::get_configure_token(jvxApiString* astr)
{
	if (astr)
	{
		std::string conf_token;
		jvx_wav_values_2_configtoken(conf_token, &params);
		astr->assign(conf_token);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuCPcmEncoder::update_configure_token(const char* token)
{
	jvxErrorType res = JVX_ERROR_INVALID_FORMAT;
	if (token)
	{
		jvxApiString famToken;
		wav_params params_new;
		res = jvx_wav_configtoken_2_values(token, &params_new, &famToken);

		// When do we accept new settings? We might restrict it to lower bit resolutions if
		// processing is already on
	}
	return JVX_ERROR_UNSUPPORTED;
}
