#ifndef _TREELIST_H__
#define _TREELIST_H__

#include <string>
#include <vector>

#include "jvx.h"
class CjvxSectionOriginList;

class jvxReturnToken
{
public:
	jvxReturnToken()
	{
		// std::cout << "Allocate" << std::endl;
	};
	~jvxReturnToken()
	{
		// std::cout << "Deallocate" << std::endl;
	};

public:
	//char expression[JVX_CONFIG_EXPRESSION_MAX_LENGTH+1]; // YYLMAX leads to a stack overflow -- 1024 
	std::string expression;
	std::string filename;
	int linenumber;
};

class treeList;

class treeListElement
{
public:
	typedef enum
	{
		STARTELM = 0,
		STOPELM = 1,
		SECTION = 2,
		VALUELIST = 3,
		ASSIGNMENTVALUE = 4,
		ASSIGNMENTSTRING = 5,
		STRINGLIST = 6,
		COMMENT = 7,
		REFERENCE = 8,
		ASSIGNMENTHEXSTRING = 9,
		/* */
		/*ASSIGNMENTINT = 9,*/
		UNDEFINED = 12
	} elementType;

	bool isGroup;

	//bool isReference;
	elementType type;

	treeList* belongsto;
	treeListElement* next;
	treeListElement* previous;

	//! Name of current element, can be section name but also name for an assignement
	std::string nameElement;

	treeList* subsection;

    //==================== For VALUELIST====================
	//! A value list is stored as fields in a two dimensional list
	std::vector<std::vector<jvxValue> > dataArrayList;

    //==================== For ASSIGNEMENTVALUE====================
	//! A simple assignement is stored as simple value
	jvxValue value;

    //==================== For ASSIGNEMENTSTRING====================
	//! A simple assigned string is stored as a string
	std::string assignedString;  // Remove quotes from specification in config file

    //==================== For STRINGLIST:====================
	//! A list of strings is stored as STL vector
	std::vector<std::string> assignedStringList; // Remove quotes from specification

	//=================== For References ==========================
	std::vector<std::string> referenceList;
	std::string referenceKey;
	bool referenceAbsolute;

	bool isEmpty;

	std::string filename;

	jvxAccessRightFlags_rwcd acc_flags;
	jvxBool acc_flags_set;

	jvxConfigModeFlags cfg_flags;
	jvxBool cfg_flags_set;

	int lineno;
public:

	treeListElement(jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL);
	
	~treeListElement();

	treeListElement* copy();

	void setGroupStatus(bool isGroup);

	void addSubSection(treeList* addList, std::string name);

	void setToAssignmentValue(jvxValue val, std::string name);
	
	void setToAssignmentHexValue(jvxBitField val, std::string name);

	void setToReference(std::string expression_key, std::string expression_ref, std::string name, bool absolutePath);

	void setToAssignmentString(std::string val, std::string name);

	void setToAssignmentHexString(std::string val, std::string name);

	void setToAssignmentValueList(std::string name);

	void setToAssignmentValueList(std::vector<std::vector<jvxValue> >& newList, std::string name);

	void setToAssignmentStringList(std::string name);
	
	void setToAssignmentStringList(std::vector<std::string> & newList, std::string name);

	void setToEmptySection(std::string name);

	void setToEmptyElement(elementType tp, std::string name);

	void setToComment(std::string comment);

	void setOrigin(std::string fName, int lineno);

	void setPriSecFlags(const std::string& bf);

	//void createValueList(std::string name, int numArraies);

	//void createValueSizeList(std::string name, int numArraies);

	bool addElementAssignmentValueList(jvxValue value, int idxArray = -1);
	
	bool addAssignmentValueList(std::vector<jvxValue> newList );
	

	bool addElementAssignmentStringList(std::string value);

	int getNumberSubsections();

	int getNumberEntries();

	bool getCopySubsectionSection_id(treeListElement** dataOut, jvxSize idx);

	bool getCopySubsectionSection_name(treeListElement** dataOut, const char* nameSection);

	bool getReferenceSubsectionSection_id(treeListElement** dataOut, jvxSize idx);

	bool getReferenceEntrySection_id(treeListElement** dataOut, jvxSize idx);

	bool getReferenceSubsectionSection_name(treeListElement** dataOut, const char* nameSection);

	bool getReferenceEntrySection_name(treeListElement** dataOut, const char* nameSection);

	bool outputToString(const std::string& path, const std::string& origin, std::string& bufOut, 
		int numTabs = 0, bool compactForm = false, CjvxSectionOriginList* decomposeIntoFiles = nullptr,
		std::list<std::string>* includeReferencesThisSection = nullptr);

	bool getOriginSection(std::string& str, int& lineno);

	elementType getElementType() { return type; };

	std::string getElementName() { return nameElement; };

	bool isSectionEmpty(jvxBool* isEmpty);
};

class treeList
{
	friend class treeListElement;

	// the concatenated elements
	treeListElement* content;

	// Upwards reference
	treeListElement* refSection;
public:

	treeList();
	~treeList();
	void push_back(treeListElement* newObject);
	void push_front(treeListElement* newObject);
	void setReference(treeListElement* tobereferenced);
	void print(unsigned int numTabs = 0);
	void outputToString(const std::string& path, const std::string& origin, std::string& bufOut, 
		unsigned int numTabs, bool compactForm, CjvxSectionOriginList* decomposeIntoFiles,
		std::list<std::string>* includeReferencesThisSectionIn);
};

#endif
