#include "jvx.h"
#include "interfaces/console/IjvxEventLoop_frontend_ctrl.h"
#include "interfaces/console/IjvxEventLoop_backend_ctrl.h"
#include "CjvxConsoleHost_fe_console.h"
#include "CjvxConsoleHost_be_drivehost.h"

static CjvxConsoleHost_fe_console* theConsole_fe = NULL;
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
};

extern "C"
{
jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* adescr, jvxComponentType tp, jvxSize id)
{
	return JVX_ERROR_UNSUPPORTED;
}
}

#include "interfaces/all-hosts/configHostFeatures_common.h"
extern "C"
{
jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
{
	configureHost_features* theFeaturesC = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesC), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);
	if (theFeaturesC)
	{
	}
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}
}
