#define JVX_NUM_CONFIGURATIONLINE_PROPERTIES_AUDIO 5
static std::string jvxProperties_configLine_audio[JVX_NUM_CONFIGURATIONLINE_PROPERTIES_AUDIO] = 
{
	"jvx_framesize_int32",
	"jvx_rate_int32",
	"jvx_num_input_channels_int32",
	"jvx_num_output_channels_int32",
	"jvx_processing_format_int16"
};

#define JVX_NUM_CONFIGURATIONLINE_PROPERTIES_VIDEO 6
static std::string jvxProperties_configLine_video[JVX_NUM_CONFIGURATIONLINE_PROPERTIES_VIDEO] =
{
	"jvx_framesize_int32",
	"jvx_segmentsize_x_int32",
	"jvx_segmentsize_y_int32",
	"jvx_rate_int32",
	"jvx_processing_format_int16",
	"jvx_processing_subformat_int16"
};
#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "typedefs/TpjvxConfiglines.h"
#endif
