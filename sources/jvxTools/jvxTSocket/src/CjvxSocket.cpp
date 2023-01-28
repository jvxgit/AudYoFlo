#include "jvx.h"
#include "CjvxSocket.h"
#include <iostream>

#define JVX_SOCKET_PORT_NAME 1024

CjvxSocket::CjvxSocket(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_CONNECTION);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxConnection*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theComponentSubTypeDescriptor = "/";
	_common_set.theComponentSubTypeDescriptor += JVX_CONNECTION_TAG_SOCKET;
	_common_set_min.theDescription = "JVX Socket connection module";
	runtime.mySocketType = JVX_CONNECT_SOCKET_TYPE_NONE;

	runtime.server.fam = NULL;
	runtime.server.ifaces = NULL;
	runtime.server.numifaces = 0;

	runtime.client.fam = NULL;
	runtime.client.ifaces = NULL;
	runtime.client.numifaces = 0;

	runtime.client.callbacks.common.callback_report_startup_complete = sccallback_report_startup_complete;
	runtime.client.callbacks.common.callback_provide_data_and_length = sccallback_provide_data_and_length;
	runtime.client.callbacks.common.callback_report_data_and_read = sccallback_report_data_and_read;
	runtime.client.callbacks.common.callback_report_error = sccallback_report_error;
	runtime.client.callbacks.common.callback_report_shutdown_complete = sccallback_report_shutdow_complete;;

	runtime.client.callbacks.callback_report_unsuccesful_outgoing = ccallback_report_unsuccesful_outgoing;
	runtime.client.callbacks.callback_report_connect_outgoing = ccallback_report_connect_outgoing;
	runtime.client.callbacks.callback_report_disconnect_outgoing = ccallback_report_disconnect_outgoing;
	
	runtime.server.callbacks.common.callback_report_startup_complete = sccallback_report_startup_complete;
	runtime.server.callbacks.common.callback_report_shutdown_complete = sccallback_report_shutdow_complete;
	runtime.server.callbacks.common.callback_provide_data_and_length = sccallback_provide_data_and_length;
	runtime.server.callbacks.common.callback_report_data_and_read = sccallback_report_data_and_read;
	runtime.server.callbacks.common.callback_report_error = sccallback_report_error;

	runtime.server.callbacks.callback_report_denied_incoming = scallback_report_denied_incoming;
	runtime.server.callbacks.callback_report_connect_incoming = scallback_report_connect_incoming;
	runtime.server.callbacks.callback_report_disconnect_incoming = scallback_report_disconnect_incoming;
}

CjvxSocket::~CjvxSocket()
{
	this->terminate();
}

jvxErrorType 
CjvxSocket::initialize(IjvxHiddenInterface* hostRef, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)
{	
	jvxErrorType res = CjvxObject::_initialize();
	jvxSize numPorts = 0;
	if(res == JVX_NO_ERROR)
	{
		runtime.mySocketType = JVX_CONNECT_SOCKET_TYPE_NONE;

		// Default config:

		config.addressViaWhatProtocol = JVX_SOCKET_OPERATE_VIA_TCP;
		config.whatToAddress = JVX_SOCKET_ADDRESS_CLIENT;

		if (whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_WHAT_AND_HOW)
		{
			jvxSocketAddressType * myPrivateField = (jvxSocketAddressType*)priv;
			if (myPrivateField)
			{
				config.addressViaWhatProtocol = myPrivateField->addressViaWhatProtocol;
				config.whatToAddress = myPrivateField->whatToAddress;
			}
		}
		switch (config.whatToAddress)
		{
		case JVX_SOCKET_ADDRESS_SERVER:
			_common_set.theComponentSubTypeDescriptor = jvx_makePathExpr(_common_set.theComponentSubTypeDescriptor, "SERVER");
			break;
		case JVX_SOCKET_ADDRESS_CLIENT:
			_common_set.theComponentSubTypeDescriptor = jvx_makePathExpr(_common_set.theComponentSubTypeDescriptor, "CLIENT");
			break;
		}

		// Start socket
		switch (config.addressViaWhatProtocol)
		{
		case JVX_SOCKET_OPERATE_VIA_TCP:
			_common_set.theComponentSubTypeDescriptor = jvx_makePathExpr(_common_set.theComponentSubTypeDescriptor, "TCP");
			runtime.mySocketType = JVX_CONNECT_SOCKET_TYPE_TCP;
			break;

		case JVX_SOCKET_OPERATE_VIA_UDP:
			_common_set.theComponentSubTypeDescriptor = jvx_makePathExpr(_common_set.theComponentSubTypeDescriptor, "UDP");
			runtime.mySocketType = JVX_CONNECT_SOCKET_TYPE_UDP;
			break;

#ifdef JVX_WITH_PCAP
		case JVX_SOCKET_OPERATE_VIA_PCAP:
			_common_set.theComponentSubTypeDescriptor = jvx_makePathExpr(_common_set.theComponentSubTypeDescriptor, "PCAP");
			runtime.mySocketType = JVX_CONNECT_SOCKET_TYPE_PCAP;
			break;
#endif
		default:
			res = JVX_ERROR_INVALID_SETTING;
			break;
		}

		if (res == JVX_NO_ERROR)
		{
			switch (config.whatToAddress)
			{
			case JVX_SOCKET_ADDRESS_SERVER:
				res = jvx_connect_server_family_initialize_st(&runtime.server.fam, runtime.mySocketType);
				assert(res == JVX_NO_ERROR);
				res = jvx_connect_server_family_number_interfaces_st(runtime.server.fam, &runtime.server.numifaces);
				assert(res == JVX_NO_ERROR);
				runtime.server.ifaces = NULL;
				if (runtime.server.numifaces)
				{
					JVX_SAFE_NEW_FLD(runtime.server.ifaces, oneServerIf, runtime.server.numifaces);
					memset(runtime.server.ifaces, 0, sizeof(oneServerIf) *  runtime.server.numifaces);
				}
				break;
			case JVX_SOCKET_ADDRESS_CLIENT:
				res = jvx_connect_client_family_initialize_st(&runtime.client.fam, runtime.mySocketType);
				assert(res == JVX_NO_ERROR);
				res = jvx_connect_client_family_number_interfaces_st(runtime.client.fam, &runtime.client.numifaces);
				assert(res == JVX_NO_ERROR);
				runtime.client.ifaces = NULL;
				if (runtime.client.numifaces)
				{
					JVX_SAFE_NEW_FLD(runtime.client.ifaces, oneClientIf, runtime.client.numifaces);
					memset(runtime.client.ifaces, 0, sizeof(oneClientIf) *  runtime.client.numifaces);
				}
				break;

			default:
				res = JVX_ERROR_INVALID_SETTING;
				break;
			}
		}

		return res;

	}
	return res;
}

jvxErrorType 
CjvxSocket::availablePorts(jvxApiStringList* allPorts)
{
	std::vector<std::string> allPortsL;
	jvxSize i;
	char fldStr[JVX_SOCKET_PORT_NAME];
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
		switch (config.whatToAddress)
		{
		case JVX_SOCKET_ADDRESS_SERVER:
			for (i = 0; i < runtime.server.numifaces; i++)
			{
				memset(fldStr, 0, JVX_SOCKET_PORT_NAME);
				res = jvx_connect_server_family_name_interfaces_st(runtime.server.fam, fldStr, JVX_SOCKET_PORT_NAME, i);
				allPortsL.push_back(fldStr);
			}
			break;
		case JVX_SOCKET_ADDRESS_CLIENT:
			for (i = 0; i < runtime.client.numifaces; i++)
			{
				memset(fldStr, 0, JVX_SOCKET_PORT_NAME);
				res = jvx_connect_client_family_name_interfaces_st(runtime.client.fam, fldStr, JVX_SOCKET_PORT_NAME, i);
				allPortsL.push_back(fldStr);
			}
			break;
		default:
			assert(0);
		}
		if (allPorts)
		{
			allPorts->assign(allPortsL);
		}
		return(res);
	}
	return JVX_ERROR_WRONG_STATE;
}

// =============================================================
// Port specific commands
// =============================================================

jvxErrorType 
CjvxSocket::state_port(jvxSize idPort, jvxState* theState)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
		switch (config.whatToAddress)
		{
		case JVX_SOCKET_ADDRESS_SERVER:
			if (idPort < runtime.server.numifaces)
			{
				if (runtime.server.ifaces[idPort].theIf)
				{
					if (theState)
					{
						*theState = JVX_STATE_ACTIVE;
					}
				}
				else
				{
					if (theState)
					{
						*theState = JVX_STATE_INIT;
					}
				}
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_ID_OUT_OF_BOUNDS;

		case JVX_SOCKET_ADDRESS_CLIENT:
			if (idPort < runtime.client.numifaces)
			{
				if (runtime.client.ifaces[idPort].theIf)
				{
					if (theState)
					{
						*theState = JVX_STATE_ACTIVE;
					}
				}
				else
				{
					if (theState)
					{
						*theState = JVX_STATE_INIT;
					}
				}
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_ID_OUT_OF_BOUNDS;

		default:
			assert(0);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxSocket::start_port(jvxSize idPort, jvxHandle* cfgIn, jvxConnectionPrivateTypeEnum whatsthis, IjvxConnection_report* theReportIn)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_socket_server_private* cfgS = NULL;
	jvx_socket_client_private* cfgC = NULL;

	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
		switch (config.whatToAddress)
		{
		case JVX_SOCKET_ADDRESS_SERVER:
			cfgS = (jvx_socket_server_private*)cfgIn;
			assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SERVER_PRIVATE);
			if (idPort < runtime.server.numifaces)
			{
				if (runtime.server.ifaces[idPort].theIf == NULL)
				{
					runtime.server.ifaces[idPort].theReport = theReportIn;
					res = jvx_connect_server_initialize_st(runtime.server.fam,
						&runtime.server.ifaces[idPort].theIf,
						&runtime.server.callbacks,
						reinterpret_cast<jvxHandle*>(this),						
						JVX_SIZE_INT32(cfgS->numConnectionsMax),
						idPort);
					assert(res == JVX_NO_ERROR);

					res = jvx_connect_server_start_listening_st(runtime.server.ifaces[idPort].theIf, cfgS->priv_start,cfgS->priv_type);
					assert(res == JVX_NO_ERROR);

					return JVX_NO_ERROR;
				}
				return JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
			break;
		case JVX_SOCKET_ADDRESS_CLIENT:
			cfgC = (jvx_socket_client_private*)cfgIn;
			assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CLIENT_PRIVATE);
			if (idPort < runtime.client.numifaces)
			{
				if (runtime.client.ifaces[idPort].theIf == NULL)
				{
					runtime.client.ifaces[idPort].theReport = theReportIn;
					res = jvx_connect_client_interface_initialize_st(runtime.client.fam,
						&runtime.client.ifaces[idPort].theIf,
						&runtime.client.callbacks,
						reinterpret_cast<jvxHandle*>(this),
						JVX_SIZE_INT(cfgC->timeOutConnect),
						idPort);
					assert(res == JVX_NO_ERROR);

					res = jvx_connect_client_interface_connect_st(runtime.client.ifaces[idPort].theIf, cfgC->priv_start, cfgC->priv_type);
					assert(res == JVX_NO_ERROR);

					return JVX_NO_ERROR;
				}
				return JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
			break;

		default:
			assert(0);
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxSocket::get_constraints_buffer(jvxSize idPort, jvxSize* bytesPrepend, jvxSize* fldMinSize, jvxSize* fldMaxSize)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
		switch (config.whatToAddress)
		{
		case JVX_SOCKET_ADDRESS_SERVER:
			if (idPort < runtime.server.numifaces)
			{
				if (runtime.server.ifaces[idPort].theIf)
				{
					res = jvx_connect_server_field_constraint_st(runtime.server.ifaces[idPort].theIf, bytesPrepend, fldMinSize, fldMaxSize);
					assert(res == JVX_NO_ERROR);
					return res;
				}
				return JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
			break;
		case JVX_SOCKET_ADDRESS_CLIENT:
			if (idPort < runtime.client.numifaces)
			{
				if (runtime.client.ifaces[idPort].theIf)
				{
					res = jvx_connect_client_field_constraint_st(runtime.client.ifaces[idPort].theIf, bytesPrepend, fldMinSize, fldMaxSize);
					assert(res == JVX_NO_ERROR);
					return res;
				}
				return JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
			break;

		default:
			assert(0);
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxSocket::sendMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld, jvxHandle* cfgIn, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_socket_send_private* cfgP = (jvx_socket_send_private*)cfgIn;

	jvx_socket_udp_private* myUdpTarget = NULL; 
	jvx_udp_target locTarget;

	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
		switch (config.whatToAddress)
		{
		case JVX_SOCKET_ADDRESS_SERVER:
			if (idPort < runtime.server.numifaces)
			{
				if (runtime.server.ifaces[idPort].theIf)
				{
					switch(config.addressViaWhatProtocol)
					{
					case JVX_SOCKET_OPERATE_VIA_UDP:
						
						switch(whatsthis)
						{
						case JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SEND_PRIVATE:
							assert(cfgP);
							myUdpTarget = (jvx_socket_udp_private*)cfgP->target; 
							res = jvx_connect_server_prepare_udp_target_st(runtime.server.ifaces[idPort].theIf, myUdpTarget->name, myUdpTarget->port, &locTarget, myUdpTarget->family);
							assert(res == JVX_NO_ERROR);
							res = jvx_connect_server_send_st(runtime.server.ifaces[idPort].theIf, JVX_SIZE_INT(cfgP->idConnection), fld, szFld, &locTarget, 
								JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_UDP_TARGET);
							break;

						case JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SEND_PRIVATE_UDP:
							assert(cfgP);
							res = jvx_connect_server_send_st(runtime.server.ifaces[idPort].theIf, 0, fld, szFld, cfgP, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_UDP_TARGET);
							break;

						default:
							res = jvx_connect_server_send_st(runtime.server.ifaces[idPort].theIf, 0, fld, szFld, cfgIn, whatsthis);
						}
						break;
					case JVX_SOCKET_OPERATE_VIA_PCAP:
						res = jvx_connect_server_send_st(runtime.server.ifaces[idPort].theIf, 0, fld, szFld, cfgIn, whatsthis);
						break;

					case JVX_SOCKET_OPERATE_VIA_TCP:
						if (cfgP)
						{
							assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SEND_PRIVATE);
							res = jvx_connect_server_send_st(runtime.server.ifaces[idPort].theIf, JVX_SIZE_INT(cfgP->idConnection), fld, szFld, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
						}
						else
						{
							res = JVX_ERROR_INVALID_ARGUMENT;
						}
						break;
					default:
						assert(0);
						break;
					}
					return res;
				}
				return JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
			break;
		case JVX_SOCKET_ADDRESS_CLIENT:
			if (idPort < runtime.client.numifaces)
			{
				if (runtime.client.ifaces[idPort].theIf)
				{
					switch(config.addressViaWhatProtocol)
					{
					case JVX_SOCKET_OPERATE_VIA_UDP:
						
						switch(whatsthis)
						{
						case JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SEND_PRIVATE:
							assert(cfgP);
							//if(cfgP->target)
							myUdpTarget = (jvx_socket_udp_private*)cfgP->target;
							res = jvx_connect_client_prepare_udp_target_st(runtime.client.ifaces[idPort].theIf, myUdpTarget->name, myUdpTarget->port, &locTarget, myUdpTarget->family);
							assert(res == JVX_NO_ERROR);
							res = jvx_connect_client_send_st(runtime.client.ifaces[idPort].theIf, fld, szFld, &locTarget, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_UDP_TARGET);
							break;
							
						case JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SEND_PRIVATE_UDP:
							assert(cfgP);
							res = jvx_connect_client_send_st(runtime.client.ifaces[idPort].theIf, fld, szFld, cfgP, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_UDP_TARGET);
							break;
						
						default:

							res = jvx_connect_client_send_st(runtime.client.ifaces[idPort].theIf, fld, szFld, cfgIn, whatsthis);
							break;							
						}
						break;

					case JVX_SOCKET_OPERATE_VIA_PCAP:
						res = jvx_connect_client_send_st(runtime.client.ifaces[idPort].theIf, fld, szFld, cfgIn, whatsthis);
						break;

					case JVX_SOCKET_OPERATE_VIA_TCP:
						
						// There is only one connection..
						res = jvx_connect_client_send_st(runtime.client.ifaces[idPort].theIf, fld, szFld, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
						break;
					default:
						assert(0);
						break;
					}
					//assert(res == JVX_NO_ERROR);
					return res;
				}
				return JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
			break;

		default:
			assert(0);
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxSocket::stop_port(jvxSize idPort)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
		switch (config.whatToAddress)
		{
		case JVX_SOCKET_ADDRESS_SERVER:
			if (idPort < runtime.server.numifaces)
			{
				if (runtime.server.ifaces[idPort].theIf)
				{
					res = jvx_connect_server_trigger_shutdown_st(runtime.server.ifaces[idPort].theIf);
					assert(res == JVX_NO_ERROR);
					jvx_connect_server_optional_wait_for_shutdown_st(runtime.server.ifaces[idPort].theIf);
					assert(res == JVX_NO_ERROR);

					res = jvx_connect_server_terminate_st(runtime.server.ifaces[idPort].theIf);
					runtime.server.ifaces[idPort].theIf = NULL;
					runtime.server.ifaces[idPort].theReport = NULL;

					return res;
				}
				return JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
			break;
		case JVX_SOCKET_ADDRESS_CLIENT:
			if (idPort < runtime.client.numifaces)
			{
				if (runtime.client.ifaces[idPort].theIf)
				{
					res = jvx_connect_client_trigger_shutdown_st(runtime.client.ifaces[idPort].theIf);
					assert(res == JVX_NO_ERROR);
					jvx_connect_client_optional_wait_for_shutdown_st(runtime.client.ifaces[idPort].theIf);
					assert(res == JVX_NO_ERROR);

					res = jvx_connect_client_interface_terminate_st(runtime.client.ifaces[idPort].theIf);
					assert(res == JVX_NO_ERROR);
					
					runtime.client.ifaces[idPort].theIf = NULL;
					runtime.client.ifaces[idPort].theReport = NULL;

					return res;
				}
				return JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
			break;
		default:
			assert(0);
		}
	}
	return res;
}

// =============================================================

jvxErrorType
CjvxSocket::control_port(jvxSize idPort, jvxSize operation_id, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_socket_get_host_name_private* myHostNameStr = NULL;
	switch (config.whatToAddress)
	{
	case JVX_SOCKET_ADDRESS_SERVER:

		switch (operation_id)
		{
		case JVX_SOCKET_SERVER_CONTROL_GET_MAC_ADDRESS:
			if (idPort < runtime.server.numifaces)
			{
				if (runtime.server.ifaces[idPort].theIf)
				{
					if (priv)
					{
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_OUTPUT_STRING);
						char whoami[JVX_MAXSTRING];
						std::string whoamistr;
						memset(whoami, 0, sizeof(whoami));
						jvxErrorType res = jvx_connect_server_whoami_st(runtime.server.ifaces[idPort].theIf, whoami, JVX_MAXSTRING, JVX_CONNECT_SOCKET_NAME_MY_NAME);
						assert(res == JVX_NO_ERROR);
						whoamistr = whoami;
						if (priv)
						{
							((jvxApiString*)priv)->assign(whoamistr);
						}
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			break;

		case JVX_SOCKET_SERVER_CONTROL_GET_FIRST_IP_ADDRESS:

			if (idPort < runtime.server.numifaces)
			{
				if (runtime.server.ifaces[idPort].theIf)
				{
					if (priv)
					{
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_OUTPUT_STRING); 
						char whoami[JVX_MAXSTRING];
						std::string whoamistr;
						memset(whoami, 0, sizeof(whoami));
						jvxErrorType res = jvx_connect_server_whoami_st(runtime.server.ifaces[idPort].theIf, whoami, JVX_MAXSTRING, JVX_CONNECT_SOCKET_NAME_ONE_IP_ADDR);
						assert(res == JVX_NO_ERROR);
						whoamistr = whoami;
						if (priv)
						{
							((jvxApiString*)priv)->assign(whoamistr);
						}
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			break;

		case JVX_SOCKET_SERVER_CONTROL_GET_ALL_IP_ADDRESSES:

			if (idPort < runtime.server.numifaces)
			{
				if (runtime.server.ifaces[idPort].theIf)
				{
					if (priv)
					{
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_OUTPUT_STRING);
						char whoami[JVX_MAXSTRING];
						memset(whoami, 0, sizeof(whoami));
						std::string whoamistr;
						jvxErrorType res = jvx_connect_server_whoami_st(runtime.server.ifaces[idPort].theIf, whoami, JVX_MAXSTRING, JVX_CONNECT_SOCKET_NAME_ALL_IP_ADDR);
						assert(res == JVX_NO_ERROR);
						whoamistr = whoami;
						if (priv)
						{
							((jvxApiString*)priv)->assign(whoamistr);
						}
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			break;
	
		case JVX_SOCKET_SERVER_CONTROL_GET_HOSTNAME_FOR_IP_ADDRESS:
			if (idPort < runtime.server.numifaces)
			{
				if (runtime.server.ifaces[idPort].theIf)
				{
					if (priv)
					{
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_HOSTNAME_PRIVATE);
						jvx_socket_get_host_name_private* myHostNameStr = (jvx_socket_get_host_name_private*)priv;
						char whoami[JVX_MAXSTRING];
						std::string whoamistr;
						memset(whoami, 0, sizeof(whoami));
						jvxErrorType res = jvx_connect_server_identify_st(runtime.server.ifaces[idPort].theIf, whoami, JVX_MAXSTRING, myHostNameStr->whoisthat);
						assert(res == JVX_NO_ERROR);
						whoamistr = whoami;
						myHostNameStr->thatis.assign(whoamistr);
					
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			break;

		default:
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;

	case JVX_SOCKET_ADDRESS_CLIENT:
		res = JVX_ERROR_INVALID_SETTING;
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}

	return(res);
}

// =============================================================

jvxErrorType
CjvxSocket::terminate()
{
	if (_common_set_min.theState > JVX_STATE_NONE)
	{
		switch (config.whatToAddress)
		{
		case JVX_SOCKET_ADDRESS_SERVER:
			if (runtime.server.fam)
			{
				jvx_connect_server_family_terminate_st(runtime.server.fam);
				runtime.server.fam = NULL;
				JVX_SAFE_DELETE_FLD(runtime.server.ifaces, oneServerIf);
				runtime.server.numifaces = 0;
			}
			break;
		case JVX_SOCKET_ADDRESS_CLIENT:
			if (runtime.client.fam)
			{
				jvx_connect_client_family_terminate_st(runtime.client.fam);
				runtime.client.fam = NULL;
				JVX_SAFE_DELETE_FLD(runtime.client.ifaces, oneClientIf);
				runtime.client.numifaces = 0;
			}
			break;
		default:
			break;
		}
		_common_set.theComponentSubTypeDescriptor = JVX_CONNECTION_TAG_SOCKET;
		jvxErrorType res = CjvxObject::_terminate();
		assert(res == JVX_NO_ERROR);
	}
	return JVX_NO_ERROR;
}

