#include "CjvxHostJvx.h"
#include "CjvxHostJvx_config.h"
#include "jvx-helpers.h"

CjvxHostJvx::CjvxHostJvx(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxHost*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
	config.use_only_static_objects = true;
#endif
}

jvxErrorType
CjvxHostJvx::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	_common_set_min.theHostRef = static_cast<IjvxHiddenInterface*>(this);
	CjvxHost::activate();
	if(res == JVX_NO_ERROR)
	{
		_init_sequences();

		// Report state switch of host - of course, it is already late..
		postrun_stateswitch(JVX_STATE_SWITCH_SELECT, _common_set.theComponentType, res);
		postrun_stateswitch(JVX_STATE_SWITCH_ACTIVATE, _common_set.theComponentType, res);
	}
	return(res);
}

jvxErrorType
CjvxHostJvx::deactivate()
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// Report state switch of host - of course, it is early..
		prerun_stateswitch(JVX_STATE_SWITCH_DEACTIVATE, _common_set.theComponentType);
		prerun_stateswitch(JVX_STATE_SWITCH_UNSELECT, _common_set.theComponentType);

		_terminate_sequences();
		res = CjvxHost::deactivate();
		_common_set_min.theHostRef = NULL;
	}
	return(res);
}

// ================================================================
// Interface IjvxHidenInterface
// ================================================================

jvxErrorType
CjvxHostJvx::object_hidden_interface(IjvxObject** objRef)
{
	if (objRef)
	{
		*objRef = static_cast<IjvxObject*>(this);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJvx::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	switch(tp)
	{
	case JVX_INTERFACE_CONFIGURATION:
		if(hdl)
		{
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this));
		}
		return(JVX_NO_ERROR);
	case JVX_INTERFACE_CONFIGURATION_DONE:
		if (hdl)
		{
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationDone*>(this));
		}
		return(JVX_NO_ERROR);
	case JVX_INTERFACE_CONFIGURATIONLINE:
		if(hdl)
		{
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationLine*>(this));
		}
		return(JVX_NO_ERROR);

	case JVX_INTERFACE_PROPERTY_POOL:
		if (hdl)
		{
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxPropertyPool*>(this));
		}
		return(JVX_NO_ERROR);

	case JVX_INTERFACE_CONFIGURATION_EXTENDER:
		if (hdl)
		{
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationExtender*>(this));
		}
		return(JVX_NO_ERROR);

	case JVX_INTERFACE_CONFIGURATION_ATTACH:
		if (hdl)
		{
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationAttach*>(this));
		}
		return(JVX_NO_ERROR); 
	}
	return(CjvxHost::request_hidden_interface(tp, hdl));
}

jvxErrorType
CjvxHostJvx::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	switch(tp)
	{
	case JVX_INTERFACE_CONFIGURATION:
		if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this)))
		{
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	case JVX_INTERFACE_CONFIGURATION_DONE:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationDone*>(this)))
		{
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	case JVX_INTERFACE_CONFIGURATIONLINE:
		if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationLine*>(this)))
		{
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);

	case JVX_INTERFACE_PROPERTY_POOL:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxPropertyPool*>(this)))
		{
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);

	case JVX_INTERFACE_CONFIGURATION_EXTENDER:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationExtender*>(this)))
		{
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	case JVX_INTERFACE_CONFIGURATION_ATTACH:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationAttach*>(this)))
		{
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	}
	return(CjvxHost::return_hidden_interface(tp, hdl));
}

jvxErrorType
CjvxHostJvx::activate_selected_component(const jvxComponentIdentification& tp)
{
	jvxErrorType res = CjvxHost::activate_selected_component(tp);
	if(res == JVX_NO_ERROR)
	{
		// Whenever a component becomes active, run the configuration lines
		this->refresh_all();
	}
	return(res);
}

jvxErrorType
CjvxHostJvx::attach_configuration_submodule(const char* prefix, IjvxConfiguration* cfg)
{
	auto elm = std::find_if(registeredConfigSubmodules.begin(), registeredConfigSubmodules.end(), [&](const CjvxConfigurationSubModule& elm) 
		{ return elm.prefix == prefix; });
	if (elm == registeredConfigSubmodules.end())
	{
		CjvxConfigurationSubModule newElm;
		newElm.prefix = prefix;
		newElm.cfgRef = cfg;
		registeredConfigSubmodules.push_back(newElm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxHostJvx::JVX_CALLINGCONVENTION detach_configuration_submodule(IjvxConfiguration* cfg)
{
	auto elm = std::find_if(registeredConfigSubmodules.begin(), registeredConfigSubmodules.end(), [&](const CjvxConfigurationSubModule& elm)
		{ return elm.cfgRef == cfg; });
	if (elm != registeredConfigSubmodules.end())
	{
		registeredConfigSubmodules.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

