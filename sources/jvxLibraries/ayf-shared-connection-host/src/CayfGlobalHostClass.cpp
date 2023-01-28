#include "CayfGlobalHostClass.h"
#include "CjvxHostJsonDefines.h"

#include "jvxTconfigProcessor.h"

static std::string combineWithCwd(const std::string& fName)
{
	std::string fNameRet;
	char wDir[JVX_MAXSTRING] = { 0 };
	JVX_GETCURRENTDIRECTORY(wDir, JVX_MAXSTRING);
	fNameRet = wDir;
	fNameRet += JVX_SEPARATOR_DIR + fName;
	return fNameRet;
}

CayfGlobalHostClass::CayfGlobalHostClass() : CjvxHostJsonCommandsProperties(config_show)
{
	
}

CayfGlobalHostClass::~CayfGlobalHostClass()
{
	
}


jvxErrorType 
CayfGlobalHostClass::register_module_host(const char* nm, jvxApiString& nmAsRegistered, IjvxObject* regMe, IjvxMinHost** hostOnReturn, IjvxConfigProcessor** cfgOnReturn)
{

	auto elm = registeredModules.find(regMe);
	if (elm != registeredModules.end())
	{
		return JVX_ERROR_DUPLICATE_ENTRY;
	}

	CayfGlobalHostClass::oneEntryHostList newElm;
	newElm.obj = regMe;
	newElm.nmRegistered = nm;
	jvxSize cnt = 1;
	while (1)
	{
		jvxBool foundName = false;
		for (auto selm : registeredModules)
		{
			if (selm.second.nmRegistered == newElm.nmRegistered)
			{
				foundName = true;
				break;
			}
		}
		if (!foundName)
		{
			break;
		}
		else
		{
			newElm.nmRegistered = nm;
			newElm.nmRegistered += "<";
			newElm.nmRegistered += jvx_size2String(cnt);
			newElm.nmRegistered += ">";
			cnt++;
		}
		assert(cnt < 100);
	}
	nmAsRegistered.assign(newElm.nmRegistered);

	if (globalHostRef == nullptr)
	{
		// start hostref
		jvxHCon_init(&globalHostObj);
		assert(globalHostObj);

		// Start config reader
		jvxTconfigProcessor_init(&confProcObj);
		if (confProcObj)
		{
			this->confProcHdl = castFromObject<IjvxConfigProcessor>(confProcObj);
		}
	
		jvxErrorType resC;
		globalHostRef = castFromObject<IjvxMinHost>(globalHostObj);

		resC = globalHostRef->initialize(nullptr);
		assert(resC == JVX_NO_ERROR);

		globalHostRef->set_location_info(jvxComponentIdentification(JVX_COMPONENT_MIN_HOST, JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_SLOT_OFF_SYSTEM, 0));

		resC = globalHostRef->select(nullptr);
		assert(resC == JVX_NO_ERROR);

		resC = globalHostRef->activate();
		assert(resC == JVX_NO_ERROR);
	}

	newElm.cfgFName = newElm.nmRegistered + ".jvx";

	assert(globalHostRef);
	registeredModules[newElm.obj] = newElm;

	if (hostOnReturn)
	{
		*hostOnReturn = globalHostRef;
	}
	if (cfgOnReturn)
	{
		*cfgOnReturn = confProcHdl;
	}

	return JVX_NO_ERROR;
}

jvxErrorType 
CayfGlobalHostClass::unregister_module_host(IjvxObject* regMe)
{
	jvxErrorType res = JVX_NO_ERROR;
	auto elm = registeredModules.find(regMe);
	if (elm == registeredModules.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	registeredModules.erase(regMe);

	if (registeredModules.empty())
	{
		jvxErrorType resC = globalHostRef->deactivate();
		assert(resC == JVX_NO_ERROR);

		resC = globalHostRef->unselect();
		assert(resC == JVX_NO_ERROR);

		resC = globalHostRef->terminate();
		assert(resC == JVX_NO_ERROR);

		globalHostRef = nullptr;
		jvxHCon_terminate(globalHostObj);
		globalHostObj = nullptr;

		if (confProcObj)
		{
			jvxTconfigProcessor_terminate(confProcObj);
			confProcObj = nullptr;
		}
	}

	return res;
}

jvxErrorType 
CayfGlobalHostClass::load_config_content(IjvxObject* priObj, jvxConfigData** datOnReturn, const char* fName)
{
	auto elm = registeredModules.find(priObj);
	if (elm != registeredModules.end())
	{		
		elm->second.resConfig = JVX_ERROR_UNSUPPORTED;
		if (confProcHdl)
		{
			if (fName)
			{
				elm->second.cfgFName = fName;
			}
			if (elm->second.cfgHandleInUse)
			{
				confProcHdl->removeHandle(elm->second.cfgHandleInUse);
				elm->second.cfgHandleInUse = nullptr;
			}
			for (auto elmL : elm->second.cfgAddPaths)
			{
				confProcHdl->addIncludePath(elmL.c_str());
			}

			std::string fName = combineWithCwd(elm->second.cfgFName);
			elm->second.errTxt.clear();
			elm->second.resConfig = confProcHdl->parseFile(elm->second.cfgFName.c_str());
			if (elm->second.resConfig != JVX_NO_ERROR)
			{
				jvxApiError err;
				confProcHdl->getParseError(&err);
				elm->second.errTxt = err.errorDescription.std_str();
			}
			else
			{
				elm->second.resConfig = confProcHdl->getConfigurationHandle(&elm->second.cfgHandleInUse);
				if (datOnReturn)
				{
					*datOnReturn = elm->second.cfgHandleInUse;
				}
			}
			return JVX_NO_ERROR;
			// jvx_load
		}
		return JVX_ERROR_UNSUPPORTED;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CayfGlobalHostClass::release_config_content(IjvxObject* priObj, jvxConfigData* datOnReturn)
{
	auto elm = registeredModules.find(priObj);
	if (elm != registeredModules.end())
	{
		if (confProcHdl)
		{
			if (datOnReturn == elm->second.cfgHandleInUse)
			{
				if (elm->second.cfgHandleInUse)
				{
					confProcHdl->removeHandle(elm->second.cfgHandleInUse);
					elm->second.cfgHandleInUse = nullptr;
				}
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		return JVX_ERROR_UNSUPPORTED;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CayfGlobalHostClass::attach_component_module(const char* nm, IjvxObject* priObj, IjvxObject* attachMe)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elm = registeredModules.find(priObj);
	if (elm != registeredModules.end())
	{
		std::string nmLnk = nm;
		auto elmE = elm->second.attachedModules.find(nmLnk);
		if (elmE == elm->second.attachedModules.end())
		{
			CayfGlobalHostClass::oneEntryAttachedComponent newElm;
			newElm.nm = nmLnk;
			newElm.theObj = attachMe;
			elm->second.attachedModules[nmLnk] = newElm;
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_DUPLICATE_ENTRY;
		}
	}
	return res;
}

jvxErrorType 
CayfGlobalHostClass::detach_component_module(const char* nm, IjvxObject* priObj)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elm = registeredModules.find(priObj);
	if (elm != registeredModules.end())
	{
		std::string nmLnk = nm;
		auto elmE = elm->second.attachedModules.find(nmLnk);
		if (elmE != elm->second.attachedModules.end())
		{
			elm->second.attachedModules.erase(elmE);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	return res;
}

jvxErrorType 
CayfGlobalHostClass::forward_text_command(const char* command, IjvxObject* priObj, jvxApiString& astr)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elm = registeredModules.find(priObj);
	if (elm != registeredModules.end())
	{
		std::string resp = processTextMessage(command, elm->second);
		astr.assign(resp);
		res = JVX_NO_ERROR;
	}
	return res;
}

/*
 * 
 * This is the base class callback to obtain a pointer to the properties object if we are running without
 * component location. The pointer <cpOfInterest> must be set before the actual call to 
 * any of the functions
 * - show_property_list
 * - act_get_property_component_core
 * - act_set_property_component_core
 * If the pointer is nullptr, the returned field should always be like "no pproperties"
 */
jvxErrorType 
CayfGlobalHostClass::requestReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp)
{
	if (cpOfInterest)
	{
		theTriple.theObj = cpOfInterest;
		IjvxNode* node = castFromObject<IjvxNode>(theTriple.theObj);
		IjvxDevice* dev = castFromObject<IjvxDevice>(theTriple.theObj);
		IjvxTechnology* tech = castFromObject<IjvxTechnology>(theTriple.theObj);
		if (node)
		{
			node->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple.theProps));
			theTriple.theHdl = node;
		}
		else if (dev)
		{
			dev->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple.theProps));
			theTriple.theHdl = dev;
		}
		else if (tech)
		{
			tech->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple.theProps));
			theTriple.theHdl = tech;
		}
	}
	return JVX_NO_ERROR;

}

jvxErrorType 
CayfGlobalHostClass::returnReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp)
{
	if (cpOfInterest)
	{
		theTriple.theObj = cpOfInterest;
		IjvxNode* node = castFromObject<IjvxNode>(theTriple.theObj);
		IjvxDevice* dev = castFromObject<IjvxDevice>(theTriple.theObj);
		IjvxTechnology* tech = castFromObject<IjvxTechnology>(theTriple.theObj);
		if (theTriple.theProps)
		{
			if (node)
			{
				node->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple.theProps));
			}
			else if (dev)
			{
				dev->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple.theProps));
			}
			else if (tech)
			{
				tech->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple.theProps));
			}
		}
		theTriple.theProps = nullptr;
		theTriple.theHdl = nullptr;
	}
	return JVX_NO_ERROR;
}

// ================================================================================
// ================================================================================

std::string
CayfGlobalHostClass::processTextMessage(std::string txt, oneEntryHostList& entry)
{
	std::string errTxt;
	jvxComponentIdentification tp(JVX_COMPONENT_UNKNOWN, JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_SLOT_OFF_SYSTEM);
	jvxErrorType resC = JVX_NO_ERROR;
	CjvxJsonElementList jelmret;
	CjvxJsonElement jelm_result;
	std::string response = "";
	std::string command = "";
	std::string component = "";
	std::string argtoken = "";
	std::string props;
	std::string loadTarget;
	std::string offs;
	jvxApiString astr;

	size_t pos = txt.find(':');
	if (pos != std::string::npos)
	{
		command = txt.substr(0, pos);
		txt = txt.substr(pos + 1);
		pos = txt.find(':');
		if (pos != std::string::npos)
		{
			component = txt.substr(0, pos);
			argtoken = txt.substr(pos + 1);
		}
		else
		{
			component = txt;
		}
	}
	else
	{
		command = txt;
	}

	if (!command.empty())
	{
		if (command == "components")
		{
			CjvxJsonArray jelmarr;

			// Return the first element - the main entry components
			CjvxJsonArrayElement jelmarre;
			entry.obj->module_reference(&astr, nullptr);
			jelmarre.makeString(astr.std_str());
			jelmarr.addConsumeElement(jelmarre);

			for (auto& elm : entry.attachedModules)
			{
				jelmarre.makeString(elm.first);
				jelmarr.addConsumeElement(jelmarre);
			}
			jelm_result.makeArray("registered_elements", jelmarr);
			jelmret.addConsumeElement(jelm_result);
			jelm_result.makeAssignmentString("return_code", jvxErrorType_str[JVX_NO_ERROR].friendly);
			jelmret.insertConsumeElementFront(jelm_result);
			
		}
		else if (command == "config")
		{
			if (component.empty())
			{
				component = "save";
			}
			if (component == "save")
			{
				resC = JVX_ERROR_UNSUPPORTED;
				if (confProcHdl)
				{
					jvxConfigData* datLocal = nullptr;
					jvxConfigData* datTmp;
					confProcHdl->createEmptySection(&datTmp, entry.nmRegistered.c_str());

					datLocal = configOneObject(entry.nmRegistered, entry.obj);
					if (datLocal)
					{
						confProcHdl->addSubsectionToSection(datTmp, datLocal);
					}
					datLocal = nullptr;

					auto elm = entry.attachedModules.begin();
					for (; elm != entry.attachedModules.end(); elm++)
					{
						if (elm->second.includeConfig)
						{
							datLocal = configOneObject(elm->second.nm.c_str(), elm->second.theObj);
						}
						else
						{
							confProcHdl->createComment(&datLocal, ("Component <" + elm->second.nm + "> not part of the configuration.").c_str());
						}
						if (datLocal)
						{
							confProcHdl->addSubsectionToSection(datTmp, datLocal);
						}
					}

					jvxApiString fldStr;
					resC = confProcHdl->printConfiguration(datTmp, &fldStr, false);
					if (resC == JVX_NO_ERROR)
					{
						std::string fName = combineWithCwd(entry.cfgFName);
						resC = jvx_writeContentToFile(fName, fldStr.std_str());
#ifdef JVX_OS_WINDOWS
						fName = jvx_replaceCharacter(fName, JVX_SEPARATOR_DIR_CHAR, JVX_SEPARATOR_DIR_CHAR_THE_OTHER);
#endif
						jelm_result.makeAssignmentString("written_to_file", fName);
						jelmret.addConsumeElement(jelm_result);
					}
					confProcHdl->removeHandle(datTmp);
				}

				jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resC].friendly);
				jelmret.insertConsumeElementFront(jelm_result);
			}
			else if (component == "status")
			{
				jelm_result.makeAssignmentString("return_code_cfg", jvxErrorType_str[entry.resConfig].friendly);
				jelmret.insertConsumeElementFront(jelm_result);

				jelm_result.makeAssignmentString("err_txt_cfg", entry.errTxt);
				jelmret.insertConsumeElementFront(jelm_result);

				jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resC].friendly);
				jelmret.insertConsumeElementFront(jelm_result);
			}
			else
			{
				resC = JVX_ERROR_ELEMENT_NOT_FOUND;
				errTxt = "Unknown sub-command <";
				errTxt += component;
				errTxt += "> addressed by text request.";
				JVX_CREATE_ERROR_NO_RETURN(jelmret, resC, errTxt);
				jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resC].friendly);
				jelmret.insertConsumeElementFront(jelm_result);
			}
		}
		else if (!component.empty())
		{
			// Reset the pointer reference at first
			cpOfInterest = nullptr;
			entry.obj->module_reference(&astr, nullptr);
			if (astr.std_str() == component)
			{
				cpOfInterest = entry.obj;
			}
			else
			{
				auto elm = entry.attachedModules.begin();
				for (; elm != entry.attachedModules.end(); elm++)
				{
					if (elm->second.nm == component)
					{
						cpOfInterest = elm->second.theObj;
						break;
					}
				}
			}

			if (cpOfInterest)
			{
				// extract additional arguments
				std::string addArg;
				std::vector<std::string> args;
				size_t posOpenSqB = command.find('[');
				size_t posCloseSqB = command.find(']');
				if (
					(posOpenSqB != std::string::npos) &&
					(posCloseSqB != std::string::npos) &&
					(posCloseSqB > posOpenSqB))
				{
					addArg = command.substr(posOpenSqB + 1, posCloseSqB - posOpenSqB - 1);
					command = command.substr(0, posOpenSqB);
				}

				// Evaluate additional arguments
				jvxSize j;
				jvxBool content_only = false;
				jvxBool compact = false;
				jvxBool reportSet = false;
				for (j = 0; j < addArg.size(); j++)
				{
					switch (addArg[j])
					{
					case 'c':
						content_only = true;
						break;
					case 'C':
						compact = true;
						break;
					case 'r':
						reportSet = true;
						break;
					default:
						// Unknown option ignored
						break;

					}
				}


				if (command == "properties")
				{
					if (argtoken.size())
					{
						jvx_parseStringListIntoTokens(argtoken, args);
					}
					std::string arg1, arg2;
					if (args.size() > 0)
					{
						arg1 = args[0];
					}
					if (args.size() > 1)
					{
						arg2 = args[1];
					}

					resC = show_property_list(tp, arg1, arg2, jelmret, errTxt);
					if (resC != JVX_NO_ERROR)
					{
						JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
					}
					jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resC].friendly);
					jelmret.insertConsumeElementFront(jelm_result);

				}
				else if (command == "get_property")
				{
					std::vector<std::string> args;
					if (argtoken.size())
					{
						jvx_parseStringListIntoTokens(argtoken, args);
					}
					if (args.size() > 0)
					{
						props = args[0];
						resC = act_get_property_component_core(tp, props, args, 0, jelmret, content_only, compact, errTxt);
					}
					else
					{
						resC = JVX_ERROR_INVALID_ARGUMENT;
					}
					if (resC != JVX_NO_ERROR)
					{
						JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
					}
					jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resC].friendly);
					jelmret.insertConsumeElementFront(jelm_result);
				}
				else if (command == "set_property")
				{
					std::vector<std::string> args;
					if (argtoken.size())
					{
						jvx_parseStringListIntoTokens(argtoken, args);
					}
					if (args.size() > 1)
					{
						props = args[0];
						loadTarget = args[1];
						if (args.size() > 2)
						{
							offs = args[3];
						}
						resC = act_set_property_component_core(tp, props, loadTarget, offs, jelmret, reportSet, errTxt);
					}
					else
					{
						resC = JVX_ERROR_INVALID_ARGUMENT;
					}
					if (resC != JVX_NO_ERROR)
					{
						JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
					}
					jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resC].friendly);
					jelmret.insertConsumeElementFront(jelm_result);
				}
				else
				{
					resC = JVX_ERROR_ELEMENT_NOT_FOUND;
					errTxt = "Unknown command <";
					errTxt += command;
					errTxt += "> addressed by text request.";
					JVX_CREATE_ERROR_NO_RETURN(jelmret, resC, errTxt);
					jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resC].friendly);
					jelmret.insertConsumeElementFront(jelm_result);
				}
			}
			else
			{
				resC = JVX_ERROR_ELEMENT_NOT_FOUND;
				errTxt = "Unknown component <";
				errTxt += component;
				errTxt += "> addressed by command.";
				JVX_CREATE_ERROR_NO_RETURN(jelmret, resC, errTxt);
				jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resC].friendly);
				jelmret.insertConsumeElementFront(jelm_result);
			}
		} // Else ok
	}
	else
	{
		resC = JVX_ERROR_INVALID_ARGUMENT;
		errTxt = "Empty command in text request.";
		JVX_CREATE_ERROR_NO_RETURN(jelmret, resC, errTxt);
		jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resC].friendly);
		jelmret.insertConsumeElementFront(jelm_result);
	}

// exit_func:
	jelmret.printToJsonView(response);
	return response;
}

jvxConfigData* 
CayfGlobalHostClass::configOneObject(const std::string& nm, IjvxObject* theObj)
{
	jvxConfigData* retPtr = nullptr;

	confProcHdl->createEmptySection(&retPtr, nm.c_str());
	if (retPtr)
	{
		IjvxConfiguration* cfgLocal = reqInterfaceObj<IjvxConfiguration>(theObj);
		if (cfgLocal)
		{
			jvxCallManagerConfiguration callMan;
			jvxErrorType resC = cfgLocal->get_configuration(&callMan, confProcHdl, retPtr);
			if (resC != JVX_NO_ERROR)
			{
				// Replace return pointer
				confProcHdl->removeHandle(retPtr);
				retPtr = nullptr;

				confProcHdl->createComment(&retPtr,
					("Component <" + nm + "> returned error <" + jvxErrorType_descr(resC) +
						"> on obtaining configuration.").c_str());
			}
			retInterfaceObj<IjvxConfiguration>(theObj, cfgLocal);
		}
		else
		{
			// Replace return pointer
			confProcHdl->removeHandle(retPtr);
			retPtr = nullptr;

			confProcHdl->createComment(&retPtr,
				("Component <" + nm + "> does not support configuration.").c_str());
		}
	}
	return retPtr;
}