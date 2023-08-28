#include "jvx.h"
#include "interfaces/console/IjvxEventLoop_frontend_ctrl.h"
#include "interfaces/console/IjvxEventLoop_backend_ctrl.h"
#include "CjvxWebControl_fe.h"
#include "CjvxConsoleHost_fe_console.h"
#include "CjvxConsoleHost_be_drivehost.h"
#include "jvx-core-host-loop.h"

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
	configureHost_features* theFeaturesH = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesH), JVX_HOST_IMPLEMENTATION_HOST);
	if (theFeaturesH)
	{
	}
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}
}

int main(int argc, char* argv[])
{
	jvx_core_host_loop(argc, argv);
	return(0);
}