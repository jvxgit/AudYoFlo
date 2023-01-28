#include "jvx_myNewProject.h"

jvx_myNewProject::jvx_myNewProject()
{
	this->myVolume = 1.0;
}

jvx_myNewProject::~jvx_myNewProject()
{
}

jvxDspBaseErrorType jvx_myNewProject::process(jvxData** inputs, jvxData** outputs, jvxSize numChannelsIn, jvxSize numChannelsOut, jvxSize bufferSize)
{
	if ( (numChannelsIn > 0) &&	(numChannelsOut > 0) )
	{
			for (jvxSize j = 0; j < bufferSize; j++)
			{
				for (jvxSize channelIdxOut = 0; channelIdxOut < numChannelsOut; channelIdxOut++)
				{
					jvxSize channelIdxIn = channelIdxOut % numChannelsIn;
					outputs[channelIdxOut][j] = inputs[channelIdxIn][j] * this->myVolume;
				}
			}
	}
	return(JVX_DSP_NO_ERROR);
}