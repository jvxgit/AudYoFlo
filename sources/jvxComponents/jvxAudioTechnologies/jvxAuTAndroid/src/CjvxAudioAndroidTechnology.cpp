#include "CjvxAudioAndroidTechnology.h"

// =========================================================================================

CjvxAudioAndroidTechnology::CjvxAudioAndroidTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	LOCAL_TEMPLATE_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	passThisInit = this;

	// No default allocation of devices!!
	numberDevicesInit = 0;
	deviceNamePrefix = "AndroidAudioWrongusage";  // used only as template default; overridden below	
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

jvxErrorType
CjvxAudioAndroidTechnology::activate()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::activate();
	if (res == JVX_NO_ERROR)
	{
		sys_pointer = reinterpret_cast<jvxNativeHostSysPointers*>(genAndroid_technology::config_select.sys_ptr.value);
	
		JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG, JVX_CREATE_CODE_LOCATION_TAG, nullptr);
		log << "Reading sys pointer reference as <" << sys_pointer  << "> via property." << std::endl;
		JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
		
#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
		// Obtain the JNI link - if we do not get it the environment is broken
		g_jvxAuTAndroid_javaVM = reinterpret_cast<JavaVM * >(sys_pointer->primary);
		assert(g_jvxAuTAndroid_javaVM);

		enumerateAndroidDevices(pendingDevices);
#endif

		if (pendingDevices.empty())
		{
			// Fallback when enumeration is unavailable or returns nothing:
			// expose one generic input device and one generic output device
			pendingDevices.push_back({0, 0, true,  "Android Input"});
			pendingDevices.push_back({0, 0, false, "Android Output"});
		}

		for (auto& elm : pendingDevices)
		{
			const JvxAndroidDeviceInfo& info = elm;

			// Override the template-provided description with the real device name
			CjvxAudioAndroidDevice* newDev = nullptr;
			JVX_SAFE_ALLOCATE_OBJECT(newDev,
				CjvxAudioAndroidDevice(
					info.productName.c_str(),
					false,
					_common_set.theDescriptor.c_str(),
					_common_set.theFeatureClass,
					_common_set.theModuleName.c_str(),
					JVX_COMPONENT_ACCESS_SUB_COMPONENT,
					(jvxComponentType)(_common_set.theComponentType.tp + 1),
					"", NULL));

			newDev->init(this);
			newDev->isInput = info.isInput ? c_true : c_false;
			newDev->androidDeviceId = info.deviceId;

			// Whatever to be done for initialization
			oneDeviceWrapper elmAdd;
			elmAdd.hdlDev = static_cast<IjvxDevice*>(newDev);
			_common_tech_set.lstDevices.push_back(elmAdd);
		}		
	}
	return res;
}

// Called by the template's activate() loop for each device slot
CjvxAudioAndroidDevice*
CjvxAudioAndroidTechnology::local_allocate_device(
	JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE,
	jvxSize idx, jvxBool actAsProxy_init, jvxHandle* fwd_arg)
{
	// We should not end up here since we have only REAL devices
	assert(false);
	return nullptr;
}

jvxErrorType
CjvxAudioAndroidTechnology::deactivate()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioAndroidDevice>::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_API
		// Return the JNI link - if we do not get it the environment is broken
		g_jvxAuTAndroid_javaVM = nullptr;
#endif

		JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG, JVX_CREATE_CODE_LOCATION_TAG, nullptr);
		log << "Releasing sys pointer reference." << std::endl;
		JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);

		CjvxTemplateTechnology<CjvxAudioAndroidDevice>::deactivate();
		pendingDevices.clear();

		sys_pointer = nullptr;
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
