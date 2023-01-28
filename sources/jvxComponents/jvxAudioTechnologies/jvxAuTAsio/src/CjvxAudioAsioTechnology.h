#ifndef __CJVXAUDIOASIOTECHNOLOGY_H__
#define __CJVXAUDIOASIOTECHNOLOGY_H__

#include "jvx.h"
#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "CjvxAudioAsioDevice.h"

class CjvxAudioAsioTechnology: public CjvxAudioTechnology
{
public:
	JVX_CALLINGCONVENTION CjvxAudioAsioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxAudioAsioTechnology(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION activate();
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate();

	bool initializeAsio();
};

#endif