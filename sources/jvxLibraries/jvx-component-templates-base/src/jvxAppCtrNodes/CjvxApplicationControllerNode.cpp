#include "jvx.h"

#include "jvxAppCtrNodes/CjvxAppCtrNode.h"

CjvxApplicationControllerNode::CjvxApplicationControllerNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_APPLICATION_CONTROLLER_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theComponentSubTypeDescriptor = "application_controller_node/app_ctrl_node";
}

CjvxApplicationControllerTechnology::~CjvxApplicationControllerTechnology()
{
	terminate();
}

jvxErrorType
CjvxApplicationControllerTechnology::activate()
{
	jvxErrorType res = _activate();
	if (res == JVX_NO_ERROR)
	{
#ifdef USE_STANDALONE_SKELETON
		// Do whatever is required
		CjvxApplicationControllerDevice* newDevice = new CjvxApplicationControllerDevice("Single Device", false, _common_set.theDescriptor.c_str(), _common_set.theFeatureClass, _common_set.theModuleName.c_str(),
			JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE, "device/application_controller_device");

		// Whatever to be done for initialization
		oneDeviceWrapper elm;
		elm.hdlDev = static_cast<IjvxDevice*>(newDevice);

		_common_tech_set.lstDevices.push_back(elm);
#endif
	}
	return(res);
}

jvxErrorType
CjvxApplicationControllerTechnology::deactivate()
{
#ifdef USE_STANDALONE_SKELETON
	jvxSize i;
#endif

	jvxErrorType res = _deactivate();
	if (res == JVX_NO_ERROR)
	{
#ifdef USE_STANDALONE_SKELETON
		for (i = 0; i < _common_tech_set.lstDevices.size(); i++)
		{
			delete(_common_tech_set.lstDevices[i].hdlDev);
		}
		_common_tech_set.lstDevices.clear();
#endif
	}
	return(res);
}

