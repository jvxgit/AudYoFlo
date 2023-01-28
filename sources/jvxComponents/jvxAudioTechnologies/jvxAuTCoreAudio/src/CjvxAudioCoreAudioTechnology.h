#ifndef __CJVXAUDIOCOREAUDIOTECHNOLOGY_H__
#define __CJVXAUDIOCOREAUDIOTECHNOLOGY_H__

#include "jvx.h"
#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "CjvxAudioCoreAudioDevice.h"

class CjvxAudioCoreAudioTechnology: public CjvxAudioTechnology
{
private:
	struct
	{
		UInt32 numDevices;
		AudioDeviceID* devicesHandles;
	} runtime;

public:
	JVX_CALLINGCONVENTION CjvxAudioCoreAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxAudioCoreAudioTechnology(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;


	jvxErrorType initializeCoreAudio();
	jvxErrorType terminateCoreAudio();

};

#endif
