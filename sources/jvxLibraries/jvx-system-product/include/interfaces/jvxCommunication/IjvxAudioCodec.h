#ifndef _IJVXAUDIOCODEC_H__
#define _IJVXAUDIOCODEC_H__

JVX_INTERFACE IjvxAudioCodec: public IjvxObject, public IjvxStateMachine
{
public:
	//=====================================================================
	virtual JVX_CALLINGCONVENTION ~IjvxAudioCodec(){};

	//! Pass in a format_token and check if the codec fits the format
	virtual jvxErrorType JVX_CALLINGCONVENTION accept_for_decoding(const char* format_token, jvxBool* compr, jvxApiString* astr) = 0;

	//! ??
	virtual jvxErrorType JVX_CALLINGCONVENTION provide_for_encoding(const char* format_token_preferred, jvxBool* compr, jvxApiString* astr) = 0;
	
	//! Check if the codec supports a given format group
	virtual jvxErrorType JVX_CALLINGCONVENTION supports_format_group(jvxDataFormatGroup query_format)	= 0;

	//! Request an encoder instance
	virtual jvxErrorType JVX_CALLINGCONVENTION request_encoder(IjvxAudioEncoder** encoder_on_return) = 0;
	
	//! Return and release an encoder instance
	virtual jvxErrorType JVX_CALLINGCONVENTION return_encoder(IjvxAudioEncoder* encoder_on_return) = 0;
	
	//! Request a decoder instance
	virtual jvxErrorType JVX_CALLINGCONVENTION request_decoder(IjvxAudioDecoder** decoder_on_return) = 0;
	
	//! Return and release a decoder instance
	virtual jvxErrorType JVX_CALLINGCONVENTION return_decoder(IjvxAudioDecoder* decoder_on_return) = 0;
};

#endif
