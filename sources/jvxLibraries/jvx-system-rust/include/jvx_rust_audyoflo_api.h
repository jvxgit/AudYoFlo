#ifndef __JVX_RUST_AUDYOFLO_C_API_H__
#define __JVX_RUST_AUDYOFLO_C_API_H__

typedef jvxData JvxData;
typedef jvxSize JvxSize;
typedef jvxDspBaseErrorType JvxDspBaseErrorType;
typedef jvxCBool JvxCBool;
typedef jvxCBitField JvxCBitField;

#include "jvx_profiler/jvx_profiler_data_entry.h"

// We need to declare the callback function pointers here
typedef jvx_register_entry_profiling_data_c  RegisterCallbackFunction;
typedef jvx_unregister_entry_profiling_data_c UnregisterCallbackFunction;

#endif
