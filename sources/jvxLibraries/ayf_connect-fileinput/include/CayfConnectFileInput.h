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

	jvxErrorType allocate_nodes(IjvxObject*& mainObjArg, std::list< IjvxObject*>& subsequentComponentsArg) override;
	jvxErrorType deallocate_nodes(IjvxObject*& mainObjArg, std::list< IjvxObject*>& subsequentComponentsArg) override;
	jvxErrorType on_node_state_switch(IjvxHiddenInterface* hostRef, IjvxNode* node, jvxStateSwitch sw) override;
};

#endif
