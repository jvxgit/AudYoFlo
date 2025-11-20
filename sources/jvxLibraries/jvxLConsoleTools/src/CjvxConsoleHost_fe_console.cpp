#include "CjvxConsoleHost_fe_console.h"
#include <signal.h>

#ifdef JVX_CONSOLE_WITH_END_TRIGGER
#include "jvx_console_tools.h"
#else
#include "jvxSpecialCharacterDefines.h"
#endif

static CjvxConsoleHost_fe_console* this_pointer = NULL;
void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        if(this_pointer)
        {
            this_pointer->trigger_sigint_ext();
        }
    }
}

//#define JVX_FE_CONSOLE_VERBOSE

CjvxConsoleHost_fe_console::CjvxConsoleHost_fe_console()
{
  timerCount = 0;
	JVX_INITIALIZE_MUTEX(safeAccessMemList);
	evLop = NULL;
	theState = JVX_STATE_INIT;
	printAllRequestsOutput = true;
	wantsRestart = true;
	config_noconsole = false;
	config.compact_out = true;
	noconsole_restart = false;
	JVX_INITIALIZE_NOTIFICATION_2WAIT(informStopLoop);
}

CjvxConsoleHost_fe_console::~CjvxConsoleHost_fe_console()
{
	JVX_TERMINATE_NOTIFICATION_2WAIT(informStopLoop);
	JVX_TERMINATE_MUTEX(safeAccessMemList);
}

jvxErrorType
CjvxConsoleHost_fe_console::add_reference_event_loop(IjvxEventLoopObject* eventLoop)
{
	if (theState == JVX_STATE_INIT)
	{
		if (evLop == NULL)
		{
			evLop = eventLoop;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ALREADY_IN_USE;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxConsoleHost_fe_console::clear_reference_event_loop(IjvxEventLoopObject* eventLoop)
{
	if (theState == JVX_STATE_INIT)
	{
		if (evLop == eventLoop)
		{
			evLop = NULL;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ===========================================================================

jvxErrorType
CjvxConsoleHost_fe_console::set_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend)
{
	if (theBackend)
	{
		if (linkedPriBackend.be == NULL)
		{
			linkedPriBackend.be = theBackend;
			linkedPriBackend.st = JVX_STATE_SELECTED;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxConsoleHost_fe_console::clear_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend)
{
	if (theBackend)
	{
		if (linkedPriBackend.be == theBackend)
		{
			assert(linkedPriBackend.st == JVX_STATE_SELECTED);
			linkedPriBackend.be = NULL;
			linkedPriBackend.st = JVX_STATE_NONE;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

// ===========================================================================

jvxErrorType
CjvxConsoleHost_fe_console::add_sec_reference_event_backend(IjvxEventLoop_backend* backend)
{
	std::list<jvxOneBackendAndState>::iterator elm = std::find(linkedSecBackends.begin(), linkedSecBackends.end(), backend);
	if (elm != linkedSecBackends.end())
	{
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	jvxOneBackendAndState newElm;
	newElm.be = backend;
	newElm.st = JVX_STATE_SELECTED;
	linkedSecBackends.push_back(newElm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_fe_console::clear_sec_reference_event_backend(IjvxEventLoop_backend* backend)
{
	std::list<jvxOneBackendAndState>::iterator elm = std::find(linkedSecBackends.begin(), linkedSecBackends.end(), backend);
	if (elm == linkedSecBackends.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	linkedSecBackends.erase(elm);
	return JVX_NO_ERROR;
}

// ===================================================================================

/*
void
CjvxConsoleHost_fe_console::start_timer(jvxSize time_diff_msec)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize mId = 0;
	std::string inputText = "This is the timer thread" + jvx_size2String(timerCount);
	timerCount++;
	JVX_LOCK_MUTEX(safeAccessMemList);

	jvxByte* newStr = NULL;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(newStr, jvxByte, inputText.size() + 1);
	memcpy(newStr, inputText.c_str(), inputText.size());

	res = evLop->event_schedule(
		static_cast<IjvxEventLoop_frontend*>(this), NULL,
		evBackend, NULL,
		&mId, newStr, JVX_EVENTLOOP_DATAFORMAT_STRING,
		JVX_EVENTLOOP_EVENT_TEXT_INPUT, JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE,
		JVX_EVENTLOOP_PRIORITY_TIMER, time_diff_msec);
	if (res == JVX_NO_ERROR)
	{
		oneAllocatedField theFld;
		theFld.buf = newStr;
		theFld.selector = mId;

		flds.push_back(theFld);
	}

	JVX_UNLOCK_MUTEX(safeAccessMemList);
}
*/

jvxErrorType
CjvxConsoleHost_fe_console::returns_from_start(jvxBool* doesReturn)
{
	if (doesReturn)
		*doesReturn = false;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_fe_console::report_want_to_shutdown_ext(jvxBool restart)
{
	if (config_noconsole)
	{
		noconsole_restart = restart;
		JVX_SET_NOTIFICATION(informShutdown);
		return JVX_NO_ERROR;

	}
	else
	{
		JVX_SET_NOTIFICATION_2WAIT(informStopLoop);
		noconsole_restart = restart;
		return JVX_NO_ERROR;
	}

	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxConsoleHost_fe_console::query_property(jvxFrontendSupportQueryType tp, jvxHandle* load)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool* ptrloadbool = (jvxBool*)load;
	switch (tp)
	{
	case jvxFrontendSupportQueryType::JVX_FRONTEND_READ_SUPPORTS_SHUTDOWN:
		*ptrloadbool = true; // <- supports restart
		if (config_noconsole)
		{
			*ptrloadbool = true;
		}
		break;	
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

jvxErrorType
CjvxConsoleHost_fe_console::trigger_sync(jvxFrontendTriggerType tp, jvxHandle* load, jvxBool blockedRun)
{
	jvxErrorType res = JVX_NO_ERROR;
	TjvxEventLoopElement evLElm;

	if (blockedRun)
	{
		evLElm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
	}
	else
	{
		evLElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
	}

	switch (tp)
	{
	case jvxFrontendTriggerType::JVX_FRONTEND_WRITE_SYNC_TRIGGER_EXT_FACTORY_INVITE:

		evLElm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		evLElm.priv_fe = NULL;
		evLElm.target_be = linkedPriBackend.be;
		evLElm.priv_be = NULL;

		evLElm.param = load;
		evLElm.paramType = JVX_EVENTLOOP_DATAFORMAT_HANDLE_PTR;

		evLElm.eventType = JVX_EVENTLOOP_EVENT_TRIGGER_EXTERNAL_MODULE_FACTORY_INVITE;
		
		evLElm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		evLElm.delta_t = JVX_SIZE_UNSELECTED;
		evLElm.autoDeleteOnProcess = c_false;

		res = evLop->event_schedule(&evLElm, NULL, NULL);
		break;
	case jvxFrontendTriggerType::JVX_FRONTEND_WRITE_SYNC_TRIGGER_EXT_FACTORY_UNINVITE:

		evLElm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		evLElm.priv_fe = NULL;
		evLElm.target_be = linkedPriBackend.be;
		evLElm.priv_be = NULL;

		evLElm.param = load;
		evLElm.paramType = JVX_EVENTLOOP_DATAFORMAT_HANDLE_PTR;

		evLElm.eventType = JVX_EVENTLOOP_EVENT_TRIGGER_EXTERNAL_MODULE_FACTORY_UNINVITE;
		evLElm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		evLElm.delta_t = JVX_SIZE_UNSELECTED;
		evLElm.autoDeleteOnProcess = c_false;

		res = evLop->event_schedule(&evLElm, NULL, NULL);
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

void 
CjvxConsoleHost_fe_console::console_wait()
{
	
	JVX_INITIALIZE_NOTIFICATION(informShutdown);
	// JVX_SIGNAL_TYPE serr = JVX_SIGNAL_INSTALL(SIGINT, sig_handler);
	JVX_WAIT_FOR_NOTIFICATION_I(informShutdown);
	JVX_WAIT_FOR_NOTIFICATION_II_INF(informShutdown);

	trigger_deactivate();
}

void
CjvxConsoleHost_fe_console::trigger_deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	TjvxEventLoopElement evLElm;
	jvxCBitField whatToDo = (jvxCBitField)-1;

	while (whatToDo != 0)
	{
		// Reset condition
		whatToDo = 0;

		evLElm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		evLElm.priv_fe = NULL;
		evLElm.target_be = linkedPriBackend.be;
		evLElm.priv_be = NULL;

		evLElm.param = &whatToDo;
		evLElm.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;

		evLElm.eventType = JVX_EVENTLOOP_EVENT_DEACTIVATE;
		evLElm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
		evLElm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		evLElm.delta_t = JVX_SIZE_UNSELECTED;
		evLElm.autoDeleteOnProcess = c_false;
		if (noconsole_restart)
		{
			wantsRestart = true;
		}
		else
		{
			wantsRestart = false;
		}

		res = evLop->event_schedule(&evLElm, NULL, NULL);

		/*
		if (jvx_bitTest(whatToDo, JVX_EVENT_CONTINUE_WANT_SHUTDOWN_SHIFT))
		{
			wantsRestart = false;
			break;
		}
		if (jvx_bitTest(whatToDo, JVX_EVENT_CONTINUE_WANT_SHUTDOWN_RESTART_SHIFT))
		{
			wantsRestart = true;
			break;
		}
		*/

		if (jvx_bitTest(whatToDo, JVX_EVENT_CONTINUE_POSTPONE_SHUTDOWN_SHIFT))
		{
			JVX_SLEEP_MS(100);
		}

		if (jvx_bitTest(whatToDo, JVX_EVENT_CONTINUE_WAIT_SCHEDULER_WANT_SHUTDOWN_SHIFT))
		{
			jvxBool seqRunning = true;
			while (seqRunning)
			{
				JVX_SLEEP_S(1);
				jvxCBitField bf = 0;
				linkedPriBackend.be->backend_status(&bf);
				seqRunning = jvx_bitTest(bf, jvxBackendStatusShiftOptions::JVX_BACKEND_STATUS_SCHEDULER_RUNNING_SHIFT);
			}
		}
	}
}

jvxErrorType
CjvxConsoleHost_fe_console::start(int argc, char* argv[])
{
	jvxSize i;
	std::string inputText;
    jvxSize posi = JVX_SIZE_UNSELECTED;
	jvxErrorType res = JVX_NO_ERROR;	

	if (theState == JVX_STATE_INIT)
	{
		config_noconsole = false;

		theState = JVX_STATE_PROCESSING;

		std::string exec = jvx_extractFileFromFilePath(argv[0]);
		commLine.assign_args((const char**)&argv[1], argc - 1, exec.c_str());

		TjvxEventLoopElement evLElm;

		evLElm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		evLElm.priv_fe = NULL;
		evLElm.target_be = linkedPriBackend.be;
		evLElm.priv_be = NULL;

		evLElm.param = static_cast<IjvxCommandLine*>(&commLine);
		evLElm.paramType = JVX_EVENTLOOP_DATAFORMAT_COMMAND_LINE;

		evLElm.eventType = JVX_EVENTLOOP_EVENT_ACTIVATE;
		evLElm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
		evLElm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		evLElm.delta_t = JVX_SIZE_UNSELECTED;
		evLElm.autoDeleteOnProcess = c_false;

		res = evLop->event_schedule(&evLElm, NULL, NULL);

		// Install Ctrl-C handler, this has no impact in Windows, by the way, https://docs.microsoft.com/en-us/previous-versions/xdkz3x12(v%3Dvs.140)
		this_pointer = this;
#if defined(JVX_OS_EMSCRIPTEN)
		assert(0);
#else	
		JVX_SIGNAL_TYPE serr = JVX_SIGNAL_INSTALL(SIGINT, sig_handler);
#endif

		/*
		* New console in linux:
		* https://stackoverflow.com/questions/26948723/checking-the-stdin-buffer-if-its-empty
		*/

#ifdef JVX_CONSOLE_WITH_END_TRIGGER

		jvxConsoleHdl hdlCons;
#if defined( JVX_OS_MACOSX) || defined(JVX_OS_IOS)

		hdlCons.prepare(informStopLoop[0]);
#else
		hdlCons.prepare(informStopLoop);
#endif

		// std::cout << "INITIALIZATION OF NEW CONSOLE LOOP" << std::endl;

#else
		// better have this:
		// http://forums.codeguru.com/showthread.php?343829-break-blocked-getchar-call
		JVX_UNBUFFERED_CHAR_STATE;
		JVX_PREPARE_UNBUFFERED_CHAR
		std::cout << "INITIALIZATION OF NEW CONSOLE LOOP" << std::endl;
#endif

		int myChar = JVX_CHARACTER_INPUT_ONE_LINE_TTY_V1;
		while (1)
		{
			jvxBool ctrlcpressed = false;

#ifdef JVX_FE_CONSOLE_VERBOSE
			std::cout << "Input character: <" << myChar << ">" << std::endl;
#endif

			if (myChar == JVX_CHARACTER_SPECIAL_CHAR_CTRLC)
			{
				// This special case will be handled later
				ctrlcpressed = true;
			}
			else if (myChar == JVX_CHARACTER_START_SPECIAL_CHAR_V1)
			{
				// Windows dos shell characters
                    // Special characters
                    /*
                    int myChar = 0;
                    for (i = 0; i < JVX_CHARACTER_REMOVE_SPECIAL_CHAR_NUM_V1; i++)
                    {
                        myChar = JVX_GET_UNBUFFERED_CHAR();
    #ifdef JVX_FE_CONSOLE_VERBOSE
                        std::cout << "ESC #" << i << " character: <" << myChar << ">" << std::endl;
    #endif
                    }
                  */
				  // Decision character
#ifdef JVX_CONSOLE_WITH_END_TRIGGER
				myChar = hdlCons.get_single_console_character();
#else
				myChar = JVX_GET_UNBUFFERED_CHAR();
#endif

				intptr_t myCharCptr = myChar;
				TjvxEventLoopElement elm;

				elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
				elm.priv_fe = NULL;
				elm.target_be = linkedPriBackend.be;
				elm.priv_be = NULL;

				elm.param = (jvxHandle*)(myCharCptr);
				elm.paramType = JVX_EVENTLOOP_DATAFORMAT_SPECIAL_CHAR;

				elm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
				elm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
				elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
				elm.delta_t = JVX_SIZE_UNSELECTED;
				elm.autoDeleteOnProcess = c_false;

				res = evLop->event_schedule(&elm, NULL, NULL);
			} 
			else if (myChar == JVX_CHARACTER_START_SPECIAL_CHAR_V2)
			{
				// Special characters
				for (i = 0; i < JVX_CHARACTER_REMOVE_SPECIAL_CHAR_NUM_V2; i++)
				{
#ifdef JVX_CONSOLE_WITH_END_TRIGGER
					myChar = hdlCons.get_single_console_character();
#else
					myChar = JVX_GET_UNBUFFERED_CHAR();
#endif
#ifdef JVX_FE_CONSOLE_VERBOSE
					std::cout << "ESC #" << i << " character: <" << myChar << ">" << std::endl;
#endif
					//if (myChar1 == 91) {
					//	int myChar2 = JVX_GET_UNBUFFERED_CHAR();
					//if (myChar2 == 51)
					//myChar2 = JVX_GET_UNBUFFERED_CHAR();

				}

#ifdef JVX_CONSOLE_WITH_END_TRIGGER
				myChar = hdlCons.get_single_console_character();
#else
				myChar = JVX_GET_UNBUFFERED_CHAR();
#endif

				if (myChar == 51)
				{
#ifdef JVX_CONSOLE_WITH_END_TRIGGER
					myChar = hdlCons.get_single_console_character();
#else
					myChar = JVX_GET_UNBUFFERED_CHAR();
#endif

#ifdef JVX_FE_CONSOLE_VERBOSE
					std::cout << "ESC #" << i << " character: <" << myChar << ">" << std::endl;
#endif
				}
#ifdef JVX_FE_CONSOLE_VERBOSE
				std::cout << "Special character: <" << myChar << ">" << std::endl;
#endif
				intptr_t myCharCptr = myChar;
				TjvxEventLoopElement elm;

				elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
				elm.priv_fe = NULL;
				elm.target_be = linkedPriBackend.be;
				elm.priv_be = NULL;

				elm.param = (jvxHandle*)(myCharCptr);
				elm.paramType = JVX_EVENTLOOP_DATAFORMAT_SPECIAL_CHAR;

				elm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
				elm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
				elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
				elm.delta_t = JVX_SIZE_UNSELECTED;
				elm.autoDeleteOnProcess = c_false;
				res = evLop->event_schedule(&elm, NULL, NULL);
			} 
			else
			{
				jvxCBitField whatToDo = (jvxCBitField)-1;
				if (
					((char)myChar == JVX_CHARACTER_INPUT_ONE_LINE_TTY_V1) ||
					((char)myChar == JVX_CHARACTER_INPUT_ONE_LINE_TTY_V2))
				{
#ifdef JVX_FE_CONSOLE_VERBOSE
					std::cout << "-Line complete character-" << std::endl;
#endif
					while (whatToDo != 0)
					{
						whatToDo = 0;

						TjvxEventLoopElement elm;

						elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
						elm.priv_fe = NULL;
						elm.target_be = linkedPriBackend.be;
						elm.priv_be = NULL;

						elm.param = (jvxHandle*)&whatToDo;
						elm.paramType = JVX_EVENTLOOP_DATAFORMAT_CHAR_LINE_COMPLETE;

						elm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
						elm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
						elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
						elm.delta_t = JVX_SIZE_UNSELECTED;
						elm.autoDeleteOnProcess = c_false;
						res = evLop->event_schedule(&elm, NULL, NULL);

						if (jvx_bitTest(whatToDo, JVX_EVENT_CONTINUE_WANT_SHUTDOWN_SHIFT))
						{
							wantsRestart = false;
							break;
						}
						if (jvx_bitTest(whatToDo, JVX_EVENT_CONTINUE_WANT_SHUTDOWN_RESTART_SHIFT))
						{
							wantsRestart = true;
							break;
						}
						if (jvx_bitTest(whatToDo, JVX_EVENT_CONTINUE_POSTPONE_SHUTDOWN_SHIFT))
						{
							JVX_SLEEP_MS(100);
						}
						if (jvx_bitTest(whatToDo, JVX_EVENT_CONTINUE_WAIT_SCHEDULER_WANT_SHUTDOWN_SHIFT))
						{
							jvxBool seqRunning = true;
							while (seqRunning)
							{
								JVX_SLEEP_S(1);
								jvxCBitField bf = 0;
								linkedPriBackend.be->backend_status(&bf);
								seqRunning = jvx_bitTest(bf, jvxBackendStatusShiftOptions::JVX_BACKEND_STATUS_SCHEDULER_RUNNING_SHIFT);
							}
						}
					}
					if (whatToDo != 0)
					{
						break;
					}
				} 
				else
				{
					intptr_t myCharCptr = myChar;
					if (myChar == JVX_CHARACTER_REMOVE_LAST_TTY) 
					{
#ifdef JVX_FE_CONSOLE_VERBOSE
						std::cout << "-Remove last character-" << std::endl;
#endif
						TjvxEventLoopElement elm;

						elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
						elm.priv_fe = NULL;
						elm.target_be = linkedPriBackend.be;
						elm.priv_be = NULL;

						elm.param = (jvxHandle*)(myCharCptr);
						elm.paramType = JVX_EVENTLOOP_DATAFORMAT_CHAR_REMOVE_LAST;

						elm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
						elm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
						elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
						elm.delta_t = JVX_SIZE_UNSELECTED;
						elm.autoDeleteOnProcess = c_false;
						res = evLop->event_schedule(&elm, NULL, NULL);

					} 
					else
					{
						TjvxEventLoopElement elm;

						elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
						elm.priv_fe = NULL;
						elm.target_be = linkedPriBackend.be;
						elm.priv_be = NULL;

						elm.param = (jvxHandle*)(myCharCptr);
						elm.paramType = JVX_EVENTLOOP_DATAFORMAT_CHAR;

						elm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
						elm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
						elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
						elm.delta_t = JVX_SIZE_UNSELECTED;
						elm.autoDeleteOnProcess = c_false;
						res = evLop->event_schedule(&elm, NULL, NULL);
					}
				}
				/*
				std::getline(std::cin, inputText);
				if (inputText == "q")
				{
					break;
				}
				else
				{
					JVX_LOCK_MUTEX(safeAccessMemList);

					std::string* newStr = new std::string;
					*newStr = inputText;
					res = evLop->event_schedule(
						static_cast<IjvxEventLoop_frontend*>(this), NULL,
						evBackend, NULL,
						&mId, (jvxHandle*)newStr, JVX_EVENTLOOP_DATAFORMAT_STDSTRING,
						JVX_EVENTLOOP_EVENT_TEXT_INPUT, JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE,
						JVX_EVENTLOOP_PRIORITY_NORMAL, JVX_SIZE_UNSELECTED);

					JVX_UNLOCK_MUTEX(safeAccessMemList);
				}*/
			}

			if (res == JVX_NO_ERROR)
			{
				res = handle_ctrl_c(ctrlcpressed, config_noconsole);
				if (res == JVX_ERROR_ABORT)
				{
					break;
				}
			}

			if (config_noconsole)
			{
				TjvxEventLoopElement elm;
				std::string mess = "Console host entering non-interactive mode. Console inputs will no longer be considered.";
				elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
				elm.priv_fe = NULL;
				elm.target_be = linkedPriBackend.be;
				elm.priv_be = NULL;

				elm.param = &mess;
				elm.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;

				elm.eventType = JVX_EVENTLOOP_EVENT_TEXT_SHOW;
				elm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
				elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
				elm.delta_t = JVX_SIZE_UNSELECTED;
				elm.autoDeleteOnProcess = c_true;

				res = evLop->event_schedule(&elm, NULL, NULL);
				console_wait();
				break;
			}
			else
			{
skip_null_char:
#ifdef JVX_CONSOLE_WITH_END_TRIGGER
				myChar = hdlCons.get_single_console_character();
#else
				myChar = JVX_GET_UNBUFFERED_CHAR();
#endif
				if (myChar == -2)
				{
					// Processing stop was triggered from external
					trigger_deactivate();
					break;
				}
				else if (myChar == EOF)
				{
					config_noconsole = true;
				}
				else if (myChar == 0)
				{
					// On windows, a null character is returned in case the Shift key is pushed
					goto skip_null_char;
				}
			}

			
        }
#ifdef JVX_CONSOLE_WITH_END_TRIGGER
		hdlCons.postprocess();
#else
		JVX_POSTPROCESS_UNBUFFERED_CHAR;
#endif
	
		commLine.unassign_args();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxConsoleHost_fe_console::stop()
{
	if (theState == JVX_STATE_PROCESSING)
	{
		theState = JVX_STATE_INIT;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxConsoleHost_fe_console::wants_restart(jvxBool *wRet)
{
	if (wRet)
	{
		*wRet = wantsRestart;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_fe_console::handle_ctrl_c(jvxBool ctrlcpressed, jvxBool config_noconsole)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numCtrlCMax = JVX_NUMBER_CTRLC_EXIT;
	if(config_noconsole)
	{
		numCtrlCMax = 1;
	}
	if (ctrlcpressed)
	{
		TjvxEventLoopElement elm;
		ctrlccounter++;
		if (ctrlccounter >= numCtrlCMax)
		{
		    std::cout << "Pressed Ctrl-C " << numCtrlCMax << " times, aborting program execution." << std::endl;
			
			if (config_noconsole)
			{
				trigger_sigint_ext_core();
			}
			else
			{
				elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
				elm.priv_fe = NULL;
				elm.target_be = linkedPriBackend.be;
				elm.priv_be = NULL;

				elm.param = NULL;
				elm.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;

				elm.eventType = JVX_EVENTLOOP_EVENT_DEACTIVATE;
				elm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
				elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
				elm.delta_t = JVX_SIZE_UNSELECTED;
				elm.autoDeleteOnProcess = c_false;
				wantsRestart = false;
				res = evLop->event_schedule(&elm, NULL, NULL);
				res = JVX_ERROR_ABORT;
				exit(0);
			}
		}
		else
		{
			intptr_t myCharCptr = JVX_CHARACTER_SPECIAL_CHAR_CTRLC;
			elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
			elm.priv_fe = NULL;
			elm.target_be = linkedPriBackend.be;
			elm.priv_be = NULL;

			elm.param = (jvxHandle*)(myCharCptr);
			elm.paramType = JVX_EVENTLOOP_DATAFORMAT_SPECIAL_CHAR;

			elm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
			elm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
			elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
			elm.delta_t = JVX_SIZE_UNSELECTED;
			elm.autoDeleteOnProcess = c_false;

			res = evLop->event_schedule(&elm, NULL, NULL);
		}
	}
	else
	{
		ctrlccounter = 0;
	}
	return res;
}

// =========================================================================
	
#define JVX_CREATE_ERROR_NO_RETURN(lstelmr, err, txt) \
	{ \
		CjvxJsonElement jelm; \
		jelm.makeAssignmentString("error_description", txt); \
		lstelmr.addConsumeElement(jelm); \
	} \

jvxErrorType 
CjvxConsoleHost_fe_console::report_process_event(TjvxEventLoopElement* theQueueElement)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxSize message_id = theQueueElement->message_id;
	jvxHandle* param = theQueueElement->param;
	jvxSize paramType = theQueueElement->paramType;
	jvxSize event_type = theQueueElement->eventType;
	jvxOneEventClass event_class = theQueueElement->eventClass;
	jvxOneEventPriority event_priority = theQueueElement->eventPriority;
	jvxCBool autoDeleteOnProcess = theQueueElement->autoDeleteOnProcess;
	jvxHandle* privBlock = NULL;
	std::string command;
	std::string f_expr;
	std::vector<std::string> args;
	jvxErrorType resParse;
	CjvxJsonElement jelm_result;
	CjvxJsonElementList jlstresp;
	std::string errTxt;

	if (theQueueElement->oneHdlBlock)
	{
		privBlock = theQueueElement->oneHdlBlock->priv;
	}


	switch (event_type)
	{
	case JVX_EVENTLOOP_EVENT_SELECT:

		// Do nothing
		break;
	case JVX_EVENTLOOP_EVENT_ACTIVATE:
		// Here, we might obtain the host reference

		/*
		res = myHostRef->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&config.hostRefConfigExtender));
		if ((res == JVX_NO_ERROR) && config.hostRefConfigExtender)
		{
			res = config.hostRefConfigExtender->register_extension(static_cast<IjvxConfigurationExtender_report*>(this), JVX_WEBCONTROL_SECTION);
		}
		*/

		break;
	case JVX_EVENTLOOP_EVENT_DEACTIVATE:

		// Clear event loop
		evLop->frontend_block(static_cast<IjvxEventLoop_frontend*>(this));
		evLop->event_clear(JVX_SIZE_UNSELECTED, static_cast<IjvxEventLoop_frontend*>(this), NULL);

		// Not sure: is the break desired or not? It was not there 0n 04.01.2019 HK
		break;

	case JVX_EVENTLOOP_EVENT_UNSELECTED:
		break;
	case JVX_EVENTLOOP_EVENT_TEXT_INPUT:

		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_STDSTRING);
		command = *((std::string*)param);
		if (
			(theQueueElement->target_be == NULL) &&
			(theQueueElement->eventClass == JVX_EVENTLOOP_REQUEST_IMMEDIATE))
		{
			if (command == "help")
			{
				std::cout << "\thost(<address>, <varargs>) : Commando to address the host frontend." << std::endl;
				std::cout << "\t\t<address>: config: address the web backend config parameters" << std::endl;
				std::cout << "\t\t\t<varags>: empty: return all config parameters" << std::endl;
				std::cout << "\t\t\t<varags>: compact_out: return all system returns in compact mode or as a full JSON string (default: yes)" << std::endl;
				std::cout << "\t\t\t<varags>: compact_out, yes/no: set current value for compact_out configuration" << std::endl;
				return JVX_ERROR_POSTPONE;
			}
			else
			{
				resParse = jvx_parseCommandIntoToken(command, f_expr, args);
				if (resParse == JVX_NO_ERROR)
				{
					if (f_expr == "host")
					{
						if (args.size() > 0)
						{
							if ((args[0] == "config") )//|| (args[0] == "no"))
							{
								if (args.size() == 1)
								{
									if (config.compact_out)
									{
										jelm_result.makeAssignmentString("compact_out", "yes");
									}
									else
									{
										jelm_result.makeAssignmentString("compact_out", "no");
									}
									jlstresp.insertConsumeElementFront(jelm_result);

									jelm_result.makeAssignmentString("return_code", jvxErrorType_str[JVX_NO_ERROR].friendly);
									jlstresp.insertConsumeElementFront(jelm_result);
								}
								else
								{
									if (args[1] == "compact_out")
									{
										if (args.size() > 2)
										{
											if (args[2] == "yes")
											{
												config.compact_out = true;
												jelm_result.makeAssignmentString("return_code", jvxErrorType_str[JVX_NO_ERROR].friendly);
												jlstresp.insertConsumeElementFront(jelm_result);
											}
											else if (args[2] == "no")
											{
												config.compact_out = false;
												jelm_result.makeAssignmentString("return_code", jvxErrorType_str[JVX_NO_ERROR].friendly);
												jlstresp.insertConsumeElementFront(jelm_result);
											}
											else
											{
												resParse = JVX_ERROR_INVALID_ARGUMENT;
												errTxt = "Specification of compact_out expects <yes> or <no>. Instead, ";
												errTxt += args[2];
												errTxt += " was specified.";
												JVX_CREATE_ERROR_NO_RETURN(jlstresp, resParse, errTxt);
											}
										}
										else
										{
											if (config.compact_out)
											{
												jelm_result.makeAssignmentString("compact_out", "yes");
											}
											else
											{
												jelm_result.makeAssignmentString("compact_out", "no");
											}
											jlstresp.insertConsumeElementFront(jelm_result);

											jelm_result.makeAssignmentString("return_code", jvxErrorType_str[JVX_NO_ERROR].friendly);
											jlstresp.insertConsumeElementFront(jelm_result);
										}
									}
									else
									{
										resParse = JVX_ERROR_INVALID_ARGUMENT;
										errTxt = "Specification of config command <";
										errTxt += args[1];
										errTxt += "> is invalid.";
										JVX_CREATE_ERROR_NO_RETURN(jlstresp, resParse, errTxt);
									}
								}
							}
							else
							{
								resParse = JVX_ERROR_INVALID_ARGUMENT;
								errTxt = "Specification of config command <";
								errTxt += args[0];
								errTxt += "> is invalid.";
								JVX_CREATE_ERROR_NO_RETURN(jlstresp, resParse, errTxt);
							}
						}
						else
						{
							resParse = JVX_ERROR_INVALID_ARGUMENT;
							errTxt = "Specification of no command is invalid.";
							JVX_CREATE_ERROR_NO_RETURN(jlstresp, JVX_ERROR_INVALID_ARGUMENT, errTxt);
						}
					}
					else
					{
						// This command is not for me..
						resParse = JVX_ERROR_INVALID_ARGUMENT;
					}
				}

				if (resParse == JVX_NO_ERROR)
				{
					TjvxEventLoopElement qel;
					qel.message_id = message_id;
					qel.param = &jlstresp;
					qel.paramType = JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS;
					qel.eventClass = event_class;
					qel.eventPriority = event_priority;
					qel.origin_fe = this;

					report_assign_output(&qel, res, privBlock);
				} // if (f_expr == "web")
				return resParse;
			}
		}

		break;
	default:
		res = cleanup(message_id, param, paramType, event_type, event_class, event_priority, autoDeleteOnProcess);
		break;
	}
	return res;
}

jvxErrorType
CjvxConsoleHost_fe_console::report_cancel_event(TjvxEventLoopElement* theQueueElement)
{
	//std::cout << "Message cancelled." << std::endl;
	jvxErrorType res = cleanup(theQueueElement->message_id, theQueueElement->param, 
		theQueueElement->paramType, theQueueElement->eventType, 
		theQueueElement->eventClass, theQueueElement->eventPriority, 
		theQueueElement->autoDeleteOnProcess);
	return res;
}

jvxErrorType
CjvxConsoleHost_fe_console::cleanup(
	jvxSize message_id,
	jvxHandle* param, jvxSize paramType,
	jvxSize event_type, jvxOneEventClass event_class, 
	jvxOneEventPriority event_priority,
	jvxCBool autoDeleteOnProcess)
{
	std::string* newStr = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	if (!autoDeleteOnProcess)
	{
		JVX_LOCK_MUTEX(safeAccessMemList);
		std::vector<oneAllocatedField>::iterator elm = flds.end();
		elm = jvx_findItemSelectorInList < oneAllocatedField, jvxSize>(flds, message_id);
		if (elm != flds.end())
		{
			switch (paramType)
			{
			case JVX_EVENTLOOP_DATAFORMAT_STDSTRING:
				newStr = (std::string*)elm->buf;
				JVX_DSP_SAFE_DELETE_OBJECT(newStr);
				break;
			default:
				res = JVX_ERROR_INVALID_SETTING;
				break;
			}
			flds.erase(elm);
		}
		JVX_UNLOCK_MUTEX(safeAccessMemList);
	}
	return res;
}

jvxErrorType 
CjvxConsoleHost_fe_console::report_assign_output(TjvxEventLoopElement* theQueueElement, jvxErrorType sucOperation, jvxHandle* priv)
{
	std::string out_string;
	CjvxJsonElementList* jlst = (CjvxJsonElementList*)theQueueElement->param;
	assert(jlst);

	if (
		(theQueueElement->origin_fe == static_cast<IjvxEventLoop_frontend*>(this)) ||
		(printAllRequestsOutput))
	{
		if (config.compact_out)
		{
			jlst->printToStringView(out_string);
			std::cout << std::endl << out_string << std::endl;
		}
		else
		{
			jlst->printToJsonView(out_string);
			std::cout << std::endl << out_string << std::endl;
		}
	}

	// We do not need this for the 
	return JVX_NO_ERROR;
};

jvxErrorType
CjvxConsoleHost_fe_console::report_special_event(TjvxEventLoopElement* theQueueElement, jvxHandle* priv)
{
	assert(theQueueElement->paramType == JVX_EVENTLOOP_DATAFORMAT_SPECIAL_EVENT);
	assert(theQueueElement->param);

	jvxSpecialEventType* theEvent = (jvxSpecialEventType*)theQueueElement->param;
	switch (theEvent->ev)
	{
	case JVX_EVENTLOOP_SPECIAL_EVENT_SYSTEM_UPDATE:
		std::cout << "Report System Update." << std::endl;
		break;
	case JVX_EVENTLOOP_SPECIAL_EVENT_SEQUENCER_UPDATE:

		// output_sequencer_event(theEvent->dat);
		// Do nothing here
		break;
	default:
		std::cout << "Unknown Special Event Report." << std::endl;
	}
	return JVX_NO_ERROR;
}

#if 0 
void 
CjvxConsoleHost_fe_console::output_sequencer_event(jvxSpecialEventType_seq* seq)
{
	/*
	jvxUInt64 tt = JVX_GET_TICKCOUNT_US_GET(&runtime.myTimerRef);
	jvxData tt_msec = (jvxData)tt * 0.001; // in msecs
	std::string txt = "[" + jvx_data2String(tt_msec, 2) + " msec]";
	*/
	std::string txt;

if (seq->event_mask & JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE)
{
	txt += "Startup complete, message: ";
	txt += seq->description;
	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
	postMessage_outThread(txt);
}

if (event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP)
{
	txt += "Successful completion sequence " + jvx_size2String(sequenceId) + ", step " + jvx_size2String(stepId) + ", ";
	switch (tp)
	{
	case JVX_SEQUENCER_QUEUE_TYPE_RUN:
		txt += "run queue, message: ";
		break;
	case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
		txt += "leave queue, message: ";
		break;
	default:
		txt += "no queue, message: ";
		break;
	}

	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
	postMessage_outThread(txt);

	if (stp == JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER)
	{
		// What can we do here?
	}
}
if (event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR)
{
	txt += "Incomplete completion sequence " + jvx_size2String(sequenceId) + ", step " + jvx_size2String(stepId) + ", ";
	switch (tp)
	{
	case JVX_SEQUENCER_QUEUE_TYPE_RUN:
		txt += "run queue, due to error, message: ";
		break;
	case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
		txt += "leave queue, due to error, message: ";
		break;
	default:
		txt += "no queue, message: ";
		break;
	}
	txt += description;
	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_ERROR);
	postMessage_outThread(txt);

	if (runtime.firstError.empty())
	{
		runtime.firstError = description;
	}
}

if (event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE)
{
	txt += "Successful completion sequence " + jvx_size2String(sequenceId) + ", message ";
	txt += description;
	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
	postMessage_outThread(txt);

}
if (event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR)
{
	txt += "Incomplete completion sequence " + jvx_size2String(sequenceId) +
		" due to error, message: ";
	txt += description;
	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_ERROR);
	postMessage_outThread(txt);
}

if (event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT)
{
	txt += "Incomplete completion sequence " + jvx_size2String(sequenceId) +
		" due to abort, message: ";
	txt += description;
	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_INFO);
	postMessage_outThread(txt);
}

if (event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS)
{
	txt += "Successful completion process.";
	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
	linestart = JVX_DEFINE_PROCESSING_OFF;
	postMessage_outThread(txt);

}
if (event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR)
{
	txt += "Incomplete completion process error, message: ";
	txt += description;
	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_ERROR);
	linestart = JVX_DEFINE_PROCESSING_OFF;
	postMessage_outThread(txt);

}
if (event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT)
{
	txt += "Process aborted.";
	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_WARNING);
	linestart = JVX_DEFINE_PROCESSING_OFF;
	postMessage_outThread(txt);
}
if (event_mask & JVX_SEQUENCER_EVENT_PROCESS_TERMINATED)
{
	txt += "Process terminated.";
	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
	postMessage_outThread(txt);
}

if (event_mask & JVX_SEQUENCER_EVENT_DEBUG_MESSAGE)
{
	/*
	if (checkBox_debug_sequencer->isChecked())
	{
		txt += description;
		runtime.report->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
	}*/
}

if (event_mask & JVX_SEQUENCER_EVENT_INFO_TEXT)
{
	txt += description;
	txt = textMessagePrioToString(txt, JVX_REPORT_PRIORITY_INFO);
	postMessage_outThread(txt);
}
#endif


jvxErrorType
CjvxConsoleHost_fe_console::report_register_fe_commandline(IjvxCommandLine* comLine)
{
	if (comLine)
	{
		comLine->register_option("--no-console-input", "", "Start host to not expect any user input.");		
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_fe_console::report_readout_fe_commandline(IjvxCommandLine* comLine)
{
	jvxSize num = 0;
	if (comLine)
	{
		comLine->number_entries_option("--no-console-input", &num);
		if (num)
		{
			config_noconsole = true;
		}		
	}
	return JVX_NO_ERROR;
}

void
CjvxConsoleHost_fe_console::trigger_sigint_ext()
{
    handle_ctrl_c(true, config_noconsole);
}

void
CjvxConsoleHost_fe_console::trigger_sigint_ext_core()
{
    JVX_SET_NOTIFICATION(informShutdown);
}


jvxErrorType
CjvxConsoleHost_fe_console::request_if_command_forward(IjvxReportSystemForward** fwdCalls)
{
	if (fwdCalls)
	{
		*fwdCalls = nullptr;
	}
	return JVX_ERROR_UNSUPPORTED;
}
