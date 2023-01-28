
#ifndef __CJVXPORTAUDIODEVICE_H__
#define __CJVXPORTAUDIODEVICE_H__

// Port audio headers
#include "portaudio.h"

#include "jvxAudioTechnologies/CjvxAudioDevice.h"

#ifdef JVX_OS_WINDOWS
#include "pa_asio.h"
#include "pa_win_wasapi.h"
#endif

#include "pcg_exports_device.h"

class CjvxPortAudioDevice: 
	public CjvxAudioDevice,  
	public genPortAudio_device
{
	/*
	typedef struct
	{
		std::string name;
		bool isSelected;
	} oneChannel;
	*/
private:
	struct
	{
		jvxSize id;
		const PaDeviceInfo *deviceInfo;
	}my_props;

	struct
	{
		int number_bits_sample;
		int bytes_space_sample;

		long minSizeBuffers;
		long maxSizeBuffers;
		long preferredSizeBuffers;
		long granBuffers;

		std::vector<long> sizesBuffers;

		std::vector<long> samplerates;
		std::vector<PaSampleFormat> formats;

		bool available;

		struct
		{
			IjvxToolsHost* theToolsHost;
			struct
			{
				IjvxObject* object;
				IjvxThreadController* hdl;
			} coreHdl;

			jvxBool syncAudioThreadAssoc;
			jvxInt32 syncAudioThreadId;
		} threads;

		std::vector<jvxInt32> usefulBSizes;
	} runtime;

	struct
	{
		jvxSize numInputChannels;
		jvxSize numOutputChannels;

		std::vector<jvxSize> inputMapper;
		std::vector<jvxSize> outputMapper;

		//jvxLinkDataDescriptor theData;

		jvxTimeStampData theTimestamp;
		jvxInt64 timestamp_previous;
		jvxData deltaT_average_us;
		jvxData deltaT_theory_us;

		PaStream* theStream;
	} inProcessing;

	const PaHostApiInfo* hostInfo = nullptr;
#ifdef JVX_OS_WINDOWS
	struct PaWasapiStreamInfo wasapiInfo;
#endif


public:

	JVX_CALLINGCONVENTION CjvxPortAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxPortAudioDevice();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate, 
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans
	)override;

	// Interface IjvxConfiguration

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "", jvxInt32 lineno = -1)override;

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
		jvxSize pipeline_offset , 
		jvxSize* idx_stage,
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

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	
	// Better han
	//virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;	

	// ===================================================================================

	void updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);

	// ================================================================================================

	void props_init(jvxSize id, const PaDeviceInfo *deviceInfo)
	{
		my_props.id = id;
		my_props.deviceInfo = deviceInfo;
	};

	jvxErrorType processBuffer_callback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags);

	


};

#endif
