#include "jvx.h"
#include "jvx-ffmpeg-helpers.h"
#include <cassert>

const char* fileFormatPostfixTranslator[] =
{
	"wav",
	"mp3",
	"m4a",
	nullptr
};

const char* optionsWav[] =
{
	"16Bit,signed",
	"32Bit,signed",
	"32Bit,float",
	"64Bit,signed",
	"64Bit,float",
	nullptr
};

const char* optionsMp3M4a[] =
{
	"32kbps",
	"48kbps",
	"64kbps",
	"128kbps",
	"192kbps",
	"256kbps",
	"320kbps",
	nullptr
};

jvxSize ratesMp3M4a[] =
{
	32000,
	48000,
	64000,
	128000,
	192000,
	256000,
	320000,
	JVX_SIZE_UNSELECTED
};

int check_stream_specifier(AVFormatContext* s, AVStream* st, const char* spec)
{
	int ret = avformat_match_stream_specifier(s, st, spec);
	if (ret < 0)
		av_log(s, AV_LOG_ERROR, "Invalid stream specifier: %s.\n", spec);
	return ret;
};

AVDictionary* filter_codec_opts(AVDictionary* opts, enum AVCodecID codec_id,
	AVFormatContext* s, AVStream* st, const AVCodec* codec)
{
	AVDictionary* ret = NULL;
	const AVDictionaryEntry* t = NULL;
	int            flags = s->oformat ? AV_OPT_FLAG_ENCODING_PARAM
		: AV_OPT_FLAG_DECODING_PARAM;
	char          prefix = 0;
	const AVClass* cc = avcodec_get_class();

	if (!codec)
		codec = s->oformat ? avcodec_find_encoder(codec_id)
		: avcodec_find_decoder(codec_id);

	switch (st->codecpar->codec_type) {
	case AVMEDIA_TYPE_VIDEO:
		prefix = 'v';
		flags |= AV_OPT_FLAG_VIDEO_PARAM;
		break;
	case AVMEDIA_TYPE_AUDIO:
		prefix = 'a';
		flags |= AV_OPT_FLAG_AUDIO_PARAM;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		prefix = 's';
		flags |= AV_OPT_FLAG_SUBTITLE_PARAM;
		break;
	}

	while (t = av_dict_iterate(opts, t)) {
		const AVClass* priv_class;
		char* p = strchr(t->key, ':');

		/* check stream specification in opt name */
		if (p)
			switch (check_stream_specifier(s, st, p + 1)) {
			case  1: *p = 0; break;
			case  0:         continue;
			default:         assert(0);
			}

		if (av_opt_find(&cc, t->key, NULL, flags, AV_OPT_SEARCH_FAKE_OBJ) ||
			!codec ||
			((priv_class = codec->priv_class) &&
				av_opt_find(&priv_class, t->key, NULL, flags,
					AV_OPT_SEARCH_FAKE_OBJ)))
			av_dict_set(&ret, t->key, t->value, 0);
		else if (t->key[0] == prefix &&
			av_opt_find(&cc, t->key + 1, NULL, flags,
				AV_OPT_SEARCH_FAKE_OBJ))
			av_dict_set(&ret, t->key + 1, t->value, 0);

		if (p)
			*p = ':';
	}
	return ret;
}

AVDictionary** setup_find_stream_info_opts(AVFormatContext* s,
	AVDictionary* codec_opts)
{
	int i;
	AVDictionary** opts = nullptr;

	if (!s->nb_streams)
		return NULL;
	opts = (AVDictionary**)av_calloc(s->nb_streams, sizeof(*opts));
	for (i = 0; i < s->nb_streams; i++)
		opts[i] = filter_codec_opts(codec_opts, s->streams[i]->codecpar->codec_id,
			s, s->streams[i], NULL);
	return opts;
}

void 
jvx_ffmpeg_update_derived(const jvxFfmpegAudioParameter& params, jvxFfmpegAudioParameterDerived& derived)
{
	char strBuf[512] = { 0 };
	av_channel_layout_describe(&params.chanLayout, strBuf, sizeof(strBuf));
	derived.chanLayoutTag = strBuf;

	// Get codec id name
	const char* cbuf = avcodec_get_name(params.idCodec);
	if (cbuf)
	{
		derived.codecIdTag = cbuf;
		// av_free((void*)cbuf); <- the get function returned a static string!!
	}

	cbuf = av_get_media_type_string(params.tpCodec);
	if (cbuf)
	{
		derived.codecTypeTag = cbuf;
		// av_free((void*)cbuf); <- the get functions return a static string
	}

	cbuf = av_get_sample_fmt_name(params.sFormatId);
	if (cbuf)
	{
		derived.sFormat = cbuf;
		// av_free((void*)cbuf); <- the get returns a static string		
	}
}

std::string 
jvx_ffmpeg_parameter_2_codec_token(const jvxFfmpegAudioParameter& params)
{
	jvxFfmpegAudioParameterDerived derived;

	std::string strText;
	strText = "fam=ffmpeg";

	jvx_ffmpeg_update_derived(params, derived);

	strText += ";tp=" + derived.codecTypeTag + ":" + params.fFormatTag + ":" + derived.codecIdTag;
	strText += ";cid=" + jvx_size2String((jvxSize)params.idCodec);
	switch (params.idCodec)
	{
	case AV_CODEC_ID_MP3:
		strText += ";cidn=" "AV_CODEC_ID_MP3";
		break;
	case AV_CODEC_ID_PCM_S16LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S16LE";
		break;
	case AV_CODEC_ID_PCM_S16BE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S16BE";
		break;
	case AV_CODEC_ID_PCM_U16LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_U16LE";
		break;
	case AV_CODEC_ID_PCM_U16BE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_U16BE";
		break;
	case AV_CODEC_ID_PCM_S8:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S8";
		break;
	case AV_CODEC_ID_PCM_U8:
		strText += ";cidn=" "AV_CODEC_ID_PCM_U8";
		break;
	case AV_CODEC_ID_PCM_MULAW:
		strText += ";cidn=" "AV_CODEC_ID_PCM_MULAW";
		break;
	case AV_CODEC_ID_PCM_ALAW:
		strText += ";cidn=" "AV_CODEC_ID_PCM_ALAW";
		break;
	case AV_CODEC_ID_PCM_S32LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S32LE";
		break;
	case AV_CODEC_ID_PCM_S32BE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S32BE";
		break;
	case AV_CODEC_ID_PCM_U32LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_U32LE";
		break;
	case AV_CODEC_ID_PCM_U32BE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_U32BE";
		break;
	case AV_CODEC_ID_PCM_S24LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S24LE";
		break;
	case AV_CODEC_ID_PCM_S24BE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S24BE";
		break;
	case AV_CODEC_ID_PCM_U24LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_U24LE";
		break;
	case AV_CODEC_ID_PCM_U24BE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_U24BE";
		break;
	case AV_CODEC_ID_PCM_S24DAUD:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S24DAUD";
		break;
	case AV_CODEC_ID_PCM_ZORK:
		strText += ";cidn=" "AV_CODEC_ID_PCM_ZORK";
		break;
	case AV_CODEC_ID_PCM_S16LE_PLANAR:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S16LE_PLANAR";
		break;
	case AV_CODEC_ID_PCM_DVD:
		strText += ";cidn=" "AV_CODEC_ID_PCM_DVD";
		break;
	case AV_CODEC_ID_PCM_F32BE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_F32BE";
		break;
	case AV_CODEC_ID_PCM_F32LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_F32LE";
		break;
	case AV_CODEC_ID_PCM_F64BE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_F64BE";
		break;
	case AV_CODEC_ID_PCM_F64LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_F64LE";
		break;
	case AV_CODEC_ID_PCM_BLURAY:
		strText += ";cidn=" "AV_CODEC_ID_PCM_BLURAY";
		break;
	case AV_CODEC_ID_PCM_LXF:
		strText += ";cidn=" "AV_CODEC_ID_PCM_LXF";
		break;
	case AV_CODEC_ID_S302M:
		strText += ";cidn=" "AV_CODEC_ID_S302M";
		break;
	case AV_CODEC_ID_PCM_S8_PLANAR:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S8_PLANAR";
		break;
	case AV_CODEC_ID_PCM_S24LE_PLANAR:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S24LE_PLANAR";
		break;
	case AV_CODEC_ID_PCM_S32LE_PLANAR:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S32LE_PLANAR";
		break;
	case AV_CODEC_ID_PCM_S16BE_PLANAR:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S16BE_PLANAR";
		break;
	case AV_CODEC_ID_PCM_S64LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S64LE";
		break;
	case AV_CODEC_ID_PCM_S64BE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_S64BE";
		break;
	case AV_CODEC_ID_PCM_F16LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_F16LE";
		break;
	case AV_CODEC_ID_PCM_F24LE:
		strText += ";cidn=" "AV_CODEC_ID_PCM_F24LE";
		break;
	case AV_CODEC_ID_PCM_VIDC:
		strText += ";cidn=" "AV_CODEC_ID_PCM_VIDC";
		break;
	case AV_CODEC_ID_PCM_SGA:
		strText += ";cidn=" "AV_CODEC_ID_PCM_SGA";
		break;
	default:
		break;
	}

	if (!params.codec_selection.empty())
	{
		strText += ";cii=" + params.codec_selection;
	}
	strText += ";ch=" + jvx_size2String(params.nChans);
	strText += ";chl=" + derived.chanLayoutTag;
	strText += ";sr=" + jvx_size2String(params.sRate);
	strText += ";br=" + jvx_size2String(params.bitRate);
	strText += ";bs=" + jvx_size2String(params.bSizeAudio);

	if (params.frameSize == 0)
	{
		strText += ";bps=" + jvx_size2String(params.bitsPerCoded);		
		strText += ";fsm=" + jvx_size2String(params.frameSizeMax);
		if (params.isFloat)
		{
			strText += ";flt=1";
		}
		else
		{ 
			strText += ";flt=0";
		}
	}	
	return strText;
}

jvxErrorType jvx_ffmpeg_codec_token_2_parameter(const char* tokenArg, jvxFfmpegAudioParameter& params)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool err = false;
	jvxSize i;
	std::string token1;
	std::string token2;
	jvxBool fsIsZero = false;
	jvxCBitField foundVals = 0;

	// Decompose into tokens
	std::vector<std::string> expr = jvx_parseCsvStringExpression(tokenArg, err);
	if (err)
	{
		return JVX_ERROR_PARSE_ERROR;
	}

	jvxFfmpegAudioParameterDerived derived;
	jvx_ffmpeg_update_derived(params, derived);

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
				if (token2 == "ffmpeg")
				{
					foundVals |= 1;
				}
				else
				{
					res = JVX_ERROR_UNSUPPORTED;
					break;
				}
			}
			if (token1 == "tp")
			{
				res = JVX_NO_ERROR;
				std::vector<std::string> args;
				// token2 = arams.codecTypeTag + ":" + params.fFormatTag + ":" + params.codecIdTag;
				jvx_parseStringListIntoTokens(token2, args, ':');
				if (args.size() == 3)
				{
					derived.codecTypeTag = args[0];
					params.fFormatTag = args[1];
					derived.codecIdTag = args[2];
					foundVals |= 2;
				}
				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
					break;
				}
			}

			if (token1 == "cid")
			{
				jvxBool err = false;
				params.idCodec = (AVCodecID)jvx_string2Size(token2, err); // token2
				if (err != true)
				{
					foundVals |= 4;
				}
				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
					break;
				}
			}

			if (token1 == "cii")
			{
				jvxBool err = false;
				params.codec_selection = token2;
			}

			if (token1 == "cidn")
			{
				if (token2 == "AV_CODEC_ID_MP3")
				{
					params.idCodec = AV_CODEC_ID_MP3;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_S16LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_S16LE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_S16BE")
				{
					params.idCodec = AV_CODEC_ID_PCM_S16BE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_U16LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_U16LE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_U16BE")
				{
					params.idCodec = AV_CODEC_ID_PCM_U16BE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_S8")
				{
					params.idCodec = AV_CODEC_ID_PCM_S8;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_U8")
				{
					params.idCodec = AV_CODEC_ID_PCM_U8;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_MULAW")
				{
					params.idCodec = AV_CODEC_ID_PCM_MULAW;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_ALAW")
				{
					params.idCodec = AV_CODEC_ID_PCM_ALAW;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_S32LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_S32LE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_S32BE")
				{
					params.idCodec = AV_CODEC_ID_PCM_S32BE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_U32LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_U32LE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_U32BE")
				{
					params.idCodec = AV_CODEC_ID_PCM_U32BE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_S24LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_S24LE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_S24BE")
				{
					params.idCodec = AV_CODEC_ID_PCM_S24BE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_U24LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_U24LE;
					foundVals |= 8;
				}
				else if (token2 == "AV_CODEC_ID_PCM_U24BE")
				{
					params.idCodec = AV_CODEC_ID_PCM_U24BE;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_S24DAUD")
				{
					params.idCodec = AV_CODEC_ID_PCM_S24DAUD;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_ZORK")
				{
					params.idCodec = AV_CODEC_ID_PCM_ZORK;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_S16LE_PLANAR")
				{
					params.idCodec = AV_CODEC_ID_PCM_S16LE_PLANAR;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_DVD")
				{
					params.idCodec = AV_CODEC_ID_PCM_DVD;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_F32BE")
				{
					params.idCodec = AV_CODEC_ID_PCM_F32BE;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_F32LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_F32LE;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_F64BE")
				{
					params.idCodec = AV_CODEC_ID_PCM_F64BE;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_F64LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_F64LE;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_BLURAY")
				{
					params.idCodec = AV_CODEC_ID_PCM_BLURAY;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_LXF")
				{
					params.idCodec = AV_CODEC_ID_PCM_LXF;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_S302M")
				{
					params.idCodec = AV_CODEC_ID_S302M;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_S8_PLANAR")
				{
					params.idCodec = AV_CODEC_ID_PCM_S8_PLANAR;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_S24LE_PLANAR")
				{
					params.idCodec = AV_CODEC_ID_PCM_S24LE_PLANAR;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_S32LE_PLANAR")
				{
					params.idCodec = AV_CODEC_ID_PCM_S32LE_PLANAR;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_S16BE_PLANAR")
				{
					params.idCodec = AV_CODEC_ID_PCM_S16BE_PLANAR;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_S64LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_S64LE;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_S64BE")
				{
					params.idCodec = AV_CODEC_ID_PCM_S64BE;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_F16LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_F16LE;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_F24LE")
				{
					params.idCodec = AV_CODEC_ID_PCM_F24LE;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_VIDC")
				{
					params.idCodec = AV_CODEC_ID_PCM_VIDC;
					foundVals |= 8;
				}

				else if (token2 == "AV_CODEC_ID_PCM_SGA")
				{
					params.idCodec = AV_CODEC_ID_PCM_SGA;
					foundVals |= 8;
				}

				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
				}
			}

			if (token1 == "ch")
			{
				jvxBool err = false;
				params.nChans = jvx_string2Size(token2, err);
				if (!err)
				{
					foundVals |= 0x10;
				}
				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
					break;
				}
			}

			if (token1 == "chl")
			{
				jvxBool err = false;
				foundVals |= 0x20;
				derived.chanLayoutTag = token2;
			}

			if (token1 == "sr")
			{
				jvxBool err = false;
				params.sRate = jvx_string2Size(token2, err);
				if (!err)
				{
					foundVals |= 0x40;
				}
				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
					break;
				}
			}

			if (token1 == "br")
			{
				jvxBool err = false;
				params.bitRate = jvx_string2Size(token2, err);
				if (!err)
				{
					foundVals |= 0x80;
				}
				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
					break;
				}
			}

			if (token1 == "bs")
			{
				jvxBool err = false;
				params.bSizeAudio = jvx_string2Size(token2, err);
				if (!err)
				{
					foundVals |= 0x80;
				}
				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
					break;
				}
			}

			if (token1 == "bps")
			{
				fsIsZero = true;
				jvxBool err = false;
				params.bitsPerCoded = jvx_string2Size(token2, err);
				if (!err)
				{
					foundVals |= 0x80;
				}
				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
					break;
				}
			}

			if (token1 == "fsm")
			{
				fsIsZero = true;
				jvxBool err = false;
				params.frameSizeMax = jvx_string2Size(token2, err);
				if (!err)
				{
					foundVals |= 0x100;
				}
				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
					break;
				}
			}
			if (token1 == "flt")
			{
				fsIsZero = true;
				jvxBool err = false;
				params.isFloat = (jvx_string2Size(token2, err) != 0);
				if (!err)
				{
					foundVals |= 0x200;
				}
				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
					break;
				}
			}
		}
	}

	if (fsIsZero)
	{
		params.frameSize = 0;
	}

	if (foundVals == 0x3FF)
	{
		res = JVX_NO_ERROR;
	}

	return res;
}

void
jvx_ffmpeg_update_format_settings_wav(jvxFfmpegAudioParameter& params, jvxSize idSubType)
{
	// 
	// Default settings
	params.bitsPerCoded = 16;
	params.idCodec = AV_CODEC_ID_PCM_S16LE;
	params.sFormatId = AV_SAMPLE_FMT_S16;
	params.isFloat = false;

	switch (idSubType)
	{
	case 0:
		params.bitsPerCoded = 16;
		params.idCodec = AV_CODEC_ID_PCM_S16LE;
		params.sFormatId = AV_SAMPLE_FMT_S16;
		params.isFloat = false;
		break;
	case 1:
		params.bitsPerCoded = 32;
		params.idCodec = AV_CODEC_ID_PCM_S32LE;
		params.sFormatId = AV_SAMPLE_FMT_S32;
		params.isFloat = false;
		break;
	case 2:
		params.bitsPerCoded = 32;
		params.idCodec = AV_CODEC_ID_PCM_F32LE;
		params.sFormatId = AV_SAMPLE_FMT_FLT;
		params.isFloat = true;
		break;
	case 3:
		params.bitsPerCoded = 64;
		params.idCodec = AV_CODEC_ID_PCM_S64LE;
		params.sFormatId = AV_SAMPLE_FMT_S64;
		params.isFloat = false;
		break;
	case 4:
		params.bitsPerCoded = 64;
		params.idCodec = AV_CODEC_ID_PCM_F64LE;
		params.sFormatId = AV_SAMPLE_FMT_DBL;
		params.isFloat = true;
		break;
	}

	params.frameSize = 0;
	params.frameSizeMax = 4096; // Start with a 1024 buffersize
	params.bSizeAudio = params.frameSizeMax / params.nChans / (params.bitsPerCoded / 8);
	params.bitRate = params.sRate * params.nChans * params.bitsPerCoded;
	params.bitsPerRaw = 0;
}

void
jvx_ffmpeg_update_format_settings_mp3(jvxFfmpegAudioParameter& params, jvxSize idSubType)
{
	// Some parameters will be fixed by the verification
	// params.sFormatId = cod->sample_fmts;
	params.isFloat = false;

	assert(JVX_CHECK_SIZE_SELECTED(ratesMp3M4a[idSubType]));

	params.bitRate = ratesMp3M4a[idSubType];

	params.frameSize = 0;
	params.frameSizeMax = 0; 
	params.bitsPerRaw = 0;
}

void
jvx_ffmpeg_verify_correct_codec_settings(jvxFfmpegAudioParameter& params)
{
	const AVCodec* cod = nullptr;
	if (params.codec_selection.empty())
	{
		cod = avcodec_find_encoder(params.idCodec);
	}
	else
	{
		cod = avcodec_find_encoder_by_name(params.codec_selection.c_str());
	}
	assert(cod);

	params.tpCodec = cod->type;

	jvxSize rateMatch = params.sRate;
	if (cod->supported_samplerates)
	{
		int cnt = 0;
		jvxSize delta = JVX_MAX_INT_64_DATA;
		while (1)
		{
			if (cod->supported_samplerates[cnt] >= 0)
			{
				jvxSize dd = JVX_ABS(cod->supported_samplerates[cnt] - rateMatch);
				if (dd < delta)
				{
					delta = dd;
					rateMatch = cod->supported_samplerates[cnt];

				}
				cnt++;
			}
			else
			{
				break;
			}
			cnt++;
		}
	}
	params.sRate = rateMatch;

	int format = AV_SAMPLE_FMT_NONE;
	if (cod->sample_fmts)
	{
		int cnt = 0;
		while (1)
		{
			if (cod->sample_fmts[cnt] >= 0)
			{
				if (cnt == 0)
				{
					// Default case: use the first valid format
					format = cod->sample_fmts[cnt];
				}

				// Override case: use the format that matches (if multi apply)
				if (cod->sample_fmts[cnt] == params.sFormatId) 
				{
					format = params.sFormatId;
					break;
				}
				cnt++;
			}
			else
			{
				break;
			}
		}
	}
	if (format >= 0)
	{
		params.sFormatId = (AVSampleFormat)format;
	}

	// Setup the default channel map
	params.chanLayout.nb_channels = params.nChans;
	params.chanLayout.order = AV_CHANNEL_ORDER_UNSPEC;
	params.chanLayout.u.mask = 0;
}

void jvx_ffmpeg_wav_params(jvxFfmpegAudioParameter& params)
{
	if (params.fFormatTag == "wav")
	{
		params.frameSizeMax = params.bSizeAudio * params.nChans * params.bitsPerCoded;
		params.frameSizeMax = params.frameSizeMax >> 3; // bits to bytes
	}
	else
	{
		params.frameSizeMax = 0;
	}
}

void jvx_ffmpeg_printout_params(jvxFfmpegAudioParameter& params)
{
	jvxFfmpegAudioParameterDerived derived;
	jvx_ffmpeg_update_derived(params, derived);

	std::cout << " -> Codec <" << derived.codecIdTag << ">: " << std::endl;
	std::cout << " -> File format <" << params.fFormatTag << ">: " << std::endl;
	std::cout << " -> Bit rate <" << params.bitRate << ">: " << std::endl;
	std::cout << " -> Sample format <" << derived.sFormat << ">: " << std::endl;
	std::cout << " -> Sample rate <" << params.sRate << ">: " << std::endl;
	std::cout << " -> Frame size <" << params.frameSize << ">: " << std::endl;
	std::cout << " -> Number channels <" << params.nChans << ">: " << std::endl;
}


void jvx_ffmpeg_printout_file_params(jvxFfmpegFileAudioParameter& params)
{
	jvxFfmpegAudioParameterDerived derived;
	jvx_ffmpeg_update_derived(params, derived);

	std::cout << "Filename <" << params.fName << ">: " << std::endl;
	jvx_ffmpeg_printout_params( params);
}

void jvx_ffmpeg_printout_input_file_params(jvxFfmpegInputFileAudioParameter& params)
{
	jvx_ffmpeg_printout_file_params(params);
	std::cout << " ii --> Length [samples] <" << params.lengthSamples << ">: " << std::endl;
	std::cout << " ii --> Length [secs] <" << params.lengthSecs << ">: " << std::endl;
	std::cout << " ii --> Number frames <" << params.numFrames << ">: " << std::endl;
}

