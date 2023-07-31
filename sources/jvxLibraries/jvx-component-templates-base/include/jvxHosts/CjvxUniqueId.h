#ifndef __CJVXUNIQUEID_H__
#define __CJVXUNIQUEID_H__

#include "jvx.h"

class CjvxUniqueId
{
protected:
	jvxSize hostUniqueId = 1;
	std::map<jvxSize, std::string> registered_ids;
	
public:
	
	jvxErrorType _obtain_unique_id(jvxSize* idOnReturn, const char* desc);
	jvxErrorType _release_unique_id(jvxSize idOnReturn);
	jvxErrorType _descriptor_unique_id(jvxSize idOnReturn, jvxApiString* onRet);
};
	
#endif
