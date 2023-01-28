#include "jvx.h"
#include "CjvxRs232.h"
#include <iostream>

#include <sys/epoll.h>
#include <sys/eventfd.h>

#define JVX_RS232_OUTPUT_BEFORE_SEND
#define JVX_RS232_OUTPUT_AFTER_RECEIVE
 
#define MAX_EVENTS_EPOLL 10 
#define JVX_TIMEOUT_SCAN_SIGNALS 100

JVX_THREAD_ENTRY_FUNCTION(thread_receive, param)
{
	assert(param);
	{
		jvxRs232PrivateThread* enter = (jvxRs232PrivateThread*)param;
		enter->this_pointer->receiveLoop(JVX_SIZE_INT(enter->portId));
	}
	return(0);
}

JVX_THREAD_ENTRY_FUNCTION(thread_signal, param)
{
    assert(param);
    {
        jvxRs232PrivateThread* enter = (jvxRs232PrivateThread*)param;
        enter->this_pointer->signalLoop(JVX_SIZE_INT(enter->portId));
    }
    return(0);
}

const char* serialTerminalWc[] =
{
    "ttyS*",
    "ttyUSB*",
    "ttyAC*",
    "ttyAL*",
    NULL
};

static bool matchesAny(const std::string& filename)
{
    jvxSize cnt = 0;
    while(1)
    {
        if(serialTerminalWc[cnt])
        {
            if(jvx_compareStringsWildcard(serialTerminalWc[cnt], filename))
            {
                return true;
            }
        }
        else
        {
            break;
        }
        cnt++;
    }
    return false;
}

CjvxRs232::CjvxRs232(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected( JVX_COMPONENT_CONNECTION);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxConnection*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theComponentSubTypeDescriptor = "/";
	_common_set.theComponentSubTypeDescriptor += JVX_CONNECTION_TAG_RS232;

	_common_set_min.theDescription = "JVX RS232 connection module glnx";
	_system.refCount = 0;
}

CjvxRs232::~CjvxRs232()
{
	// If we reach here, all ports MUST be closed
	this->terminate();
	CjvxObject::_terminate();
}

jvxErrorType 
CjvxRs232::initialize(IjvxHiddenInterface* hostRef, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)
{	
	int id = 0;
	jvxSize jj;
	bool thereIsAProblem = false;

	jvxErrorType res = JVX_NO_ERROR;
	res = _initialize(hostRef);
	if (res == JVX_NO_ERROR)
	{
	    std::string folderTty = "/dev";
	
	    JVX_HANDLE_DIR searchHandle;
	    JVX_DIR_FIND_DATA searchResult = INIT_SEARCH_DIR_DEFAULT;;
	    JVX_HMODULE dllHandle;

	    // Search for directory
	    searchHandle = JVX_FINDFIRSTFILE_WC(searchHandle, folderTty.c_str(), "", searchResult);
	    bool result = true;
	    bool foundElement = false;

	    // If entry exists
	    if (searchHandle != INVALID_HANDLE_VALUE)
	    {
		// open DLL and test for technology or plugin
		do
		{
		    std::string fileName = JVX_GETFILENAMEDIR(searchResult);
		    //if (jvx_compareStringsWildcard(ttyWc, fileName))
		    if(matchesAny(fileName))
		    {

			oneRs232Port thePort;
			thePort.fileNamePath = folderTty;
			thePort.fileNamePath += JVX_SEPARATOR_DIR;
			thePort.fileNamePath += fileName;

			std::cout << "Trying to open handle for file " << thePort.fileNamePath << std::endl;

			// O_RDWR: read and write
			// O_NOCTTY: do not react on various inputs
			// O_SYNC: Guarantee that data is transferred before write returns
			// O_DIRECT: no memcpy in kernel space. Write may reutn before complete
			// O_NDELAY: do not wait for data carrier detected line
			thePort.fd = open(thePort.fileNamePath.c_str(), O_RDWR | O_NOCTTY | O_SYNC | O_NDELAY);
			if (thePort.fd < 0)
			{
			    std::cout << "Error opening " << thePort.fileNamePath << ":" << strerror(errno) << std::endl;
			    continue;
			}

			thePort.theReport = NULL;
			thePort.theState = JVX_STATE_INIT;
			JVX_INITIALIZE_MUTEX(thePort.safeAccessConnection);
			memset(&thePort.sc, 0, sizeof(thePort.sc));
					
			thePort.threads.enter = NULL;
			thePort.threads.hdl = (JVX_THREAD_HANDLE)JVX_INVALID_HANDLE_VALUE;
			thePort.threads.id = _system.thePorts.size();
			thePort.threads.inLoopActive = false;

			jvx_rs232_initRs232Default(&thePort.cfg);

#ifdef JVX_RS232_DEBUG
			JVX_INITIALIZE_MUTEX(thePort.dbg.safeAccess);
#endif

			_system.thePorts.push_back(thePort);
			//searchResult.cFileName;
		    }
		} while (JVX_FINDNEXTFILE(searchHandle, searchResult, ""));
		JVX_FINDCLOSE(searchHandle);//FindClose( searchHandle );
	    }
	    _system.refCount = 1;
	}

	return(res);
}

jvxErrorType 
CjvxRs232::availablePorts(jvxApiStringList* allPorts)
{
	int i;
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState >= JVX_STATE_INIT)
	{
		std::vector<std::string> lst;
		for(i = 0; i < _system.thePorts.size(); i++)
		{
			lst.push_back(_system.thePorts[i].fileNamePath);
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
	return(res);
}

// =============================================================
// Port specific commands
// =============================================================

jvxErrorType 
CjvxRs232::state_port(jvxSize idPort, jvxState* theState)
{
	jvxErrorType res = JVX_NO_ERROR;
		if(_common_set_min.theState >= JVX_STATE_INIT)
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
	return(res);
}

jvxErrorType
CjvxRs232::start_port(jvxSize idPort, jvxHandle* cfgIn, jvxConnectionPrivateTypeEnum whatsthis, IjvxConnection_report* theReport)
{
    jvxErrorType res = JVX_NO_ERROR;
    struct termios options;
    if (_common_set_min.theState >= JVX_STATE_INIT)
    {

	jvxRs232Config* cfg = (jvxRs232Config*)cfgIn;
	assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_CONFIG);
	if (idPort < _system.thePorts.size())
	{
	    if (_system.thePorts[idPort].theState == JVX_STATE_INIT)
	    {
		// Open COM port

                // O_RDWR: read and write
		// O_NOCTTY: do not react on various inputs
		// O_SYNC: Guarantee that data is transferred before write returns
		// O_DIRECT: no memcpy in kernel space. Write may reutn before complete
		// O_NDELAY: do not wait for data carrier detected line
                _system.thePorts[idPort].fd = open(_system.thePorts[idPort].fileNamePath.c_str(),O_RDWR | O_NOCTTY | O_SYNC | O_NDELAY);

                if (_system.thePorts[idPort].fd < 0)
		{
		    res = JVX_ERROR_INTERNAL;
		}

		if (res == JVX_NO_ERROR)
		{
		    std::string infoTxt;
		    
		    _system.thePorts[idPort].theReport = theReport;

		    // Copy configuration
		    _system.thePorts[idPort].cfg = *cfg;

		    /*
		     * Following code taken from here: https://www.cmrr.umn.edu/~strupp/serial.html#config
		     */

		    // Get the current options for the port...
		    if(tcgetattr(_system.thePorts[idPort].fd, &options) < 0)
		    {
			assert(0);
		    }

		    res = JVX_NO_ERROR;

		    switch (_system.thePorts[idPort].cfg.bRate)
		    {
		    case 9600:
			infoTxt += "9600";
                        cfsetispeed(&options, B9600);
                        cfsetospeed(&options, B9600);
			break;
		    case 19200:
			infoTxt += "19200";
                        cfsetispeed(&options, B19200);
                        cfsetospeed(&options, B19200);
			break;
		    case 38400:
			infoTxt += "38400";
                        cfsetispeed(&options, B38400);
                        cfsetospeed(&options, B38400);
			break;
		    case 57600:
			infoTxt += "57600";
                        cfsetispeed(&options, B57600);
                        cfsetospeed(&options, B57600);
			break;
			/*
			 * case 76800:
			 cfsetispeed(&options, B76800);
			 cfsetospeed(&options, B76800);
			 break;*/
		    case 115200:
			infoTxt += "115200";
                        cfsetispeed(&options, B115200);
                        cfsetospeed(&options, B115200);
			break;
		    default:
			res = JVX_ERROR_INVALID_SETTING;
		    }

                    // Enable the receiver and set local mode...
                    options.c_cflag |= (CLOCAL | CREAD);

                    options.c_cflag &= ~CSIZE;
                    switch (_system.thePorts[idPort].cfg.bits4Byte)
                    {
                    case 5:
			infoTxt += ";5b";
                        options.c_cflag |= CS5;
                        break;
                    case 6:
			infoTxt += ";6b";
                        options.c_cflag |= CS6;
                        break;
                    case 7:
			infoTxt += ";7b";
                        options.c_cflag |= CS7;
                        break;
                    case 8:
			infoTxt += ";8b";
                        options.c_cflag |= CS8;
                        break;
                    default:
                        res = JVX_ERROR_INVALID_SETTING;
                    }

                    switch (_system.thePorts[idPort].cfg.stopBitsEnum)
                    {
                    case JVX_RS232_STOPBITS_TWO:
			infoTxt += ";2s";
                        options.c_cflag |= CSTOPB;
                        break;
                    case JVX_RS232_STOPBITS_ONE:
			infoTxt += ";1s";
                        options.c_cflag &= ~CSTOPB;
                        break;
                    default:
                        res = JVX_ERROR_INVALID_SETTING;
                    }

                    options.c_cflag &= ~PARENB;
                    options.c_cflag &= ~PARODD;
                    options.c_cflag &= ~CSTOPB;

                    switch (_system.thePorts[idPort].cfg.parityEnum)
                    {
                    case JVX_RS232_PARITY_EVEN:
			infoTxt += ";ep";
                        options.c_cflag |= PARENB;
                        break;
                    case JVX_RS232_PARITY_ODD:
			infoTxt += ";op";
                        options.c_cflag |= PARENB | PARODD;
                        break;
                    case JVX_RS232_PARITY_NO:
			infoTxt += ";np";
                        options.c_cflag &= ~PARENB;
                        break;
                    default:
                        res = JVX_ERROR_INVALID_SETTING;
                    }

                    // To disable software flow control simply mask those bits :
                    options.c_iflag &= ~(IXON | IXOFF | IXANY);
                    switch (_system.thePorts[idPort].cfg.enFlow)
                    {
                    case JVX_RS232_NO_FLOWCONTROL:
                        // options.c_cflag &= ~CNEW_RTSCTS;
                        options.c_cflag &= ~CRTSCTS;
                        break;
                    case JVX_RS232_CTSRTSFLOWCONTROL:
                        options.c_cflag |= CRTSCTS;
                        //options.c_cflag |= CNEW_RTSCTS;
                        break;
                    case JVX_RS232_DSRDTRFLOWCONTROL:
                    case JVX_RS232_DSRRTSFLOWCONTROL:
                        break;
                    default:

                        // Software flow control
                        options.c_iflag |= (IXON | IXOFF | IXANY);

                        // Set the characters:
                        options.c_cc[VSTART] = 0x11;
                        options.c_cc[VSTOP] = 0x13;
                        //_system.thePorts[idPort].cfg.XoffLim = 100;
                        //_system.thePorts[idPort].cfg.XonLim = 100;
                        break;
                    }

		    // IGNBRK : if set ignore break
		    // BRKINT : if set transfer a SIGINT on break.
		    // IGNPAR : if set parity errors are ignored
		    // INPCK : Enable input parity check
		    // PARMRK : Mark input bytes with errors (\377)
		    // ISTRIP : Strip off 8 bits
		    // INCLR : Translate NL to CR (<- !!!!!!!)
		    // IGNCR : Ignore CR
		    // ICRNL : Translate CR to NL
		    
		    // ~(IGNBRK | BRKINT)) : a break reads a \0, if PARMRK is set break reads a \377 \377 \0
		    
		    
		    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL );
		    
                    // Canocical mode: waiting for carriage return before transfer - NOT FOR US: options.c_lflag |= (ICANON | ECHO | ECHOE);
                    // Raw mode:
                    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

                    switch (_system.thePorts[idPort].cfg.parityEnum)
                    {
                    case JVX_RS232_PARITY_EVEN:
                    case JVX_RS232_PARITY_ODD:
                        // Parity checking
                        options.c_iflag |= (INPCK | ISTRIP);
                        break;
                    }

                    // Disable output processing for raw mode
                    options.c_oflag &= ~OPOST;
                    // Only output option that may be useful:
                    // ---  options.c_oflag |=ONLCR;

                    // Minimum number of characters to read
                    options.c_cc[VMIN] = 0;
                    options.c_cc[VTIME] = 0;

                    if (res == JVX_NO_ERROR)
		    {

			_report_text_message(("Configuring serial port with settings <" + infoTxt + ">.").c_str(), JVX_REPORT_PRIORITY_INFO);

#ifdef JVX_RS232_VERBOSE_PORT_CONFIG_STARTUP
			std::cout << "Configuring serial port with settings <" + infoTxt + ">." << std::endl;
#endif			
			// Set the new options for the port...
			// TCSANOW	Make changes now without waiting for data to complete
			// TCSADRAIN	Wait until everything has been transmitted
			// TCSAFLUSH	Flush input and output buffers and make the change
                        //if(tcsetattr(_system.thePorts[idPort].fd, TCSANOW, &options) < 0)
						if(tcsetattr(_system.thePorts[idPort].fd, TCSAFLUSH, &options) < 0)
                        {
                            assert(0);
                        }
			_report_text_message("Configuring serial port completed successfully.", JVX_REPORT_PRIORITY_INFO);

#ifdef JVX_RS232_VERBOSE_PORT_CONFIG_STARTUP
			std::cout << "Configuring serial port completed successfully." << std::endl;
#endif			
                        _system.thePorts[idPort].sc.transmit.szfld = 0;
                        _system.thePorts[idPort].sc.transmit.fld = NULL;

                        if (_system.thePorts[idPort].cfg.secureChannel.transmit.active)
                        {
                            if (JVX_CHECK_SIZE_SELECTED(_system.thePorts[idPort].cfg.secureChannel.transmit.preallocationSize))
                            {
                                if (_system.thePorts[idPort].cfg.secureChannel.transmit.preallocationSize > 0)
                                {
                                    _system.thePorts[idPort].sc.transmit.szfld = _system.thePorts[idPort].cfg.secureChannel.transmit.preallocationSize;
                                    JVX_SAFE_NEW_FLD(_system.thePorts[idPort].sc.transmit.fld, jvxByte, _system.thePorts[idPort].sc.transmit.szfld);
                                    memset(_system.thePorts[idPort].sc.transmit.fld, 0, sizeof(jvxByte) *  _system.thePorts[idPort].sc.transmit.szfld);
                                }
                            }
                        }

                        if (_system.thePorts[idPort].cfg.secureChannel.receive.active)
                        {
                            _system.thePorts[idPort].sc.receive.szfld = _system.thePorts[idPort].cfg.secureChannel.receive.maxPreReadSize;
                            assert(_system.thePorts[idPort].sc.receive.szfld);
                            JVX_SAFE_NEW_FLD(_system.thePorts[idPort].sc.receive.fld, jvxByte, _system.thePorts[idPort].sc.receive.szfld);
                            memset(_system.thePorts[idPort].sc.receive.fld, 0, sizeof(jvxByte)* _system.thePorts[idPort].sc.receive.szfld);
                            _system.thePorts[idPort].sc.receive.offset_read = 0;
                            _system.thePorts[idPort].sc.receive.offset_write = 0;
                        }

                        _system.thePorts[idPort].threads.enter = NULL;
                        JVX_DSP_SAFE_ALLOCATE_OBJECT(_system.thePorts[idPort].threads.enter, jvxRs232PrivateThread);
                        _system.thePorts[idPort].threads.enter->portId = idPort;
                        _system.thePorts[idPort].threads.enter->this_pointer = this;

                        _system.thePorts[idPort].theState = JVX_STATE_PROCESSING;

                        _system.thePorts[idPort].threads.inLoopActive = true;

#ifdef JVX_RS232_DEBUG
                        JVX_GET_TICKCOUNT_US_SETREF(&_system.thePorts[idPort].dbg.tRef);
                        startprintout(idPort);
#endif

                        _system.thePorts[idPort].threads.eventfd = eventfd(0, EFD_NONBLOCK);

			// Synchronize startup
			JVX_INITIALIZE_NOTIFICATION(_system.thePorts[idPort].threads.threadStart);
			JVX_WAIT_FOR_NOTIFICATION_I(_system.thePorts[idPort].threads.threadStart);

                        JVX_CREATE_THREAD(_system.thePorts[idPort].threads.hdl, thread_receive, _system.thePorts[idPort].threads.enter, _system.thePorts[idPort].threads.id);
                        if (cfg->boostPrio)
                        {
                            JVX_SET_THREAD_PRIORITY(_system.thePorts[idPort].threads.hdl, JVX_THREAD_PRIORITY_REALTIME);
                        }

                        jvx_bitSet(_system.thePorts[idPort].cfg.reportEnum, JVX_RS232_REPORT_CTS_SHFT);

                        _system.thePorts[idPort].threads.hdls = (JVX_THREAD_HANDLE)JVX_INVALID_HANDLE_VALUE;
                        if( _system.thePorts[idPort].cfg.reportEnum)
                        {
                            if (ioctl(_system.thePorts[idPort].fd, TIOCMGET, &_system.thePorts[idPort].line_status) < 0)
                            {
                                assert(0);
                            }
                            JVX_CREATE_THREAD(_system.thePorts[idPort].threads.hdls, thread_signal, _system.thePorts[idPort].threads.enter,
                                              _system.thePorts[idPort].threads.ids);
                        }

                        // Wait for thread to come up
			JVX_WAIT_FOR_NOTIFICATION_II_INF(_system.thePorts[idPort].threads.threadStart);
                        JVX_SLEEP_MS(30); // <-- wait a little longer

                        _system.thePorts[idPort].theState = JVX_STATE_PROCESSING;
                    }
		}
	    }
	    else
	    {
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
    return res;
}

jvxErrorType
CjvxRs232::get_constraints_buffer(jvxSize idPort, jvxSize* bytesPrepend, jvxSize* fldMinSize, jvxSize* fldMaxSize)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_system.safeAccess);
	if(_common_set_min.theState >= JVX_STATE_INIT)
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
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize szBytesWrap = 0;
	jvxSize* szFldPtr = szFld;
	jvxSize writtenSC = 0;
	jvxSize written = 0;
	if(_common_set_min.theState >= JVX_STATE_INIT)
	{
		if(idPort < _system.thePorts.size())
		{
			// We put the transmit in a critical section to avoid troube if accessing in parallel
			JVX_LOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
			if(_system.thePorts[idPort].theState == JVX_STATE_PROCESSING)
			{
				if (_system.thePorts[idPort].cfg.secureChannel.transmit.active)
				{
					szBytesWrap = jvx_rs232_numberBytesWrapSend(fld, *szFld, _system.thePorts[idPort].cfg.secureChannel.startByte, 
						_system.thePorts[idPort].cfg.secureChannel.stopByte, 
						_system.thePorts[idPort].cfg.secureChannel.escByte);
					if (_system.thePorts[idPort].sc.transmit.szfld < szBytesWrap)
					{
						if (_system.thePorts[idPort].sc.transmit.fld)
						{
							JVX_SAFE_DELETE_FLD(_system.thePorts[idPort].sc.transmit.fld, jvxByte);
							_system.thePorts[idPort].sc.transmit.fld = NULL;
							_system.thePorts[idPort].sc.transmit.szfld = 0;
						}
						_system.thePorts[idPort].sc.transmit.szfld = szBytesWrap;
						JVX_SAFE_NEW_FLD(_system.thePorts[idPort].sc.transmit.fld, jvxByte, _system.thePorts[idPort].sc.transmit.szfld);
						memset(_system.thePorts[idPort].sc.transmit.fld, 0, sizeof(jvxByte) *_system.thePorts[idPort].sc.transmit.szfld);
					}
					jvx_rs232_insertByteWrapReplace(fld, *szFld, _system.thePorts[idPort].sc.transmit.fld, 
						_system.thePorts[idPort].sc.transmit.szfld, 
						_system.thePorts[idPort].cfg.secureChannel.startByte,
						_system.thePorts[idPort].cfg.secureChannel.stopByte, 
						_system.thePorts[idPort].cfg.secureChannel.escByte,
						writtenSC);
					assert(writtenSC == szBytesWrap);
					fld = _system.thePorts[idPort].sc.transmit.fld;
					szFldPtr = &szBytesWrap;
				}

#ifdef JVX_RS232_DEBUG
				printoutBuffer(idPort, fld, *szFldPtr, _system.thePorts[idPort].cfg.secureChannel.startByte, 
                    _system.thePorts[idPort].cfg.secureChannel.stopByte, _system.thePorts[idPort].cfg.secureChannel.escByte, "send");
#endif

				written = write(_system.thePorts[idPort].fd, fld, *szFldPtr);

#ifdef JVX_RS232_DEBUG_TXT
                jvxSize i;
                std::cout << "--> " << std::flush;
                for(i = 0; i < *szFldPtr; i++)
                {
                    std::cout << "<" << fld[i] << ">" << std::flush;
                }
                std::cout << std::endl;
#endif

#ifdef JVX_RS232_DEBUG
				printoutText(idPort, "Send complete");
#endif

				if (_system.thePorts[idPort].cfg.secureChannel.transmit.active)
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

jvxErrorType 
CjvxRs232::stop_port(jvxSize idPort)
{
    int status = 0;
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState >= JVX_STATE_INIT)
	{
		if (idPort < _system.thePorts.size())
		{
			JVX_LOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
			if (_system.thePorts[idPort].theState == JVX_STATE_PROCESSING)
			{
				_system.thePorts[idPort].threads.inLoopActive = false;
				JVX_UNLOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);

                // Inform that loop is over
                eventfd_write(_system.thePorts[idPort].threads.eventfd, 1);

				// Wait for thread to terminate :: MUST BE OUTSIDE MUTEX!!!
				JVX_WAIT_FOR_THREAD_TERMINATE_INF(_system.thePorts[idPort].threads.hdl);

#if 0
                // Hard stop thread and release the ioctl to look for line changes
                /* THIS DOES NOT WORK AT THE MOMENT !!!!! */
                pthread_kill(_system.thePorts[idPort].threads.hdls, SIGUSR1);
#endif
                JVX_WAIT_FOR_THREAD_TERMINATE_INF(_system.thePorts[idPort].threads.hdls);

		JVX_TERMINATE_NOTIFICATION(_system.thePorts[idPort].threads.threadStart);
		
                // Close serial port
                close(_system.thePorts[idPort].fd);

                // Clear memory
                JVX_DSP_SAFE_DELETE_OBJECT(_system.thePorts[idPort].threads.enter);
                _system.thePorts[idPort].threads.enter = NULL;

#ifdef JVX_RS232_DEBUG
				stoprintout(idPort);
#endif

				if (_system.thePorts[idPort].sc.transmit.fld)
				{
					JVX_SAFE_DELETE_FLD(_system.thePorts[idPort].sc.transmit.fld, jvxByte);
					_system.thePorts[idPort].sc.transmit.fld = NULL;
				}
				// There is no need to lock since no thread is running
                _system.thePorts[idPort].fd = -1;

                JVX_LOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
				_system.thePorts[idPort].theState = JVX_STATE_INIT;
                res = JVX_NO_ERROR;
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
	return res;
}

// =============================================================

jvxErrorType
CjvxRs232::control_port(jvxSize idPort, jvxSize operation_id, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxRs232ControlCommands contrType = (jvxRs232ControlCommands) operation_id;
	jvxUInt16* retVal = NULL;

	JVX_LOCK_MUTEX(_system.safeAccess);
	if (_common_set_min.theState >= JVX_STATE_INIT)
	{
		if (idPort < _system.thePorts.size())
		{
			JVX_LOCK_MUTEX(_system.thePorts[idPort].safeAccessConnection);
			if (_system.thePorts[idPort].theState == JVX_STATE_PROCESSING)
			{
                /*
                 int ioctl(int fd, int request, ...);
                 int status;
                 ioctl(fd, TIOCMGET, &status);
                 ioctl(fd, TIOCMSET, &status);
                 TIOCM_LE	DSR (data set ready/line enable)
                 TIOCM_DTR	DTR (data terminal ready)
                 TIOCM_RTS	RTS (request to send)
                 TIOCM_ST	Secondary TXD (transmit)
                 TIOCM_SR	Secondary RXD (receive)
                 TIOCM_CTS	CTS (clear to send)
                 TIOCM_CAR	DCD (data carrier detect)
                 TIOCM_CD	Synonym for TIOCM_CAR
                 TIOCM_RNG	RNG (ring)
                 TIOCM_RI	Synonym for TIOCM_RNG
                 TIOCM_DSR	DSR (data set ready)
                 */

				switch (contrType)
				{
				case JVX_RS232_CONTROL_GET_MODEM_STATUS:
					
					if (priv)
					{
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_MODEM_STATUS);
						retVal = (jvxUInt16*)priv;
						res = JVX_NO_ERROR;

						int status = 0;

						if (ioctl(_system.thePorts[idPort].fd, TIOCMGET, &status) < 0)
						{
							assert(0);
						}
						
						if (retVal)
						{
							*retVal = 0;

							if (status & TIOCM_RNG)
							{
								*retVal = 1 << JVX_RS232_STATUS_RING_SHFT;
							}

							if (status & TIOCM_CTS)
							{
								*retVal = 1 << JVX_RS232_STATUS_CTS_SHFT;
							}

							if (status & TIOCM_CAR)
							{
								*retVal = 1 << JVX_RS232_STATUS_RLSD_SHFT;
							}

                            if (status & TIOCM_DSR)
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

                case JVX_RS232_CONTROL_SET_MODEM_STATUS:

                    if (priv)
                    {
                        assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_MODEM_STATUS);
                        retVal = (jvxUInt16*)priv;
                        res = JVX_NO_ERROR;

                        int status = 0;

                        if (retVal)
                        {
                            if (ioctl(_system.thePorts[idPort].fd, TIOCMGET, &status) < 0)
                            {
                                assert(0);
                            }

                            if(jvx_bitTest(*retVal, JVX_RS232_STATUS_RING_SHFT))
                            {
                                status |= TIOCM_RNG;
                            }
                            else
                            {
                                status &= ~TIOCM_RNG;
                            }

                            if(jvx_bitTest(*retVal, JVX_RS232_STATUS_CTS_SHFT))
                            {
                                status |= TIOCM_CTS;
                            }
                            else
                            {
                                status &= ~TIOCM_CTS;
                            }

                            if(jvx_bitTest(*retVal, JVX_RS232_STATUS_RLSD_SHFT))
                            {
                                status |= TIOCM_CAR;
                            }
                            else
                            {
                                status &= ~TIOCM_CAR;
                            }

                            if(jvx_bitTest(*retVal, JVX_RS232_STATUS_DSR_SHFT))
                            {
                                status |= TIOCM_DSR;
                            }
                            else
                            {
                                status &= ~TIOCM_DSR;
                            }

                            if (ioctl(_system.thePorts[idPort].fd, TIOCMSET, &status) < 0)
                            {
                                assert(0);
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
	if(_common_set_min.theState >= JVX_STATE_INIT)
	{
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
CjvxRs232::signalLoop(int idPort)
{
    int status = 0;
    unsigned mask = 0;
    jvxUInt16 reportVal = 0;
    jvxBitField maskReport;

    // signal(SIGUSR1, sig_handler);

    // Expect indications for all handshake control and arriving bytes
    if(jvx_bitTest(_system.thePorts[idPort].cfg.reportEnum, JVX_RS232_REPORT_RING_SHFT))
    {
        mask |= TIOCM_RNG;
    }
    if(jvx_bitTest(_system.thePorts[idPort].cfg.reportEnum, JVX_RS232_REPORT_CTS_SHFT))
    {
        mask |= TIOCM_CTS;
    }
    if(jvx_bitTest(_system.thePorts[idPort].cfg.reportEnum, JVX_RS232_REPORT_RLSD_SHFT))
    {
        mask |= TIOCM_CAR;
    }
    if(jvx_bitTest(_system.thePorts[idPort].cfg.reportEnum, JVX_RS232_REPORT_DSR_SHFT))
    {
        mask |= TIOCM_DSR;
    }

    while(_system.thePorts[idPort].threads.inLoopActive)
    {
#ifdef JVX_LINES_IOCTL
        int errCode = ioctl(_system.thePorts[idPort].fd, TIOCMIWAIT, mask);
        if(errCode == 0)
        {
#else
        JVX_SLEEP_MS(JVX_TIMEOUT_SCAN_SIGNALS);
        {
#endif
            if (ioctl(_system.thePorts[idPort].fd, TIOCMGET, &status) < 0)
            {
                assert(0);
            }

            status = status ^_system.thePorts[idPort].line_status;

            reportVal = 0;
            jvx_bitFClear(maskReport);
            if(status & mask)
            {
                if(mask & TIOCM_RNG)
                {
                    jvx_bitSet(maskReport, JVX_RS232_REPORT_RING_SHFT);

                    if(status & TIOCM_RNG)
                    {
                        reportVal |= (1 << JVX_RS232_STATUS_RING_SHFT);
                    }
                }

                if(mask & TIOCM_CTS)
                {
                    jvx_bitSet(maskReport, JVX_RS232_REPORT_CTS_SHFT);

                    if(status & TIOCM_CTS)
                    {
                        reportVal |= (1 << JVX_RS232_STATUS_CTS_SHFT);
                    }
                }

                if(mask & TIOCM_CAR)
                {
                    jvx_bitSet(maskReport, JVX_RS232_REPORT_RLSD_SHFT);

                    if(status & TIOCM_CAR)
                    {
                        reportVal |= (1 << JVX_RS232_STATUS_RLSD_SHFT);
                    }
                }

                if(mask & TIOCM_DSR)
                {
                    jvx_bitSet(maskReport, JVX_RS232_REPORT_DSR_SHFT);

                    if(status & TIOCM_DSR)
                    {
                        reportVal |= (1 << JVX_RS232_STATUS_DSR_SHFT);
                    }
                }

                if (JVX_EVALUATE_BITFIELD(maskReport))
                {
                    if (_system.thePorts[idPort].theReport)
                    {
                        _system.thePorts[idPort].theReport->report_event(maskReport, idPort, &reportVal, JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_MODEM_EVENT);
                    }
                }
            }
        }
    }
#ifdef JVX_RS232_DEBUG_TXT
    std::cout << __FUNCTION__ << " thread terminating..." << std::endl;
#endif
}

#if 0
/* Get new status of the control lines in mask */
if (ioctl(fd, TIOCMGET, &status) == -1) {
    perror("ioctl(TIOCMGET)");
    exit(1);
}
newstate = status & mask;

/* XOR old with new status to find the one(s) that changed */
changed  = newstate ^ oldstate;

/* Do something with the result of al this */
if (changed & TIOCM_RNG) {   /* RNG line has changed */
    if (newstate  & TIOCM_RNG) {
        printf("RNG went from: 0 --> 1\n");
    } else {
        printf("RNG went from: 1 --> 0\n");
    }
}
#endif


void
CjvxRs232::receiveLoop(int idPort)
{
    int readNum = 0;
    bool resRead;
    unsigned ind;

    int valuesInQueue;
	
    jvxSize sz = 0;
    jvxSize numReadApp = 0;
    jvxByte* ptrRead = NULL;
    jvxSize offset = 0;
    jvxSize write = 0;
    jvxByte theSpecialChar = 0;
    jvxSize szread;
    jvxSize i;
    int nfds = 0;

    struct epoll_event ev, events[MAX_EVENTS_EPOLL], evdown;
    int epollfd;
    int status;

    if (_system.thePorts[idPort].theReport)
    {
        jvxBitField mask;
        jvx_bitSet(mask, JVX_CONNECTION_REPORT_STARTUP_COMPLETE_SHFT);
        _system.thePorts[idPort].theReport->report_event(mask, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
    }

    epollfd = epoll_create1(0);
    assert(epollfd >= 0);

    ev.data.fd = _system.thePorts[idPort].fd;
    ev.events = EPOLLIN | EPOLLERR;

    status = epoll_ctl(epollfd, EPOLL_CTL_ADD, _system.thePorts[idPort].fd, &ev);
    assert(status >= 0);

    evdown.data.fd = _system.thePorts[idPort].threads.eventfd;
    evdown.events = EPOLLIN | EPOLLET;

    status = epoll_ctl(epollfd, EPOLL_CTL_ADD, _system.thePorts[idPort].threads.eventfd, &evdown);
    assert(status >= 0);

    // Inform main thread about successful startup
    JVX_SET_NOTIFICATION(_system.thePorts[idPort].threads.threadStart);

    while(_system.thePorts[idPort].threads.inLoopActive)
    {	
        nfds = epoll_wait(epollfd, events, MAX_EVENTS_EPOLL, -1);
        if(nfds < 0)
        {
            int errCode = errno;
#ifdef JVX_RS232_DEBUG_TXT
            std::cout << "Error in epoll, error code: " << strerror(errCode) << std::endl;
#endif
	    nfds = 0;
        }
        else
        {
#ifdef JVX_RS232_DEBUG_TXT
            std::cout << "Falling epoll, number events: " << nfds << std::endl;
#endif
        }

        // Process all events
		for (i = 0; i < nfds; ++i) 
		{
            if (events[i].data.fd == _system.thePorts[idPort].threads.eventfd)
            {
                std::cout << "Shutdown" << std::endl;
            }

			if (events[i].data.fd == _system.thePorts[idPort].fd) 
			{
				// Do something read

				// No error, hence, only return 
                ioctl(_system.thePorts[idPort].fd, FIONREAD, &valuesInQueue);
				while (valuesInQueue > 0)
				{
					if (_system.thePorts[idPort].cfg.secureChannel.receive.active)
					{
						jvxSize sz = valuesInQueue;
						sz = JVX_MIN(sz, _system.thePorts[idPort].sc.receive.szfld - _system.thePorts[idPort].sc.receive.offset_write);
						readNum = read(_system.thePorts[idPort].fd, _system.thePorts[idPort].sc.receive.fld +
							_system.thePorts[idPort].sc.receive.offset_write, sz);
						assert(readNum >= 0);


#ifdef JVX_RS232_DEBUG
                        printoutBuffer(idPort, _system.thePorts[idPort].sc.receive.fld + _system.thePorts[idPort].sc.receive.offset_read,
                            readNum, _system.thePorts[idPort].cfg.secureChannel.startByte,
                            _system.thePorts[idPort].cfg.secureChannel.stopByte,
                            _system.thePorts[idPort].cfg.secureChannel.escByte, "roff" +
                                       jvx_size2String(_system.thePorts[idPort].sc.receive.offset_read) + "-woff" +
                                       jvx_size2String(_system.thePorts[idPort].sc.receive.offset_write) + "_");
#endif
						valuesInQueue -= readNum;
						_system.thePorts[idPort].sc.receive.offset_write += readNum;

						while (1)
						{
							theSpecialChar = 0;
							sz = jvx_rs232_numberBytesUnwrapReceive(_system.thePorts[idPort].sc.receive.fld, _system.thePorts[idPort].sc.receive.offset_read,
								_system.thePorts[idPort].sc.receive.offset_write, _system.thePorts[idPort].cfg.secureChannel.startByte,
								_system.thePorts[idPort].cfg.secureChannel.stopByte, _system.thePorts[idPort].cfg.secureChannel.escByte,
								theSpecialChar);
							if (sz)
							{
								assert(_system.thePorts[idPort].theReport);
								_system.thePorts[idPort].theReport->provide_data_and_length(&ptrRead, &sz, &offset, idPort, NULL,
									JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

								numReadApp = sz;
								if (numReadApp > 0)
								{
									szread = 0;
									theSpecialChar = 0;

									jvx_rs232_removeByteWrapReplace(_system.thePorts[idPort].sc.receive.fld,
										_system.thePorts[idPort].sc.receive.offset_read,
										_system.thePorts[idPort].sc.receive.offset_write,
										ptrRead + offset, numReadApp, _system.thePorts[idPort].cfg.secureChannel.startByte,
										_system.thePorts[idPort].cfg.secureChannel.stopByte,
										_system.thePorts[idPort].cfg.secureChannel.escByte, szread, write, theSpecialChar);

									_system.thePorts[idPort].sc.receive.offset_read += szread;
									_system.thePorts[idPort].theReport->report_data_and_read(ptrRead, numReadApp, offset, idPort, &theSpecialChar, JVX_CONNECT_PRIVATE_ARG_TYPE_SECURE_CHANNEL_SPECIAL_CHAR);
								}
							}
							else
							{
								if (theSpecialChar != 0)
								{
									assert(_system.thePorts[idPort].theReport);
									_system.thePorts[idPort].theReport->provide_data_and_length(&ptrRead, &sz, &offset, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
									_system.thePorts[idPort].theReport->report_data_and_read(ptrRead, sz, offset, idPort, &theSpecialChar, JVX_CONNECT_PRIVATE_ARG_TYPE_SECURE_CHANNEL_SPECIAL_CHAR);
									_system.thePorts[idPort].sc.receive.offset_read++; // The special character HAS been read
								}
								else
								{
									jvxSize numMove = (_system.thePorts[idPort].sc.receive.offset_write -
										_system.thePorts[idPort].sc.receive.offset_read);
									if (numMove)
									{
										memmove(_system.thePorts[idPort].sc.receive.fld,
											_system.thePorts[idPort].sc.receive.fld + _system.thePorts[idPort].sc.receive.offset_read, numMove);
									}
									_system.thePorts[idPort].sc.receive.offset_read = 0;
									_system.thePorts[idPort].sc.receive.offset_write = numMove;

									break;
								}
							}
						} // while (1)
					} // if (_system.thePorts[idPort].cfg.secureChannel.receive.active)
					else
					{
						// Get buffer in which to store bytes
						jvxSize sz = valuesInQueue;
						jvxSize numReadApp = 0;
						jvxByte* ptrRead = NULL;
						jvxSize offset = 0;

						assert(_system.thePorts[idPort].theReport);
						_system.thePorts[idPort].theReport->provide_data_and_length(&ptrRead, &sz, &offset, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

						numReadApp = sz;
						if (numReadApp > 0)
						{
							assert(ptrRead);
							readNum = read(_system.thePorts[idPort].fd, ptrRead + offset, numReadApp);
							assert(readNum >= 0);
						}

#ifdef JVX_RS232_DEBUG
                        printoutBuffer(idPort, ptrRead + offset,
                            readNum, 0, 0, 0, "roff" + jvx_size2String(_system.thePorts[idPort].sc.receive.offset_read) + "-woff" +
                                       jvx_size2String(_system.thePorts[idPort].sc.receive.offset_write) + "_");
#endif

#ifdef JVX_RS232_DEBUG_TXT
                        std::cout << "<-- " << std::flush;
                        for(i = 0; i < readNum; i++)
                        {
                            std::cout << "<" << *(ptrRead + offset + i) << ":" << (int)*(ptrRead + offset + i) << ">" << std::flush;
                        }
                        std::cout << std::endl;
#endif
                        _system.thePorts[idPort].theReport->report_data_and_read(ptrRead, readNum, offset, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
						valuesInQueue -= readNum;
					}
				}// while (valuesInQueue > 0)
			}// if (events[i].data.fd == _system.thePorts[idPort].fd) 
		} // for (i = 0; i < nfds; ++i) 

	}// while(_system.thePorts[idPort].threads.inLoopActive)

    if (jvx_bitTest(_system.thePorts[idPort].cfg.reportEnum, JVX_RS232_REPORT_CONNECTION_STOPPED_SHIFT))
    {
        if (_system.thePorts[idPort].theReport)
        {
            jvxBitField mask;
            jvx_bitSet(mask, JVX_RS232_REPORT_CONNECTION_STOPPED_SHIFT);
            _system.thePorts[idPort].theReport->report_event(mask, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
        }
	}

	if (_system.thePorts[idPort].theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_CONNECTION_REPORT_SHUTDOWN_COMPLETE_SHFT);
		_system.thePorts[idPort].theReport->report_event(mask, idPort, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
	}

#ifdef JVX_RS232_DEBUG_TXT
    std::cout << __FUNCTION__ << " thread terminating..." << std::endl;
#endif
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
    jvxUInt64 tt = JVX_GET_TICKCOUNT_US_GET(&_system.thePorts[idPort].dbg.tRef);
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
    jvxUInt64 tt = JVX_GET_TICKCOUNT_US_GET(&_system.thePorts[idPort].dbg.tRef);
	if (fh)
	{
		fprintf(fh, "--> %s <" JVX_PRINTF_CAST_INT64 ">: ", token.c_str(), tt);
		fprintf(fh, "\n");
		fclose(fh);
	}
	JVX_UNLOCK_MUTEX(_system.thePorts[idPort].dbg.safeAccess);

}
#endif
