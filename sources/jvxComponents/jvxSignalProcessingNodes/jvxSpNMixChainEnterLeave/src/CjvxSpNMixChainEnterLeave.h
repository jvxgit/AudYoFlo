#ifndef __CJVXSPNMIXCHAINENTERLEAVE_H__
#define __CJVXSPNMIXCHAINENTERLEAVE_H__

#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "common/CjvxSingleInputConnector.h"
#include "common/CjvxSingleOutputConnector.h"
#include "common/CjvxConnectorCollection.h"
#include "CjvxSpNMixChainEnterLeave_common.h"

enum class jvxOperationModeMixChain
{
	JVX_OPERTION_MODE_MIX_CHAIN_INPUT,
	JVX_OPERTION_MODE_MIX_CHAIN_OUTPUT,
	JVX_OPERTION_MODE_MIX_CHAIN_TALKTHROUGH
};

#include "pcg_exports_node.h"

class chanOffsetAndMaxChans: public CjvxConnectorOffsetAndMaxChans
{
public:
	jvxComponentIdentification cpId;
	std::string deviceChannelPrefix = "Channel";
	jvxBool linkIo = false;
};

class CjvxSpNMixChainEnterLeave : public CjvxBareNode1ioRearrange, 
	public CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>,
	public CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>,
	public CjvxSpNMixChainEnterLeaveCommon, public genMixChain
{
	jvxSize uIdGen = 1;

	jvxOperationModeMixChain operationMode = jvxOperationModeMixChain::JVX_OPERTION_MODE_MIX_CHAIN_INPUT;
	
	jvxHandle*** bufsSideChannel = nullptr;
	jvxSize szExtraBuffersChannels = 0;
	jvxSize idxBufferProcess = JVX_SIZE_UNSELECTED;

	jvxHandle*** bufsToStore;

	jvxSize* ptrChannelRoutes = nullptr;
	jvxSize szChannelRoutes = 0;

	std::vector<jvxSize> oldRouting;

	// Add entries via properties:
	// <component identification>, <offset>, <max_chans>
	std::map<jvxComponentIdentification, chanOffsetAndMaxChans> presets_io_routing;
	// std::map<jvxComponentIdentification, jvxBool> presets_inout_coupling;

	/*
	struct
	{
		std::string outConName;
		CjvxSingleOutputConnector* extra_ocon_gen = nullptr;
		std::map<CjvxSingleOutputConnector*, oneConnectorPlusName<CjvxSingleOutputConnector> > selectedOutputConnectors;
		std::map<CjvxSingleOutputConnector*, oneConnectorPlusName<CjvxSingleOutputConnector> > processingOutputConnectors;
	} outputConnectors;
	*/

public:
	JVX_CALLINGCONVENTION CjvxSpNMixChainEnterLeave(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNMixChainEnterLeave();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// Negotiations
	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual void from_input_to_output() override;
	jvxErrorType accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// Processing
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	jvxErrorType set_manipulate_value(jvxSize id, jvxVariant* varray) override;
	jvxErrorType get_manipulate_value(jvxSize id, jvxVariant* varray) override;

	jvxErrorType request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl) override;
	jvxErrorType return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_on_config);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(specify_one_definition);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(remove_one_definition);

	jvxErrorType report_selected_connector(CjvxSingleInputConnector* iconn) override;
	jvxErrorType report_test_connector(CjvxSingleInputConnector* oconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	void report_process_buffers(CjvxSingleInputConnector* iconn, jvxLinkDataDescriptor& datOutThisConnector, jvxSize idxStage) override;

	// =============================================================================

	jvxErrorType report_selected_connector(CjvxSingleOutputConnector* iconn) override;
	jvxErrorType report_test_connector(CjvxSingleOutputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;


	/*
	 * 
	 * Just moved the following member functions into the <CjvxConnectorCollection> base class 
	 * 
	void request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId) override;
	void release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId) override;
	void request_unique_id_start(CjvxSingleOutputConnector* iconn, jvxSize* uId) override;
	void release_unique_id_stop(CjvxSingleOutputConnector* iconn, jvxSize uId) override;
	*/

	virtual void report_process_buffers(CjvxSingleOutputConnector* oconn, jvxLinkDataDescriptor& datOutThisConnector, jvxSize idxStage = JVX_SIZE_UNSELECTED) override;

	static void correct_order_channel_route(jvxSize* ptrChannelRoutes, jvxSize szChannelRoutes);

	bool check_positive_zero_copy()override;

	void offset_channels_to_property();
	jvxErrorType check_preset_channels(CjvxConnectorOffsetAndMaxChans& conParams, jvxBool& linkIoActive, jvxComponentIdentification cpId);
	// jvxErrorType check_linkage_sources(jvxBool& linkInOut, jvxComponentIdentification cpId);

	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	jvxErrorType transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

};

	// ============================================================================

	

#endif