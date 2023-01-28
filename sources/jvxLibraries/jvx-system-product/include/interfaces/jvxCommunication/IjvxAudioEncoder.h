#ifndef _IJVXAUDIOENCODER_H__
#define _IJVXAUDIOENCODER_H__

JVX_INTERFACE IjvxAudioEncoder: public IjvxInterfaceFactory, public IjvxStateMachine
{
public:	
	
	virtual JVX_CALLINGCONVENTION ~IjvxAudioEncoder(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configure_token(jvxApiString* astr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION update_configure_token(const char* token) = 0;
};

#endif
