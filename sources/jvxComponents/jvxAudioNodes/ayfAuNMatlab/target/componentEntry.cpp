#include "CayfAuNMatlab.h"

#ifdef JVX_EXTERNAL_CALL_ENABLED
#define COMPONENT_DESCRIPTION "Matlab Example Project - matlab"
#define COMPONENT_DESCRIPTOR Quotes(JVX_MODULE_NAME) Quotes(m)
#define FEATURE_CLASS JVX_FEATURE_CLASS_EXTERNAL_CALL | JVX_FEATURE_CLASS_HOOKUP_CALL
#else
#define COMPONENT_DESCRIPTION "Matlab Example Project"
#define COMPONENT_DESCRIPTOR Quotes(JVX_MODULE_NAME)
#endif

#ifdef JVX_PROJECT_NAMESPACE
#define COMPONENT_TYPE JVX_PROJECT_NAMESPACE::CayfAuNMatlab
#else
#define COMPONENT_TYPE CayfAuNHeadRest
#endif

#define _JVX_ALLOW_MULTIPLE_INSTANCES

#include "templates/targets/factory_IjvxObject.cpp"

