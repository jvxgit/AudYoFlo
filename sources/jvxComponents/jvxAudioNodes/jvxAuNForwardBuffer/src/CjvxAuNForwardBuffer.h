
#define JVX_NODE_TYPE_SPECIFIER_TYPE JVX_COMPONENT_AUDIO_NODE
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "audio_node"

#include "jvxNodes/CjvxBareNode1io_rearrange.h"

enum class jvxOperationMode
{
	JVX_FORWARDBUFFER_BUFFER_INPUT,
	JVX_FORWARDBUFFER_BUFFER_OUTPUT
};

#include "pcg_exports_node.h"

extern "C"
{
#include "jvx_audio_stack_sample_dispenser_cont.h"
#include "jvx_buffer_estimation/jvx_estimate_buffer_fillheight.h"
}

class CjvxAudioStackBuffer
{
protected:
	jvxAudioStackDispenserCont myAudioDispenser;

	JVX_MUTEX_HANDLE safeAccess_lock; // <- lock used in audio sample disposer
	jvxHandle** work_buffers_from_hw = nullptr;
	jvxHandle** work_buffers_to_hw = nullptr;

	jvxHandle* fHeightEstimator = nullptr;
	jvxSize numberEventsConsidered_perMinMaxSection = 0;
	jvxSize num_MinMaxSections = 0;
	jvxData recSmoothFactor = 0.;
	jvxSize numOperations = 0;

public:
	// ==================================================================================================
	CjvxAudioStackBuffer();
	~CjvxAudioStackBuffer();

	static jvxDspBaseErrorType lock_sync_threads_cb(jvxHandle* prv);
	jvxDspBaseErrorType lock_sync_threads_ic();
	static jvxDspBaseErrorType unlock_sync_threads_cb(jvxHandle* prv);
	jvxDspBaseErrorType unlock_sync_threads_ic();

	void start_audiostack(
		jvxSize num_samples_buffer,
		jvxSize number_input_channels,
		jvxDataFormat format,
		jvxBool withFHeightEstimator,
		jvxBool withStartThreshold = true);
	void stop_audiostack();
};

// =====================================================================

class CjvxAuNForwardBuffer: public CjvxBareNode1io_rearrange, 
	public IjvxThreads_report, public CjvxAudioStackBuffer,
	public IjvxManipulate, public genForwardBuffer_node
{
private:
	
	

protected:

	jvxBool bypass_buffer_mode = true;
	jvxOperationMode mode = 
		jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT;

	class proc_params
	{
	public:
		jvxSize buffersize = JVX_SIZE_UNSELECTED;
		jvxSize number_channels = JVX_SIZE_UNSELECTED;
		jvxSize samplerate = JVX_SIZE_UNSELECTED;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
	};

	proc_params input;
	proc_params output;

	refComp<IjvxThreads> refThreads;
	jvxMasterSourceType trigTp = jvxMasterSourceType::JVX_MASTER_SOURCE_INTERNAL_TRIGGER;

	jvxOperationMode buffermode = jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT;
public:

	JVX_CALLINGCONVENTION CjvxAuNForwardBuffer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxAuNForwardBuffer();
	
	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;
	*/

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// ===================================================================================
	virtual jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	virtual void test_set_output_parameters();

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

	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_processing_monitor);

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_manipulate_value(jvxSize id, jvxVariant* varray) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_manipulate_value(jvxSize id, jvxVariant* varray) override;

	// ==================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION startup(jvxInt64 timestamp_us) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION expired(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stopped(jvxInt64 timestamp_us) override;
	void read_samples_to_buffer();
	void write_samples_to_buffer();
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;


};