#include "CjvxRemoteCall.h"

#include <cassert>

#define TIMEOUT_STOP_MS 500

#define INCOMING_ERROR_SIZE_MISMATCH { incomingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_SIZE_MISMATCH;}
#define INCOMING_ERROR_NUM_PARAMETERS_IN { incomingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_NUM_PARAMETERS_IN;}
#define INCOMING_ERROR_NUM_PARAMETERS_OUT { incomingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_NUM_PARAMETERS_OUT;}
#define INCOMING_ERROR_WRONG_STATE { incomingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_WRONG_STATE;}
#define INCOMING_ERROR_WRONG_PROCESS_COUNTER { incomingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_WRONG_PROCESS_COUNTER;}
#define INCOMING_ERROR_NUM_PARAMS_INCOMPLETE { incomingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_NUM_PARAMS_INCOMPLETE;}

#define OUTGOING_ERROR_SIZE_MISMATCH { outgoingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_SIZE_MISMATCH;}
#define OUTGOING_ERROR_NUM_PARAMETERS_IN { outgoingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_NUM_PARAMETERS_IN;}
#define OUTGOING_ERROR_NUM_PARAMETERS_OUT { outgoingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_NUM_PARAMETERS_OUT;}
#define OUTGOING_ERROR_WRONG_STATE { outgoingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_WRONG_STATE;}
#define OUTGOING_ERROR_WRONG_PROCESS_COUNTER { outgoingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_WRONG_PROCESS_COUNTER;}
#define OUTGOING_ERROR_NUM_PARAMS_INCOMPLETE { outgoingError = true; localError = RTP_ERROR_REMOTE_CALL_CONNECTION_NUM_PARAMS_INCOMPLETE;}

// ================================================================
// ================================================================


/** 
 * Constructor: Allocate all internal data.
 *///==========================================================
CjvxRemoteCall::CjvxRemoteCall(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_REMOTE_CALL);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxRemoteCall*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	myConnection = NULL;

	ram.fld = NULL;
	ram.sz = 0;

	JVX_INITIALIZE_MUTEX(this->safeAccess);
	myChannelStatus = JVX_REMOTE_CALL_CHANNEL_STATUS_AVAILABLE;

	JVX_INITIALIZE_NOTIFICATION(this->outgoingRequest.hdlOperationComplete);
}
	
/** 
 * Terminate this module and deallocate all internal data.
 *///==========================================================
CjvxRemoteCall::~CjvxRemoteCall()
{
	if (_common_set_min.theState > JVX_STATE_INIT)
	{
		this->stop();
	}
	if (_common_set_min.theState == JVX_STATE_INIT)
	{
		this->terminate();
	}

	JVX_TERMINATE_MUTEX(this->safeAccess);
	JVX_TERMINATE_NOTIFICATION(this->outgoingRequest.hdlOperationComplete);
}

jvxErrorType
CjvxRemoteCall::supportsOperationMode(jvxRCOperationMode mode, jvxBool* doesSupport)
{
	if (doesSupport)
	{
		switch (mode)
		{
		case JVX_RC_OPERATE_MASTER:
		case JVX_RC_OPERATE_SLAVE:
			*doesSupport = true;
			break;
		default:
			*doesSupport = false;
			break;
		}
		return(JVX_NO_ERROR);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxRemoteCall::initialize(IjvxHiddenInterface* hostRef, IjvxConnection* conn, jvxRCOperationMode mode, jvxHandle* configStruct, IjvxRemoteCall_callback* backRef)
{
	jvxErrorType res = _initialize(hostRef);
	jvxApiString fldStr ;
	IjvxPrintf* myPrintfRef = NULL;
	if (res == JVX_NO_ERROR)
	{
		locptf = printf;
		myConnection = conn;
		myMode = mode;
		reportManager = backRef;
		globalCount = 0;
		myFrameConstraint.bytesPrepend = 0;
		myFrameConstraint.fldMinSize = 0;
		myFrameConstraint.fldMaxSize = 0;

		whatToAddress = JVX_REMOTE_CALL_ADDRESS_UNKNOWN;
		myConnection->request_specialization(NULL, NULL, NULL, &fldStr);
		myConnectionDescriptor = fldStr.std_str();
		if(_common_set_min.theHostRef)
		{ 
			_common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_PRINTF, reinterpret_cast<jvxHandle**>(&myPrintfRef));
			if (myPrintfRef)
			{
				myPrintfRef->request_printf(&locptf);
			}
			_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_PRINTF, reinterpret_cast<jvxHandle*>(myPrintfRef));
		}
		locptf("Starting <CjvxRemoteCall>\n");
		
	}
	return(res);
}

jvxErrorType 
CjvxRemoteCall::start(jvxHandle* connectionPrivate, jvxConnectionPrivateTypeEnum whatsthis, jvxSize idPortIn, jvxSize preAllocateBytes)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	std::string descr;
	std::vector<std::string> lstTokensPath;
	jvxApiString fldStr;

	if (_common_set_min.theState == JVX_STATE_INIT)
	{
		// Let us check out what type of connection we have received
		myConnection->request_specialization(NULL, NULL, NULL, &fldStr);
		descr = fldStr.std_str();

		jvx_decomposePathExpr(descr, lstTokensPath);

		whatToAddress = JVX_REMOTE_CALL_ADDRESS_UNKNOWN;
		myThreadingModel = JVX_CONNECT_THREAD_MULTI_THREAD;

		if (lstTokensPath.size() > 0)
		{
			if (lstTokensPath[0] == JVX_CONNECTION_TAG_SOCKET)
			{
				if (lstTokensPath.size() > 1)
				{
					if (lstTokensPath[1] == "CLIENT")
					{
						if (lstTokensPath.size() > 2)
						{
							if (lstTokensPath[2] == "TCP")
							{
								whatToAddress = JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_TCP;
							}
							if (lstTokensPath[2] == "UDP")
							{
								whatToAddress = JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_UDP;
							}
							if (lstTokensPath[2] == "PCAP")
							{
								whatToAddress = JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_PCAP;
							}
						}
					}
					if (lstTokensPath[1] == "SERVER")
					{
						if (lstTokensPath.size() > 2)
						{
							if (lstTokensPath[2] == "TCP")
							{
								whatToAddress = JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_TCP;
							}
							if (lstTokensPath[2] == "UDP")
							{
								whatToAddress = JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_UDP;
							}
							if (lstTokensPath[2] == "PCAP")
							{
								whatToAddress = JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_PCAP;
							}
						}
					}
				}
			}
			if (lstTokensPath[0] == JVX_CONNECTION_TAG_RS232)
			{
				whatToAddress = JVX_REMOTE_CALL_ADDRESS_RS232;
			}
		}

		myConnection->threading_model(&myThreadingModel);

		//myConnection-> Check connection state!!
		resL = jvx_remote_call_initCfg(&hdlRemoteCall);
		assert(resL == JVX_DSP_NO_ERROR);
		hdlRemoteCall.prmInit.theCallbacks.callback_unlock_receive_buffer = jvx_rc_unlock_receive_buffer;
		hdlRemoteCall.prmInit.theCallbacks.callback_allocate_memory = jvx_rc_allocate_memory;
		hdlRemoteCall.prmInit.theCallbacks.callback_deallocate_memory = jvx_rc_deallocate_memory;
		hdlRemoteCall.prmInit.theCallbacks.callback_accept_init_request = jvx_rc_accept_init_request;
		hdlRemoteCall.prmInit.theCallbacks.callback_send_buffer = jvx_rc_send_buffer;
		hdlRemoteCall.prmInit.theCallbacks.callback_process_request = jvx_rc_process_request;
		hdlRemoteCall.prmInit.theCallbacks.callback_lock = jvx_rc_lock;
		hdlRemoteCall.prmInit.theCallbacks.callback_unlock = jvx_rc_unlock;
		hdlRemoteCall.prmInit.theCallbacks.callback_error_report = jvx_rc_report_local_error;
		hdlRemoteCall.prmInit.theCallbacks.callback_transfer_terminated = jvx_rc_report_transfer_terminated;
		hdlRemoteCall.prmInit.operation_mode = myMode;
		switch (myThreadingModel)
		{
		case JVX_CONNECT_THREAD_SINGLE_THREAD:
			hdlRemoteCall.prmInit.delayedSend = false;
			break;
		case JVX_CONNECT_THREAD_MULTI_THREAD:
			hdlRemoteCall.prmInit.delayedSend = false;
			break;
		default: 
			assert(0);
		}
		hdlRemoteCall.prmInit.cb_priv = reinterpret_cast<jvxHandle*>(this);
		resL = jvx_remote_call_init(&hdlRemoteCall);
		assert(resL == JVX_DSP_NO_ERROR);
		portId = idPortIn;
		res = myConnection->start_port(portId, connectionPrivate, whatsthis, static_cast<IjvxConnection_report*>(this));
		assert(res == JVX_NO_ERROR);
		if (whatToAddress == JVX_REMOTE_CALL_ADDRESS_RS232)
		{
			assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_CONFIG);
			rs232Specific.sc_active = false;
			rs232Specific.startChar = 0;
			rs232Specific.stopChar = 0;

			jvxRs232Config* cfg = (jvxRs232Config*)connectionPrivate;
			if (cfg)
			{
				if (cfg->secureChannel.receive.active)
				{
					rs232Specific.sc_active = true;
					rs232Specific.startChar = cfg->secureChannel.startByte;
					rs232Specific.stopChar = cfg->secureChannel.stopByte;
				}
			}
		}
		/* Note that start_port may send out events immediately. Hence, we need to setup before calling this function */

		// Specify the derrived parameters - default
		derived.tcp.idChannelIn = JVX_SIZE_UNSELECTED;

		myConnection->get_constraints_buffer(portId, &myFrameConstraint.bytesPrepend, &myFrameConstraint.fldMinSize, &myFrameConstraint.fldMaxSize);

		// Prepare input handling
		ram.fld = NULL;
		ram.sz = 0;
		ram.readOffset = 0;
		ram.evaluateOffset = 0;
		stateMachine.theState = JVX_INPUT_STATE_HEADER;
		stateMachine.sc_startScanned = false;
		stateMachine.packetSize = 0;
		if (preAllocateBytes)
		{
			ram.sz = preAllocateBytes;
			JVX_SAFE_NEW_FLD(ram.fld, jvxByte, ram.sz);
		}
		_common_set_min.theState = JVX_STATE_PROCESSING;
	}
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
}

jvxErrorType 
CjvxRemoteCall::connection_state(jvxRCConnectionState* st)
{
	if (st)
	{
		*st = JVX_RC_CONNECTION_STATE_NONE;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxRemoteCall::allocateParameterList(jvxRCOneParameter** lstParams, jvxSize numberParams)
{
	jvxSize i;
	if (lstParams)
	{
		*lstParams = NULL;
		if (numberParams > 0)
		{
			JVX_SAFE_NEW_FLD(*lstParams, jvxRCOneParameter, numberParams);
			for (i = 0; i < numberParams; i++)
			{
				(*lstParams)->content = NULL;
				(*lstParams)->description.seg_length_x = 0;
				(*lstParams)->description.seg_length_y = 0;
				(*lstParams)->description.format = JVX_DATAFORMAT_NONE;
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType 
CjvxRemoteCall::deallocateParameterList(jvxRCOneParameter* lstParams)
{
	if (lstParams)
	{
		JVX_SAFE_DELETE_FLD(lstParams, jvxRCOneParameter);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType 
CjvxRemoteCall::remoteRequest(jvxRCOneParameter* inputParameters, jvxSize numInputParameters,
	jvxRCOneParameter* outputParameters, jvxSize numOutputParameters,
	const char* actionDescriptor, jvxSize actionId, jvxSize timeout_ms,
	jvxErrorType* errCode_otherside)
{
	jvxBool startSend = false;
	int ll = 0;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resLL = JVX_NO_ERROR;
	jvxBool stateOk = false;

	JVX_LOCK_MUTEX(safeAccess);
	if (myChannelStatus == JVX_REMOTE_CALL_CHANNEL_STATUS_AVAILABLE)
	{
		myChannelStatus = JVX_REMOTE_CALL_CHANNEL_STATUS_WAIT_OUTGOING;
		stateOk = true;
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	if (stateOk)
	{
		// Only read the state here, if the state changes afterwards, the socket close event will be reported somewhere
		this->outgoingRequest.fCall_result = JVX_ERROR_ABORT;
		this->outgoingRequest.processId = ((this->globalCount & 0x00FF) | ((rand() & 0xFF) << 8)); // some randomness

		if (this->myThreadingModel == JVX_CONNECT_THREAD_MULTI_THREAD)
		{
			JVX_RESET_NOTIFICATION(this->outgoingRequest.hdlOperationComplete); // Why is this??
			JVX_WAIT_FOR_NOTIFICATION_I(this->outgoingRequest.hdlOperationComplete);

			resL = jvx_remote_call_init_transfer(&hdlRemoteCall, actionDescriptor, inputParameters, numInputParameters, outputParameters, numOutputParameters, outgoingRequest.processId, 0);
			if (resL != JVX_DSP_NO_ERROR)
			{
				JVX_LOCK_MUTEX(safeAccess);
				myChannelStatus = JVX_REMOTE_CALL_CHANNEL_STATUS_AVAILABLE;
				JVX_UNLOCK_MUTEX(safeAccess);
				return(JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
			}
			// Now wait for the remote call to complete
			JVX_WAIT_RESULT resW = JVX_WAIT_SUCCESS;
			if (JVX_CHECK_SIZE_UNSELECTED(timeout_ms))
			{
				resW = JVX_WAIT_FOR_NOTIFICATION_II_INF(this->outgoingRequest.hdlOperationComplete);
			}
			else
			{
				resW = JVX_WAIT_FOR_NOTIFICATION_II_MS(this->outgoingRequest.hdlOperationComplete, timeout_ms);
			}
			if (resW == JVX_WAIT_SUCCESS)
			{
				if (errCode_otherside)
				{
					*errCode_otherside = this->outgoingRequest.fCall_result;
				}
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_TIMEOUT;
				jvx_remote_call_reset(&hdlRemoteCall);
			}// if(this->stateConnection == rtpRemoteCall::CONNECTION_STATE_CONNECTED)
			
			JVX_LOCK_MUTEX(safeAccess);
			myChannelStatus = JVX_REMOTE_CALL_CHANNEL_STATUS_AVAILABLE;
			JVX_UNLOCK_MUTEX(safeAccess);
		}
		else
		{
			resL = jvx_remote_call_init_transfer(&hdlRemoteCall, actionDescriptor, inputParameters, numInputParameters, outputParameters, numOutputParameters, outgoingRequest.processId, 0);
			if (resL == JVX_DSP_NO_ERROR)
			{
				jvxSize szFldInput = 0;
				singleThread.completed = false;

				while (1)
				{					
					resLL = myConnection->scanMessage_port(portId, &szFldInput);
					if (ram.sz < szFldInput)
					{
						if (ram.sz)
						{
							JVX_DSP_SAFE_DELETE_FIELD(ram.fld);
							ram.fld = NULL;
							ram.sz = 0;
						}
						assert(szFldInput > 0);
						JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ram.fld, jvxByte, szFldInput);
						ram.sz = szFldInput;
					}

					szFldInput = ram.sz;
					resLL = myConnection->readMessage_port(portId, ram.fld, &szFldInput, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
					if (resLL == JVX_NO_ERROR)
					{
						resL = jvx_remote_call_incoming(&hdlRemoteCall, (jvxProtocolHeader*)(ram.fld + myFrameConstraint.bytesPrepend),
							outgoingRequest.processId, NULL, 0);
						if (resL == JVX_DSP_NO_ERROR)
						{
							// Check end of transfer
							if (singleThread.completed)
							{
								// This final step is required to free the channel again
								myChannelStatus = JVX_REMOTE_CALL_CHANNEL_STATUS_AVAILABLE;

								if (errCode_otherside)
								{
									*errCode_otherside = this->outgoingRequest.fCall_result;
								}
								break;
							}
						}
					}
					else
					{
						assert(0);					
					}
				}
			}						
		}
	}
	else
	{
		res = JVX_ERROR_COMPONENT_BUSY;
	}
	return res;
}

jvxErrorType 
CjvxRemoteCall::stop()
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		myConnection->stop_port(portId);
		resL = jvx_remote_call_terminate(&hdlRemoteCall);
		assert(resL == JVX_DSP_NO_ERROR);

		if (ram.sz)
		{
			JVX_SAFE_DELETE_FLD(ram.fld, jvxByte);
			ram.sz = 0;
		}

		_common_set_min.theState = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}

	//myConnection-> Check connection state!!
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxRemoteCall::terminate()
{
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		stop();
	}
	
	if (_common_set_min.theState == JVX_STATE_INIT)
	{
		CjvxObject::_terminate();
	}

	return(JVX_NO_ERROR);
}

// =========================================================================
// =========================================================================

jvxErrorType
CjvxRemoteCall::requestPendingError(char* buf, jvxSize lField, jvxErrorType* tp)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxRemoteCall::requestPendingEvent(jvxRCEventType* tp)
{
	return JVX_ERROR_UNSUPPORTED;
}
jvxErrorType
CjvxRemoteCall::lockPendingAction()
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxRemoteCall::isLockedPendingAction(jvxBool* yes_itslocked)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxRemoteCall::requestPendingAction(jvxBool* yes_thereis)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxRemoteCall::parameterDescriptorsPendingRequest(jvxSize* numInputParams, jvxSize* numOutputParams, char* fldActionDescription, jvxSize fldLength, jvxSize* processId)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxRemoteCall::parametersPendingRequest(jvxRCOneParameter* inParams, jvxSize numInConts, jvxRCOneParameter* outParams, jvxSize numOutConts)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxRemoteCall::reportCompletePendingAction(jvxErrorType success)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxRemoteCall::unlockPendingAction()
{
	return JVX_ERROR_UNSUPPORTED;
}
