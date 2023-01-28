#include "jvxNodes/CjvxBareNode1io_zerocopy.h"
#include "jvxText2Speech/CjvxTxt2Speech_mix.h"
#include "jvx_circbuffer/jvx_circbuffer.h"
#include "pcg_exports_node.h"

class CjvxAuNPlayChannelId : public CjvxBareNode1io_zerocopy, public genChannelPlayback_node
{
private:

	typedef enum
	{
		JVX_CHANNEL_OUTPUT_STATE_NONE,
		JVX_CHANNEL_OUTPUT_STATE_READY,
		JVX_CHANNEL_OUTPUT_STATE_START_PENDING, 
		JVX_CHANNEL_OUTPUT_STATE_PROCESSING,
		JVX_CHANNEL_OUTPUT_STATE_STOP_PENDING,
		JVX_CHANNEL_OUTPUT_STATE_COMPLETE
	} jvxChannelPlaybackStatus;

	typedef struct
	{
		jvxData* fld;
		jvxSize progress;
	} oneT2SOutput;


	IjvxObject* theText2Speech_obj;
	IjvxText2Speech* theText2Speech_hdl;

	struct
	{
		jvxData res_fac;
		jvxSize res_bsize;

		jvxData* resampleBuffer;
		jvx_circbuffer* theIrsOutput;

		std::vector<oneT2SOutput> theT2SOutputs;
		JVX_MUTEX_HANDLE safeAccessOutput;
		jvxSize id_Channel;
	} inProcessing;

	CjvxTxt2Speech_mix mixT2S;
	jvxChannelPlaybackStatus playback_status;

	public:

	JVX_CALLINGCONVENTION CjvxAuNPlayChannelId(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxAuNPlayChannelId();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_processing_props);

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;
};