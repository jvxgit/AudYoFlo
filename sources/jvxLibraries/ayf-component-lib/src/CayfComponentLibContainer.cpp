#include "CayfComponentLibContainer.h"

CayfComponentLibContainer::CayfComponentLibContainer()
{
	JVX_INITIALIZE_MUTEX(safeAccess);
};

CayfComponentLibContainer::~CayfComponentLibContainer()
{
	JVX_TERMINATE_MUTEX(safeAccess);
};

void 
CayfComponentLibContainer::linkBinding(ayfHostBindingReferences* bindRef)
{
	if (bindRef)
	{
		bindRef->request_specialization(reinterpret_cast<jvxHandle**>(&bindRefsMinHost), ayfHostBindingType::AYF_HOST_BINDING_MIN_HOST);
		bindRef->request_specialization(reinterpret_cast<jvxHandle**>(&bindRefsEmbHost), ayfHostBindingType::AYF_HOST_BINDING_EMBEDDED_HOST);
	}
}

void
CayfComponentLibContainer::reset()
{
	processorRef = nullptr;
	numInChans = 0;
	numOutChans = 0;
	bSize = 0;
	sRate = 0;
	format = JVX_DATAFORMAT_NONE;
	formGroup = JVX_DATAFORMAT_GROUP_NONE;
};

jvxErrorType
CayfComponentLibContainer::setReference(
	CayfComponentLib* procArg)
{
	JVX_LOCK_MUTEX(safeAccess);
	processorRef = procArg;
	JVX_UNLOCK_MUTEX(safeAccess);
	return JVX_NO_ERROR;
};

jvxErrorType
CayfComponentLibContainer::unsetReference()
{
	JVX_LOCK_MUTEX(safeAccess);
	processorRef = nullptr;
	JVX_UNLOCK_MUTEX(safeAccess);
	return JVX_NO_ERROR;
};

void
CayfComponentLibContainer::lock()
{
	JVX_LOCK_MUTEX(safeAccess);
};

void
CayfComponentLibContainer::unlock()
{
	JVX_UNLOCK_MUTEX(safeAccess);
};


jvxErrorType
CayfComponentLibContainer::deployProcParametersStartProcessor(CayfComponentLib* compProc)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (compProc)
	{
		res = compProc->deployProcParametersStartProcessor(numInChans,
			numOutChans,
			bSize,
			sRate,
			format,
			formGroup);
	}
	return res;
};

jvxErrorType
CayfComponentLibContainer::process_one_buffer_interleaved(
	jvxData* inInterleaved, jvxSize numSamplesIn, jvxSize numChannelsIn,
	jvxData* outInterleaved, jvxSize numSamlesOut, jvxSize numChannelsOut)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resMut = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX(resMut, safeAccess);
	if (resMut == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		if (processorRef)
		{
			res = processorRef->process_one_buffer_interleaved(
				inInterleaved, numSamplesIn, numChannelsIn,
				outInterleaved, numSamlesOut, numChannelsOut);
		}
		JVX_UNLOCK_MUTEX(safeAccess);
	}
	return res;
}

jvxErrorType
CayfComponentLibContainer::stopProcessor(CayfComponentLib* compProc)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (compProc)
	{
		res = compProc->stopProcessor();
	}
	return res;
};

jvxErrorType
CayfComponentLibContainer::startBindingInner(IjvxHost* hostRef) 
{
	jvxApiString realRegName = regName;
	IjvxMinHost* minHostRef = nullptr;
	IjvxConfigProcessor* confProcHdl = nullptr;

	CayfComponentLib* deviceEntryObject = allocateDeviceObject(passthroughMode, this);

	if (hostRef)
	{
		deviceEntryObject->initialize(hostRef);
	}
	else
	{		
		if (bindRefsMinHost)
		{
			bindRefsMinHost->ayf_register_object_host_call(regName.c_str(), realRegName, deviceEntryObject, &minHostRef, &confProcHdl);
		}
		regName = realRegName.std_str();
		deviceEntryObject->initialize(minHostRef, confProcHdl, regName);
	}

	deviceEntryObject->set_location_info(jvxComponentIdentification(JVX_COMPONENT_EXTERNAL_NODE, JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_SLOT_OFF_SYSTEM, 0));
	deviceEntryObject->select(nullptr);
	deviceEntryObject->activate();

	// Add the new component to component host grid structure
	if (hostRef)
	{
		IjvxComponentHostExt* hostExt = nullptr;
		hostExt = reqInterface<IjvxComponentHostExt>(hostRef);
		if (hostExt)
		{
			hostExt->attach_external_component(deviceEntryObject, regName.c_str(), true, true, desiredSlotIdDev);
		}
	}

	jvxErrorType res = deployProcParametersStartProcessor(deviceEntryObject);
	if (res == JVX_NO_ERROR)
	{
		if(ptr_callback_on_start)
		{ 
			ptr_callback_on_start(AYF_VOID_PVOID_ID_STARTED, prv_callback_on_start, NULL);
		}
	}


	// This function is inside the lock
	setReference(deviceEntryObject);

	return JVX_NO_ERROR;
}

jvxErrorType
CayfComponentLibContainer::stopBindingInner(IjvxHost* hostRef)
{
	ayfHostBindingReferencesMinHost* bindOnReturn = nullptr;
	CayfComponentLib* deviceEntryObject = processorRef;

	// This removes the processing reference for procesing loop in a lock!!
	unsetReference();

	stopProcessor(deviceEntryObject);

	// Add the new component to component host grid structure
	if (hostRef)
	{
		IjvxComponentHostExt* hostExt = nullptr;
		hostExt = reqInterface<IjvxComponentHostExt>(hostRef);
		if (hostExt)
		{
			hostExt->detach_external_component(deviceEntryObject, regName.c_str(), JVX_STATE_ACTIVE);
		}
	}

	deviceEntryObject->deactivate();
	deviceEntryObject->unselect();
	if (hostRef)
	{
		deviceEntryObject->terminate();
	}
	else
	{
		deviceEntryObject->terminate(bindOnReturn);
		if (bindOnReturn)
		{
			if (bindOnReturn->ayf_unregister_object_host_call)
			{
				bindOnReturn->ayf_unregister_object_host_call(deviceEntryObject);
			}
		}
	}
	this->deallocateDeviceObject(deviceEntryObject);
	return JVX_NO_ERROR;
}

jvxErrorType
CayfComponentLibContainer::startBinding(const std::string& regNameArg, int numInChansArg, int numOutChansArg, 
	int bSizeArg, int sRateArg, int passthroughModeArg, int* ayfIdentsPtr, int ayfIdentsNum, 
	void_pvoid_callback ptr_callback_bwd_on_start_arg, void* prv_callback_on_start_arg)
{
	jvxApiString realRegName;

	regName = regNameArg;
	numInChans = numInChansArg;
	numOutChans = numOutChansArg;
	bSize = bSizeArg;
	sRate = sRateArg;
	format = JVX_DATAFORMAT_DATA;
	formGroup = JVX_DATAFORMAT_GROUP_AUDIO_PCM_INTERLEAVED;
	passthroughMode = passthroughModeArg;
	desiredSlotIdNode = JVX_SIZE_DONTCARE;
	desiredSlotIdDev = JVX_SIZE_DONTCARE;

	ptr_callback_on_start = ptr_callback_bwd_on_start_arg;
	prv_callback_on_start = prv_callback_on_start_arg;

	if (ayfIdentsNum > 0)
	{
		desiredSlotIdNode = ayfIdentsPtr[0];
	}
	if (ayfIdentsNum > 1)
	{
		desiredSlotIdDev = ayfIdentsPtr[1];
	}
	if (bindRefsEmbHost)
	{
		jvxSize i;
		const char** argv = nullptr;
		int argc = bindRefsEmbHost->argsFullHost.ll();
		if (argc)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(argv, const char*, argc);
			for (i = 0; i < argc; i++)
			{
				argv[i] = bindRefsEmbHost->argsFullHost.c_str_at(i);
			}
		}
		bindRefsEmbHost->ayf_register_factory_host_call(regName.c_str(), realRegName, this, argc, argv);
		if (argc)
		{
			JVX_DSP_SAFE_DELETE_FIELD(argv);
		}
	}
	else
	{
		startBindingInner(nullptr);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CayfComponentLibContainer::stopBinding()
{
	if (bindRefsEmbHost)
	{
		bindRefsEmbHost->ayf_unregister_factory_host_call(this);
	}
	else
	{
		stopBindingInner();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CayfComponentLibContainer::invite_external_components(IjvxHiddenInterface* hostRefIf, jvxBool inviteToJoin)
{
 	IjvxHost* hostRef = reqInterface<IjvxHost>(hostRefIf);
	if (hostRef)
	{
		if (inviteToJoin)
		{
			// The start binding involves the lock internally!
			startBindingInner(hostRef);
		}
		else
		{
			// The stop binding involves the lock internally!
			stopBindingInner(hostRef);
		}
	}
	return JVX_NO_ERROR;
}