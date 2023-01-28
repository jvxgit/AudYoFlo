#include "jvx.h"
#include "jvxHosts/CjvxUniqueId.h"

jvxErrorType
CjvxUniqueId::_obtain_unique_id(jvxSize* idOnReturn, const char* descr)
{
	jvxSize uId = hostUniqueId;
	hostUniqueId++;
	assert(JVX_CHECK_SIZE_SELECTED(hostUniqueId));
	if (idOnReturn)
	{
		*idOnReturn = uId;
		if (descr == NULL)
		{
			registered_ids[uId] = jvx_size2String(uId);
		}
		else
		{
			registered_ids[uId] = descr;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxUniqueId::_release_unique_id(jvxSize idOnReturn)
{
	std::map<jvxSize, std::string>::iterator elm = registered_ids.find(idOnReturn);
	if (elm != registered_ids.end())
	{
		registered_ids.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxUniqueId::_descriptor_unique_id(jvxSize idOnReturn, jvxApiString* onRet)
{
	std::map<jvxSize, std::string>::iterator elm = registered_ids.find(idOnReturn);
	if (elm != registered_ids.end())
	{
		onRet->assign(elm->second);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

