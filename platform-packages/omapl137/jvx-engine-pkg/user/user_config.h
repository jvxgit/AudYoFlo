/*
 * ==============================================
 * Filename: user_config.h
 * ==============================================
 * Purpose: Setup the audio processing parameters in this file.
 *			Based on the setup, the audio processing parameters will be computed in
 *			configAudioBoardSW.h. In the end, the used uses the preprocessor defines for 		
 *
 *			- NUM_INPUT_CHANNELS_AUDIO
 *			- NUM_OUTPUT_CHANNELS_AUDIO
 *			- AUDIO_BUFFERSIZE
 *			- SAMPLERATE 
 * 			COPYRIGHT HK, Javox, 2011
 */


/***************************************************
* Definition of channelsets: Looking from behind, that is
* the board is standing on its feed, you see SPDIF and the TOS link ports a the 
* left side:
*
*             ***     **     **     **     **        **    **    **    **
*  spdif/tos->***    *  *   *  *   *  *   *  *      *  *  *  *  *  *  *  *
*             ***     **     **     **     **        **    **    **    **
*              ***********************************************************
*                     CS0   CS1     CS2    CS3      CS0    CS1   CS2   CS3
*                      ----------------------        --------------------
*                             input                         output
***************************************************
*///=============================================================

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

// Start audio at all
#define USE_AUDIO

// Audio buffersize
#define AUDIO_BUFFERSIZE 160 /*128*/

// Active input channels (1-4 is the maximum for 4 x 2 channels)
#define INPUT_CHANNELSET_1_1 // Two input channels

// Active output channels (1-4 is the maximum for 4 x 2 channels)
#define OUTPUT_CHANNELSET_1_1 // Two output channels 

// Setup samplerate to 32 or 48 kHz
// Hint: The DAC has support for a 32 kHz de-emphasis filter.
// This, however, seems to not have any impact even though it
// seems to be active. Therefore, the recommendation is to
// use 48 kHz samplerate
#define SAMPLERATE_32000 
//#define SAMPLERATE_48000

#endif

