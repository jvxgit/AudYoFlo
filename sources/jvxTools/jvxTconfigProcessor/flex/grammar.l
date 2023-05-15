%option noyywrap
%array
%{

#define YY_NO_UNPUT
/* Note:
 * =====
 * All the stuff below (extern functions, extern variables) should be
 * kept in an extra (C++) class to avoid pollution of the global
 * name space.
 */
#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

//#define YYDEBUG
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <list>
#include "treeList.h"
#include "grammar.tab.h"

#ifdef _WIN32
#include "io.h"
#define isatty _isatty
#define fileno _fileno
#endif

// #define FLEX_DEBUG
#ifdef YYDEBUG    // environment for debugging-mode

#define DEBUG_ECHO cout << yytext << flush;
#else

#define DEBUG_ECHO    // no debugging

#endif

#define register /* empty */

/* maximum number (+1) of characters per keyword */
#define KEY_TABLE_HORIZONTAL_SIZE 32

/* table size */
#define KEY_TABLE_VERTICAL_SIZE 29

int lineNumber=1;
string currentFilename;
string currentToken;
FILE* currentFileHandle = NULL;
int currentFilenum = 0;
vector<string> stringListLex;
string errorToken;

bool modeUseFiles = false;

// ===============================================
// Ifdefs
// ===============================================
typedef struct
{
	std::string ifdefFilename;
	int ifdefLineNo;
	bool ifdef_condtrue;
} oneIfdefEntry;

std::list<oneIfdefEntry> ifdefsStack;
std::list<std::string> ifdefList;

// ===============================================

typedef struct
{
	FILE* theFptr;
	YY_BUFFER_STATE theBuf;
	int lineNumber;
	string currentFilename;

	std::list<oneIfdefEntry> ifdefsStack;
} oneFileStackElement;

std::list<oneFileStackElement> fptrlist;
std::string txt;
int txtCounter = 0;

std::string locFlexParseError;
int locFlexParseError_lineno = 0;
std::string locFlexParseError_fname;

extern std::string errorMessage;

/** table type definition */
typedef struct
{
	const char* expression;
	int return_token;
} keyTableElement;

static keyTableElement keyTable []=
  {
	  {"SECTION"			, t_SECTION},
	  {"GROUP"			, t_GROUP},
	  {"nan"			, t_NAN},
	  {"inf"			, t_INF},
	  {"include"		, t_INCLUDE},
	  {0, -1}
  };

int FindInTable(const string& expression)
	{
		int number = 0;
		while(keyTable[number].expression != 0)
		{
			if(expression == keyTable[number].expression)
			{
				return (keyTable[number].return_token);
			}
			number ++;
		}
		return(keyTable[number].return_token);
	}

	int getChar(char* buf, int max_size)
	{
		// std::cout << "BufPtr=" << (int)buf << std::endl;
		if(modeUseFiles)
		{
			assert(currentFileHandle);
			size_t numVals = fread(buf, sizeof(char), max_size, currentFileHandle);
			if(numVals == 0)
			{
				return(YY_NULL);
			}
			//std::cout << "Char:" << buf << std::endl;
			return((int)numVals);
		}
		else
		{
			size_t numChar = (txt.size()-txtCounter) < max_size ? (txt.size()-txtCounter):max_size ;
			if(numChar == 0)
			{
				return(YY_NULL);
			}
			memcpy(buf, (txt.c_str()+txtCounter), sizeof(char)*numChar);
			txtCounter += (int)numChar;
			return((int)numChar);
		}
	}

 #define YY_INPUT(buf,result,max_size) \
    { \
		result = getChar(buf, max_size); \
    }

#include "CjvxConfigProcessor.h"
extern CjvxConfigProcessor* procStore;
 void AssignYYTEXT()
	 {
		 errorToken = yytext;

		 // Make sure that we do not excede the maximum number of characters
		 /*
		 if(strlen(yytext) > JVX_CONFIG_EXPRESSION_MAX_LENGTH)
		 {
			std::cout << "Warning: Token " << yytext << " is too long for the text parser. This token will be truncated to " <<
				JVX_CONFIG_EXPRESSION_MAX_LENGTH << " characters." << std::endl;
		 }
		 memset(yylval.Return_String_V.expression, 0, JVX_CONFIG_EXPRESSION_MAX_LENGTH+1);
		 snprintf(yylval.Return_String_V.expression, JVX_CONFIG_EXPRESSION_MAX_LENGTH, "%s", yytext);
		 */

		 jvxReturnToken* newToken = nullptr;
		 if(procStore)
		 {
			procStore->garbage_allocate_collect(&newToken);
		 }
		 yylval.Return_String_V = newToken;
		 yylval.Return_String_V->expression = yytext;
		 yylval.Return_String_V->filename = currentFilename;
		 yylval.Return_String_V->linenumber = lineNumber;
		 
	 }

	int lexIncludeFile(std::string filename)
	{
	   FILE* fPtrNew = NULL;
	   YY_BUFFER_STATE buf = NULL;
	   struct stat filestat;

	   if(!modeUseFiles)
	   {
			std::cout << "Include files not allowed in string based parsing" << std::endl;
			return(0);
	   }

#if defined(VC8)  || defined(VC9)
		fopen_s(&fPtrNew, filename.c_str(), "r");
#else
		fPtrNew = fopen(filename.c_str(), "r");
#endif
		if(!fPtrNew)
		{
			return(0);
		}
	   
	   oneFileStackElement newElm;
	   newElm.theBuf = YY_CURRENT_BUFFER;
	   newElm.theFptr = currentFileHandle;
	   newElm.lineNumber = lineNumber;
	   newElm.currentFilename = currentFilename;
	   newElm.ifdefsStack = ifdefsStack;

	   fptrlist.push_front(newElm);
	   currentFileHandle = fPtrNew;

	   // Allocate fPtr and store
	   buf = yy_create_buffer(NULL, YY_BUF_SIZE);
	   yypush_buffer_state(buf);
	   lineNumber = 1;
	   currentFilename = filename;
	   ifdefsStack.clear();
		return(1);
	}

	int lexAssignIfdefs(const std::list<std::string>& ifdefs)
	{
		ifdefList = ifdefs;
		return(1);
	}

	int lexAssignFNames(std::vector<std::string> filenames)
	{
		stringListLex = filenames;
		currentFilename = stringListLex[0];
		currentFilenum = 0;
		lineNumber = 1;

#if defined(VC8)  || defined(VC9)
		fopen_s(&currentFileHandle, currentFilename.c_str(), "r");
#else
		currentFileHandle = fopen(currentFilename.c_str(), "r");
#endif
		if(!currentFileHandle)
		{
			errorMessage = "failed to open file " + currentFilename;
			return(0);
		}
		modeUseFiles = true;
		return(1);
	}

	void lexRestart()
	{
		locFlexParseError = "";
	}

	void locReportError(std::string description, int linenoArg, std::string filenameArg)
	{
		if(locFlexParseError.empty())
		{
			locFlexParseError = filenameArg + "(" + jvx_int2String(linenoArg) + "): ---> " + description;
			locFlexParseError_lineno = linenoArg;
			locFlexParseError_fname = filenameArg;		
		}
	}

	void checkLocalFlexError(std::string& retString, int& lineno, std::string& fName)
	{
		retString = locFlexParseError;
		lineno = locFlexParseError_lineno;
		fName = locFlexParseError_fname;
	}

	// Function to set lex reader to next file or to indicate end-of-scanning
	int restartLex()
	{
		if(modeUseFiles)
		{
			currentFilename = stringListLex[currentFilenum];
#if defined(VC8)  || defined(VC9)
			fopen_s(&currentFileHandle, currentFilename.c_str(), "r");
#else
			currentFileHandle = fopen(currentFilename.c_str(), "r");
#endif
			if(!currentFileHandle)
			{
				return(0);
			}
			lineNumber = 1;
			return(1);
		}
		return(0);
	}

	int lexAssignTBuffer(std::string fileNameTxt, std::string fName, int lineNo)
	{
		stringListLex.clear();
		currentFilename = fName;
		currentFilenum = 0;
		lineNumber = lineNo;
		txt = fileNameTxt;
		txtCounter = 0;
		modeUseFiles = false;
		return(1);
	}
/*		if(isFilename)
		{
			currentFilename = fileNameTxt;
			currentFilenum = 0;
#ifdef VC8
			fopen_s(&currentFileHandle, currentFilename.c_str(), "r");
#else
			currentFileHandle = fopen(currentFilename.c_str(), "r");
#endif
			if(!currentFileHandle)
			{
				return(0);
			}
			else
			{
				modeUseFiles = true;
				return(1);
			}
		}
		else
		{
			currentFilename = "<INPUTBUFFER>";
			currentFilenum = 0;
			txt = fileNameTxt;
			txtCounter = 0;
			modeUseFiles = false;
			return(1);
		}
	 }
*/

/*	int lexAssignStr(char* buf, int lBuf)
	 {
         stringListLex.clear();
		 currentFilename = "<STRINGBUFFER>";
		 currentFilenum = 0;
		yyin = NULL;
			modeUseFiles = false;
		stateInput = yy_scan_string(buf);//, lBuf);
		return(1);
	 }
*/

 int getCurrentLineNumber()
	 {
		 return(lineNumber);
	 }

 string getCurrentFilename()
	 {
		 return(currentFilename);
	 }

 string getErrorToken()
	 {
		 return(errorToken);
	 }
%}

%s start
%x QUOTED
%x IFDEF_STARTED
%x IFDEF_SKIP
%x C_COMMENT
%x QUOTED_SKIP
%x DEFINE_STARTED
%x UNDEF_STARTED


digit 			  [0-9]
integer                   (0|([1-9]{digit}*))
size                   ([-+]?{integer})\<s\>
int16                   ([-+]?{integer})\<i16\>
int32                   ([-+]?{integer})\<i32\>
int64                   ([-+]?{integer})\<i64\>
int8                   ([-+]?{integer})\<i8\>
uint16                   ([-+]?{integer})\<ui16\>
uint32                   ([-+]?{integer})\<ui32\>
uint64                   ([-+]?{integer})\<ui64\>
uint8                   ([-+]?{integer})\<ui8\>
integer0                   ({digit}*)
int_bin                   (0b([0-1]*))
int_hex                   (0x([0-9a-fA-F]*))
x_bin                     (0b([0-1]*[x|X]+[0-1|x|X]*))
x_hex                     (0x([0-9a-fA-FxX]*))
comment                   (\/\/.*)
include                   (\#include.*)
terminals                 [\.\{\}\;\:\,\(\)\*\+\|\&\-\%\=\[\]\<\>\~\?\^]

or                        (\|\|)
and                       (\&\&)
double_pound              (\#\#)

special_character   	  [\#\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\_\|]
space_character 	  [ \t]
all_empty             [ \t\n]
format_effector		  [\t\v\r\l\f]
minf						(\-inf)
mnan						(\-nan)
end_of_line		  \n
%%

\/\/.* {
	// Comments as in C++-style accepted only
#ifdef FLEX_DEBUG
	cout << "C++ Comment: " << yytext << endl;
#endif

}

[[:alpha:]\_\/][[:alnum:]\_\/]*    {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	if(	yytext == (std::string)"ifdef")
	{
#ifdef FLEX_DEBUG
		cout << yytext << endl;
		cout << "Switching to ifdef start mode" << endl;
#endif
	
		BEGIN IFDEF_STARTED;
	}
	else if(yytext == (std::string)"endif")
	{
		if(ifdefsStack.size() > 0)
		{
			ifdefsStack.pop_back();
		}
		else
		{
			locReportError((std::string)"Unmatched \"ifdef\".", lineNumber, currentFilename);
		}
	}
	else if(yytext == (std::string)"else")
	{
		if(ifdefsStack.size() > 0)
		{
			BEGIN IFDEF_SKIP;
		}
		else
		{
			locReportError((std::string)"Unmatched \"else\".", lineNumber, currentFilename);
		}	
	}
	else if(yytext == (std::string)"define")
	{
#ifdef FLEX_DEBUG
		cout << yytext << endl;
		cout << "Switching to define start mode" << endl;
#endif
	
		BEGIN DEFINE_STARTED;
	}
	else if(yytext == (std::string)"undef")
	{
#ifdef FLEX_DEBUG
		cout << yytext << endl;
		cout << "Switching to ifdef start mode" << endl;
#endif
	
		BEGIN UNDEF_STARTED;
	}
	else
	{
		int itoken = FindInTable(yytext);
		if(itoken == -1)
		{
			AssignYYTEXT();
			return(t_ANYNAME);
		}
		else
		{
			AssignYYTEXT();
			return(itoken);
		}
	}
}

\@([[:alpha:]\_][[:alnum:]\_]*)    {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_REFERENCE);
}

([[:alpha:]\_][[:alnum:]\_]*)+(\:[[:alpha:]\_][[:alnum:]\_]*)+    {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_REFERENCE_LIST);
}

{int_bin} {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_BITFIELD);
}

[\+|\-]?{integer}  {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_INTEGER);
}

{size}  {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_SIZE);
}

{int16}  {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_INT16);
}

{int32}  {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_INT32);
}

{int64}  {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_INT64);
}

{int8}  {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_INT8);
}

{uint16}  {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_UINT16);
}

{uint32}  {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_UINT32);
}

{uint64}  {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_UINT64);
}

{uint8}  {
#ifdef FLEX_DEBUG
	//("-"|"+")*[0-9]|([1-9][0-9]*)
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_UINT8);
}
[\+|\-]?{integer}?[\.]?{digit}*[e|E]?[\+|\-]?{integer0}?  {
#ifdef FLEX_DEBUG
	// "-"*([0-9]|([1-9][0-9]*))"."[0-9]+
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_DOUBLE);
}

{int_hex}  {
#ifdef FLEX_DEBUG
	// "-"*([0-9]|([1-9][0-9]*))"."[0-9]+
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_BITFIELD);
}


{minf}  {
#ifdef FLEX_DEBUG
	// "-"*([0-9]|([1-9][0-9]*))"."[0-9]+
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_MINF);
}

{mnan}  {
#ifdef FLEX_DEBUG
	// "-"*([0-9]|([1-9][0-9]*))"."[0-9]+
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_MNAN);
}

{terminals} {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return((int)yytext[0]);
}

{special_character} {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return((int)yytext[0]);
}

\/\* {
	yymore();
	BEGIN C_COMMENT;
}

<C_COMMENT>\*\/   {

#ifdef FLEX_DEBUG
	cout << "C-Comment: " << yytext << endl;
#endif
	BEGIN(start);
}

<C_COMMENT>.   {
	yymore();
}

<C_COMMENT>\n   {
	yymore();
}

\" {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	yymore();
	BEGIN QUOTED;
}

<IFDEF_STARTED>[[:alpha:]\_\/][[:alnum:]\_\/]*     {

#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	jvxBool inIfdefList = false;
	jvxSize i;
	oneIfdefEntry newEntry;
	newEntry.ifdefFilename = currentFilename;
	newEntry.ifdefLineNo = lineNumber;
	newEntry.ifdef_condtrue = false;
	ifdefsStack.push_back(newEntry);

	// Check the ifdef!	
	auto elm = ifdefList.begin();
	for(; elm != ifdefList.end(); elm++)
	{
		if(*elm == yytext)
		{
			inIfdefList = true;
			break;
		}
	}
	if(inIfdefList)
	{
		newEntry.ifdef_condtrue = true;
		BEGIN(start);
		
	}
	else
	{
		BEGIN IFDEF_SKIP;
	}	
}

<IFDEF_SKIP>endif {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif

	if(ifdefsStack.size() > 0)
	{
		ifdefsStack.pop_back();
		BEGIN(start);
	}
	else
	{
		locReportError((std::string)"Unmatched \"ifdef\".", lineNumber, currentFilename);
	}
}

<IFDEF_SKIP>else {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
 	if(ifdefsStack.size() > 0)
	{
		BEGIN start;
	}
	else
	{
		locReportError((std::string)"Unmatched \"else\".", lineNumber, currentFilename);
	}	
}

<IFDEF_STARTED>.       {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
}

<IFDEF_SKIP>\n      {
#ifdef FLEX_DEBUG
	cout << "\\n" << endl;
#endif
	if(lineNumber >= 0)
	{
		lineNumber++;
	}
}

<QUOTED_SKIP>\\\"  {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
}

<QUOTED_SKIP>\"   {

#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	BEGIN(IFDEF_SKIP);
}

<QUOTED_SKIP>.   {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
}

<QUOTED_SKIP>\n   {
#ifdef FLEX_DEBUG
	cout << "\\n" << endl;
#endif
	if(lineNumber >= 0)
	{
		lineNumber++;
	}
}



<IFDEF_SKIP>\" {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	yymore();
	BEGIN QUOTED_SKIP;
}

<IFDEF_SKIP>.       {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
}



<DEFINE_STARTED>[[:alpha:]\_\/][[:alnum:]\_\/]*     {

#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif

	jvxBool foundEntry = false;
	auto elm = ifdefList.begin();
	for(; elm != ifdefList.end(); elm++)
	{
		if(*elm == yytext)
		{
			foundEntry = true;
		}
	}

	if(!foundEntry)
	{
		ifdefList.push_back(yytext);
	}

	
	BEGIN start;
}

<DEFINE_STARTED>.       {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
}

<UNDEF_STARTED>[[:alpha:]\_\/][[:alnum:]\_\/]*     {

#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif

	jvxBool foundEntry = false;
	auto elm = ifdefList.begin();
	for(; elm != ifdefList.end(); elm++)
	{
		if(*elm == yytext)
		{
			foundEntry = true;
			break;
		}
	}

	if(foundEntry)
	{
		ifdefList.erase(elm);
	}

	
	BEGIN start;
}

<UNDEF_STARTED>.       {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
}

<<EOF>> {
	jvxSize i;
	errorToken = "<<EOF>>";
	BEGIN(start);

	for(i = 0; i < ifdefsStack.size(); i++)
	{
		oneIfdefEntry entry;
		entry = ifdefsStack.back();

		locReportError((std::string)"Unmatched \"ifdef\".", entry.ifdefLineNo, entry.ifdefFilename);
	}
	ifdefsStack.clear();

#ifdef FLEX_DEBUG
	cout << "ENDOFFILE!" << endl;
	cout << yytext << endl;
#endif

	if(modeUseFiles)
	{
		assert(currentFileHandle);
		fclose(currentFileHandle);
	}
	currentFileHandle = NULL;

	oneFileStackElement newElm;
	if(fptrlist.size())
	{
		newElm = fptrlist.front();
		fptrlist.pop_front();
		yy_delete_buffer(YY_CURRENT_BUFFER);
		yy_switch_to_buffer(newElm.theBuf);
		currentFileHandle = newElm.theFptr;
		lineNumber = newElm.lineNumber;
		currentFilename = newElm.currentFilename;
		ifdefsStack = newElm.ifdefsStack;
	}
	else
	{
		if(++currentFilenum < (int)stringListLex.size())
		{
#ifdef FLEX_DEBUG
			cout << "Restart!" << endl;
#endif
			if(restartLex() == 0)
				yyterminate();
		}
		else
		{

#ifdef FLEX_DEBUG
			cout << "TERMINATE!" << endl;
#endif
			yyterminate();
		}
	}
}

\n      {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	if(lineNumber >= 0)
	{
		lineNumber++;
	}
}

<QUOTED>\\\"  {
#ifdef FLEX_DEBUG
	// "-"*([0-9]|([1-9][0-9]*))"."[0-9]+
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_QUOTED_SPECIALCHAR);
}

<QUOTED>\\\\  {
#ifdef FLEX_DEBUG
	// "-"*([0-9]|([1-9][0-9]*))"."[0-9]+
	cout << yytext << endl;
#endif
	AssignYYTEXT();
	return(t_QUOTED_SPECIALCHAR);
}

<QUOTED>\"   {

#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
	BEGIN(start);
	AssignYYTEXT();
	return(t_QUOTED);
}

<QUOTED>.   {
	yymore();
}

<QUOTED>\n   {
	yymore();
	if(lineNumber >= 0)
	{
		lineNumber++;
	}
}

.       {
#ifdef FLEX_DEBUG
	cout << yytext << endl;
#endif
}



%%
