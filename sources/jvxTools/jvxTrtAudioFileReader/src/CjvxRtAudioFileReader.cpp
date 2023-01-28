#include "CjvxRtAudioFileReader.h"

CjvxRtAudioFileReader::CjvxRtAudioFileReader(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_RT_AUDIO_FILE_READER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxRtAudioFileReader*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

CjvxRtAudioFileReader::~CjvxRtAudioFileReader()
{
}

jvxErrorType 
CjvxRtAudioFileReader::activate(const char* fName, jvxEndpointClass descr, jvxFileDescriptionEndpoint_open* fileDescr)
{
	jvxErrorType res = _activate();
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileReader::activate(fName, descr, fileDescr);
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::get_tag(jvxAudioFileTagType tp, jvxApiString* val)
{
	std::string txt;
	jvxErrorType res = JVX_NO_ERROR;
	res = jvxFileReader::get_tag(tp, txt);
	if(res == JVX_NO_ERROR)
	{
		if (val)
		{
			val->assign(txt);
		}
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::deactivate()
{
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileReader::deactivate();
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::properties_file(jvxSize* numChannels, jvxInt32* samplerate, 
		jvxFileFormat* fformat_file, jvxEndpointClass* endpoint_description, 
		jvxSize* length_file, jvxSize* numBitsSample, jvxSize* subformat)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = jvxFileReader::get_file_properties(numChannels, length_file, samplerate,  fformat_file, NULL, endpoint_description, numBitsSample, subformat);
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::properties_processing(
		jvxSize* max_number_read_file, jvxSize* bsize_intern,
		jvxDataFormat* format_proc, jvxFileContinueType* continue_type)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = jvxFileReader::get_processing_properties(bsize_intern, max_number_read_file, format_proc, continue_type);
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::prepare(jvxInt32 internalBuffersize, jvxFileContinueType contTp, jvxBool boostPriority, jvxFileDescriptionEndpoint_prepare* fileDescr, jvxFileOperationType mode)
{
	jvxErrorType res = _prepare();
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileReader::prepare(internalBuffersize, contTp, boostPriority, fileDescr, mode);
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::restart(jvxInt32 timeout)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = jvxFileReader::restartPlayback(timeout);
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::postprocess()
{
	jvxErrorType res = _postprocess();
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileReader::postprocess();
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::get_audio_buffer(jvxHandle** buffers, jvxSize numberChannels, 
		jvxSize numberSamples, jvxBool strictlyNoBlocking, jvxData* progress)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = jvxFileReader::audio_buffer(buffers, numberChannels, numberSamples, strictlyNoBlocking, progress);
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::progress(jvxSize* myprogress)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = jvxFileReader::progress(myprogress);
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::initialize(IjvxHiddenInterface* hostRef)
{
	jvxErrorType res = _initialize(hostRef);
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileReader::initialize();
		res = _select();
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::terminate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxRtAudioFileReader::stop();
	res = CjvxRtAudioFileReader::postprocess();
	res = CjvxRtAudioFileReader::deactivate();

	res = jvxFileReader::terminate();
	res = _unselect();
	_terminate();
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::start()
{
	jvxErrorType res = _start();
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileReader::start();
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::stop()
{
	jvxErrorType res = _stop();
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileReader::stop();
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::performance(jvxSize* num_elms_to_mod, jvxSize* num_elms_to_file, jvxSize* num_elms_failed, jvxSize* num_elms_min)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = jvxFileReader::evaluate(num_elms_to_mod, num_elms_to_file, num_elms_failed, num_elms_min);
	return(res);
}


jvxErrorType 
CjvxRtAudioFileReader::wind_forward(jvxSize timeSteps)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = jvxFileReader::wind_forward(timeSteps);
	return(res);
}

jvxErrorType 
CjvxRtAudioFileReader::wind_backward(jvxSize timeSteps)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = jvxFileReader::wind_backward(timeSteps);
	return(res);
}
