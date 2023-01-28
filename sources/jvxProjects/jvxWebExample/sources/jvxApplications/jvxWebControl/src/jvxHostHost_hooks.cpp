#include "jvx.h"

#include "myCentralWidget.h"

extern "C"
{
jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm,
	jvxApiString* adescr, jvxComponentType tp, jvxSize id)
{
/*	switch (tp)
	{

	case JVX_COMPONENT_AUDIO_NODE:
		switch (id)
		{
		case 0:
			descr = JVX_ALGO_DESCRIPTION;
			mL = JVX_MAXSTRING - 1;
			*funcInit = JVX_ALGO_INIT_FUNC;
			*funcTerm = JVX_ALGO_TERMINATE_FUNC;

			// description is a buffer of length JVX_MAXSTRING
			mL = JVX_MIN(descr.size(), mL);
			memcpy(description, descr.c_str(), mL);
			return(JVX_NO_ERROR);

		default:
			break;
		}
		break;
	default:
		break;
	}*/
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
