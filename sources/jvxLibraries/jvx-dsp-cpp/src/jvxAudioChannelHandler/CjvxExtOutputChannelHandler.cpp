#include "CjvxExtOutputChannelHandler.h"

jvxErrorType jvx_factory_allocate_jvxExtOutputChannelHandler(IjvxExtOutputChannelHandler** returnedObj)
{
	CjvxExtOutputChannelHandler* retObj = NULL;
	JVX_SAFE_NEW_OBJ(retObj, CjvxExtOutputChannelHandler);
	*returnedObj = static_cast<IjvxExtOutputChannelHandler*>(retObj);
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_factory_deallocate_jvxExtOutputChannelHandler(IjvxExtOutputChannelHandler* returnedObj)
{
	JVX_SAFE_DELETE_OBJ(returnedObj);
	return(JVX_NO_ERROR);
}

// ==========================================================================

CjvxExtOutputChannelHandler::CjvxExtOutputChannelHandler()
{
	myInterface = NULL;

	sRate = 0;
	numChannels = 0;

	resetBuffer();
}

jvxErrorType 
CjvxExtOutputChannelHandler::connectIf(IjvxExternalAudioChannels* theIf, jvxSize numChannelsArg, jvxInt32 sRateArg, std::string theText)
{
	if (myInterface == NULL)
	{
		myInterface = theIf;
		numChannels = numChannelsArg;
		sRate = sRateArg;
		myInterface->register_one_set(false, numChannels, sRate, &myIdIn, static_cast<IjvxExternalAudioChannels_data*>(this), theText.c_str());
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
};

jvxErrorType
CjvxExtOutputChannelHandler::disconnectIf()
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
CjvxExtOutputChannelHandler::setBuffer(jvxHandle** buf, jvxDataFormat formatBufferArg, jvxSize szBufferArg)
{
	if(myInterface)
	{
		if (this->inProcessing.myBuffer == NULL)
		{
			this->inProcessing.myBuffer = buf;
			this->inProcessing.formBuffer = formatBufferArg;
			this->inProcessing.szBuffer = szBufferArg;
			this->inProcessing.writeOffset = 0;
			this->inProcessing.framesize = 0;
			this->inProcessing.numSkipped = 0;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxExtOutputChannelHandler::resetBuffer()
{
	this->inProcessing.myBuffer = NULL;

	this->inProcessing.formBuffer = JVX_DATAFORMAT_NONE;
	this->inProcessing.szBuffer = 0;
	this->inProcessing.writeOffset = 0;
	this->inProcessing.framesize = 0;
	inProcessing.numSkipped = 0;

	paramsOnStart.framesize = 0;
	paramsOnStart.format = JVX_DATAFORMAT_NONE;

	return JVX_NO_ERROR;
}

jvxSize 
CjvxExtOutputChannelHandler::getNumberWritten() 
{ 
	return(inProcessing.writeOffset); 
}

jvxErrorType 
CjvxExtOutputChannelHandler::get_one_frame(jvxSize register_id, jvxHandle** fld)
{
	return(JVX_NO_ERROR);
}

void 
CjvxExtOutputChannelHandler::addWriteOffset(jvxSize numSkipped)
{
	inProcessing.numSkipped += numSkipped;
}

jvxErrorType
CjvxExtOutputChannelHandler::put_one_frame(jvxSize register_id, jvxHandle** fld)
{
	jvxSize i;
	jvxSize spaceLeft = 0;

	spaceLeft = inProcessing.szBuffer - inProcessing.writeOffset;

	if (spaceLeft == 0)
	{
		return JVX_ERROR_END_OF_FILE;
	}

	jvxSize numCpyOrig = JVX_MIN(paramsOnStart.framesize, spaceLeft);
	jvxSize numCpy = numCpyOrig - inProcessing.numSkipped;
	numCpy = JVX_MAX(numCpy, 0);
	jvxSize numToSkip = numCpyOrig - numCpy;

	if (inProcessing.formBuffer == paramsOnStart.format)
	{
		for (i = 0; i < this->numChannels; i++)
		{
			jvxByte* ptrTo = (jvxByte*)inProcessing.myBuffer[i];
			ptrTo += jvxDataFormat_size[paramsOnStart.format] * inProcessing.writeOffset;
			jvxByte* ptrFrom = (jvxByte*)fld[i];
			ptrFrom += jvxDataFormat_size[paramsOnStart.format] * inProcessing.numSkipped;
			memcpy(ptrTo, ptrFrom, jvxDataFormat_size[paramsOnStart.format] * numCpy);
		}
	}
	inProcessing.writeOffset += numCpy;
	inProcessing.numSkipped -= numToSkip;
	inProcessing.numSkipped = JVX_MAX(inProcessing.numSkipped, 0);

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxExtOutputChannelHandler::prepare(jvxBool is_input, jvxSize register_id, jvxSize framesize, jvxDataFormat format)
{
	paramsOnStart.framesize = framesize;
	paramsOnStart.format = format;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxExtOutputChannelHandler::start(jvxBool is_input, jvxSize register_id)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxExtOutputChannelHandler::stop(jvxBool is_input, jvxSize register_id)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxExtOutputChannelHandler::postprocess(jvxBool is_input, jvxSize register_id)
{
	return(JVX_NO_ERROR);
}
