#include "jvx.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

extern "C"
{	
	void* init_ayf_starter(int nChannelsIn, int nChannelsOut, int fsize, int samplerate, int ayfIdentSlot_node, int ayfIdentSlot_dev, const char* nmIniFile);
	int term_ayf_starter(void* hdlArg);
	int run_frame_ayf_starter(void* hdlArg, int nChannelsIn, int nChannelsOut, int fSize, 
		pybind11::array_t<jvxData> ptrIn, pybind11::array_t<jvxData> ptrOut);
}