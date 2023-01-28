#include "jvx.h"
#include "mainCentral_host.h"

extern "C"
{
jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, 
	jvxTerminateObject_tp* funcTerm, 
	jvxApiString* description, jvxComponentType tp, jvxSize id)
{
	return(JVX_ERROR_ELEMENT_NOT_FOUND);
}
}

// =============================================================
// Main widget entry functions
// =============================================================
jvxErrorType
mainWindow_UiExtension_host_init(mainWindow_UiExtension_host** onReturn, QWidget* parent)
{
	mainCentral_host* elm = new mainCentral_host(parent);
	if (onReturn)
	{
		*onReturn = static_cast<mainWindow_UiExtension_host*>(elm);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mainWindow_UiExtension_host_terminate(mainWindow_UiExtension_host* elm)
{
	delete(elm);
	return JVX_NO_ERROR;
}

// =============================================================
// Specify some runtime host configurations
// =============================================================
#include "interfaces/qt-develop-host/configureQtDevelopHost_features.h"
extern "C"
{
jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
{
	configureQtDevelopHost_features* theFeaturesD = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesD), JVX_HOST_IMPLEMENTATION_QT_DEVELOP_HOST);
	if (theFeaturesD)
	{
#ifdef JVX_NO_SEQUENCER
		theFeaturesD->showExpertUi_seq = false; // Default: true	
#endif

#ifdef JVX_NO_RTVIEWER
		theFeaturesD->showExpertUi_props = false; // Default: true
		theFeaturesD->showExpertUi_plots = false; // Default: true
#endif
#ifdef JVX_NO_MESSAGES
		theFeaturesD->showExpertUi_messages = false;
#endif

#ifdef JVX_ALLOW_ONLY_OVERLAYS
		theFeaturesD->allowOverlayOnly_config = true;
#endif
		theFeaturesD->numSlotsComponents[JVX_COMPONENT_AUDIO_TECHNOLOGY] = 2;
		theFeaturesD->numSlotsComponents[JVX_COMPONENT_AUDIO_DEVICE] = 2;
		theFeaturesD->numSlotsComponents[JVX_COMPONENT_AUDIO_NODE] = 2;

		theFeaturesD->numSlotsComponents[JVX_COMPONENT_SIGNAL_PROCESSING_NODE] = 2;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}

}
