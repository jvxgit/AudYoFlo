#ifndef __CJVXFFMPEGAUDIOCODEC_H__
#define __CJVXFFMPEGAUDIOCODEC_H__

#include "jvxAudioCodecs/CjvxAudioCodec.h"

class CjvxAuCFfmpegAudioCodec : public CjvxAudioCodec
{
	friend class CjvxAuCFfmpegAudioEncoder;
	friend class CjvxAuCFfmpegAudioDecoder;
public:
	CjvxAuCFfmpegAudioCodec(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	jvxErrorType request_encoder(IjvxAudioEncoder** encoder_on_return);
	jvxErrorType request_decoder(IjvxAudioDecoder** decoder_on_return);
};

#endif