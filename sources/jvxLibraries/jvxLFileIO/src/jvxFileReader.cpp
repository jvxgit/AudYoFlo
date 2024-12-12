#include <iostream>

#include "jvxFileReader.h"
#include "jvxWavReader.h"

// Timeout to wait for module to terminate at end of processing
#define WAIT_FOR_CLOSE_MS 2000
#define ACCEPT_ONLY_WAV

#define WIND_TIME_BASE_MS 1000

static void jvx_generate_sine(jvxData& phase, jvxData a, jvxData f, jvxData fs, jvxData* buf, jvxSize ll)
{
	jvxSize i;
	jvxData phaseIncrement = 2 * M_PI*f / fs;
	for (i = 0; i < ll; i++)
	{
		buf[i] = a * cos(phase);
		phase += phaseIncrement;
	}
	while (phase > 2 * M_PI)
		phase -= 2 * M_PI;
}
 
static jvxData phase = 0.0;

// Use the sndFile library
//#pragma comment ( lib, "sndFile" )

/*
* C function to enter the background thread. Functionality:
* Return into class name space and loop
*///========================================================
//DWORD WINAPI readBufferCallbackThrd(void* param)
JVX_THREAD_ENTRY_FUNCTION(readBufferCallbackThrd, param)
{
	if(param)
	{
		jvxFileReader* ptrClass = (jvxFileReader*) param;
		return ptrClass->enterClassCallback();
	}
	return 0;
}

/**
* Constructor: Set all values to initial
*///===========================================================
jvxFileReader::jvxFileReader()
{
	JVX_INITIALIZE_MUTEX(protect.accessFileIo);
	JVX_INITIALIZE_MUTEX(protect.accessCircBuffer);
	JVX_INITIALIZE_NOTIFICATION(thread.readMoreSamples);
	JVX_INITIALIZE_NOTIFICATION(thread.started);

	reset();

};

void
jvxFileReader::reset()
{
	buffer_realtime.intBuffer = NULL;
	buffer_realtime.readBuffer = NULL;
	buffer_realtime.formatAppl = JVX_DATAFORMAT_NONE;
	buffer_realtime.szElement = 0;
	buffer_realtime.numChannels = 0;
	buffer_realtime.framesizeChunks = 0;
	buffer_realtime.idxRead = 0;
	buffer_realtime.fHeight = 0;
	buffer_realtime.length = 0;
	buffer_realtime.positionRead = 0;

	thread.hdl = JVX_NULLTHREAD;
	thread.running = false;

	operation.mode = JVX_FILEOPERATION_NONE;
	operation.contType = JVX_FILECONTINUE_NONE;
	operation.fileEnded = false;
	operation.underrun_occurred = false;
	operation.progress = 0;
	operation.length_file_sample = 0;

	hdlReader = NULL;
}

/**
* Destructor: Destroy critical section and all event handles.
*///==============================================================
jvxFileReader::~jvxFileReader()
{
	this->postprocess();
	this->deactivate();
	this->terminate();

	JVX_TERMINATE_MUTEX(protect.accessFileIo);
	JVX_TERMINATE_MUTEX(protect.accessCircBuffer);
	JVX_TERMINATE_NOTIFICATION(thread.readMoreSamples);
	JVX_TERMINATE_NOTIFICATION(thread.started);

	hdlReader = NULL;
}

jvxErrorType
jvxFileReader::initialize()
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxFileReader::activate(std::string fName, jvxEndpointClass descrEndpoint, 
	jvxFileDescriptionEndpoint_open* fileDescr, IjvxThreads* threadHdl)
{
	jvxWavReader* hdlReaderW = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	int errCode = 0;

	// EVENT AND CRIT SECTION ARE SETUP ALREADY
	theThread = threadHdl;

	switch(descrEndpoint)
	{
	case JVX_FILETYPE_WAV:
		// Try to open file as wav file
		hdlReader = static_cast<IjvxAudioReader*>(new jvxWavReader);
		res = hdlReader->select(fName);

		if(res == JVX_NO_ERROR)
		{
			res = hdlReader->activate(errCode);
		}

		if(res == JVX_NO_ERROR)
		{
			res = hdlReader->get_file_properties(
				&fileDescr->numberChannels,
				&fileDescr->lengthFile,
				&fileDescr->samplerate,
				&fileDescr->fFormatFile,
				&fileDescr->littleEndian,
				&fileDescr->numberBitsSample,
				&fileDescr->subtype);
		}
		else
		{
			std::cout << "Scanning input wav file failed, error code = " << errCode << std::endl;
		}

		break;

	case JVX_FILETYPE_RAW:
		res = JVX_ERROR_UNSUPPORTED;
		break;
	}

	return res;
}

jvxErrorType
jvxFileReader::get_tag(jvxAudioFileTagType tp, std::string& name)
{
	if(hdlReader)
	{
		return hdlReader->get_tag_text(tp, name);
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileReader::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;

	if(hdlReader)
	{
		res = hdlReader->deactivate();
		res = hdlReader->unselect();
		delete hdlReader;
		hdlReader = NULL;
	}

	theThread = nullptr;

	return res;
}

/**
* Prepare file for processing, allocate buffer and start the background thread.
* Return value: 0: success, -1: Function called in wrong state, -2: File could not be opened
*///===================================================================
jvxErrorType
	jvxFileReader::prepare(jvxSize internalBuffersize, jvxFileContinueType contTp, jvxBool boostPriority,
	jvxFileDescriptionEndpoint_prepare* fileDescr, jvxFileOperationType mode)
{
	jvxSize i;
	jvxSize chans = 0;
	jvxInt32 srate = 0;
	jvxErrorType res = JVX_NO_ERROR;

	if(hdlReader)
	{
		operation.contType = contTp;
		operation.fileEnded = false;
		operation.mode = mode;
		operation.underrun_occurred = false;
		operation.boostPriority = boostPriority;

		// Set to loop mode or non-loop-mode
		if(operation.contType == JVX_FILECONTINUE_LOOP)
		{
			hdlReader->set_loop(true);
		}
		else
		{
			hdlReader->set_loop(false);
		}

		res = hdlReader->get_file_properties(&chans, &operation.length_file_sample, NULL, NULL, NULL, NULL, NULL);

		eval.min_fheight_on_enter = 0;
		eval.num_samples_in = 0;
		eval.num_samples_out = 0;
		eval.num_samples_lost = 0;

		if(res == JVX_NO_ERROR)
		{

			switch(operation.mode)
			{
			case JVX_FILEOPERATION_REALTIME:

				// Start reader module
				res = hdlReader->prepare(fileDescr->numSamplesFrameMax_moveBgrd);

				buffer_realtime.fHeight = 0;
				buffer_realtime.length = internalBuffersize;
				buffer_realtime.formatAppl = fileDescr->dFormatAppl;
				buffer_realtime.framesizeChunks = fileDescr->numSamplesFrameMax_moveBgrd;
				buffer_realtime.idxRead = 0;
				buffer_realtime.intBuffer = NULL;

				eval.min_fheight_on_enter = buffer_realtime.length;

				buffer_realtime.numChannels = chans;
				buffer_realtime.szElement = jvxDataFormat_size[buffer_realtime.formatAppl];

				JVX_SAFE_NEW_FLD(buffer_realtime.readBuffer, jvxByte*, buffer_realtime.numChannels);
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
				res = hdlReader->prepare(internalBuffersize);
				buffer_realtime.formatAppl = fileDescr->dFormatAppl;
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

jvxErrorType
jvxFileReader::reset_bgrd_buffer_lock_start()
{
	if (hdlReader)
	{
		switch (operation.mode)
		{
		case JVX_FILEOPERATION_REALTIME:
			JVX_LOCK_MUTEX(protect.accessCircBuffer);
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_INVALID_SETTING;
}

jvxErrorType
jvxFileReader::reset_bgrd_buffer_lock_stop()
{
	if (hdlReader)
	{
		switch (operation.mode)
		{
		case JVX_FILEOPERATION_REALTIME:
			buffer_realtime.idxRead = (buffer_realtime.idxRead + buffer_realtime.fHeight) % buffer_realtime.length;
			buffer_realtime.fHeight = 0;
			JVX_UNLOCK_MUTEX(protect.accessCircBuffer);

			if (theThread)
			{
				theThread->trigger_wakeup();
			}
			else
			{
				// Trigger load of next audio samples
				JVX_SET_NOTIFICATION(thread.readMoreSamples);
			}
			break;
		}
	}
	return JVX_NO_ERROR;
}

/**
*///==============================================
jvxErrorType
jvxFileReader::audio_buffer(jvxHandle** buffers, jvxSize numberChannels, jvxSize numberSamples, jvxBool strictlyNoBlocking, jvxData* progress)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idxRead = 0;
	jvxSize samplesToGo = 0;
	jvxSize samplesGone = 0;
	jvxSize samplesGet = 0;
	jvxSize prog = 0;

	if(hdlReader)
	{
		switch(operation.mode)
		{
		case JVX_FILEOPERATION_BATCH:

			// The read buffer will always be complete since it fills up with zeros if file is too short
			res = hdlReader->read_one_buf(buffers, numberChannels, numberSamples, buffer_realtime.formatAppl);
			hdlReader->current_progress(&prog);
			if(progress)
			{
				*progress = (jvxData)prog / (jvxData)operation.length_file_sample;
			}
			break;
		case JVX_FILEOPERATION_REALTIME:

			if(operation.underrun_occurred)
			{
				// If underrun occurred before, avoid taking out samples until thebuffer is full
				res = JVX_ERROR_POSTPONE;
				eval.num_samples_lost += numberSamples;
			}
			else
			{
				eval.min_fheight_on_enter = JVX_MIN(eval.min_fheight_on_enter, buffer_realtime.fHeight);
				samplesToGo = numberSamples;

				if(operation.fileEnded)
				{
					res = JVX_ERROR_END_OF_FILE;
				}
				else
				{
					while(samplesToGo)
					{
						samplesGet = JVX_MIN(buffer_realtime.fHeight, samplesToGo);

						if(samplesGet > 0)
						{
							JVX_LOCK_MUTEX(protect.accessCircBuffer);
							idxRead = buffer_realtime.idxRead;
							JVX_UNLOCK_MUTEX(protect.accessCircBuffer);

							jvxSize cpChannels = JVX_MIN(numberChannels, buffer_realtime.numChannels);
							jvxSize ll1 = JVX_MIN(buffer_realtime.length - idxRead, samplesGet);
							assert(ll1 > 0);

							for(i = 0; i < cpChannels; i++)
							{
								//static jvxData phase = 0.0;
								//jvx_generate_sine(phase, 0.5, 200, 48000, (jvxData*)buffers[i] + (samplesGone * buffer_realtime.szElement * sizeof(jvxByte)), ll1);

								memcpy((jvxByte*)buffers[i] + (samplesGone * buffer_realtime.szElement *sizeof(jvxByte)),
									(jvxByte*)buffer_realtime.intBuffer[i] + (buffer_realtime.szElement * idxRead * sizeof(jvxByte)),
									buffer_realtime.szElement * ll1* sizeof(jvxByte));
							}

							samplesToGo -= ll1;
							eval.num_samples_out += ll1;
							samplesGone += ll1;

							JVX_LOCK_MUTEX(protect.accessCircBuffer);
							buffer_realtime.idxRead = (buffer_realtime.idxRead + ll1)%buffer_realtime.length;
							buffer_realtime.fHeight -= ll1;
							JVX_UNLOCK_MUTEX(protect.accessCircBuffer);
						}
						else
						{
							res = JVX_ERROR_BUFFER_UNDERRUN;
							operation.underrun_occurred = true;
							eval.num_samples_lost += samplesToGo;
							break;
						}
					} // end of while
				}
				if(samplesToGo != 0)
				{
					jvxSize cpChannels = JVX_MIN(numberChannels, buffer_realtime.numChannels);
					jvxSize ll1 = JVX_MIN(buffer_realtime.length - idxRead, samplesToGo);
					assert(ll1 > 0);

					for(i = 0; i < cpChannels; i++)
					{
						memset((jvxByte*)buffers[i] + samplesGone * buffer_realtime.szElement * sizeof(jvxByte), 0, buffer_realtime.szElement * ll1 * sizeof(jvxByte));
					}
				}
			}

			hdlReader->current_progress(&prog);
			if(progress)
			{
				jvxInt64 val = (jvxInt64)prog - (jvxInt64)buffer_realtime.fHeight; // Be careful with the jvxSize!
				val = JVX_MAX(val, 0);
				*progress = (jvxData)(val) / (jvxData)operation.length_file_sample;
			}

			if (theThread)
			{
				theThread->trigger_wakeup();
			}
			else
			{			// Trigger to load nex file content
				JVX_SET_NOTIFICATION(thread.readMoreSamples);
			}
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
	jvxFileReader::postprocess()
{
	int i;
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlReader)
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

			JVX_SAFE_DELETE_FLD(buffer_realtime.readBuffer, jvxByte*);
			JVX_SAFE_DELETE_FLD(buffer_realtime.intBuffer, jvxByte*);

			buffer_realtime.readBuffer = NULL;
			buffer_realtime.intBuffer = NULL;
			break;
		default:
			res = JVX_ERROR_WRONG_STATE;
			break;
		}
		hdlReader->postprocess();
		operation.mode = JVX_FILEOPERATION_NONE;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

/**
* Class internal thread callback function
*///====================================================
JVX_THREAD_RETURN_TYPE
jvxFileReader::enterClassCallback()
{
	JVX_WAIT_RESULT resW;
	jvxErrorType res = JVX_NO_ERROR;

	JVX_WAIT_FOR_NOTIFICATION_I(thread.readMoreSamples);
	JVX_SET_NOTIFICATION(thread.started);

	while(thread.running)
	{
		//result = WaitForSingleObject(bControl.eventReportNewData, JVX_INFINITE);
		resW = JVX_WAIT_FOR_NOTIFICATION_II_INF(thread.readMoreSamples);
		//if(result == WAIT_OBJECT_0)
		bool retriggered = false;
		do
		{
			if (resW == JVX_WAIT_SUCCESS)
			{
				jvxBool want_to_continue = true;
				while (want_to_continue)
				{
					want_to_continue = core_read_function();
				}
			}
			else
			{
				assert(0);
			}

			if (thread.running)
			{
				retriggered = JVX_WAIT_FOR_CHECK_RETRIGGER_NOTIFICATION_I(thread.readMoreSamples);
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
jvxFileReader::write_to_circ_buffer(jvxSize& samplesWrite, jvxSize idxWrite)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvxSize readNow = JVX_MIN(buffer_realtime.length - idxWrite, samplesWrite);
	for(i = 0; i < buffer_realtime.numChannels; i++)
	{
		buffer_realtime.readBuffer[i] = buffer_realtime.intBuffer[i] + (idxWrite * sizeof(jvxByte) * buffer_realtime.szElement);
	}

	JVX_LOCK_MUTEX(protect.accessFileIo);
	res = hdlReader->read_one_buf((jvxHandle**)buffer_realtime.readBuffer, buffer_realtime.numChannels, readNow, buffer_realtime.formatAppl);
	JVX_UNLOCK_MUTEX(protect.accessFileIo);

	samplesWrite = readNow;
	return res;
}

jvxErrorType
jvxFileReader::wind_forward(jvxSize deltat_samples)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlReader)
	{
		JVX_LOCK_MUTEX(protect.accessFileIo);
		res = hdlReader->wind_fwd(deltat_samples);
		JVX_UNLOCK_MUTEX(protect.accessFileIo);
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
	jvxFileReader::wind_backward(jvxSize deltat_samples)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlReader)
	{
		JVX_LOCK_MUTEX(protect.accessFileIo);
		res = hdlReader->wind_bwd(deltat_samples);
		JVX_UNLOCK_MUTEX(protect.accessFileIo);
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileReader::restartPlayback(jvxInt32 timeout)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlReader)
	{		
		jvxErrorType resL = this->reset_bgrd_buffer_lock_start();
		assert(resL == JVX_NO_ERROR);
		JVX_LOCK_MUTEX(protect.accessFileIo);
		res = hdlReader->rewind();
		operation.fileEnded = false;
		JVX_UNLOCK_MUTEX(protect.accessFileIo);
		resL = this->reset_bgrd_buffer_lock_stop();
		assert(resL == JVX_NO_ERROR);
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileReader::playback_type(jvxFileContinueType* tp)
{
	if(tp)
	{
		*tp = operation.contType;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvxFileReader::set_playback_type(jvxFileContinueType tp)
{
	operation.contType = tp;
	return JVX_NO_ERROR;
}

jvxErrorType
jvxFileReader::progress(jvxSize* progress)
{
	if(hdlReader)
	{
		if(progress)
		{
			*progress = 0;
		}
	}
	return JVX_NO_ERROR;
}


jvxErrorType
jvxFileReader::terminate()
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxFileReader::start()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlReader)
	{
		res = hdlReader->start();

		switch(operation.mode)
		{
		case JVX_FILEOPERATION_BATCH:
			break;
		case JVX_FILEOPERATION_REALTIME:

			// Initial fill of bufer
			while(1)
			{
				jvxSize idxWrite = buffer_realtime.idxRead + buffer_realtime.fHeight;
				jvxSize writeMax = buffer_realtime.length - buffer_realtime.fHeight;
				if(writeMax > 0)
				{
					this->write_to_circ_buffer(writeMax, idxWrite);
					buffer_realtime.fHeight += writeMax;
				}
				else
				{
					break;
				}
			}

			if(!operation.fileEnded)
			{
				if (theThread)
				{
					theThread->initialize(this);
					theThread->start(JVX_SIZE_UNSELECTED, operation.boostPriority);
				}
				else
				{
					//bControl.threadHandle = CreateThread(NULL, 0, readBufferCallbackThrd, (void*)this, 0, NULL);
					JVX_THREAD_ID idT;

					thread.running = true;
					JVX_WAIT_FOR_NOTIFICATION_I(thread.started);

					JVX_CREATE_THREAD(thread.hdl, readBufferCallbackThrd, (void*)this, idT);

					JVX_WAIT_FOR_NOTIFICATION_II_INF(thread.started);

					if (operation.boostPriority)
					{
						//SetThreadPriority(bControl.threadHandle, THREAD_PRIORITY_TIME_CRITICAL);
						JVX_SET_THREAD_PRIORITY(thread.hdl, JVX_THREAD_PRIORITY_REALTIME);
					}
				}
			}
			break;
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileReader::stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlReader)
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

				JVX_SET_NOTIFICATION(thread.readMoreSamples);
				JVX_WAIT_FOR_THREAD_TERMINATE_MS(thread.hdl, WAIT_FOR_CLOSE_MS);
			}
			break;
		}
		res = hdlReader->stop();
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
jvxFileReader::get_file_properties(jvxSize* channels, jvxSize* length_samples, jvxInt32* srate, jvxFileFormat* fformat, jvxBool* littleEndian, jvxEndpointClass* descr, jvxSize* numBitsSample, jvxSize* subtype)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlReader)
	{
		res = hdlReader->get_file_properties(channels, length_samples, srate, fformat, littleEndian, numBitsSample, subtype);
		if(descr)
		{
			*descr = JVX_FILETYPE_WAV;
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileReader::get_processing_properties(jvxSize* int_buffersize, jvxSize* int_bsize_move_file_max, jvxDataFormat* format_appl, jvxFileContinueType* contTp)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;

	if(hdlReader)
	{
		hdlReader->status(&stat);
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
			if(contTp)
			{
				*contTp = JVX_FILECONTINUE_NONE;
			}
			return res;
		}
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxFileReader::evaluate(jvxSize* num_samples_in, jvxSize* num_samples_out, jvxSize* num_samples_failed, jvxSize* num_elms_fheight_min)
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
		*num_elms_fheight_min = eval.min_fheight_on_enter;
		eval.min_fheight_on_enter = buffer_realtime.length;
	}
	return JVX_NO_ERROR;
}

jvxBool 
jvxFileReader::core_read_function()
{
	jvxSize samplesWrite;
	jvxSize idxWrite;

	// Set this value as the number of elements to be copied
	JVX_LOCK_MUTEX(protect.accessCircBuffer);
	idxWrite = (buffer_realtime.idxRead + buffer_realtime.fHeight) % buffer_realtime.length;
	samplesWrite = buffer_realtime.length - buffer_realtime.fHeight;
	JVX_UNLOCK_MUTEX(protect.accessCircBuffer);

	samplesWrite = JVX_MIN(buffer_realtime.framesizeChunks, samplesWrite);
	if (samplesWrite == 0)
	{
		// Buffer completely filled, go leave!
		operation.underrun_occurred = false;
		return false; // only one condition to stop: if circ buffer is full
	}

	// This function produces zeros - even if file has ended
	jvxErrorType res = write_to_circ_buffer(samplesWrite, idxWrite);

	if (res == JVX_ERROR_END_OF_FILE)
	{
		operation.fileEnded = true;
	}
	JVX_LOCK_MUTEX(protect.accessCircBuffer);
	buffer_realtime.fHeight += samplesWrite;
	JVX_UNLOCK_MUTEX(protect.accessCircBuffer);
	return true;
}

jvxErrorType
jvxFileReader::startup(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxFileReader::expired(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxFileReader::wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	jvxBool want_to_continue = true;
	while (want_to_continue)
	{
		want_to_continue = core_read_function();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvxFileReader::stopped(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}
