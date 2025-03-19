
#include "jvx.h"
#include "common/CjvxObject.h"

#include "treeList.h"

class CjvxSectionOriginList;

class CjvxConfigProcessor: public IjvxConfigProcessor, public CjvxObject
{
public:

	typedef enum
	{
		CONFIG_NOERROR = 0,
		CONFIG_PARSEERROR = 1,
		CONFIG_FILENOTEXISTSERROR = 2
	} errorCodes;

	typedef enum
	{
		CONFIG_ELEMENT_DATA,
		CONFIG_ELEMENT_SIZE
	} entryValueListType;

private:
	treeListElement* topElementTree;

	std::string errorMessage;
	int lineError;
	std::string fNameError;
	errorCodes errorCode;

	std::vector<std::string> includeFiles;

	std::list<std::string> ifdefs;

	//std::vector<rtpConfigData*> lstReferences;
	std::list<jvxReturnToken*> garbage;

	std::map< IjvxSectionOriginList*, CjvxSectionOriginList*> allocated_SectionOrigins;

public:
	
	CjvxConfigProcessor(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	

	~CjvxConfigProcessor();
	
	void setTopElement(treeListElement* topElm)
	{
		topElementTree = topElm;
	};

	// API API API API API API API API API API API API API API API API
	virtual jvxErrorType JVX_CALLINGCONVENTION addIncludePath(const char* onePath)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clearIncludePaths() override;

	jvxErrorType JVX_CALLINGCONVENTION addIfdefToken(const char* oneDefine)override;

	// Transform string into IR
	jvxErrorType JVX_CALLINGCONVENTION parseFile(const char* fName, jvxBool clearIncludePathsOnFinish)override;
	
	jvxErrorType JVX_CALLINGCONVENTION parseTextField(const char* txt, const char* fName, int lineno)override;

	jvxErrorType JVX_CALLINGCONVENTION getParseError(jvxApiError* err)override;


	jvxErrorType JVX_CALLINGCONVENTION createEmptySectionList(jvxConfigList** lst)override;

	jvxErrorType JVX_CALLINGCONVENTION addSectionToSectionList(jvxConfigList* lst, jvxConfigData* data)override;

	jvxErrorType JVX_CALLINGCONVENTION attachSectionListToSection(jvxConfigData* toAddTo, jvxConfigList* tobeAdded)override;
	// Get access to parsed IR
	jvxErrorType JVX_CALLINGCONVENTION getConfigurationHandle(jvxConfigData** data)override;

	jvxErrorType JVX_CALLINGCONVENTION getNameCurrentEntry(jvxConfigData* data, jvxApiString* name)override;

	jvxErrorType JVX_CALLINGCONVENTION getTypeCurrentEntry(jvxConfigData* data, jvxConfigSectionTypes* type)override;

	jvxErrorType JVX_CALLINGCONVENTION getSectionIsEmpty(jvxConfigData* data, jvxBool* isit)override;


	jvxErrorType JVX_CALLINGCONVENTION getNumberSubsectionsCurrentSection(jvxConfigData* data, jvxSize* num)override;

	jvxErrorType JVX_CALLINGCONVENTION getNumberEntriesCurrentSection(jvxConfigData* data, jvxSize* num)override;


	jvxErrorType JVX_CALLINGCONVENTION getReferenceSubsectionCurrentSection_id(jvxConfigData* data, jvxConfigData** dataOut, jvxSize idx)override;

	jvxErrorType JVX_CALLINGCONVENTION getReferenceEntryCurrentSection_id(jvxConfigData* data, jvxConfigData** dataOut, jvxSize idx)override;

	
	jvxErrorType JVX_CALLINGCONVENTION getReferenceSubsectionCurrentSection_name(jvxConfigData* data, jvxConfigData** dataOut, const char* nameSection)override;

	jvxErrorType JVX_CALLINGCONVENTION getReferenceEntryCurrentSection_name(jvxConfigData* dataIn, jvxConfigData** dataOut, const char* nameSection)override;

	jvxErrorType JVX_CALLINGCONVENTION getReferencePathType(jvxConfigData* data, jvxBool* isAbsolute) override;
	jvxErrorType JVX_CALLINGCONVENTION getReferenceKey(jvxConfigData* data, jvxApiString* key) override;
	jvxErrorType JVX_CALLINGCONVENTION getReferenceNumberPathTokens(jvxConfigData* data, jvxSize* num) override;
	jvxErrorType JVX_CALLINGCONVENTION getReferencePathToken_id(jvxConfigData* data, jvxApiString* token, jvxSize idx) override;

	jvxErrorType JVX_CALLINGCONVENTION getGroupStatus(jvxConfigData* data, jvxBool* value)override;

	jvxErrorType JVX_CALLINGCONVENTION getAssignmentString(jvxConfigData* data, jvxApiString* value) override;

	jvxErrorType JVX_CALLINGCONVENTION getAssignmentBitField(jvxConfigData* data, jvxBitField* value) override;

	jvxErrorType JVX_CALLINGCONVENTION getAssignmentValue(jvxConfigData* data, jvxValue*) override;

	jvxErrorType JVX_CALLINGCONVENTION getNumberValueLists(jvxConfigData* data, jvxSize* value) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION getMaxSizeValueLists(jvxConfigData* data, jvxSize* mSize) override;

	jvxErrorType JVX_CALLINGCONVENTION getValueList_id(jvxConfigData* data, jvxApiValueList* lst, jvxSize idx) override;

	jvxErrorType JVX_CALLINGCONVENTION getValueListSize_id(jvxConfigData* data, jvxSize* sz, jvxSize idx)override;

	jvxErrorType JVX_CALLINGCONVENTION getNumberStrings(jvxConfigData* data, jvxSize* value)override;

	jvxErrorType JVX_CALLINGCONVENTION getString_id(jvxConfigData* data, jvxApiString* lst, jvxSize idx)override;

	jvxErrorType JVX_CALLINGCONVENTION getBitFieldTags(jvxConfigData* data, jvxAccessRightFlags_rwcd* acc_flags, jvxBool* pri_set,
		jvxConfigModeFlags* cfg_flags, jvxBool* sec_set)override;

	// Build some stuff
	jvxErrorType JVX_CALLINGCONVENTION createAssignmentString(jvxConfigData** data, const char* name, const char* assignmentString,
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL)override;

	jvxErrorType JVX_CALLINGCONVENTION createAssignmentBitField(jvxConfigData** data, const char* name, jvxBitField assignmentBitField,
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL)override;

	jvxErrorType JVX_CALLINGCONVENTION createAssignmentStringList(jvxConfigData** data, const char* name,
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL)override;

	jvxErrorType JVX_CALLINGCONVENTION createAssignmentValue(jvxConfigData** data, const char* name, jvxValue value,
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL)override;

	jvxErrorType JVX_CALLINGCONVENTION createAssignmentValueList(jvxConfigData** data, const char* name,
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL)override;

	jvxErrorType JVX_CALLINGCONVENTION createComment(jvxConfigData** data, const char* comment)override;

	jvxErrorType JVX_CALLINGCONVENTION createEmptySection(jvxConfigData** data, const char* name,
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL)override;


	jvxErrorType JVX_CALLINGCONVENTION addAssignmentValueToList(jvxConfigData* data, int numArray, jvxValue value)override;


	jvxErrorType JVX_CALLINGCONVENTION addAssignmentStringToList(jvxConfigData* data, const char* entry)override;

	jvxErrorType JVX_CALLINGCONVENTION addSubsectionToSection(jvxConfigData* toAddTo, jvxConfigData* tobeAdded)override;

	jvxErrorType JVX_CALLINGCONVENTION isSectionEmpty(jvxConfigData*hdl, jvxBool* isEmpty)override;

	jvxErrorType JVX_CALLINGCONVENTION getOriginSection(jvxConfigData* dataIn, jvxApiString* fName, jvxInt32* lineno)override;

	// Tranform representation into string
	jvxErrorType JVX_CALLINGCONVENTION printConfiguration(jvxConfigData* print, jvxApiString* str, bool compactForm, const char* fNameOut = "", IjvxSectionOriginList * decomposeIntoFiles = nullptr)override;

	// Remove handle
	jvxErrorType JVX_CALLINGCONVENTION removeHandle(jvxConfigData* toBeRemoved)override;

	jvxErrorType JVX_CALLINGCONVENTION getNumberIncludes(jvxSize* value)override;

	jvxErrorType JVX_CALLINGCONVENTION getInclude_id(jvxApiString* lst, jvxSize idx)override;

	jvxErrorType JVX_CALLINGCONVENTION getNumberIfdefs(jvxSize* value)override;

	jvxErrorType JVX_CALLINGCONVENTION getIfdef_id(jvxApiString* lst, jvxSize idx)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION generate_section_origin_list(IjvxSectionOriginList** lst, jvxConfigData* cfgData) override;
	static jvxErrorType generate_section_origin_list(const std::string& path, CjvxSectionOriginList* lst, treeListElement* elm);

	virtual jvxErrorType JVX_CALLINGCONVENTION release_section_origin_list(IjvxSectionOriginList* lst) override;

	jvxErrorType replaceFileOriginSections(const std::string& fName, const std::string& fNameOld) override;

	void addIncludedFile(std::string fName);

	bool garbage_allocate_collect(jvxReturnToken** oneInst);
	bool garbage_release(jvxReturnToken* oneInst);
	void garbage_clear_all();
	// Deallocate function for stuff

	//jvxErrorType JVX_CALLINGCONVENTION deallocate(jvxDataList* err);

#include "codeFragments/simplify/jvxObjects_simplify.h"

};
