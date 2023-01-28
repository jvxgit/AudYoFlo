#include <vector>

#include "CjvxSPAsyncIo.h"

#ifdef JVX_ASYNCIO_MASTER_PRI
#define COMPONENT_DESCRIPTION "JVX async I/O processing node - pri master"
#else
#define COMPONENT_DESCRIPTION "JVX async I/O processing node - sec master"
#endif
#define COMPONENT_TYPE JVX_ASYNCIO_CLASSNAME
#define _JVX_ALLOW_MULTIPLE_INSTANCES

#define COMPONENT_TYPE_SPECIALIZATION JVX_COMPONENT_SIGNAL_PROCESSING_NODE
#define COMPONENT_DESCR_SPECIALIZATION "node/signal_processing_node"

#include "templates/targets/factory_IjvxObject.cpp" 

