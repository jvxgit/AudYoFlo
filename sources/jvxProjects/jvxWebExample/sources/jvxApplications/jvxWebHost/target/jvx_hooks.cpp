#include "jvx.h"
#include "interfaces/console/IjvxEventLoop_frontend_ctrl.h"
#include "interfaces/console/IjvxEventLoop_backend_ctrl.h"
#include "CjvxWebControl_fe.h"
#include "CjvxConsoleHost_fe_console.h"
#include "CjvxConsoleHost_be_drivehost.h"

#include "jvxTconfigProcessor.h"
#include "jvxTSystemTextLog.h"
#include "jvxTWebServer.h"
#include "jvxCuTExample.h"

static CjvxConsoleHost_fe_console* theConsole_fe = NULL;
static CjvxWebControl_fe* theWeb_fe = NULL;
static CjvxConsoleHost_be_drivehost* main_be = NULL;

extern "C"
{
	jvxErrorType jvx_manage_frontends(IjvxEventLoop_frontend_ctrl** theFrontend, jvxSize cnt, jvxCBool is_init)
	{
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
		else if (cnt == 1)
		{
			if (is_init == c_true)
			{
				assert( theWeb_fe == NULL);
				JVX_DSP_SAFE_ALLOCATE_OBJECT(theWeb_fe, CjvxWebControl_fe);
				*theFrontend = static_cast<IjvxEventLoop_frontend_ctrl*>(theWeb_fe);
			}
			else
			{
				assert(theWeb_fe != NULL);
				assert(theWeb_fe == *theFrontend);
				JVX_DSP_SAFE_DELETE_OBJECT(theWeb_fe);
				*theFrontend = NULL;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	jvxErrorType jvx_manage_backends(IjvxEventLoop_backend_ctrl** theBackend, jvxSize cnt, jvxCBool is_init)
	{
		if (cnt == 0)
		{
			if (is_init == c_true)
			{
				assert(main_be == NULL);
				CjvxConsoleHost_be_drivehost* thePrinter = NULL;
				JVX_DSP_SAFE_ALLOCATE_OBJECT(main_be, CjvxConsoleHost_be_drivehost);
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

	jvxErrorType jvx_link_frontends_backends(jvxCBool is_init)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (is_init)
		{
			// Create the cross references
			assert(theConsole_fe);
			assert(theWeb_fe);
			assert(main_be);

			// Both frontends control the same backend as primary target
			res = theConsole_fe->set_pri_reference_event_backend(main_be);
			assert(res == JVX_NO_ERROR);
			res = theWeb_fe->set_pri_reference_event_backend(main_be);
			assert(res == JVX_NO_ERROR);

			// The main backend gets references to both frontends
			res = main_be->set_pri_reference_frontend(theConsole_fe);
			assert(res == JVX_NO_ERROR);
			res = main_be->add_sec_reference_frontend(theWeb_fe);
			assert(res == JVX_NO_ERROR);
		}
		else
		{
			// Unlink the cross references
			assert(theConsole_fe);
			assert(theWeb_fe);
			assert(main_be);

			res = main_be->clear_sec_reference_frontend(theWeb_fe);
			assert(res == JVX_NO_ERROR);

			res = main_be->clear_pri_reference_frontend(theConsole_fe);
			assert(res == JVX_NO_ERROR);
			
			res = theWeb_fe->clear_pri_reference_event_backend(main_be);
			assert(res == JVX_NO_ERROR);
			res = theConsole_fe->clear_pri_reference_event_backend(main_be);
			assert(res == JVX_NO_ERROR);
		}
		return JVX_NO_ERROR;
	}
};

extern "C"
{
jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, 
	jvxApiString* adescr, jvxComponentType tp, jvxSize id)
{
	switch (tp)
	{
	case JVX_COMPONENT_CONFIG_PROCESSOR:
		switch (id)
		{
		case 0:
			adescr->assign("Systen Config Reader");
			*funcInit = jvxTconfigProcessor_init;
			*funcTerm = jvxTconfigProcessor_terminate;
			return(JVX_NO_ERROR);
			break;
		default:
			break;
		}
		break;
	case JVX_COMPONENT_SYSTEM_TEXT_LOG:
		switch (id)
		{
		case 0:
			adescr->assign("Systen Text Log");
			*funcInit = jvxTSystemTextLog_init;
			*funcTerm = jvxTSystemTextLog_terminate;
			return(JVX_NO_ERROR);
			break;
		default:
			break;
		}
		break;

	case JVX_COMPONENT_CUSTOM_TECHNOLOGY:
		switch (id)
		{
		case 0:
			adescr->assign("Arria10 - Inspire Technology");
			*funcInit = jvxCuTExample_init;
			*funcTerm = jvxCuTExample_terminate;
			return(JVX_NO_ERROR);
			break;
		}
		break;
#ifdef JVX_HOST_USE_DATALOGGER
		case JVX_COMPONENT_DATALOGGER:
			switch (id)
			{
			case 0:
				adescr->assign("Data Logger");
				*funcInit = jvxTDataLogger_init;
				*funcTerm = jvxTDataLogger_terminate;
				return(JVX_NO_ERROR);
			default:
				break;
			}
			break;
#endif

		case JVX_COMPONENT_WEBSERVER:
			switch (id)
			{
			case 0:
				adescr->assign("Web Server");
				*funcInit = jvxTWebServer_init;
				*funcTerm = jvxTWebServer_terminate;
				return(JVX_NO_ERROR);
			default:
				break;
			}
			break;
		default:
			break;
	}

	return JVX_ERROR_UNSUPPORTED;
}
}

// =============================================================
// Specify some runtime host configurations
// =============================================================
#include "interfaces/all-hosts/configHostFeatures_common.h"
extern "C"
{
	jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
	{
		return(JVX_NO_ERROR);
	}
	
	jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
	{
		return(JVX_NO_ERROR);
	}
}
