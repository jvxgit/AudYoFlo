#include "jvx-helpers.h"
#include "templates/jvxProperties/CjvxAccessProperties_direct.h"

void jvx_initPropertyReferenceTriple(jvx_propertyReferenceTriple* theTuple)
{
	theTuple->theObj = NULL;
	theTuple->theHdl = NULL;
	theTuple->theProps = NULL;
}

bool jvx_isValidPropertyReferenceTriple(jvx_propertyReferenceTriple* theTuple)
{
	if((theTuple->theObj != NULL) && (theTuple->theHdl != NULL) && (theTuple->theProps != NULL))
	{
		return(true);
	}
	return(false);
}

bool jvx_isValidNoPropertyReferenceTriple(jvx_propertyReferenceTriple* theTuple)
{
	if ((theTuple->theObj != NULL) && (theTuple->theHdl != NULL))
	{
		return(true);
	}
	return(false);
}

bool jvx_findPropertyDescriptor(const std::string& token, jvxSize* id, jvxDataFormat* form, jvxInt32* numElements, jvxPropertyDecoderHintType* decHt, std::string* descriptor)
{
	bool foundIt = false;
	int idx = 0;
	while(1)
	{
		if(jvxProperties[idx].id < 0)
		{
			break;
		}
		if(jvxProperties[idx].nameProperty == token)
		{
			if(id)
			{
				*id = jvxProperties[idx].id;
			}
			if(form)
			{
				*form = jvxProperties[idx].format;
			}
			if(numElements)
			{
				*numElements = jvxProperties[idx].numElements;
			}
			if(decHt)
			{
				*decHt = jvxProperties[idx].hintTp;
			}
			if(descriptor)
			{
				*descriptor = jvxProperties[idx].descriptor;
			}
			foundIt = true;
			break;
		}
		idx++;
	}
	return(foundIt);
}

bool jvx_findPropertyDescriptorId(jvxSize id, jvxDataFormat* form, jvxInt32* numElements, jvxPropertyDecoderHintType* decHt)
{
	bool foundIt = false;
	int idx = 0;
	while(1)
	{
		if(jvxProperties[idx].id < 0)
		{
			break;
		}
		if(jvxProperties[idx].id == id)
		{
			if(form)
			{
				*form = jvxProperties[idx].format;
			}
			if(numElements)
			{
				*numElements = jvxProperties[idx].numElements;
			}
			if(decHt)
			{
				*decHt = jvxProperties[idx].hintTp;
			}
			foundIt = true;
			break;
		}
		idx++;
	}
	return(foundIt);
}

bool jvx_findPropertyNameId(jvxSize id, std::string& name)
{
	bool foundIt = false;
	int idx = 0;
	while(1)
	{
		if(jvxProperties[idx].id < 0)
		{
			break;
		}
		if(jvxProperties[idx].id == id)
		{
			name = jvxProperties[idx].nameProperty;
			foundIt = true;
			break;
		}
		idx++;
	}
	return(foundIt);
}

bool JVX_CHECK_STATUS_PROPERTY_ONLY_READ(jvxBool isValid, jvxPropertyAccessType accessTp)
{
	return (!isValid) || !((accessTp == JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE) || (accessTp == JVX_PROPERTY_ACCESS_READ_ONLY_ON_START) || (accessTp == JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT));
}

bool JVX_CHECK_STATUS_PROPERTY_ONLY_READ(jvxPropertyAccessType accessTp)
{
	return !((accessTp == JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE) || (accessTp == JVX_PROPERTY_ACCESS_READ_ONLY_ON_START) || (accessTp == JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT));
}

jvxErrorType jvx_componentIdentification_properties_direct_add(
	IjvxHost* hHost,
	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo>& theRegisteredComponentReferenceTriples,
	jvxComponentIdentification tp, jvx_componentIdPropsCombo* refElmReturn)
{
	jvxBool addme = false;
	jvxApiString descr;

	jvx_componentIdPropsCombo newRefElm;
	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo>::iterator elm = theRegisteredComponentReferenceTriples.find(tp);
	if (elm != theRegisteredComponentReferenceTriples.end())
	{
		std::cout << JVX_OUTPUT_REPORT_DEBUG_LOCATION << "Registering property reference handle for type " << jvxComponentIdentification_txt(tp) << " failed since handle has been reigstered before." << std::endl;
		return JVX_ERROR_DUPLICATE_ENTRY;
	}

	if (!hHost)
	{
		std::cout << JVX_OUTPUT_REPORT_DEBUG_LOCATION << "Registering property reference handle for type " << jvxComponentIdentification_txt(tp) << " failed since host handle is invalid." << std::endl;
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	JVX_DSP_SAFE_ALLOCATE_OBJECT(newRefElm.refTriple, jvx_propertyReferenceTriple);

	jvx_initPropertyReferenceTriple(newRefElm.refTriple);
	jvx_getReferencePropertiesObject(hHost, newRefElm.refTriple, tp);
	if (jvx_isValidPropertyReferenceTriple(newRefElm.refTriple))
	{
		newRefElm.refTriple->theObj->description(&descr);
		// std::cout << "Successfully registered object <" << descr.std_str() << "> with property reference handle for type " << jvxComponentIdentification_txt(tp) << std::endl;
		addme = true;
	}
	else
	{
		if (jvx_isValidNoPropertyReferenceTriple(newRefElm.refTriple))
		{
			newRefElm.refTriple->theObj->description(&descr);
			// This is a valid option: there are components which have no property reference!!
			// std::cout << "Registered object <" << descr.std_str() << "> with missing property reference handle for type " << jvxComponentIdentification_txt(tp) << std::endl;
			addme = true;
		}
		else
		{
			std::cout << JVX_OUTPUT_REPORT_DEBUG_LOCATION << "Registering object for type " << jvxComponentIdentification_txt(tp) << " failed, reason: Missing object reference." << std::endl;
		}
	}

	if (addme)
	{
		CjvxAccessProperties_direct* newPropertyHandle = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(newPropertyHandle, CjvxAccessProperties_direct);
		newPropertyHandle->initialize(newRefElm.refTriple);
		newRefElm.accProps = newPropertyHandle;
		theRegisteredComponentReferenceTriples[tp] = newRefElm;

		if (refElmReturn)
		{
			*refElmReturn = newRefElm;
		}
		return JVX_NO_ERROR;
	}
	JVX_DSP_SAFE_DELETE_OBJECT(newRefElm.refTriple);
	return JVX_ERROR_INVALID_SETTING;
}

jvxErrorType jvx_componentIdentification_properties_find(
	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo>& theRegisteredComponentReferenceTriples,
	jvxComponentIdentification tp, jvx_componentIdPropsCombo& newRefElm)
{
	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo>::iterator elm = theRegisteredComponentReferenceTriples.find(tp);
	if (elm != theRegisteredComponentReferenceTriples.end())
	{
		newRefElm = elm->second;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType jvx_componentIdentification_properties_erase(
	IjvxHost* hHost,
	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo>& theRegisteredComponentReferenceTriples,
	jvxComponentIdentification tp)
{
	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo>::iterator elm = theRegisteredComponentReferenceTriples.find(tp);
	if (elm != theRegisteredComponentReferenceTriples.end())
	{
		JVX_DSP_SAFE_DELETE_OBJECT(elm->second.accProps); // Implicit "terminate"
		jvx_returnReferencePropertiesObject(hHost, elm->second.refTriple, tp);
		JVX_DSP_SAFE_DELETE_OBJECT(elm->second.refTriple);

		theRegisteredComponentReferenceTriples.erase(elm);

		// std::cout << "Unregistering property reference handle for type " << jvxComponentIdentification_txt(tp) << " successful." << std::endl;

		return JVX_NO_ERROR;
	}
	std::cout << JVX_OUTPUT_REPORT_DEBUG_LOCATION << "Unregistering property reference handle for type " << jvxComponentIdentification_txt(tp) << " failed since reference was not registered." << std::endl;
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
jvx_pushPropertyStringList(std::vector<std::string>& entries, IjvxObject* obj, IjvxProperties* prop, jvxSize id, jvxPropertyCategoryType cat,
	jvxCallManagerProperties& callGate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiStringList newList;

	newList.assign(entries);
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(id, cat);
	jvx::propertyDetail::CjvxTranferDetail trans(false);

	res = prop->set_property(callGate, jPRG(&newList, 1, JVX_DATAFORMAT_STRING_LIST), ident, trans);
	return(res);
}

jvxErrorType
jvx_pushPullPropertyStringList(std::vector<std::string>& entries, IjvxObject* obj, IjvxProperties* prop, jvxSize id,
	jvxPropertyCategoryType cat, jvxCallManagerProperties& callGate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiStringList newList;
	jvxSize i;

	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(id, cat);
	jvx::propertyDetail::CjvxTranferDetail trans(false);

	newList.assign(entries);
	res = prop->set_property(callGate, jPRG(&newList, 1, JVX_DATAFORMAT_STRING_LIST), ident, trans);
	if (res == JVX_NO_ERROR)
	{
		res = prop->get_property(callGate, jPRG( (jvxHandle*)&newList, 1, JVX_DATAFORMAT_STRING_LIST), ident, trans);
		if (res == JVX_NO_ERROR)
		{
			entries.clear();
			for (i = 0; i < newList.ll(); i++)
			{
				entries.push_back(newList.std_str_at(i));
			}
		}
	}
	return(res);
}

jvxErrorType
jvx_pushPullPropertyStringList(std::list<std::string>& entries, IjvxObject* obj, IjvxProperties* prop, jvxSize id,
	jvxPropertyCategoryType cat, jvxCallManagerProperties& callGate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiStringList newList;
	jvxSize i;

	newList.assign(entries);

	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(id, cat);
	jvx::propertyDetail::CjvxTranferDetail trans(false);

	res = prop->set_property(callGate, jPRG(&newList, 1, JVX_DATAFORMAT_STRING_LIST), ident, trans);
	if (res == JVX_NO_ERROR)
	{
		res = prop->get_property(callGate, jPRG( (jvxHandle*)&newList, 1, JVX_DATAFORMAT_STRING_LIST), ident, trans);
		if (res == JVX_NO_ERROR)
		{
			entries.clear();
			for (i = 0; i < newList.ll(); i++)
			{
				entries.push_back(newList.std_str_at(i));
			}
		}
	}
	return(res);
}

jvxErrorType
jvx_pullPropertyStringList(std::vector<std::string>& entries,  IjvxObject* obj, IjvxProperties* prop, jvxSize id, jvxPropertyCategoryType cat, 
	jvxCallManagerProperties& callGate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiStringList newList;
	jvxSize i;
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(id, cat);
	jvx::propertyDetail::CjvxTranferDetail trans(false);
	res = prop->get_property(callGate, jPRG( (jvxHandle*)&newList, 1, JVX_DATAFORMAT_STRING_LIST), ident, trans);
	if(res == JVX_NO_ERROR)
	{
		entries.clear();
		for (i = 0; i < newList.ll(); i++)
		{
			entries.push_back(newList.std_str_at(i));
		}
	}
	// else skippeed since error code is already set by subcomponent
	return(res);
}

jvxErrorType
jvx_pushPropertyString(std::string& entry,  IjvxObject* obj, IjvxProperties* prop, jvxSize id, jvxPropertyCategoryType cat,
	jvxCallManagerProperties& callGate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString newEntry;
	newEntry.assign_const(entry.c_str(), entry.size());
	
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(id, cat);
	jvx::propertyDetail::CjvxTranferDetail trans(false);
	
	res = prop->set_property(callGate, jPRG(&newEntry, 1, JVX_DATAFORMAT_STRING), ident, trans);
	return(res);
}

jvxErrorType
jvx_pushPullPropertyString(std::string& entry,  IjvxObject* obj, IjvxProperties* prop, jvxSize id, jvxPropertyCategoryType cat, jvxCallManagerProperties& callGate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString newEntry;
	newEntry.assign_const(entry.c_str(), entry.size());
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(id, cat);
	jvx::propertyDetail::CjvxTranferDetail trans(false);
	res = prop->set_property(callGate, jPRG(&newEntry, 1, JVX_DATAFORMAT_STRING), ident, trans);
	if (res == JVX_NO_ERROR)
	{
		res = prop->get_property(callGate, jPRG( (jvxHandle*)&newEntry, 1, JVX_DATAFORMAT_STRING), ident, trans);
		entry = newEntry.std_str();
		// else skipped, error code set by submodule
	}
	return(res);
}

jvxErrorType
jvx_pullPropertyString(std::string& entry,  IjvxObject* obj, IjvxProperties* prop, jvxSize id, jvxPropertyCategoryType cat, 
	jvxCallManagerProperties& callGate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString newEntry;
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(id, cat);
	jvx::propertyDetail::CjvxTranferDetail trans(false);
	res = prop->get_property(callGate, jPRG( (jvxHandle*)&newEntry, 1, JVX_DATAFORMAT_STRING), ident, trans);
	if(res == JVX_NO_ERROR)
	{
		entry = newEntry.std_str();
	}

	// else skipped, error code is set by submodule
	return(res);
}

/*
* A switch buffer contains numBuffers * numChannels fields of bLength elements. The field, however, is
* continuous. The format is 
* buf[0] buf[1] buf[2]
* In each buffer the format is non-interleaved,
*		------------buf[0]---------------
*	++++channel[0]++++|+++++channel[1]+++
*   elm[0]elm[1]elm[2]|elm[0]elm[1]elm[2]
*/
jvxExternalBuffer* jvx_allocate2DFieldExternalBuffer_full(jvxSize bLength,
	jvxSize numChannels, jvxDataFormat form, jvxDataFormatGroup subform,
	jvx_lock_buffer lockf, jvx_try_lock_buffer try_lockf, jvx_unlock_buffer unlockf,
	jvxSize* szFld, jvxSize numBuffers, jvxSize seg_x, jvxSize seg_y)
{
	jvxExternalBuffer* theNewHeader = NULL;

	JVX_DSP_SAFE_ALLOCATE_OBJECT(theNewHeader, jvxExternalBuffer);

	theNewHeader->fill_height = 0;
	theNewHeader->idx_read = 0;
	theNewHeader->length = bLength;
	theNewHeader->number_channels = numChannels;
	theNewHeader->tp = jvx::JVX_EXTERNAL_BUFFER_2D_FIELD_BUFFER;
	theNewHeader->subTp = jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL;

	theNewHeader->specific.the2DFieldBuffer_full.common.number_buffers = numBuffers;
	theNewHeader->specific.the2DFieldBuffer_full.common.seg_x = seg_x;
	theNewHeader->specific.the2DFieldBuffer_full.common.seg_y = seg_y;

	theNewHeader->formFld = form;
	theNewHeader->subFormFld = subform;
	theNewHeader->ptrFld = NULL;
	theNewHeader->szElmFld = jvxDataFormat_getsize(theNewHeader->formFld)*jvxDataFormatGroup_getsize(theNewHeader->subFormFld);
	assert(theNewHeader->szElmFld);
	theNewHeader->numElmFldOneChanOneBuf = theNewHeader->length;
	theNewHeader->numElmFldOneBuf = theNewHeader->numElmFldOneChanOneBuf * theNewHeader->number_channels;
	theNewHeader->numElmFld = theNewHeader->numElmFldOneBuf * theNewHeader->specific.the2DFieldBuffer_full.common.number_buffers;
	theNewHeader->szFld = theNewHeader->szElmFld * theNewHeader->numElmFld;

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(theNewHeader->ptrFld, jvxByte, theNewHeader->szFld);
	if (szFld)
	{
		*szFld = theNewHeader->szFld;
	}

	JVX_MUTEX_HANDLE* mutHdl = NULL;
	JVX_SAFE_NEW_OBJ(mutHdl, JVX_MUTEX_HANDLE);
	JVX_INITIALIZE_MUTEX(*mutHdl);
	theNewHeader->safe_access.priv = (jvxHandle*)mutHdl;
	theNewHeader->safe_access.lockf = lockf;
	theNewHeader->safe_access.unlockf = unlockf;
	theNewHeader->safe_access.try_lockf = try_lockf;

	theNewHeader->specific.the2DFieldBuffer_full.report_triggerf = NULL;
	theNewHeader->specific.the2DFieldBuffer_full.report_trigger_priv = NULL;
	theNewHeader->specific.the2DFieldBuffer_full.report_completef = NULL;
	theNewHeader->specific.the2DFieldBuffer_full.report_complete_priv = NULL;

	return(theNewHeader);
}

/*
* A switch buffer contains numBuffers * numChannels fields of bLength elements. The field, however, is
* continuous. The format is
* buf[0] buf[1] buf[2]
* In each buffer the format is non-interleaved,
*		------------buf[0]---------------
*	++++channel[0]++++|+++++channel[1]+++
*   elm[0]elm[1]elm[2]|elm[0]elm[1]elm[2]
*/
jvxExternalBuffer* jvx_allocate2DFieldExternalBuffer_inc(jvxSize bLength,
	jvxSize numChannels, jvxDataFormat form, jvxDataFormatGroup subform,
	jvx_lock_buffer lockf, jvx_try_lock_buffer try_lockf, jvx_unlock_buffer unlockf,
	jvxSize* szFld, jvxSize numBuffers, jvxSize seg_x, jvxSize seg_y)
{
	jvxExternalBuffer* theNewHeader = NULL;

	JVX_DSP_SAFE_ALLOCATE_OBJECT(theNewHeader, jvxExternalBuffer);

	theNewHeader->fill_height = 0;
	theNewHeader->idx_read = 0;
	theNewHeader->length = bLength;
	theNewHeader->number_channels = numChannels;
	theNewHeader->tp = jvx::JVX_EXTERNAL_BUFFER_2D_FIELD_BUFFER;
	theNewHeader->subTp = jvx::JVX_EXTERNAL_BUFFER_SUB_2D_INC;

	theNewHeader->specific.the2DFieldBuffer_inc.common.number_buffers = numBuffers;
	theNewHeader->specific.the2DFieldBuffer_inc.common.seg_x = seg_x;
	theNewHeader->specific.the2DFieldBuffer_inc.common.seg_y = seg_y;

	theNewHeader->formFld = form;
	theNewHeader->subFormFld = subform;
	theNewHeader->ptrFld = NULL;
	theNewHeader->szElmFld = jvxDataFormat_getsize(theNewHeader->formFld)*jvxDataFormatGroup_getsize(theNewHeader->subFormFld);
	assert(theNewHeader->szElmFld);
	theNewHeader->numElmFldOneChanOneBuf = theNewHeader->length;
	theNewHeader->numElmFldOneBuf = theNewHeader->numElmFldOneChanOneBuf * theNewHeader->number_channels;
	theNewHeader->numElmFld = theNewHeader->numElmFldOneBuf * theNewHeader->specific.the2DFieldBuffer_inc.common.number_buffers;
	theNewHeader->szFld = theNewHeader->szElmFld * theNewHeader->numElmFld;

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(theNewHeader->ptrFld, jvxByte, theNewHeader->szFld);
	if (szFld)
	{
		*szFld = theNewHeader->szFld;
	}

	JVX_MUTEX_HANDLE* mutHdl = NULL;
	JVX_SAFE_NEW_OBJ(mutHdl, JVX_MUTEX_HANDLE);
	JVX_INITIALIZE_MUTEX(*mutHdl);
	theNewHeader->safe_access.priv = (jvxHandle*)mutHdl;
	theNewHeader->safe_access.lockf = lockf;
	theNewHeader->safe_access.unlockf = unlockf;
	theNewHeader->safe_access.try_lockf = try_lockf;

	theNewHeader->specific.the2DFieldBuffer_inc.num_lost = 0;
	//theNewHeader->specific.the2DFieldBuffer_inc.posi_start = 0;
	//theNewHeader->specific.the2DFieldBuffer_inc.posi_stop = 0;

	return(theNewHeader);
}


/*
 * A circ buffer contains numChannels subfields of bLength elements. The allocated field however is continuous.
 * The format of channels is non-interleaved.
 *	++++channel[0]++++|+++++channel[1]+++
 *   elm[0]elm[1]elm[2]|elm[0]elm[1]elm[2]
 */
jvxExternalBuffer* jvx_allocate1DCircExternalBuffer(jvxSize bLength, 
	jvxSize numChannels, jvxDataFormat form, jvxDataFormatGroup subform,
	jvx_lock_buffer lockf, jvx_try_lock_buffer try_lockf, jvx_unlock_buffer unlockf,
	jvxSize* szFld)
{
	jvxExternalBuffer* theNewHeader = NULL;

	JVX_DSP_SAFE_ALLOCATE_OBJECT(theNewHeader, jvxExternalBuffer);
	
	theNewHeader->fill_height = 0;
	theNewHeader->idx_read = 0;
	theNewHeader->length = bLength;
	theNewHeader->number_channels = numChannels;
	theNewHeader->tp = jvx::JVX_EXTERNAL_BUFFER_1D_CIRCULAR_BUFFER;
	theNewHeader->subTp = jvx::JVX_EXTERNAL_BUFFER_SUB_NONE;

	theNewHeader->formFld = form;
	theNewHeader->subFormFld = subform;
	theNewHeader->ptrFld = NULL;
	theNewHeader->szElmFld = jvxDataFormat_getsize(theNewHeader->formFld) * jvxDataFormatGroup_getsize(theNewHeader->subFormFld);
	assert(theNewHeader->szElmFld);
	theNewHeader->numElmFldOneChanOneBuf = theNewHeader->length;
	theNewHeader->numElmFldOneBuf = theNewHeader->length* theNewHeader->number_channels;
	theNewHeader->numElmFld = theNewHeader->length * theNewHeader->number_channels;
	theNewHeader->szFld = theNewHeader->szElmFld * theNewHeader->numElmFld;

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(theNewHeader->ptrFld, jvxByte, theNewHeader->szFld);
	if(szFld)
	{
		*szFld = theNewHeader->szFld;
	}

	JVX_MUTEX_HANDLE* mutHdl = NULL;
	JVX_SAFE_NEW_OBJ(mutHdl, JVX_MUTEX_HANDLE);
	JVX_INITIALIZE_MUTEX(*mutHdl);
	theNewHeader->safe_access.priv = (jvxHandle*)mutHdl;
	theNewHeader->safe_access.lockf = lockf;
	theNewHeader->safe_access.unlockf = unlockf;
	theNewHeader->safe_access.try_lockf = try_lockf;

	theNewHeader->specific.the1DCircBuffer.allowOverwrite = false;
	theNewHeader->specific.the1DCircBuffer.numWraps = 0;

	return(theNewHeader);
}

void 
jvx_deallocateExternalBuffer(jvxExternalBuffer* header)
{
	JVX_MUTEX_HANDLE* mutHdl = (JVX_MUTEX_HANDLE*)header->safe_access.priv;
	JVX_TERMINATE_MUTEX(*mutHdl);
	JVX_SAFE_DELETE_OBJ(mutHdl);

	JVX_DSP_SAFE_DELETE_FIELD(header->ptrFld);
	header->ptrFld = NULL;

	JVX_DSP_SAFE_DELETE_OBJECT(header);
}

void jvx_valuesToValueInRange(jvxValueInRange& theObj, jvxData minV, jvxData maxV, jvxData val )
{
	theObj.minVal = minV;
	theObj.maxVal = maxV;
	theObj.val() = val;
}

// ==================================================================

void jvx_initPropertyDescription(jvxPropertyDescriptor& theStr)
{
	theStr.reset();
}

jvxErrorType jvx_getPropertyDescription(IjvxProperties* theProps, 
	jvx::propertyDescriptor::IjvxPropertyDescriptor& theDescr, 
	jvx::propertyAddress::IjvxPropertyAddress& ident,
	jvxCallManagerProperties& callGate)
{
	jvxErrorType res = theProps->description_property(callGate, theDescr, ident);
	return(res);
}

jvxErrorType jvx_getPropertyDescription(IjvxHost* theHost, 
	jvx::propertyDescriptor::IjvxPropertyDescriptor& theDescr, 
	jvxComponentIdentification tp, 
	jvx::propertyAddress::IjvxPropertyAddress& ident,
	jvxCallManagerProperties& callGate)
{
	jvxSize i,num;
	jvx_propertyReferenceTriple theTriple;
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;

	jvx_initPropertyReferenceTriple(&theTriple);
	jvx_getReferencePropertiesObject(theHost, &theTriple, tp);
	if(jvx_isValidPropertyReferenceTriple(&theTriple))
	{
		res = theTriple.theProps->description_property(callGate, theDescr, ident);
		jvx_returnReferencePropertiesObject(theHost, &theTriple, tp);
	}
	return(res);
}

jvxErrorType jvx_get_property(IjvxProperties* theProps, jvxHandle* fld, 
	jvxSize offsetStart,
	jvxSize numElements,
	jvxDataFormat format,
	jvxBool contentOnly,
	const char* descriptor,
	jvxCallManagerProperties& callGate)
{
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descriptor);
	jvx::propertyDetail::CjvxTranferDetail trans(contentOnly, offsetStart);	
	if(theProps)
	{
		return(theProps->get_property(callGate, jPRG( fld,
			     numElements,
			     format),
			     ident, trans));
	}
	return(JVX_ERROR_INVALID_SETTING);
}

jvxErrorType jvx_get_property(IjvxProperties* theProps, jvxHandle* fld,
	jvxSize offsetStart,
	jvxSize numElements,
	jvxDataFormat format,
	jvxBool contentOnly,
	jvxSize idxprop,
	jvxCallManagerProperties& callGate)
{
	jvxSize num = 0;
	jvxSize idx = JVX_SIZE_UNSELECTED;
	jvxPropertyCategoryType cat;
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	jvx::propertyAddress::CjvxPropertyAddressLinear ident(idxprop);
	jvx::propertyDetail::CjvxTranferDetail trans(contentOnly, offsetStart);
	if (theProps)
	{
		return(theProps->get_property(callGate, jPRG( fld,
				numElements,
				format),
				ident, trans));
	}
	return(JVX_ERROR_INVALID_SETTING);
}

/*
jvxErrorType jvx_status_get_property(IjvxProperties* theProps, const char* descriptor)
{
	return(jvx_get_property(theProps, NULL, 0, 0, JVX_DATAFORMAT_NONE, false, descriptor));
}

jvxErrorType jvx_status_set_property(IjvxProperties* theProps, const char* descriptor)
{
	return(jvx_set_property(theProps, NULL, 0, 0, JVX_DATAFORMAT_NONE, false, descriptor));
}
*/

jvxErrorType jvx_props_property(IjvxProperties* theProps,
	jvxSize* numElements,
	jvxDataFormat* format,
	const char* descriptor,
	jvxCallManagerProperties& callGate)
{
	jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor idProp(descriptor);
	jvxErrorType res;
	
	res = theProps->description_property(callGate, descr, idProp);
	if (res == JVX_NO_ERROR)
	{
		if (format) *format = descr.format;
		if (numElements) *numElements = descr.num;
	}
	return(res);
}

jvxErrorType jvx_get_property(IjvxHost* theHost, jvxHandle* fld, 
	jvxSize offsetStart,
	jvxSize numElements,
	jvxDataFormat format,
	jvxBool contentOnly,
	jvxComponentIdentification tp,
	const char* descriptor,
	jvxCallManagerProperties& callGate)
{
	jvx::propertyAddress::CjvxPropertyAddressDescriptor idProp(descriptor);
	jvx::propertyDetail::CjvxTranferDetail trans(contentOnly, offsetStart);
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;

	jvx_propertyReferenceTriple theTriple;
	jvx_initPropertyReferenceTriple(&theTriple);
	jvx_getReferencePropertiesObject(theHost, &theTriple, tp);
	if(jvx_isValidPropertyReferenceTriple(&theTriple))
	{
		res = theTriple.theProps->get_property(callGate, jPRG( fld,
			numElements,
			format),
			idProp, trans);
		jvx_returnReferencePropertiesObject(theHost, &theTriple, tp);
	}
	return(res);
}

jvxErrorType jvx_set_property(IjvxProperties* theProps, jvxHandle* fld, 
	jvxSize offsetStart,
	jvxSize numElements,
	jvxDataFormat format,
	jvxBool contentOnly,
	const char* descriptor,
	jvxCallManagerProperties& callGate)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descriptor);
	jvx::propertyDetail::CjvxTranferDetail trans(contentOnly, offsetStart);
	
	if(theProps)
	{
		res = theProps->set_property(callGate, 
			jPRG(fld, numElements, format),
			ident, trans);
	}
	return(res);
}

jvxErrorType jvx_install_property_reference(
	IjvxProperties* theProps,
	jvxCallManagerProperties& callGate,
	jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
	jvx::propertyAddress::IjvxPropertyAddress& ident)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	if(theProps)
	{
		res = theProps->install_property_reference(callGate,
			ptrRaw, ident);

		/*if (res == JVX_NO_ERROR)
		{
		jvxDataFormat form = JVX_DATAFORMAT_POINTER;
		if (fld)
		{
			form = fld->formFld;
		}
		jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(fld, isValid, form);
		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(descr.globalIdx, descr.category);
		if (fld)
		{
			return(theProps->install_property_reference(
				callGate, &ptrRaw, &ident));
		}
		else {
			return(theProps->uninstall_property_reference(
				callGate, &ptrRaw, &ident));
		}*/
	}
	return(res);
}

jvxErrorType jvx_uninstall_property_reference(
	IjvxProperties* theProps,
	jvxCallManagerProperties& callGate,
	jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
	jvx::propertyAddress::IjvxPropertyAddress& ident)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	if (theProps)
	{
		res = theProps->uninstall_property_reference(callGate,
			ptrRaw, ident);
	}
	return(res);
}

jvxCBitField 
jvx_check_config_mode_put_configuration(jvxConfigModeFlags config_mode_inCall, jvxConfigModeFlags config_mode_inProperty, 
	jvxAccessRightFlags access_right_inCall, jvxAccessRightFlags_rwcd access_right_inProperty)
{
	jvxCBitField retVal = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
	jvxAccessRightFlags access_right_write = JVX_EXTRACT_WRITE_FLAGS(access_right_inProperty);
	jvxConfigModeFlags common_config_mode = (config_mode_inCall & config_mode_inProperty);
	jvxConfigModeFlags common_access_rights = (access_right_inCall & access_right_write);

	if( 
		(common_config_mode & JVX_CONFIG_MODE_FULL) ||
		(common_config_mode & JVX_CONFIG_MODE_OVERLAY))
	{
		if (common_access_rights & 0x3FFF) // all except the highest groups
		{
			retVal |= JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT;
		}
	}
		
	if(common_config_mode & JVX_CONFIG_MODE_ACCESS_RIGHTS)
	{
		if (common_access_rights & JVX_ACCESS_ROLE_GROUP_ACCESS_RIGHTS)
		{
			retVal |= JVX_PROPERTY_FLAGTAG_OPERATE_READ_ACCESS_RIGHTS;
		}
	}

	if (common_config_mode & JVX_CONFIG_MODE_CONFIG_MODE)
	{
		if (common_access_rights & JVX_ACCESS_ROLE_GROUP_CONFIG_MODE)
		{
			retVal |= JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONFIG_MODE;
		}
	}
	return retVal;
}

jvxCBitField jvx_check_config_mode_get_configuration(jvxConfigModeFlags config_mode_inCall, jvxConfigModeFlags config_mode_inProperty,
	jvxAccessRightFlags access_right_inCall, jvxAccessRightFlags_rwcd access_right_inProperty)
{
	jvxCBitField retVal = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
	jvxAccessRightFlags access_right_read = JVX_EXTRACT_READ_FLAGS(access_right_inProperty);
	jvxConfigModeFlags common_config_mode = (config_mode_inCall & config_mode_inProperty);
	jvxConfigModeFlags common_access_rights = (access_right_inCall & access_right_read);

	if (
		(common_config_mode & JVX_CONFIG_MODE_FULL) ||
		(common_config_mode & JVX_CONFIG_MODE_OVERLAY))
	{
		if (common_access_rights & 0x3FFF) // all except the highest groups
		{
			retVal |= JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT;
		}
	}

	if (common_config_mode & JVX_CONFIG_MODE_ACCESS_RIGHTS)
	{
		if (common_access_rights & JVX_ACCESS_ROLE_GROUP_ACCESS_RIGHTS)
		{
			retVal |= JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_ACCESS_RIGHTS;
		}
	}

	if (common_config_mode & JVX_CONFIG_MODE_CONFIG_MODE)
	{
		if (common_access_rights & JVX_ACCESS_ROLE_GROUP_CONFIG_MODE)
		{
			retVal |= JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONFIG_MODE;
		}
	}
	return retVal;
}

/*
jvxCBitField jvx_check_config_mode_get_configuration(jvxFlagTag inCall, jvxFlagTag inProperty)
{
	if(inCall & (1 << JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_SHIFT))
	{
		if(inProperty& (1 << JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_SHIFT))
		{
			return(JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT);
		}
		return(JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING);
	}
	return(JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT | JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_FLAGTAG);
}
*/


jvxErrorType jvx_selectionSetAdd(jvxPropertyContainerSingle<jvxSelectionList_cpp>& sllst, const std::string& entry,
	jvxPropertySelectionListAddress addr)
{
	jvxSize i;
	for (i = 0; i < sllst.value.entries.size(); i++)
	{
		if (entry == sllst.value.entries[i])
		{
			switch (addr)
			{
			case JVX_PROPERTY_SELECTIONLIST_ADDRESS_SELECTION:
				jvx_bitSet(sllst.value.selection(), i);
				break;
			case JVX_PROPERTY_SELECTIONLIST_ADDRESS_SELECTABLE:
				jvx_bitSet(sllst.value.selectable, i);
				break;
			case JVX_PROPERTY_SELECTIONLIST_ADDRESS_EXCLUSIVE:
				jvx_bitSet(sllst.value.exclusive, i);
				break;
			default:
				assert(0);
			}
			return JVX_NO_ERROR;
			break;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

namespace jvx {
	namespace helper {
		namespace properties
		{

			std::string
			collectedPropsToString(const std::list<std::string>& propLst)
			{
				std::string allProps;
				if (!propLst.empty())
				{
					jvxBool firstFound = false;
					for (auto elm : propLst)
					{
						if (firstFound)
						{
							allProps += ",";
						}
						allProps += elm;
						firstFound = true;
					}
					if (propLst.size() > 1)
					{
						allProps = "[" + allProps + "]";
					}
				}
				return allProps;
			}

			std::list<std::string>
			stringToCollectedProps(const std::string& propLstStr)
			{

				std::list<std::string> lstRet;
				size_t idxStart = propLstStr.find('[');
				size_t idxStop = propLstStr.rfind(']');
				if (
					(idxStart != std::string::npos) && (idxStop != std::string::npos) && (idxStart < idxStop)
					)
				{
					std::string expr = propLstStr.substr(idxStart+1, idxStop - idxStart - 1);

					while (1)
					{
						idxStart = expr.find(',');
						if (idxStart != std::string::npos)
						{
							lstRet.push_back(expr.substr(0, idxStart));
							expr = expr.substr(idxStart+1);
						}
						else
						{
							break;
						}
					}
					if (expr.size())
					{
						lstRet.push_back(expr);
					}
				}
				return lstRet;
			}

			jvxErrorType fromString(jvxHandle* fld, jvxSize offsetStart, jvxSize numElements, jvxDataFormat format,
				jvxBool& contentOnly, jvxPropertyDecoderHintType decTp, std::string loadTarget, std::string loadTargetPP, jvxCBitField fineTuningParams,
				jvxSize llsel)
			{
				jvxErrorType res = JVX_NO_ERROR;
				jvxBool propAccessible = false;
				jvxSize numBytes = 0;
				jvxBool err = false;
				jvxSize txt_pos;
				std::string txtToken;
				contentOnly = true;
				jvxBool txt_lst_found = false;
				std::vector<std::string> theTokens;
				jvxSize selLst_ll = 0;

				jvxByte* fldB = (jvxByte*)fld;
				jvxSize numCopy = numElements;

				// Currently, only single elements are supported
				if (numElements > 0)
				{
					if (numElements > 1)
					{
						switch (format)
						{
						case JVX_DATAFORMAT_SIZE:
						case JVX_DATAFORMAT_DATA:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_U16BIT_LE:
						case JVX_DATAFORMAT_U8BIT:
						case JVX_DATAFORMAT_U32BIT_LE:
						case JVX_DATAFORMAT_U64BIT_LE:
							assert(fld);
							/*
							// This here should have done before. if not, we need to allocate temporarily
							numBytes = jvxDataFormat_size[format] * numElements;
							memset(fld, 0, jvxDataFormat_size[format] * numElements);
							*/
							res = jvx_binString2ValueList(loadTarget, loadTargetPP, fld, format, numElements, &numBytes);
							if (res != JVX_NO_ERROR)
							{
								res = jvx_string2ValueList(loadTarget, fld, format, numElements, &numCopy);
							}
							break;

						case JVX_DATAFORMAT_SELECTION_LIST:
						case JVX_DATAFORMAT_STRING:
						case JVX_DATAFORMAT_STRING_LIST:
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
						case JVX_DATAFORMAT_HANDLE:
						case JVX_DATAFORMAT_BYTE:
						case JVX_DATAFORMAT_LIMIT:
						case JVX_DATAFORMAT_NONE:
						case JVX_DATAFORMAT_POINTER:
							res = JVX_ERROR_UNSUPPORTED;
							break;
						default:
							assert(0);
						}
					}
					else
					{
						switch (format)
						{
						case JVX_DATAFORMAT_SIZE:
							*((jvxSize*)fld) = jvx_string2Size(loadTarget, err);
							if (err)
							{
								res = JVX_ERROR_PARSE_ERROR;
							}
							break;
						case JVX_DATAFORMAT_DATA:
							*((jvxData*)fld) = jvx_string2Data(loadTarget, err);
							if (err)
							{
								res = JVX_ERROR_PARSE_ERROR;
							}
							break;
						case JVX_DATAFORMAT_16BIT_LE:

							if (
								(decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)||
								(decTp == JVX_PROPERTY_DECODER_SINGLE_SELECTION)
								)
							{
								if (
									(loadTarget == "yes") || (loadTarget == "1"))
								{
									*((jvxInt16*)fld) = c_true;
								}
								else if (
									(loadTarget == "no") || (loadTarget == "0"))
								{
									*((jvxInt16*)fld) = c_false;
								}
								else
								{
									std::cout << "Unknown on/off specification <" << loadTarget << ">, treating as <off>." << std::endl;
									*((jvxInt16*)fld) = c_false;
								}
							}
							else if (decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
							{
								err = true;
								jvxDataFormat form = jvxDataFormat_decode(loadTarget.c_str());
								*((jvxInt16*)fld) = JVX_SIZE_INT16(form);
							}
							else if (decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
							{
								if (loadTarget == "dontcare")
								{
									*((jvxInt16*)fld) = -2;
								}
								else
								{
									*((jvxInt16*)fld) = (jvxInt16)jvx_string2IntMax(loadTarget, err);
								}
							}
							else
							{
								*((jvxInt16*)fld) = (jvxInt16)jvx_string2IntMax(loadTarget, err);
							}
							if (err)
							{
								res = JVX_ERROR_PARSE_ERROR;
							}
							break;
						case JVX_DATAFORMAT_8BIT:
							*((jvxInt8*)fld) = (jvxInt8)jvx_string2IntMax(loadTarget, err);
							if (err)
							{
								res = JVX_ERROR_PARSE_ERROR;
							}
							break;
						case JVX_DATAFORMAT_32BIT_LE:
							if (decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
							{
								if (loadTarget == "dontcare")
								{
									*((jvxInt32*)fld) = -2;
								}
								else
								{
									*((jvxInt32*)fld) = (jvxInt32)jvx_string2IntMax(loadTarget, err);
								}
							}
							else
							{
								*((jvxInt32*)fld) = (jvxInt32)jvx_string2IntMax(loadTarget, err);
							}
							if (err)
							{
								res = JVX_ERROR_PARSE_ERROR;
							}
							break;
						case JVX_DATAFORMAT_64BIT_LE:
							*((jvxInt64*)fld) = (jvxInt64)jvx_string2IntMax(loadTarget, err);
							if (err)
							{
								res = JVX_ERROR_PARSE_ERROR;
							}
							break;

						case JVX_DATAFORMAT_U16BIT_LE:

							if ((decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF) ||
								(decTp == JVX_PROPERTY_DECODER_SINGLE_SELECTION))
							{
								if (
									(loadTarget == "yes") || (loadTarget == "1"))
								{
									*((jvxUInt16*)fld) = c_true;
								}
								else if (
									(loadTarget == "no") || (loadTarget == "0"))
								{
									*((jvxUInt16*)fld) = c_false;
								}
								else
								{
									std::cout << "Unknown on/off specification" << loadTarget << ", treating as <off>." << std::endl;
									*((jvxUInt16*)fld) = c_false;
								}
							}
							else if (decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
							{
								err = true;
								jvxDataFormat form = jvxDataFormat_decode(loadTarget.c_str());
								*((jvxUInt16*)fld) = JVX_SIZE_UINT16(form);
							}
							else if (decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
							{
								if (loadTarget == "dontcare")
								{
									*((jvxUInt16*)fld) = -2;
								}
								else
								{
									*((jvxUInt16*)fld) = (jvxUInt16)jvx_string2IntMax(loadTarget, err);
								}
							}
							else
							{
								*((jvxUInt16*)fld) = (jvxUInt16)jvx_string2IntMax(loadTarget, err);
							}
							if (err)
							{
								res = JVX_ERROR_PARSE_ERROR;
							}
							break;
						case JVX_DATAFORMAT_U8BIT:
							*((jvxUInt8*)fld) = (jvxUInt8)jvx_string2IntMax(loadTarget, err);
							if (err)
							{
								res = JVX_ERROR_PARSE_ERROR;
							}
							break;
						case JVX_DATAFORMAT_U32BIT_LE:
							if (decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
							{
								if (loadTarget == "dontcare")
								{
									*((jvxUInt32*)fld) = -2;
								}
								else
								{
									*((jvxUInt32*)fld) = (jvxUInt32)jvx_string2IntMax(loadTarget, err);
								}
							}
							else
							{
								*((jvxUInt32*)fld) = (jvxUInt32)jvx_string2IntMax(loadTarget, err);
							}
							if (err)
							{
								res = JVX_ERROR_PARSE_ERROR;
							}
							break;
						case JVX_DATAFORMAT_U64BIT_LE:
							*((jvxUInt64*)fld) = (jvxUInt64)jvx_string2UInt64(loadTarget, err);
							if (err)
							{
								res = JVX_ERROR_PARSE_ERROR;
							}
							break;


						case JVX_DATAFORMAT_SELECTION_LIST:

							if (jvx_bitTest(fineTuningParams, JVX_STRING_PROPERTY_FINE_TUNING_SELECTIONLIST_SELID_SHIFT))
							{
								((jvxSelectionList*)fld)->bitFieldSelected() = jvx_string2BitField2SelId(loadTarget, llsel, err);
								if (err)
								{
									res = JVX_ERROR_PARSE_ERROR;
								}
							}
							else
							{

								txt_pos = loadTarget.find("::");
								if (txt_pos != std::string::npos)
								{
									txtToken = loadTarget.substr(0, txt_pos);
									((jvxSelectionList*)fld)->bitFieldSelected() = jvx_string2BitField(txtToken, err);
									loadTarget = loadTarget.substr(txt_pos + 2, std::string::npos);
									if (err)
									{
										res = JVX_ERROR_PARSE_ERROR;
									}
								}
								else
								{
									// Very short version: set only the selection!!!
									txtToken = loadTarget.substr(0, txt_pos);
									((jvxSelectionList*)fld)->bitFieldSelected() = jvx_string2BitField(txtToken, err);
									if (err)
									{
										res = JVX_ERROR_PARSE_ERROR;
									}
									break;
								}

								txt_pos = loadTarget.find("::");
								if (txt_pos != std::string::npos)
								{
									txtToken = loadTarget.substr(0, txt_pos);
									((jvxSelectionList*)fld)->bitFieldSelectable = jvx_string2BitField(txtToken, err);
									loadTarget = loadTarget.substr(txt_pos + 2, std::string::npos);
									if (err)
									{
										res = JVX_ERROR_PARSE_ERROR;
									}
								}
								else
								{
									// Very short version: set only the selection!!!
									txtToken = loadTarget.substr(0, txt_pos);
									((jvxSelectionList*)fld)->bitFieldSelected() = jvx_string2BitField(txtToken, err);
									if (err)
									{
										res = JVX_ERROR_PARSE_ERROR;
									}
									break;
								}

								txt_pos = loadTarget.find("::");
								if (txt_pos != std::string::npos)
								{
									txtToken = loadTarget.substr(0, txt_pos);
									((jvxSelectionList*)fld)->bitFieldExclusive = jvx_string2BitField(txtToken, err);
									loadTarget = loadTarget.substr(txt_pos + 2, std::string::npos);
									if (err)
									{
										res = JVX_ERROR_PARSE_ERROR;
									}
								}
								else
								{
									res = JVX_ERROR_ELEMENT_NOT_FOUND;
								}

								txt_pos = loadTarget.find("::");
								if (txt_pos != std::string::npos)
								{
									txt_lst_found = true;
									txtToken = loadTarget.substr(0, txt_pos);
									loadTarget = loadTarget.substr(txt_pos + 2, std::string::npos);
								}
								else
								{
									txtToken = loadTarget;
									loadTarget = "";
								}

								selLst_ll = jvx_string2Size(txtToken, err);
								if (err)
								{
									res = JVX_ERROR_PARSE_ERROR;
								}

								if (!loadTargetPP.empty())
								{
									contentOnly = false;
									jvx_parseCommandLineOneToken(loadTargetPP, theTokens, ':');
									((jvxSelectionList*)fld)->strList.assign(theTokens);
								}
								else
								{
									if (txt_lst_found)
									{
										contentOnly = false;
										jvx_parseCommandLineOneToken(loadTarget, theTokens, ':'); // We need to use a separator other than "," tp work due to the command parser
										((jvxSelectionList*)fld)->strList.assign(theTokens);
									}
									else
									{
										((jvxSelectionList*)fld)->strList.assign_empty(selLst_ll);
									}
								}
							}
							break;
						case JVX_DATAFORMAT_STRING:
							((jvxApiString*)fld)->assign(loadTarget);
							break;
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
							txt_pos = loadTarget.find("::");
							((jvxValueInRange*)fld)->val() = 0;
							((jvxValueInRange*)fld)->minVal = 0;
							((jvxValueInRange*)fld)->maxVal = 0;
							if (txt_pos == std::string::npos)
							{
								// contentOnly is true!
								((jvxValueInRange*)fld)->val() = jvx_string2Data(loadTarget, err);
								if (err)
								{
									res = JVX_ERROR_PARSE_ERROR;
								}
							}
							else
							{
								contentOnly = false;
								txtToken = loadTarget.substr(0, txt_pos);
								loadTarget = loadTarget.substr(txt_pos + 2, std::string::npos);
								((jvxValueInRange*)fld)->val() = jvx_string2Data(txtToken, err);
								if (err)
								{
									res = JVX_ERROR_PARSE_ERROR;
								}

								// loadTarget contains min::max
								if (res == JVX_NO_ERROR)
								{
									txt_pos = loadTarget.find("::");
									if (txt_pos != std::string::npos)
									{
										txtToken = loadTarget.substr(0, txt_pos);
										loadTarget = loadTarget.substr(txt_pos + 2, std::string::npos);
										((jvxValueInRange*)fld)->minVal = jvx_string2Data(txtToken, err);
										if (err)
										{
											res = JVX_ERROR_PARSE_ERROR;
										}

										((jvxValueInRange*)fld)->maxVal = jvx_string2Data(loadTarget, err);
										if (err)
										{
											res = JVX_ERROR_PARSE_ERROR;
										}
									}
									else
									{
										res = JVX_ERROR_ELEMENT_NOT_FOUND;
									}
								}
							}

							break;
						case JVX_DATAFORMAT_HANDLE:
						case JVX_DATAFORMAT_BYTE:
						case JVX_DATAFORMAT_LIMIT:
						case JVX_DATAFORMAT_NONE:
						case JVX_DATAFORMAT_POINTER:
						case JVX_DATAFORMAT_STRING_LIST:
							res = JVX_ERROR_UNSUPPORTED;
							break;
						default:
							assert(0);
						}
					}
				}
				else
				{
					res = JVX_ERROR_INTERNAL;
				}
				return res;
			}

			jvxErrorType fromString(const jvx_propertyReferenceTriple& theTriple,
				jvxCallManagerProperties& callGate,
				const jvx::propertyDescriptor::CjvxPropertyDescriptorCore & theDescr,
				std::string loadTarget, std::string loadTargetPP,
				jvxSize offsetStart,
				jvxCBitField fineTuningParams)
			{
				// Number elements is INHERENT to passed string expression !!
				jvxErrorType res = JVX_NO_ERROR;
				jvxBool propAccessible = false;
				jvxHandle* ptrVal = NULL;
				jvxSelectionList selLst;
				jvxApiString strL;
				std::string strstr;
				jvxValueInRange valR;
				std::string txtToken;
				jvxBool txt_lst_found = false;
				std::vector<std::string> theTokens;
				std::string loadTarget_copy = loadTarget;
				jvxBool contentOnly = true;
				jvxSize selLst_ll = 0;

				jvxData valD = 0;
				jvxInt16 valI16 = 0;
				jvxInt8 valI8 = 0;
				jvxInt32 valI32 = 0;
				jvxInt64 valI64 = 0;

				jvxInt16 valUI16 = 0;
				jvxInt8 valUI8 = 0;
				jvxInt32 valUI32 = 0;
				jvxInt64 valUI64 = 0;

				jvxSize valS = 0;
				jvxCBool valB = false;
				jvxBool err = false;
				jvxSize numBytes = 0;
				jvxSize numValuesPassed = 0;

				jvxSize szSel = 0;
				jvxCallManagerProperties callGateLoc;
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident;
				jvx::propertyDetail::CjvxTranferDetail trans;

				numValuesPassed = theDescr.num;
				if (theDescr.num > 0)
				{
					if (theDescr.num > 1)
					{
						switch (theDescr.format)
						{
						case JVX_DATAFORMAT_SIZE:
						case JVX_DATAFORMAT_DATA:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_U16BIT_LE:
						case JVX_DATAFORMAT_U8BIT:
						case JVX_DATAFORMAT_U32BIT_LE:
						case JVX_DATAFORMAT_U64BIT_LE:
							numBytes = jvxDataFormat_size[theDescr.format] * theDescr.num;
							ptrVal = new jvxByte[numBytes];
							memset(ptrVal, 0, numBytes);
							res = jvx_binString2ValueList(loadTarget, loadTargetPP, ptrVal, theDescr.format, theDescr.num, &numBytes, &numValuesPassed);
							if (res != JVX_NO_ERROR)
							{
								res = jvx_string2ValueList(loadTarget, ptrVal, theDescr.format, theDescr.num, &numValuesPassed);
							}
							break;

						case JVX_DATAFORMAT_SELECTION_LIST:
						case JVX_DATAFORMAT_STRING:
						case JVX_DATAFORMAT_STRING_LIST:
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
						case JVX_DATAFORMAT_HANDLE:
						case JVX_DATAFORMAT_BYTE:
						case JVX_DATAFORMAT_LIMIT:
						case JVX_DATAFORMAT_NONE:
						case JVX_DATAFORMAT_POINTER:
							res = JVX_ERROR_UNSUPPORTED;
							break;
						default:
							assert(0);
						}
					}
					else
					{
						switch (theDescr.format)
						{
						case JVX_DATAFORMAT_SIZE:
							ptrVal = &valS;
							break;
						case JVX_DATAFORMAT_DATA:
							ptrVal = &valD;
							break;
						case JVX_DATAFORMAT_16BIT_LE:
							ptrVal = &valI16;
							break;
						case JVX_DATAFORMAT_8BIT:
							ptrVal = &valI8;
							break;
						case JVX_DATAFORMAT_32BIT_LE:
							ptrVal = &valI32;
							break;
						case JVX_DATAFORMAT_64BIT_LE:
							ptrVal = &valI64;
							break;

						case JVX_DATAFORMAT_U16BIT_LE:
							ptrVal = &valUI16;
							break;
						case JVX_DATAFORMAT_U8BIT:
							ptrVal = &valUI8;
							break;
						case JVX_DATAFORMAT_U32BIT_LE:
							ptrVal = &valUI32;
							break;
						case JVX_DATAFORMAT_U64BIT_LE:
							ptrVal = &valUI64;
							break;

						case JVX_DATAFORMAT_SELECTION_LIST:
							ptrVal = &selLst;

							// Get current value from selection
							callGateLoc = callGate;
							callGateLoc.call_purpose = JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY;
							ident.reset(theDescr.globalIdx, theDescr.category);
							trans.reset(true);
							theTriple.theProps->get_property(
								callGateLoc, jPRG(&szSel, 
								1, JVX_DATAFORMAT_SIZE),
								ident, trans);
							break;
						case JVX_DATAFORMAT_STRING:
							ptrVal = &strL;
							break;
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
							ptrVal = &valR;
							break;
						case JVX_DATAFORMAT_HANDLE:
						case JVX_DATAFORMAT_BYTE:
						case JVX_DATAFORMAT_LIMIT:
						case JVX_DATAFORMAT_NONE:
						case JVX_DATAFORMAT_POINTER:
						case JVX_DATAFORMAT_STRING_LIST:
							res = JVX_ERROR_UNSUPPORTED;
							break;
						default:
							assert(0);
						}
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_SETTING;
				}

				if (res == JVX_NO_ERROR)
				{
					// Fill in field from string, temp field always starts with first element
					res = fromString(ptrVal, 0, numValuesPassed, theDescr.format, contentOnly, theDescr.decTp, loadTarget, loadTargetPP, fineTuningParams, szSel);
				}

				if (res == JVX_NO_ERROR)
				{
					//res = jvx_set_property(theTriple.theProps, ptrVal, 0, theDescr.num, theDescr.format, false, theDescr.descriptor.c_str());
					jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(theDescr.globalIdx, theDescr.category);
					jvx::propertyDetail::CjvxTranferDetail trans(contentOnly, offsetStart);

					res = theTriple.theProps->set_property(callGate, jPRG(ptrVal, numValuesPassed, theDescr.format), ident, trans);
				}

				if (theDescr.num > 1)
				{
					switch (theDescr.format)
					{
					case JVX_DATAFORMAT_SIZE:
					case JVX_DATAFORMAT_DATA:
					case JVX_DATAFORMAT_16BIT_LE:
					case JVX_DATAFORMAT_8BIT:
					case JVX_DATAFORMAT_32BIT_LE:
					case JVX_DATAFORMAT_64BIT_LE:
					case JVX_DATAFORMAT_U16BIT_LE:
					case JVX_DATAFORMAT_U8BIT:
					case JVX_DATAFORMAT_U32BIT_LE:
					case JVX_DATAFORMAT_U64BIT_LE:
						delete[]((jvxByte*)ptrVal);
						ptrVal = NULL;
						break;
					}
				}
				return res;
			}

			jvxErrorType fromString(IjvxAccessProperties* propRef,
				jvxCallManagerProperties& callGate,
				const jvx::propertyDescriptor::CjvxPropertyDescriptorCore& theDescr,
				std::string loadTarget, std::string loadTargetPP,
				jvxSize offsetStart,
				jvxCBitField fineTuningParams)
			{
				// Number elements is INHERENT to passed string expression !!
				jvxErrorType res = JVX_NO_ERROR;
				jvxBool propAccessible = false;
				jvxHandle* ptrVal = NULL;
				jvxSelectionList selLst;
				jvxApiString strL;
				std::string strstr;
				jvxValueInRange valR;
				std::string txtToken;
				jvxBool txt_lst_found = false;
				std::vector<std::string> theTokens;
				std::string loadTarget_copy = loadTarget;
				jvxBool contentOnly = true;
				jvxSize selLst_ll = 0;

				jvxData valD = 0;
				jvxInt16 valI16 = 0;
				jvxInt8 valI8 = 0;
				jvxInt32 valI32 = 0;
				jvxInt64 valI64 = 0;

				jvxInt16 valUI16 = 0;
				jvxInt8 valUI8 = 0;
				jvxInt32 valUI32 = 0;
				jvxInt64 valUI64 = 0;

				jvxSize valS = 0;
				jvxCBool valB = false;
				jvxBool err = false;
				jvxSize numBytes = 0;
				jvxSize numValuesPassed = 0;

				jvxSize szSel = 0;
				jvxCallManagerProperties callGateLoc;
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident;
				jvx::propertyDetail::CjvxTranferDetail trans;

				numValuesPassed = theDescr.num;
				if (theDescr.num > 0)
				{
					if (theDescr.num > 1)
					{
						switch (theDescr.format)
						{
						case JVX_DATAFORMAT_SIZE:
						case JVX_DATAFORMAT_DATA:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_U16BIT_LE:
						case JVX_DATAFORMAT_U8BIT:
						case JVX_DATAFORMAT_U32BIT_LE:
						case JVX_DATAFORMAT_U64BIT_LE:
							numBytes = jvxDataFormat_size[theDescr.format] * theDescr.num;
							ptrVal = new jvxByte[numBytes];
							memset(ptrVal, 0, numBytes);

							// Is this required? We will do this in the dfollowing call to "fromString"
							/*
							res = jvx_binString2ValueList(loadTarget, loadTargetPP, ptrVal, theDescr.format, theDescr.num, &numBytes, &numValuesPassed);
							if (res != JVX_NO_ERROR)
							{
								res = jvx_string2ValueList(loadTarget, ptrVal, theDescr.format, theDescr.num, &numValuesPassed);
							}
							*/
							break;

						case JVX_DATAFORMAT_SELECTION_LIST:
						case JVX_DATAFORMAT_STRING:
						case JVX_DATAFORMAT_STRING_LIST:
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
						case JVX_DATAFORMAT_HANDLE:
						case JVX_DATAFORMAT_BYTE:
						case JVX_DATAFORMAT_LIMIT:
						case JVX_DATAFORMAT_NONE:
						case JVX_DATAFORMAT_POINTER:
							res = JVX_ERROR_UNSUPPORTED;
							break;
						default:
							assert(0);
						}
					}
					else
					{
						switch (theDescr.format)
						{
						case JVX_DATAFORMAT_SIZE:
							ptrVal = &valS;
							break;
						case JVX_DATAFORMAT_DATA:
							ptrVal = &valD;
							break;
						case JVX_DATAFORMAT_16BIT_LE:
							ptrVal = &valI16;
							break;
						case JVX_DATAFORMAT_8BIT:
							ptrVal = &valI8;
							break;
						case JVX_DATAFORMAT_32BIT_LE:
							ptrVal = &valI32;
							break;
						case JVX_DATAFORMAT_64BIT_LE:
							ptrVal = &valI64;
							break;

						case JVX_DATAFORMAT_U16BIT_LE:
							ptrVal = &valUI16;
							break;
						case JVX_DATAFORMAT_U8BIT:
							ptrVal = &valUI8;
							break;
						case JVX_DATAFORMAT_U32BIT_LE:
							ptrVal = &valUI32;
							break;
						case JVX_DATAFORMAT_U64BIT_LE:
							ptrVal = &valUI64;
							break;

						case JVX_DATAFORMAT_SELECTION_LIST:
							ptrVal = &selLst;

							// Get current value from selection
							callGateLoc = callGate;
							callGateLoc.call_purpose = JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY;
							ident.reset(theDescr.globalIdx, theDescr.category);
							trans.reset(true);
							propRef->get_property(
								callGateLoc, jPRG(&szSel,
									1, JVX_DATAFORMAT_SIZE),
								ident, trans);
							break;
						case JVX_DATAFORMAT_STRING:
							ptrVal = &strL;
							break;
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
							ptrVal = &valR;
							break;
						case JVX_DATAFORMAT_HANDLE:
						case JVX_DATAFORMAT_BYTE:
						case JVX_DATAFORMAT_LIMIT:
						case JVX_DATAFORMAT_NONE:
						case JVX_DATAFORMAT_POINTER:
						case JVX_DATAFORMAT_STRING_LIST:
							res = JVX_ERROR_UNSUPPORTED;
							break;
						default:
							assert(0);
						}
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_SETTING;
				}

				if (res == JVX_NO_ERROR)
				{
					// Fill in field from string, temp field always starts with first element
					res = fromString(ptrVal, 0, numValuesPassed, theDescr.format, contentOnly, theDescr.decTp, loadTarget, loadTargetPP, fineTuningParams, szSel);
				}

				if (res == JVX_NO_ERROR)
				{
					//res = jvx_set_property(theTriple.theProps, ptrVal, 0, theDescr.num, theDescr.format, false, theDescr.descriptor.c_str());
					jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(theDescr.globalIdx, theDescr.category);
					jvx::propertyDetail::CjvxTranferDetail trans(contentOnly, offsetStart);

					res = propRef->set_property(callGate, jPRG(ptrVal, numValuesPassed, theDescr.format), ident, trans);
				}

				if (theDescr.num > 1)
				{
					switch (theDescr.format)
					{
					case JVX_DATAFORMAT_SIZE:
					case JVX_DATAFORMAT_DATA:
					case JVX_DATAFORMAT_16BIT_LE:
					case JVX_DATAFORMAT_8BIT:
					case JVX_DATAFORMAT_32BIT_LE:
					case JVX_DATAFORMAT_64BIT_LE:
					case JVX_DATAFORMAT_U16BIT_LE:
					case JVX_DATAFORMAT_U8BIT:
					case JVX_DATAFORMAT_U32BIT_LE:
					case JVX_DATAFORMAT_U64BIT_LE:
						delete[]((jvxByte*)ptrVal);
						ptrVal = NULL;
						break;
					}
				}
				return res;
			}

			// ======================================================================================================
			std::string jvx_selList2String(jvxSelectionList* selLst, std::string& retValPostProcess, jvxBool contentOnly, jvxCBitField fineTuningParams)
			{
				std::string retVal;
				
				jvxSize i;
				if (jvx_bitTest(fineTuningParams, JVX_STRING_PROPERTY_FINE_TUNING_SELECTIONLIST_SELID_SHIFT))
				{
					retVal += jvx_size2String(jvx_bitfieldSelection2Id(selLst->bitFieldSelected(), selLst->strList.ll()));
				}
				else
				{
					if (selLst->num == 1)
					{
						retVal += jvx_bitField2String(selLst->bitFieldSelected());
					}
					else
					{
						std::string oneFld;
						for (i = 0; i < selLst->num; i++)
						{
							std::string oneToken = jvx_bitField2String(selLst->bitFieldSelected(i));

							if (!oneFld.empty())
							{
								oneFld += ", ";
							}
							oneFld += oneToken;
						}
						retVal += "[" + oneFld + "]";
					}
					retVal += "::";
					retVal += jvx_bitField2String(selLst->bitFieldSelectable);
					retVal += "::";
					retVal += jvx_bitField2String(selLst->bitFieldExclusive);
					retVal += "::";
					retVal += jvx_size2String(selLst->strList.ll());

					if (!contentOnly)
					{
						for (i = 0; i < selLst->strList.ll(); i++)
						{
							if (i > 0)
							{
								retValPostProcess += ",";
							}
							retValPostProcess += selLst->strList.std_str_at(i);
						}
					}
				}
				return retVal;
			}

			std::string jvx_valRange2String(jvxValueInRange* valR, jvxBool contentOnly)
			{
				std::string retVal;
				jvxSize i;
				if (valR->num == 1)
				{
					retVal = jvx_data2String(valR->val(), 4);
				}
				else
				{
					std::string oneFld;
					for (i = 0; i < valR->num; i++)
					{
						std::string oneToken = jvx_data2String(valR->val(i), 4);

						if (!oneFld.empty())
						{
							oneFld += ", ";
						}
						oneFld += oneToken;
					}
					retVal += "[" + oneFld + "]";
				}
				if (!contentOnly)
				{
					retVal += "::";
					retVal += jvx_data2String(valR->minVal, 4);
					retVal += "::";
					retVal += jvx_data2String(valR->maxVal, 4);
				}
				return retVal;
			}

			jvxErrorType toString(jvxHandle* fld, jvxSize offsetStart, jvxSize numElements, jvxDataFormat format,
				jvxBool contentOnly, jvxPropertyDecoderHintType decTp, std::string& retVal, std::string& retValPostProcess,
				jvxSize numDigits, jvxBool binList,
				jvxContext* ctxt, jvxCBitField fineTuningParams)
			{
				jvxErrorType res = JVX_NO_ERROR;
				jvxBool propAccessible = false;
				jvxSelectionList* selLst = NULL;
				jvxApiStringList* strLst = NULL;
				jvxApiString* strL = NULL;
				jvxValueInRange* valR = NULL;
				jvxBool isValid = true;
				jvxSize i;

				jvxData valD = 0;
				jvxInt16 valI16 = 0;
				jvxInt8 valI8 = 0;
				jvxInt32 valI32 = 0;
				jvxInt64 valI64 = 0;
				jvxInt16 valUI16 = 0;
				jvxInt8 valUI8 = 0;
				jvxInt32 valUI32 = 0;
				jvxInt64 valUI64 = 0;
				jvxSize valS = 0;
				jvxCBool valB = false;

				if (fld == NULL)
				{
					return JVX_ERROR_INVALID_ARGUMENT;
				}
				// Currently, only single elements are supported
				if (numElements > 0)
				{
					if (numElements > 1)
					{
						switch (format)
						{
						case JVX_DATAFORMAT_SIZE:
						case JVX_DATAFORMAT_DATA:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_U16BIT_LE:
						case JVX_DATAFORMAT_U8BIT:
						case JVX_DATAFORMAT_U32BIT_LE:
						case JVX_DATAFORMAT_U64BIT_LE:
							if (binList)
							{
								retVal = jvx_valueList2BinString(fld, format, numElements, retValPostProcess); // base64
								/*
								// Test it!
								jvxHandle* ptr = nullptr;
								jvxDataFormat form = JVX_DATAFORMAT_NONE;
								jvxSize num = 0;
								jvx_binString2ValueListAlloc(retVal, &ptr, &num, &form);
								jvx_binString2ValueListDeallocate(ptr);
								*/
							}
							else
							{
								retVal = jvx_valueList2String(fld, format, numElements, numDigits); // text string, important to not use a , separator
							}
							break;
						case JVX_DATAFORMAT_SELECTION_LIST:
							retVal = "";
							retValPostProcess =  "";
							selLst = (jvxSelectionList*)fld;
							retVal = jvx_selList2String(selLst, retValPostProcess, contentOnly, fineTuningParams);
							break;
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
							retVal = "";
							valR = (jvxValueInRange*)fld;
							retVal = jvx_valRange2String(valR, contentOnly);
							break;
						default:
							assert(0);
						}
					}
					else
					{
						switch (format)
						{
						case JVX_DATAFORMAT_SIZE:
							valS = *((jvxSize*)fld);
							if (JVX_CHECK_SIZE_SELECTED(valS))
							{
								retVal = jvx_size2String(valS);
							}
							else
							{
								retVal = "unselected";
							}
							break;

						case JVX_DATAFORMAT_DATA:

							valD = *((jvxData*)fld);
							retVal = jvx_data2String(valD);
							break;
						case JVX_DATAFORMAT_16BIT_LE:
							valI16 = *((jvxInt16*)fld);
							if ((decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF) ||
								(decTp == JVX_PROPERTY_DECODER_SINGLE_SELECTION))
							{
								retVal = "yes";
								if (valI16 == c_false)
								{
									retVal = "no";
								}
							}
							else if (decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
							{
								retVal = jvxDataFormat_txt(valI16);
							}
							else if (decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
							{
								if (valI16 < 0)
								{
									retVal = "dontcare";
								}
								else
								{
									retVal = jvx_int2String(valI16);
								}
							}
							else
							{
								retVal = jvx_int2String(valI16);
							}
							break;
						case JVX_DATAFORMAT_8BIT:
							valI8 = *((jvxInt8*)fld);
							retVal = jvx_int2String(valI8);
							break;
						case JVX_DATAFORMAT_32BIT_LE:
							valI32 = *((jvxInt32*)fld);
							if (decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
							{
								if (valI32 < 0)
								{
									retVal = "dontcare";
								}
								else
								{
									retVal = jvx_int2String(valI32);
								}
							}
							else
							{
								retVal = jvx_int2String(valI32);
							}
							break;
						case JVX_DATAFORMAT_64BIT_LE:
							valI64 = *((jvxInt64*)fld);
							retVal = jvx_int642String(valI64);
							break;

						case JVX_DATAFORMAT_U16BIT_LE:
							valUI16 = *((jvxUInt16*)fld);
							if (decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
							{
								retVal = "yes";
								if (valUI16 == c_false)
								{
									retVal = "no";
								}
							}
							else if (decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
							{
								retVal = jvxDataFormat_txt(valUI16);
							}
							else if (decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
							{
								if (valUI16 < 0)
								{
									retVal = "dontcare";
								}
								else
								{
									retVal = jvx_int2String(valUI16);
								}
							}
							else
							{
								retVal = jvx_int2String(valUI16);
							}
							break;
						case JVX_DATAFORMAT_U8BIT:
							valUI8 = *((jvxUInt8*)fld);
							retVal = jvx_int2String(valUI8);
							break;
						case JVX_DATAFORMAT_U32BIT_LE:
							valUI32 = *((jvxUInt32*)fld);
							if (decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
							{
								if (valUI32 < 0)
								{
									retVal = "dontcare";
								}
								else
								{
									retVal = jvx_int2String(valUI32);
								}
							}
							else
							{
								retVal = jvx_int2String(valUI32);
							}
							break;
						case JVX_DATAFORMAT_U64BIT_LE:
							valUI64 = *((jvxUInt64*)fld);
							retVal = jvx_int642String(valUI64);
							break;


						case JVX_DATAFORMAT_SELECTION_LIST:
							retVal = ""; 
							retValPostProcess = "";
							selLst = (jvxSelectionList*)fld;
							assert(selLst);

							retVal = jvx_selList2String(selLst, retValPostProcess, contentOnly, fineTuningParams);
							
							break;
						case JVX_DATAFORMAT_STRING:
							retVal = "-none-error-";
							strL = (jvxApiString*)fld;
							assert(strL);
							strL->assert_valid();
							retVal = strL->std_str();
							break;
						case JVX_DATAFORMAT_STRING_LIST:
							retVal = "";
							retValPostProcess = "";
							strLst = (jvxApiStringList*)fld;
							assert(strLst);
							for (i = 0; i < strLst->ll(); i++)
							{
								if (i > 0)
								{
									retValPostProcess += ",";
								}
								retValPostProcess += strLst->std_str_at(i);
							}

							break;
						case JVX_DATAFORMAT_VALUE_IN_RANGE:

							valR = (jvxValueInRange*)fld;
							retVal = jvx_valRange2String(valR, contentOnly);
							break;
						default:
							assert(0);
						}// switch
					}
				}
				else
				{
					res = JVX_ERROR_INTERNAL;
				}
				return res;
			}

			jvxErrorType toString(const jvx_propertyReferenceTriple& theTriple, jvxCallManagerProperties& callGate,
				const jvx::propertyDescriptor::CjvxPropertyDescriptorCore& theDescr, std::string& retVal, std::string& retValPostProcess,
				jvxSize numDigits, jvxBool binList, jvxSize offStart, jvxSize numElms, jvxBool contentOnly, jvxCBitField fineTuningParams)
			{
				jvxErrorType res = JVX_NO_ERROR;
				jvxBool propAccessible = false;
				jvxHandle* ptrVal = NULL;
				jvxSelectionList selLst;
				jvxApiString strL(0);
				jvxValueInRange valR(0);

				jvxData valD = 0;
				jvxInt16 valI16 = 0;
				jvxInt8 valI8 = 0;
				jvxInt32 valI32 = 0;
				jvxInt64 valI64 = 0;
				jvxInt16 valUI16 = 0;
				jvxInt8 valUI8 = 0;
				jvxInt32 valUI32 = 0;
				jvxInt64 valUI64 = 0;
				jvxSize valS = 0;
				jvxCBool valB = false;

				jvxBool isValid = true;
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident;
				jvx::propertyDetail::CjvxTranferDetail trans;

				if (JVX_CHECK_SIZE_UNSELECTED(numElms))
				{
					numElms = theDescr.num;
				}

				if (offStart >= theDescr.num)
				{
					std::cout << __FUNCTION__ << ": specified offset is outside the array." << std::endl;
					res = JVX_ERROR_INVALID_SETTING;
					goto exit_fail;
				}

				// Currently, only single elements are supported
				if (theDescr.num == 0)
				{
					std::cout << __FUNCTION__ << ": property has zero elements." << std::endl;
					res = JVX_ERROR_NOT_READY;
					goto exit_fail;
				}

				if (theDescr.num > 1)
				{
					// If an offset is specified we need to decrease the number of entries to read
					numElms = JVX_MIN(numElms, theDescr.num - offStart);

					switch (theDescr.format)
					{
					case JVX_DATAFORMAT_SIZE:
					case JVX_DATAFORMAT_DATA:
					case JVX_DATAFORMAT_16BIT_LE:
					case JVX_DATAFORMAT_8BIT:
					case JVX_DATAFORMAT_32BIT_LE:
					case JVX_DATAFORMAT_64BIT_LE:
					case JVX_DATAFORMAT_U16BIT_LE:
					case JVX_DATAFORMAT_U8BIT:
					case JVX_DATAFORMAT_U32BIT_LE:
					case JVX_DATAFORMAT_U64BIT_LE:
						ptrVal = new jvxByte[jvxDataFormat_size[theDescr.format] * numElms];
						memset(ptrVal, 0, jvxDataFormat_size[theDescr.format] * numElms);
						break;
					case JVX_DATAFORMAT_SELECTION_LIST:
						selLst.resize(numElms);
						ptrVal = &selLst;
						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:
						valR.resize(numElms);
						ptrVal = &valR;
						break;
					case JVX_DATAFORMAT_STRING:
					case JVX_DATAFORMAT_STRING_LIST:
					case JVX_DATAFORMAT_HANDLE:
					case JVX_DATAFORMAT_BYTE:
					case JVX_DATAFORMAT_LIMIT:
					case JVX_DATAFORMAT_NONE:
					case JVX_DATAFORMAT_POINTER:
						res = JVX_ERROR_UNSUPPORTED;
						goto exit_fail;
						break;
					default:
						assert(0);
					}
				}
				else
				{
					switch (theDescr.format)
					{
					case JVX_DATAFORMAT_SIZE:
						ptrVal = &valS;
						break;
					case JVX_DATAFORMAT_DATA:
						ptrVal = &valD;
						break;
					case JVX_DATAFORMAT_16BIT_LE:
						ptrVal = &valI16;
						break;
					case JVX_DATAFORMAT_8BIT:
						ptrVal = &valI8;
						break;
					case JVX_DATAFORMAT_32BIT_LE:
						ptrVal = &valI32;
						break;
					case JVX_DATAFORMAT_64BIT_LE:
						ptrVal = &valI64;
						break;
					case JVX_DATAFORMAT_U16BIT_LE:
						ptrVal = &valUI16;
						break;
					case JVX_DATAFORMAT_U8BIT:
						ptrVal = &valUI8;
						break;
					case JVX_DATAFORMAT_U32BIT_LE:
						ptrVal = &valUI32;
						break;
					case JVX_DATAFORMAT_U64BIT_LE:
						ptrVal = &valUI64;
						break;
					case JVX_DATAFORMAT_SELECTION_LIST:
						selLst.resize(1);
						ptrVal = &selLst;
						break;
					case JVX_DATAFORMAT_STRING:
						ptrVal = &strL;
						break;
					case JVX_DATAFORMAT_STRING_LIST:
						ptrVal = &selLst.strList;
						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:
						valR.resize(1);
						ptrVal = &valR;
						break;
					case JVX_DATAFORMAT_HANDLE:
					case JVX_DATAFORMAT_BYTE:
					case JVX_DATAFORMAT_LIMIT:
					case JVX_DATAFORMAT_NONE:
					case JVX_DATAFORMAT_POINTER:
					case JVX_DATAFORMAT_INTERFACE:
						res = JVX_ERROR_UNSUPPORTED;
						break;
					default:
						assert(0);
					}
				}

				if (res == JVX_NO_ERROR)
				{
					//res = jvx_get_property(theTriple.theProps, ptrVal, 0, theDescr.num, theDescr.format, false, theDescr.descriptor.c_str());
					// Here, we have the offset
					ident.reset(theDescr.globalIdx, theDescr.category);
					trans.reset(contentOnly, offStart);
					res = theTriple.theProps->get_property(callGate, jPRG(
						ptrVal, numElms, theDescr.format),
						ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(theDescr.globalIdx, theDescr.category),
						theTriple.theProps))
					{
						// Here, we have no more offset since the filled field always starts from zero!!!
						res = toString(ptrVal, 0, numElms, theDescr.format, contentOnly, theDescr.decTp, retVal, retValPostProcess, numDigits, binList, callGate.context,
							fineTuningParams);
					}

					if (theDescr.num > 1)
					{
						switch (theDescr.format)
						{
						case JVX_DATAFORMAT_SIZE:
						case JVX_DATAFORMAT_DATA:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_U16BIT_LE:
						case JVX_DATAFORMAT_U8BIT:
						case JVX_DATAFORMAT_U32BIT_LE:
						case JVX_DATAFORMAT_U64BIT_LE:
							delete[]((jvxByte*)ptrVal);
							ptrVal = NULL;
							break;
						}
					}
				}

			exit_fail:
				return res;
			}

			jvxErrorType toString(IjvxAccessProperties* propRef, jvxCallManagerProperties& callGate,
				const jvxPropertyDescriptor& theDescr, std::string& retVal, std::string& retValPostProcess,
				jvxSize numDigits, jvxBool binList, jvxSize offStart, jvxSize numElms, jvxBool contentOnly, jvxCBitField fineTuningParams)
			{
				jvxErrorType res = JVX_NO_ERROR;
				jvxBool propAccessible = false;
				jvxHandle* ptrVal = NULL;
				jvxSelectionList selLst;
				jvxApiString strL;
				jvxValueInRange valR;

				jvxData valD = 0;
				jvxInt16 valI16 = 0;
				jvxInt8 valI8 = 0;
				jvxInt32 valI32 = 0;
				jvxInt64 valI64 = 0;
				jvxInt16 valUI16 = 0;
				jvxInt8 valUI8 = 0;
				jvxInt32 valUI32 = 0;
				jvxInt64 valUI64 = 0;
				jvxSize valS = 0;
				jvxCBool valB = false;

				jvxBool isValid = true;

				if (JVX_CHECK_SIZE_UNSELECTED(numElms))
				{
					numElms = theDescr.num;
				}

				if (offStart >= theDescr.num)
				{
					if (theDescr.num > 0)
					{
						std::cout << __FUNCTION__ << ": specified offset is outside the array." << std::endl;
						res = JVX_ERROR_INVALID_SETTING;
						goto exit_fail;
					}
				}

				if (theDescr.num > 1)
				{
					// If an offset is specified we need to decrease the number of entries to read
					numElms = JVX_MIN(numElms, theDescr.num - offStart);

					switch (theDescr.format)
					{
					case JVX_DATAFORMAT_SIZE:
					case JVX_DATAFORMAT_DATA:
					case JVX_DATAFORMAT_16BIT_LE:
					case JVX_DATAFORMAT_8BIT:
					case JVX_DATAFORMAT_32BIT_LE:
					case JVX_DATAFORMAT_64BIT_LE:
					case JVX_DATAFORMAT_U16BIT_LE:
					case JVX_DATAFORMAT_U8BIT:
					case JVX_DATAFORMAT_U32BIT_LE:
					case JVX_DATAFORMAT_U64BIT_LE:
						if (numElms > 0)
						{
							ptrVal = new jvxByte[jvxDataFormat_size[theDescr.format] * numElms];
							memset(ptrVal, 0, jvxDataFormat_size[theDescr.format] * numElms);
						}
						break;

					case JVX_DATAFORMAT_SELECTION_LIST:
					case JVX_DATAFORMAT_STRING:
					case JVX_DATAFORMAT_STRING_LIST:
					case JVX_DATAFORMAT_VALUE_IN_RANGE:
					case JVX_DATAFORMAT_HANDLE:
					case JVX_DATAFORMAT_BYTE:
					case JVX_DATAFORMAT_LIMIT:
					case JVX_DATAFORMAT_NONE:
					case JVX_DATAFORMAT_POINTER:
						res = JVX_ERROR_UNSUPPORTED;
						break;
					default:
						assert(0);
					}
				}
				else
				{
					switch (theDescr.format)
					{
					case JVX_DATAFORMAT_SIZE:
						ptrVal = &valS;
						break;
					case JVX_DATAFORMAT_DATA:
						ptrVal = &valD;
						break;
					case JVX_DATAFORMAT_16BIT_LE:
						ptrVal = &valI16;
						break;
					case JVX_DATAFORMAT_8BIT:
						ptrVal = &valI8;
						break;
					case JVX_DATAFORMAT_32BIT_LE:
						ptrVal = &valI32;
						break;
					case JVX_DATAFORMAT_64BIT_LE:
						ptrVal = &valI64;
						break;
					case JVX_DATAFORMAT_U16BIT_LE:
						ptrVal = &valUI16;
						break;
					case JVX_DATAFORMAT_U8BIT:
						ptrVal = &valUI8;
						break;
					case JVX_DATAFORMAT_U32BIT_LE:
						ptrVal = &valUI32;
						break;
					case JVX_DATAFORMAT_U64BIT_LE:
						ptrVal = &valUI64;
						break;
					case JVX_DATAFORMAT_SELECTION_LIST:
						ptrVal = &selLst;
						break;
					case JVX_DATAFORMAT_STRING:
						ptrVal = &strL;
						break;
					case JVX_DATAFORMAT_STRING_LIST:
						ptrVal = &selLst.strList;
						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:
						ptrVal = &valR;
						break;
					case JVX_DATAFORMAT_HANDLE:
					case JVX_DATAFORMAT_BYTE:
					case JVX_DATAFORMAT_LIMIT:
					case JVX_DATAFORMAT_NONE:
					case JVX_DATAFORMAT_POINTER:
					case JVX_DATAFORMAT_INTERFACE:
						res = JVX_ERROR_UNSUPPORTED;
						break;
					default:
						assert(0);
					}
				}

				//res = jvx_get_property(theTriple.theProps, ptrVal, 0, theDescr.num, theDescr.format, false, theDescr.descriptor.c_str());

				// Here, we have the offset
				res = propRef->get_property(callGate, jPRG( ptrVal, numElms, theDescr.format), 
					jPAGID(theDescr.globalIdx, theDescr.category), jPD(contentOnly, offStart));
				if (res == JVX_NO_ERROR)
				{
					// Here, we have no more offset since the filled field always starts from zero!!!
					res = toString(ptrVal, 0, numElms, theDescr.format, contentOnly, theDescr.decTp, retVal, retValPostProcess, numDigits, binList, callGate.context,
						fineTuningParams);
				}

				if (theDescr.num > 1)
				{
					switch (theDescr.format)
					{
					case JVX_DATAFORMAT_SIZE:
					case JVX_DATAFORMAT_DATA:
					case JVX_DATAFORMAT_16BIT_LE:
					case JVX_DATAFORMAT_8BIT:
					case JVX_DATAFORMAT_32BIT_LE:
					case JVX_DATAFORMAT_64BIT_LE:
					case JVX_DATAFORMAT_U16BIT_LE:
					case JVX_DATAFORMAT_U8BIT:
					case JVX_DATAFORMAT_U32BIT_LE:
					case JVX_DATAFORMAT_U64BIT_LE:
						if (ptrVal)
						{
							delete[]((jvxByte*)ptrVal);
							ptrVal = NULL;
						}
						break;
					}
				}

			exit_fail:
				return res;
			}

			void numSubEntriesSingleObjectField(jvxSize& numSubElements, jvxDataFormat format, jvxHandle* fld)
			{
				jvxValueInRange* valRg = nullptr;
				jvxSelectionList* selLst = nullptr;
				switch (format)
				{
				case JVX_DATAFORMAT_VALUE_IN_RANGE:

					valRg = (jvxValueInRange*)fld;
					if (valRg)
					{
						numSubElements = valRg->num;
					}
					break;
				case JVX_DATAFORMAT_SELECTION_LIST:
					selLst = (jvxSelectionList*)fld;
					if (selLst)
					{
						numSubElements = selLst->num;
					}
					break;
				}
			}	
		} // namespace properties
	} // namespace helpers
} // namespace jvx

