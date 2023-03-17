#ifndef __CJVXAUCFFMPEGAUDIOENCODER_H__
#define __CJVXAUCFFMPEGAUDIOENCODER_H__

#include "jvxAudioCodecs/CjvxAudioEncoder.h"
#include "jvx-ffmpeg-helpers.h"

class CjvxAuCFfmpegAudioEncoder : public CjvxAudioEncoder
{
	jvxFfmpegFileAudioParameter cParams;

public:
	CjvxAuCFfmpegAudioEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configure_token(jvxApiString* astr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION update_configure_token(const char* token) override;

	jvxErrorType activate() override;
	virtual jvxErrorType activate_encoder() override;

	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	void test_set_output_parameters() override;
	void accept_output_parameters() override;

};

#endif
