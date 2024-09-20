#ifndef __CJVXVIDEOV4L2TECHNOLOGY_H__
#define __CJVXVIDEOV4L2TECHNOLOGY_H__

#include "jvx.h"

#include "jvxTechnologies/CjvxTemplateTechnology.h"
#include "CjvxVideoV4L2Device.h"

class CjvxVideoV4L2Technology: public CjvxTemplateTechnology<CjvxVideoV4L2Device>
{
	jvxSize numDevices;

public:
	JVX_CALLINGCONVENTION CjvxVideoV4L2Technology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxVideoV4L2Technology(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION activate();
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate();

};

#endif
