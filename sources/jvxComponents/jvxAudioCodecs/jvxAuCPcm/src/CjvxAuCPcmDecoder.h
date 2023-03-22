#ifndef __CJVXAUCPCMDECODER_H__
#define __CJVXAUCPCMDECODER_H__

#include "jvxAudioCodecs/CjvxAudioDecoder.h"
#include "pcg_CjvxAudioCodec_pcg.h"

class CjvxAuCPcm;

class CjvxAuCPcmDecoder : public CjvxAudioDecoder, public CjvxAudioCodec_genpcg

{
	CjvxAuCPcm* myParent = nullptr;

public:
	JVX_CALLINGCONVENTION CjvxAuCPcmDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	virtual void accept_input_parameters()override;
	virtual void test_set_output_parameters() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION set_configure_token(const char* token) override;
	jvxErrorType updated_backward_format_spec(jvxLinkDataDescriptor& forward, jvxLinkDataDescriptor* ld_cp)override;
	
	virtual jvxErrorType configure_decoder(const char* tokenArg);

	void set_parent(CjvxAuCPcm* parent) { myParent = parent; };


};

#endif
