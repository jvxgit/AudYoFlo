#include "jvxText2Speech/CjvxTxt2Speech_mix.h"

/*
typedef struct
{
	jvxData* fld;
	jvxSize fld_length;
	jvxBool playout_done;
	jvxSize idxSamples;

} oneChannelStruct;
private:
	std::vector<oneChannelStruct> theChannels;
	jvxState theState;
	jvxSize channelIdx;
*/
CjvxTxt2Speech_mix::CjvxTxt2Speech_mix()
{
	theState = JVX_STATE_NONE;
	channelIdx = JVX_SIZE_UNSELECTED;
	coreHdl = NULL;
	formT2s = JVX_TEXT2SPEECH_48KHZ16BITMONO;
	JVX_INITIALIZE_MUTEX(safeAccess);
}

CjvxTxt2Speech_mix::~CjvxTxt2Speech_mix()
{
	JVX_TERMINATE_MUTEX(safeAccess);
}

jvxErrorType 
CjvxTxt2Speech_mix::initialize()
{
	if (theState == JVX_STATE_NONE)
	{
		theState = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}
	
jvxErrorType 
CjvxTxt2Speech_mix::prepare(IjvxText2Speech* coreHdlI, std::string txtPrefix, jvxSize srate, jvxSize buffersize, 
	jvxSize num_channels, jvxSize out_chan_offset)
{
	jvxSize i;
	if (theState == JVX_STATE_INIT)
	{
		switch (srate)
		{
		case 8000:
			formT2s = JVX_TEXT2SPEECH_8KHZ16BITMONO;
			break;
		case 16000:
			formT2s = JVX_TEXT2SPEECH_16KHZ16BITMONO;
			break;
		case 32000:
			formT2s = JVX_TEXT2SPEECH_32KHZ16BITMONO;
			break;
		case 48000:
			formT2s = JVX_TEXT2SPEECH_48KHZ16BITMONO;
			break;
		default:
			return JVX_ERROR_INVALID_SETTING;
		}
		coreHdl = coreHdlI;
		for (i = 0; i < num_channels; i++)
		{
			oneChannelStruct theNew;
			theNew.fld = NULL;
			theNew.fld_length = 0;
			theNew.idxSamples = JVX_SIZE_UNSELECTED;
			theNew.playout_done = false;
			theNew.txtOut = txtPrefix + " " + jvx_size2String(i+ out_chan_offset);
			theChannels.push_back(theNew);
		}
		this->bsize = buffersize;

		if(coreHdl)
		{
			for (i = 0; i < num_channels; i++)
			{
				coreHdl->add_task(theChannels[i].txtOut.c_str(), &formT2s, this, &theChannels[i].selector);
			}
		}
		channelIdx = JVX_SIZE_UNSELECTED;
		theState = JVX_STATE_PREPARED;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}
	
jvxErrorType
CjvxTxt2Speech_mix::trigger_start()
{
	jvxSize i;
	if (theState == JVX_STATE_PREPARED)
	{
		theState = JVX_STATE_PROCESSING;
		channelIdx = 0;
		for (i = 0; i < theChannels.size(); i++)
		{
			theChannels[i].playout_done = false;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxTxt2Speech_mix::trigger_stop()
{
	jvxSize i;
	if (theState == JVX_STATE_PROCESSING)
	{
		theState = JVX_STATE_PREPARED;
		channelIdx = 0;

		JVX_LOCK_MUTEX(safeAccess);

		for (i = 0; i < theChannels.size(); i++)
		{
			theChannels[i].playout_done = false;
			theChannels[i].idxSamples = 0;
		}
		channelIdx = 0;
		JVX_UNLOCK_MUTEX(safeAccess);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxTxt2Speech_mix::process(jvxData** bufs, jvxData gain, jvxBool add_text, jvxSize* chan_idx)
{
	jvxSize j;
	oneChannelStruct* theCurrentStruct;
	if (theState == JVX_STATE_PROCESSING)
	{
		jvxSize write_offset = 0;
		jvxSize bsize_togo = bsize;
		jvxBool leaveLoop = false;
		jvxBool stop_playback = false;
		while (!leaveLoop)
		{
			JVX_TRY_LOCK_MUTEX_RESULT_TYPE resM = JVX_TRY_LOCK_MUTEX_SUCCESS;
			JVX_TRY_LOCK_MUTEX(resM, safeAccess);
			if (resM == JVX_TRY_LOCK_MUTEX_SUCCESS)
			{
				theCurrentStruct = &theChannels[channelIdx];
				//assert(theCurrentStruct->playout_done == false);
				if (JVX_CHECK_SIZE_SELECTED(theCurrentStruct->idxSamples))
				{
					jvxSize ll1 = JVX_MIN(bsize_togo, theCurrentStruct->fld_length - theCurrentStruct->idxSamples);
					jvxData* fromPtr = theCurrentStruct->fld + theCurrentStruct->idxSamples;
					jvxData* toPtr = bufs[channelIdx] + write_offset;
					if (add_text)
					{
						for (j = 0; j < ll1; j++)
						{
							toPtr[j] += fromPtr[j] * gain;
						}
					}
					else
					{
						for (j = 0; j < ll1; j++)
						{
							toPtr[j] = fromPtr[j] * gain;
						}
					}
					write_offset += ll1;
					bsize_togo -= ll1;
					theCurrentStruct->idxSamples += ll1;
				}
				else
				{
					// No sample has been loaded yet
					leaveLoop = true;
				}

				if (theCurrentStruct->idxSamples == theCurrentStruct->fld_length)
				{
					theCurrentStruct->playout_done = true;
					theCurrentStruct->idxSamples = 0;
					channelIdx++;
					if (channelIdx == theChannels.size())
					{
						stop_playback = true;
					}
				}
				if (bsize_togo == 0)
				{
					leaveLoop = true;
				}
				JVX_UNLOCK_MUTEX(safeAccess);
			}
			else
			{
				// Output not yet ready
			}

			if (stop_playback)
			{
				theState = JVX_STATE_PREPARED;
				channelIdx = JVX_SIZE_UNSELECTED;
				leaveLoop = true;
			}
		}

		if (chan_idx)
		{
			*chan_idx = JVX_SIZE_UNSELECTED;
			if (theState == JVX_STATE_PROCESSING)
			{
				*chan_idx = channelIdx;
			}
		}
	}
	return JVX_NO_ERROR;
}
	
jvxErrorType 
CjvxTxt2Speech_mix::postprocess()
{
	jvxSize i;
	jvxErrorType resL = JVX_ERROR_COMPONENT_BUSY;
	if (theState >= JVX_STATE_PREPARED)
	{
		if(coreHdl)
		{
			while (resL == JVX_ERROR_COMPONENT_BUSY)
			{
				resL = coreHdl->postprocess();
			}
			for (i = 0; i < theChannels.size(); i++)
			{
				if (theChannels[i].fld)
				{
					JVX_DSP_SAFE_DELETE_FIELD(theChannels[i].fld);
					theChannels[i].fld = NULL;
				}
			}
		}
		theChannels.clear();
		theState = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}
	
jvxErrorType 
CjvxTxt2Speech_mix::terminate()
{
	if (theState > JVX_STATE_INIT)
	{
		this->postprocess();
	}
	theState = JVX_STATE_NONE;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxTxt2Speech_mix::task_complete(jvxByte* fld, jvxSize szOutput,
	jvxText2SpeechFormat out_format, jvxSize uniqueId)
{
	jvxSize i;
	std::vector<oneChannelStruct>::iterator elm = theChannels.begin();
	elm = jvx_findItemSelectorInList<oneChannelStruct, jvxSize>(theChannels, uniqueId);
	if (elm != theChannels.end())
	{
		assert(elm->fld == NULL);
		elm->fld_length = szOutput / sizeof(jvxInt16);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(elm->fld, jvxData, elm->fld_length);

		jvxInt16* ptrFrom = (jvxInt16*)fld;
		for (i = 0; i < elm->fld_length; i++)
		{
			elm->fld[i] = JVX_INT16_2_DATA(ptrFrom[i]);
		}
		JVX_LOCK_MUTEX(safeAccess);
		elm->idxSamples = 0;
		elm->playout_done = false;
		JVX_UNLOCK_MUTEX(safeAccess);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxTxt2Speech_mix::generation_failed(jvxSize uniqueId)
{
	std::vector<oneChannelStruct>::iterator elm = theChannels.begin();
	elm = jvx_findItemSelectorInList<oneChannelStruct, jvxSize>(theChannels, uniqueId);
	if (elm != theChannels.end())
	{
		std::cout << "Failed to output text " << elm->txtOut << std::endl;
	}
	return JVX_NO_ERROR;
}
