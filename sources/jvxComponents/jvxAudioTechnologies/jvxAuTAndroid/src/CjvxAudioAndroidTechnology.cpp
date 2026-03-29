#include "CjvxAudioAndroidTechnology.h"

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
#include <jni.h>
#include <android/log.h>
#define ALOG_TAG_TECH "jvxAuTAndroid_tech"
#define TLOGD(...) __android_log_print(ANDROID_LOG_DEBUG, ALOG_TAG_TECH, __VA_ARGS__)
#define TLOGE(...) __android_log_print(ANDROID_LOG_ERROR, ALOG_TAG_TECH, __VA_ARGS__)

// Defined in componentEntry.cpp, set by JNI_OnLoad
extern JavaVM* g_jvxAuTAndroid_javaVM;
#endif

// =========================================================================================

CjvxAudioAndroidTechnology::CjvxAudioAndroidTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	LOCAL_TEMPLATE_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	passThisInit = this;
}

CjvxAudioAndroidTechnology::~CjvxAudioAndroidTechnology()
{
}

// =========================================================================================

jvxErrorType
CjvxAudioAndroidTechnology::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genAndroid_technology::init_all();
		genAndroid_technology::allocate_all();
		genAndroid_technology::register_all(static_cast<CjvxProperties*>(this));
	}
	return res;
}

jvxErrorType
CjvxAudioAndroidTechnology::unselect()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		genAndroid_technology::unregister_all(static_cast<CjvxProperties*>(this));
		genAndroid_technology::deallocate_all();
		res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::unselect();
	}
	return res;
}

// =========================================================================================
// Device enumeration via Android AudioManager JNI
// =========================================================================================

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API

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
		std::string name = jniCharSequenceToString(env, pnCS);
		if (pnCS) env->DeleteLocalRef(pnCS);
		if (name.empty()) name = "Android Device";
		env->DeleteLocalRef(di);

		// A device can be both source and sink (e.g. USB headset) — one entry per direction
		if (isSource)
		{
			JvxAndroidDeviceInfo info;
			info.deviceId    = id;
			info.deviceType  = type;
			info.isInput     = true;
			info.productName = name + " [in]";
			out.push_back(info);
			TLOGD("  INPUT  type=%d id=%d '%s'", type, id, info.productName.c_str());
		}
		if (isSink)
		{
			JvxAndroidDeviceInfo info;
			info.deviceId    = id;
			info.deviceType  = type;
			info.isInput     = false;
			info.productName = name + " [out]";
			out.push_back(info);
			TLOGD("  OUTPUT type=%d id=%d '%s'", type, id, info.productName.c_str());
		}
	}
	env->DeleteLocalRef(devArr);
	return true;
}

#endif // JVX_USE_PART_ANDROIDAUDIO_NO_API

jvxSize
CjvxAudioAndroidTechnology::enumerateAndroidDevices(std::vector<JvxAndroidDeviceInfo>& out)
{
#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
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
#else
	// Non-Android build: return empty; caller adds fallback devices
	return 0;
#endif
}

// =========================================================================================

jvxErrorType
CjvxAudioAndroidTechnology::activate()
{
	jvxErrorType res = _pre_check_activate();
	if (res == JVX_NO_ERROR)
	{
		// Enumerate hardware audio endpoints
		pendingDevices.clear();
		enumerateAndroidDevices(pendingDevices);

		if (pendingDevices.empty())
		{
			// Fallback when enumeration is unavailable or returns nothing:
			// expose one generic input device and one generic output device
			pendingDevices.push_back({0, 0, true,  "Android Input"});
			pendingDevices.push_back({0, 0, false, "Android Output"});
		}

		pendingDeviceIdx = 0;
		numberDevicesInit = pendingDevices.size();
		deviceNamePrefix  = "AndroidAudio";  // used only as template default; overridden below

		res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::activate();
	}
	return res;
}

// Called by the template's activate() loop for each device slot
CjvxAudioAndroidDevice*
CjvxAudioAndroidTechnology::local_allocate_device(
	JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE,
	jvxSize idx, jvxBool actAsProxy_init, jvxHandle* fwd_arg)
{
	if (pendingDeviceIdx >= pendingDevices.size())
		return nullptr;

	const JvxAndroidDeviceInfo& info = pendingDevices[pendingDeviceIdx++];

	// Override the template-provided description with the real device name
	CjvxAudioAndroidDevice* newDev = nullptr;
	JVX_SAFE_ALLOCATE_OBJECT(newDev,
		CjvxAudioAndroidDevice(
			info.productName.c_str(),
			multipleObjects, descriptor, featureClass, module_name,
			acTp, tpComp, descrComp, templ));

	newDev->init(this);
	newDev->isInput        = info.isInput  ? c_true : c_false;
	newDev->androidDeviceId = info.deviceId;
	return newDev;
}

jvxErrorType
CjvxAudioAndroidTechnology::deactivate()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxTemplateTechnology<CjvxAudioAndroidDevice>::deactivate();
		pendingDevices.clear();
		pendingDeviceIdx = 0;
	}
	return res;
}

// =======================================================================================

jvxErrorType
CjvxAudioAndroidTechnology::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename, jvxInt32 lineno)
{
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		genAndroid_technology::put_configuration__config_select(
			callMan, processor, sectionToContainAllSubsectionsForMe);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioAndroidTechnology::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	genAndroid_technology::get_configuration__config_select(
		callMan, processor, sectionWhereToAddAllSubsections);
	return JVX_NO_ERROR;
}
