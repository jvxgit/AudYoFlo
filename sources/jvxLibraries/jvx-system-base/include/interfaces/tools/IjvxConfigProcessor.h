
#ifndef __IJVXCONFIGPROCESSOR_H__
#define __IJVXCONFIGPROCESSOR_H__

JVX_INTERFACE IjvxConfigProcessor: public IjvxObject
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxConfigProcessor(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION addIncludePath(const char* onePath) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION clearIncludePaths() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION addIfdefToken(const char* oneDefine) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION parseFile(const char* fName, jvxBool clearIncludePathsOnFinish = true) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNumberIncludes(jvxSize* value) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getInclude_id(jvxApiString* lst, jvxSize idx) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNumberIfdefs(jvxSize* value) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getIfdef_id(jvxApiString* lst, jvxSize idx) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION parseTextField(const char* txt, const char* fName, int lineno) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getParseError(jvxApiError* err) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getConfigurationHandle(jvxConfigData** data) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNameCurrentEntry(jvxConfigData* data, jvxApiString* name) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getTypeCurrentEntry(jvxConfigData* data, jvxConfigSectionTypes* type) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getSectionIsEmpty(jvxConfigData* data, jvxBool* isit) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNumberSubsectionsCurrentSection(jvxConfigData* data, jvxSize* num) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNumberEntriesCurrentSection(jvxConfigData* data, jvxSize* num) = 0;


	virtual jvxErrorType JVX_CALLINGCONVENTION getReferenceEntryCurrentSection_id(jvxConfigData* data, jvxConfigData** dataOut, jvxSize idx) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getReferenceSubsectionCurrentSection_id(jvxConfigData* data, jvxConfigData** dataOut, jvxSize idx) = 0;


	virtual jvxErrorType JVX_CALLINGCONVENTION getReferenceSubsectionCurrentSection_name(jvxConfigData* data, jvxConfigData** dataOut, const char* nameSection) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getReferenceEntryCurrentSection_name(jvxConfigData* dataIn, jvxConfigData** dataOut, const char* nameSection) = 0;


	virtual jvxErrorType JVX_CALLINGCONVENTION getGroupStatus(jvxConfigData* data, jvxBool* value) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getAssignmentString(jvxConfigData* data, jvxApiString* value) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getReferencePathType(jvxConfigData* data, jvxBool* isAbsolute) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION getReferenceKey(jvxConfigData* data, jvxApiString* key) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION getReferenceNumberPathTokens(jvxConfigData* data, jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION getReferencePathToken_id(jvxConfigData* data, jvxApiString* token, jvxSize idx) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getAssignmentValue(jvxConfigData* data, jvxValue* value) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getAssignmentBitField(jvxConfigData* data, jvxBitField* value) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNumberValueLists(jvxConfigData* data, jvxSize* value) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getMaxSizeValueLists(jvxConfigData* data, jvxSize* mSize) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getValueList_id(jvxConfigData* data, jvxApiValueList* lst, jvxSize idx) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getValueListSize_id(jvxConfigData* data, jvxSize* sz, jvxSize idx) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNumberStrings(jvxConfigData* data, jvxSize* value) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getString_id(jvxConfigData* data, jvxApiString* lst, jvxSize idx) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getBitFieldTags(jvxConfigData* data, jvxAccessRightFlags_rwcd* acc_flags, jvxBool* pri_set,
		jvxConfigModeFlags* cfg_flags, jvxBool* sec_set) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION createEmptySectionList(jvxConfigList** lst) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION addSectionToSectionList(jvxConfigList* lst, jvxConfigData* data) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION attachSectionListToSection(jvxConfigData* toAddTo, jvxConfigList* tobeAdded) = 0;

	// Build some stuff
	virtual jvxErrorType JVX_CALLINGCONVENTION createAssignmentString(jvxConfigData** data, const char* name, const char* assignmentString, 
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION createAssignmentStringList(jvxConfigData** data, const char* name, 
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION createAssignmentValue(jvxConfigData** data, const char* name, jvxValue value, 
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION createAssignmentBitField(jvxConfigData** data, const char* name, jvxBitField assignmentBitField, 
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION createAssignmentValueList(jvxConfigData** data, const char* name, 
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION createComment(jvxConfigData** data, const char* comment) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION createEmptySection(jvxConfigData** data, const char* name, 
		jvxAccessRightFlags_rwcd* acc_flags = NULL, jvxConfigModeFlags* cfg_flags = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION addAssignmentValueToList(jvxConfigData* data, int idxArray, jvxValue value) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION addAssignmentStringToList(jvxConfigData* data, const char* entry) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION addSubsectionToSection(jvxConfigData* toAddTo, jvxConfigData* tobeAdded) = 0;


	virtual jvxErrorType JVX_CALLINGCONVENTION isSectionEmpty(jvxConfigData* toAddTo, jvxBool* isEmpty) = 0;


	virtual jvxErrorType JVX_CALLINGCONVENTION getOriginSection(jvxConfigData* dataIn, jvxApiString* fName, jvxInt32* lineno) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION printConfiguration(jvxConfigData* print, jvxApiString* str, bool compactForm = false) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION removeHandle(jvxConfigData* toBeRemoved) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION generate_section_origin_list(IjvxSectionOriginList** lst, jvxConfigData* cfgData) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION release_section_origin_list(IjvxSectionOriginList* lst) = 0;

	//virtual jvxErrorType JVX_CALLINGCONVENTION deallocate(jvxError* err) = 0;

	//virtual jvxErrorType JVX_CALLINGCONVENTION deallocate(jvxString* err) = 0;
};

#endif
