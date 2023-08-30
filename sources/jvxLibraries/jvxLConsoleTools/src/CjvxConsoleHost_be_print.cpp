#include "CjvxConsoleHost_be_print.h"
#include "jvxSpecialCharacterDefines.h"

//#define JVX_BE_PRINT_VERBOSE

static std::string HIGHLIGHT(const std::string& in, const std::string& hll, const std::string& hlr, jvxSize posi, jvxSize* fromleft = NULL)
{
	std::string out = in;
	if (fromleft)
		*fromleft = posi;

	if (posi < in.size())
	{
		out = in.substr(0, posi);
		out += hll;
		out += in.substr(posi, 1);
		if (posi + 1 < in.size())
		{
			out += hlr;
			out += in.substr(posi + 1, std::string::npos);
		}
		if (fromleft)
			*fromleft += hll.size();

	}
	return out;
}


static void CLEANLINE(const std::string& txt, const std::string& hll, const std::string& hlr, const std::string& linestart, jvxSize insert)
{
	jvxSize i;
	std::string txtH = HIGHLIGHT(txt, hll, hlr, insert);
	std::cout << "\r " << linestart << std::flush;
	for (i = 0; i < txtH.size(); i++)
	{
		std::cout << " " << std::flush;
	}
}


/*
#define PRINTLINE(txt, insert, linestart); \
	std::cout << "\r " << linestart << internaltextbuffer << std::flush; \
	std::cout << "\r " << linestart << internaltextbuffer.substr(0, insert) << std::flush;
*/
static void PRINTLINE(const std::string& internaltextbuffer, const std::string& hll, const std::string& hlr, jvxSize insert, const std::string& linestart)
{
	jvxSize posi = 0;
	std::string txt = HIGHLIGHT(internaltextbuffer, hll, hlr, insert, &posi);
	std::cout << "\r " << linestart << txt << std::flush;
	if (posi > 0)
	{
		std::cout << "\r " << linestart << txt.substr(0, posi) << std::flush;
	}
}


CjvxConsoleHost_be_print::CjvxConsoleHost_be_print()
{
	this->evLop = NULL;
	theState = JVX_STATE_INIT;
	insert_position = 0;
	linestart = "##>";
	highlight_left = ".";
	highlight_right = ".";
	cntMem = 0;
	arrowupdownmode = false;
}


CjvxConsoleHost_be_print::~CjvxConsoleHost_be_print()
{
}

jvxErrorType 
CjvxConsoleHost_be_print::add_reference_event_loop(IjvxEventLoopObject* eventLoop)
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
CjvxConsoleHost_be_print::clear_reference_event_loop(IjvxEventLoopObject* eventLoop)
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

jvxErrorType
CjvxConsoleHost_be_print::set_pri_reference_frontend(IjvxEventLoop_frontend* theFrontend)
{
	if (theFrontend)
	{
		if (linkedPriFrontend.fe == NULL)
		{
			linkedPriFrontend.fe = theFrontend;
			linkedPriFrontend.st = JVX_STATE_INIT;

			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxConsoleHost_be_print::clear_pri_reference_frontend(IjvxEventLoop_frontend* theFrontend)
{
	if (linkedPriFrontend.fe == theFrontend)
	{
		if (linkedPriFrontend.st == JVX_STATE_INIT)
		{
			linkedPriFrontend.fe = NULL;
			linkedPriFrontend.st = JVX_STATE_NONE;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxConsoleHost_be_print::add_sec_reference_frontend(IjvxEventLoop_frontend* theFrontend) 
{
	std::list<jvxOneFrontendAndState>::iterator elm = std::find(linkedSecFrontends.begin(), linkedSecFrontends.end(), theFrontend);
	if (elm != linkedSecFrontends.end())
	{
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	jvxOneFrontendAndState newElm;
	newElm.fe = theFrontend;
	newElm.st = JVX_STATE_INIT;
	newElm.fwd = nullptr;
	if (newElm.fe)
	{
		newElm.fe->request_if_command_forward(&newElm.fwd);
	}
	linkedSecFrontends.push_back(newElm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_be_print::clear_sec_reference_frontend(IjvxEventLoop_frontend* theFrontend)
{
	std::list<jvxOneFrontendAndState>::iterator elm = std::find(linkedSecFrontends.begin(), linkedSecFrontends.end(), theFrontend);
	if (elm == linkedSecFrontends.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	linkedSecFrontends.erase(elm);
	return JVX_NO_ERROR;

}


jvxErrorType
CjvxConsoleHost_be_print::set_pri_reference_backend(IjvxEventLoop_backend* thebackend)
{
	if (thebackend)
	{
		if (linkedPriBackend.be == NULL)
		{
			linkedPriBackend.be = thebackend;
			linkedPriBackend.st = JVX_STATE_SELECTED;

			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxConsoleHost_be_print::clear_pri_reference_backend(IjvxEventLoop_backend* thebackend)
{
	if (linkedPriBackend.be == thebackend)
	{
		if (linkedPriBackend.st == JVX_STATE_SELECTED)
		{
			linkedPriBackend.be = NULL;
			linkedPriBackend.st = JVX_STATE_NONE;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxConsoleHost_be_print::add_sec_reference_backend(IjvxEventLoop_backend* thebackend)
{
	std::list<jvxOneBackendAndState>::iterator elm = std::find(linkedSecBackends.begin(), linkedSecBackends.end(), thebackend);
	if (elm != linkedSecBackends.end())
	{
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	jvxOneBackendAndState newElm;
	newElm.be = thebackend;
	newElm.st = JVX_STATE_SELECTED;
	linkedSecBackends.push_back(newElm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_be_print::clear_sec_reference_backend(IjvxEventLoop_backend* thebackend)
{
	std::list<jvxOneBackendAndState>::iterator elm = std::find(linkedSecBackends.begin(), linkedSecBackends.end(), thebackend);
	if (elm == linkedSecBackends.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	linkedSecBackends.erase(elm);
	return JVX_NO_ERROR;

}



jvxErrorType
CjvxConsoleHost_be_print::add_external_interface(jvxInterfaceType tp, jvxHandle* theIf)
{
	// std::map<jvxInterfaceType, oneRegisteredInterface> registeredInterfaceTypes;
	std::map<jvxInterfaceType, oneRegisteredInterface>::iterator elm = registeredInterfaceTypes.find(tp);
	if (elm == registeredInterfaceTypes.end())
	{
		oneRegisteredInterface newIfType;
		newIfType.tp = tp;
		newIfType.theIfs.push_back(theIf);
		registeredInterfaceTypes[newIfType.tp] = newIfType;
		return JVX_NO_ERROR;
	}
	std::list<jvxHandle*>::iterator elmp = std::find(elm->second.theIfs.begin(), elm->second.theIfs.end(), theIf);
	if (elmp == elm->second.theIfs.end())
	{
		elm->second.theIfs.push_back(theIf);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType
CjvxConsoleHost_be_print::remove_external_interface(jvxInterfaceType tp, jvxHandle* theIf)
{
	// std::map<jvxInterfaceType, oneRegisteredInterface> registeredInterfaceTypes;
	std::map<jvxInterfaceType, oneRegisteredInterface>::iterator elm = registeredInterfaceTypes.find(tp);
	if (elm == registeredInterfaceTypes.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	std::list<jvxHandle*>::iterator elmp = std::find(elm->second.theIfs.begin(), elm->second.theIfs.end(), theIf);
	if (elmp == elm->second.theIfs.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	elm->second.theIfs.erase(elmp);
	if (elm->second.theIfs.size() == 0)
	{
		registeredInterfaceTypes.erase(elm);
	}
	return JVX_NO_ERROR;
}

// =====================================================================================

jvxErrorType 
CjvxConsoleHost_be_print::start_receiving()
{
	jvxErrorType res;
	std::string content;
	std::string oneLine;

	if (theState == JVX_STATE_INIT)
	{
		res = jvx_readContentFromFile(JVX_CONSOLE_FILENAME, content, NULL);
		if (res == JVX_NO_ERROR)
		{
			// Read the history
			jvxSize cnt = 0;
			while (1)
			{
				if (cnt < content.size())
				{
					if (content[cnt] == '\n')
					{
						oneLine = content.substr(0, cnt);
						content = content.substr(cnt + 1, std::string::npos);
						cnt = 0;
						if (!oneLine.empty())
						{
							internaltextbuffer_mem.push_back(oneLine);
						}
					}
					else
					{
						cnt++;
					}
				}
				else
				{
					break;
				}
			}
			cntMem = internaltextbuffer_mem.size();
		}
		theState = JVX_STATE_PROCESSING;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxConsoleHost_be_print::stop_receiving()
{
	std::string content;
	jvxSize numEntries;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	if (theState == JVX_STATE_PROCESSING)
	{
		theState = JVX_STATE_INIT;
		numEntries = internaltextbuffer_mem.size();
		numEntries = JVX_MIN(numEntries, JVX_NUMBER_HISTORY_STORE);
		for (i = (internaltextbuffer_mem.size() - numEntries); i < internaltextbuffer_mem.size(); i++)
		{
			if (!internaltextbuffer_mem[i].empty())
			{
				content += internaltextbuffer_mem[i] + "\n";
			}
		}
		res = jvx_writeContentToFile(JVX_CONSOLE_FILENAME, content, NULL);

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxConsoleHost_be_print::wants_restart(jvxBool *wRet)
{
	if (wRet)
	{
		*wRet = false;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_be_print::process_event(TjvxEventLoopElement* theQueueElement)
	/*
	jvxSize message_id, IjvxEventLoop_frontend* origin, jvxHandle* priv_fe,
	jvxHandle* param, jvxSize paramType,
	jvxSize event_type, jvxOneEventClass event_class, 
	jvxOneEventPriority event_priority, jvxCBool deleteOnProcess,
	jvxHandle* privBlock, jvxBool* reschedule*/
{
	jvxSize message_id = theQueueElement->message_id;
	IjvxEventLoop_frontend* origin = theQueueElement->origin_fe;
	jvxHandle* priv_fe = theQueueElement->priv_fe;
	jvxHandle* param = theQueueElement->param;
	jvxSize paramType = theQueueElement->paramType;
	jvxSize event_type = theQueueElement->eventType;
	jvxOneEventClass event_class = theQueueElement->eventClass;
	jvxOneEventPriority event_priority = theQueueElement->eventPriority;
	jvxCBool autoDeleteOnProcess = theQueueElement->autoDeleteOnProcess;
	jvxBool* reschedule = &theQueueElement->rescheduleEvent;

	jvxHandle* privBlock = NULL;
	if (theQueueElement->oneHdlBlock)
	{
		privBlock = theQueueElement->oneHdlBlock->priv;
	}

	std::string txt;
	std::string* showStr;
	jvxErrorType res = JVX_NO_ERROR;
	char myChar;
	jvxSize i;
	char myTxt[2] = { 0 };
	jvxCBitField* whatToDo = NULL;
	jvxBool dontclearline = false;

	switch (event_type)
	{
	case JVX_EVENTLOOP_EVENT_ACTIVATE:
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_COMMAND_LINE);
		threadIdMainLoop = JVX_GET_CURRENT_THREAD_ID();
		this->process_init((IjvxCommandLine*)param);
		break;
	case JVX_EVENTLOOP_EVENT_DEACTIVATE:
		this->process_shutdown();
		threadIdMainLoop = JVX_THREAD_ID_INVALID;
		break;
	case JVX_EVENTLOOP_EVENT_TEXT_SHOW:
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_STDSTRING);
		showStr = (std::string*)param;
		CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
		std::cout << *showStr << std::endl;
		PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
		break;	

	case JVX_EVENTLOOP_EVENT_TEXT_INPUT:
		switch (paramType)
		{
		case JVX_EVENTLOOP_DATAFORMAT_JVXAPISTRING:
			txt = ((jvxApiString*)param)->std_str();
			this->process_full_command(txt, message_id, param, paramType, event_class, event_priority, origin, privBlock, NULL);
			PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
			break;

		case JVX_EVENTLOOP_DATAFORMAT_STDSTRING:
			txt = *((std::string*)param);
			this->process_full_command(txt, message_id, param, paramType, event_class, event_priority, origin, privBlock, NULL);
			PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
			break;
		case JVX_EVENTLOOP_DATAFORMAT_CHAR_LINE_COMPLETE:

			whatToDo = (jvxCBitField*)param;
			assert(whatToDo);
			*whatToDo = 0;

			arrowupdownmode = false;

			if (internaltextbuffer == "help")
			{
				store_entry_console_list(internaltextbuffer);
				this->process_help_command(origin);
			}
			else if (internaltextbuffer == "quit")
			{
				if (!config_noquit)
				{
					store_entry_console_list(internaltextbuffer);
					res = this->process_shutdown();

					switch (res)
					{
					case JVX_NO_ERROR:
						jvx_cbitSet(*whatToDo, JVX_EVENT_CONTINUE_WANT_SHUTDOWN_SHIFT);
						internaltextbuffer.clear();
						dontclearline = true;
						break;
					case JVX_ERROR_NOT_READY:
						jvx_cbitSet(*whatToDo, JVX_EVENT_CONTINUE_POSTPONE_SHUTDOWN_SHIFT);
						internaltextbuffer.clear();
						dontclearline = true;
						break;
					}
				}
				else
				{
					std::cout << "Command <quit> is not allowed in this configuration mode." << std::endl;
				}
			}
			else
			{
				if (!internaltextbuffer.empty())
				{
					store_entry_console_list(internaltextbuffer);					
					this->process_full_command(internaltextbuffer, message_id, param, paramType, event_class, event_priority, origin, privBlock, whatToDo);
					if (
						(jvx_bitTest(*whatToDo, JVX_EVENT_CONTINUE_WANT_SHUTDOWN_SHIFT)) ||
						(jvx_bitTest(*whatToDo, JVX_EVENT_CONTINUE_WANT_SHUTDOWN_RESTART_SHIFT)))
					{
						res = this->process_shutdown();
						internaltextbuffer.clear();
						dontclearline = true;
					}
					else
					{
						cntMem = internaltextbuffer_mem.size();
					}
				}
			}
			if (!dontclearline)
			{
				CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
				internaltextbuffer.clear();
				insert_position = 0;
				PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
			}

			break;
		case JVX_EVENTLOOP_DATAFORMAT_CHAR_REMOVE_LAST:
			arrowupdownmode = false;

			CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
			if (insert_position == internaltextbuffer.size())
			{
				internaltextbuffer = internaltextbuffer.substr(0, internaltextbuffer.size() - 1);
				insert_position = internaltextbuffer.size();
			}
			else if (insert_position > 0)
			{
				std::string tmp1;
				std::string tmp2;

				if (internaltextbuffer.size())
				{
					tmp1 = internaltextbuffer.substr(0, insert_position - 1);
				}
				if (insert_position < internaltextbuffer.size())
				{
					tmp2 = internaltextbuffer.substr(insert_position, std::string::npos);
				}
				
				internaltextbuffer = tmp1 + tmp2;
				insert_position--;
			}
			
			PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
			//printf("\r%s", internaltextbuffer.c_str());
		  break;
		case JVX_EVENTLOOP_DATAFORMAT_CHAR:
			arrowupdownmode = false;

			myChar = (char)((intptr_t)param);
			switch (myChar)
			{
			case '\r':
				assert(0); // <- should not be handled here
				break;
			case '\n':
				assert(0); // <- should not be handled here
				break;
			case '\b':
				assert(0); // <- should not be handled here
				break;
			case 0x7f:
				assert(0); // <- should not be handled here
				break;
			default:
				myTxt[0] = myChar;

				CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
				if (insert_position < internaltextbuffer.size())
				{
					std::string tmp1 = internaltextbuffer.substr(0, insert_position);
					std::string tmp2 = internaltextbuffer.substr(insert_position, std::string::npos);
					internaltextbuffer = tmp1 + myTxt + tmp2;
				}
				else
				{
					internaltextbuffer += myTxt;
				}
				insert_position++;
				//printf("\r");
				PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);

				// printf("%s", internaltextbuffer.c_str());
				break;
			}
			break;
		case JVX_EVENTLOOP_DATAFORMAT_SPECIAL_CHAR:

#ifdef JVX_BE_PRINT_VERBOSE
			std::cout << "Special character" << std::endl;
#endif
			myChar = (char)((intptr_t)param);
			switch (myChar)
			{
			case JVX_CHARACTER_SPECIAL_CHAR_CTRLC:

				arrowupdownmode = false;
				CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
				internaltextbuffer.clear();
				PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
				break;

			case JVX_ASCI_CODE_ARROW_UP_TTY:

#ifdef JVX_BE_PRINT_VERBOSE
				std::cout << "Arrow Up" << std::endl;
	#endif
				if (!arrowupdownmode)
				{
					user_input = internaltextbuffer;
					arrowupdownmode = true;
					cntMem = internaltextbuffer_mem.size();
				}

				handle_arrow_up();

				break;
			case JVX_ASCI_CODE_ARROW_DOWN_TTY:

#ifdef JVX_BE_PRINT_VERBOSE
				std::cout << "Arrow Down" << std::endl;
#endif
				if (!arrowupdownmode)
				{
					user_input = internaltextbuffer;
					arrowupdownmode = true;
					cntMem = 0;
				}

				handle_arrow_down();
				break;
			case JVX_ASCI_CODE_ARROW_LEFT_TTY:

				arrowupdownmode = false;

#ifdef JVX_BE_PRINT_VERBOSE
			std::cout << "Arrow Left" << std::endl;
#endif
				CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
				if(insert_position > 0)
					insert_position = JVX_MAX(insert_position -1, 0);
				PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
				break;

			case JVX_ASCI_CODE_ARROW_RIGHT_TTY:

				arrowupdownmode = false;

#ifdef JVX_BE_PRINT_VERBOSE
			std::cout << "Arrow Right" << std::endl;
#endif

				CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
				insert_position = JVX_MIN(insert_position + 1, internaltextbuffer.size());
				PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
				break;

			case JVX_ASCI_CODE_DELETE_TTY:

				arrowupdownmode = false;

#ifdef JVX_BE_PRINT_VERBOSE
			std::cout << "Delete" << std::endl;
#endif

				if (insert_position < internaltextbuffer.size())
				{
					CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
					std::string tmp1 = internaltextbuffer.substr(0, insert_position);
					std::string tmp2;
					if (insert_position < internaltextbuffer.size()) 
						tmp2 = internaltextbuffer.substr(insert_position + 1, std::string::npos);
					internaltextbuffer = tmp1 + tmp2;
					PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
				}
				break;

			case JVX_ASCI_CODE_BEGINLINE_TTY:

				arrowupdownmode = false;

#ifdef JVX_BE_PRINT_VERBOSE
				std::cout << "Begin line" << std::endl;
#endif
				CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
				insert_position = 0;
				PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
				break;

			case JVX_ASCI_CODE_ENDLINE_TTY:

				arrowupdownmode = false;

#ifdef JVX_BE_PRINT_VERBOSE
				std::cout << "End line" << std::endl;
#endif
				CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
				insert_position = internaltextbuffer.size();
				PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
				break;
			default:
				arrowupdownmode = false;

				// Ignore
				break;
			}
			break;
		default:
			std::cout << "Unsupported data type" << std::endl;
		}

		break;
	default:
		std::cout << "Unsupported event type" << std::endl;
	}

	return res;
}

// ============================================================================================
void
CjvxConsoleHost_be_print::process_init(IjvxCommandLine* commLine)
{
	std::cout << "Initializing backend.." << std::endl;
}

jvxErrorType
CjvxConsoleHost_be_print::process_shutdown()
{
	std::cout << "Shutting down backend.." << std::endl;
	return JVX_NO_ERROR;
}

void 
CjvxConsoleHost_be_print::process_help_command(IjvxEventLoop_frontend* origin)
{
	std::cout << std::endl;
	std::cout << "Usage: " << std::endl;
	std::cout << "\thelp: print this message" << std::endl;
	if (!config_noquit)
	{
		std::cout << "\tquit: terminate program" << std::endl;
	}
	else
	{
		std::cout << "\tquit: terminate program - this command is not allowed in this configuration." << std::endl;
	}
}

void 
CjvxConsoleHost_be_print::process_full_command(const std::string& command,
	jvxSize message_id, jvxHandle* param,
	jvxSize paramType, jvxOneEventClass event_class,
	jvxOneEventPriority event_priority,
	IjvxEventLoop_frontend* origin,
	jvxHandle* privBlock, jvxCBitField* extFlags)
{
	std::cout << " -> ACCEPTING: <" << command << ">" << std::flush;
}

jvxErrorType
CjvxConsoleHost_be_print::report_register_be_commandline(IjvxCommandLine* comLine)
{
	if (comLine)
	{
		comLine->register_option("--no-quit", "", "Option to disallow <quit>.");
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_be_print::report_readout_be_commandline(IjvxCommandLine* comLine)
{
	if (comLine)
	{
		jvxSize num = 0;
		comLine->number_entries_option("--no-quit", &num);
		if (num)
		{
			config_noquit = true;
		}
	}
	return JVX_NO_ERROR;
}

void
CjvxConsoleHost_be_print::store_entry_console_list(const std::string& entry)
{
	jvxSize i,j;
	jvxBool alreadyThere = false;
	for (i = 0; i < internaltextbuffer_mem.size(); i++)
	{
		if (internaltextbuffer_mem[i] == entry)
		{
			alreadyThere = true;
				break;
		}
	}

	if (alreadyThere)
	{
		std::vector<std::string> newLst;
		for (j = 0; j < internaltextbuffer_mem.size(); j++)
		{
			if (j != i)
			{
				newLst.push_back(internaltextbuffer_mem[j]);
			}
		}
		internaltextbuffer_mem = newLst;
	}

	internaltextbuffer_mem.push_back(entry);
}

void
CjvxConsoleHost_be_print::handle_arrow_up()
{
	int newCnt = cntMem;
	std::string matchStr = user_input;

	// Arrow up, repeat last line
	CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
	
	while (newCnt >= 0)
	{
		newCnt--;
		
		if (newCnt >= 0)
		{
			if (
				matchStr.empty() || (matchStr == internaltextbuffer_mem[newCnt].substr(0, matchStr.size())))
			{
				break;
			}
		}
	}
	//std::cout << "CNTMEM" << cntMem << std::endl;
	cntMem = newCnt;
	if ((newCnt >= 0) && newCnt < internaltextbuffer_mem.size())
	{
		internaltextbuffer = internaltextbuffer_mem[newCnt];
	}
	else
	{ 
		internaltextbuffer = user_input;
	}
	insert_position = internaltextbuffer.size();
	PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
}

void
CjvxConsoleHost_be_print::handle_arrow_down()
{
	int newCnt = cntMem;
	std::string matchStr = user_input;

	// Arrow up, repeat last line
	CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);

	while (newCnt < ((int)internaltextbuffer_mem.size()))
	{
		newCnt++;

		if (newCnt < internaltextbuffer_mem.size())
		{
			if (
				matchStr.empty() || (matchStr == internaltextbuffer_mem[newCnt].substr(0, matchStr.size())))
			{
				break;
			}
		}
	}
	//std::cout << "CNTMEM" << cntMem << std::endl;
	cntMem = newCnt;
	if ((newCnt >= 0) && newCnt < internaltextbuffer_mem.size())
	{
		internaltextbuffer = internaltextbuffer_mem[newCnt];
	}
	else
	{ 
		internaltextbuffer = user_input;
	}
	insert_position = internaltextbuffer.size();
	PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
	/*


	// Arrow down, repeat last line
	CLEANLINE(internaltextbuffer, highlight_left, highlight_right, linestart, insert_position);
	if (cntMem < internaltextbuffer_mem.size())
	{
		cntMem++;
	}
	//std::cout << "CNTMEM" << cntMem << std::endl;
	if (cntMem < (((int)internaltextbuffer_mem.size()) - 1))
	{
		internaltextbuffer = internaltextbuffer_mem[cntMem];
	}
	else
	{
		internaltextbuffer = "";
	}
	insert_position = internaltextbuffer.size();
	PRINTLINE(internaltextbuffer, highlight_left, highlight_right, insert_position, linestart);
	*/
}
