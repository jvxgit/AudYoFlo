#ifndef __CJVXCONNECTORCOLLECTION_H__
#define __CJVXCONNECTORCOLLECTION_H__

JVX_INTERFACE CjvxConnectorCollection_bwd
{
public:
	virtual ~CjvxConnectorCollection_bwd() {};
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
JVX_INTERFACE CjvxConnectorCollection_startstop
{
	virtual ~CjvxConnectorCollection_startstop() {};
	virtual void report_proc_connector_started_in_lock(T1* connPlus) = 0;
	virtual void report_proc_connector_before_stop_in_lock(T1* connPlus) = 0;
};

template <class T1, class T2>
class CjvxConnectorCollection: public CjvxSingleConnector_report<T1>
{
public:
	T2* extra_iocon_gen = nullptr;
	std::string conName;
	std::map<T1*, oneConnectorPlusName<T1> > selectedConnectors;
	std::map<T1*, oneConnectorPlusName<T1> > processingConnectors;

	CjvxConnectorCollection_bwd* cbRef = nullptr;
	CjvxConnectorCollection_startstop<T1>* cbStartStop = nullptr;

	jvxSize lockId = JVX_SIZE_UNSELECTED;

	jvxErrorType report_stopped_connector(T1* ioconn)
	{
		// This list must be protected
		cbRef->lock(lockId);
		auto elm = processingConnectors.find(ioconn);
		assert(elm != processingConnectors.end());

		if (cbStartStop)
		{
			cbStartStop->report_proc_connector_before_stop_in_lock(ioconn);
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
			cbStartStop->report_proc_connector_started_in_lock(ioconn);
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
};

#endif
