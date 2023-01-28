#ifndef _IJVXAUDIOCODEC_H__
#define _IJVXAUDIOCODEC_H__

JVX_INTERFACE IjvxAudioCodec: public IjvxObject, public IjvxStateMachine
{
public:
	//=====================================================================
	virtual JVX_CALLINGCONVENTION ~IjvxAudioCodec(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION accept_for_decoding(const char* format_token, jvxBool* compr, jvxApiString* astr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION provide_for_encoding(const char* format_token_preferred, jvxBool* compr, jvxApiString* astr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_supported_format_groups(jvxSize* num) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION supports_format_group(jvxSize idx, jvxDataFormatGroup* supported_format)	= 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_encoder(IjvxAudioEncoder** encoder_on_return) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION return_encoder(IjvxAudioEncoder* encoder_on_return) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION request_decoder(IjvxAudioDecoder** decoder_on_return) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION return_decoder(IjvxAudioDecoder* decoder_on_return) = 0;

};

#endif
