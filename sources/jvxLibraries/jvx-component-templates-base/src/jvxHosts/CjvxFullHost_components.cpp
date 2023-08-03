#include "jvx.h"
#include "jvx-helpers.h"

#include "jvxHosts/CjvxFullHost.h"

// ================================================================
// Control sub components
// ================================================================

jvxErrorType
CjvxFullHost::system_ready()
{
	// Indicate that boot process is complete
	hostReady = true;

	// Forward to class CjvxHostInteraction
	CjvxHostInteraction::_report_boot_complete(hostReady);

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
CjvxFullHost::system_about_to_shutdown()
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

	hostReady = false;

	// Forward to class CjvxHostInteraction
	CjvxHostInteraction::_report_boot_complete(hostReady);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFullHost::owner(IjvxObject** ownerOnReturn)
{
	return CjvxObject::_owner(ownerOnReturn);
}

// =============================================================================
// =============================================================================

jvxErrorType
CjvxFullHost::prerun_stateswitch(jvxStateSwitch ss, jvxComponentIdentification tp)
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
CjvxFullHost::postrun_stateswitch(jvxStateSwitch ss, jvxComponentIdentification tp, jvxErrorType res)
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
CjvxFullHost::myState()
{
	return _common_set_min.theState;
}

void
CjvxFullHost::reportErrorDescription(const std::string& descr, jvxBool isWarning)
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
CjvxFullHost::myRegisteredHostId()
{
	return this->_common_set.register_host_id;
}
