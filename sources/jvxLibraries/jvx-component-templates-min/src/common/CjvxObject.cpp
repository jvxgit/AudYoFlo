#include "common/CjvxObject.h"

jvxErrorType
CjvxObjectCore::_prepare()
{
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		_common_set_min.theState = JVX_STATE_PREPARED;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObjectCore::_start()
{
	if (_common_set_min.theState == JVX_STATE_PREPARED)
	{
		_common_set_min.theState = JVX_STATE_PROCESSING;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObjectCore::_stop()
{
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		_common_set_min.theState = JVX_STATE_PREPARED;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObjectCore::_postprocess()
{
	if (_common_set_min.theState == JVX_STATE_PREPARED)
	{
		/*
		JVX_SAFE_DELETE_FLD(_common_set_processing.theEndpoints, IjvxDataProcessor*);
		_common_set_processing.theNumberEndpoints = 0;
		*/
		_common_set_min.theState = JVX_STATE_ACTIVE;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

// ========================================================================

CjvxObject::CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) : CjvxObjectCore(description)
{
	std::string dComp;

	if (descrComp)
	{
		dComp = descrComp;
	}
	
	_common_set.theName = description;
	_common_set.theFriendlyName = _common_set.theName;
	_common_set.theModuleName = module_name;
	_common_set.theComponentAccessTp = acTp;
	_common_set.theFeatureClass = featureClass;
	if (descriptor)
	{
		_common_set.theDescriptor = descriptor;
	}
	else
	{
		_common_set.theDescriptor = description;
	}

	_common_set.theComponentType.tp = JVX_COMPONENT_UNKNOWN;
	if (tpComp != JVX_COMPONENT_UNKNOWN)
	{
		_common_set.theComponentType.tp = tpComp;
	}

	_common_set.theComponentSubTypeDescriptor = "not_specified";
	if (!dComp.empty())
	{
		_common_set.theComponentSubTypeDescriptor = descrComp;
	}

	_common_set.theComponentType.slotid = JVX_SIZE_UNSELECTED;
	_common_set.theComponentType.slotsubid = JVX_SIZE_UNSELECTED;

	_common_set.theObjectSpecialization = NULL;
	_common_set.theMultipleObjects = multipleObjects;

	_common_set.theErrorid = -1;
	_common_set.theErrorlevel = -1;
	_common_set.theErrordescr = "";

	_common_set.theToolsHost = NULL;
	_common_set.templ = templ;

	_common_set.theInterfaceFactory = NULL;
	//_common_set.theInformation;
	/*
	_common_set_processing.theEndpoints = NULL;
	_common_set_processing.theNumberEndpoints = NULL;
	*/

	_common_set.thisisme = NULL;
	_common_set.iamready = true;

	_common_set.iamalive = true;

#ifdef JVX_OBJECTS_WITH_TEXTLOG

	embLog.jvxrtst_bkp.set_module_name(_common_set.theModuleName);
#endif

	_common_set.stateChecks.prepare_flags = 0;
	_common_set.stateChecks.start_flags = 0;

	JVX_INITIALIZE_MUTEX(_common_set._safeAccessStateBound.the_lock);

}

CjvxObject::~CjvxObject()
{
	JVX_TERMINATE_MUTEX(_common_set._safeAccessStateBound.the_lock);
}


// =========================================================================

jvxErrorType
CjvxObject::_set_location_info(const jvxComponentIdentification& tp)
{
	// This function may set the slotid but it does never change the type!
	// Correction HK, 15.11.2023: It may also accept other types
	if (tp.tp != JVX_COMPONENT_UNKNOWN)
	{
		if (_common_set.theComponentType.tp != tp.tp)
		{
			if (jvxComponentType_class(_common_set.theComponentType.tp) == jvxComponentType_class(tp.tp))
			{
				_common_set.theComponentType.tp = tp.tp;
			}
		}
	}
	_common_set.theComponentType.slotid = tp.slotid;
	_common_set.theComponentType.slotsubid = tp.slotsubid;
	_common_set.theComponentType.uId = tp.uId;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxObject::_location_info(jvxComponentIdentification& tp)
{
	tp = _common_set.theComponentType;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxObject::_name(jvxApiString* str, jvxApiString* fName)
{
	if (str)
	{
		str->assign(_common_set.theName);
	}
	if (fName)
	{
		fName->assign(_common_set.theFriendlyName);
	}
	return(JVX_NO_ERROR);
	//HjvxObject_produceString(str, _common_set_min.theDescription);
}

jvxErrorType
CjvxObject::_description(jvxApiString* str)
{
	if (str)
	{
		str->assign(_common_set_min.theDescription);
	}
	return(JVX_NO_ERROR);
	//HjvxObject_produceString(str, _common_set_min.theDescription);
}

jvxErrorType
CjvxObject::_descriptor(jvxApiString* str, jvxApiString* subComponentDescriptor)
{
	if (str)
	{
		std::string descr_slot_subslot = _common_set.theDescriptor;
		std::string adds;
		if (JVX_CHECK_SIZE_SELECTED(_common_set.theComponentType.slotsubid) && (_common_set.theComponentType.slotsubid > 0))
		{
			adds += "<" + jvx_size2String(_common_set.theComponentType.slotid) + "," + jvx_size2String(_common_set.theComponentType.slotsubid) + ">";
		}
		else
		{
			if (JVX_CHECK_SIZE_SELECTED(_common_set.theComponentType.slotid) && (_common_set.theComponentType.slotid > 0))
			{
				adds += "<" + jvx_size2String(_common_set.theComponentType.slotid) + ">";
			}
		}
		// descr_slot_subslot += adds;
		str->assign(descr_slot_subslot);
	}
	if (subComponentDescriptor)
	{
		subComponentDescriptor->assign(_common_set.theComponentSubTypeDescriptor);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxObject::_module_reference(jvxApiString* str, jvxComponentAccessType* acTp)
{
	if (str)
	{
		str->assign(_common_set.theModuleName);
	}
	if (acTp)
	{
		*acTp = _common_set.theComponentAccessTp;
	}
	return(JVX_NO_ERROR);
}

void
CjvxObject::_append_compile_option(const std::string& oneOption)
{
	if (!oneOption.empty())
	{
		if (_common_set.compileOptions.empty())
		{
			_common_set.compileOptions = oneOption;
		}
		else
		{
			_common_set.compileOptions += ";" + oneOption;
		}
	}
}

jvxErrorType
CjvxObject::_compile_options(jvxApiString* str)
{
	if (str)
	{
		str->assign(_common_set.compileOptions);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxObject::_feature_class(jvxBitField* ft)
{
	if (ft)
	{
		*ft = _common_set.theFeatureClass;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxObject::_version(jvxApiString* str)
{
	std::string ver = JVX_VERSION_STRING;
	str->assign(ver);
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxObject::_requestComponentSpecialization(jvxHandle** obj, jvxComponentIdentification* compTp, jvxBool* multipleObjects,
	jvxApiString* subComponentDescriptor)
{
	if (obj)
	{
		*obj = _common_set.theObjectSpecialization;
	}

	if (compTp)
	{
		*compTp = _common_set.theComponentType;
	}
	if (multipleObjects)
	{
		*multipleObjects = _common_set.theMultipleObjects;
	}
	if (subComponentDescriptor)
	{
		subComponentDescriptor->assign(_common_set.theComponentSubTypeDescriptor);
	}
	return(JVX_NO_ERROR);
}

void CjvxObject::_seterror(jvxErrorType tp)
{
	_common_set.theErrordescr = jvxErrorType_descr((jvxSize)tp);
}

jvxErrorType
CjvxObject::_return_seterr(jvxErrorType tp, jvxInt32 errId, jvxInt32 errLvl)
{
	_common_set.theErrordescr = jvxErrorType_descr((jvxSize)tp);
	_common_set.theErrorid = errId;
	_common_set.theErrorlevel = errLvl;
	return(tp);
}

jvxErrorType
CjvxObject::_lasterror(jvxApiError* err_argout)
{
	if (err_argout)
	{
		err_argout->errorCode = _common_set.theErrorid;
		err_argout->errorLevel = _common_set.theErrorlevel;
		err_argout->errorDescription.assign(_common_set.theErrordescr);
	}
	_common_set.theErrorid = -1;
	_common_set.theErrorlevel = -1;
	_common_set.theErrordescr = "";

	return(JVX_NO_ERROR);
}

// =========================================================================

jvxErrorType
CjvxObject::_request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	return(JVX_ERROR_UNSUPPORTED);
};


jvxErrorType
CjvxObject::_return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	return(JVX_ERROR_UNSUPPORTED);
}

// =========================================================================



// =========================================================================

jvxErrorType
CjvxObject::_interface_factory(IjvxInterfaceFactory** refOnReturn)
{
	if (refOnReturn)
	{
		*refOnReturn = _common_set.theInterfaceFactory;
	}
	return JVX_NO_ERROR;
}

// =========================================================================

// State handling functions
//jvxErrorType CjvxObject::_initialize(IjvxHost* hostRef = NULL)

jvxErrorType
CjvxObject::_system_ready()
{
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxObject::_system_about_to_shutdown()
{
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxObject::_select(IjvxObject* newOwner)
{
	jvxErrorType resL = CjvxObjectMin::_select(newOwner);
	if (resL == JVX_NO_ERROR)
	{
	
		_common_set.register_host_id = JVX_SIZE_UNSELECTED;
		if (_common_set_min.theHostRef)
		{
			IjvxHost* theHost = NULL;
			_common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_HOST, reinterpret_cast<jvxHandle**>(&theHost));
			if (theHost)
			{
				theHost->request_unique_host_id(&_common_set.register_host_id);
				_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_HOST, reinterpret_cast<jvxHandle*>(theHost));
			}
			theHost = NULL;
		}

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

// ===========================================================================

jvxErrorType
CjvxObject::_activate()
{
	jvxErrorType resL = JVX_NO_ERROR;
	resL = CjvxObjectMin::_activate();

	if (resL == JVX_NO_ERROR)
	{
		if (_common_set_min.theHostRef)
		{
			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST,
				reinterpret_cast<jvxHandle**>(&_common_set.theToolsHost));

			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_UNIQUE_ID,
				reinterpret_cast<jvxHandle**>(&_common_set.theUniqueId));

			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT,
				reinterpret_cast<jvxHandle**>(&_common_set.theReport));
		}

#ifdef JVX_OBJECTS_WITH_TEXTLOG
		_request_text_log();
#endif
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
};

jvxErrorType
CjvxObject::_activate_no_text_log()
{
	jvxErrorType resL = JVX_NO_ERROR;
	resL = CjvxObjectMin::_activate();

	if (resL == JVX_NO_ERROR)
	{
		if (_common_set_min.theHostRef)
		{
			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST,
				reinterpret_cast<jvxHandle**>(&_common_set.theToolsHost));

			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_UNIQUE_ID,
				reinterpret_cast<jvxHandle**>(&_common_set.theUniqueId));

			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT,
				reinterpret_cast<jvxHandle**>(&_common_set.theReport));
		}
		
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
};

jvxErrorType
CjvxObject::_activate_lock()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	res = _activate();
	if (res != JVX_NO_ERROR)
	{
		JVX_UNLOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	}
	return(res);
}

// ===========================================================================

jvxErrorType
CjvxObject::_is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if (suc)
		{
			if (_common_set.iamready)
			{
				*suc = true;
			}
			else
			{
				if (suc)
				{
					*suc = false;
				}
				std::string txt = _common_set.reasonnotready;
				if (reasonIfNot)
				{
					reasonIfNot->assign(txt);
				}
			}
		}
	}
	else
	{
		if (suc)
		{
			*suc = false;
		}
		std::string txt = "Component is in state ";
		txt += jvxState_txt(_common_set_min.theState);
		if (reasonIfNot)
		{
			reasonIfNot->assign(txt);
		}
	}
	return(JVX_NO_ERROR);
}

// ===========================================================================

jvxErrorType
CjvxObject::_is_alive(jvxBool* alive)
{
	if (alive)
	{
		*alive = _common_set.iamalive;
	}
	return(JVX_NO_ERROR);
}

// ===========================================================================

jvxErrorType
CjvxObject::_prepare_lock()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	res = _prepare();
	if (res != JVX_NO_ERROR)
	{
		JVX_UNLOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	}
	return(res);
}
// ===========================================================================

jvxErrorType
CjvxObject::_start_lock()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	res = _start();
	if (res != JVX_NO_ERROR)
	{
		JVX_UNLOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	}
	return(res);
}

// ===========================================================================

jvxErrorType
CjvxObject::_stop_lock()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	res = _stop();
	if (res != JVX_NO_ERROR)
	{
		JVX_UNLOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	}
	return(res);
}

// ===========================================================================

jvxErrorType
CjvxObject::_postprocess_lock()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	res = _postprocess();
	if (res != JVX_NO_ERROR)
	{
		JVX_UNLOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	}
	return(res);
}

// ===========================================================================

jvxErrorType
CjvxObject::_deactivate()
{
	jvxErrorType resL = JVX_NO_ERROR;
	resL = CjvxObjectMin::_deactivate();
	if (resL == JVX_NO_ERROR)
	{
#ifdef JVX_OBJECTS_WITH_TEXTLOG
		_return_text_log();
#endif

		if (_common_set.theReport)
		{
			resL = _common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT,
				reinterpret_cast<jvxHandle**>(_common_set.theReport));
			_common_set.theReport = nullptr;
		}

		if (_common_set.theToolsHost)
		{
			resL = _common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST,
				reinterpret_cast<jvxHandle*>(_common_set.theToolsHost));
			_common_set.theToolsHost = NULL;
		}

		if (_common_set.theUniqueId)
		{
			resL = _common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_UNIQUE_ID,
				reinterpret_cast<jvxHandle*>(_common_set.theUniqueId));
			_common_set.theUniqueId = NULL;
		}

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObject::_deactivate_no_text_log()
{
	jvxErrorType resL = JVX_NO_ERROR;
	resL = CjvxObjectMin::_deactivate();
	if (resL == JVX_NO_ERROR)
	{		
		if (_common_set.theReport)
		{
			resL = _common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT,
				reinterpret_cast<jvxHandle**>(_common_set.theReport));
			_common_set.theReport = nullptr;
		}

		if (_common_set.theToolsHost)
		{
			resL = _common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST,
				reinterpret_cast<jvxHandle*>(_common_set.theToolsHost));
			_common_set.theToolsHost = NULL;
		}

		if (_common_set.theUniqueId)
		{
			resL = _common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_UNIQUE_ID,
				reinterpret_cast<jvxHandle*>(_common_set.theUniqueId));
			_common_set.theUniqueId = NULL;
		}

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxObject::_deactivate_lock()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	res = _deactivate();
	if (res != JVX_NO_ERROR)
	{
		JVX_UNLOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	}
	return(res);
}

// ===========================================================================

jvxErrorType
CjvxObject::_unselect()
{
	jvxErrorType resL = CjvxObjectMin::_unselect();
	if (resL == JVX_NO_ERROR)
	{	
		_set_location_info(jvxComponentIdentification(JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED));
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}



jvxErrorType
CjvxObject::_set_user_data(jvxSize idUserData, jvxHandle* floating_pointer)
{
	std::map<jvxSize, jvxHandle*>::iterator elm = _common_set.registeredUserData.find(idUserData);
	if (floating_pointer)
	{
		if (elm == _common_set.registeredUserData.end())
		{
			_common_set.registeredUserData[idUserData] = floating_pointer;
			return JVX_NO_ERROR;
		}
	}
	else
	{
		if (elm != _common_set.registeredUserData.end())
		{
			_common_set.registeredUserData.erase(elm);
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType
CjvxObject::_user_data(jvxSize idUserData, jvxHandle** floating_pointer)
{
	std::map<jvxSize, jvxHandle*>::iterator elm = _common_set.registeredUserData.find(idUserData);
	if (elm != _common_set.registeredUserData.end())
	{
		if (floating_pointer)
		{
			*floating_pointer = elm->second;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxObject::_lock_state()
{
	JVX_LOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxObject::_try_lock_state()
{
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE res = JVX_TRY_LOCK_MUTEX_SUCCESS;
	JVX_TRY_LOCK_MUTEX(res, _common_set._safeAccessStateBound.the_lock);
	if (res == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_COMPONENT_BUSY);
}

jvxErrorType
CjvxObject::_unlock_state()
{
	JVX_UNLOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
	return(JVX_NO_ERROR);
};

jvxErrorType
CjvxObject::_report_text_message(const char* txt, jvxReportPriority prio)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theHostRef)
	{
		IjvxReport* theReport = NULL;
		res = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
		if ((res == JVX_NO_ERROR) && theReport)
		{
			theReport->report_simple_text_message(txt, prio);
			_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReport));
		}
	}
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
}

jvxErrorType
CjvxObject::_report_command_request(
	jvxCBitField req, jvxHandle* caseSpecificData,
	jvxSize szSpecificData)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theHostRef)
	{
		IjvxReport* theReport = NULL;
		res = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
		if ((res == JVX_NO_ERROR) && theReport)
		{
			theReport->report_command_request(req, caseSpecificData, szSpecificData);
			_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReport));
		}
	}
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
}

jvxErrorType
CjvxObject::_request_command(const CjvxReportCommandRequest& request)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theHostRef)
	{
		IjvxReport* theReport = reqInterface<IjvxReport>(_common_set_min.theHostRef);
		IjvxReportSystem* theReportSystem = reqInterface<IjvxReportSystem>(_common_set_min.theHostRef);
		IjvxReportSystem* theReportSystemUse = theReportSystem;
		if (theReport)
		{
			if (!theReportSystemUse)
			{
				theReportSystemUse = static_cast<IjvxReportSystem*>(theReport);
			}
		}
		if (theReportSystemUse)
		{
			res = theReportSystemUse->request_command(request);
		}
		if (theReport)
		{
			retInterface<IjvxReport>(_common_set_min.theHostRef, theReport);
		}
		if (theReportSystem)
		{
			retInterface<IjvxReportSystem>(_common_set_min.theHostRef, theReportSystem);
		}
	}
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
}

jvxErrorType
CjvxObject::_report_property_has_changed(jvxPropertyCategoryType cat, jvxSize idProp, jvxBool onlyCont,
	jvxSize offset_start, jvxSize numElements,
	const jvxComponentIdentification& cpTo = JVX_COMPONENT_UNKNOWN, jvxPropertyCallPurpose callpurp)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theHostRef)
	{
		IjvxReport* theReport = NULL;
		res = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
		if ((res == JVX_NO_ERROR) && theReport)
		{
			theReport->report_internals_have_changed(_common_set.theComponentType, _common_set.thisisme, cat, idProp, onlyCont,
				offset_start, numElements, cpTo, callpurp);
			_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReport));
		}
	}
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
}

jvxErrorType
CjvxObject::_request_test_chain_master(jvxSize processId)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theHostRef)
	{
		IjvxReport* theReport = NULL;
		res = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
		if ((res == JVX_NO_ERROR) && theReport)
		{
			theReport->report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT,
				(jvxHandle*)((intptr_t)processId), 0);

			_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReport));
		}
	}
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
}

jvxErrorType
CjvxObject::_report_take_over_property(jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent, jvxPropertyCategoryType cat, jvxSize idProp, const jvxComponentIdentification& cpTo)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theHostRef)
	{
		IjvxReport* theReport = NULL;
		res = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
		if ((res == JVX_NO_ERROR) && theReport)
		{
			res = theReport->report_take_over_property(_common_set.theComponentType, _common_set.thisisme,
				fld, numElements, format, offsetStart, onlyContent,
				cat, idProp, cpTo);
			_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReport));
		}
	}
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
}

jvxErrorType
CjvxObject::_number_info_tokens(jvxSize* num)
{
	if (num)
	{
		*num = _common_set.theInformation.size();
	}
	return(JVX_NO_ERROR);
}


jvxErrorType
CjvxObject::_info_token(jvxSize idx, jvxApiString* fldStr)
{
	if (idx < _common_set.theInformation.size())
	{
		if (fldStr)
		{
			fldStr->assign(_common_set.theInformation[idx]);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
}

#ifdef JVX_OBJECTS_WITH_TEXTLOG
void
CjvxObject::_request_text_log()
{
	embLog.jvxrtst_bkp.theToolsHost = _common_set.theToolsHost;
	if (embLog.jvxrtst_bkp.theToolsHost)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(embLog.jvxrtst_bkp.jvxlst_buf, char, JVX_COBJECT_OS_BUF_SIZE);
		embLog.jvxrtst_bkp.jvxlst_buf_sz = JVX_COBJECT_OS_BUF_SIZE;

		jvx_request_text_log(embLog);
	}
	else
	{
		std::cout << "Error: Failed to open logger object for component< " << _common_set.theDescriptor << "> since tools host reference is not vailable." << std::endl;
	}
};

void CjvxObject::_return_text_log()
{
	jvx_return_text_log(embLog);
	if (embLog.jvxrtst_bkp.jvxlst_buf)
	{
		JVX_DSP_SAFE_DELETE_FIELD(embLog.jvxrtst_bkp.jvxlst_buf);
	}
	embLog.jvxrtst_bkp.jvxlst_buf = NULL;
	embLog.jvxrtst_bkp.jvxlst_buf_sz = 0;
}

#endif

jvxErrorType
CjvxObject::_request_unique_object_id(jvxSize* uId)
{
	if (uId)
	{
		*uId = _common_set.register_host_id;
	}
	return JVX_NO_ERROR;
}

IjvxReport*
CjvxObject::_request_report()
{
	IjvxReport* theReport = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theHostRef)
	{
		res = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
		if (res == JVX_NO_ERROR)
		{
			return theReport;
		}
		else
		{
			theReport = NULL;
		}
	}
	return theReport;
}

void
CjvxObject::_release_report(IjvxReport* theReport)
{
	if (_common_set_min.theHostRef)
	{
		_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReport));
	}
}

jvxErrorType
CjvxObject::_increment_reference()
{
	refCnt++;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxObject::_current_reference(jvxSize* cnt)
{
	if (cnt)
	{
		*cnt = refCnt;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxObject::_decrement_reference()
{
	if (refCnt > 0)
	{
		refCnt--;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

// ====================================================================

jvxErrorType
CjvxObject::report_properties_modified(const char* props_set)
{
	// Report property
	if (_common_set.theReport)
	{
		CjvxReportCommandRequest_id newRequest(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY,
			_common_set.theComponentType, props_set);
		// 
		_common_set.theReport->request_command(newRequest);
	}
	return JVX_NO_ERROR;
}
