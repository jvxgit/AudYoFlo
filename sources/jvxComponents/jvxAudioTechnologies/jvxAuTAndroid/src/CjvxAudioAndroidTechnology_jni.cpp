#include "CjvxAudioAndroidTechnology.h"

// This part only of Dll is loaded as a standalone module in Android!
#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API

#include <jni.h>

#define ALOG_TAG_TECH "jvxAuTAndroid_tech"
#define TLOGD(...) __android_log_print(ANDROID_LOG_DEBUG, ALOG_TAG_TECH, __VA_ARGS__)
#define TLOGE(...) __android_log_print(ANDROID_LOG_ERROR, ALOG_TAG_TECH, __VA_ARGS__)

JavaVM* g_jvxAuTAndroid_javaVM = nullptr;

// Typically, the JNI reference is passed to this module via sys_ptr property!!
#ifdef JVX_SHARED_LIB
// AYF_ANDROID_JNI_IN_LOCAL_LIB

extern "C"
{
	JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
	{
		g_jvxAuTAndroid_javaVM = vm;
		return JNI_VERSION_1_6;
	}
}

#endif

#endif // JVX_USE_PART_ANDROIDAUDIO_NO_API

// =========================================================================================
// Device enumeration via Android AudioManager JNI
// =========================================================================================

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API

// Maps Android AudioDeviceInfo.TYPE_* constants to human-readable labels
static const char* androidDeviceTypeName(int type)
{
	switch (type)
	{
	case 1:  return "Earpiece";
	case 2:  return "Speaker";
	case 3:  return "Wired Headphones";
	case 4:  return "Wired Headset";
	case 5:  return "Line In (Analog)";
	case 6:  return "Line In (Digital)";
	case 7:  return "Bluetooth SCO";
	case 8:  return "Bluetooth A2DP";
	case 9:  return "HDMI";
	case 10: return "HDMI ARC";
	case 11: return "USB Audio";
	case 12: return "USB Accessory";
	case 13: return "Dock";
	case 14: return "FM";
	case 15: return "Built-in Microphone";
	case 16: return "FM Tuner";
	case 17: return "TV Tuner";
	case 18: return "Telephony";
	case 19: return "Aux Line";
	case 20: return "IP Audio";
	case 21: return "Bus";
	case 22: return "USB Headset";
	case 23: return "Hearing Aid";
	case 24: return "Speaker (Safe)";
	case 25: return "Remote Submix";
	case 26: return "BLE Headset";
	case 27: return "BLE Speaker";
	case 28: return "Echo Reference";
	case 29: return "HDMI eARC";
	case 30: return "BLE Broadcast";
	default: return nullptr;
	}
}

// Helper: extract a std::string from a Java CharSequence (or String) object
static std::string jniCharSequenceToString(JNIEnv* env, jobject csObj)
{
	if (!csObj) return {};
	jclass cls = env->GetObjectClass(csObj);
	jmethodID toStringMID = env->GetMethodID(cls, "toString", "()Ljava/lang/String;");
	env->DeleteLocalRef(cls);
	if (!toStringMID) return {};
	jstring js = reinterpret_cast<jstring>(env->CallObjectMethod(csObj, toStringMID));
	if (!js) return {};
	const char* chars = env->GetStringUTFChars(js, nullptr);
	std::string result = chars ? chars : "";
	env->ReleaseStringUTFChars(js, chars);
	env->DeleteLocalRef(js);
	return result;
}

// Inner helper that does the actual JNI calls; avoids goto-past-variable issues
static bool jvxEnumDevicesJNI(JNIEnv* env, std::vector<JvxAndroidDeviceInfo>& out)
{
	// 1. Obtain Application context via ActivityThread.currentApplication()
	jclass atClass = env->FindClass("android/app/ActivityThread");
	if (!atClass) { TLOGE("FindClass ActivityThread failed"); return false; }

	jmethodID curAppMID = env->GetStaticMethodID(atClass,
		"currentApplication", "()Landroid/app/Application;");
	if (!curAppMID)
	{
		env->DeleteLocalRef(atClass);
		TLOGE("GetStaticMethodID currentApplication failed");
		return false;
	}

	jobject context = env->CallStaticObjectMethod(atClass, curAppMID);
	env->DeleteLocalRef(atClass);
	if (!context) { TLOGE("currentApplication returned null"); return false; }

	// 2. Obtain AudioManager service
	jclass ctxClass = env->FindClass("android/content/Context");
	jfieldID asFID  = env->GetStaticFieldID(ctxClass, "AUDIO_SERVICE", "Ljava/lang/String;");
	jobject asStr   = env->GetStaticObjectField(ctxClass, asFID);
	jmethodID gssM  = env->GetMethodID(ctxClass, "getSystemService",
		"(Ljava/lang/String;)Ljava/lang/Object;");
	jobject audioMgr = env->CallObjectMethod(context, gssM, asStr);
	env->DeleteLocalRef(asStr);
	env->DeleteLocalRef(ctxClass);
	env->DeleteLocalRef(context);
	if (!audioMgr) { TLOGE("getSystemService(AUDIO_SERVICE) returned null"); return false; }

	// 3. Call AudioManager.getDevices(GET_DEVICES_ALL = 3)
	jclass amClass = env->FindClass("android/media/AudioManager");
	jmethodID getDevMID = env->GetMethodID(amClass, "getDevices",
		"(I)[Landroid/media/AudioDeviceInfo;");
	env->DeleteLocalRef(amClass);
	jobjectArray devArr = reinterpret_cast<jobjectArray>(
		env->CallObjectMethod(audioMgr, getDevMID, (jint)3 /*GET_DEVICES_ALL*/));
	env->DeleteLocalRef(audioMgr);
	if (!devArr) { TLOGE("getDevices returned null"); return false; }

	// 4. Obtain AudioDeviceInfo method IDs
	jclass diClass   = env->FindClass("android/media/AudioDeviceInfo");
	jmethodID getIdM = env->GetMethodID(diClass, "getId",         "()I");
	jmethodID getTpM = env->GetMethodID(diClass, "getType",       "()I");
	jmethodID isSrcM = env->GetMethodID(diClass, "isSource",      "()Z");
	jmethodID isSnkM = env->GetMethodID(diClass, "isSink",        "()Z");
	jmethodID getPnM = env->GetMethodID(diClass, "getProductName","()Ljava/lang/CharSequence;");
	env->DeleteLocalRef(diClass);

	// 5. Iterate over AudioDeviceInfo[]
	jsize count = env->GetArrayLength(devArr);
	for (jsize i = 0; i < count; i++)
	{
		jobject di = env->GetObjectArrayElement(devArr, i);
		if (!di) continue;

		int  id       = env->CallIntMethod(di, getIdM);
		int  type     = env->CallIntMethod(di, getTpM);
		bool isSource = (bool)env->CallBooleanMethod(di, isSrcM);
		bool isSink   = (bool)env->CallBooleanMethod(di, isSnkM);

		jobject pnCS = env->CallObjectMethod(di, getPnM);
		std::string productName = jniCharSequenceToString(env, pnCS);
		if (pnCS) env->DeleteLocalRef(pnCS);
		env->DeleteLocalRef(di);

		// Build display name: type-based label as primary, productName as qualifier if informative
		const char* typeLabelCStr = androidDeviceTypeName(type);
		std::string displayBase;
		if (typeLabelCStr)
		{
			displayBase = typeLabelCStr;
			// Append productName only when it carries information beyond the type label
			if (!productName.empty()
				&& productName != typeLabelCStr
				&& productName != "Android"
				&& productName.length() > 2)
			{
				displayBase += " (" + productName + ")";
			}
		}
		else
		{
			displayBase = productName.empty() ? "Android Device" : productName;
		}

		// A device can be both source and sink (e.g. USB headset) — one entry per direction
		if (isSource)
		{
			JvxAndroidDeviceInfo info;
			info.deviceId    = id;
			info.deviceType  = type;
			info.isInput     = true;
			info.productName = displayBase + " [in]";
			out.push_back(info);
			TLOGD("  INPUT  type=%d id=%d '%s'", type, id, info.productName.c_str());
		}
		if (isSink)
		{
			JvxAndroidDeviceInfo info;
			info.deviceId    = id;
			info.deviceType  = type;
			info.isInput     = false;
			info.productName = displayBase + " [out]";
			out.push_back(info);
			TLOGD("  OUTPUT type=%d id=%d '%s'", type, id, info.productName.c_str());
		}
	}
	env->DeleteLocalRef(devArr);
	return true;
}

jvxSize
CjvxAudioAndroidTechnology::enumerateAndroidDevices(std::vector<JvxAndroidDeviceInfo>& out)
{
	if (!g_jvxAuTAndroid_javaVM)
	{
		TLOGE("enumerateAndroidDevices: no JavaVM (JNI_OnLoad not called)");
		return 0;
	}

	JNIEnv* env = nullptr;
	bool didAttach = false;
	jint stat = g_jvxAuTAndroid_javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
	if (stat == JNI_EDETACHED)
	{
		if (g_jvxAuTAndroid_javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK)
		{
			TLOGE("enumerateAndroidDevices: AttachCurrentThread failed");
			return 0;
		}
		didAttach = true;
	}
	else if (stat != JNI_OK)
	{
		TLOGE("enumerateAndroidDevices: GetEnv failed (%d)", (int)stat);
		return 0;
	}

	bool ok = jvxEnumDevicesJNI(env, out);
	if (!ok)
		TLOGE("enumerateAndroidDevices: JNI enumeration failed");

	if (didAttach)
		g_jvxAuTAndroid_javaVM->DetachCurrentThread();

	TLOGD("enumerateAndroidDevices: found %d endpoints", (int)out.size());
	return (jvxSize)out.size();
}

#endif // JVX_USE_PART_ANDROIDAUDIO_NO_API


