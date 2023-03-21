#ifndef __CJVXAUCFFMPEGAUDIOENCODER_H__
#define __CJVXAUCFFMPEGAUDIOENCODER_H__

#include "jvxAudioCodecs/CjvxAudioEncoder.h"
#include "jvx-ffmpeg-helpers.h"

class CjvxAuCFfmpegAudioEncoder : public CjvxAudioEncoder
{
	jvxFfmpegEncoderAudioParameter cParams;
	jvxBool inThreadInit = false;

public:
	CjvxAuCFfmpegAudioEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configure_token(jvxApiString* astr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION update_configure_token(const char* token) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	jvxErrorType activate() override;
	virtual jvxErrorType activate_encoder() override;

	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;


	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	void test_set_output_parameters() override;
	void accept_output_parameters() override;

	void codec_allocate_core();
	void codec_deallocate_core();

};

#endif
