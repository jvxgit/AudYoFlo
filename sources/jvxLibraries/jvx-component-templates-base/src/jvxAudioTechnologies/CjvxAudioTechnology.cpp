#include "jvx.h"

#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "jvxAudioTechnologies/CjvxAudioDevice.h"

CjvxAudioTechnology::CjvxAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	/*CjvxTechnologyDevCaps*/ CjvxTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_TECHNOLOGY);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);

}

CjvxAudioTechnology::~CjvxAudioTechnology()
{
	terminate();
}

jvxErrorType
CjvxAudioTechnology::activate()
{
	jvxErrorType res = _activate();
		//CjvxTechnologyDevCaps::activate(static_cast<CjvxProperties*>(this));
	if(res == JVX_NO_ERROR)
	{

#ifdef USE_STANDALONE_SKELETON
		// Do whatever is required
		CjvxAudioDevice* newDevice = new CjvxAudioDevice("Single Device", false, _common_set.theDescriptor.c_str(), _common_set.theFeatureClass, 
			_common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "");

		// Whatever to be done for initialization
		oneDeviceWrapper elm;
		elm.hdlDev= static_cast<IjvxDevice*>(newDevice);

		_common_tech_set.lstDevices.push_back(elm);
#endif
	}
	return(res);
}

jvxErrorType
CjvxAudioTechnology::deactivate()
{
#ifdef USE_STANDALONE_SKELETON
	jvxSize i;
#endif
	jvxErrorType res = _pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
#ifdef USE_STANDALONE_SKELETON
		for(i = 0; i < _common_tech_set.lstDevices.size(); i++)
		{
			delete(_common_tech_set.lstDevices[i].hdlDev);
		}
		_common_tech_set.lstDevices.clear();
#endif
		res = CjvxTechnology::_deactivate();
			//CjvxTechnologyDevCaps::deactivate(static_cast<CjvxProperties*>(this));
	}
	return(res);
}


