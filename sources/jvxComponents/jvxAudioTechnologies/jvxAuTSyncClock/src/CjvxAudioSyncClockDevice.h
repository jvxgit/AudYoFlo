#ifndef __CJVXAUDIOSYNCCLOCKDEVICE_H__
#define __CJVXAUDIOSYNCCLOCKDEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"
#include "pcg_exports_device.h"

class CjvxAudioSyncClockTechnology;

class CjvxAudioSyncClockDevice : public CjvxAudioDevice, 
	public IjvxThreads_report,
	public genSyncClock_device
{
	friend class CjvxAudioSyncClockTechnology;

private:
	
	CjvxAudioSyncClockTechnology* parentTech = nullptr;
	jvxSize numInChannelsMax = 2;
	jvxSize numOutChannelsMax = 2;

	refComp<IjvxThreads> threads;
	jvxData timeOutBuffer_msecs_exact = 0.0;
	jvxData progress_msecs_accumulated_exact = 0.0;
	jvxData progress_msecs_measured = 0;
	jvxInt64 tStampLastFrame = 0;
	jvxTimeStampData tStamp;

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

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
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

	jvxErrorType startup(jvxInt64 timestamp_us) override;
	jvxErrorType expired(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	jvxErrorType wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	jvxErrorType stopped(jvxInt64 timestamp_us) override;
};

#endif
