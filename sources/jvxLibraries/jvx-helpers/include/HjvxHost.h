#ifndef __HJVXHOST_H__
#define __HJVXHOST_H__

#include "jvx.h"

void activate_default_components_host(const jvxModuleOnStart* lst_ModulesOnStart[JVX_COMPONENT_ALL_LIMIT], IjvxHost* hHost, jvxBool systemComponents, jvxBool verboseOutput);
void deactivate_default_components_host(const jvxModuleOnStart* lst_ModulesOnStart[JVX_COMPONENT_ALL_LIMIT], IjvxHost* hHost, jvxBool systemComponent);

#endif