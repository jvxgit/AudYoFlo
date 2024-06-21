#ifndef __JVXCASTPRODUCT_H__
#define __JVXCASTPRODUCT_H__

#include "helpers/HpjvxCast.h"

template <typename T>
jvxBool checkComponentType(jvxComponentType tp, jvxBool skipAssert = false)
{
	jvxBool result = false;
	switch (tp)
	{
	case JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY:
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:
	case JVX_COMPONENT_VIDEO_TECHNOLOGY:
	case JVX_COMPONENT_CUSTOM_TECHNOLOGY:
	case JVX_COMPONENT_EXTERNAL_TECHNOLOGY:
	case JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY:
#ifdef JVX_COMPONENT_CASE_TECHNOLOGY
	JVX_COMPONENT_CASE_TECHNOLOGY
#endif

		if (std::is_same<T, IjvxTechnology>::value)
		{
			result = true;
		}
		break;

	case JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE:
	case JVX_COMPONENT_AUDIO_DEVICE:
	case JVX_COMPONENT_VIDEO_DEVICE:
	case JVX_COMPONENT_CUSTOM_DEVICE:
	case JVX_COMPONENT_EXTERNAL_DEVICE:
	case JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE:

#ifdef JVX_COMPONENT_CASE_DEVICE
	JVX_COMPONENT_CASE_DEVICE
#endif
		if (std::is_same<T, IjvxDevice>::value)
		{
			result = true;
		}
		break;

	case JVX_COMPONENT_SIGNAL_PROCESSING_NODE:
	case JVX_COMPONENT_AUDIO_NODE:
	case JVX_COMPONENT_VIDEO_NODE:
	case JVX_COMPONENT_CUSTOM_NODE:
	case JVX_COMPONENT_EXTERNAL_NODE:
	case JVX_COMPONENT_APPLICATION_CONTROLLER_NODE:
	case JVX_COMPONENT_DYNAMIC_NODE:

#ifdef JVX_COMPONENT_CASE_NODE
	JVX_COMPONENT_CASE_NODE
#endif

		if (std::is_same<T, IjvxNode>::value)
		{
			result = true;
		}
		break;

	case JVX_COMPONENT_AUDIO_DECODER:
	case JVX_COMPONENT_AUDIO_ENCODER:
		if (std::is_same<T, IjvxSimpleNode>::value)
		{
			result = true;
		}
		break;

	case JVX_COMPONENT_MIN_HOST:
		if (std::is_same<T, IjvxMinHost>::value)
		{
			result = true;
		}
		break;

	case JVX_COMPONENT_FACTORY_HOST:

		if (std::is_same<T, IjvxFactoryHost>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_HOST:
		if (std::is_same<T, IjvxHost>::value)
		{
			result = true;
		}
		break;

	case JVX_COMPONENT_EVENTLOOP:
		if (std::is_same<T, IjvxEventLoop>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_DATALOGGER:
		if (std::is_same<T, IjvxDataLogger>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_DATALOGREADER:
		if (std::is_same<T, IjvxDataLogReader>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_CONNECTION:
		if (std::is_same<T, IjvxConnection>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_CONFIG_PROCESSOR:
		if (std::is_same<T, IjvxConfigProcessor>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_EXTERNAL_CALL:
		if (std::is_same<T, IjvxExternalCall>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_THREADCONTROLLER:
		if (std::is_same<T, IjvxThreadController>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_THREADS:
		if (std::is_same<T, IjvxThreads>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_SHARED_MEMORY:
		if (std::is_same<T, IjvxSharedMemory>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_TEXT2SPEECH:
		if (std::is_same<T, IjvxText2Speech>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_SYSTEM_TEXT_LOG:
		if (std::is_same<T, IjvxTextLog>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_LOCAL_TEXT_LOG:
		if (std::is_same<T, IjvxLocalTextLog>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_CRYPT:
		if (std::is_same<T, IjvxCrypt>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_WEBSERVER:
		if (std::is_same<T, IjvxWebServer>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_REMOTE_CALL:
		if (std::is_same<T, IjvxRemoteCall>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_PACKETFILTER_RULE:
		if (std::is_same<T, IjvxPacketFilterRule>::value)
		{
			result = true;
		}
		break;

		// Add all specific component types here
#ifdef JVX_COMPONENT_CAST_PRODUCT
		JVX_COMPONENT_CAST_PRODUCT
#endif
	case JVX_COMPONENT_SYSTEM_AUTOMATION:
		if (std::is_same<T, IjvxSimpleComponent>::value)
		{
			result = true;
		}
		break;
		
	default:
		if (!skipAssert)
		{
			assert(0);
		}
	}
	return result;
}

template <typename T>
struct refComp
{
	T* cpPtr = nullptr;
	IjvxObject* objPtr = nullptr;
};

template <typename T>
refComp<T> reqRefTool(IjvxToolsHost* tHost, const jvxComponentIdentification& tp = jvxComponentIdentification(), jvxSize fId = 0, const char* fTag = nullptr,
	jvxBitField filter_stateMask = (jvxBitField)JVX_STATE_DONTCARE, 
	IjvxReferenceSelector* decider = nullptr)
{
	refComp<T> inst;
	jvxComponentIdentification cpTpCopy = tp;
	if (tHost)
	{
		if (cpTpCopy.tp == JVX_COMPONENT_UNKNOWN)
		{
			for (jvxSize idCheck = JVX_COMPONENT_UNKNOWN + 1; idCheck < JVX_COMPONENT_ALL_LIMIT; idCheck++)
			{
				cpTpCopy.tp = (jvxComponentType)idCheck;
				if (checkComponentType<T>(cpTpCopy.tp, true))
				{
					break;
				}
			}
			cpTpCopy.slotid = JVX_SIZE_SLOT_OFF_SYSTEM;
			cpTpCopy.slotsubid = JVX_SIZE_SLOT_OFF_SYSTEM;
		}

		// Note: we can only check here since multiple types are linked to identical type
		if (checkComponentType<T>(cpTpCopy.tp))
		{
			tHost->reference_tool(cpTpCopy, &inst.objPtr, fId, fTag, filter_stateMask, decider);
			if (inst.objPtr)
			{
				inst.objPtr->request_specialization(reinterpret_cast<jvxHandle**>(&inst.cpPtr), nullptr, nullptr);
			}
		}
	}
	return inst;
}

template <typename T>
void retRefTool(IjvxToolsHost* tHost, refComp<T>& inst, jvxComponentType tp = JVX_COMPONENT_UNKNOWN)
{
	jvxComponentType cpTpCopy = tp;
	if (tHost)
	{
		if (cpTpCopy == JVX_COMPONENT_UNKNOWN)
		{
			for (jvxSize idCheck = JVX_COMPONENT_UNKNOWN + 1; idCheck < JVX_COMPONENT_ALL_LIMIT; idCheck++)
			{
				cpTpCopy = (jvxComponentType)idCheck;
				if (checkComponentType<T>(cpTpCopy, true))
				{
					break;
				}
			}
		}
		// Note: we can only check here since multiple types are linked to identical type
		if (checkComponentType<T>(cpTpCopy))
		{
			tHost->return_reference_tool(cpTpCopy, inst.objPtr);
			inst.objPtr = nullptr;
			inst.cpPtr = nullptr;
		}
	}
}

template <typename T>
refComp<T> reqInstTool(IjvxToolsHost* tHost, jvxComponentType tp, jvxSize fId = 0, const char* fTag = nullptr)
{
	refComp<T> inst;
	if (tHost)
	{
		// Note: we can only check here since multiple types are linked to identical type
		if (checkComponentType<T>(tp))
		{
			tHost->instance_tool(tp, &inst.objPtr, fId, fTag);
			if (inst.objPtr)
			{
				inst.objPtr->request_specialization(reinterpret_cast<jvxHandle**>(&inst.cpPtr), nullptr, nullptr);
			}
		}
	}
	return inst;
}

template <typename T>
jvxErrorType retInstTool(IjvxToolsHost* tHost, jvxComponentType tp, refComp<T>& inst, jvxSize fId = 0, const char* fTag = nullptr)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (tHost)
	{
		// Note: we can only check here since multiple types are linked to identical type
		if (checkComponentType<T>(tp))
		{
			res = tHost->return_instance_tool(tp, inst.objPtr, fId, fTag);
			inst.objPtr = nullptr;
			inst.cpPtr = nullptr;
		}
	}
	return res;
}

template <class T>
T* castPropIfExtender(IjvxPropertyExtender* pExt)
{
	T* ptrRet = nullptr;
	if (0)
	{
	}

	// From sub project
	JVX_PROPERTY_EXTENDER_CAST_CASES
	return ptrRet;
}

// =====================================================================================

// Convert an object to the main component reference
template <typename T>
T* castFromObject(IjvxObject* obj)
{
	T* retPtr = nullptr;
	jvxHandle* vPtr = nullptr;
	jvxComponentIdentification cTp = JVX_COMPONENT_UNKNOWN;
	if(obj)
	{
		obj->request_specialization(&vPtr, &cTp, nullptr);

		// Note: we can only check here since multiple types are linked to identical type
		if (checkComponentType<T>(cTp.tp))
		{
			retPtr = (T*)vPtr;
		}
	}
	return retPtr;
}

#endif
