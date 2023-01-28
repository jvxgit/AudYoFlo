#include "jvx-helpers.h"

/**********************************************************************************
 * Write single entries in config file
 *********************************************************************************/

jvxErrorType
HjvxConfigProcessor_writeEntry_assignmentBitField(IjvxConfigProcessor* theReader, jvxConfigData* theSection, 
	const std::string& nmToken, jvxBitField* entry,
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags,
	jvxCBitField whattodo )
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxData valD = 0;

	jvxAccessRightFlags_rwcd* acc_flags_ptr = NULL;
	jvxConfigModeFlags* cfg_flags_ptr = NULL;

	if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_ACCESS_RIGHTS)
	{
		acc_flags_ptr = acc_flags;
	}
	if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONFIG_MODE)
	{
		cfg_flags_ptr = cfg_flags;
	}

	if(theReader && theSection && entry)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
		{
			theReader->createAssignmentBitField(&datTmp, nmToken.c_str(), *entry, acc_flags_ptr, cfg_flags_ptr);

			/*
			if(*entry > ((jvxUInt64)1 << (JVX_MAX_NUM_DIGITS_INT_IN_FLOAT)) -1)
			{
				*entry = (jvxBitField)((jvxUInt64)1 << (JVX_MAX_NUM_DIGITS_INT_IN_FLOAT)) -1;
				std::cout << "Warning: 64 Bit value ("<<nmToken<<") had to be shortened for storage in config file, value=" << *entry << std::endl;
			}

			theReader->createAssignmentValue(&datTmp, nmToken.c_str(), *entry);
			*/
			if (datTmp)
			{
				theReader->addSubsectionToSection(theSection, datTmp);
			}
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

/**********************************************************************************
 * Write array numeric entries in config file
 *********************************************************************************/



/**********************************************************************************
 * Write array numeric entries in config file with STL
 *********************************************************************************/


/**********************************************************************************
 * Write string / string list entries to config file
 *********************************************************************************/
jvxErrorType
HjvxConfigProcessor_writeEntry_assignmentStringList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, const std::string& nmToken, 
	std::vector<std::string>* entries,
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags,
	jvxCBitField whattodo)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	jvxSize num = 0;

	jvxAccessRightFlags_rwcd* acc_flags_ptr = NULL;
	jvxConfigModeFlags* cfg_flags_ptr = NULL;

	if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_ACCESS_RIGHTS)
	{
		acc_flags_ptr = acc_flags;
	}
	if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONFIG_MODE)
	{
		cfg_flags_ptr = cfg_flags;
	}

	if(theReader && theSection && entries)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
		{
			theReader->createAssignmentStringList(&datTmp, nmToken.c_str(), acc_flags_ptr, cfg_flags_ptr);
			if (datTmp)
			{
				for (i = 0; i < entries->size(); i++)
				{
					theReader->addAssignmentStringToList(datTmp, (*entries)[i].c_str());
				}
				theReader->addSubsectionToSection(theSection, datTmp);
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
HjvxConfigProcessor_writeEntry_assignmentSelectionList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, const std::string& nmToken, 
	jvxSelectionList_cpp* entries, bool onlySelectionToFile ,
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags,
	jvxCBitField whattodo)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL, * datTmp1 = NULL;
	jvxSize num = 0;
	jvxSize i;
	jvxFlagTag ft = 0;
	
	jvxAccessRightFlags_rwcd* acc_flags_ptr = NULL;
	jvxConfigModeFlags* cfg_flags_ptr = NULL;

	if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_ACCESS_RIGHTS)
	{
		acc_flags_ptr = acc_flags;
	}
	if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONFIG_MODE)
	{
		cfg_flags_ptr = cfg_flags;
	}

	if(theReader && theSection && entries)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
		{
			theReader->createEmptySection(&datTmp, nmToken.c_str(), acc_flags_ptr, cfg_flags_ptr);
			if (datTmp)
			{
				if (!onlySelectionToFile)
				{
					HjvxConfigProcessor_writeEntry_assignmentStringList(theReader, datTmp, JVX_TOKEN_CONFIG_SELECTIONLIST_NAMES, &entries->entries);
					HjvxConfigProcessor_writeEntry_assignmentBitField(theReader, datTmp, JVX_TOKEN_CONFIG_EXCLUSIVE_VALUE, (jvxBitField*)&entries->exclusive);
				}
				else
				{
					std::vector<std::string> entries_write;
					for(i = 0; i < entries->entries.size(); i++)
					{
						if (jvx_bitTest(entries->selection(), i))
						{
							entries_write.push_back(entries->entries[i]);
						}
					}
					HjvxConfigProcessor_writeEntry_assignmentStringList(theReader, datTmp, JVX_TOKEN_CONFIG_SELECTIONLIST_NAMES_SELECTED, &entries_write);
				}
				HjvxConfigProcessor_writeEntry_assignmentBitField(theReader, datTmp, JVX_TOKEN_CONFIG_SELECTION_VALUE, (jvxBitField*)&entries->selection());


				theReader->addSubsectionToSection(theSection, datTmp);
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
HjvxConfigProcessor_writeEntry_assignmentString(IjvxConfigProcessor* theReader, jvxConfigData* theSection, const std::string& nmToken, std::string* entry,
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags,
	jvxCBitField whattodo)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	
	jvxAccessRightFlags_rwcd* acc_flags_ptr = NULL;
	jvxConfigModeFlags* cfg_flags_ptr = NULL;

	if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_ACCESS_RIGHTS)
	{
		acc_flags_ptr = acc_flags;
	}
	if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONFIG_MODE)
	{
		cfg_flags_ptr = cfg_flags;
	}


	if(theReader && theSection && entry)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
		{
			theReader->createAssignmentString(&datTmp, nmToken.c_str(), entry->c_str(), acc_flags_ptr, cfg_flags_ptr);
			theReader->addSubsectionToSection(theSection, datTmp);
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType
HjvxConfigProcessor_writeEntry_assignmentValueInRange(IjvxConfigProcessor* theReader, jvxConfigData* theSection, const std::string& nmToken, jvxValueInRange* entries, bool onlySelectionToFile,
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags,
	jvxCBitField whattodo)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL, * datTmp1 = NULL;
	jvxSize num = 0;

	jvxAccessRightFlags_rwcd* acc_flags_ptr = NULL;
	jvxConfigModeFlags* cfg_flags_ptr = NULL;

	if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_ACCESS_RIGHTS)
	{
		acc_flags_ptr = acc_flags;
	}
	if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONFIG_MODE)
	{
		cfg_flags_ptr = cfg_flags;
	}


	if(theReader && theSection && entries)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
		{
			theReader->createEmptySection(&datTmp, nmToken.c_str(), acc_flags_ptr, cfg_flags_ptr);
			if (datTmp)
			{
				if (!onlySelectionToFile)
				{
					HjvxConfigProcessor_writeEntry_assignment<jvxData>(theReader, datTmp, JVX_TOKEN_CONFIG_VALUE_IN_RANGE_MIN, &entries->minVal);
					HjvxConfigProcessor_writeEntry_assignment<jvxData>(theReader, datTmp, JVX_TOKEN_CONFIG_VALUE_IN_RANGE_MAX, &entries->maxVal);
				}
				HjvxConfigProcessor_writeEntry_assignment<jvxData>(theReader, datTmp, JVX_TOKEN_CONFIG_VALUE_IN_RANGE, &entries->val());

				theReader->addSubsectionToSection(theSection, datTmp);
			}
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

