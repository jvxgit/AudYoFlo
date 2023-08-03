#include "jvxHosts/CjvxComponentHostTools.h"
#include "jvxHosts/CjvxHost-tpl.h"

CjvxComponentHostTools::CjvxComponentHostTools()
{
}
	
CjvxComponentHostTools::~CjvxComponentHostTools()
{
}

jvxErrorType 
CjvxComponentHostTools::_number_tools(const jvxComponentIdentification& tp, jvxSize* num)
{
	jvxErrorType res = JVX_NO_ERROR;
	int numRet = 0;
	jvxSize cnt = 0;
	jvxSize i;

	res = CjvxComponentHost::_number_components_system(tp, num);

	// If the first part returned an error, the type may reference another object type
	if (res != JVX_NO_ERROR)
	{
		res = CjvxHostInteractionTools<CjvxComponentHost>::_number_tools(
			tp, num);
		/*
		for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp.tp)
			{
				cnt++;
			}
		}

		if (num)
		{
			*num = cnt;
		}
		res = JVX_NO_ERROR;
		*/
	}
	return(res);
}
	
jvxErrorType 
CjvxComponentHostTools::_identification_tool(const jvxComponentIdentification& tp,
	jvxSize idx, jvxApiString* description,
	jvxApiString* descriptor, jvxBool* multipleInstances)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	int numRet = 0;
	jvxSize i;
	bool isStandardC = false;
	jvxSize idSelect = JVX_SIZE_UNSELECTED;

	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		if (
			(_common_set_types.registeredTechnologyTypes[i].selector[0] == tp.tp) ||
			(_common_set_types.registeredTechnologyTypes[i].selector[1] == tp.tp))

		{
			if (idx < this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size())
			{
				res = JVX_NO_ERROR;
				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[idx].theHandle_single->description(description);
				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[idx].theHandle_single->descriptor(descriptor);

				if (multipleInstances)
				{
					*multipleInstances = _common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[idx].common.allowsMultiObjects;
				}
			}
			return res;
		}
	}


	for (i = 0; i < _common_set_types.registeredNodeTypes.size(); i++)
	{
		if (_common_set_types.registeredNodeTypes[i].selector[0] == tp.tp)
		{
			if (idx < this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints.size())
			{
				res = JVX_NO_ERROR;
				_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[idx].theHandle_single->description(description);
				_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[idx].theHandle_single->descriptor(descriptor);

				if (multipleInstances)
				{
					*multipleInstances = _common_set_types.registeredNodeTypes[i].instances.availableEndpoints[idx].common.allowsMultiObjects;
				}
			}
			return res;
		}
	}

	return CjvxHostInteractionTools<CjvxComponentHost>::_identification_tool(
		tp, idx, description,descriptor, multipleInstances);

	/*
	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp.tp)
		{
			if (numRet == idx)
			{
				res = JVX_NO_ERROR;
				this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->description(description);
				this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->descriptor(descriptor);
				break;
			}
			numRet++;
		}
	}
	return(res);
	*/
}
	
jvxErrorType 
CjvxComponentHostTools::_reference_tool(const jvxComponentIdentification& tp, 
	IjvxObject** theObject, jvxSize filter_id,
	const char* filter_descriptor, 
	jvxBitField filter_stateMask,
	IjvxReferenceSelector* decider)
{
	jvxSize i = 0;
	jvxSize cnt = 0;
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	jvxApiString fldStr;
	jvxState stat = JVX_STATE_NONE;
	jvxSize idxSelect = JVX_SIZE_UNSELECTED;
	jvxBool foundit = false;
	jvxBool isDevice = false;
	std::string txt;

	// If there is a filter, deactivate id return
	if (filter_descriptor)
	{
		filter_id = JVX_SIZE_UNSELECTED;
	}

	// =====================================================================================
	// Find in technologies/devices
	// =====================================================================================

	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		if (_common_set_types.registeredTechnologyTypes[i].selector[0] == tp.tp)
		{
			idxSelect = (jvxInt32)i;
			break;
		}

		if (_common_set_types.registeredTechnologyTypes[i].selector[1] == tp.tp)
		{
			idxSelect = (jvxInt32)i;
			isDevice = true;
			break;
		}
	}

	if (JVX_CHECK_SIZE_SELECTED(idxSelect))
	{
		if (isDevice)
		{
			// There must be an active technology...
			if (tp.slotid < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech.size())
			{
				if (_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					jvxSize numDevices = 0;
					_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->number_devices(&numDevices);

					if (filter_stateMask == JVX_STATE_DONTCARE)
					{
						if (filter_descriptor)
						{
							for (i = 0; i < numDevices; i++)
							{
								txt = "Unknown";
								_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->descriptor_device(i, &fldStr);

								txt = fldStr.std_str();

								if (jvx_compareStringsWildcard(filter_descriptor, txt))
								{
									filter_id = i;
									break;
								}
							}
						}

						if (filter_id < numDevices)
						{
							if (theObject)
							{
								IjvxDevice* theDevice = NULL;
								_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->request_device(filter_id, &theDevice);
								if (theDevice)
								{
									*theObject = static_cast<IjvxObject*>(theDevice);
									res = JVX_NO_ERROR;
								}
								else
								{
									res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
								}
							}
							else
							{
								res = JVX_NO_ERROR;
							}
						}
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;
						}
					}
					else
					{
						cnt = 0;
						res = JVX_ERROR_ELEMENT_NOT_FOUND;

						for (i = 0; i < numDevices; i++)
						{
							if (theObject)
							{
								IjvxDevice* theDevice = NULL;
								_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->request_device(i, &theDevice);
								if (theDevice)
								{
									jvxState stat = JVX_STATE_NONE;
									theDevice->state(&stat);
									if (JVX_EVALUATE_BITFIELD(stat & filter_stateMask))
									{
										if (filter_descriptor)
										{
											txt = "Unknown";
											theDevice->descriptor(&fldStr);
											txt = fldStr.std_str();

											if (jvx_compareStringsWildcard(filter_descriptor, txt))
											{
												*theObject = static_cast<IjvxObject*>(theDevice);
												res = JVX_NO_ERROR;
												break;
											}
										}
										else
										{
											if (cnt == filter_id)
											{
												*theObject = static_cast<IjvxObject*>(theDevice);
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
								else
								{
									res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
									break;
								}
							}
							else
							{
								res = JVX_NO_ERROR;
							}
						}
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		} // if (isDevice)
		else
		{
			if (tp.slotid == JVX_SIZE_UNSELECTED)
			{
				if (filter_stateMask == JVX_STATE_DONTCARE)
				{
					if (filter_descriptor)
					{
						for (i = 0; i < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies.size(); i++)
						{
							txt = "Unknown";
							_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single->descriptor(&fldStr);

							txt = fldStr.std_str();

							if (jvx_compareStringsWildcard(filter_descriptor, txt))
							{
								filter_id = i;
								break;
							}
						}
					}


					if (filter_id < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies.size())
					{
						if (theObject)
						{
							*theObject = _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[filter_id].theHandle_single;
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
					for (i = 0; i < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies.size(); i++)
					{
						assert(_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single);
						stat = JVX_STATE_NONE;
						_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single->state(&stat);
						if (JVX_EVALUATE_BITFIELD(stat & filter_stateMask))
						{
							if (filter_descriptor)
							{
								txt = "Unknown";
								_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single->descriptor(&fldStr);
								txt = fldStr.std_str();

								if (jvx_compareStringsWildcard(filter_descriptor, txt))
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
										*theObject = _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single;
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
			else
			{
				if (tp.slotid < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech.size())
				{
					if (theObject)
					{
						*theObject = _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech;
					}
					res = JVX_NO_ERROR;
				}
			}
		}
		return(res);
	}

	// =====================================================================================
	// Find in nodes
	// =====================================================================================

	// New template based implementation
	res = t_reference_tool<IjvxNode>(_common_set_types.registeredNodeTypes,
		tp, theObject, filter_id, filter_descriptor, filter_stateMask, decider);

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		res = t_reference_tool<IjvxSimpleComponent>(_common_set_types.registeredSimpleTypes,
			tp, theObject, filter_id, filter_descriptor, filter_stateMask, decider);
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		// =====================================================================================
		// Find in all other components
		// =====================================================================================

		res = CjvxHostInteractionTools<CjvxComponentHost>::_reference_tool(
			tp, theObject, filter_id, filter_descriptor, filter_stateMask,
			decider);

		/*
		cnt = 0;
		for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp.tp)
			{
				if (filter_descriptor)
				{
					txt = "Unknown";
					this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->descriptor(&fldStr);

					txt = fldStr.std_str();

					if (jvx_compareStringsWildcard(filter_descriptor, txt))
					{
						if (theObject)
						{
							*theObject = this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single;
							this->_common_set_host.otherComponents.availableOtherComponents[i].refCount++;
						}
						res = JVX_NO_ERROR;
						break;
					}
				}
				else
				{
					if (cnt == filter_id)
					{
						if (theObject)
						{
							*theObject = this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single;
							this->_common_set_host.otherComponents.availableOtherComponents[i].refCount++;
						}
						res = JVX_NO_ERROR;
						break;
					}
					cnt++;
				}
			}
		}
			*/
	}
	return(res);
}
	
jvxErrorType 
CjvxComponentHostTools::_return_reference_tool(
	const jvxComponentIdentification& tp,
	IjvxObject* theObject)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize i;
	int numRet = 0;
	jvxApiString fldStr;

	jvxInt32 idxSelect = -1;
	jvxBool isDevice = false;

	if (theObject == NULL)
	{
		return(JVX_NO_ERROR);
	}

	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		if (_common_set_types.registeredTechnologyTypes[i].selector[0] == tp.tp)
		{
			idxSelect = (jvxInt32)i;
			break;
		}
		if (_common_set_types.registeredTechnologyTypes[i].selector[1] == tp.tp)

		{
			idxSelect = (jvxInt32)i;
			isDevice = true;
			break;
		}
	}

	if (idxSelect >= 0)
	{
		if (isDevice)
		{
			if (tp.slotid < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech.size())
			{
				// There must be an active technology...
				if (_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					jvxSize numDevices = 0;
					_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->number_devices(&numDevices);

					res = JVX_ERROR_INVALID_ARGUMENT;
					for (i = 0; i < numDevices; i++)
					{
						IjvxDevice* theDevice = NULL;
						_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->request_device(i, &theDevice);
						if (theObject == static_cast<IjvxObject*>(theDevice))
						{
							res = JVX_NO_ERROR;
							break;
						}
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			if (tp.slotid == JVX_SIZE_UNSELECTED)
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
				for (i = 0; i < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies.size(); i++)
				{
					if (theObject == _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single)
					{
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
			else
			{
				if (tp.slotid < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech.size())
				{
					if (theObject == _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
					{
						res = JVX_NO_ERROR;
					}
				}
			}
		}
		return(res);
	}

	// For the nodes we only need the type since the object already indicates which instance to release
	res = t_return_reference_tool<IjvxNode>(_common_set_types.registeredNodeTypes,
		tp.tp, theObject);
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		res = t_return_reference_tool<IjvxSimpleComponent>(_common_set_types.registeredSimpleTypes,
			tp.tp, theObject);
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		res = CjvxHostInteractionTools<CjvxComponentHost>::_return_reference_tool(
			tp, theObject);
		
		/*
		for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp.tp)
			{
				if (theObject == this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single)
				{
					this->_common_set_host.otherComponents.availableOtherComponents[i].refCount--;
					res = JVX_NO_ERROR;
					break;
				}
			}
		}
		*/
	}
	return(res);
}
jvxErrorType 
CjvxComponentHostTools::_instance_tool(jvxComponentType tp, IjvxObject** theObject, 
	jvxSize filter_id, const char* filter_descriptor)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	int numRet = 0;
	jvxApiString fldStr;
	std::string cmpStr;
	jvxSize i, j;
	std::string txt;

	// If there is a filter, deactivate id return
	if (filter_descriptor)
	{
		filter_id = JVX_SIZE_UNSELECTED;
	}

	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		if (_common_set_types.registeredTechnologyTypes[i].selector[0] == tp)
		{
			// Check if we allow return of a technology instance
			if (JVX_CHECK_SIZE_UNSELECTED(filter_id))
			{
				if (filter_descriptor)
				{
					for (j = 0; j < this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size(); i++)
					{
						this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].theHandle_single->descriptor(&fldStr);
						cmpStr = fldStr.std_str();
						if (cmpStr == (std::string)filter_descriptor)
						{
							filter_id = j;
							break;
						}
					}
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(filter_id))
			{
				if (filter_id < this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size())
				{
					if (this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[filter_id].common.allowsMultiObjects)
					{
						res = this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[filter_id].common.linkage.funcInit(
							theObject, NULL, this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].theHandle_single);
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_SETTING;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			return res;
		}
		else if (_common_set_types.registeredTechnologyTypes[i].selector[1] == tp)
		{
			res = JVX_ERROR_UNSUPPORTED;
			return res;
		}
	}

	for (i = 0; i < _common_set_types.registeredNodeTypes.size(); i++)
	{
		if (_common_set_types.registeredNodeTypes[i].selector[0] == tp)
		{
			// Check if we allow return of a node instance
			if (JVX_CHECK_SIZE_UNSELECTED(filter_id))
			{
				if (filter_descriptor)
				{
					for (j = 0; j < this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints.size(); j++)
					{
						this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[j].theHandle_single->descriptor(&fldStr);
						cmpStr = fldStr.std_str();
						if (cmpStr == (std::string)filter_descriptor)
						{
							filter_id = j;
						}
					}
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(filter_id))
			{
				if (filter_id < this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints.size())
				{
					if (this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[filter_id].common.allowsMultiObjects)
					{
						res = this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[filter_id].common.linkage.funcInit(
							theObject, NULL, this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[filter_id].theHandle_single);
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_SETTING;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			return res;
		}
	}

	return CjvxHostInteractionTools<CjvxComponentHost>::_instance_tool(
		tp, theObject, filter_id, filter_descriptor);
	/*
	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp)
		{
			if (filter_descriptor)
			{
				txt = "Unknown";
				if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.allowsMultiObjects)
				{
					if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit && this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm)
					{
						IjvxObject* locObject = NULL;
						res = this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit(&locObject,
							NULL, this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single);
						if (res == JVX_NO_ERROR)
						{
							locObject->descriptor(&fldStr);
							txt = fldStr.std_str();

							if (txt == filter_descriptor)
							{
								if (theObject)
								{
									*theObject = locObject;
									this->_common_set_host.otherComponents.availableOtherComponents[i].leased.push_back(*theObject);
								}
								res = JVX_NO_ERROR;
								break;
							}
							else
							{
								res = this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm(locObject);
								res = JVX_ERROR_ID_OUT_OF_BOUNDS;// reset to original error case
							}
						}
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;// reset to original error case
							if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.isExternalComponent)
							{
								std::cout << "Warning: Init function failed for object " << this->_common_set_host.otherComponents.availableOtherComponents[i].common.externalLink.description << "." << std::endl;

							}
							else
							{
								std::cout << "Warning: Init function failed for object from dynamic library " << this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.dllPath << "." << std::endl;
							}
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;// reset to original error case
						if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.isExternalComponent)
						{
							std::cout << "Warning: Init function failed for object " << this->_common_set_host.otherComponents.availableOtherComponents[i].common.externalLink.description << "." << std::endl;

						}
						else
						{
							std::cout << "Warning: Init function failed for object  from dynamic library " << this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.dllPath << "." << std::endl;
						}
					}
				}
				else
				{
					// Object does not allow multiple allocation, this will not be correct
				}
			}
			else
			{
				if (numRet == filter_id)
				{
					if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.allowsMultiObjects)
					{
						if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit && this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm)
						{
							if (theObject)
							{
								res = this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit(theObject,
									NULL, this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single);
								if (res == JVX_NO_ERROR)
								{
									this->_common_set_host.otherComponents.availableOtherComponents[i].leased.push_back(*theObject);
								}
								else
								{
									res = JVX_ERROR_INTERNAL;
								}
							}
							else
							{
								res = JVX_ERROR_INVALID_ARGUMENT;
							}
						}
						else
						{
							res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
						}
					}
					else
					{
						res = JVX_ERROR_UNSUPPORTED;
					}
					break;
				}
				numRet++;
			}
		}
	}
	return(res);
	*/
}
	
jvxErrorType 
CjvxComponentHostTools::_return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	int numRet = 0;
	jvxApiString fldStr;
	std::string cmpStr;
	jvxSize i, j;
	bool isStandardC = false;

	// If there is a filter, deactivate id return
	if (filter_descriptor)
	{
		filter_id = JVX_SIZE_UNSELECTED;
	}

	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		if (_common_set_types.registeredTechnologyTypes[i].selector[0] == tp)
		{
			if (_common_set_types.registeredTechnologyTypes[i].selector[0] == tp)
			{
				// Check if we allow return of a technology instance
				if (JVX_CHECK_SIZE_UNSELECTED(filter_id))
				{
					if (filter_descriptor)
					{
						for (j = 0; j < this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size(); i++)
						{
							this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].theHandle_single->descriptor(&fldStr);
							cmpStr = fldStr.std_str();
							if (cmpStr == (std::string)filter_descriptor)
							{
								filter_id = j;
								break;
							}
						}
					}
				}
				if (JVX_CHECK_SIZE_SELECTED(filter_id))
				{
					if (filter_id < this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size())
					{
						if (this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[filter_id].common.allowsMultiObjects)
						{
							res = this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[filter_id].common.linkage.funcTerm(theObject);
							res = JVX_NO_ERROR;
						}
						else
						{
							res = JVX_ERROR_INVALID_SETTING;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
				return res;
			}
			else if (_common_set_types.registeredTechnologyTypes[i].selector[1] == tp)
			{
				res = JVX_ERROR_UNSUPPORTED;
				return res;
			}
		}
	}


	for (i = 0; i < _common_set_types.registeredNodeTypes.size(); i++)
	{
		if (_common_set_types.registeredNodeTypes[i].selector[0] == tp)
		{
			// Check if we allow return of a node instance
			if (JVX_CHECK_SIZE_UNSELECTED(filter_id))
			{
				if (filter_descriptor)
				{
					for (j = 0; j < this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints.size(); j++)
					{
						this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[j].theHandle_single->descriptor(&fldStr);
						cmpStr = fldStr.std_str();

						if (cmpStr == (std::string)filter_descriptor)
						{
							filter_id = j;
							break;
						}
					}
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(filter_id))
			{
				if (filter_id < this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints.size())
				{
					if (this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[filter_id].common.allowsMultiObjects)
					{
						res = this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[filter_id].common.linkage.funcTerm(theObject);
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_SETTING;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			return res;
		}
	}

	return CjvxHostInteractionTools<CjvxComponentHost>::_return_instance_tool(
		tp, theObject, filter_id, filter_descriptor);

	/*
	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp)
		{
			if (numRet == filter_id)
			{
				if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.allowsMultiObjects)
				{
					std::vector<IjvxObject*>::iterator elm = this->_common_set_host.otherComponents.availableOtherComponents[i].leased.begin();
					for (; elm != this->_common_set_host.otherComponents.availableOtherComponents[i].leased.end(); elm++)
					{
						if (*elm == theObject)
						{
							break;
						}
					}
					if (elm != this->_common_set_host.otherComponents.availableOtherComponents[i].leased.end())
					{
						assert(this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm);

						res = this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm(theObject);
						if (res != JVX_NO_ERROR)
						{
							res = JVX_ERROR_INTERNAL;
						}
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
				else
				{
					res = JVX_ERROR_UNSUPPORTED;
				}
				break;
			}
			numRet++;
		}
	}
	return(res);
	*/
}

// ============================================================================================================

jvxErrorType 
CjvxComponentHostTools::map_reference_tool(const jvxComponentIdentification& tp, IjvxObject** theObject, jvxSize filter_id,
	const char* filter_descriptor, jvxBitField filter_stateMask ,
	IjvxReferenceSelector* decider )
{
	return _reference_tool(tp, theObject, filter_id,
		filter_descriptor, filter_stateMask,
		decider);
}

jvxErrorType 
CjvxComponentHostTools::map_return_reference_tool(const jvxComponentIdentification& tp,
	IjvxObject* theObject)
{
	return _return_reference_tool(tp, theObject);
}

