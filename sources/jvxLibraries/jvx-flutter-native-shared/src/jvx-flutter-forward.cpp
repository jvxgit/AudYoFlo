#include "jvx.h"
#include "flutter_native_host_config.h"

extern native_host_configure_func_pointers* func_pointer_reference;

extern "C"
{
	jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* adescr, jvxComponentType tp, jvxSize id)
	{
		if (func_pointer_reference)
		{
			if (func_pointer_reference->access_link_objects)
			{
				return func_pointer_reference->access_link_objects(funcInit, funcTerm, adescr, tp, id);
			}
		}
		return JVX_ERROR_INVALID_SETTING;
	}

	jvxErrorType jvx_default_connection_rules_add(IjvxDataConnections* theDataConnectionDefRule)
	{
		if (func_pointer_reference)
		{
			if (func_pointer_reference->default_connection_rules_add)
			{
				return func_pointer_reference->default_connection_rules_add(theDataConnectionDefRule);
			}
		}
		return JVX_ERROR_INVALID_SETTING;
	}

	jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
	{
		if (func_pointer_reference)
		{
			if (func_pointer_reference->configure_factoryhost_features)
			{
				return func_pointer_reference->configure_factoryhost_features(features);
			}
		}
		return JVX_ERROR_INVALID_SETTING;
	}

	jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
	{
		if (func_pointer_reference)
		{
			if (func_pointer_reference->invalidate_factoryhost_features)
			{
				return func_pointer_reference->invalidate_factoryhost_features(features);
			}
		}
		return JVX_ERROR_INVALID_SETTING;
	}

	jvxErrorType jvx_default_sequence_add(IjvxSequencer* theSeq)
	{
		if (func_pointer_reference)
		{
			if (func_pointer_reference->default_sequence_add)
			{
				return func_pointer_reference->default_sequence_add(theSeq);
			}
		}
		return JVX_ERROR_INVALID_SETTING;
	}
}
