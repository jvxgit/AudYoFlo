
#include <vector>

#include "CjvxNetworkMasterTechnology.h"

#define COMPONENT_DESCRIPTION "JVX Master Network Audio"
#ifdef JVX_PROJECT_NAMESPACE
#define COMPONENT_TYPE JVX_PROJECT_NAMESPACE::CjvxNetworkMasterTechnology
#else
#define COMPONENT_TYPE CjvxNetworkMasterTechnology
#endif
/*#define _JVX_ALLOW_MULTIPLE_INSTANCES*/
#define FEATURE_CLASS JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER 

#include "templates/targets/factory_IjvxObject.cpp"

