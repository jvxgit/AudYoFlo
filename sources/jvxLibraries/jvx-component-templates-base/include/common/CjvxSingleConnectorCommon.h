#ifndef __CJVXSINGLECONNECTORCOMMON_H__
#define __CJVXSINGLECONNECTORCOMMON_H__

class CjvxConnectorOffsetAndMaxChans
{
public:
	jvxSize idxOffset = 0;
	jvxSize channel_num = JVX_SIZE_UNSELECTED;
};

template <class T1, class T2> 
class CjvxConnectorMulti: public T2
{
public:
	jvxSize acceptNumberConnectors = 1;
	jvxSize numConnectorsInUse = 0;

	std::map<T1*, T2*> allocatedConnectors;

	CjvxConnectorMulti(jvxBool withTriggerConnectorArg): T2(withTriggerConnectorArg) {};
	~CjvxConnectorMulti()
	{
		assert(allocatedConnectors.size() == 0);
	};
};

template <class T1, class T2>
class CjvxConnector : public T1
{
public:
	T2* trig_con = nullptr;
	jvxBool withTriggerConnector = false;
	jvxSize conId = 0;
	CjvxConnectorOffsetAndMaxChans chanSetting;
	jvxBool linkageIoActive = false;
	
	// Physically the largest number of channels to be accepted
	jvxSize channelWidthMax = JVX_SIZE_UNSELECTED;
	jvxBool allowSingleConnect = false;

	// Find out the real master if desired
	jvxBool request_real_master = false;
	jvxComponentIdentification cpIdRealMaster;

public:
	CjvxConnector(jvxBool withTriggerConnectorArg) : withTriggerConnector(withTriggerConnectorArg)
	{
		if (withTriggerConnector)
		{
			JVX_SAFE_ALLOCATE_OBJECT(trig_con, T2);
		}
	};

	~CjvxConnector()
	{
		if (withTriggerConnector)
		{
			JVX_SAFE_DELETE_OBJECT(trig_con);
		}
	};

	jvxErrorType _transfer(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_ERROR_UNSUPPORTED;

		// This part here to be run "by default"
		jvxComponentIdentification* cpId = reinterpret_cast<jvxComponentIdentification*>(data);
		switch (tp)
		{
		case jvxLinkDataTransferType::JVX_LINKDATA_TRANSFER_REQUEST_REAL_MASTER:
			if (cpId)
			{
				cpId->reset();
			}
			if (T1::_common_set_io_common_ptr->_common_set_io_common.object)
			{
				T1::_common_set_io_common_ptr->_common_set_io_common.object->location_info(*cpId);
			}
			res = JVX_NO_ERROR;
			break;
		}		
		return res;
	};

	jvxErrorType _reference_component(jvxComponentIdentification* cpId, jvxApiString* modName, jvxApiString* description, jvxApiString* linkName)
	{
		if (T1::_common_set_io_common_ptr->_common_set_io_common.object)
		{
			T1::_common_set_io_common_ptr->_common_set_io_common.object->request_specialization(nullptr, cpId, nullptr);
			T1::_common_set_io_common_ptr->_common_set_io_common.object->module_reference(modName, nullptr);
			T1::_common_set_io_common_ptr->_common_set_io_common.object->description(description);
			if (linkName)
			{
				std::string nmCon = T1::_common_set_io_common_ptr->_common_set_io_common.descriptor + "<" + jvx_size2String(conId) + ">";
				linkName->assign(nmCon);
			}
		}
		return JVX_NO_ERROR;
	};
};

template <class T1, class T2>
class CjvxSingleTriggerConnector : public T1
{
public:
	T2* linked_ref = nullptr;
public:
	CjvxSingleTriggerConnector() {};
	// virtual jvxErrorType trigger(jvxTriggerConnectorPurpose purp, jvxHandle* data) override;

	virtual jvxErrorType link_connect_tcon(T2* iotcon) override
	{
		jvxErrorType res = JVX_ERROR_ALREADY_IN_USE;
		if (linked_ref == nullptr)
		{
			linked_ref = iotcon;
			res = JVX_NO_ERROR;
		}
		return res;
	}

	virtual jvxErrorType unlink_connect_tcon(T2* iotcon) override
	{
		jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
		if (linked_ref)
		{
			if (linked_ref == iotcon)
			{
				res = JVX_NO_ERROR;
				linked_ref = nullptr;
			}
			else
			{
				res = JVX_ERROR_ALREADY_IN_USE;
			}
		}
		return res;
	}
};

#endif
