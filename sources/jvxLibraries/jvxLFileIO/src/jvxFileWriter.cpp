
#include <iostream>
#include "jvxFileWriter.h"
#include "jvxWavWriter.h"

// Timeout to wait for module to terminate at end of processing
#define WAIT_FOR_CLOSE_MS 2000

/*
 * C function to enter the background thread. Functionality:
 * Return into class name space and loop
 *///========================================================
JVX_THREAD_ENTRY_FUNCTION(writeBufferCallbackThrd, param)
{
	if(param)
	{
		jvxFileWriter* ptrClass = (jvxFileWriter*) param;
		return ptrClass->enterClassCallback();
	}
	return 0;
}

/**
 * Constructor: Set all values to initial
 *///===========================================================
jvxFileWriter::jvxFileWriter()
{
	JVX_INITIALIZE_MUTEX(protect.accessFileIo);
	JVX_INITIALIZE_MUTEX(protect.accessCircBuffer);
	JVX_INITIALIZE_NOTIFICATION(thread.writeMoreSamples);
	JVX_INITIALIZE_NOTIFICATION(thread.started);

	reset();
};

void
jvxFileWriter::reset()
{
	buffer_realtime.intBuffer = NULL;
	buffer_realtime.writeBuffer = NULL;
	buffer_realtime.formatAppl = JVX_DATAFORMAT_NONE;
	buffer_realtime.szElement = 0;
	buffer_realtime.numChannels = 0;
	buffer_realtime.framesizeChunks = 0;
	buffer_realtime.idxRead = 0;
	buffer_realtime.fHeight = 0;
	buffer_realtime.length = 0;
	buffer_realtime.positionRead = 0;

#ifdef JVX_OS_WINDOWS
	thread.hdl = JVX_INVALID_HANDLE_VALUE;
#endif

	thread.running = false;

	operation.mode = JVX_FILEOPERATION_NONE;
	operation.fileEnded = false;
	operation.overrun_occurred = false;

	hdlWriter = NULL;
	theThread = nullptr;
}

jvxErrorType
jvxFileWriter::initialize()
{
	return JVX_NO_ERROR;
}

/**
 * Destructor: Destroy critical section and all event handles.
 *///==============================================================
jvxFileWriter::~jvxFileWriter()
{
	this->postprocess();
	this->deactivate();
	this->terminate();

	JVX_TERMINATE_MUTEX(protect.accessFileIo);
	JVX_TERMINATE_MUTEX(protect.accessCircBuffer);
	JVX_TERMINATE_NOTIFICATION(thread.writeMoreSamples);
	JVX_TERMINATE_NOTIFICATION(thread.started);
}

jvxErrorType
jvxFileWriter::activate(
	std::string fName, jvxEndpointClass descrEndpoint, 
	jvxFileDescriptionEndpoint_open* fileDescr,
	IjvxThreads* threadHdl)
{
		jvxErrorType res = JVX_NO_ERROR;
	int errCode = 0;
	// EVENT AND CRIT SECTION ARE SETUP ALREADY
	theThread = threadHdl;

	switch(descrEndpoint)
	{
	case JVX_FILETYPE_WAV:
		// Try to open file as wav file
		hdlWriter = static_cast<IjvxAudioWriter*>(new jvxWavWriter);
		res = hdlWriter->select(fName);

		if(res == JVX_NO_ERROR)
		{
			res = hdlWriter->activate(errCode);
			res = hdlWriter->set_file_properties(
				&fileDescr->numberChannels,
				&fileDescr->samplerate,
				&fileDescr->fFormatFile,
				&fileDescr->numberBitsSample,
				&fileDescr->subtype);
		}
		else
		{
			std::cout << "Opening output wav file " << fName << " failed, error code = " << errCode << std::endl;
		}

		break;

	case JVX_FILETYPE_RAW:
		res = JVX_ERROR_UNSUPPORTED;
		break;
	}

	return res;
}

jvxErrorType
jvxFileWriter::add_tag(jvxAudioFileTagType tp, std::string name)
{
	if(hdlWriter)
	{
		return hdlWriter->add_tag_text(tp, name);
	}
	return JVX_ERROR_WRONG_STATE;
}

/**
 * Prepare file for processing, allocate buffer and start the background thread.
 * Return value: 0: success, -1: Function called in wrong state, -2: Application datatype not supported, -3: File could not be opened
 *///===================================================================
jvxErrorType
jvxFileWriter::prepare(jvxSize internalBuffersize, jvxBool boostPriority,
		jvxFileDescriptionEndpoint_prepare* fileDescr,
		jvxFileOperationType mode)
{
	jvxSize i;
	jvxSize chans = 0;
	jvxInt32 srate = 0;
	jvxErrorType res = JVX_NO_ERROR;

	if(hdlWriter)
	{
		operation.fileEnded = false;
		operation.mode = mode;
		operation.overrun_occurred = false;
		operation.boostPriority = boostPriority;

		res = hdlWriter->get_file_properties(&chans, NULL, NULL, NULL, NULL, NULL, NULL);

		eval.min_space_on_enter = 0;
		eval.num_samples_in = 0;
		eval.num_samples_out = 0;
		eval.num_samples_lost = 0;

		if(res == JVX_NO_ERROR)
		{

			switch(operation.mode)
			{
			case JVX_FILEOPERATION_REALTIME:

				// Start reader module
				res = hdlWriter->prepare(fileDescr->numSamplesFrameMax_moveBgrd);

				buffer_realtime.fHeight = 0;
				buffer_realtime.length = internalBuffersize;
				buffer_realtime.formatAppl = fileDescr->dFormatAppl;
				buffer_realtime.framesizeChunks = fileDescr->numSamplesFrameMax_moveBgrd;
				buffer_realtime.idxRead = 0;
				buffer_realtime.intBuffer = NULL;

				eval.min_space_on_enter = buffer_realtime.length;

				buffer_realtime.numChannels = chans;
				buffer_realtime.szElement = jvxDataFormat_size[buffer_realtime.formatAppl];

				JVX_SAFE_NEW_FLD(buffer_realtime.writeBuffer, jvxByte*, buffer_realtime.numChannels);
				JVX_SAFE_NEW_FLD(buffer_realtime.intBuffer, jvxByte*, buffer_realtime.numChannels);
				for(i = 0; i < buffer_realtime.numChannels; i++)
				{
					JVX_SAFE_NEW_FLD(buffer_realtime.intBuffer[i], jvxByte, sizeof(jvxByte) * buffer_realtime.szElement * buffer_realtime.length);
					memset(buffer_realtime.intBuffer[i], 0, sizeof(jvxByte) * buffer_realtime.szElement * buffer_realtime.length);
				}

				break;
			case JVX_FILEOPERATION_BATCH:
				// Nothing needs to be done here
				// Start reader module
				buffer_realtime.formatAppl = fileDescr->dFormatAppl;
				res = hdlWriter->prepare(internalBuffersize);
				break;

			}
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

/**
 * Function to add a data bufer to the internal buffer.
 * Return value: 0: success, -1: Function called in wrong state, -2: No space left in buffer
 *///===================================================================
jvxErrorType
jvxFileWriter::add_buffer(jvxHandle** buffers, jvxSize numberChannels, jvxSize numberSamples, jvxBool strictlyNoBlocking, jvxData* bFillHeight_scale)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idxWrite = 0;
	jvxSize space = 0;
	jvxSize samplesToGo = 0;
	jvxSize samplesGone = 0;
	jvxSize samplesSet = 0;
	jvxSize prog = 0;
	jvxSize fHeight = 0;
	if(hdlWriter)
	{
		switch(operation.mode)
		{
		case JVX_FILEOPERATION_BATCH:

			// The read buffer will always be complete since it fills up with zeros if file is too short
			res = hdlWriter->write_one_buf(buffers, numberChannels, numberSamples, buffer_realtime.formatAppl);
			if (bFillHeight_scale)
				*bFillHeight_scale = 0;
			break;
		case JVX_FILEOPERATION_REALTIME:

			// Set this to return good results also in error case
			fHeight = buffer_realtime.fHeight;
			if(operation.overrun_occurred)
			{
				// If underrun occurred before, avoid taking out samples until thebuffer is full
				res = JVX_ERROR_POSTPONE;
				eval.num_samples_lost += numberSamples;
			}
			else
			{
				eval.min_space_on_enter = JVX_MIN(eval.min_space_on_enter, buffer_realtime.length - buffer_realtime.fHeight);

				samplesToGo = numberSamples;

				while(samplesToGo)
				{
					space = buffer_realtime.length - buffer_realtime.fHeight;
					samplesSet = JVX_MIN(space, samplesToGo);
					if(samplesSet > 0)
					{
						JVX_LOCK_MUTEX(protect.accessCircBuffer);
						idxWrite = (buffer_realtime.idxRead + buffer_realtime.fHeight) % buffer_realtime.length;
						JVX_UNLOCK_MUTEX(protect.accessCircBuffer);

						jvxSize cpChannels = JVX_MIN(numberChannels, buffer_realtime.numChannels);
						jvxSize ll1 = JVX_MIN(buffer_realtime.length - idxWrite, samplesSet);
						assert(ll1 > 0);

						for(i = 0; i < cpChannels; i++)
						{							
							memcpy((jvxByte*)buffer_realtime.intBuffer[i] + (buffer_realtime.szElement * idxWrite * sizeof(jvxByte)),
								(jvxByte*)buffers[i] + (samplesGone * buffer_realtime.szElement *sizeof(jvxByte)),
								buffer_realtime.szElement * ll1* sizeof(jvxByte));
						}

						samplesToGo -= ll1;
						eval.num_samples_out += ll1;
						samplesGone += ll1;

						JVX_LOCK_MUTEX(protect.accessCircBuffer);
						buffer_realtime.fHeight += ll1;
						fHeight = buffer_realtime.fHeight;
						JVX_UNLOCK_MUTEX(protect.accessCircBuffer);
					}
					else
					{
						res = JVX_ERROR_BUFFER_OVERFLOW;
						operation.overrun_occurred = true;
						eval.num_samples_lost += samplesToGo;
						break;
					}
				} // end of while

			}

			if (theThread)
			{
				theThread->trigger_wakeup();
			}
			else
			{
				// Trigger to load nex file content
				JVX_SET_NOTIFICATION(thread.writeMoreSamples);
			}
			if (bFillHeight_scale)
				*bFillHeight_scale = (jvxData)fHeight/(jvxData)buffer_realtime.length;

			break;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}



/**
 * Function to really stop processing. Background thread is terminated,
 * buffers are deallocated, and all variables are set to initial states.
 * Return value: 0: success, -1: Function called in wrong state
 *///===================================================================
jvxErrorType
jvxFileWriter::postprocess()
{
		int i;
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlWriter)
	{
		switch(operation.mode)
		{
		case JVX_FILEOPERATION_BATCH:
			break;
		case JVX_FILEOPERATION_REALTIME:

			for(i = 0; i < buffer_realtime.numChannels; i++)
			{
				JVX_SAFE_DELETE_FLD(buffer_realtime.intBuffer[i], jvxByte);
			}

			JVX_SAFE_DELETE_FLD(buffer_realtime.writeBuffer, jvxByte*);
			JVX_SAFE_DELETE_FLD(buffer_realtime.intBuffer, jvxByte*);

			buffer_realtime.writeBuffer = NULL;
			buffer_realtime.intBuffer = NULL;
			break;
		default:
			res = JVX_ERROR_WRONG_STATE;
			break;
		}
		hdlWriter->postprocess();
		operation.mode = JVX_FILEOPERATION_NONE;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxErrorType
jvxFileWriter::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;

	if (hdlWriter)
	{
		res = hdlWriter->deactivate();
		res = hdlWriter->unselect();
		delete(hdlWriter);
	}

	hdlWriter = NULL;
	theThread = nullptr;
	return res;
}


/**
 * Class internal thread callback function
 *///====================================================
JVX_THREAD_RETURN_TYPE jvxFileWriter::enterClassCallback()
{
	JVX_WAIT_RESULT resW;
	jvxErrorType res = JVX_NO_ERROR;
	while(thread.running)
	{
		JVX_WAIT_FOR_NOTIFICATION_I(thread.writeMoreSamples);
		JVX_SET_NOTIFICATION(thread.started);

		//result = WaitForSingleObject(bControl.eventReportNewData, JVX_INFINITE);
		resW = JVX_WAIT_FOR_NOTIFICATION_II_INF(thread.writeMoreSamples);
		//if(result == WAIT_OBJECT_0)
		bool retriggered = false;

		do
		{
			if(resW == JVX_WAIT_SUCCESS)
			{
				jvxBool want_to_continue = true;
				while (want_to_continue)
				{
					want_to_continue = core_write_function();
				}
			}
			else
			{
				assert(0);
			}
			if (thread.running)
			{
				retriggered = JVX_WAIT_FOR_CHECK_RETRIGGER_NOTIFICATION_I(thread.writeMoreSamples);
				if (retriggered)
				{
					// Change this from a timeout event to a signalled event if necessary
					resW = JVX_WAIT_SUCCESS;
				}
			}
			else
			{
				retriggered = false;
			}
		} while (retriggered);// This is a shortcut to not fall into the wait if signal was retriggered in the mean time - linux only		    
	}
	return 0;
}


jvxErrorType
jvxFileWriter::read_from_circ_buffer(jvxSize& samplesRead, jvxSize idxRead)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvxSize writeNow = JVX_MIN(buffer_realtime.length - idxRead, samplesRead);
	for(i = 0; i < buffer_realtime.numChannels; i++)
	{
		buffer_realtime.writeBuffer[i] = buffer_realtime.intBuffer[i] + (idxRead * sizeof(jvxByte) * buffer_realtime.szElement);
	}

	JVX_LOCK_MUTEX(protect.accessFileIo);
	res = hdlWriter->write_one_buf((jvxHandle**)buffer_realtime.writeBuffer, buffer_realtime.numChannels, writeNow, buffer_realtime.formatAppl);
	JVX_UNLOCK_MUTEX(protect.accessFileIo);
	return res;
}


jvxErrorType
jvxFileWriter::terminate()
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxFileWriter::start()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlWriter)
	{
		res = hdlWriter->start();

		switch(operation.mode)
		{
		case JVX_FILEOPERATION_BATCH:
			break;
		case JVX_FILEOPERATION_REALTIME:

			if (theThread)
			{
				theThread->initialize(this);
				theThread->start(JVX_SIZE_UNSELECTED, operation.boostPriority);
			}
			else
			{
				//bControl.threadHandle = CreateThread(NULL, 0, writeBufferCallbackThrd, (void*)this, 0, NULL);
				JVX_THREAD_ID idT;

				thread.running = true;

				JVX_WAIT_FOR_NOTIFICATION_I(thread.started);

				JVX_CREATE_THREAD(thread.hdl, writeBufferCallbackThrd, (void*)this, idT);

				JVX_WAIT_FOR_NOTIFICATION_II_INF(thread.started);

				if (operation.boostPriority)
				{
					//SetThreadPriority(bControl.threadHandle, THREAD_PRIORITY_TIME_CRITICAL);
					JVX_SET_THREAD_PRIORITY(thread.hdl, JVX_THREAD_PRIORITY_REALTIME);
				}
			}
			break;
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileWriter::stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlWriter)
	{
		switch(operation.mode)
		{
		case JVX_FILEOPERATION_BATCH:
			break;
		case JVX_FILEOPERATION_REALTIME:

			if (theThread)
			{
				theThread->stop();
				theThread->terminate();
			}
			else
			{
				thread.running = false;

				JVX_SET_NOTIFICATION(thread.writeMoreSamples);
				JVX_WAIT_FOR_THREAD_TERMINATE_MS(thread.hdl, WAIT_FOR_CLOSE_MS);
			}

			// Initial fill of bufer
			while(1)
			{
				jvxSize readMax = buffer_realtime.fHeight;
				if(readMax > 0)
				{
					this->read_from_circ_buffer(readMax, buffer_realtime.idxRead);
					buffer_realtime.fHeight -= readMax;
				}
				else
				{
					break;
				}
			}

			break;
		}
		res = hdlWriter->stop();
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileWriter::set_file_properties(jvxSize* channels, jvxInt32* srate, jvxFileFormat* fformat, jvxEndpointClass* descr, jvxSize* numBitsSample, jvxSize* extended)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlWriter)
	{
		res = hdlWriter->set_file_properties(channels, srate, fformat, numBitsSample, extended);
		if(descr)
		{
			*descr = JVX_FILETYPE_WAV;
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileWriter::get_file_properties(jvxSize* channels, jvxInt32* srate, jvxFileFormat* fformat, jvxEndpointClass* descr, jvxSize* numBitsSample, jvxSize* extended)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlWriter)
	{
        jvxSize ll = 0;
        jvxBool littleEndian = false;
		res = hdlWriter->get_file_properties(channels, &ll, srate, fformat, &littleEndian, numBitsSample, extended);

		if(descr)
		{
			*descr = JVX_FILETYPE_WAV;
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileWriter::set_processing_properties(jvxSize* int_buffersize, jvxSize* int_bsize_move_file_max, jvxDataFormat* dformat_appl)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;

	if(hdlWriter)
	{
		hdlWriter->status(&stat);
		if(stat >= JVX_STATE_PREPARED)
		{
			if(int_buffersize)
			{
				*int_buffersize = 0;
			}
			if(int_bsize_move_file_max)
			{
				*int_bsize_move_file_max = 0;
			}
			if(dformat_appl)
			{
				*dformat_appl = JVX_DATAFORMAT_NONE;
			}			
			return res;
		}
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileWriter::get_processing_properties(jvxSize* int_buffersize, jvxSize* int_bsize_move_file_max, jvxDataFormat* format_appl)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;

	if(hdlWriter)
	{
		hdlWriter->status(&stat);
		if(stat >= JVX_STATE_PREPARED)
		{
			if(int_buffersize)
			{
				*int_buffersize = 0;
			}
			if(int_bsize_move_file_max)
			{
				*int_bsize_move_file_max = 0;
			}
			if(format_appl)
			{
				*format_appl = JVX_DATAFORMAT_NONE;
			}
			return res;
		}
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileWriter::evaluate(jvxSize* num_samples_in, jvxSize* num_samples_out, jvxSize* num_samples_failed, jvxSize* num_elms_fheight_min)
{
	if(num_samples_in)
	{
		*num_samples_in = eval.num_samples_in;
	}
	if(num_samples_out)
	{
		*num_samples_out = eval.num_samples_out;
	}
	if(num_samples_failed)
	{
		*num_samples_failed = eval.num_samples_lost;
	}
	if(num_elms_fheight_min)
	{
		*num_elms_fheight_min = eval.min_space_on_enter;
		eval.min_space_on_enter = buffer_realtime.length;
	}
	return JVX_NO_ERROR;
}

jvxBool
jvxFileWriter::core_write_function()
{
	jvxSize samplesRead = 0;

	// Set this value as the number of elements to be copied
	JVX_LOCK_MUTEX(protect.accessCircBuffer);
	//idxWrite = (buffer_realtime.idxRead + buffer_realtime.fHeight) % buffer_realtime.length;
	samplesRead = buffer_realtime.fHeight;
	JVX_UNLOCK_MUTEX(protect.accessCircBuffer);

	samplesRead = JVX_MIN(buffer_realtime.framesizeChunks, samplesRead);
	if (samplesRead == 0)
	{
		// Release module to accept further input
		operation.overrun_occurred = false;
		return false;
	}

	jvxErrorType res = read_from_circ_buffer(samplesRead, buffer_realtime.idxRead);

	if (res == JVX_ERROR_END_OF_FILE)
	{
		operation.fileEnded = true;
	}
	else
	{
		JVX_LOCK_MUTEX(protect.accessCircBuffer);
		buffer_realtime.fHeight -= samplesRead;
		buffer_realtime.idxRead = (buffer_realtime.idxRead + samplesRead) % buffer_realtime.length;
		JVX_UNLOCK_MUTEX(protect.accessCircBuffer);
	}
	return true;
}

jvxErrorType 
jvxFileWriter::startup(jvxInt64 timestamp_us) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
jvxFileWriter::expired(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
jvxFileWriter::wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	jvxBool want_to_continue = true;
	while (want_to_continue)
	{
		want_to_continue = core_write_function();
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
jvxFileWriter::stopped(jvxInt64 timestamp_us) 
{
	return JVX_NO_ERROR;
}
