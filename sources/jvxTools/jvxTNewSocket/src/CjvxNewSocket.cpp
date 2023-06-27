#include "jvx.h"
#include "CjvxNewSocket.h"
#include <iostream>

#define JVX_SOCKET_PORT_NAME 1024

CjvxNewSocket::CjvxNewSocket(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_CONNECTION);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxConnection*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theComponentSubTypeDescriptor = "/";
	_common_set.theComponentSubTypeDescriptor += JVX_CONNECTION_TAG_SOCKET;
	_common_set_min.theDescription = "JVX New Socket connection module";

	JVX_INITIALIZE_MUTEX(safeAccessHandles);
}

CjvxNewSocket::~CjvxNewSocket()
{
	this->terminate();
	JVX_TERMINATE_MUTEX(safeAccessHandles);
}

jvxErrorType 
CjvxNewSocket::initialize(IjvxHiddenInterface* hostRef, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)
{	
	jvxErrorType res = CjvxObject::_initialize();
	jvxSize numPorts = 0;
	if(res == JVX_NO_ERROR)
	{
		if (whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_INPUT_STRING)
		{
			std::string command = (const char*)priv;
			if (command == "udp")
			{
				sFac.initialize(jvxSocketsConnectionType::JVX_SOCKET_TYPE_UDP);
			}
			else
			{
				res = JVX_ERROR_UNSUPPORTED;
			}
		}
	}
	return res;
}

jvxErrorType 
CjvxNewSocket::availablePorts(jvxApiStringList* allPorts)
{
	std::vector<std::string> allPortsL;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
		jvxSize num = 0;
		sFac.number_interfaces(&num);
		for (i = 0; i < num; i++)
		{
			CjvxSocketsClientInterface* theIface = nullptr;
			sFac.request_interface(&theIface, nullptr, i);
			assert(theIface);

			allPorts->add(theIface->name);
			sFac.return_interface(theIface);			
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

// =============================================================
// Port specific commands
// =============================================================

jvxErrorType 
CjvxNewSocket::state_port(jvxSize idPort, jvxState* theState)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_min.theState >= JVX_STATE_INIT)
	{	
		jvxSize num = 0;
		sFac.number_interfaces(&num);
		if (idPort < num)
		{
			CjvxSocketsClientInterface* theIface = nullptr;
			sFac.request_interface(&theIface, nullptr,idPort);
			assert(theIface);

			switch (theIface->theState)
			{
			case jvxSocketsConnectionState::JVX_STATE_CONNECTION_INIT:
				if (theState)
				{
					*theState = JVX_STATE_INIT;
				}
				break;
			default:
				if (theState)
				{
					*theState = JVX_STATE_ACTIVE;
				}
				break;
			}
			sFac.return_interface(theIface);
		}
		return(res);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxNewSocket::start_port(jvxSize idPort, jvxHandle* cfgIn, jvxConnectionPrivateTypeEnum whatsthis, IjvxConnection_report* theReportIn)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
		jvxSize num = 0;
		sFac.number_interfaces(&num);
		if (idPort < num)
		{			
			sFac.request_interface(&theIfaceStart, nullptr, idPort);
			switch (theIfaceStart->socketType)
			{
			case jvxSocketsConnectionType::JVX_SOCKET_TYPE_UDP:
				theIfaceStartUdp = static_cast<CjvxSocketsClientInterfaceTcpUdp*>(theIfaceStart);
				if (theIfaceStartUdp)
				{
					if (whatsthis == jvxConnectionPrivateTypeEnum::JVX_CONNECT_PRIVATE_ARG_TYPE_INPUT_STRING)
					{
						std::string cfgToken = (const char*)cfgIn;
					}
					theIfaceStartUdp->configure("", -1, 65000);
				}
				break;
			default:
				res = JVX_ERROR_UNSUPPORTED;
			}

			if (res == JVX_NO_ERROR)
			{
				theIfaceStart->start(this);
				theReport = theReportIn;
			}
			else
			{
				sFac.return_interface(theIfaceStart);
			}
		}
		// sFac.
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxNewSocket::get_constraints_buffer(jvxSize idPort, jvxSize* bytesPrepend, jvxSize* fldMinSize, jvxSize* fldMaxSize)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
				return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxNewSocket::sendMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld, jvxHandle* cfgIn, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_min.theState >= JVX_STATE_INIT)
	{	
		// Use the connection link to send a message
		JVX_LOCK_MUTEX(safeAccessHandles);
		if (this->theConnectionHandle)
		{
			jvxSize sz = *szFld;
			sendOne.set_buffer(fld, sz);
			this->theConnectionHandle->transfer(&sendOne, szFld);
		}
		JVX_UNLOCK_MUTEX(safeAccessHandles);

		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxNewSocket::stop_port(jvxSize idPort)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
		// CLeanup input field
		deallocate_fld();

		if (theIfaceStart)
		{
			theIfaceStart->stop();

			sFac.return_interface(theIfaceStart);
		}
		theReport = nullptr;
		theIfaceStart = nullptr;
		theIfaceStartUdp = nullptr;
	}
	return res;
}

// =============================================================

jvxErrorType
CjvxNewSocket::control_port(jvxSize idPort, jvxSize operation_id, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_socket_get_host_name_private* myHostNameStr = NULL;

	return(res);
}

// =============================================================

jvxErrorType
CjvxNewSocket::terminate()
{
	if (_common_set_min.theState > JVX_STATE_NONE)
	{		
		jvxErrorType res = CjvxObject::_terminate();
		assert(res == JVX_NO_ERROR);
	}
	return JVX_NO_ERROR;
}

// ========================================================================
// ========================================================================

jvxErrorType 
CjvxNewSocket::report_client_connect(IjvxSocketsConnection* connection) 
{
	// Register the report
	JVX_LOCK_MUTEX(safeAccessHandles);
	this->theConnectionHandle = connection;
	JVX_UNLOCK_MUTEX(safeAccessHandles);

	this->theConnectionHandle->register_report(this);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxNewSocket::report_client_disconnect(IjvxSocketsConnection* connection) 
{
	assert(this->theConnectionHandle == connection);

	// Register the report
	this->theConnectionHandle->unregister_report(this);

	JVX_LOCK_MUTEX(safeAccessHandles);
	this->theConnectionHandle = nullptr;
	JVX_UNLOCK_MUTEX(safeAccessHandles);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxNewSocket::report_client_error(jvxSocketsErrorType err, const char* description) 
{
	std::cout << __FUNCTION__ << ": Error reported by socket submodule client interface." << std::endl;
	return JVX_NO_ERROR;
}


// ========================================================================
// ========================================================================

jvxErrorType 
CjvxNewSocket::provide_data_and_length(char** fld_use,
	jvxSize* numCpy, jvxSize* offset, IjvxSocketsAdditionalInfoPacket* additionalInfo)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;

	// We add an intermediate buffer since the message must be fully read at once, otherwise an error is reported and the message is truncated
	if (*numCpy > fldReadSz)
	{
		deallocate_fld();
		allocate_fld(*numCpy);
	}

	*fld_use = fldRead;

	return res;
}

jvxErrorType 
CjvxNewSocket::report_data_and_read(char* fld_filled, jvxSize numCpy, jvxSize offset,
	IjvxSocketsAdditionalInfoPacket* additionalInfo)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;

	jvxSize offsetIn = 0;
	jvxSize offsetOut = 0;
	jvxByte* fld_use = nullptr;

	jvxSize lenFieldUse = numCpy;
	jvxByte* ptrIn = fldRead;
	
	if (theReport)
	{		
		while (lenFieldUse)
		{
			jvxSize lenCopy = lenFieldUse;
			res = theReport->provide_data_and_length(&fld_use, &lenCopy, &offsetOut, myId, additionalInfo,
				jvxConnectionPrivateTypeEnum::JVX_CONNECT_PRIVATE_ARG_TYPE_NEW_SOCKET_ADD_INFO);

			if (fld_use)
			{
				jvxByte* ptrOut = fld_use;
				ptrOut += offsetOut;
				memcpy(ptrOut, ptrIn, lenCopy);
				theReport->report_data_and_read(fld_use, lenCopy, offsetOut, myId, additionalInfo, 
					jvxConnectionPrivateTypeEnum::JVX_CONNECT_PRIVATE_ARG_TYPE_NEW_SOCKET_ADD_INFO);
			}
			else
			{
				std::cout << __FUNCTION__ << ": Error: invalid pointer to provide data!" << std::endl;
			}
			lenFieldUse -= lenCopy;
			ptrIn += lenCopy;
		}
	}
	return res;
}

jvxErrorType
CjvxNewSocket::report_connection_error(jvxSocketsErrorType errCode, const char* errDescription)
{
	std::cout << __FUNCTION__ << ": Error reported by socket submodule connection interface." << std::endl;
	return JVX_NO_ERROR;
}

// ===============================================================================================
void 
CjvxNewSocket::deallocate_fld()
{
	if (fldReadSz)
	{
		fldReadSz = 0;
		JVX_SAFE_DELETE_FIELD(fldRead);
	}
}

void
CjvxNewSocket::allocate_fld(jvxSize fldSz)
{
	if (fldSz)
	{
		fldReadSz = fldSz;
		JVX_SAFE_ALLOCATE_FIELD(fldRead, jvxByte, fldReadSz);
	}
}