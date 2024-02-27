#include "jvx.h"

extern const char* componentsOnLoad_algorithms[];
extern const char* componentsOnLoad_audiotechnologies[];

#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"
extern "C"
{
	jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
	{

		// Setup QT Host features
		configureQtAudioHost_features* theFeaturesQT = NULL;
		configureHost_features* theFeaturesH = NULL; 
		features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesQT), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);
		if (theFeaturesQT)
		{
			// Here, you may get pointer access to the main central widget!!
			theFeaturesQT->config_ui.minWidthWindow = JVX_MAX(theFeaturesQT->config_ui.minWidthWindow, 256);
			theFeaturesQT->config_ui.minHeightWindow = JVX_MAX(theFeaturesQT->config_ui.minHeightWindow, 256);
			theFeaturesQT->config_ui.tweakUi = JVX_QT_MAINWIDGET_NO_EXPAND_CENTER;

			// Simple typcast
			theFeaturesH = static_cast<configureHost_features*>(theFeaturesQT);
		}

		// Setup Host features (no qt)
		if (theFeaturesH == nullptr)
		{
			features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesH), JVX_HOST_IMPLEMENTATION_HOST);
		}
		if (theFeaturesH)
		{
			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_NODE] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_NODE] = componentsOnLoad_algorithms;

			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_TECHNOLOGY] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_TECHNOLOGY] = componentsOnLoad_audiotechnologies;
		}

		return(JVX_NO_ERROR);
	}

	jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
	{
		configureQtAudioHost_features* theFeaturesQT = NULL;
		features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesQT), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);
		if (theFeaturesQT)
		{
		}
		configureHost_features* theFeaturesH = NULL;
		features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesH), JVX_HOST_IMPLEMENTATION_HOST);
		if (theFeaturesH)
		{
		}
		return(JVX_NO_ERROR);
	}
}
