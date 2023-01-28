#ifndef __CJVXSOCKETCLIENTINTERFACEPCAP_H__
#define __CJVXSOCKETCLIENTINTERFACEPCAP_H__

#include "CjvxSockets.h"
#include "CjvxSocketsPcap.h"

class CjvxSocketsClientInterfacePcap;

class CjvxSocketsConnectionPcap : public CjvxSocketsConnection
{
	CjvxSocketsClientInterfacePcap* parent = nullptr;
public:
	CjvxSocketsConnectionPcap() {};
	jvxErrorType configure(CjvxSocketsClientInterfacePcap* par);
	void main_loop() override;
};

class CjvxSocketsClientInterfacePcap : public CjvxSocketsClientInterface
{
public:

	// ========================================================
	// PCAP stuff
	// ========================================================
	std::string dev_description;
	pcap_t* thePcapSocket = nullptr;

#ifndef JVX_PCAP_DIRECT_SEND
	pcap_send_queue* my_send_queue = nullptr;
#endif

	char mac[6];
	
public:
	CjvxSocketsClientInterfacePcap();

	virtual jvxErrorType pre_start_socket() override;
	virtual jvxErrorType start_socket()override;
	virtual jvxErrorType set_opts_socket() override;
	virtual jvxErrorType connect_socket()override;
	virtual jvxErrorType start_connection_loop()override;

	virtual jvxErrorType stop_connection_loop()override;
	virtual jvxErrorType stop_socket() override;

};
#endif 
