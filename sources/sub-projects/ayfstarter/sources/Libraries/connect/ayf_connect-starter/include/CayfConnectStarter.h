#ifndef _AYFCONNECTSTARTER_H__ 
#define _AYFCONNECTSTARTER_H__

#include "CayfComponentLib.h"
#include "CayfComponentLibContainer.h"

class CayfConnectStarterContainer : public CayfComponentLibContainer
{
public:
	CayfConnectStarterContainer();

	CayfComponentLib* allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regToken)override;
	void deallocateDeviceObject(CayfComponentLib* delMe)override;
};

class CayfConnectStarter : public CayfComponentLib
{
public:
	jvxBool passthroughMode = true;

	CayfConnectStarter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthgroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg);

	// ================================================================
	// Interface for subclass <CayfComponentLib>
	// ================================================================

	jvxErrorType allocate_nodes(IjvxObject*& mainObjArg, std::list< IjvxObject*>& subsequentComponentsArg) override;
	jvxErrorType deallocate_nodes(IjvxObject*& mainObjArg, std::list< IjvxObject*>& subsequentComponentsArg) override;
};

#endif
