#include "CjvxAudioPWireTechnology.h"

#define JVX_ASYNC_TIMEOUT 5000


// ================================================================================0
// Some hints about samplerates: https://gitlab.freedesktop.org/pipewire/pipewire/-/wikis/Guide-Rates
// -> Command: <pw-metadata -n settings 0 clock.force-rate <samplerate>>
// -> Make an entry in ~/.config/pipewire/pipewire.conf.d/10-rates.conf  -> 
// # Adds more common rates
// context.properties = {
//     default.clock.allowed-rates = [ 44100 48000 88200 96000 ]
// }

// How to delete links in helvum: https://gitlab.freedesktop.org/pipewire/helvum/-/issues/50
// -> same connection as on connect will disconnect
// Set the buffersize: https://linuxmusicians.com/viewtopic.php?t=25768
// -> pw-metadata -n settings 0 clock.force-quantum <buffersize>
// Works even with odd numbers like 53 :-)
//

static void on_async_done(void *data, uint32_t id, int seq)
{
    if(data)
    {
        CjvxAudioPWireTechnology* this_pointer = (CjvxAudioPWireTechnology* ) data;
        if(this_pointer)
        {
            this_pointer->async_run_cb(id, seq);
        }
    }
}

static void registry_event_global(void *data, uint32_t id,
                uint32_t permissions, const char *type, uint32_t version,
                const struct spa_dict *props)
{
    if(data)
    {
        CjvxAudioPWireTechnology* this_pointer = (CjvxAudioPWireTechnology* ) data;
        if(this_pointer)
        {
            this_pointer->event_global_callback(id, permissions, type, version, props);
        }
    }
        
}

static const struct pw_registry_events registry_events = 
{
    PW_VERSION_REGISTRY_EVENTS,
    .global = registry_event_global,
};

static const struct pw_core_events async_events =
{
    PW_VERSION_CORE_EVENTS,
    .done = on_async_done,
};

// ========================================================================================
// ========================================================================================

void 
CjvxAudioPWireTechnology::activate_technology_api()
{
	jvxSize i;
	std::string nmPrefix = "Dummy";
	for (i = 0; i < 3; i++)
	{
		std::string devName = nmPrefix + "_" + jvx_size2String(i);

		// Do whatever is required
		CjvxAudioPWireDevice* newDevice = local_allocate_device(devName.c_str(), false,
			_common_set.theDescriptor.c_str(),
			_common_set.theFeatureClass,
			_common_set.theModuleName.c_str(),
			JVX_COMPONENT_ACCESS_SUB_COMPONENT,
			(jvxComponentType)(_common_set.theComponentType.tp + 1),
			"", NULL, JVX_SIZE_UNSELECTED, false);

		// Whatever to be done for initialization
		oneDeviceWrapper elm;
		elm.hdlDev = static_cast<IjvxDevice*>(newDevice);
		elm.actsAsProxy = false;
		_common_tech_set.lstDevices.push_back(elm);
	}

    pw_init(&argc, &argv);

    std::cout << "Compiled with libpipewire " << pw_get_headers_version() << ", linked with libpipewire " << pw_get_library_version() << std::endl;

    loop = pw_thread_loop_new("The Technology Loop", NULL /* properties */);
    context = pw_context_new(pw_thread_loop_get_loop(loop),
                             NULL /* properties */,
                             0 /* user_data size */);

    core = pw_context_connect(context,
                              NULL /* properties */,
                              0 /* user_data size */);

    registry = pw_core_get_registry(core, PW_VERSION_REGISTRY,
                                    0 /* user_data size */);

    spa_zero(registry_listener);
    pw_registry_add_listener(registry, &registry_listener,
                             &registry_events, this);

    this->async_run_start();

    pw_thread_loop_start(loop);

    this->async_run_trigger();

    std::cout << __FUNCTION__ << ": Initialization of Pipewire service complete."  << std::endl;
};

void 
CjvxAudioPWireTechnology::deactivate_technology_api()
{
    this->async_run_stop();
    pw_proxy_destroy((struct pw_proxy *)registry);
    pw_core_disconnect(core);
    pw_context_destroy(context);
    pw_thread_loop_destroy(loop);
    pw_deinit();

    argc = 0;
    argv = nullptr;
    loop = nullptr;
    context = nullptr;
    core = nullptr;
    registry = nullptr;
    struct spa_hook registry_listener;
}

// ========================================================================================
// ========================================================================================

void 
CjvxAudioPWireTechnology::event_global_callback(uint32_t id,
                uint32_t permissions, const char *type, uint32_t version,
                const struct spa_dict *props)
{
    if (strcmp(type, PW_TYPE_INTERFACE_Device) == 0) {
        const char *media_class = spa_dict_lookup(props, "media.class");
        if (strstr(media_class, "Device") || strstr(media_class, "device")) {
            const char *nick = spa_dict_lookup(props, "device.nick");
            const char *descr = spa_dict_lookup(props, "device.description");
            const char *api_name = spa_dict_lookup(props, "device.api");
            const char *name = spa_dict_lookup(props, "device.name");
            oneDevice* newDev = nullptr;
            JVX_SAFE_ALLOCATE_OBJECT(newDev, oneDevice);
            newDev->id = id;
            newDev->description = descr;
            newDev->nick = nick;
            newDev->api_name = api_name;
            newDev->name = name;

            devices_unsorted.push_back(newDev);
        }

    if (strcmp(type, PW_TYPE_INTERFACE_Node) == 0) {
        const char *media_class = spa_dict_lookup(props, "media.class");
        if (strstr(media_class, "Sink") || strstr(media_class, "sink")) 
        {
            const char *dev_id = spa_dict_lookup(props, "device.id");
            const char *nick = spa_dict_lookup(props, "node.nick");
            const char *descr = spa_dict_lookup(props, "node.description");
            const char *name = spa_dict_lookup(props, "node.name");

            oneNode* newNode = nullptr;
            JVX_SAFE_ALLOCATE_OBJECT(newNode, oneNode);
            newNode->id = id;
            newNode->ref_device = atoi(dev_id);
            newNode->description = descr;
            newNode->nick = nick;
            newNode->name = name;
            nodes_unsorted_sinks.push_back(newNode);
        }
        else if (strstr(media_class, "Source") || strstr(media_class, "source")) 
        {
            const char *dev_id = spa_dict_lookup(props, "device.id");
            const char *nick = spa_dict_lookup(props, "node.nick");
            const char *descr = spa_dict_lookup(props, "node.description");
            const char *name = spa_dict_lookup(props, "node.name");

            oneNode* newNode = nullptr;
            JVX_SAFE_ALLOCATE_OBJECT(newNode, oneNode);
            newNode->id = id;
            newNode->ref_device = atoi(dev_id);
            newNode->description = descr;
            newNode->nick = nick;
            newNode->name = name;
            nodes_unsorted_sources.push_back(newNode);
        }
    }

    if (strcmp(type, PW_TYPE_INTERFACE_Port) == 0) {
        const char *audio_channel = spa_dict_lookup(props, "audio.channel");
        if (audio_channel) 
        {
            const char *node_id = spa_dict_lookup(props, "node.id");
            const char *nick = spa_dict_lookup(props, "port.nick");
                        const char *nick = spa_dict_lookup(props, "port.nick");
            const char *descr = spa_dict_lookup(props, "node.description");
            const char *name = spa_dict_lookup(props, "node.name");
            const char *direction = spa_dict_lookup(props, "node.direction");

            onePort* newPort = nullptr;
            JVX_SAFE_ALLOCATE_OBJECT(newPort, onePort);
            newPort->id = id;
            newPort->ref_node = atoi(node_id);            
            newPort->nick = nick;
            newPort->name = name;
            newPort->direction = direction;
            ports_unsorted.push_back(newPort);
        }        
    }

    // printf("object: id:%u type:%s/%d\n", id, type, version);

    // Ausgabe für Audioquellen (sources)
#if 0
    if (strcmp(type, PW_TYPE_INTERFACE_Node) == 0) {
        const char *media_class = spa_dict_lookup(props, "media.class");
        const char *name = spa_dict_lookup(props, "node.name");
        
        if (media_class && name) {
            if (strstr(media_class, "Source") || strstr(media_class, "source")) 
            {
                printf("Audio Source:\n");
                printf("  ID: %d\n", id);
                printf("  Name: %s\n", name);
                printf("  Media Class: %s\n\n", media_class);

                printf("=== Detailed Properties ===\n");
    
    // Iterate through all properties
    for (uint32_t i = 0; i < props->n_items; i++) {
        const struct spa_dict_item *item = &props->items[i];
        
        // Nur nicht-leere Eigenschaften ausgeben
        if (item->key && item->value) {
            printf("%s: %s\n", item->key, item->value);
        }
    }

                /*
                const char* channels_str = spa_dict_lookup(props, "audio.channels");
                if (!channels_str)
                    channels_str = spa_dict_lookup(props, "node.channel-map");
                if (!channels_str)
                    channels_str = spa_dict_lookup(props, "spa.audio.channels");

                if (channels_str) {
                    printf("  Channels: %s\n", channels_str);
                } else {
                    printf("  Channels: Unable to determine\n");
                }

                const char *sample_rate_str = NULL;
                sample_rate_str = spa_dict_lookup(props, "audio.rate");
                if (!sample_rate_str)
                    sample_rate_str = spa_dict_lookup(props, "spa.audio.rate");
                if (!sample_rate_str)
                    sample_rate_str = spa_dict_lookup(props, "device.rate");

                if (sample_rate_str) {
                    printf("  Sample Rate: %s Hz\n", sample_rate_str);
                } else {
                    printf("  Sample Rate: Unable to determine\n");
                }

                const char *format_str = NULL;
                sample_rate_str = spa_dict_lookup(props, "audio.format");
                if (!sample_rate_str)
                    sample_rate_str = spa_dict_lookup(props, "spa.audio.format");
                if (!sample_rate_str)
                    sample_rate_str = spa_dict_lookup(props, "device.format");

                if (format_str) {
                    printf("  Format: %s \n", format_str);
                } else {
                    printf("  Format: Unable to determine\n");
                }
                */
            }
            
            if (strstr(media_class, "Sink") || strstr(media_class, "sink")) 
            {
                printf("Audio Sink:\n");
                printf("  ID: %d\n", id);
                printf("  Name: %s\n", name);
                printf("  Media Class: %s\n\n", media_class);
            }
        }
        #endif
    }
}

// ========================================================================================
// ========================================================================================

// All related to async system requests - already synchronized 
void 
CjvxAudioPWireTechnology::async_run_start()
{
    pw_core_add_listener(core, &async_listener, &async_events, this);
}

void 
CjvxAudioPWireTechnology::async_run_stop()
{
    spa_hook_remove(&async_listener);
}

jvxErrorType 
CjvxAudioPWireTechnology::async_run_trigger()
{
    if(async_run.idMess_core != 0)
    {
        return JVX_ERROR_COMPONENT_BUSY;
    }
    JVX_WAIT_FOR_NOTIFICATION_I(async_run.notWait);
    async_run.idMess_received = 0;
    async_run.idMess_core = pw_core_sync(core, PW_ID_CORE, 0);
    JVX_WAIT_FOR_NOTIFICATION_II_MS(async_run.notWait, JVX_ASYNC_TIMEOUT);

    if(async_run.idMess_received == async_run.idMess_core )
    {
        std::cout << __FUNCTION__ << "Async message transfer successfully completed." << std::endl;        
    }
    else
    {
        std::cout << __FUNCTION__ << "Error in asynchronous message transfer." << std::endl;
    }
    return JVX_NO_ERROR;
}

// Collect requested data right here!!
// ========================================================================================
// ========================================================================================

void 
CjvxAudioPWireTechnology::async_run_cb(jvxUInt32 id, int seq)
{
    if(seq == async_run.idMess_core )
    {
        async_run.idMess_received = seq;
        JVX_SET_NOTIFICATION(async_run.notWait);
    }
}
#if 0

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

#endif
