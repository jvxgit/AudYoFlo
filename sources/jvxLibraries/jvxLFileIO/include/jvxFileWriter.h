
#ifndef __RTFILEWRITER_H__
#define __RTFILEWRITER_H__

#include <string>

#include "jvx.h"
#include "interfaces/IjvxAudioWriter.h"

/**
 * Class to encapsulate writing of audio data to files without
 * interfering with real-time processing. A large internal buffer is used
 * and all data is at first stored to this buffer. The stored buffer is then written
 * to harddrive in the background.
 *///=======================================================
class jvxFileWriter: public IjvxThreads_report
{
	private:

	struct
	{
		jvxByte** intBuffer;
		jvxByte** writeBuffer;
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
		JVX_NOTIFY_HANDLE writeMoreSamples;
		JVX_NOTIFY_HANDLE started;
		jvxBool running;

	} thread;

	struct
	{
		jvxFileOperationType mode;
		jvxBool fileEnded;
		jvxBool overrun_occurred;
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
		jvxSize min_space_on_enter;
	} eval;

	IjvxAudioWriter* hdlWriter = nullptr;
	IjvxThreads* theThread = nullptr;

public:

	//! Constructor
	jvxFileWriter();

	//! Destructor
	virtual ~jvxFileWriter();

	void reset();

	jvxErrorType activate(std::string fName, jvxEndpointClass endpoint, 
		jvxFileDescriptionEndpoint_open* fileDescr, IjvxThreads* threadHd = nullptr);

	jvxErrorType deactivate();

	//! API: Function to prepare recording
	jvxErrorType prepare(jvxSize internalBuffersize, jvxBool boostPriority,
		jvxFileDescriptionEndpoint_prepare* fileDescr,
		jvxFileOperationType mode);

	//! API: Add a buffer to the internal buffer. Write to file may be delayed.
	jvxErrorType add_buffer(jvxHandle** buffers, jvxSize numberChannels, jvxSize numberSamples, jvxBool strictlyNoBlocking, jvxData* bFillHeight_scale);

	//! API: Post process file stream
	jvxErrorType postprocess();

	jvxErrorType initialize();
	jvxErrorType terminate();
	jvxErrorType start();
	jvxErrorType stop();
	jvxErrorType get_file_properties(jvxSize* channels, jvxInt32* srate, jvxFileFormat* fformat, jvxEndpointClass* descr, jvxSize* numBitsSample, jvxSize* subtype);
	jvxErrorType get_processing_properties(jvxSize* int_buffersize, jvxSize* int_bsize_move_file_max, jvxDataFormat* dformat_appl);
	jvxErrorType set_file_properties(jvxSize* channels, jvxInt32* srate,  jvxFileFormat* fformat, jvxEndpointClass* descr, jvxSize* numBitsSample, jvxSize* subtype);
	jvxErrorType set_processing_properties(jvxSize* int_buffersize, jvxSize* int_bsize_move_file_max, jvxDataFormat* dformat_appl);
	jvxErrorType evaluate(jvxSize* num_samples_in, jvxSize* num_samples_out, jvxSize* num_samples_failed, jvxSize* min_fheight);
	jvxErrorType add_tag(jvxAudioFileTagType tp, std::string name);

	jvxErrorType read_from_circ_buffer(jvxSize& samplesWrite, jvxSize idxWrite);

//! Internal use, looped function to write buffered data to file
	JVX_THREAD_RETURN_TYPE enterClassCallback();

	jvxBool core_write_function();

	jvxErrorType startup(jvxInt64 timestamp_us) override;
	jvxErrorType expired(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	jvxErrorType wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	jvxErrorType stopped(jvxInt64 timestamp_us) override;

};

#endif
