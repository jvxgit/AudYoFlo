
#define JVX_NODE_TYPE_SPECIFIER_TYPE JVX_COMPONENT_AUDIO_NODE
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "audio_node"

#include "jvxNodes/CjvxBareNode1ioRearrange.h"

#include "jvx_resampler/jvx_fixed_resampler.h"

enum class jvxRateLocationMode
{
	JVX_FIXED_RATE_LOCATION_INPUT,
	JVX_FIXED_RATE_LOCATION_OUTPUT
};

enum class jvxBufferingMode
{
	JVX_BUFFERING_VARIABLE,
	JVX_BUFFERING_FIXED
};

enum class jvxResamplerQuality
{
	JVX_RESAMPLER_QUALITY_LOW,
	JVX_RESAMPLER_QUALITY_MEDIUM,
	JVX_RESAMPLER_QUALITY_HIGH
};

#include "pcg_exports_node.h"

class CjvxAuNConvert: public CjvxBareNode1ioRearrange, 
	public genConvert_node
{
private:


	struct
	{
		// jvxSize granularityOut = 1;
		jvx_fixed_resampler_conversion cc;
		jvxSize bSizeInMin = 0;
		jvxSize bSizeInMax = 0;
		jvxSize bSizeOutMin = 0;
		jvxSize bSizeOutMax = 0;
	} resampling;

	struct
	{
		jvxSize numIn = JVX_SIZE_UNSELECTED;
		jvxSize numOut = JVX_SIZE_UNSELECTED;
	} reChannel;

	struct
	{
		jvxDataFormat formIn = JVX_DATAFORMAT_NONE;
		jvxDataFormat formOut = JVX_DATAFORMAT_NONE;
	} reType;

	struct
	{
		jvxBool active_resampling = false;
		jvxBool active_rechannel = false;
		jvxBool active_retype = false;
		jvxBool requiresRebuffering = false;
		jvxBool requiresRebufferHeadroom = false;
		jvxSize lFieldRebuffer = 0;
		jvxSize fFieldRebuffer = 0;
		jvxSize nCFieldRebuffer = 0;
		jvxSize lFieldRebufferChannel = 0;
		jvxHandle** ptrFieldBuffer = nullptr;
		jvxBool requiresHeadroom = false;
		jvx_fixed_resampler* fldResampler = nullptr;
		jvxSize numResampler = 0;
	} runtime;

	jvxRateLocationMode fixedLocationMode = jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT;
	jvxResamplerQuality resamplerQuality = jvxResamplerQuality::JVX_RESAMPLER_QUALITY_MEDIUM;

	jvxCBitField checkRequestUpdate = 0;
	jvxCBitField whatChanged = 0;

public:

	JVX_CALLINGCONVENTION CjvxAuNConvert(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxAuNConvert();

	// ===================================================================================
	jvxErrorType activate()override;
	jvxErrorType deactivate()override;

	// ===================================================================================
	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	jvxErrorType accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	void from_input_to_output() override;

	// ===================================================================================
	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	jvxErrorType process_start_icon(
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL) override;

	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	void adapt_output_parameters_forward();
	void adapt_output_parameters_backward();

	void compute_buffer_relations(jvxBool fromInput);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_config);

};
