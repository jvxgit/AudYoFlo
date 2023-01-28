#include "flutter_native_local.h"

int ffi_periodic_trigger(void* opaque_hdl)
{
	jvxLibHost* ll = nullptr;
	ffi_get_libhost_pointer(opaque_hdl, ll);
	ll->periodic_trigger();
	return JVX_NO_ERROR;
}