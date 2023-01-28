#include "SimpleKernel.h"

#include <iostream>
// -> JVX_WP_MODULE_PYTHON or JVX_WP_MODULE_EMSCRIPTEN

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

SimpleKernel::SimpleKernel()
{

}

// Destructor - call terminate
SimpleKernel::~SimpleKernel()
{

	// Deallocate memory here
	terminate();
}

// Call this function to pass processing parameters
void
SimpleKernel::initialize(int bsize, int samplerate, int chans, int chansMax)
{
	int i;
	procChansMax = chansMax;
	procBSize = bsize;
	srate = samplerate;

	inBufs = new jvxData * [procChansMax];
	std::cout << "Allocating " << procChansMax << " channels at address " << inBufs << std::endl;

	outBufs = new jvxData * [procChansMax];
	std::cout << "Allocating " << procChansMax << " channels at address " << outBufs << std::endl;

	for (i = 0; i < procChansMax; i++)
	{
		inBufs[i] = nullptr;
		outBufs[i] = nullptr;
	}

	avrgLevel = new jvxData[procChansMax]; 
	maxLevel = new jvxData[procChansMax];
	maxLevelLast = new jvxData[procChansMax];
	gainLevel = new jvxData[procChansMax];
	clipPtr = new jvxCBool[procChansMax]; 
	chanMap = new jvxSize[procChansMax];
	cntSamplesSinceMax = new jvxSize[procChansMax];
	for (i = 0; i < procChansMax; i++)
	{
		avrgLevel[i] = 0;
		maxLevel[i] = 0;
		maxLevelLast[i] = 0;
		gainLevel[i] = 0;
		clipPtr[i] = c_false;
		chanMap[i] = i;
		cntSamplesSinceMax[i] = 0;
	}

	std::cout << "Init webasm code, BSize = " << bsize << ", Chans = " << chans << ", ChansMax = " << procChansMax << std::endl;
}

// Terminate function
void
SimpleKernel::terminate()
{

	if (avrgLevel)
	{
		delete[](avrgLevel);
		avrgLevel = nullptr;
	}

	if (maxLevel)
	{
		delete[](maxLevel);
		maxLevel = nullptr;
	}

	if (maxLevelLast)
	{
		delete[](maxLevelLast);
		maxLevelLast = nullptr;
	}

	if (gainLevel)
	{
		delete[](gainLevel);
		gainLevel = nullptr;
	}

	if (clipPtr)
	{
		delete[](clipPtr);
		clipPtr = nullptr;
	}

	if (cntSamplesSinceMax)
	{
		delete[](cntSamplesSinceMax);
		cntSamplesSinceMax = nullptr;
	}
	
	if (chanMap)
	{
		delete[](chanMap);
		chanMap = nullptr;
	}

	if (inBufs)
	{
		delete[](inBufs);
		inBufs = nullptr;
	}
	if (outBufs)
	{
		delete[](outBufs);
		outBufs = nullptr;
	}
}

// Process callback
void
SimpleKernel::Process(uintptr_t input_ptr, uintptr_t output_ptr,
	unsigned channel_count) {

	// It seems the channel number may change during processing - need to take into consideration
	int chansMax = MIN(procChansMax, channel_count);
	unsigned channel = 0;

	// All audio samples in one buffer non-interleaved
	// Make buffer field from that 
	jvxData* source = reinterpret_cast<jvxData*>(input_ptr);
	jvxData* destination = reinterpret_cast<jvxData*>(output_ptr);

	// By design, the channel count will always be the same
	// We prepare a buffer of fields
	for (channel = 0; channel < chansMax; ++channel) 
	{
		inBufs[channel] = source;
		outBufs[channel] = destination;
		source += procBSize;
		destination += procBSize;
	}

	// std::cout << "Process this" << std::endl;

	// Here we run the actual processing core	
	for (channel = 0; channel < chansMax; ++channel)
	{
		jvxData* inBuf = inBufs[channel];
		jvxData* outBuf = outBufs[channel];
		for (int i = 0; i < procBSize; i++)
		{
			outBuf[i] = inBuf[i];
		}
		gainLevel[channel] = gain;
		maxLevelLast[channel] = maxLevel[channel];
	}

	
	jvx_fieldLevelGainClip(reinterpret_cast<jvxHandle**>(outBufs), chansMax, procBSize, JVX_DATAFORMAT_DATA, avrgLevel, maxLevel, gainLevel, clipPtr, chanMap, 0.9, 0.95);
	
	for (channel = 0; channel < chansMax; ++channel)
	{
		if (maxLevel[channel] > maxLevelLast[channel])
		{
			cntSamplesSinceMax[channel] = 0;
		}
		else
		{
			cntSamplesSinceMax[channel] += procBSize;
		}
		if (cntSamplesSinceMax[channel] >= srate)
		{
			maxLevel[channel] *= 0.5;
			clipPtr[channel] = c_false;
		}
	}
	//abort();
}

// Parameter set/get functions
void
SimpleKernel::setGain(jvxData val)
{
	gain = val;
}

jvxData
SimpleKernel::getGain()
{
	return gain;
}

void
SimpleKernel::verifySize(int len)
{
	assert(len == procBSize);
}

void
SimpleKernel::snapshot()
{
	jvxData avrgVL = 0.0;
	jvxData maxVL = 0.0;
	jvxCBool clipL = c_false;
	unsigned channel = 0;
	for (channel = 0; channel < procChansMax; channel++)
	{
		avrgVL = JVX_MAX(avrgVL, avrgLevel[channel]);
		maxVL = JVX_MAX(maxVL, maxLevel[channel]);
		if (clipPtr[channel])
		{
			clipL = c_true;
		}
	}

	avrgV = avrgVL;
	maxV = maxVL;
	clipF = clipL;
}

jvxData 
SimpleKernel::getAvrg()
{
	return avrgV;
}

jvxData 
SimpleKernel::getMax()
{
	return maxV;
}

jvxCBool 
SimpleKernel::getClip()
{
	return clipF;
}

