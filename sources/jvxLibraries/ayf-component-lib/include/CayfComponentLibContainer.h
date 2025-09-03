#ifndef __CJVXCOMPONENTLIBCONTAINER_H__
#define __CJVXCOMPONENTLIBCONTAINER_H__

#include "jvx.h"
#include "CayfComponentLib.h"

#include "ayf-embedding-proxy.h"
#include "ayf-embedding-proxy-entries.h"
#include "ayf_component_lib_types.h"

class CayfComponentLibContainer: public IjvxExternalModuleFactory
{
	friend class CayfComponentLib;
private:
	JVX_MUTEX_HANDLE safeAccess;
	jvxSize numInChans = 0;
	jvxSize numOutChans = 0;
	jvxSize bSize = 0;
	jvxSize sRate = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxDataFormatGroup formGroup = JVX_DATAFORMAT_GROUP_NONE;
	int passthroughMode = 0;
	std::string regName;
	jvxSize desiredSlotIdDev = JVX_SIZE_DONTCARE;
	jvxSize desiredSlotIdNode = JVX_SIZE_DONTCARE;

	void_pvoid_callback ptr_callback_on_start = NULL;
	void* prv_callback_on_start = NULL;

public:

	CayfComponentLib* processorRef = nullptr;	
	ayfHostBindingReferencesMinHost* bindRefsMinHost = nullptr;
	ayfHostBindingReferencesEmbHost* bindRefsEmbHost = nullptr;

	CayfComponentLibContainer();
	~CayfComponentLibContainer();

	void reset();
	jvxErrorType setReference(CayfComponentLib* procArg);
	jvxErrorType unsetReference();
	void lock();
	void unlock();

	jvxErrorType startBinding(const std::string& regName, int numInChans, int numOutChans, int bSize, int sRate, int passthroughMod, int* ayfIdentsPtr = nullptr, int ayfIdentsNum = 0, 
		void_pvoid_callback ptr_callback_on_start = NULL, void* prv_callback_on_start = NULL);
	jvxErrorType stopBinding();

	jvxErrorType startBindingInner(IjvxHost* hostRef = nullptr);
	jvxErrorType stopBindingInner(IjvxHost* hostRef = nullptr);

	jvxErrorType deployProcParametersStartProcessor(CayfComponentLib* compProc);
	jvxErrorType process_one_buffer_interleaved(
		jvxData* inInterleaved, jvxSize numSamplesIn, jvxSize numChannelsIn,
		jvxData* outInterleaved, jvxSize numSamlesOut, jvxSize numChannelsOut);
	jvxErrorType stopProcessor(CayfComponentLib* compProc);
	
	virtual CayfComponentLib* allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent) = 0;
	virtual void deallocateDeviceObject(CayfComponentLib* delMe) = 0;

	//! Callback entry to allocate the objects
	jvxErrorType invite_external_components(IjvxHiddenInterface* hostRef, jvxBool inviteToJoin) override;

	void linkBinding(ayfHostBindingReferences* bindRef);
};

#endif

