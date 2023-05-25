#include "jvxHosts/CjvxHostInteraction.h"
#include "jvxHosts/CjvxHostInteraction-tpl.h"

CjvxHostInteraction::CjvxHostInteraction()
{
	_common_set_host.reportUnit = NULL;
	_common_set_host.reportOnConfig = NULL;
	_common_set_host.threadId_registered = JVX_NULLTHREAD;
	_common_set_host.loadFilter.theCallback = NULL;
	_common_set_host.loadFilter.theCallback_priv = NULL;
	config.minHostFunctionality = false;
}

CjvxHostInteraction::~CjvxHostInteraction()
{
	assert(_common_set_host.registeredStateSwitchHandlers.size() == 0);
}

jvxErrorType
CjvxHostInteraction::_add_external_component(CjvxObject* meObj,
	IjvxObject* theObj, IjvxGlobalInstance* theGlob, const char* locationDescription, jvxBool allowMultipleInstance,
	jvxInitObject_tp funcInit, jvxTerminateObject_tp funcTerm)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;
	meObj->_state( &stat);
	if (stat == JVX_STATE_SELECTED)
	{
		if (theObj)
		{
			jvxComponentIdentification tp = JVX_COMPONENT_UNKNOWN;
			jvxComponentIdentification tpRet = JVX_COMPONENT_UNKNOWN;
			jvxBool allowsMultiObjects = false;
			jvxHandle* specComp = NULL;
			jvxApiString astr;
			std::string mName;
			jvxBool moduleRegisteredBefore = false;

			theObj->module_reference(&astr, NULL);
			mName = astr.std_str();

			std::cout << "requested to add external component " << locationDescription << std::endl;
			theObj->request_specialization(&specComp, &tp, &allowsMultiObjects);
			std::cout << "  - component type is <" << jvxComponentType_txt(tp.tp) << ">, module name is <" << mName << ">." << std::endl;

			// CHeck if module has been added before
			auto elm = staticModules.begin();
			for (; elm != staticModules.end(); elm++)
			{
				if (elm->second == mName)
				{
					moduleRegisteredBefore = true;
					break;
				}
			}
			if (moduleRegisteredBefore)
			{
				std::cout << "  - module was added before, therefore skipping current request." << std::endl;
				return JVX_ERROR_DUPLICATE_ENTRY;
			}
			
			// Overwrite what comes from dll
			if (allowMultipleInstance == false)
			{
				allowsMultiObjects = false;
			}

			if (config.minHostFunctionality)
			{
				// All other components
				jvxOneComponentModule elm;

				elm.theObj_single = NULL;
				elm.theObj_single = theObj;
				elm.theGlob_single = theGlob;
				elm.common.isExternalComponent = true;
				elm.common.id = (jvxSize)_common_set_host.otherComponents.availableOtherComponents.size();
				elm.common.linkage.dllPath = "NONE";
				elm.common.linkage.sharedObjectEntry = NULL;
				elm.common.linkage.funcInit = funcInit;
				elm.common.linkage.funcTerm = funcTerm;				
				elm.common.tp = tp.tp;
				elm.common.allowsMultiObjects = allowsMultiObjects;
				elm.common.externalLink.description = locationDescription;
				elm.common.dllProps = 0;
				elm.common.hObject = theObj;
				elm.common.hGlobInst = theGlob;

				_common_set_host.otherComponents.availableOtherComponents.push_back(elm);
				staticModules[theObj] = mName;
				std::cout << "  - added" << std::endl;
			}
			else
			{

				std::vector<CjvxHostInteraction::oneTechnologyType>::iterator elmIt_tech;
				elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);

				if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
				{
					oneTechnology elm;
					elm.theHandle_single = NULL;
					IjvxTechnology* theTech = NULL;
					res = theObj->request_specialization(reinterpret_cast<jvxHandle**>(&theTech), &tpRet, &allowsMultiObjects);
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
								elm.common.tp = tp.tp;
								elm.common.allowsMultiObjects = allowsMultiObjects;
								elm.common.externalLink.description = locationDescription;
								elm.common.hObject = theObj;
								elm.common.hGlobInst = theGlob;
								elm.common.dllProps = 0;

								elmIt_tech->technologyInstances.availableTechnologies.push_back(elm);
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
					else
					{
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}
				}
				else
				{
					jvxErrorType resL = t_add_external_component<IjvxNode>(theObj, 
						theGlob, locationDescription, allowMultipleInstance,
						funcInit, funcTerm,
						_common_set_types.registeredNodeTypes, tp);
					if(resL == JVX_ERROR_ELEMENT_NOT_FOUND)
					{
						resL = t_add_external_component<IjvxSimpleComponent>(theObj, 
							theGlob, locationDescription, allowMultipleInstance,
							funcInit, funcTerm,
							_common_set_types.registeredSimpleTypes, tp);
						if (resL == JVX_ERROR_ELEMENT_NOT_FOUND)
						{
							// All other components
							jvxOneComponentModule elm;

							elm.theObj_single = NULL;
							elm.theObj_single = theObj;
							elm.theGlob_single = theGlob;
							elm.common.isExternalComponent = true;
							elm.common.id = (jvxSize)_common_set_host.otherComponents.availableOtherComponents.size();
							elm.common.linkage.dllPath = "NONE";
							elm.common.linkage.sharedObjectEntry = NULL;
							elm.common.linkage.funcInit = funcInit;
							elm.common.linkage.funcTerm = funcTerm;
							elm.common.tp = tp.tp;
							elm.common.allowsMultiObjects = allowsMultiObjects;
							elm.common.externalLink.description = locationDescription;
							elm.common.dllProps = 0;
							elm.common.hObject = theObj;
							elm.common.hGlobInst = theGlob;

							_common_set_host.otherComponents.availableOtherComponents.push_back(elm);
							staticModules[theObj] = mName;
							std::cout << "  - added" << std::endl;
						}
					}
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
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxHostInteraction::_remove_external_component(CjvxObject* meObj, IjvxObject* theObj)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;
	meObj->_state(&stat);
	if (stat == JVX_STATE_SELECTED)
	{
		if (!config.minHostFunctionality)
		{
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

			t_remove_external_component<IjvxNode>(theObj, _common_set_types.registeredNodeTypes);
			t_remove_external_component<IjvxSimpleComponent>(theObj, _common_set_types.registeredSimpleTypes);
			
		}

		std::vector<jvxOneComponentModule>::iterator elm = this->_common_set_host.otherComponents.availableOtherComponents.begin();
		for (; elm != this->_common_set_host.otherComponents.availableOtherComponents.end(); elm++)
		{
			if (elm->common.hObject == theObj)
			{
				break;
			}
		}
		if (elm != this->_common_set_host.otherComponents.availableOtherComponents.end())
		{
			this->_common_set_host.otherComponents.availableOtherComponents.erase(elm);
			
			auto elm = staticModules.find(theObj);
			if (elm != staticModules.end())
			{
				staticModules.erase(elm);
			}
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxHostInteraction::_set_external_report_target(CjvxObject* meObj, IjvxReport* theHdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;
	jvxCallManager callGate;
	meObj->_state(&stat);
	if (stat == JVX_STATE_SELECTED)
	{
		_common_set_host.reportUnit = theHdl;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxHostInteraction::_set_external_report_on_config(CjvxObject* meObj, IjvxReportOnConfig* theHdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;

	meObj->_state( &stat);
	if (stat == JVX_STATE_SELECTED)
	{
		_common_set_host.reportOnConfig = theHdl;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxHostInteraction::_add_external_report_state_switch(IjvxReportStateSwitch* theHdl, const char* tag)
{
	jvxErrorType res = JVX_NO_ERROR;
	oneEntryReportStateSwitch newElm;
	newElm.ptrReport = theHdl;
	newElm.tag = tag;
	auto elm = _common_set_host.registeredStateSwitchHandlers.find(theHdl);
	if (elm == _common_set_host.registeredStateSwitchHandlers.end())
	{
		_common_set_host.registeredStateSwitchHandlers[theHdl] = newElm;
	}
	else
	{
		res = JVX_ERROR_DUPLICATE_ENTRY;
	}
	return(res);
}

jvxErrorType
CjvxHostInteraction::_remove_external_report_state_switch(IjvxReportStateSwitch* theHdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	auto elm = _common_set_host.registeredStateSwitchHandlers.find(theHdl);
	if(elm != _common_set_host.registeredStateSwitchHandlers.end())
	{
		_common_set_host.registeredStateSwitchHandlers.erase(elm);	
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return(res);
}

jvxErrorType
CjvxHostInteraction::_add_external_interface(CjvxObject* meObj, jvxHandle* theHdl, jvxInterfaceType theIFacetype)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i = 0;
	std::vector<oneInterface>::iterator elmIt_ep;

	if(!_common_set_host.bootComplete)
	{
	/*
	jvxState stat = JVX_STATE_NONE;
	meObj->_state( &stat);
	if (stat == JVX_STATE_SELECTED)
	{
	*/
		elmIt_ep = jvx_findItemSelectorInList_one<oneInterface, jvxInterfaceType>(_common_set_host.externalInterfaces, theIFacetype, 0);
		if (elmIt_ep == _common_set_host.externalInterfaces.end())
		{
			oneInterface newI;
			newI.selector[0] = theIFacetype;
			newI.theHdl = theHdl;
			_common_set_host.externalInterfaces.push_back(newI);

		}
		else
		{
			res = JVX_ERROR_DUPLICATE_ENTRY;
		}
	
	 }
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	
	return(res);
}

jvxErrorType
CjvxHostInteraction::_remove_external_interface(CjvxObject* meObj, jvxHandle* theHdl, jvxInterfaceType theIFacetype)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i = 0;
	std::vector<oneInterface>::iterator elmIt_ep;
	jvxState stat = JVX_STATE_NONE;

	/*
	meObj->_state(&stat);
	if (stat == JVX_STATE_SELECTED)
	{
	*/
	if (!_common_set_host.bootComplete)
	{
		elmIt_ep = jvx_findItemSelectorInList_one<oneInterface, jvxInterfaceType>(_common_set_host.externalInterfaces, theIFacetype, 0);
		if (elmIt_ep == _common_set_host.externalInterfaces.end())
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;

		}
		else
		{
			if (elmIt_ep->theHdl != theHdl)
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			else
			{
				_common_set_host.externalInterfaces.erase(elmIt_ep);
			}
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxHostInteraction::_add_component_load_blacklist(CjvxObject* meObj, jvxComponentType theTp, jvxBool targetBlackList)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i = 0;
	std::vector<oneBlacklistEntry>::iterator elmIt_ep;

	jvxState stat = JVX_STATE_NONE;
	meObj->_state(&stat);
	if (stat == JVX_STATE_SELECTED)
	{
		if (targetBlackList)
		{
			elmIt_ep = jvx_findItemSelectorInList_one<oneBlacklistEntry, jvxComponentType>(_common_set_host.blacklist_load, theTp, 0);
			if (elmIt_ep == _common_set_host.blacklist_load.end())
			{
				oneBlacklistEntry newE;
				newE.selector[0] = theTp;
				_common_set_host.blacklist_load.push_back(newE);
			}
			else
			{
				res = JVX_ERROR_DUPLICATE_ENTRY;
			}
		}
		else
		{
			elmIt_ep = jvx_findItemSelectorInList_one<oneBlacklistEntry, jvxComponentType>(_common_set_host.inv_blacklist_load, theTp, 0);
			if (elmIt_ep == _common_set_host.inv_blacklist_load.end())
			{
				oneBlacklistEntry newE;
				newE.selector[0] = theTp;
				_common_set_host.inv_blacklist_load.push_back(newE);
			}
			else
			{
				res = JVX_ERROR_DUPLICATE_ENTRY;
			}

		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxHostInteraction::_remove_component_load_blacklist(CjvxObject* meObj, jvxComponentType theTp, jvxBool targetBlackList)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i = 0;
	std::vector<oneBlacklistEntry>::iterator elmIt_ep;
	jvxState stat = JVX_STATE_NONE;
	meObj->_state( &stat);
	if (stat == JVX_STATE_SELECTED)
	{
		if (targetBlackList)
		{
			elmIt_ep = jvx_findItemSelectorInList_one<oneBlacklistEntry, jvxComponentType>(_common_set_host.blacklist_load, theTp, 0);
			if (elmIt_ep == _common_set_host.blacklist_load.end())
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			else
			{
				_common_set_host.blacklist_load.erase(elmIt_ep);
			}
		}
		else
		{
			elmIt_ep = jvx_findItemSelectorInList_one<oneBlacklistEntry, jvxComponentType>(_common_set_host.inv_blacklist_load, theTp, 0);
			if (elmIt_ep == _common_set_host.inv_blacklist_load.end())
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			else
			{
				_common_set_host.inv_blacklist_load.erase(elmIt_ep);
			}
		}

	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

// ================================================================================
// Helper function
// ================================================================================

jvxErrorType
CjvxHostInteraction::loadAllComponents(jvxBool do_unload_dlls, std::vector<std::string>& component_path, jvxBool info_cout, jvxBool verbose_out)
{
	jvxSize i, ii;
	JVX_HANDLE_DIR searchHandle;
	JVX_DIR_FIND_DATA searchResult = INIT_SEARCH_DIR_DEFAULT;;
	JVX_HMODULE dllHandle;
	std::string fileName;
	std::string outTxt;

	// Copy the blacklist config 
	_common_set_host.blacklist_load_use = _common_set_host.blacklist_load;

	/*
	// Add the system components. These are not allowed to be taken from DLL
	for (i = JVX_MAIN_COMPONENT_LIMIT; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		jvxBool foundit = false;
		for (auto elm : _common_set_host.blacklist_load_use)
		{
			if (elm.selector[0] == (jvxComponentType)i)
			{
				foundit = true;
				break;
			}
		}
		if (!foundit)
		{
			CjvxHostInteraction::oneBlacklistEntry bl;
			bl.selector[0] = (jvxComponentType)i;
		}
	}
	*/

	/* Handle blacklist*/
	if (!_common_set_host.inv_blacklist_load.empty())
	{
		// Copy all but the entries in inv blacklist in addition to the given blacklist entries
		for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
		{
			jvxComponentType tp = (jvxComponentType)i;
			std::vector<oneBlacklistEntry>::iterator elmIt_ep;
			elmIt_ep = jvx_findItemSelectorInList_one<oneBlacklistEntry, jvxComponentType>(_common_set_host.inv_blacklist_load, tp, 0);
			if (elmIt_ep == _common_set_host.inv_blacklist_load.end())
			{
				// Component not on the list
				elmIt_ep = jvx_findItemSelectorInList_one<oneBlacklistEntry, jvxComponentType>(_common_set_host.blacklist_load_use, tp, 0);
				if (elmIt_ep == _common_set_host.blacklist_load_use.end())
				{
					oneBlacklistEntry newE;
					newE.selector[0] = tp;
					_common_set_host.blacklist_load_use.push_back(newE);
				}
			}
		}
	}

	if (verbose_out)
	{
		outTxt = "-- Used shared lib blacklist --";
		if (info_cout)
		{
			std::cout << outTxt << std::endl;
		}
		else
		{
			if (_common_set_host.reportUnit)
			{
				_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
			}
		}
	}
	for (i = 0; i < _common_set_host.blacklist_load_use.size(); i++)
	{
		if (verbose_out)
		{
			outTxt = "--> Entry #" + jvx_size2String(i) + ": ";
			outTxt += jvxComponentType_txt(_common_set_host.blacklist_load_use[i].selector[0]);
			if (info_cout)
			{
				std::cout << outTxt << std::endl;
			}
			else
			{
				if (_common_set_host.reportUnit)
				{
					_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
				}
			}
		}
	}
	if (do_unload_dlls)
	{
		for (i = 0; i < component_path.size(); i++)
		{
			std::string pathName = component_path[i];

			// Search for directory
			searchHandle = JVX_FINDFIRSTFILE_WC(searchHandle, pathName.c_str(), JVX_DLL_EXTENSION, searchResult);
			bool result = true;
			bool foundElement = false;

			// If entry exists
			if (searchHandle != INVALID_HANDLE_VALUE)
			{
				// open DLL and test for technology or plugin
				do
				{
					fileName = pathName;
					fileName += JVX_SEPARATOR_DIR;
					fileName += JVX_GETFILENAMEDIR(searchResult);//searchResult.cFileName;

					result = false;
					foundElement = false;

					if (verbose_out)
					{
						outTxt = "found lib " + fileName;
						if (info_cout)
						{
							std::cout << outTxt << std::endl;
						}
						else
						{
							if (_common_set_host.reportUnit)
							{
								_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
							}
						}
					}
					dllHandle = JVX_LOADLIBRARY(fileName.c_str());

					// Open the DLL
					if (dllHandle)
					{
						IjvxObject* newObjEnter = NULL;
						IjvxGlobalInstance* newGlobInstEnter = NULL;
						jvxInitObject_tp funcInitEnter = NULL;
						jvxTerminateObject_tp funcTermEnter = NULL;
						jvxDllProperties_tp funcDllsProps = NULL;
						jvxComponentIdentification tpEnter = JVX_COMPONENT_UNKNOWN;
						jvxBool allowMultipleObjectsEnter = false;
						jvxHandle* specCompEnter = NULL;
						jvxBitField dllProps = 0;

						funcInitEnter = (jvxInitObject_tp)JVX_GETPROCADDRESS(dllHandle, JVX_OPEN_OBJECT_DLL);
						funcTermEnter = (jvxTerminateObject_tp)JVX_GETPROCADDRESS(dllHandle, JVX_CLOSE_OBJECT_DLL);
						funcDllsProps = (jvxDllProperties_tp)JVX_GETPROCADDRESS(dllHandle, JVX_PROPERTIES_DLL);

						if (funcInitEnter && funcTermEnter)
						{
							if (verbose_out)
							{
								outTxt = "  - loading lib " + fileName;
								if (info_cout)
								{
									std::cout << outTxt << std::endl;
								}
								else
								{
									if (_common_set_host.reportUnit)
									{
										_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), 
											JVX_REPORT_PRIORITY_INFO);
									}
								}
							}

							funcInitEnter(&newObjEnter, &newGlobInstEnter, NULL);
							if (newObjEnter)
							{
								funcDllsProps(&dllProps);
								newObjEnter->request_specialization(&specCompEnter, &tpEnter, &allowMultipleObjectsEnter);

								if (verbose_out)
								{
									if (info_cout)
									{
										std::cout << "  - component type is <" << jvxComponentType_txt(tpEnter.tp) << ">" << std::endl;
									}
									else
									{
										outTxt = "  - component type is <";
										outTxt += jvxComponentType_txt(tpEnter.tp);
										outTxt += ">";

										if (_common_set_host.reportUnit)
										{
											_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(),
												JVX_REPORT_PRIORITY_INFO);
										}
									}
								}
																
								// ==========================================================================
								// Check already added modules
								// ===========================================================================
								jvxBool wasAlreadyAdded = false;
								jvxApiString astr;
								jvxComponentAccessType acTp;

								// We check the module name of the 
								newObjEnter->module_reference(&astr, &acTp);
								auto elm = this->staticModules.begin();
								for (; elm != this->staticModules.end(); elm++)
								{
									if (astr.std_str() == elm->second)
									{
										wasAlreadyAdded = true;
										break;
									}
								}
								if (wasAlreadyAdded)
								{
									if (verbose_out)
									{
										if (info_cout)
										{
											std::cout << "  - component has already been added as static library." << std::endl;
										}
										else
										{
											outTxt = "  - component has already been added as static library.";
											if (_common_set_host.reportUnit)
											{
												_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
											}
										}
									}
									// Do not store this component type
									funcTermEnter(newObjEnter);
									JVX_UNLOADLIBRARY(dllHandle);
									if (info_cout)
									{
										std::cout << "  - Library <" << fileName << "> unloaded." << std::endl;
									}
									else
									{
										outTxt = "  - Library <" + fileName + "> unloaded.";
										if (_common_set_host.reportUnit)
										{
											_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
										}
									}
									continue;
								}

								// From here on we switch from module centric to component centric 
								jvxSize numTryAdd = 1;
								jvxSize numAdded = 0;

								IjvxPackage* thePack = nullptr;
								jvxComponentIdentification tpCheck = tpEnter;
								jvxBool isPackageMainComponent = true;
								IjvxObject* newObjAdd = newObjEnter;
								jvxInitObject_tp funcInitAdd = funcInitEnter;
								jvxTerminateObject_tp funcTermAdd = funcTermEnter;
								jvxHandle* specCompAdd = specCompEnter;
								IjvxGlobalInstance* newGlobInstAdd = newGlobInstEnter;
								jvxBool allowMultipleObjectsAdd = allowMultipleObjectsEnter;

								jvxApiString descriptionComponentPackage;
								thePack = reqInterfaceObj<IjvxPackage>(newObjEnter);
								if (thePack)
								{
									jvxSize numCompsPackage = 0;
									thePack->number_components(&numCompsPackage);
									numTryAdd = numCompsPackage + 1;										
								}

								for (int tlp = 0; tlp < numTryAdd; tlp++)
								{
									jvxSize thePackIdx = JVX_SIZE_UNSELECTED;
									isPackageMainComponent = (tlp == (numTryAdd - 1));

									if (thePack)
									{
										if (!isPackageMainComponent)
										{
											jvxApiString descrComp;
											newObjAdd = nullptr;
											funcInitAdd = nullptr;
											funcTermAdd = nullptr;
											specCompAdd = nullptr;
											jvxErrorType resL = thePack->request_entries_component(tlp, &descrComp,
												&funcInitAdd, &funcTermAdd);
											thePackIdx = tlp;
											if (resL == JVX_NO_ERROR)
											{
												funcInitAdd(&newObjAdd, &newGlobInstAdd, nullptr);
												if (newObjAdd)
												{
													newObjAdd->request_specialization(&specCompAdd, &tpCheck, &allowMultipleObjectsAdd);
													if (verbose_out)
													{
														if (info_cout)
														{
															std::cout << "  - opening object <" << descrComp.std_str() <<
																" from package <" << astr.std_str() << ">." << std::endl;
														}
														else
														{
															outTxt = "  - opening object <" + descrComp.std_str() +
																" from package <" + astr.std_str() + ">.";
															if (_common_set_host.reportUnit)
															{
																_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
															}
														}
													}
												}
											}
										}
										else
										{
											tpCheck = tpEnter;
											isPackageMainComponent = true;
											newObjAdd = newObjEnter;
											funcInitAdd = funcInitEnter;
											funcTermAdd = funcTermEnter;
											specCompAdd = specCompEnter;
											newGlobInstAdd = newGlobInstEnter;
											allowMultipleObjectsAdd = allowMultipleObjectsEnter;
											thePack = nullptr;
											thePackIdx = JVX_SIZE_UNSELECTED;
										}
									}

									jvxBool componentQualified = true;
									// jvxSize numberMayUnload = 0;

									// ================================================================================
									// Load Filter check
									// ================================================================================
									bool isOnBlacklist = false;
									for (ii = 0; ii < _common_set_host.blacklist_load_use.size(); ii++)
									{
										if (tpCheck == _common_set_host.blacklist_load_use[ii].selector[0])
										{
											isOnBlacklist = true;
											break;
										}
									}

									if (isOnBlacklist)
									{
										if (verbose_out)
										{
											if (info_cout)
											{
												std::cout << "  - blacklisted lib " << fileName << "." << std::endl;
											}
											else
											{
												outTxt = "  - blacklisted lib " + fileName + ".";
												if (_common_set_host.reportUnit)
												{
													_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
												}
											}
										}

										// This component is not in use
										componentQualified = false;
									}

									if (componentQualified)
									{
										// ================================================================================
										// Load Filter check
										// ================================================================================
										if (_common_set_host.loadFilter.theCallback)
										{
											jvxBool doNotLoad = false;
											_common_set_host.loadFilter.theCallback(&doNotLoad, fileName.c_str(), tpCheck.tp, _common_set_host.loadFilter.theCallback_priv);
											if (doNotLoad)
											{
												if (verbose_out)
												{
													if (info_cout)
													{
														std::cout << "  - load filter excluded lib " << fileName << "." << std::endl;
													}
													else
													{
														outTxt = "  - load filter excluded lib " + fileName + ".";
														if (_common_set_host.reportUnit)
														{
															_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
														}
													}
												}
												componentQualified = false;
											}
										}
									}

									// Now, add the component
									if (componentQualified)
									{
										if (specCompAdd)
										{
											if (config.minHostFunctionality)
											{
												jvxOneComponentModule elm;

												elm.common.isExternalComponent = false;
												elm.common.id = (jvxSize)_common_set_host.otherComponents.availableOtherComponents.size();
												elm.common.tp = tpCheck.tp;
												elm.common.allowsMultiObjects = allowMultipleObjectsAdd;
												elm.common.hObject = newObjAdd;
												elm.common.hGlobInst = newGlobInstAdd;

												elm.common.linkage.dllPath = fileName;
												elm.common.linkage.funcInit = funcInitAdd;
												elm.common.linkage.funcTerm = funcTermAdd;
												elm.common.linkage.sharedObjectEntry = dllHandle;
												elm.common.linkage.packPtr = thePack;
												elm.common.linkage.packIdx = thePackIdx;

												elm.common.externalLink.description = "NONE";
												elm.common.dllProps = dllProps;
												elm.theObj_single = newObjAdd;
												elm.theGlob_single = newGlobInstAdd;

												_common_set_host.otherComponents.availableOtherComponents.push_back(elm);
												numAdded++;
											}
											else
											{
												std::vector<oneTechnologyType>::iterator elmIt_tech;
												elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType,
													jvxComponentType>(_common_set_types.registeredTechnologyTypes, tpCheck.tp, 0);

												if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
												{
													oneTechnology elm;

													elm.theHandle_single = NULL;
													elm.theHandle_single = reinterpret_cast<IjvxTechnology*>(specCompAdd);
													elm.common.isExternalComponent = false;
													elm.common.id = (jvxSize)elmIt_tech->technologyInstances.availableTechnologies.size();
													elm.common.tp = tpCheck.tp;
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
													numAdded++;
												}
												else
												{
													std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
													elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>,
														jvxComponentType>(_common_set_types.registeredNodeTypes, tpCheck.tp, 0);

													if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
													{
														oneObj<IjvxNode> elm;
														elm.theHandle_single = NULL;
														elm.theHandle_single = reinterpret_cast<IjvxNode*>(specCompAdd);
														elm.common.isExternalComponent = false;
														elm.common.id = (jvxSize)elmIt_ep->instances.availableEndpoints.size();
														elm.common.tp = tpCheck.tp;
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
														numAdded++;
													}
													else
													{
														std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
														elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>,
															jvxComponentType>(_common_set_types.registeredSimpleTypes, tpCheck.tp, 0);

														if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
														{
															oneObj<IjvxSimpleComponent> elm;
															elm.theHandle_single = NULL;
															elm.theHandle_single = reinterpret_cast<IjvxSimpleComponent*>(specCompAdd);
															elm.common.isExternalComponent = false;
															elm.common.id = (jvxSize)elmIt_si->instances.availableEndpoints.size();
															elm.common.tp = tpCheck.tp;
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
															numAdded++;
														}
														else
														{
															jvxOneComponentModule elm;

															elm.theObj_single = NULL;
															elm.theObj_single = newObjAdd;
															elm.theGlob_single = newGlobInstAdd;
															elm.common.isExternalComponent = false;
															elm.common.id = (jvxSize)_common_set_host.otherComponents.availableOtherComponents.size();
															elm.common.tp = tpCheck.tp;
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
															_common_set_host.otherComponents.availableOtherComponents.push_back(elm);
															numAdded++;
														}
													}
												}
											}
											if (isPackageMainComponent)
											{
												if (verbose_out)
												{
													if (info_cout)
													{
														std::cout << "  - added lib " << fileName << std::endl;;
													}
													else
													{
														outTxt = "  - added lib " + fileName;
														if (_common_set_host.reportUnit)
														{
															_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
														}
													}
												}
											}
										}//if(specComp)
									}
									if (isPackageMainComponent)
									{
										if (numAdded == 0)
										{
											// No component added, unload the DLL
											funcTermEnter(newObjEnter);
											JVX_UNLOADLIBRARY(dllHandle);

											if (info_cout)
											{
												std::cout << "  - Library <" << fileName << "> unloaded." << std::endl;
											}
											else
											{
												outTxt = "  - Library <" + fileName + "> unloaded.";
												if (_common_set_host.reportUnit)
												{
													_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
												}
											}
										}
									}
									
									// if (componentQualified)
								} // for (int tlp = 0; tlp < numTryAdd; tlp++)
							}//if (newObjEnter)
							else
							{
								// No object returned
							}
						}//if(funcInit && funcTerm)
						else
						{
							if (verbose_out)
							{
								if (info_cout)
								{
									std::cout << "! invalid lib " << fileName << std::endl;
								}
								else
								{
									outTxt = "! invalid lib " + fileName;
									if (_common_set_host.reportUnit)
									{
										_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
									}
								}
							}
						}
					} //if ((dllHandle = JVX_LOADLIBRARY( fileName.c_str())))
					else
					{
						if (verbose_out)
						{
							if (info_cout)
							{
								std::cout << "! error opening lib " << fileName << std::endl;
							}
							else
							{
								outTxt = "! error opening lib " + fileName;
								if (_common_set_host.reportUnit)
								{
									_common_set_host.reportUnit->report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_INFO);
								}
							}
						}
					}
				} while (JVX_FINDNEXTFILE(searchHandle, searchResult, JVX_DLL_EXTENSION));//FindNextFile( searchHandle, &searchResult )
				JVX_FINDCLOSE(searchHandle);//FindClose( searchHandle );
			}//if (searchHandle != INVALID_HANDLE_VALUE )
		} // for (i = 0; i < component_path.size(); i++)
	}
	else
	{
	assert(0);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxHostInteraction::pre_unloadAllComponents()
{
	jvxSize i;
	std::list<JVX_HMODULE> unloadLibs;

	if (!config.minHostFunctionality)
	{
		jvxSize j;

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

	std::vector<jvxOneComponentModule> remainListO;
	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.packPtr)
		{
			this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm(
				this->_common_set_host.otherComponents.availableOtherComponents[i].common.hObject);
			this->_common_set_host.otherComponents.availableOtherComponents[i].common.hObject = NULL;
			this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single = NULL;
			this->_common_set_host.otherComponents.availableOtherComponents[i].theGlob_single = NULL;
			this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.packPtr->release_entries_component(
				this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.packIdx);
			this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.packPtr = nullptr;
			this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.packIdx = JVX_SIZE_UNSELECTED;
			this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.sharedObjectEntry = JVX_HMODULE_INVALID;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostInteraction::unloadAllComponents()
{
	jvxSize i;
	if (!config.minHostFunctionality)
	{
		jvxSize j;
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

	std::vector<jvxOneComponentModule> remainListO;
	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.isExternalComponent)
		{
			remainListO.push_back(this->_common_set_host.otherComponents.availableOtherComponents[i]);
		}
		else
		{
			if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.hObject)
			{
				// In all other cases, unload library and delete object
				this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm(
					this->_common_set_host.otherComponents.availableOtherComponents[i].common.hObject);
				this->_common_set_host.otherComponents.availableOtherComponents[i].common.hObject = NULL;
				this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single = NULL;
				this->_common_set_host.otherComponents.availableOtherComponents[i].theGlob_single = NULL;
				this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit = NULL;
				this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm = NULL;
				if (!(JVX_EVALUATE_BITFIELD(this->_common_set_host.otherComponents.availableOtherComponents[i].common.dllProps & JVX_BITFIELD_DLL_PROPERTY_DO_NOT_UNLOAD)))
				{
					JVX_UNLOADLIBRARY(this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.sharedObjectEntry);
				}
			}
		}
	}
	this->_common_set_host.otherComponents.availableOtherComponents.clear();
	this->_common_set_host.otherComponents.availableOtherComponents = remainListO;


	_common_set_host.blacklist_load_use.clear();
	return(JVX_NO_ERROR);
}

//virtual jvxErrorType removeAllRemainingComponents();

void
CjvxHostInteraction::reset()
{
	_common_set_host.reportUnit = NULL;
	_common_set_host.doUnload = false;
};

jvxErrorType
CjvxHostInteraction::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	std::vector<oneInterface>::iterator elmIt_ep;
	elmIt_ep = jvx_findItemSelectorInList_one<oneInterface, jvxInterfaceType>(_common_set_host.externalInterfaces, tp, 0);
	if (elmIt_ep != _common_set_host.externalInterfaces.end())
	{
		if (hdl)
		{
			*hdl = elmIt_ep->theHdl;
		}
	}
	else
	{
		switch (tp)
		{
		case JVX_INTERFACE_REPORT:
			if (hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(_common_set_host.reportUnit);
			}
			break;
		default:

			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	return(res);
};

jvxErrorType
CjvxHostInteraction::_set_component_load_filter_function(jvxLoadModuleFilterCallback regme, jvxHandle* priv)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (regme)
	{
		if (_common_set_host.loadFilter.theCallback == NULL)
		{
			_common_set_host.loadFilter.theCallback = regme;
			_common_set_host.loadFilter.theCallback_priv = priv;
		}
		else
			res = JVX_ERROR_ALREADY_IN_USE;
	}
	else
	{
		_common_set_host.loadFilter.theCallback = NULL;
		_common_set_host.loadFilter.theCallback_priv = NULL;
	}
	return res;
};

jvxErrorType
CjvxHostInteraction::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	std::vector<oneInterface>::iterator elmIt_ep;
	elmIt_ep = jvx_findItemSelectorInList_one<oneInterface, jvxInterfaceType>(_common_set_host.externalInterfaces, tp, 0);
	if (elmIt_ep != _common_set_host.externalInterfaces.end())
	{
		if (hdl == elmIt_ep->theHdl)
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		switch (tp)
		{
		case JVX_INTERFACE_REPORT:
			if (hdl == _common_set_host.reportUnit)
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;
		default:
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	return res;
}

jvxErrorType
CjvxHostInteraction::_store_config(const char* token, const char* cfgToken, jvxBool overwrite_old)
{
	std::map < std::string, std::string>::iterator elm = _common_set_host.storedConfigs.find(token);
	if (elm != _common_set_host.storedConfigs.end())
	{
		if (overwrite_old)
		{
			_common_set_host.storedConfigs.erase(elm);
		}
		else
		{
			return JVX_ERROR_DUPLICATE_ENTRY;
		}
	}
	_common_set_host.storedConfigs[token] = cfgToken;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostInteraction::_copy_config(const char* token, jvxApiString* strReturn)
{
	std::map < std::string, std::string>::iterator elm =
		_common_set_host.storedConfigs.find(token);
	if (elm != _common_set_host.storedConfigs.end())
	{
		if (strReturn)
		{
			strReturn->assign(elm->second);
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;

}

jvxErrorType
CjvxHostInteraction::_clear_config(const char* token)
{
	std::map < std::string, std::string>::iterator elm = _common_set_host.storedConfigs.find(token);
	if (elm != _common_set_host.storedConfigs.end())
	{
		_common_set_host.storedConfigs.erase(elm);
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxHostInteraction::_request_id_main_thread(JVX_THREAD_ID* thread_id)
{
	if (thread_id)
	{
		*thread_id = _common_set_host.threadId_registered;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostInteraction::_report_boot_complete(jvxBool isComplete)
{
	_common_set_host.bootComplete = isComplete;
	return JVX_NO_ERROR;
}