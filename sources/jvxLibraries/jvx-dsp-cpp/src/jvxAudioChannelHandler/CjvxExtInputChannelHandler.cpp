#include "CjvxExtInputChannelHandler.h"

jvxErrorType jvx_factory_allocate_jvxExtInputChannelHandler(IjvxExtInputChannelHandler** returnedObj)
{
	CjvxExtInputChannelHandler* retObj = NULL;
	JVX_SAFE_NEW_OBJ(retObj, CjvxExtInputChannelHandler);
	*returnedObj = static_cast<IjvxExtInputChannelHandler*>(retObj);
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_factory_deallocate_jvxExtInputChannelHandler(IjvxExtInputChannelHandler* returnedObj)
{
	JVX_SAFE_DELETE_OBJ(returnedObj);
	return(JVX_NO_ERROR);
}

// =========================================================================================

CjvxExtInputChannelHandler::CjvxExtInputChannelHandler()
{
	myInterface = NULL;

	sRate = 0;
	numChannels = 0;

	resetBuffer();
}

jvxErrorType 
CjvxExtInputChannelHandler::connectIf(IjvxExternalAudioChannels* theIf, jvxSize numChannelsArg, jvxInt32 sRateArg, std::string theText)
{
	if (myInterface == NULL)
	{
		myInterface = theIf;
		numChannels = numChannelsArg;
		sRate = sRateArg;
		myInterface->register_one_set(true, numChannels, sRate, &myIdIn, static_cast<IjvxExternalAudioChannels_data*>(this), theText.c_str());
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
};

jvxErrorType
CjvxExtInputChannelHandler::disconnectIf()
{
	if (myInterface)
	{
		myInterface->unregister_one_set(myIdIn);
		myInterface = NULL;
		sRate = 0;
		numChannels = 0;

		resetBuffer();

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}


jvxErrorType 
CjvxExtInputChannelHandler::setBuffer(jvxHandle** buf, jvxDataFormat formatBufferArg, jvxSize szBufferArg)
{
	if(myInterface)
	{
		if (this->inProcessing.myBuffer == NULL)
		{
			this->inProcessing.myBuffer = buf;
			this->inProcessing.formBuffer = formatBufferArg;
			this->inProcessing.szBuffer = szBufferArg;
			this->inProcessing.readOffset = 0;
			this->inProcessing.framesize = 0;
			inProcessing.loop_start = JVX_SIZE_UNSELECTED;
			inProcessing.loop_stop = JVX_SIZE_UNSELECTED;

			inProcessing.pauseOn = false;

			inProcessing.passOnlyFullFrames = false;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxExtInputChannelHandler::resetBuffer()
{
	this->inProcessing.myBuffer = NULL;

	this->inProcessing.formBuffer = JVX_DATAFORMAT_NONE;
	this->inProcessing.szBuffer = 0;
	this->inProcessing.readOffset = 0;
	this->inProcessing.framesize = 0;

	paramsOnStart.framesize = 0;
	paramsOnStart.format = JVX_DATAFORMAT_NONE;

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxExtInputChannelHandler::setPause(jvxBool pauseOn)
{
	inProcessing.pauseOn = pauseOn;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxExtInputChannelHandler::getPause(jvxBool* pauseIsOn)
{
	if(pauseIsOn)
		*pauseIsOn = inProcessing.pauseOn;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxExtInputChannelHandler::setLoopStartStop(jvxSize loopStart , jvxSize loopStop )
{
	if (inProcessing.szBuffer > 0)
	{
		if (JVX_CHECK_SIZE_SELECTED(loopStop))
		{
			inProcessing.loop_stop = loopStop;
			inProcessing.loop_stop = JVX_MAX(inProcessing.loop_stop, 1);
			// inProcessing.loop_stop = JVX_MIN(inProcessing.loop_stop, inProcessing.szBuffer);
		}
		if (JVX_CHECK_SIZE_SELECTED(loopStart))
		{
			inProcessing.loop_start = loopStart;
			inProcessing.loop_start = JVX_MIN(inProcessing.loop_start, inProcessing.loop_stop - 1);
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxExtInputChannelHandler::getLoopStartStop(jvxSize* loopStart, jvxSize* loopStop)
{
	if (loopStart)
		*loopStart = inProcessing.loop_start;
	if (loopStop)
		*loopStop = inProcessing.loop_stop;
	return JVX_NO_ERROR;
}

void 
CjvxExtInputChannelHandler::setPassOnlyFullFrames(jvxBool passOnlyFullFrames)
{
	inProcessing.passOnlyFullFrames = passOnlyFullFrames;
}

jvxData 
CjvxExtInputChannelHandler::progress()
{
	if (inProcessing.szBuffer > 0)
		return((jvxData)inProcessing.readOffset / (jvxData)inProcessing.szBuffer);
	else
		return(0.9);
}

jvxSize 
CjvxExtInputChannelHandler::numberOutputSamples() 
{ 
	return(inProcessing.readOffset); 
}

jvxErrorType 
CjvxExtInputChannelHandler::get_one_frame(jvxSize register_id, jvxHandle** fld)
{
	jvxSize i;
	jvxSize numCopy = 0;

	for (i = 0; i < this->numChannels; i++)
	{
		memset(fld[i], 0, paramsOnStart.framesize * jvxDataFormat_size[paramsOnStart.format]);
	}

	if (inProcessing.pauseOn)
	{
		return JVX_NO_ERROR;
	}

	if (JVX_CHECK_SIZE_SELECTED(inProcessing.loop_stop))
	{
		jvxSize numToWrite = paramsOnStart.framesize;
		jvxSize offsetCopyTo = 0;
		while (numToWrite)
		{
			
			
			jvxSize const stopCopy = JVX_MIN(JVX_MIN(  inProcessing.szBuffer,  inProcessing.loop_stop), inProcessing.readOffset + numToWrite);
			jvxSize const startCopy = JVX_MIN(inProcessing.readOffset, stopCopy);

			numCopy = stopCopy - startCopy;
			
			jvxSize const stopFillZero = JVX_MIN(inProcessing.loop_stop, inProcessing.readOffset + numToWrite);
			jvxSize const startFillZero = JVX_MAX(stopCopy, inProcessing.readOffset);

			jvxSize const numFillZero = stopFillZero - startFillZero;


			if (inProcessing.formBuffer == paramsOnStart.format)
			{
				for (i = 0; i < this->numChannels; i++)
				{
					jvxByte* ptrFrom = (jvxByte*)inProcessing.myBuffer[i];
					ptrFrom += jvxDataFormat_size[paramsOnStart.format] * inProcessing.readOffset;
					jvxByte* copyTo = (jvxByte*)fld[i];
					copyTo += jvxDataFormat_size[paramsOnStart.format] * offsetCopyTo;
					memcpy(copyTo, ptrFrom, jvxDataFormat_size[paramsOnStart.format] * numCopy);
				}
			}
			inProcessing.readOffset += (numCopy + numFillZero);
			numToWrite -= (numCopy + numFillZero);
			offsetCopyTo += numCopy;

			if (inProcessing.readOffset == inProcessing.loop_stop)
			{
				if (JVX_CHECK_SIZE_SELECTED(inProcessing.loop_start))
				{
					inProcessing.readOffset = inProcessing.loop_start;
				}
				else
				{
					inProcessing.readOffset = 0;
				}
			}
		}
	}
	else
	{
		numCopy = inProcessing.szBuffer - inProcessing.readOffset;
		numCopy = JVX_MIN(paramsOnStart.framesize, numCopy);

		if (inProcessing.passOnlyFullFrames)
		{
			if (numCopy != paramsOnStart.framesize)
			{
				return JVX_ERROR_END_OF_FILE;
			}
		}
		else
		{
			if (numCopy == 0)
			{
				return JVX_ERROR_END_OF_FILE;
			}
		}
		if (inProcessing.formBuffer == paramsOnStart.format)
		{
			for (i = 0; i < this->numChannels; i++)
			{
				jvxByte* ptrFrom = (jvxByte*)inProcessing.myBuffer[i];
				ptrFrom += jvxDataFormat_size[paramsOnStart.format] * inProcessing.readOffset;
				memcpy(fld[i], ptrFrom, jvxDataFormat_size[paramsOnStart.format] * numCopy);
			}
		}
		inProcessing.readOffset += numCopy;
	}
	return(JVX_NO_ERROR);
}

void 
CjvxExtInputChannelHandler::addReadOffset(jvxSize offset)
{
	inProcessing.readOffset += offset;
}

jvxSize 
CjvxExtInputChannelHandler::getReadOffset()
{
	return this->inProcessing.readOffset;
}

void
CjvxExtInputChannelHandler::setReadOffset(jvxSize offset)
{	
	this->inProcessing.readOffset = offset;
}

jvxErrorType
CjvxExtInputChannelHandler::put_one_frame(jvxSize register_id, jvxHandle** fld)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxExtInputChannelHandler::prepare(jvxBool is_input, jvxSize register_id, jvxSize framesize, jvxDataFormat format)
{
	paramsOnStart.framesize = framesize;
	paramsOnStart.format = format;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxExtInputChannelHandler::start(jvxBool is_input, jvxSize register_id)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxExtInputChannelHandler::stop(jvxBool is_input, jvxSize register_id)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxExtInputChannelHandler::postprocess(jvxBool is_input, jvxSize register_id)
{
	return(JVX_NO_ERROR);
}
