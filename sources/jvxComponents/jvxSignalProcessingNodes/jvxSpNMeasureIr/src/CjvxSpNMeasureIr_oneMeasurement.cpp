#include "CjvxSpNMeasureIr_oneMeasurement.h"

// jvx_generatorWave theGenerator;
CjvxSpNMeasureIr_oneMeasurement::CjvxSpNMeasureIr_oneMeasurement()
{
	currentStatus = JVX_ONE_MEASUREMENT_WAIT;
	description = "undefined";

	generator.tpWave = JVX_GENERATOR_WAVE_LOGSWEEP;
	generator.seed_noise = 0;
	generator.amplitude = 0.1;
	generator.noise_type = JVX_GENERATOR_NOISE_UNIFORM;
	generator.silence_start_seconds = 1.0;
	generator.silence_stop_seconds = 1.0;
	generator.freq_low_hz_start = 0.0;
	generator.freq_low_hz = 0.0;
	generator.freq_up_hz = 100000.0;
	generator.freq_up_hz_stop = 100000.0;
	generator.length_seconds = 5.0;
	generator.loop_count = 2;

	evaluation.skip_eval = 0;
	evaluation.derive_ir = true;
	evaluation.store_data = true;

	internal_buffersize = JVX_WAV_READ_WRITE_INTERNAL_BSIZE;
	rate = 48000;

	freq_buf = NULL;
	gain_buf = NULL;
	freq_gain_len = 0;

	measurementComplete = false;
};

void 
CjvxSpNMeasureIr_oneMeasurement::allocate_one_measurement(jvxSize rate_measure, jvxSize buffersize_store,
	jvxSize* numResults)
{
	jvxSize lSimu = 0;
	jvxDspBaseErrorType resDsp;
	theGenerator.tpWave = generator.tpWave;

	if (numResults)
		*numResults = 0;

	internal_buffersize = buffersize_store;
	rate = rate_measure;

	resDsp = jvx_generatorwave_initConfig(&theGenerator);
	assert(resDsp == JVX_DSP_NO_ERROR);

	switch (theGenerator.tpWave)
	{
	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
		theGenerator.prm_init.noiseWave.common.samplerate = (jvxInt32)rate;
		theGenerator.prm_init.noiseWave.seed = generator.seed_noise;
		// theGenerator.prm_sync.
		break;
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
		theGenerator.prm_init.perfSweepWave.common.samplerate = (jvxInt32)rate;
		break;

	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:

		theGenerator.prm_init.linlogSweepWave.common.samplerate = (jvxInt32)rate;
		break;
	default:
		break;
	}

	resDsp = jvx_generatorwave_activate(&theGenerator);
	assert(resDsp == JVX_DSP_NO_ERROR);

	switch (theGenerator.tpWave)
	{
	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:

		theGenerator.prm_async.noiseWave.common.amplitude = generator.amplitude;
		theGenerator.prm_async.noiseWave.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
		theGenerator.prm_async.noiseWave.noiseType = generator.noise_type;

		// theGenerator.prm_sync.
		break;
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:

		theGenerator.prm_async.perfSweepWave.common.amplitude = generator.amplitude;
		theGenerator.prm_async.perfSweepWave.lengthSeconds = generator.length_seconds;
		theGenerator.prm_async.perfSweepWave.loopCount = (jvxInt32)generator.loop_count;
		theGenerator.prm_async.perfSweepWave.silenceStartSeconds = generator.silence_start_seconds;
		theGenerator.prm_async.perfSweepWave.silenceStopSeconds = generator.silence_stop_seconds;

		break;

	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:

		theGenerator.prm_async.linlogSweepWave.common.amplitude = generator.amplitude;
		theGenerator.prm_async.linlogSweepWave.lengthSeconds = generator.length_seconds;
		theGenerator.prm_async.linlogSweepWave.loopCount = (jvxInt32)generator.loop_count;
		theGenerator.prm_async.linlogSweepWave.silenceStartSeconds = generator.silence_start_seconds;
		theGenerator.prm_async.linlogSweepWave.silenceStopSeconds = generator.silence_stop_seconds;
		theGenerator.prm_async.linlogSweepWave.fLow = generator.freq_low_hz;
		theGenerator.prm_async.linlogSweepWave.fUp = generator.freq_up_hz;
		theGenerator.prm_async.linlogSweepWave.fLowStart = generator.freq_low_hz_start;
		theGenerator.prm_async.linlogSweepWave.fUpStop = generator.freq_up_hz_stop;
		break;
	default:
		break;
	}

	resDsp = jvx_generatorwave_update(&theGenerator, JVX_DSP_UPDATE_ASYNC, c_true);
	assert(resDsp == JVX_DSP_NO_ERROR);

	resDsp = jvx_generatorwave_prepare(&theGenerator);
	assert(resDsp == JVX_DSP_NO_ERROR);

	resDsp = jvx_generatorwave_update(&theGenerator, JVX_DSP_UPDATE_SYNC, c_false);
	assert(resDsp == JVX_DSP_NO_ERROR);

	switch (theGenerator.tpWave)
	{
	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:

		// theGenerator.prm_sync.
		break;
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
		lSimu = theGenerator.prm_sync.perfSweepWave.buffered.length;
		break;

	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:

		lSimu = theGenerator.prm_sync.linlogSweepWave.unbuffered.length;
		freq_gain_len = lSimu;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(freq_buf, jvxData, freq_gain_len);
		theGenerator.prm_sync.linlogSweepWave.unbuffered.freq_ptr = freq_buf;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(gain_buf, jvxData, freq_gain_len);
		theGenerator.prm_sync.linlogSweepWave.unbuffered.gain_ptr = gain_buf;
		jvx_generatorwave_update(&theGenerator, JVX_DSP_UPDATE_SYNC, c_true);
		break;

	default:
		break;
	}

	// Start measurement tool
	auto elm = inputs.begin();
	for (; elm != inputs.end(); elm++)
	{
		if (lSimu > 0)
		{
			elm->second.offBuf = 0;

			elm->second.oneMeasure.prm_init.numReps = generator.loop_count;
			elm->second.oneMeasure.prm_init.szTestsignal_onerep = lSimu;
			elm->second.oneMeasure.prm_init.computeIr = evaluation.derive_ir;
			elm->second.oneMeasure.prm_init.computeXc = c_false;
			elm->second.oneMeasure.prm_init.skipNumber = evaluation.skip_eval;
			elm->second.oneMeasure.prm_init.storeSuperPosed = c_true;

			resDsp = jvx_measure_ir_prepare(&elm->second.oneMeasure);
			assert(resDsp == JVX_DSP_NO_ERROR);

			resDsp = jvx_measure_ir_update(&elm->second.oneMeasure, JVX_DSP_UPDATE_SYNC, false);
			assert(resDsp == JVX_DSP_NO_ERROR);

			if (numResults)
			{
			  (*numResults)++;
			}
		}
	}
}

void
CjvxSpNMeasureIr_oneMeasurement::deallocate_one_measurement()
{
	jvxDspBaseErrorType resDsp;

	auto elm = inputs.begin();
	for (; elm != inputs.end(); elm++)
	{
		if (elm->second.oneMeasure.prv)
		{
			resDsp = jvx_measure_ir_postprocess(&elm->second.oneMeasure);
			assert(resDsp == JVX_DSP_NO_ERROR);
		}
	}

	switch (theGenerator.tpWave)
	{
	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:
		theGenerator.prm_sync.linlogSweepWave.unbuffered.freq_ptr = NULL;
		theGenerator.prm_sync.linlogSweepWave.unbuffered.gain_ptr = NULL;
		jvx_generatorwave_update(&theGenerator, JVX_DSP_UPDATE_SYNC, c_true);
		JVX_DSP_SAFE_DELETE_FIELD(freq_buf);
		JVX_DSP_SAFE_DELETE_FIELD(gain_buf);
		freq_gain_len = 0;
		break;
	}
	resDsp = jvx_generatorwave_postprocess(&theGenerator);
	resDsp = jvx_generatorwave_deactivate(&theGenerator);
	
}

// Evaluate the measurement
void
CjvxSpNMeasureIr_oneMeasurement::evaluate_measurement()
{
	auto elm = inputs.begin();
	for (; elm != inputs.end(); elm++)
	{
		if (evaluation.derive_ir)
		{
			elm->second.oneMeasure.prm_sync.ext.freq_buf = freq_buf;
			elm->second.oneMeasure.prm_sync.ext.gain_buf = gain_buf;
			elm->second.oneMeasure.prm_sync.ext.freq_gain_len = freq_gain_len;
			elm->second.oneMeasure.prm_sync.ext.sRate = this->rate;
			jvx_measure_ir_update(&elm->second.oneMeasure, JVX_DSP_UPDATE_SYNC, c_true);
			jvx_measure_ir_process(&elm->second.oneMeasure);
		}
	}
}

jvxErrorType 
CjvxSpNMeasureIr_oneMeasurement::copy_measurement_channel(jvxSize cnt, oneMeasurementChannel* oneChan)
{
	jvxSize cntLoc = 0;
	auto elm = inputs.begin();
	for (; elm != inputs.end(); elm++)
	{
		if (cnt == cntLoc)
		{
			if (oneChan)
			{
				if (oneChan->lBuf == 0)
				{
					oneChan->lBuf = elm->second.oneMeasure.prm_sync.lIResponse;
				}
				else
				{
					oneChan->validFlds = 0;
					oneChan->rate = this->rate;
					if (oneChan->bufIr)
					{
						jvxSize ml = JVX_MIN(oneChan->lBuf, elm->second.oneMeasure.prm_sync.lIResponse);
						if (ml)
						{
							memcpy(oneChan->bufIr, elm->second.oneMeasure.prm_sync.fldIResponse, sizeof(jvxData) * ml);
							oneChan->validFlds |= JVX_FLAG_IR_VALID;
						}
					}
					if (oneChan->bufMeas)
					{
						jvxSize ml = JVX_MIN(oneChan->lBuf, elm->second.oneMeasure.prm_sync.lIResponse);
						if (ml)
						{
							memcpy(oneChan->bufMeas, elm->second.oneMeasure.prm_sync.fldSPosedMeasure, sizeof(jvxData) * ml);
							oneChan->validFlds |= JVX_FLAG_MEAS_VALID;
						}
					}
					if (oneChan->bufTest)
					{
						jvxSize ml = JVX_MIN(oneChan->lBuf, elm->second.oneMeasure.prm_sync.lIResponse);
						if (ml)
						{
							memcpy(oneChan->bufTest, elm->second.oneMeasure.prm_sync.fldSPosedTest, sizeof(jvxData) * ml);
							oneChan->validFlds |= JVX_FLAG_TEST_VALID;
						}
					}
				}
				oneChan->nmChan.assign(JVX_COMPOSE_CHANNEL_NAME(elm->second.id));
			}
			return JVX_NO_ERROR;
		}
		cntLoc++;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxSpNMeasureIr_oneMeasurement::copy_measurement(oneMeasurementResult* result)
{	
	if (freq_buf)
	{
		if (result->freq_buf == NULL)
		{
			result->freq_gain_len = freq_gain_len;
		}
		else
		{
			jvxSize llmin = JVX_MIN(result->freq_gain_len, freq_gain_len);
			memcpy(result->freq_buf, freq_buf, sizeof(jvxData) * llmin);
			memcpy(result->gain_buf, gain_buf, sizeof(jvxData) * llmin);
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

void
CjvxSpNMeasureIr_oneMeasurement::write_data(const std::string& token,  IjvxRtAudioFileWriter* fWriter, IjvxConfigProcessor* proc,
	IjvxSpNMeasureIr_oneMeasurement_report* rep)
{
	jvxSize cnt = 0;
	jvxErrorType res = JVX_NO_ERROR;

	std::string fName_postfix = this->description;
	fName_postfix = jvx_replaceCharacter(fName_postfix, ' ', '_');

	if(evaluation.store_data)
	{
		if (!pathStoreFolder.empty())
		{
			JVX_CREATE_DIRECTORY_RESULT resD = JVX_CREATE_DIRECTORY(pathStoreFolder.c_str());
			if (resD == JVX_CREATE_DIRECTORY_SUCCESS)
			{
				jvxConfigData* datSec = NULL;
				proc->createEmptySection(&datSec, "ONE_MEASUREMENT");
				if (datSec)
				{
					jvxConfigData* datEntry = NULL;
					std::string tokenLoc;
					JVX_GET_CONFIGURATION_INJECT_STRING(res, "DESCRIPTION", datSec, this->description, proc);
					JVX_GET_CONFIGURATION_INJECT_STRING(res, "TOKEN", datSec, token, proc);
					JVX_GET_CONFIGURATION_INJECT_VALUE(res, "RATE", datSec, this->rate, proc);
					JVX_GET_CONFIGURATION_INJECT_VALUE(res, "AMPLITUDE", datSec, this->generator.amplitude, proc);
					JVX_GET_CONFIGURATION_INJECT_VALUE(res, "FREQUENCY_LOW", datSec, this->generator.freq_low_hz, proc);
					JVX_GET_CONFIGURATION_INJECT_VALUE(res, "FREQUENCY_UP", datSec, this->generator.freq_up_hz, proc);
					JVX_GET_CONFIGURATION_INJECT_VALUE(res, "LENGTH_SECS", datSec, this->generator.length_seconds, proc);
					JVX_GET_CONFIGURATION_INJECT_VALUE(res, "START_SIL_SECS", datSec, this->generator.silence_start_seconds, proc);
					JVX_GET_CONFIGURATION_INJECT_VALUE(res, "STOP_SIL_SECS", datSec, this->generator.silence_stop_seconds, proc);
					JVX_GET_CONFIGURATION_INJECT_VALUE(res, "NUM_ITERATIONS", datSec, this->generator.loop_count, proc);
					JVX_GET_CONFIGURATION_INJECT_VALUE(res, "SKIP_ITERATIONS", datSec, this->evaluation.skip_eval, proc);

					switch (this->generator.tpWave)
					{
					case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:
						tokenLoc = "SINE";
						break;
					case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:
						tokenLoc = "RECT";
						break;
					case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:
						tokenLoc = "RAMP";
						break;
					case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
						tokenLoc = "NOISE";
						break;
					case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
						tokenLoc = "PERFECTSWEEP";
						break;
					case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:
						tokenLoc = "WAVPLAYER";
						break;
					case JVX_GENERATOR_WAVE_LOGSWEEP:
						tokenLoc = "LOGSWEEP";
						break;
					case JVX_GENERATOR_WAVE_LINEARSWEEP:
						tokenLoc = "LINSWEEP";
						break;
					}
					JVX_GET_CONFIGURATION_INJECT_STRING(res, "GENERATOR_WAVE", datSec, tokenLoc, proc);

					datEntry = NULL;
					switch (this->generator.noise_type)
					{
					case JVX_GENERATOR_NOISE_NORMAL:
						tokenLoc = "NORMAL";
						break;
					case JVX_GENERATOR_NOISE_UNIFORM:
						tokenLoc = "UNIFORM";
						break;
					}
					JVX_GET_CONFIGURATION_INJECT_STRING(res, "NOISE_TYPE", datSec, tokenLoc, proc);

					auto elm = inputs.begin();
					for (; elm != inputs.end(); elm++)
					{
						jvxConfigData* datSubSec = NULL;
						std::string nmChan = "INPUT_" + jvx_size2String(cnt);

						if (rep)
						{
							std::string txt = "Writing input ";
							txt += jvx_size2String(cnt);
							txt += "/";
							txt += jvx_size2String(inputs.size());
							txt += ".";
							rep->jvxSpNMeasureIr_reportText(txt);
						}

						cnt++;
						proc->createEmptySection(&datSubSec, nmChan.c_str());

						std::string channelName = JVX_COMPOSE_CHANNEL_NAME(elm->second.id);
						datEntry = NULL;
						proc->createAssignmentString(&datEntry, "CHANNELNAME", channelName.c_str());
						proc->addSubsectionToSection(datSubSec, datEntry);

						if (fWriter)
						{
							std::string storeName = fName_postfix + "_" + channelName + "_sig_test" + ".wav";
							std::string token = "SIGNAL_DATA_TEST";
							if (rep)
							{
								std::string txt = "Writing file <";
								txt += pathStoreFolder + JVX_SEPARATOR_DIR + storeName;
								txt += ">.";
								rep->jvxSpNMeasureIr_reportText(txt);
							}
							writeOneFile(storeName, pathStoreFolder, token,
								elm->second.oneMeasure.prm_sync.fldBufferTestSignal, 
								elm->second.oneMeasure.prm_sync.lRecording,
								proc, datSubSec, rate, fWriter);

							storeName = fName_postfix + "_" + channelName + "_sig_measure" + ".wav";
							token = "SIGNAL_DATA_MEASURE";
							if (rep)
							{
								std::string txt = "Writing file <";
								txt += pathStoreFolder + JVX_SEPARATOR_DIR + storeName;
								txt += ">.";
								rep->jvxSpNMeasureIr_reportText(txt);
							}
							writeOneFile(storeName, 
								pathStoreFolder,
								token,
								elm->second.oneMeasure.prm_sync.fldBufferMeasureSignal,
								elm->second.oneMeasure.prm_sync.lRecording,
								proc, datSubSec, rate, fWriter);

							if (evaluation.derive_ir)
							{
								storeName = fName_postfix + "_" + channelName + "_proc_ir" + ".wav";
								token = "PROCESSED_DATA_IR";
								if (rep)
								{
									std::string txt = "Writing file <";
									txt += pathStoreFolder + JVX_SEPARATOR_DIR + storeName;
									txt += ">.";
									rep->jvxSpNMeasureIr_reportText(txt);
								}
								writeOneFile(storeName,
									pathStoreFolder,
									token,
									elm->second.oneMeasure.prm_sync.fldIResponse,
									elm->second.oneMeasure.prm_sync.lIResponse,
									proc, datSubSec, rate, fWriter);

								storeName = fName_postfix + "_" + channelName + "_proc_test" + ".wav";
								token = "PROCESSED_DATA_TEST";
								if (rep)
								{
									std::string txt = "Writing file <";
									txt += pathStoreFolder + JVX_SEPARATOR_DIR + storeName;
									txt += ">.";
									rep->jvxSpNMeasureIr_reportText(txt);
								}
								writeOneFile(storeName, 
									pathStoreFolder,
									token,
									elm->second.oneMeasure.prm_sync.fldSPosedTest,
									elm->second.oneMeasure.prm_sync.lIResponse,
									proc, datSubSec, rate, fWriter);

								storeName = fName_postfix + "_" + channelName + "_proc_measure" + ".wav";
								token = "PROCESSED_DATA_MEASURE";
								if (rep)
								{
									std::string txt = "Writing file <";
									txt += storeName;
									txt += ">.";
									rep->jvxSpNMeasureIr_reportText(txt);
								}
								writeOneFile(storeName, 
									pathStoreFolder,
									token,
									elm->second.oneMeasure.prm_sync.fldSPosedMeasure,
									elm->second.oneMeasure.prm_sync.lIResponse,
									proc, datSubSec, rate, fWriter);
							} // if (evaluation.derive_ir)
						} // if (fWriter)

						proc->addSubsectionToSection(datSec, datSubSec);
					} // if (!pathStore.empty())

					if (fWriter)
					{
						if (freq_gain_len)
						{
							if (rep)
							{
								std::string txt = "Writing frequency log.";
								rep->jvxSpNMeasureIr_reportText(txt);
							}
							std::string storeName = fName_postfix + "_freq" + ".wav";
							std::string token = "SIGNAL_FREQ";
							if (rep)
							{
								std::string txt = "Writing file <";
								txt += pathStoreFolder + JVX_SEPARATOR_DIR + storeName;
								txt += ">.";
								rep->jvxSpNMeasureIr_reportText(txt);
							}
							writeOneFile(storeName, 
								pathStoreFolder,
								token,
								freq_buf,
								freq_gain_len,
								proc, datSec, rate, fWriter);
							if (rep)
							{
								std::string txt = "Writing frequency log.";
								rep->jvxSpNMeasureIr_reportText(txt);
							}

							storeName = fName_postfix + "_gain" + ".wav";
							token = "SIGNAL_GAIN";
							if (rep)
							{
								std::string txt = "Writing file <";
								txt += pathStoreFolder + JVX_SEPARATOR_DIR + storeName;
								txt += ">.";
								rep->jvxSpNMeasureIr_reportText(txt);
							}
							writeOneFile(storeName,
								pathStoreFolder,
								token,
								gain_buf,
								freq_gain_len,
								proc, datSec, rate, fWriter);
						}
					}

					jvxApiString outStr;
					proc->printConfiguration(datSec, &outStr, false);
					pathStoreProto = pathStoreFolder + JVX_SEPARATOR_DIR + "measure_protocol.mvx";
					if (rep)
					{
						std::string txt = "Writing protocol file <";
						txt += pathStoreProto;
						txt += ">.";
						rep->jvxSpNMeasureIr_reportText(txt);
					}
					jvxErrorType resF = jvx_writeContentToFile(pathStoreProto, outStr.std_str());
					if (resF != JVX_NO_ERROR)
					{
						std::cout << "Failed to write file <" << pathStoreProto << ">." << std::endl;
					}

					proc->removeHandle(datSec);
					
					// Store the protocol filename to be stored in config later. In the next session, the
					// proper protocol file will be loaded to reconsruct latest settings
					
				} // if (datSec)
				datSec = NULL;
			} // if (!pathStore.empty())
		}		
	}
}

jvxErrorType 
CjvxSpNMeasureIr_oneMeasurement::read_measurement(
	oneMeasurementResult* oneResult, 
	const std::string& fNameNew, IjvxRtAudioFileReader* theReader,
	IjvxConfigProcessor* confHdl,
	jvxSize internal_bsize)
{
	std::string cont;
	jvxErrorType res = jvx_readContentFromFile(fNameNew, cont);
	if (res == JVX_NO_ERROR)
	{		
		std::string pathExpr = jvx_pathExprFromFilePath(fNameNew, JVX_SEPARATOR_DIR_CHAR);
		res = confHdl->parseTextField(cont.c_str(), fNameNew.c_str(), 0);
		if (res == JVX_NO_ERROR)
		{
			jvxConfigData* datSec = NULL;
			jvxSize cnt = 0;
			oneMeasurementResult resMeas;

			resMeas.pathStoreProto = fNameNew;
			resMeas.pathStoreFolder = pathExpr;
			confHdl->getConfigurationHandle(&datSec);
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "RATE", datSec, &resMeas.rate, confHdl);
			JVX_PUT_CONFIGURATION_EXTRACT_STRING(res, "DESCRIPTION", datSec, resMeas.nameMeasure, confHdl);
			JVX_PUT_CONFIGURATION_EXTRACT_STRING(res, "TOKEN", datSec, resMeas.tokenMeasure, confHdl);

			while (1)
			{
				jvxConfigData* datSubSec = NULL;
				std::string nmChan = "INPUT_" + jvx_size2String(cnt);
				cnt++;

				res = confHdl->getReferenceEntryCurrentSection_name(datSec, &datSubSec, nmChan.c_str());
				if (res == JVX_NO_ERROR)
				{
					oneMeasurementChannel oneChan;
					std::string channelName;

					JVX_PUT_CONFIGURATION_EXTRACT_STRING(res, "CHANNELNAME", datSubSec,
						channelName, confHdl);

					oneChan.validFlds = 0;
					if (theReader)
					{
						res = readOneFile_allocate("PROCESSED_DATA_IR", pathExpr, &oneChan.lBuf, &oneChan.bufIr, &oneChan.rate, confHdl, datSubSec, theReader, internal_bsize);
						oneChan.validFlds |= JVX_FLAG_IR_VALID;

						if (res == JVX_NO_ERROR)
						{
							res = readOneFile_allocate("PROCESSED_DATA_MEASURE", pathExpr , &oneChan.lBuf, &oneChan.bufMeas, &oneChan.rate, confHdl, datSubSec, theReader, internal_bsize);
							oneChan.validFlds |= JVX_FLAG_MEAS_VALID;
							if (res == JVX_NO_ERROR)
							{
								res = readOneFile_allocate("PROCESSED_DATA_TEST", pathExpr , &oneChan.lBuf, &oneChan.bufTest, &oneChan.rate, confHdl, datSubSec, theReader, internal_bsize);
								oneChan.validFlds |= JVX_FLAG_TEST_VALID;
							}
						}
					}
					else
					{
						res = JVX_ERROR_NOT_READY;
					}
					if (res == JVX_NO_ERROR)
					{
						oneChan.nmChan.assign(channelName);
						oneChan.nmMeas.assign(resMeas.nameMeasure);
						resMeas.resultsChannel.push_back(oneChan);
					}
					else
					{
						std::cout << "-- Failed to read channel <" << channelName << ">." << std::endl;
						break;
					}
					oneChan.bufIr = NULL;
					oneChan.bufMeas = NULL;
					oneChan.bufTest = NULL;
					oneChan.lBuf = 0;
					oneChan.nmChan.clear();
					oneChan.nmMeas.clear();
					oneChan.validFlds = 0;
					oneChan.rate = 0;
				}
				else
				{
					res = JVX_NO_ERROR; // <- normal stop: no more channels
					break;
				}
			}

			jvxSize rate = 0;
			res = readOneFile_allocate("SIGNAL_FREQ", pathExpr, &resMeas.freq_gain_len, &resMeas.freq_buf, &rate, confHdl, datSec, theReader, internal_bsize);
			rate = 0;
			res = readOneFile_allocate("SIGNAL_GAIN", pathExpr, &resMeas.freq_gain_len, &resMeas.gain_buf, &rate, confHdl, datSec, theReader, internal_bsize);

			confHdl->removeHandle(datSec);
			if (oneResult)
			{
				*oneResult = resMeas;
			}
		}
		

	}
	return res;
}

void 
CjvxSpNMeasureIr_oneMeasurement::addData(
	jvxData** bufsOut, 
	jvxSize numChansOut,
	jvxData** bufsIn,
	jvxSize numChansIn,
	jvxSize off,
	jvxSize* numW, 
	jvxSize bsize,
	jvxBool& src_complete,
	jvxData* progress_loop,
	jvxData* progress_in_period)
{
	jvxSize firstBuf = JVX_SIZE_UNSELECTED;
	jvxSize ncopy = bsize - off;
	if (numW)
		*numW = 0;

	// We do not need to continue if there is no output
	if (outputs.size() == 0)
	{
		src_complete = true;
	}

	auto elm = outputs.begin();
	for (; elm != outputs.end(); elm++)
	{
		jvxSize idxTo = elm->second.id;
		if (
			(idxTo < numChansOut) &&
			(elm->second.active))
		{
			jvxData* ptrTo = bufsOut[idxTo] + off;
			if (JVX_CHECK_SIZE_UNSELECTED(firstBuf))
			{
				jvx_generatorwave_process(&theGenerator, ptrTo, ncopy, numW);

				jvx_generatorwave_update(&theGenerator, JVX_DSP_UPDATE_USER_OFFSET | JVX_DSP_UPDATE_ASYNC, c_false);
				switch (theGenerator.tpWave)
				{
				case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
					if (progress_loop)
					{
						*progress_loop = (jvxData)theGenerator.prm_sync.perfSweepWave.itCount / (jvxData)theGenerator.prm_async.perfSweepWave.loopCount;
					}
					if (progress_in_period)
					{
						*progress_in_period = theGenerator.prm_sync.perfSweepWave.buffered.progress;
					}
					src_complete = (theGenerator.prm_async.perfSweepWave.common.theStatus == JVX_GENERATOR_WAVE_COMPLETE);
					break;
				case JVX_GENERATOR_WAVE_LINEARSWEEP:
				case JVX_GENERATOR_WAVE_LOGSWEEP:
					if (progress_loop)
					{
						*progress_loop = (jvxData)theGenerator.prm_sync.linlogSweepWave.itCount / (jvxData)theGenerator.prm_async.linlogSweepWave.loopCount;
					}
					if (progress_in_period)
					{
						*progress_in_period = theGenerator.prm_sync.linlogSweepWave.unbuffered.progress;
					}
					src_complete = (theGenerator.prm_async.linlogSweepWave.common.theStatus == JVX_GENERATOR_WAVE_COMPLETE);
					break;
				}
				firstBuf = idxTo;

				if (*numW > 0)
				{
					auto elmI = inputs.begin();
					for (; elmI != inputs.end(); elmI++)
					{
						if (elmI->second.oneMeasure.prv)
						{
							jvxData* ptrToI = elmI->second.oneMeasure.prm_sync.fldBufferTestSignal;
							ptrToI += elmI->second.offBuf;
							assert((elmI->second.offBuf + *numW) <= elmI->second.oneMeasure.prm_sync.lRecording);
							memcpy(ptrToI, ptrTo, sizeof(jvxData) * *numW);
							ptrToI = elmI->second.oneMeasure.prm_sync.fldBufferMeasureSignal;
							ptrToI += elmI->second.offBuf;
							if (elmI->second.id < numChansIn)
							{
								jvxData* ptrFrom = bufsIn[elmI->second.id] + off;
								memcpy(ptrToI, ptrFrom, sizeof(jvxData) * *numW);
							}
							elmI->second.offBuf += ncopy;
						}
					}
				}
			}
			else
			{
				jvxData* ptrFrom = bufsOut[firstBuf] + off;
				memcpy(ptrTo, ptrFrom, sizeof(jvxData) * ncopy);
			}
		}
	}	
}

jvxErrorType
CjvxSpNMeasureIr_oneMeasurement::writeOneFile(
	const std::string& storeName, 
	const std::string& pathStore,
	const std::string& cf_token_prefix,
	jvxData* fldBufferTestSignal, 
	jvxSize lRecording, IjvxConfigProcessor* proc, 
	jvxConfigData* datSubSec,
	jvxSize rate, 
	IjvxRtAudioFileWriter* fWriter)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvxConfigData* datEntry = NULL;
	std::string pathStoreName = pathStore + JVX_SEPARATOR_DIR + storeName;
	jvxData* copy_buf = NULL;
	jvxData normalizeFactor = 1.0;
	jvxData maxAbs = 0;
	jvxEndpointClass cls = JVX_FILETYPE_WAV;
	jvxFileDescriptionEndpoint_open fileD;
	jvxFileDescriptionEndpoint_prepare fileP;
	jvxSize stillCopy = 0;
	jvxData* ptrOut = NULL;


	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(copy_buf, jvxData, lRecording);
	for (i = 0; i < lRecording; i++)
	{
		copy_buf[i] = fldBufferTestSignal[i];
		if (fabs(copy_buf[i]) > maxAbs)
		{
			maxAbs = fabs(copy_buf[i]);
		}
	}
	if (maxAbs > JVX_NORMALIZE_MIN)
	{
		normalizeFactor = JVX_NORMALIZE_MAX_AMP / maxAbs;
	}
	for (i = 0; i < lRecording; i++)
	{
		copy_buf[i] *= normalizeFactor;
	}

	proc->createAssignmentString(&datEntry, (cf_token_prefix + "_WAV").c_str(), storeName.c_str());
	proc->addSubsectionToSection(datSubSec, datEntry);

	proc->createAssignmentValue(&datEntry, (cf_token_prefix + "_NORM").c_str(), normalizeFactor);
	proc->addSubsectionToSection(datSubSec, datEntry);

	cls = JVX_FILETYPE_WAV;

	fileD.fFormatFile = JVX_FILEFORMAT_IEEE_FLOAT;
	fileD.numberBitsSample = 32;
	fileD.numberChannels = 1;
	fileD.samplerate = (jvxInt32)rate;
	fileD.subtype = JVX_WAV_32;
	fileD.littleEndian = true;
	fileD.lengthFile = 0;
	res = fWriter->activate(pathStoreName.c_str(), cls, &fileD);
	if (res != JVX_NO_ERROR)
	{
		goto exit_error_III;
	}

	fileP.dFormatAppl = JVX_DATAFORMAT_DATA;
	fileP.numSamplesFrameMax_moveBgrd = (jvxInt32)this->internal_buffersize;
	res = fWriter->prepare(fileP.numSamplesFrameMax_moveBgrd, false, &fileP, JVX_FILEOPERATION_BATCH);
	if (res != JVX_NO_ERROR)
	{
		goto exit_error_II;
	}
	fWriter->add_tag(JVX_AUDIO_FILE_TAG_BTFLD_NORM, jvx_data2String(normalizeFactor, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
	res = fWriter->start();
	if (res != JVX_NO_ERROR)
	{
		goto exit_error_I;
	}

	stillCopy = lRecording;
	ptrOut = copy_buf;

	while (stillCopy)
	{
		jvxData* bufs[1];
		bufs[0] = ptrOut;
		jvxSize copyNow = JVX_MIN(stillCopy, this->internal_buffersize);
		res = fWriter->add_audio_buffer((jvxHandle**)bufs, 1, copyNow, false);
		if (res != JVX_NO_ERROR)
		{
			// I saw us end here if harddrive is full
			break;
		}
		stillCopy -= copyNow;
		ptrOut += copyNow;
	}

	res = fWriter->stop();
	assert(res == JVX_NO_ERROR);

	res = fWriter->postprocess();
	assert(res == JVX_NO_ERROR);

	res = fWriter->deactivate();
	assert(res == JVX_NO_ERROR);

	JVX_DSP_SAFE_DELETE_FIELD(copy_buf);
	return res;

exit_error_I:
	res = fWriter->postprocess();
exit_error_II:
	fWriter->deactivate();
exit_error_III:
	JVX_DSP_SAFE_DELETE_FIELD(copy_buf);
	return res;
}

jvxErrorType
CjvxSpNMeasureIr_oneMeasurement::readOneFile_allocate(
	const std::string& tokenPrefix,
	const std::string& pathExpr,
	jvxSize* lBuf, jvxData** buf, jvxSize* rate,
	IjvxConfigProcessor* confHdl,
	jvxConfigData* datSubSec,
	IjvxRtAudioFileReader* theReader,
	jvxSize internal_bsize)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string irName;
	jvxData norm = 0;
	jvxSize rateLoc = 0;
	jvxSize i;
	jvxSize stillCopy = 0;
	jvxData* ptrBuf = NULL;
	jvxEndpointClass cls = JVX_FILETYPE_WAV;
	jvxFileDescriptionEndpoint_open fileD;
	jvxFileDescriptionEndpoint_prepare fileP;

	JVX_PUT_CONFIGURATION_EXTRACT_STRING(res,
		(tokenPrefix + "_WAV").c_str(),
		datSubSec, irName, confHdl);

	if (res != JVX_NO_ERROR)
	{
		goto exit_error_III;
	}
	JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res,
		(tokenPrefix + "_NORM").c_str(),
		datSubSec, &norm, confHdl);
	if (res != JVX_NO_ERROR)
	{
		goto exit_error_III;
	}
	if (irName.empty())
	{
		goto exit_error_III;
	}
	if(pathExpr.empty())
	{
		goto exit_error_III;
	}

	// ===========================================================
	norm = 1.0 / norm;

	if (!pathExpr.empty())
	{
		irName = pathExpr + JVX_SEPARATOR_DIR + irName;
	}

	cls = JVX_FILETYPE_WAV;

	fileD.fFormatFile = JVX_FILEFORMAT_PCM_FIXED;
	fileD.numberBitsSample = 16;
	fileD.numberChannels = 3;
	fileD.samplerate = (jvxInt32)rateLoc;
	fileD.subtype = JVX_WAV_32;
	fileD.littleEndian = true;
	fileD.lengthFile = 0;
	res = theReader->activate(irName.c_str(), cls, &fileD);
	if (res != JVX_NO_ERROR)
	{
		goto exit_error_III;
	}

	fileP.dFormatAppl = JVX_DATAFORMAT_DATA;
	fileP.numSamplesFrameMax_moveBgrd = (jvxInt32)internal_bsize;
	res = theReader->prepare(fileP.numSamplesFrameMax_moveBgrd, JVX_FILECONTINUE_STOPEND, false, &fileP, JVX_FILEOPERATION_BATCH);
	if (res != JVX_NO_ERROR)
	{
		goto exit_error_II;
	}

	res = theReader->start();
	if (res != JVX_NO_ERROR)
	{
		goto exit_error_I;
	}

	if (*lBuf == 0)
	{
		*lBuf = fileD.lengthFile;
	}
	else
	{
		if (*lBuf != fileD.lengthFile)
		{
			res = JVX_ERROR_INVALID_SETTING;
			goto exit_error_I;
		}
	}
	if (*rate == 0)
	{
		*rate = fileD.samplerate;
	}
	else
	{
		if (*rate != fileD.samplerate)
		{
			res = JVX_ERROR_INVALID_SETTING;
			goto exit_error_I;
		}
	}

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(*buf, jvxData, *lBuf);
	stillCopy = *lBuf;
	ptrBuf = *buf;

	while (stillCopy)
	{
		jvxData* bufs[1];
		bufs[0] = ptrBuf;
		jvxSize copyNow = JVX_MIN(stillCopy, internal_bsize);
		res = theReader->get_audio_buffer((jvxHandle**)bufs, 1, copyNow, false);
		if (res != JVX_NO_ERROR)
		{
			assert(res == JVX_ERROR_END_OF_FILE);
		}
		stillCopy -= copyNow;
		ptrBuf += copyNow;
	}

	res = theReader->stop();
	assert(res == JVX_NO_ERROR);

	res = theReader->postprocess();
	assert(res == JVX_NO_ERROR);

	res = theReader->deactivate();
	assert(res == JVX_NO_ERROR);

	ptrBuf = *buf;
	for (i = 0; i < *lBuf; i++)
	{
		ptrBuf[i] *= norm;
	}
	return res;

exit_error_I:
	theReader->postprocess();
exit_error_II:
	theReader->deactivate();
exit_error_III:
	return res;
}
