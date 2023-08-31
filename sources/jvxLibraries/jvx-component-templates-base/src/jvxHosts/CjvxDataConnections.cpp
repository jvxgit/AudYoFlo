#include "jvxHosts/CjvxDataConnections.h"

CjvxDataConnections::CjvxDataConnections()
{
	_common_set_data_connection.unique_id = 1;
	_common_set_data_connection.theHost = NULL;
	unique_descriptor = "NONE_SPECIFIED";
}

CjvxDataConnections::~CjvxDataConnections()
{
}

void 
CjvxDataConnections::_set_system_refs(IjvxHiddenInterface* theHostRef, IjvxReport* theReportArg)
{
	_common_set_data_connection.theHost = theHostRef;
	_common_set_data_connection.theReport = theReportArg;

	// We need to put this line here for the case that the main thread is not the "startup" thread
	pending_test_requests.threadIdMainThread = JVX_GET_CURRENT_THREAD_ID();
}

jvxErrorType
CjvxDataConnections::_unique_descriptor(jvxApiString* str)
{
	if (str)
		str->assign(unique_descriptor);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::_set_unique_descriptor(const char* str)
{
	unique_descriptor = str;
	return JVX_NO_ERROR;
}

/*
jvxErrorType
CjvxDataConnections::_set_reference_report(IjvxDataConnection_report* hdlArg)
{
	_common_set_data_connection.connReport = hdlArg;
	return JVX_NO_ERROR;
}
*/

jvxErrorType
CjvxDataConnections::_remove_all_connection()
{
	while(_common_set_data_connection.connections_process.size())
	{
		CjvxDataConnectionsProcess* toRemove = NULL;
		std::map<jvxSize, oneEntrySomething<CjvxDataConnectionsProcess> >::iterator elm = _common_set_data_connection.connections_process.begin();
		for (; elm != _common_set_data_connection.connections_process.end(); elm++)
		{
			assert(elm->second.ptr);
			IjvxObject* theObj = NULL;

			elm->second.ptr->_owner(&theObj);
			if (theObj == NULL)
			{
				toRemove = elm->second.ptr;
				break;
			}
		}
		if (toRemove)
		{
			jvxBool reportGlobal = false;

			jvxSize proc_id = JVX_SIZE_UNSELECTED;
			toRemove->unique_id_connections(&proc_id);

			// This may remove entries in the list!!
			toRemove->remove_connection();
			
			// Now we have to find the entry in "new" list
			elm = _common_set_data_connection.connections_process.begin();
			for (; elm != _common_set_data_connection.connections_process.end(); elm++)
			{
				if (elm->second.ptr == toRemove)
				{
					_common_set_data_connection.connections_process.erase(elm);
					break;
				}
			}
		}
		else
		{
			// no further connection to be removed!
			break;
		}
	}

	// Connections may still be here if they are associated to an "owner"
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::_uid_for_reference(IjvxDataConnectionProcess* ref, jvxSize* uid)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionsProcess> >::iterator elm = _common_set_data_connection.connections_process.begin();
	for (; elm != _common_set_data_connection.connections_process.end(); elm++)
	{
		if (static_cast<IjvxDataConnectionProcess*>(elm->second.ptr) == ref)
		{
			if (uid)
				*uid = elm->second.uId;
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxDataConnections::_number_connections_process(jvxSize* num)
{
	if (num)
	{
		*num = _common_set_data_connection.connections_process.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::_reference_connection_process(jvxSize idx, IjvxDataConnectionProcess** theProc)
{
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionsProcess> >::iterator elm = _common_set_data_connection.connections_process.begin();
	std::advance(elm, idx);
	if (elm != _common_set_data_connection.connections_process.end())
	{
		if (theProc)
		{
			*theProc = static_cast<IjvxDataConnectionProcess*>(elm->second.ptr);
			elm->second.refCnt++;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_reference_connection_process_uid(jvxSize uid, IjvxDataConnectionProcess** theProc)
{
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionsProcess> >::iterator elm = _common_set_data_connection.connections_process.find(uid);
	if (elm != _common_set_data_connection.connections_process.end())
	{
		if (theProc)
		{
			*theProc = static_cast<IjvxDataConnectionProcess*>(elm->second.ptr);
			elm->second.refCnt++;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_return_reference_connection_process(IjvxDataConnectionProcess* theProc)
{
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionsProcess> >::iterator elm = _common_set_data_connection.connections_process.begin();
	for (; elm != _common_set_data_connection.connections_process.end(); elm++)
	{
		if (static_cast<IjvxDataConnectionProcess*>(elm->second.ptr) == theProc)
		{
			break;
		}
	}
	if (elm != _common_set_data_connection.connections_process.end())
	{
		assert(elm->second.refCnt > 0);
		elm->second.refCnt--;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_create_connection_process(
	const char* nm, 
	jvxSize* unique_id, 
	jvxBool interceptors, 
	jvxBool essential_for_start,
	jvxBool verbose_out,
	jvxBool report_global,
	jvxSize idProcDepends)
{
	/* How can we set the owner? Typically, the owner is set in associate_master since
	 * this is the moment at which "select" is called in the process */
	jvxBool alreadyRegistered = false;
	std::string nmLoc;
	jvxApiString txt;

	oneEntrySomething<CjvxDataConnectionsProcess> newElm;
	newElm.uId = _common_set_data_connection.unique_id;
	nmLoc = "conn_proc#" + jvx_size2String(newElm.uId);
	if (nm)
		nmLoc = nm;

	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionsProcess> >::iterator elm = _common_set_data_connection.connections_process.begin();
	for (; elm != _common_set_data_connection.connections_process.end(); elm++)
	{
		elm->second.ptr->descriptor_connection(&txt);
		if (txt.std_str() == nmLoc)
		{
			alreadyRegistered = true;
			break;
		}
	}

	if (!alreadyRegistered)
	{
		newElm.ptr = NULL;
		newElm.uId = _common_set_data_connection.unique_id++;

		JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm.ptr, 
			CjvxDataConnectionsProcess(
				this,
				nmLoc, 
				newElm.uId, 
				interceptors, 
				essential_for_start, 
				verbose_out,
				report_global,
				idProcDepends));
		newElm.refCnt = 0;

		// Set host reference
		newElm.ptr->_initialize(_common_set_data_connection.theHost);

		jvxSize slotid = 0;
		while (1)
		{
			auto elmc = _common_set_data_connection.connections_process.begin();
			jvxBool fdit = false;
			for (; elmc != _common_set_data_connection.connections_process.end(); elmc++)
			{
				jvxComponentIdentification tp;
				elmc->second.ptr->request_specialization(NULL, &tp, NULL, NULL);
				if (tp.slotid == slotid)
				{
					fdit = true;
					break;
				}
			}
			if (fdit == false)
			{
				break;
			}
			else
			{
				slotid++;
			}
		}

		// Set the slot id
		newElm.ptr->set_location_info(jvxComponentIdentification(slotid, 0, JVX_SIZE_UNSELECTED));

		_common_set_data_connection.connections_process[newElm.uId] = newElm;
		_common_set_data_connection.unique_id++;
		
		if (unique_id)
		{
			*unique_id = newElm.uId;
		}

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxDataConnections::_remove_connection_process(jvxSize unique_id)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionsProcess> >::iterator elm = _common_set_data_connection.connections_process.find(unique_id);
	if (elm != _common_set_data_connection.connections_process.end())
	{
		if (elm->second.refCnt != 0)
		{
			return JVX_ERROR_COMPONENT_BUSY;
		}

		jvxApiString astr;
		elm->second.ptr->description(&astr);
		std::cout << __FUNCTION__ << ": Removing connection <" << astr.std_str() << ">." << std::endl;

		jvxSize proc_id = JVX_SIZE_UNSELECTED;
		elm->second.ptr->unique_id_connections(&proc_id);

		jvxBool reportGlobal = false;
		res = elm->second.ptr->remove_connection();
		if (res == JVX_NO_ERROR)
		{
			elm->second.ptr->deassociate_master();
			elm->second.ptr->_terminate();
			JVX_DSP_SAFE_DELETE_OBJECT(elm->second.ptr);
			_common_set_data_connection.connections_process.erase(elm);
		}
		return res;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

// ========================================================================================
// ========================================================================================

jvxErrorType
CjvxDataConnections::_uid_for_reference(IjvxDataConnectionCommon* ref, jvxSize* uid, jvxBool* isProcess)
{
	auto elmG = _common_set_data_connection.connections_group.begin();
	for (; elmG != _common_set_data_connection.connections_group.end(); elmG++)
	{
		if (static_cast<IjvxDataConnectionCommon*>(elmG->second.ptr) == ref)
		{
			if (isProcess)
				*isProcess = false;
			if (uid)
				*uid = elmG->second.uId;
			return JVX_NO_ERROR;
		}
	}
	auto elmP = _common_set_data_connection.connections_process.begin();
	for (; elmP != _common_set_data_connection.connections_process.end(); elmP++)
	{
		if (static_cast<IjvxDataConnectionCommon*>(elmP->second.ptr) == ref)
		{
			if (isProcess)
				*isProcess = true;
			if (uid)
				*uid = elmP->second.uId;
			return JVX_NO_ERROR;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxDataConnections::_uid_for_reference(IjvxDataConnectionGroup* ref, jvxSize* uid)
{
	jvxErrorType res = JVX_NO_ERROR;
	auto elm = _common_set_data_connection.connections_group.begin();
	for (; elm != _common_set_data_connection.connections_group.end(); elm++)
	{
		if (static_cast<IjvxDataConnectionGroup*>(elm->second.ptr) == ref)
		{
			if (uid)
				*uid = elm->second.uId;
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxDataConnections::_number_connections_group(jvxSize* num)
{
	if (num)
	{
		*num = _common_set_data_connection.connections_group.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::_reference_connection_group(jvxSize idx, IjvxDataConnectionGroup** theProc)
{
	auto elm = _common_set_data_connection.connections_group.begin();
	std::advance(elm, idx);
	if (elm != _common_set_data_connection.connections_group.end())
	{
		if (theProc)
		{
			*theProc = static_cast<IjvxDataConnectionGroup*>(elm->second.ptr);
			elm->second.refCnt++;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_reference_connection_group_uid(jvxSize uid, IjvxDataConnectionGroup** theProc)
{
	auto elm = _common_set_data_connection.connections_group.find(uid);
	if (elm != _common_set_data_connection.connections_group.end())
	{
		if (theProc)
		{
			*theProc = static_cast<IjvxDataConnectionGroup*>(elm->second.ptr);
			elm->second.refCnt++;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_return_reference_connection_group(IjvxDataConnectionGroup* theProc)
{
	auto elm = _common_set_data_connection.connections_group.begin();
	for (; elm != _common_set_data_connection.connections_group.end(); elm++)
	{
		if (static_cast<IjvxDataConnectionGroup*>(elm->second.ptr) == theProc)
		{
			break;
		}
	}
	if (elm != _common_set_data_connection.connections_group.end())
	{
		assert(elm->second.refCnt > 0);
		elm->second.refCnt--;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_create_connection_group(
	const char* nm, 
	jvxSize* unique_id, 
	const std::string& descr,
	jvxBool verbose_out)
{
	jvxBool alreadyRegistered = false;
	std::string nmLoc;
	jvxApiString txt;

	oneEntrySomething<CjvxDataConnectionsGroup> newElm;
	newElm.uId = _common_set_data_connection.unique_id;
	nmLoc = "conn_group#" + jvx_size2String(newElm.uId);
	if (nm)
		nmLoc = nm;

	auto elm = _common_set_data_connection.connections_group.begin();
	for (; elm != _common_set_data_connection.connections_group.end(); elm++)
	{
		elm->second.ptr->descriptor_connection(&txt);
		if (txt.std_str() == nmLoc)
		{
			alreadyRegistered = true;
			break;
		}
	}

	if (!alreadyRegistered)
	{
		newElm.ptr = NULL;
		newElm.uId = _common_set_data_connection.unique_id++;

		JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm.ptr,
			CjvxDataConnectionsGroup(
				this,
				nmLoc, 
				newElm.uId, 
				descr, 
				verbose_out));
		newElm.refCnt = 0;

		// Set host reference
		newElm.ptr->_initialize(_common_set_data_connection.theHost);

		jvxSize slotid = 0;
		while (1)
		{
			auto elmc = _common_set_data_connection.connections_group.begin();
			jvxBool fdit = false;
			for (; elmc != _common_set_data_connection.connections_group.end(); elmc++)
			{
				jvxComponentIdentification tp;
				elmc->second.ptr->request_specialization(NULL, &tp, NULL, NULL);
				if (tp.slotid == slotid)
				{
					fdit = true;
					break;
				}
			}
			if (fdit == false)
			{
				break;
			}
			else
			{
				slotid++;
			}
		}

		// Set the slot id
		newElm.ptr->set_location_info(jvxComponentIdentification(slotid, 0, JVX_SIZE_UNSELECTED));

		_common_set_data_connection.connections_group[newElm.uId] = newElm;
		_common_set_data_connection.unique_id++;

		if (unique_id)
		{
			*unique_id = newElm.uId;
		}

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxDataConnections::_remove_connection_group(jvxSize unique_id)
{
	auto elm = _common_set_data_connection.connections_group.find(unique_id);
	if (elm != _common_set_data_connection.connections_group.end())
	{
		if (elm->second.refCnt != 0)
		{
			return JVX_ERROR_COMPONENT_BUSY;
		}

		// Removal of connection group will not be reported!
		elm->second.ptr->remove_connection();

		elm->second.ptr->_terminate();

		JVX_DSP_SAFE_DELETE_OBJECT(elm->second.ptr);
		_common_set_data_connection.connections_group.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

// ========================================================================================
// ========================================================================================

jvxErrorType
CjvxDataConnections::_register_connection_factory(IjvxConnectorFactory* theFac)
{
	jvxBool alreadyRegistered = false;
	oneEntrySomething<IjvxConnectorFactory>  newElm;
	std::map<IjvxConnectorFactory*, oneEntrySomething<IjvxConnectorFactory> >::iterator elm = _common_set_data_connection.connection_factories.begin();
	for (; elm != _common_set_data_connection.connection_factories.end(); elm++)
	{
		if (elm->second.ptr == theFac)
		{
			alreadyRegistered = true;
			break;
		}
	}

	if (!alreadyRegistered)
	{
		newElm.ptr = theFac;
		newElm.refCnt = 0;
		newElm.uId = _common_set_data_connection.unique_id++;

		_common_set_data_connection.connection_factories[theFac] = newElm;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxDataConnections::_unregister_connection_factory(IjvxConnectorFactory* theFac)
{
	jvxErrorType res = remove_connections_involved_factories(theFac, NULL);
	if (res == JVX_NO_ERROR)
	{
		std::map<IjvxConnectorFactory*, oneEntrySomething<IjvxConnectorFactory> >::iterator elm = _common_set_data_connection.connection_factories.find(theFac);
		if (elm == _common_set_data_connection.connection_factories.end())
			return JVX_ERROR_ELEMENT_NOT_FOUND;

		_common_set_data_connection.connection_factories.erase(elm);
	}
	return res;
}

jvxErrorType
CjvxDataConnections::_number_connection_factories(jvxSize* num)
{
	if (num)
	{
		*num = _common_set_data_connection.connection_factories.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::_reference_connection_factory(jvxSize idx, IjvxConnectorFactory** theFac, jvxSize* unique_id)
{
	std::map<IjvxConnectorFactory*, oneEntrySomething<IjvxConnectorFactory> >::iterator elm = _common_set_data_connection.connection_factories.begin();
	std::advance(elm, idx);
	if (elm != _common_set_data_connection.connection_factories.end())
	{
		if (theFac)
		{
			*theFac = elm->second.ptr;
			elm->second.refCnt++;
		}
		if (unique_id)
		{
			*unique_id = elm->second.uId;
		}

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_reference_connection_factory_uid(jvxSize unique_id, IjvxConnectorFactory** theFac)
{
	std::map<IjvxConnectorFactory*, oneEntrySomething<IjvxConnectorFactory> >::iterator elm = _common_set_data_connection.connection_factories.begin();
	for (; elm != _common_set_data_connection.connection_factories.end(); elm++)
	{
		if (elm->second.uId == unique_id)
		{
			if (theFac)
			{
				*theFac = elm->second.ptr;
				elm->second.refCnt++;
			}

			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxDataConnections::_return_reference_connection_factory(IjvxConnectorFactory* theFac)
{
	std::map<IjvxConnectorFactory*, oneEntrySomething<IjvxConnectorFactory> >::iterator elm = _common_set_data_connection.connection_factories.begin();
	for (; elm != _common_set_data_connection.connection_factories.end(); elm++)
	{
		if (elm->second.ptr == theFac)
		{
			break;
		}
	}
	if (elm != _common_set_data_connection.connection_factories.end())
	{
		assert(elm->second.refCnt > 0);
		elm->second.refCnt--;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

// ========================================================================================

jvxErrorType
CjvxDataConnections::_register_connection_master_factory(IjvxConnectionMasterFactory* theFac)
{
	jvxBool alreadyRegistered = false;
	oneEntrySomething<IjvxConnectionMasterFactory>  newElm;
	std::map<IjvxConnectionMasterFactory*, oneEntrySomething<IjvxConnectionMasterFactory> >::iterator elm = _common_set_data_connection.connection_master_factories.begin();
	for (; elm != _common_set_data_connection.connection_master_factories.end(); elm++)
	{
		if (elm->second.ptr == theFac)
		{
			alreadyRegistered = true;
			break;
		}
	}

	if (!alreadyRegistered)
	{
		newElm.ptr = theFac;
		newElm.refCnt = 0;
		newElm.uId = _common_set_data_connection.unique_id++;

		_common_set_data_connection.connection_master_factories[theFac] = newElm;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxDataConnections::_unregister_connection_master_factory(IjvxConnectionMasterFactory* theFac)
{
	jvxErrorType res = remove_connections_involved_factories(NULL, theFac);
	if (res == JVX_NO_ERROR)
	{
		std::map<IjvxConnectionMasterFactory*, oneEntrySomething<IjvxConnectionMasterFactory> >::iterator elm = _common_set_data_connection.connection_master_factories.find(theFac);
		if (elm == _common_set_data_connection.connection_master_factories.end())
			return JVX_ERROR_ELEMENT_NOT_FOUND;

		_common_set_data_connection.connection_master_factories.erase(elm);
	}
	return res;
}

jvxErrorType
CjvxDataConnections::_number_connection_master_factories(jvxSize* num)
{
	if (num)
	{
		*num = _common_set_data_connection.connection_master_factories.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::_reference_connection_master_factory(jvxSize idx, IjvxConnectionMasterFactory** theFac, jvxSize* unique_id)
{
	std::map<IjvxConnectionMasterFactory*, oneEntrySomething<IjvxConnectionMasterFactory> >::iterator elm = _common_set_data_connection.connection_master_factories.begin();
	std::advance(elm, idx);
	if (elm != _common_set_data_connection.connection_master_factories.end())
	{
		if (theFac)
		{
			*theFac = elm->second.ptr;
			elm->second.refCnt++;
		}
		if (unique_id)
		{
			*unique_id = elm->second.uId;
		}

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_reference_connection_master_factory_uid(jvxSize unique_id, IjvxConnectionMasterFactory** theFac)
{
	std::map<IjvxConnectionMasterFactory*, oneEntrySomething<IjvxConnectionMasterFactory> >::iterator elm =
		_common_set_data_connection.connection_master_factories.begin();
	for (; elm != _common_set_data_connection.connection_master_factories.end(); elm++)
	{
		if (elm->second.uId == unique_id)
		{
			if (theFac)
			{
				*theFac = elm->second.ptr;
				elm->second.refCnt++;
			}

			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxDataConnections::_return_reference_connection_master_factory(IjvxConnectionMasterFactory* theFac)
{
	std::map<IjvxConnectionMasterFactory*, oneEntrySomething<IjvxConnectionMasterFactory> >::iterator elm = _common_set_data_connection.connection_master_factories.begin();
	for (; elm != _common_set_data_connection.connection_master_factories.end(); elm++)
	{
		if (elm->second.ptr == theFac)
		{
			break;
		}
	}
	if (elm != _common_set_data_connection.connection_master_factories.end())
	{
		assert(elm->second.refCnt > 0);
		elm->second.refCnt--;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

// ============================================================================

jvxErrorType
CjvxDataConnections::_number_connection_rules(jvxSize* num)
{
	if (num)
	{
		*num = _common_set_data_connection.connection_rules.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::_reference_connection_rule(jvxSize idx, IjvxDataConnectionRule** rule_on_return)
{
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionRule> >::iterator elm = _common_set_data_connection.connection_rules.begin();
	std::advance(elm, idx);
	if (elm != _common_set_data_connection.connection_rules.end())
	{
		if (rule_on_return)
		{
			*rule_on_return = static_cast<IjvxDataConnectionRule*>(elm->second.ptr);
			elm->second.refCnt++;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_reference_connection_rule_uid(jvxSize uId, IjvxDataConnectionRule** rule_on_return)
{
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionRule> >::iterator elm = _common_set_data_connection.connection_rules.find(uId);
	if (elm != _common_set_data_connection.connection_rules.end())
	{
		if (rule_on_return)
		{
			*rule_on_return = static_cast<IjvxDataConnectionRule*>(elm->second.ptr);
			elm->second.refCnt++;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_uid_for_reference(IjvxDataConnectionRule* ref, jvxSize* uid)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionRule> >::iterator elm = _common_set_data_connection.connection_rules.begin();
	for (; elm != _common_set_data_connection.connection_rules.end(); elm++)
	{
		if (static_cast<IjvxDataConnectionRule*>(elm->second.ptr) == ref)
		{
			if (uid)
				*uid = elm->second.uId;
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxDataConnections::_return_reference_connection_rule(IjvxDataConnectionRule* rule_to_return)
{
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionRule> >::iterator elm = _common_set_data_connection.connection_rules.begin();
	for (; elm != _common_set_data_connection.connection_rules.end(); elm++)
	{
		if (static_cast<IjvxDataConnectionRule*>(elm->second.ptr) == rule_to_return)
		{
			break;
		}
	}
	if (elm != _common_set_data_connection.connection_rules.end())
	{
		assert(elm->second.refCnt > 0);
		elm->second.refCnt--;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnections::_create_connection_rule(
	const char* rule_name, 
	jvxSize* uId, 
	jvxDataConnectionRuleParameters* params,
	jvxSize catId
	/* jvxBool interceptors, jvxBool essential, jvxBool connect_process, jvxBool dbg_output, const char* group_descr*/)
{
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionRule> >::iterator elm = _common_set_data_connection.connection_rules.begin();
	for (; elm != _common_set_data_connection.connection_rules.end(); elm++)
	{
		jvxApiString str;
		elm->second.ptr->description_rule(&str, NULL);
		if (str.std_str() == rule_name)
		{
			break;
		}
	}
	if (elm == _common_set_data_connection.connection_rules.end())
	{
		CjvxDataConnectionRule* newRule = NULL;
		oneEntrySomething<CjvxDataConnectionRule> newElm;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(newRule, CjvxDataConnectionRule(params, catId /*interceptors, essential, connect_process, dbg_output*/));
		newRule->set_name(rule_name);
		newRule->add_descr(params->descr_object);
		
		newElm.ptr = newRule;
		newElm.refCnt = 0;
		newElm.uId = _common_set_data_connection.unique_id++;

		_common_set_data_connection.connection_rules[newElm.uId] = newElm;
		if (uId)
			*uId = newElm.uId;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType
CjvxDataConnections::_remove_connection_rule(jvxSize uId)
{
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionRule> >::iterator elm = _common_set_data_connection.connection_rules.begin();
	for (; elm != _common_set_data_connection.connection_rules.end(); elm++)
	{
		jvxApiString str;
		if (elm->second.uId == uId)
		{
			break;
		}
	}
	if (elm == _common_set_data_connection.connection_rules.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	delete elm->second.ptr;
	_common_set_data_connection.connection_rules.erase(elm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::_remove_all_connection_rules()
{
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionRule> >::iterator elm = _common_set_data_connection.connection_rules.begin();
	for (; elm != _common_set_data_connection.connection_rules.end(); elm++)
	{
		delete elm->second.ptr;
	}
	_common_set_data_connection.connection_rules.clear();
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::_add_process_test(jvxSize uIdProcess, jvxSize* numTested, jvxBool immediatetest)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool triggerIt = false;
	JVX_THREAD_ID tId = JVX_GET_CURRENT_THREAD_ID();
	JVX_LOCK_MUTEX(pending_test_requests.lock);

	// Add the element to the queue
	if (JVX_CHECK_SIZE_UNSELECTED(uIdProcess))
	{
		jvxSize num = 0;
		this->_number_connections_process(&num);
		for (i = 0; i < num; i++)
		{
			IjvxDataConnectionProcess* ptr = nullptr;
			this->_reference_connection_process(i, &ptr);
			if (ptr)
			{
				jvxSize uIdLoc = JVX_SIZE_UNSELECTED;
				this->_uid_for_reference(ptr, &uIdLoc);
				if (JVX_CHECK_SIZE_UNSELECTED(uIdLoc))
				{
					auto elm = std::find(pending_test_requests.pendingUids.begin(), pending_test_requests.pendingUids.end(), uIdLoc);
					if (elm == pending_test_requests.pendingUids.end())
					{
						pending_test_requests.pendingUids.push_back(uIdLoc);
					}
				}
				this->_return_reference_connection_process(ptr);
			}
		}
	}
	else
	{
		auto elm = std::find(pending_test_requests.pendingUids.begin(), pending_test_requests.pendingUids.end(), uIdProcess);
		if (elm == pending_test_requests.pendingUids.end())
		{
			pending_test_requests.pendingUids.push_back(uIdProcess);
		}
	}

	if (tId == pending_test_requests.threadIdMainThread)
	{
		// If we are in the middle of a "test" the new test request is in the queue and will be processed directly after this test
		if (!pending_test_requests.inCall)
		{
			// We may run a test immediately
			if (immediatetest)
			{
				triggerIt = true;
			}
			else
			{
				// this tells the caller to queue the next test run. It is better to re-schedule the next test
				res = JVX_ERROR_POSTPONE;
			}
		}
	}
	else
	{
		// this tells the caller to queue the next test run since we come from a different thread
		if (!immediatetest)
		{
			res = JVX_ERROR_POSTPONE;
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
	}

	JVX_UNLOCK_MUTEX(pending_test_requests.lock);

	if (triggerIt)
	{
		this->_trigger_process_test_all(numTested);
	}

	return res;
}

jvxErrorType
CjvxDataConnections::_trigger_process_test_all(jvxSize* numTested)
{
	JVX_THREAD_ID tId = JVX_GET_CURRENT_THREAD_ID();
	assert(tId == pending_test_requests.threadIdMainThread);
	jvxBool runLoop = true;
	jvxSize cnt = 0;
	while (runLoop)
	{
		jvxSize uIdProcess = JVX_SIZE_UNSELECTED;
		JVX_LOCK_MUTEX(pending_test_requests.lock);
		auto elm = pending_test_requests.pendingUids.begin();
		if (elm != pending_test_requests.pendingUids.end())
		{
			uIdProcess = *elm;
			pending_test_requests.pendingUids.pop_front();
		}
		JVX_UNLOCK_MUTEX(pending_test_requests.lock);

		if (JVX_CHECK_SIZE_SELECTED(uIdProcess))
		{
			pending_test_requests.inCall = true;
			IjvxDataConnectionProcess* theRef = nullptr;
			_reference_connection_process_uid(uIdProcess, &theRef);
			if (theRef)
			{
				jvxBool fwdTest = false;
				jvxSize uIdDep = JVX_SIZE_UNSELECTED;
				theRef->get_forward_test_depend(&fwdTest);
				theRef->depends_on_process(&uIdDep);

				if (fwdTest && JVX_CHECK_SIZE_SELECTED(uIdDep))
				{
					// Map the request to the dependent chain!
					JVX_LOCK_MUTEX(pending_test_requests.lock);
					auto elm = pending_test_requests.pendingUids.begin();
					if (elm == pending_test_requests.pendingUids.end())
					{
						pending_test_requests.pendingUids.push_back(uIdDep);
					}
					JVX_UNLOCK_MUTEX(pending_test_requests.lock);
				}
				else
				{
					jvxApiString nmChain;
					// Run test on chain, the result is stored internally and will be shown in next "updateWindow"
					jvxErrorType resL = theRef->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
					cnt++;
				}
				_return_reference_connection_process(theRef);
			}
			pending_test_requests.inCall = false;
		}
		else
		{
			// No more pending events, leave!
			runLoop = false;
		}
	}

	// Forward output
	if (numTested)
	{
		*numTested = cnt;
	}

	return JVX_NO_ERROR;
}

// =============================================================================================
jvxErrorType
CjvxDataConnections::_try_auto_connect(IjvxDataConnections* allConnections, IjvxHost* theHost)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool connection_established = true;
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionRule> >::iterator elm = _common_set_data_connection.connection_rules.begin();
	while (connection_established)
	{
		connection_established = false;
		for (; elm != _common_set_data_connection.connection_rules.end(); elm++)
		{
			jvxSize proc_id = JVX_SIZE_UNSELECTED;
			jvxBool reportit = false;
			res = elm->second.ptr->try_connect_direct(allConnections, theHost, &connection_established, 
				_common_set_data_connection.theReport, &proc_id,&reportit, elm->first);
			if (connection_established)
			{	
				if (reportit)
				{
					/*
					if (_common_set_data_connection.theReport)
					{
						CjvxReportCommandRequest_uid command(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_ESTABLISHED,
							JVX_COMPONENT_UNKNOWN, proc_id, jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);
						_common_set_data_connection.theReport->request_command(&command);
					}
					*/
				}
				break;
			}
		}
	}	
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::get_configuration_dataproc(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* theWriter, jvxConfigData* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize cnt = 0;
	jvxConfigData* datProcesses = NULL;
	jvxConfigData* datGroups = NULL;
	jvxConfigData* datProcess = NULL;
	jvxConfigData* datGroup = NULL;
	jvxConfigData* commentStr = NULL;
	jvxApiString nmProc;
	jvxSize ruleId = JVX_SIZE_UNSELECTED;
	// jvxBool fromAutoSetup = false;
	jvxBool preventStoredInConfig = false;

	theWriter->createEmptySection(&datGroups, "group_connect_section");

	// ===================================================================================

	cnt = 0;
	auto elmG = _common_set_data_connection.connections_group.begin();
	for (; elmG != _common_set_data_connection.connections_group.end(); elmG++)
	{
		elmG->second.ptr->descriptor_connection(&nmProc);
		elmG->second.ptr->misc_connection_parameters(&ruleId, &preventStoredInConfig);
		theWriter->createEmptySection(&datGroup, ("JVX_ACTIVE_GROUP_" + jvx_size2String(cnt)).c_str());

		if (datGroup)
		{
			res = elmG->second.ptr->get_configuration(callConf, theWriter, datGroup);
			assert(res == JVX_NO_ERROR);
			theWriter->addSubsectionToSection(datGroups, datGroup);
		}
		cnt++;
	}
	theWriter->addSubsectionToSection(sectionWhereToAddAllSubsections, datGroups);

	// ===================================================================================
	// ===================================================================================

	theWriter->createEmptySection(&datProcesses, "process_connect_section");
	cnt = 0;
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionsProcess> >::iterator elmP = _common_set_data_connection.connections_process.begin();
	for (; elmP != _common_set_data_connection.connections_process.end(); elmP++)
	{
		elmP->second.ptr->descriptor_connection(&nmProc);
		elmP->second.ptr->misc_connection_parameters(&ruleId, & preventStoredInConfig);
		if (JVX_CHECK_SIZE_SELECTED(ruleId))
		{
			theWriter->createComment(&commentStr, (" Auto connected connection process with name <" + nmProc.std_str() + "> and rule Id <" + 
				jvx_size2String(ruleId) + ".").c_str());
			theWriter->addSubsectionToSection(datProcesses, commentStr);
		}
		else
		{
			if (preventStoredInConfig)
			{
				theWriter->createComment(&commentStr, (" Connection process with name <" + nmProc.std_str() + "> is not intended to be stored in config file due to <preventStoredInConfig> flag.").c_str());
				theWriter->addSubsectionToSection(datProcesses, commentStr);
			}
			else
			{
				theWriter->createEmptySection(&datProcess, ("JVX_ACTIVE_PROCESS_" + jvx_size2String(cnt)).c_str());
				if (datProcess)
				{
					res = elmP->second.ptr->get_configuration(callConf, theWriter, datProcess);
					assert(res == JVX_NO_ERROR);
					theWriter->addSubsectionToSection(datProcesses, datProcess);
				}
				cnt++;
			}
		}
	}
	theWriter->addSubsectionToSection(sectionWhereToAddAllSubsections, datProcesses);
	return JVX_NO_ERROR;
};

jvxErrorType
CjvxDataConnections::get_configuration_dataproc_rules(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theWriter, jvxConfigData* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize cnt = 0;
	jvxConfigData* datProcesses = NULL;
	jvxConfigData* datProcess = NULL;
	jvxConfigData* commentStr = NULL;
	jvxBool isDefault = false;
	jvxApiString nmRule;

	theWriter->createEmptySection(&datProcesses, "process_connect_rule_section");

	// ===================================================================================
	cnt = 0;
	std::map<jvxSize, oneEntrySomething<CjvxDataConnectionRule> >::iterator elmR = _common_set_data_connection.connection_rules.begin();
	for (; elmR != _common_set_data_connection.connection_rules.end(); elmR++)
	{
		elmR->second.ptr->description_rule(&nmRule, &isDefault);
		if (isDefault)
		{
			theWriter->createComment(&commentStr, (" Default connection rule with name <" + nmRule.std_str() + ">").c_str());
			theWriter->addSubsectionToSection(datProcesses, commentStr);
		}
		else
		{
			theWriter->createEmptySection(&datProcess, ("JVX_CONNECTION_RULE_" + jvx_size2String(cnt)).c_str());
			if (datProcess)
			{
				res = elmR->second.ptr->get_configuration(theWriter, datProcess, 0);
				assert(res == JVX_NO_ERROR);
				theWriter->addSubsectionToSection(datProcesses, datProcess);
			}
			cnt++;
		}
	}

	theWriter->addSubsectionToSection(sectionWhereToAddAllSubsections, datProcesses);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnections::remove_connections_involved_factories(
	IjvxConnectorFactory* rem_this, 
	IjvxConnectionMasterFactory* and_this)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numP, numG, i;
	std::vector<jvxSize> lst_uids_remove;
	jvxSize uId;
	// Then, remove all rules which involve any of the deactivated parts
	res = _number_connections_group(&numG);
	for (i = 0; i < numG; i++)
	{
		IjvxDataConnectionGroup* theGroup = NULL;
		_reference_connection_group(i, &theGroup);
		if (
			(rem_this && (theGroup->connector_factory_is_involved(rem_this) == JVX_NO_ERROR)))
		{
			uId = JVX_SIZE_UNSELECTED;

			jvxApiString astr;
			theGroup->description(&astr);
			std::cout << __FUNCTION__ << ": Removing connection group <" << astr.std_str() << ">." << std::endl;

			theGroup->remove_connection();
			_uid_for_reference(theGroup, &uId);
			assert(JVX_CHECK_SIZE_SELECTED(uId));
			lst_uids_remove.push_back(uId);
		}
		_return_reference_connection_group(theGroup);
	}
	for (i = 0; i < lst_uids_remove.size(); i++)
	{
		res = _remove_connection_group(lst_uids_remove[i]);
		assert(res == JVX_NO_ERROR);
	}
	lst_uids_remove.clear();

	jvxBool moreToRemove = true;

	// Then, remove all rules which involve any of the deactivated parts
	while (moreToRemove)
	{
		jvxBool onePlusRemoved = false;
		res = _number_connections_process(&numP);
		for (i = 0; i < numP; i++)
		{
			
			IjvxDataConnectionProcess* theProc = NULL;
			_reference_connection_process(i, &theProc);
			if (
				(rem_this && (theProc->connector_factory_is_involved(rem_this) == JVX_NO_ERROR)) ||
				(and_this && (theProc->connection_master_factory_is_involved(and_this) == JVX_NO_ERROR))
				)
			{
				uId = JVX_SIZE_UNSELECTED;

				jvxApiString astr;
				theProc->description(&astr);
				std::cout << __FUNCTION__ << ": Removing connection <" << astr.std_str() << ">." << std::endl;

				jvxSize proc_id = JVX_SIZE_UNSELECTED;

				theProc->unique_id_connections(&proc_id);
				
				res = theProc->remove_connection();
				if (res == JVX_NO_ERROR)
				{
					theProc->deassociate_master();
					_uid_for_reference(theProc, &uId);
					assert(JVX_CHECK_SIZE_SELECTED(uId));
					lst_uids_remove.push_back(uId);
					onePlusRemoved = true;
				}
				else
				{
					theProc->description(&astr);
					std::cout << __FUNCTION__ << ": Warning: Failed to remove connection process <" <<
						astr.std_str() << ">, reason = " << jvxErrorType_txt(res) << "." << std::endl;
					_return_reference_connection_process(theProc);
					goto exit_fail;
				}
			}
			_return_reference_connection_process(theProc);
			if (onePlusRemoved)
			{
				break;
			}
		}
		if (!onePlusRemoved)
		{
			moreToRemove = false;
		}
	}

exit_fail:

	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < lst_uids_remove.size(); i++)
		{
			res = _remove_connection_process(lst_uids_remove[i]);
			assert(res == JVX_NO_ERROR);
		}
		lst_uids_remove.clear();
	}
	else
	{
		// if the removal failed we still may be in a good state. However, if there is another removal in the list,
		// we are in an invalid state since part of the function was successful and another part has failed:-(
		assert(lst_uids_remove.size() == 0);
	}
	return res;
}

jvxErrorType
CjvxDataConnections::_ready_for_start(jvxSize uId, jvxApiString* reason_if_not)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	IjvxDataConnectionProcess* theProcess = nullptr;
	this->_reference_connection_process_uid(uId, &theProcess);
	if (theProcess)
	{
		res = theProcess->check_process_ready(reason_if_not);
		if (res != JVX_NO_ERROR)
		{
			jvxApiString astr;
			theProcess->detail_connection_not_ready(&astr);
			if (!astr.empty())
			{
				if (reason_if_not)
				{
					*reason_if_not = astr;
				}
			}
		}
		_return_reference_connection_process(theProcess);
	}
	return res;
}

jvxErrorType
CjvxDataConnections::_ready_for_start(jvxApiString* reason_if_not) 
{
	jvxSize numP = 0;
	jvxSize numR = 0;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	IjvxDataConnectionProcess* theProcess = nullptr;
	jvxApiString descr;
	jvxBool essential = false;
	jvxBool foundError = false;
	jvxSize numEssentialReady = 0;
	jvxBitField bfCategoriesFound = 0;
	jvxSize catId = JVX_SIZE_UNSELECTED;
	this->_number_connections_process(&numP);

	// checkReadyRef is a "global" reference that may be specified by the application to check if
	// the system is ready for a "start". Typically, it will check that processes are there
	// to form a bit field which can then be checked by the callback in the reference class
	if (checkReadyRef)
	{
		// Check that all required processes are ready

		for (i = 0; i < numP; i++)
		{
			this->_reference_connection_process(i, &theProcess);
			assert(theProcess);

			theProcess->description(&descr);
			theProcess->category_id(&catId);
			res = theProcess->check_process_ready(reason_if_not);
			if (res == JVX_NO_ERROR)
			{
				if (JVX_CHECK_SIZE_SELECTED(catId))
				{
					assert(catId < JVX_NUMBER_POSITIONS_BITFIELD_MAX);
					jvx_bitSet(bfCategoriesFound, catId);
				}
			}
			else
			{
				theProcess->detail_connection_not_ready(reason_if_not);
				//std::cout << __FUNCTION__ << "Checking Status process <" << descr.std_str() << ">: " << jvxErrorType_txt(res) << "." << std::endl;
			}
			_return_reference_connection_process(theProcess);
		}

		res = checkReadyRef->check_status(bfCategoriesFound, reason_if_not);
	}
	else
	{
		for (i = 0; i < numP; i++)
		{
			_reference_connection_process(i, &theProcess);
			assert(theProcess);

			theProcess->description(&descr);
			theProcess->marked_essential_for_start(&essential);

			// Check if process is ready
			res = theProcess->check_process_ready(reason_if_not);
			if (essential)
			{
				// Only the essential processes may cause to prevent start
				if (res == JVX_NO_ERROR)
				{
					numEssentialReady++;
				}
				else
				{
					if (reason_if_not->empty())
					{
						theProcess->detail_connection_not_ready(reason_if_not);
					}

					// std::cout << __FUNCTION__ << "Checking Status process <" << descr.std_str() << ">: " << jvxErrorType_txt(res) << "." << std::endl;
					foundError = true;
				}
			}
			_return_reference_connection_process(theProcess);
			if (foundError)
			{
				break;
			}
		}

		if (foundError)
		{
			// astr_ready has been set in call to chain
			res = JVX_ERROR_NOT_READY;
		}
		else
		{
			if (numEssentialReady == 0)
			{
				reason_if_not->assign("Error: No chain ready for processing.");
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			else
			{
				res = JVX_NO_ERROR;
			}
		}
	}

	if (res == JVX_NO_ERROR)
	{
		this->_number_connection_rules(&numR);
		for (i = 0; i < numR; i++)
		{
			IjvxDataConnectionRule* rule = nullptr;
			this->_reference_connection_rule(i, &rule);
			if (rule)
			{
				res = rule->ready_for_start(reason_if_not);
				this->_return_reference_connection_rule(rule);
				if (res != JVX_NO_ERROR)
				{
					break;
				}
			}
		}
	}
	return res;
}

jvxErrorType
CjvxDataConnections::_set_ready_handler(IjvxDataConnections_checkready* ptr)
{
	checkReadyRef = ptr;
	return JVX_NO_ERROR;
}