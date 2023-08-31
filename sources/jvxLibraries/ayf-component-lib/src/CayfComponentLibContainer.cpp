#include "CayfComponentLibContainer.h"

CayfComponentLibContainer::CayfComponentLibContainer()
{
	JVX_INITIALIZE_MUTEX(safeAccess);
};

CayfComponentLibContainer::~CayfComponentLibContainer()
{
	JVX_INITIALIZE_MUTEX(safeAccess);
};

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
	jvxApiString realRegName;
	IjvxMinHost* minHostRef = nullptr;
	IjvxConfigProcessor* confProcHdl = nullptr;

	CayfComponentLib* deviceEntryObject = allocateDeviceObject(passthroughMode);

	if (hostRef)
	{
		deviceEntryObject->initialize(hostRef);
	}
	else
	{
		if (bindRefs->ayf_register_object_host_call)
		{
			bindRefs->ayf_register_object_host_call(regName.c_str(), realRegName, deviceEntryObject, &minHostRef, &confProcHdl);
		}
		deviceEntryObject->initialize(minHostRef, confProcHdl, realRegName.std_str());
	}

	deviceEntryObject->select(nullptr);
	deviceEntryObject->activate();
	deployProcParametersStartProcessor(deviceEntryObject);

	// This function is inside the lock
	setReference(deviceEntryObject);

	return JVX_NO_ERROR;
}

jvxErrorType
CayfComponentLibContainer::stopBindingInner(IjvxHost* hostRef)
{
	ayfHostBindingReferences* bindOnReturn = nullptr;
	CayfComponentLib* deviceEntryObject = processorRef;

	// This removes the processing reference for procesing loop in a lock!!
	unsetReference();

	stopProcessor(deviceEntryObject);
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
CayfComponentLibContainer::startBinding(const std::string& regNameArg, int numInChansArg, int numOutChansArg, int bSizeArg, int sRateArg, int passthroughModeArg)
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

	if (bindRefs->bindType == ayfHostBindingType::AYF_HOST_BINDING_EMBEDDED_HOST)
	{
		if (bindRefs->ayf_register_factory_host_call)
		{
			jvxSize i;
			const char** argv = nullptr;
			int argc = bindRefs->argsFullHost.ll();
			if (argc)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(argv, const char*, argc);
				for (i = 0; i < argc; i++)
				{
					argv[i] = bindRefs->argsFullHost.c_str_at(i);
				}
			}
			bindRefs->ayf_register_factory_host_call(regName.c_str(), realRegName, this, argc, argv);
			if (argc)
			{
				JVX_DSP_SAFE_DELETE_FIELD(argv);
			}
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
	if (bindRefs->bindType == ayfHostBindingType::AYF_HOST_BINDING_EMBEDDED_HOST)
	{
		if (bindRefs->ayf_unregister_factory_host_call)
		{
			bindRefs->ayf_unregister_factory_host_call(this);
		}
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