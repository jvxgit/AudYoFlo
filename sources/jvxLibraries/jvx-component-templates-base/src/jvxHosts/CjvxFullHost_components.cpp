#include "jvx.h"
#include "jvx-helpers.h"

#include "jvxHosts/CjvxFullHost.h"
#include "CjvxHost_components-tpl.h"

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

jvxErrorType 
CjvxFullHost::attach_external_component(IjvxObject* toBeAttached, const char* moduleGroup, jvxBool regConnFactory, jvxBool noCfgSave, jvxSize desiredSlotId, jvxSize desiredSlotSubId)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* refSpec = nullptr;
	jvxSize h;
	jvxComponentIdentification tp;
	jvxApiString modStr;

	toBeAttached->request_specialization(&refSpec, &tp, nullptr);

	toBeAttached->module_reference(&modStr, nullptr);
	IjvxNode* node = castFromObject<IjvxNode>(toBeAttached);
	IjvxNode* tech = castFromObject<IjvxTechnology>(toBeAttached);
	if (tech)
	{
		auto elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			assert(0); // To be done :-)
		}
	}
	if (node)
	{
		jvxBool extend_if_necessary = true;
		tp.slotsubid = 0;
		tp.slotid = desiredSlotId;
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);
		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			oneExternalObj<IjvxNode> newElm;
			newElm.theHandle_single = node;
			newElm.moduleName = modStr.std_str();			
			res = JVX_ERROR_DUPLICATE_ENTRY;
			
			// Attach a number to the object if the name was already in use
			std::string originElmName = newElm.moduleName;
			jvxSize cnt = 0;
			while (1)
			{
				jvxBool alreadyThere = false;
				if (cnt > 0)
				{
					newElm.moduleName = originElmName + "_" + jvx_size2String(cnt);
				}
				for (auto& elm : elmIt_ep->instances.externalEndpoints)
				{
					if (elm.moduleName == newElm.moduleName)
					{
						alreadyThere = true;
						break;
					}
				}
				if (alreadyThere)
				{
					cnt++;
				}
				else
				{
					break;
				}
			}

			elmIt_ep->instances.externalEndpoints.push_back(newElm);

			// Now, add the external component to the grid
			res = JVX_NO_ERROR;
			auto oldList = elmIt_ep->instances.theHandle_shortcut;

			if (JVX_CHECK_SIZE_SELECTED(tp.slotid))
			{
				if (tp.slotid >= elmIt_ep->instances.theHandle_shortcut.size())
				{
					// Ok, we may use the id as it is
				}
				else
				{
					if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj == nullptr)
					{
						// Ok, we may use the id as it is
					}
					else
					{
						// The requested id is already in use..
						// Let the system select an alternative id
						std::cout << __FUNCTION__ << ": Warning: Desired slotid <" << tp.slotid << "> is already in use. Switching to next available slotid as given by the system." << std::endl;
						tp.slotid = JVX_SIZE_DONTCARE;
					}
				}
			}
			else
			{
				if (tp.slotid == JVX_SIZE_DONTCARE)
				{
					// This case is ok, system will select the id
				}
				else
				{
					// This is a regular case. However, we will not add the component
					return JVX_NO_ERROR;
				}
			}

			// =========================================================================
			// Lookout the next available slot
			if (tp.slotid == JVX_SIZE_DONTCARE)
			{
				for (h = 0; h < elmIt_ep->instances.theHandle_shortcut.size(); h++)
				{
					if (elmIt_ep->instances.theHandle_shortcut[h].obj == NULL)
					{
						tp.slotid = h;
						break;
					}
				}
			}

			// If we still have not found an empty container, try to allocate new instance
			if (tp.slotid == JVX_SIZE_DONTCARE)
			{
				tp.slotid = elmIt_ep->instances.theHandle_shortcut.size();
			}
			// =======================================================================


			if (tp.slotid >= elmIt_ep->instances.theHandle_shortcut.size())
			{
				if (
					(tp.slotid < elmIt_ep->instances.numSlotsMax) ||
					extend_if_necessary)
				{
					// If we do not have enough slots, expand!
					for (h = elmIt_ep->instances.theHandle_shortcut.size(); h <= tp.slotid; h++)
					{
						oneSelectedObj<IjvxNode> oneElm;
						oneElm.obj = NULL;
						oneElm.cfac = NULL;
						oneElm.mfac = NULL;
						oneElm.uid = JVX_SIZE_UNSELECTED;
						oneElm.idSel = JVX_SIZE_UNSELECTED;

						// oneElm.theHandle_shortcut_dev.clear();
						elmIt_ep->instances.theHandle_shortcut.push_back(oneElm);
					}
				}
				else
				{
					res = JVX_ERROR_BUFFER_OVERFLOW;
				}
			}

			if (res == JVX_NO_ERROR)
			{
				if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj == NULL)
				{
					jvxState stat = JVX_STATE_NONE;
					jvxErrorType resL = JVX_NO_ERROR;
					jvxSize uid = 0;

					elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj = node;
					elmIt_ep->instances.theHandle_shortcut[tp.slotid].nmExternal = modStr.std_str();
					elmIt_ep->instances.theHandle_shortcut[tp.slotid].noCfgSave = noCfgSave;
					std::string nm = jvxComponentIdentification_txt(tp);
					if (uIdInst)
						uIdInst->obtain_unique_id(&uid, nm.c_str());
					tp.uId = uid;

					// ===============================================================
					// Select or indicate select to host
					// ===============================================================
					elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->state(&stat);
					prerun_stateswitch(JVX_STATE_SWITCH_SELECT, tp);
					if (stat == JVX_STATE_NONE)
					{
						resL = elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->select();
					}
					elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->set_location_info(tp);

					if (resL == JVX_NO_ERROR)
					{
						elmIt_ep->instances.theHandle_shortcut[tp.slotid].uid = uid;
						elmIt_ep->instances.theHandle_shortcut[tp.slotid].idSel = JVX_SIZE_SLOT_OFF_SYSTEM;
					}
					else
					{
						res = JVX_ERROR_INTERNAL;
					}
					postrun_stateswitch(JVX_STATE_SWITCH_SELECT, tp, resL);

					if (res == JVX_NO_ERROR)
					{
						// ===============================================================
						// Activate or indicate activate to host
						// ===============================================================
						elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->state(&stat);
						prerun_stateswitch(JVX_STATE_SWITCH_ACTIVATE, tp);
						if (stat == JVX_STATE_SELECTED)
						{
							resL = elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->activate();
						}
						postrun_stateswitch(JVX_STATE_SWITCH_ACTIVATE, tp, resL);
						if (resL != JVX_NO_ERROR)
						{
							jvxApiError theErr;
							jvxApiString theName;
							elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->description(&theName);
							elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->lasterror(&theErr);

							reportErrorDescription((std::string)"Failed to activate node type <" + elmIt_ep->description + ">, node <" +
								theName.std_str() + ">, reason: " + theErr.errorDescription.std_str());
							res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
						}
						else
						{
							if (regConnFactory)
							{
								elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->request_hidden_interface(
									JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle**>(&elmIt_ep->instances.theHandle_shortcut[tp.slotid].cfac));
								elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->request_hidden_interface(
									JVX_INTERFACE_CONNECTOR_MASTER_FACTORY, reinterpret_cast<jvxHandle**>(&elmIt_ep->instances.theHandle_shortcut[tp.slotid].mfac));

								resL = this->connection_factory_to_be_added(
									tp,
									elmIt_ep->instances.theHandle_shortcut[tp.slotid].cfac,
									elmIt_ep->instances.theHandle_shortcut[tp.slotid].mfac);
								if (resL != JVX_NO_ERROR)
								{
									reportErrorDescription("Failed to add connector and/or connector master factory for node", true);
								}
							}
						}
					}
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
		}
	}

	if (res == JVX_NO_ERROR)
	{
		if (moduleGroup)
		{
			std::string modName = moduleGroup;
			auto elm = extModuleDefinitions.find(modName);
			if (elm != extModuleDefinitions.end())
			{
				elm->second.associatedExternalComponents.push_back(toBeAttached);
			}
			else
			{
				CjvxHostTypeHandler::oneDynExtModule newElm;
				newElm.associatedExternalComponents.push_back(toBeAttached);
				newElm.moduleName = modName;
				extModuleDefinitions[modName] = newElm;
			}
		}
	}
	return res;
}

jvxErrorType 
CjvxFullHost::detach_external_component(IjvxObject* toBeDetached, const char* moduleGroup, jvxState statOnLeave)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* refSpec = nullptr;
	jvxComponentIdentification tp;
	jvxComponentIdentification tpReset;
	jvxApiString modStr;
	toBeDetached->request_specialization(&refSpec, &tp, nullptr);
	toBeDetached->module_reference(&modStr, nullptr);
	IjvxNode* node = castFromObject<IjvxNode>(toBeDetached);
	IjvxNode* tech = castFromObject<IjvxTechnology>(toBeDetached);
	if (tech)
	{
		auto elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			assert(0); // To be done :-)
		}
	}
	if (node)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);
		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
			auto elm = elmIt_ep->instances.externalEndpoints.begin();
			for (; elm != elmIt_ep->instances.externalEndpoints.end(); elm++)
			{
				if (elm->theHandle_single == node)
				{
					if (JVX_CHECK_SIZE_SELECTED(tp.slotid))
					{
						if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
						{
							if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj == node)
							{
								// Right module found!!
								jvxState stat = JVX_STATE_NONE;
								jvxErrorType resL = JVX_NO_ERROR;

								tpReset = tp;
								jvx::align::resetComponentIdOnUnset(tpReset);

								elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->state(&stat);
								if (stat == JVX_STATE_ACTIVE)
								{
									res = this->connection_factory_to_be_removed(
										tp,
										elmIt_ep->instances.theHandle_shortcut[tp.slotid].cfac,
										elmIt_ep->instances.theHandle_shortcut[tp.slotid].mfac);

									if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].cfac)
									{
										elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->return_hidden_interface(
											JVX_INTERFACE_CONNECTOR_FACTORY,
											reinterpret_cast<jvxHandle*>(elmIt_ep->instances.theHandle_shortcut[tp.slotid].cfac));
										elmIt_ep->instances.theHandle_shortcut[tp.slotid].cfac = NULL;
									}

									if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].mfac)
									{
										elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->return_hidden_interface(
											JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
											reinterpret_cast<jvxHandle*>(elmIt_ep->instances.theHandle_shortcut[tp.slotid].mfac));
										elmIt_ep->instances.theHandle_shortcut[tp.slotid].mfac = NULL;
									}

									prerun_stateswitch(JVX_STATE_SWITCH_DEACTIVATE, tp);
									if (statOnLeave < JVX_STATE_ACTIVE)
									{
										resL = elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->deactivate();
									}
									postrun_stateswitch(JVX_STATE_SWITCH_DEACTIVATE, tp, resL);

									// ==================================================================
									resL = JVX_NO_ERROR;
									prerun_stateswitch(JVX_STATE_SWITCH_UNSELECT, tp);
									if (statOnLeave < JVX_STATE_SELECTED)
									{
										resL = elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->unselect();
										elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->set_location_info(tpReset);
									}
									postrun_stateswitch(JVX_STATE_SWITCH_UNSELECT, tp, resL);

									// Release the unique id
									uIdInst->release_unique_id(elmIt_ep->instances.theHandle_shortcut[tp.slotid].uid);
									tp.uId = JVX_SIZE_UNSELECTED;
									elmIt_ep->instances.theHandle_shortcut[tp.slotid].uid = JVX_SIZE_UNSELECTED;
									elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj = nullptr;

									shrinkGridList<IjvxNode>(elmIt_ep->instances.theHandle_shortcut);
								} // if (stat == JVX_STATE_ACTIVE)
								else
								{
									res = JVX_ERROR_WRONG_STATE;
								}
							} // if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj == node)
							else
							{
								res = JVX_ERROR_INVALID_ARGUMENT;
							}
						} // if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;
						}
					} // if (JVX_CHECK_SIZE_SELECTED(tp.slotid))
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}

					if (res == JVX_NO_ERROR)
					{
						elmIt_ep->instances.externalEndpoints.erase(elm);
					}
					break;
				}
			}
		}
	}

	if (res == JVX_NO_ERROR)
	{
		if (moduleGroup)
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
			std::string modName = moduleGroup;
			auto elm = extModuleDefinitions.find(modName);
			if (elm != extModuleDefinitions.end())
			{
				auto elmI = elm->second.associatedExternalComponents.begin();
				
				for (; elmI != elm->second.associatedExternalComponents.end(); elmI++)
				{
					if (*elmI == toBeDetached)
					{
						elm->second.associatedExternalComponents.erase(elmI);
						res = JVX_NO_ERROR;
						break;
					}
				}

				if (elm->second.associatedExternalComponents.size() == 0)
				{
					extModuleDefinitions.erase(elm);
				}
			}
		}
	}

	return res;
}

jvxErrorType 
CjvxFullHost::load_config_content_module(const char* modName, jvxConfigData** cfgData)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto inst = extModulesConfigs.find(modName);
	if (inst != extModulesConfigs.end())
	{
		if (!inst->second.cfgString.empty())
		{
			jvxComponentIdentification tpCfg(JVX_COMPONENT_CONFIG_PROCESSOR, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
			auto cfg = reqRefTool<IjvxConfigProcessor>(this, tpCfg);
			if (cfg.cpPtr)
			{

				res = cfg.cpPtr->parseTextField(inst->second.cfgString.c_str(), "internal", 0);
				if (res == JVX_NO_ERROR)
				{
					cfg.cpPtr->getConfigurationHandle(cfgData);
				}
			}
			retRefTool<IjvxConfigProcessor>(this, cfg, tpCfg.tp);
		}
		res = JVX_NO_ERROR;
	}
	return res;
}

jvxErrorType 
CjvxFullHost::release_config_content_module(const char* modName, jvxConfigData* cfgData)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto inst = extModulesConfigs.find(modName);
	if (inst != extModulesConfigs.end())
	{
		if (cfgData)
		{
			jvxComponentIdentification tpCfg(JVX_COMPONENT_CONFIG_PROCESSOR, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
			auto cfg = reqRefTool<IjvxConfigProcessor>(this, tpCfg);
			if (cfg.cpPtr)
			{
				cfg.cpPtr->removeHandle(cfgData);
			}
			res = JVX_NO_ERROR;
		}
	}
	return res;
}