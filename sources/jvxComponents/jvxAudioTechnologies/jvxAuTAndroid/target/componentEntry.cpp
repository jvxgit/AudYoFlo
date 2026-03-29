#include "CjvxAudioAndroidTechnology.h"

#define COMPONENT_DESCRIPTION "JVX Android Audio Technology"
#define COMPONENT_TYPE CjvxAudioAndroidTechnology
#define COMPONENT_TYPE_SPECIALIZATION JVX_COMPONENT_AUDIO_TECHNOLOGY
/*#define _JVX_ALLOW_MULTIPLE_INSTANCES*/
#define COMPONENT_DESCR_SPECIALIZATION "audio_technology/CjvxAudioAndroidTechnology"

#include "templates/targets/factory_IjvxObject.cpp"

// =============================================================
// Android: capture the JavaVM pointer when this .so is loaded.
// The technology uses it to call AudioManager.getDevices() via JNI.
// =============================================================
#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API

#include <jni.h>

JavaVM* g_jvxAuTAndroid_javaVM = nullptr;

extern "C"
{
	JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
	{
		g_jvxAuTAndroid_javaVM = vm;
		return JNI_VERSION_1_6;
	}
}

#endif // JVX_USE_PART_ANDROIDAUDIO_NO_API

