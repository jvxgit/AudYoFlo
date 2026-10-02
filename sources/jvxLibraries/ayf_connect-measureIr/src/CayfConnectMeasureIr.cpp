#include "CayfConnectMeasureIr.h"
#include "jvxSpNMeasureIr.h"

// ========================================================================================================

#define AYF_BINDING_MODULE_NAME "CayfConnectMeasureIr"
#define AYF_MODULE_CONTAINER_TYPE CayfConnectMeasureIrContainer
#include "ayf-component-entries-tpl.h"

// ========================================================================================================

CayfConnectMeasureIrContainer::CayfConnectMeasureIrContainer()
{
}

CayfComponentLib*
CayfConnectMeasureIrContainer::allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regTokenArg)
{
	// Embedding is online, let us check what we need to do now
	CayfConnectMeasureIr* newInst = nullptr;
	JVX_SAFE_ALLOCATE_OBJECT(newInst, CayfConnectMeasureIr(
		"Audio Device Object - MeasureIr", false,
		"audio_measure_ir", 0, "ayfConnectMeasureIr",
		JVX_COMPONENT_ACCESS_DYNAMIC_LIB, JVX_COMPONENT_AUDIO_NODE,
		"audio_device/CayfConnectMeasureIr", nullptr, (passthroughMode != 0), parent, regTokenArg));
	return newInst;
}

void
CayfConnectMeasureIrContainer::deallocateDeviceObject(CayfComponentLib* delMe)
{
	JVX_SAFE_DELETE_OBJECT(delMe);
}

// ========================================================================================================

CayfConnectMeasureIr::CayfConnectMeasureIr(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg) :
	CayfComponentLib(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL, parent), passthroughMode(passthroughModeArg)
{
	/*
	modName = "CayfConnectCtrl";
	regToken = modName;
	if (regTokenArg)
	{
		regToken = regTokenArg;
	}
	*/
	mainNodeName = "jvxSpNMeasureIr";
};

jvxErrorType
CayfConnectMeasureIr::allocate_nodes(ayfConnectArgAllocateDeallocate& arg)
{
	return jvxSpNMeasureIr_init(&arg.entryObj);	
}

jvxErrorType
CayfConnectMeasureIr::deallocate_nodes(ayfConnectArgAllocateDeallocate& arg)
{
	return jvxSpNMeasureIr_terminate(arg.entryObj);
}



