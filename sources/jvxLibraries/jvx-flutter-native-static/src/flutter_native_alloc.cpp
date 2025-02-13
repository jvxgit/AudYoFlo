#include "flutter_native_local.h"

#ifdef JVX_LIB_SINGLETON
jvxLibHost myLibHost;
#endif
jvxRequestCommandHandlerLocal myReqCmdFwdSingleton;

JVX_THREAD_ID mainThreadId = JVX_THREAD_ID_INVALID;

// ===================================================================================
// Some global variables lingering around
// ===================================================================================

std::map<void*, jvxLibHost*> lst_active_referenes;

std::string dllConfigSystem = "";
native_host_configure_func_pointers func_pointer_object;
native_host_configure_func_pointers* func_pointer_reference = &func_pointer_object;
JVX_HMODULE dllHandleConfig = JVX_HMODULE_INVALID; 
flutter_config_open_ptr fptrConfig = nullptr;

// ========================================================================
// ========================================================================

/*
 * The end of the process is not reported in flutter :-(
 * Therefore, this position is the only location at which we might
 * do some resource closure.
 */
#ifdef JVX_OS_WINDOWS
bool flutternative_dllmain(DWORD fdwReason)
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:

#if 0
		// Perform any necessary cleanup.
	{
		while (1)
		{
			auto elm = lst_active_referenes.begin();
			if (elm != lst_active_referenes.end())
			{
				ffi_host_terminate(elm->second);

				// This removes the entry from the map 
				ffi_deallocate_backend_handle(elm->second);
			}
			else
			{
				break;
			}
		}
	}
#endif
	break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif

// ==================================================================================

struct retOpaqueHandle ffi_allocate_backend_handle()
{
	retOpaqueHandle retVal;
	jvxLibHost* newLib = nullptr;

	retVal.err_code = JVX_ERROR_INVALID_ARGUMENT;
	retVal.opaque_hdl = nullptr;

	jvxLibHost::verify_binary_build();

	JVX_INIT_LOCALE("C");

#ifdef JVX_LIB_SINGLETON
	newLib = &myLibHost;
#else
	jvxSize sz = sizeof(jvxLibHost);
	newLib = new jvxLibHost();
#endif

	// Set the cross links between lib and forward instance
	myReqCmdFwdSingleton.backRef = newLib;
	newLib->set_request_command_handler(&myReqCmdFwdSingleton);
	
	retVal.opaque_hdl = reinterpret_cast<void*>(newLib);
	// myLibHost.setMainThread();
	lst_active_referenes[retVal.opaque_hdl] = newLib;
	retVal.err_code = (int)JVX_NO_ERROR;

	return retVal;
};

int ffi_deallocate_backend_handle(void* opaque_hdl)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (opaque_hdl)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elm = lst_active_referenes.find(opaque_hdl);
		if (elm != lst_active_referenes.end())
		{
			jvxLibHost* ll = elm->second;

			// myLibHost.resetMainThread();

			// Set the cross links between lib and forward instance
			myReqCmdFwdSingleton.backRef = nullptr;
			ll->set_request_command_handler(nullptr);

#ifdef JVX_LIB_SINGLETON
			assert(ll == &myLibHost);
#else
			delete ll;
#endif
			lst_active_referenes.erase(elm);
			res = JVX_NO_ERROR;
		}
	}
	if (dllHandleConfig != JVX_HMODULE_INVALID)
	{
		fptrConfig = nullptr;
		JVX_UNLOADLIBRARY(dllHandleConfig);
		dllHandleConfig = JVX_HMODULE_INVALID;
		dllConfigSystem.clear();
	}
	
	return res;
}

int ffi_host_delete(void* ptr, ffiDeleteDatatype tp)
{
	jvxSize i;
	struct one_property_string_list* strLst = nullptr;
	struct ss_list* ssPtr = nullptr;
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (ptr)
	{
		switch (tp)
		{
		case JVX_DELETE_DATATYPE_ASYNC_CALLBACK:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, report_callback*);
			res = JVX_NO_ERROR;
			break;

		case JVX_DELETE_DATATYPE_CHAR_ARRAY:
			JVX_SAFE_DELETE_FIELD_TYPE(ptr, char*);
			res = JVX_NO_ERROR;
			break;

		case JVX_DELETE_DATATYPE_INTERNAL_CHANGED:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, report_internal_changed*);
			res = JVX_NO_ERROR;
			break;

		case JVX_DELETE_DATATYPE_REQUEST_COMMAND:
			// There is a dedicated deallocate function in use since we have used the alloc to create this object
			jvx_command_request_copy_dealloc((CjvxReportCommandRequest*)ptr);
			// JVX_SAFE_DELETE_OBJ_CVRT(ptr, CjvxReportCommandRequest*);
			res = JVX_NO_ERROR;
			break;

		case JVX_DELETE_DATATYPE_ONE_PROCESS:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, one_process*);
			res = JVX_NO_ERROR;
			break;

		case JVX_DELETE_DATATYPE_ONE_PROP_HANDLER:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, one_property_comp*);
			res = JVX_NO_ERROR;
			break;

		case JVX_DELETE_DATATYPE_ONE_SEQ_EVENT:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, struct one_sequencer_event*);
			res = JVX_NO_ERROR;
			break;

		case JVX_DELETE_DATATYPE_ONE_COMPONENT_IDENT:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, struct component_ident*);
			res = JVX_NO_ERROR;
			break;
		case JVX_DELETE_DATATYPE_ONE_PROPERTY_DESCRIPTOR_FULL_PLUS:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, struct one_property_full_plus*);
			res = JVX_NO_ERROR;
			break;
		case JVX_DELETE_DATATYPE_PROCESS_READY:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, struct process_ready*);
			res = JVX_NO_ERROR;
			break;
		case JVX_DELETE_DATATYPE_PROPERTY_STRING_LIST:
			strLst = (struct one_property_string_list*)ptr;
			for (i = 0; i < strLst->nEntries; i++)
			{
				char* ptr = strLst->pEntries[i];
				JVX_SAFE_DELETE_FIELD_TYPE(ptr, char*);
			}
			strLst->nEntries = 0;
			strLst->pEntries = nullptr;
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, struct one_property_string_list*);
			res = JVX_NO_ERROR;
			break;
		case JVX_DELETE_DATATYPE_DEVICE_CAPS:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, struct device_capabilities*);
			res = JVX_NO_ERROR;
			break;
		case JVX_DELETE_DATATYPE_SELECTION_OPTION:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, struct selection_option*);
			res = JVX_NO_ERROR;
			break;
		case JVX_DELETE_DATATYPE_SS_LIST:
			ssPtr = (struct ss_list*)ptr;
			if (ssPtr->slots)
			{
				JVX_SAFE_DELETE_FIELD(ssPtr->slots);
				ssPtr->slots = nullptr;
			}
			if (ssPtr->subslots)
			{
				JVX_SAFE_DELETE_FIELD(ssPtr->subslots);
				ssPtr->subslots = nullptr;
			}
			ssPtr->num = 0;
			JVX_SAFE_DELETE_OBJ(ssPtr);
			break;
		case JVX_DELETE_DATATYPE_VALUE_IN_RANGE:
			JVX_SAFE_DELETE_OBJ_CVRT(ptr, struct one_property_value_in_range*);
			res = JVX_NO_ERROR;
			break;
		default:
			assert(0);
		}
	}
	return res;
}

int ffi_host_allocate_char_array(const std::string& txt, char** txtOnReturn)
{
	if (txtOnReturn)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(*txtOnReturn, char, txt.size() + 1);
		if (*txtOnReturn)
		{
			memcpy(*txtOnReturn, txt.c_str(), txt.size());
		}
	}
	return JVX_NO_ERROR;
}

void ffi_host_allocate_val_range(const jvxValueInRange& val, one_property_value_in_range** valArrOnReturn)
{
	if (valArrOnReturn)
	{
		JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(*valArrOnReturn, one_property_value_in_range);
		if (*valArrOnReturn)
		{
			(*valArrOnReturn)->minVal = val.minVal;
			(*valArrOnReturn)->maxVal = val.maxVal;
		}
	}
}

void ffi_host_allocate_process_handle(struct one_process** ptrRet)
{
	if (ptrRet)
	{
		JVX_DSP_SAFE_ALLOCATE_OBJECT(*ptrRet, struct one_process);
		(*ptrRet)->uId = -1;
		(*ptrRet)->category = -1;
		(*ptrRet)->iterator = nullptr;
	}
}

void ffi_host_allocate_sequencer_event__char_array(TjvxSequencerEvent* ev, struct one_sequencer_event** ptrRet)
{
	if (ptrRet)
	{
		JVX_DSP_SAFE_ALLOCATE_OBJECT(*ptrRet, struct one_sequencer_event);
		ffi_host_allocate_char_array(ev->description.std_str(), &(*ptrRet)->description);
		(*ptrRet)->event_mask = ev->event_mask;
		(*ptrRet)->fId = ev->fId;
		(*ptrRet)->indicateFirstError = ev->indicateFirstError;
		(*ptrRet)->qtp = ev->tp;
		(*ptrRet)->sequenceId = ev->sequenceId;
		(*ptrRet)->seq_state_id = ev->current_state;
		(*ptrRet)->setp = ev->stp;
		(*ptrRet)->stepId = ev->stepId;
		(*ptrRet)->statSeq = ev->seqStat;
	}
}
int ffi_simple_call()
{
	std::cout << "Simple call" << std::endl;
	return JVX_NO_ERROR;
}

// ============================================================================================
void ffi_host_allocate_property_string_list(struct one_property_string_list** ptrRet, const jvxApiStringList& lst)
{
	if (ptrRet)
	{
		struct one_property_string_list* ptr = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(ptr, struct one_property_string_list);
		jvxSize ll = lst.ll();
		char** ptrStr = nullptr;
		if(ll > 0)
		{			
			if (!lst.content_only())
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrStr, char*, ll);
				for (int i = 0; i < ll; i++)
				{
					ffi_host_allocate_char_array(lst.std_str_at(i), &ptrStr[i]);
				}
			}
		}
		ptr->nEntries = ll;
		ptr->pEntries = ptrStr;
		(*ptrRet) = ptr;
	}
}

void ffi_host_allocate_device_capabilities(
	struct device_capabilities** ptrRet, 
	const jvxApiString& descror, 
	const jvxApiString& description, 
	const jvxDeviceCapabilities& caps, 
	const jvxComponentIdentification& tpId,
	const jvxState& stat)
{
	if (ptrRet)
	{
		struct device_capabilities* ptr = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(ptr, struct device_capabilities);
		ptr->caps = caps.capsFlags;
		ffi_host_allocate_char_array(description.std_str(), &ptr->description);
		ffi_host_allocate_char_array(descror.std_str(), &ptr->descriptor);
		ptr->flags = caps.flags;
		ptr->flow = (int)caps.flow;
		ptr->selectable = caps.selectable;
		ptr->proxy = caps.proxy;
		ptr->slotsubid = (int)tpId.slotsubid;
		ptr->stat = stat;
		*ptrRet = ptr;
	}
}

void ffi_host_allocate_selection_option(
	struct selection_option** ptrRet,
	const jvxApiString& descror,
	const jvxApiString& description)
{
	if (ptrRet)
	{
		struct selection_option* ptr = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(ptr, struct selection_option);
		
		ffi_host_allocate_char_array(description.std_str(), &ptr->description);
		ffi_host_allocate_char_array(descror.std_str(), &ptr->descriptor);
		*ptrRet = ptr;
	}
}

void ffi_host_allocate_component_ident(struct component_ident** ptrRet, const jvxComponentIdentification& cpId)
{
	if(ptrRet)
	{
		struct component_ident* ptr = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(ptr, component_ident);
		ptr->tp = cpId.tp;
		ptr->slotid = cpId.slotid;
		ptr->slotsubid = cpId.slotsubid;
		ptr->uId = cpId.uId;
		*ptrRet = ptr;
	}
}

void ffi_host_allocate_ss_list(
	struct ss_list** ptrRet,
	jvxSize sz, jvxBool isDevice)
{
	if (ptrRet)
	{
		struct ss_list* ptr = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(ptr, ss_list);
		ptr->num = sz;
		ptr->slots = nullptr;
		ptr->subslots = nullptr;

		if (ptr->num)
		{
			if (isDevice)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptr->subslots, int, ptr->num);
				for (int i = 0; i < ptr->num; i++)
				{
					ptr->subslots[i] = -1;
				}
			}
			else
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptr->slots, int, ptr->num);
				for (int i = 0; i < ptr->num; i++)
				{
					ptr->slots[i] = -1;
				}
			}
		}
		*ptrRet = ptr;
	}
}
		
extern "C"
{

	void jvx_command_line_specify(IjvxCommandLine* cmdLine)
	{
		if (cmdLine)
		{
			cmdLine->register_option("--natconfm", "", "Specifc the configuration lib to be loaded before startup.", "", true, JVX_DATAFORMAT_STRING);
			cmdLine->register_option("--natconfs", "", "Specifc the symbol to be loaded before startup.", "", true, JVX_DATAFORMAT_STRING);
		}
	}

	void jvx_command_line_read(IjvxCommandLine* cmdLine)
	{
		jvxApiString entry;
		if (cmdLine->content_entry_option("--natconfm", 0, &entry, JVX_DATAFORMAT_STRING) == JVX_NO_ERROR)
		{
			dllConfigSystem = entry.std_str();
			std::string symbConfigSystemName = FLUTTER_CONFIG_OPEN_FUNCTION_NAME;
			if (cmdLine->content_entry_option("--natconfs", 0, &entry, JVX_DATAFORMAT_STRING) == JVX_NO_ERROR)
			{
				symbConfigSystemName = entry.std_str();
			}

			if (!dllConfigSystem.empty())
			{
				std::cout << "Opening runtime library <" << dllConfigSystem  << "> for host configuration." << std::endl;
				dllHandleConfig = JVX_LOADLIBRARY(dllConfigSystem.c_str());
				if (dllHandleConfig != JVX_HMODULE_INVALID)
				{
					fptrConfig = (flutter_config_open_ptr)JVX_GETPROCADDRESS(dllHandleConfig, symbConfigSystemName.c_str());
					if (fptrConfig)
					{
						if (fptrConfig(&func_pointer_object) != JVX_NO_ERROR)
						{
							std::cout << "Error during access to runtime library <" << dllConfigSystem 
								<< ">: Could not find library entry point <" 
								<< FLUTTER_CONFIG_OPEN_FUNCTION_NAME  << ">." << std::endl;

							func_pointer_object.access_link_objects = nullptr;
							func_pointer_object.configure_factoryhost_features = nullptr;
							func_pointer_object.default_connection_rules_add = nullptr;
							func_pointer_object.default_sequence_add = nullptr;
							func_pointer_object.invalidate_factoryhost_features = nullptr;
							fptrConfig = nullptr;
						}
					}
					if (fptrConfig == nullptr)
					{
						JVX_UNLOADLIBRARY(dllHandleConfig);
						dllHandleConfig = JVX_HMODULE_INVALID;
					}
				}
				else
				{
					std::cout << "Error during access to runtime library <" << dllConfigSystem
						<< ">: Could not open module." << std::endl;
				}
			}
		}		
	}
}

void 
jvxRequestCommandHandlerLocal::trigger_immediate_sequencerStep()
{
	this->backRef->sequencer_trigger();
}

jvxErrorType 
jvxRequestCommandHandlerLocal::trigger_threadChange_forward(CjvxReportCommandRequest* ptr_to_copy)
{
	CjvxReportCommandRequest* ptr = jvx_command_request_copy_alloc(*ptr_to_copy);
	report_callback* cbk = nullptr;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(cbk, struct report_callback);
	cbk->callback_id = JVX_FFI_CALLBACK_REQUEST_COMMAND;
	cbk->callback_subid = (int)ptr->request();

	cbk->load_fld = ptr;
	cbk->sz_fld = sizeof(*ptr_to_copy);
	bool resultSend = send_async_dart(cbk);
	if (resultSend == false)
	{
		JVX_DSP_SAFE_DELETE_OBJECT(cbk);
		JVX_DSP_SAFE_DELETE_OBJECT(ptr);
		// return JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
	}
	return JVX_NO_ERROR;
}

void 
jvxRequestCommandHandlerLocal::run_mainthread_triggerTestChainDone()
{

}

void 
jvxRequestCommandHandlerLocal::run_mainthread_updateComponentList(jvxComponentIdentification cpId)
{

}

void 
jvxRequestCommandHandlerLocal::run_mainthread_updateProperties(jvxComponentIdentification cpId)
{

}

void 
jvxRequestCommandHandlerLocal::run_mainthread_updateSystemStatus()
{

}
  
void 
jvxRequestCommandHandlerLocal::run_immediate_rescheduleRequest(const CjvxReportCommandRequest& request)
{
	CjvxReportCommandRequest* ptr = jvx_command_request_copy_alloc(request);
	ptr->set_broadcast(jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_RESCHEDULED);

	report_callback* cbk = nullptr;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(cbk, struct report_callback);
	cbk->callback_id = JVX_FFI_CALLBACK_REQUEST_COMMAND;
	cbk->callback_subid = (int)ptr->request();

	cbk->load_fld = ptr;
	cbk->sz_fld = sizeof(request);
	bool resultSend = send_async_dart(cbk);
	if (resultSend == false)
	{
		JVX_DSP_SAFE_DELETE_OBJECT(cbk);
		JVX_DSP_SAFE_DELETE_OBJECT(ptr);
		// return JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
	}
}

void
jvxRequestCommandHandlerLocal::report_immediate_error(jvxErrorType resError, const CjvxReportCommandRequest& request)
{
	std::cout << __FUNCTION__ << ": Error reported, " << jvxErrorType_descr(resError) << std::endl;
	jvx::helper::debug_out_command_request(request, std::cout, "ERROR");
}