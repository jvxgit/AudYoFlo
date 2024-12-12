#include "CjvxRtAudioFileWriter.h"

CjvxRtAudioFileWriter::CjvxRtAudioFileWriter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_RT_AUDIO_FILE_WRITER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxRtAudioFileWriter*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

CjvxRtAudioFileWriter::~CjvxRtAudioFileWriter()
{
}

jvxErrorType
CjvxRtAudioFileWriter::activate(const char* fName, jvxEndpointClass descr,
						jvxFileDescriptionEndpoint_open* fileDescr, jvxBool allowOverwriteExisting)
{
	jvxErrorType res = _activate();
	if(res == JVX_NO_ERROR)
	{
		if (!allowOverwriteExisting)
		{
			if (JVX_FILE_EXISTS(fName) == c_true)
			{
				_deactivate();
				return JVX_ERROR_NO_ACCESS;
			}
		}

		res = _common_set.theToolsHost->instance_tool(JVX_COMPONENT_THREADS, &threads.theObj, 0, NULL);
		if ((res == JVX_NO_ERROR) && threads.theObj)
		{
			threads.theObj->request_specialization(reinterpret_cast<jvxHandle**>(&threads.theHdl), NULL, NULL);
		}

		res = jvxFileWriter::activate(fName, descr, fileDescr, threads.theHdl);
		if (res != JVX_NO_ERROR)
		{
			// If activation failed, set module into original state
			_deactivate();
		}
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::add_tag(jvxAudioFileTagType tp, const char* val)
{
	std::string txt = val;
	jvxErrorType res = JVX_NO_ERROR;
	res = jvxFileWriter::add_tag(tp, txt.c_str());
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::deactivate()
{
	jvxErrorType res = _pre_check_deactivate();
	if(res == JVX_NO_ERROR)
	{
		if (threads.theObj)
		{
			res = _common_set.theToolsHost->return_instance_tool(JVX_COMPONENT_THREADS, threads.theObj, 0, NULL);
			threads.theObj = nullptr;
			threads.theHdl = nullptr;
		}
		_deactivate();
		jvxFileWriter::deactivate();
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::prepare(jvxSize internalBuffersize, jvxBool boostPriority, jvxFileDescriptionEndpoint_prepare* fileDescr, jvxFileOperationType mode)
{
	jvxErrorType res = _prepare();
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileWriter::prepare(internalBuffersize, boostPriority, fileDescr, mode);
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::add_audio_buffer(void** buffers, jvxSize numberChannels, jvxSize numberSamples, jvxBool strictlyNoBlocking, jvxData* bFillHeight_scale)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		res = jvxFileWriter::add_buffer(buffers, numberChannels, numberSamples, strictlyNoBlocking, bFillHeight_scale);
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::postprocess()
{
	jvxErrorType res = _postprocess();
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileWriter::postprocess();
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::initialize(IjvxHiddenInterface* theHost)
{
	jvxErrorType res = _initialize(theHost);
	if(res == JVX_NO_ERROR)
	{
		_select();
		res = jvxFileWriter::initialize();
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::terminate()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		this->stop();
	}
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		this->postprocess();
	}
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		this->deactivate();
	}

	res = jvxFileWriter::terminate();
	assert(res == JVX_NO_ERROR);
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::start()
{
	jvxErrorType res = _start();
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileWriter::start();
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::stop()
{
	jvxErrorType res = _stop();
	if(res == JVX_NO_ERROR)
	{
		res = jvxFileWriter::stop();
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::performance(jvxSize* num_elms_to_mod, jvxSize* num_elms_to_file, jvxSize* num_elms_failed, jvxSize* num_elms_max)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		res = jvxFileWriter::evaluate(num_elms_to_mod, num_elms_to_file,num_elms_failed, num_elms_max);
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::get_properties_file(jvxSize* numChannels, jvxInt32* samplerate,
		jvxFileFormat* fformat_file, jvxEndpointClass* endpoint_description, jvxSize* numBitsSample, jvxSize* subtformat)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{

		// In state ACTIVE and HIGHER
		res = jvxFileWriter::get_file_properties(numChannels, samplerate, fformat_file, endpoint_description, numBitsSample, subtformat);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::set_properties_file(jvxSize* numChannels, jvxInt32* samplerate,
		jvxFileFormat* fformat_file, jvxEndpointClass* endpoint_description, jvxSize* numBitsSample, jvxSize* subtype)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{

		// In state ACTIVE and HIGHER
		res = jvxFileWriter::set_file_properties(numChannels, samplerate, fformat_file, endpoint_description, numBitsSample, subtype);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::get_properties_processing(
		jvxSize* max_number_read_file, jvxSize* bsize_intern,
		jvxDataFormat* format_proc)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState >= JVX_STATE_PREPARED)
	{
		res = jvxFileWriter::get_processing_properties(bsize_intern, max_number_read_file,
			format_proc);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxRtAudioFileWriter::set_properties_processing(
		jvxSize* max_number_read_file, jvxSize* bsize_intern,
		jvxDataFormat* format_proc)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState >= JVX_STATE_PREPARED)
	{
		res = jvxFileWriter::set_processing_properties(bsize_intern, max_number_read_file,
			format_proc);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}
