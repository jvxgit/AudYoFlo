#include "CjvxNetworkMasterTechnology.h"
#include "CjvxNetworkMasterDevice.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

	CjvxNetworkMasterTechnology::CjvxNetworkMasterTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxNetworkTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		socketClientFam = NULL;
	}

	CjvxNetworkMasterTechnology::~CjvxNetworkMasterTechnology()
	{
	}

jvxErrorType
CjvxNetworkMasterTechnology::initializeUnderlyingConnection()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numIfs = 0;
	jvxSize i;
	char fldTxt[JVX_MAXSTRING];

	if (tp != JVX_CONNECT_SOCKET_TYPE_NONE)
	{
		res = jvx_connect_client_family_initialize_st(&this->socketClientFam,
			tp);
	}

	if (res == JVX_NO_ERROR)
	{
		numIfs = 0;
		res = jvx_connect_client_family_number_interfaces_st(this->socketClientFam, &numIfs);
		assert(res == JVX_NO_ERROR);


		for (i = 0; i < numIfs; i++)
		{
			memset(fldTxt, 0, sizeof(char) * JVX_MAXSTRING);

			res = jvx_connect_client_family_description_interfaces_st(this->socketClientFam, fldTxt, JVX_MAXSTRING, i);
			assert(res == JVX_NO_ERROR);

			oneDeviceWrapper elm;
			elm.hdlDev = allocateOneDevice(fldTxt, i);
			_common_tech_set.lstDevices.push_back(elm);
		}
	}
	return res;
}

IjvxDevice*
CjvxNetworkMasterTechnology::allocateOneDevice(const char* fldTxt, jvxSize idx)
{
	CjvxNetworkMasterDevice* newDevice = new CjvxNetworkMasterDevice(fldTxt, false, _common_set.theDescriptor.c_str(),
		_common_set.theFeatureClass, _common_set.theModuleName.c_str(),
		JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "", NULL);
	newDevice->setReferences(this, idx, tp);
	return static_cast<IjvxDevice*>(newDevice);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif