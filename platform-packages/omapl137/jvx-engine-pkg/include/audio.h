/*
 * ==============================================
 * Filename: audio.h
 * ==============================================
 * Purpose: API specification for audio module 
 * 			COPYRIGHT HK, Javox, 2011
 */

#ifndef __AUDIO_H__
#define __AUDIO_H__

// Prepare McAsp and so on.
Int16 audio_prepareAudio();

// Preset the buffers used for DMA transfer
Int16 audio_initBuffers();

#endif

