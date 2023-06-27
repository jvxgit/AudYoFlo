#ifndef __CJVXSOCKETSCLIENTFACTORY_H__
#define __CJVXSOCKETSCLIENTFACTORY_H__

#include "CjvxSockets.h"

class CjvxSocketsClientInterface;

/**
 * This class is the entry to all clients. TCP/UDP clients expose one interface, PCAP may expose more.
 * The object of type CjvxSocketsClientFactory is to be initialized according to the intended socket type
 * using the initialize function. Then, the function combo of number_interfaces and request_interface allow to get
 * access to the actual interface.
 * Then, 
 */
class CjvxSocketsClientFactory
{
public:
	
	class oneInterfaceReference
	{
	public:
		CjvxSocketsClientInterface* ptr = nullptr;
		jvxSocketsConnectionType socketType = jvxSocketsConnectionType::JVX_SOCKET_TYPE_TCP;
		jvxBool inUse = false;
	};

protected:
	std::list<oneInterfaceReference> interfaces;
	

public:
	CjvxSocketsClientFactory();
	jvxErrorType initialize(jvxSocketsConnectionType tp);

	jvxErrorType number_interfaces(jvxSize* num);
	jvxErrorType request_interface(CjvxSocketsClientInterface** ifptr,
		jvxSocketsConnectionType* tp = NULL,
		jvxSize id = 0);
	jvxErrorType return_interface(CjvxSocketsClientInterface* ifptr);

	jvxErrorType terminate();
};
#endif