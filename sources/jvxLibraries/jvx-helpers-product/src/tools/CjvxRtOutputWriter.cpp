#include "tools/CjvxRtOutputWriter.h"

CjvxRtOutputWriter::CjvxRtOutputWriter()
{
	fNamePrefix = "outwav";
	JVX_INITIALIZE_MUTEX(lock);
	writer = NULL;
	writer_obj = NULL;
	buffersize = JVX_SIZE_UNSELECTED;
	samplerate = JVX_SIZE_UNSELECTED;
	num_channels = JVX_SIZE_UNSELECTED;

	wavFileResolution = 16;
	wavFileType = JVX_WAV_32;

	state_outfile = 0;
	hHost = NULL;
	tHost = NULL;
};

jvxErrorType 
CjvxRtOutputWriter::initialize(IjvxToolsHost* tHostArg, IjvxHiddenInterface* hHostArg)
{
	hHost = hHostArg;
	tHost = tHostArg;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxRtOutputWriter::terminate()
{
	hHost = NULL;
	tHost = NULL;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxRtOutputWriter::set_fname_prefix(const std::string& fnamep)
{
	fNamePrefix = fnamep;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxRtOutputWriter::set_parameters(jvxSize buffersizeArg, jvxSize samplerateArg, jvxSize numchannelsArg, jvxDataFormat form_appl)
{
	if (jvx_bitTest(state_outfile,JVX_OUTPUTWRITER_SHIFT_PARAMETERSET))
	{
		return JVX_ERROR_WRONG_STATE;
	}
	buffersize = buffersizeArg;
	samplerate = samplerateArg;
	num_channels = numchannelsArg;
	format_app = form_appl;

	JVX_LOCK_MUTEX(lock);
	if (jvx_bitTest(state_outfile, JVX_OUTPUTWRITER_SHIFT_TRIGGERSTART))
	{
		start_file_writer_lock();
		jvx_bitSet(state_outfile, JVX_OUTPUTWRITER_SHIFT_PARAMETERSET);
	}
	else
	{
		jvx_bitSet(state_outfile, JVX_OUTPUTWRITER_SHIFT_PARAMETERSET);
	}
	JVX_UNLOCK_MUTEX(lock);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxRtOutputWriter::reset_parameters()
{
	if (!jvx_bitTest(state_outfile, JVX_OUTPUTWRITER_SHIFT_PARAMETERSET))
	{
		return JVX_ERROR_WRONG_STATE;
	}

	JVX_LOCK_MUTEX(lock);
	if (jvx_bitTest(state_outfile, JVX_OUTPUTWRITER_SHIFT_TRIGGERSTART))
	{
		stop_file_writer_lock();
		jvx_bitClear(state_outfile, JVX_OUTPUTWRITER_SHIFT_PARAMETERSET);
	}
	else
	{
		jvx_bitClear(state_outfile, JVX_OUTPUTWRITER_SHIFT_PARAMETERSET);
	}
	JVX_UNLOCK_MUTEX(lock);

	buffersize = JVX_SIZE_UNSELECTED;
	samplerate = JVX_SIZE_UNSELECTED;
	num_channels = JVX_SIZE_UNSELECTED;
	format_app = JVX_DATAFORMAT_NONE;

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxRtOutputWriter::trigger_start()
{
	if (jvx_bitTest(state_outfile, JVX_OUTPUTWRITER_SHIFT_TRIGGERSTART))
	{
		return JVX_ERROR_WRONG_STATE;
	}

	JVX_LOCK_MUTEX(lock);
	if (jvx_bitTest(state_outfile, JVX_OUTPUTWRITER_SHIFT_PARAMETERSET))
	{
		start_file_writer_lock();
		jvx_bitSet(state_outfile, JVX_OUTPUTWRITER_SHIFT_TRIGGERSTART);
	}
	else
	{
		jvx_bitSet(state_outfile, JVX_OUTPUTWRITER_SHIFT_TRIGGERSTART);
	}
	JVX_UNLOCK_MUTEX(lock);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxRtOutputWriter::trigger_stop()
{
	if (!jvx_bitTest(state_outfile, JVX_OUTPUTWRITER_SHIFT_TRIGGERSTART))
	{
		return JVX_ERROR_WRONG_STATE;
	}

	JVX_LOCK_MUTEX(lock);
	if (jvx_bitTest(state_outfile, JVX_OUTPUTWRITER_SHIFT_PARAMETERSET))
	{
		stop_file_writer_lock();
		jvx_bitClear(state_outfile, JVX_OUTPUTWRITER_SHIFT_TRIGGERSTART);
	}
	else
	{
		jvx_bitClear(state_outfile, JVX_OUTPUTWRITER_SHIFT_TRIGGERSTART);
	}
	JVX_UNLOCK_MUTEX(lock);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxRtOutputWriter::write_data(jvxHandle** bufs)
{
	jvxErrorType res = JVX_ERROR_COMPONENT_BUSY;
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resW = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX(resW, lock);
	if (resW == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		if (
			(jvx_bitTest(state_outfile, JVX_OUTPUTWRITER_SHIFT_PARAMETERSET)) &&
			(jvx_bitTest(state_outfile, JVX_OUTPUTWRITER_SHIFT_TRIGGERSTART)))
		{
			// write the data
			res = writer->add_audio_buffer(bufs, num_channels, buffersize, false);			
		}
		else
		{
			res = JVX_ERROR_NOT_READY;
		}
		JVX_UNLOCK_MUTEX(lock);
	}
	return res;
}

jvxErrorType 
CjvxRtOutputWriter::start_file_writer_lock()
{
	jvxErrorType resL = JVX_NO_ERROR;
	if (tHost)
	{
		tHost->instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_WRITER, &this->writer_obj, 0, NULL);
		if (writer_obj)
		{
			writer_obj->request_specialization(reinterpret_cast<jvxHandle**>(&writer), NULL, NULL);
			if (writer)
			{
				writer->initialize(hHost);
				jvxSize cnt = 0;
				while (cnt < 100)
				{
					std::string fName = fNamePrefix + "_" + jvx_size2String(cnt) + ".wav";

					jvxFileDescriptionEndpoint_open flDescr;
					flDescr.fFormatFile = JVX_FILEFORMAT_PCM_FIXED;
					flDescr.lengthFile = 0;
					flDescr.littleEndian = true;
					flDescr.numberBitsSample = wavFileResolution;
					flDescr.numberChannels = num_channels;
					flDescr.samplerate = (jvxInt32)samplerate;
					flDescr.subtype = wavFileType;

					std::cout << "Trying to open capture file <" << fName << std::flush;

					resL = writer->activate(fName.c_str(),
						JVX_FILETYPE_WAV,
						&flDescr,
						false);
					if (resL == JVX_NO_ERROR)
					{
						std::cout << " -> OK" << std::endl;
						jvxFileDescriptionEndpoint_prepare prDescr;
						prDescr.dFormatAppl = format_app;
						prDescr.numSamplesFrameMax_moveBgrd = (jvxInt32)(2 * buffersize);
						resL = writer->prepare(numBufs * buffersize,
							true,
							&prDescr,
							JVX_FILEOPERATION_REALTIME);
						assert(resL == JVX_NO_ERROR);

						resL = writer->start();
						assert(resL == JVX_NO_ERROR);
						break;
					}
					else
					{
						std::cout << " -> FAILED" << std::endl;
						cnt++;
					}
				}
			}
			else
			{
				tHost->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_WRITER, this->writer_obj, 0, NULL);
				this->writer_obj = NULL;
				resL = JVX_ERROR_UNSUPPORTED;
			}
		}// if (writer_obj)
		else
		{ 
			resL = JVX_ERROR_UNSUPPORTED;
		}
	} //if (tHost)
	else
	{
		resL = JVX_ERROR_NOT_READY;
	}
	return resL;
}

jvxErrorType
CjvxRtOutputWriter::stop_file_writer_lock()
{
	jvxErrorType resL = JVX_NO_ERROR;
	if (tHost)
	{
		if (writer)
		{
			resL = writer->stop();
			assert(resL == JVX_NO_ERROR);

			resL = writer->postprocess();
			assert(resL == JVX_NO_ERROR);

			writer->terminate();
			assert(resL == JVX_NO_ERROR);

			writer = NULL;
		}		
	
		if (writer_obj)
		{
			tHost->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_WRITER, this->writer_obj, 0, NULL);
			writer_obj = NULL;
		}
	} //if (tHost)
	else
	{
		resL = JVX_ERROR_NOT_READY;
	}
	return resL;
}

jvxErrorType 
CjvxRtOutputWriter::set_wav_props(jvxSize wavFileResolutionArg, jvxSize wavFileTypeArg, jvxSize num_bufsArg)
{
	wavFileResolution = wavFileResolutionArg;
	wavFileType = wavFileTypeArg;
	numBufs = num_bufsArg;
	return JVX_NO_ERROR;
}