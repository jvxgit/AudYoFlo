#include "CjvxHostJvx.h"

jvxErrorType
JVX_HOST_JVX_CLASSNAME::number_lines(jvxSize* num_lines)  
	{
		if (_common_set_min.theState >= JVX_STATE_SELECTED)
		{
			return(_number_lines(num_lines));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

jvxErrorType
JVX_HOST_JVX_CLASSNAME::master_line(jvxSize idxLine, jvxComponentIdentification* tp_master) 
	{
		if (_common_set_min.theState >= JVX_STATE_SELECTED)
		{
			return(_master_line(idxLine, tp_master));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

jvxErrorType
JVX_HOST_JVX_CLASSNAME::add_line(const jvxComponentIdentification& tp_master, const char* description) 
	{
		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{
			return(_add_line(tp_master, description));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

	// ======================================================================================

jvxErrorType
JVX_HOST_JVX_CLASSNAME::add_slave_line(const jvxComponentIdentification& tp_master, const jvxComponentIdentification& tp_slave) 
	{
		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{
			return(_add_slave_line(tp_master, tp_slave));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

jvxErrorType
JVX_HOST_JVX_CLASSNAME::remove_slave_line(const jvxComponentIdentification& tp_master, const jvxComponentIdentification& tp_slave) 
	{
		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{
			return(_remove_slave_line(tp_master, tp_slave));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

jvxErrorType
JVX_HOST_JVX_CLASSNAME::number_slaves_line(const jvxComponentIdentification& tp_master, jvxSize* num_lines) 
	{
		if (_common_set_min.theState >= JVX_STATE_SELECTED)
		{
			return(_number_slaves_line(tp_master, num_lines));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

jvxErrorType
JVX_HOST_JVX_CLASSNAME::slave_line_master(const jvxComponentIdentification& tp_master, jvxSize idxEntry, jvxComponentIdentification* tp_slave) 
	{
		if (_common_set_min.theState >= JVX_STATE_SELECTED)
		{
			return(_slave_line_master(tp_master, idxEntry, tp_slave));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

	// ======================================================================================

jvxErrorType
JVX_HOST_JVX_CLASSNAME::add_configuration_property_line(const jvxComponentIdentification& tp_master, jvxSize system_property_id) 
	{
		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{
			return(_add_configuration_property_line(tp_master, system_property_id));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

jvxErrorType
JVX_HOST_JVX_CLASSNAME::remove_configuration_property_line(const jvxComponentIdentification& tp_master, jvxSize system_property_id) 
	{
		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{
			return(_remove_configuration_property_line(tp_master, system_property_id));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

jvxErrorType
JVX_HOST_JVX_CLASSNAME::number_property_ids_line(const jvxComponentIdentification& tp_master, jvxSize* num_lines) 
	{
		if (_common_set_min.theState >= JVX_STATE_SELECTED)
		{
			return(_number_property_ids_line(tp_master, num_lines));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

jvxErrorType
JVX_HOST_JVX_CLASSNAME::property_id_line_master(const jvxComponentIdentification& tp_master, jvxSize idxEntry, jvxSize* the_prop_id) 
	{
		if (_common_set_min.theState >= JVX_STATE_SELECTED)
		{
			return(_property_id_line_master(tp_master, idxEntry, the_prop_id));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

	// ======================================================================================

jvxErrorType
JVX_HOST_JVX_CLASSNAME::report_property_was_set(const jvxComponentIdentification& tpFrom, jvxPropertyCategoryType cat, jvxSize propIdx,
		jvxBool onlyContent, const jvxComponentIdentification& tpTo) 
	{
		if (_common_set_min.theState > JVX_STATE_SELECTED)
		{
			return(_report_property_was_set(tpFrom, cat, propIdx, static_cast<IjvxHost*>(this), onlyContent, tpTo));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

jvxErrorType
JVX_HOST_JVX_CLASSNAME::refresh_all() 
	{
		if (_common_set_min.theState > JVX_STATE_SELECTED)
		{
			return(_refresh_all(static_cast<IjvxHost*>(this)));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

	// ======================================================================================

jvxErrorType
JVX_HOST_JVX_CLASSNAME::remove_line(const jvxComponentIdentification& tp_master) 
	{
		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{
			return(_remove_line(tp_master));
		}
		return(JVX_ERROR_WRONG_STATE);
	};

jvxErrorType
JVX_HOST_JVX_CLASSNAME::remove_all_lines()  
	{
		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{
			return(_remove_all_lines());
		}
		return(JVX_ERROR_WRONG_STATE);
	};