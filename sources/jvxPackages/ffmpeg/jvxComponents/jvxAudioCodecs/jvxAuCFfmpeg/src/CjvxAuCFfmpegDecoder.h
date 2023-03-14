#ifndef __CJVXAUCFFMPEGAUDIODECODER_H__
#define __CJVXAUCFFMPEGAUDIODECODER_H__

#include "jvxAudioCodecs/CjvxAudioDecoder.h"
#include "jvx-ffmpeg-helpers.h"

class CjvxAuCFfmpegAudioDecoder : public CjvxAudioDecoder
{
	struct jvxFfmpegCodecParameter : public jvxFfmpegParameter
	{
		std::string codecName;
		AVCodecContext* cc = nullptr;
		const AVCodec* codec = nullptr;
		void reset()
		{
			jvxFfmpegParameter::reset();
			codecName.clear();
			cc = nullptr;
			codec = nullptr;
		}
	};
	
	jvxFfmpegCodecParameter cParams;

private:

public:
	CjvxAuCFfmpegAudioDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;

	jvxErrorType set_configure_token(const char* tokenArg);
	virtual jvxErrorType configure_decoder(const char* tokenArg) override;

};

#endif
