#include "CjvxAudioAndroidDevice.h"
#include "CjvxAudioAndroidTechnology.h"
#include "jvx-helpers-cpp.h"

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
	numInChannelsMax = ptr->genAndroid_technology::config_select.init_number_in_channels_max.value;
	numOutChannelsMax = ptr->genAndroid_technology::config_select.init_number_out_channels_max.value;

	// Set the device capabilities
	jvx_bitZSet(_common_set_device.caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT);
	jvx_bitZSet<jvxCBitField16>(_common_set_device.caps.flags, (int)jvxDeviceCapabilityFlagsShift::JVX_DEVICE_CAPABILITY_FLAGS_DEFAULT_DEVICE_SHIFT);
	_common_set_device.report = static_cast<IjvxDevice_report*>(parentTech);

}

jvxErrorType 
CjvxAudioAndroidDevice::activate()
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

		CjvxAudioDevice::properties_active.allow_operate_zero_channels.value = c_true;
			
		return JVX_NO_ERROR;
	}
	return res;
}

jvxErrorType
CjvxAudioAndroidDevice::deactivate()
{
	jvxErrorType res = CjvxAudioDevice::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
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
CjvxAudioAndroidDevice::set_property(jvxCallManagerProperties& callGate,
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
CjvxAudioAndroidDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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
	CjvxAudioAndroidDevice* this_pointer = (CjvxAudioAndroidDevice*)dwUser;
	assert(this_pointer);
	this_pointer->timerCallback();
}
#endif

jvxErrorType
CjvxAudioAndroidDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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

	}

	return res;
}

jvxErrorType
CjvxAudioAndroidDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxAudioDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
	}
	
	return res;
}

jvxErrorType
CjvxAudioAndroidDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	return CjvxAudioDevice::process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType
CjvxAudioAndroidDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
					jvxSize tobeAccessedByStage,
					callback_process_start_in_lock clbk,
					jvxHandle* priv_ptr)
{
	return CjvxAudioDevice::process_start_icon(
		pipeline_offset, idx_stage,
		tobeAccessedByStage, clbk, priv_ptr);
}

jvxErrorType
CjvxAudioAndroidDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr)
{
	return CjvxAudioDevice::process_stop_icon(
		idx_stage, shift_fwd,
		tobeAccessedByStage, clbk, priv_ptr);
}

void
CjvxAudioAndroidDevice::core_buffer_run()
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
CjvxAudioAndroidDevice::done_configuration()
{
	return JVX_NO_ERROR;
}

void
CjvxAudioAndroidDevice::timerCallback()
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

