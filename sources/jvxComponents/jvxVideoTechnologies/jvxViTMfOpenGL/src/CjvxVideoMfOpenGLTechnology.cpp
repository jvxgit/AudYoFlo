#include "CjvxVideoMfOpenGLTechnology.h"
#include <ROApi.h>

CjvxVideoMfOpenGLTechnology::CjvxVideoMfOpenGLTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxVideoTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

jvxErrorType
CjvxVideoMfOpenGLTechnology::activate()
{
	jvxErrorType res = CjvxVideoTechnology::activate();

	if(res == JVX_NO_ERROR)
	{
		jvxSize i, j;
		IMFAttributes *attributes;

		requiresComUninitialize = false;

		// Initialize Microsoft Foundation
		HRESULT hr = Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);
		// CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		// Here is one problem:
		// If using QT, the environment is set to SINGLETHREADED APPARTMENT. In that case,
		// the call "Windows::Foundation::Initialize" fails since it is normally called with 
		// RO_INIT_MULTITHREADED.
		// One option is to ignore the failure - I do not know what happens in this case.
		// The other option would be to add the line 
		// HRESULT hr = Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);
		// in the function "jvx_init_before_start_local" to redefine the default threadng model.
		// In that case, however, the QT environment outputs an error message "OleInitialize failed".
		// It is described that in GUI applications, always a single threaded appartment should be used.
		// I do not know how to solve this issue. I think that I do not need multithreaded appartments
		// here
		// 			// #include <ROApi.h>
		if (hr == S_OK)
		{
			requiresComUninitialize = true;
		}
		else if (hr == RPC_E_CHANGED_MODE)
		{
			std::cout << "Warning: Windows::Foundation::Initialize(RO_INIT_MULTITHREADED) failed with error code " << hr <<
				", hence running in STA environment!" << std::endl;
		}

		//JVX_ASSERT_X("CoInitializeEx failed", 0, (hr == S_OK));

		// Start Foundation stuff
		hr = MFStartup(MF_VERSION);
		JVX_ASSERT_X("MFStartup failed", 0, (hr == S_OK));

		// Create attributes to hold one entry
		attributes = NULL;
		hr = MFCreateAttributes(&attributes, 1);
		JVX_ASSERT_X("MFCreateAttributes failed", 0, (hr == S_OK));

		// Specify attribute to open device source video
		hr = attributes->SetGUID(
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
		);
		JVX_ASSERT_X("attributes->SetGUID failed", 0, (hr == S_OK));

		// List the devices complying with these attributes
		hr = MFEnumDeviceSources(attributes, &lstDevices, &numDevices);
		JVX_ASSERT_X("MFEnumDeviceSources failed", 0, (hr == S_OK));

		// Transform list of devices into string names and allocate list of devices
		for (i = 0; i < numDevices; i++)
		{
			WCHAR *name = 0;
			UINT32 namelen = 255;
			hr = lstDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &name, &namelen);
			JVX_ASSERT_X("lstDevices[i]->GetAllocatedString failed", 0, (hr == S_OK));
			std::string nm;
			for (j = 0; j < namelen; j++)
			{
				char cc[2] = { 0 };
				cc[0] = (char)name[j];
				nm += cc;
			}

			// Allocate new device handle in user space
			CjvxVideoMfOpenGLDevice* newDevice = new CjvxVideoMfOpenGLDevice(nm.c_str(), false, _common_set.theDescriptor.c_str(), _common_set.theFeatureClass,
				_common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_VIDEO_DEVICE, "device/video_device", NULL);

			newDevice->init(this, lstDevices[i]);

			// Whatever to be done for initialization
			oneDeviceWrapper elm;
			elm.hdlDev = static_cast<IjvxDevice*>(newDevice);

			_common_tech_set.lstDevices.push_back(elm);

			// Free the name
			CoTaskMemFree(name);
		}

		// Release the attributes
		attributes->Release();
		return JVX_NO_ERROR;
	}
	return(res);
}


jvxErrorType
CjvxVideoMfOpenGLTechnology::deactivate()
{
	jvxSize i;
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
		auto elm = _common_tech_set.lstDevices.begin();
		for (; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			delete(elm->hdlDev);
		}
		_common_tech_set.lstDevices.clear();

		if (lstDevices)
			CoTaskMemFree(lstDevices);
		lstDevices = NULL;
		numDevices = 0;

		HRESULT hr = MFShutdown();
		JVX_ASSERT_X("MFShutdown failed", 0, (hr == S_OK));

		if (requiresComUninitialize)
		{
			Windows::Foundation::Uninitialize();
		}
	}
	return(res);
}

