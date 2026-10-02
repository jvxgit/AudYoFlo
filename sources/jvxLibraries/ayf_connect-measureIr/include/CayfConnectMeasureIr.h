#ifndef _CAYFCONNECTMEASUREIR_H_ 
#define _CAYFCONNECTMEASUREIR_H_

#include "CayfComponentLib.h"
#include "CayfComponentLibContainer.h"

class CayfConnectMeasureIrContainer : public CayfComponentLibContainer
{
public:
	CayfConnectMeasureIrContainer();

	CayfComponentLib* allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regToken)override;
	void deallocateDeviceObject(CayfComponentLib* delMe)override;
};

class CayfConnectMeasureIr : public CayfComponentLib
{
public:
	jvxBool passthroughMode = true;

	CayfConnectMeasureIr(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthgroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg);

	// ================================================================
	// Interface for subclass <CayfComponentLib>
	// ================================================================

	jvxErrorType allocate_nodes(ayfConnectArgAllocateDeallocate& arg) override;
	jvxErrorType deallocate_nodes(ayfConnectArgAllocateDeallocate& arg) override;
};

#endif
