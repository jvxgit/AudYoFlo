#ifndef __CJVXAUDIOSYNCCLOCKDEVICE_H__
#define __CJVXAUDIOSYNCCLOCKDEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"
#include "pcg_exports_device.h"

class CjvxAudioSyncClockTechnology;

class CjvxAudioSyncClockDevice : public CjvxAudioDevice, public IjvxConfigurationDone,
#ifndef JVX_SYNCED_CLOCK_WINDOWS
	public IjvxThreads_report,
#endif
	public genSyncClock_device
{
	friend class CjvxAudioSyncClockTechnology;

private:
	
	CjvxAudioSyncClockTechnology* parentTech = nullptr;
	jvxSize numInChannelsMax = 2;
	jvxSize numOutChannelsMax = 2;

	// Progress as indicated by reported frames
	jvxData progress_msecs_frames = 0;

	// Progress as measured from high perfomance conuter
	jvxData progress_msecs_measured = 0;

	// Progress at which to involve the next bufferswitch
	jvxData progress_msecs_next_call = 0;

	// Framesize in msecs - exact value due to float variable
	jvxData timeOutBuffer_msecs_exact = 0.0;

	// Framesize in msecs - scaled down by period factor
	jvxData timeOutBuffer_msecs_exact_n = 0;

	// Framesize in msecs when rounding towards next small integer
	jvxSize timeOutBuffer_msecs_floor = 0;

	// Preserve tstamp from last frame
	jvxInt64 tStampLastFrame = 0;
	// jvxInt64 tStampFirst = 0;
	jvxTimeStampData tStamp;
	// jvxTick tStampAbsolute;

#ifndef JVX_SYNCED_CLOCK_WINDOWS
	refComp<IjvxThreads> threads;
#else
	MMRESULT timerId = 0;
#endif


public:
	JVX_CALLINGCONVENTION CjvxAudioSyncClockDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAudioSyncClockDevice();
	
	virtual void init(CjvxAudioSyncClockTechnology* ptr);

	jvxErrorType activate();
	jvxErrorType deactivate();

	// ===================================================================================
	// New linkdata connection interface
	// ===================================================================================

	// ===================================================================================
	virtual jvxErrorType test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===================================================================================
	
	virtual jvxErrorType JVX_CALLINGCONVENTION done_configuration() override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
	    jvxSize pipeline_offset , jvxSize* idx_stage,
	    jvxSize tobeAccessedByStage,
	    callback_process_start_in_lock clbk,
	    jvxHandle* priv_ptr)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
	    jvxSize idx_stage ,
	    jvxBool shift_fwd,
	    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr)override;

	void core_buffer_run();

	void timerCallback();

#ifndef JVX_SYNCED_CLOCK_WINDOWS

	jvxErrorType startup(jvxInt64 timestamp_us) override;
	jvxErrorType expired(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	jvxErrorType wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	jvxErrorType stopped(jvxInt64 timestamp_us) override;
#endif


#define JVX_INTERFACE_SUPPORT_CONFIGURATION_DONE
#define JVX_INTERFACE_SUPPORT_BASE_CLASS CjvxAudioDevice

#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"

#undef JVX_INTERFACE_SUPPORT_CONFIGURATION_DONE
#undef JVX_INTERFACE_SUPPORT_BASE_CLASS
};

#endif
