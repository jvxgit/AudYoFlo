#include "CjvxConfigProcessor.h"

extern int lexAssignFNames(std::vector<std::string> filenames);
extern int lexAssignTBuffer(std::string filenames, std::string filename, int linenoStart);
extern int lexAssignStr(char* buf, int sz);
extern int yyparse();
extern std::string getErrorMessage();
extern int getErrorLinenumber();
extern std::string getErrorFilename();
extern bool associatePostProcessor(CjvxConfigProcessor* ptr);
extern bool deassociatePostProcessor();
extern void yyrestart(FILE*);
extern void addOneIncludePath(std::string path);
extern void clearAllIncludePaths();
extern void setDependencyModus(bool dependencyMode);
extern int lexAssignIfdefs(const std::list<std::string>& ifdefs);
extern void checkLocalFlexError(std::string& locErrorMess, int& locLineNo, std::string& locFName);

#ifdef WRITE_PARSE_RESULTS
FILE* logParseErrors = NULL;
#endif

//=======================================================
//=======================================================

CjvxConfigProcessor::CjvxConfigProcessor(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_CONFIG_PROCESSOR);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigProcessor*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	this->topElementTree = NULL;
	this->errorCode = CjvxConfigProcessor::CONFIG_NOERROR;
	this->errorMessage = "";
	this->fNameError = "UNKNOWN";
#ifdef WRITE_PARSE_RESULTS
	if(logParseErrors == NULL)
		logParseErrors = fopen("logParseErrors.txt", "w");
#endif
}

CjvxConfigProcessor::~CjvxConfigProcessor()
{
	if(topElementTree)
		delete(topElementTree);
	topElementTree = NULL;
}

jvxErrorType
CjvxConfigProcessor::addIncludePath(const char* onePath)
{
	addOneIncludePath(onePath);
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxConfigProcessor::clearIncludePaths()
{
	clearAllIncludePaths();
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxConfigProcessor::addIfdefToken(const char* fName)
{
	ifdefs.push_back(fName);
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::getNumberIncludes(jvxSize* value)
{
	if(value)
	{
		*value = includeFiles.size();
	}

	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxConfigProcessor::getInclude_id(jvxApiString* value, jvxSize idx)
{
	if(idx < includeFiles.size())
	{
		if (value)
		{
			value->assign(includeFiles[idx]);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
}
	
jvxErrorType
CjvxConfigProcessor::getNumberIfdefs(jvxSize* value)
{
	if(value)
	{
		*value = ifdefs.size();
	}

	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxConfigProcessor::getIfdef_id(jvxApiString* value, jvxSize idx)
{
	if(idx < includeFiles.size())
	{
		if (value)
		{
			value->assign(includeFiles[idx]);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
}

jvxErrorType
CjvxConfigProcessor::parseFile(const char* fName, jvxBool clearIncludePathsOnFinish)
{
	jvxSize i;
	std::string locErrorMess;
	int locLineNo;
	std::string locFName;
	std::vector<std::string> filenames;
	filenames.push_back(fName);
	
	std::vector<std::string> localpaths;

	this->errorMessage = "";
	this->lineError = -1;
	this->fNameError = "";
	this->errorCode = CjvxConfigProcessor::CONFIG_NOERROR;

	for (i = 0; i < filenames.size(); i++)
	{
		std::string pathStrFile = filenames[i];
		std::string pathStr;
		size_t posi = pathStrFile.rfind(JVX_SEPARATOR_DIR);
		if (posi != std::string::npos)
		{
			pathStr = pathStrFile.substr(0, posi);
		}
		else
		{
			posi = pathStrFile.rfind(JVX_SEPARATOR_DIR_THE_OTHER);
			if (posi != std::string::npos)
			{
				pathStr = pathStrFile.substr(0, posi);
			}
		}
		if (!pathStr.empty())
		{
			addIncludePath(pathStr.c_str());
		}
	}
	lexAssignIfdefs(ifdefs);

	if(lexAssignFNames(filenames))
	{
#ifdef WRITE_PARSE_RESULTS
		if(logParseErrors)
		{
			int  i;

			fprintf(logParseErrors, ">>>>>>>>>>>>>> START ======================\n");
			for(i = 0; i < filenames.size(); i++)
			{
				fprintf(logParseErrors, "Parsing file %s\n", filenames[i].c_str());
			}
		}
#endif
		associatePostProcessor(this);

		includeFiles.clear();

		// Is that what we need???
		yyrestart(NULL);
		if(yyparse() == 0)
		{
			if (clearIncludePathsOnFinish)
			{
				this->clearIncludePaths();
			}
			checkLocalFlexError(locErrorMess, locLineNo, locFName);
			if(!locErrorMess.empty())
			{
				this->errorCode = CjvxConfigProcessor::CONFIG_PARSEERROR;
				this->errorMessage = locErrorMess;
				this->lineError = locLineNo;
				this->fNameError = locFName;
				return(JVX_ERROR_PARSE_ERROR);
			}

#ifdef WRITE_PARSE_RESULTS
			if(logParseErrors)
			{
				fprintf(logParseErrors, "Parsing successfully completed.\n");
				fprintf(logParseErrors, "<<<<<<<<<<<<<< STOP ======================\n");
			}
#endif
			this->errorCode = CjvxConfigProcessor::CONFIG_NOERROR;
			deassociatePostProcessor();
			return(JVX_NO_ERROR);
		}
		else
		{
			if (clearIncludePathsOnFinish)
			{
				this->clearIncludePaths();
			}

			this->errorMessage = getErrorMessage();
			this->lineError = getErrorLinenumber();
			this->fNameError = getErrorFilename();
			this->errorCode = CjvxConfigProcessor::CONFIG_PARSEERROR;
			deassociatePostProcessor();

#ifdef WRITE_PARSE_RESULTS
			if(logParseErrors)
			{
				fprintf(logParseErrors, "Parse ended with error!\n");
				fprintf(logParseErrors, "Error Message: %s\n", this->errorMessage.c_str());
				fprintf(logParseErrors, "<<<<<<<<<<<<<< STOP ======================\n");
			}
#endif
			return(JVX_ERROR_PARSE_ERROR);
		}
	}
	this->errorCode = CjvxConfigProcessor::CONFIG_FILENOTEXISTSERROR;
	this->errorMessage = getErrorMessage();
	return(JVX_ERROR_OPEN_FILE_FAILED);
}

jvxErrorType
CjvxConfigProcessor::parseTextField(const char* txt, const char* fName, int lineno)
{
	std::string locErrorMess;
	int locLineNo;
	std::string locFName;

	this->errorMessage = "";
	this->lineError = -1;
	this->fNameError = "";

	this->errorCode = CjvxConfigProcessor::CONFIG_NOERROR;
	if(lexAssignTBuffer(txt, fName, lineno))
	{
#ifdef WRITE_PARSE_RESULTS
		if(logParseErrors)
		{
			fprintf(logParseErrors, ">>>>>>>>>>>>>> START ======================\n");
			fprintf(logParseErrors, "Parsing text token: \n %s \n", txt);
		}
#endif
		associatePostProcessor(this);

		yyrestart(NULL);
		if(yyparse() == 0)
		{
			deassociatePostProcessor();

			checkLocalFlexError(locErrorMess, locLineNo, locFName);
			if(!locErrorMess.empty())
			{
				this->errorCode = CjvxConfigProcessor::CONFIG_PARSEERROR;
				this->errorMessage = locErrorMess;
				this->lineError = locLineNo;
				this->fNameError = locFName;
				return(JVX_ERROR_PARSE_ERROR);
			}
#ifdef WRITE_PARSE_RESULTS
			if(logParseErrors)
			{
				fprintf(logParseErrors, "Parsing successfully completed.\n");
				fprintf(logParseErrors, "<<<<<<<<<<<<<< STOP ======================\n");
			}
#endif
			return(JVX_NO_ERROR);
		}
		deassociatePostProcessor();
	}

	this->errorMessage = getErrorMessage();
	this->lineError = getErrorLinenumber();
	this->fNameError = getErrorFilename();
	this->errorCode = CjvxConfigProcessor::CONFIG_PARSEERROR;
#ifdef WRITE_PARSE_RESULTS
	if(logParseErrors)
	{
		fprintf(logParseErrors, "ERROR::: Parse ended with error!\n");
		fprintf(logParseErrors, "Error Message: %s\n", this->errorMessage.c_str());
		fprintf(logParseErrors, "<<<<<<<<<<<<<< STOP ======================\n");
	}
#endif
	return(JVX_ERROR_PARSE_ERROR);
}

jvxErrorType
CjvxConfigProcessor::getParseError(jvxApiError* err)
{
	if (err)
	{
		err->errorCode = 0;
		err->errorLevel = 0;
		err->errorDescription.assign(this->errorMessage);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::getConfigurationHandle(jvxConfigData** data)
{
	if(data)
	{
		*data = reinterpret_cast<jvxConfigData*>(this->topElementTree);
		this->topElementTree = NULL;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::getNameCurrentEntry(jvxConfigData* data, jvxApiString* name)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);
	if (name)
	{
		name->assign(elm->nameElement);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::getSectionIsEmpty(jvxConfigData* data, jvxBool* isit)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(isit)
		*isit = elm->isEmpty;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::getTypeCurrentEntry(jvxConfigData* data, jvxConfigSectionTypes* type)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	treeListElement::elementType tp = elm->type;


	if(type)
	{
		switch(tp)
		{
			case treeListElement::SECTION:
				*type = JVX_CONFIG_SECTION_TYPE_SECTION;
				break;
			case treeListElement::VALUELIST:
				*type = JVX_CONFIG_SECTION_TYPE_VALUELIST;
				break;
			case treeListElement::ASSIGNMENTVALUE:
				*type = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
				break;
			case treeListElement::ASSIGNMENTHEXSTRING:
				*type = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTHEXSTRING;
				break;
			case treeListElement::ASSIGNMENTSTRING:
				*type = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING;
				break;
			case treeListElement::STRINGLIST:
				*type = JVX_CONFIG_SECTION_TYPE_STRINGLIST;
				break;
			case treeListElement::REFERENCE:
				*type = JVX_CONFIG_SECTION_TYPE_REFERENCE;
				break;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::getNumberEntriesCurrentSection(jvxConfigData* data, jvxSize* num)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::SECTION)
	{
		if(num)
		{
			*num = elm->getNumberEntries();
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getNumberSubsectionsCurrentSection(jvxConfigData* data, jvxSize* num)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::SECTION)
	{
		if(num)
		{
			*num = elm->getNumberSubsections();
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getReferenceEntryCurrentSection_id(jvxConfigData* dataIn, jvxConfigData** dataOut, jvxSize idx)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(dataIn);

	if(elm->type == treeListElement::SECTION)
	{
		unsigned int num = elm->getNumberEntries();
		if(idx < num)
		{
			elm->getReferenceEntrySection_id(reinterpret_cast<treeListElement**>(dataOut), idx);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getReferenceSubsectionCurrentSection_id(jvxConfigData* dataIn, jvxConfigData** dataOut, jvxSize idx)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(dataIn);

	if(elm->type == treeListElement::SECTION)
	{
		unsigned int num = elm->getNumberSubsections();
		if(idx < num)
		{
			elm->getReferenceSubsectionSection_id(reinterpret_cast<treeListElement**>(dataOut), idx);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getReferenceSubsectionCurrentSection_name(jvxConfigData* dataIn, jvxConfigData** dataOut, const char* nameSection)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(dataIn);

	if(elm->type == treeListElement::SECTION)
	{
		if(!elm->getReferenceSubsectionSection_name(reinterpret_cast<treeListElement**>(dataOut), nameSection))
		{
			return(JVX_ERROR_INTERNAL);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getReferenceEntryCurrentSection_name(jvxConfigData* dataIn, jvxConfigData** dataOut, const char* nameSection)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(dataIn);

	if(elm->type == treeListElement::SECTION)
	{
		if(!elm->getReferenceEntrySection_name(reinterpret_cast<treeListElement**>(dataOut), nameSection))
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType 
CjvxConfigProcessor::getGroupStatus(jvxConfigData* data, jvxBool* value)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::SECTION)
	{
		if(value)
		{
			*value = elm->isGroup;
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}


jvxErrorType
CjvxConfigProcessor::getAssignmentString(jvxConfigData* data, jvxApiString* value)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::ASSIGNMENTSTRING)
	{
		if(elm->isEmpty)
		{
			return(JVX_ERROR_EMPTY_LIST);
		}

		std::string tmp = elm->assignedString;
		//tmp = tmp.substr(1, tmp.size()-2);
		if (value)
		{
			value->assign(tmp);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getAssignmentValue(jvxConfigData* data, jvxValue* value)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm)
	{
		if(elm->type == treeListElement::ASSIGNMENTVALUE)
		{
			if(elm->isEmpty)
			{
				return(JVX_ERROR_EMPTY_LIST);
			}

			if(value)
				*value = elm->value;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_SECTION_TYPE);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}


jvxErrorType
CjvxConfigProcessor::getAssignmentBitField(jvxConfigData* data, jvxBitField* value)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);
	jvxBool err = false;

	if(elm)
	{
		if(elm->type == treeListElement::ASSIGNMENTHEXSTRING)
		{
			if(elm->isEmpty)
			{
				return(JVX_ERROR_EMPTY_LIST);
			}

			if (value)
			{
				*value = jvx_string2BitField(elm->assignedString, err);
				if (err)
				{
					return JVX_ERROR_PARSE_ERROR;
				}
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_SECTION_TYPE);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}




jvxErrorType
CjvxConfigProcessor::getReferencePathType(jvxConfigData* data, jvxBool* isAbsolute)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::REFERENCE)
	{
		if(isAbsolute)
			*isAbsolute = elm->referenceAbsolute;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getReferenceKey(jvxConfigData* data, jvxApiString* key)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::REFERENCE)
	{
		if (key)
		{
			key->assign(elm->referenceKey);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getReferenceNumberPathTokens(jvxConfigData* data, jvxSize* num)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::REFERENCE)
	{
		if(num)
		{
			*num = elm->referenceList.size();
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getReferencePathToken_id(jvxConfigData* data, jvxApiString* token, jvxSize idx)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::REFERENCE)
	{
		if(idx < elm->referenceList.size())
		{
			if (token)
			{
				token->assign(elm->referenceList[idx]);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}


jvxErrorType
CjvxConfigProcessor::getNumberValueLists(jvxConfigData* data, jvxSize* value)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::VALUELIST)
	{
		if(elm->isEmpty)
		{
			return(JVX_ERROR_EMPTY_LIST);
		}
		if(value)
			*value = (jvxInt32)elm->dataArrayList.size();
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getMaxSizeValueLists(jvxConfigData* data, jvxSize* mSize)
{
	jvxSize i;
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if (elm->type == treeListElement::VALUELIST)
	{
		if (elm->isEmpty)
		{
			return(JVX_ERROR_EMPTY_LIST);
		}
		if (mSize)
		{
			*mSize = 0;
			for (i = 0; i < elm->dataArrayList.size(); i++)
			{
				*mSize = JVX_MAX(*mSize, elm->dataArrayList[i].size());
			}
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getValueList_id(jvxConfigData* data, jvxApiValueList* lst, jvxSize idx)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::VALUELIST)
	{
		if(elm->isEmpty)
		{
			return(JVX_ERROR_EMPTY_LIST);
		}
		if(lst)
		{
			unsigned int num = (unsigned int)elm->dataArrayList.size();
			if(idx < num)
			{
				std::vector<jvxValue> lstDes = elm->dataArrayList[idx];
				if (lst)
				{
					lst->assign(lstDes);
				}
				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		else if (elm->type == treeListElement::ASSIGNMENTVALUE)
		{
			if (elm->isEmpty)
			{
				return(JVX_ERROR_EMPTY_LIST);
			}
			if (lst)
			{
				lst->assign(&elm->value, 1);
				return(JVX_NO_ERROR);
			}
		}
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType 
CjvxConfigProcessor::getValueListSize_id(jvxConfigData* data, jvxSize* sz, jvxSize idx)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if (elm->type == treeListElement::VALUELIST)
	{
		if (elm->isEmpty)
		{
			return(JVX_ERROR_EMPTY_LIST);
		}
		if (sz)
		{
			unsigned int num = (unsigned int)elm->dataArrayList.size();
			if (idx < num)
			{
				std::vector<jvxValue> lstDes = elm->dataArrayList[idx];
				*sz = lstDes.size();
				return(JVX_NO_ERROR);
			}
			*sz = 0;
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		else if (elm->type == treeListElement::ASSIGNMENTVALUE)
		{
			if (elm->isEmpty)
			{
				return(JVX_ERROR_EMPTY_LIST);
			}
			if (sz)
			{
				*sz = 1;
				return(JVX_NO_ERROR);
			}
		}
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}
jvxErrorType
CjvxConfigProcessor::getNumberStrings(jvxConfigData* data, jvxSize* value)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::STRINGLIST)
	{
		if(elm->isEmpty)
		{
			return(JVX_ERROR_EMPTY_LIST);
		}
		if(value)
			*value = (jvxInt32)elm->assignedStringList.size();
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getString_id(jvxConfigData* data, jvxApiString* value, jvxSize idx)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(elm->type == treeListElement::STRINGLIST)
	{
		if(elm->isEmpty)
		{
			return(JVX_ERROR_EMPTY_LIST);
		}
		if(idx < elm->assignedStringList.size())
		{
			std::string tmp = elm->assignedStringList[idx];
			//tmp = tmp.substr(1, tmp.size()-2);
			if (value)
			{
				value->assign(tmp);
			}
			return(JVX_NO_ERROR);
		}
	}
	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType 
CjvxConfigProcessor::getBitFieldTags(jvxConfigData* data, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxBool* acc_set,
	jvxConfigModeFlags* cfg_flags, jvxBool* cfg_set)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if (acc_set)
	{
		*acc_set = false;
	}
	if (cfg_set)
	{
		*cfg_set = false;
	}

	if (elm->acc_flags_set)
	{
		if (acc_set)
		{
			*acc_set = true;
		}
		if (acc_flags)
		{
			*acc_flags = elm->acc_flags;
		}
	}
	if (elm->cfg_flags_set)
	{
		if (cfg_set)
		{
			*cfg_set = true;
		}
		if (cfg_flags)
		{
			*cfg_flags = elm->cfg_flags;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::createEmptySectionList(jvxConfigList** lst)
{
	treeList* newList = new treeList;
	if(lst)
	{
		*lst = newList;
		return(JVX_NO_ERROR);
	}
	delete(newList);
	return(JVX_ERROR_EMPTY_LIST);
}

jvxErrorType
CjvxConfigProcessor::addSectionToSectionList(jvxConfigList* lst, jvxConfigData* data)
{
	treeList* newList = reinterpret_cast<treeList*>(lst);
	newList->push_back(reinterpret_cast<treeListElement*>(data));
	return(JVX_NO_ERROR);
}


jvxErrorType
CjvxConfigProcessor::createAssignmentString(jvxConfigData** data, const char* name, const char* assignmentString, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags)
{
	treeListElement* newElement = new treeListElement(acc_flags, cfg_flags);//IjvxConfigProcessor::JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING);
	newElement->setToAssignmentString(assignmentString, name);
	if(data)
		*data = newElement;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::createAssignmentStringList(jvxConfigData** data, const char* name, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags)
{
	treeListElement* newElement = new treeListElement(acc_flags, cfg_flags);//IjvxConfigProcessor::JVX_CONFIG_SECTION_TYPE_STRINGLIST);
	newElement->setToAssignmentStringList(name);
	if(data)
		*data = newElement;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::createAssignmentValue(jvxConfigData** data, const char* name, jvxValue value, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags)
{
	treeListElement* newElement = new treeListElement(acc_flags, cfg_flags);//IjvxConfigProcessor::JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE);
	newElement->setToAssignmentValue(value, name);
	if(data)
		*data = newElement;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::createAssignmentBitField(jvxConfigData** data, const char* name, jvxBitField assignmentBitField, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags)
{
	treeListElement* newElement = new treeListElement(acc_flags, cfg_flags);//IjvxConfigProcessor::JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE);
	newElement->setToAssignmentHexValue(assignmentBitField, name);
	if(data)
		*data = newElement;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::createAssignmentValueList(jvxConfigData** data, const char* name, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags)
{
	treeListElement* newElement = new treeListElement(acc_flags, cfg_flags);//IjvxConfigProcessor::JVX_CONFIG_SECTION_TYPE_VALUELIST);
	newElement->setToAssignmentValueList(name);

	if(data)
		*data = newElement;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::createEmptySection(jvxConfigData** data, const char* name, 
	jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags)
{
	treeListElement* newElement = new treeListElement(acc_flags, cfg_flags);//IjvxConfigProcessor::JVX_CONFIG_SECTION_TYPE_SECTION);
	newElement->setToEmptySection(name);

	if(data)
		*data = newElement;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::createComment(jvxConfigData** data, const char* comment)
{
	treeListElement* newElement = new treeListElement();//IjvxConfigProcessor::JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING);

	newElement->setToComment(comment);

	if(data)
		*data = newElement;
	return(JVX_NO_ERROR);
}


jvxErrorType
CjvxConfigProcessor::addAssignmentValueToList(jvxConfigData* data, int idxArray, jvxValue value)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(!elm->addElementAssignmentValueList(value, idxArray))
	{
		return(JVX_ERROR_INTERNAL);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::isSectionEmpty(jvxConfigData*hdl, jvxBool* isEmpty)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(hdl);
	elm->isSectionEmpty(isEmpty);
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::addAssignmentStringToList(jvxConfigData* data, const char* entry)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(data);

	if(!(elm->addElementAssignmentStringList(entry)))
	{
		return(JVX_ERROR_INTERNAL);
	}
	return(JVX_NO_ERROR);
}


jvxErrorType
CjvxConfigProcessor::addSubsectionToSection(jvxConfigData* toAddTo, jvxConfigData* tobeAdded)
{
	//unsigned i;
	treeListElement* elm = reinterpret_cast<treeListElement*>(toAddTo);
	treeListElement* elmAdd = reinterpret_cast<treeListElement*>(tobeAdded);

	if (!toAddTo)
	{
		return(JVX_ERROR_INVALID_ARGUMENT);
	}
	if(tobeAdded)
	{
		if(elm->type == treeListElement::SECTION)
		{
			/*
			for(i = 0; i < lstReferences.size(); i++)
			{
			// References must not be pasted into subsections, use copies instead
			if(elm == lstReferences[i])
			{
			return(false);
			}
			}
			*/
			elm->subsection->push_back(elmAdd);
			return(JVX_NO_ERROR);
		}
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
CjvxConfigProcessor::attachSectionListToSection(jvxConfigData* toAddTo, jvxConfigList* tobeAdded)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(toAddTo);
	treeList* elmList = reinterpret_cast<treeList*>(tobeAdded);

	if(elm->type == treeListElement::SECTION)
	{
		treeList* oldList = elm->subsection;
		elm->subsection = elmList;
		delete(oldList);
		return(JVX_NO_ERROR);
	}

	return(JVX_ERROR_WRONG_SECTION_TYPE);
}

jvxErrorType
CjvxConfigProcessor::getOriginSection(jvxConfigData* dataIn, jvxApiString* fName, jvxInt32* lineno)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(dataIn);

	std::string str;
	int line = -1;
	elm->getOriginSection(str, line);
	if (fName)
	{
		fName->assign(str);
	}
	if(lineno)
		*lineno = line;
	return(JVX_NO_ERROR);
}


jvxErrorType
CjvxConfigProcessor::printConfiguration(jvxConfigData* print, jvxApiString* str, bool compactForm)
{
	treeListElement* elm = reinterpret_cast<treeListElement*>(print);

	std::string buf;
	elm->outputToString(buf, 0, compactForm);
	//std::cout << buf << std::endl;
	if (str)
	{
		str->assign(buf);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConfigProcessor::removeHandle(jvxConfigData* toBeRemoved)
{
//	int i;

	treeListElement* elm = reinterpret_cast<treeListElement*>(toBeRemoved);

/*
	if(elm == this->topElementTree)
	{
		return(false);
	}
	for(i = 0; i < (int)lstReferences.size(); i++)
	{
		if(elm == lstReferences[i])
		{
			return(false);
		}
	}
	*/
	delete(elm);

	return(JVX_NO_ERROR);
}

void 
CjvxConfigProcessor::addIncludedFile(std::string fName)
{
	includeFiles.push_back(fName);
}

bool 
CjvxConfigProcessor::garbage_allocate_collect(jvxReturnToken** oneInst)
{
	*oneInst = new jvxReturnToken;
	garbage.push_back(*oneInst);
	return true;
}

bool 
CjvxConfigProcessor::garbage_release(jvxReturnToken* oneInst)
{
	auto elm = std::find(garbage.begin(), garbage.end(), oneInst);
	if (elm != garbage.end())
	{
		delete oneInst;
		garbage.erase(elm);
		return true;
	}
	return false;
}

void 
CjvxConfigProcessor::garbage_clear_all()
{
	auto elm = garbage.begin();
	for (; elm != garbage.end(); elm++)
	{
		delete* elm;
	}
	garbage.clear();
}

