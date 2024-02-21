#include "CayfAuNStarter.h"

#ifdef JVX_EXTERNAL_CALL_ENABLED
#define COMPONENT_DESCRIPTION "AudYoFlo Starter Project - matlab"
#define COMPONENT_DESCRIPTOR Quotes(JVX_MODULE_NAME) Quotes(m)
#else
#define COMPONENT_DESCRIPTION "AudYoFlo Starter Project"
#endif 

#ifdef JVX_PROJECT_NAMESPACE
#define COMPONENT_TYPE JVX_PROJECT_NAMESPACE::CayfAuNStarter
#else
#define COMPONENT_TYPE CayfAuNStarter
#endif

#define FEATURE_CLASS JVX_FEATURE_CLASS_EXTERNAL_CALL | JVX_FEATURE_CLASS_HOOKUP_CALL
#define _JVX_ALLOW_MULTIPLE_INSTANCES

#include "templates/targets/factory_IjvxObject.cpp"

