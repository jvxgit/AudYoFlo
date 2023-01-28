#ifndef __REALTIMEVIEWER_TYPEDEFS_H__
#define __REALTIMEVIEWER_TYPEDEFS_H__

#include "jvx.h"

typedef enum
{
	JVX_REALTIME_PLOT_COLOR_BLACK, 
	JVX_REALTIME_PLOT_COLOR_BROWN, 
	JVX_REALTIME_PLOT_COLOR_BLUE, 
	JVX_REALTIME_PLOT_COLOR_PURPLE, 
	JVX_REALTIME_PLOT_COLOR_RED, 
	JVX_REALTIME_PLOT_COLOR_ORANGE, 
	JVX_REALTIME_PLOT_COLOR_YELLOW, 
	JVX_REALTIME_PLOT_COLOR_GREEN
} jvxRealtimePlotColorType;

#define JVX_REALTIME_PLOT_COLOR_NUM 8
static std::string jvxRealtimePlotColorType_txt[JVX_REALTIME_PLOT_COLOR_NUM] = 
{
	"black", 
	"brown", 
	"blue", 
	"purple", 
	"red", 
	"orange", 
	"yellow", 
	"green"
};

typedef enum
{
	JVX_REALTIME_PLOT_1PIXEL, 
	JVX_REALTIME_PLOT_2PIXEL, 
	JVX_REALTIME_PLOT_3PIXEL
} jvxRealtimePlotWidthType;

#define JVX_REALTIME_PLOT_WIDTH_NUM 3
static std::string jvxRealtimePlotWidthType_txt[JVX_REALTIME_PLOT_WIDTH_NUM] = 
{
	"1 pixel", 
	"2 pixel", 
	"3 pixel"
};

static int jvxRealtimePlotWidthType_translate[JVX_REALTIME_PLOT_WIDTH_NUM] = 
{
	1,
	2,
	3
};

typedef enum
{
	JVX_REALTIME_PLOT_LINE_SOLID, 
	JVX_REALTIME_PLOT_LINE_DOTTED, 
	JVX_REALTIME_PLOT_LINE_DASHED, 
	JVX_REALTIME_PLOT_LINE_DASH_DOTTED 
} jvxRealtimePlotLinestyleType;

#define JVX_REALTIME_PLOT_LINE_NUM 4
static std::string jvxRealtimePlotLinestyleType_txt[JVX_REALTIME_PLOT_LINE_NUM] = 
{
	"solid", 
	"dotted", 
	"slashed", 
	"slash-dotted"
};

#define JVX_RTV_LINPLOT_COLOR_ID "rtv_item_color_id"
#define JVX_RTV_LINPLOT_WIDTH_ID "rtv_item_width_id"
#define JVX_RTV_LINPLOT_STYLE_ID "rtv_item_style_id"
#define JVX_RTV_LINPLOT_XMAX "rtv_item_xmax"
#define JVX_RTV_LINPLOT_XMIN "rtv_item_xmin"
#define JVX_RTV_LINPLOT_YMAX "rtv_item_ymax"
#define JVX_RTV_LINPLOT_YMIN "rtv_item_ymin"
#define JVX_RTV_LINPLOT_SHOW "rtv_item_show"
#define JVX_RTV_LINPLOT_AUTOX "rtv_item_autox"
#define JVX_RTV_LINPLOT_AUTOY "rtv_item_autoy"
#define JVX_RTV_LINPLOT_XAXIS "rtv_item_xaxis"
#define JVX_RTV_LINPLOT_YAXIS "rtv_item_yaxis"
#define JVX_RTV_ONE_LINEAR_PLOT_GROUP "rtv_linear_plot_group_data_decoder"
#define JVX_RTV_ONE_LINEAR_PLOT_ITEM_PREFIX "rtv_linear_plot_item_prop_data_"
#define JVX_RTV_ONE_LINEAR_CIRC_PLOT_ITEM_PREFIX "rtv_linear_circ_splot_item_prop_data_"

#define JVX_RTV_CIRC_SPLOT_LINE_WIDTH "rtv_item_circ_splot_width"
#define JVX_RTV_CIRC_SPLOT_XCHANNEL_SELECTION "rtv_item_circ_splot_xchannel_select"
#define JVX_RTV_CIRC_SPLOT_BUFFER_LENGTH "rtv_item_circ_splot_buffer_length"
#define JVX_RTV_CIRC_SPLOT_NUM_CHANNELS "rtv_item_circ_splot_num_channels"
#define JVX_RTV_LINPLOT_CIRCPLOT_SELECTION_Y "rtv_item_circ_splot_ychannel_selection"

#define JVX_RTV_ONE_PROP_SECTION "rtv_property_section_decoder"
#define JVX_RTV_PROPERTY_SECTION_SCROLL_X "rtv_property_section_scrollx"
#define JVX_RTV_PROPERTY_SECTION_SCROLL_Y "rtv_property_section_scrolly"

#endif

