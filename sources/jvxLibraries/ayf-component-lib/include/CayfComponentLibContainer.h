#ifndef __CJVXCOMPONENTLIBCONTAINER_H__
#define __CJVXCOMPONENTLIBCONTAINER_H__

#include "jvx.h"
#include "CayfComponentLib.h"

#include "ayf-embedding-proxy.h"
#include "ayf-embedding-proxy-entries.h"

class CayfComponentLibContainer: public IjvxExternalModuleFactory
{
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
	jvxSize desiredSlotId = JVX_SIZE_DONTCARE;

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

	jvxErrorType startBinding(const std::string& regName, int numInChans, int numOutChans, int bSize, int sRate, int passthroughMod, jvxSize desSlotId = JVX_SIZE_DONTCARE);
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

