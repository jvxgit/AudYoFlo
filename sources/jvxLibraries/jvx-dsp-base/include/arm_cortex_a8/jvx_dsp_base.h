#ifndef __JVXL_DSP_BASE_H__
#define __JVXL_DSP_BASE_H__

#ifndef JVX_SYSTEM_USE_DATA_FORMAT_FLOAT
/* #undef JVX_DSP_BASE_USE_DATA_FORMAT_FLOAT */
#endif

#define JVX_VERSION_STRING "float-default"

#define JVX_DATA_FORMAT_FLOAT

#ifdef __cplusplus
#define JVX_DSP_LIB_BEGIN extern "C" {
#define JVX_DSP_LIB_END }
#else
#define JVX_DSP_LIB_BEGIN
#define JVX_DSP_LIB_END
#endif

// for unique module IDs
extern unsigned int jvxID;
unsigned int jvx_id(const char *description, // should be __func__ in caller
                    unsigned int pid); // parent module id

#ifdef __GNUC__
#define JVX_TARGET_GCC_CORTEX_A8
#else
#error "only GCC is supported so far"
#endif

#endif
