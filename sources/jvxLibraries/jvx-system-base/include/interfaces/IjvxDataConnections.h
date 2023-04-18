#ifndef __IJVXDATACONNECTIONS_H__
#define __IJVXDATACONNECTIONS_H__

#include "jvx.h"

JVX_INTERFACE IjvxDataConnections;

JVX_INTERFACE IjvxDataConnections_checkready
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxDataConnections_checkready() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION check_status(jvxBitField curStat, jvxApiString* reason_if_not) = 0;
};

JVX_INTERFACE IjvxDataConnectionCommon: public IjvxHiddenInterface, public IjvxObject
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxDataConnectionCommon(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION interceptors_active(jvxBool* is_act) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION connector_factory_is_involved(IjvxConnectorFactory* rem_this) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION unique_id_connections(jvxSize* uid) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_bridges(jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_bridge(jvxSize idx, IjvxConnectorBridge** theBridge) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_bridge( IjvxConnectorBridge* theBridge) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION create_bridge(IjvxOutputConnectorSelect* conn_from, IjvxInputConnectorSelect* conn_to, const char* nm, jvxSize* unqiue_id, jvxBool dedicatedThread = false, jvxBool boostThread = false) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_bridge(jvxSize unique_id) = 0;
	//virtual jvxErrorType JVX_CALLINGCONVENTION remove_bridges(IjvxOutputConnector* conn_from, IjvxInputConnector* conn_to) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_bridges() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connection(jvxApiString* nmRturn) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION misc_connection_parameters(jvxSize* connRuleId, jvxBool* preventStoreConfig) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION set_misc_connection_parameters(jvxSize connRuleIdArg, jvxBool preventStoreConfigArg) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_connection() = 0;

	//! Access the connection context. Typically, all connections arewithin the same main context in the host - but we may define other contexts
	virtual jvxErrorType JVX_CALLINGCONVENTION set_connection_context(IjvxDataConnections* context) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION connection_context(IjvxDataConnections** context) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_connection_association(const char* tag, jvxComponentIdentification cpTp) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION connection_association(jvxApiString* tagOnRet, jvxComponentIdentification* cpTp) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_dependency(IjvxDataConnectionCommon* depedent) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_dependency(IjvxDataConnectionCommon* depedent) = 0;
};

JVX_INTERFACE IjvxDataConnectionGroup : public IjvxDataConnectionCommon
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxDataConnectionGroup() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION status(jvxState* stat) = 0;
};

JVX_INTERFACE IjvxDataConnectionProcess: public IjvxDataConnectionGroup
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxDataConnectionProcess(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION set_category_id(jvxSize catId) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION category_id(jvxSize* catId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION connection_master_factory_is_involved(IjvxConnectionMasterFactory* and_this) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION depends_on_process(jvxSize* uIdProcess) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_chain(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_chain(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	//! Associate master. We can set an owner to prevent auto-unload of the process: If an owner exists you must disconnect the process by calling the disconnect function
	virtual jvxErrorType JVX_CALLINGCONVENTION associate_master(IjvxConnectionMaster* theMaster, IjvxObject* theOwner = NULL) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION associated_master(IjvxConnectionMaster** theMaster) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION match_master(jvxBool* doesMatch, jvxComponentIdentification, const char* wildcardMatch) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION apply_update(jvxConnectionEventType theEvent JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	//virtual jvxErrorType JVX_CALLINGCONVENTION test_master() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION deassociate_master() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION status_chain(jvxBool* testok JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_chain(jvxBool storeProtocol JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_test_on_connect(jvxBool test_on_connect, const jvxLinkDataDescriptor_con_params* init_params = nullptr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_test_on_connect(jvxBool* test_on_connect, jvxLinkDataDescriptor_con_params* init_params = nullptr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_forward_test_depend(jvxBool test_on_connect) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_forward_test_depend(jvxBool* test_on_connect) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION check_process_ready(jvxApiString* reason_if_not_ready) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION detail_connection_not_ready(jvxApiString* reason_if_not_ready) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION marked_essential_for_start(jvxBool* marked_essential) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION iterator_chain(IjvxConnectionIterator** it) = 0;
};

JVX_INTERFACE IjvxDataConnectionRule
{
public:
	virtual ~IjvxDataConnectionRule(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION description_rule(jvxApiString* nmRturn, jvxBool* isDefaultRule) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION mark_rule_default() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION specify_master(const jvxComponentIdentification&, const char* selectionExpressionFac, const char* selectionExpressionMas) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reset_master() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_bridge_specification(const jvxComponentIdentification& cp_id_from, const char* from_factory, const char* from_connector, 
		const jvxComponentIdentification& cp_id_to, const char* to_factory, const char* to_connector, const char* nmBridge,
		jvxBool thread_group = false, jvxBool boost_group = false) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION rem_bridge(jvxSize uniqueId) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION rem_all_bridges() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_master(jvxComponentIdentification*, jvxApiString* selectionExpressionFac, jvxApiString* selectionExpressionMas) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION number_bridges(jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_bridge(jvxSize idx, jvxApiString* name, jvxComponentIdentification* cp_id_from, jvxApiString* from_factory, jvxApiString* from_conn,
		jvxComponentIdentification* cp_id_to, jvxApiString* to_factory, jvxApiString* to_conn) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION status(jvxSize* numMatched) = 0; 

	virtual jvxErrorType JVX_CALLINGCONVENTION try_connect_direct(
		IjvxDataConnections* allConnections,
		IjvxHost* theHost,
		jvxBool* connection_established,
		IjvxReport* rep,
		jvxSize* proc_id,
		jvxBool* rep_global,
		jvxSize ruleId = JVX_SIZE_UNSELECTED) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION update_connected(jvxBool added) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION ready_for_start(jvxApiString* reason_if_not) = 0;

};

JVX_INTERFACE IjvxDataConnections
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxDataConnections(){};
		
	virtual jvxErrorType JVX_CALLINGCONVENTION unique_descriptor(jvxApiString* str) = 0;
	// virtual jvxErrorType JVX_CALLINGCONVENTION set_reference_report(IjvxDataConnection_report* hdlArg) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION uid_for_reference(IjvxDataConnectionCommon* ref, jvxSize* uid, jvxBool* isProcess) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION uid_for_reference(IjvxDataConnectionProcess* ref, jvxSize* uid) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_connections_process(jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_process(jvxSize idx, IjvxDataConnectionProcess** connected) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_process_uid(jvxSize uid, IjvxDataConnectionProcess** connected) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connection_process(IjvxDataConnectionProcess* connected) = 0;
	
	// Special hint: the owner can be set in the associate_master member function
	virtual jvxErrorType JVX_CALLINGCONVENTION create_connection_process(
		const char* nm, 
		jvxSize* unique_id,
		jvxBool interceptors, 
		jvxBool essential_for_start,
		jvxBool verbose_out,
		jvxBool report_global,
		jvxSize idDependsOn = JVX_SIZE_UNSELECTED) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_connection_process(jvxSize unique_id) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION set_ready_handler(IjvxDataConnections_checkready* ptr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION ready_for_start( jvxApiString* reason_if_not) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION ready_for_start(jvxSize uid, jvxApiString* reason_if_not) = 0;

	// ================================================================================================0

	virtual jvxErrorType JVX_CALLINGCONVENTION uid_for_reference(IjvxDataConnectionGroup* ref, jvxSize* uid) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_connections_group(jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_group(jvxSize idx, IjvxDataConnectionGroup** connected) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_group_uid(jvxSize uid, IjvxDataConnectionGroup** connected) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connection_group(IjvxDataConnectionGroup* connected) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION create_connection_group(
		const char* nm, 
		jvxSize* unique_id, 
		const char* descr, 
		jvxBool verbose_out) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_connection_group(jvxSize unique_id) = 0;

	// ================================================================================================0

	virtual jvxErrorType JVX_CALLINGCONVENTION register_connection_factory(IjvxConnectorFactory* theFac) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_connection_factory(
		IjvxConnectorFactory* theFac) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_connection_factories(jvxSize* theFac) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_factory(jvxSize idx, IjvxConnectorFactory** theFac, jvxSize* unique_id) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_factory_uid(jvxSize unique_id, IjvxConnectorFactory** theFac ) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connection_factory(IjvxConnectorFactory* theFac) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_connection_master_factory(IjvxConnectionMasterFactory* theFac) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_connection_master_factory(IjvxConnectionMasterFactory* theFac) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_connection_master_factories(jvxSize* theFac) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_master_factory(jvxSize idx, IjvxConnectionMasterFactory** theFac, jvxSize* unique_id) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_master_factory_uid(jvxSize unique_id, IjvxConnectionMasterFactory** theFac) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connection_master_factory(IjvxConnectionMasterFactory* theFac) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION uid_for_reference(IjvxDataConnectionRule* ref, jvxSize* uid) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_connection_rules(jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_rule(jvxSize idx, IjvxDataConnectionRule** rule_on_return) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_rule_uid(jvxSize uId, IjvxDataConnectionRule** rule_on_return) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connection_rule(IjvxDataConnectionRule* rule_to_return) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION create_connection_rule(
		const char* rule_name, jvxSize* uId, 
		jvxDataConnectionRuleParameters* params, 
		jvxSize catId = JVX_SIZE_UNSELECTED) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_connection_rule(jvxSize uId) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_connection_rules() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_connection() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_process_test(jvxSize uIdProcess, jvxSize* numTested, jvxBool immediatetest) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_process_test_all(jvxSize* numTested) = 0;

};

#endif