#include "CjvxNetworkSlaveTechnology.h"
#include "CjvxNetworkSlaveDevice.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxNetworkSlaveTechnology::CjvxNetworkSlaveTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxNetworkTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	socketServerFam = NULL;
}

CjvxNetworkSlaveTechnology::~CjvxNetworkSlaveTechnology()
{
}

jvxErrorType
CjvxNetworkSlaveTechnology::initializeUnderlyingConnection()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numIfs = 0;
	jvxSize i;
	char fldTxt[JVX_MAXSTRING];

	if (tp != JVX_CONNECT_SOCKET_TYPE_NONE)
	{
		res = jvx_connect_server_family_initialize_st(&this->socketServerFam,
			tp);
	}

	if (res == JVX_NO_ERROR)
	{
		numIfs = 0;
		res = jvx_connect_server_family_number_interfaces_st(this->socketServerFam, &numIfs);
		assert(res == JVX_NO_ERROR);


		for (i = 0; i < numIfs; i++)
		{
			memset(fldTxt, 0, sizeof(char) * JVX_MAXSTRING);

			res = jvx_connect_server_family_description_interfaces_st(this->socketServerFam, fldTxt, JVX_MAXSTRING, i);
			assert(res == JVX_NO_ERROR);

			oneDeviceWrapper elm;
			elm.hdlDev = allocateOneDevice(fldTxt, i);
			_common_tech_set.lstDevices.push_back(elm);
		}
	}
	return res;
}
IjvxDevice*
CjvxNetworkSlaveTechnology::allocateOneDevice(const char* fldTxt, jvxSize idx)
{
	CjvxNetworkSlaveDevice* newDevice = new CjvxNetworkSlaveDevice(fldTxt, false, _common_set.theDescriptor.c_str(),
		_common_set.theFeatureClass, _common_set.theModuleName.c_str(),
		JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "", NULL);
	newDevice->setReferences(this, idx, tp);
	return static_cast<IjvxDevice*>(newDevice);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif