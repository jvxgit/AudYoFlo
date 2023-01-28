#ifndef _CJVXOFFLINEAUDIOTECHNOLOGY_H__
#define _CJVXOFFLINEAUDIOTECHNOLOGY_H__

#include "jvx.h"
#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "CjvxOfflineAudioDevice.h"

class CjvxOfflineAudioTechnology: public CjvxAudioTechnology
{
public:
	JVX_CALLINGCONVENTION CjvxOfflineAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxOfflineAudioTechnology();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate();
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate(); 

};

#endif
