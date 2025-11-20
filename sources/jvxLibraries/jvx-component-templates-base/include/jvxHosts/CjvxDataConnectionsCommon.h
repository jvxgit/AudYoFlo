#ifndef __CJVXDATACONNECTIONCOMMON_H__
#define __CJVXDATACONNECTIONCOMMON_H__

#include "jvx.h"
#include "jvxHosts/CjvxDataConnections_types.h"
#include "jvxHosts/CjvxConnectorBridge.h"
#include "common/CjvxObject.h"

#include "common/CjvxDataChainInterceptor.h"

class CjvxDataConnections;

class CjvxDataConnectionCommon: public CjvxObject
{
protected:

	struct oneChainTriggerIdElement
	{
		std::list<IjvxTriggerInputConnector*> itcons;
		std::list<IjvxTriggerOutputConnector*> otcons;
	};

	struct oneInterceptor
	{
		CjvxDataChainInterceptor* ptr;
		jvxBool fromConnected;
		jvxBool toConnected;
		oneInterceptor()
		{
			ptr = NULL;
			fromConnected = false;
			toConnected = false;
		};
	};

	struct
	{
	
		std::map<jvxSize, oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > > bridges;
		std::map<jvxSize, oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > > bridges_orig;
		jvxSize unique_id_bridge = JVX_SIZE_UNSELECTED;
		jvxSize unique_id_system = JVX_SIZE_UNSELECTED;
		std::string name;
		//jvxState theState;
		jvxBool interceptors = false;
		jvxBool verbose_out = false;
		jvxBool report_automation = false;
		jvxSize id_conn_category = JVX_SIZE_UNSELECTED;
	} _common_set_conn_comm;

	jvxBool preventStoredInConfig = false;
	jvxBool allowControlSequencer = true;
	jvxSize connRuleId = JVX_SIZE_UNSELECTED;

	CjvxDataConnections* theParent = nullptr;
	IjvxDataConnections* theContext;

	std::string theTag;
	jvxComponentIdentification theCpTp = JVX_COMPONENT_UNKNOWN;

	IjvxDataConnectionGroup* theDataConnectionGroup;
	std::list<oneInterceptor> theInterceptors;
	std::list <IjvxDataConnectionCommon*> dependencies;

	std::map<jvxSize, oneChainTriggerIdElement> mapIdTriggerConnect;

public:
	CjvxDataConnectionCommon(
		CjvxDataConnections* parentArg,
		const std::string& nm, 
		jvxSize unique_id_system, 
		jvxBool interceptors, 
		const std::string& descr,
		jvxBool verbose_mode,
		jvxBool report_global);

	~CjvxDataConnectionCommon();

	virtual jvxErrorType JVX_CALLINGCONVENTION _interceptors_active(jvxBool* is_act);

	virtual jvxErrorType JVX_CALLINGCONVENTION _unique_id_connections(jvxSize* uid);
	virtual jvxErrorType JVX_CALLINGCONVENTION _descriptor_connection(jvxApiString* str);
	virtual jvxErrorType JVX_CALLINGCONVENTION _misc_connection_parameters(jvxSize* connRuleId, jvxBool* preventStoreConfig, jvxBool* allowControlSequencerArg);
	virtual jvxErrorType JVX_CALLINGCONVENTION _set_misc_connection_parameters(jvxSize connRuleIdArg, jvxBool preventStoredInConfigArg, jvxBool allowControlSequencerArg);

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_bridges(jvxSize* num);
	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_bridge(jvxSize idx, IjvxConnectorBridge** theBridge);
	virtual jvxErrorType JVX_CALLINGCONVENTION _return_reference_bridge(IjvxConnectorBridge* theBridge);

	virtual jvxErrorType JVX_CALLINGCONVENTION _connector_factory_is_involved(IjvxConnectorFactory* rem_this);
	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_connection();
	virtual jvxErrorType JVX_CALLINGCONVENTION _get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section);
	virtual jvxErrorType JVX_CALLINGCONVENTION _set_connection_context(IjvxDataConnections* context);
	virtual jvxErrorType JVX_CALLINGCONVENTION _set_connection_association(const char* tag, jvxComponentIdentification cpTp);
	virtual jvxErrorType JVX_CALLINGCONVENTION _connection_association(jvxApiString* tagOnRet, jvxComponentIdentification* cpTp);
	virtual jvxErrorType JVX_CALLINGCONVENTION _connection_context(IjvxDataConnections** context);
	virtual jvxErrorType JVX_CALLINGCONVENTION _status(jvxState* stat);
	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_bridge_local(jvxSize unique_id);
	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_all_bridges_local();
	//virtual jvxErrorType JVX_CALLINGCONVENTION _set_reference_report(IjvxDataConnection_report* hdlArg);

	virtual jvxErrorType JVX_CALLINGCONVENTION _add_dependency(IjvxDataConnectionCommon* depedent);
	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_dependency(IjvxDataConnectionCommon* depedent);

	virtual void set_name_unique_descriptor(IjvxCallProt* fdb) = 0;
	virtual void get_configuration_local(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section) = 0;
	virtual jvxErrorType create_bridge_check() = 0;
	virtual IjvxConnectionMaster* get_master_ref() = 0;
	virtual jvxErrorType remove_bridge_local(jvxSize unique_id) = 0;
	virtual jvxErrorType remove_all_bridges_local() = 0;

	void remove_interceptor_if(IjvxInputConnector* icon, IjvxOutputConnector* ocon);
	virtual void about_release_intereptor(CjvxDataChainInterceptor* ptr);

	virtual jvxErrorType JVX_CALLINGCONVENTION _link_triggers_connection();
	virtual jvxErrorType JVX_CALLINGCONVENTION _unlink_triggers_connection();

};

#endif
