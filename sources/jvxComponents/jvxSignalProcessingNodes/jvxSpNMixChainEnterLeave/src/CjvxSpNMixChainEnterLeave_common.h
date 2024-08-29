#ifndef __CJVXSPNMIXCHAINENTERLEAVE_COMMON_H__
#define __CJVXSPNMIXCHAINENTERLEAVE_COMMON_H__

class CjvxSpNMixChainEnterLeaveCommon: public CjvxConnectorCollection_bwd
{
	JVX_MUTEX_HANDLE safeAccessInputsOutputs;

public:

	CjvxSpNMixChainEnterLeaveCommon()
	{
		JVX_INITIALIZE_MUTEX(safeAccessInputsOutputs);
	}

	~CjvxSpNMixChainEnterLeaveCommon()
	{
		JVX_TERMINATE_MUTEX(safeAccessInputsOutputs);
	}
	void lock(jvxSize idLock = JVX_SIZE_UNSELECTED) 
	{
		JVX_LOCK_MUTEX(safeAccessInputsOutputs);
	}

	void unlock(jvxSize idLock = JVX_SIZE_UNSELECTED)
	{
		JVX_UNLOCK_MUTEX(safeAccessInputsOutputs);
	}
};

#endif
