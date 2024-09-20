#include "common/CjvxTechnology.h"

CjvxTechnology::CjvxTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
}

CjvxTechnology::~CjvxTechnology()
{
}

// jvxErrorType CjvxTechnology::_activate();

jvxErrorType
CjvxTechnology::_number_devices(jvxSize* num)
{
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if (num)
		{
			*num = _common_tech_set.lstDevices.size();
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxTechnology::_name_device(jvxSize idx, jvxApiString* name, jvxApiString* fName)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	std::string nm;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if (idx < _common_tech_set.lstDevices.size())
		{
			auto elm = _common_tech_set.lstDevices.begin();
			std::advance(elm, idx);
			elm->hdlDev->name(name, fName);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return(res);
}

jvxErrorType
CjvxTechnology::_description_device(jvxSize idx, jvxApiString* fldStr)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	std::string nm;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if (idx < _common_tech_set.lstDevices.size())
		{
			auto elm = _common_tech_set.lstDevices.begin();
			std::advance(elm, idx);
			elm->hdlDev->description(fldStr);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return(res);
}

jvxErrorType
CjvxTechnology::_descriptor_device(jvxSize idx, jvxApiString* fldStr, jvxApiString* substr)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if (idx < _common_tech_set.lstDevices.size())
		{
			auto elm = _common_tech_set.lstDevices.begin();
			std::advance(elm, idx);
			elm->hdlDev->descriptor(fldStr, substr);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return(res);
}

jvxErrorType
CjvxTechnology::_module_name_device(jvxSize idx, jvxApiString* fldStr)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if (idx < _common_tech_set.lstDevices.size())
		{
			auto elm = _common_tech_set.lstDevices.begin();
			std::advance(elm, idx);
			elm->hdlDev->module_reference(fldStr, NULL);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return(res);
}

jvxErrorType
CjvxTechnology::_request_device(jvxSize idx, IjvxDevice** hdl_onreturn)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if (idx < _common_tech_set.lstDevices.size())
		{
			if (hdl_onreturn)
			{
				auto elm = _common_tech_set.lstDevices.begin();
				std::advance(elm, idx);
				elm->refCnt++;
				*hdl_onreturn = elm->hdlDev;
				res = JVX_NO_ERROR;
			}
			//*hdl_onreturn = _common_tech_set.lstDevices[idx].hdl;
			return res;
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxTechnology::_ident_device(jvxSize* idx, IjvxDevice* dev)
{
	jvxSize id = JVX_SIZE_UNSELECTED;

	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		jvxSize cnt = 0;
		auto elm = _common_tech_set.lstDevices.begin();
		for (; elm != _common_tech_set.lstDevices.end(); elm++, cnt++)
		{
			if (elm->hdlDev == dev)
			{
				id = cnt;
				res = JVX_NO_ERROR;
				break;
			}
		}
	}

	if (idx)
	{
		*idx = id;
	}
	return res;
};

jvxErrorType
CjvxTechnology::_return_device(IjvxDevice* dev)
{
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		auto elm = _common_tech_set.lstDevices.begin();
		for (; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			if (dev == elm->hdlDev)
			{
				break;
			}

		}

		if (elm != _common_tech_set.lstDevices.end())
		{
			elm->refCnt--;

			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxTechnology::_deactivate_cbxt()
{
	jvxErrorType res = _deactivate();
	if (res == JVX_NO_ERROR)
	{
		// Do whatever is required
		_common_tech_set.lstDevices.clear();
	}
	return(res);
}

jvxErrorType
CjvxTechnology::_capabilities_device(jvxSize idx, jvxDeviceCapabilities& caps)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if (idx < _common_tech_set.lstDevices.size())
		{
			auto elm = _common_tech_set.lstDevices.begin();
			std::advance(elm, idx);
			elm->hdlDev->capabilities_device(caps);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return(res);
}

jvxErrorType
CjvxTechnology::_location_info_device(jvxSize idx, jvxComponentIdentification& tpId)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if (idx < _common_tech_set.lstDevices.size())
		{
			auto elm = _common_tech_set.lstDevices.begin();
			std::advance(elm, idx);
			elm->hdlDev->location_info(tpId);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return(res);
}

jvxErrorType
CjvxTechnology::_status_device(jvxSize idx, jvxState* stat)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if (idx < _common_tech_set.lstDevices.size())
		{
			auto elm = _common_tech_set.lstDevices.begin();
			std::advance(elm, idx);
			elm->hdlDev->state(stat);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return(res);
}
jvxErrorType
CjvxTechnology::on_device_caps_changed(IjvxDevice* dev)
{
	IjvxReport* rep = reqInterface<IjvxReport>(_common_set_min.theHostRef);
	if (rep)
	{
		
		CjvxReportCommandRequest post(
			jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT,
			_common_set.theComponentType);
		rep->request_command(post);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxTechnology::config_device_set(jvxSize subslotid, const char* txt)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	if (txt != nullptr)
	{
		res = JVX_NO_ERROR;
		auto elm = previousConfigs.find(subslotid);
		if (elm != previousConfigs.end())
		{
			elm->second = txt;
		}
		else
		{
			previousConfigs[subslotid] = txt;
		}
	}
	else
	{
		auto elm = previousConfigs.find(subslotid);
		if (elm != previousConfigs.end())
		{
			previousConfigs.erase(elm);
			res = JVX_NO_ERROR;
		}
	}
	return res;
}

jvxErrorType 
CjvxTechnology::config_device_get(jvxSize subslotid, jvxApiString* astr)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (astr != nullptr)
	{
		auto elm = previousConfigs.find(subslotid);
		if (elm != previousConfigs.end())
		{
			astr->assign(elm->second);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	
	return res;
}
