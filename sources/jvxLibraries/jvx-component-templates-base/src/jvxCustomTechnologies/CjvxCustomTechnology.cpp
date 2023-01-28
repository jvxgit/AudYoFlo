#include "jvx.h"

#include "jvxCustomTechnologies/CjvxCustomTechnology.h"
#include "jvxCustomTechnologies/CjvxCustomDevice.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxCustomTechnology::CjvxCustomTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_CUSTOM_TECHNOLOGY);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

}

CjvxCustomTechnology::~CjvxCustomTechnology()
{
	terminate();
}

jvxErrorType
CjvxCustomTechnology::activate()
{
	jvxErrorType res = _activate();
	if(res == JVX_NO_ERROR)
	{
#ifdef USE_STANDALONE_SKELETON
		// Do whatever is required
		CjvxCustomDevice* newDevice = new CjvxCustomDevice("Single Device", false, (_common_set.theDescriptor + "#" + jvx_size2String(0)).c_str(), _common_set.theFeatureClass, 
			_common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_CUSTOM_DEVICE, "device/custom_device");

		// Whatever to be done for initialization
		oneDeviceWrapper elm;
		elm.hdlDev= static_cast<IjvxDevice*>(newDevice);

		_common_tech_set.lstDevices.push_back(elm);
#endif
	}
	return(res);
}

jvxErrorType
CjvxCustomTechnology::deactivate()
{
#ifdef USE_STANDALONE_SKELETON
	jvxSize i;
#endif
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
#ifdef USE_STANDALONE_SKELETON
		for(i = 0; i < _common_tech_set.lstDevices.size(); i++)
		{
			delete(_common_tech_set.lstDevices[i].hdlDev);
		}
		_common_tech_set.lstDevices.clear();
#endif
	}
	return(res);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif