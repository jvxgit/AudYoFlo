#ifndef __JVX_SMALL_H__
#define __JVX_SMALL_H__

#ifndef JVX_COMPILE_SMALL
#define JVX_COMPILE_SMALL
#endif

// Platform extensions for multi-platform
#include "jvx_platform.h"

// =============================================================
// TYPEDEFS TYPEDEFS TYPEDEFS TYPEDEFS TYPEDEFS TYPEDEFS TYPEDEFS
// =============================================================

// System typedefs
#include "typedefs/TjvxTypes.h"

#ifdef __cplusplus
#include "typedefs/TjvxTypes_cpp.h"
#endif

#include "typedefs/TjvxDataProcessor.h"

// Definition of Javox protocol for all remote data transfers
#include "typedefs/network/TjvxProtocol.h"

// Dependent tool definition
#include "typedefs/tools/TjvxRemoteCall.h"


// =============================================================
// INTERFACES INTERFACES INTERFACES INTERFACES INTERFACES INTERFACES
// =============================================================

// Main System interfaces
#include "compatibility/IjvxDataProcessor.h"

#endif
