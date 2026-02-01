#include "jvx.h"

extern "C"
{	
	void* init_ayf_starter(int nChannelsIn, int nChannelsOut, int fsize, int samplerate, int ayfIdentSlot_node, int ayfIdentSlot_dev, jvxHandle* priv);
	int term_ayf_starter(void* hdlArg);
	int run_frame_ayf_starter(void* hdlArg, int nChannelsIn, int nChannelsOut, int fSize, jvxData* ptrsIn, jvxData* ptrsOut);
}