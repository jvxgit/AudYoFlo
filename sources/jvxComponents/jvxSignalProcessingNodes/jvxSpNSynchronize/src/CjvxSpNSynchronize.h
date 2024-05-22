#ifndef __CJVXSPNSYNCHRONIZE_H__
#define __CJVXSPNSYNCHRONIZE_H__

#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "CjvxSpNSynchronize_sec.h"

#include "pcg_exports_node.h"

class CjvxSpNSynchronize : public CjvxBareNode1ioRearrange, public genSynchronize
{
	friend class CjvxSpNSynchronize_sec;
protected:

	CjvxSpNSynchronize_sec sec_master;
	jvxSynchronizeBufferMode bufferMode = jvxSynchronizeBufferMode::JVX_SYNCHRONIZE_UNBUFFERED_PULL;

public:
	JVX_CALLINGCONVENTION CjvxSpNSynchronize(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNSynchronize();

	// ===================================================

	jvxErrorType select(IjvxObject* owner)override;
	jvxErrorType unselect()override;

	// ===================================================

	jvxErrorType activate()override;
	jvxErrorType deactivate()override;

	// =================================================================================
	// Extend interfaces by master connnector factory
	// =================================================================================
	jvxErrorType request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	jvxErrorType return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;

	// Negotiations
	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	/*


	virtual void from_input_to_output() override;
	jvxErrorType accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// Processing
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

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

	jvxErrorType report_selected_connector(CjvxSingleInputConnector* iconn) override;
	void request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId) override;
	void release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId) override;
	void report_process_buffers(CjvxSingleInputConnector* iconn, jvxHandle** bufferPtrs, const jvxLinkDataDescriptor_con_params& params) override;

	// =============================================================================

	jvxErrorType report_selected_connector(CjvxSingleOutputConnector* iconn) override;
	void request_unique_id_start(CjvxSingleOutputConnector* iconn, jvxSize* uId) override;
	void release_unique_id_stop(CjvxSingleOutputConnector* iconn, jvxSize uId) override;
	virtual void report_process_buffers(CjvxSingleOutputConnector* oconn, jvxHandle** bufferPtrs, const jvxLinkDataDescriptor_con_params& params) override;

	static void correct_order_channel_route(jvxSize* ptrChannelRoutes, jvxSize szChannelRoutes);
	*/
};

	// ============================================================================

	

#endif