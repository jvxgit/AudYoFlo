#ifndef __TEXTPROCESSOR_CORE_H__
#define __TEXTPROCESSOR_CORE_H__

#include "jvx.h"

typedef enum
{
	GENPROPS_KEY_VALUE = 0,
	GENPROPS_KEY_LENGTH = 1
} genPropsReferencesKeys;

enum class jvxAudioPluginParamTypes
{
	AUDIO_PLUGIN_PARAM_NORMALIZED,
	AUDIO_PLUGIN_PARAM_NORM_SO,
	AUDIO_PLUGIN_PARAM_NORM_STRLIST,
	AUDIO_PLUGIN_PARAM_NORM_BOOL,
	AUDIO_PLUGIN_PARAM_ID_ONLY
};

constexpr jvxSize AUDIO_PLUGIN_PARAM_FLAGS_NOFLAGS = 0;
constexpr jvxSize AUDIO_PLUGIN_PARAM_FLAGS_AUTOMATE = 0x1;
constexpr jvxSize AUDIO_PLUGIN_PARAM_FLAGS_READONLY = 0x2;

struct onePropertySection;
struct exprOrigin
{
	std::string expr;
	std::string fName;
	std::string repl_expr;
	int lineno = -1;
};

class onePropertyAudioPluginDesription
{
public:
	jvxBool audio_plugin_id_set;
	jvxInt32 audio_plugin_id;
	exprOrigin audio_plugin_fragment_param_2_prop;
	exprOrigin audio_plugin_fragment_prop_2_param;
	jvxBool audio_plugin_param_stream_in;
	jvxBool audio_plugin_param_message_in;
	jvxBool audio_plugin_sync_active;
	jvxAudioPluginParamTypes audio_plugin_param_type;
	struct
	{
		jvxData audio_plugin_param_norm_offset;
		jvxData audio_plugin_param_norm_scalefac;
		jvxData audio_plugin_param_norm_init;
		std::string audio_plugin_param_norm_unit;
		struct
		{
			std::vector<std::string> audio_plugin_param_norm_strlist;
		} strlist;
	} norm;
	jvxSize audio_plugin_param_unit_id;
	jvxSize audio_plugin_param_num_digits;
	std::vector<std::string> audio_plugin_param_flags;
	jvxBool allocate_param;
};

struct onePropertyDefinition
{
	std::string name;
	std::string description;
	std::string descriptor;
	std::string callback_set;
	std::string callback_set_pre;
	std::string callback_get;
	std::string callback_get_post;

	struct
	{
		std::string content;
		bool set;
	} showhint;

	struct
	{
		std::string content;
		bool set;
	} moreinfo;

	struct
	{
		std::string content;
		bool set;
	} origin;

	jvxSize groupid = JVX_SIZE_UNSELECTED;
	
	bool dynamic;
	bool ignoreproblemsconfig;
	bool associateExternal;
	bool installable;
	bool nullable = false;
	bool singleElementField;
	bool fldConfigIsOptional;
	struct
	{
		std::vector<std::string> path;
		std::string key;
		bool absolute;
		bool set;
		onePropertyDefinition* solved;
		std::string fname;
		int lineno;
	} reference;


	onePropertySection* parent;

	jvxCBitField16 allowThreadingMask;
	jvxCBitField16 allowStateMask;
	jvxAccessRightFlags access_rights_read;
	jvxAccessRightFlags access_rights_write;
	jvxAccessRightFlags access_rights_create;
	jvxAccessRightFlags access_rights_destroy;
	jvxConfigModeFlags config_mode;
	jvxPropertyCategoryType category;
	jvxDataFormat format;
	jvxPropertyDecoderHintType decoderType;
	jvxPropertyContext contxt;
	bool updateOnAssociate;
	bool validOnInit;
	jvxPropertyInvalidateType invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_INACTIVE;
	jvxPropertyInvalidateType invalidateOnTest = JVX_PROPERTY_INVALIDATE_INACTIVE;

	struct
	{
		std::vector<jvxData> values;
		std::vector<std::string> strings;
		std::string string;
		bool filename_set;
	} init_set;

	struct
	{
		std::vector<std::string> strings;
		std::string extract_string;
		std::vector<jvxUInt16> selected;
		std::string extract_selected;
		std::vector<jvxUInt16> exclusive;

		std::string varname;
		std::string nummax;
		std::string numprefix;
		std::string varpostfix;
	} selection;

	struct
	{
		jvxBool enumClass;
		std::string tpName;
		std::vector<std::string> strings;
	} translations;

	struct
	{
		jvxData minVal;
		jvxData maxVal;
		jvxData value;
	} initValueInRange;

	jvxSize id;
	jvxSize numElements;

	std::string tokenType = "invalid_type";
	std::string tokenTypeAss = "invalid_type";
	std::string tokenTypeName = "invalid_type_name";
	bool associateNumberEntries = true;

	bool onlySelectionToConfig;
	bool allowOptionsConfigIfNotFound;
	//bool readonly;
	jvxPropertyAccessType accessType;
	bool generateConfigFileEntry;
	bool generateLinkObjectEntry;

	std::string updateContextToken;
	onePropertyAudioPluginDesription audioPluginDescr;
} ;

struct onePropertySection
{
	std::vector<onePropertyDefinition> properties;
	std::string name;
	std::string description;
	std::string myLocalPrefix;
	std::vector<std::string> prefixPathList;
	std::string comment;
	std::string mat_tag;
	std::string callback_set;
	std::string callback_get;
	std::string callback_set_pre;
	std::string callback_get_post;
	bool generateConfigFileEntries;
	bool generateLinkObjectEntries;
	jvxCBitField16 allowedStateMask;
	jvxCBitField16 allowedThreadingMask;
	jvxPropertyAccessType accessType;
	std::string updateContextToken;
	jvxPropertyInvalidateType invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_INACTIVE;
	jvxPropertyInvalidateType invalidateOnTest = JVX_PROPERTY_INVALIDATE_INACTIVE;
	struct
	{
		std::string content;
		bool set;
	} showhint;
	std::string default_audioplugin_sync_active;
	std::string default_audioplugin_stream_in;
	std::string default_audioplugin_message_in;
	jvxSize groupid = JVX_SIZE_UNSELECTED;
};

struct onePropertyElement;

struct onePropertyGroup
{
	std::vector<onePropertyElement> thePropertyElements;
	std::string thePrefix;
};

struct onePropertyElement
{
	onePropertyGroup thePropertyGroup;
	onePropertySection thePropertySection;
	jvxBool iamagroup;
};

struct propertiesPassThrough
{
	jvxBool duplicateSystemProperties;
};

class oneAudioPluginEntry
{
public:
	onePropertyAudioPluginDesription oneEntry;
	std::string nm_property;
	std::string expr_property;
	std::string const_property;
};

// ===========================================================

class textProcessor_core
{
private:


	typedef enum
	{
		MAT_TYPE_DOUBLE,
		MAT_TYPE_FLOAT,
		MAT_TYPE_INT64,
		MAT_TYPE_INT32,
		MAT_TYPE_INT16,
		MAT_TYPE_INT8,
		MAT_TYPE_STRING,
		MAT_TYPE_INHERIT,
		MAT_TYPE_MXARRAY,
		MAT_TYPE_NONE
	} dataTypes;

	typedef enum
	{
		MAT_DIM_2D,
		MAT_DIM_1D,
		MAT_DIM_0D
	} dimensionField;

	typedef struct
	{
		dataTypes type;
		dimensionField dimension;
		int dimX;
		int dimY;
		std::string description;
	} oneArgumentIn;

	typedef struct
	{
		dataTypes type;
		dimensionField dimension;
		int dimX;
		int dimY;
		int refX;
		int refY;
		int refType;
		std::string description;
	} oneArgumentOut;

	typedef struct
	{
		std::string nameFunctionC;
		std::string nameFunctionExpose;
		std::string descriptionFunctionExpose;
		std::string nameFunctionCpp;
		std::vector<oneArgumentIn> inputArguments;
		int acceptInputMin;
		int acceptInputMax;
		std::vector<oneArgumentOut> outputArguments;
		int acceptOutputMin;
		int acceptOutputMax;
		bool generateBackwardLink;
	}oneFunction;

	typedef struct
	{
		bool generateForLibraryUse;
		std::string nameReferenceClass;
		std::string friendlyNameReferenceClass;
		std::vector<oneFunction> lstFunctions;
	} allFunctions;

	allFunctions allFs;

	typedef struct
	{
		std::string theName;
		std::vector<std::string> thePrefix;
	} oneCallback;

	struct
	{
		std::vector<onePropertyElement> thePropertyElements;
		std::string fileOutputMiddle;
		std::string className;
		std::string prefix_component;
		std::vector<oneCallback> callbacks;
		jvxComponentType tp_component;
		std::string audio_plugin_register_tag;
		std::string default_audioplugin_sync_active;
		std::string default_audioplugin_stream_in;
		std::string default_audioplugin_message_in;
	} intermediateStruct;

	std::vector<jvxSize> ids_specific;
	std::vector<jvxSize> ids_common;

public:

	textProcessor_core(){};
	bool scanInputFromIr(jvxConfigData* theMainSection, IjvxConfigProcessor* theReader, std::string& purefilenameOutput, jvxBool pluginSupportMode);
	bool processOneGroupSection(jvxConfigData* theContent, std::string prefix, IjvxConfigProcessor* theReader, onePropertyElement& theGroup, 
		const std::vector<std::string>& lstPrefixes, std::vector<oneCallback>& lstCallbacks, propertiesPassThrough* passthrough,
		jvxBool pluginSupportMode, jvxSize groupidArg);

	bool checkForConsistency();
	void generateCode_c(const std::string& outFilenameH, jvxBool generateLineInfo);

	void generateCode_oneElement_c(onePropertyElement theElm,
			std::ostream& streamVariables,
			std::ostream& streamInit,
			std::ostream& streamAllocate,
			std::ostream& streamDeallocate,
			std::ostream& streamRegister,
			std::ostream& streamUnregister,
			std::ostream& streamReadConfig,
			std::ostream& streamWriteConfig,
			std::ostream& streamAssociateFunctions,
			std::ostream& streamDeassociateFunctions,
			std::ostream& streamTranslations,
			std::ostream& streamTagUpdate,
			int tabOffset,
			std::string structToken,
			std::string funcToken,
			std::string prefixToken,
			std::vector<std::string>& lstInitFunctions,
			std::vector<std::string>& lstAllocateFunctions,
			std::vector<std::string>& lstDeallocateFunctions,
			std::vector<std::string>& lstRegisterFunctions,
			std::vector<std::string>& lstUnregisterFunctions,
			std::vector<std::string>& lstPutConfigFunctions,
			std::vector<std::vector<std::string> >& lstPutConfigFunctionArgs,
			std::vector<std::string>& lstGetConfigFunctions,
			std::vector<std::string>& lstTagUpdateFunctions,
		std::map<jvxInt32, oneAudioPluginEntry>& collectAudioPluginsIds,
		std::list<oneAudioPluginEntry>& collectInvaidAudioPluginsIds);

	void generateCode_mat(const std::string& outDir, std::string& Cptp, std::string& Cppf);

	void generateCode_oneElement_mat(onePropertyElement& theElm, const std::string& outDir, 
		std::string& Cptp, std::string& Cppf, std::string funcPrefix, std::string propPrefix);


	// helpers:
	void produceOutput_c_prepare(onePropertyDefinition& elm);
		
	void produceOutput_c_variables(std::ostream& out, onePropertyDefinition& elm, int tabOffset);

	void produceOutput_c_init(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName, 
		std::string& fncSectionName, std::ostream& out_translators,
		std::map<jvxInt32, oneAudioPluginEntry>& collectAudioPluginsIds,
		std::list<oneAudioPluginEntry>& collectInvaidAudioPluginsIds);

	void produceOutput_c_allocate(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName, 
		std::string& funcSectionName);

	void produceOutput_c_deallocate(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName);

	void produceOutput_c_associate(std::ostream& out, std::ostream& out_inner, bool& isFirstAssociate, onePropertyDefinition& elm, std::string& propertySectionName, std::string& funcSectionName);

	void produceOutput_c_deassociate(std::ostream& out, std::ostream& out_inner, bool& isFirstAssociate, onePropertyDefinition& elm, std::string& propertySectionName, std::string& funcSectionName);

	void produceOutput_c_register(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName, std::vector<std::string>& prefixList, std::vector<oneCallback>& allCallbacks);

	void produceOutput_c_unregister(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName);

	void produceOutput_c_readconfig(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName);

	void produceOutput_c_writeconfig(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName);

	void produceOutput_c_updateTag(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName);
	
	void produceOutput_c_AudioPluginIds(std::ostream& out, std::map<jvxInt32, oneAudioPluginEntry>& audioPluginsIds,
		std::list<oneAudioPluginEntry> collectInvaidAudioPluginsIds, const std::string& register_tag, jvxBool generateLineReferences);

	void produceOutput_c_AudioPluginParameters(
		std::ostream& out_declare, std::ostream& out_run, 
		std::map<jvxInt32, oneAudioPluginEntry>& audioPluginsIds,
		const std::string& audioplugin_register_tag);
};
#endif
