#include "jvx.h"

#include "myCentralWidget.h"
#include "jvxTSystemTextLog.h"

extern "C"
{
	jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* adescr, jvxComponentType tp, jvxSize id)
	{
		std::cout << "Asking for extra component type " << jvxComponentType_txt(tp) << std::endl;

		switch (tp)
		{
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
		default:
			break;
		}
		return(JVX_ERROR_ELEMENT_NOT_FOUND);
	}
}

// =============================================================
// Main widget entry functions
// =============================================================
jvxErrorType
mainWindow_UiExtension_hostfactory_init(mainWindow_UiExtension_hostfactory** onReturn, QWidget* parent)
{
#ifdef JVX_PROJECT_NAMESPACE
	JVX_PROJECT_NAMESPACE::myCentralWidget* elm = new JVX_PROJECT_NAMESPACE::myCentralWidget(parent);
#else
	myCentralWidget* elm = new myCentralWidget(parent);
#endif
	if (onReturn)
	{
		*onReturn = static_cast<mainWindow_UiExtension_hostfactory*>(elm);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mainWindow_UiExtension_hostfactory_terminate(mainWindow_UiExtension_hostfactory* elm)
{
	delete(elm);
	return JVX_NO_ERROR;
}

void module_load_dll_cb(jvxBool* doNotLoad, const char* module_name, jvxComponentType tp, jvxHandle* priv)
{
	switch (tp)
	{
	case JVX_COMPONENT_SYSTEM_TEXT_LOG:
	case JVX_COMPONENT_CONFIG_PROCESSOR:
		*doNotLoad = false;
		break;
	default:
		*doNotLoad = true;
	}
}

// =============================================================
// Specify some runtime host configurations
// =============================================================
#include "interfaces/qt-factory-host/configureQtFactoryHost_features.h"
extern "C"
{
	jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
{
	configureQtFactoryHost_features* theFeaturesQT = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesQT), JVX_HOST_IMPLEMENTATION_QT_FACTORY_HOST);
	if (theFeaturesQT)
	{
		theFeaturesQT->cb_loadfilter = module_load_dll_cb;
		theFeaturesQT->cb_loadfilter_priv = NULL;
		theFeaturesQT->mainWidgetInScrollArea = false;

		theFeaturesQT->config_ui.iconh_small = 100;
		theFeaturesQT->config_ui.iconw_small = 100;
	}
	
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}
}

#if 0
jvxErrorType configureDevelopHost_features_request(jvxHandle* features)
{
	return(JVX_NO_ERROR);
}
#endif
