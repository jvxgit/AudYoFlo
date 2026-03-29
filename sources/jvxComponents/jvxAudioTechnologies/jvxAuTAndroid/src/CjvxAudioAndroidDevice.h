#ifndef __CJVXAUDIOANDROIDDEVICE_H__
#define __CJVXAUDIOANDROIDDEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"
#include "pcg_exports_device.h"

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
#include <oboe/Oboe.h>
#include <android/log.h>
#endif

class CjvxAudioAndroidTechnology;

class CjvxAudioAndroidDevice : public CjvxAudioDevice, public IjvxConfigurationDone,
	public genAndroid_device
#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
	, public oboe::AudioStreamDataCallback
#endif
{
	friend class CjvxAudioAndroidTechnology;

private:

	CjvxAudioAndroidTechnology* parentTech = nullptr;
	jvxSize numInChannelsMax = 2;
	jvxSize numOutChannelsMax = 2;

	// Whether this device instance captures (true) or renders (false)
	jvxBool isInput = c_false;

	// Android AudioDeviceInfo.getId() — 0 means "system default"
	int androidDeviceId = 0;

	// Progress as indicated by reported frames
	jvxData progress_msecs_frames = 0;

	// Progress as measured from high performance counter
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
	jvxTimeStampData tStamp;

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
	oboe::ManagedStream audioStream;
	volatile jvxBool streamRunning = c_false;

	// oboe::AudioStreamDataCallback interface
	oboe::DataCallbackResult onAudioReady(
		oboe::AudioStream* stream, void* audioData, int32_t numFrames) override;
#endif

public:
	JVX_CALLINGCONVENTION CjvxAudioAndroidDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAudioAndroidDevice();

	virtual void init(CjvxAudioAndroidTechnology* ptr);

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	// ===================================================================================
	// Chain master interface
	// ===================================================================================

	virtual jvxErrorType test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===================================================================================
	// Icon connector interface (return path from chain back to device)
	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION done_configuration() override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
	    jvxSize pipeline_offset, jvxSize* idx_stage,
	    jvxSize tobeAccessedByStage,
	    callback_process_start_in_lock clbk,
	    jvxHandle* priv_ptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(
		jvxSize mt_mask, jvxSize idx_stage) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
	    jvxSize idx_stage,
	    jvxBool shift_fwd,
	    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr) override;

	// ===================================================================================

	jvxErrorType set_property(jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans) override;

	void updateDependentProperties();

	// ===================================================================================

	// Core processing: copy PCM ↔ jvx buffers and drive downstream chain
	void core_buffer_process(void* audioData, int32_t numFrames);

	// Legacy synchronous-clock entry point (kept for non-API builds / timerCallback)
	void core_buffer_run();

	void timerCallback();

#define JVX_INTERFACE_SUPPORT_CONFIGURATION_DONE
#define JVX_INTERFACE_SUPPORT_BASE_CLASS CjvxAudioDevice

#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"

#undef JVX_INTERFACE_SUPPORT_CONFIGURATION_DONE
#undef JVX_INTERFACE_SUPPORT_BASE_CLASS
};

#endif
