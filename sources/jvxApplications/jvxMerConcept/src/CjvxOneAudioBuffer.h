#ifndef __CJVXONEAUDIOBUFFER_H__
#define __CJVXONEAUDIOBUFFER_H__

#include "jvx.h"
#include "jvx_threads.h"

class CjvxOneAudioBuffer
{
	friend class CjvxRtAudioBuffers;
public:
	typedef enum
	{
		JVX_AUDIOBUFFER_RECORDED,
		JVX_AUDIOBUFFER_FILE
	} jvxAudioBuffertype;

private:

	typedef struct
	{
		jvxData** buffers;
		jvxSize fHeight;
		jvxSize idx;
	} oneAudioSegment;

	struct
	{
		std::vector<oneAudioSegment*> theSegments;
		jvxSize lengthSegment;
		jvxInt32 sRate;
		jvxInt32 numChannels;
		JVX_MUTEX_HANDLE safeAccessSegments;
		jvxState theState;
		std::string tag;
		jvxAudioBuffertype type;
		std::string fName;
	} runtime;
		
	struct
	{
		oneAudioSegment* theCurrentSegment;
		oneAudioSegment* theNextSegment;
		jvxSize idxSegment;
		jvxSize idxInSegment;
		jvxBool rtMode;
		struct
		{
			jvxHandle* hdl;
			jvx_thread_handler funcs;
		} thread;
		jvxBool* loop;
		jvxSize buf_cnt;
	} inOperation_readwrite;
	
public:

	CjvxOneAudioBuffer();

	~CjvxOneAudioBuffer();

	void lock()
	{
		JVX_LOCK_MUTEX(runtime.safeAccessSegments);
	};

	void unlock()
	{
		JVX_UNLOCK_MUTEX(runtime.safeAccessSegments);
	};

	jvxErrorType configure(jvxSize numChans, jvxInt32 sRate, jvxData segment_secs, const char* tag);
	jvxErrorType configure(jvxSize numChans, jvxInt32 sRate, jvxData segment_secs, const char* tag, const char* fname);

	jvxErrorType prepare_write(jvxBool rtMode);
	void prepare_write_next_noLock();
	jvxErrorType add_samples_ni(jvxData** ptrIn, jvxSize numSamples);
	jvxErrorType add_samples_i(jvxData* ptrIn_interleaved, jvxSize numSamples);
	jvxErrorType postprocess_write();

	jvxErrorType prepare_read(jvxBool* loop);
	jvxErrorType rewind_read();
	jvxErrorType get_samples_ni(jvxData** ptrIn, jvxSize numSamples, jvxBool realtime, jvxSize& num);
	jvxErrorType get_samples_i(jvxData* ptrIn_interleaved, jvxSize numSamples, jvxBool realtime, jvxSize& num);
	jvxErrorType postprocess_read();

	jvxErrorType length_samples(jvxSize& sz);
	static jvxErrorType callback_thread_wokeup_static(jvxHandle* privateData_thread, jvxInt64 timestamp_us);
	jvxErrorType callback_thread_wokeup_inClass(jvxInt64 timestamp_us);

	void fill_view_buffer(jvxData** y, jvxData** x, jvxSize length, jvxInt32 maxNumberSamplesShow);
};

#endif