#ifndef __CJVXCORECONTROLLERDEVICE_H__
#define __CJVXCORECONTROLLERDEVICE_H__

#include "jvxAppCtrTech/CjvxAppCtrDevice.h"
#include "pcg_exports_device.h"

class CjvxCoreControllerDevice: public CjvxApplicationControllerDevice, public genAppController_device
{
	struct
	{
		IjvxToolsHost* theToolsHost;

		struct
		{
			IjvxObject* object;
			IjvxThreadController* hdl;
		} coreHdl;

		jvxBool subModuleLoaded;
	} runtime;

public:
	JVX_CALLINGCONVENTION CjvxCoreControllerDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	JVX_CALLINGCONVENTION ~CjvxCoreControllerDevice();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
};

#endif
