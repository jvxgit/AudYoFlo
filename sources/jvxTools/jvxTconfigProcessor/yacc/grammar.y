%{

#include <stdio.h>
#include <stdlib.h>
#include <list>


#include <fstream>
#include <iostream>
#include <string>
#include "jvx.h"
#include "treeList.h"
#include "CjvxConfigProcessor.h"
#include "jvx-helpers.h"

 extern int yylex();
 extern int lineNumber;
 extern std::string getCurrentFilename();
 extern std::string getErrorToken();
 extern int getCurrentLineNumber();
extern int lexIncludeFile(std::string filename);

#define YYDEBUG 1
#define MATH_INFINITE (1.0/0.0)

std::vector<std::string> includePaths;

 std::string errorMessage;
 std::string getErrorMessage()
	 {
		 return(errorMessage);
	 }


CjvxConfigProcessor* procStore = NULL;
bool associatePostProcessor(CjvxConfigProcessor* ptr)
{
	if(procStore == NULL)
	{
		procStore = ptr;
		return(true);
	}
	return(false);
}

bool deassociatePostProcessor()
{
	procStore = NULL;
	return(true);
}

void addOneIncludePath(std::string path)
{
	includePaths.push_back(path);
}

void clearAllIncludePaths()
{
	includePaths.clear();
}

std::string removeQuotes(std::string txt)
{
	//std::cout << "REMOVE quotes: " << txt << std::endl;
	return(txt.substr(1, txt.size()-2));
}

std::string removeBackSlashSpecialChar(std::string txt)
{
	std::string returnToken = txt.substr(0, txt.size()-2);
	returnToken += txt.substr(txt.size()-1,1);
	return(returnToken);
}

//========================
int errorLinenumber = -1;
int getErrorLinenumber()
{
	return(errorLinenumber);
}

//========================
std::string errorFilename = "";
std::string getErrorFilename()
{
	return(errorFilename);
}

treeList* listWhenReady = NULL;

#define YYPARSE_RETURN_TYPE int

%}


%{
	const char* CURRENTLY_NOT_IMPLEMENTED = "(currently not implemented)";
	const char* AT_OR_ABOVE = "(error occurred at or above specified location)";

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//------------------- ***   *  *  *    ****   *** -------------------------------
//------------------- * *   *  *  *    *     * ----------------------------------
//------------------- **    *  *  *    ***   *** --------------------------------
//------------------- * *   *  *  *    *        * -------------------------------
//------------------- * *    **   **** ****   ** --------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

	void yyerror(const std::string& str);
	void yyerrorfile(const std::string& str);

	%}

%union{
	jvxReturnToken*	Return_String_V;
    treeList * list_V;
    treeListElement* list_elm_V;
	std::vector<jvxData>*  Double_Vector_V;
	std::vector<jvxSize>*  Size_Vector_V;
	std::vector<std::vector<jvxData> >*  Double_2Vector_V;
	std::vector<std::vector<jvxSize> >*  Size_2Vector_V;
	
	jvxValue* Value_V;
	std::vector<jvxValue>* Value_Vector_V;
	std::vector<std::vector<jvxValue> >* Value_2Vector_V;

	std::vector<std::string>*  String_List_V;
	std::string* String_V;
	jvxData* Double_Ptr_V;
	jvxCBitField BitField_V;
	}
%{


/* keywords
  ========== */
%}
/*t_Space */

%token t_SECTION t_GROUP t_ANYNAME t_QUOTED t_QUOTED_SPECIALCHAR t_EOF t_INTEGER t_DOUBLE t_NAN t_MNAN t_INF t_MINF t_REFERENCE t_REFERENCE_LIST t_BITFIELD t_INCLUDE t_SIZE t_INT16 t_INT32 t_INT64 t_INT8 t_UINT16 t_UINT32 t_UINT64 t_UINT8

%%

/*=============================================================*/

start :
{
	// For debug, activate this!!
	//yydebug = 1;

	errorLinenumber = -1;
	errorFilename = "";
	errorMessage = "";

	/*if(reader)
		reader->setCurrentSectionName("MAIN");*/
}
t_SECTION t_ANYNAME acc_cfg_flags '{' section '}' ';'
{
	std::string ss;
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_SECTION);
	$<list_elm_V>$->setOrigin($<Return_String_V>3->filename, $<Return_String_V>3->linenumber);
	if($<String_V>4)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>4);
		delete $<String_V>4;
	}
	$<list_elm_V>$->addSubSection($<list_V>6, $<Return_String_V>3->expression);

	/*
	$<list_elm_V>$->outputToString(ss);
	std::cout << ss << std::endl;
	*/

	if(procStore)
	{
		procStore->setTopElement($<list_elm_V>$);

		// Indeed, I have moved the string handling to a garbage collector
		// since the yacc code is too old to solve he allocate/deallocate 
		// properly with a union.
		// If the memory consumption is too large, I might have to change the structure
		procStore->garbage_clear_all();
	}	
}
;

section:
section subsection
{
	if($<list_elm_V>2)
	{
		$<list_V>1->push_back($<list_elm_V>2);
		$<list_V>$ = $<list_V>1;
	}
}
|
{
	/* Create empty list */
	$<list_V>$ = new treeList;
}
;


subsection: t_SECTION t_ANYNAME acc_cfg_flags '{' section '}' ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_SECTION);
	$<list_elm_V>$->setOrigin($<Return_String_V>2->filename, $<Return_String_V>2->linenumber);
	if($<String_V>3)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>3);
		delete $<String_V>3;
	}
	$<list_elm_V>$->addSubSection($<list_V>5, $<Return_String_V>2->expression);
}
| t_GROUP t_ANYNAME acc_cfg_flags '{' section '}' ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_SECTION);
	$<list_elm_V>$->setOrigin($<Return_String_V>2->filename, $<Return_String_V>2->linenumber);
	if($<String_V>3)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>3);
		delete $<String_V>3;
	}
	$<list_elm_V>$->setGroupStatus(true);
	$<list_elm_V>$->addSubSection($<list_V>5, $<Return_String_V>2->expression);
}
| t_ANYNAME acc_cfg_flags '=' '[' valuearraylist_plusempty ']' ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_VALUELIST);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
	}
	if($<Value_2Vector_V>5)
	{
		$<list_elm_V>$->setToAssignmentValueList(*$<Value_2Vector_V>5, $<Return_String_V>1->expression);
		delete($<Value_2Vector_V>5);
	}
	else
	{
		$<list_elm_V>$->setToEmptyElement(treeListElement::VALUELIST, $<Return_String_V>1->expression);
	}
	
}
| t_ANYNAME acc_cfg_flags '=' '[' valuelist ']' ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_VALUELIST);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
	}
	
	if($<Value_Vector_V>5)
	{
		std::vector<std::vector<jvxValue> > newLst;
		newLst.push_back(*$<Value_Vector_V>5);
		$<list_elm_V>$->setToAssignmentValueList(newLst, $<Return_String_V>1->expression);		
		delete($<Value_Vector_V>5);
	}
	else
	{
		$<list_elm_V>$->setToEmptyElement(treeListElement::VALUELIST, $<Return_String_V>1->expression);
	}
}
| t_ANYNAME acc_cfg_flags '=' t_REFERENCE '(' t_REFERENCE_LIST ')' ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_VALUELIST);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
	}
	$<list_elm_V>$->setToReference($<Return_String_V>4->expression, $<Return_String_V>6->expression, $<Return_String_V>1->expression, false);
}
| t_ANYNAME acc_cfg_flags '=' t_REFERENCE '(' t_ANYNAME ')' ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_VALUELIST);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
	}
	$<list_elm_V>$->setToReference($<Return_String_V>4->expression, $<Return_String_V>6->expression, $<Return_String_V>1->expression, false);
	
}
| t_ANYNAME acc_cfg_flags '=' t_REFERENCE '(' ':' t_REFERENCE_LIST ')' ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_VALUELIST);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
	}
	$<list_elm_V>$->setToReference($<Return_String_V>4->expression, $<Return_String_V>7->expression, $<Return_String_V>1->expression, true);	
}
| t_ANYNAME acc_cfg_flags '=' t_REFERENCE '(' ':' t_ANYNAME ')' ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_VALUELIST);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
	}
	$<list_elm_V>$->setToReference($<Return_String_V>4->expression, $<Return_String_V>7->expression, $<Return_String_V>1->expression, true);
}
| t_ANYNAME acc_cfg_flags '=' '{' valuelist '}' ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_VALUELIST);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
	}
	if($<Value_Vector_V>5)
	{
		std::vector<std::vector<jvxValue> > lst;
		lst.push_back(*$<Value_Vector_V>5);
		$<list_elm_V>$->setToAssignmentValueList(lst, $<Return_String_V>1->expression);
		delete($<Value_Vector_V>5);
	}
	else
	{
		$<list_elm_V>$->setToEmptyElement(treeListElement::VALUELIST, $<Return_String_V>1->expression);
	}
}
| t_ANYNAME acc_cfg_flags '=' '{' stringlist_plusempty '}' ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_STRINGLIST);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
	}
	if($<String_List_V>5)
	{
		$<list_elm_V>$->setToAssignmentStringList(*$<String_List_V>5, $<Return_String_V>1->expression);
		delete($<String_List_V>5);
	}
	else
	{
		$<list_elm_V>$->setToEmptyElement(treeListElement::STRINGLIST, $<Return_String_V>1->expression);
	}
}
| t_ANYNAME acc_cfg_flags '=' assignement_hexstr ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
	}
	$<list_elm_V>$->setToAssignmentHexString(*$<String_V>4, $<Return_String_V>1->expression);	
	delete($<String_V>4);
}
| t_ANYNAME acc_cfg_flags '=' assignement_str ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
	}
	$<list_elm_V>$->setToAssignmentString(*$<String_V>4, $<Return_String_V>1->expression);
	delete($<String_V>4);
}
| t_ANYNAME acc_cfg_flags '=' singlevalue ';'
{
	$<list_elm_V>$ = new treeListElement();//IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING);
	$<list_elm_V>$->setOrigin($<Return_String_V>1->filename, $<Return_String_V>1->linenumber);
	if($<String_V>2)
	{
		$<list_elm_V>$->setPriSecFlags(*$<String_V>2);
		delete $<String_V>2;
		}
	$<list_elm_V>$->setToAssignmentValue(*$<Value_V>4, $<Return_String_V>1->expression);	
	delete($<Value_V>4);
}
| t_INCLUDE quoted ';' 
{
	std::string fName = removeQuotes(*$<String_V>2);
	fName = jvx_replaceCharacter(fName, JVX_SEPARATOR_DIR_CHAR, JVX_SEPARATOR_DIR_CHAR_THE_OTHER);
	int i;
	//std::cout << "Open include file " << *$<String_V>2 <<std::endl;

	int retVal = 0;
	retVal = lexIncludeFile(fName);
	if(retVal == 0)
	{
		for(i = 0; i < includePaths.size(); i++)
		{
			std::string tmp = includePaths[i] + JVX_SEPARATOR_DIR + fName;
			retVal = lexIncludeFile(tmp);
			if(retVal != 0)
			{
				jvx_replaceStrInStr(tmp, JVX_SEPARATOR_DIR_THE_OTHER, JVX_SEPARATOR_DIR);
				if(procStore)
				{
					procStore->addIncludedFile(tmp);
				}
				break;
			}
		}
	}
	else
	{
		if(procStore)
		{
			procStore->addIncludedFile(fName);
		}
	}
	if(retVal == 0)
	{
		yyerrorfile(fName);
		YYABORT;
	}
	delete($<String_V>2); 
	$<list_elm_V>$ = NULL;
}
;

acc_cfg_flags: 
'<' assignement_hexstr ',' assignement_hexstr'>'
{
	*$<String_V>2 += ",";
	*$<String_V>2 += *$<String_V>4;
	$<String_V>$ = $<String_V>2;
}
| '<' assignement_hexstr '>'
{
	*$<String_V>2 += ",";
	$<String_V>$ = $<String_V>2;
}
| '<' ',' assignement_hexstr '>'
{
	*$<String_V>3 = "," + *$<String_V>3;
	$<String_V>$ = $<String_V>2;
}
|
{
	$<String_V>$ = NULL;
}
;
valuearraylist_plusempty:
valuearraylist
{
	$<Value_2Vector_V>$ = $<Value_2Vector_V>1;
}
|
{
	$<Value_2Vector_V>$ = NULL;
};

stringlist_plusempty:
stringlist
{
	$<String_List_V>$ = $<String_List_V>1;
}
|
{
	$<String_List_V>$ = NULL;
};

valuearraylist:
valuearraylist ',' '[' valuelist ']'
{
	$<Value_2Vector_V>1->push_back(*$<Value_Vector_V>4);
	delete($<Value_Vector_V>4);
	$<Value_2Vector_V>$ = $<Value_2Vector_V>1;
}
| valuearraylist ';' '[' valuelist ']'
{
	$<Value_2Vector_V>1->push_back(*$<Value_Vector_V>4);
	delete($<Value_Vector_V>4);
	$<Value_2Vector_V>$ = $<Value_2Vector_V>1;
}
| '[' valuelist ']'
{
	$<Value_2Vector_V>$ = new std::vector<std::vector<jvxValue> >;
	$<Value_2Vector_V>$->push_back(*$<Value_Vector_V>2);
	delete($<Value_Vector_V>2);
}
;

valuelist:
valuelist ',' singlevalue
{
	$<Value_Vector_V>1->push_back(*$<Value_V>3);
	$<Value_Vector_V>$ = $<Value_Vector_V>1;
	delete($<Value_V>3);
}
| singlevalue
{
	$<Value_Vector_V>$ = new std::vector<jvxValue>;
	$<Value_Vector_V>$->push_back(*$<Value_V>1);
	delete($<Value_V>1);
}
;

singlevalue:
t_INTEGER
{
	$<Value_V>$ = new jvxValue((jvxData)atoi($<Return_String_V>1->expression.c_str()));
}
| t_DOUBLE
{
	$<Value_V>$ = new jvxValue((jvxData)atof($<Return_String_V>1->expression.c_str()));
}
| t_NAN
{
	$<Value_V>$ = new jvxValue((jvxData)JVX_MATH_NAN);
}
| t_INF
{
	$<Value_V>$ = new jvxValue((jvxData)JVX_MATH_INFINITE);
}
| t_MINF
{
	$<Value_V>$ = new jvxValue((jvxData)-JVX_MATH_INFINITE);
}
| t_MNAN
{
	$<Value_V>$ = new jvxValue((jvxData)-JVX_MATH_NAN);
}
| t_SIZE
{
	jvxSize valS = 0;
	std::string str = $<Return_String_V>1->expression;
	str = str.substr(0, str.size()-3);
	if(str == "-1")
	{
		valS = JVX_SIZE_UNSELECTED;
	}
	else if(str == "-2")
	{
		valS = JVX_SIZE_DONTCARE;
	}
	else if(str == "-3")
	{
		valS = JVX_SIZE_SLOT_RETAIN;
	}
    else if(str == "-4")
    {
        valS = JVX_SIZE_SLOT_OFF_SYSTEM;
    }
	else if(str == "-5")
    {
        valS = JVX_SIZE_STATE_INCREMENT;
    }
	else if(str == "-6")
    {
        valS = JVX_SIZE_STATE_DECREMENT;
    }
	else
	{
		if (sscanf(str.c_str(), JVX_PRINTF_CAST_SIZE, &valS) != 1)
		{
			std::cout << "Problem reading " << str << " into a jvxSize variable." << std::endl;
		}
	}
	$<Value_V>$ = new jvxValue(valS);
}
| t_INT16
{
	jvxBool err = false;
	jvxInt64 valI = 0;
	std::string str = $<Return_String_V>1->expression;
	str = str.substr(0, str.size()-5);
	
	valI = jvx_string2Int64(str, err);
	//if (sscanf(str.c_str(), "%d", &valI) != 1)
	if(err)
	{
		std::cout << "Problem reading " << str << " into a jvxInt16 variable." << std::endl;
	}
	jvx_check_value(valI, JVX_DATAFORMAT_16BIT_LE);
	$<Value_V>$ = new jvxValue((jvxInt16)valI);
}
| t_INT32
{
	jvxBool err = false;
	jvxInt64 valI = 0;
	std::string str = $<Return_String_V>1->expression;
	str = str.substr(0, str.size()-5);
	
	valI = jvx_string2Int64(str, err);
	//if (sscanf(str.c_str(), "%d", &valI) != 1)
	if(err)
	{
		std::cout << "Problem reading " << str << " into a jvxInt32 variable." << std::endl;
	}
	jvx_check_value(valI, JVX_DATAFORMAT_32BIT_LE);
	$<Value_V>$ = new jvxValue((jvxInt32)valI);
}
| t_INT64
{
	jvxBool err = false;
	jvxInt64 valI = 0;
	std::string str = $<Return_String_V>1->expression;
	str = str.substr(0, str.size()-5);
	
	valI = jvx_string2Int64(str, err);
	//if (sscanf(str.c_str(), "%d", &valI) != 1)
	if(err)
	{
		std::cout << "Problem reading " << str << " into a jvxInt64 variable." << std::endl;
	}
	$<Value_V>$ = new jvxValue(valI);
}
| t_INT8
{
	jvxBool err = false;
	jvxUInt64 valI = 0;
	std::string str = $<Return_String_V>1->expression;
	str = str.substr(0, str.size()-5);
	
	valI = jvx_string2Int64(str, err);
	//if (sscanf(str.c_str(), "%d", &valI) != 1)
	if(err)
	{
		std::cout << "Problem reading " << str << " into a jvxInt8 variable." << std::endl;
	}
	jvx_check_value(valI, JVX_DATAFORMAT_8BIT);
	$<Value_V>$ = new jvxValue((jvxInt8)valI);
}
| t_UINT16
{
	jvxBool err = false;
	jvxUInt64 valI = 0;
	std::string str = $<Return_String_V>1->expression;
	str = str.substr(0, str.size()-6);
	
    valI = jvx_string2UInt64(str, err);
	//if (sscanf(str.c_str(), "%d", &valI) != 1)
	if(err)
	{
		std::cout << "Problem reading " << str << " into a jvxUInt16 variable." << std::endl;
	}
	jvx_check_value(valI, JVX_DATAFORMAT_U16BIT_LE);
	$<Value_V>$ = new jvxValue((jvxUInt16)valI);
}
| t_UINT32
{
	jvxBool err = false;
	jvxUInt64 valI = 0;
	std::string str = $<Return_String_V>1->expression;
	str = str.substr(0, str.size()-6);
	
    valI = jvx_string2UInt64(str, err);
	//if (sscanf(str.c_str(), "%d", &valI) != 1)
	if(err)
	{
		std::cout << "Problem reading " << str << " into a jvxUInt32 variable." << std::endl;
	}
	jvx_check_value(valI, JVX_DATAFORMAT_U32BIT_LE);
	$<Value_V>$ = new jvxValue((jvxUInt32)valI);
}
| t_UINT64
{
	jvxBool err = false;
	jvxUInt64 valI = 0;
	std::string str = $<Return_String_V>1->expression;
	str = str.substr(0, str.size()-6);
	
    valI = jvx_string2UInt64(str, err);
	//if (sscanf(str.c_str(), "%d", &valI) != 1)
	if(err)
	{
		std::cout << "Problem reading " << str << " into a jvxUInt64 variable." << std::endl;
	}
	$<Value_V>$ = new jvxValue(valI);
}
| t_UINT8
{
	jvxBool err = false;
	jvxUInt64 valI = 0;
	std::string str = $<Return_String_V>1->expression;
	str = str.substr(0, str.size()-6);
	
    valI = jvx_string2UInt64(str, err);
	//if (sscanf(str.c_str(), "%d", &valI) != 1)
	if(err)
	{
		std::cout << "Problem reading " << str << " into a jvxUInt8 variable." << std::endl;
	}
	jvx_check_value(valI, JVX_DATAFORMAT_U8BIT);
	$<Value_V>$ = new jvxValue((jvxUInt8)valI);
};

stringlist:
stringlist ',' quoted
{
	//$<String_List_V>1->push_back(removeQuotes($<Return_String_V>3->expression));
	$<String_List_V>1->push_back(removeQuotes(*$<String_V>3));
	delete($<String_V>3);
	$<String_List_V>$ = $<String_List_V>1;
}
| quoted
{
	$<String_List_V>$ = new std::vector<std::string>;
	//$<String_List_V>$->push_back(removeQuotes($<Return_String_V>1->expression));
	$<String_List_V>$->push_back(removeQuotes(*$<String_V>1));
	delete($<String_V>1);
};

assignement_str:
quoted
{
	$<String_V>$ = new std::string;
	//*$<String_V>$ = removeQuotes($<Return_String_V>1->expression);
	*$<String_V>$ = removeQuotes(*$<String_V>1);
	delete($<String_V>1);
};

assignement_hexstr:
t_BITFIELD
{
	$<String_V>$ = new std::string;
	*$<String_V>$ = $<Return_String_V>1->expression;
}

quoted:
quoted t_QUOTED
{
	$<String_V>$ = $<String_V>1;
	*$<String_V>$ += $<Return_String_V>2->expression;
}
| quoted t_QUOTED_SPECIALCHAR
{
	$<String_V>$ = $<String_V>1;
	*$<String_V>$ += $<Return_String_V>2->expression;
	*$<String_V>$ = removeBackSlashSpecialChar(*$<String_V>$); // Remove the special character backslash
}
| t_QUOTED
{
	$<String_V>$ = new std::string;
	*$<String_V>$ = $<Return_String_V>1->expression;
}
| t_QUOTED_SPECIALCHAR
{
	$<String_V>$ = new std::string;
	*$<String_V>$ = $<Return_String_V>1->expression;
	*$<String_V>$ = removeBackSlashSpecialChar(*$<String_V>$); // Remove the special character backslash
};


%%

void yyerror(const std::string& str)
{
//----------------------------------------------------------------------
	errorLinenumber = getCurrentLineNumber();
	errorFilename = getCurrentFilename();
	errorMessage += getCurrentFilename() + "(" + jvx_int2String(getCurrentLineNumber()) +
		"):" + str + ":---> \"" + std::string(getErrorToken()) +"\" is wrong statement.";
//----------------------------------------------------------------------
}

void yyerrorfile(const std::string& fName)
{
//----------------------------------------------------------------------
	errorLinenumber = getCurrentLineNumber();
	errorFilename = getCurrentFilename();
	errorMessage += getCurrentFilename() + "(" + jvx_int2String(getCurrentLineNumber()) +
		"): ---> " + " unable to open include file " + fName + ".";
//----------------------------------------------------------------------
}


// END-
