#ifndef __CJVXJSON_H__
#define __CJVXJSON_H__

#include "jvx.h"

/* Valid example
{
    "array":[
        {
           "Description":"Component#1",
           "Name":"Hallo1",
           "State":"bla"
        },
        "zwei"
            ],
"assignment": "text",
"oassign": 1.2,
"section": {
"a":1,
"b":2}
}
*/

typedef enum
{
	JVX_JSON_PRINT_JSON,
	JVX_JSON_PRINT_TXTCONSOLE
} jvxJsonPrintTarget;
	
typedef enum
{
	// 0 means FULL
	JVX_JSON_PRINT_MODE_COMPACT_SHIFT = 0,
	JVX_JSON_PRINT_MODE_ADD_LAYER_OUTPUT_COMPACT = 2
} jvxJsonPrintMode;


class CjvxJsonElementList;
class CjvxJsonElement;

class CjvxJsonArrayElement
{
public:

	typedef enum
	{
		JVX_JSON_ARRAY_VALUE_NON,
		JVX_JSON_ARRAY_VALUE_DATA,
		JVX_JSON_ARRAY_VALUE_INT,
		JVX_JSON_ARRAY_VALUE_SIZE,
		JVX_JSON_ARRAY_STRING,
		JVX_JSON_ARRAY_SECTION
	} jvxJsonArrayElementType;

private:
		
	typedef struct
	{
		jvxData value;
		int prec;
	} jvxJsonArrayData;

	CjvxJsonElementList* elm_section;
	std::string elm_string;
	int elm_int;
	jvxSize elm_size;
	jvxJsonArrayData elm_data;
	jvxJsonArrayElementType tp;
	jvxBool markFlat;

public:

	CjvxJsonArrayElement();
	virtual ~CjvxJsonArrayElement();
	virtual void deleteAll();
	virtual void reset();
	virtual jvxBool markAllowsOneLineOutput();
	virtual jvxBool getMarkFlat() 
	{
		return markFlat;
	};
	virtual jvxErrorType makeString(const std::string& assignedvalue);
	virtual jvxErrorType makeAssignmentInt(int assignedvalue);
	virtual jvxErrorType makeAssignmentSize(jvxSize assignedvalue);
	virtual jvxErrorType makeAssignmentData(jvxData assignedvalue, int prec);
	virtual jvxErrorType makeSection(CjvxJsonElementList& assignedvalue);
	virtual jvxErrorType consumeAllData(CjvxJsonArrayElement& in);
	virtual jvxErrorType printString(std::string& writeto, jvxJsonPrintTarget tpP, jvxSize indent, const std::string indToken, 
		const std::string& initToken, const std::string& ret_token, jvxBool oneLineOutputFromHere);

	virtual jvxErrorType reference_section(CjvxJsonElementList** ret);
	virtual jvxErrorType reference_string(jvxApiString& ret);
	virtual jvxErrorType reference_int(int* ret);
	virtual jvxErrorType reference_size(jvxSize* ret);
	virtual jvxErrorType reference_data(jvxData* ret);
};

// =======================================================================

class CjvxJsonArray
{
private:

	std::list<CjvxJsonArrayElement*> lst;
	jvxCBitField printMode;
	jvxBool markFlat;

public:
	CjvxJsonArray();
	virtual ~CjvxJsonArray();
	virtual void reset();
	virtual void deleteAll();
	virtual void setPrintMode(jvxCBitField pTp)
	{
		printMode = pTp;
	};
	jvxCBitField getPrintMode()
	{
		return printMode;
	};
	virtual jvxBool getMarkFlat()
	{
		return markFlat;
	};
	virtual jvxBool markAllowsOneLineOutput();
	virtual jvxErrorType addConsumeElement(CjvxJsonArrayElement&);
	virtual jvxErrorType numElements(jvxSize* num);
	virtual jvxErrorType reference_elementAt(jvxSize idx, CjvxJsonArrayElement** ret);
	virtual jvxErrorType consumeAllElements(CjvxJsonArray& in);
	virtual jvxErrorType printString(std::string& writeto, jvxJsonPrintTarget tpP, jvxSize indent, const std::string indToken, 
		const std::string& initToken, const std::string& ret_token, jvxBool oneLineOutputFromHere);
};

// =======================================================================

class CjvxJsonElement
{
public:
	
	typedef enum
	{
		JVX_JSON_ASSIGNMENT_VALUE_NON,
		JVX_JSON_ASSIGNMENT_VALUE_DATA,
		JVX_JSON_ASSIGNMENT_VALUE_INT,
		JVX_JSON_ASSIGNMENT_VALUE_SIZE,
		JVX_JSON_ASSIGNMENT_STRING,
		JVX_JSON_ASSIGNMENT_ARRAY,
		JVX_JSON_ASSIGNMENT_SECTION
	} jvxJsonElementType;

private:

	typedef struct
	{
		std::string rhs;
	} jvxJsonAssignementString;
	
	typedef struct
	{
		int rhs;
	} jvxJsonAssignementInt;

	typedef struct
	{
		jvxSize rhs;
	} jvxJsonAssignementSize;

	typedef struct
	{
		jvxData rhs;
		int prec;
	} jvxJsonAssignementData;

	// ============================================================
	
	std::string name;
	jvxJsonElementType tp;
	CjvxJsonArray* elm_array;
	CjvxJsonElementList* elm_section;
	jvxJsonAssignementString elm_string;
	jvxJsonAssignementInt elm_int;
	jvxJsonAssignementSize elm_size;
	jvxJsonAssignementData elm_data;
	jvxBool markFlat;

public:

	CjvxJsonElement();
	virtual ~CjvxJsonElement();
	virtual void reset();
	virtual void deleteAll();
	bool isSection()
	{
		return (tp == JVX_JSON_ASSIGNMENT_SECTION);
	};
	bool isArray()
	{
		return (tp == JVX_JSON_ASSIGNMENT_ARRAY);
	};
	virtual jvxBool getMarkFlat()
	{
		return markFlat;
	};
	virtual jvxBool markAllowsOneLineOutput();
	virtual jvxErrorType consumeElement(CjvxJsonElement& in);
	virtual jvxErrorType makeAssignmentString(const std::string& nm, const std::string& assignedvalue);
	virtual jvxErrorType makeAssignmentInt(const std::string& nm, int assignedvalue);
	virtual jvxErrorType makeAssignmentSize(const std::string& nm, jvxSize assignedvalue);
	virtual jvxErrorType makeAssignmentData(const std::string& nm, jvxData assignedvalue, int prec);
	virtual jvxErrorType makeArray(const std::string& nm, CjvxJsonArray& CjvxJsonArray);
	virtual jvxErrorType makeSection(const std::string& nm, CjvxJsonElementList& assignedvalue);
	virtual jvxErrorType printString(std::string& writeto, jvxJsonPrintTarget tpP, jvxSize indent, const std::string indToken, 
		const std::string& initToken, const std::string& ret_token, jvxBool oneLineOutputFromHere);

	virtual jvxBool matchName(const std::string& nm, jvxBool wilcardMatch = true);
	virtual jvxBool matchType(jvxJsonElementType tp);

	virtual jvxErrorType reference_array(CjvxJsonArray** ret);
	virtual jvxErrorType reference_section(CjvxJsonElementList** ret);
	virtual jvxErrorType reference_string(jvxApiString& ret);
	virtual jvxErrorType reference_int(int* ret);
	virtual jvxErrorType reference_size(jvxSize* ret);
	virtual jvxErrorType reference_data(jvxData* ret);

};

class CjvxJsonElementList
{
private:
	std::list<CjvxJsonElement*> lst;
	jvxCBitField printMode;
	jvxBool markFlat;
public:
	CjvxJsonElementList();	
	virtual ~CjvxJsonElementList();
	virtual void reset();
	virtual void deleteAll();
	virtual void setPrintMode(jvxCBitField pTp)
	{
		printMode = pTp;
	};
	jvxCBitField getPrintMode()
	{
		return printMode;
	};
	virtual jvxBool getMarkFlat()
	{
		return markFlat;
	};
	virtual jvxBool markAllowsOneLineOutput();
	virtual jvxErrorType insertConsumeElementFront(CjvxJsonElement& in);
	virtual jvxErrorType addConsumeElement(CjvxJsonElement& in);
	virtual jvxErrorType consumeAllElements(CjvxJsonElementList& in);
	virtual jvxErrorType numElements(jvxSize* num);
	//virtual jvxErrorType reference_elementAt(jvxSize idx, CjvxJsonElement** ret);
	virtual jvxErrorType printString(std::string& writeto, jvxJsonPrintTarget tp, jvxSize indent, const std::string indToken, 
		const std::string& initToken, const std::string& ret_token, jvxBool oneLineOutputFromHere);
	virtual void printToStringView(std::string& response);
	virtual void printToJsonView(std::string& response);
	virtual void printToJsonFlat(std::string& response);
	virtual jvxErrorType reference_element(CjvxJsonElement** ret, const std::string& name,
		CjvxJsonElement::jvxJsonElementType tp = CjvxJsonElement::JVX_JSON_ASSIGNMENT_VALUE_NON,
		jvxSize idx = 0, jvxBool matchWildcard = true);

	static jvxErrorType stringToRepresentation(const std::string& in, CjvxJsonElementList& onReturn, std::vector<std::string>& errors);
};


#endif