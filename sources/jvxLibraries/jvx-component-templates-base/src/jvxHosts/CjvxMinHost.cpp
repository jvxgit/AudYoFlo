#include "jvxHosts/CjvxMinHost.h"

CjvxMinHost::CjvxMinHost()
{
	_common_set_min_host.unique_object_id = 1;
}

jvxErrorType 
CjvxMinHost::_request_unique_host_id(jvxSize* uId)
{
	if (uId)
	{
		*uId = _common_set_min_host.unique_object_id;
		_common_set_min_host.unique_object_id++;
	}
	return JVX_NO_ERROR;
}