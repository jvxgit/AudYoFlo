#include "CjvxAudioAndroidDevice.h"
#include "CjvxAudioAndroidTechnology.h"
#include "jvx-helpers-cpp.h"

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
#define ALOG_TAG "jvxAuTAndroid"
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, ALOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, ALOG_TAG, __VA_ARGS__)
#endif

// =============================================================

// #define MEASURE_TIMING
#ifdef MEASURE_TIMING
#define JVX_NUM_EVENTS_OBSERVE 1000
struct oneEntryTimer
{
	jvxData desiredTimeout = 0;
	jvxData measuredTimeout = 0;
	jvxData progressTimerFrames = 0;
	jvxData progressTimerMeasured = 0;
	jvxData progressTimerNext = 0;
	jvxData progressTimerDirect = 0;
	jvxBool runBuffer = false;
};
static oneEntryTimer gTimerLog[JVX_NUM_EVENTS_OBSERVE];
static int gCnt = 0;
#endif

// =============================================================

CjvxAudioAndroidDevice::CjvxAudioAndroidDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_properties.moduleReference = _common_set.theModuleName;

	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_16BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_32BIT_LE);
}

CjvxAudioAndroidDevice::~CjvxAudioAndroidDevice()
{
}

// ============================================================================

void
CjvxAudioAndroidDevice::init(CjvxAudioAndroidTechnology* ptr)
{
	parentTech = ptr;
	numInChannelsMax  = ptr->genAndroid_technology::config_select.init_number_in_channels_max.value;
	numOutChannelsMax = ptr->genAndroid_technology::config_select.init_number_out_channels_max.value;

	// isInput is set by the technology after init() returns
	jvx_bitZSet(_common_set_device.caps.capsFlags,
		(int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT);
	jvx_bitZSet<jvxCBitField16>(_common_set_device.caps.flags,
		(int)jvxDeviceCapabilityFlagsShift::JVX_DEVICE_CAPABILITY_FLAGS_DEFAULT_DEVICE_SHIFT);
	_common_set_device.report = static_cast<IjvxDevice_report*>(parentTech);
}

// ============================================================================

jvxErrorType
CjvxAudioAndroidDevice::activate()
{
	jvxSize i;
	jvxBitField bf;
	jvxErrorType res = CjvxAudioDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		if (isInput)
		{
			// Input device: expose input channels, no output
			jvx_bitFClear(bf);
			for (i = 0; i < numInChannelsMax; i++)
			{
				std::string nm = "Input Channel #" + jvx_size2String(i);
				CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(nm.c_str());
				jvx_bitSet(bf, i);
			}
			CjvxAudioDevice::properties_active.inputchannelselection.value.selection() = bf;
			CjvxAudioDevice::properties_active.numberinputchannels.value = numInChannelsMax;
		}
		else
		{
			// Output device: expose output channels, no input
			jvx_bitFClear(bf);
			for (i = 0; i < numOutChannelsMax; i++)
			{
				std::string nm = "Output Channel #" + jvx_size2String(i);
				CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(nm.c_str());
				jvx_bitSet(bf, i);
			}
			CjvxAudioDevice::properties_active.outputchannelselection.value.selection() = bf;
			CjvxAudioDevice::properties_active.numberoutputchannels.value = numOutChannelsMax;
		}

		CjvxAudioDevice::properties_active.samplerate.value = 48000;
		CjvxAudioDevice::properties_active.buffersize.value = 256;
		CjvxAudioDevice::properties_active.allow_operate_zero_channels.value = c_true;
	}
	return res;
}

jvxErrorType
CjvxAudioAndroidDevice::deactivate()
{
	jvxErrorType res = CjvxAudioDevice::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxAudioDevice::properties_active.buffersize.value = 256;
		CjvxAudioDevice::properties_active.samplerate.value = 48000;
		CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
		CjvxAudioDevice::properties_active.inputchannelselection.value.selection() = 0;
		CjvxAudioDevice::properties_active.inputchannelselection.value.exclusive = 0;
		CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
		CjvxAudioDevice::properties_active.outputchannelselection.value.selection() = 0;
		CjvxAudioDevice::properties_active.outputchannelselection.value.exclusive = 0;

		res = CjvxAudioDevice::deactivate();
	}
	return res;
}

// ==========================================================================

jvxErrorType
CjvxAudioAndroidDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = CjvxAudioDevice::set_property(callGate, rawPtr, ident, trans);
	if (res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		updateDependentVariables_lock(propId, category, false);
	}
	return res;
}

// ==========================================================================

jvxErrorType
CjvxAudioAndroidDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_ERROR_REQUEST_CALL_AGAIN)
	{
		res = CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

// ==========================================================================
// Chain master: prepare / postprocess
// ==========================================================================

jvxErrorType
CjvxAudioAndroidDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Ensure the device itself transitions to PREPARED state if needed
	jvxErrorType resL = prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	// Set output connector params — these describe the data fed into the chain
	_common_set_ocon.theData_out.con_compat.buffersize    = _inproc.buffersize;
	_common_set_ocon.theData_out.con_params.buffersize    = _inproc.buffersize;
	_common_set_ocon.theData_out.con_params.format        = _inproc.format;
	_common_set_ocon.theData_out.con_data.buffers         = NULL;
	_common_set_ocon.theData_out.con_data.number_buffers  = 1;
	_common_set_ocon.theData_out.con_params.number_channels = _inproc.numInputs;
	_common_set_ocon.theData_out.con_params.rate          = CjvxAudioDevice::properties_active.samplerate.value;

	// Propagate prepare through the downstream chain
	res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
		goto leave_error;

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
	{
		jvxSize sampleRate = CjvxAudioDevice::properties_active.samplerate.value;
		jvxSize bufferSize = _inproc.buffersize;
		jvxSize numCh      = isInput ? _inproc.numInputs : _inproc.numOutputs;
		if (numCh == 0) numCh = 1;

		oboe::AudioStreamBuilder builder;		
		builder.setDirection(isInput ? oboe::Direction::Input : oboe::Direction::Output)
		       ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
		       ->setSharingMode(oboe::SharingMode::Exclusive)
		       ->setFormat(oboe::AudioFormat::Float)
		       ->setChannelCount((int32_t)numCh)
		       ->setSampleRate((int32_t)sampleRate)
		       ->setFramesPerDataCallback((int32_t)bufferSize)
		       ->setDataCallback(this);

		// Target the specific hardware endpoint (0 = system default)
		if (androidDeviceId != 0)
			builder.setDeviceId(androidDeviceId);

		oboe::Result oboeRes = builder.openManagedStream(audioStream);
		if (oboeRes != oboe::Result::OK)
		{
			ALOGE("prepare_chain_master: openManagedStream failed: %s",
				oboe::convertToText(oboeRes));
			_postprocess_chain_master(NULL);
			res = JVX_ERROR_COMPONENT_BUSY;
			goto leave_error;
		}

		ALOGD("prepare_chain_master: %s stream opened (rate=%d, buf=%d, ch=%d)",
			isInput ? "input" : "output",
			(int)sampleRate, (int)bufferSize, (int)numCh);
	}
#endif

	return res;
leave_error:
	return res;
}

jvxErrorType
CjvxAudioAndroidDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
	if (audioStream)
	{
		audioStream->close();
		audioStream.reset();
		ALOGD("postprocess_chain_master: %s stream closed", isInput ? "input" : "output");
	}
#endif

	res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	jvxErrorType resL = postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;
}

// ==========================================================================
// Chain master: start / stop
// ==========================================================================

jvxErrorType
CjvxAudioAndroidDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxErrorType resL = start_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	res = _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
		goto leave_error;

	// Init timing state
	{
		jvxData timeOutBuffer_secs = (jvxData)_common_set_ocon.theData_out.con_params.buffersize
		                           / (jvxData)_common_set_ocon.theData_out.con_params.rate;
		timeOutBuffer_msecs_exact = timeOutBuffer_secs * 1e3;
		progress_msecs_frames = -1;
		progress_msecs_next_call = -1;
		progress_msecs_measured = -1;
		tStampLastFrame = -1;
		timeOutBuffer_msecs_exact_n = timeOutBuffer_msecs_exact / 4;
		timeOutBuffer_msecs_floor = (jvxSize)floor(timeOutBuffer_msecs_exact_n);
		JVX_GET_TICKCOUNT_US_SETREF(&tStamp);
	}

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
	if (audioStream)
	{
		streamRunning = c_true;
		oboe::Result oboeRes = audioStream->requestStart();
		if (oboeRes != oboe::Result::OK)
		{
			ALOGE("start_chain_master: requestStart failed: %s",
				oboe::convertToText(oboeRes));
			streamRunning = c_false;
			_stop_chain_master(NULL);
			res = JVX_ERROR_COMPONENT_BUSY;
			goto leave_error;
		}
		ALOGD("start_chain_master: %s stream started", isInput ? "input" : "output");
	}
#endif

	return res;
leave_error:
	return res;
}

jvxErrorType
CjvxAudioAndroidDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
	streamRunning = c_false;
	if (audioStream)
	{
		audioStream->requestStop();
		ALOGD("stop_chain_master: %s stream stopped", isInput ? "input" : "output");
	}
#endif

	res = _stop_chain_master(NULL);
	assert(res == JVX_NO_ERROR);

	jvxErrorType resL = stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;
}

// ==========================================================================
// Icon connector: prepare / postprocess (return path from chain back to device)
// ==========================================================================

jvxErrorType
CjvxAudioAndroidDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	_common_set_icon.theData_in->con_params.buffersize       = _inproc.buffersize;
	_common_set_icon.theData_in->con_params.format           = _inproc.format;
	_common_set_icon.theData_in->con_data.buffers            = NULL;
	_common_set_icon.theData_in->con_data.number_buffers     =
		JVX_MAX(_common_set_icon.theData_in->con_data.number_buffers, 1);
	_common_set_icon.theData_in->con_params.number_channels  = _inproc.numOutputs;
	_common_set_icon.theData_in->con_params.rate             = _inproc.samplerate;

	res = allocate_pipeline_and_buffers_prepare_to();

	_common_set_icon.theData_in->con_compat.user_hints = NULL;

	return res;
}

jvxErrorType
CjvxAudioAndroidDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return deallocate_pipeline_and_buffers_postprocess_to();
}

// ==========================================================================
// process_*_icon: called when this device IS a connector in someone else's chain
// ==========================================================================

jvxErrorType
CjvxAudioAndroidDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	return CjvxAudioDevice::process_start_icon(
		pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
}

jvxErrorType
CjvxAudioAndroidDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	return CjvxAudioDevice::process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType
CjvxAudioAndroidDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	return CjvxAudioDevice::process_stop_icon(
		idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
}

// ==========================================================================
// Core buffer processing — drives the downstream chain and transfers PCM data
// ==========================================================================

void
CjvxAudioAndroidDevice::core_buffer_process(void* audioData, int32_t numFrames)
{
	if (!_common_set_ocon.theData_out.con_link.connect_to)
	{
		// No downstream chain: fill output with silence
		if (!isInput && audioData)
		{
			jvxSize numCh = isInput ? (jvxSize)_inproc.numInputs : (jvxSize)_inproc.numOutputs;
			if (numCh == 0) numCh = 1;
			memset(audioData, 0, (jvxSize)numFrames * numCh * sizeof(float));
		}
		return;
	}

	// Step 1: advance the pipeline slot
	_common_set_ocon.theData_out.con_link.connect_to->process_start_icon();

	jvxSize idxToProc = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;

	// Step 2 (input device): copy hardware PCM → ocon buffers
	if (isInput && audioData)
	{
		const float* src = static_cast<const float*>(audioData);
		jvxSize numCh = (jvxSize)_common_set_ocon.theData_out.con_params.number_channels;
		for (jvxSize ch = 0; ch < numCh; ++ch)
		{
			jvxData* dst = static_cast<jvxData*>(
				_common_set_ocon.theData_out.con_data.buffers[idxToProc][ch]);
			if (dst)
			{
				for (int32_t f = 0; f < numFrames; ++f)
					dst[f] = static_cast<jvxData>(src[f * numCh + ch]);
			}
		}
	}

	// Step 3: run the processing chain
	_common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();

	// Step 4 (output device): copy icon buffers → hardware PCM
	if (!isInput && audioData)
	{
		if (_common_set_icon.theData_in && _common_set_icon.theData_in->con_data.buffers)
		{
			jvxSize idxFromProc = *_common_set_icon.theData_in->con_pipeline.idx_stage_ptr;
			float* dst = static_cast<float*>(audioData);
			jvxSize numCh = (jvxSize)_common_set_icon.theData_in->con_params.number_channels;
			for (jvxSize ch = 0; ch < numCh; ++ch)
			{
				const jvxData* src = static_cast<const jvxData*>(
					_common_set_icon.theData_in->con_data.buffers[idxFromProc][ch]);
				if (src)
				{
					for (int32_t f = 0; f < numFrames; ++f)
						dst[f * numCh + ch] = static_cast<float>(src[f]);
				}
			}
		}
		else
		{
			// Chain is connected but no return data: silence
			jvxSize numCh = (jvxSize)_common_set_ocon.theData_out.con_params.number_channels;
			if (numCh == 0) numCh = 1;
			memset(audioData, 0, (jvxSize)numFrames * numCh * sizeof(float));
		}
	}

	// Step 5: release the pipeline slot
	_common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
}

void
CjvxAudioAndroidDevice::core_buffer_run()
{
	// Fallback used by timerCallback when no audioData pointer is available
	if (!_common_set_ocon.theData_out.con_link.connect_to)
		return;

	_common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
	_common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
	_common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
}

// ==========================================================================
// Oboe callback (Android only)
// ==========================================================================

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API

oboe::DataCallbackResult
CjvxAudioAndroidDevice::onAudioReady(oboe::AudioStream* /*stream*/, void* audioData,
	int32_t numFrames)
{
	if (!streamRunning)
	{
		// Stream is stopping; fill output with silence and exit cleanly
		if (!isInput && audioData)
		{
			jvxSize numCh = (jvxSize)_inproc.numOutputs;
			if (numCh == 0) numCh = 1;
			memset(audioData, 0, (jvxSize)numFrames * numCh * sizeof(float));
		}
		return oboe::DataCallbackResult::Stop;
	}

	core_buffer_process(audioData, numFrames);
	return oboe::DataCallbackResult::Continue;
}

#endif // JVX_USE_PART_ANDROIDAUDIO_NO_API

// ==========================================================================

jvxErrorType
CjvxAudioAndroidDevice::done_configuration()
{
	return JVX_NO_ERROR;
}

void
CjvxAudioAndroidDevice::timerCallback()
{
	jvxTick tickEnter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

#ifdef MEASURE_TIMING
	gCnt = (gCnt + 1) % JVX_NUM_EVENTS_OBSERVE;
#endif

	if (progress_msecs_frames < 0)
	{
		progress_msecs_frames = 0;
		progress_msecs_next_call = 0;
		progress_msecs_measured = 0;
	}
	else
	{
		progress_msecs_measured += (tickEnter - tStampLastFrame) * 1e-3;
		progress_msecs_frames   += timeOutBuffer_msecs_floor;

		if (progress_msecs_frames > progress_msecs_next_call)
		{
			core_buffer_run();
			progress_msecs_next_call += timeOutBuffer_msecs_exact;
		}
	}
	tStampLastFrame = tickEnter;
}
