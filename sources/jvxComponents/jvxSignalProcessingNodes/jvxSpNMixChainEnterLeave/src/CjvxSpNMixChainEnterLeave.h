#ifndef __CJVXSPNMIXCHAINENTERLEAVE_H__
#define __CJVXSPNMIXCHAINENTERLEAVE_H__

#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "common/CjvxSingleInputConnector.h"
#include "common/CjvxSingleOutputConnector.h"

enum class jvxOperationModeMixChain
{
	JVX_OPERTION_MODE_MIX_CHAIN_INPUT,
	JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT,
	JVX_OPERTION_MODE_MIX_CHAIN_TALKTHROUGH
};

#include "pcg_exports_node.h"

class CjvxSpNMixChainEnterLeave : public CjvxBareNode1ioRearrange, 
	public CjvxSingleInputConnector_report,
	public CjvxSingleOutputConnector_report,
	public genMixChain
{
	jvxSize uIdGen = 1;

	template <class T> class oneInputConnectorPlusName
	{
	public:
		T* iconn = nullptr;
		std::string nmUnique;
		jvxBool inProcessing = false;
	};

	jvxOperationModeMixChain operationMode = jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT;
	
	jvxHandle*** bufsSideChannel = nullptr;
	jvxSize szExtraBuffersChannels = 0;
	jvxSize idxBufferProcess = JVX_SIZE_UNSELECTED;

	jvxHandle*** bufsToStore;

	JVX_MUTEX_HANDLE safeAccessInputsOutputs;

	struct
	{
		CjvxSingleInputConnectorMulti* extra_icon_gen = nullptr;
		std::string inConName;
		std::map<CjvxSingleInputConnector*, oneInputConnectorPlusName<CjvxSingleInputConnector> > selectedInputConnectors;
		std::map<CjvxSingleInputConnector*, oneInputConnectorPlusName<CjvxSingleInputConnector> > processingInputConnectors;
	} inputConnectors;

	std::string outConName;
	CjvxSingleOutputConnector* extra_ocon_gen = nullptr;
	std::list<CjvxSingleOutputConnector*> selectedOutputConnectors;
	std::list<CjvxSingleOutputConnector*> processingOutputConnectors;

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

	jvxErrorType report_selected_connector(CjvxSingleInputConnector* iconn) override;
	void request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId) override;
	jvxErrorType report_started_connector(CjvxSingleInputConnector* iconn) override;

	jvxErrorType report_stopped_connector(CjvxSingleInputConnector* iconn) override;
	void release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId) override;
	jvxErrorType report_unselected_connector(CjvxSingleInputConnector* iconn) override;

	virtual void report_process_buffers(CjvxSingleInputConnector* iconn, jvxHandle** bufferPtrs, const jvxLinkDataDescriptor_con_params& params) override;

	jvxErrorType report_selected_connector(CjvxSingleOutputConnector* iconn);
	jvxErrorType report_unselected_connector(CjvxSingleOutputConnector* iconn);
};

#endif