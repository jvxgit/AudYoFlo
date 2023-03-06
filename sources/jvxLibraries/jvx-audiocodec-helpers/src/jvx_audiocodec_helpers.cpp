#include "jvx.h"
#include "jvx_audiocodec_helpers.h"

static jvxErrorType
jvx_acodec_configtoken_2_values(
	const std::vector<std::string>& expr,
	audio_codec_params_common* params)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvxBool err = false;
	std::string token1;
	std::string token2;

	// Second run for all parameters
	for (i = 0; i < expr.size(); i++)
	{
		std::string token = expr[i];
		size_t posi = std::string::npos;
		posi = token.find("=");
		if (posi != std::string::npos)
		{
			token1 = token.substr(0, posi);
			token2 = token.substr(posi + 1, std::string::npos);

			if (token1 == "sr")
			{
				jvxSize val = jvx_string2Size(token2, err);
				params->srate = val;
			}

			else if (token1 == "bs")
			{
				jvxSize val = jvx_string2Size(token2, err);
				params->bsize = val;
			}

			else if (token1 == "fsm")
			{
				jvxSize val = jvx_string2Size(token2, err);
				params->fsizemax = val;
			}

			else if (token1 == "ch")
			{
				jvxSize val = jvx_string2Size(token2, err);
				params->nchans = val;
			}
		}
	}
	return res;
}

static jvxErrorType
jvx_acodec_values_2_configtoken(
	std::string& tokenRet,
	audio_codec_params_common* paramsArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (JVX_CHECK_SIZE_SELECTED(paramsArg->nchans))
	{
		tokenRet += ";ch=" + jvx_size2String(paramsArg->nchans);
	}
	if (JVX_CHECK_SIZE_SELECTED(paramsArg->srate))
	{
		tokenRet += ";sr=" + jvx_size2String(paramsArg->srate);
	}
	if (JVX_CHECK_SIZE_SELECTED(paramsArg->bsize))
	{
		tokenRet += ";bs=" + jvx_size2String(paramsArg->bsize);
	}
	if (JVX_CHECK_SIZE_SELECTED(paramsArg->fsizemax))
	{
		tokenRet += ";fsm=" + jvx_size2String(paramsArg->fsizemax);
	}
	return res;
}

// ========================================================================================

jvxErrorType
jvx_codec_configtoken_2_fam(
	const char* tokenArg,
	jvxApiString& astrFam)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool err = false;
	jvxSize i;
	std::string token1;
	std::string token2;

	// Decompose into tokens
	std::vector<std::string> expr = jvx_parseCsvStringExpression(tokenArg, err);
	if (err)
	{
		return JVX_ERROR_PARSE_ERROR;
	}

	res = JVX_ERROR_INVALID_FORMAT;

	// Find the type
	for (i = 0; i < expr.size(); i++)
	{
		std::string token = expr[i];
		size_t posi = std::string::npos;
		posi = token.find("=");
		if (posi != std::string::npos)
		{
			jvxBool err = false;
			token1 = token.substr(0, posi);
			token2 = token.substr(posi + 1, std::string::npos);
			if (token1 == "fam")
			{
				astrFam = token2;
				return JVX_NO_ERROR;				
			}
		}
	}
	return res;
}

jvxErrorType
jvx_wav_configtoken_2_values(
	const char* tokenArg,
	wav_params* params,
	jvxApiString* astrFam)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool err = false;
	jvxSize i;
	std::string token1;
	std::string token2;

	// Decompose into tokens
	std::vector<std::string> expr = jvx_parseCsvStringExpression(tokenArg, err);
	if (err)
	{
		return JVX_ERROR_PARSE_ERROR;
	}

	res = JVX_ERROR_INVALID_FORMAT;

	// Find the type
	for (i = 0; i < expr.size(); i++)
	{
		std::string token = expr[i];
		size_t posi = std::string::npos;
		posi = token.find("=");
		if (posi != std::string::npos)
		{
			jvxBool err = false;
			token1 = token.substr(0, posi);
			token2 = token.substr(posi + 1, std::string::npos);
			if (astrFam)
			{
				if (token1 == "fam")
				{
					if (astrFam)
					{
						*astrFam = token2;
					}
				}
			}
			if (params)
			{
				if (token1 == "tp")
				{
					res = JVX_NO_ERROR;
					if (token2 == "wav64sony")
					{
						params->sub_type = audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_64_SONY;
					}
					else if (token2 == "wav64")
					{
						params->sub_type = audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_64;
					}
					else if (token2 == "wav32")
					{
						params->sub_type = audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_32;
					}
					else
					{
						res = JVX_ERROR_INVALID_FORMAT;
					}
				}
			}
		}
	}

	// If we do not need any wav params, return here!
	if (!params)
	{
		return res;
	}

	// If type was correct, scan for paramters
	if (res == JVX_NO_ERROR)
	{
		res = jvx_acodec_configtoken_2_values(
			expr, static_cast<audio_codec_params_common*>(params));
	}

	if (res == JVX_NO_ERROR)
	{
		params->sample_type = audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FLOATING_POINT;// Default
		params->endian = audio_codec_wav_endian::AUDIO_CODEC_WAV_BIG_ENDIAN; // Default

		// Second run for all parameters
		for (i = 0; i < expr.size(); i++)
		{
			std::string token = expr[i];
			size_t posi = std::string::npos;
			posi = token.find("=");
			if (posi != std::string::npos)
			{
				token1 = token.substr(0, posi);
				token2 = token.substr(posi + 1, std::string::npos);
				
				if (token1 == "bps")
				{
					if (token2 == "16")
					{
						params->resolution = audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT;
					}
					else if (token2 == "24")
					{
						params->resolution = audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_24BIT;
					}
					else if (token2 == "32")
					{
						params->resolution = audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT;
					}
				}

			}
			else
			{
				if (token == "pcmfi")
				{
					params->sample_type = audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FIXED_POINT;
				}
				else if (token == "le")
				{
					params->endian = audio_codec_wav_endian::AUDIO_CODEC_WAV_LITTLE_ENDIAN;
				}
			}
		} // for
	} // if (res == JVX_NO_ERROR)
	return res;
}

// ==============================================================================

jvxErrorType
jvx_wav_values_2_configtoken(
	std::string& tokenRet,
	wav_params* paramsArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool err = false;
	jvxSize i;
	
	tokenRet = "fam=jvx;";
	tokenRet += "tp=";
	switch (paramsArg->sub_type)
	{
	case audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_64_SONY:
		tokenRet += "wav64sony;";
		break;
	case audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_64:
		tokenRet += "wav64;";
		break;
	case audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_32:
		tokenRet += "wav32;";
		break;
	default:
		assert(0);
	}

	res = jvx_acodec_values_2_configtoken(
		tokenRet,
		static_cast<audio_codec_params_common*>(paramsArg));

	if (res == JVX_NO_ERROR)
	{

		if (paramsArg->sample_type == audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FIXED_POINT)
		{
			tokenRet += ";pcmfi";
		}
		if (paramsArg->endian == audio_codec_wav_endian::AUDIO_CODEC_WAV_LITTLE_ENDIAN)
		{
			tokenRet += ";le";
		}
		switch(paramsArg->resolution)
		{
		case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT:
			tokenRet += ";bps=16";
			break;
		case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_24BIT:
			tokenRet += ";bps=24";
			break;
		case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT:
			tokenRet += ";bps=32";
			break;
		}
	}
	return res;
}

jvxSize
jvx_wav_compute_bsize_bytes_pcm(wav_params* file_params, jvxSize bsize)
{
	jvxSize nchans = file_params->nchans;
	jvxSize bitssample = jvx_wav_get_bits_sample(file_params);
	jvxSize bytesFrame = (nchans * bitssample * file_params->bsize / 8);
	if (JVX_CHECK_SIZE_SELECTED(bsize))
	{
		// Override
		bytesFrame = (nchans * bitssample * bsize / 8);
	}
	return bytesFrame;
}

jvxSize
jvx_wav_compute_bsize_audio(wav_params* file_params, jvxSize bsize)
{
	jvxSize nchans = file_params->nchans;
	jvxSize bitssample = jvx_wav_get_bits_sample(file_params);
	return (bsize / (bitssample * nchans / 8));
}

jvxSize
jvx_wav_get_bits_sample(wav_params* file_params)
{
	jvxSize bitssample = 16;
	switch (file_params->resolution)
	{
	case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT:
		bitssample = 16;
		break;
	case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_24BIT:
		bitssample = 24;
		break;
	case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT:
		bitssample = 32;
		break;
	}
	return bitssample;
}

std::string
jvx_wav_produce_codec_token(wav_params* file_params)
{
	std::string propstring;
	jvx_wav_values_2_configtoken(propstring, file_params);
	return propstring;
}
