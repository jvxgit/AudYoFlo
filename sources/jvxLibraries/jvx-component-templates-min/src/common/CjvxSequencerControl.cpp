#include "common/CjvxSequencerControl.h"

CjvxSequencerControl::CjvxSequencerControl()
{
}

CjvxSequencerControl::~CjvxSequencerControl()
{
}

jvxErrorType
CjvxSequencerControl::_number_conditions(jvxSize* num)
{
	if (num)
	{
		*num = _common_set_sequencerControl.registeredConditions.size();
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxSequencerControl::_description_condition(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxCBitField* stateValid)
{
	if (idx < _common_set_sequencerControl.registeredConditions.size())
	{
		auto elm = _common_set_sequencerControl.registeredConditions.begin();
		std::advance(elm, idx);
		if (uniqueId)
		{
			*uniqueId = elm->second.uniqueId;
		}

		if (description)
		{
			description->assign(elm->second.description);
		}
		if (stateValid)
		{
			*stateValid = elm->second.stateValid;
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
}

jvxErrorType
CjvxSequencerControl::_status_condition(jvxSize uniqueId, jvxBool* fulfilled, jvxSize* oper_state_on_return, IjvxObject* theObj)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;

	jvxCallManager callGateTmp;
	auto elm = _common_set_sequencerControl.registeredConditions.find(uniqueId);
	if (elm == _common_set_sequencerControl.registeredConditions.end())
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	else
	{
		theObj->state(&stat);
		if (stat & elm->second.stateValid)
		{
			if (elm->second.theStaticFunc)
			{
				res = elm->second.theStaticFunc(uniqueId, fulfilled, oper_state_on_return, elm->second.priv);
			}
			else
			{
				if (fulfilled)
				{
					*fulfilled = *(elm->second.fulfilled);
				}
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	return(res);
}

jvxErrorType
CjvxSequencerControl::_number_commands(jvxSize* num)
{
	if (num)
	{
		*num = _common_set_sequencerControl.registeredCommands.size();
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxSequencerControl::_description_command(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxCBitField* stateValid)
{
	if (idx < _common_set_sequencerControl.registeredCommands.size())
	{
		auto elm = _common_set_sequencerControl.registeredCommands.begin();
		std::advance(elm, idx);
		if (uniqueId)
		{
			*uniqueId = elm->second.uniqueId;
		}

		if (description)
		{
			description->assign(elm->second.description);
		}

		if (stateValid)
			*stateValid = elm->second.stateValid;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
}

jvxErrorType
CjvxSequencerControl::_trigger_command(jvxSize uniqueId, jvxSize* oper_state_on_return, IjvxObject* theObj)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;

	auto elm = _common_set_sequencerControl.registeredCommands.find(uniqueId);
	if (elm == _common_set_sequencerControl.registeredCommands.end())
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	if (res == JVX_NO_ERROR)
	{
		theObj->state(&stat);
		if (stat & elm->second.stateValid)
		{
			if (elm->second.theStaticFunc)
			{
				res = elm->second.theStaticFunc(uniqueId, oper_state_on_return, elm->second.priv);
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	return(res);
}

jvxErrorType
CjvxSequencerControl::_number_relative_jumps(jvxSize* num)
{
	if (num)
	{
		*num = _common_set_sequencerControl.registeredJumps.size();
	}
	return(JVX_NO_ERROR);
};

jvxErrorType
CjvxSequencerControl::_description_relative_jump(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxCBitField* stateValid)
{
	if (idx < _common_set_sequencerControl.registeredJumps.size())
	{
		auto elm = _common_set_sequencerControl.registeredJumps.begin();
		std::advance(elm, idx);
		if (uniqueId)
		{
			*uniqueId =elm->second.uniqueId;
		}

		if (description)
		{
			description->assign(elm->second.description);
		}

		if (stateValid)
		{
			*stateValid =elm->second.stateValid;
		}

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
}

jvxErrorType
CjvxSequencerControl::_trigger_relative_jump(jvxSize uniqueId, jvxSize* pc_increment, jvxSize* oper_state_on_return, IjvxObject* theObj)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;

	auto elm = _common_set_sequencerControl.registeredJumps.find(uniqueId);
	if (elm == _common_set_sequencerControl.registeredJumps.end())
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	if (res == JVX_NO_ERROR)
	{
		theObj->state( &stat);
		if (stat & elm->second.stateValid)
		{
			if (elm->second.theStaticFunc)
			{
				res = elm->second.theStaticFunc(uniqueId, pc_increment, oper_state_on_return, elm->second.priv);
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	return(res);
}

// =============================================================================

jvxErrorType
CjvxSequencerControl::addCondition(oneCondition& elmCo)
{
	if (JVX_CHECK_SIZE_SELECTED(elmCo.uniqueId))
	{
		auto elm = _common_set_sequencerControl.registeredConditions.find(elmCo.uniqueId);
		if (elm == _common_set_sequencerControl.registeredConditions.end())
		{
			_common_set_sequencerControl.registeredConditions[elmCo.uniqueId] = elmCo;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxSequencerControl::remCondition(jvxSize uId)
{
	auto elm = _common_set_sequencerControl.registeredConditions.find(uId);
	if (elm == _common_set_sequencerControl.registeredConditions.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	_common_set_sequencerControl.registeredConditions.erase(elm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSequencerControl::remAllConditions()
{
	_common_set_sequencerControl.registeredConditions.clear();
	return JVX_NO_ERROR;
}
// ===========================================================================================

jvxErrorType
CjvxSequencerControl::addCommand(oneCommand& elmC)
{
	if (JVX_CHECK_SIZE_SELECTED(elmC.uniqueId))
	{
		auto elm = _common_set_sequencerControl.registeredCommands.find(elmC.uniqueId);
		if (elm == _common_set_sequencerControl.registeredCommands.end())
		{
			_common_set_sequencerControl.registeredCommands[elmC.uniqueId] = elmC;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxSequencerControl::remCommand(jvxSize uId)
{
	auto elm = _common_set_sequencerControl.registeredCommands.find(uId);
	if (elm == _common_set_sequencerControl.registeredCommands.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	_common_set_sequencerControl.registeredCommands.erase(elm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSequencerControl::remAllCommands()
{
	_common_set_sequencerControl.registeredCommands.clear();
	return JVX_NO_ERROR;
}

// ========================================================================================

jvxErrorType
CjvxSequencerControl::addRelativeJump(oneRelativeJump& elmJ)
{
	if (JVX_CHECK_SIZE_SELECTED(elmJ.uniqueId))
	{
		auto elm = _common_set_sequencerControl.registeredJumps.find(elmJ.uniqueId);
		if (elm == _common_set_sequencerControl.registeredJumps.end())
		{
			_common_set_sequencerControl.registeredJumps[elmJ.uniqueId] = elmJ;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxSequencerControl::remRelativeJump(jvxSize uId)
{
	auto elm = _common_set_sequencerControl.registeredJumps.find(uId);
	if (elm == _common_set_sequencerControl.registeredJumps.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	_common_set_sequencerControl.registeredJumps.erase(elm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSequencerControl::remAllRelativeJumps()
{
	_common_set_sequencerControl.registeredJumps.clear();
	return JVX_NO_ERROR;
}
