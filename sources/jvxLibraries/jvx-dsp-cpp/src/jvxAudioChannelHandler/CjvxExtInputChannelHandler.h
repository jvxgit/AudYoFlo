#ifndef __CJVXEXTINPUTCHANNELHANDLER_H__
#define __CJVXEXTINPUTCHANNELHANDLER_H__

#include "jvx.h"
#include "jvxAudioChannelHandler/IjvxExtInputChannelHandler.h"

class CjvxExtInputChannelHandler: public IjvxExtInputChannelHandler, public IjvxExternalAudioChannels_data
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
		jvxSize readOffset;
		jvxSize framesize;
		jvxBool passOnlyFullFrames;

		jvxBool pauseOn;
		jvxSize loop_start;
		jvxSize loop_stop;
	} inProcessing;

	struct
	{
		jvxDataFormat format;
		jvxSize framesize;
	} paramsOnStart;

public:
	CjvxExtInputChannelHandler();

	// Connect this class to external audio interface
	virtual JVX_CALLINGCONVENTION jvxErrorType connectIf(IjvxExternalAudioChannels* theIf, jvxSize numChannelsArg, jvxInt32 sRateArg, std::string theText);

	// Disconnect this class to external audio interface
	virtual JVX_CALLINGCONVENTION jvxErrorType disconnectIf();

	// Set a buffer reference from which we take the audio samples
	virtual JVX_CALLINGCONVENTION jvxErrorType setBuffer(jvxHandle** buf, jvxDataFormat formatBufferArg, jvxSize szBufferArg);

	// Reset the buffer reference if no further audio i/o is desired
	virtual JVX_CALLINGCONVENTION jvxErrorType resetBuffer();

	virtual JVX_CALLINGCONVENTION jvxErrorType setPause(jvxBool pauseOn);
	virtual JVX_CALLINGCONVENTION jvxErrorType getPause(jvxBool* pauseIsOn);

	virtual JVX_CALLINGCONVENTION jvxErrorType setLoopStartStop(jvxSize loopStart = JVX_SIZE_UNSELECTED, jvxSize loopStop = JVX_SIZE_UNSELECTED);
	virtual JVX_CALLINGCONVENTION jvxErrorType getLoopStartStop(jvxSize* loopStart = NULL, jvxSize* loopStop = NULL);

	// Show current progress while processing
	virtual JVX_CALLINGCONVENTION jvxData progress();

	// Return number of samples in buffer all together
	virtual JVX_CALLINGCONVENTION jvxSize numberOutputSamples();

	// Specify behavior parameter to out only full frames. If not specified (default is false), non-full frames are filled up with zeros
	virtual JVX_CALLINGCONVENTION void setPassOnlyFullFrames(jvxBool passOnlyFullFrames);

	// Shift output by a specific read offset to froward output
	virtual JVX_CALLINGCONVENTION void addReadOffset(jvxSize offset);

	// Get read offset 
	virtual JVX_CALLINGCONVENTION jvxSize getReadOffset();

	// Set read offset 
	virtual JVX_CALLINGCONVENTION void setReadOffset(jvxSize offset);

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