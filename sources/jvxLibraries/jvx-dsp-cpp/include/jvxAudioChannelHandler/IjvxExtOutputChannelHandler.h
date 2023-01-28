#ifndef __IJVXEXTOUTPUTCHANNELHANDLER_H__
#define __IJVXEXTOUTPUTCHANNELHANDLER_H__

JVX_INTERFACE IjvxExtOutputChannelHandler
{

public:
	virtual JVX_CALLINGCONVENTION ~IjvxExtOutputChannelHandler() {};

	// Connect this class to external audio interface
	virtual JVX_CALLINGCONVENTION jvxErrorType connectIf(IjvxExternalAudioChannels* theIf, jvxSize numChannelsArg, jvxInt32 sRateArg, std::string theText) = 0;

	// Disconnect this class to external audio interface
	virtual JVX_CALLINGCONVENTION jvxErrorType disconnectIf() = 0;

	// Set a buffer reference in which we store the audio samples
	virtual JVX_CALLINGCONVENTION jvxErrorType setBuffer(jvxHandle** buf, jvxDataFormat formatBufferArg, jvxSize szBufferArg) = 0;

	// Reset the buffer reference if no further audio i/o is desired
	virtual JVX_CALLINGCONVENTION jvxErrorType resetBuffer() = 0;

	// We may shift first few samples on write
	virtual JVX_CALLINGCONVENTION void addWriteOffset(jvxSize numSkipped) = 0;

	// Return number of currently written samples
	virtual JVX_CALLINGCONVENTION jvxSize getNumberWritten() = 0;
};

extern jvxErrorType jvx_factory_allocate_jvxExtOutputChannelHandler(IjvxExtOutputChannelHandler** returnedObj);
extern jvxErrorType jvx_factory_deallocate_jvxExtOutputChannelHandler(IjvxExtOutputChannelHandler* returnedObj);

#endif