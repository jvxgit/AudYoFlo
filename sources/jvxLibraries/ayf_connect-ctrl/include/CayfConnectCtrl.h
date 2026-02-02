#ifndef _AYFWEAVERCONNECTCTRL_H__ 
#define _AYFWEAVERCONNECTCTRL_H_

#include "CayfComponentLib.h"
#include "CayfComponentLibContainer.h"

class CayfConnectCtrlContainer : public CayfComponentLibContainer
{
public:
	CayfConnectCtrlContainer();

	CayfComponentLib* allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regToken)override;
	void deallocateDeviceObject(CayfComponentLib* delMe)override;
};

class CayfConnectCtrl : public CayfComponentLib
{
public:
	jvxBool passthroughMode = true;

	CayfConnectCtrl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthgroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg);

	// ================================================================
	// Interface for subclass <CayfComponentLib>
	// ================================================================

	jvxErrorType allocate_main_node() override;
	jvxErrorType deallocate_main_node() override;
};

#endif
