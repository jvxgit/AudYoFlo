#include "jvx.h"
#include "jvxAuNMyAlgorithm.h"

#define JVX_ALGO_DESCRIPTION "Audio Simple"
#define JVX_ALGO_INIT_FUNC jvxAuNMyAlgorithm_init
#define JVX_ALGO_TERMINATE_FUNC jvxAuNMyAlgorithm_terminate

#include "jvxApplications/jvx_init_link_objects_tpl.h"

// =============================================================
// Configure ui extensions
// =============================================================
JVX_INTERFACE IjvxQtComponentWidget;
jvxErrorType
IjvxQtComponentWidget_init(IjvxQtComponentWidget** onReturn, jvxBitField featureClass, const jvxComponentIdentification& cpId)
{
	if (onReturn)
		*onReturn = NULL;
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
IjvxQtComponentWidget_terminate(IjvxQtComponentWidget* returnMe, jvxBitField featureClass, const jvxComponentIdentification& cpId)
{
	assert(returnMe == NULL);
	return JVX_ERROR_UNSUPPORTED;
}

// ===============================================================================

#include "myCentralWidget.h"

// =============================================================
// Main widget entry functions
// =============================================================
jvxErrorType
mainWindow_UiExtension_host_init(mainWindow_UiExtension_host** onReturn, QWidget* parent)
{
#ifdef JVX_PROJECT_NAMESPACE
	JVX_PROJECT_NAMESPACE::myCentralWidget* elm = new JVX_PROJECT_NAMESPACE::myCentralWidget(parent);
#else
	myCentralWidget* elm = new myCentralWidget(parent);
#endif
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
#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"
extern "C"
{
jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
{
	configureQtAudioHost_features* theFeaturesA = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesA), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);
	if (theFeaturesA)
	{
	}
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}

}

jvxErrorType configureDevelopHost_features_request(jvxHandle* features)
{
	return(JVX_NO_ERROR);
}
