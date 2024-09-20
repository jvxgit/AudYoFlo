#ifndef __JVX_DATAFORMATS_H__
#define __JVX_DATAFORMATS_H__

#include <string.h>
#include <assert.h>
#include <stdio.h>

#define JVX_DATA_EPS_DOUBLE DBL_EPSILON
#define JVX_DATA_EPS_FLOAT FLT_EPSILON

#define JVX_FLOAT_DATAFORMAT_DOUBLE_SHIFT 0
#define JVX_FLOAT_DATAFORMAT_SINGLE_SHIFT 1

#ifdef JVX_DATA_FORMAT_DOUBLE


typedef double jvxData;
#define JVX_DBL_2_DATA(a) (a)
#define JVX_DATA_MAX_POS DBL_MAX
#define JVX_DATA_MAX_NEG -DBL_MAX
#define JVX_DATA_EPS DBL_EPSILON
#define JVX_DATA_MIN DBL_MIN
#define JVX_DATA_MIN_UNDERFLOW_PREVENT DBL_EPSILON
#define JVX_DATA_DESCRIPTOR "double"
#define JVX_DATA_DESCRIPTOR_THE_OTHER "single"
#define toData toDouble
#define JVX_DATA_MEX_CONVERT mxDOUBLE_CLASS
#define mxIsData mxIsDouble
#define mxDATA_CLASS mxDOUBLE_CLASS
#define sf_writef_data sf_writef_double
#define sf_readf_data sf_readf_double
#define JVX_FLOAT_DATAFORMAT_ID 1
#define JVX_FLOAT_DATAFORMAT_ID_OTHER 0
#define JVX_MAX_NUM_DIGITS_INT_IN_FLOAT 53
#define JVX_FABS fabs
#define JVX_LOG10 log10
#define JVX_C_STR2DATA strtod
#define JVX_FLOAT_DATAFORMAT_SHIFT JVX_FLOAT_DATAFORMAT_DOUBLE_SHIFT
#define JVX_DATA_DATAFORMAT_ENUM JVX_DATA_TYPE_SPEC_DOUBLE

#else // JVX_DATA_FORMAT_DOUBLE

#ifdef JVX_DATA_FORMAT_FLOAT

typedef float jvxData;

#define JVX_DBL_2_DATA(a) (float)(a)
#define JVX_DATA_MAX_POS FLT_MAX
#define JVX_DATA_MAX_NEG -FLT_MAX
#define JVX_DATA_EPS FLT_EPSILON
#define JVX_DATA_MIN FLT_MIN
#define JVX_DATA_MIN_UNDERFLOW_PREVENT FLT_EPSILON
#define JVX_DATA_DESCRIPTOR "single"
#define JVX_DATA_DESCRIPTOR_THE_OTHER "double"
#define toData toFloat
#define JVX_DATA_MEX_CONVERT mxSINGLE_CLASS
#define mxIsData mxIsSingle
#define mxDATA_CLASS mxSINGLE_CLASS
#define sf_writef_data sf_writef_float
#define sf_readf_data sf_readf_float
#define JVX_FLOAT_DATAFORMAT_ID 0
#define JVX_FLOAT_DATAFORMAT_ID_OTHER 1
#define JVX_MAX_NUM_DIGITS_INT_IN_FLOAT 23
#define JVX_FABS fabsf
#define JVX_LOG10 log10f
#define JVX_C_STR2DATA strtof
#define JVX_FLOAT_DATAFORMAT_SHIFT JVX_FLOAT_DATAFORMAT_SINGLE_SHIFT
#define JVX_DATA_DATAFORMAT_ENUM JVX_DATA_TYPE_SPEC_FLOAT

#else // JVX_DATA_FORMAT_FLOAT

#error("unkown arithmetic datatype")

#endif // JVX_DSP_DATA_FORMAT_FLOAT
#endif // JVX_DSP_DATA_FORMAT_DOUBLE

#if JVX_SYS_EMSCRIPTEN_CLANG_32BIT 
#define JVX_SIZE_OVERFLOW 0x7FFFFFFF
#else
#define JVX_SIZE_OVERFLOW 0x7FFFFFFFFFFFFFFF
#endif

typedef jvxUInt64 jvxCBitField64;
typedef jvxUInt32 jvxCBitField32;
typedef jvxUInt16 jvxCBitField16;
typedef jvxSize jvxUniqueId;
#define JVX_UNIQUE_ID_UNSPECIFIED JVX_SIZE_UNSELECTED

typedef enum
{
        JVX_DATAFORMAT_NONE = 0,                                        // Undefined (invalid)
        JVX_DATAFORMAT_DATA,                                        // double or float datatype
        JVX_DATAFORMAT_16BIT_LE,                                    // 16 Bit signed
        JVX_DATAFORMAT_32BIT_LE,                                    // 32 Bit signed
        JVX_DATAFORMAT_64BIT_LE,                                    // 64 Bit signed
        JVX_DATAFORMAT_8BIT,                                                // 8 Bit
		JVX_DATAFORMAT_SIZE,

        JVX_DATAFORMAT_STRING,                                              // String type (character buffers)
        JVX_DATAFORMAT_STRING_LIST,                                         // String type (character buffers)
        JVX_DATAFORMAT_SELECTION_LIST,
        JVX_DATAFORMAT_VALUE_IN_RANGE,
		
		JVX_DATAFORMAT_U16BIT_LE,                                   // 16 Bit unsigned
		JVX_DATAFORMAT_U32BIT_LE,                                   // 32 Bit unsigned
		JVX_DATAFORMAT_U64BIT_LE,                                   // 64 Bit unsigned
		JVX_DATAFORMAT_U8BIT,										// 8 Bit unsigned
		
		JVX_DATAFORMAT_FLOAT,										// explicit float for external access
		JVX_DATAFORMAT_DOUBLE,										// explicit double for external access
		JVX_DATAFORMAT_BYTE, // Void: Gateway for other datatypes

		/*
		* The external buffer datatype has a valid value type to identify
		* if the associated dataformat is correct.
		* The other "installable" formats are of a dedicated type
		JVX_DATAFORMAT_EXTERNAL_BUFFER,
		*/
		JVX_DATAFORMAT_HANDLE, // Void: Gateway for other datatypes
		JVX_DATAFORMAT_POINTER, // Void: Gateway for other datatypes
		JVX_DATAFORMAT_CALLBACK, // Pointer for callbacks
		JVX_DATAFORMAT_INTERFACE,
		JVX_DATAFORMAT_LIMIT
} jvxDataFormat; 

static jvxTextHelpers jvxDataFormat_str[JVX_DATAFORMAT_LIMIT + 1] =
{
	{"none", "JVX_DATAFORMAT_NONE"},
	{"jvxData(" JVX_DATA_DESCRIPTOR ")", "JVX_DATAFORMAT_DATA" },
	{"16 Bit le", "JVX_DATAFORMAT_16BIT_LE" },

	{"32 Bit le", "JVX_DATAFORMAT_32BIT_LE" },
	{"64 Bit le", "JVX_DATAFORMAT_64BIT_LE" },
	{"8 Bit", "JVX_DATAFORMAT_8BIT" },
	{"size", "JVX_DATAFORMAT_SIZE" },
	
	{"string", "JVX_DATAFORMAT_STRING" },
	{"string list", "JVX_DATAFORMAT_STRING_LIST" },

	{"selection list", "JVX_DATAFORMAT_SELECTION_LIST" },
	{"value in range","JVX_DATAFORMAT_VALUE_IN_RANGE" },
	
	{"u16 Bit le", "JVX_DATAFORMAT_U16BIT_LE" },
	{"u32 Bit le", "JVX_DATAFORMAT_U32BIT_LE" },
	{"u64 Bit le", "JVX_DATAFORMAT_U64BIT_LE" },
	{"u8 Bit", "JVX_DATAFORMAT_U8BIT" },
	
	{"float", "JVX_DATAFORMAT_FLOAT" },
	{"double", "JVX_DATAFORMAT_DOUBLE" },
	{"byte", "JVX_DATAFORMAT_BYTE"},


	/*
	{"extbuf", "JVX_DATAFORMAT_EXTERNAL_BUFFER"},
	*/
	{"handle", "JVX_DATAFORMAT_HANDLE" },
	{"pointer", "JVX_DATAFORMAT_POINTER"},
	{"callback", "JVX_DATAFORMAT_CALLBACK" },
	{"interface", "JVX_DATAFORMAT_INTERFACE"},

	{NULL, NULL},
};

static inline const char* jvxDataFormat_txt(jvxSize id)
{
	assert(id < JVX_DATAFORMAT_LIMIT);
	return jvxDataFormat_str[id].friendly;
}

static inline jvxDataFormat jvxDataFormat_decode(const char * token)
{
	jvxSize i;
	jvxDataFormat res = JVX_DATAFORMAT_NONE;
	for (i = 0; i < JVX_DATAFORMAT_LIMIT; i++)
	{
		if (!strcmp(jvxDataFormat_str[i].friendly, token))
		{
			res = (jvxDataFormat) i;
			break;
		}
		  if (!strcmp(jvxDataFormat_str[i].full, token))
		  {
			  res = (jvxDataFormat)i;
			  break;
		  }
	}
	return(res);
}

//! Typedef enum for sampleformat
static jvxSize jvxDataFormat_size[JVX_DATAFORMAT_LIMIT + 1] =
{
	0,					// JVX_DATAFORMAT_NONE
	sizeof(jvxData),    // JVX_DATAFORMAT_DATA
	sizeof(jvxInt16),	// JVX_DATAFORMAT_16BIT_LE,                                    // 16 Bit signed	
	sizeof(jvxInt32),	// JVX_DATAFORMAT_32BIT_LE
	sizeof(jvxInt64),	// JVX_DATAFORMAT_64BIT_LE
	sizeof(jvxInt8),	// JVX_DATAFORMAT_8BIT
	sizeof(jvxSize),	// JVX_DATAFORMAT_SIZE

	0,					// JVX_DATAFORMAT_STRING
	0,					// JVX_DATAFORMAT_STRING_LIST
	0,					// JVX_DATAFORMAT_SELECTION_LIST
	0,					// JVX_DATAFORMAT_VALUE_IN_RANGE

	sizeof(jvxUInt16),	//JVX_DATAFORMAT_U16BIT_LE,                                   // 16 Bit unsigned
	sizeof(jvxUInt32),	//JVX_DATAFORMAT_U32BIT_LE,                                   // 32 Bit unsigned
	sizeof(jvxUInt64),	//JVX_DATAFORMAT_U64BIT_LE,                                   // 64 Bit unsigned
	sizeof(jvxUInt8),	//JVX_DATAFORMAT_U8BIT,

	sizeof(float),		// JVX_DATAFORMAT_FLOAT
	sizeof(double),		//JVX_DATAFORMAT_DOUBLE
	sizeof(jvxByte),	// JVX_DATAFORMAT_BYTE

	0,					// JVX_DATAFORMAT_HANDLE
	sizeof(jvxHandle*),	// JVX_DATAFORMAT_POINTER
	0,					// JVX_DATAFORMAT_CALLBACK
	0,					// JVX_DATAFORMAT_INTERFACE
	JVX_DATAFORMAT_LIMIT
} ;

static inline jvxSize jvxDataFormat_getsize(jvxSize idx)
{
	assert(idx < JVX_DATAFORMAT_LIMIT);
	return(jvxDataFormat_size[idx]);
}

// ===================================================================
#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "typedefs/TpjvxTypes_dataGroups.h"
#endif

typedef enum
{
        JVX_DATAFORMAT_GROUP_NONE = 0, 

		JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED,
		JVX_DATAFORMAT_GROUP_AUDIO_PCM_INTERLEAVED,

		JVX_DATAFORMAT_GROUP_FFT_R2C,
		JVX_DATAFORMAT_GROUP_FFT_C2C,

		JVX_DATAFORMAT_GROUP_VIDEO_RGBA32,		
		JVX_DATAFORMAT_GROUP_VIDEO_RGB24,
		JVX_DATAFORMAT_GROUP_VIDEO_NV12,
		JVX_DATAFORMAT_GROUP_VIDEO_YUYV, // also YUY2
		JVX_DATAFORMAT_GROUP_VIDEO_SINGLE8,
		JVX_DATAFORMAT_GROUP_VIDEO_MONO,

		JVX_DATAFORMAT_GROUP_GENERIC_INTERLEAVED,
		JVX_DATAFORMAT_GROUP_GENERIC_NON_INTERLEAVED,

		JVX_DATAFORMAT_GROUP_GENERIC,
		JVX_DATAFORMAT_GROUP_TRIGGER_ONLY,

		JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD,
		JVX_DATAFORMAT_GROUP_FFMPEG_FRAME_FWD

#ifndef JVX_NO_SYSTEM_EXTENSIONS
		JVX_DATAFORMAT_GROUP_DEFINES_PRODUCT
#endif
		,JVX_DATAFORMAT_GROUP_LIMIT
} jvxDataFormatGroup; 

static jvxTextHelpers jvxDataFormatGroup_str[JVX_DATAFORMAT_GROUP_LIMIT + 1] =
{
	{"none", "JVX_DATAFORMAT_NONE"},

	{"audio pcm di", "JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED" },
	{"audio pcm i", "JVX_DATAFORMAT_GROUP_AUDIO_PCM_INTERLEAVED" },

	{"fft real2complex", "JVX_DATAFORMAT_GROUP_FFT_R2C"},
	{"fft complex2complex", "JVX_DATAFORMAT_GROUP_FFT_C2C"},

	{"rgba32", "JVX_DATAFORMAT_GROUP_VIDEO_RGBA32" },
	{"rgb24", "JVX_DATAFORMAT_GROUP_VIDEO_RGB24" },
	{"nv12", "JVX_DATAFORMAT_GROUP_VIDEO_NV12" },
	{"yuyv", "JVX_DATAFORMAT_GROUP_VIDEO_YUYV" },
	{ "single8", "JVX_DATAFORMAT_GROUP_VIDEO_SINGLE8" },
	{ "mono", "JVX_DATAFORMAT_GROUP_VIDEO_MONO" },
	
	{ "generic il", "JVX_DATAFORMAT_GROUP_GENERIC_INTERLEAVED" },
	{ "generic non-il", "JVX_DATAFORMAT_GROUP_GENERIC_NON_INTERLEAVED" },
	
	{ "generic", "JVX_DATAFORMAT_GROUP_GENERIC" },
	{ "trigger", "JVX_DATAFORMAT_GROUP_TRIGGER_ONLY"},

	{ "ffmpg-pkt", "JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD"},
	{ "ffmpg-frm", "JVX_DATAFORMAT_GROUP_FFMPEG_FRAME_FWD"}

#ifndef JVX_NO_SYSTEM_EXTENSIONS
	JVX_DATAFORMAT_GROUP_STRINGS_PRODUCT
#endif

	, {NULL, NULL}
};

static inline const char* jvxDataFormatGroup_txt(jvxSize id)
{
	assert(id < JVX_DATAFORMAT_GROUP_LIMIT);
	return jvxDataFormatGroup_str[id].friendly;
}

static inline jvxDataFormatGroup jvxDataFormatGroup_decode(const char * token)
{
	jvxSize i;
	jvxDataFormatGroup res = JVX_DATAFORMAT_GROUP_NONE;
	for (i = 0; i < JVX_DATAFORMAT_GROUP_LIMIT; i++)
	{
		if (!strcmp(jvxDataFormatGroup_str[i].friendly, token))
		{
			res = (jvxDataFormatGroup) i;
			break;
		}
		  if (!strcmp(jvxDataFormatGroup_str[i].full, token))
		  {
			  res = (jvxDataFormatGroup)i;
			  break;
		  }
	}
	return(res);
}

//! Typedef enum for sampleformat
static jvxSize jvxDataFormatGroup_size[JVX_DATAFORMAT_GROUP_LIMIT + 1] =
{
	0, // JVX_DATAFORMAT_GROUP_NONE

	1, // JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED
	1, // JVX_DATAFORMAT_GROUP_AUDIO_PCM_INTERLEAVED

	0, // JVX_DATAFORMAT_GROUP_FFT_R2C
	0, // JVX_DATAFORMAT_GROUP_FFT_C2C

	(jvxSize)4, // JVX_DATAFORMAT_GROUP_VIDEO_RGBA32	
	(jvxSize)3, // JVX_DATAFORMAT_GROUP_VIDEO_RGB	
	(jvxSize)3, // JVX_DATAFORMAT_GROUP_VIDEO_NV12	
	(jvxSize)4, // JVX_DATAFORMAT_GROUP_VIDEO_YUYV
	0, // JVX_DATAFORMAT_GROUP_VIDEO_SINGLE8
	0, // JVX_DATAFORMAT_GROUP_VIDEO_MONO

	0, // JVX_DATAFORMAT_GROUP_GENERIC_INTERLEAVED
	0, // JVX_DATAFORMAT_GROUP_GENERIC_NON_INTERLEAVED

	1, // JVX_DATAFORMAT_GROUP_GENERIC
	0, // JVX_DATAFORMAT_GROUP_TRIGGER_ONLY,

	1, //JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD,
	1 // JVX_DATAFORMAT_GROUP_FFMPEG_FRAME_FWD


#ifndef JVX_NO_SYSTEM_EXTENSIONS
	JVX_DATAFORMAT_GROUP_SIZES_PRODUCT
#endif
	, JVX_DATAFORMAT_GROUP_LIMIT // <- this one for a generic test of number of entries
};

static jvxSize jvxDataFormatGroup_size_div[JVX_DATAFORMAT_GROUP_LIMIT + 1] =
{
	1, // JVX_DATAFORMAT_GROUP_NONE

	1, // JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED
	1, // JVX_DATAFORMAT_GROUP_AUDIO_PCM_INTERLEAVED

	1, // JVX_DATAFORMAT_GROUP_FFT_R2C
	1, // JVX_DATAFORMAT_GROUP_FFT_C2C

	(jvxSize)1, // JVX_DATAFORMAT_GROUP_VIDEO_RGBA32	
	(jvxSize)1, // JVX_DATAFORMAT_GROUP_VIDEO_RGB24	
	(jvxSize)2, // JVX_DATAFORMAT_GROUP_VIDEO_NV12	
	(jvxSize)2, // JVX_DATAFORMAT_GROUP_VIDEO_YUYV
	1, // JVX_DATAFORMAT_GROUP_VIDEO_SINGLE8
	1, // JVX_DATAFORMAT_GROUP_VIDEO_MONO

	1, // JVX_DATAFORMAT_GROUP_GENERIC_INTERLEAVED
	1, // JVX_DATAFORMAT_GROUP_GENERIC_NON_INTERLEAVED

	1, // JVX_DATAFORMAT_GROUP_GENERIC
	1, // JVX_DATAFORMAT_GROUP_TRIGGER_ONLY,

	1, //JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD,
	1 // JVX_DATAFORMAT_GROUP_FFMPEG_FRAME_FWD

#ifndef JVX_NO_SYSTEM_EXTENSIONS
	JVX_DATAFORMAT_GROUP_SIZES_DIV_PRODUCT
#endif
	, JVX_DATAFORMAT_GROUP_LIMIT // <- this one for a generic test of number of entries
};

// I added an underscore since the completion always uses this function instead on jvxDataFormat_getsize
static inline jvxSize jvxDataFormatGroup_getsize_mult(jvxSize idx)
{
	assert(idx < JVX_DATAFORMAT_GROUP_LIMIT);
	return(jvxDataFormatGroup_size[idx]);
}

static inline jvxSize jvxDataFormatGroup_getsize_div(jvxSize idx)
{
	assert(idx < JVX_DATAFORMAT_GROUP_LIMIT);
	return(jvxDataFormatGroup_size_div[idx]);
}
// ====================================================================		

typedef enum
{
	JVX_DATAFLOW_PUSH_ACTIVE, // Devices and nodes push data forward by calling process function of subsequent linked component
	JVX_DATAFLOW_PUSH_ON_PULL, // Devices and nodes are triggered from subsequent element and call process function within this "pull"
	JVX_DATAFLOW_PUSH_ASYNC, // Devices and nodes push data forward by calling process function of subsequent linked component
	JVX_DATAFLOW_DONT_CARE, // All dataflows are accepted, this indicates that the value from input is used on output side in 1-to-1 connections
	JVX_DATAFLOW_LIMIT
} jvxDataflow;

static jvxTextHelpers jvxDataflow_str[JVX_DATAFLOW_LIMIT] =
{
	{"push", "JVX_DATAFLOW_PUSH_ACTIVE" },
	{"pull", "JVX_DATAFLOW_PUSH_ON_PULL"},
	{"async", "JVX_DATAFLOW_PUSH_ASYNC"},
	{"dontcare", "JVX_DATAFLOW_DONT_CARE"}
};

static inline const char* jvxDataflow_txt(jvxSize id)
{
	assert(id < JVX_DATAFLOW_LIMIT);
	return jvxDataflow_str[id].friendly;
}

static inline jvxDataflow jvxDataflow_decode(const char* token)
{
	jvxSize i;
	jvxDataflow res = JVX_DATAFLOW_DONT_CARE;
	for (i = 0; i < JVX_DATAFORMAT_GROUP_LIMIT; i++)
	{
		if (!strcmp(jvxDataflow_str[i].friendly, token))
		{
			res = (jvxDataflow)i;
			break;
		}
		if (!strcmp(jvxDataflow_str[i].full, token))
		{
			res = (jvxDataflow)i;
			break;
		}
	}
	return(res);
}

// ====================================================================		

typedef int (*the_jvx_printf_ext) (jvxHandle* priv, const char * txt);
typedef int (*the_jvx_printf) (const char * format, ... );

// Floating pointer for out-of-thread callbacks
struct jvxFloatingPointer;

typedef void(*jvx_floatingpointer_copy_allocate)(struct jvxFloatingPointer* copy_this, struct jvxFloatingPointer** to_this, jvxHandle* priv);
typedef void(*jvx_floatingpointer_deallocate)(struct jvxFloatingPointer* deallocate_this, jvxHandle* priv);

typedef enum 
{
	JVX_FLOATPOINTER_RELEASE_ALLOW_SHIFT = 0, // Set this flag to indicate that content has been copied
	JVX_FLOATPOINTER_REQUIRES_COPY_SHIFT = 1, // The data should be copied by the application as the content may change in the future
	JVX_FLOATPOINTER_REQUIRES_XCHG_DATA_LOCAL_GET = 2, // Use the floating pointer to exchage a single data. Tagid is the JVX_DATAFORMAT, the content is a pointer to a field acc. to get_property. A text field follows to describe property (non-zero-term)
	JVX_FLOATPOINTER_REQUIRES_XCHG_DATA_LOCAL_SET = 3 // Use the floating pointer to exchage a single data. Tagid is the JVX_DATAFORMAT, the content is a pointer to a field acc. to get_property. A text field follows to describe property (non-zero-term)
} jvxFloatingPointerFlags;

#ifdef JVX_OS_WINDOWS
#pragma pack(push, 1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

struct jvxFloatingPointer
{
	jvx_floatingpointer_copy_allocate copy;
	jvx_floatingpointer_deallocate deallocate;
	jvxHandle* priv;
	jvxSize tagid;
	jvxSize sz;
	jvxCBitField flags;
};

struct jvxFloatingPointerUInt32
{
	struct jvxFloatingPointer hdr;
	jvxUInt32 id;
};

#ifdef JVX_OS_WINDOWS
#pragma pack(pop)
#else
#pragma pack(pop)
#endif 

typedef enum
{
	JVX_FLOATINGPOINTER_NONE = 0,
	JVX_FLOATINGPOINTER_STRING_REPORT = 1 /* <- [jvxFloatingPointer char0 char1 ... Nul-Char] */,
	JVX_FLOATINGPOINTER_STATE_REPORT_IMMEDIATE = 2 /* <- jvxFloatingPointerUInt32 */,
	JVX_FLOATINGPOINTER_STATUS_REQUEST_IMMEDIATE = 3 /* <- jvxFloatingPointerUInt32 */,
	JVX_FLOATINGPOINTER_UPDATE_PROPERTY_REPORT_IMMEDIATE = 3 /* <- jvxFloatingPointer */,
	JVX_FLOATINGPOINTER_USER_OFFSET = 256
} jvxFloatingPointerType;

// Struct to combine property constraints
typedef struct
{
	jvxSize offset;
	jvxSize num;
	jvxSize form;
	jvxHandle* fld;
} jvxFloatingPointer_propadmin;

// Maximum length of packets for abused floating pointer in case of property exchange
#define JVX_FLOATING_POINTER_PROP_ABUSE_SIZE (sizeof(jvxFloatingPointer) + 25)

/*
 * Here an example: newPrv->runtime.buffers.pt->copy = NULL;
	newPrv->runtime.buffers.pt->deallocate = NULL;
	newPrv->runtime.buffers.pt->priv = NULL;
	newPrv->runtime.buffers.pt->tagid = 1;
	newPrv->runtime.buffers.pt->sz = sld;
	newPrv->runtime.buffers.pt->flags = 1 << JVX_FLOATPOINTER_REQUIRES_COPY_SHIFT;
 */
typedef jvxErrorType(*jvx_data_callback)(jvxHandle* priv, struct jvxFloatingPointer* content);

/////////////////////////
// numerical constants
/////////////////////////

#define JVX_MAX_INT_32_INT32 ((jvxInt32)0x7FFFFFFF)
#define JVX_MAX_INT_16_INT16 ((jvxInt16)0x7FFF)

#define JVX_MAX_INT_64_DATA ((jvxData)0x7FFFFFFFFFFFFFFF)
#define JVX_MAX_INT_32_DATA ((jvxData)0x7FFFFFFF)
#define JVX_MAX_UINT_32_DATA ((jvxData)0xFFFFFFFF)
#define JVX_MAX_INT_24_DATA ((jvxData)0x7FFFFF)
#define JVX_MAX_INT_16_DATA ((jvxData)0x7FFF)
#define JVX_MAX_INT_8_DATA ((jvxData)0x7F)
#define JVX_MAX_INT_64_DIV ((jvxData)1.0/JVX_MAX_INT_64_DATA)
#define JVX_MAX_INT_32_DIV ((jvxData)1.0/JVX_MAX_INT_32_DATA)
#define JVX_MAX_INT_24_DIV ((jvxData)1.0/JVX_MAX_INT_24_DATA)
#define JVX_MAX_INT_16_DIV ((jvxData)1.0/JVX_MAX_INT_16_DATA)
#define JVX_MAX_INT_8_DIV ((jvxData)1.0/JVX_MAX_INT_8_DATA)

#ifndef M_PI
#define M_PI (jvxData)(3.14159265358979323846264338327950288)
#endif

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
#define mat_epsMin  2.220446049250313100000000000000e-016
#define mat_epsMax 1.797693134862315700000000000000e+308
#else
#define mat_epsMin JVX_DBL_2_DATA(1.192092895507812500000000000000e-007)
#define mat_epsMax JVX_DBL_2_DATA(3.402823466385288600000000000000e+038)
#endif

#define JVX_DATA_2_DATA(in) in
#define JVX_DATA_2_FLOAT(in) ((float)in)
#define JVX_DATA_2_DOUBLE(in) ((double)in)
#define JVX_FLOAT_2_DATA(in) ((jvxData)in)
#define JVX_DOUBLE_2_DATA(in) ((jvxData)in)

#define JVX_INT32_2_DATA(in) ((jvxData)in * JVX_MAX_INT_32_DIV)
#define JVX_INT16_2_DATA(in) ((jvxData)in * JVX_MAX_INT_16_DIV)
#define JVX_INT64_2_DATA(in) ((jvxData)in * JVX_MAX_INT_64_DIV)
#define JVX_INT8_2_DATA(in) ((jvxData)in * JVX_MAX_INT_8_DIV)

#define JVX_DATA_2_INT32(in) ((jvxInt32) (in * JVX_MAX_INT_32_DATA))
#define JVX_DATA_2_UINT32(in) ((jvxUInt32) (in * JVX_MAX_UINT_32_DATA))
#define JVX_DATA_2_INT16(in) ((jvxInt16) (in * JVX_MAX_INT_16_DATA))
#define JVX_DATA_2_INT8(in) ((jvxInt8) (in * JVX_MAX_INT_8_DATA))
#define JVX_DATA_2_INT64(in) ((jvxInt64) (in * JVX_MAX_INT_64_DATA))

// ===============================================================
// ===============================================================

JVX_SYSTEM_LIB_BEGIN

JVX_STATIC_INLINE jvxInt64
JVX_DATA2INT64(jvxData a)
{
	return (jvxInt64)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE jvxUInt64
JVX_DATA2UINT64(jvxData a)
{
	return (jvxUInt64)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE jvxInt64
JVX_DDOUBLE2INT64(double a)
{
	return (jvxInt64)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE jvxInt64
JVX_DFLOAT2INT64(float a)
{
	return (jvxInt64)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE jvxInt32
JVX_DATA2INT32(jvxData a)
{
	return (jvxInt32)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE jvxUInt32
JVX_DATA2UINT32(jvxData a)
{
	return (jvxUInt32)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE jvxInt16
JVX_DATA2INT16(jvxData a)
{
	return (jvxInt16)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE jvxUInt16
JVX_DATA2UINT16(jvxData a)
{
	return (jvxUInt16)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE jvxInt8
JVX_DATA2INT8(jvxData a)
{
	return (jvxInt8)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE jvxUInt8
JVX_DATA2UINT8(jvxData a)
{
	return (jvxUInt8)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE jvxSize
JVX_DATA2SIZE(jvxData a)
{
	return (jvxSize)((a)+JVX_SIGN(a)*0.5);
}

JVX_STATIC_INLINE void
JVX_DATA2INT32Q(jvxData input, jvxInt32 Q, jvxInt32 *output)
{
	jvxData multCoeff = (jvxData)((long long)1<<Q);
	jvxData maxVal = (long long)1<<31;

	input *= multCoeff;

	// Apply saturation
	if(input > maxVal)
	{
		input = maxVal;
	}

	if(input < -maxVal)
	{
		input = -maxVal;
	}

	if(input > 0)
	{
		input += 0.5;
	}
	else
	{
		input -= 0.5;
	}

	*output = (jvxInt32)input;

	// For the maximum positive case: Avoid wrap around!!
	if((input > 0 )&&(*output < 0))
	{
		*output = ((long long)1<<31)-1;
	}
}


// Use unsigned value for selection index. Unselected is also available
#define JVX_CHECK_SIZE_OVERFLOW
#define JVX_SIZE_UNSELECTED ((jvxSize)-1)
#define JVX_SIZE_DONTCARE ((jvxSize)-2)
#define JVX_SIZE_SLOT_RETAIN ((jvxSize)-3)
#define JVX_SIZE_SLOT_OFF_SYSTEM ((jvxSize)-4)
#define JVX_SIZE_STATE_INCREMENT ((jvxSize)-5)
#define JVX_SIZE_STATE_DECREMENT ((jvxSize)-6)
#define JVX_SIZE_LIMIT JVX_SIZE_STATE_DECREMENT
#define JVX_SIZE_NUM_EXTRA_CASES_WITH_0 7 

#define JVX_SIZE_UNSELECTED_INT32 ((jvxInt32)-1)

#define JVX_CHECK_SIZE_SELECTED(var) (((jvxSize)var) < JVX_SIZE_LIMIT)
#define JVX_CHECK_SIZE_DONTCARE(var) (((jvxSize)var) == JVX_SIZE_DONTCARE)
#define JVX_CHECK_SIZE_UNSELECTED(var) (((jvxSize)var) == JVX_SIZE_UNSELECTED)
#define JVX_CHECK_SIZE_RETAIN(var) (((jvxSize)var) == JVX_SIZE_SLOT_RETAIN)
#define JVX_CHECK_SIZE_OFF_SYSTEM(var) (((jvxSize)var) == JVX_SIZE_SLOT_OFF_SYSTEM)
#define JVX_CHECK_SIZE_STATE_INCREMENT(var) (((jvxSize)var) == JVX_SIZE_STATE_INCREMENT)
#define JVX_CHECK_SIZE_STATE_DECREMENT(var) (((jvxSize)var) == JVX_SIZE_STATE_DECREMENT)

static inline jvxInt64 JVX_SIZE_INT64(jvxSize val) 
{
	switch (val)
	{
	case JVX_SIZE_UNSELECTED:
		return((jvxInt64)-1);
	case JVX_SIZE_DONTCARE:
		return((jvxInt64)-2);
	case JVX_SIZE_SLOT_RETAIN:
		return((jvxInt64)-3);
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		return((jvxInt64)-4);
	}
#ifdef JVX_CHECK_SIZE_OVERFLOW
	if(val >= JVX_SIZE_OVERFLOW)
	{
		printf("jvxSize to jvxInt64 conversion overflow.\n");
	}
#endif	
	return((jvxInt64) val);
}

static inline jvxSize JVX_INT_SIZE(int val)
{
	switch (val)
	{
	case -1:
		return(JVX_SIZE_UNSELECTED);
	case -2:
		return(JVX_SIZE_DONTCARE);
	case -3:
		return(JVX_SIZE_SLOT_RETAIN);
	case -4:
		return JVX_SIZE_SLOT_OFF_SYSTEM;
	}
	return((jvxSize)val);
}

static inline jvxSize JVX_UINT16_SIZE(jvxUInt16 val)
{
	switch (val)
	{
	case (jvxUInt16)-1:
		return(JVX_SIZE_UNSELECTED);
	case (jvxUInt16)-2:
		return(JVX_SIZE_DONTCARE);
	case (jvxUInt16)-3:
		return(JVX_SIZE_SLOT_RETAIN);
	case(jvxUInt16)-4:
		return JVX_SIZE_SLOT_OFF_SYSTEM;
	}
	return((jvxSize)val);
}

static inline jvxSize JVX_UINT32_SIZE(jvxUInt32 val)
{
	switch (val)
	{
	case (jvxUInt32)-1:
		return(JVX_SIZE_UNSELECTED);
	case (jvxUInt32)-2:
		return(JVX_SIZE_DONTCARE);
	case (jvxUInt32)-3:
		return(JVX_SIZE_SLOT_RETAIN);
	case(jvxUInt32)-4: 
		return JVX_SIZE_SLOT_OFF_SYSTEM;
	}
	return((jvxSize)val);
}

static inline int JVX_SIZE_INT(jvxSize val)
{
	switch(val)
	{
	case JVX_SIZE_UNSELECTED:
		return((int)-1);
	case JVX_SIZE_DONTCARE:
		return((int)-2);
	case JVX_SIZE_SLOT_RETAIN:
		return((int)-3);
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		return((int)-4);
	}

#ifdef JVX_CHECK_SIZE_OVERFLOW
	if(val >= INT_MAX-1)
	{
		printf("jvxSize to int conversion overflow.\n");
	}
#endif	
	return((int) val);
}

static inline unsigned int JVX_SIZE_UINT(jvxSize val)
{
	switch (val)
	{
	case JVX_SIZE_UNSELECTED:
		return((unsigned int)-1);
	case JVX_SIZE_DONTCARE:
		return((unsigned int)-2);
	case JVX_SIZE_SLOT_RETAIN:
		return((unsigned int)-3);
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		return((unsigned int)-4);
	}

#ifdef JVX_CHECK_SIZE_OVERFLOW
	if (val >= UINT_MAX - 1)
	{
		printf("jvxSize to int conversion overflow.\n");
	}
#endif	
	return((unsigned int)val);
}

static inline jvxInt32 JVX_SIZE_INT32(jvxSize val) 
{
	switch (val)
	{
	case JVX_SIZE_UNSELECTED:
		return((jvxInt32)-1);
	case JVX_SIZE_DONTCARE:
		return((jvxInt32)-2);
	case JVX_SIZE_SLOT_RETAIN:
		return((jvxInt32)-3);
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		return((jvxInt32)-4);
	}

#ifdef JVX_CHECK_SIZE_OVERFLOW

	if((jvxInt64)val > 0x7FFFFFFF)
	{
		printf("jvxSize to jvxInt32 conversion overflow.\n");
	}
#endif	
	return((jvxInt32) val);
}

static inline jvxUInt32 JVX_SIZE_UINT32(jvxSize val)
{
	switch (val)
	{
	case JVX_SIZE_UNSELECTED:
		return((jvxUInt32)-1);
	case JVX_SIZE_DONTCARE:
		return((jvxUInt32)-2);
	case JVX_SIZE_SLOT_RETAIN:
		return((jvxUInt32)-3);
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		return((jvxUInt32)-4);
	}

#ifdef JVX_CHECK_SIZE_OVERFLOW
	if (val >= (jvxUInt32)-1)
	{
		printf("jvxSize to jvxUInt32 conversion overflow.\n");
	}
#endif	
	return((jvxUInt32)val);
}

static inline jvxUInt32 JVX_ULONG_UINT32(unsigned long val)
{
#ifdef JVX_CHECK_SIZE_OVERFLOW
	if (val >= (jvxUInt32)-1)
	{
		printf("unsigned long to jvxUInt32 conversion overflow.\n");
	}
#endif	
	return((jvxUInt32)val);
}

static inline jvxInt32 JVX_INTMAX_INT32(intmax_t val)
{

#ifdef JVX_CHECK_SIZE_OVERFLOW
	if (val > 0x7FFFFFFF)
	{
		printf("intmax_t to jvxInt32 conversion overflow.\n");
	}
#endif	
	return((jvxInt32)val);
}

static inline jvxUInt32 JVX_LONG_INT32(long val)
{

#ifdef JVX_CHECK_SIZE_OVERFLOW
	if (val > 0x7FFFFFFF)
	{
		printf("unsigned long to jvxInt32 conversion overflow.\n");
	}
#endif	
	return((jvxUInt32)val);
}

static inline jvxInt16 JVX_SIZE_INT16(jvxSize val) 
{
	switch (val)
	{
	case JVX_SIZE_UNSELECTED:
		return((jvxInt16)-1);
	case JVX_SIZE_DONTCARE:
		return((jvxInt16)-2);
	case JVX_SIZE_SLOT_RETAIN:
		return((jvxInt16)-3);
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		return((jvxInt16)-4);
	}

#ifdef JVX_CHECK_SIZE_OVERFLOW
	if((jvxInt64)val > 0x7FFF)
	{
		printf("jvxSize to jvxInt16 conversion overflow.\n");
	}
#endif	
	return((jvxInt16) val);
}

static inline jvxUInt16 JVX_SIZE_UINT16(jvxSize val)
{
	switch (val)
	{
	case JVX_SIZE_UNSELECTED:
		return((jvxUInt16)-1);
	case JVX_SIZE_DONTCARE:
		return((jvxUInt16)-2);
	case JVX_SIZE_SLOT_RETAIN:
		return((jvxUInt16)-3);
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		return((jvxUInt16)-4);
	}

#ifdef JVX_CHECK_SIZE_OVERFLOW
	if (val >= (jvxUInt16)-1)
	{
		printf("jvxSize to jvxUInt16 conversion overflow.\n");
	}
#endif	
	return((jvxUInt16)val);
}

static inline jvxInt8 JVX_SIZE_INT8(jvxSize val)
{
	switch (val)
	{
	case JVX_SIZE_UNSELECTED:
		return((jvxInt8)-1);
	case JVX_SIZE_DONTCARE:
		return((jvxInt8)-2);
	case JVX_SIZE_SLOT_RETAIN:
		return((jvxInt8)-3);
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		return((jvxInt8)-4);
	}
#ifdef JVX_CHECK_SIZE_OVERFLOW
	if(val > 0x7F)
	{
		printf("jvxSize to jvxInt8 conversion overflow.\n");
	}
#endif	
	return((jvxInt8) val);
}

static inline jvxUInt8 JVX_SIZE_UINT8(jvxSize val)
{
	switch (val)
	{
	case JVX_SIZE_UNSELECTED:
		return((jvxUInt8)-1);
	case JVX_SIZE_DONTCARE:
		return((jvxUInt8)-2);
	case JVX_SIZE_SLOT_RETAIN:
		return((jvxUInt8)-3);
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		return((jvxUInt8)-4);
	}
#ifdef JVX_CHECK_SIZE_OVERFLOW
	if (val >= (jvxUInt8)-1)
	{
		printf("jvxSize to jvxInt8 conversion overflow.\n");
	}
#endif	
	return((jvxInt8)val);
}

static inline jvxData JVX_INT64_DATA(jvxInt64 val)
{
	jvxData valD = (jvxData)val;
	return(valD);
}

static inline jvxData JVX_UINT64_DATA(jvxUInt64 val)
{
	jvxData valD = (jvxData)val;
	return(valD);
}

static inline jvxData JVX_INT32_DATA(jvxInt32 val)
{
	jvxData valD = (jvxData)val;
	return(valD);
}

static inline jvxData JVX_UINT32_DATA(jvxUInt32 val)
{
	jvxData valD = (jvxData)val;
	return(valD);
}

static inline jvxData JVX_SIZE_DATA(jvxSize val)
{
	jvxData valD = (jvxData)val;
	return(valD);
}

JVX_SYSTEM_LIB_END

#endif
