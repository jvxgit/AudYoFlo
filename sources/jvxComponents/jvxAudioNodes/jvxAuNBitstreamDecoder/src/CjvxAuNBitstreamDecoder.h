
#define JVX_NODE_TYPE_SPECIFIER_TYPE JVX_COMPONENT_AUDIO_NODE
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "audio_node"

#include "jvxNodes/CjvxBareNode1io_rearrange.h"
#include "pcg_exports_node.h"

#include "HjvxMicroConnection.h"

class CjvxAuNBitstreamDecoder: public CjvxBareNode1io_rearrange, 
	public HjvxMicroConnection_hooks_simple,
	public HjvxMicroConnection_hooks_fwd,
	public genBitstreamDecoder_node
{
private:
	class audio_params
	{
	public:
		jvxSize rate = 0;
		jvxSize bsize = 0;
		jvxSize num_channels = 0;
	};
	
	struct audio_params_wav: public audio_params
	{
		jvxSize bitspersample = 16;
		jvxBool fixedpt = true;
		jvxBool lendian = true;
	};

	enum class jvxBiStreamClass
	{
		JVX_AUDIO_BITSTREAM_NONE,
		JVX_AUDIO_BITSTREAM_WAV
	};

	jvxBiStreamClass theBstreamClass = jvxBiStreamClass::JVX_AUDIO_BITSTREAM_NONE;
	audio_params_wav params_wav;

	std::map<jvxSize, refComp<IjvxAudioCodec> > lstCodecInstances;
	jvxSize id_selected = JVX_SIZE_UNSELECTED;

	std::string config_token;
	HjvxMicroConnection* theMicroConnection = nullptr;
	IjvxAudioCodec* ptrCodec = nullptr;
	IjvxAudioDecoder* theDecoder = nullptr;
	IjvxProperties* theDecoderProps = nullptr;

	jvxSize uIdl = JVX_SIZE_UNSELECTED;
	jvxSize refCountHost = 1;
	jvxLinkDataDescriptor decoderDescrIn;
	jvxLinkDataDescriptor decoderDescrOut;

public:

	JVX_CALLINGCONVENTION CjvxAuNBitstreamDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxAuNBitstreamDecoder();

#if 0
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;
#endif

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// ===================================================================================
	jvxErrorType disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// ===================================================================================
	virtual jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
			jvxSize pipeline_offset,
			jvxSize* idx_stage,
			jvxSize tobeAccessedByStage,
			callback_process_start_in_lock clbk,
			jvxHandle* priv_ptr);
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
			jvxSize idx_stage,
			jvxBool operate_first_call,
			jvxSize tobeAccessedByStage,
			callback_process_stop_in_lock cb,
			jvxHandle* priv_ptr);
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_processing_monitor);

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ========================================================================
	// Interface HjvxMicroConnection_hooks_simple
	// ========================================================================

	virtual jvxErrorType hook_test_negotiate(jvxLinkDataDescriptor* proposed JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType hook_test_accept(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType hook_test_update(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType hook_check_is_ready(jvxBool* is_ready, jvxApiString* astr) override;
	virtual jvxErrorType hook_forward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	// ========================================================================
	// Interface HjvxMicroConnection_hooks_ext
	// ========================================================================

	virtual jvxErrorType hook_prepare(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType hook_postprocess(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType hook_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType hook_stop(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType hook_process_start(
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr)override;
	virtual jvxErrorType hook_process(
		jvxSize mt_mask, jvxSize idx_stage) override;
	virtual jvxErrorType hook_process_stop(
		jvxSize idx_stage,
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr) override;

	// ==============================================================================

	jvxErrorType init_microconnection();
	
	jvxErrorType activate_decoder_connection(jvxSize idProc);
	jvxErrorType release_decoder_connection();


};