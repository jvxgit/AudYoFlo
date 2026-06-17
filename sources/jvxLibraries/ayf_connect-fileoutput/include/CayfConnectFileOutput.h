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
	// CayfComponentLib::_oneSubModule subModMixOut;
	IjvxObject* priObj = nullptr;
	IjvxObject* secObj = nullptr;


	CayfConnectFileOutput(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthgroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg);

	// ================================================================
	// Interface for subclass <CayfComponentLib>
	// ================================================================

	jvxErrorType allocate_nodes(IjvxObject*& mainObjArg, std::list< IjvxObject*>& subsequentComponentsArg) override;
	jvxErrorType deallocate_nodes(IjvxObject*& mainObjArg, std::list< IjvxObject*>& subsequentComponentsArg) override;
	jvxErrorType on_node_state_switch(IjvxHiddenInterface* hostRef, IjvxNode* node, jvxStateSwitch sw) override;
	
};

#endif
