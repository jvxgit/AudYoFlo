#ifndef __JVXAUTFILEREADER_DEVICE_H__
#define __JVXAUTFILEREADER_DEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"
#include "jvx-ffmpeg-helpers.h"

// Must follow jvx_audiocodec_helpers.h
#include "pcg_exports_device.h"

class CjvxAudioFFMpegReaderTechnology;

class CjvxAudioFFMpegReaderDevice :
	public CjvxAudioDevice,
	public IjvxThreads_report,
    public genFFMpegReader_device
{
	friend class CjvxAudioFFMpegReaderTechnology;

	enum class jvxAudioFileReaderBufferStatus
	{
		JVX_BUFFER_STATUS_NONE,
		JVX_BUFFER_STATUS_CHARGING,
		JVX_BUFFER_STATUS_OPERATION,
		JVX_BUFFER_STATUS_COMPLETE
	};

private:
	
	CjvxAudioFFMpegReaderTechnology* parentTech = nullptr;

	std::string last_error;
	jvxApiString format_descriptor;


	jvxBool involve_read_thread = false;
	jvxByte* preuse_buffer_ptr = nullptr;
	jvxSize preuse_buffer_sz = 0;
	jvxSize readposi = 0;
	jvxSize fHeight = 0;
	jvxSize readsize = 0;
	jvxAudioFileReaderBufferStatus bufstatus = jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_NONE;
	JVX_MUTEX_HANDLE safeAccessBuffer;
	refComp<IjvxThreads> refThreads;
	JVX_MUTEX_HANDLE safeAccessRead;
	
	struct jvxFfmpegFileParameter: public jvxFfmpegParameter
	{
		std::string fName;
		
		AVStream* st = nullptr;
		AVFormatContext* ic = nullptr;
		AVPacket* pkt = nullptr;
		AVInputFormat* iformat = nullptr;


		void reset()
		{
			jvxFfmpegParameter::reset();
			st = nullptr;			
			ic = nullptr;
			pkt = nullptr;
			iformat = nullptr;			
		};
	};

	jvxFfmpegFileParameter fParams;

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
	
	void send_one_buffer();
	void send_buffer_thread();
	void send_buffer_direct();

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION startup(jvxInt64 timestamp_us) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION expired(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stopped(jvxInt64 timestamp_us) override;
	void read_samples_to_buffer();

	// ===================================================================================
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_config);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(trigger_command);

};

#endif
