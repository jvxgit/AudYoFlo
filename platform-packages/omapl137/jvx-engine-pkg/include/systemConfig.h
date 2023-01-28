/*
 * ==============================================
 * Filename: systemConfig.h
 * ==============================================
 * Purpose: This file contaons some system specific definitions
 *			as well as some definitions derived from user_config.h.
 *			In general, this header file should not be modified. 
 * 			COPYRIGHT HK, Javox, 2011
 */

#ifndef __CONFIGAUDIOBOARDSW_H__
#define __CONFIGAUDIOBOARDSW_H__

// Configuration of test software for Spectrum Digital OMAP-L137 test
// code.
// ===================================================================
#include "user_config.h"


// Number of zeros to place in write FIFO before starting processing (THIS IS ABSOLTELY NECESSARY!!)
#define NUMBER_ZEROS_FIFO 256/4/4

// McAsp definitions
#define NUM_SERIALIZERS 4
#define NUM_BYTES_SAMPLE 4
#define NUM_CHANNELS_PER_SERIALIZER 2

// DMA definitions
#define X_DMA_NUM_S0 3 /*22*/
#define R_DMA_NUM_S0 2 /*22*/
#define X_DMA_NUM_S1 70
#define X_DMA_NUM_S2 (X_DMA_NUM_S1+1)
#define R_DMA_NUM_S1 72
#define R_DMA_NUM_S2 (R_DMA_NUM_S1+1)

// UART DMA configuration
#define X_DMA_UART 31 

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// Derive number of input channels 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
#if (defined(INPUT_CHANNELSET_1_4))
#define NUM_INPUT_CHANNELS_AUDIO 8
#elif (defined(INPUT_CHANNELSET_1_3))
#define NUM_INPUT_CHANNELS_AUDIO 6
#elif (defined(INPUT_CHANNELSET_1_2))
#define NUM_INPUT_CHANNELS_AUDIO 4
#else 
#define NUM_INPUT_CHANNELS_AUDIO 2
#endif

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// Derive number of output channels
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
#if (defined(OUTPUT_CHANNELSET_1_4))
#define NUM_OUTPUT_CHANNELS_AUDIO 8
#elif (defined(OUTPUT_CHANNELSET_1_3))
#define NUM_OUTPUT_CHANNELS_AUDIO 6
#elif (defined(OUTPUT_CHANNELSET_1_2))
#define NUM_OUTPUT_CHANNELS_AUDIO 4
#else 
#define NUM_OUTPUT_CHANNELS_AUDIO 2
#endif

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// Derive samplerate macro
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
#ifdef SAMPLERATE_32000
#define SAMPLERATE 32000
#endif
#ifdef SAMPLERATE_48000
#define SAMPLERATE 48000
#endif

#define INIT_FREQUENCY 1000

// UART configuration: 115200 bps + 8 bit, one stop, no parity
#define BAUDRATE 115200

#endif

