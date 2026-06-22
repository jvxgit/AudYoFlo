
#define JVX_NODE_TYPE_SPECIFIER_TYPE JVX_COMPONENT_AUDIO_NODE
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "audio_node"

#include "jvxNodes/CjvxBareNode1ioRearrange.h"

#include "jvx_resampler/jvx_fixed_resampler.h"
#include "common/CjvxInputOutputSettings.h"

/*
enum class jvxBufferingMode
{
	JVX_BUFFERING_VARIABLE,
	JVX_BUFFERING_FIXED
};
*/
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


	/*
	struct
	{
		// jvxSize granularityOut = 1;
		jvx_fixed_resampler_conversion cc;
		jvxSize bSizeInMin = 0;
		jvxSize bSizeInMax = 0;
		jvxSize bSizeOutMin = 0;
		jvxSize bSizeOutMax = 0;
	} resampling;
	*/

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

	template <class T>
	class dataBaseProcessing
	{
	public:
		T** bufsInConvert = nullptr;
		T** bufsOutConvert = nullptr;
		T** ptrFieldBuffer = nullptr;
	};

	struct
	{
		jvxBool active_resampling = false;
		jvxBool active_rechannel = false;
		jvxBool active_retype_input = false;
		jvxBool active_retype_output = false;
		jvxBool requiresRebuffering = false;
		jvxBool requiresRebufferHeadroom = false;
		jvxSize lFieldRebuffer = 0;
		jvxSize fFieldRebuffer = 0;
		jvxSize nCFieldRebuffer = 0;
		jvxSize lFieldRebufferChannel = 0;
		jvxBool requiresHeadroom = false;
		jvx_fixed_resampler* fldResampler = nullptr;
		jvxSize numResampler = 0;
		jvxDataFormat commonFormat = JVX_DATAFORMAT_DATA;
		
		dataBaseProcessing<jvxData>* bufsConvert_data = nullptr;
		dataBaseProcessing<jvxInt32>* bufsConvert_int32 = nullptr;
		dataBaseProcessing<jvxInt16>* bufsConvert_int16 = nullptr;
		dataBaseProcessing<jvxInt64>* bufsConvert_int64 = nullptr;
	} runtime;

	jvxResamplerQuality resamplerQuality = jvxResamplerQuality::JVX_RESAMPLER_QUALITY_MEDIUM;

	jvxCBitField checkRequestUpdate = 0;

	CjvxInputOutputSettings currNegoStat;

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
	jvxErrorType accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	void test_set_output_parameters() override;

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

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_config);

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

};
