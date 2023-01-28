#include <vector>

#include "CjvxAuNNtask.h"

#ifdef JVX_EXTERNAL_CALL_ENABLED
#define COMPONENT_DESCRIPTION "JVX AuNTask - matlab"
#define COMPONENT_TYPE CjvxAuNNtaskm
#else
#define COMPONENT_DESCRIPTION "JVX AuNTask"
#define COMPONENT_TYPE CjvxAuNTasks
#endif
#define _JVX_ALLOW_MULTIPLE_INSTANCES

#define COMPONENT_TYPE_SPECIALIZATION JVX_COMPONENT_AUDIO_NODE

#ifdef JVX_EXTERNAL_CALL_ENABLED
#define FEATURE_CLASS JVX_FEATURE_CLASS_EXTERNAL_CALL | JVX_FEATURE_CLASS_HOOKUP_CALL
#endif

#define COMPONENT_DESCR_SPECIALIZATION "node/audio_node"

#include "templates/targets/factory_IjvxObject.cpp"

