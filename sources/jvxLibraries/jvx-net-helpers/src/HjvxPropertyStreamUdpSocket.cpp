#include "jvx-net-helpers.h"
#include "jvx_connect_client_st.h"

HjvxPropertyStreamUdpSocket::HjvxPropertyStreamUdpSocket()
{
	theSocket = NULL;
	obj = NULL;
	port = 0;
	tools = NULL;
	host = NULL;
	buffer_receive_sz = 0;
	buffer_receive = NULL;
	theState = JVX_STATE_INIT;
}

jvxErrorType
HjvxPropertyStreamUdpSocket::initialize(HjvxPropertyStreamUdpSocket_report* reportRef, IjvxToolsHost* toolsRef, IjvxHiddenInterface* theHostRef, jvxBool client)
{
	if (theState == JVX_STATE_INIT)
	{
		if (client)
		{
			oper_tp = JVX_PROP_UDP_CLIENT;
		}
		else
		{ 
			oper_tp = JVX_PROP_UDP_SERVER;
		}
		tools = toolsRef;
		host = theHostRef;
		theState = JVX_STATE_ACTIVE;
		report = reportRef;
		return JVX_NO_ERROR;

	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
HjvxPropertyStreamUdpSocket::terminate()
{
	if (theState == JVX_STATE_ACTIVE)
	{
		assert(prop_ids.size() == 0);
		tools = NULL;
		host = NULL;
		theState = JVX_STATE_INIT;
		return JVX_NO_ERROR;

	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
HjvxPropertyStreamUdpSocket::register_property(jvxSize uId, jvxInt32 portArg, const std::string& targetArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theState != JVX_STATE_ACTIVE)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	auto elm = prop_ids.begin();
	while (elm != prop_ids.end())
	{
		if (uId == *elm)
		{
			res = JVX_ERROR_ALREADY_IN_USE;
			break;
		}
		elm++;
	}

	if(res == JVX_NO_ERROR)
	{
	    std::cout << __FUNCTION__ << ": Registering property for UDP socket on port <" << portArg << "> with uId <" << uId << ">." << std::endl;
		assert(elm == prop_ids.end());
		if (prop_ids.size() == 0)
		{
			target = targetArg;
			port = portArg;

			if (oper_tp == JVX_PROP_UDP_CLIENT)
			{
				// On first entry, start socket
				res = open_socket(true);
			}
			else
			{
				// On first entry, start socket
				res = open_socket(false);
			}

		}
		else
		{
			assert(target == targetArg);
			assert(port == portArg);
		}

		if (res == JVX_NO_ERROR)
		{
			prop_ids.push_back(uId);
		}
	}
	return res;
}

jvxErrorType 
HjvxPropertyStreamUdpSocket::unregister_property(jvxSize uId, jvxInt32& port_remove)
{
	if (theState != JVX_STATE_ACTIVE)
	{
		return JVX_ERROR_WRONG_STATE;
	}
	auto elm = prop_ids.begin();
	while (elm != prop_ids.end())
	{
		if (uId == *elm)
		{
			break;
		}
		elm++;
	}
	if (elm == prop_ids.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	std::cout << __FUNCTION__ << ":Unregistering property for UDP socket for uId <" << uId << ">" << std::endl;

	prop_ids.erase(elm);
	if (prop_ids.size() == 0)
	{
		if (oper_tp == JVX_PROP_UDP_CLIENT)
		{
			close_socket(true);
		}
		else
		{
			close_socket(false);
		}
		port_remove = port;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
HjvxPropertyStreamUdpSocket::open_socket(jvxBool startasclient, jvxSize buffer_receive_sz_ref, jvxSize timeoutConnectRef)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSocketAddressType str;
	jvx_socket_client_private cfgC;
	jvx_socket_server_private cfgS;
	jvx_connect_spec_udp cfgUdp;

	str.addressViaWhatProtocol = JVX_SOCKET_OPERATE_VIA_UDP;
	if (startasclient)
	{
		str.whatToAddress = JVX_SOCKET_ADDRESS_CLIENT;
	}
	else
	{
		str.whatToAddress = JVX_SOCKET_ADDRESS_SERVER;
	}


	cfgC.priv_start = &cfgUdp;
	cfgC.priv_type = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_UDP;
	cfgC.timeOutConnect = timeoutConnectRef;

	cfgS.numConnectionsMax = 1;
	cfgS.priv_start = &cfgUdp;
	cfgS.priv_type = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_UDP;

	res = tools->instance_tool(JVX_COMPONENT_CONNECTION, &obj, 0, "jvxTSocket");
	if ((res == JVX_NO_ERROR) && obj)
	{
		obj->request_specialization(
			reinterpret_cast<jvxHandle**>(&theSocket),
			NULL, NULL);
	
		res = theSocket->initialize(host, &str, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_WHAT_AND_HOW);

		if (res == JVX_NO_ERROR)
		{
			allocate_buffer(buffer_receive_sz_ref);
			if (startasclient)
			{
				cfgUdp.targetName = target.c_str();
				cfgUdp.localPort = -1;
				cfgUdp.connectToPort = port;
				cfgUdp.family = JVX_IP_V4;

				res = theSocket->start_port(0, &cfgC,
					JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CLIENT_PRIVATE,
					static_cast<IjvxConnection_report*>(this));
			}
			else
			{
				cfgUdp.targetName = target.c_str();
				cfgUdp.localPort = port;
				cfgUdp.connectToPort = -1;
				cfgUdp.family = JVX_IP_V4;

				res = theSocket->start_port(0, &cfgS,
					JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SERVER_PRIVATE,
					static_cast<IjvxConnection_report*>(this));
			}
			if (res == JVX_NO_ERROR)
			{
				if (startasclient)
				{
					std::cout << "Successfully started UDP port to connect to <" << target << ":" << port << ">." << std::endl;
				}
				else
				{
					std::cout << "Successfully started UDP port to wait for incoming messages." << std::endl;
				}
			}
			else
			{
				if (startasclient)
				{
					std::cout << "Failed to start UDP port to connect to <" << target << ":" << port << ">." << std::endl;
				}
				else
				{
					std::cout << "Failed to start UDP port to wait for incoming messages." << std::endl;

				}
				JVX_DSP_SAFE_DELETE_FIELD(buffer_receive);
				buffer_receive_sz = 0;
				theSocket->terminate();
				tools->return_instance_tool(JVX_COMPONENT_CONNECTION, obj, 0, "jvxTSocket");
				theSocket = NULL;
				obj = NULL;
			}
		}
		else
		{
			tools->return_instance_tool(JVX_COMPONENT_CONNECTION, obj, 0, "jvxTSocket");
			theSocket = NULL;
			obj = NULL;
		}
	}
	return res;
}

jvxErrorType 
HjvxPropertyStreamUdpSocket::close_socket(jvxBool stopasclient)
{
	if (theState != JVX_STATE_ACTIVE)
	{
		return JVX_ERROR_WRONG_STATE;
	}
	if (theSocket)
	{
		theSocket->stop_port(0);

		deallocate_buffer();

		theSocket->terminate();
	}
	if (obj)
	{
		tools->return_instance_tool(JVX_COMPONENT_CONNECTION, obj, 0, "jvxTSocket");
	}
	obj = NULL;
	theSocket = NULL;

	if (stopasclient)
	{
		std::cout << "Successfully stopped UDP port connected to <" << target << ":" << port << ">." << std::endl;
	}
	else
	{
		std::cout << "Successfully stopped UDP port to wait for messages." << std::endl;
	}

	return JVX_NO_ERROR;
}

jvxErrorType
HjvxPropertyStreamUdpSocket::send_packet(const char* packet, jvxSize szPacket, const char* target, int port)
{
	if (theState != JVX_STATE_ACTIVE)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	jvxErrorType res = JVX_ERROR_NOT_READY;
	jvxSize szPacketLoc = 0;
	if (theSocket)
	{
		szPacketLoc = szPacket;
#if 0
		std::cout << __FUNCTION__ << ": Sending out <" << szPacketLoc << "> bytes via UDP." << std::endl;
#endif

		theSocket->sendMessage_port(0, (jvxByte*)packet, &szPacketLoc, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
		if (szPacket != szPacketLoc)
		{
			res = JVX_ERROR_ABORT;
		}
		else
		{
			res = JVX_NO_ERROR;
		}
	}
	return res;
}

void
HjvxPropertyStreamUdpSocket::deallocate_buffer()
{
	JVX_DSP_SAFE_DELETE_FIELD(buffer_receive);
	buffer_receive_sz = 0;
}

void
HjvxPropertyStreamUdpSocket::allocate_buffer(jvxSize sz)
{
	buffer_receive_sz = sz;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(buffer_receive, jvxByte, buffer_receive_sz);
}

// =============================================================================================================

jvxErrorType
HjvxPropertyStreamUdpSocket::provide_data_and_length(jvxByte**ptr, jvxSize* maxNumCopy, jvxSize* offset, jvxSize id_port, 
	jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	// Auto resize buffers
	if (*maxNumCopy > buffer_receive_sz)
	{
		if (buffer_receive)
		{
			deallocate_buffer();
		}

		allocate_buffer(*maxNumCopy);
	}
	*ptr = buffer_receive;
	*maxNumCopy = buffer_receive_sz;
	return JVX_NO_ERROR;
}

jvxErrorType
HjvxPropertyStreamUdpSocket::report_data_and_read(jvxByte* ptr, jvxSize numRead, jvxSize offset, jvxSize id_port, jvxHandle* addInfo,
	jvxConnectionPrivateTypeEnum whatsthis)
{
	// Report buffer further on
	if (report)
	{
		report->report_incoming_packet(ptr, numRead, this);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
HjvxPropertyStreamUdpSocket::report_event(jvxBitField eventMask, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	if (jvx_bitTest(eventMask, JVX_CONNECTION_REPORT_ERROR_SHFT))
	{
		if (whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_ERROR)
		{
			jvx_socket_error_private* myError = (jvx_socket_error_private*)addInfo;
			if (report)
			{
				report->report_connection_error(myError->errDescr);
			}
		}
	}
	return JVX_NO_ERROR;
}

