#ifndef __CJVXAUDIOALSATECHNOLOGY_H__
#define __CJVXAUDIOALSATECHNOLOGY_H__

#include "jvx.h"
#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "CjvxAudioAlsaDevice.h"

class CjvxAudioAlsaTechnology: public CjvxAudioTechnology
{
public:
	JVX_CALLINGCONVENTION CjvxAudioAlsaTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxAudioAlsaTechnology(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	bool initializeAlsa();
};

#endif
