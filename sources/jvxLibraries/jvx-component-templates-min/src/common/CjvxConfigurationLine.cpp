
#include "common/CjvxConfigurationLine.h"

CjvxConfigurationLine::CjvxConfigurationLine()
{
	
}

CjvxConfigurationLine::~CjvxConfigurationLine()
{
		
}

jvxErrorType 
CjvxConfigurationLine::_number_lines(jvxSize* num_lines )
	{
		jvxErrorType res = JVX_NO_ERROR;
		if(num_lines)
		{
			*num_lines = _common_configurationline.theLines.size();
		}
		return(res);
	}

jvxErrorType
CjvxConfigurationLine::_master_line(jvxSize idxLine, jvxComponentIdentification* tp_master)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (idxLine < _common_configurationline.theLines.size())
		{
			if(tp_master)
				*tp_master = _common_configurationline.theLines[idxLine].selector;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		return res;
	}

jvxErrorType
CjvxConfigurationLine::_add_line(const jvxComponentIdentification& tp_master, const char* description)
	{
		jvxOneConfigurationLine newItem;
		jvxErrorType res = JVX_NO_ERROR;
		std::string selector = description;

		std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
		elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
		if (elm == _common_configurationline.theLines.end())
		{
			newItem.selector = tp_master;
			newItem.descriptor = description;
			_common_configurationline.theLines.push_back(newItem);
		}
		else
		{
			res = JVX_ERROR_DUPLICATE_ENTRY;
		}

		return(res);
	}

	// ======================================================================================

jvxErrorType
CjvxConfigurationLine::_add_slave_line(const jvxComponentIdentification& tp_master, const jvxComponentIdentification& tp_slave)
	{
		jvxErrorType res = JVX_NO_ERROR;

		std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
		elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
		if (elm != _common_configurationline.theLines.end())
		{
			
			std::vector<jvxOneSlaveConfigurationLine>::iterator elm2 = elm->slaves_tp.begin();
			elm2 = jvx_findItemSelectorInList<jvxOneSlaveConfigurationLine, jvxComponentIdentification>(elm->slaves_tp, tp_slave);
			if (elm2 == elm->slaves_tp.end())
			{
				jvxOneSlaveConfigurationLine newEntry;
				newEntry.selector = tp_slave;
				elm->slaves_tp.push_back(newEntry);
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
		return(res);
	}

jvxErrorType
CjvxConfigurationLine::_remove_slave_line(const jvxComponentIdentification& tp_master, const jvxComponentIdentification& tp_slave)
	{
		jvxErrorType res = JVX_NO_ERROR;

		std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
		elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
		if (elm != _common_configurationline.theLines.end())
		{
			std::vector<jvxOneSlaveConfigurationLine>::iterator elm2 = elm->slaves_tp.begin();
			elm2 = jvx_findItemSelectorInList<jvxOneSlaveConfigurationLine, jvxComponentIdentification>(
				elm->slaves_tp, tp_slave);

			if (elm2 != elm->slaves_tp.end())
			{
				elm->slaves_tp.erase(elm2);
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return(res);
	}

jvxErrorType
CjvxConfigurationLine::_number_slaves_line(const jvxComponentIdentification& tp_master, jvxSize* num_slaves)
	{
		jvxErrorType res = JVX_NO_ERROR;

		std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
		elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
		if (elm != _common_configurationline.theLines.end())
		{
			if (num_slaves)
			{
				*num_slaves = elm->slaves_tp.size();
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return(res);
	}

jvxErrorType
CjvxConfigurationLine::_slave_line_master(const jvxComponentIdentification& tp_master, jvxSize idxEntry, jvxComponentIdentification* tp_slave)
	{
		jvxErrorType res = JVX_NO_ERROR;

		std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
		elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
		if (elm != _common_configurationline.theLines.end())
		{
			if (idxEntry < elm->slaves_tp.size())
			{
			if(tp_slave)
				*tp_slave = elm->slaves_tp[idxEntry].selector;
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return(res);
	}

	// ======================================================================================

jvxErrorType
CjvxConfigurationLine::_add_configuration_property_line(const jvxComponentIdentification& tp_master, jvxSize system_property_id)
	{
		jvxErrorType res = JVX_NO_ERROR;

		std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
		elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
		if (elm != _common_configurationline.theLines.end())
		{
			std::vector<jvxOneConfigurationId>::iterator elm2 = elm->link_properties.begin();
			elm2 = jvx_findItemSelectorInList<jvxOneConfigurationId, jvxSize>(
				elm->link_properties, system_property_id);

			if (elm2 == elm->link_properties.end())
			{
				jvxOneConfigurationId newElm;
				newElm.selector = system_property_id;
				elm->link_properties.push_back(newElm);
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
		return(res);
	};


jvxErrorType
CjvxConfigurationLine::_remove_configuration_property_line(const jvxComponentIdentification& tp_master, jvxSize system_property_id)
	{
		jvxErrorType res = JVX_NO_ERROR;

		std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
		elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
		if (elm != _common_configurationline.theLines.end())
		{
			std::vector<jvxOneConfigurationId>::iterator elm2 = elm->link_properties.begin();
			elm2 = jvx_findItemSelectorInList<jvxOneConfigurationId, jvxSize>(
				elm->link_properties, system_property_id);

			if (elm2 != elm->link_properties.end())
			{
				elm->link_properties.erase(elm2);
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return(res);
	};

jvxErrorType
CjvxConfigurationLine::_number_property_ids_line(const jvxComponentIdentification& tp_master, jvxSize* num_Props)
	{
		jvxErrorType res = JVX_NO_ERROR;

		std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
		elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
		if (elm != _common_configurationline.theLines.end())
		{
			if (num_Props)
			{
				*num_Props = elm->link_properties.size();
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return(res);
	}

jvxErrorType
CjvxConfigurationLine::_property_id_line_master(const jvxComponentIdentification& tp_master, jvxSize idxEntry, jvxSize* the_prop_id)
	{
		jvxErrorType res = JVX_NO_ERROR;

		std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
		elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
		if (elm != _common_configurationline.theLines.end())
		{
			if (idxEntry < elm->link_properties.size())
			{
				if (the_prop_id)
				{
					*the_prop_id = elm->link_properties[idxEntry].selector;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return(res);
	}

	// ======================================================================================

jvxErrorType
CjvxConfigurationLine::_remove_line(const jvxComponentIdentification& tp_master)
	{
		jvxErrorType res = JVX_NO_ERROR;

		std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
		elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
		if (elm != _common_configurationline.theLines.end())
		{
			_common_configurationline.theLines.erase(elm);
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return(res);
	}

jvxErrorType
CjvxConfigurationLine::_report_property_was_set(const jvxComponentIdentification& tp_master, jvxPropertyCategoryType cat, jvxSize propIdx,
		IjvxHost* theHost, jvxBool onlyContent, const jvxComponentIdentification& tpTo)
	{
		jvxCallManagerProperties callGate;
		jvxSize j, k;
		jvxDataFormat form = JVX_DATAFORMAT_NONE;
		jvxInt32 num = 0;
		jvxInt32 valI32 = 0;
		jvxInt64 valI64 = 0;
		jvxInt16 valI16 = 0;
		jvxInt8 valI8 = 0;
		jvxData valDbl = 0;
		jvxData valFlt = 0;
		jvxSelectionList lst;
		jvxApiStringList strListLoc;

		jvx_propertyReferenceTriple theTriple_get;
		jvx_propertyReferenceTriple theTriple_sets;

		jvxHandle* theDataPtr = NULL;
		jvxErrorType res = JVX_NO_ERROR;

		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident;
		jvx::propertyDetail::CjvxTranferDetail trans;

		if(tpTo.tp == JVX_COMPONENT_UNKNOWN)
		{
			std::vector<jvxOneConfigurationLine>::iterator elm = _common_configurationline.theLines.begin();
			elm = jvx_findItemSelectorInList<jvxOneConfigurationLine, jvxComponentIdentification>(_common_configurationline.theLines, tp_master);
			if (elm != _common_configurationline.theLines.end())
			{
				for (j = 0; j < elm->link_properties.size(); j++)
				{
					jvxSize thePropId = elm->link_properties[j].selector;
					if (thePropId == propIdx)
					{
						if (jvx_findPropertyDescriptorId(thePropId, &form, &num, NULL))
						{
							if (num == 1)
							{
								switch (form)
								{
								case JVX_DATAFORMAT_64BIT_LE:
									theDataPtr = &valI64;
									break;
								case JVX_DATAFORMAT_32BIT_LE:
									theDataPtr = &valI32;
									break;
								case JVX_DATAFORMAT_16BIT_LE:
									theDataPtr = &valI16;
									break;
								case JVX_DATAFORMAT_8BIT:
									theDataPtr = &valI8;
									break;
								case JVX_DATAFORMAT_DATA:
									theDataPtr = &valDbl;
									break;
								case JVX_DATAFORMAT_SELECTION_LIST:
									theDataPtr = &lst;
									break;
								default:
									theDataPtr = NULL;
								}

								if (theHost && theDataPtr)
								{
									jvx_initPropertyReferenceTriple(&theTriple_get);
									res = jvx_getReferencePropertiesObject(theHost, &theTriple_get, tp_master);
									if ((res == JVX_NO_ERROR) && theTriple_get.theProps)
									{
										bool isValid = false;

										switch (form)
										{
										case JVX_DATAFORMAT_SELECTION_LIST:
											
											ident.reset(thePropId, JVX_PROPERTY_CATEGORY_PREDEFINED);
											trans.reset(onlyContent);

											res = theTriple_get.theProps->get_property(callGate, jPRG( theDataPtr, num, form), ident, trans);
											if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(thePropId, JVX_PROPERTY_CATEGORY_PREDEFINED),
												theTriple_get.theProps))
											{
												for (k = 0; k < elm->slaves_tp.size(); k++)
												{
													jvx_initPropertyReferenceTriple(&theTriple_sets);
													jvxComponentIdentification tpSet(elm->slaves_tp[k].selector);
													int id = JVX_SIZE_INT(tpTo.slotid);
													if (id < 0)
													{
														tpSet.slotid = tp_master.slotid;
													}
													id = JVX_SIZE_INT(tpTo.slotsubid);
													if (id < 0)
													{
														tpSet.slotsubid = tp_master.slotsubid;
													}

													res = jvx_getReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
													if ((res == JVX_NO_ERROR) && theTriple_sets.theProps)
													{
														ident.reset(thePropId, JVX_PROPERTY_CATEGORY_PREDEFINED);
														trans.reset(onlyContent);

														res = theTriple_sets.theProps->set_property(callGate, 
															jPRG(theDataPtr, num, form), 
															ident, trans);
														jvx_returnReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
													}
												}
											}
											break;
										default:

											ident.reset(thePropId, JVX_PROPERTY_CATEGORY_PREDEFINED);
											trans.reset(onlyContent);

											res = theTriple_get.theProps->get_property(callGate, jPRG( theDataPtr, num, form), ident, trans);
											if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(thePropId, JVX_PROPERTY_CATEGORY_PREDEFINED),
												theTriple_get.theProps))
											{
												for (k = 0; k < elm->slaves_tp.size(); k++)
												{
													jvx_initPropertyReferenceTriple(&theTriple_sets);
													jvxComponentIdentification tpSet(elm->slaves_tp[k].selector);
													int id = JVX_SIZE_INT(tpTo.slotid);
													if (id < 0)
													{
														tpSet.slotid = tp_master.slotid;
													}
													id = JVX_SIZE_INT(tpTo.slotsubid);
													if (id < 0)
													{
														tpSet.slotsubid = tp_master.slotsubid;
													}

													res = jvx_getReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
													if ((res == JVX_NO_ERROR) && theTriple_sets.theProps)
													{
														ident.reset(thePropId, JVX_PROPERTY_CATEGORY_PREDEFINED);
														trans.reset(onlyContent);

														res = theTriple_sets.theProps->set_property(callGate, 
															jPRG(theDataPtr, num, form), ident, trans);
														jvx_returnReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
													}
												}
											}
											break;
										}
										jvx_returnReferencePropertiesObject(theHost, &theTriple_get, tp_master); // <- return reference
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if(jvx_findPropertyDescriptorId(propIdx, &form, &num, NULL))
			{
				if(num == 1)
				{
					switch(form)
					{
					case JVX_DATAFORMAT_64BIT_LE:
						theDataPtr = &valI64;
						break;
					case JVX_DATAFORMAT_32BIT_LE:
						theDataPtr = &valI32;
						break;
					case JVX_DATAFORMAT_16BIT_LE:
						theDataPtr = &valI16;
						break;
					case JVX_DATAFORMAT_8BIT:
						theDataPtr = &valI8;
						break;
					case JVX_DATAFORMAT_DATA:
						theDataPtr = &valDbl;
						break;
					case JVX_DATAFORMAT_SELECTION_LIST:
						theDataPtr = &lst;
						break;
					default:
						theDataPtr = NULL;					
					}

					if(theHost && theDataPtr)
					{
						jvx_initPropertyReferenceTriple(&theTriple_get);
						res = jvx_getReferencePropertiesObject(theHost, &theTriple_get, tp_master);
						if((res == JVX_NO_ERROR) && theTriple_get.theProps)
						{
							switch(form)
							{
							case JVX_DATAFORMAT_SELECTION_LIST:

								ident.reset(propIdx, JVX_PROPERTY_CATEGORY_PREDEFINED);
								trans.reset(onlyContent);

								res = theTriple_get.theProps->get_property(callGate, jPRG( theDataPtr, num, form), ident, trans);
								if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(propIdx, JVX_PROPERTY_CATEGORY_PREDEFINED),
									theTriple_get.theProps))
								{

									jvx_initPropertyReferenceTriple(&theTriple_sets);
									jvxComponentIdentification tpSet(tpTo);
									int id = JVX_SIZE_INT(tpTo.slotid);
									if (id < 0)
									{
										tpSet.slotid = tp_master.slotid;
									}
									id = JVX_SIZE_INT(tpTo.slotsubid);
									if (id < 0)
									{
										tpSet.slotsubid = tp_master.slotsubid;
									}

									res = jvx_getReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
									if ((res == JVX_NO_ERROR) && theTriple_sets.theProps)
									{
										ident.reset(propIdx, JVX_PROPERTY_CATEGORY_PREDEFINED);
										trans.reset(onlyContent);

										res = theTriple_sets.theProps->set_property(callGate, 
											jPRG(theDataPtr, num, form), 
											ident, trans);
										jvx_returnReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
									}
								}
								break;
							default:

								ident.reset(propIdx, JVX_PROPERTY_CATEGORY_PREDEFINED);
								trans.reset(onlyContent);

								res = theTriple_get.theProps->get_property(callGate, jPRG( theDataPtr, num, form), ident, trans);
								if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(propIdx, JVX_PROPERTY_CATEGORY_PREDEFINED),
									theTriple_get.theProps))
								{

									jvx_initPropertyReferenceTriple(&theTriple_sets);
									jvxComponentIdentification tpSet(tpTo);
									int id = JVX_SIZE_INT(tpTo.slotid);
									if (id < 0)
									{
										tpSet.slotid = tp_master.slotid;
									}
									id = JVX_SIZE_INT(tpTo.slotsubid);
									if (id < 0)
									{
										tpSet.slotsubid = tp_master.slotsubid;
									}
									res = jvx_getReferencePropertiesObject(theHost, &theTriple_sets,tpSet);
									if((res == JVX_NO_ERROR) && theTriple_sets.theProps)
									{
										ident.reset(propIdx, JVX_PROPERTY_CATEGORY_PREDEFINED);
										trans.reset(onlyContent);

										res = theTriple_sets.theProps->set_property(callGate, 
											jPRG(theDataPtr, num, form), 
											ident, trans);
										jvx_returnReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
									}
								}
							}
							jvx_returnReferencePropertiesObject(theHost, &theTriple_get, tp_master); // <- return reference
						}
					}
				}
			}
		}
		return(JVX_NO_ERROR);
	}

	jvxErrorType
		CjvxConfigurationLine::_refresh_all(IjvxHost* theHost)
	{
		jvxSize i, j, k;
		jvxDataFormat form = JVX_DATAFORMAT_NONE;
		jvxInt32 num = 0;
		jvxInt32 valI32 = 0;
		jvxInt64 valI64 = 0;
		jvxInt16 valI16 = 0;
		jvxInt8 valI8 = 0;
		jvxData valDbl = 0;
		jvxData valFlt = 0;
		jvxSelectionList lst;

		jvx_propertyReferenceTriple theTriple_get;
		jvx_propertyReferenceTriple theTriple_sets;

		jvxHandle* theDataPtr = NULL;
		jvxErrorType res = JVX_NO_ERROR;
		jvxCallManagerProperties callGate;
		jvxCallManagerProperties call_gate;

		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident;
		jvx::propertyDetail::CjvxTranferDetail trans;

		for(i = 0; i < _common_configurationline.theLines.size(); i++)
		{
			jvxComponentIdentification tp = _common_configurationline.theLines[i].selector;
				
			for(j = 0; j < _common_configurationline.theLines[i].link_properties.size(); j++)
			{
				jvxSize thePropId = _common_configurationline.theLines[i].link_properties[j].selector;
				if(jvx_findPropertyDescriptorId(thePropId, &form, &num, NULL))
				{
					if (num == 1)
					{
						switch (form)
						{
						case JVX_DATAFORMAT_64BIT_LE:
							theDataPtr = &valI64;
							break;
						case JVX_DATAFORMAT_32BIT_LE:
							theDataPtr = &valI32;
							break;
						case JVX_DATAFORMAT_16BIT_LE:
							theDataPtr = &valI16;
							break;
						case JVX_DATAFORMAT_8BIT:
							theDataPtr = &valI8;
							break;
						case JVX_DATAFORMAT_DATA:
							theDataPtr = &valDbl;
							break;
						case JVX_DATAFORMAT_SELECTION_LIST:
							theDataPtr = &lst;
							break;
						default:
							assert(0);
						}

						if (theHost)
						{
							jvx_initPropertyReferenceTriple(&theTriple_get);
							jvxSize slotid = 0;
							while (1)
							{
								jvxSize slotsubid = 0;
								while (1)
								{
									tp.slotid = slotid;
									tp.slotsubid = slotsubid;

									res = jvx_getReferencePropertiesObject(theHost, &theTriple_get, tp);
									if (res == JVX_ERROR_ID_OUT_OF_BOUNDS)
									{
										break;
									}
									else if ((res == JVX_NO_ERROR) && theTriple_get.theProps)
									{
										bool isValid = false;
										ident.reset(thePropId, JVX_PROPERTY_CATEGORY_PREDEFINED);
										trans.reset();
										res = theTriple_get.theProps->get_property(callGate, jPRG( theDataPtr, num, form), ident, trans);
										if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(thePropId, JVX_PROPERTY_CATEGORY_PREDEFINED),
											theTriple_get.theProps))
										{
											switch (form)
											{
											case  JVX_DATAFORMAT_SELECTION_LIST:

												for (k = 0; k < _common_configurationline.theLines[i].slaves_tp.size(); k++)
												{
													jvx_initPropertyReferenceTriple(&theTriple_sets);
													jvxComponentIdentification tpSet = _common_configurationline.theLines[i].slaves_tp[k].selector;
													int id = JVX_SIZE_INT(tpSet.slotid);
													if (id < 0)
													{
														tpSet.slotid = slotid;
													}
													id = JVX_SIZE_INT(tpSet.slotsubid);
													if (id < 0)
													{
														tpSet.slotsubid = slotsubid;
													}
													res = jvx_getReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
													if ((res == JVX_NO_ERROR) && theTriple_sets.theProps)
													{
														ident.reset(thePropId, JVX_PROPERTY_CATEGORY_PREDEFINED);
														trans.reset(false);
														res = theTriple_sets.theProps->set_property(callGate, 
															jPRG(theDataPtr, num, form), 
															ident,trans);
														jvx_returnReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
													}
												}

												break;
											default:

												for (k = 0; k < _common_configurationline.theLines[i].slaves_tp.size(); k++)
												{
													jvx_initPropertyReferenceTriple(&theTriple_sets);
													jvxComponentIdentification tpSet = _common_configurationline.theLines[i].slaves_tp[k].selector;
													int id = JVX_SIZE_INT(tpSet.slotid);
													if (id < 0)
													{
														tpSet.slotid = slotid;
													}
													id = JVX_SIZE_INT(tpSet.slotsubid);
													if (id < 0)
													{
														tpSet.slotsubid = slotsubid;
													}
													res = jvx_getReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
													if ((res == JVX_NO_ERROR) && theTriple_sets.theProps)
													{
														jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(thePropId, JVX_PROPERTY_CATEGORY_PREDEFINED);
														jvx::propertyDetail::CjvxTranferDetail trans(false);
														res = theTriple_sets.theProps->set_property(
															callGate, 
															jPRG(theDataPtr, num, form), 
															ident, trans);
														jvx_returnReferencePropertiesObject(theHost, &theTriple_sets, tpSet);
													}
												}

												break;
											}
										}
										jvx_returnReferencePropertiesObject(theHost, &theTriple_get, tp); // <- return reference

									}
									slotsubid++;
								} // while (1)
								
								if (res == JVX_ERROR_ID_OUT_OF_BOUNDS)
								{
									break;
								}
								slotid++;
							} // while (1)
						}
					}
				}
			}
		}
		return(JVX_NO_ERROR);
	}

	jvxErrorType
		CjvxConfigurationLine::_updateAllAfterStateChanged(jvxComponentIdentification tp, jvxState newState, IjvxHost* theHost)
	{
		jvxSize i,j,k;
		switch(newState)
		{
		case JVX_STATE_SELECTED:
		case JVX_STATE_ACTIVE:

			for(i = 0; i < _common_configurationline.theLines.size(); i++)
			{
				for(k = 0; k < _common_configurationline.theLines[i].slaves_tp.size(); k++)
				{
					if(tp == _common_configurationline.theLines[i].slaves_tp[k].selector)
					{
						for(j = 0; j < _common_configurationline.theLines[i].link_properties.size(); j++)
						{
							jvxSize thePropId = _common_configurationline.theLines[i].link_properties[j].selector;
							_report_property_was_set(_common_configurationline.theLines[i].selector, JVX_PROPERTY_CATEGORY_PREDEFINED,
								thePropId, theHost, true, tp);
						}
					}
				}
			}
			break;
		}
		return(JVX_NO_ERROR);
	}

	jvxErrorType
		CjvxConfigurationLine::_remove_all_lines()
	{
		_common_configurationline.theLines.clear();
		return(JVX_NO_ERROR);
	}
