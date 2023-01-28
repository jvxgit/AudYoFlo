#include "jvxCustomTechnologies/CjvxCustomDevice.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxCustomDevice::CjvxCustomDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_CUSTOM_DEVICE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	this->_common_set_custom_device.dummy = 0;
}

CjvxCustomDevice::~CjvxCustomDevice()
{
	this->terminate();
}

jvxErrorType
CjvxCustomDevice::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _activate();
	if(res == JVX_NO_ERROR)
	{
		init_all();
		allocate_all();
		register_all(static_cast<CjvxProperties*>(this));
	}
	return(res);
}

jvxErrorType
CjvxCustomDevice::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _deactivate();
	if(res == JVX_NO_ERROR)
	{

		unregister_all(static_cast<CjvxProperties*>(this));
		deallocate_all();
	}
	return(res);
}

jvxErrorType
CjvxCustomDevice::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

	if(this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// Read all parameters from this class
		CjvxCustomDevice::put_configuration__Control(callConf, processor, sectionToContainAllSubsectionsForMe,  &warnings);
		if(!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for(i = 0; i < warnings.size(); i++)
			{
				txt = "Component <";
				txt += _common_set_min.theDescription;
				txt += ">: " + warnings[i];
				_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}


			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	return(res);
}

jvxErrorType
CjvxCustomDevice::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{

	// Write all parameters from this class
	if (_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		CjvxCustomDevice::get_configuration__Control(callConf, processor, sectionWhereToAddAllSubsections);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxCustomDevice::prepare()
{
	jvxErrorType res = _prepare();
	if(res == JVX_NO_ERROR)
	{
		CjvxProperties::_update_properties_on_start();
	}
	return(res);
}

jvxErrorType
CjvxCustomDevice::postprocess()
{
	jvxErrorType res = _postprocess();
	if(res == JVX_NO_ERROR)
	{		
		CjvxProperties::_update_properties_on_stop();
	}
	return(res);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif