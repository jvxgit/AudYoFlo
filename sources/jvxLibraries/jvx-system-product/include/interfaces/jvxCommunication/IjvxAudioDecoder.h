#ifndef _IJVXAUDIODECODER_H__
#define _IJVXAUDIODECODER_H__

JVX_INTERFACE IjvxAudioDecoder: public IjvxSimpleNode
{
public:	

	virtual JVX_CALLINGCONVENTION ~IjvxAudioDecoder() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_configure_token(const char* token) = 0;
};

#endif
