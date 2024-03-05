#include "CjvxAudioSyncClockDevice.h"
#include "CjvxAudioSyncClockTechnology.h"
#include "jvx-helpers-cpp.h"

/*
 * Some general hints for WINDOWS here: I started to use my own thread function for the timer functionality.
 * Then, I was very disappointed that the algorithm did not work - however, I found out why:
 * In my threads, I use WaitForSingleObject with a timeout which is recomputed every time.
 * Passing a timeout of 20 msecs should sleep and return after 20 msecs. but this does not happen in Windows OS.
 * There is a good paper on this issue here:
 * 
 * https://randomascii.wordpress.com/2020/10/04/windows-timer-resolution-the-great-rule-change/
 * 
 * It says that there is a timer period which is 1000/64 in msecs -> ~15 msecs. And this lets the
 * timeout return after 20 + 15 = 35 msecs in the worst case. And this destroys the processing in the sense 
 * of a simulated realtime process.
 * 
 * A user then mentioned the timeSetEvent function 
 * 
 *  https://learn.microsoft.com/en-us/previous-versions/dd757634(v=vs.85)
 *
 * which is obsolete. However, only this function seems to work properly. Even the proposed function 
 * CreateTimerQueueTimer which is proposed seems to be broken due to the above mentioned article:
 * 
 * "I am writing a streaming application, that used one shot CreateTimerQueueTimer timers to trigger 
 *  the sending of UDP packets every few milliseconds. Since Win 10 2004 this is broken, my timers 
 *  only fire after 15ms despite calling timeBeginPeriod(1). I changed it to use timeSetEvent, now 
 *  it works again, including on Windows 10 2004 and 20H2.
 *
 *  So since the 2004 update CreateTimerQueueTimer() ignores the resolution defined by timeBeginPeriod.
 *
 *  Unfortunately timeSetEvent is desclared obsolete, with a recommendation to use the (now broken) 
 *  CreateTimerQueueTimer. On the other hand timeSetEvent has been declared obsolete since at least 
 *  2008 (earliest mention of the obsolence found in a quick Google search) and it is still around. 
 *  Hopefully it will stay oder Microsoft fixes CreateTimerQueueTimer."
 *
 * 
 */

 // =============================================================
 // =============================================================

// Buffering debugging here!!

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
}
static oneEntryTimer[JVX_NUM_EVENTS_OBSERVE];
static int Cbs = 0;
static int cnt = 0;

#endif

// =============================================================
// =============================================================


CjvxAudioSyncClockDevice::CjvxAudioSyncClockDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_properties.moduleReference = _common_set.theModuleName;

	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_16BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_32BIT_LE);

	
}


CjvxAudioSyncClockDevice::~CjvxAudioSyncClockDevice()
{
}

// ============================================================================

void 
CjvxAudioSyncClockDevice::init(CjvxAudioSyncClockTechnology* ptr)
{
	parentTech = ptr;
	numInChannelsMax = ptr->genSyncClock_technology::config_select.init_number_in_channels_max.value;
	numOutChannelsMax = ptr->genSyncClock_technology::config_select.init_number_out_channels_max.value;

	// Set the device capabilities
	jvx_bitZSet(_common_set_device.caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT);
	jvx_bitZSet<jvxCBitField16>(_common_set_device.caps.flags, (int)jvxDeviceCapabilityFlagsShift::JVX_DEVICE_CAPABILITY_FLAGS_DEFAULT_DEVICE_SHIFT);
	_common_set_device.report = static_cast<IjvxDevice_report*>(parentTech);

}

jvxErrorType 
CjvxAudioSyncClockDevice::activate()
{
	jvxSize i;
	jvxBitField bf;
	jvxErrorType res = CjvxAudioDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		// Should all be zeroed
		jvx_bitFClear(bf);
		for (i = 0; i < numInChannelsMax; i++)
		{
			std::string nmToken = "Input Channel #" + jvx_size2String(i);
			CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(nmToken.c_str());
			jvx_bitSet(bf, i);
		}
		CjvxAudioDevice::properties_active.inputchannelselection.value.selection() = bf;
		CjvxAudioDevice::properties_active.numberinputchannels.value = numInChannelsMax;

		jvx_bitFClear(bf);
		for (i = 0; i < numOutChannelsMax; i++)
		{
			std::string nmToken = "Output Channel #" + jvx_size2String(i);
			CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(nmToken.c_str());
			jvx_bitSet(bf, i);
		}
		CjvxAudioDevice::properties_active.outputchannelselection.value.selection() = bf;
		CjvxAudioDevice::properties_active.numberoutputchannels.value = numOutChannelsMax;

		CjvxAudioDevice::properties_active.samplerate.value = 48000;
		CjvxAudioDevice::properties_active.buffersize.value = 1024;

#ifndef JVX_SYNCED_CLOCK_WINDOWS
		threads = reqInstTool<IjvxThreads>(_common_set.theToolsHost, JVX_COMPONENT_THREADS);
		assert(threads.cpPtr);
		threads.cpPtr->initialize(this);
#endif
			
		return JVX_NO_ERROR;
	}
	return res;
}

jvxErrorType
CjvxAudioSyncClockDevice::deactivate()
{
	jvxErrorType res = CjvxAudioDevice::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
#ifndef JVX_SYNCED_CLOCK_WINDOWS
		threads.cpPtr->terminate();
		retInstTool<IjvxThreads>(_common_set.theToolsHost, JVX_COMPONENT_THREADS, threads);
		threads.cpPtr = nullptr;
		threads.objPtr = nullptr;
#endif
		CjvxAudioDevice::properties_active.buffersize.value = 1024;
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
CjvxAudioSyncClockDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxBool report_update = false;

	jvxErrorType res = CjvxAudioDevice::set_property(callGate, rawPtr, ident, trans);
	if (res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);

		// Update the properties in the base class
		updateDependentVariables_lock(propId, category, false);
		
	}
	return(res);
}

// ==========================================================================

jvxErrorType 
CjvxAudioSyncClockDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res =  CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_ERROR_REQUEST_CALL_AGAIN)
	{
		res = CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

#ifdef JVX_SYNCED_CLOCK_WINDOWS
// typedef void (CALLBACK TIMECALLBACK)(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
void myCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1,DWORD_PTR dw2)
{
	CjvxAudioSyncClockDevice* this_pointer = (CjvxAudioSyncClockDevice*)dwUser;
	assert(this_pointer);
	this_pointer->timerCallback();
}
#endif

jvxErrorType
CjvxAudioSyncClockDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxAudioDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvxData timeOutBuffer_secs = (jvxData)_common_set_ocon.theData_out.con_params.buffersize / (jvxData)_common_set_ocon.theData_out.con_params.rate;
		timeOutBuffer_msecs_exact = timeOutBuffer_secs * 1e3;
		
		progress_msecs_frames = -1;
		progress_msecs_next_call = -1;
		progress_msecs_measured = -1;
	
		// Preset the timestamp to measure delta time
		tStampLastFrame = -1;

		// Involve a higher rate compared to approx buffersize to take core of the round problem
		timeOutBuffer_msecs_exact_n = timeOutBuffer_msecs_exact / 4;

		// Here, we round downwards
		timeOutBuffer_msecs_floor = floor(timeOutBuffer_msecs_exact_n);
		
		// Init the time measurement
		JVX_GET_TICKCOUNT_US_SETREF(&tStamp);		

#ifdef JVX_SYNCED_CLOCK_WINDOWS
		// Start the time - note that we are allowed to use a maximum of 16 timers according to
		// https://stackoverflow.com/questions/18637470/how-to-replace-timesetevent-function-without-losing-functionality
		// in windows!
		timerId = timeSetEvent(timeOutBuffer_msecs_floor, 0, (LPTIMECALLBACK)myCallback, (DWORD_PTR)this, TIME_PERIODIC);
#else
		threads.cpPtr->start(timeOutBuffer_msecs_floor, true, true);
#endif
	}

	return res;
}

jvxErrorType
CjvxAudioSyncClockDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxAudioDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
	}
#ifdef JVX_SYNCED_CLOCK_WINDOWS
	timeKillEvent(timerId);
	timerId = 0;
#else
	threads.cpPtr->stop();
#endif

	
	return res;
}

jvxErrorType
CjvxAudioSyncClockDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	return CjvxAudioDevice::process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType
CjvxAudioSyncClockDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
					jvxSize tobeAccessedByStage,
					callback_process_start_in_lock clbk,
					jvxHandle* priv_ptr)
{
	return CjvxAudioDevice::process_start_icon(
		pipeline_offset, idx_stage,
		tobeAccessedByStage, clbk, priv_ptr);
}

jvxErrorType
CjvxAudioSyncClockDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr)
{
	return CjvxAudioDevice::process_stop_icon(
		idx_stage, shift_fwd,
		tobeAccessedByStage, clbk, priv_ptr);
}

void
CjvxAudioSyncClockDevice::core_buffer_run()
{
	// Run one buffer!
	// =================================================================================================
	jvxErrorType res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
	assert(res == JVX_NO_ERROR);

	res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
	assert(res == JVX_NO_ERROR);

	res = _common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
	assert(res == JVX_NO_ERROR);
}

jvxErrorType 
CjvxAudioSyncClockDevice::done_configuration()
{
	return JVX_NO_ERROR;
}

void
CjvxAudioSyncClockDevice::timerCallback()
{
	jvxTick tickEnter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

#ifdef MEASURE_TIMING
	// Entry to callback by switching to next element
	cnt = (cnt + 1) % JVX_NUM_EVENTS_OBSERVE;
	oneEntryTimer[cnt].measuredTimeout = (tickEnter - tickLeave) * 1e-3;
	// oneEntryTimer[cnt].progressTimerDirect = (tickEnter - tStampAbsolute) * 1e-3;
#endif

	if (progress_msecs_frames < 0)
	{
		// First frame, reset everything
		progress_msecs_frames = 0;
		progress_msecs_next_call = 0;
		progress_msecs_measured = 0;
		// tStampAbsolute = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	}
	else
	{
		progress_msecs_measured += (tickEnter - tStampLastFrame) * 1e-3;
		
		// We use the timerperiod from the count of timer callback to derive progress
		progress_msecs_frames += timeOutBuffer_msecs_floor;

#ifdef MEASURE_TIMING
		oneEntryTimer[cnt].progressTimerMeasured = progress_msecs_measured;
		oneEntryTimer[cnt].progressTimerFrames = progress_msecs_frames;
		oneEntryTimer[cnt].progressTimerNext = progress_msecs_next_call;
		oneEntryTimer[cnt].runBuffer = 0;
#endif
		
		jvxData valueInFocus = progress_msecs_frames;
		// jvxData valueInFocus = progress_msecs_measured;

		if (valueInFocus > progress_msecs_next_call)
		{
			//Cbs++;
			core_buffer_run();

#ifdef MEASURE_TIMING
			oneEntryTimer[cnt].runBuffer = 1;
#endif
			progress_msecs_next_call += timeOutBuffer_msecs_exact;
		}
	}
	tStampLastFrame = tickEnter;
}

#ifndef JVX_SYNCED_CLOCK_WINDOWS


jvxErrorType
CjvxAudioSyncClockDevice::startup(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioSyncClockDevice::expired(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxTick tickLeave = 0;

	this->timerCallback();

	// Compute the time between enter and "processing done".
	// We will consider this when computing the next time period to wait for!!
	tickLeave = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

	// Compute deviation between corrent timer and measured timer
	jvxData deltaT = (progress_msecs_measured - progress_msecs_frames); // These values in msecs
	jvxData deltaMsec = timeOutBuffer_msecs_exact_n - deltaT;

	// Consider the time it took to run up to here!
	deltaT = (tickLeave - tStampLastFrame) * 1e-3;
	deltaMsec -= deltaT;

	if (deltaMsec < 0)
	{
		res = JVX_ERROR_THREAD_ONCE_MORE;
		deltaMsec = 0;
	}
	
	*delta_ms = floor(deltaMsec);

#ifdef MEASURE_TIMING
	oneEntryTimer[cnt].desiredTimeout = deltaMsec;// *delta_ms;
#endif

#ifdef MEASURE_TIMING
	if (cnt == 0)
	{
		// std::cout << "Hallo" << std::endl;
	}
#endif

/*
	jvxErrorType res = JVX_NO_ERROR;
	jvxTick tickEnter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

#ifdef MEASURE_TIMING
	oneEntryTimer[cnt].measuredTimeout = (tickEnter - tickLeave) * 1e-3;
#endif

	if (progress_msecs_measured < 0)
	{
		// First frame
		progress_msecs_measured = 0;
		progress_msecs_next_call = 0;
	}
	else
	{
		// Start time this frame
		progress_msecs_measured += (tickEnter - tStampLastFrame) * 1e-3;

		if (progress_msecs_measured > progress_msecs_next_call)
		{
			core_buffer_run();
			progress_msecs_next_call += timeOutBuffer_msecs_exact;
		}
	}
	tStampLastFrame = tickEnter;

	// =================================================================================================


	// =================================================================================================

	jvxTick tickNow = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	progress_msecs_measured += (tickNow - tickEnter) * 1e-3;

	jvxData deltaMsec = progress_msecs_next_call - progress_msecs_measured;
	if (deltaMsec < 0)
	{
		jvxSize numFramesToCompensate = -deltaMsec / timeOutBuffer_msecs_exact;
		if (numFramesToCompensate > 5)
		{
			// If we are here, we have lost too many buffers!!
			numFramesToCompensate -= 5;
			progress_msecs_measured = -1;
			progress_msecs_next_call = 0;
			deltaMsec = timeOutBuffer_msecs_exact;
		}
	}

	deltaMsec = JVX_MAX(deltaMsec, 0);

	*delta_ms = floor(deltaMsec);

#ifdef MEASURE_TIMING
	cnt = (cnt + 1) % JVX_NUM_EVENTS_OBSERVE;
	if (cnt == 0)
	{
		std::cout << "Hallo" << std::endl;
	}
	oneEntryTimer[cnt].desiredTimeout = *delta_ms;
#endif

	tickLeave = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

	*/
	return res;
}

jvxErrorType
CjvxAudioSyncClockDevice::wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioSyncClockDevice::stopped(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}
#endif

#if 0
jvxErrorType 
CjvxAudioSyncClockDevice::expired(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxTick tickEnter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

	oneEntryTimer[cnt].measuredTimeout = (tickEnter - tickLeave) * 1e-3;

	if (progress_msecs_measured < 0)
	{
		// First frame
		progress_msecs_measured = 0;
		progress_msecs_accumulated_exact = 0;
	}
	else
	{
		// Start time this frame
		progress_msecs_measured += (tickEnter - tStampLastFrame) * 1e-3;
		jvxData targetTimer = (progress_msecs_accumulated_exact + timeOutBuffer_msecs_exact * 0.8);
		if (progress_msecs_measured < targetTimer)
		{
			return JVX_NO_ERROR;
		}
		progress_msecs_accumulated_exact += timeOutBuffer_msecs_exact;
	}
	tStampLastFrame = tickEnter;

	// =================================================================================================
	res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
	assert(res == JVX_NO_ERROR);

	res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
	assert(res == JVX_NO_ERROR);

	res = _common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
	assert(res == JVX_NO_ERROR);

	// =================================================================================================
	
	jvxData progress_msecs_accumulated_expect_next = progress_msecs_accumulated_exact + timeOutBuffer_msecs_exact;
	jvxTick tickNow = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	progress_msecs_measured += (tickNow - tickEnter) * 1e-3;

	jvxData deltaMsec = progress_msecs_accumulated_expect_next - progress_msecs_measured;
	if (deltaMsec < 0)
	{
		jvxSize numFramesToCompensate = -deltaMsec / timeOutBuffer_msecs_exact;
		if (numFramesToCompensate > 5)
		{
			// If we are here, we have lost too many buffers!!
			numFramesToCompensate -= 5;
			progress_msecs_measured = -1;
			progress_msecs_accumulated_exact = 0;
			deltaMsec = timeOutBuffer_msecs_exact;
		}
	}

	deltaMsec = JVX_MAX(deltaMsec, 0);

	*delta_ms = floor(deltaMsec);

	cnt = (cnt + 1) % JVX_NUM_EVENTS_OBSERVE;
	if (cnt == 0)
	{
		std::cout << "Hallo" << std::endl;
	}
	oneEntryTimer[cnt].desiredTimeout = *delta_ms;
	tickLeave = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	return JVX_NO_ERROR;
}
#endif

