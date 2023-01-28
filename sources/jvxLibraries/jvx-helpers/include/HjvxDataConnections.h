#ifndef __HJVXDATACONNECTIONS_H__
#define __HJVXDATACONNECTIONS_H__

#include <map>

typedef struct
{
	IjvxConnectorFactory* theConnFac;
	jvxSize theConnFac_id;
} oneSlaveConnection;

typedef struct
{
	IjvxInputConnector* inputC;
	IjvxOutputConnector* outputC;
	std::string bridge_name;
	jvxSize theBId;
	jvxSize uId_to;
	jvxSize uId_from;
} oneBridgeContainer;

typedef struct
{
	std::string process_name;

	IjvxConnectionMasterFactory* theMasterFac;
	jvxSize theMasterFac_id;

	IjvxConnectionMaster* theConnMaster;

	IjvxConnectorFactory* theConnFac;
	jvxSize theConnFac_id;

	std::map<jvxSize, oneSlaveConnection> lstSlaveConnectors;
	std::map<jvxSize, oneBridgeContainer> lstBridgeContainers;

	IjvxDataConnectionProcess* theProc;
	jvxSize theProc_id;
	jvxBool interceptors;
	jvxBool essentialForStart;
} oneStartedConnection;

// ==================================================================

typedef struct
{
	jvxComponentIdentification fromCpTp;
	std::string fromFactoryWildcard;
	std::string fromConnectorWildcard;

	jvxComponentIdentification toCpTp;
	std::string toFactoryWildcard;
	std::string toConnectorWildcard;

	std::string bridge_name;
} jvxConnectionRuleBridge;

class jvxConnectionRuleDropzone
{
public:
	std::string rule_name;
	jvxComponentIdentification cpMaster;
	std::string masterFactoryWildcard;
	std::string masterWildcard;
	std::list<jvxConnectionRuleBridge> bridges;
	jvxBool interceptors;
	jvxBool essential;
	jvxConnectionRuleDropzone()
	{
		interceptors = false;
		essential = false;
		cpMaster = JVX_COMPONENT_UNKNOWN;
	};
};
// =============================================================================


/*
 * CjvxDataConnectionRule -> jvxDescriptConnection -> jvx_connect_description() -> jvxPrepareConnection -> jvx_process_connect()
 */

// ==================================================================================
// ====================================================================================
// ====================================================================================

jvxErrorType jvx_rule_from_dropzone(IjvxDataConnections* theDatConnections, jvxConnectionRuleDropzone* dropzone);

// ====================================================================================

jvxErrorType jvx_put_configuration_dataproc(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theReader, jvxConfigData* sectionToLookFor, const char* fName,
	int lineno, IjvxDataConnections* theDatConnections, std::vector<std::string>& warnMess);

jvxErrorType jvx_put_configuration_dataproc_rules(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theReader, jvxConfigData* sectionToLookFor, const char* fName,
	int lineno, IjvxDataConnections* theDatConnections, std::vector<std::string>& warnMess);

jvxErrorType jvx_put_configuration_dataproc_processes_groups(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theReader, jvxConfigData* theSection,
	const char* fName, int lineno, IjvxDataConnections* theDatConnections, std::vector<std::string>& warnMess,
	jvxBool isProcess);


// ====================================================================================

jvxErrorType jvx_match_slots(IjvxHost* theHost, jvxComponentIdentification& tpThis, const jvxComponentIdentification& tpToThis);

jvxErrorType jvx_find_best_match_bsize_srate(
	jvxSize targetBsize, jvxSize targetSrate,
	const std::vector<std::string>* entriesSizes,
	jvxSize& idxSize,
	const std::vector<std::string>* entriesRates,
	jvxSize& idxRate, 
	jvxInt32* bsize, jvxInt32* srate);


#endif
