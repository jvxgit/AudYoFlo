#include "jvx.h"
#include "flutter_native_host_config.h"

extern "C"
{
	extern jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* adescr, jvxComponentType tp, jvxSize id);
	extern jvxErrorType jvx_default_connection_rules_add(IjvxDataConnections* theDataConnectionDefRule);
	extern jvxErrorType jvx_default_sequence_add(IjvxSequencer* theSeq);
	extern jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features);
	extern jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features);
}

extern "C"
{
	jvxErrorType flutter_config_open(native_host_configure_func_pointers* fcPtr)
	{
		if (fcPtr)
		{
			fcPtr->access_link_objects = jvx_access_link_objects;
			fcPtr->configure_factoryhost_features = jvx_configure_factoryhost_features;
			fcPtr->invalidate_factoryhost_features = jvx_invalidate_factoryhost_features;
			fcPtr->default_connection_rules_add = jvx_default_connection_rules_add;
			fcPtr->default_sequence_add = jvx_default_sequence_add;
		}
		return JVX_NO_ERROR;
	}
} 