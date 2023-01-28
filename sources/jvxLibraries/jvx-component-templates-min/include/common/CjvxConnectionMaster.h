#ifndef __CJVXCONNECTIONMASTER_H__
#define __CJVXCONNECTIONMASTER_H__

#include "jvx.h"
#include "CjvxJson.h"

class CjvxObject;
class CjvxConnectionMaster;

class CjvxConnectionMasterIterator : public IjvxConnectionIterator
{
protected:
	CjvxConnectionMaster* parentMaster = nullptr;
public:
	virtual jvxErrorType JVX_CALLINGCONVENTION number_next(jvxSize* num) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_next(jvxSize idx, IjvxConnectionIterator** next) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_component(
		jvxComponentIdentification* cpTp, 
		jvxApiString* modName, 
		jvxApiString* lContext) override;

	void set_master(CjvxConnectionMaster* master);
};

class CjvxConnectionMaster
{
	friend class CjvxConnectionMasterIterator;

protected:

	IjvxDataConnectionCommon* theContext;
	struct
	{
		jvxState state;
		jvxBool isConnected;
		CjvxObject* object;
		std::string name;
		IjvxDataConnectionProcess* refProc;
		IjvxConnectionMasterFactory* myParent;

		IjvxOutputConnector* oconn;
		IjvxConnectionMaster* mas;

		jvxSize postpone_attempts_max;
		jvxCBitField supported_delivery_type;

		jvxComponentIdentification cpTpAss;
		std::string tagAss;
	} _common_set_ld_master;

	CjvxConnectionMasterIterator localIterator;

	// ======================================================
	CjvxConnectionMaster();
	
	~CjvxConnectionMaster();
	
	// ===========================================================================

	jvxErrorType _descriptor_master(jvxApiString* nm);

	// ============================================================================
	// INIT
	// ============================================================================

	jvxErrorType _init_master(CjvxObject* object, std::string name, IjvxConnectionMasterFactory* myParent);

	// ============================================================================
	// TERMINATE
	// ============================================================================

	jvxErrorType _terminate_master();

	// ============================================================================
	// NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME
	// ============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _name_master(jvxApiString* str);

	// ============================================================================
	// DATA DELIVERY TYPE DATA DELIVERY TYPE DATA DELIVERY TYPE DATA DELIVERY TYPE
	// ============================================================================
	
	virtual jvxErrorType _data_delivery_type(jvxCBitField* deltp);

	// ============================================================================
	// SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT
	// ============================================================================

	jvxErrorType _select_master(IjvxDataConnectionProcess* ref);

	// ============================================================================
	// UNSELECT
	// ============================================================================

	jvxErrorType _unselect_master();
	
	// ============================================================================
	// AVAILABLE
	// ============================================================================

	jvxErrorType _available_master(jvxBool* isAvail);

	// ============================================================================
	// PROCESS REFERENCE
	// ============================================================================
	jvxErrorType _associated_process(IjvxDataConnectionProcess** ref);

	// ============================================================================
	// ACTIVATE
	// ============================================================================

	jvxErrorType _activate_master(IjvxOutputConnector* conn_out, IjvxConnectionMaster* mas);

	// ============================================================================
	// DEACTIVATE
	// ============================================================================

	jvxErrorType _deactivate_master(IjvxOutputConnector* conn_out, IjvxConnectionMaster* mas);

	// ============================================================================
	// CONNECT
	// ============================================================================

	jvxErrorType _connect_chain_master(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ============================================================================
	// DISCONNECT
	// ============================================================================

	jvxErrorType _disconnect_chain_master(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ============================================================================
	// PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE
	// ============================================================================

	jvxErrorType _prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _prepare_chain_master_no_link();

	// ============================================================================
	// POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS
	// ============================================================================

	jvxErrorType _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _postprocess_chain_master_no_link();
	jvxBool _check_postprocess_chain_master();

	// ============================================================================
	// START START START START START START START START START START START START START
	// ============================================================================

	jvxErrorType _start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _start_chain_master_no_link();

	// ============================================================================
	// STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP
	// ============================================================================

	jvxErrorType _stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _stop_chain_master_no_link();
	jvxBool _check_stop_chain_master();

	// ============================================================================
	// ============================================================================
	// ============================================================================
	// ============================================================================
	// ============================================================================

	jvxErrorType _parent_master_factory(IjvxConnectionMasterFactory** my_parent);
	// ===========================================================================

	jvxErrorType _inform_changed_master(jvxMasterChangedEventType tp JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ===========================================================================

	jvxErrorType _transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ===========================================================================

	jvxErrorType _transfer_chain_backward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ===========================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// =====================================================================================
	
	virtual jvxErrorType JVX_CALLINGCONVENTION _iterator_chain(IjvxConnectionIterator** it);
	
	// =====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _set_connection_context(IjvxDataConnectionCommon* ctxt);
	virtual jvxErrorType JVX_CALLINGCONVENTION _connection_context(IjvxDataConnectionCommon** ctxt);

	// =====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _connection_association(jvxApiString* astr, jvxComponentIdentification* cpId);
};


#endif