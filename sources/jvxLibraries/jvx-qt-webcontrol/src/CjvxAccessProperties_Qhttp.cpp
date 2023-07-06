#include "CjvxAccessProperties_Qhttp.h"
extern "C"
{
#include "jvx-crc.h"
}

CjvxAccessProperties_Qhttp::CjvxAccessProperties_Qhttp(): QObject(), JVX_INIT_RT_ST_INSTANCES
{
	myUniqueId_groups = 1;
	httpRefPtr = NULL;
	tpRef = JVX_COMPONENT_UNKNOWN;
	theState = JVX_STATE_NONE;
}

CjvxAccessProperties_Qhttp::~CjvxAccessProperties_Qhttp() 
{
}

jvxErrorType 
CjvxAccessProperties_Qhttp::description_object(jvxApiString* astr)
{
	if (astr)
		astr->assign(descrionRef);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAccessProperties_Qhttp::descriptor_object(jvxApiString* astr, jvxApiString* tagstr)
{
	if (astr)
		astr->assign(descrorRef);
	if (tagstr)
		tagstr->assign(tagRef);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAccessProperties_Qhttp::initialize(IjvxHttpApi* httpRef, 
	jvxComponentIdentification tp, const char* description, 
	const char* descriptor, const char* tag,
	IjvxFactoryHost* hFHostRef)
{
	if (theState == JVX_STATE_NONE)
	{
		httpRefPtr = httpRef;
		httpRef->unblockCheckPending();
		tpRef = tp;
		descrorRef = descriptor;
		descrionRef = description;
		tagRef = tag;
		theState = JVX_STATE_INIT;
		hFHost = hFHostRef;

#ifdef JVX_OBJECTS_WITH_TEXTLOG

		if (hFHost)
		{
			// Reset the logfile module
			jvx_init_text_log(jvxrtst_bkp);

			jvxrtst_bkp.set_module_name("CjvxAccessProperties_Qhttp");

			hFHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&jvxrtst_bkp.theToolsHost));
			if (jvxrtst_bkp.theToolsHost)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(jvxrtst_bkp.jvxlst_buf, char, JVX_COBJECT_OS_BUF_SIZE);
				jvxrtst_bkp.jvxlst_buf_sz = JVX_COBJECT_OS_BUF_SIZE;
				jvx_request_text_log(jvxrtst_bkp);
			}
		}
#endif
		connect(this, SIGNAL(emit_reschedule_all_props_in(jvxSize)), this, SLOT(reschedule_all_props_in(jvxSize)), Qt::QueuedConnection);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxAccessProperties_Qhttp::terminate(IjvxHttpApi** httpRef, jvxComponentIdentification* tp)
{
	if (theState == JVX_STATE_INIT)
	{
#ifdef JVX_OBJECTS_WITH_TEXTLOG

		if (jvxrtst_bkp.theToolsHost)
		{
			jvx_return_text_log(jvxrtst_bkp);
			JVX_DSP_SAFE_DELETE_FIELD(jvxrtst_bkp.jvxlst_buf);

			jvxrtst_bkp.jvxlst_buf = NULL;
			jvxrtst_bkp.jvxlst_buf_sz = 0;
			jvxrtst_bkp.dbgModule = false;

			hFHost->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(jvxrtst_bkp.jvxlst_buf));
		}
		jvxrtst_bkp.jvxlst_buf = NULL;
		jvx_terminate_text_log(jvxrtst_bkp);
#endif

		// Cancel all currently running messages
		httpRefPtr->request_kill_all();

		allPropsCached.clear();

		disconnect(this, SIGNAL(emit_reschedule_all_props_in(jvxSize)));

		// myUniqueId_groups = 1; <-> No, keep the counter for next restart! 
		if (httpRef)
			*httpRef = httpRefPtr;
		if (tp)
			*tp = tpRef;

		httpRefPtr = NULL;
		tpRef = JVX_COMPONENT_UNKNOWN;
		theState = JVX_STATE_NONE;
		hFHost = nullptr;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxAccessProperties_Qhttp::block_check_pending(jvxBool* isPending)
{
	if (httpRefPtr)
		return httpRefPtr->blockCheckPending(isPending);
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxAccessProperties_Qhttp::set_user_data(jvxSize idUserData, jvxHandle* floating_pointer)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	return res;
};

jvxErrorType
CjvxAccessProperties_Qhttp::user_data(jvxSize idUserData, jvxHandle** floating_pointer)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	return res;
};

jvxErrorType
CjvxAccessProperties_Qhttp::state_object(jvxState* stat)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	return res;
};

jvxErrorType
CjvxAccessProperties_Qhttp::state(jvxState* theStat)
{
	if (theStat)
		*theStat = theState;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAccessProperties_Qhttp::reference_status(jvxBool* hasValidObjectRef, jvxBool* hasValidPropertyRef)
{
	if(hasValidObjectRef)
		*hasValidObjectRef = true;
	if (hasValidPropertyRef)
		*hasValidPropertyRef = true;
	return JVX_NO_ERROR;
}


jvxErrorType 
CjvxAccessProperties_Qhttp::get_features(jvxBitField* theFeatures)
{
	if (theFeatures)
	{
		jvx_bitZSet(*theFeatures, JVX_PROPERTY_ACCESS_DELAYED_RESPONSE_SHIFT);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAccessProperties_Qhttp::get_reference_component_description(jvxApiString* descror, jvxApiString* descrion, jvxApiString* modref, jvxComponentIdentification* cpId)
{
	if (cpId)
	{
		*cpId = tpRef;
	}
	if (descror)
	{
		descror->assign(descrorRef);
	}
	if (descrion)
	{
		descrion->assign(descrionRef);
	}
	if (modref)
	{
		modref->assign("Not Implemented!"); // moduleRef
	}
	return JVX_NO_ERROR;
}

/*
jvxErrorType 
CjvxAccessProperties_Qhttp::get_property_description_simple__descriptor(jvxSize* numElements, 
	jvxDataFormat* format, const char* descriptor, jvxSize* processId,
	jvxCallManagerProperties& callGate)
{
	auto elm = thePropCache.find(descriptor);
	if (elm == thePropCache.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	if (format)
	{
		*format = elm->second.format;
	}
	if (numElements)
	{
		*numElements = elm->second.num;
	}

	return JVX_NO_ERROR;
}
*/

jvxErrorType
CjvxAccessProperties_Qhttp::addRequestToGroup(
	oneAssociatedGroup& entry,
	const std::string& descriptor,
	jvxOneAssociatedRequestType tp,
	const std::string& httparg,
	jvxBool contentOnly,
	jvxContext* ctxt
	//jvxPropertyCategoryType cat,
	//jvxSize id
	)
{
	std::list<oneRequestEntry>::iterator elmD;
	oneRequestEntry newReq;
	oneRequestEntry searchme;

	//newReq.prop_id = id;
	//newReq.prop_cat = cat;
	newReq.stat = JVX_STATE_INIT;
	newReq.json_response = "";
	newReq.http_arg = httparg;
	newReq.req_type = tp;
	newReq.tag = descriptor; 
	newReq.errRes = JVX_ERROR_INTERNAL;
	newReq.contentOnly = contentOnly;
	if (ctxt)
	{
		newReq.ctxt = *ctxt;
	}
	
	entry.requests[entry.myUniqueId_inGroup] = newReq;
	entry.myUniqueId_inGroup++;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAccessProperties_Qhttp::get_descriptor_property(
	jvxCallManagerProperties& callGate,
	jvx::propertyDescriptor::IjvxPropertyDescriptor& theDescr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertySpecifics::IjvxConnectionType& spec)
{
/*
jvxErrorType
CjvxAccessProperties_Qhttp::get_property_descriptor__descriptor(jvxPropertyDescriptor& theStr, 
	const char* descriptor, jvxSize* processId,
	jvxCallManagerProperties& callGate)
{*/
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	jvxBool allRequestsGone = false;
	std::map<jvxSize, oneRequestEntry>::iterator elmD;
	oneRequestEntry newReq;
	oneRequestEntry searchReq;
	jvxSize* processId = nullptr;
	const jvx::propertySpecifics::CjvxConnectionTypeHttp* httpParams = castPropSpecific< const jvx::propertySpecifics::CjvxConnectionTypeHttp>(&spec);
	if (httpParams)
	{
		processId = httpParams->ptrIdHttp;
	}

	if (!processId)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	const jvx::propertyAddress::CjvxPropertyAddressLinear* addrLin = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressLinear>(&ident);
	const jvx::propertyAddress::CjvxPropertyAddressDescriptor* addrDescr = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressDescriptor>(&ident);
	const jvx::propertyAddress::CjvxPropertyAddressGlobalId* addrIdx = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressGlobalId>(&ident);

	jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus* dFullPlus = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus>(&theDescr);
	jvx::propertyDescriptor::CjvxPropertyDescriptorFull* dFull = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorFull>(&theDescr);
	jvx::propertyDescriptor::CjvxPropertyDescriptorControl* dContr = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorControl>(&theDescr);
	jvx::propertyDescriptor::CjvxPropertyDescriptorCore* dCore = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorCore>(&theDescr);

	if (addrDescr)
	{
		if (!dFull)
		{
			assert(0);
		}

		std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.begin();
		elm = thegroups.find(*processId);
		if (elm != thegroups.end())
		{
			switch (elm->second.currentMode)
			{
			case JVX_ACCESS_PROPERTY_MODE_COLLECT:

				res = addRequestToGroup(elm->second, addrDescr->descriptor, JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_DESCRIPTION, "", false, NULL);
				if (res == JVX_NO_ERROR)
				{
					return JVX_ERROR_POSTPONE; // <- This is good!
				}
				break;
			case JVX_ACCESS_PROPERTY_MODE_READOUT:
				searchReq.req_type = JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_DESCRIPTION;
				searchReq.tag = addrDescr->descriptor;
				//searchReq.prop_id = JVX_SIZE_UNSELECTED;
				//searchReq.prop_cat = JVX_PROPERTY_CATEGORY_UNKNOWN;

				elmD = elm->second.requests.begin();
				for (; elmD != elm->second.requests.end(); elmD++)
				{
					if (elmD->second == searchReq)
					{
						// Transform response
						if (elmD->second.errRes == JVX_NO_ERROR)
						{
							// Preset the descriptor	
							dFull->descriptor = addrDescr->descriptor;
							res = convertResponseToContent__descriptor(theDescr, *addrDescr,
								elmD->second, &callGate.access_protocol);
						}
						else
						{
							res = elmD->second.errRes;
						}
						elmD->second.stat = JVX_STATE_NONE;
						elm->second.requests.erase(elmD);
						foundit = true;
						break;
					}
				}
				//if (res != JVX_NO_ERROR)
				//{
				//	std::cout << "Hier!" << std::endl;
				//}
				if (!foundit)
				{
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvxrtst << __FUNCTION__ << ": Error: requesting readout for descriptor <" << addrDescr->descriptor << "> which is not available." << std::endl;
					}
					std::cout << __FUNCTION__ << ": Error: requesting readout for descriptor <" << addrDescr->descriptor << "> which is not available." << std::endl;
					return JVX_ERROR_ELEMENT_NOT_FOUND;
				}

				// =============================================
				if (elm->second.requests.empty())
				{
					elm->second.currentMode = JVX_ACCESS_PROPERTY_MODE_INIT;
					elm->second.requests.clear();
					allRequestsGone = true;
				}

				if (allRequestsGone)
				{
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvxrtst << __FUNCTION__ << " Erasing property group <" << elm->first << ">." << std::endl;
					}
					thegroups.erase(elm);
				}
				break;
			default:
				return JVX_ERROR_WRONG_STATE;
			}
		}
		else
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvxrtst << __FUNCTION__ << ": Error: received invalid unique id within group: " << *processId << std::endl;
			}
			std::cout << __FUNCTION__ << ": Error: received invalid unique id within group: " << *processId << std::endl;
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else if (addrLin)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		if (addrLin->idx < thePropCache.size())
		{
			auto elm = thePropCache.begin();
			std::advance(elm, addrLin->idx);

			if (dFullPlus)
			{
				dFullPlus->accessFlags = elm->second.accessFlags;
				dFullPlus->configModeFlags = elm->second.configModeFlags;
			}
			if (dContr)
			{
				dContr->allowedStateMask = elm->second.allowedStateMask;
				dContr->allowedThreadingMask = elm->second.allowedThreadingMask;
				dContr->isValid = elm->second.isValid;
				dContr->invalidateOnStateSwitch = elm->second.invalidateOnStateSwitch;
				dContr->installable = elm->second.installable;
			}
			if (dFull)
			{
				dFull->description = elm->second.description;
				dFull->descriptor = elm->second.descriptor;
				dFull->name = elm->second.name;
			}
			if (dCore)
			{
				dCore->accessType = elm->second.accessType;
				dCore->category = elm->second.category;
				dCore->format = elm->second.format;
				dCore->num = elm->second.num;
				dCore->accessType = elm->second.accessType;
				dCore->decTp = elm->second.decTp;
				dCore->globalIdx = elm->second.globalIdx;
				dCore->ctxt = elm->second.ctxt;
			}
			res = JVX_NO_ERROR;
		}
	}
	else if (addrIdx)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		for (auto& elm : thePropCache)
		{
			if (
				(elm.second.globalIdx == addrIdx->id) &&
				(elm.second.category == addrIdx->cat))
			{
				if (dFullPlus)
				{
					dFullPlus->accessFlags = elm.second.accessFlags;
					dFullPlus->configModeFlags = elm.second.configModeFlags;
				}
				if (dContr)
				{
					dContr->allowedStateMask = elm.second.allowedStateMask;
					dContr->allowedThreadingMask = elm.second.allowedThreadingMask;
					dContr->isValid = elm.second.isValid;
					dContr->invalidateOnStateSwitch = elm.second.invalidateOnStateSwitch;
					dContr->installable = elm.second.installable;
				}
				if (dFull)
				{
					dFull->description = elm.second.description;
					dFull->descriptor = elm.second.descriptor;
					dFull->name = elm.second.name;
				}
				if (dCore)
				{
					dCore->accessType = elm.second.accessType;
					dCore->category = elm.second.category;
					dCore->format = elm.second.format;
					dCore->num = elm.second.num;
					dCore->accessType = elm.second.accessType;
					dCore->decTp = elm.second.decTp;
					dCore->globalIdx = elm.second.globalIdx;
					dCore->ctxt = elm.second.ctxt;
				}
				res = JVX_NO_ERROR;
				break;
			}
		}
	}

	return res;
}

jvxErrorType
CjvxAccessProperties_Qhttp::create_refresh_property_cache(jvxCallManagerProperties& callGate,
	const char* descriptor_expr,
	const jvx::propertySpecifics::IjvxConnectionType& spec )
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	jvxBool allRequestsGone = false;
	std::map<jvxSize, oneRequestEntry>::iterator elmD;
	oneRequestEntry newReq;
	oneRequestEntry searchReq;

	jvxSize* processId = nullptr;
	const jvx::propertySpecifics::CjvxConnectionTypeHttp* httpParams = castPropSpecific< const jvx::propertySpecifics::CjvxConnectionTypeHttp>(&spec);
	if (httpParams)
	{
		processId = httpParams->ptrIdHttp;
	}
	if (!processId)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.begin();
	elm = thegroups.find(*processId);
	if (elm != thegroups.end())
	{
		switch (elm->second.currentMode)
		{
		case JVX_ACCESS_PROPERTY_MODE_COLLECT:

			res = addRequestToGroup(elm->second, descriptor_expr, JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_PROPERTY_LIST, "", false, callGate.context);
			if (res == JVX_NO_ERROR)
			{
				return JVX_ERROR_POSTPONE; // <- This is good!
			}
			break;
		case JVX_ACCESS_PROPERTY_MODE_READOUT:
			searchReq.req_type = JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_PROPERTY_LIST;
			searchReq.tag = descriptor_expr;
			//searchReq.prop_id = JVX_SIZE_UNSELECTED;
			//searchReq.prop_cat = JVX_PROPERTY_CATEGORY_UNKNOWN;

			elmD = elm->second.requests.begin();
			for (; elmD != elm->second.requests.end(); elmD++)
			{
				if (elmD->second == searchReq)
				{
					// Transform response
					if (elmD->second.errRes == JVX_NO_ERROR)
					{
						thePropCache.clear();
						jPAD ident(descriptor_expr);
						res = convertResponseToContent__proplist(thePropCache,
							ident,
							elmD->second,
							&callGate.access_protocol);
					}
					else
					{
						res = elmD->second.errRes;
					}
					elmD->second.stat = JVX_STATE_NONE;
					elm->second.requests.erase(elmD);
					foundit = true;
					break;
				}
			}
			if (!foundit)
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvxrtst << __FUNCTION__ << ": Error: requesting readout for descriptor <" << descriptor_expr << "> which is not available." << std::endl;
				}

				std::cout << __FUNCTION__ << ": Error: requesting readout for descriptor <" << descriptor_expr << "> which is not available." << std::endl;
				return JVX_ERROR_ELEMENT_NOT_FOUND;
			}

			// =============================================
			if (elm->second.requests.empty())
			{
				elm->second.currentMode = JVX_ACCESS_PROPERTY_MODE_INIT;
				elm->second.requests.clear();
				allRequestsGone = true;
			}

			if (allRequestsGone)
			{
				assert(elm->second.response);
				elm->second.response->report_nonblocking_delayed_update_terminated(elm->first, elm->second.priv);

				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvxrtst << __FUNCTION__ << " Erasing property group <" << elm->first << ">." << std::endl;
				}
				thegroups.erase(elm);
			}
			break;
		default:
			return JVX_ERROR_WRONG_STATE;
		}
	}
	else
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvxrtst << __FUNCTION__ << ": Error: received invalid unique id within group: " << *processId << std::endl;
		}
		std::cout << __FUNCTION__ << ": Error: received invalid unique id within group: " << *processId << std::endl;
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	return res;
}

jvxErrorType
CjvxAccessProperties_Qhttp::clear_property_cache()
{
	jvxErrorType res = JVX_NO_ERROR;

	thePropCache.clear();
	return res;
}

jvxErrorType 
CjvxAccessProperties_Qhttp::get_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans,
	const jvx::propertySpecifics::IjvxConnectionType& spec)

{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	jvxBool allRequestsGone = false;
	std::map<jvxSize, oneRequestEntry>::iterator elmD;
	oneRequestEntry newReq;
	oneRequestEntry searchReq;
	jvxSize* processId = nullptr;
	const jvx::propertySpecifics::CjvxConnectionTypeHttp* httpParams = castPropSpecific< const jvx::propertySpecifics::CjvxConnectionTypeHttp>(&spec);
	if (httpParams)
	{
		processId = httpParams->ptrIdHttp;
	}
	if (!processId)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	const jvx::propertyAddress::CjvxPropertyAddressDescriptor* ptrDescr = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressDescriptor>(&ident);

	if (ptrDescr)
	{
		JVX_TRANSLATE_PROP_GENERIC_FROM_RAW(rawPtr);
		std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.begin();
		elm = thegroups.find(*processId);
		if (elm != thegroups.end())
		{
			switch (elm->second.currentMode)
			{
			case JVX_ACCESS_PROPERTY_MODE_COLLECT:

				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvxrtst << __FUNCTION__ << ": Added request id <" << elm->first << "/" << elm->second.myUniqueId_inGroup << ">- " << ptrDescr->descriptor << " to group." << std::endl;
				}
				res = addRequestToGroup(elm->second, ptrDescr->descriptor, JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_CONTENT, "", trans.contentOnly, callGate.context);
				if (res == JVX_NO_ERROR)
				{
					return JVX_ERROR_POSTPONE; // <- This is good!
				}
				break;
			case JVX_ACCESS_PROPERTY_MODE_READOUT:
				searchReq.req_type = JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_CONTENT;
				searchReq.tag = ptrDescr->descriptor;
				//searchReq.prop_id = JVX_SIZE_UNSELECTED;
				//searchReq.prop_cat = JVX_PROPERTY_CATEGORY_UNKNOWN;

				elmD = elm->second.requests.begin();
				for (; elmD != elm->second.requests.end(); elmD++)
				{
					if (elmD->second == searchReq)
					{
						// Transform response
						if (elmD->second.errRes == JVX_NO_ERROR)
						{
							res = convertResponseToContent__get_content(fld,
								trans.offsetStart, numElements, format,
								trans.contentOnly, trans.decTp, 
								ptrDescr->descriptor, elmD->second,
								&callGate.access_protocol, callGate);
						}
						else
						{
							res = elmD->second.errRes;
						}
						elmD->second.stat = JVX_STATE_NONE;
						if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
						{
							jvxrtst << __FUNCTION__ << ": Removing request id <" << elm->first << "/" << elmD->first << ">-" << ptrDescr->descriptor << " from group." << std::endl;
						}

						elm->second.requests.erase(elmD);
						foundit = true;
						break;
					}
				}
				if (!foundit)
				{
					// This may happen as follows:
					// The property was triggered to be updated first. Then, while updating, another request was triggered.
					// The pending update prevents the next update to happen. Therefore the group is started but isolated
					// requests have been blocked. Now, once the secondary update is complete, all properties are read.
					// Those who were blocked, however, are not available since the trigger did not add those to the 
					// requested group. Hence, this is not an error but a warning!
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvxrtst << __FUNCTION__ << ": Warning: requesting readout for descriptor <" << ptrDescr->descriptor << "> which is not available." << std::endl;
					}
					return JVX_ERROR_ELEMENT_NOT_FOUND;
				}

				// =============================================
				if (elm->second.requests.empty())
				{
					elm->second.currentMode = JVX_ACCESS_PROPERTY_MODE_INIT;
					elm->second.requests.clear();
					allRequestsGone = true;
				}

				if (allRequestsGone)
				{
					assert(elm->second.response);
					elm->second.response->report_nonblocking_delayed_update_terminated(elm->first, elm->second.priv);

					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvxrtst << __FUNCTION__ << " Erasing property group <" << elm->first << ">." << std::endl;
					}
					thegroups.erase(elm);
				}
				break;
			default:
				return JVX_ERROR_WRONG_STATE;
			}
		}
		else
		{
			// This may happen as follows:
			// The property was triggered to be updated first. Then, while updating, another request was triggered.
			// The pending update prevents the next update to happen. Therefore the group is started but isolated
			// requests have been blocked. Now, once the secondary update is complete, all properties are read.
			// Those who were blocked, however, are not available since the trigger did not add those to the 
			// requested group. At first this event is reported when reading out the isolated property. However, if 
			// the property is one of the last in a group, even the complete group id has been removed already
			// if all requested properties have been handled. In that case, we end up here
			// This is not an error but a warning!
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvxrtst << __FUNCTION__ << ": Warning: received invalid unique id within group: " << *processId << std::endl;
			}
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		return JVX_ERROR_UNSUPPORTED;
	}

	return res;
}

/*
jvxErrorType 
CjvxAccessProperties_Qhttp::set_property__descriptor(jvxHandle* fld, jvxSize offsetStart,
			jvxSize numElements, jvxDataFormat format, jvxBool contentOnly,
			const char* descriptor, jvxPropertyDecoderHintType decTp, jvxSize* processId, jvxBool binList,
	jvxCallManagerProperties& callGate)

	jvxErrorType
CjvxAccessProperties_Qhttp::set_property__id(jvxHandle* fld,
	jvxSize numElements, jvxDataFormat, jvxSize handleIdx,
	jvxPropertyCategoryType category, jvxSize offsetStart,
	jvxBool onlyContent, jvxSize* processId, jvxBool binList,
	jvxCallManagerProperties& callGate)
{
	return JVX_ERROR_UNSUPPORTED;
}
*/
jvxErrorType CjvxAccessProperties_Qhttp::set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans,
	const jvx::propertySpecifics::IjvxConnectionType& spec)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	jvxBool allRequestsGone = false;
	std::map<jvxSize, oneRequestEntry>::iterator elmD;
	oneRequestEntry newReq;
	oneRequestEntry searchReq;
	std::string httparg;
	std::string httpargPP;
	jvxSize* processId = nullptr;
	const jvx::propertySpecifics::CjvxConnectionTypeHttp* httpParams = castPropSpecific< const jvx::propertySpecifics::CjvxConnectionTypeHttp>(&spec);
	if (httpParams)
	{
		processId = httpParams->ptrIdHttp;
	}

	if (!processId)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	const jvx::propertyAddress::CjvxPropertyAddressDescriptor* ptrDescr = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressDescriptor>(&ident);

	if (ptrDescr)
	{
		JVX_TRANSLATE_PROP_GENERIC_FROM_RAW(rawPtr);
		const char* descriptor = ptrDescr->descriptor;
		std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.begin();
		elm = thegroups.find(*processId);
		if (elm != thegroups.end())
		{
			switch (elm->second.currentMode)
			{
			case JVX_ACCESS_PROPERTY_MODE_COLLECT:

				jvx::helper::properties::toString(fld, trans.offsetStart, numElements, format,
					trans.contentOnly, trans.decTp, httparg,
					httpargPP, 4, trans.binList);

				if ((numElements > 1) && trans.binList)
				{
					httparg += httpargPP + "<>";
				}
				switch (format)
				{
				case JVX_DATAFORMAT_STRING_LIST:
					httparg = httpargPP;
					break;
				case JVX_DATAFORMAT_SELECTION_LIST:
					if (!trans.contentOnly)
					{
						httpargPP = jvx_replaceCharacter(httpargPP, ',', ':');
						httparg += "::" + httpargPP;
					}
					break;

					// array with base64 encode should not be used here since bin_list is always "false"
				default:
					break;
				}

				res = addRequestToGroup(elm->second, descriptor, JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_SET_CONTENT,
					httparg, trans.contentOnly, callGate.context);
				if (res == JVX_NO_ERROR)
				{
					return JVX_ERROR_POSTPONE; // <- This is good!
				}
				break;
			case JVX_ACCESS_PROPERTY_MODE_READOUT:
				searchReq.req_type = JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_SET_CONTENT;
				searchReq.tag = descriptor;
				//searchReq.prop_id = JVX_SIZE_UNSELECTED;
				//searchReq.prop_cat = JVX_PROPERTY_CATEGORY_UNKNOWN;

				elmD = elm->second.requests.begin();
				for (; elmD != elm->second.requests.end(); elmD++)
				{
					if (elmD->second == searchReq)
					{
						// Transform response
						if (elmD->second.errRes == JVX_NO_ERROR)
						{
							res = convertResponseToContent__set_content(fld,
								trans.offsetStart, numElements, format,
								trans.contentOnly, trans.decTp, descriptor, elmD->second,
								&callGate.access_protocol);
						}
						else
						{
							res = elmD->second.errRes;
						}
						elmD->second.stat = JVX_STATE_NONE;
						elm->second.requests.erase(elmD);
						foundit = true;
						break;
					}
				}
				if (!foundit)
				{
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvxrtst << __FUNCTION__ << ": Error: requesting readout for descriptor <" << descriptor << "> which is not available." << std::endl;
					}
					std::cout << __FUNCTION__ << ": Error: requesting readout for descriptor <" << descriptor << "> which is not available." << std::endl;
					return JVX_ERROR_ELEMENT_NOT_FOUND;
				}

				// =============================================
				if (elm->second.requests.empty())
				{
					elm->second.currentMode = JVX_ACCESS_PROPERTY_MODE_INIT;
					elm->second.requests.clear();
					allRequestsGone = true;
				}

				if (allRequestsGone)
				{
					assert(elm->second.response);
					elm->second.response->report_nonblocking_delayed_update_terminated(elm->first, elm->second.priv);
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvxrtst << __FUNCTION__ << " Erasing property group <" << elm->first << ">." << std::endl;
					}
					thegroups.erase(elm);
				}
				break;
			default:
				return JVX_ERROR_WRONG_STATE;
			}
		}
		else
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvxrtst << __FUNCTION__ << ": Error: received invalid unique id within group: " << *processId << std::endl;
			}

			std::cout << __FUNCTION__ << ": Error: received invalid unique id within group: " << *processId << std::endl;
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		return JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

/*
jvxErrorType 
CjvxAccessProperties_Qhttp::set_property_ext_ref__descriptor(jvxExternalBuffer* fld, jvxBool* isValid, const char* descriptor, jvxSize* processId,
	jvxCallManagerProperties& callGate)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxAccessProperties_Qhttp::set_property_ext_ref__id(jvxExternalBuffer* fld,
	jvxBool* isValid, jvxSize propId, jvxPropertyCategoryType category,
	jvxSize* processId, jvxCallManagerProperties& callGate)
{
	return JVX_ERROR_UNSUPPORTED;
}

*/

jvxErrorType
CjvxAccessProperties_Qhttp::install_referene_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertySpecifics::IjvxConnectionType& spec)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxAccessProperties_Qhttp::uninstall_referene_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertySpecifics::IjvxConnectionType& spec)
{
	return JVX_ERROR_UNSUPPORTED;
}

/*
jvxErrorType
CjvxAccessProperties_Qhttp::get_property_descriptor__id(jvxPropertyDescriptor& theStr, jvxSize idGlob, jvxSize* processId,
	jvxCallManagerProperties& callGate)
{
	assert(0);
	return JVX_ERROR_UNSUPPORTED;
}

*/

jvxErrorType
CjvxAccessProperties_Qhttp::get_number_properties(jvxCallManagerProperties& callGate, 
	jvxSize* num, const jvx::propertySpecifics::IjvxConnectionType& spec)
{
	// This function is typically not needed in case of a web connection since the property cache 
	// must be constructed at first
	if (num)
		*num = thePropCache.size();
	callGate.access_protocol = JVX_ACCESS_PROTOCOL_OK;

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAccessProperties_Qhttp::get_property_extended_info(jvxCallManagerProperties& callGate,
	jvxHandle* fld, jvxSize requestId, const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertySpecifics::IjvxConnectionType& spec)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	jvxBool allRequestsGone = false;
	std::map<jvxSize, oneRequestEntry>::iterator elmD;
	oneRequestEntry newReq;
	oneRequestEntry searchReq;
	jvxSize* processId = nullptr;
	const jvx::propertySpecifics::CjvxConnectionTypeHttp* httpParams = castPropSpecific< const jvx::propertySpecifics::CjvxConnectionTypeHttp>(&spec);
	if (httpParams)
	{
		processId = httpParams->ptrIdHttp;
	}
	if (!processId)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	const jvx::propertyAddress::CjvxPropertyAddressDescriptor* ptrDescr = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressDescriptor>(&ident);
	if (ptrDescr)
	{
		std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.begin();
		elm = thegroups.find(*processId);
		if (elm != thegroups.end())
		{
			switch (elm->second.currentMode)
			{
			case JVX_ACCESS_PROPERTY_MODE_COLLECT:

				res = addRequestToGroup(elm->second, ptrDescr->descriptor, JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_EXTENDED_INFO, "", false, NULL);
				if (res == JVX_NO_ERROR)
				{
					return JVX_ERROR_POSTPONE; // <- This is good!
				}
				break;
			case JVX_ACCESS_PROPERTY_MODE_READOUT:
				searchReq.req_type = JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_EXTENDED_INFO;
				searchReq.tag = ptrDescr->descriptor;
				//searchReq.prop_id = propId;
				//searchReq.prop_cat = category;

				elmD = elm->second.requests.begin();
				for (; elmD != elm->second.requests.end(); elmD++)
				{
					if (elmD->second == searchReq)
					{
						if (elmD->second.errRes == JVX_NO_ERROR)
						{
							res = convertResponseToContent__extendedInfo(fld, requestId, elmD->second,
								&callGate.access_protocol);
						}
						else
						{
							res = elmD->second.errRes;
						}
						elmD->second.stat = JVX_STATE_NONE;
						elm->second.requests.erase(elmD);
						foundit = true;
						break;
					}
				}
				if (!foundit)
				{
					return JVX_ERROR_ELEMENT_NOT_FOUND;
				}

				// =============================================
				if (elm->second.requests.empty())
				{
					elm->second.currentMode = JVX_ACCESS_PROPERTY_MODE_INIT;
					elm->second.requests.clear();
					allRequestsGone = true;
				}

				if (allRequestsGone)
				{
					assert(elm->second.response);
					elm->second.response->report_nonblocking_delayed_update_terminated(elm->first, elm->second.priv);
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvxrtst << __FUNCTION__ << " Erasing property group <" << elm->first << ">." << std::endl;
					}
					thegroups.erase(elm);
				}

				break;
			default:
				return JVX_ERROR_WRONG_STATE;
			}
		}
		else
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvxrtst << __FUNCTION__ << ": Error: received invalid unique id within group: " << *processId << std::endl;
			}

			std::cout << __FUNCTION__ << ": Error: received invalid unique id within group: " << *processId << std::endl;
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}

	return res;
}

jvxErrorType 
CjvxAccessProperties_Qhttp::translate_group_id(jvxCallManagerProperties& callGate, jvxSize gId, jvxApiString& astr)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxAccessProperties_Qhttp::property_start_delayed_group(jvxCallManagerProperties& callGate,
	IjvxAccessProperties_delayed_report* theRef, jvxHandle* priv,
	const jvx::propertySpecifics::IjvxConnectionType& spec)
{
	oneAssociatedGroup theNewGroup;
	std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.begin();
	jvxSize* processId = nullptr;
	const jvx::propertySpecifics::CjvxConnectionTypeHttp* httpParams = castPropSpecific< const jvx::propertySpecifics::CjvxConnectionTypeHttp>(&spec);
	if (httpParams)
	{
		processId = httpParams->ptrIdHttp;
	}
	if (!processId)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	if (*processId > 0)
	{
		return JVX_ERROR_COMPONENT_BUSY;
	}

	//currentMode = JVX_ACCESS_PROPERTY_MODE_INIT;
	//myUniqueId = 1;
	//if (currentMode != JVX_ACCESS_PROPERTY_MODE_INIT)
	//{
//		return JVX_ERROR_COMPONENT_BUSY;
//	}

	theNewGroup.currentMode = JVX_ACCESS_PROPERTY_MODE_COLLECT;
	theNewGroup.myUniqueId_inGroup = 1;
	theNewGroup.priv = priv;
	theNewGroup.response = theRef;
	theNewGroup.requests.clear();

	thegroups[myUniqueId_groups] = theNewGroup;
	*processId = myUniqueId_groups;
	myUniqueId_groups++;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAccessProperties_Qhttp::property_stop_delayed_group(jvxCallManagerProperties& callGate, jvxHandle** priv,
	const jvx::propertySpecifics::IjvxConnectionType& spec )
{
	std::string uri;
	jvxErrorType res = JVX_NO_ERROR;
	assert(httpRefPtr);
	jvxSize* processId = nullptr;
	const jvx::propertySpecifics::CjvxConnectionTypeHttp* httpParams = castPropSpecific< const jvx::propertySpecifics::CjvxConnectionTypeHttp>(&spec);
	if (httpParams)
	{
		processId = httpParams->ptrIdHttp;
	}
	if (!processId)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.begin();
	elm = thegroups.find(*processId);
	if (elm != thegroups.end())
	{
		if (elm->second.currentMode != JVX_ACCESS_PROPERTY_MODE_COLLECT)
		{
			return JVX_ERROR_WRONG_STATE;
		}

		if (elm->second.requests.size() == 0)
		{
			// If nothing is to be transmitted, do not start anything and remove object from list
			elm->second.currentMode = JVX_ACCESS_PROPERTY_MODE_INIT;
			if (priv)
				*priv = elm->second.priv;

			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvxrtst << __FUNCTION__ << " Erasing property group <" << elm->first << ">." << std::endl;
			}
			thegroups.erase(elm);
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		elm->second.currentMode = JVX_ACCESS_PROPERTY_MODE_TRANSFER;
		// Transfer all collected requests
		std::map<jvxSize, oneRequestEntry>::iterator elmI = elm->second.requests.begin();
		for (; elmI != elm->second.requests.end(); elmI++)
		{
			res = transform_request_to_uri(uri, elmI->second);
			switch (elmI->second.req_type)
			{
			case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_CONTENT:

				// Get the property content
			/*
			case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_DESCRIPTOR_ONLY: <- I think this construct is no longer used!

				// This returns the descriptor for only the descriptor as passed
			*/
			case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_DESCRIPTION:

				// This will return a FULL descriptor
			case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_EXTENDED_INFO:

				// This will return the extended info only
			case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_PROPERTY_LIST:

				// This comes in via <create_refresh_property_cache> which is used in <trigger_transfer_property_list>
				// It returns all properties as requested by the "wildcard" string as passed in the descriptor
				httpRefPtr->request_transfer_get(uri, elm->first, elmI->first, static_cast<IjvxHttpApi_httpreport*>(this), NULL);
				break;

			case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_SET_CONTENT:
				httpRefPtr->request_transfer_post(uri, elmI->second.http_arg, elm->first, elmI->first, static_cast<IjvxHttpApi_httpreport*>(this), NULL);
				break;
			default:
				assert(0);
			}
		}

		// All entries treanfered
		elm->second.currentMode = JVX_ACCESS_PROPERTY_MODE_WAIT_RESPONSE;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxAccessProperties_Qhttp::get_revision(jvxSize* rev, jvxCallManagerProperties& callGate)
{
	if (rev)
		*rev = JVX_SIZE_UNSELECTED;
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxAccessProperties_Qhttp::transform_request_to_uri(std::string& uri, oneRequestEntry & entry)
{
	jvxSize i;
	std::string addrTarget = "/jvx/host/components/";
	std::string addrComponent;
	std::vector<std::string> query_string_lst;
	jvxErrorType resL = JVX_NO_ERROR;

	uri.clear();

	addrComponent = jvx_replaceCharacter(jvxComponentType_txt(tpRef.tp), ' ', '_');
	addrTarget = jvx_makePathExpr(addrTarget, addrComponent);

	assert(!entry.tag.empty());
	addrTarget = jvx_makePathExpr(addrTarget, entry.tag);

	if (tpRef.slotid != 0)
	{
		query_string_lst.push_back("slotid=" + jvx_int2String(tpRef.slotid));
	}
	
	if (tpRef.slotsubid != 0)
	{
		query_string_lst.push_back("slotsubid=" + jvx_int2String(tpRef.slotsubid));
	}

	switch (entry.req_type)
	{
	case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_CONTENT:
		// no additional text!
		break;
	/*
	case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_DESCRIPTOR_ONLY: <-I think this construct is no longer used!

		// Get only the descriptor for only the specific element
		query_string_lst.push_back("purpose=list");
		break;
		*/
	case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_DESCRIPTION:

		//Get the full description
		query_string_lst.push_back("purpose=full");
		break;
	case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_EXTENDED_INFO:

		// Get only the extended info
		query_string_lst.push_back("purpose=x");
		break;
	case JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_PROPERTY_LIST:

		// Get the list of all properties for a specific component
		query_string_lst.push_back("purpose=list"); 
		break;
	}

	if (entry.contentOnly)
	{
		query_string_lst.push_back("content_only=yes");
	}
	if (entry.ctxt.valid)
	{
		std::string txtCtxt;
		resL = jvx_context2String(txtCtxt, &entry.ctxt);
		if (resL == JVX_NO_ERROR)
		{
			query_string_lst.push_back("call_context=" + txtCtxt);
		}
	}

	if (query_string_lst.size())
	{
		addrTarget += "?";
		for (i = 0; i < query_string_lst.size(); i++)
		{
			if (i != 0)
			{
				addrTarget += "&";
			}

			// A percent encode might happen here!!
			addrTarget += query_string_lst[i];
		}
	}
	uri = addrTarget;

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAccessProperties_Qhttp::complete_transfer_http(jvxSize uniqueIdGroup, jvxSize uniqueId_ingroup, const char* response, jvxHandle* priv, jvxCBitField reqId)
{
	jvxBool allPropsReceived = true;
	std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.begin();
	elm = thegroups.find(uniqueIdGroup);
	if (elm != thegroups.end())
	{
		std::map<jvxSize, oneRequestEntry>::iterator elmD = elm->second.requests.find(uniqueId_ingroup);
		if (elmD != elm->second.requests.end())
		{
			elmD->second.json_response = response;
			elmD->second.errRes = JVX_NO_ERROR;
			elmD->second.stat = JVX_STATE_ACTIVE;

			check_for_all_props_complete(uniqueIdGroup);
			return JVX_NO_ERROR;
		}
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvxrtst << __FUNCTION__ << ": Error: received invalid unique id within group: " << uniqueIdGroup << "::" << uniqueId_ingroup << std::endl;
		}

		std::cout << __FUNCTION__ << ": Error: received invalid unique id within group: " << uniqueIdGroup << "::" << uniqueId_ingroup << std::endl;
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
	{
		jvxrtst << __FUNCTION__ << ": Error: received invalid unique id for group: " << uniqueIdGroup << "::" << uniqueId_ingroup << std::endl;
	}
	std::cout << __FUNCTION__ << ": Error: received invalid unique id for group: " << uniqueIdGroup << "::" << uniqueId_ingroup << std::endl;
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxAccessProperties_Qhttp::failed_transfer_http(jvxSize uniqueIdGroup, jvxSize uniqueId_ingroup, jvxErrorType errRes, jvxHandle* priv, jvxCBitField reqId)
{
	
	std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.begin();
	elm = thegroups.find(uniqueIdGroup);
	if (elm != thegroups.end())
	{
		std::map<jvxSize, oneRequestEntry>::iterator elmD = elm->second.requests.find(uniqueId_ingroup);
		if (elmD != elm->second.requests.end())
		{
			elmD->second.json_response = "";
			elmD->second.errRes = errRes;
			elmD->second.stat = JVX_STATE_ACTIVE;

			check_for_all_props_complete(uniqueIdGroup);
			return JVX_NO_ERROR;
		}
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvxrtst << __FUNCTION__ << ": Error: received invalid unique id within group: " << uniqueIdGroup << "::" << uniqueId_ingroup << std::endl;
		}

		std::cout << __FUNCTION__ << ": Error: received invalid unique id within group: " << uniqueIdGroup << "::" << uniqueId_ingroup << std::endl;
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
	{
		jvxrtst << __FUNCTION__ << ": Error: received invalid unique id for group: " << uniqueIdGroup << "::" << uniqueIdGroup << std::endl;
	}
	std::cout << __FUNCTION__ << ": Error: received invalid unique id for group: " << uniqueIdGroup << "::" << uniqueIdGroup << std::endl;
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

void 
CjvxAccessProperties_Qhttp::check_for_all_props_complete(jvxSize uniqueIdGroup)
{
	jvxBool allPropsReceived = true;
	std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.begin();
	elm = thegroups.find(uniqueIdGroup);
	if (elm != thegroups.end())
	{
		std::map<jvxSize, oneRequestEntry>::iterator elmD = elm->second.requests.begin();
		for (; elmD != elm->second.requests.end(); elmD++)
		{
			if (elmD->second.stat == JVX_STATE_INIT)
			{
				allPropsReceived = false;
				break;
			}
		}

		if (allPropsReceived)
		{
			emit_reschedule_all_props_in(uniqueIdGroup);
		}
	}
	else
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvxrtst << __FUNCTION__ << ": Error: received invalid unique id for group: " << uniqueIdGroup << std::endl;
		}
		std::cout << __FUNCTION__ << ": Error: received invalid unique id for group: " << uniqueIdGroup << std::endl;		
	}
}

void 
CjvxAccessProperties_Qhttp::reschedule_all_props_in(jvxSize uniqueIdGroup)
{
	std::map<jvxSize, oneAssociatedGroup>::iterator elm = thegroups.end();
	elm = thegroups.find(uniqueIdGroup);
	if (elm != thegroups.end())
	{
		assert(elm->second.currentMode == JVX_ACCESS_PROPERTY_MODE_WAIT_RESPONSE);
		elm->second.currentMode = JVX_ACCESS_PROPERTY_MODE_READOUT;

		if (elm->second.response)
		{
			// This will cause a delayed callback trigger
			elm->second.response->report_nonblocking_delayed_update_complete(uniqueIdGroup, elm->second.priv);
		}
	}
	else
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvxrtst << __FUNCTION__ << ": Error: received invalid unique id for group: " << uniqueIdGroup << std::endl;
		}

		std::cout << __FUNCTION__ << ": Error: received invalid unique id for group: " << uniqueIdGroup << std::endl;
	}
}

// =========================================================================================================

jvxErrorType
CjvxAccessProperties_Qhttp::convertResponseToContent__descriptor(
	jvx::propertyDescriptor::IjvxPropertyDescriptor& theDescr, 
	const jPAD& ident, oneRequestEntry req, jvxAccessProtocol* accProt)
{
	jvxErrorType resL = JVX_NO_ERROR;
	std::vector<std::string> errs;

	// Transfor Json
	CjvxJsonElement* elm_work = NULL;
	CjvxJsonElementList* sec_work = NULL;
	CjvxJsonElement* elm_sec_work = nullptr;
	CjvxJsonElementList* elm_lst_work = NULL;

	jvxApiString myString;
	jvxData valD;

	jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus* dFullPlus = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus>(&theDescr);
	jvx::propertyDescriptor::CjvxPropertyDescriptorFull* dFull = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorFull>(&theDescr);
	jvx::propertyDescriptor::CjvxPropertyDescriptorControl* dCtrl = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorControl>(&theDescr);
	jvx::propertyDescriptor::CjvxPropertyDescriptorCore* dCore = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorCore>(&theDescr);
	jvx::propertyDescriptor::CjvxPropertyDescriptorMin* dMin = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorMin>(&theDescr);

	CjvxJsonElementList lst;
	CjvxJsonElementList::stringToRepresentation(req.json_response, lst, errs);

	elm_work = NULL;
	lst.reference_element(
		&elm_work,
		"return_code",
		CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
	if (elm_work)
	{
		resL = elm_work->reference_string(myString);
		if (resL == JVX_NO_ERROR)
		{
			jvxErrorType resRet = jvxErrorType_decode(myString.c_str());

			if (resRet == JVX_NO_ERROR)
			{
				elm_work = NULL;
				resL = lst.reference_element(&elm_work,
					"properties",
					CjvxJsonElement::JVX_JSON_ASSIGNMENT_SECTION);
				if (resL == JVX_NO_ERROR)
				{
					resL = elm_work->reference_section(&sec_work);
				}
				if (resL == JVX_NO_ERROR)
				{
					resL = sec_work->reference_element(&elm_sec_work, ident.descriptor);
				}
				if (resL == JVX_NO_ERROR)
				{
					resL = elm_sec_work->reference_section(&elm_lst_work);
				}

				if (resL == JVX_NO_ERROR)
				{
					if(dFull)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work,
							"descriptor",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							dFull->descriptor = myString;
							assert(ident.descriptor == myString.std_str());
							/*
							 {
								resL = JVX_ERROR_INVALID_FORMAT;
								}
								*/
						}
					}
				}

				if (resL == JVX_NO_ERROR)
				{
					if (dFull)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work,
							"name",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							dFull->name = myString;							
						}
					}
				}
				if (resL == JVX_NO_ERROR)
				{
					if (dFull)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work,
							"description",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							dFull->description = myString.std_str();
						}
					}
				}

				if (resL == JVX_NO_ERROR)
				{
					if (dCore)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work,
							"format",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							dCore->format = jvxDataFormat_decode(myString.c_str());
							assert(dCore->format != JVX_DATAFORMAT_LIMIT);
						}
					}
				}

				if (resL == JVX_NO_ERROR)
				{
					if (dCore)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work,
							"number_elements",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_VALUE_DATA);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_data(&valD);
							dCore->num = JVX_DATA2SIZE(valD);
						}
					}
				}

				if (resL == JVX_NO_ERROR)
				{
					if(dCore)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work, "access_type",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							dCore->accessType = jvxPropertyAccessType_decode(myString.c_str());
							assert(dCore->accessType != JVX_PROPERTY_ACCESS_LIMIT);
						}
					}
				}

				if (resL == JVX_NO_ERROR)
				{
					if (dMin)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work, "category",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							dMin->category = jvxPropertyCategoryType_decode(myString.c_str());
							assert(dMin->category != JVX_PROPERTY_CATEGORY_LIMIT);
						}
					}
				}

				if (resL == JVX_NO_ERROR)
				{
					if (dMin)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work, "global_idx",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_VALUE_DATA);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_data(&valD);
							dMin->globalIdx = JVX_DATA2SIZE(valD);
						}
					}
				}

				if (resL == JVX_NO_ERROR)
				{
					if (dCore)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work, "decoder_type",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							dCore->decTp = jvxPropertyDecoderHintType_decode(myString.c_str());
							assert(dCore->decTp != JVX_PROPERTY_DECODER_LIMIT);
						}
					}
				}

				if (resL == JVX_NO_ERROR)
				{
					if (dCore)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work, "context",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							dCore->ctxt = jvxPropertyContext_decode(myString.c_str());
							assert(dCore->ctxt != JVX_PROPERTY_CONTEXT_LIMIT);
						}
					}
				}

				if (dCtrl)
				{
					dCtrl->isValid = false;
					if (resL == JVX_NO_ERROR)
					{
						elm_work = NULL;
						resL = elm_lst_work->reference_element(
							&elm_work, "valid",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							if (myString.std_str() == "*")
							{
								dCtrl->isValid = true;
							}
						}
					}
				}
				if (accProt)
				{
					*accProt = JVX_ACCESS_PROTOCOL_OK;
				}

				if (resL == JVX_NO_ERROR)
				{
					std::map<std::string, jvxOnePropertyConnected>::iterator elm = allPropsCached.find(ident.descriptor);
					if (elm != allPropsCached.end())
					{
						if (dFull)
						{
							elm->second.descr = *dFull;
						}
						else
						{
							assert(0);
						}
					}
					else
					{
						jvxOnePropertyConnected newStr;
						if (dFull)
						{
							newStr.descr = *dFull;
						}
						else
						{
							assert(0);
						}
						newStr.extProps.groupid.content = JVX_SIZE_UNSELECTED;
						newStr.extProps.groupid.valid = false;
						newStr.extProps.moreinfo.valid = false;
						newStr.extProps.origin.valid = false;
						newStr.extProps.showhint.valid = false;
						newStr.extProps.update_context.valid = false;
						allPropsCached[newStr.descr.descriptor.std_str()] = newStr;
					}
				}
				if (resL != JVX_NO_ERROR)
				{
					resL = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
		}
	}
	else
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return resL;
}

jvxErrorType
CjvxAccessProperties_Qhttp::convertResponseToContent__proplist(
	std::map<std::string, jvxPropertyDescriptorP>& theProps, 
	const jPAD& ident, oneRequestEntry req, jvxAccessProtocol* accProt)
{
	jvxSize i;
	jvxErrorType resL = JVX_NO_ERROR;
	std::vector<std::string> errs;
	jvxPropertyDescriptorP theStr;
	// Transfor Json
	CjvxJsonElement* elm_work = NULL;
	CjvxJsonArray* arr_work = NULL;
	CjvxJsonArrayElement* arr_elm_work = NULL;
	CjvxJsonElementList* elm_lst_work = NULL;

	jvxApiString myString;
	jvxData valD;

	CjvxJsonElementList lst;
	CjvxJsonElementList::stringToRepresentation(req.json_response,
		lst, errs);

	elm_work = NULL;
	lst.reference_element(
		&elm_work,
		"return_code",
		CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
	if (elm_work)
	{
		resL = elm_work->reference_string(myString);
		if (resL == JVX_NO_ERROR)
		{
			jvxErrorType resRet = jvxErrorType_decode(myString.c_str());

			if (resRet == JVX_NO_ERROR)
			{
				elm_work = NULL;
				resL = lst.reference_element(&elm_work,
					"selected_property",
					CjvxJsonElement::JVX_JSON_ASSIGNMENT_ARRAY);
				if (resL == JVX_NO_ERROR)
				{
					resL = elm_work->reference_array(&arr_work);
				}
				if (resL == JVX_NO_ERROR)
				{
					jvxSize numElms = 0;
					arr_work->numElements(&numElms);
					for (i = 0; i < numElms; i++)
					{
						resL = arr_work->reference_elementAt(i, &arr_elm_work);
						if (resL == JVX_NO_ERROR)
						{
							resL = arr_elm_work->reference_section(&elm_lst_work);
						}

						if (resL == JVX_NO_ERROR)
						{
							elm_work = NULL;
							resL = elm_lst_work->reference_element(
								&elm_work,
								"descriptor",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_string(myString);
								theStr.descriptor = myString.std_str();
							}
						}

						if (resL == JVX_NO_ERROR)
						{
							elm_work = NULL;
							resL = elm_lst_work->reference_element(
								&elm_work,
								"description",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_string(myString);
								theStr.description = myString.std_str();
							}
						}

						if (resL == JVX_NO_ERROR)
						{
							elm_work = NULL;
							resL = elm_lst_work->reference_element(
								&elm_work,
								"format",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_string(myString);
								theStr.format = jvxDataFormat_decode(myString.c_str());
								assert(theStr.format != JVX_DATAFORMAT_LIMIT);
							}
						}

						if (resL == JVX_NO_ERROR)
						{
							elm_work = NULL;
							resL = elm_lst_work->reference_element(
								&elm_work,
								"number_elements",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_VALUE_DATA);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_data(&valD);
								theStr.num = JVX_DATA2SIZE(valD);
							}
						}

						if (resL == JVX_NO_ERROR)
						{
							elm_work = NULL;
							resL = elm_lst_work->reference_element(
								&elm_work, "access_type",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_string(myString);
								theStr.accessType = jvxPropertyAccessType_decode(myString.c_str());
								assert(theStr.accessType != JVX_PROPERTY_ACCESS_LIMIT);
							}
						}

						if (resL == JVX_NO_ERROR)
						{
							elm_work = NULL;
							resL = elm_lst_work->reference_element(
								&elm_work, "category",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_string(myString);
								theStr.category = jvxPropertyCategoryType_decode(myString.c_str());
								assert(theStr.category != JVX_PROPERTY_CATEGORY_LIMIT);
							}
						}

						if (resL == JVX_NO_ERROR)
						{
							elm_work = NULL;
							resL = elm_lst_work->reference_element(
								&elm_work, "global_idx",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_VALUE_DATA);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_data(&valD);
								theStr.globalIdx = JVX_DATA2SIZE(valD);
							}
						}

						if (resL == JVX_NO_ERROR)
						{
							elm_work = NULL;
							resL = elm_lst_work->reference_element(
								&elm_work, "decoder_type",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_string(myString);
								theStr.decTp = jvxPropertyDecoderHintType_decode(myString.c_str());
								assert(theStr.decTp != JVX_PROPERTY_DECODER_LIMIT);
							}
						}

						if (resL == JVX_NO_ERROR)
						{
							elm_work = NULL;
							resL = elm_lst_work->reference_element(
								&elm_work, "context",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_string(myString);
								theStr.ctxt = jvxPropertyContext_decode(myString.c_str());
								assert(theStr.ctxt != JVX_PROPERTY_CONTEXT_LIMIT);
							}
						}

						theStr.isValid = false;
						if (resL == JVX_NO_ERROR)
						{
							elm_work = NULL;
							resL = elm_lst_work->reference_element(
								&elm_work, "valid",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_string(myString);
								if (myString.std_str() == "*")
								{
									theStr.isValid = true;
								}
							}
						}
						auto elmCache = thePropCache.find(theStr.descriptor.std_str());
						if (elmCache == thePropCache.end())
						{
							thePropCache[theStr.descriptor.std_str()] = theStr;
						}
					}// for (i = 0; i < numElms; i++)
				} // if (resL == JVX_NO_ERROR)
				if (accProt)
				{
					*accProt = JVX_ACCESS_PROTOCOL_OK;
				}
				if (resL != JVX_NO_ERROR)
				{
					resL = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
		}
	}
	else
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return resL;
}

jvxErrorType
CjvxAccessProperties_Qhttp::convertResponseToContent__extendedInfo(jvxHandle* whattoget, jvxSize tp, oneRequestEntry req, jvxAccessProtocol* accProt)
{
	jvxErrorType resL = JVX_NO_ERROR;
	std::vector<std::string> errs;

	// Transfor Json
	CjvxJsonElement* elm_work = NULL;
	CjvxJsonArray* arr_work = NULL;
	CjvxJsonArrayElement* arr_elm_work = NULL;
	CjvxJsonElementList* elm_lst_work = NULL;

	jvxApiString myString;
	jvxApiString* fldStr = NULL;
	jvxData valD;

	std::string descr_string;

	CjvxJsonElementList lst;
	CjvxJsonElementList::stringToRepresentation(req.json_response,
		lst, errs);

	elm_work = NULL;
	lst.reference_element(
		&elm_work,
		"return_code",
		CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
	if (elm_work)
	{
		resL = elm_work->reference_string(myString);
		if (resL == JVX_NO_ERROR)
		{
			jvxErrorType resRet = jvxErrorType_decode(myString.c_str());

			if (resRet == JVX_NO_ERROR)
			{
				elm_work = NULL;
				resL = lst.reference_element(&elm_work,
					"selected_property",
					CjvxJsonElement::JVX_JSON_ASSIGNMENT_ARRAY);
				if (resL == JVX_NO_ERROR)
				{
					resL = elm_work->reference_array(&arr_work);
				}
				if (resL == JVX_NO_ERROR)
				{
					resL = arr_work->reference_elementAt(0, &arr_elm_work);
				}
				if (resL == JVX_NO_ERROR)
				{
					resL = arr_elm_work->reference_section(&elm_lst_work);
				}

				if (resL == JVX_NO_ERROR)
				{
					resL = elm_lst_work->reference_element(
						&elm_work,
						"descriptor",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
					if (resL == JVX_NO_ERROR)
					{
						elm_work->reference_string(myString);
						descr_string = myString.std_str();

						std::map<std::string, jvxOnePropertyConnected>::iterator elm = allPropsCached.find(descr_string);
						if (elm != allPropsCached.end())
						{
							elm_work = NULL;
							switch (tp)
							{
							case JVX_PROPERTY_INFO_SHOWHINT:

								resL = elm_lst_work->reference_element(
									&elm_work,
									"showhint",
									CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
								if (resL == JVX_NO_ERROR)
								{
									elm_work->reference_string(myString);

									elm->second.extProps.showhint.content = myString.std_str();
									elm->second.extProps.showhint.valid = true;

									fldStr = (jvxApiString*)whattoget;
									if (fldStr)
									{
										fldStr->assign(myString.std_str());
									}

								}
								break;

							case JVX_PROPERTY_INFO_ORIGIN:
								resL = elm_lst_work->reference_element(
									&elm_work,
									"origin",
									CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
								if (resL == JVX_NO_ERROR)
								{
									elm_work->reference_string(myString);

									elm->second.extProps.origin.content = myString.std_str();
									elm->second.extProps.origin.valid = true;
									fldStr = (jvxApiString*)whattoget;
									if (fldStr)
									{
										fldStr->assign(myString.std_str());
									}
								}
								break;
							case JVX_PROPERTY_INFO_MOREINFO:
								resL = elm_lst_work->reference_element(
									&elm_work,
									"moreinfo",
									CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
								if (resL == JVX_NO_ERROR)
								{
									elm_work->reference_string(myString);

									elm->second.extProps.moreinfo.content = myString.std_str();
									elm->second.extProps.moreinfo.valid = true;

									fldStr = (jvxApiString*)whattoget;
									if (fldStr)
									{
										fldStr->assign(myString.std_str());
									}
								}
								break;
							default:
								assert(0);
							}
						}
						else
						{
							if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
							{
								jvxrtst << __FUNCTION__ << ": Unknown property descriptor <" << descr_string << "> in response for <extendedInfo>." << std::endl;
							}

							std::cout << __FUNCTION__ << ": Unknown property descriptor <" << descr_string  << "> in response for <extendedInfo>." << std::endl;
						}
					}
					else
					{
						if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
						{
							jvxrtst << __FUNCTION__ << ": Property descriptor not found in response for <extendedInfo>." << std::endl;
						}

						std::cout << __FUNCTION__ << ": Property descriptor not found in response for <extendedInfo>." << std::endl;

					}
				} // if (resL == JVX_NO_ERROR)

				if (accProt)
				{
					*accProt = JVX_ACCESS_PROTOCOL_OK;
				}

				if (resL != JVX_NO_ERROR)
				{
					resL = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
		}
	}
	else
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return resL;
}

jvxErrorType
CjvxAccessProperties_Qhttp::convertResponseToContent__get_content(jvxHandle* fldCpy,
	jvxSize offsetStart, jvxSize numElements, jvxDataFormat format,
	jvxBool contentOnly, jvxPropertyDecoderHintType decTp, const char* descriptor, oneRequestEntry req,
	jvxAccessProtocol* accProt, jvxCallManagerProperties& callGate)
{
	jvxErrorType resL = JVX_NO_ERROR;
	std::vector<std::string> errs;
	std::string locString;
	// Transfor Json
	CjvxJsonElement* elm_work = NULL;
	CjvxJsonArray* arr_work = NULL;
	CjvxJsonArrayElement* arr_elm_work = NULL;
	CjvxJsonElementList* elm_lst_work = NULL;

	jvxApiString myString;
	jvxApiString* fldStr = NULL;
	jvxData valD;

	jvxSize offset_local = JVX_SIZE_UNSELECTED;
	jvxSize numElements_local = JVX_SIZE_UNSELECTED;
	jvxApiString descror_local;
	jvxDataFormat format_local = JVX_DATAFORMAT_NONE;
	jvxPropertyDecoderHintType decoder_local = JVX_PROPERTY_DECODER_NONE;
	jvxPropertyAccessType accesstp_local = JVX_PROPERTY_ACCESS_NONE;
	jvxBool is_valid_local = true;

	jvxSize a, nArr = 0;

	jvxBool errDetected = false;
	std::string propString;
	std::string propStringPtr;
	std::string descriptorString;
	std::string formatString;
	std::string accString;
	std::string decoderString;
	std::string dString;
	std::string isValidString;
	jvxUInt16 crc_prop_local = 0;
	jvxUInt16 crc_prop = 0;

	if (fldCpy == NULL)
	{
		if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_GET_ACCESS_DATA)
		{
			return JVX_ERROR_INVALID_ARGUMENT;
		}
	}

	CjvxJsonElementList lst;
	resL = CjvxJsonElementList::stringToRepresentation(req.json_response,
		lst, errs);
	if (resL == JVX_ERROR_CALL_SUB_COMPONENT_FAILED)
	{
		// std::cout << "Failed to convert json string <" << req.json_response << ">." << std::endl;
		// We can just let go in case of error: json string has been reported before and this is identical to no response
	}

	elm_work = NULL;
	lst.reference_element(
		&elm_work,
		"return_code",
		CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
	if (elm_work)
	{
		if (accProt)
			*accProt = JVX_ACCESS_PROTOCOL_OK;

		resL = elm_work->reference_string(myString);
		if (resL == JVX_NO_ERROR)
		{
			jvxErrorType resRet = jvxErrorType_decode(myString.c_str());

			if (resRet == JVX_NO_ERROR)
			{
				elm_work = NULL;
				resL = lst.reference_element(&elm_work,
					"property",
					CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
				if (resL == JVX_NO_ERROR)
				{
					// If we found "property", we see the full mode
					elm_work->reference_string(myString);
					propString = myString.std_str();

					propStringPtr = "";
					elm_work = NULL;
					resL = lst.reference_element(&elm_work,
						"property_fld",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_ARRAY);
					if (resL == JVX_NO_ERROR)
					{
						
						elm_work->reference_array(&arr_work);
						if (arr_work)
						{
							
							arr_work->numElements(&nArr);
							for (a = 0; a < nArr; a++)
							{
								arr_work->reference_elementAt(a, &arr_elm_work);
								if (arr_elm_work)
								{
									std::string strTmp;
									arr_elm_work->reference_string(myString);
								}
								if (a != 0)
								{
									propStringPtr += ":";// The following list extractor expects a ":" - changed by HK 06.07.2023
								}
								propStringPtr += myString.std_str();
							}
						}
					}

					elm_work = NULL;
					resL = lst.reference_element(&elm_work,
						"property_fld",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
					if (resL == JVX_NO_ERROR)
					{
						elm_work->reference_string(myString);
						propStringPtr = myString.std_str();
					}
					// =====================================================

					elm_work = NULL;
					resL = lst.reference_element(&elm_work,
						"descriptor",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
					if (resL == JVX_NO_ERROR)
					{
						elm_work->reference_string(myString);
						descriptorString = myString.std_str();
					}

					elm_work = NULL;
					resL = lst.reference_element(&elm_work,
						"format",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
					if (resL == JVX_NO_ERROR)
					{
						elm_work->reference_string(myString);
						formatString = myString.std_str();
					}

					elm_work = NULL;
					resL = lst.reference_element(&elm_work,
						"decoder_type",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
					if (resL == JVX_NO_ERROR)
					{
						elm_work->reference_string(myString);
						decoderString = myString.std_str();
					}

					elm_work = NULL;
					resL = lst.reference_element(&elm_work,
						"offset",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_VALUE_DATA);
					if (resL == JVX_NO_ERROR)
					{
						elm_work->reference_data(&valD);
						offset_local = JVX_DATA2SIZE(valD);
					}

					elm_work = NULL;
					resL = lst.reference_element(&elm_work,
						"number_elements",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_VALUE_DATA);
					if (resL == JVX_NO_ERROR)
					{
						elm_work->reference_data(&valD);
						numElements_local = JVX_DATA2SIZE(valD);
					}

					elm_work = NULL;
					resL = lst.reference_element(&elm_work,
						"access_type",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
					if (resL == JVX_NO_ERROR)
					{
						elm_work->reference_string(myString);
						accString = myString.std_str();
					}

					elm_work = NULL;
					resL = lst.reference_element(&elm_work,
						"valid",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
					if (resL == JVX_NO_ERROR)
					{
						elm_work->reference_string(myString);
						isValidString = myString.std_str();
					}
					
					if (callGate.context)
					{
						callGate.context->valid = false;

						// =====================================================

						elm_work = NULL;
						resL = lst.reference_element(&elm_work,
							"call_context",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							jvx_string2Context(myString.std_str(), callGate.context);
						}
					}
					format_local = jvxDataFormat_decode(formatString.c_str());
					decoder_local = jvxPropertyDecoderHintType_decode(decoderString.c_str());
					accesstp_local = jvxPropertyAccessType_decode(accString.c_str());
					is_valid_local = false;
					if (isValidString == "*")
					{
						is_valid_local = true;
					}
					if (descriptorString != descriptor)
					{
						std::cout << __FUNCTION__ << ": Property descriptor mismatch: <" << descriptorString << "> vs <" << descriptor << ">." << std::endl;
					}

				}
				else
				{
					elm_work = NULL;
					resL = lst.reference_element(&elm_work,
						"p",
						CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);

					if (resL == JVX_NO_ERROR)
					{
						elm_work->reference_string(myString);
						propString = myString.std_str();

						propStringPtr = "";
						elm_work = NULL;
						resL = lst.reference_element(&elm_work,
							"pf",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_ARRAY);
						if (resL == JVX_NO_ERROR)
						{

							elm_work->reference_array(&arr_work);
							if (arr_work)
							{

								arr_work->numElements(&nArr);
								for (a = 0; a < nArr; a++)
								{
									arr_work->reference_elementAt(a, &arr_elm_work);
									if (arr_elm_work)
									{
										std::string strTmp;
										arr_elm_work->reference_string(myString);
									}
									if (a != 0)
									{
										propStringPtr += ":"; // The following list extractor expects a ":" - changed by HK 06.07.2023
									}
									propStringPtr += myString.std_str();
								}
							}
						}

						elm_work = NULL;
						resL = lst.reference_element(&elm_work,
							"pf",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							propStringPtr = myString.std_str();
						}

						// =====================================================================

						elm_work = NULL;
						resL = lst.reference_element(&elm_work,
							"d",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							elm_work->reference_string(myString);
							dString = myString.std_str();

							if (!dString.empty())
							{
								jvxBool err = false;
								jvxUInt32 allBits = jvx_string2UInt64(dString, err);
								if (!err)
								{
									is_valid_local = (allBits & 0x1);
									allBits = allBits >> 1;
									accesstp_local = (jvxPropertyAccessType)(allBits & 0xF);
									allBits = allBits >> 4;
									decoder_local = (jvxPropertyDecoderHintType)(allBits & 0x1F);
									allBits = allBits >> 5;
									format_local = (jvxDataFormat)(allBits & 0x1F);
									crc_prop_local = allBits >> 6; // 16 MSBs remain for cic check
								}
								descriptorString = descriptor;
								crc_prop = jvx_crc16_ccitt(descriptorString.c_str(), descriptorString.size());

								if (crc_prop != crc_prop_local)
								{
									if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
									{
										jvxrtst << __FUNCTION__ << ": Property descriptor mismatch, crc <" << crc_prop_local << "> vs <" << crc_prop << ">." << std::endl;
									}
									std::cout << __FUNCTION__ << ": Property descriptor mismatch, crc <" << crc_prop_local << "> vs <" << crc_prop << ">." << std::endl;
								}
							}
						}
					
						// Decode call context
						if (callGate.context)
						{
							callGate.context->valid = false;

							elm_work = NULL;
							resL = lst.reference_element(&elm_work,
								"c",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (resL == JVX_NO_ERROR)
							{
								elm_work->reference_string(myString);
								jvx_string2Context(myString.std_str(), callGate.context);
							}
						}
					}
					else
					{
						// No p and no property. There may still be a access_prot
						
						elm_work = NULL;
						resL = lst.reference_element(&elm_work,
							"a",
							CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
						if (resL == JVX_NO_ERROR)
						{
							if (accProt)
							{
								elm_work->reference_string(myString);
								*accProt = jvxAccessProtocol_decode(myString.c_str());
							}
						}
						else
						{
							elm_work = NULL;
							resL = lst.reference_element(&elm_work,
								"access_prot",
								CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (resL == JVX_NO_ERROR)
							{
								if (accProt)
								{
									elm_work->reference_string(myString);
									*accProt = jvxAccessProtocol_decode(myString.c_str());
								}
							}
							else
							{
								resL = JVX_ERROR_ELEMENT_NOT_FOUND;
							}
						}
					}
				}

				std::map<std::string, jvxOnePropertyConnected>::iterator elm = allPropsCached.find(descriptorString);
				if (elm != allPropsCached.end())
				{
					if (JVX_CHECK_SIZE_SELECTED(offset_local))
					{
						if (offsetStart != offset_local)
						{
							if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
							{
								jvxrtst << __FUNCTION__ << ": Property offset mismatch: <" << offsetStart << "> vs <" << offset_local << ">." << std::endl;
							}

							std::cout << __FUNCTION__ << ": Property offset mismatch: <" << offsetStart << "> vs <" << offset_local << ">." << std::endl;
							// errDetected = true;
						}
					}
					if (JVX_CHECK_SIZE_SELECTED(numElements_local))
					{
						if (numElements_local != numElements)
						{
							if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
							{
								jvxrtst << __FUNCTION__ << ": Property length mismatch: <" << numElements << "> vs <" << numElements_local << ">." << std::endl;
							}

							std::cout << __FUNCTION__ << ": Property length mismatch: <" << numElements << "> vs <" << numElements_local << ">." << std::endl;
							//errDetected = true;
						}
					}

					if (format_local != JVX_DATAFORMAT_NONE)
					{
						if (
							(format == JVX_DATAFORMAT_NONE) &&
							((callGate.call_purpose == JVX_PROPERTY_CALL_PURPOSE_GET_ACCESS_DATA)))
						{

						}
						else
						{
							if (format_local != format)
							{
								if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
								{
									jvxrtst << __FUNCTION__ << ": Property format mismatch between call and arrival: <" << jvxDataFormat_txt(format) << "> vs <" << jvxDataFormat_txt(format_local) << ">." << std::endl;
								}

								std::cout << __FUNCTION__ << ": Property format mismatch between call and arrival: <" << jvxDataFormat_txt(format) << "> vs <" << jvxDataFormat_txt(format_local) << ">." << std::endl;
								// errDetected = true;
							}
						
							if (format_local != elm->second.descr.format)
							{
								if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
								{
									jvxrtst << __FUNCTION__ << ": Property format mismatch between stored and arrival: <" << jvxDataFormat_txt(format) << "> vs <" << jvxDataFormat_txt(elm->second.descr.format) << ">." << std::endl;
								}
								std::cout << __FUNCTION__ << ": Property format mismatch between stored and arrival: <" << jvxDataFormat_txt(format) << "> vs <" << jvxDataFormat_txt(elm->second.descr.format) << ">." << std::endl;
								// errDetected = true;
							}
						}
					}
					if (decoder_local != JVX_PROPERTY_DECODER_NONE)
					{
						if (decoder_local != decTp)
						{
							if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
							{
								jvxrtst << __FUNCTION__ << ": Property format mismatch between call and arrival: <" << jvxPropertyDecoderHintType_txt(decoder_local) <<
									"> vs <" << jvxPropertyDecoderHintType_txt(decTp) << ">." << std::endl;
							}

							std::cout << __FUNCTION__ << ": Property format mismatch between call and arrival: <" << jvxPropertyDecoderHintType_txt(decoder_local) <<
								"> vs <" << jvxPropertyDecoderHintType_txt(decTp) << ">." << std::endl;
							//errDetected = true;
						}

						if (decoder_local != elm->second.descr.decTp)
						{
							if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
							{
								jvxrtst << __FUNCTION__ << ": Property format mismatch between stored and arrival: <" << jvxPropertyDecoderHintType_txt(decoder_local) <<
									"> vs <" << jvxPropertyDecoderHintType_txt(elm->second.descr.decTp) << ">." << std::endl;
							}

							std::cout << __FUNCTION__ << ": Property format mismatch between stored and arrival: <" << jvxPropertyDecoderHintType_txt(decoder_local) <<
								"> vs <" << jvxPropertyDecoderHintType_txt(elm->second.descr.decTp) << ">." << std::endl;
							//errDetected = true;
						}
					}
				}

				// ===============================================================
				// ===============================================================
				if (!errDetected)
				{
					if (*accProt == JVX_ACCESS_PROTOCOL_OK)
					{
						// Note: the fld pointer may be NULL if only the accessType is intended 
						if (fldCpy)
						{
					
							jvx::helper::properties::fromString(fldCpy, offsetStart, numElements, format, contentOnly, decTp, propString, propStringPtr);
						}
					}
					else
					{
						// Protocol reports something not ok
						if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
						{
							jvxrtst << "Property <" << descriptor << ">: access protocol indicates <" << jvxAccessProtocol_txt(*accProt) << ">." << std::endl;
						}

						std::cout << "Property <" << descriptor << ">: access protocol indicates <" << jvxAccessProtocol_txt(*accProt) << ">." << std::endl;
					}
					if (callGate.on_get.prop_access_type)
					{
						*callGate.on_get.prop_access_type = accesstp_local;
					}
					if (callGate.on_get.is_valid)
					{
						*callGate.on_get.is_valid = is_valid_local;
					}
					
					resL = JVX_NO_ERROR;
				}
				else
				{
					resL = JVX_ERROR_PROTOCOL_ERROR;
				}
			}
		}
	}
	else
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return resL;
}

jvxErrorType
CjvxAccessProperties_Qhttp::convertResponseToContent__set_content(jvxHandle* fld,
	jvxSize offsetStart, jvxSize numElements, jvxDataFormat format,
	jvxBool contentOnly, jvxPropertyDecoderHintType decTp, const char* descriptor, oneRequestEntry req,
	jvxAccessProtocol* accProt)
{
	jvxErrorType resL = JVX_NO_ERROR;
	std::vector<std::string> errs;

	// Transfor Json
	CjvxJsonElement* elm_work = NULL;
	CjvxJsonArray* arr_work = NULL;
	CjvxJsonArrayElement* arr_elm_work = NULL;
	CjvxJsonElementList* elm_lst_work = NULL;

	jvxApiString myString;
	jvxApiString* fldStr = NULL;
	jvxData valD;

	jvxSize offset_local = 0;
	jvxSize numElements_local = 0;
	jvxApiString descror_local;
	jvxDataFormat format_local = JVX_DATAFORMAT_NONE;

	CjvxJsonElementList lst;
	CjvxJsonElementList::stringToRepresentation(req.json_response,
		lst, errs);

	elm_work = NULL;
	lst.reference_element(
		&elm_work,
		"return_code",
		CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
	if (elm_work)
	{
		resL = elm_work->reference_string(myString);
		if (resL == JVX_NO_ERROR)
		{
			jvxErrorType resRet = jvxErrorType_decode(myString.c_str());
		}

		if (accProt)
		{
			*accProt = JVX_ACCESS_PROTOCOL_OK;
		}

	}
	else
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return resL;
}


