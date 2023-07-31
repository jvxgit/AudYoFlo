#include "jvx.h"
#include "jvxHosts/CjvxFullHost.h"
#include <sstream>

jvxErrorType
CjvxFullHost::connection_factory_to_be_added(
	jvxComponentIdentification tp_activated,
	IjvxConnectorFactory* add_this, 
	IjvxConnectionMasterFactory* and_this)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool run_autoconnect = true;
	jvxBool firstRun = true;

	// Default case: just add
	if (add_this)
	{
		res = this->register_connection_factory(add_this);
	}
	if (and_this)
	{
		res = this->register_connection_master_factory(and_this);
	}

	/*
	JVX_OUTPUT_REPORT_DEBUG_COUT_START(_common_set_host.reportUnit)
	JVX_OUTPUT_REPORT_DEBUG_COUT_REF << JVX_OUTPUT_REPORT_DEBUG_LOCATION << " On activation of the component <" << jvxComponentIdentification_txt(tp_activated) << ">. ***" << std::flush;
	JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(_common_set_host.reportUnit);
	*/

	while (run_autoconnect)
	{
		// Apply auto connection rule
		res = _try_auto_connect(static_cast<IjvxDataConnections*>(this),
			static_cast<IjvxHost*>(this));

		run_autoconnect = false;

		// Run the custom connection callbacks
		if (res != JVX_NO_ERROR)
		{
			std::cout << "Application of auto connect rule failed, error: <" << jvxErrorType_descr(res) << ">." << std::endl;
		}

		if (firstRun)
		{
			// This must only be run once!!
			std::vector<oneInterface> ::iterator elm = jvx_findItemSelectorInList_one<oneInterface, jvxInterfaceType>(
				_common_set_host.externalInterfaces, JVX_INTERFACE_AUTO_DATA_CONNECT, 0);
			if (elm != _common_set_host.externalInterfaces.end())
			{
				res = reinterpret_cast<IjvxAutoDataConnect*>(elm->theHdl)->report_connection_factory_to_be_added(
					tp_activated,
					add_this
					/*,
					static_cast<IjvxDataConnections*>(this),
					add_this,
					and_this,
					static_cast<IjvxHost*>(this)*/);
				switch (res)
				{
				case JVX_NO_ERROR:
					break;
				case JVX_ERROR_POSTPONE:

					// Here we have the chance to run the autoconnect loop a second time
					run_autoconnect = true;
					break;
				default:
					std::cout << "External interface <IjvxAutoDataConnect> failed on function <request_new_connection_factory_to_be_added>, error: <" << jvxErrorType_descr(res) << ">." << std::endl;
				}
			}
		}
		firstRun = false;
	}

	return res;
}

jvxErrorType
CjvxFullHost::connection_factory_to_be_removed(
	jvxComponentIdentification tp_activated, IjvxConnectorFactory* rem_this, IjvxConnectionMasterFactory* and_this)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numP, numG, i;
	std::vector<jvxSize> lst_uids_remove;
	jvxSize uId = JVX_SIZE_UNSELECTED;

	// First the specific disconnect rules

#if 0
	std::vector<oneInterface> ::iterator elm = jvx_findItemSelectorInList_one<oneInterface, jvxInterfaceType>(_common_set_host.externalInterfaces, JVX_INTERFACE_AUTO_DATA_CONNECT, 0);
	if (elm != _common_set_host.externalInterfaces.end())
	{
		res = reinterpret_cast<IjvxAutoDataConnect*>(elm->theHdl)->request_new_connection_factory_to_be_removed(
			tp_activated
			/*,
			static_cast<IjvxDataConnections*>(this), 
			rem_this, 
			and_this,
			static_cast<IjvxHost*>(this)*/);
		if (res != JVX_NO_ERROR)
		{
			std::cout << "External interface <IjvxAutoDataConnect> failed on function <request_new_connection_factory_to_be_added>, error: <" << jvxErrorType_descr(res) << ">." << std::endl;
		}
	}
#endif

	// remove_connections_involved_factories();

	// Default case: just add
	if (rem_this)
	{
		res = this->unregister_connection_factory(rem_this);
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
	}
	if (and_this)
	{
		res = this->unregister_connection_master_factory(and_this);
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
	}

	std::vector<oneInterface> ::iterator elm = jvx_findItemSelectorInList_one<oneInterface, jvxInterfaceType>(_common_set_host.externalInterfaces, JVX_INTERFACE_AUTO_DATA_CONNECT, 0);
	if (elm != _common_set_host.externalInterfaces.end())
	{
		res = reinterpret_cast<IjvxAutoDataConnect*>(elm->theHdl)->report_connection_factory_removed(
			tp_activated,
			rem_this
			/*,
			static_cast<IjvxDataConnections*>(this),
			
			and_this,
			static_cast<IjvxHost*>(this)*/);
		if (res != JVX_NO_ERROR)
		{
			std::cout << "External interface <IjvxAutoDataConnect> failed on function <request_new_connection_factory_to_be_added>, error: <" << jvxErrorType_descr(res) << ">." << std::endl;
		}
	}
	return res;
}

