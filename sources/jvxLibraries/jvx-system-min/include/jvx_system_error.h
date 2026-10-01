#ifndef __JVX_SYSTEM_ERROR_TYPE_H__
#define __JVX_SYSTEM_ERROR_TYPE_H__

#include "jvx_system_error_types.h"

JVX_SYSTEM_LIB_BEGIN

// ==================================================================
// Error types
// ==================================================================

extern jvxTextHelpers jvxErrorType_str[JVX_ERROR_LIMIT];

const char* jvxErrorType_txt(jvxSize id);
const char* jvxErrorType_descr(jvxSize id);
jvxErrorType jvxErrorType_decode(const char* txt);

JVX_SYSTEM_LIB_END

#endif
