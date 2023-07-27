#include "jvx.h"
#include "jvx-helpers.h"

#include "jvxHosts/CjvxHost.h"

// ================================================================
// Control sub components
// ================================================================

jvxErrorType
CjvxHost::system_ready()
{
	// Indicate that boot process is complete
	bootComplete = true;
	auto elmTT = _common_set_types.registeredTechnologyTypes.begin();
	for (; elmTT != _common_set_types.registeredTechnologyTypes.end(); elmTT++)
	{
		auto elmT = elmTT->technologyInstances.selTech.begin();
		for (; elmT != elmTT->technologyInstances.selTech.end(); elmT++)
		{
			if (elmT->theHandle_shortcut_tech)
			{
				elmT->theHandle_shortcut_tech->system_ready();

				auto elmD = elmT->theHandle_shortcut_dev.begin();
				for (; elmD != elmT->theHandle_shortcut_dev.end(); elmD++)
				{
					if (elmD->dev)
					{
						elmD->dev->system_ready();
					}
				}
			}
		}
	}
	auto elmN = _common_set_types.registeredNodeTypes.begin();
	for (; elmN != _common_set_types.registeredNodeTypes.end(); elmN++)
	{
		auto elmNN = elmN->instances.theHandle_shortcut.begin();
		for (; elmNN != elmN->instances.theHandle_shortcut.end(); elmNN++)
		{
			if (elmNN->obj)
			{
				elmNN->obj->system_ready();
			}
		}
	}
	auto elmS = _common_set_types.registeredSimpleTypes.begin();
	for (; elmS != _common_set_types.registeredSimpleTypes.end(); elmS++)
	{
		auto elmSN = elmS->instances.theHandle_shortcut.begin();
		for (; elmSN != elmS->instances.theHandle_shortcut.end(); elmSN++)
		{
			if (elmSN->obj)
			{
				elmSN->obj->system_ready();
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHost::system_about_to_shutdown()
{
	auto elmTT = _common_set_types.registeredTechnologyTypes.begin();
	for (; elmTT != _common_set_types.registeredTechnologyTypes.end(); elmTT++)
	{
		auto elmT = elmTT->technologyInstances.selTech.begin();
		for (; elmT != elmTT->technologyInstances.selTech.end(); elmT++)
		{
			if (elmT->theHandle_shortcut_tech)
			{
				elmT->theHandle_shortcut_tech->system_about_to_shutdown();

				auto elmD = elmT->theHandle_shortcut_dev.begin();
				for (; elmD != elmT->theHandle_shortcut_dev.end(); elmD++)
				{
					if (elmD->dev)
					{
						elmD->dev->system_about_to_shutdown();
					}
				}
			}
		}
	}
	auto elmN = _common_set_types.registeredNodeTypes.begin();
	for (; elmN != _common_set_types.registeredNodeTypes.end(); elmN++)
	{
		auto elmNN = elmN->instances.theHandle_shortcut.begin();
		for (; elmNN != elmN->instances.theHandle_shortcut.end(); elmNN++)
		{
			if (elmNN->obj)
			{
				elmNN->obj->system_about_to_shutdown();
			}
		}
	}
	auto elmS = _common_set_types.registeredSimpleTypes.begin();
	for (; elmS != _common_set_types.registeredSimpleTypes.end(); elmS++)
	{
		auto elmSN = elmS->instances.theHandle_shortcut.begin();
		for (; elmSN != elmS->instances.theHandle_shortcut.end(); elmSN++)
		{
			if (elmSN->obj)
			{
				elmSN->obj->system_about_to_shutdown();
			}
		}
	}

	bootComplete = false;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHost::owner(IjvxObject** ownerOnReturn)
{
	return CjvxObject::_owner(ownerOnReturn);
}

jvxErrorType
CjvxHost::boot_complete(jvxBool* bootCompleteReturn)
{
	if (bootCompleteReturn)
	{
		*bootCompleteReturn = bootComplete;
	}
	return JVX_NO_ERROR;
}

// =============================================================================
// =============================================================================

jvxErrorType
CjvxHost::prerun_stateswitch(jvxStateSwitch ss, jvxComponentIdentification tp)
{
	for (auto& elm : _common_set_host.registeredStateSwitchHandlers)
	{
		elm.second.ptrReport->pre_hook_stateswitch(ss, tp);
	}
	/*
	if (_common_set_host.reportOnStateSwitch)
	{
		_common_set_host.reportOnStateSwitch->pre_hook_stateswitch(JVX_STATE_SWITCH_ACTIVATE, tp);
	}
	*/
	return JVX_NO_ERROR;
}

// ======================================================================================
jvxErrorType
CjvxHost::postrun_stateswitch(jvxStateSwitch ss, jvxComponentIdentification tp, jvxErrorType res)
{
	for (auto& elm : _common_set_host.registeredStateSwitchHandlers)
	{
		elm.second.ptrReport->post_hook_stateswitch(ss, tp, res);
	}

	/*
	if (_common_set_host.reportOnStateSwitch)
	{
		_common_set_host.reportOnStateSwitch->post_hook_stateswitch(JVX_STATE_SWITCH_ACTIVATE, tp, res);
	}
	*/
	return JVX_NO_ERROR;
}

jvxState
CjvxHost::myState()
{
	return _common_set_min.theState;
}

void
CjvxHost::reportErrorDescription(const std::string& descr, jvxBool isWarning)
{
	if (isWarning)
	{
		_report_text_message(descr.c_str(), JVX_REPORT_PRIORITY_WARNING);
	}
	else
	{
		_common_set.theErrordescr = descr;
	}
}

jvxSize
CjvxHost::myRegisteredHostId()
{
	return this->_common_set.register_host_id;
}
