#ifndef __CJVXIOSAUDIOTECHNOLOGY_H__
#define __CJVXIOSAUDIOTECHNOLOGY_H__

#include "jvx.h"
#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "CjvxIosAudioDevice.h"

class CjvxIosAudioTechnology: public CjvxAudioTechnology
{
    friend class CjvxIosAudioDevice;

private:

    jvxAudioProcessor_cpp theAudioLink;

/*
	struct
	{
		UInt32 numDevices;
		AudioDeviceID* devicesHandles;
	} runtime;
*/
public:
	JVX_CALLINGCONVENTION CjvxIosAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
  JVX_CALLINGCONVENTION ~CjvxIosAudioTechnology(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	jvxErrorType initializeIosAudio();
	jvxErrorType terminateIosAudio();

};

#endif
