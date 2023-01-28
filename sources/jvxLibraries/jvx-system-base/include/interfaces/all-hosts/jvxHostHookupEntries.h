#ifndef JVX_HOST_HOOK_ENTRIES_H__
#define JVX_HOST_HOOK_ENTRIES_H__

#include "interfaces/all-hosts/configHostFeatures_common.h"

extern "C"
{
	// Define entry for link object extension of host
	extern jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, 
		jvxApiString* adescr, jvxComponentType tp, jvxSize id);

	// Entry for request of desired other features which can be specified by setting simple variables
	extern jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features);

	// Entry for request of desired other features which can be specified by setting simple variables
	extern jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features);
}

#endif
