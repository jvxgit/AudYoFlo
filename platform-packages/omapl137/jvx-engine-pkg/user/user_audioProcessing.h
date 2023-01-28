/*
 * ==============================================
 * Filename: user_audioProcessing.h
 * ==============================================
 * Purpose: Audio processing API (very small RTProc in C).
 *			COPYRIGHT HK, Javox, 2011
 */
 
#ifndef _USER_AUDIOPROCESSING_H__
#define _USER_AUDIOPROCESSING_H__

// Function to be called at begin of audio processing. The return parameter holds the memory to be used in processing on return
void user_prepareProcessing(void** handle);

// Process one buffer. Note that buffersize, channels and samplerate are available as preprocessor macros
void user_process(void* handle, float** inBufs, float** outBufs);

#endif
