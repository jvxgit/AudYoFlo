#ifndef _CJVXSINGLEMASTER_H_
#define _CJVXSINGLEMASTER_H_

#include "jvx.h"

class CjvxSingleMaster;

JVX_INTERFACE CjvxSingleMaster_report
{
public:
	virtual ~CjvxSingleMaster_report() {};

	virtual void before_test_chain(CjvxSingleMaster* mas) = 0;
	virtual void before_connect_chain() = 0;
};

class CjvxSingleMaster : public IjvxConnectionMaster, public IjvxConnectionIterator
{
public:
	class common_set_ma_common_t
	{
	public:
		IjvxObject* object = nullptr;
		std::string descriptor = "master";
		std::string name = "CjvxSingleMaster - master";

		IjvxConnectionMasterFactory* myParent = nullptr;
		IjvxOutputConnector* conn_out = nullptr;
		jvxSize myRuntimeId = JVX_SIZE_UNSELECTED;

		jvxSize postpone_attempts_max = 1;
		CjvxSingleMaster_report* cbRef = nullptr;
	};

	common_set_ma_common_t _common_set_ma_common;

	struct
	{
		IjvxDataConnectionProcess* proc = nullptr;
		IjvxDataConnectionCommon* connCtx = nullptr;
		jvxState stat = JVX_STATE_INIT;		
	} runtime;

public:
	CjvxSingleMaster();
	jvxErrorType name_master(jvxApiString* str) override;
	jvxErrorType data_delivery_type(jvxCBitField* delTp) override;
	jvxErrorType select_master(IjvxDataConnectionProcess* ref) override;
	jvxErrorType unselect_master() override;
	jvxErrorType available_master(jvxBool* isAvail) override;
	jvxErrorType associated_process(IjvxDataConnectionProcess** ref) override;
	jvxErrorType set_connection_context(IjvxDataConnectionCommon* ctxt) override;
	jvxErrorType connection_context(IjvxDataConnectionCommon** ctxt) override;
	jvxErrorType connect_chain_master(const jvxChainConnectArguments& params,const jvxLinkDataDescriptor_con_params* init_params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType disconnect_chain_master(const jvxChainConnectArguments& params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType inform_changed_master(jvxMasterChangedEventType tp JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType transfer_chain_backward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType parent_master_factory(IjvxConnectionMasterFactory** my_parent) override;
	jvxErrorType descriptor_master(jvxApiString* name) override;
	jvxErrorType iterator_chain(IjvxConnectionIterator** it) override;
	jvxErrorType connection_association(jvxApiString* astr, jvxComponentIdentification* cpId) override;

	// ===============================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION number_next(jvxSize* num) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_next(jvxSize idx, IjvxConnectionIterator** next) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_component(
		jvxComponentIdentification* cpTp,
		jvxApiString* modName,
		jvxApiString* description,
		jvxApiString* lContext) override;

};

#endif
