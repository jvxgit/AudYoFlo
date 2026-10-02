#ifndef _CAYFCONNECTGENERIC_H_ 
#define _CAYFCONNECTGENERIC_H_

#include "CayfComponentLib.h"
#include "CayfComponentLibContainer.h"

class CayfConnectGenericContainer : public CayfComponentLibContainer
{
public:
	CayfConnectGenericContainer();

	CayfComponentLib* allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regToken)override;
	void deallocateDeviceObject(CayfComponentLib* delMe)override;
};

class CayfConnectGeneric : public CayfComponentLib
{
public:
	jvxBool passthroughMode = true;

	CayfConnectGeneric(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthgroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg);

	// ================================================================
	// Interface for subclass <CayfComponentLib>
	// ================================================================

	jvxErrorType allocate_nodes(ayfConnectArgAllocateDeallocate& arg) override;
	jvxErrorType deallocate_nodes(ayfConnectArgAllocateDeallocate& arg) override;
};

#endif
