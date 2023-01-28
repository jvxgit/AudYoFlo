// The "kernel" is an object that processes a audio stream, which contains
// one or more channels. It is supposed to obtain the frame data from an
// |input|, process and fill an |output| of the AudioWorkletProcessor.
//
//       AudioWorkletProcessor Input(multi-channel, 128-frames)
//                                 |
//                                 V
//                               Kernel
//                                 |
//                                 V
//       AudioWorkletProcessor Output(multi-channel, 128-frames)
//
// In this implementation, the kernel operates based on 128-frames, which is
// the render quantum size of Web Audio API.

#include "jvx.h"

class SimpleKernel {

	// Single parameter
	jvxData gain = 1.0;
	jvxData avrgV = 0.0;
	jvxData maxV = 0.0;
	jvxCBool clipF = c_false;
  
	// Populated after "initialize"
	jvxData** inBufs = nullptr;
	jvxData** outBufs = nullptr;
	int procChansMax = 0;
	int procBSize = 0;
	jvxSize srate = 0;

	jvxData* gainLevel = nullptr;
	jvxData* avrgLevel = nullptr;
	jvxData* maxLevel = nullptr;
	jvxData* maxLevelLast = nullptr;
	jvxCBool* clipPtr = nullptr;
	jvxSize* chanMap = nullptr;
	jvxSize* cntSamplesSinceMax = nullptr;
  
 public:
 
	// Constructor - do nothing
	 SimpleKernel();
	
	// Destructor - call terminate
	 ~SimpleKernel();

	  // Call this function to pass processing parameters
	 void initialize(int bsize, int srate, int chans, int chansMax);
  
	  // Terminate function
	 void terminate();
  
	// Process callback
	 void Process(uintptr_t input_ptr, uintptr_t output_ptr, unsigned channel_count);

// Parameter set/get functions
	 void setGain(jvxData val);

	 jvxData getGain();

	 void verifySize(int len);

	 void snapshot();

	 jvxData getAvrg();
	 jvxData getMax();
	 jvxCBool getClip();
};