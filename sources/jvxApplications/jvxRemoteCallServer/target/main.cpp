#include <iostream>
#include "jvx.h"
#include "jvxTSocket.h"
#include "jvxTRemoteCall.h"
#include "jvx_connect_server_st.h"

#define TCP_UDP_TARGET "192.168.178.121"
#define PCAP_TARGET "40:00:00:00:00:00"

#ifdef JVX_RS232_ACTIVE
#ifdef JVX_OS_WINDOWS
#include "jvxTRs232Win32.h"
#endif
#ifdef JVX_OS_LINUX
#include "jvxTRs232Glnx.h"
#endif
#endif

class remoteCallEndpoint: public IjvxRemoteCall_callback
{
public:
	~remoteCallEndpoint() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION reportPendingError(const char* errorDescription, jvxErrorType tp)
	{
		std::cout << __FUNCTION__ << std::endl;
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reportConnectionEstablished(const char* conn_descriptor)
	{
		std::cout << __FUNCTION__ << std::endl;
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reportConnectionClosed()
	{
		std::cout << __FUNCTION__ << std::endl;
		return JVX_NO_ERROR;
	};
	virtual jvxErrorType JVX_CALLINGCONVENTION reportPendingAction(jvxRCOneParameter* input_params, 
		jvxSize numInParams, jvxRCOneParameter* output_params, jvxSize numOutParams,
		const char* nmAction, jvxSize actionId, jvxCBool* immediateResponse)
	{
		jvxSize i,j;
		if ((std::string)nmAction == "loop_back")
		{
			jvxSize numParams = JVX_MIN(numInParams, numOutParams);
			for(i = 0; i < numParams; i++)
			{ 
				jvxSize segys = JVX_MIN(input_params[i].description.seg_length_y, output_params[i].description.seg_length_y);

				for (j = 0; j < segys; j++)
				{
					jvxSize num = JVX_MIN(input_params[i].description.seg_length_x, output_params[i].description.seg_length_x);
					assert((jvxDataFormat)input_params[i].description.format == (jvxDataFormat)output_params[i].description.format);
					memcpy(output_params[i].content[j], input_params[i].content[j], num * jvxDataFormat_size[input_params[i].description.format]);
				}
			}
			return(JVX_NO_ERROR);
		}
		if(immediateResponse)
		{
			*immediateResponse = c_true;
		}
		std::cout << __FUNCTION__ << std::endl;
		return(JVX_ERROR_ELEMENT_NOT_FOUND);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reportActionRequest(const char* description, jvxSize numinparams, jvxSize numoutparams, jvxSize processId)
	{
		std::cout << __FUNCTION__ << std::endl;
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reportLocalError(const char* description, jvxSize localErrorId)
	{
		std::cout << __FUNCTION__ << std::endl;
		return JVX_NO_ERROR;
	};

};

remoteCallEndpoint myCallbackClass;

int main(int argc, char* argv[])
{
	jvxSize i;
	jvxSize connectionId = JVX_SIZE_UNSELECTED;

	std::string var;
	std::string myIpAddresses;
	std::string myFirstIpAddr;
	std::string myName;
	std::string macAddress;
	std::string portAddress;
	jvxErrorType res = JVX_NO_ERROR;
	int port = 12345;

	jvxApiString fldStr;
	std::string myIpAddr = "Unknown";
	std::string myMacAddr = "Unknown";

	jvxSocketAddressType cfgServer;
	jvx_socket_server_private startServer;
	jvx_connect_spec_tcp cfgTcp;
	jvx_connect_spec_udp cfgUdp;
	jvx_connect_spec_pcap cfgPcap;

	IjvxObject* myRemoteCallo = NULL;
	IjvxRemoteCall* myRemoteCall = NULL;

	IjvxObject* mySocketo = NULL;
	IjvxConnection* mySocket = NULL;

	jvxHandle* ptrPrivStart = NULL;
	std::string txt;
	jvxSize numPorts = 0;
	jvxApiStringList lstStr;

#ifdef JVX_RS232_ACTIVE
	jvxRs232Config cfgRs232;
	IjvxObject* myRs232o = NULL;
	IjvxConnection* myRs232 = NULL;

#if defined(JVX_OS_WINDOWS)
	res = jvxTRs232Win32_init(&myRs232o);
#elif defined(JVX_OS_LINUX)
	res = jvxTRs232Glnx_init(&myRs232o);
#else
#error NO RS232 IMPLEMENTATION AVAILABLE	
#endif

	assert((res == JVX_NO_ERROR) && (myRs232o));
	res = myRs232o->request_specialization(reinterpret_cast<jvxHandle**>(&myRs232), NULL, NULL, NULL);
	assert((res == JVX_NO_ERROR) && (myRs232));
#endif
	
	res = jvxTSocket_init(&mySocketo);
	assert((res == JVX_NO_ERROR) && (mySocketo));
	res = mySocketo->request_specialization(reinterpret_cast<jvxHandle**>(&mySocket), NULL, NULL, NULL);
	assert((res == JVX_NO_ERROR) && (mySocket));

	res = jvxTRemoteCall_init(&myRemoteCallo);
	assert((res == JVX_NO_ERROR) && (myRemoteCallo));
	res = myRemoteCallo->request_specialization(reinterpret_cast<jvxHandle**>(&myRemoteCall), NULL, NULL, NULL);
	assert((res == JVX_NO_ERROR) && (myRemoteCall));

	// =========================================
#ifdef JVX_RS232_ACTIVE
	cfgRs232.bRate = 115200;
	cfgRs232.bits4Byte = 8;
	cfgRs232.stopBitsEnum = JVX_RS232_STOPBITS_ONE;
	cfgRs232.enFlow = JVX_RS232_NO_FLOWCONTROL;
	cfgRs232.parityEnum = JVX_RS232_PARITY_NO;
	cfgRs232.boostPrio = c_false;
	cfgRs232.reportEnum = 0;
	cfgRs232.secureChannel.transmit.active = c_true;
	cfgRs232.secureChannel.transmit.preallocationSize = 0;

	cfgRs232.secureChannel.receive.active = c_true;
	cfgRs232.secureChannel.receive.maxPreReadSize = 128;

	cfgRs232.secureChannel.startByte = JVX_RS232_SECURE_CHANNEL_START_BYTE;
	cfgRs232.secureChannel.stopByte = JVX_RS232_SECURE_CHANNEL_STOP_BYTE;
	cfgRs232.secureChannel.escByte = JVX_RS232_SECURE_CHANNEL_ESC_BYTE;

	jvx_bitSet(cfgRs232.reportEnum, JVX_RS232_REPORT_CONNECTION_ESTABLISHED_SHIFT);
	jvx_bitSet(cfgRs232.reportEnum, JVX_RS232_REPORT_CONNECTION_STOPPED_SHIFT); // No CTS 
#endif
																				
	// =========================================

	std::cout << "Tool to demonstrate test and usage of jvx remote calls" << std::endl;

	while (1)
	{
		while (1)
		{
			std::cout << "Connect via TCP socket[0], UDP Socket [1]" << std::flush;
#ifdef JVX_WITH_PCAP
			std::cout << ",PCAP Socket[2]" << std::flush;
#endif
#ifdef JVX_RS232_ACTIVE
			std::cout << ", RS - 232[3]" << std::flush;
#endif
			std::cout << "?" << std::endl;

			std::cin >> txt;
			if (txt == "0")
			{

				std::cout << "Starting TCP Socket Server on port " << port << std::endl;
				cfgServer.whatToAddress = JVX_SOCKET_ADDRESS_SERVER;
				cfgServer.addressViaWhatProtocol = JVX_SOCKET_OPERATE_VIA_TCP;

				mySocket->initialize(NULL, &cfgServer, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_WHAT_AND_HOW);

				myRemoteCall->initialize(NULL, mySocket, JVX_RC_OPERATE_SLAVE, NULL, &myCallbackClass);

				startServer.numConnectionsMax = 1;
				startServer.priv_start = &cfgTcp;
				startServer.priv_type = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_TCP;
				cfgTcp.disableNagleAlgorithmTcp = 1;
				cfgTcp.connect.localPort = port;
				cfgTcp.connect.connectToPort = port;
				cfgTcp.connect.targetName = "localhost";
				cfgTcp.connect.family = JVX_IP_V4;

				connectionId = 0;

				// Start connection
				myRemoteCall->start(&startServer, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SERVER_PRIVATE, connectionId);


				// Get ip address text and display
				mySocket->control_port(connectionId, JVX_SOCKET_SERVER_CONTROL_GET_ALL_IP_ADDRESSES, &fldStr, JVX_CONNECT_PRIVATE_ARG_TYPE_OUTPUT_STRING);
				myIpAddresses = fldStr.std_str();
				mySocket->control_port(connectionId, JVX_SOCKET_SERVER_CONTROL_GET_FIRST_IP_ADDRESS, &fldStr, JVX_CONNECT_PRIVATE_ARG_TYPE_OUTPUT_STRING);
				myFirstIpAddr = fldStr.std_str();

				jvx_socket_get_host_name_private pp;
				pp.whoisthat = myFirstIpAddr.c_str();
				mySocket->control_port(connectionId, JVX_SOCKET_SERVER_CONTROL_GET_HOSTNAME_FOR_IP_ADDRESS, &pp, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_HOSTNAME_PRIVATE);
				myName = pp.thatis.std_str();
				std::cout << "Tcp Server <" << myName << ">, ip address: " << myIpAddresses << ", port = " << cfgTcp.connect.localPort << std::endl;

				break;

			}
			else if (txt == "1")
			{
				std::cout << "Starting UDP Socket Server on port " << port << std::endl;
				cfgServer.whatToAddress = JVX_SOCKET_ADDRESS_SERVER;
				cfgServer.addressViaWhatProtocol = JVX_SOCKET_OPERATE_VIA_UDP;
				startServer.priv_start = &cfgUdp;
				startServer.priv_type = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_UDP;
				cfgUdp.localPort = port;
				cfgUdp.connectToPort  = port;
				cfgUdp.targetName = "localhost";
				cfgUdp.family = JVX_IP_V4;

				startServer.numConnectionsMax = 1;

				mySocket->initialize(NULL,  &cfgServer, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_WHAT_AND_HOW);

				myRemoteCall->initialize(NULL, mySocket, JVX_RC_OPERATE_SLAVE, NULL, &myCallbackClass);
				connectionId = 0;

				// Start connection
				myRemoteCall->start(&startServer, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SERVER_PRIVATE, connectionId);

				// Get ip address text and display
				mySocket->control_port(connectionId, JVX_SOCKET_SERVER_CONTROL_GET_ALL_IP_ADDRESSES, &fldStr, JVX_CONNECT_PRIVATE_ARG_TYPE_OUTPUT_STRING);
				myIpAddresses = fldStr.std_str();

				mySocket->control_port(connectionId, JVX_SOCKET_SERVER_CONTROL_GET_FIRST_IP_ADDRESS, &fldStr, JVX_CONNECT_PRIVATE_ARG_TYPE_OUTPUT_STRING);
				myFirstIpAddr = fldStr.std_str();

				jvx_socket_get_host_name_private pp;
				pp.whoisthat = myFirstIpAddr.c_str();
				mySocket->control_port(connectionId, JVX_SOCKET_SERVER_CONTROL_GET_HOSTNAME_FOR_IP_ADDRESS, &pp, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_HOSTNAME_PRIVATE);
				myName = pp.thatis.std_str();
				std::cout << "Udp Server <" << myName << ">, ip addresses: " << myIpAddresses << ", port = " << cfgUdp.localPort << std::endl;

				break;
			}
#ifdef JVX_WITH_PCAP
			else if (txt == "2")
			{
				std::cout << "Starting PCAP Socket Server" << std::endl;
				cfgServer.whatToAddress = JVX_SOCKET_ADDRESS_SERVER;
				cfgServer.addressViaWhatProtocol = JVX_SOCKET_OPERATE_VIA_PCAP;

				/*
				macAddress = PCAP_TARGET;
				std::cout << "Specify Target Mac Address" << std::endl;
				std::cin >> macAddress;
				startClient.target = macAddress.c_str();
				jvxErrorType resL = jvx_stringToMacAddress(startClient.target, cfgPcap.dest_mac);
				if (resL == JVX_ERROR_INVALID_ARGUMENT)
				{
					startClient.target = "40:00:00:00:00:00";
					resL = jvx_stringToMacAddress(startClient.target, cfgPcap.dest_mac);
				}
				;
				cfgPcap.timeoutConnect_msec = 1000;
				*/
				startServer.numConnectionsMax = 1;
				cfgPcap.receive_mode = 0;
				jvx_bitSet(cfgPcap.receive_mode, JVX_PCAP_RECEIVE_FILTER_INCOMING_MAC_SHIFT);
				cfgPcap.timeoutConnect_msec = 1000;
				memset(cfgPcap.dest_mac, 0, sizeof(cfgPcap.dest_mac));

				// Do some partial decoding to cut out relevant of message before transferring to application
				cfgPcap.receive_mode = 0;
				jvx_bitSet(cfgPcap.receive_mode, JVX_PCAP_RECEIVE_OUTPUT_REPORT_JVX_PACKETS_SHIFT);
				jvx_bitSet(cfgPcap.receive_mode, JVX_PCAP_RECEIVE_FILTER_INCOMING_MAC_SHIFT);

				startServer.priv_start = &cfgPcap;
				startServer.priv_type = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_PCAP;

				mySocket->initialize(NULL, &cfgServer, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_WHAT_AND_HOW);

				mySocket->availablePorts(&lstStr);
				numPorts = lstStr.ll();
				if (lstStr.ll() > 1)
				{
					for (i = 0; i < numPorts; i++)
					{
						std::cout << "Ethernet Adapter #" << i << ": " << lstStr.std_str_at(i) << std::endl;
					}
					std::cout << "Which adapter to select .." << std::endl;
					std::cin >> txt;
					connectionId = atoi(txt.c_str());
				}
				else
				{
					connectionId = 0;
				}

				if (JVX_CHECK_SIZE_SELECTED(connectionId) && (connectionId < numPorts))
				{
					std::cout << "Using ethernet adapter #" << connectionId << " for remote calls." << std::endl;
				}
				else
				{
					std::cout << "Sorry, no ethernet adapter available!" << std::endl;
					return 0;
				}

				myRemoteCall->initialize(NULL, mySocket, JVX_RC_OPERATE_SLAVE, NULL, &myCallbackClass);

				// Start connection
				myRemoteCall->start(&startServer, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SERVER_PRIVATE, connectionId);

				// Get ip address text and display
				mySocket->control_port(connectionId, JVX_SOCKET_SERVER_CONTROL_GET_MAC_ADDRESS, &fldStr, JVX_CONNECT_PRIVATE_ARG_TYPE_OUTPUT_STRING);
				myMacAddr = fldStr.std_str();
				std::cout << "Server specification: " << myMacAddr << std::endl;

				break;
			}
#endif
#ifdef JVX_RS232_ACTIVE
			else if (txt == "3")
			{

				myRs232->initialize(NULL, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
				myRs232->availablePorts(&lstStr);
				numPorts = lstStr.ll();
				if (lstStr.ll() > 1)
				{
					for (i = 0; i < numPorts; i++)
					{
						std::cout << "COM Port #" << i << ": " << lstStr.std_str_at(i) << std::endl;
					}
					std::cout << "Which port to select" << std::endl;
					std::cin >> txt;
					connectionId = atoi(txt.c_str());
				}
				else
				{
					connectionId = 0;
				}
				portAddress = lstStr.std_str_at(connectionId);
				

				if (JVX_CHECK_SIZE_SELECTED(connectionId) && (connectionId < numPorts))
				{
					std::cout << "Using COM port #" << connectionId << " for remote calls." << std::endl;
				}
				else
				{
					std::cout << "Sorry, no COM port available!" << std::endl;
					return 0;
				}

				myRemoteCall->initialize(NULL, myRs232, JVX_RC_OPERATE_SLAVE, NULL, &myCallbackClass);

				// Start connection
				myRemoteCall->start(&cfgRs232, JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_CONFIG, connectionId);

				// Get ip address text and display
				//std::cout << "Server mac address: " << myMacAddr << std::endl;
				break;
			}
#endif
			else
			{
				std::cout << "Invalid user specification!" << std::endl;
			}
		}

		std::cout << "Quit <q>?" << std::endl;
		std::cin >> var;

		myRemoteCall->stop();
		mySocket->terminate();

		if (var == "q")
		{
			break;
		}
		else
		{
			std::cout << "Restarting.." << std::endl;
		}
	}
}
	
