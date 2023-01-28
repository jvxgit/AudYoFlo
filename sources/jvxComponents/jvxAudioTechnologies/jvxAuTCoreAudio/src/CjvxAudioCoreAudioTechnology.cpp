#include "CjvxAudioCoreAudioTechnology.h"
#include <CoreAudio/AudioHardware.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFString.h>

// =========================================================================================

CjvxAudioCoreAudioTechnology::CjvxAudioCoreAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	runtime.numDevices = 0;
	runtime.devicesHandles = NULL;

}

jvxErrorType
CjvxAudioCoreAudioTechnology::activate()
{
	jvxErrorType res = CjvxAudioTechnology::activate();

	if(res == JVX_NO_ERROR)
	{
		res = initializeCoreAudio();
	}
	return(res);
}


jvxErrorType
CjvxAudioCoreAudioTechnology::deactivate()
{
	jvxSize i;
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
		auto elm = _common_tech_set.lstDevices.begin();
		for(; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			delete elm->hdlDev;
		}
		_common_tech_set.lstDevices.clear();
		terminateCoreAudio();
	}
	return(res);
}

jvxErrorType
CjvxAudioCoreAudioTechnology::initializeCoreAudio()
{
	// Step 1: Return the number of devices currently available
	UInt32 tmp;
	jvxErrorType res = JVX_NO_ERROR;
	std::string nm = "Unknown";
	std::string descr = "Unknown";
	CFStringRef str;
	UInt32 sz;

	if(!CjvxAudioCoreAudioDevice::GetPropertyDataSize(kAudioObjectSystemObject, 0,true, kAudioHardwarePropertyDevices, &tmp))
	{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
	}


	if(res == JVX_NO_ERROR)
	{
		runtime.numDevices = tmp / sizeof(AudioDeviceID);

#ifdef VERBOSE_OUTPUT
		std::cout << "Number of devices available: " << runtime.numDevices << std::endl;
#endif

		runtime.devicesHandles = new AudioDeviceID[runtime.numDevices];
		tmp = runtime.numDevices * sizeof(AudioDeviceID);

	// Return the number of devices
		if(!CjvxAudioCoreAudioDevice::GetPropertyData(kAudioObjectSystemObject, 0,true, kAudioHardwarePropertyDevices, &tmp, runtime.devicesHandles))
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		}
	}

	// Allocate memory for all devices
	if(res == JVX_NO_ERROR)
	{
		for(int i = 0; i < runtime.numDevices; i++)
		{
			// Obtain the name of the current device
			sz = sizeof(str);
			if(!CjvxAudioCoreAudioDevice::GetPropertyData_AudioDeviceN(runtime.devicesHandles[i], 0, true, kAudioObjectPropertyName, &sz, &str))
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
				break;
			}

			nm = CjvxAudioCoreAudioDevice::CFString2StdString(str);
			CFRelease(str);

			// Get the manufacturer name as description
			sz = sizeof(str);
			if(!CjvxAudioCoreAudioDevice::GetPropertyData_AudioDeviceN(runtime.devicesHandles[i], 0, true, kAudioObjectPropertyManufacturer, &sz, &str))
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
				break;
			}
			descr = (std::string)"Audio Device from Manufacturer " + CjvxAudioCoreAudioDevice::CFString2StdString(str);
			CFRelease(str);

			CjvxAudioCoreAudioDevice* ptrDev = NULL;

			ptrDev = new CjvxAudioCoreAudioDevice(nm.c_str(), false, _common_set.theDescriptor.c_str(),
									  _common_set.theFeatureClass,
									  _common_set.theModuleName.c_str(),
									  JVX_COMPONENT_ACCESS_SUB_COMPONENT,
							      JVX_COMPONENT_AUDIO_DEVICE, "", NULL
									  );
			ptrDev->initialize(descr, i, runtime.devicesHandles[i]);
			CjvxTechnology::oneDeviceWrapper elm;
			elm.hdlDev = ptrDev;
			_common_tech_set.lstDevices.push_back(elm);
		}
	}

	if(res != JVX_NO_ERROR)
	{
		this->deactivate();
	}
	return(res);
}

jvxErrorType
CjvxAudioCoreAudioTechnology::terminateCoreAudio()
{
	if(runtime.devicesHandles)
	{
		delete[](runtime.devicesHandles);
		runtime.devicesHandles = NULL;
		runtime.numDevices = 0;
	}
	return(JVX_NO_ERROR);
}
