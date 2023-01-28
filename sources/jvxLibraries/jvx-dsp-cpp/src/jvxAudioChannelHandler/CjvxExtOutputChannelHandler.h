#ifndef __CJVXEXTOUTPUTCHANNELHANDLER_H__
#define __CJVXEXTOUTPUTCHANNELHANDLER_H__

#include "jvx.h"
#include "jvxAudioChannelHandler/IjvxExtOutputChannelHandler.h"

class CjvxExtOutputChannelHandler: public IjvxExtOutputChannelHandler, public IjvxExternalAudioChannels_data
{
private:
	IjvxExternalAudioChannels* myInterface;

	jvxSize numChannels;
	jvxInt32 sRate;
	jvxSize myIdIn;

	struct
	{
		jvxHandle** myBuffer;
		jvxDataFormat formBuffer;
		jvxSize szBuffer;
		jvxSize writeOffset;
		jvxSize framesize;
		jvxSize numSkipped;
	} inProcessing;

	struct
	{
		jvxDataFormat format;
		jvxSize framesize;
	} paramsOnStart;

public:
	CjvxExtOutputChannelHandler();

	// Connect this class to external audio interface
	virtual JVX_CALLINGCONVENTION jvxErrorType connectIf(IjvxExternalAudioChannels* theIf, jvxSize numChannelsArg, jvxInt32 sRateArg, std::string theText);

	// Disconnect this class to external audio interface
	virtual JVX_CALLINGCONVENTION jvxErrorType disconnectIf();

	// Set a buffer reference in which we store the audio samples
	virtual JVX_CALLINGCONVENTION jvxErrorType setBuffer(jvxHandle** buf, jvxDataFormat formatBufferArg, jvxSize szBufferArg);

	// Reset the buffer reference if no further audio i/o is desired
	virtual JVX_CALLINGCONVENTION jvxErrorType resetBuffer();

	// We may shift first few samples on write
	virtual JVX_CALLINGCONVENTION void addWriteOffset(jvxSize numSkipped);

	// Return number of currently written samples
	virtual JVX_CALLINGCONVENTION jvxSize getNumberWritten();

	// =====================================================================

	// Following functions are abstract API functions of interface IjvxExternalAudioChannels_data
	virtual jvxErrorType JVX_CALLINGCONVENTION get_one_frame(jvxSize register_id, jvxHandle** fld);
	virtual jvxErrorType JVX_CALLINGCONVENTION put_one_frame(jvxSize register_id, jvxHandle** fld);
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare(jvxBool is_input, jvxSize register_id, jvxSize framesize, jvxDataFormat format);
	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxBool is_input, jvxSize register_id);
	virtual jvxErrorType JVX_CALLINGCONVENTION stop(jvxBool is_input, jvxSize register_id);
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess(jvxBool is_input, jvxSize register_id);
};

#endif