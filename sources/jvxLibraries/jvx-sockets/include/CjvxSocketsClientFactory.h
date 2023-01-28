#ifndef __CJVXSOCKETSCLIENTFACTORY_H__
#define __CJVXSOCKETSCLIENTFACTORY_H__

#include "CjvxSockets.h"

class CjvxSocketsClientInterface;

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