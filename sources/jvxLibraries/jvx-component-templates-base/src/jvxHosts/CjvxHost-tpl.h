/*
 * Options to call this function:
 * 1) Specify a valid slotid: return reference of the associated element
 * 2) Specify slotid JVX_SIZE_UNSELECTED:
 *		Return the single offside object
 * 3) Specify slotid JVX_SIZE_DONTCARE:
 *      Search the currently selected objects of the type to find among those objects that have the right state (filter_stateMask)
 *		- filter_id = 0: Description matches the string in filter_token
 *		- filter_id = 1: Descriptor matches the string in filter_token
 *		- otherwise: Module name matches the string in filter_token
 */

template <class T> jvxErrorType
CjvxHost::t_reference_tool(std::vector<oneObjType<T>>& registeredTypes,
	const jvxComponentIdentification& tp,
	IjvxObject** theObject, jvxSize filter_id,
	const char* filter_token,
	jvxBitField filter_stateMask)
{
	jvxSize i = 0, cnt = 0;
	jvxApiString fldStr;
	std::string txt;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	typename std::vector<oneObjType<T>>::iterator elmIt_ob;
	elmIt_ob = jvx_findItemSelectorInList_one<oneObjType<T>, jvxComponentType>(registeredTypes, tp.tp, 0);

	if (elmIt_ob != registeredTypes.end())
	{
		res = JVX_NO_ERROR;

		if (tp.slotid == JVX_SIZE_UNSELECTED)
		{
			if (filter_stateMask == JVX_STATE_DONTCARE)
			{
				if (filter_token)
				{
					for (i = 0; i < elmIt_ob->instances.availableEndpoints.size(); i++)
					{
						txt = "Unknown";
						elmIt_ob->instances.availableEndpoints[i].theHandle_single->descriptor(&fldStr);

						txt = fldStr.std_str();

						if (jvx_compareStringsWildcard(filter_token, txt))
						{
							filter_id = i;
							break;
						}
					}
				}

				if (filter_id < elmIt_ob->instances.availableEndpoints.size())
				{
					if (theObject)
					{
						*theObject = elmIt_ob->instances.availableEndpoints[filter_id].theHandle_single;
					}
					res = JVX_NO_ERROR;
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				cnt = 0;
				for (i = 0; i < elmIt_ob->instances.availableEndpoints.size(); i++)
				{
					if (elmIt_ob->instances.availableEndpoints[i].theHandle_single)
					{
						jvxState stat = JVX_STATE_NONE;
						elmIt_ob->instances.availableEndpoints[i].theHandle_single->state(&stat);
						if (JVX_EVALUATE_BITFIELD(stat & filter_stateMask))
						{
							if (filter_token)
							{
								txt = "Unknown";
								elmIt_ob->instances.availableEndpoints[i].theHandle_single->descriptor(&fldStr);

								txt = fldStr.std_str();

								if (jvx_compareStringsWildcard(filter_token, txt))
								{
									filter_id = i;
									break;
								}
							}
							else
							{
								if (filter_id == cnt)
								{
									if (theObject)
									{
										*theObject = elmIt_ob->instances.availableEndpoints[i].theHandle_single;
									}
									res = JVX_NO_ERROR;
									break;
								}
								else
								{
									cnt++;
								}
							}
						}
					}
				}
			}
		}
		else if (tp.slotid == JVX_SIZE_DONTCARE)
		{
			// Here, we find a link to an object located in the slot hierarchie that has a specific state.
			// Since the object must be in the slot hierarchie, the minimum state is hence "selected"
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
			cnt = JVX_SIZE_UNSELECTED;
			for (i = 0; i < elmIt_ob->instances.theHandle_shortcut.size(); i++)
			{
				if (elmIt_ob->instances.theHandle_shortcut[i].obj)
				{
					jvxState stat = JVX_STATE_NONE;
					elmIt_ob->instances.theHandle_shortcut[i].obj->state(&stat);
					if (JVX_EVALUATE_BITFIELD(stat & filter_stateMask))
					{
						if (filter_token)
						{
							txt = "Unknown";
							switch (filter_id)
							{
							case 0:
								elmIt_ob->instances.theHandle_shortcut[i].obj->description(&fldStr);
								txt = fldStr.std_str();
								break;
							case 1:
								elmIt_ob->instances.theHandle_shortcut[i].obj->descriptor(&fldStr);
								txt = fldStr.std_str();
								break;
							default:
								elmIt_ob->instances.theHandle_shortcut[i].obj->module_reference(&fldStr, nullptr);
								txt = fldStr.std_str();
								break;
							}

							if (jvx_compareStringsWildcard(filter_token, txt))
							{
								cnt = i;
								break;
							}
						}
					}
				}
			}

			if (JVX_CHECK_SIZE_SELECTED(cnt))
			{
				if (theObject)
				{
					*theObject = elmIt_ob->instances.theHandle_shortcut[cnt].obj;
					// tp.slotid = cnt; <- this is not allowed since it is a const reference. However, we need to pass this as a const reference since often there is no real reference in use in the call position - which leads to an error in GLNX.
				}
				res = JVX_NO_ERROR;				
			}
		}
		else
		{
			if (tp.slotid < elmIt_ob->instances.theHandle_shortcut.size())
			{
				if (theObject)
				{
					*theObject = elmIt_ob->instances.theHandle_shortcut[tp.slotid].obj;
				}
				res = JVX_NO_ERROR;
			}
		}
	}
	return res;
}

template <class T> jvxErrorType
CjvxHost::t_return_reference_tool(std::vector<oneObjType<T>>& registeredTypes,
	jvxComponentType tp,
	IjvxObject* theObject)
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	typename std::vector<oneObjType<T>>::iterator elmIt_ob;
	elmIt_ob = jvx_findItemSelectorInList_one<oneObjType<T>, jvxComponentType>(registeredTypes, tp, 0);

	if (elmIt_ob != registeredTypes.end())
	{
		for (i = 0; i < elmIt_ob->instances.availableEndpoints.size(); i++)
		{
			if (theObject == elmIt_ob->instances.availableEndpoints[i].theHandle_single)
			{
				res = JVX_NO_ERROR;
				break;
			}
		}
		if(res != JVX_NO_ERROR)
		{
			for (i = 0; i < elmIt_ob->instances.theHandle_shortcut.size(); i++)
			{
				if (theObject == elmIt_ob->instances.theHandle_shortcut[i].obj)
				{
					res = JVX_NO_ERROR;
					break;
				}
			}
		}
	}
	return res;
}