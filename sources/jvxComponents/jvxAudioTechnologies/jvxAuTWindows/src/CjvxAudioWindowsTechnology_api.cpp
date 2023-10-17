#include "CjvxAudioWindowsTechnology.h"

#define INITGUID
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <functiondiscoverykeys.h>
#include <mmdeviceapi.h>
#include <strsafe.h>

// ==========================================================================
template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

jvxErrorType
scan_device(IMMDeviceCollection* DeviceCollection, IMMDevice** device, WAVEFORMATEXTENSIBLE* pwex, std::string& retVal, jvxSize DeviceIndex, jvxBool& defDevice, LPWSTR deviceIdDefault)
{
    jvxErrorType res = JVX_NO_ERROR;
    
    LPWSTR deviceId;
    HRESULT hr;

    WAVEFORMATEXTENSIBLE wext = { 0 };
    WAVEFORMATEX wex = { 0 };
    if (*device == nullptr)
    {
        hr = DeviceCollection->Item(DeviceIndex, device);
        if (FAILED(hr))
        {
            printf("Unable to get device " JVX_PRINTF_CAST_SIZE ": %x\n", DeviceIndex, hr);
            res = JVX_ERROR_INTERNAL;
            goto leave_exit_error;
        }
    }
    hr = (*device)->GetId(&deviceId);
    if (FAILED(hr))
    {
        printf("Unable to get device " JVX_PRINTF_CAST_SIZE " id: %x\n", DeviceIndex, hr);
        res = JVX_ERROR_INTERNAL;
        goto leave_exit_error;
    }

    defDevice = false;
    if (wcscmp(deviceId, deviceIdDefault) == 0)
    {
        defDevice = true;
    }

    IPropertyStore* propertyStore;
    hr = (*device)->OpenPropertyStore(STGM_READ, &propertyStore);
    if (FAILED(hr))
    {
        printf("Unable to open device " JVX_PRINTF_CAST_SIZE " property store: %x\n", DeviceIndex, hr);
        res = JVX_ERROR_INTERNAL;
        goto leave_exit_error;
    }

    PROPVARIANT friendlyName;
    PropVariantInit(&friendlyName);
    hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);

    if (FAILED(hr))
    {
        printf("Unable to retrieve friendly name for device " JVX_PRINTF_CAST_SIZE " : %x\n", DeviceIndex, hr);
        res = JVX_ERROR_INTERNAL;
        goto leave_exit_error;
    }

    wchar_t deviceNamew[128] = { 0 };
    char deviceName[128] = { 0 };
    switch (friendlyName.vt)
    {
    case VT_LPWSTR:
        hr = StringCbPrintfW(deviceNamew, sizeof(deviceNamew), L"%s", friendlyName.pwszVal);
        (LONG)WideCharToMultiByte(CP_ACP, 0, (LPWSTR)deviceNamew, -1, deviceName, sizeof(deviceNamew), 0, 0);
        break;
    case VT_LPSTR:
        hr = StringCbPrintfA(deviceName, sizeof(deviceName), "%s", friendlyName.pszVal);
        break;
    }
    if (FAILED(hr))
    {
        assert(0);
    }

    // ======================================================================
   
    PROPVARIANT channels;
    PropVariantInit(&channels);
    hr = propertyStore->GetValue(PKEY_AudioEndpoint_PhysicalSpeakers, &channels);

    // =============================================================================

    PROPVARIANT devFormat;
    PropVariantInit(&devFormat);
    hr = propertyStore->GetValue(PKEY_AudioEngine_DeviceFormat, &devFormat);
    if (devFormat.vt == VT_BLOB)
    {
        WAVEFORMATEX* wformEx = (WAVEFORMATEX*)devFormat.blob.pBlobData;
        *((WAVEFORMATEX*)pwex) = *wformEx;
        WAVEFORMATEXTENSIBLE* wformExt = (WAVEFORMATEXTENSIBLE*)wformEx;
        if (wformEx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
        {
            *pwex = *wformExt;
        }
    }
    // =============================================================================

    PropVariantClear(&friendlyName);
    PropVariantClear(&channels);
    PropVariantClear(&devFormat);
    
    SafeRelease(&propertyStore);

    CoTaskMemFree(deviceId);
    
    retVal = deviceName;

    return res;
leave_exit_error:
    return res;
}

// ==================================================================
void
CjvxAudioWindowsTechnology::activate_windows_audio_technology()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    UINT deviceCount = 0;

	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollectionInput = NULL;
    IMMDeviceCollection* deviceCollectionOutput = NULL;

    LPWSTR deviceIdDefault = nullptr;
    IMMDevice* device = nullptr;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		assert(0);
	}

    deviceCount = 0;
	hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &deviceCollectionInput);
	if (FAILED(hr))
	{
		assert(0);
	}
	    
    hr = deviceCollectionInput->GetCount(&deviceCount);
	if (FAILED(hr))
	{
		assert(0);
	}

    // First, open the default device to identify it
     device = nullptr;
    deviceIdDefault = nullptr;
    deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &device);
    if (device)
    {
        hr = device->GetId(&deviceIdDefault);
        if (FAILED(hr))
        {
            assert(0);
        }
        device->Release();
        device = nullptr;
    }


	for (UINT i = 0; i < deviceCount; i += 1)
	{
        device = nullptr;
		std::string deviceName;
        jvxBool defDevice = false;
        WAVEFORMATEXTENSIBLE wext = { 0 };
        
        if (scan_device(deviceCollectionInput, &device, &wext, deviceName, i, defDevice, deviceIdDefault) == JVX_NO_ERROR)
        {
            std::string txt = _common_set.theComponentSubTypeDescriptor;
            txt += "/input";

            // Modified HK: According to 
            // https://learn.microsoft.com/de-de/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immdeviceenumerator-getdefaultaudioendpoint
            // there is no need to add a reference as it holds one if open
             
            // Keep this handle on, therefore ref count
            // device->AddRef();

            // Convert device name to UTF8
            deviceName = jvx::helper::asciToUtf8(deviceName);

            CjvxAudioWindowsDevice* newDevice = new CjvxAudioWindowsDevice(
                deviceName.c_str(), false,
                _common_set.theDescriptor.c_str(),
                _common_set.theFeatureClass,
                _common_set.theModuleName.c_str(),
                JVX_COMPONENT_ACCESS_SUB_COMPONENT,
                (jvxComponentType)(_common_set.theComponentType.tp + 1),
                txt.c_str(), NULL);
            newDevice->setHandle(this, device, &wext, true, defDevice);

            // Whatever to be done for initialization
            oneDeviceWrapper elm;
            elm.hdlDev = static_cast<IjvxDevice*>(newDevice);
            _common_tech_set.lstDevices.push_back(elm);
        }
	}

    // ============================================================

    deviceCount = 0;
    hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollectionOutput);
    if (FAILED(hr))
    {
        assert(0);
    }

    hr = deviceCollectionOutput->GetCount(&deviceCount);
    if (FAILED(hr))
    {
        assert(0);
    }
    
    // First, open the default device to identify it
    device = nullptr;
    deviceIdDefault = nullptr;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eCommunications, &device); 
    if (device)
    {
        hr = device->GetId(&deviceIdDefault);
        if (FAILED(hr))
        {
            assert(0);
        }
        device->Release();
        device = nullptr;
    }

    for (UINT i = 0; i < deviceCount; i += 1)
    {
        device = nullptr;
        std::string deviceName;
        jvxBool defDevice = false;
        WAVEFORMATEXTENSIBLE wext = { 0 };

        if (scan_device(deviceCollectionOutput, &device, &wext, deviceName, i, defDevice, deviceIdDefault) == JVX_NO_ERROR)
        {
            std::string txt = _common_set.theComponentSubTypeDescriptor;
            txt += "/output";

            // Modified HK: According to 
            // https://learn.microsoft.com/de-de/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immdeviceenumerator-getdefaultaudioendpoint
            // there is no need to add a reference as it holds one if open
            
            // Keep this handle on, therefore ref count
            // device->AddRef();
            
            // Convert device name to UTF8
            deviceName = jvx::helper::asciToUtf8(deviceName);

            CjvxAudioWindowsDevice* newDevice = new CjvxAudioWindowsDevice(
                deviceName.c_str(), false,
                _common_set.theDescriptor.c_str(),
                _common_set.theFeatureClass,
                _common_set.theModuleName.c_str(),
                JVX_COMPONENT_ACCESS_SUB_COMPONENT,
                (jvxComponentType)(_common_set.theComponentType.tp + 1),
                txt.c_str(), NULL);
            newDevice->setHandle(this, device, &wext, false, defDevice);

            // Whatever to be done for initialization
            oneDeviceWrapper elm;
            elm.hdlDev = static_cast<IjvxDevice*>(newDevice);
            _common_tech_set.lstDevices.push_back(elm);
        }
    }

    SafeRelease(&deviceCollectionInput);
    SafeRelease(&deviceCollectionOutput);
    SafeRelease(&deviceEnumerator);

}

void
CjvxAudioWindowsTechnology::deactivate_windows_audio_device(IMMDevice* device)
{
    if (device)
    {
        SafeRelease(&device);
    }
}

void
CjvxAudioWindowsTechnology::deactivate_windows_audio_technology()
{
    CoUninitialize();
}
