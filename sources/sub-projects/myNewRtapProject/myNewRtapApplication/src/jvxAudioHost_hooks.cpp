#include "jvx.h"
#include "myNewRtapRtHooks.h"

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

// =======================================================================


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
	myCentralWidget* newElm = new myCentralWidget(parent);
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
	configureQtAudioHost_features* theFeaturesQt = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesQt), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);
	if (theFeaturesQt)
	{

#ifdef JVX_PROJECT_NAMESPACE
		JVX_PROJECT_NAMESPACE::myCentralWidget* theWidget = dynamic_cast<JVX_PROJECT_NAMESPACE::myCentralWidget*>(theFeaturesQt->myMainWidget);
#else
		myCentralWidget* theWidget = dynamic_cast<myCentralWidget*>(theFeaturesA->myMainWidget);
#endif
		theFeaturesQt->config_ui.minWidthWindow = JVX_MAX(theFeaturesQt->config_ui.minWidthWindow, 256);
		theFeaturesQt->config_ui.minHeightWindow = JVX_MAX(theFeaturesQt->config_ui.minHeightWindow, 256);
		theFeaturesQt->config_ui.tweakUi = JVX_QT_MAINWIDGET_EXPAND_FULLSIZE;
	}

	configureHost_features* theFeaturesF = static_cast<configureHost_features*>(theFeaturesQt);
	if (theFeaturesF)
	{
		configureHost_features_local(theFeaturesF);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}
}



