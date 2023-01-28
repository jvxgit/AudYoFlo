#ifndef __TJVXPROPERTYIDS_H__
#define __TJVXPROPERTYIDS_H__

#define JVXPROPERRTY_SETTYPE_NUM 4

typedef enum
{
	JVX_THREADING_INIT = 0x1,
	JVX_THREADING_ASYNC = 0x2,
	JVX_THREADING_SYNC = 0x4,
	JVX_THREADING_DONTCARE = 0xFFFF
} jvxPropertySetType;

typedef enum
{
	JVX_PROPERTY_CATEGORY_UNKNOWN = 0,
	JVX_PROPERTY_CATEGORY_PREDEFINED = 1,
	JVX_PROPERTY_CATEGORY_UNSPECIFIC = 2,
	JVX_PROPERTY_CATEGORY_LIMIT
} jvxPropertyCategoryType;

typedef enum
{
	JVX_PROPERTY_CONTEXT_UNKNOWN = 0,
	JVX_PROPERTY_CONTEXT_PARAMETER = 1,
	JVX_PROPERTY_CONTEXT_INFO = 2,
	JVX_PROPERTY_CONTEXT_RESULT = 3,
	JVX_PROPERTY_CONTEXT_COMMAND = 4,
	JVX_PROPERTY_CONTEXT_VIEWBUFFER = 5,
	JVX_PROPERTY_CONTEXT_LIMIT = 6
} jvxPropertyContext;

typedef enum
{
	JVX_PROPERTY_DECODER_NONE = 0,
	JVX_PROPERTY_DECODER_FILENAME_OPEN,
	JVX_PROPERTY_DECODER_DIRECTORY_SELECT,
	JVX_PROPERTY_DECODER_FILENAME_SAVE,
	JVX_PROPERTY_DECODER_IP_ADDRESS,
	JVX_PROPERTY_DECODER_PLOT_ARRAY,
	JVX_PROPERTY_DECODER_BITFIELD,
	JVX_PROPERTY_DECODER_SINGLE_SELECTION,
	JVX_PROPERTY_DECODER_MULTI_SELECTION,
	JVX_PROPERTY_DECODER_FORMAT_IDX,

	JVX_PROPERTY_DECODER_PROGRESSBAR,
	JVX_PROPERTY_DECODER_INPUT_FILE_LIST,
	JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST,
	JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER,
	JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER,
	JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER,
	JVX_PROPERTY_DECODER_COMMAND,
	JVX_PROPERTY_DECODER_SIMPLE_ONOFF,
	JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE,
	JVX_PROPERTY_DECODER_SUBFORMAT_IDX,

	JVX_PROPERTY_DECODER_ENUM_TYPE,
	JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE,
	JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG,

	JVX_PROPERTY_DECODER_LIMIT
} jvxPropertyDecoderHintType;

typedef enum
{
	JVX_PROPERTY_INFO_UNKNOWN = 0,
	JVX_PROPERTY_INFO_SHOWHINT = 1,
	JVX_PROPERTY_INFO_MOREINFO = 2,
	JVX_PROPERTY_INFO_ORIGIN = 3,
	JVX_PROPERTY_INFO_UPDATE_CONTENT_TOKEN = 4,
	JVX_PROPERTY_INFO_RECOMMENDED_SIZE_Y = 5,
	JVX_PROPERTY_INFO_RECOMMENDED_SIZE_X = 6,
	JVX_PROPERTY_INFO_GROUPID = 7,
	JVX_PROPERTY_INFO_LIMIT,
	JVX_PROPERTY_USER_OFFSET = 128
} jvxPropertyExtendedInfoType;

typedef enum
{
	JVX_PROPERTY_INVALIDATE_INACTIVE,
	JVX_PROPERTY_INVALIDATE_CONTENT,
	JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT,
	JVX_PROPERTY_INVALIDATE_LIMIT
} jvxPropertyInvalidateType;

typedef enum
{
	AYF_BACKEND_REPORT_SET_PROPERTY_ACTIVE,
	AYF_BACKEND_REPORT_SET_PROPERTY_INACTIVE
} AyfBackendReportPropertySet;

typedef struct
{
	struct
	{
		std::string content;
		jvxBool valid;
	} showhint;
	struct
	{
		std::string content;
		jvxBool valid;
	} moreinfo;
	struct
	{
		std::string content;
		jvxBool valid;
	} origin;
	struct
	{
		jvxSize content;
		jvxBool valid;
	} groupid;
	struct
	{
		std::string content;
		jvxBool valid;
	} update_context;
} jvxExtendedProps;
/*
typedef enum
{

	JVX_COMMON_PROPERTY_SCANNER_BATCHSIZE = 0
} jvxCommonPropertiesScanner;
*/

typedef enum
{
	JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC = 0,
	JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS = 1,
	JVX_PROPERTY_CALL_PURPOSE_TAKE_OVER = 2,
	JVX_PROPERTY_CALL_PURPOSE_PROPERTY_POOL = 3,
	JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY = 4,
	JVX_PROPERTY_CALL_PURPOSE_GET_ACCESS_DATA = 5
	/*JVX_PROPERTY_CALL_PURPOSE_COMPONENT_RECONFIGURE = 4*/
} jvxPropertyCallPurpose;

typedef enum
{
	JVX_PROPERTY_ACCESS_NONE = 0,
	JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE = 1,
	JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT = 2,
	JVX_PROPERTY_ACCESS_READ_ONLY = 3,
	JVX_PROPERTY_ACCESS_READ_ONLY_ON_START = 4,
	JVX_PROPERTY_ACCESS_WRITE_ONLY = 5,
	JVX_PROPERTY_ACCESS_LIMIT
} jvxPropertyAccessType;

enum class jvxPropertyCallFlags
{
	JVX_PROPERTY_CALL_FLAG_NONE = 0,
	JVX_PROPERTY_CALL_FLAG_PREVENT_TRIGGER_TEST = 0x1
};

typedef struct
{
	std::string nameProperty;
	jvxInt32 id;
	jvxDataFormat format;
	jvxInt32 numElements;
	jvxPropertyDecoderHintType hintTp;
	std::string descriptor;
} oneEntryProperties;

typedef struct
{
	jvxPropertyCallback cb;
	jvxHandle* cb_priv;
} jvxCallbackPrivate;

// Types:
// jvxInt16, jvxInt64, jvxInt64, jvxSize, jvxCBool
// std::string, jvxValueInRange, jvx_oneSelectionOption_cpp, 

/*
class jvxPropertyAccessDescriptor
{
public:
	jvxPropertyCategoryType category = JVX_PROPERTY_CATEGORY_UNKNOWN;
	jvxUInt64 settype = 0;
	jvxUInt64 stateAccess = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxSize num = 0;
	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_NONE;
	jvxPropertyDecoderHintType decTp = JVX_PROPERTY_DECODER_NONE;
	jvxSize globalIdx = JVX_SIZE_UNSELECTED;
	jvxPropertyContext ctxt = JVX_PROPERTY_CONTEXT_UNKNOWN;
	jvxApiString name;
	jvxApiString description;
	jvxApiString descriptor;
	jvxBool isValid = false;
	jvxFlagTag accessFlags = 0;
};
*/

/*
#define JVX_PROPERTY_CALL_ACCESS_DESCRIPTOR(propDesc) \
	& propDescr.category, & propDescr.settype, \
	& propDescr.stateAccess, & propDescr.format, \
	& propDescr.num, & propDescr.accessType, \
	& propDescr.decTp, & propDescr.globalIdx, \
	& propDescr.ctxt, & propDescr.name, \
	& propDescr.description, & propDescr.descriptor, & propDescr.isValid, \
	& propDescr.accessFlags
*/

namespace jvx
{

	namespace propertyCall
	{
		class manager : public jvx::callManager
		{
		public:

			// When accessing properties, a special purpose can be specified to indiate the origin of the call
			jvxPropertyCallPurpose call_purpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC;

			// Access to properties can also be combined with a context
			jvxContext* context = nullptr;

			jvxCBitField accessFlags = JVX_ACCESS_ROLE_ALL_RCWD;
			
			jvxCBitField call_flags = (jvxCBitField)jvxPropertyCallFlags::JVX_PROPERTY_CALL_FLAG_NONE;

			struct
			{
				// Property access type to be returned if pointer is non-zero
				jvxPropertyAccessType* prop_access_type = nullptr;
				jvxBool* is_valid = nullptr;
			} on_get;

			struct
			{
				// Property access type to be returned if pointer is non-zero
				jvxBool modification_impact = true;
				
				jvxBool report_set = false;
			} on_set;

			struct
			{
				jvxPropertyCallPurpose call_purpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC;
				jvxContext* context = nullptr;
				jvxPropertyAccessType* prop_access_type = nullptr;
				jvxBool* is_valid = nullptr;;
				jvxBool modification_impact = true;
			} internal_stack;

			manager()
			{
				s_push();
			};

			void reset()
			{
				jvx::callManager::reset();
				call_purpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC;
				context = nullptr;
				on_get.prop_access_type = nullptr;
				on_get.is_valid = nullptr;
			};

			void s_push()
			{
				internal_stack.call_purpose = call_purpose;
				internal_stack.context = context;
				internal_stack.prop_access_type = on_get.prop_access_type;
				internal_stack.is_valid = on_get.is_valid;
				internal_stack.modification_impact = on_set.modification_impact;
			};
			void s_pop()
			{
				call_purpose = internal_stack.call_purpose;
				context = internal_stack.context;
				on_get.prop_access_type = internal_stack.prop_access_type;
				on_get.is_valid = internal_stack.is_valid;
				on_set.modification_impact = internal_stack.modification_impact;
			};
		};
	};
};

typedef jvx::propertyCall::manager jvxCallManagerProperties;

// class jvxCallManagerProperties : public jvxCallManager
//{
// public:

// Entries of predefined properties
#define JVX_PROPERTIES_OFFSET_SYSTEM 0
#define JVX_PROPERTIES_OFFSET_PRODUCT 64
#define JVX_PROPERTIES_OFFSET_UNSPECIFIC 128

#define JVX_PROPERTIES_OFFSET_SUBCOMPONENT 1024

static oneEntryProperties jvxProperties[] =
{
	/*	Identifier-string						,	ID as integer									,	dataformat						,	number elements */
	{	"jvx_command_string"					,	JVX_PROPERTIES_OFFSET_SYSTEM + 0				,	JVX_DATAFORMAT_STRING			,	1	, JVX_PROPERTY_DECODER_COMMAND				,"system/command_string"},
	// {	"jvx_start_on_prepare_cbool"			,	JVX_PROPERTIES_OFFSET_SYSTEM + 1				,	JVX_DATAFORMAT_SELECTION_LIST	,	1	, JVX_PROPERTY_DECODER_SINGLE_SELECTION		,"system/start_on_prepare"},
	{	"jvx_framesize_int32"					,	JVX_PROPERTIES_OFFSET_SYSTEM + 2				,	JVX_DATAFORMAT_32BIT_LE			,	1	, JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE	,"system/framesize"},
	{	"jvx_segmentsize_x_int32"				,	JVX_PROPERTIES_OFFSET_SYSTEM + 3				,	JVX_DATAFORMAT_32BIT_LE			,	1	, JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE	,"system/segmentsizex"},
	{	"jvx_segmentsize_y_int32"				,	JVX_PROPERTIES_OFFSET_SYSTEM + 4				,	JVX_DATAFORMAT_32BIT_LE			,	1	, JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE	,"system/segmentsizey"},
	{	"jvx_selection_framesize_sellist"		,	JVX_PROPERTIES_OFFSET_SYSTEM + 5				,	JVX_DATAFORMAT_SELECTION_LIST	,	1	, JVX_PROPERTY_DECODER_SINGLE_SELECTION		,"system/sel_framesize"},
	{	"jvx_rate_int32"						,	JVX_PROPERTIES_OFFSET_SYSTEM + 6				,	JVX_DATAFORMAT_32BIT_LE			,	1	, JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE	,"system/rate"},
	{	"jvx_selection_rate_sellist"			,	JVX_PROPERTIES_OFFSET_SYSTEM + 7				,	JVX_DATAFORMAT_SELECTION_LIST	,	1	, JVX_PROPERTY_DECODER_SINGLE_SELECTION		,"system/sel_rate"},
	{	"jvx_num_input_channels_int32"			,	JVX_PROPERTIES_OFFSET_SYSTEM + 8				,	JVX_DATAFORMAT_32BIT_LE			,	1	, JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE	,"system/num_input_channels"},
	{	"jvx_num_output_channels_int32"			,	JVX_PROPERTIES_OFFSET_SYSTEM + 9				,	JVX_DATAFORMAT_32BIT_LE			,	1	, JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE	,"system/num_output_channels"},
	{	"jvx_selection_input_channels_sellist"	,	JVX_PROPERTIES_OFFSET_SYSTEM + 10				,	JVX_DATAFORMAT_SELECTION_LIST	,	1	, JVX_PROPERTY_DECODER_MULTI_SELECTION		,"system/sel_input_channels"},
	{	"jvx_selection_output_channels_sellist"	,	JVX_PROPERTIES_OFFSET_SYSTEM + 11				,	JVX_DATAFORMAT_SELECTION_LIST	,	1	, JVX_PROPERTY_DECODER_MULTI_SELECTION		,"system/sel_output_channels"},
	{	"jvx_processing_format_int16"			,	JVX_PROPERTIES_OFFSET_SYSTEM + 12				,	JVX_DATAFORMAT_16BIT_LE			,	1	, JVX_PROPERTY_DECODER_FORMAT_IDX			,"system/dataformat"},
	{	"jvx_processing_subformat_int16"		,	JVX_PROPERTIES_OFFSET_SYSTEM + 13				,	JVX_DATAFORMAT_16BIT_LE			,	1	, JVX_PROPERTY_DECODER_SUBFORMAT_IDX		,"system/datasubformat"},
	{	"jvx_selection_formats_sellist"			,	JVX_PROPERTIES_OFFSET_SYSTEM + 14				,	JVX_DATAFORMAT_SELECTION_LIST	,	1	, JVX_PROPERTY_DECODER_SINGLE_SELECTION		,"system/sel_dataformat"},
	{	"jvx_selection_subformats_sellist"		,	JVX_PROPERTIES_OFFSET_SYSTEM + 15				,	JVX_DATAFORMAT_SELECTION_LIST	,	1	, JVX_PROPERTY_DECODER_SINGLE_SELECTION		,"system/sel_datasubformat"},	
	{	"jvx_skip_select_sellist"				,	JVX_PROPERTIES_OFFSET_SYSTEM + 16				,	JVX_DATAFORMAT_SELECTION_LIST	,	1	, JVX_PROPERTY_DECODER_SINGLE_SELECTION		,"system/sel_skip_select"},
	{	"jvx_component_path_str_list"			,	JVX_PROPERTIES_OFFSET_SYSTEM + 17				,	JVX_DATAFORMAT_STRING_LIST		,	1	, JVX_PROPERTY_DECODER_NONE					,"system/str_component_path_lst"},
	{	"jvx_config_path_str_list"				,	JVX_PROPERTIES_OFFSET_SYSTEM + 18				,	JVX_DATAFORMAT_STRING_LIST		,	1	, JVX_PROPERTY_DECODER_NONE					,"system/str_config_path_lst"},
	{	"jvx_system_path_str"					,	JVX_PROPERTIES_OFFSET_SYSTEM + 19				,	JVX_DATAFORMAT_STRING			,	1	, JVX_PROPERTY_DECODER_NONE					,"system/str_system_path"},
	{	"jvx_user_path_str"						,	JVX_PROPERTIES_OFFSET_SYSTEM + 20				,	JVX_DATAFORMAT_STRING			,	1	, JVX_PROPERTY_DECODER_NONE					,"system/str_user_path"},
	{	"jvx_load_dlls_sellist"					,	JVX_PROPERTIES_OFFSET_SYSTEM + 21				,	JVX_DATAFORMAT_SELECTION_LIST	,	1	, JVX_PROPERTY_DECODER_MULTI_SELECTION		,"system/sel_load_dlls"},
	{	"jvx_input_delay_int32"					,	JVX_PROPERTIES_OFFSET_SYSTEM + 22				,	JVX_DATAFORMAT_32BIT_LE			,	1	, JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE	,"system/input_delay"},
	{	"jvx_output_delay_int32"				,	JVX_PROPERTIES_OFFSET_SYSTEM + 23				,	JVX_DATAFORMAT_32BIT_LE			,	1	, JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE	,"system/output_delay"},
	{	"jvx_engage_sellist"					,	JVX_PROPERTIES_OFFSET_SYSTEM + 24				,	JVX_DATAFORMAT_SELECTION_LIST	,	1	, JVX_PROPERTY_DECODER_SINGLE_SELECTION		,"system/sel_engage"},
	{	"jvx_number_lost_buffer_int32"			,	JVX_PROPERTIES_OFFSET_SYSTEM + 25				,	JVX_DATAFORMAT_32BIT_LE			,	1	, JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE	,"system/number_lost_buffer"},
	{	"jvx_exactrate_int32"					,	JVX_PROPERTIES_OFFSET_SYSTEM + 26				,	JVX_DATAFORMAT_DATA				,	1	, JVX_PROPERTY_DECODER_NONE					,"system/exactrate" },
	{	"jvx_activate_fillup_silence_on_stop_bool"	,	JVX_PROPERTIES_OFFSET_SYSTEM + 27				,	JVX_DATAFORMAT_BOOL		,	1	, JVX_PROPERTY_DECODER_SIMPLE_ONOFF					,"system/act_fillup_silence_stop"},
	{	"jvx_period_fillup_silence_on_stop_data"	,	JVX_PROPERTIES_OFFSET_SYSTEM + 28				,	JVX_DATAFORMAT_DATA			,	1	, JVX_PROPERTY_DECODER_NONE					,"system/per_fillup_silence_stop"},
	{	"jvx_src_name_str"					,	JVX_PROPERTIES_OFFSET_SYSTEM + 29				,	JVX_DATAFORMAT_STRING			,	1	, JVX_PROPERTY_DECODER_NONE					,"system/src_name"},
	{	"jvx_snk_name_str"					,	JVX_PROPERTIES_OFFSET_SYSTEM + 30				,	JVX_DATAFORMAT_STRING			,	1	, JVX_PROPERTY_DECODER_NONE					,"system/snk_name"},

#ifndef JVX_NO_SYSTEM_EXTENSIONS	
#include "typedefs/TpjvxProperties_entries.h"
#endif
	{	""										,	-1				,	JVX_DATAFORMAT_NONE			,	-1}
};

#endif
