#include "CjvxRemoteCall.h"
extern "C"
{
#include "jvx-crc.h"
}

jvxDspBaseErrorType 
CjvxRemoteCall::jvx_rc_accept_init_request(jvxHandle* hdl, const char* description, jvxSize numInParams, jvxSize numOutParams, jvxSize processingId)
{
	if (hdl)
	{
		CjvxRemoteCall* myPtr = (CjvxRemoteCall*)hdl;
		return myPtr->ic_rc_accept_init_request(description, numInParams, numOutParams, processingId);
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}
	
jvxDspBaseErrorType
CjvxRemoteCall::jvx_rc_unlock_receive_buffer(jvxHandle* hdl, jvxSize unique_id)
{
	if (hdl)
	{
		CjvxRemoteCall* myPtr = (CjvxRemoteCall*)hdl;
		return myPtr->ic_rc_unlock_receive_buffer(unique_id);
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxRemoteCall::jvx_rc_allocate_memory(jvxHandle* hdl, jvx_rc_allocation_purpose purp, jvxHandle** ptrReturn, jvxSize* offset, 
	jvxSize numElements, jvxSize* szFldAllocated, jvxDataFormat form)
{
	if (hdl)
	{
		CjvxRemoteCall* myPtr = (CjvxRemoteCall*)hdl;
		return myPtr->ic_rc_allocate_memory(purp, ptrReturn, offset, numElements, szFldAllocated, form);
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxRemoteCall::jvx_rc_deallocate_memory(jvxHandle* hdl, jvx_rc_allocation_purpose purp, jvxHandle* ptrReturn, jvxSize szFldAllocated)
{
	if (hdl)
	{
		CjvxRemoteCall* myPtr = (CjvxRemoteCall*)hdl;
		return myPtr->ic_rc_deallocate_memory(purp, ptrReturn, szFldAllocated);
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxRemoteCall::jvx_rc_send_buffer(jvxHandle* hdl, jvxByte* resp, jvxSize numBytes)
{
	if (hdl)
	{
		CjvxRemoteCall* myPtr = (CjvxRemoteCall*)hdl;
		return myPtr->ic_rc_send_buffer(resp, numBytes);
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxRemoteCall::jvx_rc_process_request(jvxHandle* hdl, const char* description, jvxRCOneParameter* paramsIn, jvxSize numParamsIn, 
	jvxRCOneParameter* paramsOut, jvxSize numParamsOut, jvxSize processingId, jvxCBool* immediateResponse)
{
	if (hdl)
	{
		CjvxRemoteCall* myPtr = (CjvxRemoteCall*)hdl;
		return myPtr->ic_rc_process_request(description, paramsIn, numParamsIn, paramsOut, numParamsOut, processingId, immediateResponse);
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxRemoteCall::jvx_rc_lock(jvxHandle* hdl)
{
	if (hdl)
	{
		CjvxRemoteCall* myPtr = (CjvxRemoteCall*)hdl;
		return myPtr->ic_rc_lock();
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxRemoteCall::jvx_rc_unlock(jvxHandle* hdl)
{
	if (hdl)
	{
		CjvxRemoteCall* myPtr = (CjvxRemoteCall*)hdl;
		return myPtr->ic_rc_unlock();
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxRemoteCall::jvx_rc_report_local_error(jvxHandle* hdl, const char* text, jvxDspBaseErrorType errCode, jvxSize errId)
{
	if (hdl)
	{
		CjvxRemoteCall* myPtr = (CjvxRemoteCall*)hdl;
		return myPtr->ic_rc_report_local_error(text, errCode, errId);
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxRemoteCall::jvx_rc_report_transfer_terminated(jvxHandle* hdl, jvxSize processing_id, jvxSize processing_counter, jvxDspBaseErrorType resulTransfer, const char* err_description)
{
	if (hdl)
	{
		CjvxRemoteCall* myPtr = (CjvxRemoteCall*)hdl;
		return myPtr->ic_rc_report_transfer_terminated(processing_id, processing_counter, resulTransfer, err_description);
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

// ==========================================================================================================

jvxDspBaseErrorType
CjvxRemoteCall::ic_rc_accept_init_request(const char* description, jvxSize numInParams, jvxSize numOutParams, jvxSize processingId)
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	if (reportManager)
	{
		res = JVX_DSP_NO_ERROR;
		resL = reportManager->reportActionRequest(description, numInParams, numOutParams, processingId);
		if (resL != JVX_NO_ERROR)
		{
			res = JVX_DSP_ERROR_UNSUPPORTED;
		}
	}
	return(res);
}
	
jvxDspBaseErrorType
CjvxRemoteCall::ic_rc_unlock_receive_buffer( jvxSize unique_id)
{
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
CjvxRemoteCall::ic_rc_allocate_memory(jvx_rc_allocation_purpose purp, jvxHandle** ptrReturn, jvxSize* offset, 
	jvxSize numElements, jvxSize* szFldAllocated, jvxDataFormat form)
{
	jvxSize sz = 0;
	jvxSize szElm = 0;
	jvxSize szFld = 0;
	if(ptrReturn && offset)
	{
		switch(purp)
		{
		case JVX_RC_ALLOCATE_PURPOSE_INPUT_FIELD:
		case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_FIELD:
		case JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD_PTR:
		case JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD:
		case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD_PTR:
		case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD:

			if(offset)
				*offset = 0;

			szElm = jvxDataFormat_size[form];
			assert(szElm != 0);
			szFld = szElm * numElements;
			
			JVX_SAFE_NEW_FLD_CVRT(*ptrReturn, jvxByte, szFld, jvxHandle*);
			memset(*ptrReturn, 0, sizeof(jvxByte) * szFld);

			if(szFldAllocated)
			{
				* szFldAllocated = szFld;
			}
			break;

		case JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD:

			sz = myFrameConstraint.bytesPrepend;
			if (offset)
				*offset = sz;

			szElm = jvxDataFormat_size[form];
			assert(szElm != 0);
			szFld = szElm * numElements + sz;

			if(JVX_CHECK_SIZE_SELECTED(myFrameConstraint.fldMinSize))
			{
				szFld = JVX_MAX(myFrameConstraint.fldMinSize, szFld);
			}
			if(JVX_CHECK_SIZE_SELECTED(myFrameConstraint.fldMaxSize))
			{
				assert(szFld <= myFrameConstraint.fldMaxSize);
			}
			JVX_SAFE_NEW_FLD_CVRT(*ptrReturn, jvxByte, szFld, jvxHandle*);
			memset(*ptrReturn, 0, sizeof(jvxByte) * szFld);
			if(szFldAllocated)
			{
				* szFldAllocated = szFld;
			}
			break;

		default:
			assert(0);
		}

		return JVX_DSP_NO_ERROR;
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType
CjvxRemoteCall::ic_rc_deallocate_memory(jvx_rc_allocation_purpose purp, jvxHandle* ptrReturn, jvxSize szFldAllocated)
{
	if(ptrReturn)
	{
		switch(purp)
		{
		case JVX_RC_ALLOCATE_PURPOSE_INPUT_FIELD:
		case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_FIELD:
		case JVX_RC_ALLOCATE_PURPOSE_TRANSFER_FIELD:
		case JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD_PTR:
		case JVX_RC_ALLOCATE_PURPOSE_INPUT_MULTCHAN_FLD:
		case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD_PTR:
		case JVX_RC_ALLOCATE_PURPOSE_OUTPUT_MULTCHAN_FLD:
			JVX_SAFE_DELETE_FLD(ptrReturn, jvxByte);
			break;
		default:
			assert(0);
		}

		return JVX_DSP_NO_ERROR;
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);}

jvxDspBaseErrorType
CjvxRemoteCall::ic_rc_send_buffer(jvxByte* resp, jvxSize numBytes)
{
#ifdef JVX_INTEGRATE_CRC_REMOTE_CALL

	jvxRcProtocol_simple_message* fldCrc = NULL;
#endif

	jvxSize sendThis = numBytes;
	jvxHandle* addData = NULL;

	jvx_socket_send_private myPrivateSendTarget;
	jvxConnectionPrivateTypeEnum whatsthis = JVX_CONNECT_PRIVATE_ARG_TYPE_NONE;

	switch (whatToAddress)
	{
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_TCP:
		break;
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_TCP:

		myPrivateSendTarget.idConnection = derived.tcp.idChannelIn;
		myPrivateSendTarget.target = NULL;
		addData = &myPrivateSendTarget;
		whatsthis = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SEND_PRIVATE;
		break;

	case JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_UDP:
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_UDP:

		addData = NULL;
		whatsthis = JVX_CONNECT_PRIVATE_ARG_TYPE_NONE;
		break;
	case JVX_REMOTE_CALL_ADDRESS_RS232:
		break;
	}

#ifdef JVX_INTEGRATE_CRC_REMOTE_CALL
	assert(sendThis >= sizeof(jvxRcProtocol_simple_message));
	fldCrc = (jvxRcProtocol_simple_message*)(resp + myFrameConstraint.bytesPrepend);
	fldCrc->crc = 0;
	fldCrc->crc = jvx_crc16_ccitt((jvxByte*)fldCrc, fldCrc->loc_header.paketsize);
#endif

	jvxErrorType res = myConnection->sendMessage_port(portId, resp,&sendThis, addData, whatsthis);
	if (sendThis == numBytes)
	{
		if (res == JVX_NO_ERROR)
		{
			return JVX_DSP_NO_ERROR;
		}
	}
	return JVX_DSP_ERROR_CALL_SUB_COMPONENT_FAILED;
}

jvxDspBaseErrorType
CjvxRemoteCall::ic_rc_process_request(const char* description, jvxRCOneParameter* paramsIn, jvxSize numParamsIn, 
	jvxRCOneParameter* paramsOut, jvxSize numParamsOut, jvxSize processingId, jvxCBool* immediateResponse)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (reportManager)
	{
		res = reportManager->reportPendingAction(paramsIn, numParamsIn, paramsOut, numParamsOut, description, processingId, immediateResponse);
		if (res != JVX_NO_ERROR)
		{
			return JVX_DSP_ERROR_UNSUPPORTED;
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_UNSUPPORTED;
}

jvxDspBaseErrorType
CjvxRemoteCall::ic_rc_lock()
{
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
CjvxRemoteCall::ic_rc_unlock()
{
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
CjvxRemoteCall::ic_rc_report_local_error(const char* text, jvxDspBaseErrorType errCode, jvxSize errId)
{
	if (reportManager)
	{
		reportManager->reportLocalError(text, errId);
	}

	JVX_LOCK_MUTEX(safeAccess);
	switch (myChannelStatus)
	{
	case JVX_REMOTE_CALL_CHANNEL_STATUS_WAIT_OUTGOING:

		std::cout << "Local error reported: " << text << ", error type: " << errCode << std::endl;
		this->outgoingRequest.fCall_result = JVX_ERROR_ABORT;

		// State is switched back by caller
		if (this->myThreadingModel == JVX_CONNECT_THREAD_MULTI_THREAD)
		{
			JVX_SET_NOTIFICATION(outgoingRequest.hdlOperationComplete);
		}
		else
		{
			singleThread.completed = true;
		}
		break;
	case JVX_REMOTE_CALL_CHANNEL_STATUS_ACCEPT_INCOMING:
		myChannelStatus = JVX_REMOTE_CALL_CHANNEL_STATUS_AVAILABLE;
		break;
	default:
		// Some pendinh operation may still arrive
		break;
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
CjvxRemoteCall::ic_rc_report_transfer_terminated( jvxSize processing_id, jvxSize processing_counter, jvxDspBaseErrorType resulTransfer, const char* err_description)
{
	JVX_LOCK_MUTEX(safeAccess);
	switch(myChannelStatus)
	{
	case JVX_REMOTE_CALL_CHANNEL_STATUS_WAIT_OUTGOING:
	
		// Set the local error code:
		if (resulTransfer == JVX_DSP_NO_ERROR)
		{
			this->outgoingRequest.fCall_result = JVX_NO_ERROR;
		}
		else
		{
			this->outgoingRequest.fCall_result = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		}

		// State is switched back by caller
		if (this->myThreadingModel == JVX_CONNECT_THREAD_MULTI_THREAD)
		{
			// No need to retrigger thread if in single threading mode
			JVX_SET_NOTIFICATION(outgoingRequest.hdlOperationComplete);
		}
		else
		{
			singleThread.completed = true;
		}

		break;
	case JVX_REMOTE_CALL_CHANNEL_STATUS_ACCEPT_INCOMING:
		myChannelStatus = JVX_REMOTE_CALL_CHANNEL_STATUS_AVAILABLE;
		break;
	default:
		// Some pendinh operation may still arrive
		break;
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	return JVX_DSP_NO_ERROR;
}

// ================================================================================
// ================================================================================

/*
 * General strategy: we should always read ALL bytes from the input. That is, we may
 * get problems on UDP sockets if we do not read full datagrams at once. On the other hand, 
 * we may get buffers which contain more than one packet. All this, we need to take into account.
 */

jvxErrorType
CjvxRemoteCall::provide_data_and_length(jvxByte** ptrRet, jvxSize* maxNumCopy, jvxSize* offset, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvx_socket_connection_tcp_private* pS = NULL;

	// 	jvx_socket_connection_private* pS = (jvx_socket_connection_tcp_private*)addInfo;

	const char* pC = NULL;
	//const char* pC = (const char*)addInfo;

	jvxSize numCopy;
	switch (whatToAddress)
	{
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_TCP:
		pS = (jvx_socket_connection_tcp_private*)addInfo;
		if (pS->idConnection != derived.tcp.idChannelIn)
		{
			*ptrRet = NULL;
			*maxNumCopy = 0;
			*offset = 0;
			return(JVX_ERROR_INVALID_SETTING);
		}
		break;
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_TCP:
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_UDP:
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_UDP:
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_PCAP:
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_PCAP:
	case JVX_REMOTE_CALL_ADDRESS_RS232:
		break;

	default:
		assert(0);
	}

	if (*maxNumCopy > (ram.sz - ram.readOffset))
	{
		jvxByte* tmp = ram.fld;

		numCopy = (ram.readOffset - ram.evaluateOffset);
		ram.sz = *maxNumCopy + numCopy;
		JVX_SAFE_NEW_FLD(ram.fld, jvxByte, ram.sz);
		if (numCopy)
		{
			memcpy(ram.fld, tmp + ram.evaluateOffset, numCopy);
		}
		if (tmp)
		{
			JVX_SAFE_DELETE_FLD(tmp, jvxByte);
		}
		ram.readOffset = numCopy;
		ram.evaluateOffset = 0;
	}
	*ptrRet = ram.fld;
	//*sz = ram.sz; <- always read maxNumCopy
	*offset = ram.readOffset;

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxRemoteCall::report_data_and_read(jvxByte* ptr, jvxSize numRead, jvxSize offset, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
#ifdef JVX_INTEGRATE_CRC_REMOTE_CALL

	jvxRcProtocol_simple_message* fldCrc = NULL;
	jvxUInt16 crcS = 0;
	jvxUInt16 crcP = 0;

#endif

	//jvx_socket_connection_tcp_private* pS = (jvx_socket_connection_tcp_private*)addInfo;
	//const char* pC = (const char*)addInfo;
	jvxProtocolHeader* inField = NULL;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize numMove;
	jvxBool loopCont = true;

	jvxProtocolHeader* dbg = (jvxProtocolHeader*)ram.fld + ram.evaluateOffset;

	if (numRead > 0)
	{
		ram.readOffset += numRead;
		while (loopCont)
		{
			switch (stateMachine.theState)
			{
			case JVX_INPUT_STATE_HEADER:
				if ((ram.readOffset - ram.evaluateOffset) >= sizeof(jvxProtocolHeader))
				{
					inField = (jvxProtocolHeader*)ram.fld + ram.evaluateOffset;
					stateMachine.theState = JVX_INPUT_STATE_FIELD;
					stateMachine.packetSize = inField->paketsize;

					//std::cout << "--> Expect packet size of: " << stateMachine.packetSize << std::endl;
				}
				else
				{
					loopCont = false;
				}
				break;
			case JVX_INPUT_STATE_FIELD:
				if ((ram.readOffset - ram.evaluateOffset) >= stateMachine.packetSize)
				{
					inField = (jvxProtocolHeader*)ram.fld + ram.evaluateOffset;

					if (inField->fam_hdr.proto_family == JVX_PROTOCOL_TYPE_REMOTE_CALL)
					{
						switch (whatToAddress)
						{
						case JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_TCP:
							break;
						case JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_TCP:
							break;
						default:
							break;
						}

						//std::cout << "Hier!" << inField->paketsize << std::endl;
						if (reportManager)
						{

#ifdef JVX_INTEGRATE_CRC_REMOTE_CALL
							fldCrc = (jvxRcProtocol_simple_message*)inField;
							crcS = fldCrc->crc;
							fldCrc->crc = 0;
							crcP = jvx_crc16_ccitt((jvxByte*)fldCrc, fldCrc->loc_header.paketsize);
							assert(crcS == crcP);
#endif

							resL = jvx_remote_call_incoming(&hdlRemoteCall, inField, 0, NULL, 0);
							if (resL != JVX_DSP_NO_ERROR)
							{
								std::cout << "Remote Call State Machine Error" << std::endl;
							}
						}
					}
					ram.evaluateOffset += stateMachine.packetSize;
					numMove = (ram.readOffset - ram.evaluateOffset);
					if (numMove > 0)
					{
						memmove(ram.fld, ram.fld + ram.evaluateOffset, numMove);
					}
					ram.evaluateOffset = 0;
					ram.readOffset = 0;
					stateMachine.theState = JVX_INPUT_STATE_HEADER;
				}
				else
				{
					loopCont = false;
				}
				break;
			}
		}
	}

	// Post processing
	switch (whatToAddress)
	{
	case JVX_REMOTE_CALL_ADDRESS_RS232:
		// A zero may be reported if there is a start or stop character
		if(rs232Specific.sc_active)
		{
			if (addInfo)
			{
				jvxByte* specChar = (jvxByte*)addInfo;
				if (*specChar == rs232Specific.startChar)
				{
					if (stateMachine.theState == JVX_INPUT_STATE_HEADER)
					{
						if (stateMachine.sc_startScanned == false)
						{
							stateMachine.sc_startScanned = true;
						}
						else
						{
							return(JVX_ERROR_PROTOCOL_ERROR);
						}
					}
					else
					{
						return(JVX_ERROR_PROTOCOL_ERROR);
					}
				}
				if (*specChar == rs232Specific.stopChar)
				{
					if (stateMachine.theState == JVX_INPUT_STATE_HEADER)
					{
						if (stateMachine.sc_startScanned == true)
						{
							stateMachine.sc_startScanned = false;
						}
						else
						{
							return(JVX_ERROR_PROTOCOL_ERROR);
						}
					}
					else
					{
						return(JVX_ERROR_PROTOCOL_ERROR);
					}
				}
			}
		}
		else
		{
			if (numRead == 0)
			{
				assert(0);
			}
		}
		break;
	default:
		if (numRead == 0)
		{
			assert(0);
		}
		break;
	}
	return(JVX_NO_ERROR);
}


jvxErrorType
CjvxRemoteCall::report_event(jvxBitField eventMask, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	std::string txt;
	jvx_socket_get_host_name_private getHostName;
	jvxBool reportDisconnect = false;
	jvxBool reportConnect = false;
	std::string thePartner = "unknown";
	jvxBool reportError = false;
	jvxSize errId = JVX_SIZE_UNSELECTED;
	std::string errText;

	if (jvx_bitTest(eventMask, JVX_CONNECTION_REPORT_STARTUP_COMPLETE_SHFT))
	{
		std::cout << __FUNCTION__ << ": Event STARTUP COMPLETE" << std::endl;
	}
	if (jvx_bitTest(eventMask, JVX_CONNECTION_REPORT_SHUTDOWN_COMPLETE_SHFT))
	{
		std::cout << __FUNCTION__ << ": Event SHUTDOWN COMPLETE" << std::endl;
	}
	if (jvx_bitTest(eventMask, JVX_CONNECTION_REPORT_ERROR_SHFT))
	{
		std::cout << __FUNCTION__ << ": Event REPORT ERROR" << std::endl;
	}

	// ==========================================================================
	switch (whatToAddress)
	{
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_TCP:
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_UDP:
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_PCAP:

		if (jvx_bitTest(eventMask, JVX_SOCKET_CLIENT_REPORT_UNSUCCESSFUL_OUTGOING_SHFT))
		{
			errText = "Timeout while trying to connect";
		}
		if (jvx_bitTest(eventMask, JVX_SOCKET_CLIENT_REPORT_CONNECT_OUTGOING_SHFT))
		{
			reportConnect = true;
		}
		if (jvx_bitTest(eventMask, JVX_SOCKET_CLIENT_REPORT_DISCONNECT_OUTGOING_SHFT))
		{
			reportDisconnect = true;
		}
		break;
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_TCP:
		if (jvx_bitTest(eventMask, JVX_SOCKET_SERVER_REPORT_DENIED_INCOMING_SHFT))
		{
			reportError = true;
		}
		if (jvx_bitTest(eventMask, JVX_SOCKET_SERVER_REPORT_CONNECT_INCOMING_SHFT))
		{
			jvx_socket_connection_tcp_private* connData = (jvx_socket_connection_tcp_private*)addInfo;
			assert(connData);
			assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONNECTION_TCP);
			derived.tcp.idChannelIn = connData->idConnection;
			getHostName.whoisthat = (const char*)connData->origin;
			//getHostName.thatis;

			myConnection->control_port(portId, JVX_SOCKET_SERVER_CONTROL_GET_HOSTNAME_FOR_IP_ADDRESS,
						   &getHostName, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_HOSTNAME_PRIVATE);
			txt = getHostName.thatis.std_str();
			thePartner = txt;
			reportConnect = true;

		}
		if (jvx_bitTest(eventMask, JVX_SOCKET_SERVER_REPORT_DISCONNECT_INCOMING_SHFT))
		{
			derived.tcp.idChannelIn = JVX_SIZE_UNSELECTED;
			reportDisconnect = true;
		}
		break;
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_UDP:
	case JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_PCAP:

		if (jvx_bitTest(eventMask, JVX_SOCKET_SERVER_REPORT_DENIED_INCOMING_SHFT))
		{
			reportError = true;
		}
		if (jvx_bitTest(eventMask, JVX_SOCKET_SERVER_REPORT_CONNECT_INCOMING_SHFT))
		{
			thePartner = "";
			reportConnect = true;

		}
		if (jvx_bitTest(eventMask, JVX_SOCKET_SERVER_REPORT_DISCONNECT_INCOMING_SHFT))
		{
			reportDisconnect = true;
		}
		break;
	case JVX_REMOTE_CALL_ADDRESS_RS232:
		if (jvx_bitTest(eventMask, JVX_RS232_REPORT_CONNECTION_ESTABLISHED_SHIFT))
		{
			reportConnect = true;
		}
		if (jvx_bitTest(eventMask, JVX_RS232_REPORT_CONNECTION_STOPPED_SHIFT))
		{
			reportDisconnect = true;
		}
		if (jvx_bitTest(eventMask, JVX_RS232_REPORT_ERROR_SHFT))
		{
			assert(0);
		}
		break;
	}

	if(reportConnect)
	{
		if(reportManager)
		{
			reportManager->reportConnectionEstablished(thePartner.c_str());
		}
	}
	if(reportDisconnect)
	{
		if(reportManager)
		{
			reportManager->reportConnectionClosed();
		}
	}
	if(reportError)
	{
		if(reportManager)
		{
			reportManager->reportLocalError(errText.c_str(), errId);
		}
	}
		
	return(JVX_NO_ERROR);
}
