#ifndef __JVXL_DSP_BASE_H__
#define __JVXL_DSP_BASE_H__

/* #undef JVX_DSP_BASE_USE_DATA_FORMAT_FLOAT */
#define JVX_VERSION_STRING "@myversion@"

#ifdef JVX_SYSTEM_USE_DATA_FORMAT_FLOAT
#define JVX_DATA_FORMAT_FLOAT
#else
#define JVX_DATA_FORMAT_DOUBLE
#endif

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

#endif

