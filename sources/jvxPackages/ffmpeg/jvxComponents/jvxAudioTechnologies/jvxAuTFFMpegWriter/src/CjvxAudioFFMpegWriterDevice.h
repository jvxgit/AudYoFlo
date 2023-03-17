#ifndef __JVXAUTFILEWRITER_DEVICE_H__
#define __JVXAUTFILEWRITER_DEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"
#include "audio_file_writer_common.h"
#include "jvx_audiocodec_helpers.h"
#include "jvxWavWriter.h"

#include "jvx-ffmpeg-helpers.h"

#include "pcg_exports_device.h"

enum class jvxAudioFFMpegWriteFiletype
{
	// WAV File
	JVX_FFMPEG_FILEWRITER_WAV, 
	
	// MP3 File
	JVX_FFMPEG_FILEWRITER_MP3,
	
	// AAC File
	JVX_FFMPEG_FILEWRITER_M4A,
	
	JVX_FFMPEG_FILEWRITER_LIMIT
};

struct jvxFfmpegOutputFileParameter : public jvxFfmpegFileAudioParameter
{
	const AVOutputFormat* fmt = nullptr;
	AVFormatContext* oc = nullptr;

	AVCodecContext* cctx = nullptr;
	const AVCodec* cod = nullptr;

	// AVFormatContext* ic = nullptr;
	// AVPacket* pkt = nullptr;
	// AVInputFormat* iformat = nullptr;
	// AVSampleFormat sFormatId = AV_SAMPLE_FMT_NONE;
	// jvxSize bSizeMax = 0;
	// jvxSize sizePerSample = 0;
	void reset()
	{
		jvxFfmpegFileAudioParameter::reset();
		// ic = nullptr;
		// pkt = nullptr;
		// iformat = nullptr;
		// sFormatId = AV_SAMPLE_FMT_NONE;
		// bSizeMax = 0;
		// sizePerSample = 0;
	};
};

class CjvxAudioFFMpegWriterTechnology;

class CjvxAudioFFMpegWriterDevice :
	public CjvxAudioDevice,
    public genFFMpegWriter_device
{
	friend class CjvxAudioFFMpegWriterTechnology;

	enum class jvxAudioFFMpegWriterBufferStatus
	{
		JVX_BUFFER_STATUS_NONE,
		JVX_BUFFER_STATUS_CHARGING,
		JVX_BUFFER_STATUS_OPERATION,
		JVX_BUFFER_STATUS_COMPLETE
	};

private:
	
	CjvxAudioFFMpegWriterTechnology* parentTech = nullptr;
	
	std::string fileprefix;
	std::string foldername;

	std::string last_error;
	// jvxApiString format_descriptor;

	wav_params file_params;
	std::string config_compact;

	jvxWavWriter wavFileWriter;
	// fileprops_wav_base wav_props_init;

	

	jvxFfmpegOutputFileParameter fParams;

public:
	JVX_CALLINGCONVENTION CjvxAudioFFMpegWriterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAudioFFMpegWriterDevice();
	
	// =================================================================
	// Some very local functions, no interface
	jvxErrorType init_parameters(
		const std::string& filename_folder,
		const std::string& filename_prefix,
		jvxAudioFFMpegWriteFiletype fType,
		jvxSize fSType,
		wav_params* wav_init_params,
		const std::string& cfg_compact,
		CjvxAudioFFMpegWriterTechnology* par);

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

	// ===================================================================================
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_wav_parameters);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_fixed);

	void trigger_one_buffer();
	jvxErrorType open_wav_file_for_writing();
	jvxErrorType close_wav_file_for_writing();

	void file_props_2_ayf_props();

};

#endif
