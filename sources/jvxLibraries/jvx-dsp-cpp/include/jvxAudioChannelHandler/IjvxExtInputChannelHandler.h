#ifndef __IJVXEXTINPUTCHANNELHANDLER_H__
#define __IJVXEXTINPUTCHANNELHANDLER_H__

JVX_INTERFACE IjvxExtInputChannelHandler
{

public:
	virtual JVX_CALLINGCONVENTION ~IjvxExtInputChannelHandler(){};

	// Connect this class to external audio interface
	virtual JVX_CALLINGCONVENTION jvxErrorType connectIf(IjvxExternalAudioChannels* theIf, jvxSize numChannelsArg, jvxInt32 sRateArg, std::string theText) = 0;

	// Disconnect this class to external audio interface
	virtual JVX_CALLINGCONVENTION jvxErrorType disconnectIf() = 0;

	// Set a buffer reference from which we take the audio samples
	virtual JVX_CALLINGCONVENTION jvxErrorType setBuffer(jvxHandle** buf, jvxDataFormat formatBufferArg, jvxSize szBufferArg) = 0;

	// Reset the buffer reference if no further audio i/o is desired
	virtual JVX_CALLINGCONVENTION jvxErrorType resetBuffer() = 0;

	// Show current progress while processing
	virtual JVX_CALLINGCONVENTION jvxData progress() = 0;

	// Return number of samples in buffer all together
	virtual JVX_CALLINGCONVENTION jvxSize numberOutputSamples() = 0;

	// Specify behavior parameter to out only full frames. If not specified (default is false), non-full frames are filled up with zeros
	virtual JVX_CALLINGCONVENTION void setPassOnlyFullFrames(jvxBool passOnlyFullFrames) = 0;

	// Shift output by a specific read offset to froward output
	virtual JVX_CALLINGCONVENTION void addReadOffset(jvxSize offset) = 0;

	// Get read offset 
	virtual JVX_CALLINGCONVENTION jvxSize getReadOffset() = 0;

	// Set read offset 
	virtual JVX_CALLINGCONVENTION void setReadOffset(jvxSize offset) = 0;


	virtual JVX_CALLINGCONVENTION jvxErrorType setPause(jvxBool pauseOn) = 0;
	virtual JVX_CALLINGCONVENTION jvxErrorType getPause(jvxBool* pauseIsOn) = 0;

	virtual JVX_CALLINGCONVENTION jvxErrorType setLoopStartStop(jvxSize loopStart = JVX_SIZE_UNSELECTED, jvxSize loopStop = JVX_SIZE_UNSELECTED) = 0;
	virtual JVX_CALLINGCONVENTION jvxErrorType getLoopStartStop(jvxSize* loopStart = NULL, jvxSize* loopStop = NULL) = 0;
};

extern jvxErrorType jvx_factory_allocate_jvxExtInputChannelHandler(IjvxExtInputChannelHandler** returnedObj);
extern jvxErrorType jvx_factory_deallocate_jvxExtInputChannelHandler(IjvxExtInputChannelHandler* returnedObj);

#endif