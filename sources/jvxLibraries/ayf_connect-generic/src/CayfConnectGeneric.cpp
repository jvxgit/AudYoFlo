#include "CayfConnectGeneric.h"

// ========================================================================================================

#define AYF_BINDING_MODULE_NAME "CayfConnectGeneric"
#define AYF_MODULE_CONTAINER_TYPE CayfConnectGenericContainer
#include "ayf-component-entries-tpl.h"

// ========================================================================================================

CayfConnectGenericContainer::CayfConnectGenericContainer()
{
}

CayfComponentLib*
CayfConnectGenericContainer::allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regTokenArg)
{
	// Embedding is online, let us check what we need to do now
	CayfConnectGeneric* newInst = nullptr;
	JVX_SAFE_ALLOCATE_OBJECT(newInst, CayfConnectGeneric(
		"Audio Device Object - Generic", false,
		"audio_generic", 0, "CayfConnectGeneric",
		JVX_COMPONENT_ACCESS_DYNAMIC_LIB, JVX_COMPONENT_AUDIO_NODE,
		"audio_device/CayfConnectGeneric", nullptr, (passthroughMode != 0), parent, regTokenArg));
	return newInst;
}

void
CayfConnectGenericContainer::deallocateDeviceObject(CayfComponentLib* delMe)
{
	JVX_SAFE_DELETE_OBJECT(delMe);
}

// ========================================================================================================

CayfConnectGeneric::CayfConnectGeneric(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg) :
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
	mainNodeName = "jvxGeneric";
};

jvxErrorType
CayfConnectGeneric::allocate_nodes(ayfConnectArgAllocateDeallocate& arg)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if(arg.ptr_callback_multipurpose)
	{ 
		res = arg.ptr_callback_multipurpose(ayfVoidPvoidDefinition::AYF_VOID_PVOID_ID_ALLOCATE, arg.prv_callback_multipurpose, &arg);
	}
	return res;
}

jvxErrorType
CayfConnectGeneric::deallocate_nodes(ayfConnectArgAllocateDeallocate& arg)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (arg.ptr_callback_multipurpose)
	{
		res = arg.ptr_callback_multipurpose(ayfVoidPvoidDefinition::AYF_VOID_PVOID_ID_DEALLOCATE, arg.prv_callback_multipurpose, &arg);
	}
	return res;
}



