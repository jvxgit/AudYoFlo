#ifndef __CJVXTEXTWEBSOCKETS_H__
#define __CJVXTEXTWEBSOCKETS_H__

#include "CjvxWebControl_fe_types.h"
#include "jvx-net-helpers.h"

class CjvxWebControl_fe;

class CjvxTextWebSockets
{

private:
	CjvxWebControl_fe* hostRef = nullptr;

public:
	// This is the binary socket of which there is always only one

	// Here, we hold all "simple" sockets
	// We need no LOCK here since the connections are always mapped into main thread
	std::map<jvxHandle*, jvxWsTextConnection> connectedTextSockets;

	jvxErrorType register_text_socket_main_loop(jvxWebContext* ctxt);
	jvxErrorType unregister_text_socket_main_loop(jvxHandle* refCtxt);

	void initialize(CjvxWebControl_fe* hostrefArg);

	jvxErrorType process_incoming_text_message(jvxOneWsTextRequest* arg);
};

#endif
