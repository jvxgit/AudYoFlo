#include "CayfConnectStarter.h"
#include "ayfAuNStarter.h"

// ========================================================================================================

#define AYF_BINDING_MODULE_NAME "CayfConnectStarter"
#define AYF_MODULE_CONTAINER_TYPE CayfConnectStarterContainer
#include "ayf-component-entries-tpl.h"

// ========================================================================================================

CayfConnectStarterContainer::CayfConnectStarterContainer()
{
}

CayfComponentLib*
CayfConnectStarterContainer::allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regTokenArg)
{
	// Embedding is online, let us check what we need to do now
	CayfConnectStarter* newInst = nullptr;
	JVX_SAFE_ALLOCATE_OBJECT(newInst, CayfConnectStarter(
		"Audio Connect Device Object - Starter", false,
		"audio_connect_starter", 0, "ayfConnectStarter",
		JVX_COMPONENT_ACCESS_DYNAMIC_LIB, JVX_COMPONENT_AUDIO_NODE,
		"audio_device/CayfConnectStarter", nullptr, (passthroughMode != 0), parent, regTokenArg));
	return newInst;
}

void
CayfConnectStarterContainer::deallocateDeviceObject(CayfComponentLib* delMe)
{
	JVX_SAFE_DELETE_OBJECT(delMe);
}

// ========================================================================================================

CayfConnectStarter::CayfConnectStarter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg) :
	CayfComponentLib(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL, parent), passthroughMode(passthroughModeArg)
{
	/*
	modName = "CayfWeaverConnectStarter";
	regToken = modName;
	if (regTokenArg)
	{
		regToken = regTokenArg;
	}
	*/
	mainNodeName = "ayfAuNStarter";
};

jvxErrorType
CayfConnectStarter::allocate_main_node()
{
	return ayfAuNStarter_init(&this->mainObj);
}

jvxErrorType
CayfConnectStarter::deallocate_main_node()
{
	return ayfAuNStarter_terminate(this->mainObj);
}

jvxErrorType
CayfConnectStarter::on_main_node_selected()
{
	IjvxProperties* props = nullptr;
	// Add the sub components
	props = reqInterfaceObj<IjvxProperties>(this->mainNode);
	if (props)
	{
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CayfConnectStarter::before_main_node_unselect()
{
	IjvxProperties* props = nullptr;
	// Add the sub components
	props = reqInterfaceObj<IjvxProperties>(this->mainNode);
	if (props)
	{
	}
	return JVX_NO_ERROR;
}

