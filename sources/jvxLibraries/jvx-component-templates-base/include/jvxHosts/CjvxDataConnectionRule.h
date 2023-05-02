#ifndef __CJVXDATACONNECTIONRULE_H__
#define __CJVXDATACONNECTIONRULE_H__

#include "jvx.h"

class CjvxDataConnectionRule: public IjvxDataConnectionRule
{
protected:

	// =====================================================================

	class idAndIdentification_component
	{
	public:
		jvxComponentIdentification idCp;
		std::string sel_expression_fac;
		std::string sel_expression_macon;
		jvxSize idTrigger = JVX_SIZE_UNSELECTED;
	};

	class idAndBridge
	{
	public:
		idAndIdentification_component conn_from;
		idAndIdentification_component conn_to;
		std::string bridge_name;
		jvxBool thread = false;
		jvxBool boost = false;
	};
	
	std::string my_rule_name;
	std::string my_object_descr;

	idAndIdentification_component theMaster;
	std::list<idAndBridge> theBridges;

	jvxDataConnectionRuleParameters params_init;

	jvxBool isDefault;

	jvxSize catId = JVX_SIZE_UNSELECTED;

	jvxSize connectedCnt = 0;

	/*
	jvxBool interceptors;
	jvxBool essential;
	jvxBool connectProcess;
	jvxBool dbgOutput;
	*/

	//std::list<oneStartedConnection> startedConnections;

public:
	CjvxDataConnectionRule(jvxDataConnectionRuleParameters* params, jvxSize cat_id );

  virtual ~CjvxDataConnectionRule(){};

	void set_name(const std::string& nm);
	void add_descr(const std::string& descr);

	// ==================================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION description_rule(jvxApiString* nmReturn, jvxBool* isDefaultRule) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION mark_rule_default()override;

	// ==================================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION specify_master(const jvxComponentIdentification& cpId, const char* selectionExpressionFac, const char*  selectionExpressionMas) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION reset_master() override;

	// ==================================================================================================================


	// ==================================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION add_bridge_specification(const jvxComponentIdentification& cp_id_from, const char* from_factory, const char* from_connector,
		const jvxComponentIdentification& cp_id_to, const char* to_factory, const char* to_connector, 
		const char* nmBridge, jvxBool thread_group, jvxBool boost_group, jvxSize oconIdTrigger, jvxSize iconIdTrigger) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION rem_bridge(jvxSize idCnt) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION rem_all_bridges() override;

	// ==================================================================================================================
	// ==================================================================================================================
	// ==================================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION get_master(jvxComponentIdentification* cpId, jvxApiString* selectionExpressionFac, jvxApiString* selectionExpressionMaster)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_bridges(jvxSize* num)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_bridge(jvxSize idx, jvxApiString* nmBridge, jvxComponentIdentification* cp_id_from, jvxApiString* from_factory, jvxApiString* from_conn,
		jvxComponentIdentification* cp_id_to, jvxApiString* to_factory, jvxApiString* to_conn)override;

	// ==================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION status(jvxSize* numMatched)override;

	// ==================================================================================

	virtual jvxErrorType try_connect_direct(
		IjvxDataConnections* allConnections, 
		IjvxHost* theHost, 
		jvxBool* connection_established, 
		IjvxReport* rep, 
		jvxSize* proc_id, 
		jvxBool* rep_global,
		jvxSize ruleId) override;

	jvxBool _try_auto_connect_bridge_part(
		IjvxDataConnections* allConnections, 
		CjvxConnectionDescription& theNewConnection, 
		IjvxHost* theHost, 
		IjvxReport* rep
	);
	jvxBool _try_auto_connect_bridge_part_finalize(
		IjvxDataConnections* allConnections, 
		CjvxConnectionDescription& theNewConnection,
		IjvxHost* theHost,
		std::list<idAndBridge>::iterator& elmB,
		CjvxDescriptBridge& theBridge,
		IjvxReport* rep
	);

	// =====================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION update_connected(jvxBool added) override;

	// =====================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION ready_for_start(jvxApiString* reason_if_not) override;

	// =====================================================================

	jvxErrorType cleanup_containers(IjvxDataConnections* allConnections, oneStartedConnection& theContainer);
	
	// =====================================================================
	// If all bridges could be established successfully, start the actual 
	// component linakge
	// =====================================================================
	
	// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 
	// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 
	// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 

	virtual jvxErrorType get_master_factory(IjvxDataConnections* allConnections, IjvxConnectionMasterFactory** theMF,
		jvxSize* uIdMF, jvxComponentIdentification& idTarget);

	virtual jvxErrorType release_master_factory(IjvxDataConnections* allConnections, IjvxConnectionMasterFactory* theMF);

	// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 

	virtual jvxErrorType get_connector_factory(IjvxDataConnections* allConnections, IjvxConnectorFactory** theCF,
		jvxSize* uIdCF, jvxComponentIdentification& idTarget);

	virtual jvxErrorType release_connector_factory(IjvxDataConnections* allConnections, IjvxConnectorFactory* theCF);

	// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 

	virtual jvxErrorType get_master(IjvxConnectionMasterFactory* theFactory, IjvxConnectionMaster** theM, const std::string & expression);

	virtual jvxErrorType release_master(IjvxConnectionMasterFactory* theFactory, IjvxConnectionMaster* theM);

	jvxErrorType get_configuration(IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section, jvxFlagTag flagtag);
};

#endif
