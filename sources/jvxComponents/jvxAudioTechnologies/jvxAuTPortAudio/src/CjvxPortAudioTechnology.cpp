#include "CjvxPortAudioTechnology.h"

// Port audio headers
#include "portaudio.h"
#ifdef JVX_OS_WINDOWS
#include "pa_asio.h"
#endif

CjvxPortAudioTechnology::CjvxPortAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

CjvxPortAudioTechnology::~CjvxPortAudioTechnology()
{
}

jvxErrorType
CjvxPortAudioTechnology::select(IjvxObject* theOwner)
{
	jvxSize i;
	jvxErrorType res = CjvxAudioTechnology::select( theOwner);
	if(res == JVX_NO_ERROR)
	{
		genPortAudio_technology::allocate__properties_selected();

		// Initialize port audio
		Pa_Initialize();

		PaHostApiIndex numApis = Pa_GetHostApiCount();
		if (numApis > 0)
		{
			for (i = 0; i < numApis; i++)
			{
				const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(i);
				genPortAudio_technology::properties_selected.platformFilters.value.entries.push_back(hostInfo->name);
			}
		}
		genPortAudio_technology::properties_selected.platformFilters.value.exclusive = 0;
		genPortAudio_technology::properties_selected.platformFilters.value.selection() = ((jvxBitField)1 << numApis) - 1; // Select all!

		genPortAudio_technology::register__properties_selected(static_cast<CjvxProperties*>(this));
	}
	return(res);
}

jvxErrorType
CjvxPortAudioTechnology::activate()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioTechnology::activate();
	if(res == JVX_NO_ERROR)
	{
		// For each available device, allocate one object. The device specific part is handled in device objects
		int numberDevices =  Pa_GetDeviceCount();
		if (numberDevices > 0)
		{
			for (i = 0; i < numberDevices; i++)
			{
				const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
				const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

				if (JVX_EVALUATE_BITFIELD(((jvxBitField)1 << deviceInfo->hostApi) & genPortAudio_technology::properties_selected.platformFilters.value.selection()))
				{
					std::string device_name = "[";
					device_name += hostInfo->name;
					device_name += "]: ";
					device_name += jvx::helper::filterEscapes(jvx::helper::asciToUtf8(deviceInfo->name));
					// device_name += jvx::helper::asciToUtf8(deviceInfo->name);

					CjvxPortAudioDevice* ptrDev = new CjvxPortAudioDevice(device_name.c_str(), false, _common_set.theDescriptor.c_str(), _common_set.theFeatureClass,
						_common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "", NULL);
					ptrDev->props_init(i, deviceInfo);
					CjvxTechnology::oneDeviceWrapper elm;
					elm.hdlDev = ptrDev;
					_common_tech_set.lstDevices.push_back(elm);
				}
			}
		}
		//update_capabilities();
	}
	return(res);
}

jvxErrorType
CjvxPortAudioTechnology::deactivate()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioTechnology::_pre_check_deactivate();
	if(res == JVX_NO_ERROR)
	{
		auto elm = _common_tech_set.lstDevices.begin();
		for (; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			delete(elm->hdlDev);
		}
		_common_tech_set.lstDevices.clear();
		CjvxAudioTechnology::deactivate();
	}
	return(res);
}

jvxErrorType
CjvxPortAudioTechnology::unselect()
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(_common_set_min.theState == JVX_STATE_SELECTED)
	{
		Pa_Terminate();
		res = CjvxAudioTechnology::unselect();
	}
	return(res);
}
