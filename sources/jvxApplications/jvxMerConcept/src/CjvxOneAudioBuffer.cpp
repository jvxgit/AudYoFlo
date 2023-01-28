#include "CjvxOneAudioBuffer.h"
#include "jvx_resampler/jvx_resampler_plotter.h"

CjvxOneAudioBuffer::CjvxOneAudioBuffer()
{
	runtime.theState = JVX_STATE_INIT;
	runtime.lengthSegment = 0;
	runtime.numChannels = 0;
	runtime.sRate = 0;
	
	inOperation_readwrite.theCurrentSegment = NULL;
	inOperation_readwrite.theNextSegment = NULL;
	inOperation_readwrite.idxSegment = 0;
	inOperation_readwrite.idxInSegment = 0;

	inOperation_readwrite.thread.hdl = NULL;
	inOperation_readwrite.thread.funcs.callback_thread_startup = NULL;
	inOperation_readwrite.thread.funcs.callback_thread_stopped = NULL;
	inOperation_readwrite.thread.funcs.callback_thread_timer_expired = NULL;
	inOperation_readwrite.thread.funcs.callback_thread_wokeup = callback_thread_wokeup_static;

	this->inOperation_readwrite.loop = NULL;
	JVX_INITIALIZE_MUTEX(runtime.safeAccessSegments);
}

CjvxOneAudioBuffer::~CjvxOneAudioBuffer()
{
	jvxSize i,j;
	for(i = 0; i < runtime.theSegments.size(); i ++)
	{
		if(runtime.theSegments[i])
		{
			for(j = 0; j < runtime.numChannels; j++)
			{
				delete[](runtime.theSegments[i]->buffers[j]);
			}
			delete[](runtime.theSegments[i]->buffers);
			delete(runtime.theSegments[i]);
		}
	}
	JVX_TERMINATE_MUTEX(runtime.safeAccessSegments);
}

jvxErrorType 
CjvxOneAudioBuffer::configure(jvxSize numChans, jvxInt32 sRate, jvxData segment_secs, const char* tag)
{
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	if(runtime.theState == JVX_STATE_INIT)
	{
		runtime.sRate = sRate;
		runtime.numChannels = numChans;
		runtime.lengthSegment = JVX_DATA2INT32(segment_secs * sRate);
		runtime.tag = tag;
		runtime.type = JVX_AUDIOBUFFER_RECORDED;
		runtime.theState = JVX_STATE_ACTIVE;
	}
	this->unlock();
	return(res);
}

jvxErrorType 
CjvxOneAudioBuffer::configure(jvxSize numChans, jvxInt32 sRate, jvxData segment_secs, const char* tag, const char* fName)
{
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	if(runtime.theState == JVX_STATE_INIT)
	{	
		runtime.sRate = sRate;
		runtime.numChannels = numChans;
		runtime.lengthSegment = JVX_DATA2INT32(segment_secs * sRate);
		runtime.tag = tag;
		runtime.type = JVX_AUDIOBUFFER_FILE;
		runtime.fName = fName;
		runtime.theState = JVX_STATE_ACTIVE;
	}
	this->unlock();
	return(res);
}

jvxErrorType 
CjvxOneAudioBuffer::length_samples(jvxSize& sz)
{
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	sz = 0;
	if(runtime.theState >= JVX_STATE_ACTIVE)
	{	
		if(runtime.theSegments.size() > 0)
		{
			sz = (runtime.theSegments.size()-1) * (runtime.lengthSegment);
			sz += runtime.theSegments[(runtime.theSegments.size()-1)]->fHeight;
		}
	}
	this->unlock();
	return(res);
}

// ===================================================================================

jvxErrorType 
CjvxOneAudioBuffer::prepare_write(jvxBool rtMode)
{
	jvxSize i;	
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	if(runtime.theState == JVX_STATE_ACTIVE)
	{	
		inOperation_readwrite.buf_cnt = 0;
		inOperation_readwrite.rtMode = rtMode;
		prepare_write_next_noLock();
		inOperation_readwrite.theCurrentSegment = inOperation_readwrite.theNextSegment;
		prepare_write_next_noLock();
		if(inOperation_readwrite.rtMode)
		{
			// Start background thread to produce new buffers
			jvx_thread_initialize(&inOperation_readwrite.thread.hdl, &inOperation_readwrite.thread.funcs, reinterpret_cast<jvxHandle*>(this), true);
			jvx_thread_start(inOperation_readwrite.thread.hdl, JVX_INFINITE);

		}
		runtime.theState = JVX_STATE_PROCESSING;
	}
	this->unlock();
	return(res);
}

jvxErrorType 
CjvxOneAudioBuffer::postprocess_write()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	if(runtime.theState == JVX_STATE_PROCESSING)
	{	
		this->unlock();

		// We need to exclude this to avoid blocking forever
		jvx_thread_stop(inOperation_readwrite.thread.hdl);
		jvx_thread_terminate(inOperation_readwrite.thread.hdl);
		this->lock();

		if(inOperation_readwrite.theCurrentSegment->fHeight > 0)
		{
			runtime.theSegments.push_back(inOperation_readwrite.theCurrentSegment);
		}
		else
		{
			if(inOperation_readwrite.theCurrentSegment)
			{
				for(i = 0; i < runtime.numChannels; i++)
				{
					delete[](inOperation_readwrite.theCurrentSegment->buffers[i]);
				}
				delete[](inOperation_readwrite.theCurrentSegment->buffers);
				delete(inOperation_readwrite.theCurrentSegment);
			}
		}
		if(inOperation_readwrite.theNextSegment)
		{
			for(i = 0; i < runtime.numChannels; i++)
			{
				delete[](inOperation_readwrite.theNextSegment->buffers[i]);
			}
			delete[](inOperation_readwrite.theNextSegment->buffers);
			delete(inOperation_readwrite.theNextSegment);
		}
		runtime.theState = JVX_STATE_ACTIVE;
	}
	this->unlock();
	return(res);
}

void 
CjvxOneAudioBuffer::prepare_write_next_noLock()
{		
	jvxSize i;
	inOperation_readwrite.theNextSegment = new oneAudioSegment;
	inOperation_readwrite.theNextSegment->buffers = new jvxData*[runtime.numChannels];
	inOperation_readwrite.theNextSegment->fHeight = 0;
	inOperation_readwrite.theNextSegment->idx = inOperation_readwrite.buf_cnt++;

	for(i = 0; i < runtime.numChannels; i++)
	{
		inOperation_readwrite.theNextSegment->buffers[i] = new jvxData[runtime.lengthSegment];
		memset(inOperation_readwrite.theNextSegment->buffers[i], 0, sizeof(jvxData)*runtime.lengthSegment);
	}
}

jvxErrorType 
CjvxOneAudioBuffer::add_samples_ni(jvxData** ptrIn_noninterleaved, jvxSize numSamples)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	if(runtime.theState == JVX_STATE_PROCESSING)
	{	
		if(inOperation_readwrite.theCurrentSegment)
		{
			while(numSamples)
			{
				jvxSize ll = JVX_MIN(runtime.lengthSegment - inOperation_readwrite.theCurrentSegment->fHeight, numSamples);
				if(ll > 0)
				{
					for(i = 0; i < runtime.numChannels; i++)
					{
						memcpy(inOperation_readwrite.theCurrentSegment->buffers[i] + inOperation_readwrite.theCurrentSegment->fHeight, ptrIn_noninterleaved[i], sizeof(jvxData) * ll);
					}
					numSamples -= ll;
					inOperation_readwrite.theCurrentSegment->fHeight += ll;
				}
				else
				{
					if(inOperation_readwrite.rtMode)
					{
						jvxBool isReady = false;
						oneAudioSegment* tmp = inOperation_readwrite.theCurrentSegment;
						inOperation_readwrite.theCurrentSegment = inOperation_readwrite.theNextSegment;
						inOperation_readwrite.theNextSegment = tmp;
						jvx_thread_test_complete(inOperation_readwrite.thread.hdl, &isReady);
						assert(isReady);
						jvx_thread_wakeup(inOperation_readwrite.thread.hdl);
					}
					else
					{
						runtime.theSegments.push_back(inOperation_readwrite.theCurrentSegment);
						inOperation_readwrite.theCurrentSegment = inOperation_readwrite.theNextSegment;
						inOperation_readwrite.theNextSegment = NULL;
						prepare_write_next_noLock();
					}
				}
			}
		}
		else
		{
			assert(0);
		}
	}
	this->unlock();
	return(res);
}

jvxErrorType 
CjvxOneAudioBuffer::add_samples_i(jvxData* ptrIn_interleaved, jvxSize numFrames)
{
	jvxSize i, ii;
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	if(runtime.theState == JVX_STATE_PROCESSING)
	{	
		if(inOperation_readwrite.theCurrentSegment)
		{
			while(numFrames)
			{
				jvxSize ll = JVX_MIN(runtime.lengthSegment - inOperation_readwrite.theCurrentSegment->fHeight, numFrames);
				if(ll > 0)
				{
					for(ii = 0; ii < ll; ii++)
					{
						for(i = 0; i < runtime.numChannels; i++)
						{
							inOperation_readwrite.theCurrentSegment->buffers[i][ii + inOperation_readwrite.theCurrentSegment->fHeight] = *ptrIn_interleaved;
							ptrIn_interleaved++;
						}
					}
					numFrames -= ll;
					inOperation_readwrite.theCurrentSegment->fHeight += ll;
				}
				else
				{
					if(inOperation_readwrite.rtMode)
					{
						// Trigger 
						// Switch buffers
						jvxBool isReady = false;
						oneAudioSegment* tmp = inOperation_readwrite.theCurrentSegment;
						inOperation_readwrite.theCurrentSegment = inOperation_readwrite.theNextSegment;
						inOperation_readwrite.theNextSegment = tmp;
						jvx_thread_test_complete(inOperation_readwrite.thread.hdl, &isReady);
						assert(isReady);
						jvx_thread_wakeup(inOperation_readwrite.thread.hdl);
					}
					else
					{
						runtime.theSegments.push_back(inOperation_readwrite.theCurrentSegment);
						inOperation_readwrite.theCurrentSegment = inOperation_readwrite.theNextSegment;
						inOperation_readwrite.theNextSegment = NULL;
						prepare_write_next_noLock();
					}
				}
			}
		}
		else
		{
			assert(0);
		}
	}
	this->unlock();
	return(res);
}

// =====================================================================

jvxErrorType 
CjvxOneAudioBuffer::rewind_read()
{
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	if(runtime.theState == JVX_STATE_PROCESSING)
	{	
		inOperation_readwrite.idxSegment = 0;
		inOperation_readwrite.idxInSegment = 0;
	}
	this->unlock();
	return(res);
}

jvxErrorType 
CjvxOneAudioBuffer::prepare_read(jvxBool* loop)
{
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	if(runtime.theState == JVX_STATE_ACTIVE)
	{	
		this->inOperation_readwrite.loop = loop;
		inOperation_readwrite.buf_cnt = 0;
		runtime.theState = JVX_STATE_PROCESSING;
		this->rewind_read();

		inOperation_readwrite.theCurrentSegment = NULL;
		if(inOperation_readwrite.idxSegment < this->runtime.theSegments.size())
		{
			inOperation_readwrite.theCurrentSegment = this->runtime.theSegments[inOperation_readwrite.idxSegment];
			//inOperation_readwrite.idxSegment++;
			inOperation_readwrite.idxInSegment = 0;
		}
	}
	this->unlock();
	return(res);
}


jvxErrorType 
CjvxOneAudioBuffer::get_samples_ni(jvxData** ptrIn, jvxSize numSamples, jvxBool realtime, jvxSize& numReturned)
{
	jvxSize i;
	jvxInt32 offset = 0;
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	numReturned = numSamples;
	if(runtime.theState == JVX_STATE_PROCESSING)
	{	
		if(inOperation_readwrite.theCurrentSegment)
		{
			while(numSamples)
			{
				jvxSize ll = JVX_MIN(numSamples, inOperation_readwrite.theCurrentSegment->fHeight - inOperation_readwrite.idxInSegment);
				for(i = 0; i < this->runtime.numChannels; i++)
				{
					memcpy(ptrIn[i] + offset,  inOperation_readwrite.theCurrentSegment->buffers[i] + inOperation_readwrite.idxInSegment, ll * sizeof(jvxData));
				}
				numSamples -= ll;
				inOperation_readwrite.idxInSegment += ll;
				offset += ll;

				if(inOperation_readwrite.idxInSegment == inOperation_readwrite.theCurrentSegment->fHeight)
				{
					inOperation_readwrite.idxSegment++;
					inOperation_readwrite.idxInSegment = 0;

					inOperation_readwrite.theCurrentSegment = NULL;
					if(inOperation_readwrite.idxSegment < this->runtime.theSegments.size())
					{
						inOperation_readwrite.theCurrentSegment = this->runtime.theSegments[inOperation_readwrite.idxSegment];
					}
					else
					{
						if(this->inOperation_readwrite.loop) // Loop may be a NULL pointer if loop is not allowed!
						{
							if(*this->inOperation_readwrite.loop)
							{
								inOperation_readwrite.idxSegment = 0;
								inOperation_readwrite.idxInSegment = 0;
								if(inOperation_readwrite.idxSegment < this->runtime.theSegments.size())
								{
									inOperation_readwrite.theCurrentSegment = this->runtime.theSegments[inOperation_readwrite.idxSegment];
								}
							}
						}
					}
				}

				if(inOperation_readwrite.theCurrentSegment == NULL)
				{
					res = JVX_ERROR_END_OF_FILE;
					for(i = 0; i < this->runtime.numChannels; i++)
					{
						memset(ptrIn[i] + offset,  0, numSamples * sizeof(jvxData));
					}
					numSamples = 0;
				}
			}
		}
		else
		{
			res = JVX_ERROR_END_OF_FILE;
			numReturned = offset;
		}
	}
	this->unlock();
	return(res);
}

jvxErrorType
CjvxOneAudioBuffer::get_samples_i(jvxData* ptrIn_interleaved, jvxSize numSamples, jvxBool realtime, jvxSize& numReturned)
{
	jvxSize i,j;
	jvxInt32 offset = 0;
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	numReturned = numSamples;
	if(runtime.theState == JVX_STATE_PROCESSING)
	{	
		if(inOperation_readwrite.theCurrentSegment)
		{
			while(numSamples)
			{
				jvxSize ll = JVX_MIN(numSamples, inOperation_readwrite.theCurrentSegment->fHeight - inOperation_readwrite.idxInSegment);
				for(i = 0; i < ll; i++)
				{
					for(j = 0; j < this->runtime.numChannels; j++)
					{
						*ptrIn_interleaved++ = inOperation_readwrite.theCurrentSegment->buffers[j][i + inOperation_readwrite.idxInSegment];
					}
				}
				numSamples -= ll;
				inOperation_readwrite.idxInSegment += ll;
				offset += ll;

				if(inOperation_readwrite.idxInSegment == inOperation_readwrite.theCurrentSegment->fHeight)
				{
					inOperation_readwrite.idxSegment++;
					inOperation_readwrite.idxInSegment = 0;

					inOperation_readwrite.theCurrentSegment = NULL;
					if(inOperation_readwrite.idxSegment < this->runtime.theSegments.size())
					{
						inOperation_readwrite.theCurrentSegment = this->runtime.theSegments[inOperation_readwrite.idxSegment];
					}
				}
				if(inOperation_readwrite.theCurrentSegment == NULL)
				{
					res = JVX_ERROR_END_OF_FILE;
					for(i = 0; i < ll; i++)
					{
						for(j = 0; j < this->runtime.numChannels; j++)
						{
							*ptrIn_interleaved++ = 0.0;
						}
					}
					numSamples = 0;
				}
			}
		}
		else
		{
			res = JVX_ERROR_END_OF_FILE;
			numReturned = offset;
		}
	}
	this->unlock();
	return(res);
}

jvxErrorType 
CjvxOneAudioBuffer::postprocess_read()
{
	jvxErrorType res = JVX_NO_ERROR;
	this->lock();
	if(runtime.theState == JVX_STATE_PROCESSING)
	{	
		inOperation_readwrite.theCurrentSegment = NULL;
		this->inOperation_readwrite.loop = NULL;
		runtime.theState = JVX_STATE_ACTIVE;
	}
	this->unlock();
	return(res);
}

jvxErrorType 
CjvxOneAudioBuffer::callback_thread_wokeup_static(jvxHandle* privateData_thread, jvxInt64 timestamp_us)
{
	if(privateData_thread)
	{
		CjvxOneAudioBuffer* this_pointer = (CjvxOneAudioBuffer*)privateData_thread;

		return(this_pointer->callback_thread_wokeup_inClass(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxOneAudioBuffer::callback_thread_wokeup_inClass(jvxInt64 timestamp_us)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	oneAudioSegment  * newSeg = new oneAudioSegment;

	newSeg->buffers = new jvxData*[runtime.numChannels];
	newSeg->fHeight = 0;
	newSeg->idx = this->inOperation_readwrite.buf_cnt++;

	for(i = 0; i < runtime.numChannels; i++)
	{
		newSeg->buffers[i] = new jvxData[runtime.lengthSegment];
		memset(newSeg->buffers[i], 0, sizeof(jvxData)*runtime.lengthSegment);
	}

	if(runtime.theState == JVX_STATE_PROCESSING)
	{
		this->runtime.theSegments.push_back(inOperation_readwrite.theNextSegment); // If we are in state processing, nothing should be happening to the segments

		this->lock();
		inOperation_readwrite.theNextSegment = newSeg;
		this->unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(JVX_NO_ERROR);
}

void 
CjvxOneAudioBuffer::fill_view_buffer(jvxData** y, jvxData** x, jvxSize bufLength, jvxInt32 maxNumberSamplesShow)
{
	jvxSize i;
	jvxSize llSig = 0;
	jvxSize numSamplesSoFar = 0;
	resampler_plotter_private* resLeft = NULL;
	resampler_plotter_private* resRight = NULL;
	jvxData in_left;
	jvxData in_right;
	jvxData min_left;
	jvxData min_right;
	jvxData max_left;
	jvxData max_right;
	jvxCBool prodLeft; 
	jvxCBool prodRight; 
	jvxInt32 idxSeg = 0;
	jvxSize idxInSeg = 0;
	oneAudioSegment* theSeg;

	this->lock();
	this->length_samples(llSig);
	this->unlock();

	jvxSize maxShow = llSig;
	if(maxNumberSamplesShow > 0)
	{
		idxSeg = -floor(((jvxData)maxNumberSamplesShow - (jvxData)llSig)/(jvxData)runtime.lengthSegment);
		idxSeg = JVX_MAX(0, idxSeg);
	}

	jvxData xmax = (jvxData)llSig / (jvxData)runtime.sRate;
	jvxData xmin = ((jvxData)runtime.lengthSegment * idxSeg) / (jvxData)runtime.sRate;

	for(i = 0; i < bufLength; i++)
	{
		jvxData tmp = (jvxData)i/(jvxData)bufLength*xmax + xmin;
		x[0][i] = tmp;
		x[1][i] = tmp;
	}

	jvxData resamplingFactor = ((jvxData)llSig - (jvxData)runtime.lengthSegment * idxSeg)/(jvxData)bufLength * 2;
	resamplingFactor = JVX_MAX(1, resamplingFactor);
	jvx_resampler_plotter_init(&resLeft, resamplingFactor);
	jvx_resampler_plotter_init(&resRight, resamplingFactor);
	
	theSeg = NULL;

	while(numSamplesSoFar < bufLength)
	{
		if(theSeg == NULL)
		{
			this->lock();
			if(idxSeg < this->runtime.theSegments.size())
			{
				theSeg = runtime.theSegments[idxSeg];
				idxSeg++;
			}
			this->unlock();

			if(theSeg == NULL)
			{
				// Fill the rest with zeros
				memset(y[0] + numSamplesSoFar, 0, sizeof(jvxData) * (bufLength - numSamplesSoFar));
				memset(y[1] + numSamplesSoFar, 0, sizeof(jvxData) * (bufLength - numSamplesSoFar));
				numSamplesSoFar = bufLength;
			}
		}
		else
		{
			for(i = 0; i < theSeg->fHeight; i++)
			{
				in_left = theSeg->buffers[0][i];
				in_right = theSeg->buffers[1][i];

				jvx_resampler_plotter_process(in_left, &min_left, &max_left, &prodLeft, resLeft);
				jvx_resampler_plotter_process(in_right, &min_right, &max_right, &prodRight, resRight);

				if(prodLeft && prodRight)
				{
					y[0][numSamplesSoFar] = min_left;
					y[1][numSamplesSoFar] = min_right;
					numSamplesSoFar++;
					y[0][numSamplesSoFar] = max_left;
					y[1][numSamplesSoFar] = max_right;
					numSamplesSoFar++;
					if(numSamplesSoFar >= bufLength)
					{
						break;
					}
				}
			}
			theSeg = NULL;
		}
	}
}