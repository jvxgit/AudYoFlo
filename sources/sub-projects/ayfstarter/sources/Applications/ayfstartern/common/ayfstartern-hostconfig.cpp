#include "jvx.h"
#include "../common/ayfstartern-common.h"

#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"

static jvxNativeHostSysPointers theSystemHandles;

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
		if (theFeaturesH == nullptr)
		{
			features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesH), JVX_HOST_IMPLEMENTATION_LIB_HOST);
		}
		if (theFeaturesH)
		{
			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_NODE] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_NODE] = componentsOnLoad_algorithms;
			theFeaturesH->numSlotsComponents[JVX_COMPONENT_AUDIO_NODE] = 2;

			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_TECHNOLOGY] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_TECHNOLOGY] = componentsOnLoad_audiotechnologies;
#ifdef JVX_FFMPEG_FILE_IO
			theFeaturesH->numSlotsComponents[JVX_COMPONENT_AUDIO_TECHNOLOGY] = 5;
#else
			theFeaturesH->numSlotsComponents[JVX_COMPONENT_AUDIO_TECHNOLOGY] = 3;
#endif

			// For the specific audio technology, allow 2 devices!
			theFeaturesH->mapSpecSubSlots[jvxComponentIdentification(JVX_COMPONENT_AUDIO_TECHNOLOGY, 1)] = 2;

			// Add video technology in default setup
			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_VIDEO_TECHNOLOGY] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_VIDEO_TECHNOLOGY] = componentsOnLoad_videotechnologies;
			theFeaturesH->numSlotsComponents[JVX_COMPONENT_VIDEO_TECHNOLOGY] = 1;

			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_VIDEO_NODE] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_VIDEO_NODE] = componentsOnLoad_videonodes;
			theFeaturesH->numSlotsComponents[JVX_COMPONENT_VIDEO_NODE] = 1;

			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_SYSTEM_AUTOMATION] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_SYSTEM_AUTOMATION] = componentsOnLoad_automation;

			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_SIGNAL_PROCESSING_NODE] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_SIGNAL_PROCESSING_NODE] = componentsOnLoad_spnodes;
			theFeaturesH->numSlotsComponents[JVX_COMPONENT_SIGNAL_PROCESSING_NODE] = 3;

			// Activate usage of dynamic nodes
			theFeaturesH->numSlotsComponents[JVX_COMPONENT_DYNAMIC_NODE] = JVX_SIZE_UNSELECTED; 
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

	// Expose this function for cross referencing
	jvxErrorType flutter_vmref_open(jvxNativeHostSysPointers* refsOnReturn)
	{
		if (refsOnReturn) *refsOnReturn = theSystemHandles;
		return JVX_NO_ERROR;
	};
}
