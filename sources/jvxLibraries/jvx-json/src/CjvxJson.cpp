#include "CjvxJson.h"
#include "jsmn.h"

#define JVX_DEFAULT_PRECISION 2
#define JVX_MARKER_FLAT_SWITCH_POINT "X"

#define JVX_WRITE_RETURN_INDENTATION(stradd, indent, indToken, initToken) \
	stradd += "\n"; \
	stradd += initToken; \
	for (i = 0; i < indent; i++) \
	{ \
		stradd += indToken; \
	}

#define JVX_WRITE_INDENTATION(stradd, indent, indToken, initToken) \
	stradd += initToken; \
	for (i = 0; i < indent; i++) \
	{ \
		stradd += indToken; \
	}
#define JVX_WRITE_RETURN(stradd, return_token) \
	stradd += return_token;

// ===========================================================================================================
// =============================================================================================================

static jvxErrorType createFromJsmn(CjvxJsonElement& jelm, const std::string& in, jsmntok_t* tokens, jvxSize& inTokenCnt, jvxSize numTokens, std::vector<std::string>& errors);
static jvxErrorType createFromJsmn(CjvxJsonElementList& elm, const std::string& in, jsmntok_t* tokens, jvxSize& inTokenCnt, jvxSize numTokens, std::vector<std::string>& errors);
static jvxErrorType createFromJsmn(CjvxJsonArray& jarr, const std::string& in, jsmntok_t* tokens, jvxSize& inTokenCnt, jvxSize numTokens, std::vector<std::string>& errors);
static jvxErrorType createFromJsmn(CjvxJsonArrayElement& jarrelm, const std::string& in, jsmntok_t* tokens, jvxSize& inTokenCnt, jvxSize numTokens, std::vector<std::string>& errors);

// =============================================================================================================
// ===========================================================================================================

jvxErrorType createFromJsmn(CjvxJsonArrayElement& jarrelm, const std::string& in, jsmntok_t* tokens, jvxSize& inTokenCnt, jvxSize numTokens, std::vector<std::string>& errors)
{
	CjvxJsonElementList jelmlst;
	std::string value;
	jvxData valueD;
	jvxBool err = false;
	std::string oneErr;

	if (inTokenCnt >= numTokens)
	{
		oneErr = "Trying to read token id " + jvx_size2String(inTokenCnt) + " but only " + jvx_size2String(numTokens) + " have been scanned.";
		errors.push_back(oneErr);
		return JVX_ERROR_PARSE_ERROR;
	}

	switch (tokens[inTokenCnt].type)
	{
	case JSMN_PRIMITIVE:
		value += in.substr(tokens[inTokenCnt].start, tokens[inTokenCnt].end - tokens[inTokenCnt].start);
		valueD = jvx_string2Data(value, err);
		jarrelm.makeAssignmentData(valueD, 4);
		break;
	case JSMN_STRING:
		value = in.substr(tokens[inTokenCnt].start, tokens[inTokenCnt].end - tokens[inTokenCnt].start);
		jarrelm.makeString(value);
		break;
	case JSMN_OBJECT:
		createFromJsmn(jelmlst, in, tokens, inTokenCnt, numTokens, errors);
		jarrelm.makeSection(jelmlst);
		break;
	default:
		return JVX_ERROR_PARSE_ERROR;
	}
	return JVX_NO_ERROR;
}

jvxErrorType createFromJsmn(CjvxJsonArray& jarr, const std::string& in, jsmntok_t* tokens, jvxSize& inTokenCnt, jvxSize numTokens, std::vector<std::string>& errors)
{
	jvxSize i;
	jvxErrorType resL = JVX_NO_ERROR;
	CjvxJsonArrayElement jarrelm;
	jvxSize numLoop;
	std::string oneErr;
	if (inTokenCnt >= numTokens)
	{
		oneErr = "Trying to read token id " + jvx_size2String(inTokenCnt) + " but only " + jvx_size2String(numTokens) + " have been scanned.";
		errors.push_back(oneErr);
		return JVX_ERROR_PARSE_ERROR;
	}

	switch (tokens[inTokenCnt].type)
	{
	case JSMN_ARRAY:
		numLoop = tokens[inTokenCnt].size;
		for (i = 0; i < numLoop; i++)
		{
			inTokenCnt++;
			resL = createFromJsmn(jarrelm, in, tokens, inTokenCnt, numTokens, errors);
			if (resL != JVX_NO_ERROR)
			{
				jarrelm.deleteAll();
				return resL;
			}
			else
			{
				jarr.addConsumeElement(jarrelm);
			}
		}
		break;
	default:
		return JVX_ERROR_PARSE_ERROR;
	}
	return JVX_NO_ERROR;
}

jvxErrorType createFromJsmn(CjvxJsonElement& jelm, const std::string& in, jsmntok_t* tokens, jvxSize& inTokenCnt, jvxSize numTokens, std::vector<std::string>& errors)
{
	std::string key;
	std::string value;
	jvxData valueD;
	CjvxJsonArray jarr;
	CjvxJsonElementList jelmlst;
	jvxBool err = false;
	jvxErrorType resL = JVX_NO_ERROR;
	std::string oneErr;
	if (inTokenCnt >= numTokens)
	{
		oneErr = "Trying to read token id " + jvx_size2String(inTokenCnt) + " but only " + jvx_size2String(numTokens) + " have been scanned.";
		errors.push_back(oneErr);
		return JVX_ERROR_PARSE_ERROR;
	}

	switch (tokens[inTokenCnt].type)
	{
	case JSMN_STRING:
		key = in.substr(tokens[inTokenCnt].start, tokens[inTokenCnt].end - tokens[inTokenCnt].start);
		break;
	default:
		return JVX_ERROR_PARSE_ERROR;
	}

	inTokenCnt++;
	if (inTokenCnt >= numTokens)
	{
		oneErr = "Trying to read token id " + jvx_size2String(inTokenCnt) + " but only " + jvx_size2String(numTokens) + " have been scanned.";
		errors.push_back(oneErr);
		return JVX_ERROR_PARSE_ERROR;
	}
	switch (tokens[inTokenCnt].type)
	{
	case JSMN_ARRAY:
		resL = createFromJsmn(jarr, in, tokens, inTokenCnt, numTokens, errors);
		if (resL != JVX_NO_ERROR)
		{
			jelm.deleteAll();
			return resL;
		}
		else
		{
			jelm.makeArray(key, jarr);
		}
		break;
	case JSMN_OBJECT:
		resL = createFromJsmn(jelmlst, in, tokens, inTokenCnt, numTokens, errors);
		if (resL != JVX_NO_ERROR)
		{
			jelm.deleteAll();
			return resL;
		}
		else
		{
			jelm.makeSection(key, jelmlst);
		}
		break;
	case JSMN_STRING:
		value = in.substr(tokens[inTokenCnt].start, tokens[inTokenCnt].end - tokens[inTokenCnt].start);
		jelm.makeAssignmentString(key, value);
		break;
	case JSMN_PRIMITIVE:
		value = in.substr(tokens[inTokenCnt].start, tokens[inTokenCnt].end - tokens[inTokenCnt].start);
		valueD = jvx_string2Data(value, err);
		jelm.makeAssignmentData(key, valueD, 4);
		break;
	default:
		return JVX_ERROR_PARSE_ERROR;
	}
	return JVX_NO_ERROR;
}

jvxErrorType createFromJsmn(CjvxJsonElementList& elm, const std::string& in, jsmntok_t* tokens, jvxSize& inTokenCnt, jvxSize numTokens, std::vector<std::string>& errors)
{
	jvxSize i;
	CjvxJsonElement jelm;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize numLoop = 0;
	std::string oneErr;
	if (inTokenCnt >= numTokens)
	{
		oneErr = "Trying to read token id " + jvx_size2String(inTokenCnt) + " but only " + jvx_size2String(numTokens) + " have been scanned.";
		errors.push_back(oneErr);
		return JVX_ERROR_PARSE_ERROR;
	}

	switch (tokens[inTokenCnt].type)
	{
	case JSMN_OBJECT:
		numLoop = tokens[inTokenCnt].size;
		for (i = 0; i < numLoop; i++)
		{
			inTokenCnt++;
			resL = createFromJsmn(jelm, in, tokens, inTokenCnt, numTokens, errors);
			if (resL != JVX_NO_ERROR)
			{
				elm.deleteAll();
				return resL;
			}
			elm.addConsumeElement(jelm);
		}
		break;
	default:
		std::cout << "error" << std::endl;
	}
	return JVX_NO_ERROR;
}

// =============================================================================================================
// =============================================================================================================

CjvxJsonArrayElement::CjvxJsonArrayElement()
{
	reset();
}

void
CjvxJsonArrayElement::reset()
{
	elm_section = NULL;
	elm_string = "";;
	elm_int = 0;
	elm_size = 0;
	elm_data.value = 0;
	elm_data.prec = JVX_DEFAULT_PRECISION;
	markFlat = false;
	tp = JVX_JSON_ARRAY_VALUE_NON;
}

CjvxJsonArrayElement::~CjvxJsonArrayElement()
{
	deleteAll();
}

void
CjvxJsonArrayElement::deleteAll()
{
	if (elm_section)
	{
		elm_section->deleteAll();
		JVX_DSP_SAFE_DELETE_OBJECT(elm_section);
	}
}

jvxBool
CjvxJsonArrayElement::markAllowsOneLineOutput()
{
	switch(tp)
	{ 
	case JVX_JSON_ARRAY_SECTION:
		markFlat = elm_section->markAllowsOneLineOutput();
		break;
	default:
		markFlat = true;
		break;
	}
	return markFlat;
}

jvxErrorType 
CjvxJsonArrayElement::makeString(const std::string& assignedvalue)
{
	if (tp == JVX_JSON_ARRAY_VALUE_NON)
	{
		elm_string = assignedvalue;
		tp = JVX_JSON_ARRAY_STRING;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxJsonArrayElement::makeAssignmentInt(int assignedvalue)
{
	if (tp == JVX_JSON_ARRAY_VALUE_NON)
	{
		elm_int = assignedvalue;
		tp = JVX_JSON_ARRAY_VALUE_INT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxJsonArrayElement::makeAssignmentSize(jvxSize assignedvalue)
{
	if (tp == JVX_JSON_ARRAY_VALUE_NON)
	{
		elm_size = assignedvalue;
		tp = JVX_JSON_ARRAY_VALUE_SIZE;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxJsonArrayElement::makeAssignmentData(jvxData assignedvalue, int prec)
{
	if (tp == JVX_JSON_ARRAY_VALUE_NON)
	{
		elm_data.value = assignedvalue;
		elm_data.prec = prec;
		tp = JVX_JSON_ARRAY_VALUE_DATA;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxJsonArrayElement::makeSection(CjvxJsonElementList& assignedvalue)
{
	if (tp == JVX_JSON_ARRAY_VALUE_NON)
	{
		assert(elm_section == NULL);
		JVX_DSP_SAFE_ALLOCATE_OBJECT(elm_section, CjvxJsonElementList);
		elm_section->consumeAllElements(assignedvalue);
		tp = JVX_JSON_ARRAY_SECTION;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxJsonArrayElement::consumeAllData(CjvxJsonArrayElement& in)
{
	if (tp == JVX_JSON_ARRAY_VALUE_NON)
	{
		tp = in.tp;
		elm_section = in.elm_section;
		elm_string = in.elm_string;
		elm_int = in.elm_int;
		elm_size = in.elm_size;
		elm_data = in.elm_data;

		in.reset();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}
jvxErrorType 
CjvxJsonArrayElement::printString(std::string& writeto, jvxJsonPrintTarget tpP, jvxSize indent, const std::string indToken, const std::string& initToken, 
	const std::string& ret_token, jvxBool oneLineOutputFromHere)
{
	jvxSize i;
	std::list<CjvxJsonArrayElement>::iterator elmA;
	jvxCBitField mo = 0;
	switch (tpP)
	{
	case JVX_JSON_PRINT_TXTCONSOLE:

		if (tp == JVX_JSON_ARRAY_VALUE_NON)
		{
			std::cout << "Error: invalid element of type  JVX_JSON_ARRAY_VALUE_NON in json print function." << std::endl;
		}

		switch (tp)
		{
		case JVX_JSON_ARRAY_STRING:
			writeto += elm_string;
			//JVX_WRITE_RETURN(writeto);
			break;
		case JVX_JSON_ARRAY_VALUE_INT:
			writeto += jvx_int2String(elm_int);
			//JVX_WRITE_RETURN(writeto);
			break;
		case JVX_JSON_ARRAY_VALUE_SIZE:
			writeto += jvx_size2String(elm_size);
			//JVX_WRITE_RETURN(writeto);
			break;
		case JVX_JSON_ARRAY_VALUE_DATA:
			writeto += jvx_data2String(elm_data.value, elm_data.prec);
			//JVX_WRITE_RETURN(writeto);
			break;
		case JVX_JSON_ARRAY_SECTION:
			assert(elm_section);
			if (oneLineOutputFromHere)
			{
				elm_section->printString(writeto, tpP, 0, indToken, "", ret_token, oneLineOutputFromHere);
			}
			else
			{
				elm_section->printString(writeto, tpP, indent, indToken, initToken, ret_token, oneLineOutputFromHere);
			}
			//JVX_WRITE_RETURN(writeto);
			break;
		}
		break;
	case JVX_JSON_PRINT_JSON:

		if (tp == JVX_JSON_ARRAY_VALUE_NON)
		{
			std::cout << "Error: invalid element of type  JVX_JSON_ARRAY_VALUE_NON in json print function." << std::endl;
		}

#ifdef JVX_WRITE_TREE_STRUCTURE_TAGS

		JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
		if (markFlat)
		{
			writeto += "\"markflat\" : \"yes\"";
		}
		else
		{
			writeto += "\"markflat\": \"no\"";
		}
		JVX_WRITE_RETURN(writeto, ret_token);
#endif
		switch (tp)
		{
		case JVX_JSON_ARRAY_STRING:
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken); 
			writeto += "\"";
			writeto += elm_string;
			writeto += "\"";
			break;
		case JVX_JSON_ARRAY_VALUE_INT:
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken); 
			writeto += jvx_int2String(elm_int);
			break;
		case JVX_JSON_ARRAY_VALUE_SIZE:
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			writeto += jvx_size2String(elm_size);
			break;
		case JVX_JSON_ARRAY_VALUE_DATA:
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			writeto += jvx_data2String(elm_data.value, elm_data.prec);
			break;
		case JVX_JSON_ARRAY_SECTION:
			assert(elm_section);
			elm_section->printString(writeto, tpP, indent, indToken, initToken, ret_token, oneLineOutputFromHere);
			break;
		}
		break;
	default:
		assert(0);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxJsonArrayElement::reference_section(CjvxJsonElementList** ret)
{
	if (tp == JVX_JSON_ARRAY_SECTION)
	{
		if (ret)
		{
			*ret = elm_section;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxJsonArrayElement::reference_string(jvxApiString& ret)
{
	if (tp == JVX_JSON_ARRAY_STRING)
	{
		ret.assign(elm_string);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxJsonArrayElement::reference_int(int* ret)
{
	if (tp == JVX_JSON_ARRAY_VALUE_INT)
	{
		if (ret)
		{
			*ret = elm_int;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxJsonArrayElement::reference_size(jvxSize* ret)
{
	if (tp == JVX_JSON_ARRAY_VALUE_SIZE)
	{
		if (ret)
		{
			*ret = elm_size;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxJsonArrayElement::reference_data(jvxData* ret)
{
	if (tp == JVX_JSON_ARRAY_VALUE_DATA)
	{
		if (ret)
		{
			*ret = elm_data.value;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

// =====================================================================
// =====================================================================

CjvxJsonArray::CjvxJsonArray()
{
	reset();
}

CjvxJsonArray::~CjvxJsonArray()
{
	deleteAll();
}

void 
CjvxJsonArray::reset()
{
	this->printMode = 0;
	markFlat = false;
	lst.clear();
}

void
CjvxJsonArray::deleteAll()
{
	std::list<CjvxJsonArrayElement*>::iterator elm = lst.begin();
	for (; elm != lst.end(); elm++)
	{
		(*elm)->deleteAll();
		JVX_DSP_SAFE_DELETE_OBJECT(*elm);
	}
	reset();
}

jvxBool
CjvxJsonArray::markAllowsOneLineOutput()
{
	jvxBool markFlat = true;
	if (lst.size() <= 1)
	{
		std::list<CjvxJsonArrayElement*>::iterator elm = lst.begin();
		if (elm != lst.end())
		{
			markFlat = (*elm)->markAllowsOneLineOutput();
		}
	}
	else
	{
		std::list<CjvxJsonArrayElement*>::iterator elm = lst.begin();
		for (; elm != lst.end(); elm++)
		{
			// Recursive..
			(*elm)->markAllowsOneLineOutput();
		}

		// If more than one output, definitely not flat
		markFlat = false;
	}
	return markFlat;
}

jvxErrorType 
CjvxJsonArray::addConsumeElement(CjvxJsonArrayElement& in)
{
	CjvxJsonArrayElement* newElm = NULL;
	JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm, CjvxJsonArrayElement);
	newElm->consumeAllData(in);
	lst.push_back(newElm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxJsonArray::numElements(jvxSize* num)
{
	if (num)
		*num = lst.size();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxJsonArray::reference_elementAt(jvxSize idx, CjvxJsonArrayElement** ret)
{
	jvxSize cnt = 0;
	jvxSize num = lst.size();
	std::list<CjvxJsonArrayElement*>::iterator elm = lst.begin();
	for (; elm != lst.end(); elm++)
	{
		if (cnt == idx)
		{
			break;
		}
		cnt++;
	}
	if (elm != lst.end())
	{
		if (ret)
		{
			*ret = *elm;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxJsonArray::consumeAllElements(CjvxJsonArray& in)
{
	std::list<CjvxJsonArrayElement*>::iterator elm = in.lst.begin();
	for (; elm != in.lst.end(); elm++)
	{
		lst.push_back(*elm);
	}
	printMode = in.printMode;
	in.reset();
	return JVX_NO_ERROR;
}

/*
 * Indentation: This element ends with a non-newline character.
 */
jvxErrorType
CjvxJsonArray::printString(std::string& writeto, jvxJsonPrintTarget tpP, jvxSize indent, const std::string indToken, const std::string& initToken, 
	const std::string& ret_token, jvxBool oneLineOutputFromHere)
{
	jvxSize cnt = 0;
	jvxSize num = 0;
	jvxSize i;
	//jvxSize tmp;

	std::list<CjvxJsonArrayElement*>::iterator elmA;
	switch (tpP)
	{
	case JVX_JSON_PRINT_TXTCONSOLE:

		elmA = lst.begin();
		num = lst.size();	
		
		num = lst.size();
		for (; elmA != lst.end(); elmA++)
		{
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			writeto += "#" + jvx_size2String(cnt) + ":";
			jvxBool doFlat = (*elmA)->getMarkFlat();
			if (doFlat)
			{
				// All following string will be compact and without indentation
				(*elmA)->printString(writeto, tpP, 0, indToken, "", ret_token, true);
			}
			else
			{
				writeto += "->";
				JVX_WRITE_RETURN(writeto, ret_token);
				(*elmA)->printString(writeto, tpP, indent+1, indToken, initToken, ret_token,false);
			}
			if (cnt < num - 1)
			{
				JVX_WRITE_RETURN(writeto, ret_token);
			}
			cnt++;
		}
		break;
	case JVX_JSON_PRINT_JSON:

		elmA = lst.begin();
		JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
		writeto += "[";
		JVX_WRITE_RETURN(writeto, ret_token);

		num = lst.size();
		for (; elmA != lst.end(); elmA++)
		{
			(*elmA)->printString(writeto, tpP, indent + 1, indToken, initToken, ret_token,false);
			if (cnt != (num - 1))
			{
				writeto += ",";
			}
			JVX_WRITE_RETURN(writeto, ret_token);
			cnt++;
		}
		JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken); 
		writeto += "]";

		break;
	default:
		assert(0);
	}
	return JVX_NO_ERROR;
}

// =====================================================================
// =====================================================================

CjvxJsonElement::CjvxJsonElement()
{
	reset();
};

CjvxJsonElement::~CjvxJsonElement()
{
	deleteAll();
};

void
CjvxJsonElement::deleteAll()
{
	if (elm_section)
	{
		elm_section->deleteAll();
		JVX_DSP_SAFE_DELETE_OBJECT(elm_section);
	}

	if (elm_array)
	{
		elm_array->deleteAll();
		JVX_DSP_SAFE_DELETE_OBJECT(elm_array);
	}
	reset();
}

jvxBool
CjvxJsonElement::markAllowsOneLineOutput()
{
	switch (tp)
	{
	case JVX_JSON_ASSIGNMENT_ARRAY:
		markFlat = elm_array->markAllowsOneLineOutput();
		break;
	case JVX_JSON_ASSIGNMENT_SECTION:
		markFlat = elm_section->markAllowsOneLineOutput();
		break;
	default:
		markFlat = true;
		break;
	}
	return markFlat;
}

void 
CjvxJsonElement::reset()
{
	tp = JVX_JSON_ASSIGNMENT_VALUE_NON;
	elm_int.rhs = 0;
	elm_size.rhs = 0;
	elm_data.rhs = 0.0;
	elm_data.prec = JVX_DEFAULT_PRECISION;
	elm_section = NULL;
	elm_array = NULL;
	name = "";
	elm_string.rhs = "";
	markFlat = false;

}

jvxErrorType 
CjvxJsonElement::consumeElement(CjvxJsonElement& in)
{
	if (tp == JVX_JSON_ASSIGNMENT_VALUE_NON)
	{
		tp = in.tp;
		elm_int = in.elm_int;
		elm_size = in.elm_size;
		elm_data = in.elm_data;
		elm_section = in.elm_section;
		elm_array = in.elm_array;
		elm_string.rhs = in.elm_string.rhs;
		name = in.name;

		in.reset();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxJsonElement::makeAssignmentString(const std::string& nm, const std::string& assignedvalue)
{
	if (tp == JVX_JSON_ASSIGNMENT_VALUE_NON)
	{
		name = nm;
		tp = JVX_JSON_ASSIGNMENT_STRING;
		elm_string.rhs = assignedvalue;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxJsonElement::makeAssignmentInt(const std::string& nm, int assignedvalue)
{
	if (tp == JVX_JSON_ASSIGNMENT_VALUE_NON)
	{
		name = nm;
		tp = JVX_JSON_ASSIGNMENT_VALUE_INT;
		elm_int.rhs = assignedvalue;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
};

jvxErrorType
CjvxJsonElement::makeAssignmentSize(const std::string& nm, jvxSize assignedvalue)
{
	if (tp == JVX_JSON_ASSIGNMENT_VALUE_NON)
	{
		name = nm;
		tp = JVX_JSON_ASSIGNMENT_VALUE_SIZE;
		elm_size.rhs = assignedvalue;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
};

jvxErrorType
CjvxJsonElement::makeAssignmentData(const std::string& nm, jvxData assignedvalue, int prec)
{
	if (tp == JVX_JSON_ASSIGNMENT_VALUE_NON)
	{
		name = nm;
		tp = JVX_JSON_ASSIGNMENT_VALUE_DATA;
		elm_data.rhs = assignedvalue;
		elm_data.prec = prec;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
};

jvxErrorType
CjvxJsonElement::makeArray(const std::string& nm, CjvxJsonArray& in)
{
	if (tp == JVX_JSON_ASSIGNMENT_VALUE_NON)
	{
		name = nm;
		tp = JVX_JSON_ASSIGNMENT_ARRAY;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(elm_array, CjvxJsonArray);
		elm_array->consumeAllElements(in);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
};

jvxErrorType
CjvxJsonElement::makeSection(const std::string& nm, CjvxJsonElementList& assignedvalue)
{
	if (tp == JVX_JSON_ASSIGNMENT_VALUE_NON)
	{
		assert(elm_section == NULL);
		name = nm;
		tp = JVX_JSON_ASSIGNMENT_SECTION;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(elm_section, CjvxJsonElementList);
		elm_section->consumeAllElements( assignedvalue);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
};

jvxErrorType
CjvxJsonElement::printString(std::string& writeto, jvxJsonPrintTarget tpP, jvxSize indent, const std::string indToken, 
	const std::string& initToken, const std::string& ret_token, jvxBool oneLineOutputFromHere)
{
	jvxSize cnt = 0;
	jvxSize num = 0;
	jvxSize i;
	jvxCBitField mo = 0;
	std::list<CjvxJsonArrayElement*>::iterator elmA;
	switch (tpP)
	{
	case JVX_JSON_PRINT_TXTCONSOLE:

		if (tp == JVX_JSON_ASSIGNMENT_VALUE_NON)
		{
			std::cout << "Error: invalid element of type  JVX_JSON_ASSIGNMENT_VALUE_NON in json print function." << std::endl;
		}

		switch (tp)
		{
		case JVX_JSON_ASSIGNMENT_VALUE_DATA:
			if (!oneLineOutputFromHere)
			{
				JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			}
			// HK, 240122: variable name removed for max compact out. Same as for STRING
			//writeto += name;
			//writeto += "=";
			writeto += jvx_data2String(elm_data.rhs, elm_data.prec);
			break;
		case JVX_JSON_ASSIGNMENT_VALUE_INT:
			if (!oneLineOutputFromHere)
			{
				JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			}
			// HK, 240122: variable name removed for max compact out. Same as for STRING
			//writeto += name;
			//writeto += "=";
			writeto += jvx_int2String(elm_int.rhs);
			break;

		case JVX_JSON_ASSIGNMENT_VALUE_SIZE:
			if (!oneLineOutputFromHere)
			{
				JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			}
			writeto += jvx_size2String(elm_size.rhs);
			break;

		case JVX_JSON_ASSIGNMENT_STRING:
			if (!oneLineOutputFromHere)
			{
				JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			}
			writeto += "\"";
			writeto += elm_string.rhs;
			writeto += "\"";
			break;
		case JVX_JSON_ASSIGNMENT_ARRAY:
			assert(elm_array);
			if (oneLineOutputFromHere)
			{
				writeto += name + "->";
				elm_array->printString(writeto, tpP, indent + 1, indToken, initToken, ret_token, oneLineOutputFromHere);
			}
			else
			{
				JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
				writeto += name + "->";
				JVX_WRITE_RETURN(writeto, ret_token);
				elm_array->printString(writeto, tpP, indent + 1, indToken, initToken, ret_token, oneLineOutputFromHere);
			}
			break;
		case JVX_JSON_ASSIGNMENT_SECTION:
			assert(elm_section);
			if (oneLineOutputFromHere)
			{
				writeto += name + "->";
				elm_section->printString(writeto, tpP, 0, "", "", ret_token, oneLineOutputFromHere);
			}
			else
			{
				JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
				writeto += name + "->";
				JVX_WRITE_RETURN(writeto, ret_token);
				elm_section->printString(writeto, tpP, indent + 1, indToken, initToken, ret_token, oneLineOutputFromHere);
			}
			break;
		}
		break;

	case JVX_JSON_PRINT_JSON:

		if (tp == JVX_JSON_ASSIGNMENT_VALUE_NON)
		{
			std::cout << "Error: invalid element of type  JVX_JSON_ASSIGNMENT_VALUE_NON in json print function." << std::endl;
		}

#ifdef JVX_WRITE_TREE_STRUCTURE_TAGS
		JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
		writeto += "\"";
		writeto += "markflat";
		writeto += "\"";
		writeto += ": ";
		if (markFlat)
		{
			writeto += "\"";
			writeto += "yes";
			writeto += "\"";
		}
		else
		{
			writeto += "\"";
			writeto += "no";
			writeto += "\"";
		}
		JVX_WRITE_RETURN(writeto, ret_token);
#endif
		switch (tp)
		{
		case JVX_JSON_ASSIGNMENT_VALUE_DATA:
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			writeto += "\"";
			writeto += name;
			writeto += "\"";
			writeto += ": ";
			writeto += jvx_data2String(elm_data.rhs, elm_data.prec);
			break;
		case JVX_JSON_ASSIGNMENT_VALUE_INT:
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			writeto += "\"";
			writeto += name;
			writeto += "\"";
			writeto += ": ";
			writeto += jvx_int2String(elm_int.rhs);
			break;
		case JVX_JSON_ASSIGNMENT_VALUE_SIZE:
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			writeto += "\"";
			writeto += name;
			writeto += "\"";
			writeto += ": ";
			writeto += jvx_size2String(elm_size.rhs);
			break;
		case JVX_JSON_ASSIGNMENT_STRING:
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			writeto += "\"";
			writeto += name;
			writeto += "\"";
			writeto += ": ";
			writeto += "\"";
			writeto += elm_string.rhs;
			writeto += "\"";
			break;
		case JVX_JSON_ASSIGNMENT_ARRAY:
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
			assert(elm_array);
			writeto += "\"";
			writeto += name;
			writeto += "\": ";
			JVX_WRITE_RETURN(writeto, ret_token);
			elm_array->printString(writeto, tpP, indent, indToken, initToken, ret_token, oneLineOutputFromHere);
#ifdef JVX_WRITE_TREE_STRUCTURE_TAGS
			writeto += ",";
			JVX_WRITE_RETURN_INDENTATION(writeto, indent, indToken, initToken);
			writeto += "\"";
			writeto += "__" + name + "__presentation";
			writeto += "\": ";
			writeto += "\"";
			mo = elm_array->getPrintMode();
			if (jvx_cbitTest(mo, JVX_JSON_PRINT_MODE_COMPACT_SHIFT))
			{
				writeto += "compact";
			}
			else
			{
				writeto += "full";
			}
			writeto += "\"";
#endif
			break;
		case JVX_JSON_ASSIGNMENT_SECTION:
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken); 
			writeto += "\"";
			writeto += name;
			writeto += "\": "; 
			assert(elm_section);
			JVX_WRITE_RETURN(writeto, ret_token);
			elm_section->printString(writeto, tpP, indent, indToken, initToken, ret_token, oneLineOutputFromHere);
#ifdef JVX_WRITE_TREE_STRUCTURE_TAGS
			writeto += ",";
			JVX_WRITE_RETURN_INDENTATION(writeto, indent, indToken, initToken);
			writeto += "\"";
			writeto += "__" + name + "__presentation";
			writeto += "\": ";
			writeto += "\"";
			mo = elm_section->getPrintMode();
			if (jvx_cbitTest(mo, JVX_JSON_PRINT_MODE_COMPACT_SHIFT))
			{
				writeto += "compact";
			}
			else
			{
				writeto += "full";
			}
			writeto += "\"";
#endif
			break;
		}
		break;
	default:
		assert(0);
	}
	return JVX_NO_ERROR;
}

jvxBool 
CjvxJsonElement::matchName(const std::string& nm, jvxBool wilcardMatch)
{
	if (wilcardMatch)
	{
		return jvx_compareStringsWildcard(nm, name);
	}
	return (nm == name);
}

jvxBool 
CjvxJsonElement::matchType(jvxJsonElementType tpF)
{
	if (
		(tpF == JVX_JSON_ASSIGNMENT_VALUE_NON) ||
		(tpF == tp))
	{
		return true;
	}
	return false;
}

jvxErrorType 
CjvxJsonElement::reference_array(CjvxJsonArray** ret)
{
	if (tp == JVX_JSON_ASSIGNMENT_ARRAY)
	{
		if (ret)
		{
			*ret = elm_array;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxJsonElement::reference_section(CjvxJsonElementList** ret)
{
	if (tp == JVX_JSON_ASSIGNMENT_SECTION)
	{
		if (ret)
		{
			*ret = elm_section;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxJsonElement::reference_string(jvxApiString& ret)
{
	if (tp == JVX_JSON_ASSIGNMENT_STRING)
	{
		ret.assign(elm_string.rhs);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxJsonElement::reference_int(int* ret)
{
	if (tp == JVX_JSON_ASSIGNMENT_VALUE_INT)
	{
		if (ret)
		{
			*ret = elm_int.rhs;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxJsonElement::reference_size(jvxSize* ret)
{
	if (tp == JVX_JSON_ASSIGNMENT_VALUE_SIZE)
	{
		if (ret)
		{
			*ret = elm_size.rhs;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxJsonElement::reference_data(jvxData* ret)
{
	if (tp == JVX_JSON_ASSIGNMENT_VALUE_DATA)
	{
		if (ret)
		{
			*ret = elm_data.rhs;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}
// ======================================================================

CjvxJsonElementList::CjvxJsonElementList()
{
	reset();
};

CjvxJsonElementList::~CjvxJsonElementList()
{
	deleteAll();
	reset();
};

void 
CjvxJsonElementList::reset()
{
	lst.clear();
	printMode = 0;
	markFlat = false;

}

jvxBool 
CjvxJsonElementList::markAllowsOneLineOutput()
{
	markFlat = true;
	// If of approriate type, the entries in the section may be combined in one
	std::list<CjvxJsonElement*>::iterator elm = lst.begin();
	for (; elm != lst.end(); elm++)
	{
		jvxBool tt = (*elm)->markAllowsOneLineOutput();
		markFlat = markFlat && tt;
	}

	return markFlat;
}

jvxErrorType
CjvxJsonElementList::insertConsumeElementFront(CjvxJsonElement& in)
{
	CjvxJsonElement* newElm = NULL;
	JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm, CjvxJsonElement);
	newElm->consumeElement(in);
	lst.push_front(newElm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxJsonElementList::addConsumeElement(CjvxJsonElement& in)
{
	CjvxJsonElement* newElm = NULL;
	JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm, CjvxJsonElement);
	newElm->consumeElement(in);
	lst.push_back(newElm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxJsonElementList::consumeAllElements(CjvxJsonElementList& in)
{
	std::list<CjvxJsonElement*>::iterator elm = in.lst.begin();
	jvxSize cnt = 0;
	for (; elm != in.lst.end(); elm++)
	{
		lst.push_back(*elm);
	}
	printMode = in.printMode;
	in.reset();
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxJsonElementList::numElements(jvxSize* num)
{
	if (num)
	{
		*num = lst.size();
	}
	return JVX_NO_ERROR;
};

#if 0
jvxErrorType
CjvxJsonElementList::reference_elementAt(jvxSize idx, CjvxJsonElement** ret)
{
	std::list<CjvxJsonElement*>::iterator elm = lst.begin();
	jvxSize cnt = 0;
	for (; elm != lst.end(); elm++)
	{
		cnt++;
	}
	if (elm != lst.end())
	{
		if(ret)
			*ret = *elm;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
};
#endif

void
CjvxJsonElementList::deleteAll()
{
	std::list<CjvxJsonElement*>::iterator elm = lst.begin();
	for (; elm != lst.end(); elm++)
	{
		assert(*elm);
		(*elm)->deleteAll();
		JVX_DSP_SAFE_DELETE_OBJECT(*elm);
	}
	reset();
}

jvxErrorType
CjvxJsonElementList::printString(std::string& writeto, jvxJsonPrintTarget tpP, jvxSize indent, const std::string indToken,
	const std::string& initToken, const std::string& ret_token,  jvxBool oneLineOutputFromHere)
{
	jvxSize cnt = 0;
	jvxSize num = 0;
	jvxSize i;
	jvxBool mayShowFlat = true;
	std::list<CjvxJsonElement*>::iterator elm = lst.begin();

	switch (tpP)
	{
	case JVX_JSON_PRINT_TXTCONSOLE:
		cnt = 0;
		num = lst.size();
		elm = lst.begin();
		if (markFlat)
		{
			oneLineOutputFromHere = true;
		}

		if (oneLineOutputFromHere)
		{
			JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken)
			for (; elm != lst.end(); elm++)
			{
				assert(*elm);
				(*elm)->printString(writeto, tpP, indent, "", "", ret_token,oneLineOutputFromHere);
				if (cnt < num - 1)
				{
					writeto += ":";
				}
				cnt++;
			}
		}
		else
		{
			for (; elm != lst.end(); elm++)
			{
				assert(*elm);
				jvxBool doflat = (*elm)->getMarkFlat();
				if (doflat)
				{
					JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
					(*elm)->printString(writeto, tpP, 0, indToken, "", ret_token, true);
				}
				else
				{
					(*elm)->printString(writeto, tpP, indent, indToken, initToken, ret_token, false);
				}
				if (cnt < num - 1)
				{
					JVX_WRITE_RETURN(writeto, ret_token);
				}
				cnt++;
			}
		}

		break;
	case JVX_JSON_PRINT_JSON:

		JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
		writeto += "{";
		JVX_WRITE_RETURN(writeto, ret_token);

		cnt = 0;
		num = lst.size();
		for (; elm != lst.end(); elm++)
		{
			assert(*elm);
			(*elm)->printString(writeto, tpP, indent + 1, indToken, initToken, ret_token, false);
			if (cnt != (num - 1))
			{
				writeto += ", ";
			}
			JVX_WRITE_RETURN(writeto, ret_token);
			cnt++;
		}
	
		JVX_WRITE_INDENTATION(writeto, indent, indToken, initToken);
		writeto += "}";
		break;
	default:
		assert(0);
	}
	return JVX_NO_ERROR;
};

void
CjvxJsonElementList::printToJsonFlat(std::string& response)
{
	markAllowsOneLineOutput();
	printString(response, JVX_JSON_PRINT_JSON, 0, "", "", "", false);
};

void
CjvxJsonElementList::printToJsonView(std::string& response)
{
	markAllowsOneLineOutput();
	printString(response, JVX_JSON_PRINT_JSON, 0, "\t", "\t", "\n", false);
};

void 
CjvxJsonElementList::printToStringView(std::string& response)
{
	markAllowsOneLineOutput();
	printString(response, JVX_JSON_PRINT_TXTCONSOLE, 0, "\t", "\t", "\n", false);
	//jelmlst.printString(response, JVX_JSON_PRINT_JSON, 0, "\t", "\t", "\n", false);
};

jvxErrorType
CjvxJsonElementList::stringToRepresentation(const std::string& in, CjvxJsonElementList& onReturn, std::vector<std::string>& errors)
{
	jvxErrorType res = JVX_NO_ERROR;
	// JSMN_ERROR_INVAL - bad token, JSON string is corrupted
	// JSMN_ERROR_NOMEM - not enough tokens, JSON string is too large
	// JSMN_ERROR_PART- JSON string is too short, expecting more JSON data
	jsmn_parser myParser;
	jvxSize tokenIdx = 0;
	jsmn_init(&myParser);
	int numTokens = jsmn_parse(&myParser, in.c_str(), in.size(), NULL, 0);
	//std::cout << in << std::endl;
	if (numTokens > 0)
	{
		// Allocate memory for tokens
		jsmntok_t* tokens = NULL;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(tokens, jsmntok_t, numTokens);

		// Run parser again
		jsmn_init(&myParser);
		numTokens = jsmn_parse(&myParser, in.c_str(), in.size(), tokens, numTokens);

		// First representation complete, next, copy content into C++ structs
		if (numTokens > 0)
		{
			res = createFromJsmn(onReturn, in, tokens, tokenIdx, numTokens, errors);
			JVX_DSP_SAFE_DELETE_FIELD(tokens);
		}
		else
		{
			res = JVX_ERROR_INTERNAL;
		}
	}
	else
	{
		std::string errToken;
		switch (numTokens)
		{
		case JSMN_ERROR_INVAL:
			errToken = "Parse error in json statement starting with <";
			errToken += in.substr(myParser.pos, std::string::npos);
			errToken += ">";
			errors.push_back(errToken);
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			break;
		case JSMN_ERROR_NOMEM:
		case JSMN_ERROR_PART:
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			break;
		}
	}
	return res;
}

jvxErrorType
CjvxJsonElementList::reference_element(CjvxJsonElement** ret, 
	const std::string& name,
	CjvxJsonElement::jvxJsonElementType tp,
	jvxSize idx,
	jvxBool matchWildcard)
{
	jvxSize cnt = 0;
	std::list<CjvxJsonElement*>::iterator elm = this->lst.begin();
	for (; elm != lst.end(); elm++)
	{
		if((*elm)->matchName(name, matchWildcard) &&
			(*elm)->matchType(tp))
		{
			if (cnt == idx)
			{
				if (ret)
				{
					*ret = (*elm);
				}
				return JVX_NO_ERROR;
			}
			else
			{ 
				cnt++;
			}
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}
