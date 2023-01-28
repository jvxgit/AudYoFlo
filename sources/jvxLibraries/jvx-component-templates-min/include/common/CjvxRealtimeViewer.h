#ifndef __CJVXREALTIMEVIEWER_H__
#define __CJVXREALTIMEVIEWER_H__

#include "jvx.h"

#define JVX_RTV_MAIN_SECTION_NAME "rtv_section_main_name"
#define JVX_RTV_SUBSECTION_NAME "rtv_sub_section_name"
#define JVX_RTV_SUBSECTION_VIEW_TYPE "rtv_sub_section_viewtype"
#define JVX_RTV_ONE_SUBSECTION_PREFIX "rtv_sub_section_"
#define JVX_RTV_ONE_SUBGROUP_PREFIX "rtv_sub_group_"
#define JVX_RTV_GROUP_TYPE "rtv_sub_section_grouptype"
#define JVX_RTV_GROUP_DATA_DECODE_ID "rtv_sub_section_group_data_decode_id"
#define JVX_RTV_GROUP_DESCRIPTION "rtv_sub_section_group_description"
#define JVX_RTV_PROP_DESCRIPTIONS "rtv_item_prop_description"
#define JVX_RTV_PROP_DESCRIPTORS "rtv_item_prop_descriptors"
#define JVX_RTV_ITEM_DESCRIPTIONS "rtv_item_item_description"
#define JVX_RTV_CATEGORIES "rtv_item_categories"
#define JVX_RTV_COMPONENT_SLOTID "rtv_item_comp_slotid"
#define JVX_RTV_COMPONENT_SLOTSUBID "rtv_item_comp_slotsubid"
#define JVX_RTV_UPDATES "rtv_item_updates"
#define JVX_RTV_DECODE_IDS "rtv_item_decode_ids"
#define JVX_RTV_COMPONENT_TYPES "rtv_item_component_types"

#define ALLOW_TRIGGER_IN_STATE_ACTIVE

typedef enum
{
	JVX_REALTIME_VIEWER_VIEW_NONE = 0,
	JVX_REALTIME_VIEWER_VIEW_PROPERTIES = 1,
	JVX_REALTIME_VIEWER_VIEW_PLOTS = 2
} jvxRealtimeViewerType;

typedef enum
{
	JVX_REALTIME_VIEWER_GROUP_NONE = 0,
	JVX_REALTIME_VIEWER_GROUP_PROPERTY = 1, 
	JVX_REALTIME_VIEWER_GROUP_LINEAR_PLOT = 2,
	JVX_REALTIME_VIEWER_GROUP_CIRC_PLOT = 3,
	JVX_REALTIME_VIEWER_GROUP_POLAR_PLOT = 4
} jvxRealtimeViewerGroupType;

typedef enum
{
	JVX_REALTIME_PRIVATE_MEMORY_DECODER_NONE = 0,
	JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_GROUP = 1,
	JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_ITEM = 2,
	JVX_REALTIME_PRIVATE_MEMORY_LINEAR_SPLOT_SINGLE_DIAG_ITEM = 3,
	JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM = 4,
	JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_SECTION = 5
} jvxRealtimePrivateMemoryDecoderEnum;


typedef jvxErrorType (*getConfigurationPrivateData)(IjvxConfigProcessor* theReader, jvxConfigData* writeEntriesTo, jvxHandle* privateDataItem, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum typeHint, jvxInt16 itemId);
typedef jvxErrorType (*putConfigurationPrivateData)(IjvxConfigProcessor* theReader, jvxConfigData* readEntriesFrom, jvxHandle* privateDataItem, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum typeHint, jvxInt16 itemId);

typedef jvxErrorType (*copyHandler)(jvxHandle* copyTo, jvxHandle* copyFrom, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decId);
typedef jvxErrorType (*allocateHandler)(jvxHandle** privateData, jvxSize* sz, jvxRealtimePrivateMemoryDecoderEnum decodeHint);
typedef jvxErrorType (*deallocateHandler)(jvxHandle* privateData, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decodeHint);

// ============================================================================

typedef jvxErrorType (*requestUpdateOnePropertyPlotItem)(jvxSize sectionId, jvxSize groupId, jvxSize itemId, jvxHandle* privateData);
typedef struct
{
	requestUpdateOnePropertyPlotItem callback_item;
	jvxHandle* privData;
} callbackStruct_section;

// ================================================================================

typedef jvxErrorType (*triggerUpdateOnePropertyPlotGroup)(jvxSize sectionId, jvxSize groupId, jvxBool callOnStart, jvxHandle* privateData);

typedef struct
{
	callbackStruct_section callback_section;
	triggerUpdateOnePropertyPlotGroup callback_group;
} callbackStruct_group;

// ================================================================================
class CjvxRealtimeViewer
{
protected:

	typedef struct
	{
		jvxComponentIdentification tp;
		jvxSize inPropId;
		bool update;
		std::string description;
		struct
		{
			jvxHandle* data;
			jvxSize sz;
			jvxRealtimePrivateMemoryDecoderEnum decodeId;
		} prop;
	} oneEntryViewerItem;

	typedef struct
	{
		std::vector<oneEntryViewerItem> theItemList;
		std::string description;
		jvxRealtimeViewerGroupType group_tp;
		callbackStruct_group* callback_struct;
		bool update;
		struct
		{
			jvxHandle* data;
			jvxSize sz;
			jvxRealtimePrivateMemoryDecoderEnum decodeId;
		} prop;
	} oneEntryViewerGroup;

	typedef struct
	{
		std::vector<oneEntryViewerGroup> theViewList;
		jvxRealtimeViewerType whatToView;
		std::string description;
		callbackStruct_section* callback_struct;
		jvxBool visible;
		struct
		{
			jvxHandle* data;
			jvxSize sz;
			jvxRealtimePrivateMemoryDecoderEnum decodeId;
		} prop;
	} oneEntryViewerSection;


	struct
	{
		std::vector<oneEntryViewerSection> lstViews;
		jvxState theState;
		allocateHandler theAllocater;
		deallocateHandler theDeallocater;
		copyHandler theCopier;
	} _content;

public:

	CjvxRealtimeViewer(allocateHandler theAllocater, deallocateHandler theDeallocater, copyHandler theCopier);

	~CjvxRealtimeViewer();

	jvxErrorType _insert_view_section_after(jvxRealtimeViewerType tp, const char* description, jvxSize position,
		jvxHandle* theProp,	jvxSize szProp, jvxRealtimePrivateMemoryDecoderEnum decodePropId);

	jvxErrorType _description_section(jvxSize sectionId, jvxRealtimeViewerType& tp, std::string& description);

	jvxErrorType _set_description_section(jvxSize sectionId, const char*  description);

	jvxErrorType _set_update_callback_view_section(jvxSize sectionId, callbackStruct_section* callback_str);

	jvxErrorType _set_update_callback_view_group_in_section(jvxSize sectionId, jvxSize groupId, callbackStruct_group* callback_str);

	jvxErrorType _run_viewer();

	jvxErrorType _trigger_viewer();

	jvxErrorType _stop_viewer();

	jvxErrorType _number_view_sections(jvxSize* num);

	jvxErrorType _remove_view_section(jvxSize position);

	jvxErrorType _clear();

	jvxErrorType _insert_group_in_section(jvxSize sectionId, jvxSize position, const char* descr, jvxRealtimeViewerGroupType tp, jvxHandle* theProp,
		jvxSize szProp, jvxRealtimePrivateMemoryDecoderEnum decodePropId);

	jvxErrorType _remove_group_in_section(jvxSize sectionId, jvxSize position);

	jvxErrorType _number_groups_in_section(jvxSize sectionId, jvxSize* num);

	jvxErrorType _insert_item_in_group_in_section(jvxSize sectionId, jvxSize groupId, jvxComponentIdentification tp, jvxSize inPropId, jvxSize position,
		std::string& descr, jvxHandle* theProp, jvxSize szProp, jvxRealtimePrivateMemoryDecoderEnum decodePropId);

	jvxErrorType _description_item_in_group_in_section(jvxSize sectionId, jvxSize groupId, jvxSize itemId, jvxSize* inPropId, jvxComponentIdentification* tp, std::string& nm);

	jvxErrorType _description_group_in_section(jvxSize sectionId, jvxSize groupId, jvxRealtimeViewerGroupType& tp, std::string& description);

	jvxErrorType _set_description_group_in_section(jvxSize sectionId, jvxSize groupId, const char* description);

	jvxErrorType _set_update_item_in_group_in_section(jvxSize sectionId, jvxSize groupId, jvxSize itemId, jvxBool update);

	jvxErrorType _update_item_in_group_in_section(jvxSize sectionId, jvxSize groupId, jvxSize itemId, jvxBool* update);

	jvxErrorType _set_update_group_in_section(jvxSize sectionId, jvxSize groupId, jvxBool update);

	jvxErrorType _update_group_in_section(jvxSize sectionId, jvxSize groupId, jvxBool* update);

	jvxErrorType _remove_item_in_group_in_section(jvxSize sectionId, jvxSize groupId, jvxSize position);

	jvxErrorType _request_property_item_in_group_in_section(jvxSize sectionId, jvxSize groupId, jvxSize position, jvxHandle** prop, jvxSize* szFld, jvxInt16* decodeId);

	jvxErrorType _request_property_section(jvxSize sectionId, jvxHandle** prop, jvxSize* szFld, jvxInt16* decodeId);

	jvxErrorType _request_property_group_in_section(jvxSize sectionId, jvxSize groupId, jvxHandle** prop, jvxSize* szFld, jvxInt16* decodeId);

	jvxErrorType _return_property_item_in_group_in_section(jvxSize sectionId, jvxSize groupId, jvxSize position, jvxHandle* prop);

	jvxErrorType _return_property_group_in_section(jvxSize sectionId, jvxSize groupId, jvxHandle* prop);

	jvxErrorType _return_property_section(jvxSize sectionId, jvxSize groupId, jvxHandle* prop);

	jvxErrorType _number_items_in_group_in_section(jvxSize sectionId, jvxSize groupId, jvxSize* num);

	jvxErrorType _set_section_visible(jvxSize sectionId, jvxBool isVisible);

	jvxErrorType _get_section_visible(jvxSize sectionId, jvxBool* isVisible);

	jvxErrorType _get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, 
		IjvxHost* theHost, getConfigurationPrivateData theCallback, std::string postfixMainSection);

	jvxErrorType _put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, 
		IjvxHost* theHost, const char* fName, int lineno, std::vector<std::string>& warnings, putConfigurationPrivateData putConfig, std::string postfixMainSection);
	
};

#endif
