#ifndef __FLUTTER_NATIVE_MACROS_H__
#define __FLUTTER_NATIVE_MACROS_H__

#ifdef __cplusplus
#define JVX_FLUTTER_LIB_BEGIN extern "C" {
#define JVX_FLUTTER_LIB_END }
#else
#define JVX_FLUTTER_LIB_BEGIN
#define JVX_FLUTTER_LIB_END
#endif

#endif
