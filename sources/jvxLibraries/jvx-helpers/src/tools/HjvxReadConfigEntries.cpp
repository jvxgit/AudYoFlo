#include "jvx-helpers.h"

jvxErrorType
HjvxConfigProcessor_readEntry_nameEntry(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string& entry)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString fldStr;
	res = theReader->getNameCurrentEntry(theSection, &fldStr);
	if(res == JVX_NO_ERROR)
	{
		entry = fldStr.std_str();
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return(res);
}

jvxErrorType
HjvxConfigProcessor_readEntry_originEntry(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::string& fileName, jvxInt32& lineno)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxApiString fldStr;
	jvxInt32 valI = 0;

	if(theReader && theSection)
	{
		theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
		if(datTmp)
		{
			res = theReader->getOriginSection(datTmp, &fldStr, &lineno);
			if(res == JVX_NO_ERROR)
			{
				fileName = fldStr.std_str();
			}
			else
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
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
HjvxConfigProcessor_readEntries_originEntry(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::vector<oneOriginCfgEntry>& origins)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxApiString fldStr;
	jvxInt32 valI = 0;

	origins.clear();

	if (theReader && theSection)
	{
		jvxSize num = 0;
		theReader->getNumberEntriesCurrentSection(theSection, &num);
		for (jvxSize idx = 0; idx < num; idx++)
		{
			theReader->getReferenceEntryCurrentSection_id(theSection, &datTmp, idx);
			if (datTmp)
			{
				jvxApiString astr;
				res = theReader->getNameCurrentEntry(datTmp, &astr);
				if (res == JVX_NO_ERROR)
				{
					if (astr.std_str() == nmToken)
					{
						oneOriginCfgEntry newEntry;
						res = theReader->getOriginSection(datTmp, &fldStr, &newEntry.lineNo);
						if (res == JVX_NO_ERROR)
						{
							newEntry.fName = fldStr.std_str();
							origins.push_back(newEntry);
						}
						else
						{
							res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
						}
					}
				}
				else
				{
					res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
				}
			}
			else
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
			if (res != JVX_NO_ERROR)
			{
				break;
			}
		}
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return(res);
}
jvxErrorType
HjvxConfigProcessor_readEntry_originEntry(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string& fileName, jvxInt32& lineno)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxApiString fldStr;
	jvxInt32 valI = 0;

	if(theReader && theSection)
	{
		res = theReader->getOriginSection(theSection, &fldStr, &lineno);
		if(res == JVX_NO_ERROR)
		{
			fileName = fldStr.std_str();
		}
		else
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		}
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return(res);
}

jvxErrorType
HjvxConfigProcessor_readEntry_references(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::string& nmKey, jvxBool* isAbsolute, std::vector<std::string>& lstReference)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxApiString fldStr;
	jvxSize num = 0;

	if(theReader && theSection)
	{
		theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
		if(datTmp)
		{
			res = theReader->getReferenceKey(datTmp, &fldStr);
			if(res == JVX_NO_ERROR)
			{
				nmKey = fldStr.std_str();
				theReader->getReferenceNumberPathTokens(datTmp, &num);
				for (i = 0; i < num; i++)
				{
					res = theReader->getReferencePathToken_id(datTmp, &fldStr, i);
					if (res == JVX_NO_ERROR)
					{
						lstReference.push_back(fldStr.std_str());
					}
				}
				theReader->getReferencePathType(datTmp, isAbsolute);
			}
			else
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{ 
		res = JVX_ERROR_INVALID_ARGUMENT;
	}

	return(res);
}

/**********************************************************************************
 * Read single entries in config file 
 *********************************************************************************/

jvxErrorType
HjvxConfigProcessor_readEntry_assignmentBitField(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, jvxBitField* entry, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags, jvxCBitField whattodo)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxValue valD = 0;

	if(theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{
				res = theReader->getAssignmentBitField(datTmp, entry);
				if (res != JVX_NO_ERROR)
				{
					res = theReader->getAssignmentValue(datTmp, &valD);
					if (res == JVX_NO_ERROR)
					{
						if (entry)
						{
							switch (valD.tp)
							{
							case JVX_DATAFORMAT_DATA:
								*entry = JVX_DATA2BITFIELD(valD.ct.valD);
								break;
							case JVX_DATAFORMAT_16BIT_LE:
								*entry = jvxBitField(valD.ct.valI16);
								break;
							case JVX_DATAFORMAT_32BIT_LE:
								*entry = jvxBitField(valD.ct.valI32);
								break;
							case JVX_DATAFORMAT_64BIT_LE:
								*entry = jvxBitField(valD.ct.valI64);
								break;
							case JVX_DATAFORMAT_8BIT:
								*entry = jvxBitField(valD.ct.valI8);
								break;
							case JVX_DATAFORMAT_SIZE:
								*entry = jvxBitField((jvxInt32)valD.ct.valS);
								break;
							default:
								assert(0);
							}
						}
					}
					else
					{
						res = JVX_ERROR_WRONG_SECTION_TYPE;
					}
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}
	else
	{ 
		res = JVX_ERROR_INVALID_ARGUMENT;
	}

	if ((res == JVX_NO_ERROR) && datTmp)
	{
		jvxAccessRightFlags_rwcd* acc_flag_ptr = NULL;
		jvxConfigModeFlags* cfg_flag_ptr = NULL;
		jvxBool acc_set = false;
		jvxBool cfg_set = false;

		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_ACCESS_RIGHTS)
		{
			acc_flag_ptr = acc_flags;
		}
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONFIG_MODE)
		{
			cfg_flag_ptr = cfg_flags;
		}
		theReader->getBitFieldTags(datTmp, acc_flag_ptr, &acc_set, cfg_flag_ptr, &cfg_set);
	}

	return(res);
}

/**********************************************************************************
 * Read array numeric entries in config file, size known in advance 
 *********************************************************************************/



jvxErrorType
HjvxConfigProcessor_readEntry_1dList_allocate(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken,  jvxApiValueList* entries, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags, jvxCBitField whattodo)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxSize num = 0;

	if(theReader && theSection && entries)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{
				res = theReader->getNumberValueLists(datTmp, &num);
				if (res == JVX_NO_ERROR)
				{
					if (0 < num)
					{
						theReader->getValueList_id(datTmp, entries, 0);
					}
					else
					{
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}
				}
				else
				{
					res = JVX_ERROR_WRONG_SECTION_TYPE;
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}
	else
	{ 
		res = JVX_ERROR_INVALID_ARGUMENT;
	}

	if ((res == JVX_NO_ERROR) && datTmp)
	{
		jvxAccessRightFlags_rwcd* acc_flag_ptr = NULL;
		jvxConfigModeFlags* cfg_flag_ptr = NULL;
		jvxBool acc_set = false;
		jvxBool cfg_set = false;

		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_ACCESS_RIGHTS)
		{
			acc_flag_ptr = acc_flags;
		}
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONFIG_MODE)
		{
			cfg_flag_ptr = cfg_flags;
		}
		theReader->getBitFieldTags(datTmp, acc_flag_ptr, &acc_set, cfg_flag_ptr, &cfg_set);
	}

	return(res);
}

/**********************************************************************************
 * Read array numeric entries in config file, size known in advance 
 *********************************************************************************/



/**********************************************************************************
 * Read string/string list entries in config file 
 *********************************************************************************/
jvxErrorType
HjvxConfigProcessor_readEntry_assignmentStringList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::vector<std::string>* entries, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags, jvxCBitField whattodo)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxApiString fldStr;
	jvxSize num = 0;

	if (theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{				
				res = theReader->getNumberStrings(datTmp, &num);
				if (res == JVX_NO_ERROR)
				{
					for (i = 0; i < num; i++)
					{
						res = theReader->getString_id(datTmp, &fldStr, i);
						if (res == JVX_NO_ERROR)
						{
							if (entries)
							{
								entries->push_back(fldStr.std_str());
							}
					
						}
						else
						{
							res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
						}
					}
				}
				else
				{
					res = JVX_ERROR_WRONG_SECTION_TYPE;
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}
	else
	{ 
		res = JVX_ERROR_INVALID_ARGUMENT;
	}

	if ((res == JVX_NO_ERROR) && datTmp)
	{
		jvxAccessRightFlags_rwcd* acc_flag_ptr = NULL;
		jvxConfigModeFlags* cfg_flag_ptr = NULL;
		jvxBool acc_set = false;
		jvxBool cfg_set = false;

		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_ACCESS_RIGHTS)
		{
			acc_flag_ptr = acc_flags;
		}
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONFIG_MODE)
		{
			cfg_flag_ptr = cfg_flags;
		}
		theReader->getBitFieldTags(datTmp, acc_flag_ptr, &acc_set, cfg_flag_ptr, &cfg_set);
	}

	return(res);
}

jvxErrorType
HjvxConfigProcessor_readEntry_assignmentSelectionList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, 
	std::string nmToken, jvxSelectionList_cpp* entries,
	bool onlySelectionToConfig, jvxBool ignoreProblems, jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags, jvxCBitField whattodo,
	std::vector<std::string>* entriesOnReturnIfNotAssigned)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL, *datTmp1 = NULL;
	jvxSize num = 0;
	jvxSize i, j;

	if(theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceSubsectionCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{
				if (!onlySelectionToConfig)
				{
					entries->entries.clear();
					HjvxConfigProcessor_readEntry_assignmentStringList(theReader, datTmp, JVX_TOKEN_CONFIG_SELECTIONLIST_NAMES, &entries->entries);
					HjvxConfigProcessor_readEntry_assignmentBitField(theReader, datTmp, JVX_TOKEN_CONFIG_EXCLUSIVE_VALUE, &entries->exclusive);
					HjvxConfigProcessor_readEntry_assignmentBitField(theReader, datTmp, JVX_TOKEN_CONFIG_SELECTION_VALUE, &entries->selection());
				}
				else
				{
					std::vector<std::string> entries_read;
					resL = HjvxConfigProcessor_readEntry_assignmentStringList(theReader, datTmp, JVX_TOKEN_CONFIG_SELECTIONLIST_NAMES_SELECTED, &entries_read);
					if (resL == JVX_NO_ERROR)
					{
						jvxBitField restore_if_none_found = entries->selection();
						jvxBool problemDetected = false;
						jvx_bitFClear(entries->selection());
						for (i = 0; i < entries_read.size(); i++)
						{
							jvxSize foundEntryTimes = 0;
							for (j = 0; j < entries->entries.size(); j++)
							{
								if(jvx_compareStringsWildcard(entries_read[i], entries->entries[j]))								
								// if (entries_read[i] == entries->entries[j])
								{
									jvx_bitSet(entries->selection(), j);
									foundEntryTimes++;
									// break; explicitely no break here if names are identical!
								}
							}

							if (foundEntryTimes != 1)
							{
								std::cout << "Warning: found entry <" << entries_read[i] << "> " << foundEntryTimes << " times in list of selections specified in configuration for selection list <" << 
									nmToken << ">." << std::endl;
								problemDetected = true;
							}
						}
						if (ignoreProblems)
						{
							if (entriesOnReturnIfNotAssigned)
							{
								*entriesOnReturnIfNotAssigned = entries_read;
							}
						}
						else
						{
							if(problemDetected)
							{
								// Restore original bitfield if there was a problem
								entries->selection() = restore_if_none_found;
							}
						}
					}
					else
					{
						HjvxConfigProcessor_readEntry_assignmentBitField(theReader, datTmp, JVX_TOKEN_CONFIG_SELECTION_VALUE, &entries->selection());
					}
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}
	else
	{ 
		res = JVX_ERROR_INVALID_ARGUMENT;
	}

	if ((res == JVX_NO_ERROR) && datTmp)
	{
		jvxAccessRightFlags_rwcd* acc_flag_ptr = NULL;
		jvxConfigModeFlags* cfg_flag_ptr = NULL;
		jvxBool acc_set = false;
		jvxBool cfg_set = false;

		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_ACCESS_RIGHTS)
		{
			acc_flag_ptr = acc_flags;
		}
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONFIG_MODE)
		{
			cfg_flag_ptr = cfg_flags;
		}
		theReader->getBitFieldTags(datTmp, acc_flag_ptr, &acc_set, cfg_flag_ptr, &cfg_set);
	}

	return(res);
}





jvxErrorType
HjvxConfigProcessor_readEntry_assignmentString(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::string* entry, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags, jvxCBitField whattodo)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxApiString fldStr;

	if(theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{
				res = theReader->getAssignmentString(datTmp, &fldStr);
				if (res == JVX_NO_ERROR)
				{
					if (entry)
					{
						*entry = fldStr.std_str();
					}
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}
	else
	{ 
		res = JVX_ERROR_INVALID_ARGUMENT;
	}

	if ((res == JVX_NO_ERROR) && datTmp)
	{
		jvxAccessRightFlags_rwcd* acc_flag_ptr = NULL;
		jvxConfigModeFlags* cfg_flag_ptr = NULL;
		jvxBool acc_set = false;
		jvxBool cfg_set = false;

		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_ACCESS_RIGHTS)
		{
			acc_flag_ptr = acc_flags;
		}
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONFIG_MODE)
		{
			cfg_flag_ptr = cfg_flags;
		}
		theReader->getBitFieldTags(datTmp, acc_flag_ptr, &acc_set, cfg_flag_ptr, &cfg_set);
	}

	return(res);
}

jvxErrorType
HjvxConfigProcessor_readEntries_assignmentStringList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, const std::string& nmToken, std::vector < std::string>& entries,
	std::vector< oneFlagSetEntry>* flags, jvxCBitField whattodo)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxApiString fldStr;
	entries.clear();
	if (flags)
	{
		flags->clear();
	}

	if (theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			jvxSize num = 0;
			theReader->getNumberEntriesCurrentSection(theSection, &num);
			for (jvxSize idx = 0; idx < num; idx++)
			{
				datTmp = nullptr;
				theReader->getReferenceEntryCurrentSection_id(theSection, &datTmp, idx);
				if (datTmp)
				{
					jvxApiString astr;
					theReader->getNameCurrentEntry(datTmp, &astr);
					if (astr.std_str() == nmToken)
					{
						jvxSize numStr = 0;
						res = theReader->getNumberStrings(datTmp, &numStr);
						if (res == JVX_NO_ERROR)
						{
							for (jvxSize idxi = 0; idxi < numStr; idxi++)
							{
								theReader->getString_id(datTmp, &fldStr, idxi);
								entries.push_back(fldStr.std_str());

								if (flags)
								{
									oneFlagSetEntry newFlag;
									jvxAccessRightFlags_rwcd* acc_flag_ptr = NULL;
									jvxConfigModeFlags* cfg_flag_ptr = NULL;
									jvxBool acc_set = false;
									jvxBool cfg_set = false;

									if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_ACCESS_RIGHTS)
									{
										acc_flag_ptr = &newFlag.acc_flags;
									}
									if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONFIG_MODE)
									{
										cfg_flag_ptr = &newFlag.cfg_flags;
									}
									theReader->getBitFieldTags(datTmp, acc_flag_ptr, &acc_set, cfg_flag_ptr, &cfg_set);
									flags->push_back(newFlag);
								}
							}
						}
						res = JVX_NO_ERROR;
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
				if (res != JVX_NO_ERROR)
				{
					break;
				}
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
HjvxConfigProcessor_readEntry_assignmentValueInRange(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, jvxValueInRange* entries, 
	bool onlySelectionToConfig, jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags, jvxCBitField whattodo)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL, *datTmp1 = NULL;
	jvxSize num = 0;

	if(theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceSubsectionCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{
				if (!onlySelectionToConfig)
				{
					HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, datTmp, JVX_TOKEN_CONFIG_VALUE_IN_RANGE_MIN, &entries->minVal);
					HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, datTmp, JVX_TOKEN_CONFIG_VALUE_IN_RANGE_MAX, &entries->maxVal);
				}
				HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, datTmp, JVX_TOKEN_CONFIG_VALUE_IN_RANGE, &entries->val());
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}
	else
	{ 
		res = JVX_ERROR_INVALID_ARGUMENT;
	}

	if ((res == JVX_NO_ERROR) && datTmp)
	{
		jvxAccessRightFlags_rwcd* acc_flag_ptr = NULL;
		jvxConfigModeFlags* cfg_flag_ptr = NULL;
		jvxBool acc_set = false;
		jvxBool cfg_set = false;

		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_ACCESS_RIGHTS)
		{
			acc_flag_ptr = acc_flags;
		}
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONFIG_MODE)
		{
			cfg_flag_ptr = cfg_flags;
		}
		theReader->getBitFieldTags(datTmp, acc_flag_ptr, &acc_set, cfg_flag_ptr, &cfg_set);
	}

	return(res);
}

