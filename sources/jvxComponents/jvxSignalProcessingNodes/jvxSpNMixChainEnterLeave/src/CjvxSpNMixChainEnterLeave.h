#ifndef __CJVXSPNMIXCHAINENTERLEAVE_H__
#define __CJVXSPNMIXCHAINENTERLEAVE_H__

#include "jvxNodes/CjvxBareNode1ioRearrange.h"

enum class jvxOperationModeMixChain
{
	JVX_OPERTION_MODE_MIX_CHAIN_INPUT,
	JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT,
	JVX_OPERTION_MODE_MIX_CHAIN_TALKTHROUGH
};

#include "pcg_exports_node.h"

class CjvxSpNMixChainEnterLeave : public CjvxBareNode1ioRearrange, public genMixChain
{

	jvxOperationModeMixChain operationMode = jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT;
	
	jvxHandle*** bufsSideChannel = nullptr;
	jvxSize szExtraBuffersChannels = 0;

	jvxHandle*** bufsToStore;

public:
	JVX_CALLINGCONVENTION CjvxSpNMixChainEnterLeave(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNMixChainEnterLeave();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// Negotiations
	virtual void from_input_to_output() override;
	jvxErrorType accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// Processing
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_on_config);
};

#endif