#include "CjvxMultiChannelCircBufferPlotResampler.h"

inline jvxErrorType processSamplesToCircBuffer_visualResampler(jvxData* bPtrOut, jvxSize lCircBuffer, jvxSize idxWrite, jvxSize spaceCircBuffer,
			jvxData** inPtrSecBuffer, jvxSize lSecBuffer, jvxSize idxRead, 
			jvxSize numSamplesIn, jvxSize numChannels, jvxHandle* privateData, 
			jvxSize& numSamplesTaken, jvxSize& numSamplesWritten, jvxBool& requestCallAgain)
{
	jvxSize i, j;
	jvxSize sampleIncrement;
	jvxData tmp;
	jvxData out_min;
	jvxData out_max;
	jvxCBool gave_output;

	jvxSize numNewSamples = 0;

	numSamplesTaken = 0;
	numSamplesWritten = 0;
	requestCallAgain = true;

	privateVisualResampleStruct* str= (privateVisualResampleStruct*)privateData;

	// Make sure there is enough space for write...
	if(str->resamplingFactor_fac == 1)
	{
		if(spaceCircBuffer < 2 )
		{
			// Supersede the calling function end decision..
			requestCallAgain = false;
			return(JVX_NO_ERROR);
		}

		for(i = 0; i < numSamplesIn; i++)
		{
			// Make sure there is enough space for write...
			if(spaceCircBuffer < 2 )
			{
				// Supersede the calling function end decision..
				requestCallAgain = true;
				return(JVX_NO_ERROR);
			}

			numSamplesTaken ++;
			sampleIncrement = 0;

			jvxSize idxWrite0 = idxWrite;
			jvxSize numSamplesWritten0 = numSamplesWritten;
			jvxSize spaceCircBuffer0 = spaceCircBuffer;

			for(j = 0; j < numChannels; j++)
			{
				jvxData* inPtr = inPtrSecBuffer[j];
				inPtr += idxRead;
				
				// Reset in buffer counters
				idxWrite0 = idxWrite;
				numSamplesWritten0 = numSamplesWritten;
				spaceCircBuffer0 = spaceCircBuffer;

				tmp = inPtr[i];

				jvx_resampler_plotter_process_reconfig(tmp, &out_min, &out_max, &gave_output, str->resamplersPlotter[j], str->resamplingFactor_use, NULL);
				if(gave_output)
				{
					jvxData* outFld = bPtrOut;
					outFld += j * lCircBuffer;
					
					outFld[idxWrite0] = out_min;
					idxWrite0 = (idxWrite0 + 1) % lCircBuffer;
					spaceCircBuffer0 --;
					numSamplesWritten0 ++;
					outFld[idxWrite0] = out_max;
					idxWrite0 = (idxWrite0 + 1) % lCircBuffer;
					spaceCircBuffer0 --;
					numSamplesWritten0 ++;
				}
			}

			// Update all frame counters. It will happen only based on last channel update in hope hat all channels are treated syncronously
			idxWrite = idxWrite0;
			numSamplesWritten = numSamplesWritten0;
			spaceCircBuffer = spaceCircBuffer0;
		}
	}
	else
	{
		if(spaceCircBuffer < 2 *str->resamplingFactor_fac)
		{
			// Supersede the calling function end decision..
			requestCallAgain = false;
			return(JVX_NO_ERROR);
		}

		for(i = 0; i < numSamplesIn; i++)
		{
			// Make sure there is enough space for write...
			if(spaceCircBuffer < 2 * str->resamplingFactor_fac)
			{
				// Supersede the calling function end decision..
				requestCallAgain = true;
				return(JVX_NO_ERROR);
			}

			numSamplesTaken ++;
			sampleIncrement = 0;
			for(j = 0; j < numChannels; j++)
			{
				jvxData* inPtr = inPtrSecBuffer[j];
				inPtr += idxRead;

				tmp = inPtr[i];

				jvx_resampler_plotter_process_reconfig(tmp, &out_min, &out_max, &gave_output, str->resamplersPlotter[j], str->resamplingFactor_use, NULL);
				if(gave_output)
				{
					jvxData* outFld = bPtrOut;
					outFld += j * lCircBuffer;
					for(j = 0; j < str->resamplingFactor_fac; j++)
					{
						outFld[idxWrite] = out_min;
						idxWrite = (idxWrite + 1) % lCircBuffer;
						spaceCircBuffer --;
						numSamplesWritten ++;
					}
					for(j = 0; j < str->resamplingFactor_fac; j++)
					{
						outFld[idxWrite] = out_max;
						idxWrite = (idxWrite + 1) % lCircBuffer;
						spaceCircBuffer --;
						numSamplesWritten ++;
					}
				}
			}
		}
	}
	return(JVX_NO_ERROR);
}

inline jvxErrorType processSamplesToCircBuffer_inbandTrigger(jvxData* bPtrOut, jvxSize lCircBuffer, jvxSize idxWrite, jvxSize spaceCircBuffer,
			jvxData** inPtrSecBuffer, jvxSize lSecBuffer, jvxSize idxRead, 
			jvxSize numSamplesIn, jvxSize numChannels, jvxHandle* privateData, 
			jvxSize& numSamplesTaken, jvxSize& numSamplesWritten, jvxBool& requestCallAgain)
{
	jvxSize i, j;
	jvxData tmp;

	jvxData factor = 1;

	numSamplesTaken = 0;
	numSamplesWritten = 0;
	requestCallAgain = true;

	privateTriggeredBuffer_inband* str= (privateTriggeredBuffer_inband*)privateData;
	if(str->onRisingEdge)
	{
		factor = -1;
	}

	if(spaceCircBuffer == 0)
	{
		// Supersede the calling function end decision..

		// If circular buffer is full, take out the samples from secondary buffer and throw away
		numSamplesTaken = numSamplesIn;
		requestCallAgain = false;
		return(JVX_NO_ERROR);
	}

	for(i = 0; i < numSamplesIn; i++)
	{
		if(str->theStatus == JVX_STATE_INIT)
		{
			if(str->triggerLine < numChannels)
			{
				jvxData* inPtr = inPtrSecBuffer[str->triggerLine];
				inPtr += idxRead;

				tmp = inPtr[i];

				if(((tmp - str->triggerValue) * factor) > 0)
				{
					str->theStatus = JVX_STATE_SELECTED;
				}
			}
		}

		if(str->theStatus == JVX_STATE_SELECTED)
		{
			if(str->triggerLine < numChannels)
			{
				jvxData* inPtr = inPtrSecBuffer[str->triggerLine];
				inPtr += idxRead;

				tmp = inPtr[i];

				if(((tmp - str->triggerValue) * factor) < 0)
				{
					str->theStatus = JVX_STATE_PROCESSING;
				}
			}
		}

		if(str->theStatus == JVX_STATE_PROCESSING)
		{
			for(j = 0; j < numChannels; j++)
			{
				jvxData* inPtr = inPtrSecBuffer[j];
				jvxData* outFld = bPtrOut;
				outFld += j * lCircBuffer;
				inPtr += idxRead;

				tmp = inPtr[i];

				outFld[idxWrite] = tmp;
			}
			idxWrite = (idxWrite + 1) % lCircBuffer;
			spaceCircBuffer --;
			numSamplesWritten ++;
			numSamplesTaken ++;
			if(spaceCircBuffer == 0)
			{
				return(JVX_NO_ERROR);
			}
		}
		else
		{
			numSamplesTaken++;
		}
	}
	return(JVX_NO_ERROR);
}

// =================================================================================

CjvxMultiChannelCircBufferPlotResampler::CjvxMultiChannelCircBufferPlotResampler()
{
	processing.theState = JVX_STATE_INIT;

	processing.str_visualResampler.resamplersPlotter = NULL;
	processing.str_visualResampler.resamplingFactor_use = 1.0;
	processing.str_visualResampler.resamplingFactor_spec = 1.0;
	processing.str_visualResampler.resamplingFactor_fac = 1.0;


	processing.str_triggerInband.onRisingEdge = true;
	processing.str_triggerInband.theStatus = JVX_STATE_ACTIVE;
	processing.str_triggerInband.triggerLine = 0;
	processing.str_triggerInband.triggerValue = 0.0;

	processing.sec_buffers.fHeight = 0;
	processing.sec_buffers.fld = NULL;
	processing.sec_buffers.bLength = 0;
	processing.sec_buffers.idxRead = 0;

	processing.num_channels = 0;
	processing.buffersize = 0;

	processing.my_callbacks.callback_thread_startup = st_callback_thread_startup;
	processing.my_callbacks.callback_thread_stopped = st_callback_thread_stopped;
	processing.my_callbacks.callback_thread_timer_expired = st_callback_thread_timer_expired;
	processing.my_callbacks.callback_thread_wokeup = st_callback_thread_wokeup;
}
	
jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::init_circbuffer(jvxSize numChannels, jvxSize buffersize, jvxSize numBuffersSecBuffer, jvxBitField features)
{
	if(processing.theState == JVX_STATE_INIT)
	{
		jvxSize i;
		processing.num_channels = numChannels;
		processing.buffersize = buffersize;
		processing.featuresModes = features;

		if(JVX_EVALUATE_BITFIELD(processing.featuresModes & JVX_CIRCBUFFER_VISUAL_RESAMPLER))
		{
			// Allocate the resamplers for plotting
			JVX_SAFE_NEW_FLD(processing.str_visualResampler.resamplersPlotter, resampler_plotter_private*, processing.num_channels);
			for(i = 0; i < processing.num_channels; i++)
			{
				jvx_resampler_plotter_init(&processing.str_visualResampler.resamplersPlotter[i], 1);
			}
		}

		// Start secondary field in case plotter is currently blocked
		processing.sec_buffers.bLength = processing.buffersize * numBuffersSecBuffer;
		processing.sec_buffers.idxRead = 0;
		processing.sec_buffers.fHeight = 0;

		JVX_SAFE_NEW_FLD(processing.sec_buffers.fld, jvxData*, processing.num_channels);
		for(i = 0; i < processing.num_channels; i++)
		{		
			JVX_SAFE_NEW_FLD(processing.sec_buffers.fld[i], jvxData, processing.sec_buffers.bLength );
			memset(processing.sec_buffers.fld[i], 0, sizeof(jvxData) * processing.sec_buffers.bLength);
		}

		JVX_INITIALIZE_MUTEX(processing.sec_buffers.safeAccess);
		processing.theState = JVX_STATE_ACTIVE;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}
	
jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::set_endpoint_circbuffer(jvxExternalBuffer* endpoint)
{
	if(processing.theState == JVX_STATE_ACTIVE)
	{
		processing.theEndpoint = endpoint;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}
	
jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::terminate_circbuffer()
{
	jvxSize i;
	if(processing.theState == JVX_STATE_ACTIVE)
	{
		if(JVX_EVALUATE_BITFIELD(processing.featuresModes & JVX_CIRCBUFFER_VISUAL_RESAMPLER))
		{
			// Deallocate the resamplers
			for(i = 0; i < processing.num_channels; i++)
			{
				jvx_resampler_plotter_terminate(processing.str_visualResampler.resamplersPlotter[i]);
			}
			JVX_SAFE_DELETE_FLD(processing.str_visualResampler.resamplersPlotter, resampler_plotter_private*);
		}

		for(i = 0; i < processing.num_channels; i++)
		{
			JVX_SAFE_DELETE_FLD(processing.sec_buffers.fld[i], jvxData);
		}

		JVX_SAFE_DELETE_FLD(processing.sec_buffers.fld, jvxData*);
		processing.sec_buffers.fHeight = 0;
		processing.sec_buffers.bLength = 0;
		processing.sec_buffers.idxRead = 0;

		processing.theEndpoint = NULL;

		JVX_TERMINATE_MUTEX(processing.sec_buffers.safeAccess);

		processing.theState = JVX_STATE_INIT;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::process_circbuffer(jvxData** samples, bool strictlyNoBlock)
{
	jvxSize i;
	jvxSize spaceTargetBuffer = 0;
	jvxSize idxWrite = 0;
	jvxData* fldSrc = NULL;
	jvxData* fldDest = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE rs = JVX_TRY_LOCK_MUTEX_SUCCESS;

	assert(processing.theState == JVX_STATE_PROCESSING);

	if(strictlyNoBlock)
	{
		JVX_TRY_LOCK_MUTEX(rs, processing.sec_buffers.safeAccess);
		if(rs != JVX_TRY_LOCK_MUTEX_SUCCESS)
			return(JVX_ERROR_COMPONENT_BUSY);
	}
	else
	{
		JVX_LOCK_MUTEX(processing.sec_buffers.safeAccess);
	}

	idxWrite = (processing.sec_buffers.idxRead + processing.sec_buffers.fHeight) % processing.sec_buffers.bLength;
	spaceTargetBuffer = processing.sec_buffers.bLength - processing.sec_buffers.fHeight;

	JVX_UNLOCK_MUTEX(processing.sec_buffers.safeAccess);

	if(spaceTargetBuffer >= processing.buffersize)
	{
		// Regular case: Enough space in sec buffer
		jvxSize ll1 = processing.buffersize;
		ll1 = JVX_MIN(ll1, processing.sec_buffers.bLength - idxWrite);
		jvxSize ll2 = processing.buffersize - ll1;

		for(i = 0; i < processing.num_channels; i++)
		{
			fldSrc = samples[i];
			if(ll1)
			{
				fldDest = processing.sec_buffers.fld[i] + idxWrite;
				memcpy(fldDest, fldSrc, ll1 * sizeof(jvxData));

				fldSrc += ll1;
			}
			if(ll2)
			{
				fldDest = processing.sec_buffers.fld[i];
				memcpy(fldDest, fldSrc, ll2 * sizeof(jvxData));
			}
		}

		if(strictlyNoBlock)
		{
			JVX_TRY_LOCK_MUTEX(rs,processing.sec_buffers.safeAccess);
			if(rs != JVX_TRY_LOCK_MUTEX_SUCCESS)
				return(JVX_ERROR_COMPONENT_BUSY);
		}
		else
		{
			JVX_LOCK_MUTEX(processing.sec_buffers.safeAccess);
		}

		processing.sec_buffers.fHeight += processing.buffersize;

		JVX_UNLOCK_MUTEX(processing.sec_buffers.safeAccess);

		// Trigger background thread even if busy
		jvx_thread_wakeup(processing.theResamplerThread);

		res = JVX_NO_ERROR;
	}
	else
	{
		res = JVX_ERROR_BUFFER_OVERFLOW;
		
		// Trigger background thread even if currently overflow
		jvx_thread_wakeup(processing.theResamplerThread);
	}
	return(res);
}

jvxErrorType
CjvxMultiChannelCircBufferPlotResampler::set_properties_circbuffer(jvxCircBufferDataCollectorMode collectorMode, jvxSize idx, jvxHandle* prop_val)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize lineId = 0;
	if(processing.theState >= JVX_STATE_ACTIVE)
	{
		if(JVX_EVALUATE_BITFIELD(processing.featuresModes & collectorMode))
		{
			switch(collectorMode)
			{
			case JVX_CIRCBUFFER_VISUAL_RESAMPLER:
				processing.str_visualResampler.resamplingFactor_spec = *((jvxData*)prop_val);
				processing.str_visualResampler.resamplingFactor_fac = 1;
				processing.str_visualResampler.resamplingFactor_use = processing.str_visualResampler.resamplingFactor_spec;
				if(processing.str_visualResampler.resamplingFactor_use < 1.0)
				{
					processing.str_visualResampler.resamplingFactor_fac = JVX_DATA2INT32(ceil(1.0/processing.str_visualResampler.resamplingFactor_use));
					processing.str_visualResampler.resamplingFactor_use *= processing.str_visualResampler.resamplingFactor_fac;
				}
				break;
			case JVX_CIRCBUFFER_TRIGGER_INBAND:
				switch(idx)
				{
				case 0:
					processing.str_triggerInband.theStatus = JVX_STATE_INIT;
					break;
				case 1:
					processing.str_triggerInband.triggerLine = *((jvxSize*)prop_val);
					break;
				case 2:
					processing.str_triggerInband.triggerValue = *((jvxData*)prop_val);
					break;
				case 3:

					processing.str_triggerInband.onRisingEdge = *((jvxBool*)prop_val);
					break;
				default:
					assert(0);
				}
				break;
			default:
				res = JVX_ERROR_INVALID_SETTING;
				break;
			}
		}
		else
		{
			res = JVX_ERROR_UNSUPPORTED;
		}
		return(res);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxMultiChannelCircBufferPlotResampler::get_properties_circbuffer(jvxCircBufferDataCollectorMode collectorMode, jvxSize idx, jvxHandle* prop_val)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(processing.theState >= JVX_STATE_ACTIVE)
	{
		if(JVX_EVALUATE_BITFIELD(processing.featuresModes & collectorMode))
		{
			switch(collectorMode)
			{
			case JVX_CIRCBUFFER_VISUAL_RESAMPLER:
				*((jvxData*)prop_val) = processing.str_visualResampler.resamplingFactor_spec;
				break;
			default:
				res = JVX_ERROR_INVALID_SETTING;
				break;
			}
		}
		else
		{
			res = JVX_ERROR_UNSUPPORTED;
		}
		return(res);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxMultiChannelCircBufferPlotResampler::start_circbuffer(jvxCircBufferThreadingMode threadMode, jvxBool boostPrio)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	if(processing.theState == JVX_STATE_ACTIVE)
	{
		res = jvx_thread_initialize(&processing.theResamplerThread, &processing.my_callbacks, reinterpret_cast<jvxHandle*>(this), false); 
		assert(res == JVX_NO_ERROR);

		res = jvx_thread_start(processing.theResamplerThread, JVX_INFINITE_MS);
		assert(res == JVX_NO_ERROR);

		if(boostPrio)
		{
			res = jvx_thread_set_priority(processing.theResamplerThread, JVX_PRIORITY_REALTIME);
			assert(res == JVX_NO_ERROR);
		}
		processing.theState = JVX_STATE_PROCESSING;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::set_mode_circbuffer(jvxCircBufferDataCollectorMode theMode)
{
	if(processing.theState >= JVX_STATE_ACTIVE)
	{
		if(JVX_EVALUATE_BITFIELD(processing.featuresModes & theMode))
		{
			processing.theMode = theMode;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_SETTING);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxMultiChannelCircBufferPlotResampler::stop_circbuffer()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(processing.theState == JVX_STATE_PROCESSING)
	{
		res = jvx_thread_stop(processing.theResamplerThread);
		assert(res == JVX_NO_ERROR);

		res = jvx_thread_terminate(processing.theResamplerThread);
		assert(res == JVX_NO_ERROR);

		processing.theResamplerThread = NULL;

		processing.theState = JVX_STATE_ACTIVE;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

// ===================================================================================================
// ===================================================================================================

jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::st_callback_thread_startup(jvxHandle* privateData_thread, jvxInt64 timestamp_us)
{
	if(privateData_thread)
	{
		CjvxMultiChannelCircBufferPlotResampler* tpointer = (CjvxMultiChannelCircBufferPlotResampler*)privateData_thread;
		return(tpointer->ic_callback_thread_startup(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::st_callback_thread_timer_expired(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	if(privateData_thread)
	{
		CjvxMultiChannelCircBufferPlotResampler* tpointer = (CjvxMultiChannelCircBufferPlotResampler*)privateData_thread;
		return(tpointer->ic_callback_thread_timer_expired(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::st_callback_thread_wokeup(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	if(privateData_thread)
	{
		CjvxMultiChannelCircBufferPlotResampler* tpointer = (CjvxMultiChannelCircBufferPlotResampler*)privateData_thread;
		return(tpointer->ic_callback_thread_wokeup(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::st_callback_thread_stopped(jvxHandle* privateData_thread, jvxInt64 timestamp_us)
{
	if(privateData_thread)
	{
		CjvxMultiChannelCircBufferPlotResampler* tpointer = (CjvxMultiChannelCircBufferPlotResampler*)privateData_thread;
		return(tpointer->ic_callback_thread_stopped(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::ic_callback_thread_startup(jvxInt64 timestamp_s)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::ic_callback_thread_timer_expired(jvxInt64 timestamp_us)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::ic_callback_thread_wokeup(jvxInt64 timestamp_us)
{
	jvxSize spaceCircBuffer = 0;
	jvxSize fHeightBuffer = 0;
	jvxSize idxWrite = 0;
	jvxSize idxRead = 0;
	jvxSize lCircBuffer = 0;
	jvxSize lSecBuffer = 0;
	jvxSize numSamplesUntilWrap = 0;

	jvxSize ll1;
	jvxBool sampleIncrement = 1;

	jvxSize numSamplesTaken = 0;
	jvxSize numSamplesWritten = 0;
	jvxBool requestCallAgain = true;

	assert(processing.theEndpoint);

	while(1)
	{
		// Get safe access to circ buffer
		processing.theEndpoint->safe_access.lockf(processing.theEndpoint->safe_access.priv);

		lCircBuffer = processing.theEndpoint->length;
		spaceCircBuffer = JVX_MAX((jvxInt32)lCircBuffer - (jvxInt32)processing.theEndpoint->fill_height, 0);
		idxWrite = (processing.theEndpoint->idx_read + processing.theEndpoint->fill_height) % lCircBuffer;
		numSamplesUntilWrap = lCircBuffer - idxWrite;

		processing.theEndpoint->safe_access.unlockf(processing.theEndpoint->safe_access.priv);

		JVX_LOCK_MUTEX(processing.sec_buffers.safeAccess);

		lSecBuffer = processing.sec_buffers.bLength;
		idxRead =  processing.sec_buffers.idxRead;
		fHeightBuffer = processing.sec_buffers.fHeight;

		JVX_UNLOCK_MUTEX(processing.sec_buffers.safeAccess);

		if(fHeightBuffer == 0)
		{
			// No input samples
			break;
		}

		jvxByte* bPtrOut = processing.theEndpoint->ptrFld;
		
		assert(processing.theEndpoint->formFld == JVX_DATAFORMAT_DATA);

		// Let us only process input samples until the end of the circ input buffer
		ll1 = fHeightBuffer;
		ll1 = JVX_MIN(lSecBuffer - idxRead, ll1);
		
		numSamplesTaken = 0;
		numSamplesWritten = 0;
		requestCallAgain = true;

		switch(processing.theMode)
		{
		case JVX_CIRCBUFFER_VISUAL_RESAMPLER:
			processSamplesToCircBuffer_visualResampler(
				(jvxData*)bPtrOut, lCircBuffer, idxWrite, spaceCircBuffer,
				processing.sec_buffers.fld, lSecBuffer, idxRead, 
				ll1, processing.num_channels, &processing.str_visualResampler,
				numSamplesTaken, numSamplesWritten, requestCallAgain);
			break;
		case JVX_CIRCBUFFER_TRIGGER_INBAND:
			processSamplesToCircBuffer_inbandTrigger(
				(jvxData*)bPtrOut, lCircBuffer, idxWrite, spaceCircBuffer,
				processing.sec_buffers.fld, lSecBuffer, idxRead, 
				ll1, processing.num_channels, &processing.str_triggerInband,
				numSamplesTaken, numSamplesWritten, requestCallAgain);
			break;
		}

		// Get safe access to circ buffer
		processing.theEndpoint->safe_access.lockf(processing.theEndpoint->safe_access.priv);

		processing.theEndpoint->fill_height += numSamplesWritten;
		if(numSamplesWritten >= numSamplesUntilWrap)
		{
			processing.theEndpoint->specific.the1DCircBuffer.numWraps ++;
		}

		processing.theEndpoint->safe_access.unlockf(processing.theEndpoint->safe_access.priv);

		JVX_LOCK_MUTEX(processing.sec_buffers.safeAccess);

		processing.sec_buffers.idxRead = (processing.sec_buffers.idxRead + numSamplesTaken) % processing.sec_buffers.bLength;
		processing.sec_buffers.fHeight -= numSamplesTaken;

		JVX_UNLOCK_MUTEX(processing.sec_buffers.safeAccess);

		if(!requestCallAgain)
		{
			// The specific rfesampler informed that we are in a condition where we can not take additional output data
			break;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxMultiChannelCircBufferPlotResampler::ic_callback_thread_stopped(jvxInt64 timestamp_us)
{
	return(JVX_NO_ERROR);
}
