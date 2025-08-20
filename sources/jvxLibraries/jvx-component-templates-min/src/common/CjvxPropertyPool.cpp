#include "common/CjvxPropertyPool.h"

CjvxPropertyPool::CjvxPropertyPool()
{
}

CjvxPropertyPool::~CjvxPropertyPool()
{
}

// ======================================================================================

jvxErrorType
CjvxPropertyPool::_number_system_properties(jvxSize* num_properties)
{
	if (num_properties)
	{
		*num_properties = pool_propids.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPropertyPool::_system_property_id(jvxSize id, jvxSize* id_prop, jvxApiString* astr)
{
	auto elm = pool_propids.begin();
	if (id < pool_propids.size())
	{
		std::advance(elm, id);
		assert(elm != pool_propids.end());

		if (id_prop)
		{
			*id_prop = elm->first;
		}
		if (astr)
		{
			astr->assign(elm->second.description);
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType 
CjvxPropertyPool::_number_entries_pool_property(jvxSize id_prop, jvxSize* num_pool_associations)
{
	auto elm = pool_propids.find(id_prop);
	if (elm != pool_propids.end())
	{
		if (num_pool_associations)
		{
			*num_pool_associations = elm->second.pool.size();
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}
	
jvxErrorType 
CjvxPropertyPool::_entry_pool_property(jvxSize id_prop, jvxSize id_prop_entry, jvxComponentIdentification* tp, jvxBool* isMaster)
{
	auto elm = pool_propids.find(id_prop);
	if (elm != pool_propids.end())
	{
		if (id_prop_entry < elm->second.pool.size())
		{
			auto eelm = elm->second.pool.begin();
			std::advance(eelm, id_prop_entry);
			assert(eelm != elm->second.pool.end());
			if (tp)
			{
				*tp = eelm->tp;
			}
			if (isMaster)
			{
				*isMaster = eelm->isMaster;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

// ======================================================================================

jvxErrorType
CjvxPropertyPool::_add_system_property(jvxSize id_prop, jvxDataFormat form, const char* descr)
{
	auto elm = pool_propids.find(id_prop);
	if (elm == pool_propids.end())
	{
		onePropertyPoolEntry newElm;
		newElm.propid = id_prop;
		newElm.description = descr;
		newElm.format = form;
		pool_propids[newElm.propid] = newElm;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType
CjvxPropertyPool::_add_entry_system_property(jvxSize id_prop, jvxComponentIdentification tp, jvxBool isMaster)
{
	auto elm = pool_propids.find(id_prop);
	if (elm != pool_propids.end())
	{
		onePropertyPoolMember newElms;
		newElms.tp = tp;
		newElms.detectLoop = false;
		newElms.isMaster = isMaster;
		elm->second.pool.push_back(newElms);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;		
}

jvxErrorType
CjvxPropertyPool::_clear_entries_system_property(jvxSize id_prop)
{
	auto elm = pool_propids.find(id_prop);
	if (elm != pool_propids.end())
	{
		elm->second.pool.clear();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxPropertyPool::_remove_system_property(jvxSize id_prop)
{
	auto elm = pool_propids.find(id_prop);
	if (elm != pool_propids.end())
	{
		pool_propids.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxBool jvx_componentMatch(jvxComponentIdentification tpOrigin, jvxComponentIdentification tpMatchThis, jvxBool hasParent)
{
	jvxBool matches = false;
	if(
		(tpOrigin.tp == tpMatchThis.tp) || 
		(tpMatchThis.tp == JVX_COMPONENT_UNKNOWN))
	{
		matches = true;
	}

	if (matches && hasParent)
	{
		matches = false;
		if (
			(tpOrigin.slotsubid == tpMatchThis.slotsubid) ||
			(tpMatchThis.slotsubid == JVX_SIZE_DONTCARE))
		{
			matches = true;
		}
	}

	if (matches)
	{
		matches = false;
		if (
			(tpOrigin.slotid == tpMatchThis.slotid) ||
			(tpMatchThis.slotid == JVX_SIZE_DONTCARE))
		{
			matches = true;
		}
	}
	return matches;
}

// ====================================================================================
jvxErrorType
CjvxPropertyPool::_report_property_changed(
	IjvxHost* host, jvxSize propId, 
	jvxPropertyCategoryType category, jvxBool contentOnly, 
	jvxSize offsetStart,
	jvxSize numElements, 
	jvxComponentIdentification tpOrigin)
{
	jvxSize szSlots = 0;
	jvxComponentType parentTp = JVX_COMPONENT_UNKNOWN;
	jvxComponentType childTp = JVX_COMPONENT_UNKNOWN;

	jvxSize j,k;

	jvxSelectionList selLst;
	jvxInt16 valI16;
	jvxInt32 valI32;
	jvxInt8 valI8;
	jvxInt64 valI64;
	jvxData valData;
	jvxApiString valStr;
	jvxApiStringList valStrLst;

	jvxHandle* fld = NULL;
	
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	jvxFlagTag accessFlags = 0;

	if (category == JVX_PROPERTY_CATEGORY_PREDEFINED)
	{
		auto elm = pool_propids.find(propId);
		if (elm != pool_propids.end())
		{
			jvxBool isMaster = false;
			auto eelmp = elm->second.pool.begin();
			for (; eelmp != elm->second.pool.end(); eelmp++)
			{
				jvxBool matches = jvx_componentMatch(tpOrigin, eelmp->tp, (parentTp != JVX_COMPONENT_UNKNOWN));
				if (matches)
				{
					if (eelmp->isMaster)
					{
						isMaster = true;
						break;
					}
				}
			}
			if (isMaster)
			{
				auto eelm = elm->second.pool.begin();
				for (; eelm != elm->second.pool.end(); eelm++)
				{
					host->role_component_system(eelm->tp.tp, &parentTp, &childTp, nullptr);
					// host->number_slots_component_system(eelm->tp, NULL, NULL, nullptr, nullptr);
					jvxBool matches = jvx_componentMatch(tpOrigin, eelm->tp, (parentTp != JVX_COMPONENT_UNKNOWN));
					if (matches)
					{
						// Found the entry, expand to all entries
						jvx_propertyReferenceTriple theTuple;
						jvx_initPropertyReferenceTriple(&theTuple);
						jvx_getReferencePropertiesObject(host, &theTuple, tpOrigin);
						if (jvx_isValidNoPropertyReferenceTriple(&theTuple))
						{
							switch (elm->second.format)
							{
							case JVX_DATAFORMAT_SELECTION_LIST:
								fld = &selLst;
								break;
							case JVX_DATAFORMAT_STRING:
								fld = &valStr;
								break;
							case JVX_DATAFORMAT_STRING_LIST:
								fld = &valStrLst;
								break;
							case JVX_DATAFORMAT_8BIT:
								fld = &valI8;
								break;
							case JVX_DATAFORMAT_16BIT_LE:
								fld = &valI16;
								break;
							case JVX_DATAFORMAT_32BIT_LE:
								fld = &valI32;
								break;
							case JVX_DATAFORMAT_64BIT_LE:
								fld = &valI64;
								break;
							case JVX_DATAFORMAT_DATA:
								fld = &valData;
								break;
							default:
								assert(0);
							}

							callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_TAKE_OVER;
							ident.reset(propId, category);
							trans.reset(contentOnly, offsetStart);
							jvxErrorType resSet = theTuple.theProps->get_property(
								callGate, jPRG(fld, numElements,
								elm->second.format), ident, trans);

							jvx_returnReferencePropertiesObject(host, &theTuple, tpOrigin);

							if (
								(resSet == JVX_NO_ERROR) && (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK))
							{
								auto eelmc = elm->second.pool.begin();
								for (; eelmc != elm->second.pool.end(); eelmc++)
								{
									jvxSize numSlots = 0;
									jvxSize numSubSlots = 0;

									host->role_component_system(eelmc->tp.tp, &parentTp, &childTp, nullptr);
									host->number_slots_component_system(eelmc->tp, &numSlots, nullptr, nullptr, nullptr);

									for (j = 0; j < numSlots; j++)
									{
										jvxComponentIdentification tpCheck;
										tpCheck.tp = eelmc->tp.tp;
										tpCheck.slotid = j;

										if (parentTp == JVX_COMPONENT_UNKNOWN)
										{
											numSubSlots = 1;
										}
										else
										{
											host->number_slots_component_system(tpCheck, NULL, &numSubSlots, nullptr, nullptr);
										}

										for (k = 0; k < numSubSlots; k++)
										{
											tpCheck.slotsubid = k;

											// make sure we do not have a self reference PROPERTY POOL call
											if (tpOrigin != tpCheck)
											{
												if (jvx_componentMatch(tpCheck, eelmc->tp, (parentTp != JVX_COMPONENT_UNKNOWN)))
												{
													jvx_initPropertyReferenceTriple(&theTuple);
													jvx_getReferencePropertiesObject(host, &theTuple, tpCheck);
													if (jvx_isValidPropertyReferenceTriple(&theTuple))
													{
														// Loop detection here!
														assert(eelmc->detectLoop == false);

														eelmc->detectLoop = true;
														callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_PROPERTY_POOL;
														ident.reset(propId, category);
														trans.reset(contentOnly, offsetStart);

														theTuple.theProps->set_property(callGate, 
															jPRG(fld, numElements, elm->second.format),
															ident, trans);
														jvx_returnReferencePropertiesObject(host, &theTuple, tpOrigin);
														eelmc->detectLoop = false;
													}
												}
											} // if (tpOrigin != tpCheck)
										}
									} // for (j = 0; j < numSlots; j++)
								} // for (; eelmc != elm->second.pool.end(); eelmc++)
								return JVX_NO_ERROR;
							} // if( (resSet == JVX_NO_ERROR) && (accProt == JVX_ACCESS_PROTOCOL_OK))
							return JVX_ERROR_INTERNAL;
						} // if (jvx_isValidNoPropertyReferenceTriple(&theTuple))
						return JVX_ERROR_UNSUPPORTED;
					} // if (matches)
				} // for (; eelm != elm->second.pool.end(); eelm++)
			}
		} // if (elm != pool_propids.end())
		return JVX_ERROR_ELEMENT_NOT_FOUND;
		} // if (category == JVX_PROPERTY_CATEGORY_PREDEFINED)
	return JVX_ERROR_INVALID_ARGUMENT;
}
