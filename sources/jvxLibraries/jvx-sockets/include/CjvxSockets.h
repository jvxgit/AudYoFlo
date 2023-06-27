#ifndef CJVXSOCKETS_H__
#define CJVXSOCKETS_H__

#include <stdio.h>

#include "jvx.h"

/**
 * This class implememts a socket in any of the given implementations:
 * 1) CjvxSocketsServer -> either a TCP server or a UNIX socket server
 * 2) 
 */
#include "CjvxSocketsApi.h"
#include "CjvxSocketsCommon.h"

#include "CjvxSocketsConnection.h"
#include "CjvxSocketsInterfaceTcpUdp.h"

#include "CjvxSocketsServer.h"

#include "CjvxSocketsClientInterface.h"
#include "CjvxSocketsClientInterfaceTcpUdp.h"
#include "CjvxSocketsClientInterfaceTcp.h"
#include "CjvxSocketsClientInterfaceUnix.h"
#include "CjvxSocketsClientFactory.h"


// ==============================================================================================

/*
class HjvxSocketConnectionUdp: public CjvxSocketConnectionCommon
{
public:
	HjvxSocketConnectionUdp() {};
};

class HjvxSocketConnectionTcp : public CjvxSocketConnectionCommon
{
public:
	HjvxSocketConnectionTcp() {};
};
*/
// ========================================================

#endif