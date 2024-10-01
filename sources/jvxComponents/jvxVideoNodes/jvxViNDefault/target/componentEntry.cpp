#include "jvxNodes/CjvxBareNode1io.h"
#include "jvxVideoNodes/CjvxVideoNodeTpl.h"

#define COMPONENT_DESCRIPTION "JVX Default Video Node (skeleton)"
#define COMPONENT_TYPE CjvxVideoNodeTpl<CjvxBareNode1io>
#define COMPONENT_TYPE_SPECIALIZATION JVX_COMPONENT_VIDEO_NODE
#define _JVX_ALLOW_MULTIPLE_INSTANCES

#include "templates/targets/factory_IjvxObject.cpp"

