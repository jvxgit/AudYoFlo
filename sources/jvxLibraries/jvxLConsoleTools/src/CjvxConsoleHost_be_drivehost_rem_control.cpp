#include "CjvxConsoleHost_be_drivehost.h"

jvxErrorType 
CjvxConsoleHost_be_drivehost::report_assign_output(TjvxEventLoopElement* theQueueElement, jvxErrorType sucOperation, jvxHandle* priv)
{
	if (theQueueElement)
	{
		rem_control.res = sucOperation;
		if (theQueueElement->paramType == JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS)
		{
			CjvxJsonElementList* jsonelm = (CjvxJsonElementList*)theQueueElement->param;
			jsonelm->printToStringView(rem_control.txt);
		}
	}
	return JVX_NO_ERROR;
};

JVX_PROPERTIES_CALLBACK_DEFINE(CjvxConsoleHost_be_drivehost, cb_save_config)
{
	std::string command = "file(config, write)";
	return run_prop_command(command);
}

JVX_PROPERTIES_CALLBACK_DEFINE(CjvxConsoleHost_be_drivehost, cb_shutdown)
{
	// In property set of shutdown property
	return run_quit_command(false);
}

JVX_PROPERTIES_CALLBACK_DEFINE(CjvxConsoleHost_be_drivehost, cb_restart)
{
	// In property set of restart property
	return run_quit_command(true);	
}

JVX_PROPERTIES_CALLBACK_DEFINE(CjvxConsoleHost_be_drivehost, cb_xchg_property)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string cmd;
	jvxFloatingPointer_propadmin* adm = NULL;
	jvxBool do_set = true;

	// We receive the data in this property set forward in content and convert
	// it to a command (cmd) and a bool to indicate wether set or get (do_set)
	// In adm we get the other property set/get constraints.
	// The floating pointer is fully abused, therefore we use the two functions 
	// <jvx_exchange_property_callback_local_pack> and <jvx_exchange_property_callback_local_unpack>
	// to pack and unpack.
	res = jvx_exchange_property_callback_local_unpack(content, adm, cmd, do_set);

	/*
	if (res == JVX_NO_ERROR)
	{
		if (cmd == "autostart")
		{
			jvxInt16* valI16 = (jvxInt16*)adm->fld;
			if (do_set)
			{
				config.auto_start = (*valI16 == c_true);
			}
			else
			{
				*valI16 = config.auto_start;
			}
		}
	}
	*/
	return res;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::run_quit_command(jvxBool restart)
{
	if (runtime.myState == JVX_STATE_ACTIVE)
	{
		return this->linkedPriFrontend.fe->report_want_to_shutdown_ext(restart);
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::run_prop_command(const std::string command)
{
	jvxCallManagerProperties callGate;
	std::string errTxt;
	jvxErrorType res = JVX_NO_ERROR;
	IjvxProperties* theProps = NULL;
	jvxApiString str;

	TjvxEventLoopElement evelm;
	evelm.origin_fe = this;
	evelm.priv_fe = NULL;
	evelm.target_be = this;
	evelm.priv_be = NULL;

	evelm.param = (jvxHandle*)&command;
	evelm.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;

	evelm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
	evelm.eventClass = JVX_EVENTLOOP_REQUEST_IMMEDIATE;
	evelm.eventPriority = JVX_EVENTLOOP_PRIORITY_UNKNOWN;
	evelm.delta_t = 0;
	evelm.autoDeleteOnProcess = c_false;
	jvxErrorType resL = evLop->event_schedule(&evelm, NULL, NULL);

	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);
	if (theProps)
	{		
		str.assign(rem_control.txt);
		resL = jvx_set_property(theProps, &str, 0, 1, JVX_DATAFORMAT_STRING, true, "/ext_interfaces/last_error", callGate);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);
	}
	theProps = NULL;
	return res;
}

JVX_PROPERTIES_CALLBACK_DEFINE(CjvxConsoleHost_be_drivehost, cb_load_config)
{
	std::string filename;
	std::string errTxt;
	jvxErrorType res = JVX_NO_ERROR;

	
	/*
	if (!filename.empty())
	{
		_command_line_parameters.configFilename = filename;
	}

	if (!_command_line_parameters.configFilename.empty())
	{
		res = try_reopen_config();
		if (res == JVX_NO_ERROR)
		{
			assert(extFlags);
			jvx_bitSet(*extFlags, JVX_EVENT_CONTINUE_WANT_SHUTDOWN_RESTART_SHIFT);
		}
		else
		{
			errTxt = "Failed to prepare reading of config file " + filename;
			errTxt += ", reason: ";
			errTxt += jvxErrorType_descr(res);
			JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
		}
	}
	*/
	return JVX_NO_ERROR;
}

#if 0

WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE))

https://github.com/toshic/libfcgi/blob/master/libfcgi/os_win32.c

static void StdinThread(LPDWORD startup) {

	int doIo = TRUE;
	int fd;
	int bytesRead;
	POVERLAPPED_REQUEST pOv;

	while (doIo) {
		/*
		 * Block until a request to read from stdin comes in or a
		 * request to terminate the thread arrives (fd = -1).
		 */
		if (!GetQueuedCompletionStatus(hStdinCompPort, &bytesRead, &fd,
			(LPOVERLAPPED *)&pOv, (DWORD)-1) && !pOv) {
			doIo = 0;
			break;
		}

		ASSERT((fd == STDIN_FILENO) || (fd == -1));
		if (fd == -1) {
			doIo = 0;
			break;
		}
		ASSERT(pOv->clientData1 != NULL);

		if (ReadFile(stdioHandles[STDIN_FILENO], pOv->clientData1, bytesRead,
			&bytesRead, NULL)) {
			PostQueuedCompletionStatus(hIoCompPort, bytesRead,
				STDIN_FILENO, (LPOVERLAPPED)pOv);
		}
		else {
			doIo = 0;
			break;
		}
	}

	ExitThread(0);
}
#endif

