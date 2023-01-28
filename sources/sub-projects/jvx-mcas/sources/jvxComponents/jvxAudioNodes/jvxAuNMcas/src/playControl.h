#include "jvx_dsp_base.h"

#include <string>
#include <vector>


class playControl
{
public:
	playControl();
	~playControl();

	void prepare(int sampleRate, int bufferSize);

	jvxDspBaseErrorType process(jvxData** inputs, jvxData** outputs, jvxSize numChannelsIn, jvxSize numChannelsOut, jvxSize bufferSize);

	
	// properties
	struct { 
		jvxData volume;
		jvxSize inputChannelOffset;
		jvxSize numberOfChannels;
		jvxSize dirtyFlag;
		jvxSize monoToStereoPlayback;
	} playProps;


	// dynamic settings
	struct {
		jvxData volume;
	    jvxSize inputChannelOffset;
		jvxSize numberOfChannels;
		jvxSize monoToStereoPlayback;
	} playSettings;
	
	
	jvxSize clippingIndicator;
private:

	void updatePlaySettings();
	

};