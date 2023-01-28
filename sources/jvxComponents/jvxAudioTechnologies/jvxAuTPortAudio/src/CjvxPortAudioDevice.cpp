#include "CjvxPortAudioDevice.h"
#include <cassert>
#include "jvx_config.h"

// Port audio headers
#include "portaudio.h"
#ifdef JVX_OS_WINDOWS
#include "pa_asio.h"
#endif

#define ALPHA_SMOOTH_DELTA_T 0.995

// ================================================================
// Test the following samplerates
// ================================================================

#define NUMBER_TEST_FORMATS 5
PaSampleFormat testFormats[NUMBER_TEST_FORMATS] =
{
	(paNonInterleaved| paFloat32),
	(paNonInterleaved| paInt16),
	(paNonInterleaved| paInt24),
	(paNonInterleaved| paInt32),
	(paNonInterleaved| paInt8)
};


// ==========================================================================
// Audio processing callback
// ==========================================================================
static int theCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
	CjvxPortAudioDevice* loc_this = (CjvxPortAudioDevice*)userData;
	jvxErrorType res = loc_this->processBuffer_callback(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags);
	if(res == JVX_NO_ERROR)
	{
		return(paContinue);
	}
	return(paAbort);
}

// ==========================================================================

CjvxPortAudioDevice::CjvxPortAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));

	//resetAsio();
	//allHandlerPtrs = allHandlers;
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_8BIT);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_16BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_32BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_64BIT_LE);

	_common_set.thisisme = static_cast<IjvxObject*>(this);
	
	runtime.usefulBSizes.clear();
	jvxSize cnt = 0;
	while(1)
	{
		int oneBSize = jvxUsefulBSizes[cnt++];
		if(oneBSize > 0)
		{
			runtime.usefulBSizes.push_back(oneBSize);
		}
		else
		{
			break;
		}
	}
}

CjvxPortAudioDevice::~CjvxPortAudioDevice()
{
}


jvxErrorType
CjvxPortAudioDevice::activate()
{
	int i;
	int j;
	std::string name;
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 delta = JVX_MAX_INT_32_INT32;
	jvxInt16 idxDeltaMin = 0;

	res = CjvxAudioDevice::activate();
	if(res == JVX_NO_ERROR)
	{
		genPortAudio_device::init__properties_active();
		genPortAudio_device::allocate__properties_active();
		genPortAudio_device::register__properties_active(static_cast<CjvxProperties*>(this));

		genPortAudio_device::init__properties_active_higher();
		genPortAudio_device::allocate__properties_active_higher();
		genPortAudio_device::register__properties_active_higher(static_cast<CjvxProperties*>(this));
		genPortAudio_device::properties_active_higher.loadpercent.isValid = false;


		hostInfo = Pa_GetHostApiInfo(my_props.deviceInfo->hostApi);

		// ============================================================
		// Scan for available input channels
		// ============================================================
		CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
		CjvxAudioDevice::properties_active.inputchannelselection.value.selection() = 0;
		CjvxAudioDevice::properties_active.inputchannelselection.value.exclusive = 0;


		for(i = 0; i < (int)my_props.deviceInfo->maxInputChannels; i++)
		{
			name = "In #" + jvx_int2String(i);
#ifdef JVX_OS_WINDOWS
			if(hostInfo->type == paASIO)
			{
				const char* tmpC = NULL;
				PaAsio_GetInputChannelName (my_props.id, i, &tmpC);
				if(tmpC)
				{
					name = "In #" + jvx_int2String(i) + ":" + tmpC;
				}
			}
#endif

			CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(name.c_str());
			CjvxAudioDevice::properties_active.inputchannelselection.value.selection() |= ((jvxBitField)1 << i);
		}

		CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
		CjvxAudioDevice::properties_active.outputchannelselection.value.selection() = 0;
		CjvxAudioDevice::properties_active.outputchannelselection.value.exclusive = 0;

		for(i = 0; i < (int)my_props.deviceInfo->maxOutputChannels; i++)
		{
			name = "Out #" + jvx_int2String(i);
#ifdef JVX_OS_WINDOWS
			if(hostInfo->type == paASIO)
			{
				const char* tmpC = NULL;
				PaAsio_GetOutputChannelName (my_props.id, i, &tmpC);
				if(tmpC)
				{
					name = "Out #" + jvx_int2String(i) + ":" + tmpC;
				}
			}
#endif

			CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(name.c_str());
			CjvxAudioDevice::properties_active.outputchannelselection.value.selection() |= ((jvxBitField)1 << i);
		}


		// ============================================================
		// Setup the buffersize lists
		// ============================================================

		genPortAudio_device::properties_active.sizesselection.value.selection() = 0;
		genPortAudio_device::properties_active.sizesselection.isValid = false;
		genPortAudio_device::properties_active.sizesselection.value.entries.clear();
		CjvxAudioDevice_genpcg::properties_active.buffersize.value = 1024;

		runtime.sizesBuffers.clear();
		this->_update_property_access_type(JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT, JVX_PROPERTY_CATEGORY_PREDEFINED, CjvxAudioDevice::properties_active.buffersize.globalIdx);

#ifdef JVX_OS_WINDOWS

		if(hostInfo->type == paASIO )
		{
			genPortAudio_device::properties_active.sizesselection.isValid = true;
			long bufCnt;
			PaAsio_GetAvailableLatencyValues( my_props.id, &runtime.minSizeBuffers, &runtime.maxSizeBuffers, &runtime.preferredSizeBuffers, &runtime.granBuffers );

			genPortAudio_device::properties_active.sizesselection.value.selection() = 0;
			if(runtime.granBuffers > 0)
			{
				// List up all available buffersizes
				bufCnt = runtime.minSizeBuffers;
				while(bufCnt <= runtime.maxSizeBuffers)
				{
					jvxBool isUseful = false;
					for(i = 0; i < runtime.usefulBSizes.size(); i++)
					{
						if(runtime.usefulBSizes[i] == bufCnt)
						{
							isUseful = true;
							break;
						}
					}

					if(isUseful)
					{
						runtime.sizesBuffers.push_back(bufCnt);
						genPortAudio_device::properties_active.sizesselection.value.entries.push_back(jvx_int2String(bufCnt));
					}
					bufCnt += runtime.granBuffers;
				}
			}
			else
			{
				// Buffersizes are power of two inbetween min and max
				bufCnt = runtime.minSizeBuffers;
				while(bufCnt <= runtime.maxSizeBuffers)
				{
					runtime.sizesBuffers.push_back(bufCnt);
					genPortAudio_device::properties_active.sizesselection.value.entries.push_back(jvx_int2String(bufCnt));
					bufCnt *= 2;
				}
			}

			CjvxAudioDevice::properties_active.buffersize.value = runtime.preferredSizeBuffers;

			if(runtime.sizesBuffers.size() == 0)
			{
				res = JVX_ERROR_NOT_READY;
				_common_set.theErrordescr = "Failed to query a single valid buffersize, <CjvxAudioAsioDevice::activateAsioProperties>";
			}
			else
			{
				delta = JVX_MAX_INT_32_INT32;
				idxDeltaMin = 0;

				for(i = 0; i < (int)runtime.sizesBuffers.size(); i++)
				{
					if(abs(runtime.sizesBuffers[i] - runtime.preferredSizeBuffers) < delta)
					{
						delta = abs(runtime.sizesBuffers[i] - runtime.preferredSizeBuffers);
						idxDeltaMin = i;
					}
				}

				genPortAudio_device::properties_active.sizesselection.value.selection() = (jvxBitField)1 << idxDeltaMin;
				CjvxAudioDevice_genpcg::properties_active.buffersize.value = runtime.sizesBuffers[idxDeltaMin];
			}
			this->_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, JVX_PROPERTY_CATEGORY_PREDEFINED, CjvxAudioDevice::properties_active.buffersize.globalIdx);
		}
		else
		{
			genPortAudio_device::properties_active.sizesselection.isValid = false;
		}
#endif


		// ===========================================================
		// Check for samplerates and native formats
		// ===========================================================

		genPortAudio_device::properties_active.ratesselection.value.entries.clear();
		runtime.samplerates.clear();

		PaStreamParameters inputParams;
		PaStreamParameters outputParams;
		size_t k;

		PaStreamParameters* argIn = NULL;
		PaStreamParameters* argOut = NULL;

		CjvxAudioDevice_genpcg::properties_active.samplerate.value = (jvxInt32)my_props.deviceInfo->defaultSampleRate;
		bool foundFirstValidSamplerate = false;

		runtime.formats.clear();

		i = 0;
		while(1)
		{
			if(jvxUsefulSRates[i] > 0)
			{
				std::vector<PaSampleFormat> idsFormatSupportedLocal;

				for(j = 0; j < NUMBER_TEST_FORMATS; j++)
				{
					inputParams.device = my_props.id;
					inputParams.channelCount = my_props.deviceInfo->maxInputChannels;
					inputParams.sampleFormat = testFormats[j];
					inputParams.suggestedLatency = 0; /* ignored by Pa_IsFormatSupported() */
					inputParams.hostApiSpecificStreamInfo = NULL;

					outputParams.device = my_props.id;
					outputParams.channelCount = my_props.deviceInfo->maxOutputChannels;
					outputParams.sampleFormat = testFormats[j];
					outputParams.suggestedLatency = 0; /* ignored by Pa_IsFormatSupported() */
					outputParams.hostApiSpecificStreamInfo = NULL;

					// Control use of pointer parameters
					if(inputParams.channelCount == 0)
					{
						argIn = NULL;
					}
					else
					{
						argIn = &inputParams;
					}

					if(outputParams.channelCount == 0)
					{
						argOut = NULL;
					}
					else
					{
						argOut = &outputParams;
					}

					PaError err = Pa_IsFormatSupported(argIn, argOut, jvxUsefulSRates[i]);
					if( err == paFormatIsSupported )
					{
						idsFormatSupportedLocal.push_back(j);
					}
				}
				if(idsFormatSupportedLocal.size() > 0)
				{
					// If there was at least one valid combination..
					if(foundFirstValidSamplerate == false)
					{
						// The supported formats should be identical for each samplerate
						runtime.formats = idsFormatSupportedLocal;
						foundFirstValidSamplerate = true;
					}
					else
					{
						// Check that the supported formats for all samplerates are identical
						if(runtime.formats.size() == idsFormatSupportedLocal.size())
						{
							for(k = 0; k < runtime.formats.size(); k++)
							{
								assert(runtime.formats[k] == idsFormatSupportedLocal[k]);
							}
						}
					}
					idsFormatSupportedLocal.clear();
					genPortAudio_device::properties_active.ratesselection.value.entries.push_back(jvx_int2String(jvxUsefulSRates[i]));
					this->runtime.samplerates.push_back(jvxUsefulSRates[i]);
				}
				i++;
			}
			else
			{
				break;
			}
		}

		CjvxAudioDevice_genpcg::properties_active.formatselection.value.entries.clear();
		for(i = 0; i < _common_set_audio_device.formats.size(); i++) /* runtime.formats */
		{
			CjvxAudioDevice_genpcg::properties_active.formatselection.value.entries.push_back(jvxDataFormat_txt(_common_set_audio_device.formats[i])); /* runtime.formats */
			jvx_bitZSet(CjvxAudioDevice_genpcg::properties_active.formatselection.value.selection(), 0);
		}

		if(runtime.samplerates.size() > 0)
		{
			CjvxAudioDevice::properties_active.samplerate.value = runtime.samplerates[0];
			genPortAudio_device::properties_active.ratesselection.value.selection() = (1 << 0);
			this->_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, JVX_PROPERTY_CATEGORY_PREDEFINED, CjvxAudioDevice::properties_active.samplerate.globalIdx);
		}
		else
		{
			res = JVX_ERROR_NOT_READY;
			_common_set.theErrordescr = "Failed to query a single valid samplerate, <CjvxAudioAsioDevice::activateAsioProperties>";
		}

	}
	return(res);
}

jvxErrorType
CjvxPortAudioDevice::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioDevice::deactivate();
	if(res == JVX_NO_ERROR)
	{

		/* Clear all structures */
		runtime.samplerates.clear();
		genPortAudio_device::properties_active.ratesselection.value.selection() = 0;
		this->_update_property_access_type(JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT, JVX_PROPERTY_CATEGORY_PREDEFINED, CjvxAudioDevice::properties_active.samplerate.globalIdx);

		CjvxAudioDevice_genpcg::properties_active.formatselection.value.entries.clear();
		CjvxAudioDevice_genpcg::properties_active.formatselection.value.selection() = 0;
		runtime.formats.clear();

		runtime.sizesBuffers.clear();
		genPortAudio_device::properties_active.sizesselection.value.selection() = 0;
		genPortAudio_device::properties_active.sizesselection.value.entries.clear();
		CjvxAudioDevice_genpcg::properties_active.buffersize.value = 0;
		this->_update_property_access_type(JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT, JVX_PROPERTY_CATEGORY_PREDEFINED, CjvxAudioDevice::properties_active.buffersize.globalIdx);

		genPortAudio_device::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		genPortAudio_device::deallocate__properties_active_higher();

		genPortAudio_device::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genPortAudio_device::deallocate__properties_active();
	}
	return(res);
}

jvxErrorType
CjvxPortAudioDevice::prepare()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	res = CjvxAudioDevice::prepare();
	if(res == JVX_NO_ERROR)
	{
		CjvxProperties::_lock_properties_local();

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genPortAudio_device::properties_active.ratesselection.category,
			genPortAudio_device::properties_active.ratesselection.globalIdx);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genPortAudio_device::properties_active.sizesselection.category,
			genPortAudio_device::properties_active.sizesselection.globalIdx);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genPortAudio_device::properties_active.controlThreads.category,
			genPortAudio_device::properties_active.controlThreads.globalIdx);

		CjvxProperties::_unlock_properties_local();


	}
	return(res);
}

jvxErrorType
CjvxPortAudioDevice::start()
{
	jvxErrorType res = CjvxAudioDevice::start();
	if(res == JVX_NO_ERROR)
	{
		//res = start_master(&inProcessing.theData);
	}
	return(res);
}

//	virtual bool JVX_CALLINGCONVENTION queryDegreeBusyProcessingDevice(rtpDouble* degree);
jvxErrorType
CjvxPortAudioDevice::stop()
{
	jvxErrorType res = CjvxAudioDevice::stop();
	if(res == JVX_NO_ERROR)
	{
		//res = stop_master(&inProcessing.theData);
	}
	return(res);
}

jvxErrorType
CjvxPortAudioDevice::postprocess()
{
	jvxSize numNotDeallocated = 0;
	jvxErrorType res = CjvxAudioDevice::postprocess();
	jvxErrorType resL;
	if(res == JVX_NO_ERROR)
	{
		//res = postprocess_master(&inProcessing.theData);

		CjvxProperties::_lock_properties_local();

		CjvxProperties::_undo_update_property_access_type(
			genPortAudio_device::properties_active.ratesselection.category,
			genPortAudio_device::properties_active.ratesselection.globalIdx);

		CjvxProperties::_undo_update_property_access_type(
			genPortAudio_device::properties_active.sizesselection.category,
			genPortAudio_device::properties_active.sizesselection.globalIdx);

		CjvxProperties::_undo_update_property_access_type(
			genPortAudio_device::properties_active.controlThreads.category,
			genPortAudio_device::properties_active.controlThreads.globalIdx);

		CjvxProperties::_unlock_properties_local();

	}
	return(res);
}

jvxErrorType
CjvxPortAudioDevice::processBuffer_callback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags)
{
	jvxSize i,ii;
	jvxErrorType res = JVX_NO_ERROR;
	const void** iBuf = (const void**) inputBuffer;
	const void** oBuf = (const void**) outputBuffer;

	jvxTick tStamp_start = JVX_GET_TICKCOUNT_US_GET_CURRENT(&inProcessing.theTimestamp);

	if(runtime.threads.coreHdl.hdl)
	{
		if(!runtime.threads.syncAudioThreadAssoc)
		{
			runtime.threads.coreHdl.hdl->associate_tc_thread(JVX_GET_CURRENT_THREAD_ID(), runtime.threads.syncAudioThreadId);
			runtime.threads.syncAudioThreadAssoc = true;
		}
	}

	// I am the master. I will start the chain here!
	if (_common_set_ldslave.theData_out.con_link.connect_to)
	{
		_common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
	}

	jvxSize idxToProc = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
	jvxSize idxFromProc = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;

	switch (_inproc.format)
	{
	case JVX_DATAFORMAT_DATA:
		/* Convert float to jvxData */
		for (i = 0; i < inProcessing.inputMapper.size(); i++)
		{
			jvxData* ptrDest = (jvxData*)_common_set_ldslave.theData_out.con_data.buffers[idxToProc][i];
			float* ptrSrc = (float*)iBuf[inProcessing.inputMapper[i]];

			for (ii = 0; ii < _common_set_ldslave.theData_out.con_params.buffersize; ii++)
			{
				*ptrDest++ = (jvxData)*ptrSrc++;
			}
		}
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		for (i = 0; i < inProcessing.inputMapper.size(); i++)
		{
			jvxInt64* ptrDest = (jvxInt64*)_common_set_ldslave.theData_out.con_data.buffers[idxToProc][i];
			jvxInt32* ptrSrc = (jvxInt32*)iBuf[inProcessing.inputMapper[i]];
			jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt32, jvxInt64, jvxInt64>(ptrSrc, ptrDest, 
				_common_set_ldslave.theData_out.con_params.buffersize, 32);
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		for (i = 0; i < inProcessing.inputMapper.size(); i++)
		{
			jvxInt32* ptrDest = (jvxInt32*)_common_set_ldslave.theData_out.con_data.buffers[idxToProc][i];
			jvxInt32* ptrSrc = (jvxInt32*)iBuf[inProcessing.inputMapper[i]];
			jvx_convertSamples_memcpy(ptrSrc, ptrDest, jvxDataFormat_size[_inproc.format], 
				_common_set_ldslave.theData_out.con_params.buffersize);
		}
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		for (i = 0; i < inProcessing.inputMapper.size(); i++)
		{
			jvxInt16* ptrDest = (jvxInt16*)_common_set_ldslave.theData_out.con_data.buffers[idxToProc][i];
			jvxInt16* ptrSrc = (jvxInt16*)iBuf[inProcessing.inputMapper[i]];
			jvx_convertSamples_memcpy(ptrSrc, ptrDest, jvxDataFormat_size[_inproc.format], _common_set_ldslave.theData_out.con_params.buffersize);
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		for (i = 0; i < inProcessing.inputMapper.size(); i++)
		{
			jvxInt8* ptrDest = (jvxInt8*)_common_set_ldslave.theData_out.con_data.buffers[idxToProc][i];
			jvxInt16* ptrSrc = (jvxInt16*)iBuf[inProcessing.inputMapper[i]];
			jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt16, jvxInt8, jvxInt16>(ptrSrc, ptrDest,
				_common_set_ldslave.theData_out.con_params.buffersize, 8);
		}
		break;
	}

	// I am the master. I will start the chain here!
	if (_common_set_ldslave.theData_out.con_link.connect_to)
	{
		_common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
	}
	
	switch (_inproc.format)
	{
	case JVX_DATAFORMAT_DATA:
		/* Convert float to double */
		for (i = 0; i < inProcessing.outputMapper.size(); i++)
		{
			float* ptrDest = (float*)oBuf[inProcessing.outputMapper[i]];
			jvxData* ptrSrc = (jvxData*)_common_set_ldslave.theData_in->con_data.buffers[idxFromProc][i];

			for (ii = 0; ii < _common_set_ldslave.theData_out.con_params.buffersize; ii++)
			{
				*ptrDest++ = (jvxData)*ptrSrc++;
			}
		}
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		/* Convert float to double */
		for (i = 0; i < inProcessing.outputMapper.size(); i++)
		{
			jvxInt32* ptrDest = (jvxInt32*)oBuf[inProcessing.outputMapper[i]];
			jvxInt64* ptrSrc = (jvxInt64*)_common_set_ldslave.theData_in->con_data.buffers[idxFromProc][i];

			jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt32, jvxInt64>(ptrSrc, ptrDest, _common_set_ldslave.theData_out.con_params.buffersize, 32);
		}
		break;

	case JVX_DATAFORMAT_32BIT_LE:
		for (i = 0; i < inProcessing.outputMapper.size(); i++)
		{
			jvxInt32* ptrDest = (jvxInt32*)oBuf[inProcessing.outputMapper[i]];
			jvxInt32* ptrSrc = (jvxInt32*)_common_set_ldslave.theData_in->con_data.buffers[idxFromProc][i];

			jvx_convertSamples_memcpy(ptrSrc, ptrDest, jvxDataFormat_size[_inproc.format], _common_set_ldslave.theData_in->con_params.buffersize);
		}
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		for (i = 0; i < inProcessing.outputMapper.size(); i++)
		{
			jvxInt16* ptrDest = (jvxInt16*)oBuf[inProcessing.outputMapper[i]];
			jvxInt16* ptrSrc = (jvxInt16*)_common_set_ldslave.theData_in->con_data.buffers[idxFromProc][i];

			jvx_convertSamples_memcpy(ptrSrc, ptrDest, jvxDataFormat_size[_inproc.format], _common_set_ldslave.theData_in->con_params.buffersize);
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		for (i = 0; i < inProcessing.outputMapper.size(); i++)
		{
			jvxInt16* ptrDest = (jvxInt16*)oBuf[inProcessing.outputMapper[i]];
			jvxInt8* ptrSrc = (jvxInt8*)_common_set_ldslave.theData_in->con_data.buffers[idxFromProc][i];

			jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt8, jvxInt16, jvxInt16>(ptrSrc, ptrDest,
				_common_set_ldslave.theData_out.con_params.buffersize, 8);
		}
		break;
	}

	if (_common_set_ldslave.theData_out.con_link.connect_to)
	{
		_common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
	}

	jvxTick tStamp_stop = JVX_GET_TICKCOUNT_US_GET_CURRENT(&inProcessing.theTimestamp);
	double deltaT = (jvxData)(tStamp_stop - tStamp_start);

	/*
	ds[cnt] = deltaT;
	cnt++;
	if(cnt >= 1000)
	cnt = 0;
	*/

	inProcessing.deltaT_average_us = (1-ALPHA_SMOOTH_DELTA_T) * deltaT + ALPHA_SMOOTH_DELTA_T *inProcessing.deltaT_average_us;
	jvxData load = inProcessing.deltaT_average_us / inProcessing.deltaT_theory_us * 100.0;
	load = JVX_MIN(load, 100.0);
	genPortAudio_device::properties_active_higher.loadpercent.value = load;

	return(res);
}

jvxErrorType
CjvxPortAudioDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxSize i;
	jvxBool report_update = false;
	jvxErrorType res = CjvxAudioDevice::set_property(callGate,
		rawPtr, ident, trans);
	if(res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		if(category == JVX_PROPERTY_CATEGORY_PREDEFINED)
		{
			if(propId == genPortAudio_device::properties_active.sizesselection.globalIdx)
			{
				report_update = true;
				assert(genPortAudio_device::properties_active.sizesselection.value.selection() != 0);
				for(i = 0; i < runtime.sizesBuffers.size(); i++)
				{
					if(jvx_bitTest(genPortAudio_device::properties_active.sizesselection.value.selection(), i))
					{
						CjvxAudioDevice::properties_active.buffersize.value = runtime.sizesBuffers[i];
						break;
					}
				}
			}
			else if(propId == genPortAudio_device::properties_active.ratesselection.globalIdx)
			{
				report_update = true;
				assert(genPortAudio_device::properties_active.ratesselection.value.selection() != 0);
				for(i = 0; i < runtime.samplerates.size(); i++)
				{
					if(jvx_bitTest(genPortAudio_device::properties_active.ratesselection.value.selection(),i))
					{
						CjvxAudioDevice::properties_active.samplerate.value = runtime.samplerates[i];
						break;
					}
				}
			}
		}

		if(report_update)
		{
			if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)
			{
				if (_common_set_ld_master.refProc)
				{
					//_report_property_has_changed(category, propId, true, ( JVX_COMPONENT_UNKNOWN), callPurpose);
					jvxSize uId = JVX_SIZE_UNSELECTED;
					_common_set_ld_master.refProc->unique_id_connections(&uId);
					this->_request_test_chain_master(uId);
				}
			}
		}

	}
	return(res);
}

jvxErrorType
CjvxPortAudioDevice::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno )
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

	// Read all parameters for base class
	CjvxAudioDevice::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno );

	if(this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// Read all parameters from this class
		genPortAudio_device::put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		if(!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for(i = 0; i < warnings.size(); i++)
			{
				txt = "Failed to read property " + warnings[i];
				_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}

			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		this->updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, false);

	}
	return(res);
}

jvxErrorType
CjvxPortAudioDevice::get_configuration(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	// Write all parameters from base class
	CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

	// Write all parameters from this class
	genPortAudio_device::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);

	return(JVX_NO_ERROR);
}

void
CjvxPortAudioDevice::updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp)
{
	jvxSize i;
	jvxBool report_update = false;
	jvxInt32 newValue = 0;

	// Update the variables in base class
	CjvxAudioDevice::updateDependentVariables_lock(propId, category, updateAll);

	if(category == JVX_PROPERTY_CATEGORY_PREDEFINED)
	{
		if(
			(propId == genPortAudio_device::properties_active.ratesselection.globalIdx) ||
			(propId == CjvxAudioDevice::properties_active.samplerate.globalIdx))
		{
			for(i = 0; i < (int)genPortAudio_device::properties_active.ratesselection.value.entries.size(); i++)
			{
				if(jvx_bitTest(genPortAudio_device::properties_active.ratesselection.value.selection(), i))
				{
					newValue = runtime.samplerates[i];
					break;
				}
			}
			CjvxAudioDevice::properties_active.samplerate.value = newValue;

			this->_report_property_has_changed(
				JVX_PROPERTY_CATEGORY_PREDEFINED,
				CjvxAudioDevice::properties_active.samplerate.globalIdx,
				true, 0, 1, (JVX_COMPONENT_UNKNOWN), callPurp);
			this->_report_property_has_changed(
				JVX_PROPERTY_CATEGORY_PREDEFINED,
				genPortAudio_device::properties_active.ratesselection.globalIdx,
				true, 0,1,(JVX_COMPONENT_UNKNOWN), callPurp);
		}

		// =================================================================================================
		if(genPortAudio_device::properties_active.sizesselection.isValid)
		{
			if(
				(propId == genPortAudio_device::properties_active.sizesselection.globalIdx) ||
				(propId == CjvxAudioDevice::properties_active.buffersize.globalIdx))
			{
				for(i = 0; i < (int)genPortAudio_device::properties_active.sizesselection.value.entries.size(); i++)
				{
					if(jvx_bitTest(genPortAudio_device::properties_active.sizesselection.value.selection(), i))
					{
						newValue = runtime.sizesBuffers[i];
						break;
					}
				}
				CjvxAudioDevice::properties_active.buffersize.value = newValue;

				this->_report_property_has_changed(
					JVX_PROPERTY_CATEGORY_PREDEFINED, CjvxAudioDevice::properties_active.buffersize.globalIdx,
					true, 0, 1, JVX_COMPONENT_UNKNOWN, callPurp);
				this->_report_property_has_changed(
					JVX_PROPERTY_CATEGORY_PREDEFINED, genPortAudio_device::properties_active.sizesselection.globalIdx,
					true, 0, 1, JVX_COMPONENT_UNKNOWN, callPurp);
			}
		}
	}
}

jvxErrorType
CjvxPortAudioDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize i;

	// Start the port audio stream
	PaStreamParameters inputParams;
	PaStreamParameters outputParams;
	PaStreamParameters* argIn = NULL;
	PaStreamParameters* argOut = NULL;
	PaError err = paNoError;
	
	runtime.threads.coreHdl.hdl = NULL;
	runtime.threads.coreHdl.object = NULL;
	runtime.threads.theToolsHost = NULL;
	runtime.threads.syncAudioThreadAssoc = false;
	runtime.threads.syncAudioThreadId = 0;

	// If the chain is prepared, the object itself should also be prepared if not done so before
	resL = prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	// Start to set all parameters at first
	_common_set_ldslave.theData_out.con_params.buffersize = _inproc.buffersize;
	_common_set_ldslave.theData_out.con_params.format = _inproc.format;
	_common_set_ldslave.theData_out.con_data.buffers = NULL;
	_common_set_ldslave.theData_out.con_data.number_buffers = 1;
	_common_set_ldslave.theData_out.con_params.number_channels = _inproc.numInputs;
	_common_set_ldslave.theData_out.con_params.rate = _inproc.samplerate;
	_common_set_ldslave.theData_out.con_user.chain_spec_user_hints = NULL;
	res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		resL = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(resL == JVX_NO_ERROR);
		goto leave_error;
	}

	if (JVX_EVALUATE_BITFIELD(genPortAudio_device::properties_active.controlThreads.value.selection() & 0x2))
	{
		if (_common_set_min.theHostRef)
		{
			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&runtime.threads.theToolsHost));
			if ((resL == JVX_NO_ERROR) && runtime.threads.theToolsHost)
			{
				resL = runtime.threads.theToolsHost->reference_tool(JVX_COMPONENT_THREADCONTROLLER, &runtime.threads.coreHdl.object, 0, NULL);
				if ((resL == JVX_NO_ERROR) && (runtime.threads.coreHdl.object))
				{
					resL = runtime.threads.coreHdl.object->request_specialization(reinterpret_cast<jvxHandle**>(&runtime.threads.coreHdl.hdl), NULL, NULL);
					if ((resL == JVX_NO_ERROR) && runtime.threads.coreHdl.hdl)
					{
						jvxState stat = JVX_STATE_NONE;
						runtime.threads.coreHdl.hdl->state(&stat);
						if (stat < JVX_STATE_ACTIVE)
						{
							runtime.threads.theToolsHost->return_reference_tool(JVX_COMPONENT_THREADCONTROLLER, runtime.threads.coreHdl.object);
							runtime.threads.coreHdl.hdl = NULL;
							runtime.threads.coreHdl.object = NULL;
						}
					}
				}
			}
		}
	}

	if (runtime.threads.coreHdl.hdl)
	{
		runtime.threads.syncAudioThreadAssoc = false;
		resL = runtime.threads.coreHdl.hdl->register_tc_thread(&runtime.threads.syncAudioThreadId);
		if (resL != JVX_NO_ERROR)
		{
			this->_report_text_message("Failed to control audio thread, no thread control used.", JVX_REPORT_PRIORITY_WARNING);
		}
		else
		{
			this->_report_text_message("Failed to control sync threads, no thread control used.", JVX_REPORT_PRIORITY_WARNING);
		}
	}

	inProcessing.numInputChannels = 0;
	inProcessing.numOutputChannels = 0;

	inProcessing.inputMapper.clear();
	inProcessing.outputMapper.clear();

	for (i = 0; i < my_props.deviceInfo->maxInputChannels; i++)
	{
		if (jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i))
		{
			inProcessing.inputMapper.push_back(i);
			inProcessing.numInputChannels++;
		}
	}

	for (i = 0; i < my_props.deviceInfo->maxOutputChannels; i++)
	{
		if (jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i))
		{
			inProcessing.outputMapper.push_back(i);
			inProcessing.numOutputChannels++;
		}
	}

	switch (CjvxAudioDevice_genpcg::properties_active.format.value)
	{
	case JVX_DATAFORMAT_DATA:
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
		//inProcessing.useNative = false;
		inputParams.sampleFormat = (paNonInterleaved | paFloat32);
#else
		//inProcessing.useNative = true;
		inputParams.sampleFormat = (paNonInterleaved | paFloat32);
#endif
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		//inProcessing.useNative = false;
		inputParams.sampleFormat = (paNonInterleaved | paInt32);
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		//inProcessing.useNative = true;
		inputParams.sampleFormat = (paNonInterleaved | paInt32);
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		//inProcessing.useNative = true;
		inputParams.sampleFormat = (paNonInterleaved | paInt16);
		break;
	case JVX_DATAFORMAT_8BIT:
		//inProcessing.useNative = true;
		inputParams.sampleFormat = (paNonInterleaved | paInt8);
		break;
	}

	JVX_GET_TICKCOUNT_US_SETREF(&inProcessing.theTimestamp);
	inProcessing.timestamp_previous = -1;
	inProcessing.deltaT_theory_us = (jvxData)_common_set_ldslave.theData_out.con_params.buffersize /
		(jvxData)_common_set_ldslave.theData_out.con_params.rate * 1000.0 * 1000.0;
	inProcessing.deltaT_average_us = 0;

	// Now start the PA stream
	inputParams.device = my_props.id;
	inputParams.channelCount = my_props.deviceInfo->maxInputChannels;
	// inputParams.sampleFormat set before
	inputParams.suggestedLatency =
		my_props.deviceInfo->defaultLowInputLatency * (1.0 - genPortAudio_device::properties_active.preselectlatency.value.val() / 100.0) +
		my_props.deviceInfo->defaultHighInputLatency * (genPortAudio_device::properties_active.preselectlatency.value.val() / 100.0);
	inputParams.hostApiSpecificStreamInfo = NULL;

	outputParams.device = my_props.id;
	outputParams.channelCount = my_props.deviceInfo->maxOutputChannels;
	outputParams.sampleFormat = inputParams.sampleFormat;
	outputParams.suggestedLatency = //this->deviceInfo->defaultLowOutputLatency;
		my_props.deviceInfo->defaultLowOutputLatency * (1.0 - genPortAudio_device::properties_active.preselectlatency.value.val() / 100.0) +
		my_props.deviceInfo->defaultHighOutputLatency * (genPortAudio_device::properties_active.preselectlatency.value.val() / 100.0);
	outputParams.hostApiSpecificStreamInfo = NULL;

	// Distinguish between duplex and half duplex devices
	if (my_props.deviceInfo->maxInputChannels == 0)
	{
		argIn = NULL;
	}
	else
	{
		argIn = &inputParams;
	}
	if (my_props.deviceInfo->maxOutputChannels == 0)
	{
		argOut = NULL;
	}
	else
	{
		argOut = &outputParams;
	}

	// Open the stream
	// For WASAPI, we may switch from Shared mode to Exclusive mode here
	// As it seems some devices do not really work in exclusive mode and
	// other problems are reported, therefore by default we do not use the
	// exclusive mode
	/*
	if (hostInfo->type == paWASAPI)
	{
		wasapiInfo.size = sizeof(PaWasapiStreamInfo);
		wasapiInfo.hostApiType = paWASAPI;
		wasapiInfo.version = 1;
		wasapiInfo.flags = (paWinWasapiExclusive | paWinWasapiThreadPriority);
		wasapiInfo.threadPriority = eThreadPriorityProAudio;

		if (argIn)
		{
			argIn->hostApiSpecificStreamInfo = (&wasapiInfo);
		}
	}
	*/

	err = Pa_OpenStream(&inProcessing.theStream, argIn, argOut,
		_inproc.samplerate, _inproc.buffersize, 0, // (1 << 0)/* paWinWasapiExclusive*/,
		theCallback, this);

	// Check wether that was successful
	if (err != paNoError)
	{
		resL = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(resL == JVX_NO_ERROR);
		res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
	}

	return res;

leave_error:

	return(res);
}

// ========================================================================

jvxErrorType
CjvxPortAudioDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize numNotDeallocated = 0;
	jvxErrorType resL;
	jvxErrorType res = JVX_NO_ERROR;

	if ((_common_set_ld_master.state == JVX_STATE_PREPARED) && (_common_set_ld_master.isConnected))
	{
		if (inProcessing.theStream)
		{
			PaError err = Pa_CloseStream(inProcessing.theStream);
			assert(err == paNoError);
		}
		inProcessing.theStream = NULL;

		res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));

		assert(res == JVX_NO_ERROR);

		if (runtime.threads.coreHdl.hdl)
		{
			if (runtime.threads.syncAudioThreadId >= 0)
			{
				resL = runtime.threads.coreHdl.hdl->unregister_tc_thread(runtime.threads.syncAudioThreadId);
				runtime.threads.syncAudioThreadAssoc = false;
				runtime.threads.syncAudioThreadId = -1;
			}

			resL = runtime.threads.theToolsHost->return_reference_tool(JVX_COMPONENT_THREADCONTROLLER, runtime.threads.coreHdl.object);
			runtime.threads.coreHdl.object = NULL;
			runtime.threads.coreHdl.hdl = NULL;
		}
		if (runtime.threads.theToolsHost)
		{
			resL = _common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(runtime.threads.theToolsHost));
			runtime.threads.theToolsHost = NULL;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	// If the chain is postprocessed, the object itself should also be postprocessed if not done so before
	resL = postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;
}


jvxErrorType
CjvxPortAudioDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	PaError err = paNoError;
	
	// If the chain is started, the object itself should also be started if not done so before
	resL = start_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	res = CjvxConnectionMaster::_start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		resL = CjvxConnectionMaster::_stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(resL == JVX_NO_ERROR);
		goto leave_error;
	}
	genPortAudio_device::properties_active_higher.loadpercent.value = 0;
	genPortAudio_device::properties_active_higher.loadpercent.isValid = true;
	
	*_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr = 0;

	err = Pa_StartStream(inProcessing.theStream);
	if (err != paNoError)
	{
		res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
	}
	return res;

leave_error:
	return res;
}

jvxErrorType
CjvxPortAudioDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	PaError err = Pa_StopStream(inProcessing.theStream);
	if (err != paNoError)
	{
		res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
	}

	res = CjvxConnectionMaster::_stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Stopping chain failed." << std::endl;
	}

	// If the chain is stopped, the object itself should also be stopped if not done so before
	resL = stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;
}

jvxErrorType
CjvxPortAudioDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	// Overwrite the specified settings and start reverse connect
	_common_set_ldslave.theData_in->con_params.buffersize = _inproc.buffersize;
	_common_set_ldslave.theData_in->con_params.format = _inproc.format;
	_common_set_ldslave.theData_in->con_data.buffers = NULL;
	_common_set_ldslave.theData_in->con_data.number_buffers = JVX_MAX(1, _common_set_ldslave.theData_in->con_data.number_buffers);
	_common_set_ldslave.theData_in->con_params.number_channels = _inproc.numOutputs;
	_common_set_ldslave.theData_in->con_params.rate = _inproc.samplerate;

	// Connect the output side and start this link
	res = allocate_pipeline_and_buffers_prepare_to();

	// Do not attach any user hint into backward direction
	_common_set_ldslave.theData_in->con_compat.user_hints = NULL;
	*_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr = 0;

	return res;
}

jvxErrorType
CjvxPortAudioDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxPortAudioDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list
	return res;
}

jvxErrorType
CjvxPortAudioDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = shift_buffer_pipeline_idx_on_start( pipeline_offset,  idx_stage, tobeAccessedByStage,
		clbk, priv_ptr);
	return res;
}

jvxErrorType
CjvxPortAudioDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Unlock the buffer for this pipeline
	res = shift_buffer_pipeline_idx_on_stop( idx_stage,  shift_fwd, tobeAccessedByStage, clbk, priv_ptr);

	return res;
}

// ===================================================================================

jvxErrorType 
CjvxPortAudioDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	// Call a specific version of this function to find a parameter setting
	if (tp == JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS)
	{
		res = CjvxAudioDevice::transfer_backward_ocon_match_setting(tp, data, &genPortAudio_device::properties_active.ratesselection, &genPortAudio_device::properties_active.sizesselection JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
	}
	return CjvxAudioDevice::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

