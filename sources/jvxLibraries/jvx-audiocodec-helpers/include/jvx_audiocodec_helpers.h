#ifndef __JVX_AUDIOCODEC_HELPERS_H__
#define __JVX_AUDIOCODEC_HELPERS_H__

struct audio_codec_params_common
{
	jvxSize srate = JVX_SIZE_UNSELECTED;
	jvxSize bsize = JVX_SIZE_UNSELECTED;
	jvxSize nchans = JVX_SIZE_UNSELECTED;
	jvxSize fsizemax = JVX_SIZE_UNSELECTED;
};

// =================================================================
// =================================================================

enum class audio_codec_wav_resolution
{
	AUDIO_CODEC_WAV_RESOLUTION_16BIT,
	AUDIO_CODEC_WAV_RESOLUTION_24BIT,
	AUDIO_CODEC_WAV_RESOLUTION_32BIT,
	AUDIO_CODEC_WAV_RESOLUTION_64BIT,
};

enum class audio_codec_wav_sample_type
{
	AUDIO_CODEC_WAV_FIXED_POINT,
	AUDIO_CODEC_WAV_FLOATING_POINT
};

enum class audio_codec_wav_endian
{
	AUDIO_CODEC_WAV_LITTLE_ENDIAN,
	AUDIO_CODEC_WAV_BIG_ENDIAN
}; 

enum class audio_codec_wav_sub_type
{
	AUDIO_CODEC_WAV_SUBTYPE_WAV_64_SONY,
	AUDIO_CODEC_WAV_SUBTYPE_WAV_64,
	AUDIO_CODEC_WAV_SUBTYPE_WAV_32
};

struct wav_params : public audio_codec_params_common
{
	audio_codec_wav_endian endian = audio_codec_wav_endian::AUDIO_CODEC_WAV_LITTLE_ENDIAN;
	audio_codec_wav_sample_type sample_type = audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FIXED_POINT;
	audio_codec_wav_resolution resolution = audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT;
	audio_codec_wav_sub_type sub_type = audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_32;
};

// =================================================================
// =================================================================

jvxErrorType
jvx_codec_configtoken_2_fam(
	const char* tokenArg,
	jvxApiString* astrFam);

jvxErrorType jvx_wav_configtoken_2_values(
	const char* tokenArg,
	wav_params* params,
	jvxApiString* astrFam);

// =================================================================

jvxErrorType jvx_wav_values_2_configtoken(
	std::string& tokenRet,
	wav_params* paramsArg);

jvxSize jvx_wav_compute_bsize_bytes_pcm(wav_params* paramsArg, jvxSize nomBsize);
jvxSize jvx_wav_compute_bsize_audio(wav_params* paramsArg, jvxSize nomBsize);
jvxSize jvx_wav_get_bits_sample(wav_params* paramsArg);

std::string jvx_wav_produce_codec_token(wav_params* file_params);

#endif
