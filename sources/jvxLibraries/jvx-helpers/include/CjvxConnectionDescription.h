#ifndef _CJVXCONNECTIONDESCRIPTION__H__
#define _CJVXCONNECTIONDESCRIPTION__H__

class CjvxDescriptBridge
{
public:
	std::string bridge_name;
	jvxBool ded_thread;
	jvxBool boost_thread;
	struct
	{
		std::string connector_factory_name;
		std::string connector_name;
		jvxComponentIdentification tp;
		jvxSize uid;
	} from;
	struct
	{
		std::string connector_factory_name;
		std::string connector_name;
		jvxComponentIdentification tp;
		jvxSize uid;
	} to;
	
	CjvxDescriptBridge();
};

class CjvxConnectionDescription
{
public:

	std::string process_name;
	std::string process_description;
	std::string master_factory_name;
	std::string master_name;
	jvxComponentIdentification master_factory_tp;

	std::list<CjvxDescriptBridge> connectors;

	CjvxConnectionDescription();
	
	jvxErrorType create_process_group_from_description(
		jvxBool isProcess, 
		IjvxDataConnections* allConnections, 
		jvxSize ruleId,
		jvxBool preventStoreConfig,
		jvxBool interceptors, 
		jvxBool essential_for_start, 
		jvxSize* uidCreated,
		jvxBool verbose_mode,
		jvxBool report_automation,
		jvxSize procCatId
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
};

#endif