#ifndef __HJVXWRITECONFIGENTRIES_H__
#define __HJVXWRITECONFIGENTRIES_H__

#include "jvx-helpers.h"

/**********************************************************************************
 * Write single entries in config file
 *********************************************************************************/

template<typename T> jvxErrorType
HjvxConfigProcessor_writeEntry_assignment(IjvxConfigProcessor* theReader, 
	jvxConfigData* theSection, const std::string& nmToken, 
	T* entry,
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL,
	jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;
	// jvxData valD = 0;

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

	jvxValue oneElm(*entry);
	if (theReader && theSection && entry)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
		{
			theReader->createAssignmentValue(&datTmp, nmToken.c_str(), *entry, acc_flags_ptr, cfg_flags_ptr);
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
};


jvxErrorType
HjvxConfigProcessor_writeEntry_assignmentBitField(IjvxConfigProcessor* theReader, jvxConfigData* theSection, 
	const std::string& nmToken, jvxBitField* entry,
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL,
	jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT);

/**********************************************************************************
 * Write array numeric entries in config file
 *********************************************************************************/

template<typename T> jvxErrorType
HjvxConfigProcessor_writeEntry_1dList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, 
	const std::string& nmToken, T* entries, jvxSize numValues,
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL,
	jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* datTmp = NULL;

	// jvxSize num = 0;
//	jvxDataList* lst = NULL;

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

	if (theReader && theSection && entries)
	{
		if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
		{

			theReader->createAssignmentValueList(&datTmp, nmToken.c_str(), acc_flags_ptr, cfg_flags_ptr);
			if (datTmp)
			{
				for (i = 0; i < numValues; i++)
				{
					theReader->addAssignmentValueToList(datTmp, 0, jvxValue(entries[i]));
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
};

/**********************************************************************************
 * Write array numeric entries in config file with STL
 *********************************************************************************/

template<typename T> jvxErrorType
HjvxConfigProcessor_writeEntry_1dList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, 
	const std::string& nmToken, std::vector<T>& entries,
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL,
	jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
{
	{
		jvxSize i;
		jvxErrorType res = JVX_NO_ERROR;

		jvxConfigData* datTmp = NULL;
		// jvxSize num = 0;
//		jvxDataList* lst = NULL;

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

		if (theReader && theSection)
		{
			if (whattodo & JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT)
			{

				if (entries.size() == 1)
				{
					theReader->createAssignmentValue(&datTmp, nmToken.c_str(), jvxValue(entries[0]), acc_flags_ptr, cfg_flags_ptr);
					if (datTmp)
					{
						theReader->addSubsectionToSection(theSection, datTmp);
					}
				}
				else
				{
					theReader->createAssignmentValueList(&datTmp, nmToken.c_str(), acc_flags_ptr, cfg_flags_ptr);
					if (datTmp)
					{
						for (i = 0; i < entries.size(); i++)
						{
							theReader->addAssignmentValueToList(datTmp, 0, jvxValue(entries[i]));
						}
						theReader->addSubsectionToSection(theSection, datTmp);
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
};

/**********************************************************************************
 * Write string / string list entries to config file
 *********************************************************************************/
jvxErrorType
HjvxConfigProcessor_writeEntry_assignmentStringList(IjvxConfigProcessor* theReader, 
	jvxConfigData* theSection, const std::string& nmToken, 
	std::vector<std::string>* entries,
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL,
	jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT);

jvxErrorType
HjvxConfigProcessor_writeEntry_assignmentSelectionList(IjvxConfigProcessor* theReader, jvxConfigData* theSection, const std::string& nmToken, 
	jvxSelectionList_cpp* entries, bool onlySelectionToFile = false,
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL,
	jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT);

jvxErrorType
HjvxConfigProcessor_writeEntry_assignmentString(IjvxConfigProcessor* theReader, jvxConfigData* theSection, 
	const std::string& nmToken, std::string* entry,
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL,
	jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT);

jvxErrorType
HjvxConfigProcessor_writeEntry_assignmentValueInRange(IjvxConfigProcessor* theReader, 
	jvxConfigData* theSection, const std::string& nmToken, jvxValueInRange* entries,
	bool onlySelectionToFile = false,
	jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL,
	jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT);

#endif
