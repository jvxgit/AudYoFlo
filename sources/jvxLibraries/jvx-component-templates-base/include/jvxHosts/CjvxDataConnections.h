#ifndef __CJVXDATACONNECTIONS_H__
#define __CJVXDATACONNECTIONS_H__

#include "jvx.h"

#include <map>
#include "CjvxDataConnections_types.h"

#include "CjvxJson.h"
#include "HjvxDataConnections.h"

#include "CjvxDataConnectionsProcess.h"
#include "CjvxDataConnectionsGroup.h"
#include "CjvxDataConnectionRule.h"

//#define JVX_DATACONNECTIONS_VERBOSE


// ========================================================================================================
// ========================================================================================================
// ========================================================================================================
// ========================================================================================================

class CjvxDataConnections
{
	friend class CjvxDataConnectionsCommon;
	friend class CjvxDataConnectionsGroup;
	friend class CjvxDataConnectionsProcess;

protected:

	class 
	{
	public:
		std::map<jvxSize, oneEntrySomething<CjvxDataConnectionsProcess> > connections_process;
		std::map<jvxSize, oneEntrySomething<CjvxDataConnectionsGroup> > connections_group;
		std::map<IjvxConnectorFactory*, oneEntrySomething<IjvxConnectorFactory> > connection_factories;
		std::map<IjvxConnectionMasterFactory*, oneEntrySomething<IjvxConnectionMasterFactory> > connection_master_factories;
		std::map<jvxSize, oneEntrySomething<CjvxDataConnectionRule> > connection_rules;
		jvxSize unique_id = JVX_SIZE_UNSELECTED;
		IjvxHiddenInterface* theHost = nullptr;
		IjvxReport* theReport = nullptr;
		// IjvxDataConnection_report* connReport = nullptr;
	} _common_set_data_connection;
	
	std::string unique_descriptor;
	IjvxDataConnections_checkready* checkReadyRef = nullptr;

	// ================================================================
	class _pending_test_requests
	{
	public:
		JVX_THREAD_ID threadIdMainThread;
		std::list<jvxSize> pendingUids;
		JVX_MUTEX_HANDLE lock;
		jvxBool inCall = false;
		_pending_test_requests() { JVX_INITIALIZE_MUTEX(lock); };
		~_pending_test_requests() { JVX_TERMINATE_MUTEX(lock); };
	};

	_pending_test_requests pending_test_requests;

	// ================================================================

public:
	CjvxDataConnections();
	
	~CjvxDataConnections();
	
	void _set_system_refs(IjvxHiddenInterface* theHostRef, IjvxReport* theReportArg);

	virtual jvxErrorType JVX_CALLINGCONVENTION _unique_descriptor(jvxApiString* str);

	virtual jvxErrorType JVX_CALLINGCONVENTION _set_unique_descriptor(const char* str);
	// virtual jvxErrorType JVX_CALLINGCONVENTION _set_reference_report(IjvxDataConnection_report* hdlArg);
	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_all_connection();

	jvxErrorType _uid_for_reference(IjvxDataConnectionCommon* ref, jvxSize* uid, jvxBool* isProcess);

	// =============================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _uid_for_reference(IjvxDataConnectionProcess* ref, jvxSize* uid);

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_connections_process(jvxSize* num);
	
	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_connection_process(jvxSize idx, IjvxDataConnectionProcess** theProc);

	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_connection_process_uid(jvxSize uid, IjvxDataConnectionProcess** theProc);

	virtual jvxErrorType JVX_CALLINGCONVENTION _return_reference_connection_process(IjvxDataConnectionProcess* theProc);
	
	virtual jvxErrorType JVX_CALLINGCONVENTION _create_connection_process(
		const char* nm,
		jvxSize* unique_id, 
		jvxBool interceptors, 
		jvxBool essential_for_start,
		jvxBool verbose_out,
		jvxBool report_global,
		jvxSize idDependsOn);

	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_connection_process(jvxSize unique_id);

	virtual jvxErrorType JVX_CALLINGCONVENTION _set_ready_handler(IjvxDataConnections_checkready* ptr);
	virtual jvxErrorType JVX_CALLINGCONVENTION _ready_for_start(jvxApiString* reason_if_not);
	virtual jvxErrorType JVX_CALLINGCONVENTION _ready_for_start(jvxSize uId, jvxApiString* reason_if_not);

	// ========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _uid_for_reference(IjvxDataConnectionGroup* ref, jvxSize* uid);

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_connections_group(jvxSize* num);

	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_connection_group(jvxSize idx, IjvxDataConnectionGroup** connected);

	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_connection_group_uid(jvxSize uid, IjvxDataConnectionGroup** theProc);

	virtual jvxErrorType JVX_CALLINGCONVENTION _return_reference_connection_group(IjvxDataConnectionGroup* connected);

	virtual jvxErrorType JVX_CALLINGCONVENTION _create_connection_group(
		const char* nm, 
		jvxSize* unique_id, 
		const std::string& descr,
		jvxBool verbose_out);

	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_connection_group(jvxSize unique_id);

	// ========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _register_connection_factory(IjvxConnectorFactory* theFac);

	virtual jvxErrorType JVX_CALLINGCONVENTION _unregister_connection_factory(IjvxConnectorFactory* theFac);

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_connection_factories(jvxSize* num);

	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_connection_factory(jvxSize idx, IjvxConnectorFactory** theFac, jvxSize* unique_id);

	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_connection_factory_uid(jvxSize unique_id, IjvxConnectorFactory** theFac);

	virtual jvxErrorType JVX_CALLINGCONVENTION _return_reference_connection_factory(IjvxConnectorFactory* theFac);

	// ========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _register_connection_master_factory(IjvxConnectionMasterFactory* theFac);

	virtual jvxErrorType JVX_CALLINGCONVENTION _unregister_connection_master_factory(IjvxConnectionMasterFactory* theFac);

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_connection_master_factories(jvxSize* num);

	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_connection_master_factory(jvxSize idx, IjvxConnectionMasterFactory** theFac, jvxSize* unique_id);

	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_connection_master_factory_uid(jvxSize unique_id, IjvxConnectionMasterFactory** theFac);

	virtual jvxErrorType JVX_CALLINGCONVENTION _return_reference_connection_master_factory(IjvxConnectionMasterFactory* theFac);
	
	// ============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_connection_rules(
		jvxSize* num);
		
	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_connection_rule(
		jvxSize idx, IjvxDataConnectionRule** rule_on_return);

	
	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_connection_rule_uid(
		jvxSize uId, 
		IjvxDataConnectionRule** rule_on_return);

	virtual jvxErrorType JVX_CALLINGCONVENTION _uid_for_reference(
		IjvxDataConnectionRule* ref, jvxSize* uid) ;

	virtual jvxErrorType JVX_CALLINGCONVENTION _return_reference_connection_rule(
		IjvxDataConnectionRule* rule_to_return);

	virtual jvxErrorType JVX_CALLINGCONVENTION _create_connection_rule(
		const char* rule_name, 
		jvxSize* uId, 
		jvxDataConnectionRuleParameters* params,
		jvxSize catId);

	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_connection_rule(jvxSize uId);

	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_all_connection_rules();

	virtual jvxErrorType JVX_CALLINGCONVENTION _add_process_test(jvxSize uIdProcess, jvxSize* numTested, jvxBool immediatetest);
	virtual jvxErrorType JVX_CALLINGCONVENTION _trigger_process_test_all(jvxSize* numTested);

	// =============================================================================================
	jvxErrorType _try_auto_connect(IjvxDataConnections* allConnections, IjvxHost* theHost);

	jvxErrorType get_configuration_dataproc(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* theWriter, jvxConfigData* sectionWhereToAddAllSubsections);

	jvxErrorType get_configuration_dataproc_rules(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* theWriter, jvxConfigData* sectionWhereToAddAllSubsections);

	jvxErrorType remove_connections_involved_factories(IjvxConnectorFactory* rem_this, 
		IjvxConnectionMasterFactory* and_this);
};

#endif
