template <typename T> jvxErrorType
CjvxComponentHost::t_select_component(std::vector<oneObjType<T>>& registeredTypes,
	jvxComponentIdentification& tp, jvxSize idx,
	IjvxObject* theOwner, jvxBool extend_if_necessary)
{
	jvxSize h;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	typename std::vector<oneObjType<T>>::iterator elmIt_ob;
	elmIt_ob = jvx_findItemSelectorInList_one<oneObjType<T>, jvxComponentType>(registeredTypes, tp.tp, 0);

	if (elmIt_ob != registeredTypes.end())
	{
		res = JVX_NO_ERROR;
		std::vector<oneSelectedObj<T>> oldList = elmIt_ob->instances.theHandle_shortcut;
		if (tp.slotsubid != 0)
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		else
		{
			// =========================================================================
			// Lookout the next available slot
			if (tp.slotid == JVX_SIZE_DONTCARE)
			{
				for (h = 0; h < elmIt_ob->instances.theHandle_shortcut.size(); h++)
				{
					if (elmIt_ob->instances.theHandle_shortcut[h].obj == NULL)
					{
						tp.slotid = h;
						break;
					}
				}
			}

			// If we still have not found an empty container, try to allocate new instance
			if (tp.slotid == JVX_SIZE_DONTCARE)
			{
				tp.slotid = elmIt_ob->instances.theHandle_shortcut.size();
			}
			// =======================================================================


			if (tp.slotid >= elmIt_ob->instances.theHandle_shortcut.size())
			{
				if (
					(tp.slotid < elmIt_ob->instances.numSlotsMax) ||
					extend_if_necessary)
				{
					// If we do not have enough slots, expand!
					for (h = elmIt_ob->instances.theHandle_shortcut.size(); h <= tp.slotid; h++)
					{
						oneSelectedObj<T> oneElm;
						oneElm.obj = NULL;
						oneElm.cfac = NULL;
						oneElm.mfac = NULL;
						// oneElm.theHandle_shortcut_dev.clear();
						elmIt_ob->instances.theHandle_shortcut.push_back(oneElm);
					}
				}
				else
				{
					res = JVX_ERROR_BUFFER_OVERFLOW;
				}
			}

			if (res == JVX_NO_ERROR)
			{
				if (elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj == NULL)
				{
					if (idx < elmIt_ob->instances.availableEndpoints.size())
					{
						IjvxObject* theHandle = NULL;
						T* theObj = NULL;
						if (elmIt_ob->instances.availableEndpoints[idx].common.allowsMultiObjects)
						{
							if (elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj)
							{
								res = JVX_ERROR_WRONG_STATE_SUBMODULE;
							}
							else
							{
								res = elmIt_ob->instances.availableEndpoints[idx].common.linkage.funcInit(&theHandle, NULL,
									elmIt_ob->instances.availableEndpoints[idx].theHandle_single);
								assert(theHandle);
								theHandle->request_specialization(reinterpret_cast<jvxHandle**>(&theObj), NULL, NULL, NULL);
							}
						}
						else
						{
							theHandle = static_cast<IjvxObject*>(elmIt_ob->instances.availableEndpoints[idx].theHandle_single);
							theObj = elmIt_ob->instances.availableEndpoints[idx].theHandle_single;
						}

						if (res == JVX_NO_ERROR)
						{
							res = theObj->initialize(hIfRef);
							if (res == JVX_NO_ERROR)
							{
								jvxSize uid = 0;
								std::string nm = jvxComponentIdentification_txt(tp);
								uIdInst->obtain_unique_id(&uid, nm.c_str());

								IjvxHiddenInterface* theHinterface = static_cast<IjvxHiddenInterface*>(theObj);
								IjvxCoreStateMachine* theObjectSm = static_cast<IjvxCoreStateMachine*>(theObj);
								IjvxObject* theObject = static_cast<IjvxObject*>(theObj);
								tp.uId = uid;

								prerun_stateswitch(JVX_STATE_SWITCH_SELECT, tp);
								res = static_local_select(theHinterface, theObjectSm, theObject, tp, theOwner);

								if (res == JVX_NO_ERROR)
								{
									elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj = theObj;
									elmIt_ob->instances.theHandle_shortcut[tp.slotid].idSel = idx;
									elmIt_ob->instances.theHandle_shortcut[tp.slotid].uid = uid;
								}
								else
								{
									tp.uId = JVX_SIZE_UNSELECTED;
									theObj->terminate();
									if (elmIt_ob->instances.availableEndpoints[idx].common.allowsMultiObjects)
									{
										elmIt_ob->instances.availableEndpoints[idx].common.linkage.funcTerm(theHandle);
									}
									res = JVX_ERROR_INTERNAL;
								}
								postrun_stateswitch(JVX_STATE_SWITCH_SELECT, tp, res);
							}
							else
							{
								if (elmIt_ob->instances.availableEndpoints[idx].common.allowsMultiObjects)
								{
									elmIt_ob->instances.availableEndpoints[idx].common.linkage.funcTerm(theHandle);
								}								
							}
						} // if (res == JVX_NO_ERROR)
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE;
				}

				if (res != JVX_NO_ERROR)
				{
					elmIt_ob->instances.theHandle_shortcut = oldList;
				}
			} // if (res == JVX_NO_ERROR)
		} // else
	} // if (elmIt_t != registeredTypes.end())
	return res;
}

template <class T> jvxErrorType
CjvxComponentHost::t_activate_component(std::vector<oneObjType<T>>& registeredObjs,
	const jvxComponentIdentification& tp)
{
	jvxApiString theName;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	typename std::vector<oneObjType<T>>::iterator elmIt_ob;
	elmIt_ob = jvx_findItemSelectorInList_one<oneObjType<T>, jvxComponentType>(registeredObjs, tp.tp, 0);

	if (elmIt_ob != registeredObjs.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_ob->instances.theHandle_shortcut.size())
		{
			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj)
				{
					prerun_stateswitch(JVX_STATE_SWITCH_ACTIVATE, tp);				
					res = elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj->activate();
					postrun_stateswitch(JVX_STATE_SWITCH_ACTIVATE, tp, res);					
					if (res != JVX_NO_ERROR)
					{
						jvxApiError theErr;
						elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj->description(&theName);
						elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj->lasterror(&theErr);

						reportErrorDescription((std::string)"Failed to activate node type <" + elmIt_ob->description + ">, node <" +
							theName.std_str() + ">, reason: " + theErr.errorDescription.std_str());
					}
					else
					{
						elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj->request_hidden_interface(
							JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle**>(&elmIt_ob->instances.theHandle_shortcut[tp.slotid].cfac));
						elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj->request_hidden_interface(
							JVX_INTERFACE_CONNECTOR_MASTER_FACTORY, reinterpret_cast<jvxHandle**>(&elmIt_ob->instances.theHandle_shortcut[tp.slotid].mfac));

						res = this->connection_factory_to_be_added(
							tp,
							elmIt_ob->instances.theHandle_shortcut[tp.slotid].cfac,
							elmIt_ob->instances.theHandle_shortcut[tp.slotid].mfac);
						if (res != JVX_NO_ERROR)
						{
							reportErrorDescription("Failed to add connector and/or connector master factory for node", true);
						}
					}
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return res;
}

template <class T> jvxErrorType 
CjvxComponentHost::t_deactivate_component(std::vector<oneObjType<T>>& registeredObjs,
	const jvxComponentIdentification& tp)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxState stat = JVX_STATE_NONE;
	typename std::vector<oneObjType<T>>::iterator elmIt_ob;
	elmIt_ob = jvx_findItemSelectorInList_one<oneObjType<T>, jvxComponentType>(registeredObjs, tp.tp, 0);

	if (elmIt_ob != registeredObjs.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_ob->instances.theHandle_shortcut.size())
		{
			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj)
				{
					// HK:CHECKTHIS
					elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj->state(&stat);
					if (stat == JVX_STATE_ACTIVE)
					{
						res = this->connection_factory_to_be_removed(
							tp,
							elmIt_ob->instances.theHandle_shortcut[tp.slotid].cfac,
							elmIt_ob->instances.theHandle_shortcut[tp.slotid].mfac);

						if (elmIt_ob->instances.theHandle_shortcut[tp.slotid].cfac)
						{
							elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj->return_hidden_interface(
								JVX_INTERFACE_CONNECTOR_FACTORY,
								reinterpret_cast<jvxHandle*>(elmIt_ob->instances.theHandle_shortcut[tp.slotid].cfac));
							elmIt_ob->instances.theHandle_shortcut[tp.slotid].cfac = NULL;
						}

						if (elmIt_ob->instances.theHandle_shortcut[tp.slotid].mfac)
						{
							elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj->return_hidden_interface(
								JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
								reinterpret_cast<jvxHandle*>(elmIt_ob->instances.theHandle_shortcut[tp.slotid].mfac));
							elmIt_ob->instances.theHandle_shortcut[tp.slotid].mfac = NULL;
						}

						prerun_stateswitch(JVX_STATE_SWITCH_DEACTIVATE, tp);						
						res = elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj->deactivate();
						postrun_stateswitch(JVX_STATE_SWITCH_DEACTIVATE, tp, res);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE_SUBMODULE;
					}
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return res;
}

template <class T> jvxErrorType
CjvxComponentHost::t_unselect_component(std::vector<oneObjType<T>>& registeredObjs,
	jvxComponentIdentification& tp)
{
	int h;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	typename std::vector<oneObjType<T>>::iterator elmIt_ob;
	elmIt_ob = jvx_findItemSelectorInList_one<oneObjType<T>, jvxComponentType>(registeredObjs, tp.tp, 0);

	if (elmIt_ob != registeredObjs.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_ob->instances.theHandle_shortcut.size())
		{
			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj != NULL)
				{
					IjvxHiddenInterface* theHinterface = static_cast<IjvxHiddenInterface*>(elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj);
					IjvxCoreStateMachine* theObjectSm = static_cast<IjvxCoreStateMachine*>(elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj);

					prerun_stateswitch(JVX_STATE_SWITCH_UNSELECT, tp);					
					res = static_local_unselect(theHinterface, theObjectSm);
					postrun_stateswitch(JVX_STATE_SWITCH_UNSELECT, tp, res);					

					if (res == JVX_NO_ERROR)
					{
						// Release the unique id
						uIdInst->release_unique_id(elmIt_ob->instances.theHandle_shortcut[tp.slotid].uid);
						tp.uId = JVX_SIZE_UNSELECTED;
						elmIt_ob->instances.theHandle_shortcut[tp.slotid].uid = JVX_SIZE_UNSELECTED;

						res = elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj->terminate();
						if (res == JVX_NO_ERROR)
						{
							if (elmIt_ob->instances.availableEndpoints[elmIt_ob->instances.theHandle_shortcut[tp.slotid].idSel].common.allowsMultiObjects)
							{
								elmIt_ob->instances.availableEndpoints[elmIt_ob->instances.theHandle_shortcut[tp.slotid].idSel].common.linkage.funcTerm(
									elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj);
							}
							elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj = NULL;
						}
					}

					// Try to reduce size of the slot list
					std::vector<oneSelectedObj<T>> newLst;
					int sz = (int)elmIt_ob->instances.theHandle_shortcut.size();
					for (h = sz - 1; h >= 0; h--)
					{
						if (elmIt_ob->instances.theHandle_shortcut[h].obj)
						{
							break;
						}
					}
					int copyTo = h;
					for (h = 0; h <= copyTo; h++)
					{
						newLst.push_back(elmIt_ob->instances.theHandle_shortcut[h]);
					}
					elmIt_ob->instances.theHandle_shortcut = newLst;
					if (res != JVX_NO_ERROR)
					{
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return res;
}
