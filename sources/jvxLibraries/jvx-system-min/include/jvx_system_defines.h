#ifndef __JVX_SYSTEM_DEFINES_H__
#define __JVX_SYSTEM_DEFINES_H__

#ifdef __cplusplus
#define JVX_SYSTEM_LIB_BEGIN extern "C" {
#define JVX_SYSTEM_LIB_END }
#else
#define JVX_SYSTEM_LIB_BEGIN
#define JVX_SYSTEM_LIB_END
#endif

// =======================================================================
// Declare enums and string desciptions in one table automatically!
// =======================================================================

#define GENERATE_ENUM(name) name,
#define GENERATE_STRING(name) #name,

/* Example usage:
#define OUTPUT_OPTIONS_AF_CFADE(X) \
    X(output_option_crossfade_off) \
    X(output_option_crossfade_on)

static const char* ayf_af_output_af_cfade_option_str[] =
{
    OUTPUT_OPTIONS_AF_CFADE(GENERATE_STRING)
};

*/

#endif