#include "playControl.h"

playControl::playControl()
{
	this->playSettings.volume = 0;
	this->playSettings.numberOfChannels = 0;
	this->playSettings.inputChannelOffset = 0;
	this->clippingIndicator = false;
	this->playSettings.monoToStereoPlayback = 1;
	

	this->playProps.inputChannelOffset = 0;
	this->playProps.numberOfChannels = 0;
	this->playProps.volume = 0;
	this->playProps.dirtyFlag = 0;
	this->playProps.monoToStereoPlayback = 1;
	
}

playControl::~playControl()
{

}

void playControl::prepare(int sampleRate, int bufferSize)
{
//	this->sampleRate = sampleRate;
//	this->bufferSize = bufferSize;
//	// this->sampleCounter = 0;
//	this->playPositionInSec = 0;
}

void playControl::updatePlaySettings()
{
	this->playSettings.volume = this->playProps.volume;
	this->playSettings.inputChannelOffset = this->playProps.inputChannelOffset;
	this->playSettings.numberOfChannels = this->playProps.numberOfChannels;
	this->playSettings.monoToStereoPlayback = this->playProps.monoToStereoPlayback;
	this->playProps.dirtyFlag = 0;
}

jvxDspBaseErrorType
playControl::process(jvxData** inputs, jvxData** outputs, jvxSize numChannelsIn, jvxSize numChannelsOut, jvxSize bufferSize)
{
	if (this->playProps.dirtyFlag)
		this->updatePlaySettings();

	
	jvxSize numInputChannelsToPlay = JVX_MIN(this->playSettings.numberOfChannels, numChannelsOut);

	jvxSize numOutputChannelsToPlay = numInputChannelsToPlay;

	if (this->playSettings.monoToStereoPlayback)
		numOutputChannelsToPlay = JVX_MAX(numOutputChannelsToPlay, 2);
	
	numOutputChannelsToPlay = JVX_MIN(numOutputChannelsToPlay, numChannelsOut);

	jvxSize const inputChannelOffset = this->playSettings.inputChannelOffset;
	jvxData const volume_multiplyer = this->playSettings.volume;
	

	// write weighted input channels to output
	for (jvxSize outputChannel = 0; outputChannel < numOutputChannelsToPlay; outputChannel++)
	{

		jvxSize const inputChannel = (outputChannel % numInputChannelsToPlay) + inputChannelOffset;

		for (jvxSize i = 0; i < bufferSize; i++)
		{
			outputs[outputChannel][i] = inputs[inputChannel][i] * volume_multiplyer;
			
			if (outputs[outputChannel][i] > 1.0){
				this->clippingIndicator = true;
			}
		}
	}

	// set rest of output to zero
	for (jvxSize outputChannel = numOutputChannelsToPlay; outputChannel < numChannelsOut; outputChannel++)
	{
		for (jvxSize i = 0; i < bufferSize; i++)
		{
			outputs[outputChannel][i] = 0;
		}
	}






	return(JVX_DSP_NO_ERROR);
}
