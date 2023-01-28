#include "jvx.h"
#include "jvxQtComponentWidgets.h"

// =============================================================
// Configure ui extensions PART I
// This function is called whenever a component or technology is activated.
// It then post-loads widget element for dedicated control of specific devices.
// Currently, there is support only for JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER and
// JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE. We do not need these callbacks in this project
// =============================================================
jvxErrorType
IjvxQtComponentWidget_init(IjvxQtComponentWidget** onReturn, jvxBitField featureClass, const jvxComponentIdentification& cpId)
{
	if (onReturn)
		*onReturn = NULL;
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
IjvxQtComponentWidget_terminate(IjvxQtComponentWidget* returnMe, jvxBitField featureClass,
	const jvxComponentIdentification& cpId)
{
	return JVX_ERROR_UNSUPPORTED;
}

// =============================================================
// Configure ui extensions PART II
// This function is called whenever the main widget shall be loaded.
// =============================================================

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
// Configure parameters in the host template
// =============================================================
// 
// #include "../../common_render/src/jvxRenderAutomate.h"
#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"
#include "../../common/common_link_hook.h"
extern "C"
{
	jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
{
	configureQtAudioHost_features* theFeaturesQt = NULL;
	configureHost_features* theFeaturesF = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesQt), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);
	if (theFeaturesQt)
	{

		theFeaturesQt->config_ui.minWidthWindow = JVX_MAX(theFeaturesQt->config_ui.minWidthWindow, 600);
		theFeaturesQt->config_ui.minHeightWindow = JVX_MAX(theFeaturesQt->config_ui.minHeightWindow, 480);
		theFeaturesQt->config_ui.tweakUi = JVX_QT_MAINWIDGET_SCROLL;
		theFeaturesQt->config_ui.smallicons = true;
	}

	theFeaturesF = static_cast<configureHost_features*>(theFeaturesQt);
	if(theFeaturesF)
	{
		configureHost_features_local(theFeaturesF);

		//theStaticAutomation.setHostReference(theFeaturesF->hHost);
		//theFeaturesF->automation.if_report_automate = static_cast<IjvxReportSystem*>(&theStaticAutomation);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	configureQtAudioHost_features* theFeaturesQt = NULL;
	configureHost_features* theFeaturesF = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesQt), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);
	theFeaturesF = static_cast<configureHost_features*>(theFeaturesQt);
	if (theFeaturesF)
	{
		theFeaturesF->automation.if_report_automate = nullptr;
	}
	// theStaticAutomation.setHostReference(nullptr);
	return(JVX_NO_ERROR);
}
}

