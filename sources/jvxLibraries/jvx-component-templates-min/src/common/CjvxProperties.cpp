#include "common/CjvxProperties.h"

CjvxProperties::CjvxProperties(const std::string& moduleName, CjvxObjectMin& objRef): theObjRef(objRef)
{
	JVX_INITIALIZE_MUTEX(_common_set_properties.csec);
	_common_set_properties.threadId = JVX_NULLTHREAD;
	//_common_set_properties.maxPropId = JVX_PROPERTIES_OFFSET_UNSPECIFIC;
	_common_set_properties.propSetRevision = 0;
	_common_set_properties.wasStarted = false;
	_common_set_properties.reportIfNoChange = false;
	_common_set_properties.inPropertyGroupCnt = 0;
	_common_set_properties.moduleReference = moduleName;
	_common_set_properties.reportMissedCallbacks = false;
	_common_set_property_report.reportRef = static_cast<IjvxProperties_report*>(&objRef);
	resetPropertyIdCount();

}

CjvxProperties::~CjvxProperties()
{
	_reset_properties();

	assert(_common_set_properties.registeredSubmodules.size() == 0);

	_common_set_properties.threadId = JVX_NULLTHREAD;
	JVX_TERMINATE_MUTEX(_common_set_properties.csec);
}

jvxErrorType
CjvxProperties::_reset_properties()
{
	_common_set_properties.registeredProperties.clear();
	_common_set_properties.nextPropId = JVX_PROPERTIES_OFFSET_UNSPECIFIC;// 1;
	//_common_set_properties.maxPropId = JVX_PROPERTIES_OFFSET_UNSPECIFIC;
	return JVX_NO_ERROR;
}

jvxErrorType CjvxProperties::_reset_callbacks()
{
	_common_set_properties.registeredCallbacks.clear();
	return JVX_NO_ERROR;
}

	
jvxErrorType
CjvxProperties::_number_properties(jvxCallManagerProperties& callGate, jvxSize * num)
{
	jvxSize idx;
	jvxSize numL = 0;
	jvxErrorType res = JVX_NO_ERROR;
	if (num)
	{
		*num = 0;
		this->_number_properties_local(callGate, num);
		
		for(auto elm: _common_set_properties.registeredSubmodules)
		{
			res = elm->propRef->number_properties(
				callGate, &numL);
			if (res == JVX_NO_ERROR)
			{
				*num += numL;
			}
		}
	}
	return JVX_NO_ERROR;
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxProperties::_check_access_property_obj(jvxCallManager& callGate,
	jvxCheckAccessEnum accEnum)
{
	theObjRef._check_access(callGate);
	return JVX_NO_ERROR;
}

jvxErrorType CjvxProperties::_check_access_property(jvxCallManager& callGate, const oneProperty & prop,
	jvxCheckAccessEnum accEnum)
{
	// Check prop for access rights
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxProperties::_lock_properties_local()
{
	JVX_LOCK_MUTEX(_common_set_properties.csec);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxProperties::_trylock_properties_local()
{
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resL = JVX_TRY_LOCK_MUTEX_SUCCESS;
	JVX_TRY_LOCK_MUTEX(resL, _common_set_properties.csec);
	if (resL == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_COMPONENT_BUSY;
}

jvxErrorType
CjvxProperties::_unlock_properties_local()
{
	JVX_UNLOCK_MUTEX(_common_set_properties.csec);
	return JVX_NO_ERROR;
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxProperties::_get_revision(jvxCallManagerProperties& callGate, jvxSize * revision)
{
	if (revision)
		*revision = _common_set_properties.propSetRevision;
	return JVX_NO_ERROR;
}


jvxErrorType
CjvxProperties::_lock_properties(jvxCallManagerProperties& callGate)
{
	jvxErrorType res = _check_access_property_obj(callGate, jvxCheckAccessEnum::JVX_CHECK_ACCESS_LOCK);
	if (res != JVX_NO_ERROR)
		return res;

	return _lock_properties_local();
}

jvxErrorType
CjvxProperties::_trylock_properties(jvxCallManagerProperties& callGate)
{
	jvxErrorType res = _check_access_property_obj(callGate, jvxCheckAccessEnum::JVX_CHECK_ACCESS_LOCK);
	if (res != JVX_NO_ERROR)
		return res;

	return _trylock_properties_local();
}

jvxErrorType
CjvxProperties::_unlock_properties(jvxCallManagerProperties& callGate)
{
	jvxErrorType res = _check_access_property_obj(callGate, jvxCheckAccessEnum::JVX_CHECK_ACCESS_LOCK);
	if (res != JVX_NO_ERROR)
		return res;

	return _unlock_properties_local();
}



std::list<CjvxProperties::oneProperty>::iterator 
CjvxProperties::findSelectionFromAddress(propAddressing& pAddress)
{
	std::list<CjvxProperties::oneProperty>::iterator selection = _common_set_properties.registeredProperties.end();
	// Check the type of request

	if (pAddress.addrDescr)
	{
		selection = findInPropertyList(_common_set_properties.registeredProperties, pAddress.addrDescr->descriptor);
	}
	else if (pAddress.addrLin)
	{
		jvxSize idxLin = pAddress.addrLin->idx;
		if (idxLin < _common_set_properties.registeredProperties.size())
		{
			selection = _common_set_properties.registeredProperties.begin();
			std::advance(selection, idxLin);
		}		
		// _common_set_properties.registeredProperties
	}
	else if (pAddress.addrIdx)
	{
		selection = findInPropertyList(_common_set_properties.registeredProperties, pAddress.addrIdx->id, pAddress.addrIdx->cat);
	}
	return selection;
}

jvxErrorType
CjvxProperties::_description_property(
	jvxCallManagerProperties& callGate,
	jvx::propertyDescriptor::IjvxPropertyDescriptor& descr,
	const jvx::propertyAddress::IjvxPropertyAddress& addrProp)
{
	jvxErrorType res = JVX_NO_ERROR;
	//jvxSize idxLin = JVX_SIZE_UNSELECTED;
	const char* descriptorTag = nullptr;
	jvxSize offset = 0;
	std::list<oneProperty>::iterator selection;
	jvxSize i;

	propAddressing addresses;
	addresses.cast(&addrProp);

	propDescriptors descrs;
	descrs.cast(&descr);

	res = _check_access_property_obj(callGate, jvxCheckAccessEnum::JVX_CHECK_ACCESS_READ);
	if (res != JVX_NO_ERROR)
	{
		goto leave_error;
	}

	_lock_properties_local();

	selection = findSelectionFromAddress(addresses);

	// First check to find the right property here or within a submodule
	if (selection != _common_set_properties.registeredProperties.end())
		//if(idx < _common_set_properties.registeredProperties.size())
	{
		res = _check_access_property(callGate, *selection,
			jvxCheckAccessEnum::JVX_CHECK_ACCESS_READ);
		if (res == JVX_NO_ERROR)
		{
			assert(descrs.dMin);

			descrs.dMin->valid_parts = 0;
			jvx_bitSet(descrs.dMin->valid_parts, (jvxSize)jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_MIN);
			descrs.dMin->globalIdx = selection->propDescriptor->globalIdx;
			descrs.dMin->category = selection->propDescriptor->category;

			if (descrs.dCore)
			{
				jvx_bitSet(descrs.dMin->valid_parts, (jvxSize)jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_CORE);
				descrs.dCore->format = selection->propDescriptor->format;
				descrs.dCore->accessType = selection->propDescriptor->accessType;
				descrs.dCore->decTp = selection->propDescriptor->decTp;
				descrs.dCore->num = selection->propDescriptor->num;
				descrs.dCore->ctxt = selection->propDescriptor->ctxt;
			}

			if (descrs.dContr)
			{
				jvx_bitSet(descrs.dMin->valid_parts, (jvxSize)jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_CONTROL);
				descrs.dContr->allowedStateMask = selection->propDescriptor->allowedStateMask;
				descrs.dContr->allowedThreadingMask = selection->propDescriptor->allowedThreadingMask;
				descrs.dContr->isValid = selection->propDescriptor->isValid;

				if (selection->references.isValidPtr)
				{
					descrs.dContr->isValid = *(selection->references.isValidPtr);
				}
				descrs.dContr->invalidateOnStateSwitch = selection->propDescriptor->invalidateOnStateSwitch;
			}

			if (descrs.dFull)
			{
				jvx_bitSet(descrs.dMin->valid_parts, (jvxSize)jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_FULL);

				descrs.dFull->name.assign(selection->propDescriptor->name);
				descrs.dFull->description.assign(selection->propDescriptor->description);
				descrs.dFull->descriptor.assign(selection->propDescriptor->descriptor);
				descrs.dFull->pTag.assign(selection->propDescriptor->pTag);
			}

			if (descrs.dFullPlus)
			{
				jvx_bitSet(descrs.dMin->valid_parts, (jvxSize)jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_FULL_PLUS);
				descrs.dFullPlus->accessFlags = selection->propDescriptor->accessFlags;

				if (selection->references.accessFlagsPtr)
				{
					descrs.dFullPlus->accessFlags = *selection->references.accessFlagsPtr;
				}

				descrs.dFullPlus->configModeFlags = selection->propDescriptor->configModeFlags;
				if (selection->references.configModeFlagsPtr)
				{
					descrs.dFullPlus->configModeFlags = *selection->references.configModeFlagsPtr;
				}
			}
		}
	}
	else
	{
		res = _forward_submodules(callGate, addresses, [&](const jvx::propertyAddress::IjvxPropertyAddress& addr, CjvxPropertySubModule* theSubModule)
			{
				jvxErrorType resL = JVX_NO_ERROR;
				resL = theSubModule->propRef->description_property(callGate, descr, addr);
				return resL;
			}, [&](CjvxPropertySubModule* theSubModule) {
				if (descrs.dMin)
				{
					descrs.dMin->globalIdx += theSubModule->offsetShift;
				}
				if (descrs.dFull)
				{
					std::string tag;
					if (!theSubModule->propDescriptionPrefix.empty())
					{
						tag = jvx_makePathExpr(theSubModule->propDescriptorTag,
							descrs.dFull->descriptor.std_str());
					}
					else
					{
						tag = descrs.dFull->descriptor.std_str();
					}

					descrs.dFull->descriptor.assign(tag);
				}
			});

#if 0
		// Assumption: submodule not found
		res = JVX_ERROR_ELEMENT_NOT_FOUND;

		jvxErrorType resL = JVX_NO_ERROR;

		// Need to get cnt offset OUTSIDE the for loop
		jvxSize cnt = 0;
		this->_number_properties_local(callGate, &cnt, theObj);

		for (i = 0; i < _common_set_properties.registeredSubmodules.size(); i++)
		{
			if (addresses.addrLin)
			{
				// _common_set_properties.registeredProperties.size();
				jvxSize numL = 0;

				resL = addresses.prepLin(cnt);
				if (resL == JVX_NO_ERROR)
				{
					resL = _common_set_properties.registeredSubmodules[i].propRef->description_property(callGate, descr, addrProp);
					addresses.postLin();

					if (resL == JVX_NO_ERROR)
					{
						// =============================================================================
						if (descrs.dNames)
						{
							if (_common_set_properties.registeredSubmodules[i].propDescriptionPrefix.size())
							{
								descrs.dNames->descriptor.assign(jvx_makePathExpr(_common_set_properties.registeredSubmodules[i].propDescriptorTag,
									descrs.dNames->descriptor.std_str()));
							}
						}
						descrs.dCore->globalIdx += _common_set_properties.registeredSubmodules[i].offsetShift;
						// ==============================================================================
						res = JVX_NO_ERROR;

						break;
					}
				}
				_common_set_properties.registeredSubmodules[i].propRef->number_properties(callGate, &numL);
				cnt += numL;
			} // 
			else if(addresses.addrDescr)
			{
				resL = addresses.prepDescr(_common_set_properties.registeredSubmodules[i].propDescriptorTag);
				if(resL == JVX_NO_ERROR)
				{
					resL = _common_set_properties.registeredSubmodules[i].propRef->description_property(
								callGate, descr, addrProp);

					// Reset the string pointer to original value
					addresses.postDescr();
				
					if (resL == JVX_NO_ERROR)
					{
						// =========================================================================
						descrs.dCore->globalIdx += _common_set_properties.registeredSubmodules[i].offsetShift;
						// =========================================================================
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
			else if (addresses.addrIdx)
			{
				resL = addresses.prepIdx(_common_set_properties.registeredSubmodules[i].offsetShift);
				if(resL == JVX_NO_ERROR)
				{
					resL = _common_set_properties.registeredSubmodules[i].propRef->description_property(
						callGate, descr, addrProp);
					addresses.postIdx();

					if (resL == JVX_NO_ERROR)
					{
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
		}
#endif
	}

	_unlock_properties_local();
	
leave_error:

	if (res != JVX_NO_ERROR)
	{
		callGate.access_protocol = JVX_ACCESS_PROTOCOL_ERROR;
	}
	
	return(res);
}

/*
jvxErrorType
	CjvxProperties:: _description_property(
		jvxCallManagerProperties& callGate,
		const char* descriptor,
		jvxDataFormat* format,
		jvxSize* num,
		jvxSize* handleIdx,
		jvxPropertyCategoryType* category,
		IjvxObject* theObj)
{
   std::vector<oneProperty>::iterator selection;
   jvxSize idx;

   jvxErrorType res = _check_access_property_obj(callGate, theObj);
   if (res != JVX_NO_ERROR)
	   return res;

   _lock_properties_local();

   selection = findInPropertyList(_common_set_properties.registeredProperties, descriptor);
   if(selection != _common_set_properties.registeredProperties.end())
   {
	   if(format)
	   {
		   *format = selection->format;
	   }
	   if(num)
	   {
		   *num = selection->num;
	   }
	   if(handleIdx)
	   {
		   *handleIdx = selection->globalIdx;
	   }
	   if(category)
	   {
		   *category = selection->category;
	   }
   }
   else
   {
	   jvxBool founditinsubmodule = false;
	   for (idx = 0; idx < _common_set_properties.registeredSubmodules.size(); idx++)
	   {
		   std::string loc_descriptor = descriptor;
		   std::vector<std::string> lst;
		   jvx_decomposePathExpr(loc_descriptor, lst);
		   if (lst.size() > 0)
		   {
			   if (lst[0] == _common_set_properties.registeredSubmodules[idx].propDescriptorTag)
			   {
				   jvx_composePathExpr(lst, loc_descriptor, 1);
				   res = _common_set_properties.registeredSubmodules[idx].propRef->description_property(
					   callGate,
					   loc_descriptor.c_str(),
					   format,
					   num,
					   handleIdx,
					   category);

				   if (res == JVX_NO_ERROR)
				   {
					   if (handleIdx)
					   {
						   *handleIdx += _common_set_properties.registeredSubmodules[idx].offsetShift;
					   }
					   founditinsubmodule = true;
					   break;
				   }
			   }
		   }
	   }
	   if (!founditinsubmodule)
	   {

		   res = JVX_ERROR_ELEMENT_NOT_FOUND;
	   }
   }

   _unlock_properties_local();

	return res;
}
*/

jvxErrorType 
CjvxProperties::_max_property_span(jvxSize* idSpan)
{
	if (idSpan)
	{
		*idSpan = _common_set_properties.propIdSpan;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxProperties::_set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawData,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& detail)
{
	jvxErrorType res = JVX_NO_ERROR;
	const jvx::propertyRawPointerType::IjvxRawPointerType* rawPtr = &rawData;
	jvxSize i;
	std::list<oneProperty>::iterator selection;
	jvxState stat = JVX_STATE_NONE;
	jvxSize maxSpace = 0;
	jvxSize szElem = 0;
	jvxBool propAccessible = false;

	// A cast to generic is possible with any format argument	
	JVX_TRANSLATE_PROP_GENERIC_FROM_RAW(rawData);

	jvxDataFormat formatLocal = format;
	//jvxSize offsetStart = 0;
	jvxSize numElementsLocal = numElements;
	//jvxBool contentOnly = false;

	propAddressing addresses;
	addresses.cast(&ident);

	callGate.access_protocol = JVX_ACCESS_PROTOCOL_OK;
	
	res = _check_access_property_obj(callGate);
	if (res != JVX_NO_ERROR)
	{
		goto leave_error;
	}

	_lock_properties_local();

	selection = findSelectionFromAddress(addresses);

	if (selection != _common_set_properties.registeredProperties.end())
	{
		res = _check_access_property(callGate, *selection);
		if (res != JVX_NO_ERROR)
		{
			res = JVX_ERROR_NO_ACCESS;
			goto leave_function_unlock;
		}
		
		theObjRef._state(&stat);
		if ((stat & selection->propDescriptor->allowedStateMask) == 0)
		{
			res = JVX_ERROR_WRONG_STATE;
			goto leave_function_unlock;
		}

		// Property and categ are taken from the selection element
		const jvx::propertyAddress::CjvxPropertyAddressGlobalId identHook(
			selection->propDescriptor->globalIdx,
			selection->propDescriptor->category);

		jvx::propertyDetail::CjvxTranferDetail tune(detail.contentOnly, detail.offsetStart);

		res = _run_hook(selection->callbacks.callback_set_prehook,
			selection->propDescriptor->name.std_str(),
			callGate, rawPtr, identHook, tune, &selection->extended_props,
			JVX_PROPERTY_CALLBACK_SET);
		if (res != JVX_NO_ERROR)
		{
			if (res == JVX_ERROR_ABORT)
			{
				// This is the case if the callback has shortened the procedure
				// A shortcut procedure may be due to an embedded device in a device:
				// If the embedded device has run the get_property function successfully,
				// we do not need to run through all checks again.
				// This reduces the processing time
				res = JVX_NO_ERROR;
			}
			goto leave_function_unlock;
		}

		numElementsLocal = JVX_MIN(numElementsLocal, selection->propDescriptor->num - tune.offsetStart);
		
		// At least one element should remain
		if (numElementsLocal == 0)
		{
			res = JVX_ERROR_INVALID_SETTING;
			goto leave_function_unlock;
		}

		if (tune.contentOnly)
		{
			if (
				(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_WRITE_ONLY) ||
				(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE) ||
				(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT) ||
				(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_ONLY_ON_START) ||
				((selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_ONLY) &&
					(callGate.call_purpose == JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)))
			{
				propAccessible = true;
			}
		}
		else
		{
			if (
				(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_WRITE_ONLY) ||
				(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE) ||
				(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_ONLY_ON_START))
			{
				propAccessible = true;
			}
		}

		if (!propAccessible)
		{
			res = JVX_ERROR_NO_ACCESS;
			callGate.access_protocol = JVX_ACCESS_PROTOCOL_NO_ACCESS__WRITE;			
			goto leave_function_unlock;
		}

		if (fld == NULL)
		{
			if (selection->propDescriptor->format != JVX_DATAFORMAT_CALLBACK)
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
				goto leave_function_unlock;
			}
		}
		
		if (format != selection->propDescriptor->format)
		{
			if (
				(format == JVX_DATAFORMAT_STRING) &&
				(selection->propDescriptor->format == JVX_DATAFORMAT_SELECTION_LIST))
			{
				// ok
			}
			else
			{
				convert.convertFilterStart(fld, selection->propDescriptor->format, format);
				if (format != selection->propDescriptor->format)
				{
					if (
						((format == JVX_DATAFORMAT_16BIT_LE) && (selection->propDescriptor->format == JVX_DATAFORMAT_U16BIT_LE)) ||
						((format == JVX_DATAFORMAT_U16BIT_LE) && (selection->propDescriptor->format == JVX_DATAFORMAT_16BIT_LE)))
					{
						// We need to catch this case since I have re-defined JVX_DATAFORMAT_BOOL to JVX_DATAFORMAT_U16BIT_LE
						assert(0);
					}
					res = JVX_ERROR_INVALID_ARGUMENT;
					goto leave_function_unlock;
				}
			}
		}
		
		callGate.on_set.modification_impact = true;
		if (selection->references.fld)
		{
			switch (formatLocal)
			{
			case JVX_DATAFORMAT_STRING:
				if (numElementsLocal == 1)
				{
					if (selection->propDescriptor->format == JVX_DATAFORMAT_SELECTION_LIST)
					{
						jvxSelectionList_cpp* selL = (jvxSelectionList_cpp*)selection->references.fld;
						if (tune.offsetStart < selL->entries.size())
						{
							jvxApiString* fldIn = (jvxApiString*)fld;

							selL->entries[tune.offsetStart] = fldIn->std_str();
						}
					}
					else if (selection->propDescriptor->decTp == JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG)
					{
						IjvxLocalTextLog* localLog = (IjvxLocalTextLog*)selection->references.fld;
						jvxApiString* fldIn = (jvxApiString*)fld;
						if (localLog && fldIn)
						{
							// Add the passed parameter to logfile - make sure, threading issues are safe!!
							localLog->attach_entry(fldIn->std_str().c_str());
						}
						else
						{
							res = JVX_ERROR_INTERNAL;
							goto leave_function_unlock;
						}
					}
					else
					{
						std::string* str = (std::string*)selection->references.fld;
						if (fld)
						{
							if (_common_set_properties.reportIfNoChange)
							{
								if (*str == ((jvxApiString*)fld)->std_str())
								{
									callGate.on_set.modification_impact = false;
								}
							}
							*str = ((jvxApiString*)fld)->std_str();
						}
						else
						{
							res = JVX_ERROR_INVALID_ARGUMENT;
							goto leave_function_unlock;
						}
					}
				}
				else
				{
					res = JVX_ERROR_SIZE_MISMATCH;
					goto leave_function_unlock;
				}
				break;
			case JVX_DATAFORMAT_STRING_LIST:
				if (numElementsLocal == 1)
				{

					std::vector<std::string>* strL = (std::vector<std::string>*)selection->references.fld;
					jvxApiStringList* strLIn = (jvxApiStringList*)fld;
					assert(strLIn);
					assert(strL);

					if (_common_set_properties.reportIfNoChange)
					{
						if (strLIn->ll() == strL->size())
						{
							jvxBool foundDiff = false;
							for (i = 0; i < strLIn->ll(); i++)
							{
								if ((*strL)[i] != strLIn->std_str_at(i))
								{
									foundDiff = true;
									break;
								}
							}
							if (!foundDiff)
							{
								callGate.on_set.modification_impact = false;
							}
						}
					}

					strL->clear();
					if (strLIn)
					{
						for (i = 0; i < strLIn->ll(); i++)
						{
							strL->push_back(strLIn->std_str_at(i));
						}
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
						goto leave_function_unlock;
					}
				}
				else
				{
					res = JVX_ERROR_SIZE_MISMATCH;
					goto leave_function_unlock;
				}
				break;

			case JVX_DATAFORMAT_SELECTION_LIST:
			{
				jvxSelectionList_cpp* selL = (jvxSelectionList_cpp*)selection->references.fld;
				jvxSelectionList* selLIn = (jvxSelectionList*)fld;

				assert(selLIn);
				assert(selL);

				if (_common_set_properties.reportIfNoChange)
				{
					callGate.on_set.modification_impact = false;
					selL->content_copy(*selLIn, tune.offsetStart, 0, JVX_SIZE_UNSELECTED, &callGate.on_set.modification_impact);
				}
				else
				{
					selL->content_copy(*selLIn, tune.offsetStart, 0, JVX_SIZE_UNSELECTED, nullptr);
				}

				if (!tune.contentOnly)
				{
					if (_common_set_properties.reportIfNoChange)
					{
						if (selLIn->strList.ll() == selL->entries.size())
						{
							jvxBool foundDiff = false;
							for (i = 0; i < selLIn->strList.ll(); i++)
							{
								if (i < selL->entries.size())
								{
									if (selL->entries[i] != selLIn->strList.std_str_at(i))
									{
										foundDiff = true;
										break;
									}
								}
								else
								{
									foundDiff = true;
									break;
								}
							}
							if (foundDiff)
							{
								callGate.on_set.modification_impact = true;
							}
						}
					}

					selL->entries.clear();
					for (i = 0; i < selLIn->strList.ll(); i++)
					{
						selL->entries.push_back(selLIn->strList.std_str_at(i));
					}

					selL->exclusive = selLIn->bitFieldExclusive;
					selL->selectable = selLIn->bitFieldSelectable;
				}
			}
				break;

			case JVX_DATAFORMAT_VALUE_IN_RANGE:
			{
				jvxValueInRange* selL = (jvxValueInRange*)selection->references.fld;
				jvxValueInRange* selVal = (jvxValueInRange*)fld;
				assert(selL);
				assert(selVal);
				if (_common_set_properties.reportIfNoChange)
				{
					selL->content_copy(*selVal, tune.offsetStart, 0, JVX_SIZE_UNSELECTED, &callGate.on_set.modification_impact);
				}
				else
				{
					selL->content_copy(*selVal, tune.offsetStart, 0);
				}

				if (!tune.contentOnly)
				{
					if (
						(selL->maxVal != selVal->maxVal) ||
						(selL->minVal != selVal->minVal))
					{
						callGate.on_set.modification_impact = true;
					}
				}
			}
				break;
			case JVX_DATAFORMAT_DATA:


				if (numElementsLocal + tune.offsetStart <= selection->propDescriptor->num)
				{
					jvxData* ptrOut = (jvxData*)selection->references.fld;
					ptrOut += tune.offsetStart;
					jvxData* ptrIn = (jvxData*)fld;
					if (_common_set_properties.reportIfNoChange)
					{
						if (
							(tune.offsetStart == 0) &&
							(selection->propDescriptor->num == 1))
						{
							if (*ptrIn == *ptrOut)
							{
								callGate.on_set.modification_impact = false;
							}
						}
					}
					memcpy(ptrOut, ptrIn, sizeof(jvxData) * numElementsLocal);
				}
				else
				{
					res = JVX_ERROR_INVALID_SETTING;
					goto leave_function_unlock;
				}
				break;
			case JVX_DATAFORMAT_8BIT:
			case JVX_DATAFORMAT_U8BIT:
				if (numElementsLocal + tune.offsetStart <= selection->propDescriptor->num)
				{
					jvxInt8* ptrOut = (jvxInt8*)selection->references.fld;
					ptrOut += tune.offsetStart;
					jvxInt8* ptrIn = (jvxInt8*)fld;
					if (_common_set_properties.reportIfNoChange)
					{
						if (
							(tune.offsetStart == 0) &&
							(selection->propDescriptor->num == 1))
						{
							if (*ptrIn == *ptrOut)
							{
								callGate.on_set.modification_impact = false;
							}
						}
					}
					memcpy(ptrOut, ptrIn, sizeof(jvxInt8) * numElementsLocal);
				}
				else
				{
					res = JVX_ERROR_INVALID_SETTING;
					goto leave_function_unlock;
				}
				break;
			case JVX_DATAFORMAT_16BIT_LE:
			case JVX_DATAFORMAT_U16BIT_LE:
				if (numElementsLocal + tune.offsetStart <= selection->propDescriptor->num)
				{
					jvxInt16* ptrOut = (jvxInt16*)selection->references.fld;
					ptrOut += tune.offsetStart;
					jvxInt16* ptrIn = (jvxInt16*)fld;
					if (_common_set_properties.reportIfNoChange)
					{
						if (
							(tune.offsetStart == 0) &&
							(selection->propDescriptor->num == 1))
						{
							if (*ptrIn == *ptrOut)
							{
								callGate.on_set.modification_impact = false;
							}
						}
					}
					memcpy(ptrOut, ptrIn, sizeof(jvxInt16) * numElementsLocal);
				}
				else
				{
					res = JVX_ERROR_INVALID_SETTING;
					goto leave_function_unlock;
				}
				break;
			case JVX_DATAFORMAT_32BIT_LE:
			case JVX_DATAFORMAT_U32BIT_LE:
				if (numElementsLocal + tune.offsetStart <= selection->propDescriptor->num)
				{
					jvxInt32* ptrOut = (jvxInt32*)selection->references.fld;
					ptrOut += tune.offsetStart;
					jvxInt32* ptrIn = (jvxInt32*)fld;
					if (_common_set_properties.reportIfNoChange)
					{
						if (
							(tune.offsetStart == 0) &&
							(selection->propDescriptor->num == 1))
						{
							if (*ptrIn == *ptrOut)
							{
								callGate.on_set.modification_impact = false;
							}
						}
					}
					memcpy(ptrOut, ptrIn, sizeof(jvxInt32) * numElementsLocal);
				}
				else
				{
					res = JVX_ERROR_INVALID_SETTING;
					goto leave_function_unlock;
				}
				break;
			case JVX_DATAFORMAT_64BIT_LE:
			case JVX_DATAFORMAT_U64BIT_LE:
				if (numElementsLocal + tune.offsetStart <= selection->propDescriptor->num)
				{
					jvxInt64* ptrOut = (jvxInt64*)selection->references.fld;
					ptrOut += tune.offsetStart;
					jvxInt64* ptrIn = (jvxInt64*)fld;
					if (_common_set_properties.reportIfNoChange)
					{
						if (
							(tune.offsetStart == 0) &&
							(selection->propDescriptor->num == 1))
						{
							if (*ptrIn == *ptrOut)
							{
								callGate.on_set.modification_impact = false;
							}
						}
					}
					memcpy(ptrOut, ptrIn, sizeof(jvxInt64) * numElementsLocal);
				}
				else
				{
					res = JVX_ERROR_INVALID_SETTING;
					goto leave_function_unlock;
				}
				break;

			case JVX_DATAFORMAT_HANDLE:

				// HANDLE treated in callback only
				break;
			case JVX_DATAFORMAT_INTERFACE:

				assert(0); // must use the "install" callback
				// Pass pointer to field
				*((IjvxPropertyExtender**)selection->references.fld) = reinterpret_cast<IjvxPropertyExtender*>(fld);
				break;
			default:
				maxSpace = (selection->propDescriptor->num - tune.offsetStart);
				szElem = jvx_numberBytesField(1, formatLocal, 1);
				if (numElementsLocal <= maxSpace)
				{
					memcpy(((jvxByte*)selection->references.fld + (szElem * tune.offsetStart)), fld, szElem * numElementsLocal);
				}
				else
				{
					res = JVX_ERROR_SIZE_MISMATCH;
					goto leave_function_unlock;
				}
			}
		}
		else
		{
			// A nullptr fld may be useful if setting the pointer value to zero!
			switch (formatLocal)
			{
			case JVX_DATAFORMAT_CALLBACK:
				if (selection->references.fld_prop_cb)
				{
					if (numElementsLocal == 1)
					{
						if (fld != NULL)
						{
							if (selection->references.fld_prop_cb)
							{
								if (*selection->references.fld_prop_cb == NULL)
								{
									*selection->references.fld_prop_cb = (jvxCallbackPrivate*)fld;
								}
								else
								{
									res = JVX_ERROR_ALREADY_IN_USE;
									goto leave_function_unlock;
								}
							}
							else
							{
								res = JVX_ERROR_WRONG_STATE;
								goto leave_function_unlock;
							}
						}
						else
						{
							if (selection->references.fld_prop_cb)
							{
								if (*selection->references.fld_prop_cb != NULL)
								{
									*selection->references.fld_prop_cb = NULL;
								}
								// It is allowed to reset it multiple times
							}
							else
							{
								res = JVX_ERROR_WRONG_STATE;
								goto leave_function_unlock;
							}
						}
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
						goto leave_function_unlock;
					}

				}
				break;
			case JVX_DATAFORMAT_INTERFACE:

				assert(0); // must use the "install" callback
				// Reset pointer to field
				*((IjvxPropertyExtender**)selection->references.fld) = nullptr;
				break;
			default:
				res = JVX_ERROR_NOT_READY;
				goto leave_function_unlock;
			}
		}		
		
		convert.convertFilterStop(fld, selection->propDescriptor->format, format, false); 
		
		assert(res == JVX_NO_ERROR);
		res = _run_hook(selection->callbacks.callback_set_posthook,
			selection->propDescriptor->name.std_str(),
			callGate, rawPtr, identHook, tune, &selection->extended_props,
			JVX_PROPERTY_CALLBACK_SET);

		if (res == JVX_NO_ERROR)
		{
			if (callGate.on_set.report_set || _common_set_properties.allSetWithReport)
			{
				jvxErrorType resL = add_property_report_collect(selection->propDescriptor->descriptor.std_str());
				// What to do with resL??
			}
		}

	} // if (selection != _common_set_properties.registeredProperties.end())
	else
	{
		res = _forward_submodules(callGate, addresses, [&](const jvx::propertyAddress::IjvxPropertyAddress& addr, CjvxPropertySubModule* theSubModule)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			resL = theSubModule->propRef->set_property(
				callGate,
				rawData,
				/*
				fld,
				numElementsLocal,
				formatLocal,
				*/
				addr, detail);				
			return resL;
		});

	} // else :: if (selection != _common_set_properties.registeredProperties.end())

	
leave_function_unlock:

	convert.convertFilterCancel();
	_unlock_properties_local();

leave_error:

	if (res != JVX_NO_ERROR)
	{
		if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
		{
			callGate.access_protocol = JVX_ACCESS_PROTOCOL_ERROR;
		}
	}

	return res;
}

jvxErrorType
CjvxProperties::_get_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawData,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& detail)
{
	jvxErrorType res = JVX_NO_ERROR;
	const jvx::propertyRawPointerType::IjvxRawPointerType* rawPtr = &rawData;
	std::list<oneProperty>::iterator selection;
	jvxState stat = JVX_STATE_NONE;
	jvxSize maxSpace = 0;
	jvxSize szElem = 0;
	jvxBool propAccessable = false;

	JVX_TRANSLATE_PROP_GENERIC_FROM_RAW(rawData);

	jvxDataFormat formatLocal = format;
	//jvxSize offsetStart = 0;
	jvxSize numElementsLocal = numElements;
	//jvxBool contentOnly = false;

	// Initialize some of the "additional" returns
	if (callGate.on_get.prop_access_type)
		*callGate.on_get.prop_access_type = JVX_PROPERTY_ACCESS_NONE;

	if (callGate.on_get.is_valid)
		*callGate.on_get.is_valid = false;

	propAddressing addresses;
	addresses.cast(&ident);

	callGate.access_protocol = JVX_ACCESS_PROTOCOL_OK;

	res = _check_access_property_obj(callGate);
	if (res != JVX_NO_ERROR)
	{
		goto leave_error;
	}

	_lock_properties_local();

	selection = findSelectionFromAddress(addresses);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		res = _check_access_property(callGate, *selection);
		if (res != JVX_NO_ERROR)
		{
			//if (callPurpose != JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY) does it make sense to return size if access is not ok?

			res = JVX_ERROR_NO_ACCESS;
			goto leave_function_unlock;
		}

		jvx::propertyDetail::CjvxTranferDetail tune(detail.contentOnly, detail.offsetStart);
		// Property and categ are taken from the selection element
		const jvx::propertyAddress::CjvxPropertyAddressGlobalId identHook(
			selection->propDescriptor->globalIdx,
			selection->propDescriptor->category);

		// These parameters are details and therefore taken from the 
		// passed struct if any
		//contentOnly = detail.contentOnly;
		//offsetStart = detail.offsetStart;		

		theObjRef._state(&stat);
		if ((stat & selection->propDescriptor->allowedStateMask) == 0)
		{
			if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY)
			{
				res = JVX_ERROR_WRONG_STATE;
				goto leave_function_unlock;
			}
		}


		res = _run_hook(selection->callbacks.callback_get_prehook,
			selection->propDescriptor->name.std_str(),
			callGate, rawPtr, identHook, tune, &selection->extended_props,
			JVX_PROPERTY_CALLBACK_GET);

		if (res != JVX_NO_ERROR)
		{
			if (res == JVX_ERROR_ABORT)
			{
				// This is the case if the callback has shortened the procedure
				// A shortcut procedure may be due to an embedded device in a device:
				// If the embedded device has run the get_property function successfully,
				// we do not need to run through all checks again.
				// This reduces the processing time
				res = JVX_NO_ERROR;
			}
			goto leave_function_unlock;
		}

		// Store access type if desired
		if (callGate.on_get.prop_access_type)
		{
			*callGate.on_get.prop_access_type = selection->propDescriptor->accessType;
		}

		if (callGate.on_get.is_valid)
		{
			*callGate.on_get.is_valid = selection->propDescriptor->isValid;
			if (selection->references.isValidPtr)
			{
				if (*selection->references.isValidPtr == false)
				{
					*callGate.on_get.is_valid = false;
				}
				else
				{
					// Everything OK, variable set on entry
				}
			}
			else
			{
				// Everything OK, if variable is not specified it is always valid
			}
		}

		if (fld == NULL)
		{
			// Special rule to only return isValid and accessType in a "get"
			res = JVX_NO_ERROR;

			// POSITION#1 We need to specify this to jump out of this function - to distinguish between error and desired function
			if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_GET_ACCESS_DATA)
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
			// ================================================================
			// Break this call here without error!
			// ================================================================
			goto leave_function_unlock;

		}

		// ================================================================
		// ================================================================
		// ================================================================

		if (
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE) ||
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_ONLY) ||	
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT) ||
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_ONLY_ON_START))
		{
			propAccessable = true;
		}

		// If we reach here, there is really the desire to get data
		if (!propAccessable)
		{
			if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY)
			{
				callGate.access_protocol = JVX_ACCESS_PROTOCOL_NO_ACCESS__READ;
				res = JVX_ERROR_NO_ACCESS;
				goto leave_function_unlock;
			}
		}

		// The position of this check is well selected: Calling with 0 element size may be valid if we only 
		// determine the accessType (callGate.call_purpose == JVX_PROPERTY_CALL_PURPOSE_GET_ACCESS_DATA)
		// Refer to <POSITION#1>
		if (numElementsLocal == 0)
		{
			res = JVX_ERROR_INVALID_SETTING;
			goto leave_function_unlock;
		}

		if (formatLocal != selection->propDescriptor->format)
		{
			if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY)
			{
				// Special rule:
				if (
					(formatLocal == JVX_DATAFORMAT_STRING) &&
					(selection->propDescriptor->format == JVX_DATAFORMAT_SELECTION_LIST))
				{
					// ok
				}
				else
				{
					if ((selection->propDescriptor->num == 1) && (tune.offsetStart == 0))
					{
						// Corrected on 03.07.2023 HK - this led to a segmentation fault!!
						// The auto conversion only works in case of a single value. More than one value can not be corrected!!
						convert.convertFilterStart(fld, selection->propDescriptor->format, formatLocal);
					}
					if (formatLocal != selection->propDescriptor->format)
					{
						if (
							((format == JVX_DATAFORMAT_16BIT_LE) && (selection->propDescriptor->format == JVX_DATAFORMAT_U16BIT_LE)) ||
							((format == JVX_DATAFORMAT_U16BIT_LE) && (selection->propDescriptor->format == JVX_DATAFORMAT_16BIT_LE)))
						{
							// We need to catch this case since I have re-defined JVX_DATAFORMAT_BOOL to JVX_DATAFORMAT_U16BIT_LE
							assert(0);
						}
						res = JVX_ERROR_INVALID_ARGUMENT;
						goto leave_function_unlock;
					}
				}
			}
		}

		if (res == JVX_NO_ERROR)
		{
			if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY)
			{
				jvxBool isValid = selection->propDescriptor->isValid;
				if (selection->references.isValidPtr)
				{
					isValid = *selection->references.isValidPtr;
				}
				if (isValid == false)
				{
					callGate.access_protocol = JVX_ACCESS_PROTOCOL_INVALID;
					res = JVX_NO_ERROR;
					goto leave_function_unlock;
				}
			}
		}

		

		if (res == JVX_NO_ERROR)
		{
			if (selection->references.fld)
			{
				if (callGate.call_purpose == JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY)
				{
					if (formatLocal == JVX_DATAFORMAT_SIZE)
					{
						jvxSize* returnVal = (jvxSize*)fld;
						switch (selection->propDescriptor->format)
						{
						case JVX_DATAFORMAT_SELECTION_LIST:
						{
							jvxSelectionList_cpp* selL = (jvxSelectionList_cpp*)selection->references.fld;
							*returnVal = selL->entries.size();
						}
						break;
						case JVX_DATAFORMAT_STRING_LIST:
						{
							std::vector<std::string>* strL = (std::vector<std::string>*)selection->references.fld;
							*returnVal = strL->size();
						}
						break;
						case JVX_DATAFORMAT_DATA:
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_U16BIT_LE:
						case JVX_DATAFORMAT_U32BIT_LE:
						case JVX_DATAFORMAT_U64BIT_LE:
							*returnVal = selection->propDescriptor->num;
							break;
						default:
							res = JVX_ERROR_UNSUPPORTED;
							goto leave_function_unlock;
						}
					}
				}
				else
				{
					switch (formatLocal)
					{
					case JVX_DATAFORMAT_STRING:
						if (numElementsLocal == 1)
						{
							if (selection->propDescriptor->format == JVX_DATAFORMAT_SELECTION_LIST)
							{
								jvxSelectionList_cpp* selL = (jvxSelectionList_cpp*)selection->references.fld;
								if (tune.offsetStart < selL->entries.size())
								{
									((jvxApiString*)fld)->assign(selL->entries[tune.offsetStart]);
								}
								else
								{
									res = JVX_ERROR_ELEMENT_NOT_FOUND;
									goto leave_function_unlock;
								}
							}
							else if (selection->propDescriptor->decTp == JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG)
							{
								IjvxLocalTextLog* localLog = (IjvxLocalTextLog*)selection->references.fld;
								jvxApiString* fldRet = (jvxApiString*)fld;
								jvxSize numCharsLost = 0;
								jvxBool newReadStarted = false;
								jvxApiString txtLogOut;
								jvxApiString txtLogNew;
								jvxApiString txtLogLost;
								std::string txtLog;
								if (localLog && fldRet)
								{
									localLog->read_update(&txtLogOut, callGate.context, &numCharsLost, &newReadStarted);

									if (newReadStarted)
									{
										localLog->get_text_new_read(&txtLogNew);
										txtLog += txtLogNew.std_str();
									}

									if (numCharsLost > 0)
									{
										localLog->get_text_lost_characters(&txtLogLost, numCharsLost);
										txtLog += txtLogLost.std_str();
									}

									txtLog = txtLogOut.std_str();
									fldRet->assign(txtLog);
								}
								else
								{
									res = JVX_ERROR_INTERNAL;
									goto leave_function_unlock;
								}
							}
							else
							{
								std::string* str = (std::string*)selection->references.fld;
								((jvxApiString*)fld)->assign(*str);
							}
						}
						else
						{
							res = JVX_ERROR_SIZE_MISMATCH;
							goto leave_function_unlock;
						}
						break;
					case JVX_DATAFORMAT_STRING_LIST:
						if (numElementsLocal == 1)
						{
							std::vector<std::string>* strL = (std::vector<std::string>*)selection->references.fld;
							assert(strL);
							if (fld)
							{
								if (tune.contentOnly)
								{
									((jvxApiStringList*)fld)->assign_empty(strL->size());
								}
								else
								{
									((jvxApiStringList*)fld)->assign(*strL);
								}
							}
						}
						else
						{
							res = JVX_ERROR_SIZE_MISMATCH;
							goto leave_function_unlock;
						}
						break;
					case JVX_DATAFORMAT_SELECTION_LIST:
					{
						jvxSelectionList_cpp* selL = (jvxSelectionList_cpp*)selection->references.fld;
						jvxSelectionList* returnVal = (jvxSelectionList*)fld;

						assert(selL);
						assert(returnVal);
						returnVal->content_copy(*selL, 0, tune.offsetStart);

						if (tune.contentOnly)
						{
							returnVal->strList.assign_empty(selL->entries.size());
						}
						else
						{
							returnVal->bitFieldExclusive = selL->exclusive;
							returnVal->bitFieldSelectable = selL->selectable;
							returnVal->strList.assign(selL->entries);
						}
					}
						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:
						{
							jvxValueInRange* selL = (jvxValueInRange*)selection->references.fld;
							jvxValueInRange* selVal = (jvxValueInRange*)fld;
							selVal->content_copy(*selL);

							if (!tune.contentOnly)
							{
								selVal->maxVal = selL->maxVal;
								selVal->minVal = selL->minVal;
							}
						}
						break;
					case JVX_DATAFORMAT_DATA:
						if (numElementsLocal + tune.offsetStart <= selection->propDescriptor->num)
						{
							jvxData* ptrOut = (jvxData*)fld;
							jvxData* ptrIn = (jvxData*)selection->references.fld;
							ptrIn += tune.offsetStart;
							memcpy(ptrOut, ptrIn, sizeof(jvxData) * numElementsLocal);
						}
						else
						{
							res = JVX_ERROR_INVALID_SETTING;
							goto leave_function_unlock;
						}
						break;
					case JVX_DATAFORMAT_8BIT:
					case JVX_DATAFORMAT_U8BIT:
						if (numElementsLocal + tune.offsetStart <= selection->propDescriptor->num)
						{
							jvxInt8* ptrOut = (jvxInt8*)fld;
							jvxInt8* ptrIn = (jvxInt8*)selection->references.fld;
							ptrIn += tune.offsetStart;
							memcpy(ptrOut, ptrIn, sizeof(jvxInt8) * numElementsLocal);
						}
						else
						{
							res = JVX_ERROR_INVALID_SETTING;
							goto leave_function_unlock;
						}
						break;
					case JVX_DATAFORMAT_16BIT_LE:
					case JVX_DATAFORMAT_U16BIT_LE:
						if (numElementsLocal + tune.offsetStart <= selection->propDescriptor->num)
						{
							jvxInt16* ptrOut = (jvxInt16*)fld;
							jvxInt16* ptrIn = (jvxInt16*)selection->references.fld;
							ptrIn += tune.offsetStart;
							memcpy(ptrOut, ptrIn, sizeof(jvxInt16) * numElementsLocal);
						}
						else
						{
							res = JVX_ERROR_INVALID_SETTING;
							goto leave_function_unlock;
						}
						break;
					case JVX_DATAFORMAT_32BIT_LE:
					case JVX_DATAFORMAT_U32BIT_LE:
						if (numElementsLocal + tune.offsetStart <= selection->propDescriptor->num)
						{
							jvxInt32* ptrOut = (jvxInt32*)fld;
							jvxInt32* ptrIn = (jvxInt32*)selection->references.fld;
							ptrIn += tune.offsetStart;
							memcpy(ptrOut, ptrIn, sizeof(jvxInt32) * numElementsLocal);
						}
						else
						{
							res = JVX_ERROR_INVALID_SETTING;
							goto leave_function_unlock;
						}
						break;
					case JVX_DATAFORMAT_64BIT_LE:
					case JVX_DATAFORMAT_U64BIT_LE:
						if (numElementsLocal + tune.offsetStart <= selection->propDescriptor->num)
						{
							jvxInt64* ptrOut = (jvxInt64*)fld;
							jvxInt64* ptrIn = (jvxInt64*)selection->references.fld;
							ptrIn += tune.offsetStart;
							memcpy(ptrOut, ptrIn, sizeof(jvxInt64) * numElementsLocal);
						}
						else
						{
							res = JVX_ERROR_INVALID_SETTING;
							goto leave_function_unlock;
						}
						break;
					case JVX_DATAFORMAT_HANDLE:

						// Do not do anything, use the get_callback for data exchange
						break;
					case JVX_DATAFORMAT_INTERFACE:

						// This is a write only parameter
						res = JVX_ERROR_NO_ACCESS;
						goto leave_function_unlock;
						break;

					default:
						maxSpace = (selection->propDescriptor->num - tune.offsetStart);
						szElem = jvx_numberBytesField(1, formatLocal, 1);
						if (numElementsLocal <= maxSpace)
						{
							memcpy(fld, ((jvxByte*)selection->references.fld + (szElem * tune.offsetStart)), szElem * numElementsLocal);
						}
						else
						{
							res = JVX_ERROR_SIZE_MISMATCH;
							goto leave_function_unlock;
						}
					}
				}
			}
			else
			{
				// The <selection->references.fld> is the OLD style, interfaces are 
				// implemented the NEW way
				callGate.access_protocol = JVX_ACCESS_PROTOCOL_NO_DATA;
				
				if (numElementsLocal == 1)
				{
					const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxLocalTextLog>* ptrRawLog =
						castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxLocalTextLog> >(
							selection->rawpointer, selection->propDescriptor->format);

					const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxPropertyExtender>* ptrRawExtFrom =
						castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxPropertyExtender> >(
							selection->rawpointer, selection->propDescriptor->format);
					const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxPropertyExtender>* ptrRawExtTo =
						castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxPropertyExtender> >(
							rawPtr, selection->propDescriptor->format);

					switch (selection->propDescriptor->decTp)
					{
					case JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG:

						switch (formatLocal)
						{
						case JVX_DATAFORMAT_STRING:
							
							if (ptrRawLog)
							{
								IjvxLocalTextLog** localLogCont = ptrRawLog->typedPointer();
								if (localLogCont)
								{
									IjvxLocalTextLog* localLog = *localLogCont;
									jvxApiString* fldRet = (jvxApiString*)fld;
									jvxSize numCharsLost = 0;
									jvxBool newReadStarted = false;
									jvxApiString txtLogOut;
									jvxApiString txtLogNew;
									jvxApiString txtLogLost;
									std::string txtLog;
									if (localLog && fldRet)
									{
										callGate.access_protocol = JVX_ACCESS_PROTOCOL_ERROR;
										localLog->read_update(&txtLogOut, callGate.context, &numCharsLost, &newReadStarted);

										if (newReadStarted)
										{
											localLog->get_text_new_read(&txtLogNew);
											txtLog += txtLogNew.std_str();
										}

										if (numCharsLost > 0)
										{
											localLog->get_text_lost_characters(&txtLogLost, numCharsLost);
											txtLog += txtLogLost.std_str();
										}

										txtLog = txtLogOut.std_str();
										fldRet->assign(txtLog);
									}
									else
									{
										if (localLog == nullptr)
										{
											callGate.last_err_hint = "No local log associated.";
										}
										res = JVX_ERROR_INTERNAL;
										goto leave_function_unlock;
									}
								}
							}
							break;
						}
						break;
					case JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE:
						switch (formatLocal)
						{ 
						case JVX_DATAFORMAT_INTERFACE:
						
							// castPropIfPointer
							if (ptrRawExtFrom && ptrRawExtTo)
							{
								auto ptrFrom = ptrRawExtFrom->typedPointer();
								auto ptrTo = ptrRawExtTo->typedPointer();
								if (ptrFrom && ptrTo)
								{
									*ptrTo = *ptrFrom;
								}
							}
							else
							{
								res = JVX_ERROR_UNSUPPORTED;
								goto leave_function_unlock;
							}
							break;
						default:
							res = JVX_ERROR_UNSUPPORTED;
							goto leave_function_unlock;
							break;
						}
						break;

					default:
						res = JVX_ERROR_INVALID_ARGUMENT;
						goto leave_function_unlock;
						break;
					}
				}
				// We may use pre and posthook even with non-registered data!!!
			}
		}

		convert.convertFilterStop(fld, selection->propDescriptor->format, formatLocal, true);

		if (res == JVX_NO_ERROR)
		{
			if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY)
			{
				res = _run_hook(selection->callbacks.callback_get_posthook,
					selection->propDescriptor->name.std_str(),
					callGate, rawPtr, identHook, tune, &selection->extended_props,
					JVX_PROPERTY_CALLBACK_GET);
			}
		}
	}
	else
	{
		res = _forward_submodules(callGate, addresses, [&](const jvx::propertyAddress::IjvxPropertyAddress& addr, CjvxPropertySubModule* theSubModule)
			{
				jvxErrorType resL = JVX_NO_ERROR;
				resL = theSubModule->propRef->get_property(
					callGate,
					rawData,
					addr, detail);
				return resL;
			});
	}

leave_function_unlock:

	_unlock_properties_local();

leave_error:

	if (res != JVX_NO_ERROR)
	{
		callGate.access_protocol = JVX_ACCESS_PROTOCOL_ERROR;
	}
	else
	{
		callGate.last_err_hint.clear();
	}

	return res;
}

jvxErrorType
CjvxProperties::_install_property_reference(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawData,
	const jvx::propertyAddress::IjvxPropertyAddress& ident)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<oneProperty>::iterator selection;
	jvxState stat = JVX_STATE_NONE;
	jvxBool propAccessible = false;
	
	const jvx::propertyRawPointerType::IjvxRawPointerType* ptrRaw = &rawData;

	propAddressing addresses;
	addresses.cast(&ident);

	// Find property identification

	res = _check_access_property_obj(callGate);
	if (res != JVX_NO_ERROR)
	{
		goto leave_error;
	}

	_lock_properties_local();

	selection = findSelectionFromAddress(addresses);

		//findInPropertyList(_common_set_properties.registeredProperties, propId, category);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		res = _check_access_property(callGate, *selection);
		if (res != JVX_NO_ERROR)
		{
			res = JVX_ERROR_NO_ACCESS;
			goto leave_function_unlock;
		}

		theObjRef._state(&stat);
		if ((stat & selection->propDescriptor->allowedStateMask) == 0)
		{
			res = JVX_ERROR_WRONG_STATE;
			goto leave_function_unlock;
		}


		jvx::propertyDetail::CjvxTranferDetail tune;
		const jvx::propertyAddress::CjvxPropertyAddressGlobalId identHook(
			selection->propDescriptor->globalIdx,
			selection->propDescriptor->category);

		res = _run_hook(selection->callbacks.callback_set_prehook,
			selection->propDescriptor->name.std_str(),
			callGate, ptrRaw, identHook, tune, &selection->extended_props,
			JVX_PROPERTY_CALLBACK_INSTALL_PREHOOK);
		if (res != JVX_NO_ERROR)
		{
			if (res == JVX_ERROR_ABORT)
			{
				// This is the case if the callback has shortened the procedure
				// A shortcut procedure may be due to an embedded device in a device:
				// If the embedded device has run the get_property function successfully,
				// we do not need to run through all checks again.
				// This reduces the processing time
				res = JVX_NO_ERROR;
			}
			goto leave_function_unlock;
		}
		
		if (
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_WRITE_ONLY) ||
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE) ||
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT) ||
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_ONLY_ON_START))
		{
			propAccessible = true;
		}

		if (!propAccessible)
		{
			callGate.access_protocol = JVX_ACCESS_PROTOCOL_NO_ACCESS__WRITE;
			res = JVX_ERROR_NO_ACCESS;
			goto leave_function_unlock;
		}

		// Now, check the buffer content		
		if (selection->rawpointer)
		{
			res = JVX_ERROR_INVALID_FORMAT;
			// We do not need this check I guess!
			// if (
				// (selection->rawpointer->atype() == jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_LOCAL) &&
				// There is no difference between DLL and LOCAL for this type
				//(ptrRaw->atype() == jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_DLL))
			//{
				if (
					(selection->rawpointer->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTERNAL_BUFFER_POINTER) &&
					(ptrRaw->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTERNAL_BUFFER))
				{
					res = _install_match_external_buffer(selection, *ptrRaw);
				}

				if (
					(selection->rawpointer->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTENSION_INTERFACE_POINTER) &&
					(ptrRaw->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTENSION_INTERFACE))
				{
					res = _install_match_external<IjvxPropertyExtender>(selection, *ptrRaw);
				}

				if (
					(selection->rawpointer->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_TEXT_LOG_POINTER) &&
					(ptrRaw->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_TEXT_LOG))
				{
					res = _install_match_external<IjvxLocalTextLog>(selection, *ptrRaw);
				}

				if (
					(selection->rawpointer->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_CALLBACK_POINTER) &&
					(ptrRaw->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_CALLBACK))
				{
					res = _install_match_callback(selection, *ptrRaw);
				}
			// }
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}		

		assert(res == JVX_NO_ERROR); // Add this quick check to find all API problems - should be removed later

		if (res == JVX_NO_ERROR)
		{
			res = _run_hook(selection->callbacks.callback_set_posthook,
				selection->propDescriptor->name.std_str(),
				callGate, ptrRaw, identHook, tune, &selection->extended_props,
				JVX_PROPERTY_CALLBACK_INSTALL_POSTHOOK);
		}
	} // if (selection != _common_set_properties.registeredProperties.end())
	else
	{
		res = _forward_submodules(callGate, addresses, [&](const jvx::propertyAddress::IjvxPropertyAddress& addr, CjvxPropertySubModule* theSubModule)
		{
			 jvxErrorType resL = theSubModule->propRef->install_property_reference(
				callGate,
				rawData, 
				addr);
			return resL;
		});

	} // else :: if (selection != _common_set_properties.registeredProperties.end())

leave_function_unlock:

	_unlock_properties_local();

leave_error:

	if (res != JVX_NO_ERROR)
	{
		callGate.access_protocol = JVX_ACCESS_PROTOCOL_ERROR;
	}
	
	return res;
}

jvxErrorType
CjvxProperties::_uninstall_property_reference(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawData,
	const jvx::propertyAddress::IjvxPropertyAddress &ident)
{
	std::list<oneProperty>::iterator selection;
	jvxState stat = JVX_STATE_NONE;
	jvxBool propAccessible = false;

	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxSize offsetStart = 0;
	jvxSize numElements = 0;

	const jvx::propertyRawPointerType::IjvxRawPointerType* ptrRaw = &rawData;

	// Find property identification
	propAddressing addresses;
	addresses.cast(&ident);

	jvxErrorType res = _check_access_property_obj(callGate);
	if (res != JVX_NO_ERROR)
	{
		goto leave_error;
	}

	_lock_properties_local();

	//selection = findInPropertyList(_common_set_properties.registeredProperties, propId, category);
	selection = findSelectionFromAddress(addresses);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		res = _check_access_property(callGate, *selection);
		if (res != JVX_NO_ERROR)
		{
			res = JVX_ERROR_NO_ACCESS;
			goto leave_function_unlock;
		}

		theObjRef._state(&stat);
		if ((stat & selection->propDescriptor->allowedStateMask) == 0)
		{
			res = JVX_ERROR_WRONG_STATE;
			goto leave_function_unlock;
		}

		jvx::propertyDetail::CjvxTranferDetail tune;
		const jvx::propertyAddress::CjvxPropertyAddressGlobalId identHook(
			selection->propDescriptor->globalIdx,
			selection->propDescriptor->category);

		res = _run_hook(selection->callbacks.callback_set_prehook,
			selection->propDescriptor->name.std_str(),
			callGate, ptrRaw, identHook, tune, &selection->extended_props,
			JVX_PROPERTY_CALLBACK_UNINSTALL_PREHOOK);
		if (res != JVX_NO_ERROR)
		{
			if (res == JVX_ERROR_ABORT)
			{
				// This is the case if the callback has shortened the procedure
				// A shortcut procedure may be due to an embedded device in a device:
				// If the embedded device has run the get_property function successfully,
				// we do not need to run through all checks again.
				// This reduces the processing time
				res = JVX_NO_ERROR;
			}
			goto leave_function_unlock;
		}

		if (
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_WRITE_ONLY) ||
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE) ||
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT) ||
			(selection->propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_ONLY_ON_START))
		{
			propAccessible = true;
		}

		if (!propAccessible)
		{
			callGate.access_protocol = JVX_ACCESS_PROTOCOL_NO_ACCESS__WRITE;
			res = JVX_ERROR_NO_ACCESS;
			goto leave_function_unlock;
		}

		res = JVX_ERROR_INVALID_ARGUMENT;
		// "Unlink" the circ buffer field
		if (selection->rawpointer)
		{
			// I think we do not need this check
			// if ((selection->rawpointer->atype() == jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_LOCAL) &&
			//	(ptrRaw->atype() == jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_DLL))
			//{
				if (
					(selection->rawpointer->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTERNAL_BUFFER_POINTER) &&
					(ptrRaw->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTERNAL_BUFFER))
				{
					res = _uninstall_match_external_buffer(selection, *ptrRaw);
				}

				else if (
					(selection->rawpointer->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTENSION_INTERFACE_POINTER) &&
					(ptrRaw->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTENSION_INTERFACE))
				{
					res = _uninstall_match_external<IjvxPropertyExtender>(selection, *ptrRaw);
				}

				else if (
					(selection->rawpointer->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_TEXT_LOG_POINTER) &&
					(ptrRaw->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_TEXT_LOG))
				{
					res = _uninstall_match_external<IjvxPropertyExtender>(selection, *ptrRaw);
				}
				else if (
					(selection->rawpointer->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_CALLBACK_POINTER) &&
					(ptrRaw->etype() == jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_CALLBACK))
				{
					res = _uninstall_match_callback(selection, *ptrRaw);
				}
			//}
		}

		if (res == JVX_NO_ERROR)
		{
			// Run the posthook
			res = _run_hook(selection->callbacks.callback_set_posthook,
				selection->propDescriptor->name.std_str(),
				callGate, ptrRaw, identHook, tune, &selection->extended_props,
				JVX_PROPERTY_CALLBACK_UNINSTALL_POSTHOOK);
		}
	}
	else
	{
		res = _forward_submodules(callGate, addresses, [&](const jvx::propertyAddress::IjvxPropertyAddress& addr, CjvxPropertySubModule* theSubModule)
		{
			jvxErrorType resL = theSubModule->propRef->uninstall_property_reference(
				callGate,
				rawData, 
				addr);
			return resL;
		});

	} // else :: if (selection != _common_set_properties.registeredProperties.end())

leave_function_unlock:
	_unlock_properties_local();

leave_error:

	if (res != JVX_NO_ERROR)
	{
		callGate.access_protocol = JVX_ACCESS_PROTOCOL_ERROR;
	}

	return res;
}

jvxErrorType
CjvxProperties::_get_property_extended_info(jvxCallManagerProperties& callGate, 
	jvxHandle * fld,
	jvxSize requestId,
	const jvx::propertyAddress::IjvxPropertyAddress& ident)
{
	std::list<oneProperty>::iterator selection;

	propAddressing addresses;
	addresses.cast(&ident);

	jvxErrorType res = _check_access_property_obj(callGate);
	if (res != JVX_NO_ERROR)
	{
		goto leave_error;
	}

	_lock_properties_local();

	selection = findSelectionFromAddress(addresses);

	if (selection != _common_set_properties.registeredProperties.end())
	{
		res = _check_access_property(callGate, *selection);
		if (res != JVX_NO_ERROR)
		{
			res = JVX_NO_ERROR;
			goto leave_error_unlock;
		}
		switch (requestId)
		{
		case JVX_PROPERTY_INFO_SHOWHINT:
			if (selection->extended_props.showhint.valid)
			{
				((jvxApiString*)fld)->assign(selection->extended_props.showhint.content);
			}
			else
			{
				res = JVX_ERROR_NOT_READY;
			}
			break;
		case JVX_PROPERTY_INFO_MOREINFO:
			if (selection->extended_props.moreinfo.valid)
			{
				((jvxApiString*)fld)->assign(selection->extended_props.moreinfo.content);
			}
			else
			{
				res = JVX_ERROR_NOT_READY;
			}
			break;
		case JVX_PROPERTY_INFO_ORIGIN:
			if (selection->extended_props.origin.valid)
			{
				((jvxApiString*)fld)->assign(selection->extended_props.origin.content);
			}
			else
			{
				res = JVX_ERROR_NOT_READY;
			}
			break;
		case JVX_PROPERTY_INFO_GROUPID:
			if (selection->extended_props.groupid.valid)
			{
				*((jvxSize*)fld) = selection->extended_props.groupid.content;
			}
			else
			{
				res = JVX_ERROR_NOT_READY;
			}
			break;
		default:
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = _forward_submodules(callGate, addresses, [&](const jvx::propertyAddress::IjvxPropertyAddress& addr, CjvxPropertySubModule* theSubModule)
			{
				jvxErrorType resL = JVX_NO_ERROR;
				resL = theSubModule->propRef->get_property_extended_info(
					callGate, fld, requestId, ident);
				return resL;
			});
	}

leave_error_unlock:
	_unlock_properties_local();

leave_error:

	if (res != JVX_NO_ERROR)
	{
		callGate.access_protocol = JVX_ACCESS_PROTOCOL_ERROR;
	}
	return res;
}

jvxErrorType
CjvxProperties::_get_meta_flags(jvxCallManagerProperties& callGate,	
	jvxAccessRightFlags_rwcd * access_flags,
	jvxConfigModeFlags * mode_flags,
	const jvx::propertyAddress::IjvxPropertyAddress& ident)
{
	std::list<oneProperty>::iterator selection;
	jvxState stat = JVX_STATE_NONE;
	jvxSize maxSpace = 0;
	jvxSize szElem = 0;

	propAddressing addresses;
	addresses.cast(&ident);

	callGate.access_protocol = JVX_ACCESS_PROTOCOL_OK;

	jvxErrorType res = _check_access_property_obj(callGate);
	if (res != JVX_NO_ERROR)
	{
		goto leave_error;
	}

	_lock_properties_local();

	selection = findSelectionFromAddress(addresses);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		res = _check_access_property(callGate, *selection);
		if (res != JVX_NO_ERROR)
		{
			res = JVX_NO_ERROR;
			goto leave_error_unlock;
		}

		if (access_flags)
		{
			*access_flags = selection->propDescriptor->accessFlags;
			if (selection->references.accessFlagsPtr)
			{
				*access_flags = *selection->references.accessFlagsPtr;
			}
		}
		if (mode_flags)
		{
			*mode_flags = selection->propDescriptor->configModeFlags;
			if (selection->references.configModeFlagsPtr)
			{
				*mode_flags = *selection->references.configModeFlagsPtr;
			}
		}
	}
	else
	{
		res = _forward_submodules(callGate, addresses, [&](const jvx::propertyAddress::IjvxPropertyAddress& addr, CjvxPropertySubModule* theSubModule)
			{
				jvxErrorType resL = JVX_NO_ERROR;
				resL = theSubModule->propRef->get_meta_flags(
					callGate,
					access_flags,
					mode_flags,
					ident);
				return resL;
			});
	}

leave_error_unlock:

	_unlock_properties_local();
leave_error:

	if (res != JVX_NO_ERROR)
	{
		callGate.access_protocol = JVX_ACCESS_PROTOCOL_ERROR;
	}
	return(res);
}

jvxErrorType
CjvxProperties::_set_meta_flags(jvxCallManagerProperties& callGate,	
	jvxAccessRightFlags_rwcd * access_flags,
	jvxConfigModeFlags * mode_flags,
	const jvx::propertyAddress::IjvxPropertyAddress& ident)
{
	std::list<oneProperty>::iterator selection;
	jvxState stat = JVX_STATE_NONE;
	jvxSize maxSpace = 0;
	jvxSize szElem = 0;

	propAddressing addresses;
	addresses.cast(&ident);

	callGate.access_protocol = JVX_ACCESS_PROTOCOL_OK;

	jvxErrorType res = _check_access_property_obj(callGate);
	if (res != JVX_NO_ERROR)
	{
		goto leave_error;
	}

	_lock_properties_local();

	selection = findSelectionFromAddress(addresses);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		res = _check_access_property(callGate, *selection);
		if (res != JVX_NO_ERROR)
		{
			res = JVX_NO_ERROR;
			goto leave_error_unlock;
		}

		if (access_flags)
		{
			if (selection->references.accessFlagsPtr)
			{
				*selection->references.accessFlagsPtr = *access_flags;
			}
			else
			{
				selection->propDescriptor->accessFlags = *access_flags;
			}
		}
		if (mode_flags)
		{
			if (selection->references.configModeFlagsPtr)
			{
				*selection->references.configModeFlagsPtr = *mode_flags;
			}
			else
			{
				selection->propDescriptor->configModeFlags = *mode_flags;
			}
		}
	}
	else
	{
		res = _forward_submodules(callGate, addresses, [&](const jvx::propertyAddress::IjvxPropertyAddress& addr, CjvxPropertySubModule* theSubModule)
			{
				jvxErrorType resL = JVX_NO_ERROR;
				resL = theSubModule->propRef->set_meta_flags(
					callGate,
					access_flags,
					mode_flags,
					ident);
				return resL;
			});
	}

leave_error_unlock:

	_unlock_properties_local();
leave_error:

	if (res != JVX_NO_ERROR)
	{
		callGate.access_protocol = JVX_ACCESS_PROTOCOL_ERROR;
	}
	return(res);
}

/*

jvxErrorType
	CjvxProperties:: _get_flagtag_property(
		jvxCallManager* callGate,
		jvxSize propId,
		jvxPropertyCategoryType category, jvxFlagTag* value_return,
		IjvxObject* theObj)
{
	std::vector<oneProperty>::iterator selection;
	jvxState stat = JVX_STATE_NONE;
	jvxSize maxSpace = 0;
	jvxSize szElem = 0;

	jvxErrorType res = _check_access_property_obj(theObj, accessFlags, accProt);
	if (res != JVX_NO_ERROR)
		return res;

	_lock_properties_local();

	selection = findInPropertyList(_common_set_properties.registeredProperties, propId, category);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		res = _check_access_property(*selection, accessFlags, accProt);
		if (res != JVX_NO_ERROR)
		{
			res = JVX_NO_ERROR;
			goto leave_error;
		}

		if (value_return)
		{
			*value_return = *selection->myTagFlag;
		}
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
leave_error:

	if (res != JVX_NO_ERROR)
	{
		if (accProt)
			*accProt = JVX_ACCESS_PROTOCOL_ERROR;
	}
	_unlock_properties_local();
	return(res);
}

jvxErrorType
	CjvxProperties:: _set_flagtag_property(jvxSize propId,
	jvxPropertyCategoryType category, jvxFlagTag* value_return,
		jvxFlagTag accessFlags,
		jvxAccessProtocol* accProt,
		IjvxObject* theObj)
{
	std::vector<oneProperty>::iterator selection;
	jvxState stat = JVX_STATE_NONE;
	jvxSize maxSpace = 0;
	jvxSize szElem = 0;

	jvxErrorType res = _check_access_property_obj(theObj, accessFlags, accProt);
	if (res != JVX_NO_ERROR)
		return res;

	_lock_properties_local();

	selection = findInPropertyList(_common_set_properties.registeredProperties, propId, category);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		res = _check_access_property(*selection, accessFlags, accProt);
		if (res != JVX_NO_ERROR)
		{
			res = JVX_NO_ERROR;
			goto leave_error;
		}

		if (value_return)
		{
			*selection->myTagFlag = *value_return;
		}
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
leave_error:

	if (res != JVX_NO_ERROR)
	{
		if (accProt)
			*accProt = JVX_ACCESS_PROTOCOL_ERROR;
	}
	_unlock_properties_local();
	return(res);
}
*/

// ================================================================

jvxErrorType
CjvxProperties::_setThreadId_properties(JVX_THREAD_ID theThreadId)
{
	_common_set_properties.threadId = theThreadId;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxProperties::_register_callback(std::string tag, property_callback theCallback, jvxHandle * priv)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneCallback>::iterator selection;
	selection = findInCallbackList(_common_set_properties.registeredCallbacks, tag);
	if (selection == _common_set_properties.registeredCallbacks.end())
	{
		oneCallback oneElm;
		oneElm.tag = tag;
		oneElm.priv = priv;
		oneElm.theCallback = theCallback;
		_common_set_properties.registeredCallbacks.push_back(oneElm);
	}
	else
	{
		res = JVX_ERROR_DUPLICATE_ENTRY;
	}
	return res;
}

jvxErrorType
CjvxProperties::_unregister_callback(std::string tag)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneCallback>::iterator selection;
	selection = findInCallbackList(_common_set_properties.registeredCallbacks, tag);
	if (selection != _common_set_properties.registeredCallbacks.end())
	{
		_common_set_properties.registeredCallbacks.erase(selection);
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return res;
}

jvxErrorType
CjvxProperties::_register_property(
	jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus * propDescr,
	const pointerReferences & references,
	const callbackReferences & callbacks,
	jvx::propertyRawPointerType::IjvxRawPointerType * ptrRaw
	/*
	jvxHandle* fld,
jvxSize numberElements,
jvxDataFormat format,
jvxSize* idProp,
jvxPropertyCategoryType cat,
jvxUInt64 allowStateMask,
jvxUInt64 allowThreadingMask,
const char* name,
const char* description,
const char* descriptor,
jvxBool* isValidPtr,
jvxBool* isFrozen,
jvxPropertyAccessType accessType,
jvxPropertyDecoderHintType decTp,
jvxPropertyContext contxt,
jvxAccessRightFlags_rwcd* ref_access_flags,
jvxConfigModeFlags* ref_config_mode_flags,
std::string callback_set_posthook ,
std::string callback_get_prehook,
std::string callback_set_prehook ,
std::string callback_get_posthook
*/
)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<oneProperty>::iterator selection;
	jvxSize propProposed = _common_set_properties.nextPropId;	

	if (!propDescr)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	if (JVX_CHECK_SIZE_SELECTED(propDescr->globalIdx))
	{
		propProposed = propDescr->globalIdx;
		if (propDescr->category == JVX_PROPERTY_CATEGORY_UNSPECIFIC)
		{
			_common_set_properties.nextPropId = JVX_MAX(_common_set_properties.nextPropId, propProposed) + 1;
		}
	}
	else
	{
		if (propDescr->category == JVX_PROPERTY_CATEGORY_UNSPECIFIC)
		{
			_common_set_properties.nextPropId++;
		}
		else
		{
			assert(0);
		}
	}

	selection = findInPropertyList(_common_set_properties.registeredProperties, propProposed, propDescr->category);
	if (selection == _common_set_properties.registeredProperties.end())
	{
		oneProperty elm;
		elm.propDescriptor = propDescr;
		elm.propDescriptor->globalIdx = propProposed;
		elm.rawpointer = ptrRaw;
		switch (elm.propDescriptor->format)
		{
		case JVX_DATAFORMAT_CALLBACK:
			elm.references.fld_prop_cb = references.fld_prop_cb;
			break;
			break;
		default:

			switch (elm.propDescriptor->decTp)
			{
			case JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER:
			case JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER:
				elm.references.fld_ext = references.fld_ext;
				break;

			default:
				elm.references.fld = references.fld;
				break;
			}
		}

		elm.propDescriptor->isValid = true;
		elm.references.isValidPtr = references.isValidPtr;
		if (elm.references.isValidPtr)
		{
			elm.propDescriptor->isValid = *elm.references.isValidPtr;
		}

		elm.references.accessFlagsPtr = references.accessFlagsPtr;
		if (elm.references.accessFlagsPtr)
		{
			elm.propDescriptor->accessFlags = *elm.references.accessFlagsPtr;
		}

		elm.references.configModeFlagsPtr = references.configModeFlagsPtr;
		if (elm.references.configModeFlagsPtr)
		{
			elm.propDescriptor->configModeFlags = *elm.references.configModeFlagsPtr;
		}

		elm.callbacks = callbacks;

		elm.extended_props.showhint.valid = false;
		elm.extended_props.moreinfo.valid = false;
		elm.extended_props.origin.valid = false;
		elm.extended_props.groupid.valid = false;
		elm.extended_props.groupid.content = 0; // neutral property group

		_common_set_properties.registeredProperties.push_back(elm);
		_common_set_properties.propSetRevision++;
	}
	else
	{
		res = JVX_ERROR_DUPLICATE_ENTRY;
	}
	return res;
}

jvxErrorType
CjvxProperties::_set_property_extended_info(jvxHandle * fld,
	jvxSize requestId,
	jvxSize propId,
	jvxPropertyCategoryType category)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<oneProperty>::iterator selection;
	_lock_properties_local();

	selection = findInPropertyList(_common_set_properties.registeredProperties, propId, category);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		switch (requestId)
		{
		case JVX_PROPERTY_INFO_SHOWHINT:
			if (fld)
			{
				if (!selection->extended_props.showhint.valid)
				{
					selection->extended_props.showhint.content = ((jvxApiString*)fld)->std_str();
					selection->extended_props.showhint.valid = true;
				}
				else
				{
					res = JVX_ERROR_ALREADY_IN_USE;
				}
			}
			else
			{
				selection->extended_props.showhint.content = "";
				selection->extended_props.showhint.valid = false;
			}
			break;
		case JVX_PROPERTY_INFO_MOREINFO:
			if (fld)
			{
				if (!selection->extended_props.moreinfo.valid)
				{
					selection->extended_props.moreinfo.content = ((jvxApiString*)fld)->std_str();
					selection->extended_props.moreinfo.valid = true;
				}
				else
				{
					res = JVX_ERROR_ALREADY_IN_USE;
				}
			}
			else
			{
				selection->extended_props.moreinfo.content = "";
				selection->extended_props.moreinfo.valid = false;
			}
			break;
		case JVX_PROPERTY_INFO_ORIGIN:
			if (fld)
			{
				if (!selection->extended_props.origin.valid)
				{
					selection->extended_props.origin.content = ((jvxApiString*)fld)->std_str();
					selection->extended_props.origin.valid = true;
				}
				else
				{
					res = JVX_ERROR_ALREADY_IN_USE;
				}
			}
			else
			{
				selection->extended_props.origin.content = "";
				selection->extended_props.origin.valid = false;
			}
			break;
		case JVX_PROPERTY_INFO_GROUPID:
			if (fld)
			{
				if (!selection->extended_props.groupid.valid)
				{
					selection->extended_props.groupid.content = *((jvxSize*)fld);
					selection->extended_props.groupid.valid = true;
				}
				else
				{
					res = JVX_ERROR_ALREADY_IN_USE;
				}
			}
			else
			{
				selection->extended_props.groupid.content = 0; // 0 means no specific group (for AND combines)
				selection->extended_props.groupid.valid = false;
			}
			break;
		case JVX_PROPERTY_INFO_UPDATE_CONTENT_TOKEN:
			if (fld)
			{
				if (!selection->extended_props.update_context.valid)
				{
					selection->extended_props.update_context.content = ((jvxApiString*)fld)->std_str();
					selection->extended_props.update_context.valid = true;
				}
				else
				{
					res = JVX_ERROR_ALREADY_IN_USE;
				}
			}
			else
			{
				selection->extended_props.update_context.content = "";
				selection->extended_props.update_context.valid = false;
			}
			break;

		default:
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	_unlock_properties_local();
	return res;
}

jvxErrorType
CjvxProperties::_update_property_size(jvxSize numberElements, jvxPropertyCategoryType cat, jvxSize idProp, jvxBool lockOnChange)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<oneProperty>::iterator selection;

	if (lockOnChange)
	{
		_lock_properties_local();
	}
	selection = findInPropertyList(_common_set_properties.registeredProperties, idProp, cat);
	if (selection != _common_set_properties.registeredProperties.end())
	{		
		selection->propDescriptor->num = numberElements;
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	if (lockOnChange)
	{
		_unlock_properties_local();
	}
	return res;
}

jvxErrorType
CjvxProperties::_update_property(jvxHandle * fld, jvxSize numberElements, jvxPropertyCategoryType cat, jvxSize idProp, jvxBool lockOnChange)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<oneProperty>::iterator selection;

	if (lockOnChange)
	{
		_lock_properties_local();
	}
	selection = findInPropertyList(_common_set_properties.registeredProperties, idProp, cat);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		selection->references.fld = fld;
		selection->propDescriptor->num = numberElements;

	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	if (lockOnChange)
	{
		_unlock_properties_local();
	}
	return res;
}

jvxErrorType
CjvxProperties::_update_property_access_type(jvxPropertyAccessType accessType, const jvxPropertyContainer& pc)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<oneProperty>::iterator selection;

	_lock_properties_local();
	selection = findInPropertyList(_common_set_properties.registeredProperties, pc.globalIdx, pc.category);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		if (accessType != selection->propDescriptor->accessType)
		{
			if (selection->shadow.accessType == JVX_PROPERTY_ACCESS_NONE)
			{
				selection->shadow.accessType = selection->propDescriptor->accessType;
				selection->propDescriptor->accessType = accessType;
			}
			else
			{
				res = JVX_ERROR_ALREADY_IN_USE;
			}
		}
		else
		{
			res = JVX_ERROR_DUPLICATE_ENTRY;
		}
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	_unlock_properties_local();
	return res;
}

jvxErrorType
CjvxProperties::_update_property_access_type_all(jvxPropertyAccessType accessType)
{
	jvxErrorType res = JVX_NO_ERROR;

	_lock_properties_local();

	for (auto& elm : _common_set_properties.registeredProperties)
	{
		if (elm.shadow.accessType == JVX_PROPERTY_ACCESS_NONE)
		{
			elm.shadow.accessType = elm.propDescriptor->accessType;
			elm.propDescriptor->accessType = accessType;
		}
	}

	_unlock_properties_local();

	return res;
}

jvxErrorType
CjvxProperties::_undo_update_property_access_type( const jvxPropertyContainer& pc)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<oneProperty>::iterator selection;

	_lock_properties_local();

	selection = findInPropertyList(_common_set_properties.registeredProperties, pc.globalIdx, pc.category);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		if (selection->shadow.accessType != JVX_PROPERTY_ACCESS_NONE)
		{
			selection->propDescriptor->accessType = selection->shadow.accessType;
			selection->shadow.accessType = JVX_PROPERTY_ACCESS_NONE;
		}
		else
		{
			res = JVX_ERROR_DUPLICATE_ENTRY;
		}
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	_unlock_properties_local();
	return res;
}

jvxErrorType
CjvxProperties::_undo_update_property_access_type_all()
{
	jvxErrorType res = JVX_NO_ERROR;

	_lock_properties_local();

	for (auto& elm : _common_set_properties.registeredProperties)
	{
		if (elm.shadow.accessType != JVX_PROPERTY_ACCESS_NONE)
		{
			elm.propDescriptor->accessType = elm.shadow.accessType;
		}
	}

	_unlock_properties_local();

	return res;
}

jvxErrorType
CjvxProperties::_update_property_decoder_type(jvxPropertyDecoderHintType decTp, const jvxPropertyContainer& pc)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<oneProperty>::iterator selection;

	_lock_properties_local();
	selection = findInPropertyList(_common_set_properties.registeredProperties, pc.globalIdx, pc.category);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		selection->propDescriptor->decTp = decTp;
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	_unlock_properties_local();
	return res;
}

jvxErrorType
CjvxProperties::_unregister_property(jvxPropertyCategoryType cat, jvxSize idProp)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<oneProperty>::iterator selection;
	_lock_properties_local();
	selection = findInPropertyList(_common_set_properties.registeredProperties, idProp, cat);
	if (selection != _common_set_properties.registeredProperties.end())
	{
		_common_set_properties.registeredProperties.erase(selection);
		_common_set_properties.propSetRevision++;
	}
	else
	{
		res = JVX_ERROR_DUPLICATE_ENTRY;
	}
	_unlock_properties_local();
	return res;
}

jvxErrorType
CjvxProperties::_request_takeover_property(
	jvxCallManagerProperties& callGate,
	const jvxComponentIdentification & thisismytype, IjvxObject * thisisme,
	jvxHandle * fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
	jvxPropertyCategoryType category, jvxSize propId)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxProperties::_translate_group_id(
	jvxCallManagerProperties& callGate, jvxSize gId, jvxApiString& astr)
{
	astr.clear();
	auto elm = this->tableGroupIdTranslations.find(gId);
	if (elm != this->tableGroupIdTranslations.end())
	{
		astr.assign(elm->second);
	}
	else
	{
		astr.assign(jvx_size2String(gId));
	}
	return JVX_NO_ERROR;
}

/*
 jvxErrorType CjvxProperties::_freeze_properties(jvxSize* idProps, jvxPropertyCategoryType* category, jvxSize num,
	 jvxFlagTag flagtag,
	 jvxAccessProtocol* accProt ,
	 IjvxObject* theObj)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneProperty>::iterator selection;

	_lock_properties_local();

	for(i = 0; i < num; i++)
	{
		selection = findInPropertyList(_common_set_properties.registeredProperties, idProps[i], category[i]);
		if(selection != _common_set_properties.registeredProperties.end())
		{
			*selection->isFrozen = true;
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}

	_unlock_properties_local();

	if(res != JVX_NO_ERROR)
	{
		_unfreeze_properties(idProps, category, num, flagtag, accProt, theObj);
	}
	return res;
}


 jvxErrorType
	 CjvxProperties::_unfreeze_properties(jvxSize* idProps, jvxPropertyCategoryType* category, jvxSize num,
		 jvxFlagTag flagtag,
		 jvxAccessProtocol* accProt,
		 IjvxObject* theObj)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneProperty>::iterator selection;

	_lock_properties_local();

	for(i = 0; i < num; i++)
	{
		selection = findInPropertyList(_common_set_properties.registeredProperties, idProps[i], category[i]);
		if(selection != _common_set_properties.registeredProperties.end())
		{
			*selection->isFrozen = false;
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}

	_unlock_properties_local();
	assert(res == JVX_NO_ERROR);
	return res;
}
*/
jvxErrorType
CjvxProperties::_update_properties_on_start()
{
	jvxErrorType res = JVX_NO_ERROR;

	_lock_properties_local();

	if (_common_set_properties.wasStarted == false)
	{
		for (auto& elm : _common_set_properties.registeredProperties)
		{
			if (elm.propDescriptor->accessType == JVX_PROPERTY_ACCESS_READ_ONLY_ON_START)
			{
				elm.shadow.accessType = elm.propDescriptor->accessType;
				elm.propDescriptor->accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
				elm.shadow.autoset = true;
			}
		}
		_common_set_properties.wasStarted = true;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	_unlock_properties_local();

	return res;
}

jvxErrorType
CjvxProperties::_update_properties_on_stop()
{
	jvxErrorType res = JVX_NO_ERROR;

	_lock_properties_local();

	if (_common_set_properties.wasStarted == true)
	{
		for (auto& elm : _common_set_properties.registeredProperties)
		{
			if (
				(elm.shadow.accessType == JVX_PROPERTY_ACCESS_READ_ONLY_ON_START) &&
				(elm.shadow.autoset))
			{
				elm.propDescriptor->accessType = elm.shadow.accessType;
				elm.shadow.accessType = JVX_PROPERTY_ACCESS_NONE;
				elm.shadow.autoset = false;
			}
		}
		_common_set_properties.wasStarted = false;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	_unlock_properties_local();

	return res;
}

jvxErrorType
CjvxProperties::_register_sub_module(IjvxProperties* theProps, jvxSize offsetShft, std::string propDescriptorTag, std::string propDescriptionPrefix)
{
	jvxErrorType res = JVX_ERROR_DUPLICATE_ENTRY;
	jvxCallManagerProperties callGate;

	_lock_properties_local();

	std::list<CjvxPropertySubModule*>::iterator elm = _common_set_properties.registeredSubmodules.begin();
	while (elm != _common_set_properties.registeredSubmodules.end())
	{
		if ((*elm)->propDescriptorTag == propDescriptorTag)
		{
			break;
		}
		elm++;
	}
	
	if (elm == _common_set_properties.registeredSubmodules.end())
	{
		res = JVX_NO_ERROR;

		jvxSize numPrev = 0, i = 0;
		jPDM lDescr;
		jPAL propA;
		this->_number_properties(callGate, &numPrev);
		for (i = 0; i < numPrev; i++)
		{
			propA.reset(i);
			this->_description_property(callGate, lDescr, propA);
			if (lDescr.globalIdx >= offsetShft)
			{
				res = JVX_ERROR_DUPLICATE_ENTRY;
				break;
			}
		}
	}

	if(res == JVX_NO_ERROR)
	{
		CjvxPropertySubModule* newElm = nullptr;

		JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm, CjvxPropertySubModule);
		newElm->propRef = theProps;
		newElm->parentReport = _common_set_property_report.reportRef;
		//newElm.objRef = theObj;
		if (JVX_CHECK_SIZE_SELECTED(offsetShft))
		{
			newElm->offsetShift = offsetShft;
		}
		else
		{
			newElm->offsetShift = _update_max_prop_id() + 1;
		}
		newElm->propDescriptorTag = propDescriptorTag;
		newElm->propDescriptionPrefix = propDescriptionPrefix;

		newElm->propRef->install_ref_property_report_collect(callGate, newElm, newElm->propDescriptionPrefix.c_str());

		_common_set_properties.registeredSubmodules.push_back(newElm);

		// Put the list in the order of increasing offset!
		_common_set_properties.registeredSubmodules.sort();
	}
	
	_unlock_properties_local();
	return res;
}

jvxSize CjvxProperties::_update_max_prop_id()
{
	jvxSize i;
	jvxSize maxPropId = 0;
	jvxCallManagerProperties callGate;
	_lock_properties_local();
	for (auto& elm : _common_set_properties.registeredProperties)
	{
		if (elm.propDescriptor->globalIdx > maxPropId)
		{
			maxPropId = elm.propDescriptor->globalIdx;
		}
	}

	if (_common_set_properties.registeredSubmodules.size() > 0)
	{
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);
		jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;

		auto elmLast = _common_set_properties.registeredSubmodules.end();
		jvxSize offs = (*elmLast)->offsetShift;
		jvxSize numP = 0;
		(*elmLast)->propRef->number_properties(callGate, &numP);

		for (ident.idx = 0; ident.idx < numP; ident.idx++)
		{
			(*elmLast)->propRef->
				description_property(callGate,theDescr, ident);
			/*
			_common_set_properties.registeredSubmodules[_common_set_properties.registeredSubmodules.size() - 1].propRef->
				description_property(&callGate, i, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &idL, NULL, NULL, NULL, NULL, NULL, NULL);
			*/
			theDescr.globalIdx += offs;
			if (theDescr.globalIdx > maxPropId)
			{
				maxPropId = theDescr.globalIdx;
			}
		}
	}
	_unlock_properties_local();
	return maxPropId;
}

jvxErrorType
CjvxProperties::_unregister_sub_module(std::string propDescriptorTag)
{
	jvxErrorType res = JVX_NO_ERROR;
	_lock_properties_local();

	std::list<CjvxPropertySubModule*>::iterator elm = _common_set_properties.registeredSubmodules.begin();
	while (elm != _common_set_properties.registeredSubmodules.end())
	{
		if ((*elm)->propDescriptorTag == propDescriptorTag)
		{
			break;
		}
		elm++;
	}
	if (elm != _common_set_properties.registeredSubmodules.end())
	{
		JVX_DSP_SAFE_DELETE_OBJECT(*elm);

		_common_set_properties.registeredSubmodules.erase(elm);

	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	_unlock_properties_local();
	return res;
}

jvxErrorType
CjvxProperties::_unregister_sub_module(IjvxProperties* arg)
{
	jvxErrorType res = JVX_NO_ERROR;
	_lock_properties_local();

	std::list<CjvxPropertySubModule*>::iterator elm = _common_set_properties.registeredSubmodules.begin();
	while (elm != _common_set_properties.registeredSubmodules.end())
	{
		if ((*elm)->propRef == arg)
		{
			break;
		}
		elm++;
	}
	if (elm != _common_set_properties.registeredSubmodules.end())
	{
		JVX_DSP_SAFE_DELETE_OBJECT(*elm);
		_common_set_properties.registeredSubmodules.erase(elm);

	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	_unlock_properties_local();
	return res;
}

jvxErrorType
CjvxProperties::_reset_property_group(jvxCallManagerProperties& callGate)
{
	_common_set_properties.inPropertyGroupCnt = 0;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxProperties::_start_property_group(jvxCallManagerProperties& callGate)
{
	_common_set_properties.inPropertyGroupCnt++;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxProperties::_status_property_group(
	jvxCallManagerProperties& callGate,
	jvxSize * startCnt)
{
	if (startCnt)
	{
		*startCnt = _common_set_properties.inPropertyGroupCnt;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxProperties::_stop_property_group(jvxCallManagerProperties& callGate)
{
	if (_common_set_properties.inPropertyGroupCnt == 0)
	{
		return JVX_ERROR_WRONG_STATE;
	}
	_common_set_properties.inPropertyGroupCnt--;
	return JVX_NO_ERROR;
}

// ===============================================================================

jvxErrorType
CjvxProperties::_install_ref_property_report_collect(jvxCallManagerProperties& callGate,
	IjvxProperties_report* extReport, const char* prefix)
{
	if (_common_set_property_report.reportRef_stack == nullptr)
	{
		_common_set_property_report. reportRef_stack = _common_set_property_report.reportRef;
		_common_set_property_report.reportRef = extReport;
		_common_set_property_report.reportPrefix = prefix;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxProperties::_uninstall_ref_property_report_collect(
	jvxCallManagerProperties& callGate, IjvxProperties_report* extReport)
{
	if (_common_set_property_report.reportRef == extReport)
	{
		_common_set_property_report.reportRef = _common_set_property_report.reportRef_stack;
		_common_set_property_report.reportRef_stack = nullptr;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType 
CjvxProperties::_start_property_report_collect(jvxCallManagerProperties& callGate)
{
	// Need to put the report ist into a lock: this function is thread safe!
	_lock_properties_local();
	_common_set_property_report.startCntStack++;
	_unlock_properties_local();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxProperties::_stop_property_report_collect(jvxCallManagerProperties& callGate)
{
	jvxBool report = false;
	std::string allProps;

	// Need to put the report ist into a lock: this function is thread safe!
	_lock_properties_local();
	assert(_common_set_property_report.startCntStack > 0);
	_common_set_property_report.startCntStack--;
	if (_common_set_property_report.startCntStack == 0)
	{
		allProps = jvx::helper::properties::collectedPropsToString(_common_set_property_report.collectedProps);
		_common_set_property_report.collectedProps.clear();
		report = true;
	}
	_unlock_properties_local();

	if(report)
	{
		if (_common_set_property_report.reportRef)
		{
			_common_set_property_report.reportRef->report_properties_modified(allProps.c_str());
		}
		return JVX_NO_ERROR;
	}

	return JVX_ERROR_UNSUPPORTED;
}

std::string&
CjvxProperties::property_changed_descriptor_tag_add(std::string& descriptor)
{
	std::vector<std::string> lst;
	jvx::helper::parseStringListIntoTokens(descriptor, lst);
	descriptor.clear();
	for (auto& elm : lst)
	{
		if (!descriptor.empty())
		{
			descriptor += ",";
		}
		descriptor += elm + "+";
	}
	return descriptor;
}

jvxErrorType
CjvxProperties::add_property_report_collect(const std::string& propDescr, 
	jvxBool reportDescriptorChanged, CjvxProperties* cpReport, jvxComponentIdentification cpTp)
{
	jvxBool reportImmediate = true;
	std::string propPrefix = _common_set_property_report.reportPrefix;
	std::string cpPrefix;
	if (cpReport)
	{
		cpPrefix = jvxComponentIdentification_txt(cpTp);
		// propPrefix = cpReport->_common_set_properties.
		propPrefix += cpReport->_common_set_property_report.reportPrefix;
		
	}
	propPrefix = jvx_makePathExpr(propPrefix, propDescr);
	if (!cpPrefix.empty())
	{
		propPrefix = cpPrefix + ":" + propPrefix;
	}

	std::string propPrefixContent = propPrefix;
	if (reportDescriptorChanged)
	{
		propPrefix = CjvxProperties::property_changed_descriptor_tag_add(propPrefix);
	}
	_lock_properties_local();

	// If we want to report, we should modify the revision
	if (cpReport)
	{
		cpReport->_common_set_properties.propSetRevision++;
	}
	else
	{
		_common_set_properties.propSetRevision++;
	}

	if (_common_set_property_report.startCntStack > 0)
	{
		auto elm = std::find(_common_set_property_report.collectedProps.begin(), _common_set_property_report.collectedProps.end(), propPrefix);
		if (elm == _common_set_property_report.collectedProps.end())
		{
			// This property is not yet in the list. We will add it

			// Try to find a "content-only" version
			elm = std::find(_common_set_property_report.collectedProps.begin(), _common_set_property_report.collectedProps.end(), propPrefixContent);
			if (elm != _common_set_property_report.collectedProps.end())
			{
				// If we found a content only version, remove it
				_common_set_property_report.collectedProps.erase(elm);
			}

			// Now add the new element
			_common_set_property_report.collectedProps.push_back(propPrefix);
		}
		reportImmediate = false;
	}
	_unlock_properties_local();
	
	if(reportImmediate)
	{
		if (_common_set_property_report.reportRef)
		{
			_common_set_property_report.reportRef->report_properties_modified(propPrefix.c_str());
		}
	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxProperties::add_properties_report_collect(
	const std::list<std::string>& propDescr, 
	jvxBool reportDescriptorChanged, CjvxProperties* cpReport, 
	jvxComponentIdentification cpTp)
{
	jvxBool reportImmediate = true;
	std::string repToken;
	_lock_properties_local();

	// If we want to report, we should modify the revision
	_common_set_properties.propSetRevision++;

	// Check if we arrive here within a property set function run
	if (_common_set_property_report.startCntStack > 0)
	{
		// Yes, a run is on. Add the property to the list but do not report yet
		reportImmediate = false;
	}
		
	for (auto elmN : propDescr)
	{
		std::string propPrefix = _common_set_property_report.reportPrefix;
		std::string cpPrefix;
		if (cpReport)
		{
			cpPrefix = jvxComponentIdentification_txt(cpTp);
			// propPrefix = cpReport->_common_set_properties.
			propPrefix += cpReport->_common_set_property_report.reportPrefix;

		}
		propPrefix = jvx_makePathExpr(propPrefix, elmN);
		if (!cpPrefix.empty())
		{
			propPrefix = cpPrefix + ":" + propPrefix;
		}

		std::string propPrefixContentOnly = propPrefix;
		if (reportDescriptorChanged)
		{
			propPrefix = CjvxProperties::property_changed_descriptor_tag_add(propPrefix);
		}

		/*

		propPrefix = jvx_makePathExpr(_common_set_property_report.reportPrefix, elmN);
		std::string propPrefixContentOnly = propPrefix;
		if (reportDescriptorChanged)
		{
			propPrefix = CjvxProperties::property_changed_descriptor_tag_add(propPrefix);
		}

		*/

		// Report each property only once! Check if the entry is already in the list. If not, try to find the version without the content flag
		auto elm = std::find(_common_set_property_report.collectedProps.begin(), _common_set_property_report.collectedProps.end(), propPrefix);
		if (elm == _common_set_property_report.collectedProps.end())
		{
			// This property is not yet in the list. We will add it.
			
			// However, at first, try to find a "content-only" version
			elm = std::find(_common_set_property_report.collectedProps.begin(), _common_set_property_report.collectedProps.end(), propPrefixContentOnly);
			if (elm != _common_set_property_report.collectedProps.end())
			{
				// If we found a content only version, remove it
				_common_set_property_report.collectedProps.erase(elm);
			}
		
			// Now add the new element
			_common_set_property_report.collectedProps.push_back(propPrefix);
		}
	}

	if (reportImmediate)
	{
		repToken = jvx::helper::properties::collectedPropsToString(_common_set_property_report.collectedProps);
		_common_set_property_report.collectedProps.clear();		
	}
	_unlock_properties_local();

	if (reportImmediate)
	{
		if (_common_set_property_report.reportRef)
		{
			_common_set_property_report.reportRef->report_properties_modified(repToken.c_str());
		}
	}

	return JVX_NO_ERROR;
}
// ===============================================================================

jvxErrorType 
CjvxProperties::_translate_address_idx(
	jvxCallManagerProperties& callGate,
	const jvx::propertyAddress::IjvxPropertyAddress& ident, 
	jvxSize& globIdx, jvxPropertyCategoryType& cat)
{
	const jvx::propertyAddress::CjvxPropertyAddressGlobalId* ptr = nullptr;
	jvx::propertyDescriptor::CjvxPropertyDescriptorMin theDescr;
	jvxErrorType res = JVX_NO_ERROR;
		
	globIdx = JVX_SIZE_UNSELECTED;
	cat = JVX_PROPERTY_CATEGORY_UNKNOWN;

	ptr = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressGlobalId>(&ident);
	if (ptr)
	{
		globIdx = ptr->id;
		cat = ptr->cat;
		return JVX_NO_ERROR;
	}
	
	res = _description_property(callGate, theDescr, ident);
	if (res == JVX_NO_ERROR)
	{
		globIdx = theDescr.globalIdx;
		cat = theDescr.category;
	}
	return res;
}

jvxErrorType
CjvxProperties::_translate_address_descriptor(
	jvxCallManagerProperties& callGate,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	jvxApiString& descr)
{
	const jvx::propertyAddress::CjvxPropertyAddressDescriptor* ptr = nullptr;
	jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
	jvxErrorType res = JVX_NO_ERROR;

	descr.clear();

	ptr = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressDescriptor>(&ident);
	if (ptr)
	{
		descr.assign(ptr->descriptor);		
		return JVX_NO_ERROR;
	}

	res = _description_property(callGate, theDescr, ident);
	if (res == JVX_NO_ERROR)
	{
		descr.assign(theDescr.descriptor);
	}
	return res;
}

void
CjvxProperties::resetPropertyIdCount()
{
	_common_set_properties.nextPropId = JVX_PROPERTIES_OFFSET_UNSPECIFIC;// 1;
}

// =========================================================================================
std::list<CjvxProperties::oneProperty>::iterator CjvxProperties::findInPropertyList(std::list<CjvxProperties::oneProperty>&lst, jvxSize propId, jvxPropertyCategoryType cat)
{
	std::list<CjvxProperties::oneProperty>::iterator elm = lst.begin();
	while (elm != lst.end())
	{
		if (
			(elm->propDescriptor->category == cat) &&
			(elm->propDescriptor->globalIdx == propId)
			)
		{
			break;
		}
		elm++;
	}
	return elm;
}

// =========================================================================================
std::list<CjvxProperties::oneProperty>::iterator
CjvxProperties::findInPropertyList(std::list<CjvxProperties::oneProperty>&lst, const char* descriptor)
{
	std::list<CjvxProperties::oneProperty>::iterator elm = lst.begin();
	while (elm != lst.end())
	{
		if (
			(elm->propDescriptor->descriptor == descriptor))
		{
			break;
		}
		elm++;
	}
	return elm;
}

// =========================================================================================
std::vector<CjvxProperties::oneCallback>::iterator
CjvxProperties::findInCallbackList(std::vector<CjvxProperties::oneCallback>&lst, std::string tag)
{
	std::vector<CjvxProperties::oneCallback>::iterator elm = lst.begin();
	while (elm != lst.end())
	{
		if (
			(elm->tag == tag)
			)
		{
			break;
		}
		elm++;
	}
	return elm;
}

jvxErrorType
CjvxProperties::_install_match_external_buffer(
	std::list<oneProperty>::iterator & selection,
	const jvx::propertyRawPointerType::IjvxRawPointerType& install_this)
{
	jvxErrorType res = JVX_NO_ERROR;

	const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<jvxExternalBuffer>* ptrRawInstall =
		castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<jvxExternalBuffer> >(
		selection->rawpointer, selection->propDescriptor->format);
	const jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer>* ptrRawToInstall =
		castPropRawPointer<const  jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> >(
			&install_this, selection->propDescriptor->format);

	// Check that the decoder format is as registered
	jvxSize cnt = 0;
	jvxBool validType = false;
	jvx::externalBufferType tpLook = jvx::JVX_EXTERNAL_BUFFER_NONE;
	while (1)
	{
		if (entriesTranslateExtBuffers[cnt].tpUi == JVX_PROPERTY_DECODER_NONE)
		{
			break;
		}
		if (entriesTranslateExtBuffers[cnt].tpUi == selection->propDescriptor->decTp)
		{
			tpLook = entriesTranslateExtBuffers[cnt].tpHd;
			validType = true;
			break;
		}
		cnt++;
	}

	if (!validType)
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}

	if (ptrRawInstall && ptrRawToInstall)
	{
		jvxExternalBuffer** extBufPtrFromRawInstall = nullptr;
		jvxExternalBuffer* extBufPtrFromRawToInstall = nullptr;
		jvxBool* isValidPtr = nullptr;
		extBufPtrFromRawInstall = ptrRawInstall->typedPointer();
		extBufPtrFromRawToInstall = ptrRawToInstall->typedPointer();
		isValidPtr = ptrRawToInstall->isValidPtr();

		if (extBufPtrFromRawInstall && extBufPtrFromRawToInstall)
		{
			if (*extBufPtrFromRawInstall == NULL)
			{
				// Check that the format is as promised
				if (res == JVX_NO_ERROR)
				{
					if (extBufPtrFromRawToInstall->formFld != selection->propDescriptor->format)
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
				if (tpLook == extBufPtrFromRawToInstall->tp)
				{
					jvxSize szLoc = jvxDataFormat_getsize(extBufPtrFromRawToInstall->formFld) * jvxDataFormatGroup_getsize_mult(extBufPtrFromRawToInstall->subFormFld);
					szLoc /= jvxDataFormatGroup_getsize_div(extBufPtrFromRawToInstall->subFormFld);

					switch (tpLook)
					{
						// Check that size secification is consistent
					case jvx::JVX_EXTERNAL_BUFFER_1D_CIRCULAR_BUFFER:
						if (
							(extBufPtrFromRawToInstall->szElmFld != szLoc) ||
							(extBufPtrFromRawToInstall->numElmFld != (extBufPtrFromRawToInstall->length * extBufPtrFromRawToInstall->number_channels)) ||
							(extBufPtrFromRawToInstall->szFld != (extBufPtrFromRawToInstall->szElmFld * extBufPtrFromRawToInstall->numElmFld)))
						{
							res = JVX_ERROR_SIZE_MISMATCH;
						}
						break;
					case jvx::JVX_EXTERNAL_BUFFER_2D_FIELD_BUFFER:
						if (
							(extBufPtrFromRawToInstall->szElmFld != szLoc) ||
							(extBufPtrFromRawToInstall->numElmFld != (extBufPtrFromRawToInstall->length * extBufPtrFromRawToInstall->number_channels * extBufPtrFromRawToInstall->specific.the2DFieldBuffer_full.common.number_buffers)) ||
							(extBufPtrFromRawToInstall->szFld != (extBufPtrFromRawToInstall->szElmFld * extBufPtrFromRawToInstall->numElmFld)))
						{
							res = JVX_ERROR_SIZE_MISMATCH;
						}
						break;
					default:
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_SETTING;
				}

				// Now we can be sure that everything is at least consistent.
				if (res == JVX_NO_ERROR)
				{
					*extBufPtrFromRawInstall = extBufPtrFromRawToInstall;

					// Safe old values
					selection->shadow.accessType = selection->propDescriptor->accessType;
					selection->shadow.isValidPtr = selection->references.isValidPtr;

					selection->propDescriptor->accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;
					selection->propDescriptor->isValid = isValidPtr;
				}
			} // if (*extBufPtrFromRawInstall == NULL)
			else
			{
				res = JVX_ERROR_ALREADY_IN_USE;
			}
		} // if (extBufPtrFromRawInstall && extBufPtrFromRawToInstall)
		else
		{
			// Error: pointers were of incorrect type
			res = JVX_ERROR_INVALID_FORMAT;
		}
	}
	else
	{
		res = JVX_ERROR_ALREADY_IN_USE;
	}
	return res;
}

jvxErrorType
CjvxProperties::_uninstall_match_external_buffer(
	std::list<oneProperty>::iterator & selection,
	const jvx::propertyRawPointerType::IjvxRawPointerType& install_this)
{
	jvxErrorType res = JVX_NO_ERROR;

	const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<jvxExternalBuffer>* ptrRawInstall =
		castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<jvxExternalBuffer> >(
			selection->rawpointer, selection->propDescriptor->format);
	const jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer>* ptrRawToInstall =
		castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> >(
			&install_this, selection->propDescriptor->format);

	if (ptrRawInstall)
	{
		jvxExternalBuffer** extBufPtrFromRawInstall = nullptr;
		jvxExternalBuffer* extBufPtrFromRawToInstall = nullptr;
		jvxBool* isValidPtr = nullptr;
		extBufPtrFromRawInstall = ptrRawInstall->typedPointer();
		if (ptrRawToInstall)
		{
			extBufPtrFromRawToInstall = ptrRawToInstall->typedPointer();
			isValidPtr = ptrRawToInstall->isValidPtr();
		}

		if (extBufPtrFromRawInstall)
		{
			if (*extBufPtrFromRawInstall == nullptr)
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			else if (
				(extBufPtrFromRawToInstall == nullptr) /* old style uninstall */ ||
				(*extBufPtrFromRawInstall == extBufPtrFromRawToInstall))
			{
				*extBufPtrFromRawInstall = nullptr;

				selection->propDescriptor->accessType = selection->shadow.accessType;
				selection->references.isValidPtr = selection->shadow.isValidPtr;

				// Mark property as not valid
				jvxBool* isValid = &selection->propDescriptor->isValid;
				if (selection->references.isValidPtr)
				{
					isValid = selection->references.isValidPtr;
				}
				if (*isValid)
				{
					*isValid = false;
				}
			}
			else
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
		} // if (extBufPtrFromRawInstall)
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
	} // if (ptrRawInstall && ptrRawToInstall)
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return res;
}

jvxErrorType 
CjvxProperties::_install_match_callback(
	std::list<oneProperty>::iterator& selection,
	const jvx::propertyRawPointerType::IjvxRawPointerType& install_this)
{
	jvxErrorType res = JVX_NO_ERROR;

	const jvx::propertyRawPointerType::CjvxRawPointerTypeCallbackPointer* ptrRawInstall =
		castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeCallbackPointer >(
			selection->rawpointer, selection->propDescriptor->format);
	const jvx::propertyRawPointerType::CjvxRawPointerTypeCallback* ptrRawToInstall =
		castPropRawPointer<const  jvx::propertyRawPointerType::CjvxRawPointerTypeCallback >(
			&install_this, selection->propDescriptor->format);

	if (ptrRawInstall && ptrRawToInstall)
	{
		jvxCallbackPrivate** ptrRawStore = ptrRawInstall->typedPointer();
		jvxCallbackPrivate* ptrRawIn = ptrRawToInstall->typedPointer();
		if (*ptrRawStore == nullptr)
		{
			*ptrRawStore = ptrRawIn;
		}
		else
		{
			res = JVX_ERROR_ALREADY_IN_USE;
		}
	} // if (extBufPtrFromRawInstall && extBufPtrFromRawToInstall)
	else
	{
		// Error: pointers were of incorrect type
		res = JVX_ERROR_INVALID_FORMAT;
	}
	return res;
}

jvxErrorType 
CjvxProperties::_uninstall_match_callback(
	std::list<oneProperty>::iterator& selection,
	const jvx::propertyRawPointerType::IjvxRawPointerType& install_this)
{
	jvxErrorType res = JVX_NO_ERROR;

	const jvx::propertyRawPointerType::CjvxRawPointerTypeCallbackPointer* ptrRawInstall =
		castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeCallbackPointer >(
			selection->rawpointer, selection->propDescriptor->format);
	const jvx::propertyRawPointerType::CjvxRawPointerTypeCallback* ptrRawToInstall =
		castPropRawPointer<const  jvx::propertyRawPointerType::CjvxRawPointerTypeCallback >(
			&install_this, selection->propDescriptor->format);

	if (ptrRawInstall && ptrRawToInstall)
	{
		jvxCallbackPrivate** ptrRawStore = ptrRawInstall->typedPointer();
		jvxCallbackPrivate* ptrRawIn = ptrRawToInstall->typedPointer();
		if (*ptrRawStore == ptrRawIn)
		{
			*ptrRawStore = nullptr;
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	} // if (extBufPtrFromRawInstall && extBufPtrFromRawToInstall)
	else
	{
		// Error: pointers were of incorrect type
		res = JVX_ERROR_INVALID_FORMAT;
	}
	return res;
}

// ==================================================================================================

jvxErrorType
CjvxProperties::_number_properties_local(jvxCallManagerProperties& callGate, jvxSize* num)
{
	if (num)
	{
		*num = _common_set_properties.registeredProperties.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxProperties::_forward_submodules(jvxCallManagerProperties& callGate,
	propAddressing& addresses,
	std::function<jvxErrorType(const jvx::propertyAddress::IjvxPropertyAddress& addr, CjvxPropertySubModule* theSubModule)> func,
	std::function < void(CjvxPropertySubModule* theSubModule)> func_post_success)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxErrorType resL = JVX_NO_ERROR;	
	jvxSize cnt = 0;
	// _common_set_properties.registeredProperties.size();
	this->_number_properties_local(callGate, &cnt);

	for (auto elm : _common_set_properties.registeredSubmodules)
	{
		if (addresses.addrLin)
		{
			jvxSize numL = 0;
			jvx::propertyAddress::CjvxPropertyAddressLinear addrLinCp;
			resL = addresses.prepLin(addrLinCp, cnt);
			if (resL == JVX_NO_ERROR)
			{
				res = func(addrLinCp, elm);

				if (res == JVX_NO_ERROR)
				{
					if (func_post_success)
					{
						func_post_success(elm);
					}					
				}
			}
			elm->propRef->number_properties(callGate, &numL);
			cnt += numL;
		}
		else if (addresses.addrDescr)
		{
			jvx::propertyAddress::CjvxPropertyAddressDescriptor addrDescrCp;
			resL = addresses.prepDescr(addrDescrCp, elm->propDescriptorTag);
			if (resL == JVX_NO_ERROR)
			{
				res = func(addrDescrCp, elm);

				if (res == JVX_NO_ERROR)
				{
					if (func_post_success)
					{
						func_post_success(elm);
					}

					break;
				}
			}
		}
		else if (addresses.addrIdx)
		{
			jvx::propertyAddress::CjvxPropertyAddressGlobalId addrIdxCp;
			resL = addresses.prepIdx(addrIdxCp, elm->offsetShift);
			if (resL == JVX_NO_ERROR)
			{
				res = func(addrIdxCp, elm);

				if (res == JVX_NO_ERROR)
				{
					if (func_post_success)
					{
						func_post_success(elm);
					}
					break;
				}
			}
		}

		if (res != JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			// This indicates an error in the property function
			break;
		}
	} // for (i = 0; i < _common_set_properties.registeredSubmodules.size(); i++)
	return res;
}

jvxErrorType
CjvxProperties::_run_hook(
	const std::string& hookname, 
	const std::string& propname,
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType*& ptrRaw,
	const jvx::propertyAddress::CjvxPropertyAddressGlobalId& ident,
	jvx::propertyDetail::CjvxTranferDetail& tune, jvxExtendedProps* extProps,
	jvxPropertyCallbackPurpose purp)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hookname != "")
	{
		std::vector<oneCallback>::iterator selectionC;
		selectionC = findInCallbackList(_common_set_properties.registeredCallbacks, hookname);
		if (selectionC != _common_set_properties.registeredCallbacks.end())
		{
			if (selectionC->theCallback)
			{
				res = selectionC->theCallback(callGate, ptrRaw, ident, tune, extProps, purp, selectionC->priv);
				if (res != JVX_NO_ERROR)
				{
					if (_common_set_properties.reportMissedCallbacks)
					{
						*jout << "Warning <" << _common_set_properties.moduleReference << ">: shortcut callback <" << hookname
							<< "> for property " << propname << " failed, return value: " << jvxErrorType_descr(res) << "." << std::endl;
					}
				}
			}
			else
			{
				if (_common_set_properties.reportMissedCallbacks)
				{
					*jout << "Warning <" << _common_set_properties.moduleReference << ">: shortcut callback <" << hookname
						<< "> for property " << propname << " is registered as a NULL pointer reference." << std::endl;
				}
			}
		}
		else
		{
			if (_common_set_properties.reportMissedCallbacks)
			{
				*jout << "Warning <" << _common_set_properties.moduleReference << ">: shortcut callback <" << hookname << "> for property "
					<< propname << " missed." << std::endl;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxPropertySubModule::report_properties_modified(const char* props_set)
{
	if (parentReport)
	{
		parentReport->report_properties_modified(props_set);
	}
	return JVX_NO_ERROR;
}

