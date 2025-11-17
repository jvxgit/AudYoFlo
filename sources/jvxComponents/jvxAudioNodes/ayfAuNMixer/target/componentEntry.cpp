#include "CayfAuNMixer.h"

#ifdef JVX_EXTERNAL_CALL_ENABLED
#define COMPONENT_DESCRIPTION "AudYoFlo Mixer Project - matlab"
#define COMPONENT_DESCRIPTOR Quotes(JVX_MODULE_NAME) Quotes(m)
#else
#define COMPONENT_DESCRIPTION "AudYoFlo Mixer Project"
#endif 

#ifdef JVX_PROJECT_NAMESPACE
#define COMPONENT_TYPE JVX_PROJECT_NAMESPACE::CayfAuNMixer
#else
#define COMPONENT_TYPE CayfAuNMixer
#endif

#define FEATURE_CLASS JVX_FEATURE_CLASS_EXTERNAL_CALL | JVX_FEATURE_CLASS_HOOKUP_CALL
#define _JVX_ALLOW_MULTIPLE_INSTANCES

#include "templates/targets/factory_IjvxObject.cpp"

