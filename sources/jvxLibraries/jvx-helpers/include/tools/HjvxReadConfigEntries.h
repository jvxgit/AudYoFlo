#ifndef __HJVXREADCONFIGENTRIES_H__
#define __HJVXREADCONFIGENTRIES_H__

#include "jvx-helpers.h"

struct oneOriginCfgEntry
{
	std::string fName;
	int lineNo = -1;
};

struct oneFlagSetEntry
{
	jvxAccessRightFlags_rwcd acc_flags = 0;
	jvxConfigModeFlags cfg_flags = 0;
};

jvxErrorType
HjvxConfigProcessor_readEntry_nameEntry(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string& entry);

jvxErrorType
HjvxConfigProcessor_readEntry_originEntry(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::string& fileName, jvxInt32& lineno);

jvxErrorType
HjvxConfigProcessor_readEntry_originEntry(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string& fileName, jvxInt32& lineno);

jvxErrorType
HjvxConfigProcessor_readEntries_originEntry(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::vector<oneOriginCfgEntry>& origins);

jvxErrorType
HjvxConfigProcessor_readEntry_references(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::string& nmKey, 
	jvxBool* isAbsolute, std::vector<std::string>& lstReference);

jvxErrorType
HjvxConfigProcessor_readEntries_assignmentStringList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, const std::string& nmToken, std::vector<std::string>& entries,
	std::vector< oneFlagSetEntry>* flags = nullptr, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT);

template <typename T> jvxErrorType
HjvxConfigProcessor_readEntry_assignment(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, T* entries, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxValue valD = 0;

	if (theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{
				res = theReader->getAssignmentValue(datTmp, &valD);
				if (res == JVX_NO_ERROR)
				{
					if (entries)
					{
						res = valD.toContent(entries);
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

template <typename T> jvxErrorType
HjvxConfigProcessor_readEntry_1dList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, jvxSize idx, T* entries, jvxSize numValues, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* datTmp = NULL;
	jvxSize num = 0;
	jvxApiValueList lst;
	jvxConfigSectionTypes tp;
	jvxValue oneVal;
	if (theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{
				res = theReader->getTypeCurrentEntry(datTmp, &tp);
				if (tp == JVX_CONFIG_SECTION_TYPE_VALUELIST)
				{
					res = theReader->getNumberValueLists(datTmp, &num);
					if (res == JVX_NO_ERROR)
					{
						if (idx < num)
						{
							theReader->getValueList_id(datTmp, &lst, idx);
							if (entries)
							{
								for (i = 0; i < lst.ll(); i++)
								{
									if (i < numValues)
									{
										lst.elm_at(i).toContent(&entries[i]);
									}
									else
									{
										break;
									}
								}
							}
						}
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;
						}
					}
				}
				else if (tp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE)
				{
					if (entries)
					{
						if (idx == 0)
						{
							if (numValues)
							{
								res = theReader->getAssignmentValue(datTmp, &oneVal);
								if (res == JVX_NO_ERROR)
								{
									oneVal.toContent(&entries[0]);
								}
								else
								{
									res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
								}
							}
							else
							{
								res = JVX_ERROR_ID_OUT_OF_BOUNDS;
							}
						}
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;
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
  
template <typename T> jvxErrorType
HjvxConfigProcessor_readEntry_1dList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, T* entries, jvxSize numValues, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
{
	return(HjvxConfigProcessor_readEntry_1dList<T>(theReader, theSection, nmToken, 0, entries, numValues, acc_flags, cfg_flags, whattodo));
}

/**********************************************************************************
 * Read single entries in config file 
 *********************************************************************************/

jvxErrorType
HjvxConfigProcessor_readEntry_assignmentBitField(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, jvxBitField* entry, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT);

template <typename T>  jvxErrorType
HjvxConfigProcessor_readEntry_assignmentSpec(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, T* entry, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxValue valD = 0;
	jvxInt16 valI16;
	if(theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{
				res = theReader->getAssignmentValue(datTmp, &valD);
				if (res == JVX_NO_ERROR)
				{
					if (entry)
					{
						valD.toContent(&valI16);
						*entry = (T)(valI16);
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

jvxErrorType
HjvxConfigProcessor_readEntry_1dList_allocate(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken,  jvxApiValueList* entries, jvxSize* num, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT);

/**********************************************************************************
 * Read array numeric entries in config file, size known in advance 
 *********************************************************************************/

template <typename T>  jvxErrorType
HjvxConfigProcessor_readEntry_1dList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, jvxSize idx, std::vector<T>& entries, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxSize num = 0;
	jvxApiValueList lst;
	T val;
	jvxValue oneVal;
	jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	entries.clear();

	if (theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{
				res = theReader->getTypeCurrentEntry(datTmp, &tp);
				if (tp == JVX_CONFIG_SECTION_TYPE_VALUELIST)
				{
					res = theReader->getNumberValueLists(datTmp, &num);
					if (res == JVX_NO_ERROR)
					{
						if (idx < num)
						{
							theReader->getValueList_id(datTmp, &lst, idx);
							for (i = 0; i < lst.ll(); i++)
							{
								lst.elm_at(i).toContent(&val);
								entries.push_back(val);
							}
						}
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;
						}
					}
				}
				else if (tp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE)
				{
					if (idx == 0)
					{
						res = theReader->getAssignmentValue(datTmp, &oneVal);
						if (res == JVX_NO_ERROR)
						{
							oneVal.toContent(&val);
							entries.push_back(val);
						}
						else
						{
							res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
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

template <typename T>  jvxErrorType
HjvxConfigProcessor_readEntry_1dList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::vector<T>& entries, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
{
	return(HjvxConfigProcessor_readEntry_1dList<T>(theReader, theSection, nmToken, 0, entries));
}

template <typename T>  jvxErrorType
HjvxConfigProcessor_readEntry_1dSpecList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, const std::string& nmToken, std::vector<T>& entries, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxSize num = 0;
	jvxApiValueList lst;
	jvxInt16 valI16 = 0;
	jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	jvxValue oneVal;
	entries.clear();

	if(theReader && theSection)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
		{
			theReader->getReferenceEntryCurrentSection_name(theSection, &datTmp, nmToken.c_str());
			if (datTmp)
			{
				res = theReader->getTypeCurrentEntry(datTmp, &tp);
				if (tp == JVX_CONFIG_SECTION_TYPE_VALUELIST)
				{
					res = theReader->getNumberValueLists(datTmp, &num);
					if (res == JVX_NO_ERROR)
					{
						if (0 < num)
						{
							theReader->getValueList_id(datTmp, &lst, 0);
							for (i = 0; i < lst.ll(); i++)
							{
								lst.elm_at(i).toContent(&valI16);
								entries.push_back((T)(valI16));
							}
						}
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;
						}
					}
				}
				else if (tp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE)
				{
					res = theReader->getAssignmentValue(datTmp, &oneVal);
					if (res == JVX_NO_ERROR)
					{
						oneVal.toContent(&valI16);
						entries.push_back((T)valI16);
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
 * Read string/string list entries in config file 
 *********************************************************************************/
 jvxErrorType
HjvxConfigProcessor_readEntry_assignmentStringList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::vector<std::string>* entries, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT);

 jvxErrorType
HjvxConfigProcessor_readEntry_assignmentSelectionList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, 
	std::string nmToken, jvxSelectionList_cpp* entries, bool onlySelectionToConfig = false, jvxBool ignoreProblems = true,
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT,
	std::vector<std::string>* entriesOnReturnIfNotAssigned = nullptr);

jvxErrorType
HjvxConfigProcessor_readEntry_assignmentString(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, std::string* entry, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT);

jvxErrorType
HjvxConfigProcessor_readEntry_assignmentValueInRange(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, jvxValueInRange* entries, bool onlySelectionToConfig = false, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT);

template <typename T> jvxErrorType
HjvxConfigProcessor_readEntry_1dList_allocate_circ(IjvxConfigProcessor* theReader, jvxConfigData* theSection, std::string nmToken, T** entries, jvxSize* numValues, jvxSize& idxEntry, 
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL, jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxSize num = 0;
	jvxApiValueList lst;

	if (theReader && theSection && entries && numValues)
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
						idxEntry = idxEntry % num;
						theReader->getValueList_id(datTmp, &lst, idxEntry);
						JVX_DSP_SAFE_ALLOCATE_FIELD(*entries, T, lst.ll());
						*numValues = lst.ll();
						for (i = 0; i < lst.ll(); i++)
						{
						  lst.elm_at(i).toContent(&(*entries)[i]);
						}
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

#endif
