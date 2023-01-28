#include "common/CjvxObjectMin.h"

CjvxObjectMin::CjvxObjectMin(const std::string& theDescription)
{
	_common_set_min.theDescription = theDescription;
	_common_set_min.theState = JVX_STATE_NONE;
}

jvxErrorType
CjvxObjectMin::_check_access(jvxCallManager& callGate)
{
	// Here, we can check for accessibility of PROPERTY
	callGate.access_protocol = JVX_ACCESS_PROTOCOL_OK;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxObjectMin::_state(jvxState* stat)
{
	// For now, ignoring the access flags and the protocol
	if (stat)
	{
		*stat = _common_set_min.theState;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxObjectMin::_initialize(IjvxHiddenInterface* hostRef)
{
	if (_common_set_min.theState == JVX_STATE_NONE)
	{
		_common_set_min.theHostRef = hostRef;
		_common_set_min.theState = JVX_STATE_INIT;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObjectMin::_terminate()
{
	if (_common_set_min.theState == JVX_STATE_INIT)
	{
		_common_set_min.theState = JVX_STATE_NONE;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObjectMin::_select(IjvxObject* newOwner)
{
	if (_common_set_min.theState == JVX_STATE_INIT)
	{
		if (newOwner)
		{
			if (_common_set_min.theOwner)
			{
				std::cout << __FUNCTION__ << ": Object <" << _common_set_min.theDescription << ">: Duplicate owner specification." << std::endl;
				return JVX_ERROR_DUPLICATE_ENTRY;
			}
			_common_set_min.theOwner = newOwner;
		}


		_common_set_min.theState = JVX_STATE_SELECTED;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObjectMin::_unselect()
{
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		_common_set_min.theOwner = nullptr;
		_common_set_min.theState = JVX_STATE_INIT;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObjectMin::_activate()
{
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		_common_set_min.theState = JVX_STATE_ACTIVE;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObjectMin::_deactivate()
{
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		_common_set_min.theState = JVX_STATE_SELECTED;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObjectMin::_owner(IjvxObject** ownerOnReturn)
{
	if (ownerOnReturn)
	{
		*ownerOnReturn = _common_set_min.theOwner;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxObjectMin::_pre_check_activate()
{
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxObjectMin::_pre_check_deactivate()
{
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxObjectMin::_pre_check_unselect()
{
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxObjectMin::report_properties_modified(const char* props_set)
{
	std::cout << __FUNCTION__ << ": Modified properties were reported. This report will not be forwarded!" << std::endl;
	return JVX_NO_ERROR;
}