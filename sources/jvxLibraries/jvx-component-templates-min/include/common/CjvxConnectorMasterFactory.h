#ifndef __CJVXCONNECTORMASTERFACTORY_H__
#define __CJVXCONNECTORMASTERFACTORY_H__

#include "jvx.h"
#include <map>

class CjvxConnectionMasterFactory
{
private:

protected:
	struct oneMasterElement
	{
		IjvxConnectionMaster* theMaster;
		std::string descror;
		jvxSize refCnt;
	public:
		oneMasterElement()
		{
			theMaster = NULL;
			refCnt = 0;
		};
	} ;
	
	struct
	{
		std::map<IjvxConnectionMaster*, oneMasterElement> masters;
		jvxState theStatus;
	} _common_set_conn_master_factory;

	CjvxConnectionMasterFactory();
	
	~CjvxConnectionMasterFactory();

	jvxErrorType _activate_master_factory(IjvxObject* theRefObject);

	jvxErrorType _deactivate_master_factory();

	jvxErrorType _number_connector_masters(jvxSize* num_masters);

	jvxErrorType _reference_connector_master(jvxSize idx, IjvxConnectionMaster** ref);

	jvxErrorType _return_reference_connector_master(IjvxConnectionMaster* ref);
};

#define JVX_ACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER(obj, name) \
	{ \
		CjvxConnectionMaster::_init_master(obj, name, static_cast<IjvxConnectionMasterFactory*>(this)); \
		oneMasterElement newElm; \
		newElm.descror = "default"; \
		newElm.theMaster = static_cast<IjvxConnectionMaster*>(this); \
		_common_set_conn_master_factory.masters[newElm.theMaster] = newElm; \
		CjvxConnectionMasterFactory::_activate_master_factory(static_cast<IjvxObject*>(this)); \
	}


#define JVX_DEACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER() \
	{ \
		CjvxConnectionMasterFactory::_deactivate_master_factory(); \
		_common_set_conn_master_factory.masters.clear(); \
		CjvxConnectionMaster::_terminate_master(); \
	}


#endif
