#include "common/CjvxConnectorMasterFactory.h"

CjvxConnectionMasterFactory::CjvxConnectionMasterFactory()
	{
		_common_set_conn_master_factory.theStatus = JVX_STATE_INIT;
 	}
	
CjvxConnectionMasterFactory::~CjvxConnectionMasterFactory()
	{
	}

	jvxErrorType 
		CjvxConnectionMasterFactory::_activate_master_factory(IjvxObject* theRefObject)
	{
		if (_common_set_conn_master_factory.theStatus == JVX_STATE_INIT)
		{
			_common_set_conn_master_factory.theStatus = JVX_STATE_ACTIVE;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	jvxErrorType
		CjvxConnectionMasterFactory::_deactivate_master_factory()
	{
		if (_common_set_conn_master_factory.theStatus == JVX_STATE_ACTIVE)
		{
			_common_set_conn_master_factory.theStatus = JVX_STATE_INIT;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	jvxErrorType
		CjvxConnectionMasterFactory::_number_connector_masters(jvxSize* num_masters)
	{
		if (num_masters)
		{
			*num_masters = _common_set_conn_master_factory.masters.size();
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CjvxConnectionMasterFactory::_reference_connector_master(jvxSize idx, IjvxConnectionMaster** ref)
	{
		std::map<IjvxConnectionMaster*, oneMasterElement>::iterator elm = _common_set_conn_master_factory.masters.begin();
		std::advance(elm, idx);
		if (elm != _common_set_conn_master_factory.masters.end())
		{
			if (ref)
			{
				*ref = elm->second.theMaster;
				elm->second.refCnt++;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

	jvxErrorType
		CjvxConnectionMasterFactory::_return_reference_connector_master(IjvxConnectionMaster* ref)
	{
		std::map<IjvxConnectionMaster*, oneMasterElement>::iterator elm = _common_set_conn_master_factory.masters.find(ref);
		if (elm != _common_set_conn_master_factory.masters.end())
		{
			assert(elm->second.refCnt > 0);
			elm->second.refCnt--;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

