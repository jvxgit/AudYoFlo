#ifndef __CJVXMINHOST_H__
#define __CJVXMINHOST_H__

#include "jvx.h"

class CjvxMinHost
{
public:
	CjvxMinHost();

	struct _common_set_min_host_T
	{
		jvxSize unique_object_id = 1;
	} _common_set_min_host;

	jvxErrorType _request_unique_host_id(jvxSize* uId);
};

#endif

