#include "CjvxAuCFfmpeg.h"
#include "CjvxAuCFfmpegEncoder.h"
#include "CjvxAuCFfmpegDecoder.h"

CjvxAuCFfmpegAudioCodec::CjvxAuCFfmpegAudioCodec(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxAudioCodec(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	myFamily = "ffmpeg";
	myTypeWc = "*";
}

jvxErrorType
CjvxAuCFfmpegAudioCodec::request_encoder(IjvxAudioEncoder** encoder_on_return)
{
	if (encoder_on_return)
	{
		if (_common_set_min.theState >= JVX_STATE_ACTIVE)
		{
			CjvxAudioEncoder* newEncoder = NULL;
			newEncoder = new CjvxAuCFfmpegAudioEncoder("Ffmpeg Audio Encoder", false, "ffmpeg_audio_encoder", 0, "sub_module", JVX_COMPONENT_ACCESS_SUB_COMPONENT,
				JVX_COMPONENT_AUDIO_ENCODER, "audio_encoder", NULL);
			newEncoder->set_parent(this);
			uniqueId++;
			_common_set_codec.requestedEncoder.push_back(newEncoder);
			*encoder_on_return = newEncoder;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxAuCFfmpegAudioCodec::request_decoder(IjvxAudioDecoder** decoder_on_return)
{
	if (decoder_on_return)
	{
		if (_common_set_min.theState >= JVX_STATE_ACTIVE)
		{
			CjvxAudioDecoder* newDecoder = NULL;
			newDecoder = new CjvxAuCFfmpegAudioDecoder(("Ffmpeg Audio Decoder (" + jvx_int2String(uniqueId) + ")").c_str(), false,
				"ffmpeg_audio_decoder", 0, "sub_module", JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DECODER, "audio_decoder",
				NULL);
			newDecoder->set_parent(this);
			uniqueId++;
			_common_set_codec.requestedDecoder.push_back(newDecoder);
			*decoder_on_return = newDecoder;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}