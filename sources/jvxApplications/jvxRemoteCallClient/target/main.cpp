#include <iostream>
#include "jvx.h"
#include "jvxTSocket.h"
#include "jvxTRemoteCall.h"
#include "jvx_connect_server_st.h"

#ifdef JVX_RS232_ACTIVE
#ifdef JVX_OS_WINDOWS
	#include "jvxTRs232Win32.h"
#endif
#ifdef JVX_OS_LINUX
	#include "jvxTRs232Glnx.h"
#endif
#endif

#define TCP_UDP_TARGET "192.168.178.121"
#define PCAP_TARGET "40:00:00:00:00:00"

volatile jvxBool doSendCalls = false;

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
		doSendCalls = true;
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
		if(immediateResponse)
		{
			*immediateResponse = c_true;
		}
		std::cout << __FUNCTION__ << std::endl;
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reportActionRequest(const char* description, jvxSize numinparams, jvxSize numoutparams, jvxSize processId)
	{
		std::cout << __FUNCTION__ << std::endl;
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reportLocalError(const char* description, jvxSize localErrorId)
	{
		std::cout << __FUNCTION__ << ": " << description << std::endl;
		return JVX_NO_ERROR;
	};

};

remoteCallEndpoint myCallbackClass;

int main(int argc, char* argv[])
{
	jvxSize i, j, k, l;
	int numIterations = 10;
	jvxSize connectionId = JVX_SIZE_UNSELECTED;

	std::string var;
	std::string ipAddress;
	std::string macAddress;
	std::string portAddress;
	jvxErrorType res = JVX_NO_ERROR;
	int port = 12345;

	jvxSocketAddressType cfgClient;
	jvx_socket_client_private startClient;

	jvx_connect_spec_udp cfgUdp;
	jvx_connect_spec_tcp cfgTcp;
	jvx_connect_spec_pcap cfgPcap;

	IjvxObject* myRemoteCallo = NULL;
	IjvxRemoteCall* myRemoteCall = NULL;

	IjvxObject* mySocketo = NULL;
	IjvxConnection* mySocket = NULL;

	int cnt = 1;
	jvxErrorType resRemote = JVX_NO_ERROR;

	jvxHandle* ptrPrivStart = NULL;
	std::string txt;
	jvxSize numPorts = 0;
	jvxApiStringList lstStr;

	jvxConnectionPrivateTypeEnum whatsthis = JVX_CONNECT_PRIVATE_ARG_TYPE_NONE;

#ifdef JVX_RS232_ACTIVE
	jvxRs232Config cfgRs232;
	IjvxObject* myRs232o = NULL;
	IjvxConnection* myRs232 = NULL;
#endif

#ifdef JVX_RS232_ACTIVE
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
			cfgClient.whatToAddress = JVX_SOCKET_ADDRESS_CLIENT;
			cfgClient.addressViaWhatProtocol = JVX_SOCKET_OPERATE_VIA_TCP;
			ipAddress = TCP_UDP_TARGET;

			std::cout << "Specify TCP Target" << std::endl;
			std::cin >> ipAddress;

			startClient.priv_start = &cfgTcp;
			startClient.priv_type = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_TCP;
			startClient.timeOutConnect = 10000;
			cfgTcp.disableNagleAlgorithmTcp = true;
			cfgTcp.connect.localPort = -1; // Let OS choose the port
			cfgTcp.connect.connectToPort = port;
			cfgTcp.connect.targetName = ipAddress.c_str();
			cfgTcp.connect.family = JVX_IP_V4;


			mySocket->initialize(NULL, &cfgClient, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_WHAT_AND_HOW);
		
			ptrPrivStart = &startClient;
			whatsthis = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CLIENT_PRIVATE;
			myRemoteCall->initialize(NULL, mySocket, JVX_RC_OPERATE_MASTER, NULL, &myCallbackClass);
			connectionId = 0;
			break;

		}
		else if (txt == "1")
		{
			std::cout << "Starting UDP Socket Server on port " << port << std::endl;
			cfgClient.whatToAddress = JVX_SOCKET_ADDRESS_CLIENT;
			cfgClient.addressViaWhatProtocol = JVX_SOCKET_OPERATE_VIA_UDP;

			std::cout << "Specify TCP Target" << std::endl;
			std::cin >> ipAddress;

			startClient.timeOutConnect = 10000;
			startClient.priv_start = &cfgUdp;
			startClient.priv_type = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_UDP;
			cfgUdp.localPort = port;
			cfgUdp.connectToPort = port;
			cfgUdp.targetName = ipAddress;
			cfgUdp.family = JVX_IP_V4;

			mySocket->initialize(NULL, &cfgClient, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_WHAT_AND_HOW);

			ptrPrivStart = &startClient;
			whatsthis = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CLIENT_PRIVATE;
			myRemoteCall->initialize(NULL,  mySocket, JVX_RC_OPERATE_MASTER, NULL, &myCallbackClass);
			connectionId = 0;
			break;
		}
#ifdef JVX_WITH_PCAP
		else if (txt == "2")
		{
			std::cout << "Starting PCAP Socket Server on port " << port << std::endl;
			cfgClient.whatToAddress = JVX_SOCKET_ADDRESS_CLIENT;
			cfgClient.addressViaWhatProtocol = JVX_SOCKET_OPERATE_VIA_PCAP;
			macAddress = PCAP_TARGET;

			mySocket->initialize(NULL,  &cfgClient, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_WHAT_AND_HOW);

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

			if (JVX_CHECK_SIZE_SELECTED(connectionId) && (connectionId  < numPorts))
			{
				std::cout << "Using ethernet adapter #" << connectionId << " for remote calls." << std::endl;
			}
			else
			{
				std::cout << "Sorry, no ethernet adapter available!" << std::endl;
				return 0;
			}

			std::cout << "Specify Target Mac Address" << std::endl;
			std::cin >> macAddress;
			jvxErrorType resL = jvx_stringToMacAddress(macAddress, cfgPcap.dest_mac);
			if (resL == JVX_ERROR_INVALID_ARGUMENT)
			{
				macAddress = "40:00:00:00:00:00";
				resL = jvx_stringToMacAddress(macAddress, cfgPcap.dest_mac);
			}
			;
			cfgPcap.timeoutConnect_msec = 1000;
			
			// Do some partial decoding to cut out relevant of message before transferring to application
			cfgPcap.receive_mode = 0;
			jvx_bitSet(cfgPcap.receive_mode, JVX_PCAP_RECEIVE_OUTPUT_REPORT_JVX_PACKETS_SHIFT);
			jvx_bitSet(cfgPcap.receive_mode, JVX_PCAP_RECEIVE_FILTER_INCOMING_MAC_SHIFT);
			//jvx_bitSet(cfgPcap.receive_mode, JVX_PCAP_RECEIVE_FILTER_NO_SELF_CAPTURE_SHIFT);
			//jvx_bitSet(cfgPcap.receive_mode, JVX_PCAP_RECEIVE_FILTER_INCOMING_ONLY_SHIFT);

			startClient.timeOutConnect = 10000;
			startClient.priv_start = &cfgPcap;
			startClient.priv_type = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_PCAP;

			ptrPrivStart = &startClient;
			whatsthis = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CLIENT_PRIVATE;

			myRemoteCall->initialize(NULL, mySocket, JVX_RC_OPERATE_MASTER, NULL, &myCallbackClass);
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

			if (JVX_CHECK_SIZE_SELECTED(connectionId) && (connectionId  < numPorts))
			{
				std::cout << "Using COM port #" << connectionId << " for remote calls." << std::endl;
			}
			else
			{
				std::cout << "Sorry, no COM port available!" << std::endl;
				return 0;
			}

			ptrPrivStart = &cfgRs232;
			whatsthis = JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_CONFIG;

			myRemoteCall->initialize(NULL, myRs232, JVX_RC_OPERATE_MASTER, NULL, &myCallbackClass);

			break;
		}
#endif
		else
		{
			std::cout << "Invalid user specification!" << std::endl;
		}
	}

	// =======================================================================
	// Connect and start
	// =======================================================================
	assert(JVX_CHECK_SIZE_SELECTED(connectionId));

	// Start connection
	myRemoteCall->start(ptrPrivStart, whatsthis, connectionId);

	while(1)
	{
		if(doSendCalls)
		{
			jvxRCOneParameter* paramsIn = NULL;
			jvxRCOneParameter* paramsOut = NULL;
			for(l = 0; l < numIterations; l++)
			{
				std::cout << "Trigger remote call #" << l << std::endl;

				myRemoteCall->allocateParameterList(&paramsIn, 1);
				myRemoteCall->allocateParameterList(&paramsOut, 1);

				paramsIn[0].description.format = JVX_DATAFORMAT_DATA;
				paramsIn[0].description.float_precision_id = JVX_FLOAT_DATAFORMAT_ID;
				paramsIn[0].description.seg_length_y = 1;
				paramsIn[0].description.seg_length_x = 128;
				paramsIn[0].content = NULL;
				JVX_SAFE_NEW_FLD_CVRT(paramsIn[0].content, jvxData*, paramsIn[0].description.seg_length_y, jvxHandle**);
				for(i = 0; i < paramsIn[0].description.seg_length_y; i++)
				{
					JVX_SAFE_NEW_FLD_CVRT(paramsIn[0].content[i], jvxData, paramsIn[0].description.seg_length_x, jvxHandle*);
					memset(paramsIn[0].content[i], 0, sizeof(jvxData) * paramsIn[0].description.seg_length_x);
					jvxData* ptr = (jvxData*)(paramsIn[0].content[i]);
					for (j = 0; j < paramsIn[0].description.seg_length_x; j++)
					{
						ptr[j] = (jvxData)j;
					}
				}

				paramsOut[0].description.format = JVX_DATAFORMAT_DATA;
				paramsOut[0].description.float_precision_id = JVX_FLOAT_DATAFORMAT_ID;
				paramsOut[0].description.seg_length_y = 1;
				paramsOut[0].description.seg_length_x = 128;
				paramsOut[0].content = NULL;
				JVX_SAFE_NEW_FLD_CVRT(paramsOut[0].content, jvxData*, paramsOut[0].description.seg_length_y, jvxHandle**);
				for(i = 0; i < paramsOut[0].description.seg_length_y; i++)
				{
					JVX_SAFE_NEW_FLD_CVRT(paramsOut[0].content[i], jvxData, paramsOut[0].description.seg_length_x, jvxHandle*);
					memset(paramsOut[0].content[i], 0, sizeof(jvxData) * paramsOut[0].description.seg_length_x);
				}

				res = myRemoteCall->remoteRequest(paramsIn, 1, paramsOut, 1, "loop_back", cnt, 1000000, &resRemote);
				if(res == JVX_NO_ERROR)
				{
					std::cout << "Successful remote call: " << std::endl;
					for(i = 0; i < paramsOut[0].description.seg_length_y; i++)
					{
						jvxData* ptr = (jvxData*)paramsOut[0].content[i];
						for(k = 0; k < paramsOut[0].description.seg_length_x; k++)
						{
							if(k != 0)
								std::cout << ", " << std::flush;
							std::cout << jvx_data2String(ptr[k],1) << std::flush;
						}
					}
					std::cout << std::endl;
				}
				else
				{
					std::cout << "Remote call ended with error: " << jvxErrorType_descr(res) << std::endl;
				}
			}
			break;
		}

		else
		{
			JVX_SLEEP_S(1);
		}
	}
	myRemoteCall->stop();
	mySocket->terminate();
}
	
