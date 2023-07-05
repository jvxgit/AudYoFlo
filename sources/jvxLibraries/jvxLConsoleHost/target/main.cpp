#include <iostream>
#include "jvx.h"

#include "jvxEStandalone.h"

#include "CjvxConsoleHost_fe_console.h"
#include "CjvxConsoleHost_be_print.h"

static CjvxConsoleHost_fe_console* theConsole_fe = NULL;
static CjvxConsoleHost_be_print* main_be = NULL;

extern "C"
{

#define FUNC_CORE_PROT_DECLARE jvx_init_before_start
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_init_before_start_local
#define FUNC_CORE_PROT_ARGS 
#define FUNC_CORE_PROT_RETURNVAL void

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_init_before_start=jvx_init_before_start_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_init_before_start=_jvx_init_before_start_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL

	// ===========================================================


#define FUNC_CORE_PROT_DECLARE jvx_manage_frontends
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_manage_frontends_local
#define FUNC_CORE_PROT_ARGS IjvxEventLoop_frontend_ctrl** theFrontend, jvxSize cnt, jvxCBool is_init
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_manage_frontends=jvx_manage_frontends_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_manage_frontends=_jvx_manage_frontends_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		if (cnt == 0)
		{
			if (is_init == c_true)
			{
				assert(theConsole_fe == NULL);
				JVX_DSP_SAFE_ALLOCATE_OBJECT(theConsole_fe, CjvxConsoleHost_fe_console);
				*theFrontend = static_cast<IjvxEventLoop_frontend_ctrl*>(theConsole_fe);
			}
			else
			{
				assert(theConsole_fe != NULL);
				assert(theConsole_fe == *theFrontend);
				JVX_DSP_SAFE_DELETE_OBJECT(theConsole_fe);
				theConsole_fe = NULL;
				*theFrontend = NULL;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL

	// ===========================================================

#define FUNC_CORE_PROT_DECLARE jvx_manage_backends
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_manage_backends_local
#define FUNC_CORE_PROT_ARGS IjvxEventLoop_backend_ctrl** theBackend, jvxSize cnt, jvxCBool is_init
  #define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_manage_backends=jvx_manage_backends_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_manage_backends=_jvx_manage_backends_local")
#endif

  #include "platform/jvx_platform_weak_defines.h"
        {
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		if (cnt == 0)
		{
			if (is_init == c_true)
			{
				assert(main_be == NULL);
				CjvxConsoleHost_be_print* thePrinter = NULL;
				JVX_DSP_SAFE_ALLOCATE_OBJECT(main_be, CjvxConsoleHost_be_print);
				*theBackend = static_cast<IjvxEventLoop_backend_ctrl*>(main_be);
			}
			else
			{
				assert(main_be != NULL);
				assert(main_be == *theBackend);
				JVX_DSP_SAFE_DELETE_OBJECT(*theBackend);
				*theBackend = NULL;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}
#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL


	// ===========================================================

#define FUNC_CORE_PROT_DECLARE jvx_link_frontends_backends
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_link_frontends_backends_local
#define FUNC_CORE_PROT_ARGS jvxCBool is_init
  #define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_link_frontends_backends=jvx_link_frontends_backends_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_link_frontends_backends=_jvx_link_frontends_backends_local")
#endif

  #include "platform/jvx_platform_weak_defines.h"
  {
			std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
			if (is_init)
		{
			// Create the cross references
			if (theConsole_fe)
			{
				theConsole_fe->set_pri_reference_event_backend(main_be);
			}

			if (main_be)
			{
				main_be->set_pri_reference_frontend(theConsole_fe);
			}
		}
		else
		{
			// Unlink the cross references
			if (main_be)
			{
				main_be->clear_pri_reference_frontend(theConsole_fe);
			}
			if (theConsole_fe)
			{
				theConsole_fe->clear_pri_reference_event_backend(main_be);
			}
		}
		return JVX_NO_ERROR;
  }
  
  #undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
  #undef FUNC_CORE_PROT_ARGS
  #undef FUNC_CORE_PROT_RETURNVAL

}

// ===================================================================================
// ===================================================================================
// ===================================================================================
// ===================================================================================
// ===================================================================================
// ===================================================================================

typedef struct
{
	IjvxEventLoop_frontend_ctrl* ref;
	jvxSize cnt;
	jvxBool returnsFromStart;
} oneFrontendStruct;

typedef struct
{
	IjvxEventLoop_backend_ctrl* ref;
	jvxSize cnt;
} oneBackendStruct;


// ===================================================================================

int main(int argc, char* argv[])
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR, resL;
	std::string inputText;
	std::vector<oneFrontendStruct> theFrontends;
	std::vector<oneBackendStruct> theBackends;
	jvx_init_before_start();

	IjvxObject* evLoop_obj = NULL;
	IjvxEventLoopObject* eLoop_hdl = NULL;
	IjvxHiddenInterface* hostRef = NULL;
	jvxSize numNoReturnStart = 0;
	jvxBool wantsRestart = false;
	jvxBool wantsRestartRet = false;

	// Line required for Linux systems (whatever this is)
	JVX_INIT_LOCALE("C");

	std::vector<std::string> lstArgs;
	char** newArgv = NULL;
	int newArgc = 0;
	jvxBool generatedArgs = false;

	if (argc == 1)
	{
		// If there are no arguments, we need to create our own
		jvxSize i;
		lstArgs.push_back(argv[0]);
		jvx_deriveRunArgsJvx(lstArgs, argv[0]);
	}
	else
	{
		jvxSize i;
		for (i = 0; i < argc; i++)
		{
			lstArgs.push_back(argv[i]);
		}
	}

	jvx_appArgs(newArgv, newArgc, lstArgs);
	generatedArgs = true;

	jvxEStandalone_init(&evLoop_obj, NULL);
	if (evLoop_obj)
	{
		evLoop_obj->request_specialization(reinterpret_cast<jvxHandle**>(&eLoop_hdl), NULL, NULL, NULL);
		if (eLoop_hdl)
		{
			std::cout << "Starting console host" << std::endl;
			jvxSize cnt = 0;
			while (1)
			{
				oneBackendStruct theNewBackend;
				theNewBackend.ref = NULL;
				theNewBackend.cnt = cnt;

				jvx_manage_backends(&theNewBackend.ref, cnt, c_true);
				if (theNewBackend.ref)
				{
					theBackends.push_back(theNewBackend);
				}
				else
				{
					break;
				}
				cnt++;
			}

			cnt = 0;
			numNoReturnStart = 0;
			while (1)
			{
				oneFrontendStruct theNewFrontend;
				theNewFrontend.ref = NULL;
				theNewFrontend.returnsFromStart = false;
				theNewFrontend.cnt = cnt;

				resL = jvx_manage_frontends(&theNewFrontend.ref, cnt, c_true);
				if (resL == JVX_NO_ERROR)
				{
					theNewFrontend.ref->returns_from_start(&theNewFrontend.returnsFromStart);
					if (!theNewFrontend.returnsFromStart)
					{
						numNoReturnStart++;
					}
					theFrontends.push_back(theNewFrontend);
				}
				else
				{
					break;
				}
				cnt++;
			}

			if (numNoReturnStart != 1)
			{
				std::cout << "Error: exactly one non-returning frontend is required!" << std::endl;
				exit(0);
			}

			// All combinations...
			for (i = 0; i < theBackends.size(); i++)
			{
				resL = theBackends[i].ref->add_reference_event_loop(eLoop_hdl);
				if (resL != JVX_NO_ERROR)
				{
					std::cout << "Error: Failed to add event loop references to event loop backend #" << i << std::endl;
				}
			}
			for (i = 0; i < theFrontends.size(); i++)
			{
				resL = theFrontends[i].ref->add_reference_event_loop(eLoop_hdl);
				if (resL != JVX_NO_ERROR)
				{
					std::cout << "Error: Failed to add event loop references to event loop frontend #" << i << std::endl;
				}
			}

			// Call this function to create cross references
			jvx_link_frontends_backends(true);

			// Initialize event loop
			res = eLoop_hdl->initialize(
				hostRef,
				JVX_SIZE_UNSELECTED,
				JVX_SIZE_UNSELECTED,
				JVX_SIZE_UNSELECTED, 1000);
			assert(res == JVX_NO_ERROR);

			do
			{
				wantsRestart = false;

				// Start event loop
				res = eLoop_hdl->start();
				assert(res == JVX_NO_ERROR);

				/*
				res = eLoop_hdl->event_schedule(
					NULL, NULL, NULL, NULL, &mId, NULL, JVX_SIZE_UNSELECTED,
					JVX_EVENTLOOP_EVENT_INIT, JVX_EVENTLOOP_REQUEST_TRIGGER,
					JVX_EVENTLOOP_PRIORITY_NORMAL, JVX_SIZE_UNSELECTED);
				assert(res == JVX_NO_ERROR);
				*/
				// Wait for user input
				//theFrontend.start_timer(500);
				//theFrontend.start_timer(50000);

				// Start the backends
				for (j = 0; j < theBackends.size(); j++)
				{
					resL = theBackends[j].ref->start_receiving();
				}

				// Start the frontends which do not block main thread
				for (i = 0; i < theFrontends.size(); i++)
				{
					if (theFrontends[i].returnsFromStart)
					{
						//theFrontends[i].ref->start(argc, &argv[0]);
						theFrontends[i].ref->start(newArgc, newArgv);
					}
				}

				for (i = 0; i < theFrontends.size(); i++)
				{
					if (!theFrontends[i].returnsFromStart)
					{
						theFrontends[i].ref->start(newArgc, newArgv);
						//theFrontends[i].ref->start(argc, &argv[0]); // Will stay here..


						theFrontends[i].ref->wants_restart(&wantsRestartRet);
						if (wantsRestartRet)
						{
							wantsRestart = true;
						}

						// Here, processing is just over..
						theFrontends[i].ref->stop(); // From here, shutdown begins
						break;
					}
				}

				for (i = 0; i < theFrontends.size(); i++)
				{
					if (theFrontends[i].returnsFromStart)
					{
						theFrontends[i].ref->wants_restart(&wantsRestartRet);
						if (wantsRestartRet)
						{
							wantsRestart = true;
						}

						theFrontends[i].ref->stop();
					}
				}

				for (j = 0; j < theBackends.size(); j++)
				{
					theBackends[j].ref->wants_restart(&wantsRestartRet);
					if (wantsRestartRet)
					{
						wantsRestart = true;
					}
					resL = theBackends[j].ref->stop_receiving();
				}

				// Trigger stop from internal - we need this to have the event loop to run out cleanly
				TjvxEventLoopElement evLElm;

				evLElm.origin_fe = NULL;
				evLElm.priv_fe = NULL;
				evLElm.target_be = NULL;
				evLElm.priv_be = NULL;

				evLElm.param = NULL;
				evLElm.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;

				evLElm.eventType = JVX_EVENTLOOP_EVENT_DEACTIVATE;
				evLElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
				evLElm.eventPriority = JVX_EVENTLOOP_PRIORITY_HIGH;
				evLElm.delta_t = JVX_SIZE_UNSELECTED;
				evLElm.autoDeleteOnProcess = c_false;

				res = eLoop_hdl->event_schedule(&evLElm, NULL, NULL);
				
				assert(res == JVX_NO_ERROR);

				res = eLoop_hdl->wait_stop(10000);
				assert(res == JVX_NO_ERROR);
			} while (wantsRestart);

			res = eLoop_hdl->terminate();
			assert(res == JVX_NO_ERROR);

			// Call this function to create cross references
			jvx_link_frontends_backends(false);

			for (i = 0; i < theBackends.size(); i++)
			{
				resL = theBackends[i].ref->clear_reference_event_loop(eLoop_hdl);
			}

			// Now, delete the instances
			for (i = 0; i < theFrontends.size(); i++)
			{
				jvx_manage_frontends(&theFrontends[i].ref, theFrontends[i].cnt, c_false);
			}
			for (i = 0; i < theBackends.size(); i++)
			{
				jvx_manage_backends(&theBackends[i].ref, theBackends[i].cnt, c_false);
			}
		}

		jvxEStandalone_terminate(evLoop_obj);
	}
	
	if (generatedArgs)
	{
		delete[](newArgv);
		newArgc = 0;
	}

	return(0);
}

