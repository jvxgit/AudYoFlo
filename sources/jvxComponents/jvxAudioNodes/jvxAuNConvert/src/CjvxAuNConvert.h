
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

#include "pcg_exports_node.h"

class CjvxAuNConvert: public CjvxBareNode1ioRearrange, 
	
	public genConvert_node
{
private:


	struct
	{
		jvxSize granularityIn = 1;
		jvxSize granularityOut = 1;
		jvx_fixed_resampler_conversion cc;
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
	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	void adapt_output_parameters();
	jvxErrorType computeResamplerOperation(jvxSize sRateIn, jvxSize sRateOut,
		jvxSize bSizeIn, jvxSize bSizeOut, jvxSize runCnt, jvxLinkDataDescriptor& tryThis
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	/*

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ====================================================================================
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_level_pre);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_level_post);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_passthru);
	*/

};
