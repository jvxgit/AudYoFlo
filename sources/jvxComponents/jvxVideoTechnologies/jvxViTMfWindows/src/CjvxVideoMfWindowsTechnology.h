#ifndef __CJVXVIDEOMFOPENGLTECHNOLOGY_H__
#define __CJVXVIDEOMFOPENGLTECHNOLOGY_H__

#include "jvx.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include "jvxTechnologies/CjvxTemplateTechnology.h"
#include "CjvxVideoMfWindowsDevice.h"

class CjvxVideoMfWindowsTechnology: public CjvxTemplateTechnology<CjvxVideoMfWindowsDevice>
{
	IMFActivate** lstDevices;
	UINT32 numDevices;
	jvxBool requiresComUninitialize;

public:
	JVX_CALLINGCONVENTION CjvxVideoMfWindowsTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxVideoMfWindowsTechnology(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION activate();
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate();

};

#endif