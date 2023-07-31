#include "jvxHosts/CjvxConnectionMinHost.h"

// Trick to put macro for quotes around macro from:
// http://stackoverflow.com/questions/6671698/adding-quotes-to-argument-in-c-preprocessor
#define Q(x) #x
#define QUOTE(x) Q(x)

#define COMPONENT_DESCRIPTION "Connection Min Host"
#define COMPONENT_DESCRIPTOR QUOTE(COMPONENT_DESCRIPTOR_NAME)
#define COMPONENT_TYPE CjvxConnectionMinHost
/*#define _JVX_ALLOW_MULTIPLE_INSTANCES*/

#include "templates/targets/factory_IjvxObject.cpp"

