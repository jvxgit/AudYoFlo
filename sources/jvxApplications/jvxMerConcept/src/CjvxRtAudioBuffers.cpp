#include "CjvxRtAudioBuffers.h"

CjvxRtAudioBuffers::CjvxRtAudioBuffers()
{
	JVX_INITIALIZE_MUTEX(safeAccessEntries);
	theState = JVX_STATE_INIT;

	config.numberChannels = 0;
	config.rate = 0;

	config.sessionName = ".session";
	config.sessionName_tmp = ".session.tmp";
	config.loop = true;
	runtime.entryWrite = NULL;
	runtime.entryRead = NULL;
	runtime.entryDraw = NULL;
	runtime.idSelect = -1;
	runtime.offlineMode = false;
}

CjvxRtAudioBuffers::~CjvxRtAudioBuffers()
{
	jvxSize i;
	std::vector<CjvxOneAudioBuffer*>::iterator elm = entries.begin();

	for(; elm != entries.end(); elm++)
	{
		delete(*elm);
	}
	entries.clear();
	JVX_TERMINATE_MUTEX(safeAccessEntries);
}

jvxErrorType 
CjvxRtAudioBuffers::configure_type(jvxSize numberChannels, jvxInt32 rate)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_INIT)
	{
		config.numberChannels = numberChannels;
		config.rate = rate;
		theState = JVX_STATE_ACTIVE;
		res =JVX_NO_ERROR;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::configure_session(const char* session_name, const char* session_name_tmp)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		config.sessionName = session_name;
		config.sessionName_tmp = session_name_tmp;
		res = JVX_NO_ERROR;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}
	
jvxErrorType 
CjvxRtAudioBuffers::number_entries(jvxSize& num)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		num = this->entries.size();
		unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::selection(jvxInt32& id)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		id = runtime.idSelect;
		unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::set_selection(jvxInt32 id)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(id < entries.size())
		{
			runtime.idSelect = id;
		}
		unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::tag_entry(jvxSize id, std::string& tag)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(id < entries.size())
		{
			tag = entries[id]->runtime.tag;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::edit_tag_entry(jvxInt32 id, std::string tag)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(id < 0)
		{
			id = runtime.idSelect;
		}
		if(id < entries.size())
		{
			entries[id]->runtime.tag = tag;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

/*
jvxErrorType 
CjvxRtAudioBuffers::import_file(const char* fName)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::export_to_file(const char* fName, jvxSize idEntry)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
	}
	return(res);
}
*/

jvxErrorType 
CjvxRtAudioBuffers::fill_view_buffer(jvxData** buffersY, jvxData** buffersX, jvxSize bLength)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData tmp;
	if(theState == JVX_STATE_ACTIVE)
	{
		this->lock();
		if((runtime.idSelect >= 0) && (runtime.idSelect < entries.size()))
		{
			if(runtime.entryDraw == NULL)
			{
				runtime.entryDraw = entries[runtime.idSelect];
			}
		}
		this->unlock();

		if(runtime.entryDraw)
		{
			runtime.entryDraw->fill_view_buffer(buffersY, buffersX, bLength, -1);
		}
		this->lock();
		runtime.entryDraw = NULL;
		this->unlock();
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::fill_view_buffer_online(jvxData** buffersY, jvxData** buffersX, jvxSize bLength, jvxInt32 maxNumberSamplesShow)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		this->lock();
		if(runtime.entryDraw == NULL)
		{
			runtime.entryDraw = runtime.entryWrite;
		}
		this->unlock();

		if(runtime.entryDraw)
		{
			runtime.entryDraw->fill_view_buffer(buffersY,buffersX, bLength, maxNumberSamplesShow);
			this->lock();
			runtime.entryDraw = NULL;
			this->unlock();
		}
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::prepare_read()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		this->lock();
		if(this->runtime.entryRead == NULL)
		{
			if(runtime.idSelect < entries.size())
			{
				runtime.entryRead = entries[runtime.idSelect];
				if(runtime.offlineMode)
				{
					runtime.entryRead->prepare_read(NULL);
				}
				else
				{
					runtime.entryRead->prepare_read(&config.loop);
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		this->unlock();
	}
	return(res);
}
	
jvxErrorType 
CjvxRtAudioBuffers::read_frame(jvxData** data, jvxSize number_samples)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num;
	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(runtime.entryRead)
		{
			res = runtime.entryRead->get_samples_ni(data, number_samples, true, num);
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		this->unlock();
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::read_frame_position(jvxData** data, jvxSize number_samples, jvxData progress)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::postprocess_read()
{
jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(runtime.entryRead)
		{
			res = runtime.entryRead->postprocess_read();
			runtime.entryRead = NULL;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		this->unlock();
	}
	return(res);
}

// ===========================================================================================

jvxErrorType
CjvxRtAudioBuffers::prepare_write(jvxSize numChannels, jvxInt32 sRate, jvxData segLength_sec, const char* tag)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		this->lock();
		if(this->runtime.entryWrite == NULL)
		{
			this->runtime.entryWrite = new CjvxOneAudioBuffer();
			this->runtime.entryWrite->configure(
				numChannels, sRate, segLength_sec,
				"Recording");
			if(runtime.offlineMode)
			{
				this->runtime.entryWrite->prepare_write(false);
			}
			else
			{
				this->runtime.entryWrite->prepare_write(true);
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		this->unlock();
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::write_frame(jvxData** data, jvxSize number_samples)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		this->lock();
		if(this->runtime.entryWrite)
		{
			res = this->runtime.entryWrite->add_samples_ni(data, number_samples);
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		this->unlock();
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::postprocess_write()
{
jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(runtime.entryWrite)
		{
			res = runtime.entryWrite->postprocess_write();
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		this->unlock();
	}
	return(res);
}

// ========================================================================

jvxErrorType 
CjvxRtAudioBuffers::save_session(IjvxConfigProcessor* proc)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
	}
	return(res);
}
	
jvxErrorType 
CjvxRtAudioBuffers::open_session(IjvxConfigProcessor* proc)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::addEntry(CjvxOneAudioBuffer* newBuffer)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		this->lock();
		this->entries.push_back(newBuffer);
		runtime.idSelect = this->entries.size() - 1;
		/*
		if((runtime.idSelect < 0) || (runtime.idSelect >= this->entries.size()))
		{
			runtime.idSelect = 0;
		}*/
		this->unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::addRecordedEntry()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		this->lock();
		if(runtime.entryWrite)
		{
			this->entries.push_back(runtime.entryWrite);
			runtime.entryWrite = NULL;
		}
		runtime.idSelect = this->entries.size() - 1;
		/*
		if((runtime.idSelect < 0) || (runtime.idSelect >= this->entries.size()))
		{
			runtime.idSelect = 0;
		}*/
		this->unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::entry_type(jvxSize id, CjvxOneAudioBuffer::jvxAudioBuffertype& tp)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(id < entries.size())
		{
			tp = entries[id]->runtime.type;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::entry_description(jvxSize id, std::string& description)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(id < entries.size())
		{
			description = entries[id]->runtime.tag;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::entry_filename(jvxSize id, std::string& filename)
{
		jvxErrorType res = JVX_NO_ERROR;

	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(id < entries.size())
		{
			filename = entries[id]->runtime.fName;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxRtAudioBuffers::entry_length_seconds(jvxSize id, jvxData& valD)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(id < entries.size())
		{			
			jvxSize lSamples = 0;
			lSamples = 0;
			entries[id]->length_samples(lSamples);
			valD = (jvxData)lSamples / (jvxData)entries[id]->runtime.sRate;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		unlock();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxRtAudioBuffers::borrowActiveEntry(CjvxOneAudioBuffer** theBuf)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		lock();
		if(runtime.idSelect < entries.size())
		{			
			if(theBuf)
			{
				*theBuf = entries[runtime.idSelect];
			}
		}
		else
		{
			unlock();
			res = JVX_ERROR_INVALID_SETTING;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxRtAudioBuffers::returnActiveEntry(CjvxOneAudioBuffer* theBuf)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_ACTIVE)
	{
		if(theBuf == entries[runtime.idSelect])
		{
			unlock();			
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}