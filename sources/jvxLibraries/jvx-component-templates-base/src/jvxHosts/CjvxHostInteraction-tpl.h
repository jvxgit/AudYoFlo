template <class T> void
CjvxHostInteraction::t_remove_external_component(IjvxObject* theObj, std::vector<oneObjType<T>>& registeredObj)
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

			auto elm = staticModules.find(theObj);
			if (elm != staticModules.end())
			{
				staticModules.erase(elm);
			}
		}
	}
}

template <class T> jvxErrorType
CjvxHostInteraction::t_add_external_component(IjvxObject* theObj,
	IjvxGlobalInstance* theGlob, const char* locationDescription, jvxBool allowMultipleInstance,
	jvxInitObject_tp funcInit, jvxTerminateObject_tp funcTerm,
	std::vector<oneObjType<T>>& registeredObj, const jvxComponentIdentification& tp)
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
					staticModules[theObj] = mName;
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
CjvxHostInteraction::t_pre_unload_dlls(std::vector<oneObjType<T>>& registeredObj)
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
CjvxHostInteraction::t_unload_dlls(std::vector<oneObjType<T>>& registeredObj)
{
	jvxSize i,j;
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