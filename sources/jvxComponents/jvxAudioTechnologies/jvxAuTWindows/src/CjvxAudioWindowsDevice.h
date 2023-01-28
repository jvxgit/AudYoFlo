#ifndef __CJVXAUDIOALSADEVICE_H__
#define __CJVXAUDIOALSADEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"

// Make sure this is included first OF ALL!!
// https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/ceff4e2d-8f63-4ab6-b09b-fdac65d62a80/pkeyaudioenginedeviceformat-link-error?forum=windowspro-audiodevelopment

#include <initguid.h>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include "pcg_exports_device.h"

class CjvxAudioWindowsTechnology;

class CjvxAudioWindowsDevice: 
	public CjvxAudioDevice,
    public genWindows_device
{

	friend class CjvxAudioWindowsTechnology;
private:
	IMMDevice* device = nullptr;
	CjvxAudioWindowsTechnology* parentTech = nullptr;

	HANDLE _ShutdownEvent = JVX_INVALID_HANDLE_VALUE;
	HANDLE _AudioSamplesReadyEvent = JVX_INVALID_HANDLE_VALUE;
	HANDLE _StreamSwitchEvent = JVX_INVALID_HANDLE_VALUE;
	IAudioClient* audioClient = nullptr;

	WAVEFORMATEXTENSIBLE wext_init = { 0 };
	WAVEFORMATEXTENSIBLE waveFormatStrProcessing = { 0 };
	UINT32 realBufferSize = 0;

	jvxBool isInput = false;
	std::list<jvxSize> supportedRates;

	IAudioCaptureClient* captureClient = nullptr;
	IAudioRenderClient* renderClient = nullptr;

	JVX_THREAD_HANDLE sigProcThread = JVX_INVALID_HANDLE_VALUE;
	jvxCBitField channelMaskInProc = 0;

	jvxSize num_proc_loops = 0;
	jvxSize procBufferSize = 0;

	jvxSize numBytesSample = 0;
	jvxSize numBytesSampleAllChans = 0;
	AUDCLNT_SHAREMODE audioMode = AUDCLNT_SHAREMODE_SHARED;
	jvxByte* shared_buf = nullptr;
	jvxSize shared_buf_length = 0;
	jvxSize shared_buf_fill = 0;
	jvxBool startupComplete = false;
public:
	JVX_CALLINGCONVENTION CjvxAudioWindowsDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAudioWindowsDevice();

	void setHandle(
		CjvxAudioWindowsTechnology* parentTechArg, 
		IMMDevice* deviceArg,
		WAVEFORMATEXTENSIBLE* wexArg,
		jvxBool isInputArg,
		jvxBool isDefaultDevice);

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans
	)override;

	// Interface IjvxConfiguration

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename = "", jvxInt32 lineno = -1 )override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================
	// New linkdata connection interface
	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
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

	//virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	
	void updateDependentParameters(jvxBool inform_chain);

	// jvxErrorType updateCurrentProperties(jvxBool initRun, jvxBool updateRates, jvxBool updateSizes); //, jvxBool readBuffersizes

	// ================================================================================================
	jvxErrorType activate_wasapi_device();
	jvxErrorType deactivate_wasapi_device();

	// ================================================================================================
	jvxErrorType prepare_wasapi_device();
	jvxErrorType postprocess_wasapi_device();

	// ================================================================================================

	jvxErrorType start_wasapi_device();
	jvxErrorType stop_wasapi_device();
		
	// ================================================================================================
	DWORD audioProcessThread_ic();
	jvxErrorType core_buffer_process_render(BYTE** ptrTo, jvxSize numSamples);
	// ================================================================================================
	// 
	// ================================================================================================
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_new_rate);
};

#endif
