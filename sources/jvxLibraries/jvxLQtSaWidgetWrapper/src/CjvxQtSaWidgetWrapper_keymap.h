#ifndef __CJVXQTSAWIDGETWRAPPER_KEYMAP_H__
#define __CJVXQTSAWIDGETWRAPPER_KEYMAP_H__

typedef enum
{
	JVX_WW_KEY_VALUE_TYPE_INT32 = 0,
	JVX_WW_KEY_VALUE_TYPE_DATA = 1,
	JVX_WW_KEY_VALUE_TYPE_STRING = 2,
	JVX_WW_KEY_VALUE_TYPE_ENUM = 3,
	JVX_WW_KEY_VALUE_TYPE_BOOL = 4
} jvxWwKeyValueType;

typedef struct
{
	std::string translator;
	jvxSize id;
} jvxWwTranslator;

typedef struct  
{
	std::string selector;
	jvxWwKeyValueType type;
	jvxHandle* value;
	jvxHandle* dvalue;
	std::vector<jvxWwTranslator> translators;
	jvxBool foundit;
} oneKeyValue;


class keyValueList
{
	jvxInt32* ptrI;
	jvxBool* ptrB;
	jvxData* ptrF;
	std::string* ptrS;
	std::vector<oneKeyValue> registeredElements;
	std::string lastErrorToken;
public:

	keyValueList() 
	{
		ptrI = NULL;
		ptrB = NULL;
		ptrF = NULL;
		ptrS = NULL;
	};

	~keyValueList()
	{
		jvxSize i;
		for (i = 0; i < registeredElements.size(); i++)
		{
			switch (registeredElements[i].type)
			{
			case JVX_WW_KEY_VALUE_TYPE_INT32:
				ptrI = (jvxInt32*)registeredElements[i].value;
				delete ptrI;
				ptrI = (jvxInt32*)registeredElements[i].dvalue;
				delete ptrI;
				break;
			case JVX_WW_KEY_VALUE_TYPE_DATA:
				ptrF = (jvxData*)registeredElements[i].value;
				delete ptrF;
				ptrF = (jvxData*)registeredElements[i].dvalue;
				delete ptrF;
				break;
			case JVX_WW_KEY_VALUE_TYPE_STRING:
				ptrS = (std::string*)registeredElements[i].value;
				delete ptrS;
				ptrS = (std::string*)registeredElements[i].dvalue;
				delete ptrS;
				break;
			case JVX_WW_KEY_VALUE_TYPE_ENUM:
				ptrI = (jvxInt32*)registeredElements[i].value;
				delete ptrI;
				ptrI = (jvxInt32*)registeredElements[i].dvalue;
				delete ptrI;
				break;
			case JVX_WW_KEY_VALUE_TYPE_BOOL:
				ptrB = (jvxBool*)registeredElements[i].value;
				delete ptrB;
				ptrB = (jvxBool*)registeredElements[i].dvalue;
				delete ptrB;
				break;
			}
		}
		registeredElements.clear();
	};
	
	jvxErrorType registerElement(std::string key, jvxWwKeyValueType tp, jvxHandle* defValue)
	{
		jvxInt32* ptrI = NULL;
		jvxBool* ptrB = NULL;
		jvxData* ptrF = NULL;
		std::string* ptrS = NULL;

		std::vector<oneKeyValue>::iterator sel =
			jvx_findItemSelectorInList<oneKeyValue, std::string>(registeredElements, key);
		if (sel == registeredElements.end())
		{
			oneKeyValue newElm;
			newElm.selector = key;
			newElm.type = tp;
			switch (tp)
			{
			case JVX_WW_KEY_VALUE_TYPE_INT32:
			case JVX_WW_KEY_VALUE_TYPE_ENUM:
				ptrI = new jvxInt32;
				*ptrI = 0;
				newElm.value = ptrI;
				ptrI = new jvxInt32;
				*ptrI = 0;
				if (defValue)
				{
					*ptrI = *(jvxInt32*)defValue;
				}
				newElm.dvalue = ptrI;
				break;
			case JVX_WW_KEY_VALUE_TYPE_DATA:
				ptrF = new jvxData;
				*ptrF = 0;
				newElm.value = ptrF;
				ptrF = new jvxData;
				*ptrF = 0;
				if (defValue)
				{
					*ptrF = *((jvxData*)defValue);
				}
				newElm.dvalue = ptrF;
				break;
			case JVX_WW_KEY_VALUE_TYPE_STRING:
				ptrS = new std::string;
				newElm.value = ptrS;
				ptrS = new std::string;
				if (defValue)
				{
					*ptrS = *((std::string*)defValue);
				}
				newElm.dvalue = ptrS;
				break;
			case JVX_WW_KEY_VALUE_TYPE_BOOL:
				ptrB = new jvxBool;
				newElm.value = ptrB;
				ptrB = new jvxBool;
				if (defValue)
				{
					*ptrB = *((jvxBool*)defValue);
				}
				newElm.dvalue = ptrB;
				break;
			default:
				assert(0);
			}
			registeredElements.push_back(newElm);
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ALREADY_IN_USE;
	};

	jvxErrorType registerTranslator(std::string key, std::string transl, jvxSize id)
	{
		jvxErrorType res = JVX_NO_ERROR;

		std::vector<oneKeyValue>::iterator sel =
			jvx_findItemSelectorInList<oneKeyValue, std::string>(registeredElements, key);
		if (sel != registeredElements.end())
		{
			jvxWwTranslator newElm;
			newElm.translator = transl;
			newElm.id = id;
			sel->translators.push_back(newElm);
		}
		else
		{
			jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return res;
	};

	jvxErrorType parseEntryList(std::string entry, std::string entryFor)
	{
		jvxErrorType res = JVX_NO_ERROR;
		std::vector<std::string> paramlst;

		res = jvx_parsePropertyStringToKeylist(entry, paramlst);
		return(parseEntryList(paramlst, entry));
	};

	jvxErrorType parseEntryList(std::vector<std::string>& paramlst, std::string entryFor)
	{
		jvxSize i, j;
		jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
		std::string sprop;
		jvxBool foundit = false;
		lastErrorToken = "";
		std::vector<jvxBool> matched;
		jvxSize matchedId = 0;
		for (i = 0; i < registeredElements.size(); i++)
		{
			switch (registeredElements[i].type)
			{
			case JVX_WW_KEY_VALUE_TYPE_INT32:
			case JVX_WW_KEY_VALUE_TYPE_ENUM:
				*((jvxInt32*)registeredElements[i].value) = *((jvxInt32*)registeredElements[i].dvalue);
				break;
			case JVX_WW_KEY_VALUE_TYPE_DATA:
				*((jvxData*)registeredElements[i].value) = *((jvxData*)registeredElements[i].dvalue);
				break;
			case JVX_WW_KEY_VALUE_TYPE_STRING:
				*((std::string*)registeredElements[i].value) = *((std::string*)registeredElements[i].dvalue);
				break;
			case JVX_WW_KEY_VALUE_TYPE_BOOL:
				*((jvxBool*)registeredElements[i].value) = *((jvxBool*)registeredElements[i].dvalue);
				break;
			}
			registeredElements[i].foundit = false;
		}

		for (i = 0; i < paramlst.size(); i++)
		{
			matched.push_back(false);
		}
		for (i = 0; i < registeredElements.size(); i++)
		{
			resL = jvx_parsePropertyKeyList(paramlst, registeredElements[i].selector, sprop, &matchedId);
			if (resL == JVX_NO_ERROR)
			{
				matched[matchedId] = true;
				registeredElements[i].foundit = true;
				switch (registeredElements[i].type)
				{
				case JVX_WW_KEY_VALUE_TYPE_INT32:
					*((jvxInt32*)registeredElements[i].value) = atoi(sprop.c_str());
					break;
				case JVX_WW_KEY_VALUE_TYPE_DATA:
					*((jvxData*)registeredElements[i].value) = atof(sprop.c_str());
					break;
				case JVX_WW_KEY_VALUE_TYPE_STRING:
					*((std::string*)registeredElements[i].value) = sprop;
					break;
				case JVX_WW_KEY_VALUE_TYPE_BOOL:
					if (sprop == "yes")
						*((jvxBool*)registeredElements[i].value) = true;
					else if(sprop == "no")
						*((jvxBool*)registeredElements[i].value) = false;
					else
					{
						*((jvxBool*)registeredElements[i].value) = false;
						std::cout << "Specification of a BOOLEAN specification for assignment <" <<
							registeredElements[i].selector << "> with expression <" << sprop <<
							"> is not valid. Assuming <no>." << std::endl;
					}
					break;
				case JVX_WW_KEY_VALUE_TYPE_ENUM:

					foundit = false;

					for (j = 0; j < registeredElements[i].translators.size(); j++)
					{
						if (sprop == registeredElements[i].translators[j].translator)
						{
							*((jvxInt32*)registeredElements[i].value) = (jvxInt32)registeredElements[i].translators[j].id;
							foundit = true;
							break;
						}
					}
					if (!foundit)
					{
						lastErrorToken = sprop;
						res = JVX_ERROR_ELEMENT_NOT_FOUND;
					}
					break;
				}
			}
		}

		for (i = 0; i < paramlst.size(); i++)
		{
			if (matched[i] == false)
			{
				std::cout << "Warning when scanning entry " << entryFor << ": Failed to match entry " << paramlst[i] << std::endl;
			}
		}
		return(res);
	};

	jvxErrorType getValueForKey(std::string key, jvxHandle* fldRet, jvxWwKeyValueType tp, jvxBool* foundit = NULL)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (foundit)
			*foundit = false;
		std::vector<oneKeyValue>::iterator sel =
			jvx_findItemSelectorInList<oneKeyValue, std::string>(registeredElements, key);
		if (sel != registeredElements.end())
		{
			if (sel->type == tp)
			{
				if (foundit)
				{
					if (sel->foundit)
					{
						// The default value is only used if the value was specified. Otherwise,
						// no value is returned. Hence, make sure, the variable was initialized!
						switch (sel->type)
						{
						case JVX_WW_KEY_VALUE_TYPE_INT32:
						case JVX_WW_KEY_VALUE_TYPE_ENUM:
							*((jvxInt32*)fldRet) = *((jvxInt32*)sel->value);
							break;
						case JVX_WW_KEY_VALUE_TYPE_DATA:
							*((jvxData*)fldRet) = *((jvxData*)sel->value);
							break;
						case JVX_WW_KEY_VALUE_TYPE_STRING:
							*((std::string*)fldRet) = *((std::string*)sel->value);
							break;
						case JVX_WW_KEY_VALUE_TYPE_BOOL:
							*((jvxBool*)fldRet) = *((jvxBool*)sel->value);
							break;
						}
						*foundit = true;
					}
				}
				else
				{
					// If we do not see the found status, the caller may not react properly.
					// There we take over the default value
					switch (sel->type)
					{
					case JVX_WW_KEY_VALUE_TYPE_INT32:
					case JVX_WW_KEY_VALUE_TYPE_ENUM:
						*((jvxInt32*)fldRet) = *((jvxInt32*)sel->value);
						break;
					case JVX_WW_KEY_VALUE_TYPE_DATA:
						*((jvxData*)fldRet) = *((jvxData*)sel->value);
						break;
					case JVX_WW_KEY_VALUE_TYPE_STRING:
						*((std::string*)fldRet) = *((std::string*)sel->value);
						break;
					case JVX_WW_KEY_VALUE_TYPE_BOOL:
						*((jvxBool*)fldRet) = *((jvxBool*)sel->value);
						break;
					}
				}
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
				assert(0);
			}
		}
		else
		{
			jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return res;
	};

	void lastError(std::string& err)
	{
		err = lastErrorToken;
	}
};

#endif
