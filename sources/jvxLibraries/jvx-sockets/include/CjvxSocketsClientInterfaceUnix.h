#ifndef __CJVXSOCKETCLIENTINTERFACEUNIX_H__
#define __CJVXSOCKETCLIENTINTERFACEUNIX_H__

#include "CjvxSockets.h"

class CjvxSocketsClientInterfaceUnix : public CjvxSocketsClientInterfaceTcp
{
public:
	// ========================================================
	// UNIX Socket
	// ========================================================
	jvxBool abstract_file_mode_server = false;

	std::string client_socket = " jvxrt.sock.client";
	jvxBool abstract_file_mode_client = false;

public:
	
	CjvxSocketsClientInterfaceUnix();
	jvxErrorType configure(		
		const std::string& serverNameArg = " jvxrt.sock.server",
		const std::string& clientNameArg = " jvxrt.sock.client");

	virtual jvxErrorType start_socket() override;
	virtual jvxErrorType set_opts_socket() override;
	virtual jvxErrorType connect_socket() override;
};

#endif