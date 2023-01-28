#ifndef __JVX_COMMON_H__
#define __JVX_COMMON_H__

#include "jvx_system_config.h"

#ifdef __cplusplus
#define JVX_SYSTEM_LIB_BEGIN extern "C" {
#define JVX_SYSTEM_LIB_END }
#else
#define JVX_SYSTEM_LIB_BEGIN
#define JVX_SYSTEM_LIB_END
#endif

#define Q(x) #x
#define Quotes(x) Q(x)

typedef struct
{
	const char* friendly;
	const char* full;
} jvxTextHelpers;

#define JVX_LIB_UPDATE_ASYNC_SHIFT 0
#define JVX_LIB_UPDATE_SYNC_SHIFT 1
#define JVX_LIB_UPDATE_INIT_SHIFT 2

#define JVX_LIB_UPDATE_USER_OFFSET_SHIFT 5
#define JVX_LIB_UPDATE_USER_OFFSET_EXTRACT_UINT16(val) ((jvxUInt16)val & 0xFFE0)
#define JVX_LIB_UPDATE_USER_OFFSET_EXTRACT_UINT32(val) ((jvxUInt32)val & 0xFFFFFFE0)
#define JVX_LIB_UPDATE_SYSTEM_EXTRACT(val) (val & 0x1F)
#define JVX_LIB_UPDATE_SYSTEM_SHIFT_USER_OFFSET(val) ((val) << JVX_LIB_UPDATE_USER_OFFSET_SHIFT)

typedef enum
{
	JVX_LIB_UPDATE_ASYNC = (1 << JVX_LIB_UPDATE_ASYNC_SHIFT), //0x1, // shift 0
	JVX_LIB_UPDATE_SYNC = (1 << JVX_LIB_UPDATE_SYNC_SHIFT), //  0x2, // shift 1
	JVX_LIB_UPDATE_INIT = (1 << JVX_LIB_UPDATE_INIT_SHIFT), //0x4, // shift 2
	JVX_LIB_UPDATE_USER_OFFSET = (1 << JVX_LIB_UPDATE_USER_OFFSET_SHIFT)
} jvxParameterUpdateType;

#define JVX_LIB_UPDATE_ALL16 0xFFFF
#define JVX_LIB_UPDATE_ALL16_USER_OFFSET JVX_LIB_UPDATE_SYSTEM_SHIFT_USER_OFFSET(0xEFF)
#define JVX_LIB_UPDATE_ALL32 0xFFFFFFFF

// for unique module IDs
extern unsigned int jvxID;
unsigned int jvx_id(const char *description, // should be __func__ in caller
	unsigned int pid); // parent module id

#ifndef JVX_COMMAND_LINE_TOKENS_BUILDIN 
#define JVX_COMMAND_LINE_TOKENS_BUILDIN "--textlog --verbose_dll --textloglev 10"
#endif

#endif
