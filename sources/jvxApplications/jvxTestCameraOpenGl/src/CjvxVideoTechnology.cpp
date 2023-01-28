#include "CjvxVideoTechnology.h"
#include <ROApi.h>

CjvxVideoTechnology::CjvxVideoTechnology()
{
	this->lstDevices = NULL;
	this->numDevices = 0;
	status = JVX_STATE_NONE;
	//CoInitialize(NULL);
}
	
jvxErrorType 
CjvxVideoTechnology::init()
{
	jvxSize i,j;
	IMFAttributes *attributes; 

	if (status == JVX_STATE_NONE)
	{
		// Initialize Microsoft Foundation
		HRESULT hr = Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);
			//Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);
		// CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		JVX_ASSERT_X("CoInitializeEx failed", 0, (hr == S_OK));

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
			CjvxVideoDevice* newDev = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newDev, CjvxVideoDevice);
			newDev->init(this, nm, lstDevices[i]);

			allDevices.push_back(newDev);

			// Free the name
			CoTaskMemFree(name);
		}

		// Release the attributes
		attributes->Release();
		status = JVX_STATE_ACTIVE;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxVideoTechnology::activateDevice(CjvxVideoDevice**retDevice, jvxSize id)
{
	if (status == JVX_STATE_ACTIVE)
	{
		if (id < allDevices.size())
		{
			if (retDevice)
			{
				CjvxVideoDevice* devRet = NULL;
				devRet = allDevices[id];
				*retDevice = devRet;
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}
	
jvxErrorType 
CjvxVideoTechnology::deactivateDevice(CjvxVideoDevice* dev)
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	if (status = JVX_STATE_ACTIVE)
	{
		for (i = 0; i < allDevices.size(); i++)
		{
			if (allDevices[i] == dev)
			{
				allDevices[i]->terminate();
				res = JVX_NO_ERROR;
				break;
			}
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxVideoTechnology::terminate()
{
	jvxSize i;
	if (status = JVX_STATE_ACTIVE)
	{
		for (i = 0; i < allDevices.size(); i++)
		{
			allDevices[i]->terminate();
		}

		if (lstDevices)
			CoTaskMemFree(lstDevices);
		lstDevices = NULL;
		numDevices = 0;

		HRESULT hr = MFShutdown();
		JVX_ASSERT_X("MFShutdown failed", 0, (hr == S_OK));

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxVideoTechnology::number_devices(jvxSize* sz)
{
	if (status = JVX_STATE_ACTIVE)
	{
		if (sz)
		{
			*sz = allDevices.size();
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxVideoTechnology::name_device(jvxSize id, std::string& nm)
{
	nm = "";
	if (status = JVX_STATE_ACTIVE)
	{
		if (id < allDevices.size())
		{
			nm = allDevices[id]->get_name();
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}