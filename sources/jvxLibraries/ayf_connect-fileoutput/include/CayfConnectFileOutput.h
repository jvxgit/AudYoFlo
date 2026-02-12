#ifndef _AYFCONNECTFILEOUTPUT_H__ 
#define _AYFCONNECTFILEOUTPUT_H__

#include "CayfComponentLib.h"
#include "CayfComponentLibContainer.h"

class CayfConnectFileOutputContainer : public CayfComponentLibContainer
{
public:
	CayfConnectFileOutputContainer();

	CayfComponentLib* allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regToken)override;
	void deallocateDeviceObject(CayfComponentLib* delMe)override;
};

class CayfConnectFileOutput : public CayfComponentLib
{
public:
	jvxBool passthroughMode = true;

	CayfConnectFileOutput(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthgroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg);

	// ================================================================
	// Interface for subclass <CayfComponentLib>
	// ================================================================

	jvxErrorType allocate_main_node() override;
	jvxErrorType deallocate_main_node() override;
	jvxErrorType on_main_node_selected() override;
	jvxErrorType before_main_node_unselect() override;
};

#endif
