#ifndef __NATIVE_HOST_CONFIGURE_FUNC_POINTERS_H__
#define __NATIVE_HOST_CONFIGURE_FUNC_POINTERS_H__

class configureFactoryHost_features;
class jvxNativeHostSysPointers;

struct native_host_configure_func_pointers
{
	jvxErrorType (*access_link_objects)(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* adescr, jvxComponentType tp, jvxSize id) = nullptr;
	jvxErrorType (*default_connection_rules_add)(IjvxDataConnections* theDataConnectionDefRule) = nullptr;
	jvxErrorType (*configure_factoryhost_features)(configureFactoryHost_features* features) = nullptr;
	jvxErrorType (*invalidate_factoryhost_features)(configureFactoryHost_features* features) = nullptr;
	jvxErrorType (*default_sequence_add)(IjvxSequencer* theSeq) = nullptr;
};

typedef jvxErrorType(*flutter_config_open_ptr)(native_host_configure_func_pointers* fcPtr);
typedef jvxErrorType(*flutter_vm_open_ptr)(jvxNativeHostSysPointers* fcPtr);

#define FLUTTER_CONFIG_OPEN_FUNCTION_NAME "flutter_config_open"

#endif
