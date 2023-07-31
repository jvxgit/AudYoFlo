#include "CjvxAppFactoryHost.h"
#include "jvx-helpers.h"

#define REPORT_ERROR(a, b) if(report) {report->report_simple_text_message(a,b);}

// =================================================================
// =================================================================
// =================================================================
// =================================================================

jvxErrorType
CjvxAppFactoryHost::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename, jvxInt32 lineno)
{
	jvxConfigData* datTmp = NULL, *datTmpLoc = NULL, *datTmpOut = NULL;
	jvxApiValueList datLst;
	IjvxConfiguration* config_local = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize targetStateIdx = 0;
	jvxApiString fldStr;
	jvxApiString fldStrL1;
	jvxApiString fldStrL2;
	jvxInt32 valI32 = 0;
	std::string entry;
	std::string desc;
	jvxBool errorDetected = false;
	jvxInt16 selectme = -1;
	jvxSize num = 0, numL1, numL2;
	jvxSize i,j,k;
	jvxValue valD;
	jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	jvxSize h;
	jvxSize slotid;
	jvxSize slotsubid;
	std::string slottoken;
	jvxSize numSlots = 1;
	jvxValue val;
	jvxConfigData* theSlotNum = NULL;

	// First, deactivate all components which have been previously selected
	if(processor && sectionToContainAllSubsectionsForMe)
	{

		// Get the section for this object
		processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, SECTIONNAME_ALL_SUBSECTIONS);
		if(datTmp)
		{
			for (i = 0; i < externalConfigHooks.size(); i++)
			{
				processor->getReferenceEntryCurrentSection_name(datTmp, &datTmpLoc, externalConfigHooks[i].selector.c_str());
				if (datTmpLoc)
				{
					if (externalConfigHooks[i].ref)
					{
						processor->getOriginSection(datTmpLoc, &fldStr, &valI32);
						externalConfigHooks[i].ref->put_configuration_ext(callConf, processor, datTmpLoc, fldStr.c_str(), valI32);
					}
				}
			}

			if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
			{
				processor->getReferenceEntryCurrentSection_name(datTmp, &datTmpLoc, SECTIONNAME_ALL_EXTERNAL_ENTRIES);
				if (datTmpLoc)
				{
					externalConfigEntries.clear();
					processor->getNumberEntriesCurrentSection(datTmpLoc, &numL1);
					for (i = 0; i < numL1; i++)
					{
						datTmpOut = NULL;
						processor->getReferenceEntryCurrentSection_id(datTmpLoc, &datTmpOut, i);
						if (datTmpOut)
						{
							tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
							processor->getTypeCurrentEntry(datTmpOut, &tp);
							processor->getNameCurrentEntry(datTmpOut, &fldStrL1);

							switch (tp)
							{
							case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:

								processor->getAssignmentString(datTmpOut, &fldStrL2);

								this->set_configuration_entry(fldStrL1.c_str(), (jvxHandle*)&fldStrL2, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING, -1);
								break;
							case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
								processor->getAssignmentValue(datTmpOut, &valD);
								this->set_configuration_entry(fldStrL1.c_str(), (jvxHandle*)&valD, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, -1);
								break;
							case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
							{
								oneExternalConfigEntry newEntry;
								newEntry.tp = JVX_CONFIG_SECTION_TYPE_STRINGLIST;
								newEntry.selector = fldStrL1.std_str();
								processor->getNumberStrings(datTmpOut, &numL2);
								for (j = 0; j < numL2; j++)
								{
									processor->getString_id(datTmpOut, &fldStrL2, j);
									newEntry.assignmentStringList.push_back(fldStrL2.std_str());
								}
								externalConfigEntries.push_back(newEntry);
							}
							break;
							case JVX_CONFIG_SECTION_TYPE_VALUELIST:
								processor->getNumberValueLists(datTmpOut, &numL2);
								for (j = 0; j < numL2; j++)
								{
									processor->getValueList_id(datTmpOut, &datLst, j);
									this->set_configuration_entry(fldStrL1.c_str(), (jvxHandle*)&datLst, JVX_CONFIG_SECTION_TYPE_VALUELIST, (jvxInt32)j);
								}
								break;
							}
							
						}
					}
				}// if(datTmpLoc)
			}
		} // if(datTmp)
	}// if(processor && sectionToContainAllSubsectionsForMe)
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType
CjvxAppFactoryHost::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize i,j,k,h;
	jvxConfigData* datTmp = NULL;
	jvxConfigData* datTmp_add = NULL;
	IjvxConfiguration* config_local = NULL;
	jvxApiString fldStr;
	jvxBool errorDetected = false;
	std::string entry;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD = 0;
	jvxConfigData* datTmpLoc = NULL, *datAdd = NULL;
	std::string slottoken;
	jvxConfigData* theSlotNum = NULL;
	jvxSize numSlots = 0;
	if(processor && sectionWhereToAddAllSubsections)
	{
		processor->createEmptySection(&datTmp, SECTIONNAME_ALL_SUBSECTIONS);
		if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
		{
			if (externalConfigEntries.size())
			{
				processor->createEmptySection(&datTmpLoc, SECTIONNAME_ALL_EXTERNAL_ENTRIES);
				if (datTmpLoc)
				{
					for (i = 0; i < externalConfigEntries.size(); i++)
					{
						datAdd = NULL;
						switch (externalConfigEntries[i].tp)
						{
						case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
							processor->createAssignmentString(&datAdd, externalConfigEntries[i].selector.c_str(),
								externalConfigEntries[i].assignmentString.c_str());
							break;

						case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
							processor->createAssignmentValue(&datAdd, externalConfigEntries[i].selector.c_str(),
								externalConfigEntries[i].assignmentValue);
							break;
						case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
							processor->createAssignmentStringList(&datAdd, externalConfigEntries[i].selector.c_str());

							for (j = 0; j < externalConfigEntries[i].assignmentStringList.size(); j++)
							{
								processor->addAssignmentStringToList(datAdd, externalConfigEntries[i].assignmentStringList[j].c_str());
							}
							break;
						case JVX_CONFIG_SECTION_TYPE_VALUELIST:
							processor->createAssignmentValueList(&datAdd, externalConfigEntries[i].selector.c_str());
							for (j = 0; j < externalConfigEntries[i].assignmentValueMatrix.size(); j++)
							{
								for (k = 0; k < externalConfigEntries[i].assignmentValueMatrix[j].size(); k++)
								{
									processor->addAssignmentValueToList(datAdd, JVX_SIZE_INT(j), externalConfigEntries[i].assignmentValueMatrix[j][k]);
								}
							}
							break;
						}
						if (datAdd)
						{
							processor->addSubsectionToSection(datTmpLoc, datAdd);
						}
					}
					processor->addSubsectionToSection(datTmp, datTmpLoc);
				}
			}
		}

		// Add config sections by hook callbacks
		for (i = 0; i < externalConfigHooks.size(); i++)
		{
			processor->createEmptySection(&datTmpLoc, externalConfigHooks[i].selector.c_str());
			if (externalConfigHooks[i].ref)
			{
				externalConfigHooks[i].ref->get_configuration_ext(callConf, processor, datTmpLoc);
			}
			processor->addSubsectionToSection(datTmp, datTmpLoc);
		}

		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
CjvxAppFactoryHost::register_extension(IjvxConfigurationExtender_report* bwd, const char* sectionName)
{
	std::string entryStr = sectionName;
	std::vector<oneExternalConfigHook>::iterator elm = jvx_findItemSelectorInList<oneExternalConfigHook, std::string>(externalConfigHooks, entryStr);
	if (elm == externalConfigHooks.end())
	{
		oneExternalConfigHook newElm;
		newElm.selector = entryStr;
		newElm.ref = bwd;
		externalConfigHooks.push_back(newElm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType
CjvxAppFactoryHost::unregister_extension(const char* sectionName)
{
	std::string entryStr = sectionName;
	std::vector<oneExternalConfigHook>::iterator elm = jvx_findItemSelectorInList<oneExternalConfigHook, std::string>(externalConfigHooks, entryStr);
	if (elm != externalConfigHooks.end())
	{
		externalConfigHooks.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxAppFactoryHost::clear_configuration_entries()
{
	externalConfigEntries.clear();
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxAppFactoryHost::set_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes tp, jvxSize id)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiStringList* lstStr;
	jvxApiValueList* lstData;
	std::vector<jvxValue> newLst;
	jvxSize i;
	std::string entryStr = entryname;
	if(fld)
	{
		std::vector<oneExternalConfigEntry>::iterator elm = jvx_findItemSelectorInList<oneExternalConfigEntry, std::string>(externalConfigEntries, entryStr);
		if(elm == externalConfigEntries.end())
		{
			oneExternalConfigEntry newEntry;
			newEntry.selector = entryname;
			newEntry.tp = tp;
			switch(newEntry.tp)
			{
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
				newEntry.assignmentString = ((jvxApiString*)fld)->std_str();
				break;
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
				newEntry.assignmentValue = *((jvxValue*)fld);
				break;
			case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
				newEntry.assignmentStringList.clear();
				lstStr = (jvxApiStringList*)fld;
				for(i = 0; i < lstStr->ll(); i++)
				{
					newEntry.assignmentStringList.push_back(lstStr->std_str_at(i));
				}
				break;
			case JVX_CONFIG_SECTION_TYPE_VALUELIST:
				if(JVX_CHECK_SIZE_UNSELECTED(id))
				{
					newEntry.assignmentValueMatrix.clear();
				}
				else
				{
					lstData = (jvxApiValueList*)fld;
					newLst.clear();
					for(i = 0; i < lstData->ll(); i++)
					{
						newLst.push_back(lstData->elm_at(i));
					}
                    
                    if(id < newEntry.assignmentValueMatrix.size())
                    {
                        newEntry.assignmentValueMatrix[id] = newLst;
                    }
                    else
                    {
                        for(i = 0; i < (id - newEntry.assignmentValueMatrix.size());i++)
                        {
                            std::vector<jvxValue> llst;
                            newEntry.assignmentValueMatrix.push_back(llst);
                        }
                        newEntry.assignmentValueMatrix.push_back(newLst);
                    }
				}
				break;
			default:
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			if(res == JVX_NO_ERROR)
			{
				externalConfigEntries.push_back(newEntry);
			}
		}
		else
		{
			elm->tp = tp;
			switch(elm->tp)
			{
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
				elm->assignmentString = ((jvxApiString*)fld)->std_str();
				break;
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
				elm->assignmentValue = *((jvxValue*)fld);
				break;
			case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
				elm->assignmentStringList.clear();
				lstStr = (jvxApiStringList*)fld;
				for(i = 0; i < lstStr->ll(); i++)
				{
					elm->assignmentStringList.push_back(lstStr->std_str_at(i));
				}
				break;
			case JVX_CONFIG_SECTION_TYPE_VALUELIST:
				if(JVX_CHECK_SIZE_UNSELECTED(id))
				{
					elm->assignmentValueMatrix.clear();
				}
				else
				{
					lstData = (jvxApiValueList*)fld;
					newLst.clear();
					for(i = 0; i < lstData->ll(); i++)
					{
						newLst.push_back(lstData->elm_at(i));
					}	
					elm->assignmentValueMatrix[id] = newLst;
				}
				break;
			default:
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType 
CjvxAppFactoryHost::get_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes* tp, jvxSize id)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string entryStr = entryname;
	jvxBool allowCopyTypeSafe = false;
	std::vector<oneExternalConfigEntry>::iterator elm = jvx_findItemSelectorInList<oneExternalConfigEntry, std::string>(externalConfigEntries, entryStr);
	if(elm != externalConfigEntries.end())
	{
		if(tp)
		{
			if (*tp == JVX_CONFIG_SECTION_TYPE_UNKNOWN)
			{
				*tp = elm->tp;
			}
			else
			{
				allowCopyTypeSafe = (elm->tp == *tp);
			}
		}
		if(fld)
		{
			if (allowCopyTypeSafe)
			{
				switch (elm->tp)
				{
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
					((jvxApiString*)fld)->assign( elm->assignmentString);
					break;
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
					*((jvxValue*)fld) = elm->assignmentValue;
					break;
				case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
					((jvxApiStringList*)fld)->assign(elm->assignmentStringList);
					break;
				case JVX_CONFIG_SECTION_TYPE_VALUELIST:
					if (id < elm->assignmentValueMatrix.size())
					{
						((jvxApiValueList*)fld)->assign(elm->assignmentValueMatrix[id]);
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					break;
				}
			}
			else
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
		}
	}
	else
	{
		if(tp)
		{
			*tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
		}
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return(res);
}


