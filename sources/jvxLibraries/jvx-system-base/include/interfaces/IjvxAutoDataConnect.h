#ifndef __IJVXAUTODATACONNECT_H__
#define __IJVXAUTODATACONNECT_H__

JVX_INTERFACE IjvxAutoDataConnect
{
public:
	virtual ~IjvxAutoDataConnect(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION report_connection_factory_to_be_added(
		jvxComponentIdentification tp_activated,
		IjvxConnectorFactory* add
		/*,
		IjvxDataConnections* connections,
		IjvxConnectorFactory* toremove,
		IjvxConnectionMasterFactory* toremovemaster,
		IjvxHost* hostRef
		*/) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_connection_factory_removed(
		jvxComponentIdentification tp_activated,
		IjvxConnectorFactory* removed
		/*,
		IjvxDataConnections* connections,
		IjvxConnectorFactory* toremove,
		IjvxConnectionMasterFactory* toremovemaster,
		IjvxHost* hostRef
		*/) = 0;
};
//JVX_INTERFACE_AUTO_DATA_CONNECT

#endif
