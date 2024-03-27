#include "jvxHosts/CjvxHostTypeHandler.h"



template <class T> void
t_remove_external_component(IjvxObject* theObj, std::vector<oneObjType<T>>& registeredObj,
	std::map<IjvxObject*, std::string>& staticModulesRem)
{
	jvxSize i;
	for (i = 0; i < registeredObj.size(); i++)
	{
		typename std::vector<oneObj<T>>::iterator elm = registeredObj[i].instances.availableEndpoints.begin();
		for (; elm != registeredObj[i].instances.availableEndpoints.end(); elm++)
		{
			if (elm->common.hObject == theObj)
			{
				break;
			}
		}
		if (elm != registeredObj[i].instances.availableEndpoints.end())
		{
			registeredObj[i].instances.availableEndpoints.erase(elm);

			auto elm = staticModulesRem.find(theObj);
			if (elm != staticModulesRem.end())
			{
				staticModulesRem.erase(elm);
			}
		}
	}
}

template <class T> jvxErrorType
t_add_external_component(IjvxObject* theObj,
	IjvxGlobalInstance* theGlob, const char* locationDescription, jvxBool allowMultipleInstance,
	jvxInitObject_tp funcInit, jvxTerminateObject_tp funcTerm,
	std::vector<oneObjType<T>>& registeredObj, const jvxComponentIdentification& tp,
	std::map<IjvxObject*, std::string>& staticModulesAdd)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	typename std::vector<oneObjType<T>>::iterator elmIt_ob;
	elmIt_ob = jvx_findItemSelectorInList_one<oneObjType<T>, jvxComponentType>(registeredObj, tp.tp, 0);

	jvxComponentIdentification tpRet = JVX_COMPONENT_UNKNOWN;
	jvxBool allowsMultiObjects = false;
	jvxHandle* specComp = NULL;
	jvxApiString astr;
	std::string mName;
	jvxBool moduleRegisteredBefore = false;

	if (elmIt_ob != registeredObj.end())
	{
		oneObj<T> elm;
		T* theNode = NULL;
		res = theObj->request_specialization(reinterpret_cast<jvxHandle**>(&theNode), &tpRet, &allowsMultiObjects);
		if (res == JVX_NO_ERROR)
		{
			if (theNode)
			{
				if (tp == tpRet)
				{
					elm.theHandle_single = NULL;
					elm.theHandle_single = theNode;
					elm.common.isExternalComponent = true;
					elm.common.id = (jvxSize)elmIt_ob->instances.availableEndpoints.size();
					elm.common.linkage.dllPath = "NONE";
					elm.common.linkage.sharedObjectEntry = NULL;
					elm.common.linkage.funcInit = funcInit;
					elm.common.linkage.funcTerm = funcTerm;
					elm.common.tp = tp.tp;
					elm.common.allowsMultiObjects = allowsMultiObjects;
					elm.common.externalLink.description = locationDescription;
					elm.common.hObject = theObj;
					elm.common.hGlobInst = theGlob;

					elm.common.dllProps = 0;
					elmIt_ob->instances.availableEndpoints.push_back(elm);
					staticModulesAdd[theObj] = mName;
					res = JVX_NO_ERROR;
					std::cout << "  - added" << std::endl;
				}
				else
				{
					res = JVX_ERROR_UNEXPECTED;
				}
			}
			else
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
		}
	}
	return res;
}

template <class T> void
t_pre_unload_dlls(std::vector<oneObjType<T>>& registeredObj)
{
	jvxSize i, j;
	for (i = 0; i < registeredObj.size(); i++)
	{
		std::vector<oneObj<T>> remainListN;
		for (j = 0; j < registeredObj[i].instances.availableEndpoints.size(); j++)
		{
			if (registeredObj[i].instances.availableEndpoints[j].common.linkage.packPtr)
			{
				registeredObj[i].instances.availableEndpoints[j].common.linkage.funcTerm(
					registeredObj[i].instances.availableEndpoints[j].common.hObject);
				registeredObj[i].instances.availableEndpoints[j].common.hObject = NULL;
				registeredObj[i].instances.availableEndpoints[j].theHandle_single = NULL;

				registeredObj[i].instances.availableEndpoints[j].common.linkage.packPtr->release_entries_component(
					registeredObj[i].instances.availableEndpoints[j].common.linkage.packIdx);
				registeredObj[i].instances.availableEndpoints[j].common.linkage.packPtr = nullptr;
				registeredObj[i].instances.availableEndpoints[j].common.linkage.packIdx = JVX_SIZE_UNSELECTED;
				registeredObj[i].instances.availableEndpoints[j].common.linkage.sharedObjectEntry = JVX_HMODULE_INVALID;
			}
		}
	}
}

template <class T> void
t_unload_dlls(std::vector<oneObjType<T>>& registeredObj)
{
	jvxSize i, j;
	for (i = 0; i < registeredObj.size(); i++)
	{
		std::vector<oneObj<T>> remainListN;
		for (j = 0; j < registeredObj[i].instances.availableEndpoints.size(); j++)
		{

			if (registeredObj[i].instances.availableEndpoints[j].common.isExternalComponent)
			{
				remainListN.push_back(registeredObj[i].instances.availableEndpoints[j]);
			}
			else
			{
				if (registeredObj[i].instances.availableEndpoints[j].common.hObject)
				{
					// In all other cases, unload library and delete object
					registeredObj[i].instances.availableEndpoints[j].common.linkage.funcTerm(
						registeredObj[i].instances.availableEndpoints[j].common.hObject);
					registeredObj[i].instances.availableEndpoints[j].common.hObject = NULL;
					registeredObj[i].instances.availableEndpoints[j].theHandle_single = NULL;
					registeredObj[i].instances.availableEndpoints[j].common.linkage.funcInit = NULL;
					registeredObj[i].instances.availableEndpoints[j].common.linkage.funcTerm = NULL;
					if (!(JVX_EVALUATE_BITFIELD(registeredObj[i].instances.availableEndpoints[j].common.dllProps & JVX_BITFIELD_DLL_PROPERTY_DO_NOT_UNLOAD)))
					{
						JVX_UNLOADLIBRARY(registeredObj[i].instances.availableEndpoints[j].common.linkage.sharedObjectEntry);
					}
				}
			}
		}
		registeredObj[i].instances.availableEndpoints.clear();
		registeredObj[i].instances.availableEndpoints = remainListN;
	}
}


CjvxHostTypeHandler::CjvxHostTypeHandler()
{
}

CjvxHostTypeHandler::~CjvxHostTypeHandler()
{
}

// ==================================================================================
jvxErrorType 
CjvxHostTypeHandler::fwd_add_external_component(CjvxObject * meObj,
	IjvxObject * theObj, IjvxGlobalInstance * theGlob, 
	const char* locationDescription, jvxBool allowMultipleInstance,
	jvxInitObject_tp funcInit, jvxTerminateObject_tp funcTerm, jvxComponentType tp)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxComponentIdentification tpRet = JVX_COMPONENT_UNKNOWN;
	jvxBool allowsMultiObjects = false;
	std::string mName;

	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(
		_common_set_types.registeredTechnologyTypes, tp, 0);

	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		oneTechnology elm;
		elm.theHandle_single = NULL;
		IjvxTechnology* theTech = NULL;
		res = theObj->request_specialization(reinterpret_cast<jvxHandle**>(&theTech),
			&tpRet, &allowsMultiObjects);
		if (res == JVX_NO_ERROR)
		{
			if (theTech)
			{
				if (tp == tpRet)
				{
					if (allowMultipleInstance == false)
					{
						allowsMultiObjects = false;
					}

					elm.theHandle_single = theTech;
					elm.common.isExternalComponent = true;
					elm.common.id = (jvxSize)elmIt_tech->technologyInstances.availableTechnologies.size();
					elm.common.linkage.dllPath = "NONE";
					elm.common.linkage.sharedObjectEntry = NULL;
					elm.common.linkage.funcInit = funcInit;
					elm.common.linkage.funcTerm = funcTerm;
					elm.common.tp = tp;
					elm.common.allowsMultiObjects = allowsMultiObjects;
					elm.common.externalLink.description = locationDescription;
					elm.common.hObject = theObj;
					elm.common.hGlobInst = theGlob;
					elm.common.dllProps = 0;

					elmIt_tech->technologyInstances.availableTechnologies.push_back(elm);
					staticModules[theObj] = mName;
					res = JVX_NO_ERROR;
					std::cout << "  - added" << std::endl;
				}
				else
				{
					res = JVX_ERROR_UNEXPECTED;
				}
			}
			else
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
		}
		else
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		}
	}
	else
	{
		res = t_add_external_component<IjvxNode>(theObj,
			theGlob, locationDescription, allowMultipleInstance,
			funcInit, funcTerm,
			_common_set_types.registeredNodeTypes, tp,
			staticModules);
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			res = t_add_external_component<IjvxSimpleComponent>(theObj,
				theGlob, locationDescription, allowMultipleInstance,
				funcInit, funcTerm,
				_common_set_types.registeredSimpleTypes, tp,
				staticModules);	
		}
	}
	return res;
}

void 
CjvxHostTypeHandler::fwd_remove_external_component(CjvxObject* meObj, IjvxObject* theObj)
{
	jvxSize i;
	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		std::vector<oneTechnology>::iterator elm = _common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.begin();
		for (; elm != _common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.end(); elm++)
		{
			if (elm->common.hObject == theObj)
			{
				break;
			}
		}
		if (elm != _common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.end())
		{
			_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.erase(elm);

			auto elm = staticModules.find(theObj);
			if (elm != staticModules.end())
			{
				staticModules.erase(elm);
			}
		}
	}

	t_remove_external_component<IjvxNode>(theObj, _common_set_types.registeredNodeTypes, staticModules);
	t_remove_external_component<IjvxSimpleComponent>(theObj, _common_set_types.registeredSimpleTypes, staticModules);
}

jvxErrorType
CjvxHostTypeHandler::fwd_load_all_components(jvxComponentType tp,
	jvxHandle* specCompAdd, jvxBool allowMultipleObjectsAdd,
	IjvxObject* newObjAdd, IjvxGlobalInstance* newGlobInstAdd,
	IjvxPackage* thePack, jvxSize thePackIdx, const std::string& fileName,
	jvxInitObject_tp funcInitAdd, jvxTerminateObject_tp funcTermAdd,
	JVX_HMODULE dllHandle, jvxBitField dllProps, jvxSize& numAdded)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	std::vector<oneTechnologyType>::iterator elmIt_tech;
	elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType,
		jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp, 0);

	if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		oneTechnology elm;

		elm.theHandle_single = NULL;
		elm.theHandle_single = reinterpret_cast<IjvxTechnology*>(specCompAdd);
		elm.common.isExternalComponent = false;
		elm.common.id = (jvxSize)elmIt_tech->technologyInstances.availableTechnologies.size();
		elm.common.tp = tp;
		elm.common.allowsMultiObjects = allowMultipleObjectsAdd;
		elm.common.hObject = newObjAdd;
		elm.common.hGlobInst = newGlobInstAdd;
		elm.common.linkage.packPtr = thePack;
		elm.common.linkage.packIdx = thePackIdx;

		elm.common.linkage.dllPath = fileName;
		elm.common.linkage.funcInit = funcInitAdd;
		elm.common.linkage.funcTerm = funcTermAdd;
		elm.common.linkage.sharedObjectEntry = dllHandle;

		elm.common.externalLink.description = "NONE";
		elm.common.dllProps = dllProps;

		elmIt_tech->technologyInstances.availableTechnologies.push_back(elm);
		res = JVX_NO_ERROR;
		numAdded++;
	}
	else
	{
		std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
		elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>,
			jvxComponentType>(_common_set_types.registeredNodeTypes, tp, 0);

		if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
		{
			oneObj<IjvxNode> elm;
			elm.theHandle_single = NULL;
			elm.theHandle_single = reinterpret_cast<IjvxNode*>(specCompAdd);
			elm.common.isExternalComponent = false;
			elm.common.id = (jvxSize)elmIt_ep->instances.availableEndpoints.size();
			elm.common.tp = tp;
			elm.common.allowsMultiObjects = allowMultipleObjectsAdd;
			elm.common.hObject = newObjAdd;
			elm.common.hGlobInst = newGlobInstAdd;
			elm.common.linkage.packPtr = thePack;
			elm.common.linkage.packIdx = thePackIdx;

			elm.common.linkage.dllPath = fileName;
			elm.common.linkage.funcInit = funcInitAdd;
			elm.common.linkage.funcTerm = funcTermAdd;
			elm.common.linkage.sharedObjectEntry = dllHandle;

			elm.common.externalLink.description = "NONE";
			elm.common.dllProps = dllProps;

			elmIt_ep->instances.availableEndpoints.push_back(elm);
			res = JVX_NO_ERROR;
			numAdded++;
		}
		else
		{
			std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
			elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>,
				jvxComponentType>(_common_set_types.registeredSimpleTypes, tp, 0);

			if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
			{
				oneObj<IjvxSimpleComponent> elm;
				elm.theHandle_single = NULL;
				elm.theHandle_single = reinterpret_cast<IjvxSimpleComponent*>(specCompAdd);
				elm.common.isExternalComponent = false;
				elm.common.id = (jvxSize)elmIt_si->instances.availableEndpoints.size();
				elm.common.tp = tp;
				elm.common.allowsMultiObjects = allowMultipleObjectsAdd;
				elm.common.hObject = newObjAdd;
				elm.common.hGlobInst = newGlobInstAdd;

				elm.common.linkage.dllPath = fileName;
				elm.common.linkage.funcInit = funcInitAdd;
				elm.common.linkage.funcTerm = funcTermAdd;
				elm.common.linkage.sharedObjectEntry = dllHandle;

				elm.common.externalLink.description = "NONE";
				elm.common.dllProps = dllProps;

				elm.common.linkage.packPtr = thePack;
				elm.common.linkage.packIdx = thePackIdx;
				elmIt_si->instances.availableEndpoints.push_back(elm);
				res = JVX_NO_ERROR;
				numAdded++;
			}
		}
	}
	return res;
}

void 
CjvxHostTypeHandler::fwd_pre_unload_all_components()
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;
	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		std::vector<oneTechnology> remainListT;
		for (j = 0; j < _common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size(); j++)
		{
			if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.packPtr)
			{
				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.funcTerm(
					_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.hObject);
				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.hObject = NULL;
				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].theHandle_single = NULL;

				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.packPtr->release_entries_component(
					_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.packIdx);
				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.packPtr = nullptr;
				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.packIdx = JVX_SIZE_UNSELECTED;
				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.sharedObjectEntry = JVX_HMODULE_INVALID;
			}
		}
	}

	t_pre_unload_dlls<IjvxNode>(_common_set_types.registeredNodeTypes);
	t_pre_unload_dlls<IjvxSimpleComponent>(_common_set_types.registeredSimpleTypes);
}

void
CjvxHostTypeHandler::fwd_unload_all_components()
{
	jvxSize i, j;
	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		std::vector<oneTechnology> remainListT;
		for (j = 0; j < _common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size(); j++)
		{

			if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.isExternalComponent)
			{
				remainListT.push_back(_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j]);
			}
			else
			{
				if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.hObject)
				{
					// In all other cases, unload library and delete object
					_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.funcTerm(
						_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.hObject);
					_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.hObject = NULL;
					_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].theHandle_single = NULL;
					_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.funcInit = NULL;
					_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.funcTerm = NULL;
					if (!(JVX_EVALUATE_BITFIELD(_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.dllProps & JVX_BITFIELD_DLL_PROPERTY_DO_NOT_UNLOAD)))
					{
						JVX_UNLOADLIBRARY(_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].common.linkage.sharedObjectEntry);
					}
				}
			}
		}
		_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.clear();
		_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies = remainListT;
	}

	t_unload_dlls<IjvxNode>(_common_set_types.registeredNodeTypes);
	t_unload_dlls<IjvxSimpleComponent>(_common_set_types.registeredSimpleTypes);
}
// ===================================================================================
jvxErrorType
CjvxHostTypeHandler::_component_class(jvxComponentType tp, jvxComponentTypeClass& tpCls)
{
	jvxErrorType res = JVX_NO_ERROR;
	tpCls = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;

	// First some fixed types
	switch (tp)
	{
	case JVX_COMPONENT_HOST:
	case JVX_COMPONENT_MIN_HOST:
	case JVX_COMPONENT_OFF_HOST:
	case JVX_COMPONENT_FACTORY_HOST:
		tpCls = jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST;
		return JVX_NO_ERROR;
	default:
		break;
	}

	// Here, only return the registered types
	auto elm_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp, 0);
	if (elm_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		tpCls = elm_tech->classType;
		return res;
	}

	auto elm_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp, 1);
	if (elm_dev != _common_set_types.registeredTechnologyTypes.end())
	{
		tpCls = jvxComponentTypeClass::JVX_COMPONENT_TYPE_DEVICE;
		return res;
	}

	auto elm_node = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp, 0);
	if (elm_node != _common_set_types.registeredNodeTypes.end())
	{
		tpCls = elm_node->classType;
		return res;
	}

	auto elm_simple= jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp, 0);
	if (elm_simple != _common_set_types.registeredSimpleTypes.end())
	{
		tpCls = elm_simple->classType;
		return res;
	}
	res = JVX_ERROR_ELEMENT_NOT_FOUND;
	return res;
}

jvxErrorType
CjvxHostTypeHandler::_add_type_host(IjvxObject* theObj, jvxComponentType* tp, jvxSize numtp, const char* description,
	const char* tokenConfig, 
	jvxComponentTypeClass classType, jvxSize numSlotsMax,
	jvxSize numSubSlotsMax, jvxComponentTypeClass childClassType)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState theState = JVX_STATE_NONE;
	theObj->state(&theState);
	std::vector<oneTechnologyType>::iterator elm_tech;
	std::vector<oneObjType<IjvxNode>>::iterator elm_end;
	std::vector<oneObjType<IjvxSimpleComponent>>::iterator elm_simple;
	oneTechnologyType newElm_tech;
	oneObjType<IjvxNode> newElm_end;
	oneObjType<IjvxSimpleComponent> newElm_simple;
	if (theState == JVX_STATE_SELECTED)
	{
		if (tp && (numtp >= 1))
		{
			elm_tech = _common_set_types.registeredTechnologyTypes.begin();
			elm_end = _common_set_types.registeredNodeTypes.begin();
			elm_simple = _common_set_types.registeredSimpleTypes.begin();

			elm_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp[0], 0);
			elm_end = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp[0], 0);
			elm_simple = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp[0], 0);

			if ((elm_tech == _common_set_types.registeredTechnologyTypes.end()) && (elm_end == _common_set_types.registeredNodeTypes.end()))
			{
				switch (classType)
				{
				case jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY:
					if (numtp >= 2)
					{
						newElm_tech.classType = classType;
						newElm_tech.childClassType = childClassType;
						newElm_tech.description = description;
						newElm_tech.tokenInConfigFile = tokenConfig;
						newElm_tech.selector[0] = tp[0];
						newElm_tech.selector[1] = tp[1];
						assert(numtp >= 2);
						//newElm_tech.technologyInstances.selTech.theHandle_shortcut_dev = NULL;
						//newElm_tech.technologyInstances.selTech.theHandle_shortcut_tech = NULL;
						newElm_tech.technologyInstances.numSlotsMax = numSlotsMax;
						newElm_tech.technologyInstances.numSubSlotsMaxDefault = numSubSlotsMax;

						//newElm_tech.technologyInstances.selTech.numSubSlotsMax = numSubSlotsMax;
						_common_set_types.registeredTechnologyTypes.push_back(newElm_tech);
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
					break;
				case jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE:

					newElm_end.classType = classType;
					newElm_end.description = description;
					newElm_end.tokenInConfigFile = tokenConfig;
					newElm_end.selector[0] = tp[0];
					assert(numtp >= 1);
					//newElm_end.nodeInstances.selNode.theHandle_shortcut = NULL;
					newElm_end.instances.numSlotsMax = numSlotsMax;
					_common_set_types.registeredNodeTypes.push_back(newElm_end);
					break;

				case jvxComponentTypeClass::JVX_COMPONENT_TYPE_SIMPLE:

					newElm_simple.classType = classType;					
					newElm_simple.description = description;
					newElm_simple.tokenInConfigFile = tokenConfig;
					newElm_simple.selector[0] = tp[0];
					assert(numtp >= 1);
					//newElm_end.nodeInstances.selNode.theHandle_shortcut = NULL;
					newElm_simple.instances.numSlotsMax = numSlotsMax;
					_common_set_types.registeredSimpleTypes.push_back(newElm_simple);
					break;
				default:
					res = JVX_ERROR_UNSUPPORTED;
				}
			}
			else
			{
				res = JVX_ERROR_DUPLICATE_ENTRY;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);

}

jvxErrorType
CjvxHostTypeHandler::_add_numsubslots_type_host(const jvxComponentIdentification& tp,
	jvxSize numSubSlotsMax)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elm_tech = _common_set_types.registeredTechnologyTypes.begin();
	elm_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
	if (elm_tech != _common_set_types.registeredTechnologyTypes.end())
	{
		res = JVX_ERROR_DUPLICATE_ENTRY;
		auto elm = elm_tech->technologyInstances.numSubSlotsMax_spec.find(tp.slotid);
		if (elm == elm_tech->technologyInstances.numSubSlotsMax_spec.end())
		{
			elm_tech->technologyInstances.numSubSlotsMax_spec[tp.slotid] = numSubSlotsMax;
			res = JVX_NO_ERROR;
		}
	}
	return res;
}

jvxErrorType
CjvxHostTypeHandler::_number_types_host(jvxSize* num, jvxComponentTypeClass classType)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (num)
	{
		switch (classType)
		{
		case jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY:
			*num = _common_set_types.registeredTechnologyTypes.size();
			break;
		case jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE:
			*num = _common_set_types.registeredNodeTypes.size();
			break;
		case jvxComponentTypeClass::JVX_COMPONENT_TYPE_SIMPLE:
			*num = _common_set_types.registeredSimpleTypes.size();
			break;
		default:
			res = JVX_ERROR_UNSUPPORTED;
		}
	}
	return(res);
}

jvxErrorType
CjvxHostTypeHandler::_description_type_host(jvxSize idx, jvxApiString* theDescription, jvxApiString* theTokenInConfig, jvxComponentType* tp, jvxSize numtp, jvxComponentTypeClass classType)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (classType)
	{
	case jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY:
		if (idx < _common_set_types.registeredTechnologyTypes.size())
		{
			if (theDescription)
			{
				theDescription->assign(_common_set_types.registeredTechnologyTypes[idx].description);
			}
			if (theTokenInConfig)
			{
				theTokenInConfig->assign(_common_set_types.registeredTechnologyTypes[idx].tokenInConfigFile);
			}
			if (tp)
			{
				if (numtp >= 1)
				{
					tp[0] = _common_set_types.registeredTechnologyTypes[idx].selector[0];
				}
				if (numtp >= 2)
				{
					tp[1] = _common_set_types.registeredTechnologyTypes[idx].selector[1];
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		break;
	case jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE:
		if (idx < _common_set_types.registeredNodeTypes.size())
		{
			if (theDescription)
			{
				theDescription->assign(_common_set_types.registeredNodeTypes[idx].description);
			}
			if (theTokenInConfig)
			{
				theTokenInConfig->assign(_common_set_types.registeredNodeTypes[idx].tokenInConfigFile);
			}
			if (tp)
			{
				if (numtp >= 1)
				{
					tp[0] = _common_set_types.registeredNodeTypes[idx].selector[0];
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		break;

	case jvxComponentTypeClass::JVX_COMPONENT_TYPE_SIMPLE:
		if (idx < _common_set_types.registeredSimpleTypes.size())
		{
			if (theDescription)
			{
				theDescription->assign(_common_set_types.registeredSimpleTypes[idx].description);
			}
			if (theTokenInConfig)
			{
				theTokenInConfig->assign(_common_set_types.registeredSimpleTypes[idx].tokenInConfigFile);
			}
			if (tp)
			{
				if (numtp >= 1)
				{
					tp[0] = _common_set_types.registeredSimpleTypes[idx].selector[0];
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
}

jvxErrorType
CjvxHostTypeHandler::_remove_all_host_types()
{
	_common_set_types.registeredTechnologyTypes.clear();
	_common_set_types.registeredNodeTypes.clear();
	_common_set_types.registeredSimpleTypes.clear();
	return(JVX_NO_ERROR);
}

