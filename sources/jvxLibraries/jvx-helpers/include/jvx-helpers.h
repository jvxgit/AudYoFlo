// No header guard here since there is one in each file

// Helper functionalities from jvx-helpers lib
#ifdef JVX_COMPILE_SMALL

#include "jvx_small.h"
#include "HjvxBitfieldTools.h"
#include "HjvxDataLinkDescriptor.h"
#include "HjvxAllocateDeallocateTpl.h"

#else

#include "jvx.h"
#include "HjvxBitfieldTools.h"
#include "HjvxMisc.h"
#include "HjvxJsonTools.h"
#include "HjvxArgs.h"
#include "HjvxSystem.h"
#include "HjvxBase64.h"
#include "HjvxAllocateDeallocateTpl.h"
#include "HjvxDataConnections.h"

#include "HjvxReadWriteFile.h"
#include "HjvxObject.h" 
#include "HjvxDataProcessor.h"
#include "HjvxDataLinkDescriptor.h"
#include "HjvxProperties.h"
#include "HjvxComponentPreselection.h"
#include "HjvxComponentPreselection_common.h"
#include "HjvxComponentTempConfig.h"

#include "HjvxSequencer.h"

#include "HjvxHostsAndProperties.h"

#include "CjvxConnectionDropzone.h"
#include "CjvxConnectionDescription.h"

#include "HjvxHost.h"

#include "HjvxDataLogger_Otf.h"
#include "HjvxDataLogger.h"

// Helpers
#include "tools/HjvxRs232.h"
#include "tools/HjvxRemoteCall.h"
#include "tools/HjvxExternalCommand.h"
#include "tools/HjvxReadConfigEntries.h"
#include "tools/HjvxWriteConfigEntries.h"
#include "tools/HjvxCodeGeneration.h"

#endif

