/*
 * ==============================================
 * Filename: user_audioProcessing.c
 * ==============================================
 * Purpose: User entry point for audio processing.
 * 			Source file to place all algorithm specific 
 *			routines or the main routine which branches to sub modules. 
 * 			All algorithm specific variables should be static since
 *			the use of dynamic memory is very limited (not in HWI and SWI).
 *			The initialization, however, should be done in function prepareProcessing,
 * 			The process function is the main signal processing callback to which all
 *			input samples are passed once being converted to float values. Go and fill
 *			the output buffers based on the desired algorithm.
 * 			All processing parameters are defined in a preprocessor step by setting the
 *			variables
 *
 *			- NUM_INPUT_CHANNELS_AUDIO
 *			- NUM_OUTPUT_CHANNELS_AUDIO
 *			- AUDIO_BUFFERSIZE
 *			- SAMPLERATE 
 *
 *			The incoming audiosamples are collected in buffers of size
 * 			AUDIO_BUFFERSIZE in the background using a DMA transfer.
 * 			Once a buffer is complete, a HWI is triggered which is then mapped to a software interrupt.
 *			In the software interrupt all inpt samples are converted to float and then control is 
 *			passed to the process function.
 *			Once function process completes, the output buffers are converted from float to the 24 bit dataformat of the
 *			AD/DA converters. If this happens too late, real-time constraints are no longer fulfilled and 
 *			artefacts will occur.
 *			
 * 			Currently the process function realizes a talkthrough with multiplication.
 *			Also, the maximum amplitude is checked to detect overflow conditions. 
 * 
 * 			Buffersize and number of channels are specified in the header file user_config.h.
 *			Channels are specified by means of channelsets. The max number of channelsets is 4, 
 *			each defined as one stereo output. By defining a subset of channelsets (1-2), only
 *			a subset of channels will arrive in the process function. The subset selection happens
 *			when converting from INT24 to float and does not affect the DMA transfer.
 * 			COPYRIGHT HK, Javox, 2011
 */

#include "math.h"
#include "systemConfig.h"
#include "dskda830_led.h"
#include "nrMotor_dsp.h"

//#define OUTPUT_SINE
#define SWEEP

// Example for an additional buffer defined to be in the 
// SDRAM
#pragma DATA_SECTION (testBufferSDRAM, ".userDefinedData") 
float testBufferSDRAM[AUDIO_BUFFERSIZE];

// Example for additional code defined to be in the 
// SDRAM
#pragma CODE_SECTION (testCodeSDRAM, ".userDefinedCode") 
int testCodeSDRAM()
{
	return(0);
}

// Define the threshold to identify overflow conditions (very simple)
#define THRESHOLD_OVERLOAD 0.98

// Switch between outputting a sinus or the input sígnal
//#define OUTPUT_SINE

/**
 * Initialization code for algorithm. This function is called from within the 
 * DSP/BIOS initialization routine which is main(). I noticed that some mathematical 
 * functions may not work propperly since the initialization code of the DSP is not yet
 * complete. 
 *///============================================================
void 
user_prepareProcessing(void** handle)
{
	// Todo: Add initialization code here
	if(handle)
	{
		*handle = 0;
	}
	rtp_nrMotor_initialize(40, 2, 1);
}

// Definitions for sinus output signal
#ifdef OUTPUT_SINE	
// Define PI 
#define M_PI 3.141592654

// phase to be updated from sample time to sample time
static float phaseU = 0.0; 
float frequency = INIT_FREQUENCY;
#endif


/**
 * Main processing function: Copy input samples to output samples
 *///============================================================
void
user_process(void* handle, float** inBufs, float** outBufs) // , NUM_INPUT_CHANNELS_AUDIO, NUM_OUTPUT_CHANNELS_AUDIO, AUDIO_BUFFERSIZE
{
	int i,j;
	int idxIn = 0;
	float* bOut = 0;
	float* bIn = 0;
	int setFlag = 0;

#ifdef OUTPUT_SINE	

	// ===============================================================
	// Test code to output a sine function with a 1000 Hz frequency 
	// this was how I detected the 41.7 kHz sampling rate
	// ===============================================================
	float sample = 0.0;
	float* b1 = outBufs[0];
	float* b2 = outBufs[1];

	for(i = 0; i < AUDIO_BUFFERSIZE; i++)
	{
		sample = 0.5 * cos(phaseU);
		b1[i] = sample;
		b2[i] = sample;
		phaseU += 2 * M_PI * frequency/SAMPLERATE;// <- put the samplerate in here
		while(phaseU > 2 * M_PI)
		{
			phaseU -= 2 * M_PI;
		}
	}

#ifdef SWEEP
	frequency ++;

	if(frequency > SAMPLERATE/2)
	{
		frequency = INIT_FREQUENCY;
	}
#endif

#else

	// ===============================================================
	// Signal processing default functionality, copy input to output
	// ===============================================================
	for(i = 0; i < NUM_OUTPUT_CHANNELS_AUDIO; i++)
	{
		idxIn = i % NUM_INPUT_CHANNELS_AUDIO;
		bOut = outBufs[i];
		bIn = inBufs[idxIn];
		for(j = 0; j < AUDIO_BUFFERSIZE; j++)
		{
			*bOut = *bIn++ *10.0;
			if(*bOut > THRESHOLD_OVERLOAD)
			{
				setFlag = 1;
			}
			bOut++;
		}						 
	}

	// Show output overload by LED
	if(setFlag)
	{
		DSKDA830_LED_on( 3 );
	}
	else
	{
		DSKDA830_LED_off( 3 );
	}
#endif
}

// =============================================================================================
