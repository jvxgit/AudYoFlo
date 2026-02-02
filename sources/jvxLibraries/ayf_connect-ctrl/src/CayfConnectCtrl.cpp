#include "CayfConnectCtrl.h"
// #include "ayfAuNCtc.h"

// ========================================================================================================

#define AYF_BINDING_MODULE_NAME "CayfConnectCtrl"
#define AYF_MODULE_CONTAINER_TYPE CayfConnectCtrlContainer
#include "ayf-component-entries-tpl.h"

// ========================================================================================================

CayfConnectCtrlContainer::CayfConnectCtrlContainer()
{
}

CayfComponentLib*
CayfConnectCtrlContainer::allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regTokenArg)
{
	// Embedding is online, let us check what we need to do now
	CayfConnectCtrl* newInst = nullptr;
	JVX_SAFE_ALLOCATE_OBJECT(newInst, CayfConnectCtrl(
		"Audio Device Object - Ctrl", false,
		"audio_ctrl", 0, "ayfConnectCtrl",
		JVX_COMPONENT_ACCESS_DYNAMIC_LIB, JVX_COMPONENT_AUDIO_NODE,
		"audio_device/CayfConnectCtrl", nullptr, (passthroughMode != 0), parent, regTokenArg));
	return newInst;
}

void
CayfConnectCtrlContainer::deallocateDeviceObject(CayfComponentLib* delMe)
{
	JVX_SAFE_DELETE_OBJECT(delMe);
}

// ========================================================================================================

CayfConnectCtrl::CayfConnectCtrl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg) :
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
	mainNodeName = "ayfAuNCtrl";
};

jvxErrorType
CayfConnectCtrl::allocate_main_node()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CayfConnectCtrl::deallocate_main_node()
{
	return JVX_NO_ERROR;
}



