#include "jvx.h"

static jvxNativeHostSysPointers theSystemHandles;

#ifdef JVX_OS_ANDROID

#include "jni.h"
#include <android/log.h>

#define LOG_TAG "ayfstarter"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	// Initialisierungscode, z.B. Caching von Klassen oder Methoden
	LOGI("AYF - Entering function <%s>", __FUNCTION__);

	// Return the entries required for access to vm or/and environment
	theSystemHandles.primary = reinterpret_cast<jvxHandle*>(vm);
	int getEnvStat = vm->GetEnv((void**)&theSystemHandles.secondary, JNI_VERSION_1_6);

	theSystemHandles.thread_id = JVX_GET_CURRENT_THREAD_ID();

	LOGI("## Obtained reference to vm = %p", theSystemHandles.primary);
	LOGI("## Obtained reference to env = %p", theSystemHandles.secondary);
	LOGI("## Returned current thread id = %li", theSystemHandles.thread_id);

	return JNI_VERSION_1_6; // oder eine andere unterstützte Version
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
	LOGI("AYF - Entering function <%s>", __FUNCTION__);
}

#endif

// ========================================================
// Declare the autostart pre-selections
// ========================================================

#include "../common/ayfstarter-common.h"

// ========================================================
// ========================================================

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
		if (theFeaturesH == nullptr)
		{
			features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesH), JVX_HOST_IMPLEMENTATION_LIB_HOST);
		}
		if (theFeaturesH)
		{
			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_NODE] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_NODE] = componentsOnLoad_algorithms;

			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_TECHNOLOGY] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_TECHNOLOGY] = componentsOnLoad_audiotechnologies;

			theFeaturesH->flag_blockModuleEdit[JVX_COMPONENT_SYSTEM_AUTOMATION] = true;
			theFeaturesH->lst_ModulesOnStart[JVX_COMPONENT_SYSTEM_AUTOMATION] = componentsOnLoad_automation;
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
