#ifndef __CJVXQTSAWIDGETWRAPPER_TYPES_H__
#define __CJVXQTSAWIDGETWRAPPER_TYPES_H__

	typedef enum
	{
		JVX_WW_PUSHBUTTON_DERIVE_TYPE = 0,
		JVX_WW_PUSHBUTTON_CBOOL = 1,
		JVX_WW_PUSHBUTTON_SELLIST = 2,
		JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY = 3,
		JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD = 4,
		JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE = 5
	} jvxWwConnectionType_pb;

	typedef enum
	{
		JVX_WW_COMBOBOX_ONLY_SELECTION = 0,
		JVX_WW_COMBOBOX_READ_ONCE = 1,
		JVX_WW_COMBOBOX_READ_ALWAYS = 2
	} jvxWwConnectionType_cb;

	typedef enum
	{
		JVX_WW_LINE_SEPARATE_LINEFEED = 0, /* \n */
		JVX_WW_LINE_SEPARATE_CARRRETURN = 1, /* \r */
		JVX_WW_LINE_SEPARATE_CARRRETURN_LINEFEED = 2 /* \r\n */
	} jvxWwLineSeparation_lw;

	typedef enum
	{
		JVX_WW_SHOW_DIRECT = 0,
		JVX_WW_SHOW_DB10 = 1,
		JVX_WW_SHOW_DB20 = 2,
		JVX_WW_SHOW_SCALED = 3
	} jvxWwTransformValueType;

	typedef enum
	{
		JVX_WW_LABEL_TEXT = 0,
		JVX_WW_LABEL_HIDE_ON_TRUE = 1,
		JVX_WW_LABEL_GREEN_GRAY = 2,
		JVX_WW_LABEL_ALERT = 3,
		JVX_WW_LABEL_CUSTOM_COLOR = 4
	} jvxWwConnectionType_la;

	typedef enum
	{
		JVX_WW_REPORT_ELEMENT = 0,
		JVX_WW_REPORT_LOCAL = 1,
		JVX_WW_REPORT_GLOBAL = 2,
		JVX_WW_REPORT_LOCAL_GLOBAL = 3
	} jvxWwReportType;

	typedef enum
	{
		JVX_WW_WIDGET_SEARCH_ALL_CHILDREN,
		JVX_WW_WIDGET_SEARCH_FROM_ROOT,
		JVX_WW_WIDGET_SEARCH_CURRENT_AND_CHILDREN,
		JVX_WW_WIDGET_SEARCH_CURRENT_ONLY
	} jvxWwSearchMode;

	typedef enum
	{
		JVX_WW_FRAME_ELEMENTS_LABELS = 0,
		JVX_WW_FRAME_ELEMENTS_BUTTONS = 1,
		JVX_WW_FRAME_ELEMENTS_CHECKBOXES = 2
	} jvxWwUiElementType_fr;

	struct basePropInfos
	{
		jvxBool updateRt = false;
		jvxBool tooltipDebug = false;
		jvxBool verbose_out = false;
		jvxSize dbgLevel = 1;
		jvxBool supersede_enable = false;
		jvxBool report_get = false;
		jvxWwReportType reportTp = JVX_WW_REPORT_ELEMENT;
		jvxSize group_id_trigger = 0;
		jvxSize group_id_emit = 0;
	};

	typedef enum
	{
		JVX_WW_MATCH_GROUPID_VALUE,
		JVX_WW_MATCH_GROUPID_BITFIELD
	} jvxWwMatchIdType;

	typedef enum
	{
		JVX_WW_CONFIG_MATCH_TYPE /* plus jvxMatchIdType* */,
		JVX_WW_CONFIG_LINEEDIT_REACT_MODE_RETURN_ONLY,
		JVX_WW_CONFIG_DISTRIBUTE_ALL, /* plus jvxBool* */
		JVX_WW_CONFIG_TYPE_LIMIT
	} jvxWwConfigType;

#endif
