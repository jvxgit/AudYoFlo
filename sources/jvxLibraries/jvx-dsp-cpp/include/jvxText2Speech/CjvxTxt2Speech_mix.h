#ifndef __CJVXTXT2SPEECH_MIX_H__
#define __CJVXTXT2SPEECH_MIX_H__

#include "jvx.h"

class CjvxTxt2Speech_mix: public IjvxText2Speech_report
{
	typedef struct
	{
		jvxData* fld;
		jvxSize fld_length;
		jvxBool playout_done;
		jvxSize idxSamples;
		std::string txtOut;
		jvxSize selector;
	} oneChannelStruct;
private:
	std::vector<oneChannelStruct> theChannels;
	jvxState theState;
	jvxSize channelIdx;
	JVX_MUTEX_HANDLE safeAccess;
	
	IjvxText2Speech* coreHdl;
	jvxText2SpeechFormat formT2s;
	jvxSize bsize;
	
public:
	CjvxTxt2Speech_mix();
	~CjvxTxt2Speech_mix();
	
	// ============================================
	jvxErrorType initialize();
	
	// ============================================
	jvxErrorType prepare(IjvxText2Speech* coreHdl, std::string txtPrefix, jvxSize srate, jvxSize buffersize, jvxSize num_channels, jvxSize out_chan_offset);
	
	jvxErrorType trigger_start();

	jvxErrorType trigger_stop();

	// ============================================
	jvxErrorType process(jvxData** bufs, jvxData gain, jvxBool add_text = true, jvxSize* chan_idx = NULL);
	
	// ============================================
	jvxErrorType postprocess();
	
	// ============================================
	jvxErrorType terminate();

	virtual jvxErrorType JVX_CALLINGCONVENTION task_complete(jvxByte* fld, jvxSize szOutput,
		jvxText2SpeechFormat out_format, jvxSize uniqueId);

	virtual jvxErrorType JVX_CALLINGCONVENTION generation_failed(jvxSize uniqueId);
};

#endif