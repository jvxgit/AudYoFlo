#ifndef __CJVXAUCPCMENCODER_H__
#define __CJVXAUCPCMENCODER_H__

#include "jvxAudioCodecs/CjvxAudioEncoder.h"
#include "pcg_CjvxAudioCodec_pcg.h"

class CjvxAuCPcm;

class CjvxAuCPcmEncoder : public CjvxAudioEncoder, public CjvxAudioCodec_genpcg
{
	wav_params params;
	CjvxAuCPcm* myParent = nullptr;
public:
	JVX_CALLINGCONVENTION CjvxAuCPcmEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual jvxErrorType activate() override;
	virtual jvxErrorType deactivate() override;

	virtual jvxErrorType set_property(jvxCallManagerProperties& callGate,
			const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
			const jvx::propertyAddress::IjvxPropertyAddress& ident,
			const jvx::propertyDetail::CjvxTranferDetail& trans) override;

	virtual jvxErrorType check_backward_parameters(jvxLinkDataDescriptor* ld_cp, jvxLinkDataDescriptor& forward, jvxBool& forwardRequest) override;
	virtual void accept_input_parameters() override;
	virtual void test_set_output_parameters() override;

	virtual jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configure_token(jvxApiString* astr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION update_configure_token(const char* token) override;

	void set_parent(CjvxAuCPcm* parent) { myParent = parent; };
};

#endif