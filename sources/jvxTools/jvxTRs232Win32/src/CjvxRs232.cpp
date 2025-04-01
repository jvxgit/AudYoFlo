#include "jvx.h"
#include "CjvxRs232.h"
#include <iostream>

#define JVX_RS232_OUTPUT_BEFORE_SEND
#define JVX_RS232_OUTPUT_AFTER_RECEIVE
 
JVX_THREAD_ENTRY_FUNCTION(thread_receive, param)
{
	assert(param);
	{
		threadEntryStruct* enter = (threadEntryStruct*) param;
		enter->this_pointer->receiveLoop(JVX_SIZE_INT(enter->portId));
	}
	return(0);
}

CjvxRs232::CjvxRs232(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_CONNECTION);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxConnection*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theComponentSubTypeDescriptor = "/";
	_common_set.theComponentSubTypeDescriptor += JVX_CONNECTION_TAG_RS232;

	_common_set_min.theDescription = "JVX RS232 connection module";
	_system.refCount = 0;

	JVX_INITIALIZE_MUTEX(_system.safeAccess);
}

CjvxRs232::~CjvxRs232()
{
	// If we reach here, all ports MUST be closed
	this->terminate();
	CjvxObject::_terminate();

	JVX_TERMINATE_MUTEX(_system.safeAccess);
}

jvxErrorType 
CjvxRs232::initialize(IjvxHiddenInterface* hostRef, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)
{	
	int id = 0;
	jvxSize jj;
	bool thereIsAProblem = false;

#ifdef JVX_OS_WINDOWS
	
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_system.safeAccess);

	res = _initialize(hostRef);
	res = _select();
	res = _activate();
	// if(_common_set_min.theState == JVX_STATE_NONE)
	if(res == JVX_NO_ERROR)
	{
		jvxSize numComPortsPoll = POLL_COM_PORTS_MAX_ID;

		if (whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_CONNECTION_NUM_PORT)
		{
			numComPortsPoll = *((jvxSize*)priv);
		}
		// This method tries to open any com port in the range between 0 and 
		for(jj = 0; jj < numComPortsPoll; jj++)
		{
			std::string nmComPort = "\\\\.\\COM" + jvx_size2String(jj);

			JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG)
			log << "Testing rs-232 port <" << nmComPort << ">" << std::endl;
			JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG)

			HANDLE hdl  = CreateFile(nmComPort.c_str(),
				GENERIC_WRITE|GENERIC_READ,
				0, NULL, OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED, NULL);
			if(hdl != INVALID_HANDLE_VALUE)
			{
				oneRs232Port elm;
				elm.init.friendlyName = "COM" + jvx_size2String(jj);
				elm.init.description = elm.init.friendlyName;
				elm.init.shortcut = elm.init.friendlyName;
				elm.init.id = id++;
				elm.init.location = "unknown";
				elm.theState = JVX_STATE_INIT;

				jvx_rs232_initRs232Default(&elm.runtime.cfg);

				CloseHandle(hdl);
				elm.runtime.hdl = JVX_INVALID_HANDLE_VALUE;
				elm.runtime.theReport = NULL;
				JVX_INITIALIZE_MUTEX(elm.safeAccessConnection);

#ifdef JVX_RS232_DEBUG
				JVX_INITIALIZE_MUTEX(elm.dbg.safeAccess);
				elm.dbg.fn = "";
#endif

				JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG)
				log << "-> Rs-232 port <" << nmComPort << "> is ready." << std::endl;
				JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG)

				this->_system.thePorts.push_back(elm);
			}
		}
		// _common_set_min.theState = JVX_STATE_INIT;
		_system.refCount = 1;
	}
	else
	{
		_system.refCount++;
	}
	JVX_UNLOCK_MUTEX(_system.safeAccess);
	return(res);
#else
	ERROR
#endif
}

jvxErrorType 
CjvxRs232::availablePorts(jvxApiStringList* allPorts)
{
	int i;
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_system.safeAccess);
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		std::vector<std::string> lst;
		for(i = 0; i < _system.thePorts.size(); i++)
		{
			lst.push_back(_system.thePorts[i].init.shortcut);
		}

		if (allPorts)
		{
			allPorts->assign(lst);
		}
		res = JVX_NO_ERROR;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(_system.safeAccess);
	return(res);
}

// =============================================================
// Port specific commands
// =============================================================

jvxErrorType 
CjvxRs232::state_port(jvxSize idPort, jvxState* theState)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	JVX_LOCK_MUTEX(_system.safeAccess);
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		if(idPort < _system.thePorts.size())
		{
			if(theState)
			{
				*theState = _system.thePorts[idPort].theState;
			}
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(_system.safeAccess);
	return(res);
}

jvxErrorType 
CjvxRs232::start_port(jvxSize idPort, jvxHandle* cfgIn, jvxConnectionPrivateTypeEnum whatsthis, IjvxConnection_report* theReport)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_system.safeAccess);
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		jvxRs232Config* cfg = (jvxRs232Config*)cfgIn;
		assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_CONFIG);
		if(idPort < _system.thePorts.size())
		{
			JVX_LOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
			if(_system.thePorts[idPort].theState == JVX_STATE_INIT)
			{
				// Open COM port
				_system.thePorts[idPort].runtime.hdl  = CreateFile(("\\\\.\\" + _system.thePorts[idPort].init.shortcut).c_str(),
					GENERIC_WRITE|GENERIC_READ,
					0, NULL, OPEN_EXISTING,
					FILE_FLAG_OVERLAPPED, NULL);

				if (_system.thePorts[idPort].runtime.hdl != INVALID_HANDLE_VALUE)
				{
					// Successful start of RS-232 device!

					_system.thePorts[idPort].runtime.theReport = theReport;

					// Copy configuration
					_system.thePorts[idPort].runtime.cfg = *cfg;

					DCB rs232SetupElement;
					rs232SetupElement.DCBlength = sizeof(DCB);

					GetCommState(_system.thePorts[idPort].runtime.hdl, &rs232SetupElement);
					rs232SetupElement.BaudRate = cfg->bRate;

					rs232SetupElement.ByteSize = cfg->bits4Byte;
					switch (cfg->stopBitsEnum)
					{
					case JVX_RS232_STOPBITS_ONE:
						rs232SetupElement.StopBits = ONESTOPBIT;
						break;
					case JVX_RS232_STOPBITS_ONE5:
						rs232SetupElement.StopBits = ONE5STOPBITS;
						break;
					default: // case JVX_RS232_STOPBITS_TWO:
						rs232SetupElement.StopBits = TWOSTOPBITS;
						break;
					}

					switch (cfg->parityEnum)
					{
					case JVX_RS232_PARITY_EVEN:
						rs232SetupElement.fParity = EVENPARITY;
						break;
					case JVX_RS232_PARITY_MARK:
						rs232SetupElement.fParity = MARKPARITY;
						break;
					case JVX_RS232_PARITY_ODD:
						rs232SetupElement.fParity = ODDPARITY;
						break;
					case JVX_RS232_PARITY_NO:
						rs232SetupElement.fParity = NOPARITY;
						break;
					default: // case JVX_RS232_PARITY_SPACE:
						rs232SetupElement.fParity = SPACEPARITY;
						break;
					}

					//Setup the flow control 
					rs232SetupElement.fDsrSensitivity = 0;
					rs232SetupElement.fDtrControl = DTR_CONTROL_ENABLE;

					/*
					Code taken from putty and aligned 02.08.2024:

						if (GetCommState(serport, &dcb)) {
						const char *str;

						/ *
						 * Boilerplate.
						 * /
						dcb.fBinary = true;
						dcb.fDtrControl = DTR_CONTROL_ENABLE;
						dcb.fDsrSensitivity = false;
						dcb.fTXContinueOnXoff = false;
						dcb.fOutX = false;
						dcb.fInX = false;
						dcb.fErrorChar = false;
						dcb.fNull = false;
						dcb.fRtsControl = RTS_CONTROL_ENABLE;
						dcb.fAbortOnError = false;
						dcb.fOutxCtsFlow = false;
						dcb.fOutxDsrFlow = false;
		
						/ *
						  * Configurable parameters.
						  * /
					dcb.BaudRate = conf_get_int(conf, CONF_serspeed);
					logeventf(serial->logctx, "Configuring baud rate %lu",
						(unsigned long)dcb.BaudRate);

					dcb.ByteSize = conf_get_int(conf, CONF_serdatabits);
					logeventf(serial->logctx, "Configuring %u data bits",
						(unsigned)dcb.ByteSize);

					switch (conf_get_int(conf, CONF_serstopbits)) {
					case 2: dcb.StopBits = ONESTOPBIT; str = "1 stop bit"; break;
					case 3: dcb.StopBits = ONE5STOPBITS; str = "1.5 stop bits"; break;
					case 4: dcb.StopBits = TWOSTOPBITS; str = "2 stop bits"; break;
					default: return dupstr("Invalid number of stop bits "
						"(need 1, 1.5 or 2)");
					}
					logeventf(serial->logctx, "Configuring %s", str);

					switch (conf_get_int(conf, CONF_serparity)) {
					case SER_PAR_NONE: dcb.Parity = NOPARITY; str = "no"; break;
					case SER_PAR_ODD: dcb.Parity = ODDPARITY; str = "odd"; break;
					case SER_PAR_EVEN: dcb.Parity = EVENPARITY; str = "even"; break;
					case SER_PAR_MARK: dcb.Parity = MARKPARITY; str = "mark"; break;
					case SER_PAR_SPACE: dcb.Parity = SPACEPARITY; str = "space"; break;
					}
					logeventf(serial->logctx, "Configuring %s parity", str);

					switch (conf_get_int(conf, CONF_serflow)) {
					case SER_FLOW_NONE:
						str = "no";
						break;
					case SER_FLOW_XONXOFF:
						dcb.fOutX = dcb.fInX = true;
						str = "XON/XOFF";
						break;
					case SER_FLOW_RTSCTS:
						dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
						dcb.fOutxCtsFlow = true;
						str = "RTS/CTS";
						break;
					case SER_FLOW_DSRDTR:
						dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
						dcb.fOutxDsrFlow = true;
						str = "DSR/DTR";
						break;
					}
					logeventf(serial->logctx, "Configuring %s flow control", str);

					if (!SetCommState(serport, &dcb))
						return dupprintf("Configuring serial port: %s",
							win_strerror(GetLastError()));

					 */


					switch (cfg->enFlow)
					{
					case JVX_RS232_NO_FLOWCONTROL:
						rs232SetupElement.fOutxCtsFlow = FALSE;
						rs232SetupElement.fOutxDsrFlow = FALSE;
						rs232SetupElement.fOutX = FALSE;
						rs232SetupElement.fInX = FALSE;
						break;
					case JVX_RS232_CTSRTSFLOWCONTROL:
						rs232SetupElement.fOutxCtsFlow = TRUE;
						rs232SetupElement.fOutxDsrFlow = FALSE;
						rs232SetupElement.fRtsControl = RTS_CONTROL_HANDSHAKE;
						rs232SetupElement.fOutX = FALSE;
						rs232SetupElement.fInX = FALSE;
						break;
					case JVX_RS232_CTSDTRFLOWCONTROL:
						rs232SetupElement.fOutxCtsFlow = TRUE;
						rs232SetupElement.fOutxDsrFlow = FALSE;
						rs232SetupElement.fDtrControl = DTR_CONTROL_HANDSHAKE;
						rs232SetupElement.fOutX = FALSE;
						rs232SetupElement.fInX = FALSE;
						break;
					case JVX_RS232_DSRRTSFLOWCONTROL:
						rs232SetupElement.fOutxCtsFlow = FALSE;
						rs232SetupElement.fOutxDsrFlow = TRUE;
						rs232SetupElement.fRtsControl = RTS_CONTROL_HANDSHAKE;
						rs232SetupElement.fOutX = FALSE;
						rs232SetupElement.fInX = FALSE;
						break;
					case JVX_RS232_DSRDTRFLOWCONTROL:
						rs232SetupElement.fOutxCtsFlow = FALSE;
						rs232SetupElement.fOutxDsrFlow = TRUE;
						rs232SetupElement.fDtrControl = DTR_CONTROL_HANDSHAKE;
						rs232SetupElement.fOutX = FALSE;
						rs232SetupElement.fInX = FALSE;
						break;
					default: // case JVX_RS232_XONXOFFFLOWCONTROL:
						rs232SetupElement.fOutxCtsFlow = FALSE;
						rs232SetupElement.fOutxDsrFlow = FALSE;
						rs232SetupElement.fOutX = TRUE;
						rs232SetupElement.fInX = TRUE;
						/*
						rs232SetupElement.XonChar = 0x11;
						rs232SetupElement.XoffChar = 0x13;
						rs232SetupElement.XoffLim = 100;
						rs232SetupElement.XonLim = 100;
						*/
						break;
					}

					BOOL resCom = SetCommState(_system.thePorts[idPort].runtime.hdl, &rs232SetupElement);

					// Set port configurations
					assert(resCom == TRUE);
					//					DWORD sysError = GetLastError();

					_system.thePorts[idPort].runtime.sc.transmit.szfld = 0;
					_system.thePorts[idPort].runtime.sc.transmit.fld = NULL;

					if (_system.thePorts[idPort].runtime.cfg.secureChannel.transmit.active)
					{
						if (JVX_CHECK_SIZE_SELECTED(_system.thePorts[idPort].runtime.cfg.secureChannel.transmit.preallocationSize))
						{
							if (_system.thePorts[idPort].runtime.cfg.secureChannel.transmit.preallocationSize > 0)
							{
								_system.thePorts[idPort].runtime.sc.transmit.szfld = _system.thePorts[idPort].runtime.cfg.secureChannel.transmit.preallocationSize;
								JVX_SAFE_NEW_FLD(_system.thePorts[idPort].runtime.sc.transmit.fld, jvxByte, _system.thePorts[idPort].runtime.sc.transmit.szfld);
								memset(_system.thePorts[idPort].runtime.sc.transmit.fld, 0, sizeof(jvxByte) *  _system.thePorts[idPort].runtime.sc.transmit.szfld);
							}
						}
					}

					if (_system.thePorts[idPort].runtime.cfg.secureChannel.receive.active)
					{
						_system.thePorts[idPort].runtime.sc.receive.szfld = _system.thePorts[idPort].runtime.cfg.secureChannel.receive.maxPreReadSize;
						assert(_system.thePorts[idPort].runtime.sc.receive.szfld);
						JVX_SAFE_NEW_FLD(_system.thePorts[idPort].runtime.sc.receive.fld, jvxByte, _system.thePorts[idPort].runtime.sc.receive.szfld);
						memset(_system.thePorts[idPort].runtime.sc.receive.fld, 0, sizeof(jvxByte)* _system.thePorts[idPort].runtime.sc.receive.szfld);
						_system.thePorts[idPort].runtime.sc.receive.offset_read = 0;
						_system.thePorts[idPort].runtime.sc.receive.offset_write = 0;
					}

					_system.thePorts[idPort].threads.inLoopActive = true;
					_system.thePorts[idPort].threads.enter.this_pointer = this;
					_system.thePorts[idPort].threads.enter.portId = idPort;

#ifdef JVX_RS232_DEBUG
					JVX_GET_TICKCOUNT_US_SETREF(_system.thePorts[idPort].dbg.tRef);
					startprintout(idPort);
#endif
					JVX_CREATE_THREAD(_system.thePorts[idPort].threads.hdl, thread_receive, &_system.thePorts[idPort].threads.enter, _system.thePorts[idPort].threads.id);
					if (cfg->boostPrio)
					{
						JVX_SET_THREAD_PRIORITY(_system.thePorts[idPort].threads.hdl, JVX_THREAD_PRIORITY_REALTIME);
					}

					// Wait for thread to come up
					JVX_SLEEP_MS(200);

					_system.thePorts[idPort].theState = JVX_STATE_PROCESSING;
					res = JVX_NO_ERROR;
				}
				else
				{
					res = JVX_ERROR_COMPONENT_BUSY;
				}
			}
			else
			{
				res = JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
			JVX_UNLOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(_system.safeAccess);
	return res;
}

jvxErrorType
CjvxRs232::get_constraints_buffer(jvxSize idPort, jvxSize* bytesPrepend, jvxSize* fldMinSize, jvxSize* fldMaxSize)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_system.safeAccess);
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		if(idPort < _system.thePorts.size())
		{
			if(_system.thePorts[idPort].theState == JVX_STATE_PROCESSING)
			{
				if(bytesPrepend)
				{
					*bytesPrepend = 0;
				}
				if(fldMinSize)
				{
					*fldMinSize = JVX_SIZE_UNSELECTED;
				}
				if(fldMaxSize)
				{
					*fldMaxSize = JVX_SIZE_UNSELECTED;
				}
			}
			else
			{
				res = JVX_ERROR_WRONG_STATE;
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(_system.safeAccess);
	return(res);
}

jvxErrorType
CjvxRs232::sendMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld, jvxHandle* priv_send, jvxConnectionPrivateTypeEnum whatsthis)
{
	DWORD written = 0;
	OVERLAPPED osSend = {0};
	JVX_NOTIFY_HANDLE eventSendOL;
	JVX_INITIALIZE_NOTIFICATION(eventSendOL);
	osSend.hEvent = eventSendOL;
	DWORD resB;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize szBytesWrap = 0;
	jvxSize* szFldPtr = szFld;
	jvxSize writtenSC = 0;

	JVX_LOCK_MUTEX(_system.safeAccess);
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		if(idPort < _system.thePorts.size())
		{
			JVX_LOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
			if(_system.thePorts[idPort].theState == JVX_STATE_PROCESSING)
			{
				if (_system.thePorts[idPort].runtime.cfg.secureChannel.transmit.active)
				{
					szBytesWrap = numberBytesWrapSend(fld, *szFld, _system.thePorts[idPort].runtime.cfg.secureChannel.startByte, 
						_system.thePorts[idPort].runtime.cfg.secureChannel.stopByte, 
						_system.thePorts[idPort].runtime.cfg.secureChannel.escByte);
					if (_system.thePorts[idPort].runtime.sc.transmit.szfld < szBytesWrap)
					{
						if (_system.thePorts[idPort].runtime.sc.transmit.fld)
						{
							JVX_SAFE_DELETE_FLD(_system.thePorts[idPort].runtime.sc.transmit.fld, jvxByte);
							_system.thePorts[idPort].runtime.sc.transmit.fld = NULL;
							_system.thePorts[idPort].runtime.sc.transmit.szfld = 0;
						}
						_system.thePorts[idPort].runtime.sc.transmit.szfld = szBytesWrap;
						JVX_SAFE_NEW_FLD(_system.thePorts[idPort].runtime.sc.transmit.fld, jvxByte, _system.thePorts[idPort].runtime.sc.transmit.szfld);
						memset(_system.thePorts[idPort].runtime.sc.transmit.fld, 0, sizeof(jvxByte) *_system.thePorts[idPort].runtime.sc.transmit.szfld);
					}
					insertByteWrapReplace(fld, *szFld, _system.thePorts[idPort].runtime.sc.transmit.fld, 
						_system.thePorts[idPort].runtime.sc.transmit.szfld, 
						_system.thePorts[idPort].runtime.cfg.secureChannel.startByte,
						_system.thePorts[idPort].runtime.cfg.secureChannel.stopByte, 
						_system.thePorts[idPort].runtime.cfg.secureChannel.escByte,
						writtenSC);
					assert(writtenSC == szBytesWrap);
					fld = _system.thePorts[idPort].runtime.sc.transmit.fld;
					szFldPtr = &szBytesWrap;
				}

				JVX_WAIT_FOR_NOTIFICATION_I(eventSendOL);

#ifdef JVX_RS232_DEBUG
				printoutBuffer(idPort, fld, *szFldPtr, _system.thePorts[idPort].runtime.cfg.secureChannel.startByte, _system.thePorts[idPort].runtime.cfg.secureChannel.stopByte, _system.thePorts[idPort].runtime.cfg.secureChannel.escByte, "send");
#endif
				if(!WriteFile(_system.thePorts[idPort].runtime.hdl, fld, (DWORD)*szFldPtr, &written, &osSend))
				{
					resB = JVX_WAIT_FOR_NOTIFICATION_II_INF(eventSendOL);
					assert(resB == WAIT_OBJECT_0);
					GetOverlappedResult(_system.thePorts[idPort].runtime.hdl, &osSend, &written, TRUE);
				}
#ifdef JVX_RS232_DEBUG
				printoutText(idPort, "Send complete");
#endif

				if (_system.thePorts[idPort].runtime.cfg.secureChannel.transmit.active)
				{
					if (written < szBytesWrap)
					{
						// Indicate that not all bytes were send..
						*szFld = 0;
					}
				}
				else
				{
					*szFld = written;
				}				
			}
			else
			{
				res = JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
			JVX_UNLOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(_system.safeAccess);
	return(res);
}

jvxSize 
CjvxRs232::numberBytesWrapSend(jvxByte* fld, jvxSize szFld, jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc)
{
	jvxSize i;
	jvxSize numBytes = 2; // Start and stop
	for (i = 0; i < szFld; i++)
	{
		if (
			(fld[i] == byteStart) ||
			(fld[i] == byteStop) ||
			(fld[i] == byteEsc))
		{
			numBytes ++;
		}
	}
	numBytes += szFld;
	return numBytes;
}


void
CjvxRs232::insertByteWrapReplace(jvxByte* copyForm, jvxSize szFldFrom, jvxByte* copyTo, jvxSize szFldTo, jvxByte byteStart, 
			jvxByte byteStop, jvxByte byteEsc, jvxSize& written)
{
	jvxSize i;
	jvxByte* ptrw = copyTo;
	
	written = 0;
	jvxByte in; 

	*ptrw = byteStart;
	ptrw++;
	written++;

	for (i = 0; i < szFldFrom; i++)
	{
		in = copyForm[i];
		if (
			(in == byteStart) ||
			(in == byteStop) ||
			(in == byteEsc))
		{
			*ptrw = byteEsc;
			ptrw++;
			written++;
		}
		*ptrw = in;
		ptrw++;
		written++;
	}
	*ptrw = byteStop;
	ptrw++;
	written++;
}

jvxSize 
CjvxRs232::numberBytesUnwrapReceive(jvxByte* fld, jvxSize offset_read, jvxSize offset_write, jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc, jvxByte& theChar)
{
	jvxSize i;
	jvxSize numRet = 0;
	jvxBool precededByEsc = false;
	for (i = offset_read; i < offset_write; i++)
	{
		jvxByte in = fld[i];
		if (in == byteEsc)
		{
			if (precededByEsc)
			{
				numRet++;
				precededByEsc = false;
			}
			else
			{
				precededByEsc = true;
			}
		}
		else if (
			(in == byteStart) ||
			(in == byteStop))
		{
			if (precededByEsc)
			{
				numRet++;
				precededByEsc = false;
			}
			else
			{
				theChar = in;
				break;
			}
		}
		else
		{
			numRet++;
		}
	}
	return(numRet);
}

void 
CjvxRs232::removeByteWrapReplace(jvxByte* copyForm, jvxSize offset_from_read, jvxSize  offset_from_write,
	jvxByte* copyTo, jvxSize szFldTo,
	jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc,
	jvxSize & read, jvxSize & write, jvxByte& theStopChar)
{
	jvxSize i;
	theStopChar = 0;
	read = 0;
	write = 0;
	jvxBool precededByEsc = false;
	for (i = offset_from_read; i < offset_from_write; i++)
	{
		jvxByte in = copyForm[i];
		read++;
		if (in == byteEsc)
		{
			if (precededByEsc)
			{
				copyTo[write] = in;
				write++;
				precededByEsc = false;
			}
			else
			{
				if (i == offset_from_write - 1)
				{
					// Esc at end to be rejected
					read--;
					break;
				}
				else
				{
					precededByEsc = true;
				}
			}
		}
		else if (
			(in == byteStart) ||
			(in == byteStop))
		{
			if (precededByEsc)
			{
				copyTo[write] = in;
				write++;
				precededByEsc = false;
			}
			else
			{
				theStopChar = in;
				break;
			}
		}
		else
		{
			copyTo[write] = in;
			write++;
		}
	}
}


jvxErrorType 
CjvxRs232::stop_port(jvxSize idPort)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_system.safeAccess);
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		if(idPort < _system.thePorts.size())
		{
			JVX_LOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
			if(_system.thePorts[idPort].theState == JVX_STATE_PROCESSING)
			{
				_system.thePorts[idPort].threads.inLoopActive = false;
				_system.thePorts[idPort].theState = JVX_STATE_COMPLETE;
				JVX_UNLOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);

				// RS232 connection is in use, close port at first
				CloseHandle(_system.thePorts[idPort].runtime.hdl);

				// Wait for thread to terminate :: MUST BE OUTSIDE MUTEX!!!
				JVX_WAIT_FOR_THREAD_TERMINATE_INF(_system.thePorts[idPort].threads.hdl);

				assert(_system.thePorts[idPort].theState == JVX_STATE_INIT);

#ifdef JVX_RS232_DEBUG
				stoprintout(idPort);
#endif

				_system.thePorts[idPort].threads.hdl = INVALID_HANDLE_VALUE;
				if (_system.thePorts[idPort].runtime.sc.transmit.fld)
				{
					JVX_SAFE_DELETE_FLD(_system.thePorts[idPort].runtime.sc.transmit.fld, jvxByte);
					_system.thePorts[idPort].runtime.sc.transmit.fld = NULL;
				}
				
			}
			else
			{
				JVX_UNLOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
				res = JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(_system.safeAccess);
	return res;
}

// =============================================================

jvxErrorType
CjvxRs232::control_port(jvxSize idPort, jvxSize operation_id, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;
	DWORD modemState = 0;
	jvxRs232ControlCommands contrType = (jvxRs232ControlCommands) operation_id;
	jvxUInt16* retVal = NULL;

	JVX_LOCK_MUTEX(_system.safeAccess);
	if (_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		if (idPort < _system.thePorts.size())
		{
			JVX_LOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
			if (_system.thePorts[idPort].theState == JVX_STATE_PROCESSING)
			{
				switch (contrType)
				{
				case JVX_RS232_CONTROL_GET_MODEM_STATUS:
					
					if (priv)
					{
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_MODEM_STATUS);
						retVal = (jvxUInt16*)priv;
						res = JVX_NO_ERROR;
						if (!GetCommModemStatus(_system.thePorts[idPort].runtime.hdl, &modemState))
						{
							assert(0);
						}
						if (retVal)
						{
							*retVal = 0;

							if (modemState & MS_RING_ON)
							{
								*retVal = 1 << JVX_RS232_STATUS_RING_SHFT;
							}

							if (modemState & MS_CTS_ON)
							{
								*retVal = 1 << JVX_RS232_STATUS_CTS_SHFT;
							}

							if (modemState & MS_RLSD_ON)
							{
								*retVal = 1 << JVX_RS232_STATUS_RLSD_SHFT;
							}

							if (modemState & MS_DSR_ON)
							{
								*retVal = 1 << JVX_RS232_STATUS_DSR_SHFT;
							}
						}

						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
					break;

				default:
					res = JVX_ERROR_UNSUPPORTED;
				}
			}
			else
			{
				res = JVX_ERROR_WRONG_STATE;
			}
			JVX_UNLOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(_system.safeAccess);

	return(res);
}

// =============================================================

jvxErrorType 
CjvxRs232::terminate()
{
	int i;

	JVX_LOCK_MUTEX(_system.safeAccess);
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		jvxErrorType res = _deactivate();
		res = _unselect();

		if(_system.refCount == 1)
		{
			for(i = 0; i < _system.thePorts.size(); i++)
			{
				this->stop_port(i);
				JVX_TERMINATE_MUTEX(_system.thePorts[i].safeAccessConnection);
#ifdef JVX_RS232_DEBUG
				JVX_TERMINATE_MUTEX(_system.thePorts[i].dbg.safeAccess);
#endif
			}
			_system.thePorts.clear();
			CjvxObject::_terminate(); // This modifies the state to NONE
			//_common_set_min.theState = JVX_STATE_NONE;
		}
		_system.refCount--;
	}
	JVX_UNLOCK_MUTEX(_system.safeAccess);
	return(JVX_NO_ERROR);
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void
CjvxRs232::receiveLoop(int idPort)
{
	DWORD read = 0;
	bool resRead;

	// Create the overlapped events
	OVERLAPPED osObservate = {0};
	HANDLE eventObserve = CreateEvent(0, true, false, NULL);
	
	OVERLAPPED osRead = {0};
	HANDLE eventRead = CreateEvent(0, true, false, NULL);

	DWORD evtMask;
	
	unsigned ind;

	int valuesInQueue;
	DWORD errorWord;
	COMSTAT staErr;
	
	jvxSize sz = 0;
	jvxSize numReadApp = 0;
	jvxByte* ptrRead = NULL;
	jvxSize offset = 0;
	jvxSize write = 0;
	jvxByte theSpecialChar = 0;
	jvxSize szread;

	osObservate.hEvent = eventObserve;
	osRead.hEvent = eventRead;

	// Expect indications for all handshake control and arriving bytes
	DWORD mask = EV_RXCHAR;
	if(jvx_bitTest(_system.thePorts[idPort].runtime.cfg.reportEnum, JVX_RS232_REPORT_RING_SHFT))
	{
		mask |= EV_RING;
	}
	if(jvx_bitTest(_system.thePorts[idPort].runtime.cfg.reportEnum, JVX_RS232_REPORT_CTS_SHFT))
	{
		mask |= EV_CTS;
	}
	if(jvx_bitTest(_system.thePorts[idPort].runtime.cfg.reportEnum, JVX_RS232_REPORT_RLSD_SHFT))
	{
		mask |= EV_RLSD;
	}
	if(jvx_bitTest(_system.thePorts[idPort].runtime.cfg.reportEnum, JVX_RS232_REPORT_DSR_SHFT))
	{
		mask |= EV_DSR;
	}

	SetCommMask(_system.thePorts[idPort].runtime.hdl, mask);

	if (_system.thePorts[idPort].runtime.theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_CONNECTION_REPORT_STARTUP_COMPLETE_SHFT);
		_system.thePorts[idPort].runtime.theReport->report_event(mask, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
	}

	if (!jvx_bitTest(_system.thePorts[idPort].runtime.cfg.reportEnum, JVX_RS232_REPORT_CTS_SHFT))
	{
		if (jvx_bitTest(_system.thePorts[idPort].runtime.cfg.reportEnum, JVX_RS232_REPORT_CONNECTION_ESTABLISHED_SHIFT))
		{
			if (_system.thePorts[idPort].runtime.theReport)
			{
				jvxBitField mask;
				jvx_bitSet(mask, JVX_RS232_REPORT_CONNECTION_ESTABLISHED_SHIFT);
				_system.thePorts[idPort].runtime.theReport->report_event(mask, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
			}
		}
	}

	while(_system.thePorts[idPort].threads.inLoopActive)
	{
		evtMask = 0;

		// Do not block critical section
		if(!WaitCommEvent(_system.thePorts[idPort].runtime.hdl, &evtMask, &osObservate))
		{
			WaitForSingleObject(osObservate.hEvent, INFINITE);
		}

		/*
		 * If handle is closed, event is triggered even if event mask is 0. This function
		 * will end in this case by falling through all if statements since loop bool is false
		 */

		// Now process the single event..
		if(evtMask & EV_RXCHAR)
		{
			resRead = true;
			ind = 0;

			// Obtain the input FIFO status: How many bytes are ready to be read?
			ClearCommError(_system.thePorts[idPort].runtime.hdl, &errorWord, &staErr);

			// What to expect here?
			switch (errorWord)
			{
			case CE_BREAK:
				_report_text_message(
					("Break Error Condition reported by RS232 device on " + _system.thePorts[idPort].init.friendlyName).c_str(),
					JVX_REPORT_PRIORITY_WARNING);
				break;
			case CE_FRAME:
				_report_text_message(
					("Frame Error Condition reported by RS232 device on " + _system.thePorts[idPort].init.friendlyName).c_str(),
					JVX_REPORT_PRIORITY_WARNING);
				break;
			case CE_IOE:
				_report_text_message(
					("I/O Error Condition reported by RS232 device on " + _system.thePorts[idPort].init.friendlyName).c_str(),
					JVX_REPORT_PRIORITY_WARNING);
				break;
			case CE_MODE:
				_report_text_message(
					("Mode Error Condition reported by RS232 device on " + _system.thePorts[idPort].init.friendlyName).c_str(),
					JVX_REPORT_PRIORITY_WARNING);
				break;
			case CE_OVERRUN:
				_report_text_message(
					("Overrun Error Condition reported by RS232 device on " + _system.thePorts[idPort].init.friendlyName).c_str(),
					JVX_REPORT_PRIORITY_WARNING);
				break;
			case CE_RXPARITY:
				_report_text_message(
					("Mode Error Condition reported by RS232 device on " + _system.thePorts[idPort].init.friendlyName).c_str(),
					JVX_REPORT_PRIORITY_WARNING);
				break;
			case CE_TXFULL:
				_report_text_message(
					("TX Full Error Condition reported by RS232 device on " + _system.thePorts[idPort].init.friendlyName).c_str(),
					JVX_REPORT_PRIORITY_WARNING);
				break;
			default:
				break;
			}

			// No error, hence, only return 
			valuesInQueue = staErr.cbInQue;			
			
			while (valuesInQueue > 0)
			{
				if (_system.thePorts[idPort].runtime.cfg.secureChannel.receive.active)
				{
					jvxSize sz = valuesInQueue;
					sz = JVX_MIN(sz, _system.thePorts[idPort].runtime.sc.receive.szfld - _system.thePorts[idPort].runtime.sc.receive.offset_write);
					if (ReadFile(_system.thePorts[idPort].runtime.hdl,
						_system.thePorts[idPort].runtime.sc.receive.fld + _system.thePorts[idPort].runtime.sc.receive.offset_write, (DWORD)sz, &read, &osRead) == FALSE)
					{
						// Overlapped IO (required for Vista)
						DWORD err = GetLastError();
						if (err == ERROR_IO_PENDING)
						{
							DWORD res = WaitForSingleObject(osRead.hEvent, INFINITE);
							assert(res == WAIT_OBJECT_0);
							read = (DWORD)osRead.InternalHigh;
							assert(read == sz);
						}
						else if (err == ERROR_INVALID_HANDLE)
						{
							/* If handle is closed while reading, we may end up here ... */
							valuesInQueue = 0;
							continue;
						}
						else
						{
							assert(0);
						}
					}

#ifdef JVX_RS232_DEBUG
					printoutBuffer(idPort, _system.thePorts[idPort].runtime.sc.receive.fld + _system.thePorts[idPort].runtime.sc.receive.offset_read,
						read, _system.thePorts[idPort].runtime.cfg.secureChannel.startByte,
						_system.thePorts[idPort].runtime.cfg.secureChannel.stopByte,
						_system.thePorts[idPort].runtime.cfg.secureChannel.escByte, "roff" + jvx_size2String(_system.thePorts[idPort].runtime.sc.receive.offset_read) + "-woff" + jvx_size2String(_system.thePorts[idPort].runtime.sc.receive.offset_write) + "_");
#endif
					valuesInQueue -= read;
					_system.thePorts[idPort].runtime.sc.receive.offset_write += read;

					while (1)
					{
						theSpecialChar = 0;
						sz = numberBytesUnwrapReceive(_system.thePorts[idPort].runtime.sc.receive.fld, _system.thePorts[idPort].runtime.sc.receive.offset_read,
							_system.thePorts[idPort].runtime.sc.receive.offset_write, _system.thePorts[idPort].runtime.cfg.secureChannel.startByte,
							_system.thePorts[idPort].runtime.cfg.secureChannel.stopByte, _system.thePorts[idPort].runtime.cfg.secureChannel.escByte,
							theSpecialChar);
						if (sz)
						{
							assert(_system.thePorts[idPort].runtime.theReport);
							_system.thePorts[idPort].runtime.theReport->provide_data_and_length(&ptrRead, &sz, &offset, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

							numReadApp = sz;
							if (numReadApp > 0)
							{
								szread = 0;
								theSpecialChar = 0;

								removeByteWrapReplace(_system.thePorts[idPort].runtime.sc.receive.fld, _system.thePorts[idPort].runtime.sc.receive.offset_read, 
									_system.thePorts[idPort].runtime.sc.receive.offset_write,
									ptrRead + offset, numReadApp, _system.thePorts[idPort].runtime.cfg.secureChannel.startByte,
									_system.thePorts[idPort].runtime.cfg.secureChannel.stopByte, 
									_system.thePorts[idPort].runtime.cfg.secureChannel.escByte, szread, write, theSpecialChar);

								_system.thePorts[idPort].runtime.sc.receive.offset_read += szread;
								_system.thePorts[idPort].runtime.theReport->report_data_and_read(ptrRead, numReadApp, offset, idPort, &theSpecialChar, JVX_CONNECT_PRIVATE_ARG_TYPE_SECURE_CHANNEL_SPECIAL_CHAR);
							}
						}
						else
						{
							if (theSpecialChar != 0)
							{
								assert(_system.thePorts[idPort].runtime.theReport);
								_system.thePorts[idPort].runtime.theReport->provide_data_and_length(&ptrRead, &sz, &offset, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
								_system.thePorts[idPort].runtime.theReport->report_data_and_read(ptrRead, sz, offset, idPort, &theSpecialChar, JVX_CONNECT_PRIVATE_ARG_TYPE_SECURE_CHANNEL_SPECIAL_CHAR);
								_system.thePorts[idPort].runtime.sc.receive.offset_read++; // The special character HAS been read
							}
							else
							{
								jvxSize numMove = (_system.thePorts[idPort].runtime.sc.receive.offset_write - _system.thePorts[idPort].runtime.sc.receive.offset_read);
								if (numMove)
								{
									memmove(_system.thePorts[idPort].runtime.sc.receive.fld,
										_system.thePorts[idPort].runtime.sc.receive.fld + _system.thePorts[idPort].runtime.sc.receive.offset_read, numMove);
								}
								_system.thePorts[idPort].runtime.sc.receive.offset_read = 0;
								_system.thePorts[idPort].runtime.sc.receive.offset_write = numMove;

								break;
							}
						}
					}
				}
				else
				{
					// Get buffer in which to store bytes
					jvxSize sz = valuesInQueue;
					jvxSize numReadApp = 0;
					jvxByte* ptrRead = NULL;
					jvxSize offset = 0;

					assert(_system.thePorts[idPort].runtime.theReport);
					_system.thePorts[idPort].runtime.theReport->provide_data_and_length(&ptrRead, &sz, &offset, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

					numReadApp = sz;
					if (numReadApp > 0)
					{
						assert(ptrRead);
						if (ReadFile(_system.thePorts[idPort].runtime.hdl, ptrRead + offset, (DWORD)numReadApp, &read, &osRead) == FALSE)
						{
							// Overlapped IO (required for Vista)
							DWORD err = GetLastError();
							if (err == ERROR_IO_PENDING)
							{
								DWORD res = WaitForSingleObject(osRead.hEvent, INFINITE);
								assert(res == WAIT_OBJECT_0);
								read = (DWORD)osRead.InternalHigh;
								assert(read == numReadApp);
							}
							else if (err == ERROR_INVALID_HANDLE)
							{
								/* If handle is closed while reading, we may end up here ... */
								valuesInQueue = 0;
								continue;
							}
							else
							{
								assert(0);
							}
						}
					}

					_system.thePorts[idPort].runtime.theReport->report_data_and_read(ptrRead, read, offset, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
					valuesInQueue -= read;
				}
			}
		}
		

		if(evtMask)
		{
			jvxBitField maskReport = 0;
			jvxUInt16 reportVal = 0;
			DWORD modemState;

			if (evtMask & EV_ERR)
			{
				jvx_bitSet(maskReport, JVX_RS232_REPORT_ERROR_SHFT);
			}

			// Next all line specific events
			if (evtMask & (EV_RING | EV_CTS | EV_RLSD | EV_DSR))
			{

				if (!GetCommModemStatus(_system.thePorts[idPort].runtime.hdl, &modemState))
				{
					assert(0);
				}

				if (evtMask & EV_RING)
				{
					jvx_bitSet(maskReport, JVX_RS232_REPORT_RING_SHFT);
					if (modemState & MS_RING_ON)
					{
						reportVal = 1 << JVX_RS232_STATUS_RING_SHFT;
					}
				}
				if (evtMask & EV_CTS)
				{
					jvx_bitSet(maskReport, JVX_RS232_REPORT_CTS_SHFT);
					if (modemState & MS_CTS_ON)
					{
						reportVal = 1 << JVX_RS232_STATUS_CTS_SHFT;
					}
				}
				if (evtMask & EV_RLSD)
				{
					jvx_bitSet(maskReport, JVX_RS232_REPORT_RLSD_SHFT);
					if (modemState & MS_RLSD_ON)
					{
						reportVal = 1 << JVX_RS232_STATUS_RLSD_SHFT;
					}
				}
				if (evtMask & EV_DSR)
				{
					jvx_bitSet(maskReport, JVX_RS232_REPORT_DSR_SHFT);
					if (modemState & MS_DSR_ON)
					{
						reportVal = 1 << JVX_RS232_STATUS_DSR_SHFT;
					}
				}
			}
			if (JVX_EVALUATE_BITFIELD(maskReport))
			{
				if (_system.thePorts[idPort].runtime.theReport)
				{
					_system.thePorts[idPort].runtime.theReport->report_event(maskReport, idPort, &reportVal, JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_MODEM_EVENT);
				}
			}

			if (evtMask & EV_CTS)
			{
				if (_system.thePorts[idPort].runtime.theReport)
				{
					if (jvx_bitTest(_system.thePorts[idPort].runtime.cfg.reportEnum, JVX_RS232_REPORT_CONNECTION_ESTABLISHED_SHIFT))
					{
						if (reportVal & MS_CTS_ON)
						{
							jvxBitField mask;
							jvx_bitSet(mask, JVX_RS232_REPORT_CONNECTION_ESTABLISHED_SHIFT);
							_system.thePorts[idPort].runtime.theReport->report_event(mask, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
						}
					}
					if (jvx_bitTest(_system.thePorts[idPort].runtime.cfg.reportEnum, JVX_RS232_REPORT_CONNECTION_STOPPED_SHIFT))
					{
						if (!(reportVal & MS_CTS_ON))
						{
							jvxBitField mask;
							jvx_bitSet(mask, JVX_RS232_REPORT_CONNECTION_STOPPED_SHIFT);
							_system.thePorts[idPort].runtime.theReport->report_event(mask, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
						}
					}
				}
			}
		} // if(evtMask)
	}

	if (!jvx_bitTest(_system.thePorts[idPort].runtime.cfg.reportEnum, JVX_RS232_REPORT_CTS_SHFT))
	{
		if (!jvx_bitTest(_system.thePorts[idPort].runtime.cfg.reportEnum, JVX_RS232_REPORT_CONNECTION_STOPPED_SHIFT))
		{
			if (_system.thePorts[idPort].runtime.theReport)
			{
				jvxBitField mask;
				jvx_bitSet(mask, JVX_RS232_REPORT_CONNECTION_STOPPED_SHIFT);
				_system.thePorts[idPort].runtime.theReport->report_event(mask, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
			}
		}
	}
	// If this thread stops, state drops down to INIT state
	JVX_LOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
	_system.thePorts[idPort].theState = JVX_STATE_INIT;
	JVX_UNLOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);

	if (_system.thePorts[idPort].runtime.theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_CONNECTION_REPORT_SHUTDOWN_COMPLETE_SHFT);
		_system.thePorts[idPort].runtime.theReport->report_event(mask, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
	}
}

#ifdef JVX_RS232_DEBUG
void
CjvxRs232::startprintout(jvxSize portId)
{
	FILE* fh = NULL;
	_system.thePorts[portId].dbg.fn = ("jvx_dbg_rs232_" + jvx_size2String(portId) + ".txt");	
	fh = fopen(_system.thePorts[portId].dbg.fn.c_str(), "w");
	if (fh)
	{
		fprintf(fh, "Start debug output, %s", JVX_DATE().c_str());
		fclose(fh);
	}
}

void
CjvxRs232::stoprintout(jvxSize portId)
{
}

void
CjvxRs232::printoutBuffer(jvxSize idPort, jvxByte* fld, jvxSize sz, jvxByte startByte, jvxByte stopByte, jvxByte escByte, std::string token)
{
	jvxSize i;
	JVX_LOCK_MUTEX(_system.thePorts[idPort].dbg.safeAccess);
	FILE* fh = fopen(_system.thePorts[idPort].dbg.fn.c_str(), "a");
	jvxUInt64 tt = JVX_GET_TICKCOUNT_US_GET(_system.thePorts[idPort].dbg.tRef);
	if (fh)
	{
		fprintf(fh, "--> %s <%d - " JVX_PRINTF_CAST_INT64 ">: ", token.c_str(), (int)sz, tt);
		for (i = 0; i < sz; i++)
		{
			jvxByte in = fld[i];
			if (i != 0)
				fprintf(fh, ",");
			fprintf(fh, "[#%d", (int)i);
			if (in == startByte)
				fprintf(fh, ">+>");
			if (in == stopByte)
				fprintf(fh, ">->");
			if (in == escByte)
				fprintf(fh, ">*>");
			fprintf(fh, "-s-%d-]", (int)fld[i]);
		}
		fprintf(fh, "\n");
		fclose(fh);
	}
	JVX_UNLOCK_MUTEX(_system.thePorts[idPort].dbg.safeAccess);
	
}

void
CjvxRs232::printoutText(jvxSize idPort, std::string token)
{
	JVX_LOCK_MUTEX(_system.thePorts[idPort].dbg.safeAccess);
	FILE* fh = fopen(_system.thePorts[idPort].dbg.fn.c_str(), "a");
	jvxUInt64 tt = JVX_GET_TICKCOUNT_US_GET(_system.thePorts[idPort].dbg.tRef);
	if (fh)
	{
		fprintf(fh, "--> %s <" JVX_PRINTF_CAST_INT64 ">: ", token.c_str(), tt);
		fprintf(fh, "\n");
		fclose(fh);
	}
	JVX_UNLOCK_MUTEX(_system.thePorts[idPort].dbg.safeAccess);

}
#endif
