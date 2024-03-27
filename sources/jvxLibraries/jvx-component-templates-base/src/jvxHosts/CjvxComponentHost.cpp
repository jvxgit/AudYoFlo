#include "jvxHosts/CjvxComponentHost.h"
#include "jvxHosts/CjvxHost_components-tpl.h"

jvxErrorType
CjvxComponentHost::static_local_select(IjvxHiddenInterface* theHinterface,
	IjvxCoreStateMachine* theObjectSm,
	IjvxObject* theObject,
	const jvxComponentIdentification& tpId,
	IjvxObject* theOwner)
{
	jvxErrorType res = JVX_NO_ERROR;
	assert(theObjectSm);
	assert(theHinterface);

	res = theObjectSm->select(theOwner);
	if (res == JVX_NO_ERROR)
	{
		theObject->set_location_info(tpId);
	}

	return res;
}

jvxErrorType
CjvxComponentHost::static_local_unselect(IjvxHiddenInterface* theHinterface,
	IjvxObject* theObject, const jvxComponentIdentification& tpIdOld,
	IjvxCoreStateMachine* theObjectSm)
{
	jvxErrorType res = JVX_NO_ERROR;
	assert(theObjectSm);
	assert(theHinterface);

	jvxComponentIdentification tpId = tpIdOld;
	jvx::align::resetComponentIdOnUnset(tpId);

	res = theObjectSm->unselect();
	res = theObject->set_location_info(tpId);

	return res;
}

// =====================================================================

CjvxComponentHost::CjvxComponentHost() : CjvxHostTypeHandler()
{
}

CjvxComponentHost::~CjvxComponentHost() 
{
}

jvxErrorType 
CjvxComponentHost::_number_components_system(const jvxComponentIdentification& tp, jvxSize* numPtr)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize numL = 0;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		numL = elmIt_tech->technologyInstances.availableTechnologies.size() + elmIt_tech->technologyInstances.externalTechnologies.size();
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->number_devices(&numL);
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			numL = elmIt_ep->instances.availableEndpoints.size() + elmIt_ep->instances.externalEndpoints.size();
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			numL = elmIt_si->instances.availableEndpoints.size();
		}
	}
	if (numPtr)
	{
		*numPtr = numL;
	}
	return(res);
}

/*
 * Return the names of components of a specific type
 */
jvxErrorType
CjvxComponentHost::_name_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (idx < elmIt_tech->technologyInstances.availableTechnologies.size())
		{
			res = elmIt_tech->technologyInstances.availableTechnologies[idx].theHandle_single->name(str);
		}
		else
		{
			jvxSize idxOff = idx - elmIt_tech->technologyInstances.availableTechnologies.size();
			if (idxOff < elmIt_tech->technologyInstances.externalTechnologies.size())
			{
				res = elmIt_tech->technologyInstances.externalTechnologies[idxOff].theHandle_single->name(str);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->name_device(idx, str);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (idx < elmIt_ep->instances.availableEndpoints.size())
			{
				res = elmIt_ep->instances.availableEndpoints[idx].theHandle_single->name(str);
			}
			else
			{
				jvxSize idxOff = idx - elmIt_ep->instances.availableEndpoints.size();
				if (idxOff < elmIt_ep->instances.externalEndpoints.size())
				{
					res = elmIt_ep->instances.externalEndpoints[idxOff].theHandle_single->name(str);
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (idx < elmIt_si->instances.availableEndpoints.size())
			{
				res = elmIt_si->instances.availableEndpoints[idx].theHandle_single->name(str);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	return(res);
}

/*
 * Return the names of components of a specific type
 */
jvxErrorType
CjvxComponentHost::_description_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (idx < elmIt_tech->technologyInstances.availableTechnologies.size())
		{
			res = elmIt_tech->technologyInstances.availableTechnologies[idx].theHandle_single->description(str);
		}
		else
		{
			jvxSize idxOff = idx - elmIt_tech->technologyInstances.availableTechnologies.size();
			if (idxOff < elmIt_tech->technologyInstances.externalTechnologies.size())
			{
				std::string combine;

				jvxApiString astrP;
				res = elmIt_tech->technologyInstances.externalTechnologies[idxOff].theHandle_single->module_reference(&astrP, nullptr);
				combine = (std::string)"E:" + astrP.std_str() + ":";
				astrP.clear();
				res = elmIt_tech->technologyInstances.externalTechnologies[idxOff].theHandle_single->description(&astrP);
				combine += astrP.std_str();
				if (str)
				{
					*str = combine;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->description_device(idx, str);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (idx < elmIt_ep->instances.availableEndpoints.size())
			{
				res = elmIt_ep->instances.availableEndpoints[idx].theHandle_single->description(str);
			}
			else
			{
				jvxSize idxOff = idx - elmIt_ep->instances.availableEndpoints.size();
				if (idxOff < elmIt_ep->instances.externalEndpoints.size())
				{
					std::string combine;

					jvxApiString astrP;
					res = elmIt_ep->instances.externalEndpoints[idxOff].theHandle_single->module_reference(&astrP, nullptr);
					combine = (std::string)"E:" + astrP.std_str() + ":";
					astrP.clear();
					res = elmIt_ep->instances.externalEndpoints[idxOff].theHandle_single->description(&astrP);
					combine += astrP.std_str();
					if (str)
					{
						*str = combine;
					}

				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}

		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (idx < elmIt_si->instances.availableEndpoints.size())
			{
				res = elmIt_si->instances.availableEndpoints[idx].theHandle_single->description(str);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	return(res);
}

/*
 * Return the names of components of a specific type
 */
jvxErrorType
CjvxComponentHost::_descriptor_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str, jvxApiString* substr)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (idx < elmIt_tech->technologyInstances.availableTechnologies.size())
		{
			res = elmIt_tech->technologyInstances.availableTechnologies[idx].theHandle_single->descriptor(str, substr);
		}
		else
		{
			jvxSize idxOff = idx - elmIt_tech->technologyInstances.availableTechnologies.size();
			if (idxOff < elmIt_tech->technologyInstances.externalTechnologies.size())
			{
				res = elmIt_tech->technologyInstances.externalTechnologies[idxOff].theHandle_single->descriptor(str, substr);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}

	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->descriptor_device(idx, str, substr);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (idx < elmIt_ep->instances.availableEndpoints.size())
			{
				res = elmIt_ep->instances.availableEndpoints[idx].theHandle_single->descriptor(str, substr);
			}
			else
			{
				jvxSize idxOff = idx - elmIt_ep->instances.availableEndpoints.size();
				if (idxOff < elmIt_ep->instances.externalEndpoints.size())
				{
					res = elmIt_ep->instances.externalEndpoints[idxOff].theHandle_single->descriptor(str, substr);
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (idx < elmIt_si->instances.availableEndpoints.size())
			{
				res = elmIt_si->instances.availableEndpoints[idx].theHandle_single->descriptor(str, substr);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}

	return(res);
}

/*
 * Return the names of components of a specific type
 */
jvxErrorType
CjvxComponentHost::_module_reference_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str, jvxComponentAccessType* acTp)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (idx < elmIt_tech->technologyInstances.availableTechnologies.size())
		{
			res = elmIt_tech->technologyInstances.availableTechnologies[idx].theHandle_single->module_reference(str, acTp);
		}
		else
		{
			jvxSize idxOff = idx - elmIt_tech->technologyInstances.availableTechnologies.size();
			if (idxOff < elmIt_tech->technologyInstances.externalTechnologies.size())
			{
				res = elmIt_tech->technologyInstances.externalTechnologies[idxOff].theHandle_single->module_reference(str, acTp);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}

	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					if (acTp)
					{
						*acTp = JVX_COMPONENT_ACCESS_SUB_COMPONENT;
					}
					res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->module_name_device(idx, str);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (idx < elmIt_ep->instances.availableEndpoints.size())
			{
				elmIt_ep->instances.availableEndpoints[idx].theHandle_single->module_reference(str, acTp);
			}
			else
			{
				jvxSize idxOff = idx - elmIt_ep->instances.availableEndpoints.size();
				if (idxOff < elmIt_ep->instances.externalEndpoints.size())
				{
					res = elmIt_ep->instances.externalEndpoints[idxOff].theHandle_single->module_reference(str, acTp);
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}

		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (idx < elmIt_si->instances.availableEndpoints.size())
			{
				elmIt_si->instances.availableEndpoints[idx].theHandle_single->module_reference(str, acTp);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}

	return(res);
}

/*
 * Return feature class identifier
 */
jvxErrorType
CjvxComponentHost::_feature_class_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxBitField* ft)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize num = 0;
	jvxState stat = JVX_STATE_NONE;
	IjvxDevice* dev = NULL;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (idx < elmIt_tech->technologyInstances.availableTechnologies.size())
		{
			res = elmIt_tech->technologyInstances.availableTechnologies[idx].theHandle_single->feature_class(ft);
		}
		else
		{
			jvxSize idxOff = idx - elmIt_tech->technologyInstances.availableTechnologies.size();
			if (idxOff < elmIt_tech->technologyInstances.externalTechnologies.size())
			{
				res = elmIt_tech->technologyInstances.externalTechnologies[idxOff].theHandle_single->feature_class(ft);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}

	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->state(&stat);
					if (stat >= JVX_STATE_ACTIVE)
					{
						res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->number_devices(&num);
						if (idx < num)
						{
							res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->request_device(idx, &dev);
							if ((res == JVX_NO_ERROR) && dev)
							{
								res = dev->feature_class(ft);
							}
							else
							{
								res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
							}
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
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (idx < elmIt_ep->instances.availableEndpoints.size())
			{
				res = elmIt_ep->instances.availableEndpoints[idx].theHandle_single->feature_class(ft);
			}
			else
			{
				jvxSize idxOff = idx - elmIt_ep->instances.availableEndpoints.size();
				if (idxOff < elmIt_ep->instances.externalEndpoints.size())
				{
					res = elmIt_ep->instances.externalEndpoints[idxOff].theHandle_single->feature_class(ft);
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}

		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (idx < elmIt_si->instances.availableEndpoints.size())
			{
				res = elmIt_si->instances.availableEndpoints[idx].theHandle_single->feature_class(ft);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}

	return(res);
}

jvxErrorType 
CjvxComponentHost::_capabilities_devices_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxDeviceCapabilities& caps)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
		{
			if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
			{
				res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->capabilities_device(idx, caps);
			}
		}
	}
	return res;
}

jvxErrorType
CjvxComponentHost::_role_component_system(jvxComponentType tp,
	jvxComponentType* parentTp, jvxComponentType* childTp,
	jvxComponentTypeClass* classTp)
{
	std::string theDescr;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize num = 0;
	jvxState stat = JVX_STATE_NONE;
	IjvxDevice* dev = NULL;

	if (parentTp)
	{
		*parentTp = JVX_COMPONENT_UNKNOWN;
	}

	if (childTp)
	{
		*childTp = JVX_COMPONENT_UNKNOWN;
	}
	if (classTp)
	{
		*classTp = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;
	}

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (childTp)
		{
			*childTp = elmIt_tech->selector[1];
		}
		if (classTp)
		{
			*classTp = elmIt_tech->classType;
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (parentTp)
			{
				*parentTp = elmIt_dev->selector[0];
			}

			if (classTp)
			{
				*classTp = elmIt_dev->childClassType;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (classTp)
			{
				*classTp = elmIt_ep->classType;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (classTp)
			{
				*classTp = elmIt_si->classType;
			}
		}
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		if (tp == JVX_COMPONENT_HOST)
		{
			res = JVX_NO_ERROR;
			if (classTp)
			{
				*classTp = jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST;
			}
		}
		else if (tp == JVX_COMPONENT_PROCESSING_PROCESS)
		{
			res = JVX_NO_ERROR;

			if (classTp)
			{
				*classTp = jvxComponentTypeClass::JVX_COMPONENT_TYPE_PROCESS;
			}
		}
		else if (tp == JVX_COMPONENT_PROCESSING_GROUP)
		{
			res = JVX_NO_ERROR;
			if (classTp)
			{
				*classTp = jvxComponentTypeClass::JVX_COMPONENT_TYPE_PROCESS;
			}
		}
	}

	return(res);
}

jvxErrorType
CjvxComponentHost::_number_slots_component_system(const jvxComponentIdentification& tp, jvxSize* szSlots_current,
	jvxSize* szSubSlots_current, jvxSize* szSlots_max, jvxSize* szSubSlots_max)
{
	std::string theDescr;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize num = 0;
	jvxState stat = JVX_STATE_NONE;
	IjvxDevice* dev = NULL;

	if (szSlots_current)
	{
		*szSlots_current = 0;
	}

	if (szSubSlots_current)
	{
		*szSubSlots_current = 0;
	}
	if (szSlots_max)
	{
		*szSlots_max = 0;
	}

	if (szSubSlots_max)
	{
		*szSubSlots_max = 0;
	}

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;

		if (szSlots_current)
		{
			*szSlots_current = elmIt_tech->technologyInstances.selTech.size();
		}
		if (szSubSlots_current)
		{
			*szSubSlots_current = 1; // If there only slot ids, the subslot number 1 is good to always return the number of overall combintions as (slot * subslot)
		}
		if (szSlots_max)
		{
			*szSlots_max = elmIt_tech->technologyInstances.numSlotsMax;
		}
		if (szSubSlots_max)
		{
			*szSubSlots_max = elmIt_tech->technologyInstances.numSubSlotsMaxDefault; // Number of subslots is specified here and within the device!! 

			for (auto elmH = elmIt_tech->technologyInstances.numSubSlotsMax_spec.begin(); elmH != elmIt_tech->technologyInstances.numSubSlotsMax_spec.end(); elmH++)
			{
				if (
					(elmH->first == tp.slotid))
				{
					*szSubSlots_max = elmH->second;
					break;
				}
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (szSlots_current)
			{
				*szSlots_current = elmIt_dev->technologyInstances.selTech.size();
			}

			if (szSlots_max)
			{
				*szSlots_max = elmIt_dev->technologyInstances.numSlotsMax;
			}

			if (szSubSlots_max)
			{
				*szSubSlots_max = elmIt_dev->technologyInstances.numSubSlotsMaxDefault;
			}

			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (szSubSlots_current)
				{
					*szSubSlots_current = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size();
				}
				if (szSubSlots_max)
				{
					*szSubSlots_max = elmIt_dev->technologyInstances.selTech[tp.slotid].numSubSlotsMax;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (szSlots_current)
			{
				*szSlots_current = elmIt_ep->instances.theHandle_shortcut.size();
			}
			if (szSubSlots_current)
			{
				*szSubSlots_current = 1; // If there only slot ids, the subslot number 1 is good to always return the number of overall combintions as (slot * subslot)
			}
			if (szSlots_max)
			{
				*szSlots_max = elmIt_ep->instances.numSlotsMax;
			}
			if (szSubSlots_max)
			{
				*szSubSlots_max = 1;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (szSlots_current)
			{
				*szSlots_current = elmIt_si->instances.theHandle_shortcut.size();
			}
			if (szSubSlots_current)
			{
				*szSubSlots_current = 1; // If there only slot ids, the subslot number 1 is good to always return the number of overall combintions as (slot * subslot)
			}
			if (szSlots_max)
			{
				*szSlots_max = elmIt_si->instances.numSlotsMax;
			}
			if (szSubSlots_max)
			{
				*szSubSlots_max = 1;
			}
		}
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		if (
			(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
		{
			res = JVX_NO_ERROR;
			if (szSlots_current)
			{
				*szSlots_current = 1;
			}
			if (szSubSlots_current)
			{
				*szSubSlots_current = 1; // If there only slot ids, the subslot number 1 is good to always return the number of overall combintions as (slot * subslot)
			}
			if (szSlots_max)
			{
				*szSlots_max = 1;
			}
			if (szSubSlots_max)
			{
				*szSubSlots_max = 1;
			}
		}
		else if (tp.tp == JVX_COMPONENT_PROCESSING_PROCESS)
		{
			if (datConns)
			{
				datConns->number_connections_process(&num);
				int slotidmax = -1;
				for (jvxSize i = 0; i < num; i++)
				{
					IjvxDataConnectionProcess* conProc = NULL;
					IjvxObject* objRef = NULL;
					jvxComponentIdentification tp;
					datConns->reference_connection_process(i, &conProc);
					conProc->object_hidden_interface(&objRef);
					if (objRef)
					{
						objRef->request_specialization(NULL, &tp, NULL, NULL);
						slotidmax = JVX_MAX(tp.slotid, slotidmax);
						res = JVX_NO_ERROR;
					}
					datConns->return_reference_connection_process(conProc);
					conProc = NULL;
				}
				if (szSlots_current)
				{
					*szSlots_current = slotidmax + 1;
				}
				if (szSubSlots_current)
				{
					*szSubSlots_current = 1;
				}
				if (szSlots_max)
				{
					*szSlots_max = JVX_SIZE_UNSELECTED;
				}
				if (szSubSlots_max)
				{
					*szSubSlots_max = JVX_SIZE_UNSELECTED;
				}
			}
		}
		else if (tp.tp == JVX_COMPONENT_PROCESSING_GROUP)
		{
			if (datConns)
			{
				datConns->number_connections_group(&num);
				int slotidmax = -1;
				for (jvxSize i = 0; i < num; i++)
				{
					IjvxDataConnectionGroup* conGrp = NULL;
					IjvxObject* objRef = NULL;
					jvxComponentIdentification tp;
					datConns->reference_connection_group(i, &conGrp);
					conGrp->object_hidden_interface(&objRef);
					if (objRef)
					{
						objRef->request_specialization(NULL, &tp, NULL, NULL);
						slotidmax = JVX_MAX(tp.slotid, slotidmax);
						res = JVX_NO_ERROR;
					}
					datConns->return_reference_connection_group(conGrp);
					conGrp = NULL;
				}
				if (szSlots_current)
				{
					*szSlots_current = slotidmax + 1;
				}
				if (szSubSlots_current)
				{
					*szSubSlots_current = 1;
				}
				if (szSlots_max)
				{
					*szSlots_max = JVX_SIZE_UNSELECTED;
				}
				if (szSubSlots_max)
				{
					*szSubSlots_max = JVX_SIZE_UNSELECTED;
				}
			}
		}
	}

	return(res);
}

jvxErrorType
CjvxComponentHost::_set_number_subslots_system(const jvxComponentIdentification& tp, jvxSize newVal)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	std::vector<oneTechnologyType>::iterator elmIt_tech;

	std::vector<oneTechnologyType>::iterator elmIt_dev;
	elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0); // <= specified by means of the TECHNOLOGY type
	if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
		{
			elmIt_dev->technologyInstances.selTech[tp.slotid].numSubSlotsMax = newVal;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return res;
}

// ====================================================================================================================
// ====================================================================================================================
// ====================================================================================================================
// ====================================================================================================================
// ====================================================================================================================

/*
 * Return the names of components of a specific type
 */
jvxErrorType
CjvxComponentHost::_module_reference_selected_component(const jvxComponentIdentification& tp, jvxApiString* str, jvxComponentAccessType* acTp)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
		{
			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->module_reference(str, acTp);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
				{
					if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
					{
						res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->module_reference(str, acTp);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE_SUBMODULE;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
					{
						res = elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->module_reference(str, acTp);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_si->instances.theHandle_shortcut[tp.slotid].obj)
					{
						res = elmIt_si->instances.theHandle_shortcut[tp.slotid].obj->module_reference(str, acTp);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		if (
			(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
		{
			res = JVX_NO_ERROR;
			if (str)
			{
				if (myObjectRef)
				{
					myObjectRef->module_reference(str, nullptr);
				}
				//str->assign(myModuleName);
			}
			if (acTp)
			{
				*acTp = JVX_COMPONENT_ACCESS_SYSTEM;
			}
		}
		else if (tp.tp == JVX_COMPONENT_PROCESSING_PROCESS)
		{
			res = JVX_ERROR_UNSUPPORTED;

			if (datConns)
			{
				res = JVX_NO_ERROR;
				jvxSize num = 0;
				jvxSize i;
				IjvxDataConnectionProcess* conProc = NULL;
				jvxComponentIdentification tpComp;

				datConns->number_connections_process(&num);
				for (i = 0; i < num; i++)
				{
					datConns->reference_connection_process(i, &conProc);
					assert(conProc);
					conProc->request_specialization(NULL, &tpComp, NULL, NULL);
					datConns->return_reference_connection_process(conProc);
					if (tp.slotid == tpComp.slotid)
					{
						conProc->module_reference(str, acTp);
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
		}
		else if (tp.tp == JVX_COMPONENT_PROCESSING_GROUP)
		{
			res = JVX_ERROR_UNSUPPORTED;
			
			if (datConns)
			{
				res = JVX_NO_ERROR;
				jvxSize num = 0;
				jvxSize i;
				IjvxDataConnectionGroup* conGrp = NULL;
				jvxComponentIdentification tpComp;

				datConns->number_connections_group(&num);
				for (i = 0; i < num; i++)
				{
					datConns->reference_connection_group(i, &conGrp);
					assert(conGrp);
					conGrp->request_specialization(NULL, &tpComp, NULL, NULL);
					datConns->return_reference_connection_group(conGrp);
					if (tp.slotid == tpComp.slotid)
					{
						conGrp->module_reference(str, acTp);
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
		}
	}

	return(res);
}

/*
 * Return the names of components of a specific type
 */
jvxErrorType
CjvxComponentHost::_description_selected_component(const jvxComponentIdentification& tp, jvxApiString* str)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
		{
			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->description(str);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
				{
					if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
					{
						res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->description(str);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE_SUBMODULE;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
					{
						res = elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->description(str);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_si->instances.theHandle_shortcut[tp.slotid].obj)
					{
						res = elmIt_si->instances.theHandle_shortcut[tp.slotid].obj->description(str);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
			}
		}
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		if (
			(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
		{
			res = JVX_NO_ERROR;
			if (myObjectRef)
			{
				myObjectRef->description(str);
			}
		}
		else if (tp.tp == JVX_COMPONENT_PROCESSING_PROCESS)
		{
			jvxSize num = 0;
			jvxSize i;
			IjvxDataConnectionProcess* conProc = NULL;
			jvxComponentIdentification tpComp;
			if (datConns)
			{
				datConns->number_connections_process(&num);
				for (i = 0; i < num; i++)
				{
					datConns->reference_connection_process(i, &conProc);
					assert(conProc);
					conProc->request_specialization(NULL, &tpComp, NULL, NULL);
					datConns->return_reference_connection_process(conProc);
					if (tp.slotid == tpComp.slotid)
					{
						conProc->description(str);
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
		}
		else if (tp.tp == JVX_COMPONENT_PROCESSING_GROUP)
		{
			jvxSize num = 0;
			jvxSize i;
			IjvxDataConnectionGroup* conGrp = NULL;
			jvxComponentIdentification tpComp;
			if (datConns)
			{
				datConns->number_connections_group(&num);
				for (i = 0; i < num; i++)
				{
					datConns->reference_connection_group(i, &conGrp);
					assert(conGrp);
					conGrp->request_specialization(NULL, &tpComp, NULL, NULL);
					datConns->return_reference_connection_group(conGrp);
					if (tp.slotid == tpComp.slotid)
					{
						conGrp->description(str);
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
		}
	}

	return(res);
}


/*
 * Return the names of components of a specific type
 */
jvxErrorType
CjvxComponentHost::_descriptor_selected_component(const jvxComponentIdentification& tp, jvxApiString* str, jvxApiString* substr)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
		{
			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->descriptor(str, substr);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;

			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
				{

					if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
					{
						res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->descriptor(str, substr);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE_SUBMODULE;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
					{
						res = elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->descriptor(str, substr);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_si->instances.theHandle_shortcut[tp.slotid].obj)
					{
						res = elmIt_si->instances.theHandle_shortcut[tp.slotid].obj->descriptor(str, substr);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		if (
			(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
		{
			res = JVX_NO_ERROR;
			if (myObjectRef)
			{
				myObjectRef->descriptor(str);
			}
		}
	}

	return(res);
}

/*
 * Return feature class identifier
 */
jvxErrorType
CjvxComponentHost::_feature_class_selected_component(const jvxComponentIdentification& tp, jvxBitField* ft)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize num = 0;
	jvxState stat = JVX_STATE_NONE;
	IjvxDevice* dev = NULL;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
		{
			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->feature_class(ft);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
				{

					if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
					{
						res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->feature_class(ft);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);
		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
					{
						res = elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->feature_class(ft);

					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);
		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_si->instances.theHandle_shortcut[tp.slotid].obj)
					{
						res = elmIt_si->instances.theHandle_shortcut[tp.slotid].obj->feature_class(ft);

					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}

	return(res);
}

/*
 * Select a component of a specific type
 */
jvxErrorType
CjvxComponentHost::_select_component(jvxComponentIdentification& tp, jvxSize idx,
	IjvxObject* theOwner, jvxBool extend_if_necessary)
{
	jvxSize h;

	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (myState() >= JVX_STATE_ACTIVE)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			std::vector<oneSelectedTechnology> oldList = elmIt_tech->technologyInstances.selTech;

			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (tp.slotid >= elmIt_tech->technologyInstances.selTech.size())
				{

					if (tp.slotid < elmIt_tech->technologyInstances.numSlotsMax)
					{
						// If we do not have enough slots, expand!
						for (h = elmIt_tech->technologyInstances.selTech.size(); h <= tp.slotid; h++)
						{
							oneSelectedTechnology oneElm;

							// Set the default value!!
							oneElm.numSubSlotsMax = elmIt_tech->technologyInstances.numSubSlotsMaxDefault;
							oneElm.theHandle_shortcut_tech = NULL;
							// oneElm.theHandle_shortcut_dev.clear();
							elmIt_tech->technologyInstances.selTech.push_back(oneElm);
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
						return res;
					}
				}

				if (res == JVX_NO_ERROR)
				{
					if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech == NULL)
					{
						if (idx < elmIt_tech->technologyInstances.availableTechnologies.size())
						{
							IjvxObject* theHandle = NULL;
							IjvxTechnology* theTech = NULL;
							if (elmIt_tech->technologyInstances.availableTechnologies[idx].common.allowsMultiObjects)
							{
								if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
								{
									res = JVX_ERROR_WRONG_STATE_SUBMODULE;
								}
								else
								{
									elmIt_tech->technologyInstances.availableTechnologies[idx].common.linkage.funcInit(&theHandle, NULL,
										elmIt_tech->technologyInstances.availableTechnologies[idx].theHandle_single);
									assert(theHandle);
									theHandle->request_specialization(reinterpret_cast<jvxHandle**>(&theTech), NULL, NULL, NULL);
								}
							}
							else
							{
								theHandle = static_cast<IjvxObject*>(elmIt_tech->technologyInstances.availableTechnologies[idx].theHandle_single);
								theTech = elmIt_tech->technologyInstances.availableTechnologies[idx].theHandle_single;
							}
							if (res == JVX_NO_ERROR)
							{
								jvxSize uid = 0;
								std::string nm = jvxComponentIdentification_txt(tp);
								if(uIdInst)
									uIdInst->obtain_unique_id(&uid, nm.c_str());

								res = theTech->initialize(hIfRef);
								if (res == JVX_NO_ERROR)
								{
									IjvxHiddenInterface* theHinterface = static_cast<IjvxHiddenInterface*>(theTech);
									IjvxCoreStateMachine* theObjectSm = static_cast<IjvxCoreStateMachine*>(theTech);
									IjvxObject* theObject = static_cast<IjvxObject*>(theTech);
									tp.uId = uid;

									prerun_stateswitch(JVX_STATE_SWITCH_SELECT, tp);
									res = static_local_select(theHinterface, theObjectSm, theObject, tp, theOwner);

									if (res == JVX_NO_ERROR)
									{
										elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech = theTech;
										elmIt_tech->technologyInstances.selTech[tp.slotid].idSel = idx;
										elmIt_tech->technologyInstances.selTech[tp.slotid].uid = uid;

										// Override number of 
										for (auto elmH = elmIt_tech->technologyInstances.numSubSlotsMax_spec.begin(); elmH != elmIt_tech->technologyInstances.numSubSlotsMax_spec.end(); elmH++)
										{
											if (
												(elmH->first == tp.slotid))
											{
												elmIt_tech->technologyInstances.selTech[tp.slotid].numSubSlotsMax = elmH->second;
												break;
											}
										}
									}
									else
									{
										tp.uId = JVX_SIZE_UNSELECTED;
										res = elmIt_tech->technologyInstances.availableTechnologies[idx].theHandle_single->terminate();
										if (elmIt_tech->technologyInstances.availableTechnologies[idx].common.allowsMultiObjects)
										{
											elmIt_tech->technologyInstances.availableTechnologies[idx].common.linkage.funcTerm(theHandle);
										}
										res = JVX_ERROR_INTERNAL;
									}
									postrun_stateswitch(JVX_STATE_SWITCH_SELECT, tp, res);
								}
								else
								{
									if (elmIt_tech->technologyInstances.availableTechnologies[idx].common.allowsMultiObjects)
									{
										elmIt_tech->technologyInstances.availableTechnologies[idx].common.linkage.funcTerm(theHandle);
									}
									res = JVX_ERROR_INTERNAL;
								}
							}
						}
						else
						{
							jvxSize idxOff = idx - elmIt_tech->technologyInstances.availableTechnologies.size();
							if (idxOff < elmIt_tech->technologyInstances.externalTechnologies.size())
							{
								res = JVX_ERROR_NO_ACCESS;
							}
							else
							{
								res = JVX_ERROR_ID_OUT_OF_BOUNDS;
							}
						}

					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}

					if (res != JVX_NO_ERROR)
					{
						// Revert to old list
						elmIt_tech->technologyInstances.selTech = oldList;
					}
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				IjvxDevice* theNewDevice = NULL;
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					// =========================================================================
					// Use the next valid slot - find empty container first
					if (tp.slotsubid == JVX_SIZE_DONTCARE)
					{
						for (h = 0; h < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size(); h++)
						{
							if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[h].dev == NULL)
							{
								tp.slotsubid = h;
								break;
							}
						}
					}

					// If we still have not found an empty container, try to allocate new instance
					if (tp.slotsubid == JVX_SIZE_DONTCARE)
					{
						tp.slotsubid = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size();
					}
					// =========================================================================


					std::vector<oneSelectedDevice> oldList = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev;
					if (tp.slotsubid >= elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (
							(tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].numSubSlotsMax) ||
							extend_if_necessary)
						{
							// If we do not have enough slots, expand!
							for (h = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size(); h <= tp.slotsubid; h++)
							{
								oneSelectedDevice newElm;
								newElm.dev = NULL;
								newElm.cfac = NULL;
								newElm.mfac = NULL;
								elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.push_back(newElm);
							}
						}
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;
							return res;
						}
					}

					if (res == JVX_NO_ERROR)
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev == NULL)
						{
							if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
							{
								res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->request_device(idx, &theNewDevice);
								if ((res == JVX_NO_ERROR) && (theNewDevice))
								{
									res = theNewDevice->initialize(hIfRef);
									if (res == JVX_NO_ERROR)
									{
										jvxSize uid = 0;
										std::string nm = jvxComponentIdentification_txt(tp);
										if (uIdInst)
											uIdInst->obtain_unique_id(&uid, nm.c_str());

										IjvxHiddenInterface* theHinterface = static_cast<IjvxHiddenInterface*>(theNewDevice);
										IjvxCoreStateMachine* theObjectSm = static_cast<IjvxCoreStateMachine*>(theNewDevice);
										IjvxObject* theObject = static_cast<IjvxObject*>(theNewDevice);
										tp.uId = uid;
										prerun_stateswitch(JVX_STATE_SWITCH_SELECT, tp);
										res = static_local_select(theHinterface, theObjectSm, theObject, tp, theOwner);

										if (res == JVX_NO_ERROR)
										{
											oneSelectedDevice newElm;
											newElm.dev = theNewDevice;
											newElm.cfac = NULL;
											newElm.mfac = NULL;
											newElm.uid = uid;
											elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid] = newElm;
										}
										else
										{
											tp.uId = JVX_SIZE_UNSELECTED;
											res = JVX_ERROR_INTERNAL;
										}
										postrun_stateswitch(JVX_STATE_SWITCH_SELECT, tp, res);
									}
									else
									{
										res = JVX_ERROR_INTERNAL;
									}
								}
								else
								{
									res = JVX_ERROR_INTERNAL;
								}
							}
							else
							{
								res = JVX_ERROR_WRONG_STATE_SUBMODULE;
							}
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE;
						}

						if (res != JVX_NO_ERROR)
						{
							// Revert to old state
							elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev = oldList;
						}
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			res = t_select_component<IjvxNode>(_common_set_types.registeredNodeTypes, tp,
				idx, theOwner, extend_if_necessary);
		}// 

		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			res = t_select_component<IjvxSimpleComponent>(_common_set_types.registeredSimpleTypes, tp,
				idx, theOwner, extend_if_necessary);
		}// 

		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			if (
				(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
			{
				res = JVX_ERROR_NO_ACCESS;
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	// TODO: minimize size of dynamic arrays
	return(res);
}

/**
 * Return the idx to identify the currently selected component
 */
jvxErrorType
CjvxComponentHost::_selection_component(const jvxComponentIdentification& tp, jvxSize* idRet, jvxApiString* modNmRet)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize id = JVX_SIZE_UNSELECTED;
	std::string nmSelected;
	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
		{
			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				id = elmIt_tech->technologyInstances.selTech[tp.slotid].idSel;
				nmSelected = elmIt_tech->technologyInstances.selTech[tp.slotid].nmExternal;
				/*
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech != NULL)
				{
					for (i = 0; i < elmIt_tech->technologyInstances.availableTechnologies.size(); i++)
					{
						if (elmIt_tech->technologyInstances.availableTechnologies[i].theHandle ==
							elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
						{
							id = i;
							break;
						}
					}
				}*/
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
				{
					if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech != NULL)
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev != NULL)
						{
							res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->ident_device(&id,
								elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
						}
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;

			if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					id = elmIt_ep->instances.theHandle_shortcut[tp.slotid].idSel;
					nmSelected = elmIt_ep->instances.theHandle_shortcut[tp.slotid].nmExternal;
				}
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;

			if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					id = elmIt_si->instances.theHandle_shortcut[tp.slotid].idSel;
				}
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		if (
			(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
		{
			res = JVX_NO_ERROR;
			id = 0;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}

	if (idRet)
	{
		*idRet = id;
	}
	if (modNmRet)
	{
		*modNmRet = nmSelected;
	}
	return(res);
}

/**
 * Activate a component of a specific type as currently selected.
 */
jvxErrorType
CjvxComponentHost::_activate_selected_component(const jvxComponentIdentification& tp)
{
	jvxApiString theName;

	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (myState() >= JVX_STATE_ACTIVE)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;

		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
					{
						if (JVX_CHECK_SIZE_SELECTED(elmIt_tech->technologyInstances.selTech[tp.slotid].idSel))
						{
							prerun_stateswitch(JVX_STATE_SWITCH_ACTIVATE, tp);
							res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->activate();
							postrun_stateswitch(JVX_STATE_SWITCH_ACTIVATE, tp, res);

							if (res != JVX_NO_ERROR)
							{
								jvxApiError theErr;
								elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->description(&theName);
								elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->lasterror(&theErr);

								reportErrorDescription((std::string)"Failed to activate technology type <" + elmIt_tech->description + ">, technology <" +
									theName.std_str() + ">, reason: " + theErr.errorDescription.std_str());
							}
						}
						else
						{
							res = JVX_ERROR_NO_ACCESS;
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

		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							this->prerun_stateswitch(JVX_STATE_SWITCH_ACTIVATE, tp);
							res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->activate();
							this->postrun_stateswitch(JVX_STATE_SWITCH_ACTIVATE, tp, res);
							if (res != JVX_NO_ERROR)
							{
								jvxApiError theErr;
								elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->description(&theName);
								elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->lasterror(&theErr);

								reportErrorDescription((std::string)"Failed to activate device type <" + elmIt_dev->description + ">, device <" +
									theName.std_str() + ">, reason: " + theErr.errorDescription.std_str());
							}
							else
							{
								// Link the corresponding connector factory
								elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->request_hidden_interface(
									JVX_INTERFACE_CONNECTOR_FACTORY,
									reinterpret_cast<jvxHandle**>(&elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].cfac));
								elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->request_hidden_interface(
									JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
									reinterpret_cast<jvxHandle**>(&elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].mfac));

								res = this->connection_factory_to_be_added(
									tp,
									elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].cfac,
									elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].mfac);
								if (res != JVX_NO_ERROR)
								{
									reportErrorDescription("Failed to add connector and/or connector master factory for device", true);
								}
							}
							/*
							else
							{
								_updateAllAfterStateChanged(tp, JVX_STATE_ACTIVE, reinterpret_cast<IjvxHost*>(this));
							}*/
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			res = t_activate_component<IjvxNode>(_common_set_types.registeredNodeTypes, tp);
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			res = t_activate_component<IjvxSimpleComponent>(_common_set_types.registeredSimpleTypes, tp);
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			if (
				(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
			{
				res = JVX_ERROR_NO_ACCESS;
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxComponentHost::_is_ready_selected_component(const jvxComponentIdentification& tp, jvxBool* ready, jvxApiString* reasonIfNot)
{
	std::string reason;

	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (myState() >= JVX_STATE_ACTIVE)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;

		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
					{
						res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->is_ready(ready, reasonIfNot);
					}
					else
					{
						if (ready)
						{
							*ready = false;
						}
						reason = "No technology selected!";
						if (reasonIfNot) reasonIfNot->assign(reason);
						//res = JVX_ERROR_WRONG_STATE_SUBMODULE;
					}
				}
			}
			else
			{
				if (ready)
				{
					*ready = false;
				}
				reason = "Technology slot id is out of valid range!";
				if (reasonIfNot) reasonIfNot->assign(reason);

				//res = JVX_ERROR_WRONG_STATE_SUBMODULE;
			}
		}

		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->is_ready(ready, reasonIfNot);
							assert(res == JVX_NO_ERROR);
						}
						else
						{
							if (ready)
							{
								*ready = false;
							}
							reason = "No device selected!";
							//res = JVX_ERROR_WRONG_STATE_SUBMODULE;
							if (reasonIfNot) reasonIfNot->assign(reason);

						}
					}
					else
					{
						if (ready)
						{
							*ready = false;
						}
						reason = "Device slot id is out of valid range!";
						//res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						if (reasonIfNot) reasonIfNot->assign(reason);

					}
				}
				else
				{
					if (ready)
					{
						*ready = false;
					}
					reason = "Technology slot id is out of valid range!";
					if (reasonIfNot) reasonIfNot->assign(reason);

					//res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
			elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

			if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
				{
					if (tp.slotsubid != 0)
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					else
					{
						if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
						{
							res = elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->is_ready(ready, reasonIfNot);
						}
						else
						{
							if (ready)
							{
								*ready = false;
							}
							reason = "No node selected!";
							if (reasonIfNot) reasonIfNot->assign(reason);

							//res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
				}
				else
				{
					if (ready)
					{
						*ready = false;
					}
					reason = "Node slot id is out of valid range!";
					if (reasonIfNot) reasonIfNot->assign(reason);

					//res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
			elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

			if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
				{
					if (tp.slotsubid != 0)
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					else
					{
						// elmIt_si->instances.theHandle_shortcut[tp.slotid].obj->is_ready(ready, reasonIfNot); 
						// is_ready is not defined for simple components!!

						if (ready)
						{
							*ready = false;
						}
						reason = "No node selected!";
						if (reasonIfNot)
						{
							reasonIfNot->assign(reason);
						}
						//res = JVX_ERROR_WRONG_STATE_SUBMODULE;						
					}
				}
				else
				{
					if (ready)
					{
						*ready = false;
					}
					reason = "Node slot id is out of valid range!";
					if (reasonIfNot) reasonIfNot->assign(reason);

					//res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
		}
	}

	return(res);
}

/**
 * Return the state of the component currently selected.
 */
jvxErrorType
CjvxComponentHost::_state_selected_component(const jvxComponentIdentification& tp, jvxState* st)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxState stat = JVX_STATE_NONE;

	bool foundit = false;

	if (st)
	{
		*st = JVX_STATE_NONE;
	}
	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
		{
			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech != NULL)
				{
					res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->state(st);
				}
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
				{
					if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev != NULL)
					{
						res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->state(st);
					}
				}
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj != NULL)
					{
						elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->state(st);
					}
				}
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
		elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

		if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_si->instances.theHandle_shortcut[tp.slotid].obj != NULL)
					{
						elmIt_si->instances.theHandle_shortcut[tp.slotid].obj->state(st);
					}
				}
			}
		}
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		if (
			(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
		{
			res = JVX_NO_ERROR;
			if (st)
			{
				*st = myState();
			}
		}
	}
	return(res);
}

/**
 * Deactivate a component of a specific type as currently selected.
 */
jvxErrorType
CjvxComponentHost::_deactivate_selected_component(const jvxComponentIdentification& tp)
{
	jvxSize k;
	jvxState stat = JVX_STATE_NONE;

	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	jvxErrorType resL;

	if (myState() == JVX_STATE_ACTIVE)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;

			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
					{

						if (JVX_CHECK_SIZE_SELECTED(elmIt_tech->technologyInstances.selTech[tp.slotid].idSel))
						{

							elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->state(&stat);

							if (stat >= JVX_STATE_ACTIVE)
							{
								// Deactivate all devices
								while (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
								{
									for (k = 0; k < elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size(); k++)
									{
										if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].dev)
										{
											jvxComponentIdentification tpIdDev = jvxComponentIdentification(elmIt_tech->selector[1], tp.slotid, k);
											resL = _state_selected_component(tpIdDev, &stat);
											if (stat >= JVX_STATE_ACTIVE)
											{
												res = this->connection_factory_to_be_removed(
													tpIdDev,
													elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].cfac,
													elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].mfac);

												if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].cfac)
												{
													elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].dev->return_hidden_interface(
														JVX_INTERFACE_CONNECTOR_FACTORY,
														reinterpret_cast<jvxHandle*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].cfac));
													elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].cfac = NULL;
												}
												if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].mfac)
												{
													elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].dev->return_hidden_interface(
														JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
														reinterpret_cast<jvxHandle*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].mfac));
													elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[k].mfac = NULL;
												}

												resL = _deactivate_selected_component(tpIdDev);
												assert(resL == JVX_NO_ERROR);
											}

											resL = _unselect_selected_component(tpIdDev);
											assert(resL == JVX_NO_ERROR);
											break;
										}
									}
								}

								prerun_stateswitch(JVX_STATE_SWITCH_DEACTIVATE, tp);
								res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->deactivate();
								postrun_stateswitch(JVX_STATE_SWITCH_DEACTIVATE, tp, res);
							}
							else
							{
								res = JVX_ERROR_WRONG_STATE_SUBMODULE;
							}

						}
						else
						{
							res = JVX_ERROR_NO_ACCESS;
						}
					}
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							// HK:CHECKTHIS
							elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->state(&stat);

							// Important: the device may also be still processing! The stop of processing may
							// only happen if the connection is stopped by sequencer before the disconnect.
							// This task, however, can only be realized in the AUTOMATION functionality
							if (stat >= JVX_STATE_ACTIVE)
							{
								if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].mfac ||
									elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].cfac)
								{
									res = this->connection_factory_to_be_removed(
										tp,
										elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].cfac,
										elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].mfac);
									if (res != JVX_NO_ERROR)
									{
										return res;
									}
								}
								if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].cfac)
								{
									elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->return_hidden_interface(
										JVX_INTERFACE_CONNECTOR_FACTORY,
										reinterpret_cast<jvxHandle*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].cfac));
									elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].cfac = NULL;
								}
								if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].mfac)
								{
									elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->return_hidden_interface(
										JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
										reinterpret_cast<jvxHandle*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].mfac));
									elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].cfac = NULL;
								}

								prerun_stateswitch(JVX_STATE_SWITCH_DEACTIVATE, tp);
								res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->deactivate();
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
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			res = t_deactivate_component<IjvxNode>(_common_set_types.registeredNodeTypes, tp);
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			res = t_deactivate_component<IjvxSimpleComponent>(_common_set_types.registeredSimpleTypes, tp);
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			if (
				(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
			{
				res = JVX_ERROR_NO_ACCESS;
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}




/**
 * Unselect a component of a specific type as currently selected.
 */
jvxErrorType
CjvxComponentHost::_unselect_selected_component(jvxComponentIdentification& tp)
{
	int h; // must be int!
	jvxState stat = JVX_STATE_NONE;

	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (myState() == JVX_STATE_ACTIVE)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;

		// Implicit deactivate on unselect
		_state_selected_component(tp, &stat);
		if (stat >= JVX_STATE_ACTIVE)
		{
			res = _deactivate_selected_component(tp);
		}

		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech != NULL)
					{
						if (JVX_CHECK_SIZE_SELECTED(elmIt_tech->technologyInstances.selTech[tp.slotid].idSel))
						{
							IjvxHiddenInterface* theHinterface = static_cast<IjvxHiddenInterface*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
							IjvxStateMachine* theObjectSm = static_cast<IjvxStateMachine*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
							IjvxObject* theObject = static_cast<IjvxObject*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);

							prerun_stateswitch(JVX_STATE_SWITCH_UNSELECT, tp);
							res = static_local_unselect(theHinterface, theObject, tp, theObjectSm);
							postrun_stateswitch(JVX_STATE_SWITCH_UNSELECT, tp, res);

							if (res == JVX_NO_ERROR)
							{
								// Release the unique id
								uIdInst->release_unique_id(elmIt_tech->technologyInstances.selTech[tp.slotid].uid);
								tp.uId = JVX_SIZE_UNSELECTED;
								elmIt_tech->technologyInstances.selTech[tp.slotid].uid = JVX_SIZE_UNSELECTED;

								res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->terminate();
								if (res == JVX_NO_ERROR)
								{
									if (elmIt_tech->technologyInstances.availableTechnologies[elmIt_tech->technologyInstances.selTech[tp.slotid].idSel].
										common.allowsMultiObjects)
									{
										elmIt_tech->technologyInstances.availableTechnologies[elmIt_tech->technologyInstances.selTech[tp.slotid].idSel].common.linkage.funcTerm(
											elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
									}
									elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech = NULL;
								}

								// Try to reduce size of the slot list
								std::vector<oneSelectedTechnology> newLst;
								int sz = (int)elmIt_tech->technologyInstances.selTech.size();
								for (h = sz - 1; h >= 0; h--)
								{
									if (elmIt_tech->technologyInstances.selTech[h].theHandle_shortcut_tech)
									{
										break;
									}
								}
								int copyTo = h;
								for (h = 0; h <= copyTo; h++)
								{
									newLst.push_back(elmIt_tech->technologyInstances.selTech[h]);
								}
								elmIt_tech->technologyInstances.selTech = newLst;

							}
							if (res != JVX_NO_ERROR)
							{
								res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
							}
						}
						else
						{
							res = JVX_ERROR_NO_ACCESS;
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
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev != NULL)
						{
							IjvxHiddenInterface* theHinterface = static_cast<IjvxHiddenInterface*>(
								elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
							IjvxStateMachine* theObjectSm = static_cast<IjvxStateMachine*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
							IjvxObject* theObject = static_cast<IjvxObject*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);

							prerun_stateswitch(JVX_STATE_SWITCH_UNSELECT, tp);
							res = static_local_unselect(theHinterface, theObject, tp, theObjectSm);
							postrun_stateswitch(JVX_STATE_SWITCH_UNSELECT, tp, res);

							if (res == JVX_NO_ERROR)
							{
								// Release the unique id
								uIdInst->release_unique_id(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].uid);
								tp.uId = JVX_SIZE_UNSELECTED;
								elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].uid = JVX_SIZE_UNSELECTED;

								res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->terminate();
								assert(res == JVX_NO_ERROR);

								elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->return_device(
									elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
								elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev = NULL;
							}

							// Try to reduce size of the slot list
							std::vector<oneSelectedDevice> newLst;
							int sz = (int)elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size();
							for (h = sz - 1; h >= 0; h--)
							{
								if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[h].dev)
								{
									break;
								}
							}
							int copyTo = h;
							for (h = 0; h <= copyTo; h++)
							{
								oneSelectedDevice newElm;
								/*
								newElm.dev = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[h].dev;
								newElm.cfac = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[h].cfac;
								newElm.mfac = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[h].mfac;
								newElm.uid = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[h].uid;
								*/
								newElm = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[h];
								newLst.push_back(newElm);
							}
							elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev = newLst;

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
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			res = t_unselect_component<IjvxNode>(_common_set_types.registeredNodeTypes, tp);
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			res = t_unselect_component<IjvxSimpleComponent>(_common_set_types.registeredSimpleTypes, tp);
		}

		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			if (
				(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
			{
				res = JVX_ERROR_NO_ACCESS;
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxErrorType
CjvxComponentHost::_unique_id_selected_component(const jvxComponentIdentification& tp, jvxSize* uId)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
		{
			if (tp.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					res = elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->request_unique_object_id(uId);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
				{
					if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
					{
						res = elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev->request_unique_object_id(uId);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE_SUBMODULE;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
					{
						elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj->request_unique_object_id(uId);
					}				
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		if (
			(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
		{
			res = JVX_NO_ERROR;
			if (uId)
			{
				*uId = myRegisteredHostId();
			}
		}
	}

	return(res);
}

jvxErrorType
CjvxComponentHost::_switch_state_component(const jvxComponentIdentification& cpId, jvxStateSwitch sswitch)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	IjvxStateMachine* targetObject = nullptr;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, cpId.tp, 0);
	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_NO_ERROR;
		if (cpId.slotid < elmIt_tech->technologyInstances.selTech.size())
		{
			if (cpId.slotsubid != 0)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				if (elmIt_tech->technologyInstances.selTech[cpId.slotid].theHandle_shortcut_tech)
				{
					if (JVX_CHECK_SIZE_SELECTED(elmIt_tech->technologyInstances.selTech[cpId.slotid].idSel))
					{
						targetObject = elmIt_tech->technologyInstances.selTech[cpId.slotid].theHandle_shortcut_tech;
					}
					else
					{
						// External components can be addressed if state is ACTIVE or higher
						switch (sswitch)
						{
						case jvxStateSwitch::JVX_STATE_SWITCH_PREPARE:
						case jvxStateSwitch::JVX_STATE_SWITCH_START:
						case jvxStateSwitch::JVX_STATE_SWITCH_STOP:
						case jvxStateSwitch::JVX_STATE_SWITCH_POSTPROCESS:
							targetObject = elmIt_tech->technologyInstances.selTech[cpId.slotid].theHandle_shortcut_tech;
							break;
						default: 
							res = JVX_ERROR_NO_ACCESS;
						}
					}
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneTechnologyType>::iterator elmIt_dev;
		elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, cpId.tp, 1);
		if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (cpId.slotid < elmIt_dev->technologyInstances.selTech.size())
			{
				if (cpId.slotsubid < elmIt_dev->technologyInstances.selTech[cpId.slotid].theHandle_shortcut_dev.size())
				{
					if (elmIt_dev->technologyInstances.selTech[cpId.slotid].theHandle_shortcut_dev[cpId.slotsubid].dev)
					{
						targetObject = elmIt_dev->technologyInstances.selTech[cpId.slotid].theHandle_shortcut_dev[cpId.slotsubid].dev;
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE_SUBMODULE;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, cpId.tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			res = JVX_NO_ERROR;
			if (cpId.slotid < elmIt_ep->instances.theHandle_shortcut.size())
			{
				if (cpId.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_ep->instances.theHandle_shortcut[cpId.slotid].obj)
					{
						if (JVX_CHECK_SIZE_SELECTED(elmIt_ep->instances.theHandle_shortcut[cpId.slotid].idSel))
						{
							targetObject = elmIt_ep->instances.theHandle_shortcut[cpId.slotid].obj;
						}
						else
						{
							// External components can be addressed if state is ACTIVE or higher
							switch (sswitch)
							{
							case jvxStateSwitch::JVX_STATE_SWITCH_PREPARE:
							case jvxStateSwitch::JVX_STATE_SWITCH_START:
							case jvxStateSwitch::JVX_STATE_SWITCH_STOP:
							case jvxStateSwitch::JVX_STATE_SWITCH_POSTPROCESS:
								targetObject = elmIt_ep->instances.theHandle_shortcut[cpId.slotid].obj;
								break;
							default:
								res = JVX_ERROR_NO_ACCESS;
							}
						}
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE;
					}
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	if (targetObject)
	{
		prerun_stateswitch(sswitch, cpId);
		switch (sswitch)
		{
		case JVX_STATE_SWITCH_PREPARE:
			res = targetObject->prepare();
			break;
		case JVX_STATE_SWITCH_START:
			res = targetObject->start();
			break;
		case JVX_STATE_SWITCH_STOP:
			res = targetObject->stop();
			break;
		case JVX_STATE_SWITCH_POSTPROCESS:
			res = targetObject->postprocess();
			break;
		default:
			res = JVX_ERROR_WRONG_STATE;
		}
		postrun_stateswitch(sswitch, cpId, res);
	}
	return(res);
}

jvxErrorType
CjvxComponentHost::_request_hidden_interface_selected_component(const jvxComponentIdentification& tp, jvxInterfaceType ifTp, jvxHandle** iface)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (myState() == JVX_STATE_ACTIVE)
	{
		IjvxHiddenInterface* theIface = NULL;

		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					theIface = static_cast<IjvxHiddenInterface*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							theIface = static_cast<IjvxHiddenInterface*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
				elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

				if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
				{
					if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
					{
						if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
						{
							theIface = static_cast<IjvxHiddenInterface*>(elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
			}
		}

		if ((res == JVX_NO_ERROR) && theIface)
		{
			res = theIface->request_hidden_interface(ifTp, iface);
			if (res != JVX_NO_ERROR)
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxComponentHost::_return_hidden_interface_selected_component(const jvxComponentIdentification& tp, jvxInterfaceType ifTp, jvxHandle* iface)
{

	jvxErrorType res = JVX_NO_ERROR;
	if (myState() == JVX_STATE_ACTIVE)
	{
		IjvxHiddenInterface* theIface = NULL;

		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					theIface = static_cast<IjvxHiddenInterface*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							theIface = static_cast<IjvxHiddenInterface*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
				elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

				if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
				{
					if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
					{
						if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
						{
							theIface = static_cast<IjvxHiddenInterface*>(elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
			}
		}

		if ((res == JVX_NO_ERROR) && theIface)
		{
			res = theIface->return_hidden_interface(ifTp, iface);
			if (res != JVX_NO_ERROR)
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

// ======================================================================
// ======================================================================

jvxErrorType
CjvxComponentHost::_request_object_selected_component(const jvxComponentIdentification& tp, IjvxObject** theObj)
{

	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (myState() == JVX_STATE_ACTIVE)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;

		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
					{
						if (theObj)
						{
							*theObj = static_cast<IjvxObject*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
						}
						res = JVX_NO_ERROR;
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
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							if (theObj)
							{
								*theObj = static_cast<IjvxObject*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
							}
							res = JVX_NO_ERROR;
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
			elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

			if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
			{
				if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
				{
					if (tp.slotsubid != 0)
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					else
					{
						if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
						{
							if (theObj)
							{
								*theObj = static_cast<IjvxObject*>(elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj);
							}
							res = JVX_NO_ERROR;
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
			elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

			if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
			{
				if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
				{
					if (tp.slotsubid != 0)
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					else
					{
						if (elmIt_si->instances.theHandle_shortcut[tp.slotid].obj)
						{
							if (theObj)
							{
								*theObj = static_cast<IjvxObject*>(elmIt_si->instances.theHandle_shortcut[tp.slotid].obj);
							}
							res = JVX_NO_ERROR;
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
				}
			}
		}

		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			// Special rule to return host component
			if (
				(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
			{
				if (theObj)
				{
					*theObj = myObjectRef;
				}
				res = JVX_NO_ERROR;
			}
			else if (tp.tp == JVX_COMPONENT_PROCESSING_PROCESS)
			{
				res = JVX_ERROR_UNSUPPORTED;
				if (datConns)
				{
					jvxSize num = 0;
					jvxSize i;
					IjvxDataConnectionProcess* conProc = NULL;
					jvxComponentIdentification tpComp;

					res = JVX_ERROR_ELEMENT_NOT_FOUND;
					datConns->number_connections_process(&num);
					for (i = 0; i < num; i++)
					{
						datConns->reference_connection_process(i, &conProc);
						assert(conProc);
						conProc->request_specialization(NULL, &tpComp, NULL, NULL);
						datConns->return_reference_connection_process(conProc);
						if (tp.slotid == tpComp.slotid)
						{
							conProc->object_hidden_interface(theObj);
							res = JVX_NO_ERROR;
							break;
						}
					}
				}
			}
			else if (tp.tp == JVX_COMPONENT_PROCESSING_GROUP)
			{
				res = JVX_ERROR_UNSUPPORTED;
				if (datConns)
				{
					jvxSize num = 0;
					jvxSize i;
					IjvxDataConnectionGroup* conGrp = NULL;
					jvxComponentIdentification tpComp;

					res = JVX_ERROR_ELEMENT_NOT_FOUND;
					datConns->number_connections_group(&num);
					for (i = 0; i < num; i++)
					{
						datConns->reference_connection_group(i, &conGrp);
						assert(conGrp);
						conGrp->request_specialization(NULL, &tpComp, NULL, NULL);
						datConns->return_reference_connection_group(conGrp);
						if (tp.slotid == tpComp.slotid)
						{
							conGrp->object_hidden_interface(theObj);
							res = JVX_NO_ERROR;
							break;
						}
					}
				}
			}
			else
			{
				// Here, we call a virtual abstract function implementation since the 
				// "tools" host is in deriving class
				res = map_reference_tool(tp, theObj, JVX_SIZE_UNSELECTED, nullptr);
			}
		}
	}

	return(res);
}

jvxErrorType
CjvxComponentHost::_return_object_selected_component(const jvxComponentIdentification& tp, IjvxObject* theObj)
{
	IjvxObject* theObjComp = NULL;

	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (myState() == JVX_STATE_ACTIVE)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;

		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
					{
						theObjComp = static_cast<IjvxObject*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
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
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							theObjComp = static_cast<IjvxObject*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
			elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

			if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
				{
					if (tp.slotsubid != 0)
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					else
					{
						if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
						{
							theObjComp = static_cast<IjvxObject*>(elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj);
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
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
			elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

			if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
				{
					if (tp.slotsubid != 0)
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					else
					{
						if (elmIt_si->instances.theHandle_shortcut[tp.slotid].obj)
						{
							theObjComp = static_cast<IjvxObject*>(elmIt_si->instances.theHandle_shortcut[tp.slotid].obj);
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
		}

		if (res == JVX_NO_ERROR)
		{
			if (theObjComp)
			{
				if (theObjComp != theObj)
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			return res;
		}

		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			if (
				(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
			{
				res = JVX_NO_ERROR;
				if (theObj != myObjectRef)
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else if (tp.tp == JVX_COMPONENT_PROCESSING_PROCESS)
			{
				res = JVX_ERROR_UNSUPPORTED;
				if (datConns)
				{
					jvxSize num = 0;
					jvxSize i;
					IjvxDataConnectionProcess* conProc = NULL;
					jvxComponentIdentification tpComp;

					res = JVX_ERROR_ELEMENT_NOT_FOUND;
					datConns->number_connections_process(&num);
					for (i = 0; i < num; i++)
					{
						datConns->reference_connection_process(i, &conProc);
						assert(conProc);
						conProc->request_specialization(NULL, &tpComp, NULL, NULL);
						datConns->return_reference_connection_process(conProc);
						if (tp.slotid == tpComp.slotid)
						{
							// conProc->object_hidden_interface(theObj); returning not directly required
							res = JVX_NO_ERROR;
							break;
						}
					}
				}
			}
			else if (tp.tp == JVX_COMPONENT_PROCESSING_GROUP)
			{
				res = JVX_ERROR_UNSUPPORTED;
				if (datConns)
				{
					jvxSize num = 0;
					jvxSize i;
					IjvxDataConnectionGroup* conGrp = NULL;
					jvxComponentIdentification tpComp;

					res = JVX_ERROR_ELEMENT_NOT_FOUND;
					datConns->number_connections_group(&num);
					for (i = 0; i < num; i++)
					{
						datConns->reference_connection_group(i, &conGrp);
						assert(conGrp);
						conGrp->request_specialization(NULL, &tpComp, NULL, NULL);
						datConns->return_reference_connection_group(conGrp);
						if (tp.slotid == tpComp.slotid)
						{
							// conProc->object_hidden_interface(theObj); returning not directly required
							res = JVX_NO_ERROR;
							break;
						}
					}
				}
			}
			else
			{
				// Here, we call a virtual abstract function implementation since the 
				// "tools" host is in deriving class
				res = map_return_reference_tool(tp, theObj);
			}
		}
	}

	return(res);
}
