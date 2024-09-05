#ifndef __CJVXCONNECTORCOLLECTION_H__
#define __CJVXCONNECTORCOLLECTION_H__

JVX_INTERFACE CjvxConnectorCollection_lock
{
public:
	virtual ~CjvxConnectorCollection_lock() {};
	virtual void lock(jvxBool rwExclusive = true, jvxSize idLock = JVX_SIZE_UNSELECTED) = 0;
	virtual void unlock(jvxBool rwExclusive = true, jvxSize idLock = JVX_SIZE_UNSELECTED) = 0;
};

template <class T> class oneConnectorPlusName
{
public:
	T* ioconn = nullptr;
	std::string nmUnique;
	jvxBool inProcessing = false;
	jvxHandle* privData = nullptr;
};

template <class T1>
JVX_INTERFACE CjvxConnectorCollection_fwd_in_lock
{
public:
	virtual ~CjvxConnectorCollection_fwd_in_lock() {};
	virtual void report_started_connector_in_lock(T1* connPlus, jvxHandle* priv) = 0;
	virtual void report_before_stop_connector_in_lock(T1* connPlus, jvxHandle* priv) = 0;
};

template <class T1, class T2>
class CjvxConnectorCollection: public CjvxSingleConnector_report<T1>, public CjvxSingleConnectorTransfer_report<T1>
{
public:
	T2* extra_iocon_gen = nullptr;
	std::string conName;

	// 
	std::map<T1*, oneConnectorPlusName<T1> > selectedConnectors;
	std::map<T1*, oneConnectorPlusName<T1> > processingConnectors;

	CjvxConnectorCollection_lock* cbRef = nullptr;
	CjvxConnectorCollection_fwd_in_lock<T1>* cbStartStop = nullptr;
	IjvxUniqueId* theUniqueId = nullptr;

	jvxSize lockId = JVX_SIZE_UNSELECTED;

	// Function < report_selected_connector> typically is not implemented in this class since
	// other arguments need to be set, e.g., processing arguments etc.

	jvxErrorType report_stopped_connector(T1* ioconn)
	{
		// This list must be protected
		cbRef->lock(lockId);
		auto elm = processingConnectors.find(ioconn);
		assert(elm != processingConnectors.end());

		if (cbStartStop)
		{
			cbStartStop->report_before_stop_connector_in_lock(ioconn, elm->second.privData);
		}		
		processingConnectors.erase(elm);
		cbRef->unlock(lockId);

		auto elmS = selectedConnectors.find(ioconn);
		assert(elmS != selectedConnectors.end());
		elmS->second.inProcessing = false;

		return JVX_NO_ERROR;
	};

	jvxErrorType report_started_connector(T1* ioconn)
	{
		auto elm = selectedConnectors.find(ioconn);
		assert(elm != selectedConnectors.end());
		elm->second.inProcessing = true;

		// The list of processing connectors is to be protected as this list is operated in different threads
		cbRef->lock(lockId);

		// Copy to the list of active connections
		processingConnectors[ioconn] = elm->second;
		
		if (cbStartStop)
		{
			cbStartStop->report_started_connector_in_lock(ioconn, elm->second.privData);
		}

		cbRef->unlock(lockId);

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

	void request_unique_id_start(T1* iconn, jvxSize* uId) override 
	{
		auto elm =selectedConnectors.find(iconn);
		if (elm != selectedConnectors.end())
		{
			// Request a iniqueId from host
			if (theUniqueId)
			{
				theUniqueId->obtain_unique_id(uId, elm->second.nmUnique.c_str());
			}
		}

	}
	void release_unique_id_stop(T1* iconn, jvxSize uId) override
	{
		auto elm = selectedConnectors.find(iconn);
		if (elm != selectedConnectors.end())
		{
			// Request a iniqueId from host
			if (theUniqueId)
			{
				theUniqueId->release_unique_id(uId);
			}
		}
	}

	jvxErrorType report_transfer_connector(T1* ioconn, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
	{
		return JVX_ERROR_UNSUPPORTED;
	}
};

#endif
