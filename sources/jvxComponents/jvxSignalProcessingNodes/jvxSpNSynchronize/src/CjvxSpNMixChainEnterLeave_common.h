#ifndef __CJVXSPNMIXCHAINENTERLEAVE_COMMON_H__
#define __CJVXSPNMIXCHAINENTERLEAVE_COMMON_H__

template <class T> class oneConnectorPlusName
{
public:
	T* ioconn = nullptr;
	std::string nmUnique;
	jvxBool inProcessing = false;
};

class CjvxSpNMixChainEnterLeaveCommon
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
	void lock() 
	{
		JVX_LOCK_MUTEX(safeAccessInputsOutputs);
	}

	void unlock() 
	{
		JVX_UNLOCK_MUTEX(safeAccessInputsOutputs);
	}
};

template <class T1, class T2>
class CjvxConnectorCollection: public CjvxSingleConnector_report<T1>
{
public:
	T2* extra_iocon_gen = nullptr;
	std::string conName;
	std::map<T1*, oneConnectorPlusName<T1> > selectedConnectors;
	std::map<T1*, oneConnectorPlusName<T1> > processingConnectors;

	CjvxSpNMixChainEnterLeaveCommon* cbRef = nullptr;

	jvxErrorType report_stopped_connector(T1* iconn)
	{
		// This list must be protected
		cbRef->lock();
		auto elm = processingConnectors.find(iconn);
		assert(elm != processingConnectors.end());
		processingConnectors.erase(elm);
		cbRef->unlock();

		auto elmS = selectedConnectors.find(iconn);
		assert(elmS != selectedConnectors.end());
		elmS->second.inProcessing = false;

		return JVX_NO_ERROR;
	};

	jvxErrorType report_started_connector(T1* iconn)
	{
		auto elm = selectedConnectors.find(iconn);
		assert(elm != selectedConnectors.end());
		elm->second.inProcessing = true;

		// The list of processing connectors is to be protected as this list is operated in different threads
		cbRef->lock();
		// Copy to the list of active connections
		processingConnectors[iconn] = elm->second;
		cbRef->unlock();

		return JVX_NO_ERROR;
	};

	jvxErrorType report_unselected_connector(T1* iconn)
	{
		auto elm = selectedConnectors.find(iconn);
		if (elm != selectedConnectors.end())
		{
			selectedConnectors.erase(elm);
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	};
};

#endif
