/*
 *****************************************************
 * Filename: rtFileReader.h
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description: This class uses the fileIOLib to read data
 *				from WAV (and RAW) files. The special thing about this
 *				module is that there is a buffer used internally
 *				which may be very big, and the file content is written
 *				to this buffer at first. If inputdata is taken from the
 *          	buffer to the application, new data is loaded in the
 *				background. Therefore, deadlocks during the access to the
 *				file that might interfere with RT processing should be avoided
 *				if the buffer is only large enough.                                     *
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: rtproc@javox-solutions.com               *
 *****************************************************
*/

#ifndef __JVXFILEREADER_H__
#define __JVXFILEREADER_H__

#include <string>

#include "jvx.h"
#include "interfaces/IjvxAudioReader.h"

/**
 * Class to realize the buffered file reading. Multiple instances should also be ok
 *///=============================================================================
class jvxFileReader
{

private:

	struct
	{
		jvxByte** intBuffer;
		jvxByte** readBuffer;
		jvxDataFormat formatAppl;
		jvxSize szElement;
		jvxSize numChannels;
		jvxSize framesizeChunks;
		jvxSize idxRead;
		jvxSize fHeight;
		
		jvxSize length;
		jvxSize positionRead;
	} buffer_realtime;

	struct
	{
		JVX_THREAD_HANDLE hdl;
		JVX_NOTIFY_HANDLE readMoreSamples;
		JVX_NOTIFY_HANDLE started;
		jvxBool running;

	} thread;

	struct
	{
		jvxFileOperationType mode;
		jvxFileContinueType contType;
		jvxBool fileEnded;
		jvxBool restartTriggered;
		jvxBool underrun_occurred;
		jvxData progress;
		jvxSize length_file_sample;
		jvxBool boostPriority;
	} operation;

	struct
	{
		JVX_MUTEX_HANDLE accessFileIo;
		JVX_MUTEX_HANDLE accessCircBuffer;
	} protect;

	struct 
	{
		jvxSize num_samples_in;
		jvxSize num_samples_out;
		jvxSize num_samples_lost;
		jvxSize min_fheight_on_enter;
	} eval;

	IjvxAudioReader* hdlReader;

public:

	//! Constructor
	jvxFileReader();

	//! Destructor
	virtual ~jvxFileReader();

	void reset();

	//! Open the input file
	jvxErrorType activate(std::string fName, jvxEndpointClass descrEndpoint, jvxFileDescriptionEndpoint_open* fileDescr);

	//! Close the input file
	jvxErrorType deactivate();

	//! API: Function to prepare file for incoming data
	jvxErrorType prepare(jvxSize internalBuffersize, jvxFileContinueType contTp, jvxBool boostPriority,
		jvxFileDescriptionEndpoint_prepare* fileDescr, jvxFileOperationType mode);

	jvxErrorType restartPlayback(jvxInt32 timeout);

	jvxErrorType wind_forward(jvxSize deltat);

	jvxErrorType wind_backward(jvxSize deltat);

	//! API: Post process file stream
	jvxErrorType postprocess();

	//! Get the type of strategy at end of input file
	jvxErrorType playback_type(jvxFileContinueType* tp);

	//! Get the type of strategy at end of input file
	jvxErrorType set_playback_type(jvxFileContinueType tp);

	//! API: Add a buffer to the internal buffer. Write to file may be delayed.
	jvxErrorType audio_buffer(jvxHandle** buffers, jvxSize numberChannels, jvxSize numberSamples, jvxBool strictlyNoBlocking, jvxData* progress = NULL);

	jvxErrorType progress(jvxSize* progress_samples);

	jvxErrorType initialize();
	jvxErrorType terminate();
	jvxErrorType start();
	jvxErrorType stop();
	jvxErrorType get_file_properties(jvxSize* channels, jvxSize* length_samples, jvxInt32* srate, jvxFileFormat* fformat, jvxBool* littleEndian, jvxEndpointClass* descr, jvxSize* numBitsSample, jvxSize* subtype);
	jvxErrorType get_processing_properties(jvxSize* int_buffersize, jvxSize* int_bsize_move_file_max, jvxDataFormat* format_appl, jvxFileContinueType* contTp);
	jvxErrorType evaluate(jvxSize* num_samples_in, jvxSize* num_samples_out, jvxSize* num_samples_failed, jvxSize* min_fheight);
	jvxErrorType get_tag(jvxAudioFileTagType tp, std::string& name);


	//! Internal use, looped function to write buffered data to file
	JVX_THREAD_RETURN_TYPE enterClassCallback();

	jvxErrorType write_to_circ_buffer(jvxSize& samplesWrite, jvxSize idxWrite);

	jvxErrorType reset_bgrd_buffer_lock_start();
	jvxErrorType reset_bgrd_buffer_lock_stop();

};

#endif
