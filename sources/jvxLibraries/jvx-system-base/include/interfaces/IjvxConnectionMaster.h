#ifndef __IJVXCONNECTIONMASTERFACTORY_H__
#define __IJVXCONNECTIONMASTERFACTORY_H__

JVX_INTERFACE IjvxDataConnectionProcess;
JVX_INTERFACE IjvxConnectionMasterFactory;

JVX_INTERFACE IjvxConnectionMaster//: public IjvxConnectionIterator
{
public:
  virtual ~IjvxConnectionMaster(){};
  
	virtual jvxErrorType JVX_CALLINGCONVENTION name_master(jvxApiString* str) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION data_delivery_type(jvxCBitField* delTp) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION select_master(IjvxDataConnectionProcess* ref) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_master() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION available_master(jvxBool* isAvail) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION associated_process(IjvxDataConnectionProcess** ref) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_connection_context(IjvxDataConnectionCommon* ctxt) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION connection_context(IjvxDataConnectionCommon** ctxt) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_chain_master(const jvxChainConnectArguments& params,
		const jvxLinkDataDescriptor_con_params* init_params 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_chain_master(const jvxChainConnectArguments& params 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION inform_changed_master(jvxMasterChangedEventType tp JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_chain_backward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION parent_master_factory(IjvxConnectionMasterFactory** my_parent) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_master(jvxApiString* name) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION iterator_chain(IjvxConnectionIterator** it) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION connection_association(jvxApiString* astr, jvxComponentIdentification* cpId) = 0;
};

JVX_INTERFACE IjvxConnectionMasterFactory : public IjvxInterfaceReference
{
public:
	virtual ~IjvxConnectionMasterFactory(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION number_connector_masters(jvxSize* num_in_connectors) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connector_master(jvxSize idx, IjvxConnectionMaster** ref) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connector_master( IjvxConnectionMaster* ref) = 0;
};

#endif
