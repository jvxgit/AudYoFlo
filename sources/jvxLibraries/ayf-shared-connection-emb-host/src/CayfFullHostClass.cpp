#include "CayfFullHostClass.h"
#include "CjvxHostJsonDefines.h"
#include "jvxMHAppMinHostCon.h"

#include "jvxTconfigProcessor.h"

#include "jvx-core-host-loop.h"

static std::string combineWithCwd(const std::string& fName)
{
	std::string fNameRet = ".";
	char wDir[JVX_MAXSTRING] = { 0 };
	if (JVX_GETCURRENTDIRECTORY(wDir, JVX_MAXSTRING))
	{
		fNameRet = wDir;
		fNameRet += JVX_SEPARATOR_DIR + fName;
	}
	return fNameRet;
}

CayfFullHostClass::CayfFullHostClass() : CjvxHostJsonCommandsProperties(config_show)
{
	JVX_INITIALIZE_MUTEX(safeAccess);
}

CayfFullHostClass::~CayfFullHostClass()
{
	JVX_TERMINATE_MUTEX(safeAccess);
}

JVX_THREAD_ENTRY_FUNCTION(runHost, param)
{
	// void parameter param
	CayfFullHostClass* this_pointer = (CayfFullHostClass*)param;
	if (this_pointer)
	{
		return this_pointer->mainThreadLoop();
	}
	return JVX_THREAD_EXIT_ERROR;
}

jvxErrorType 
CayfFullHostClass::register_factory_host(const char* nm, jvxApiString& nmAsRegistered, IjvxExternalModuleFactory* regMe, int argc, const char* argv[])
{
	jvxBool triggerNew = false;
	oneRegisteredFactory newElm;
	jvxBool runMeInit = true;
	newElm.name = nm;
	newElm.nameAsRegistered = nm;
	newElm.theFactory = regMe;

	// Expect that these requests all come from a single thread!!
	if (requestThread == JVX_THREAD_ID_INVALID)
	{
		requestThread = JVX_GET_CURRENT_THREAD_ID();
	}
	assert(requestThread == JVX_GET_CURRENT_THREAD_ID());

	// If we drop in here while shutting down, we run active wait.
	// I do not see any way to loop here, typically, the host should not be in shutdown since
	// the dll will be closed immediately if there is no other request
	while (runMeInit)
	{
		JVX_LOCK_MUTEX(safeAccess);
		if (!shutDownProcess)
		{
			runMeInit = false;
		}
		JVX_UNLOCK_MUTEX(safeAccess);
	}

	JVX_LOCK_MUTEX(safeAccess);
	if (firstRunExternalFacs)
	{
		triggerNew = true;
	}
	factories_pending.push_back(newElm);
	JVX_UNLOCK_MUTEX(safeAccess);

	if (hostStarted == false)
	{
		jvxSize i;
		for (i = 0; i < argc; i++)
		{
			astrlArgv.add(argv[i]);
		}
		JVX_CREATE_THREAD(hdlHostThread, runHost, this, idHostThread);
		hostStarted = true;
	}

	if (triggerNew)
	{
		jvx_core_host_loop_trigger_invite(this, false);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfFullHostClass::unregister_factory_host(IjvxExternalModuleFactory* regMe)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool triggerRemove = false;

	assert(requestThread == JVX_GET_CURRENT_THREAD_ID());

	JVX_LOCK_MUTEX(safeAccess);
	for(auto elm = factories_active.begin(); elm != factories_active.end(); elm++)
	{
		if (elm->theFactory == regMe)
		{
			factories_remove.push_back(*elm);
			factories_active.erase(elm);
			triggerRemove = true;
			break;
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	if (triggerRemove)
	{
		jvx_core_host_loop_trigger_uninvite(this, true); // Blocking run: must be OUTSIDE the lock
	}

	jvxSize cntAll = 0;
	jvxBool shutdownRun = false;

	// We put the shutdown into a mutex: if another something requests a register while in shutdown, we should block it until shutdown is complete!!
	JVX_LOCK_MUTEX(safeAccess);
	cntAll += factories_active.size() + factories_pending.size();
	if (cntAll == 0)
	{
		shutDownProcess = true;
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	// The shutdown must not be in the lock: we will run the factory disinvite within the shutdown loop and would block
	if (shutDownProcess)
	{
		if (hostStarted)
		{
			jvxErrorType res = jvx_core_host_loop_stop();
			assert(res == JVX_NO_ERROR);
			JVX_WAIT_FOR_THREAD_TERMINATE_INF(hdlHostThread);			
			shutdownRun = true;
		}
	}

	JVX_LOCK_MUTEX(safeAccess);
	shutDownProcess = false;	
	if (shutdownRun)
	{
		hostStarted = false;
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	return res;
}

JVX_THREAD_EXIT_CODE
CayfFullHostClass::mainThreadLoop()
{
	const char** argv = nullptr;
	int argc = 1 + astrlArgv.ll();
	if (argc)
	{
		jvxSize i;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(argv, const char*, argc);
		argv[0] = "CayfFullHostClass";
		for (i = 1; i < argc; i++)
		{
			argv[i] = astrlArgv.c_str_at(i - 1);
		}
	}

	IjvxEventLoop_frontend_ctrl* addFe = static_cast<IjvxEventLoop_frontend_ctrl*>(this);
	jvx_core_host_loop(argc, (char**)argv, &addFe, 1);
	return JVX_THREAD_EXIT_NORMAL;
}

jvxErrorType 
CayfFullHostClass::load_config_content(IjvxExternalModuleFactory* priIf, jvxConfigData** datOnReturn, const char* fName)
{
	if (hHost)
	{
		auto inst = reqInterface<IjvxComponentHostExt>(hHost);
		if (inst)
		{
			std::string modName;
			JVX_LOCK_MUTEX(safeAccess);
			for (auto& elm : factories_active)
			{
				if (elm.theFactory == priIf)
				{
					modName = elm.nameAsRegistered;
					break;
				}
			}
			if (modName.empty())
			{
				for (auto& elm : factories_pending)
				{
					if (elm.theFactory == priIf)
					{
						modName = elm.nameAsRegistered;
						break;
					}
				}
			}
			JVX_UNLOCK_MUTEX(safeAccess);

			if (!modName.empty())
			{
				inst->load_config_content_module(modName.c_str(), datOnReturn);
			}
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CayfFullHostClass::release_config_content(IjvxExternalModuleFactory* priIf, jvxConfigData* datOnReturn)
{
	if (hHost)
	{
		auto inst = reqInterface<IjvxComponentHostExt>(hHost);
		if (inst)
		{
			std::string modName;
			JVX_LOCK_MUTEX(safeAccess);
			for (auto& elm : factories_active)
			{
				if (elm.theFactory == priIf)
				{
					modName = elm.nameAsRegistered;
					break;
				}
			}
			if (modName.empty())
			{
				for (auto& elm : factories_pending)
				{
					if (elm.theFactory == priIf)
					{
						modName = elm.nameAsRegistered;
						break;
					}
				}
			}
			if (modName.empty())
			{
				for (auto& elm : factories_remove)
				{
					if (elm.theFactory == priIf)
					{
						modName = elm.nameAsRegistered;
						break;
					}
				}
			}
			JVX_UNLOCK_MUTEX(safeAccess);

			if (!modName.empty())
			{
				inst->release_config_content_module(modName.c_str(), datOnReturn);
			}
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

/*

jvxErrorType
CayfFullHostClass::attach_component_module(const char* nm, IjvxObject* priObj, IjvxObject* attachMe)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	
	return res;
}

jvxErrorType 
CayfFullHostClass::detach_component_module(const char* nm, IjvxObject* priObj)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	
	return res;
}

jvxErrorType 
CayfFullHostClass::forward_text_command(const char* command, IjvxObject* priObj, jvxApiString& astr)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	
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
CayfFullHostClass::requestReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp)
{
	
	return JVX_NO_ERROR;

}

jvxErrorType 
CayfFullHostClass::returnReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp)
{
	
	return JVX_NO_ERROR;
}

// ================================================================================
// ================================================================================

jvxErrorType 
CayfFullHostClass::invite_external_components(IjvxHiddenInterface* hostRef, jvxBool isInvite)
{
	if (isInvite)
	{
		JVX_LOCK_MUTEX(safeAccess);
		firstRunExternalFacs = true;
		while(factories_pending.size())
		{
			auto elm = factories_pending.begin(); 
			elm->theFactory->invite_external_components(hostRef, isInvite);
			factories_active.push_back(*elm);
			factories_pending.erase(elm);
		}
		JVX_UNLOCK_MUTEX(safeAccess);
	}
	else
	{
		JVX_LOCK_MUTEX(safeAccess);
		while (factories_remove.size())
		{
			auto elm = factories_remove.begin();
			elm->theFactory->invite_external_components(hostRef, isInvite);
			factories_remove.erase(elm);
		}
		JVX_UNLOCK_MUTEX(safeAccess);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CayfFullHostClass::forward_text_command(const char* command, IjvxObject* priObj, jvxApiString& astr)
{
	// oneThreadReturnType myPrivateMemReturn;
	jvxApiString cmd = command;
	JVX_LOCK_MUTEX(safeAccess);
	if (evLoop && priBeHandle)
	{
		TjvxEventLoopElement elm;

		//myPrivateMemReturn.res_mthread = JVX_ERROR_UNSUPPORTED;
		//myPrivateMemReturn.ret_mthread = "";
		evLoop->reserve_message_id(&messageId);		

		// Transfer command to the primary backend

		elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		elm.priv_fe = NULL;
		elm.target_be = priBeHandle;
		elm.priv_be = NULL;

		elm.param = (jvxHandle*)(&cmd);
		elm.paramType = JVX_EVENTLOOP_DATAFORMAT_JVXAPISTRING;

		elm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
		elm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
		elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		elm.delta_t = JVX_SIZE_UNSELECTED;
		elm.autoDeleteOnProcess = c_false;

		jvxErrorType res = evLoop->event_schedule(&elm, NULL, nullptr);
		switch (res)
		{
		case JVX_ERROR_END_OF_FILE:
			std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
			break;
		case JVX_ERROR_ABORT:
			std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
			break;
		case JVX_NO_ERROR:

			// Copy the output all back
			astr = tmpOutputOnCommand;
			break;
		default:
			assert(0);
		}
	}	
	else
	{
		astr = "Invalid call";
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	return JVX_NO_ERROR;

}
/*
std::string
CayfFullHostClass::processTextMessage(std::string txt, oneEntryHostList& entry)
{
	std::string response;
	return response;
}

jvxConfigData* 
CayfFullHostClass::configOneObject(const std::string& nm, IjvxObject* theObj)
{
	jvxConfigData* retPtr = nullptr;
	return retPtr;
}
*/
