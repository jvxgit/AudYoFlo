#ifndef _AYFCONNECTFILEINPUT_H__ 
#define _AYFCONNECTFILEINPUT_H__

#include "CayfComponentLib.h"
#include "CayfComponentLibContainer.h"

class CayfConnectFileInputContainer : public CayfComponentLibContainer
{
public:
	CayfConnectFileInputContainer();

	CayfComponentLib* allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regToken)override;
	void deallocateDeviceObject(CayfComponentLib* delMe)override;
};

class CayfConnectFileInput : public CayfComponentLib
{
public:
	jvxBool passthroughMode = true;

	CayfConnectFileInput(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthgroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg);

	// ================================================================
	// Interface for subclass <CayfComponentLib>
	// ================================================================

	jvxErrorType allocate_main_node() override;
	jvxErrorType deallocate_main_node() override;
	jvxErrorType on_main_node_selected() override;
	jvxErrorType before_main_node_unselect() override;
};

#endif
