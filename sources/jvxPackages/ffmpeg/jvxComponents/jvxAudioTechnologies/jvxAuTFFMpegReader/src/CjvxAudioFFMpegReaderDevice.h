#ifndef __JVXAUTFILEREADER_DEVICE_H__
#define __JVXAUTFILEREADER_DEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"
#include "jvx-ffmpeg-helpers.h"

// Definition must be BEFORE the include of the generated header to run the translation functions.
enum class processingState
{
	JVX_STATUS_READY,
	JVX_STATUS_RUNNING,
	JVX_STATUS_PAUSED,
	JVX_STATUS_DONE,
	JVX_STATUS_ERROR
};

// Must follow jvx_audiocodec_helpers.h
#include "pcg_exports_device.h"

class CjvxAudioFFMpegReaderTechnology;

class CjvxAudioFFMpegReaderDevice :
	public CjvxAudioDevice,
    public genFFMpegReader_device
{
	friend class CjvxAudioFFMpegReaderTechnology;

private:
	
	CjvxAudioFFMpegReaderTechnology* parentTech = nullptr;

	std::string last_error;
	jvxApiString format_descriptor;
	
	processingState statusOutput = processingState::JVX_STATUS_READY;
	
	
	jvxFfmpegInputFileAudioParameter fParams;
	jvxBool triggeredRestart = false;
	jvxBool fwd10triggered = false;
	jvxBool bwd10triggered = false;
	uint64_t latest_pts = 0;

public:
	JVX_CALLINGCONVENTION CjvxAudioFFMpegReaderDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAudioFFMpegReaderDevice();
	
	// =================================================================
	// Some very local functions, no interface
	jvxErrorType init_from_filename(const std::string& fname, CjvxAudioFFMpegReaderTechnology* par);
	jvxErrorType term_file();

	std::string get_last_error();
	// =================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	// Interface IjvxConfiguration

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(
		jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename = "", jvxInt32 lineno = -1 )override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(
		jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================
	// New linkdata connection interface
	// ===================================================================================

	// ===================================================================================
	virtual jvxErrorType test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	void test_set_output_parameters() override; 

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(
		JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
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

	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	
	jvxErrorType send_one_buffer();
	jvxErrorType send_buffer_direct();

	// ===================================================================================

	void restart_stream();
	void skip_stream(jvxData skipPos, uint64_t curPos);

	// ===================================================================================
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_config);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(trigger_command);

};

#endif
