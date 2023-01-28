#ifndef __CJVXAUDIOCODEC_H__
#define __CJVXAUDIOCODEC_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"

#include "pcg_CjvxAudioCodec_pcg.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxAudioCodec: public IjvxAudioCodec, public CjvxObject
{
	friend class CjvxAudioEncoder;
	friend class CjvxAudioDecoder;

protected:
	
	struct
	{
		std::vector<IjvxAudioEncoder*> requestedEncoder;
		std::vector<IjvxAudioDecoder*> requestedDecoder;
	} _common_set_codec;

	jvxInt32 uniqueId;

#ifdef JVX_OS_WINDOWS
#pragma pack(push, 1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

	typedef struct
	{
		jvxInt8 numChannels;
		jvxInt8 mode;
		jvxInt16 bsize;
		jvxInt32 srate;
	} pcm_fld_header;

#ifdef JVX_OS_WINDOWS
#pragma pack(pop)
#else
#pragma pack(pop)
#endif 

public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxAudioCodec(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAudioCodec();

	virtual jvxErrorType JVX_CALLINGCONVENTION accept_for_decoding(const char* format_token, jvxBool* compr, jvxApiString* astr) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION provide_for_encoding(const char* format_token_preferred, jvxBool* compr, jvxApiString* astr) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_supported_format_groups(jvxSize* num) override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION supports_format_group(jvxSize idx, jvxDataFormatGroup* supported_format)	override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION request_encoder(IjvxAudioEncoder** encoder_on_return)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_encoder(IjvxAudioEncoder* encoder_on_return)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_decoder(IjvxAudioDecoder** decoder_on_return)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_decoder(IjvxAudioDecoder* decoder_on_return)override;

#include "codeFragments/simplify/jvxStateMachineFull_simplify.h"

#include "codeFragments/simplify/jvxObjects_simplify.h"

// #include "codeFragments/simplify/jvxInterfaceReference_simplify.h"	

	virtual void computeRateAndOutBuffersize(CjvxAudioCodec_genpcg* myCfgStrict);
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
