#include "jvxHosts/CjvxDataConnectionsProcess.h"
#include "jvxHosts/CjvxDataConnections.h"

#define ALPHA 0.9

// =====================================================================================================
// =====================================================================================================

CjvxDataConnectionsProcess::CjvxDataConnectionsProcess(
	CjvxDataConnections* parentArg,
	const std::string& nm, 
	jvxSize unique_id_system, 
	jvxBool interceptors, 
	jvxBool essential_for_start,
	jvxBool verbose_out,
	jvxBool report_automation,
	jvxSize idDependsOn,
	jvxBool tagNameFromMasterArg):
	CjvxDataConnectionCommon(parentArg, nm, unique_id_system, interceptors, "", verbose_out, report_automation), CjvxProperties(nm, *this)
{
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	_common_set.theComponentType.tp = JVX_COMPONENT_PROCESSING_PROCESS;
	_common_set.theComponentType.slotid = JVX_SIZE_SLOT_OFF_SYSTEM;
	_common_set.theComponentType.slotsubid = JVX_SIZE_SLOT_OFF_SYSTEM;
	_common_set_min.theDescription = nm;
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(this);

	_common_set_conn_proc.id_depends_on = idDependsOn;
	_common_set_conn_proc.lastTestOk = false;
	_common_set_conn_proc.lastCheckReady = false;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS_INIT(_common_set_conn_proc.fdb);

	_common_set_conn_proc.associated_master = NULL;
	theDataConnectionGroup = static_cast<IjvxDataConnectionGroup*>(this);

	_common_set_conn_proc.test_on_connect = true;
	_common_set_conn_proc.marked_essential_start = essential_for_start;

	timestamp_start_us = JVX_SIZE_UNSELECTED;

	timestamp_buffer_start_enter = NULL;
	timestamp_buffer_start_leave = NULL;
	timestamp_buffer_process_enter = NULL;
	timestamp_buffer_process_leave = NULL;
	timestamp_buffer_stop_enter = NULL;
	timestamp_buffer_stop_leave = NULL;
	log_data = false;
	log_num_channels = 0;

	tagFromMaster = tagNameFromMasterArg;
}

CjvxDataConnectionsProcess::~CjvxDataConnectionsProcess()
{
	assert(_common_set_conn_proc.associated_master == NULL);
	assert(mapIdTriggerConnect.size() == 0);
}

void
CjvxDataConnectionsProcess::set_name_unique_descriptor(IjvxCallProt* fdb)
{
	jvxApiString uDescStr;
	if (fdb)
	{
		if (theContext)
			theContext->unique_descriptor(&uDescStr);

		fdb->setIdentification(JVX_COMPONENT_PROCESSING_PROCESS,
			"CjvxDataConnectionsProcess", "--",
			JVX_CONNECTION_ORIGIN,
			uDescStr.c_str());
	}
}

jvxErrorType
CjvxDataConnectionsProcess::set_category_id(jvxSize catIdArg)
{
	this->_common_set_conn_proc.catId = catIdArg;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionsProcess::category_id(jvxSize* catIdArg)
{
	if (catIdArg)
		*catIdArg = this->_common_set_conn_proc.catId;

	return JVX_NO_ERROR;
}

/**
 * Currently a process which depends on another process can not be prepared, started, postprocessed or stopped directly.
 * This typically is the case if a chain is part of another chain.
 */
jvxErrorType
CjvxDataConnectionsProcess::depends_on_process(jvxSize* uIdProcess)
{
	if (uIdProcess)
	{
		*uIdProcess = _common_set_conn_proc.id_depends_on;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionsProcess::test_chain(jvxBool storeProtocol JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	/*
	 * We can not automatically jump back to the master connection of dependent exists, otherwise
	 * we observe an endless loop. This must be handled by the instance receiving the events.
	 * 
	if (JVX_CHECK_SIZE_SELECTED(_common_set_conn_proc.id_depends_on) && _common_set_conn_proc.fwd_test_depends)
	{
		IjvxDataConnectionProcess* depConnProc = nullptr;
		theParent->_reference_connection_process_uid(_common_set_conn_proc.id_depends_on, &depConnProc);
		if (depConnProc)
		{
			return depConnProc->test_chain(storeProtocol JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	*/

	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		assert(_common_set_conn_proc.associated_master);

		if (storeProtocol)
		{
			_common_set_conn_proc.fdb->clear();
			set_name_unique_descriptor(_common_set_conn_proc.fdb);
			res = _common_set_conn_proc.associated_master->test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(_common_set_conn_proc.fdb));
		}
		else
		{
			// This is to append requests
			set_name_unique_descriptor(fdb);
			res = _common_set_conn_proc.associated_master->test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}

		if (res == JVX_NO_ERROR)
		{
			_common_set_conn_proc.lastTestOk = true;
		}
		else
		{
			_common_set_conn_proc.lastTestOk = false;
		}

		if (_common_set_conn_proc.lastTestOk)
		{
			if (_common_set.theReport)
			{
				//This must be reported immediately to be able to adapt before we actually start
				CjvxReportCommandRequest_uid reportit(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_TEST_SUCCESS,
					_common_set.theComponentType, _common_set_conn_comm.unique_id_system, jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);
				reportit.set_immediate(true);
				_common_set.theReport->request_command(reportit);
			}
		}
		return res; // We need to return the success of the test_chain function
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxDataConnectionsProcess::connection_master_factory_is_involved(IjvxConnectionMasterFactory* and_this)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize num = 0, i;
	IjvxConnectionMaster* mas = NULL;
	if (_common_set_conn_proc.associated_master == NULL)
	{
		return res;
	}

	and_this->number_connector_masters(&num);
	for (i = 0; i < num; i++)
	{
		mas = NULL;
		jvxErrorType resL = and_this->reference_connector_master(i, &mas);
		assert(resL == JVX_NO_ERROR);
		assert(mas);

		if (_common_set_conn_proc.associated_master == mas)
		{
			res = JVX_NO_ERROR;
		}
		and_this->return_reference_connector_master(mas);
		if (res == JVX_NO_ERROR)
		{
			break;
		}
	}
	return res;
}

// ======================================================================================

jvxErrorType
CjvxDataConnectionsProcess::associated_master(IjvxConnectionMaster** theMasterPtr)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (!theMasterPtr)
		return JVX_ERROR_INVALID_ARGUMENT;

	*theMasterPtr = _common_set_conn_proc.associated_master;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionsProcess::associate_master(IjvxConnectionMaster* theMaster, IjvxObject* theOwner)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (!theMaster)
		return JVX_ERROR_INVALID_ARGUMENT;

	if (_common_set_conn_proc.associated_master == NULL)
	{
		assert(_common_set_min.theState == JVX_STATE_INIT);
		_common_set_conn_proc.associated_master = theMaster;

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
		std::cout << "++ associating master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif
		res = _common_set_conn_proc.associated_master->select_master(static_cast<IjvxDataConnectionProcess*>(this));
		assert(res == JVX_NO_ERROR);

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		this->_select(theOwner);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxDataConnectionsProcess::create_bridge(IjvxOutputConnectorSelect* conn_from, IjvxInputConnectorSelect* conn_to, const char* nm, 
	jvxSize* unique_id, jvxBool dedicatedThread, jvxBool boostThread, jvxSize oconIdTrigger, jvxSize iconIdTrigger)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resf = JVX_NO_ERROR;
	jvxErrorType rest = JVX_NO_ERROR;
	std::string bname;
	jvxApiString txt;
	jvxBool alreadyRegistered = false;
	oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > newElm_from;
	oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > newElm_to;
	oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > newElm_orig;

	if (!conn_from)
		return JVX_ERROR_INVALID_ARGUMENT;

	if (!conn_to)
		return JVX_ERROR_INVALID_ARGUMENT;

	res = create_bridge_check();
	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	newElm_orig.uId = _common_set_conn_comm.unique_id_bridge;
	bname = "bridge_#" + jvx_size2String(newElm_orig.uId);
	if (nm)
	{
		bname = nm;
	}

	auto elm = _common_set_conn_comm.bridges.begin();
	for (; elm != _common_set_conn_comm.bridges.end(); elm++)
	{
		elm->second.ptr->descriptor_bridge(&txt);
		if (elm->second.ptr->check_exist(conn_from, conn_to) || (txt.std_str() == bname))
		{
			alreadyRegistered = true;
			break;
		}
	}

	if (!alreadyRegistered)
	{
		if (_common_set_conn_comm.interceptors)
		{
			jvxBool noErr = true;

			// One uId for both bridges
			if (unique_id)
			{
				*unique_id = newElm_orig.uId;
			}

			oneInterceptor myInterceptor;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(myInterceptor.ptr, CjvxDataChainInterceptor(NULL, static_cast<IjvxObject*>(this),
				 _common_set_conn_comm.name, bname, theInterceptors.size(), static_cast<IjvxDataChainInterceptorReport*>(this)));

			// ====================================================================================
			newElm_orig.ptr = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm_orig.ptr, CjvxConnectorBridge<CjvxDataConnectionCommon>(conn_from, conn_to, this, bname,
				dedicatedThread, boostThread, false, oconIdTrigger, iconIdTrigger));
			newElm_orig.refCnt = 0;
			// ====================================================================================

			newElm_from.ptr = NULL;
			newElm_from.uId = _common_set_conn_comm.unique_id_bridge;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm_from.ptr, CjvxConnectorBridge<CjvxDataConnectionCommon>(conn_from, 
				myInterceptor.ptr, this, bname + "_i_to",
				dedicatedThread, boostThread, true, oconIdTrigger, iconIdTrigger));
			newElm_from.refCnt = 0;
			myInterceptor.fromConnected = true;

			resf = newElm_from.ptr->init(get_master_ref(), theDataConnectionGroup/*static_cast<IjvxDataConnectionCommon*>(this)*/);
			if (resf != JVX_NO_ERROR)
			{
				noErr = false;
			}
			
			newElm_to.uId = _common_set_conn_comm.unique_id_bridge + 1;
			newElm_to.ptr = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm_to.ptr, CjvxConnectorBridge<CjvxDataConnectionCommon>(myInterceptor.ptr, conn_to,
				this, bname + "_from_o",
				dedicatedThread, boostThread, true, oconIdTrigger, iconIdTrigger));
			newElm_to.refCnt = 0;
			myInterceptor.toConnected = true;

			rest = newElm_to.ptr->init(get_master_ref(), theDataConnectionGroup/*static_cast<IjvxDataConnectionCommon*>(this)*/);
			if (rest != JVX_NO_ERROR)
			{
				noErr = false;
			}

			if (noErr)
			{
				_common_set_conn_comm.unique_id_bridge += 2;
				_common_set_conn_comm.bridges[newElm_from.uId] = newElm_from;
				_common_set_conn_comm.bridges[newElm_to.uId] = newElm_to;
				_common_set_conn_comm.bridges_orig[newElm_orig.uId] = newElm_orig;
				theInterceptors.push_back(myInterceptor);
			}
			else
			{
				newElm_from.ptr->terminate();
				JVX_DSP_SAFE_DELETE_OBJECT(newElm_from.ptr);

				newElm_to.ptr->terminate();
				JVX_DSP_SAFE_DELETE_OBJECT(newElm_to.ptr);

				JVX_DSP_SAFE_DELETE_OBJECT(newElm_orig.ptr);

				JVX_DSP_SAFE_DELETE_OBJECT(myInterceptor.ptr);

				return JVX_ERROR_INTERNAL;
			}
		}
		else
		{
			IjvxConnectionMaster* master = NULL;
			newElm_orig.ptr = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm_orig.ptr, CjvxConnectorBridge<CjvxDataConnectionCommon>(conn_from, conn_to, this, bname,
				dedicatedThread, boostThread, false, oconIdTrigger, iconIdTrigger));
			newElm_orig.refCnt = 0;
			res = newElm_orig.ptr->init(get_master_ref(), theDataConnectionGroup/*static_cast<IjvxDataConnectionCommon*>(this)*/);
			if (res != JVX_NO_ERROR)
			{
				newElm_orig.ptr->terminate();
				JVX_DSP_SAFE_DELETE_OBJECT(newElm_orig.ptr);
				return JVX_ERROR_INTERNAL;
			}

			_common_set_conn_comm.bridges[newElm_orig.uId] = newElm_orig;
			_common_set_conn_comm.unique_id_bridge++;

			if (unique_id)
			{
				*unique_id = newElm_orig.uId;
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
};

jvxErrorType
CjvxDataConnectionsProcess::link_triggers_connection()
{
	return _link_triggers_connection();
}

jvxErrorType 
CjvxDataConnectionsProcess::unlink_triggers_connection()
{
	return _unlink_triggers_connection();
}

jvxErrorType
CjvxDataConnectionsProcess::connect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_conn_proc.associated_master)
	{
		assert(_common_set_min.theState == JVX_STATE_SELECTED);
#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
		std::cout << "++ connecting master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif		
		res = _common_set_conn_proc.associated_master->set_connection_context(this);
		assert(res == JVX_NO_ERROR);

		if (tagFromMaster)
		{
			// Obtain master component id to be passed through chain. This may be modified
			// within the course of the chain
			jvxApiString astr;
			_common_set_conn_proc.associated_master->connection_association(&astr, &theCpTp);
			theTag = astr.std_str();
		}
		
		res = _common_set_conn_proc.associated_master->connect_chain_master(jvxChainConnectArguments(_common_set_conn_comm.unique_id_system, nullptr), 
			_common_set_conn_proc.init_params_ptr
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res == JVX_NO_ERROR)
		{
			jvxErrorType resL = this->_activate();
			assert(resL == JVX_NO_ERROR);

			if (JVX_CHECK_SIZE_SELECTED(connRuleId))
			{
				IjvxDataConnectionRule* rule = nullptr;
				if (theParent)
				{
					theParent->_reference_connection_rule_uid(connRuleId, &rule);
					if (rule)
					{
						rule->update_connected(true);
						theParent->_return_reference_connection_rule(rule);
					}
				}
			}

			CjvxDataConnectionProcess_genpcg::init_all();
			CjvxDataConnectionProcess_genpcg::allocate_all();
			CjvxDataConnectionProcess_genpcg::register_all(static_cast<CjvxProperties*>(this));

			_lock_properties_local();
			CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.entries.clear();
			jvx_bitFClear(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.selection());
			jvx_bitFClear(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.selectable);
			jvx_bitFClear(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.exclusive);
			auto elm = theInterceptors.begin();
			jvxSize cnt = 0;
			for (; elm != theInterceptors.end(); elm++)
			{
				jvxApiString astr;
				//(*elm)->descriptor_connector(&astr);
				elm->ptr->description_interceptor(&astr);
				CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.entries.push_back(astr.std_str());
				jvx_bitSet(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.selectable, cnt);
				jvx_bitSet(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.exclusive, cnt);
			}
			if (CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.entries.size())
			{
				jvx_bitSet(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.selection(), 0);
			}
			_unlock_properties_local();
		}
		else
		{
			_common_set_conn_proc.associated_master->set_connection_context(NULL);
			std::cout << __FUNCTION__ << ": Connect chaining not successful, reason: " << jvxErrorType_descr(res) << std::endl;
		}
		
#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		if (res == JVX_NO_ERROR)
		{
			// ======================================================================================
			// Report ALL new connections to the host
			// ======================================================================================
			if (theParent->_common_set_data_connection.theReport)
			{
				CjvxReportCommandRequest_uid reportit(
					jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED,
					_common_set.theComponentType,
					_common_set_conn_comm.unique_id_system);
				reportit.set_immediate(true);
				if (_common_set_conn_comm.report_automation)
				{
					reportit.set_broadcast(jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);
				}
				
				theParent->_common_set_data_connection.theReport->request_command(reportit);
			}			
			// ======================================================================================

			/*
			if (theParent->_common_set_data_connection.connReport)
			{
				if (_common_set_conn_comm.report_automation)
				{
					theParent->_common_set_data_connection.connReport->report_connected(_common_set_conn_comm.unique_id_system,
						_common_set_conn_comm.id_conn_category);
				}
			}
			*/
		}

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif

		if (res == JVX_NO_ERROR)
		{
			if (_common_set_conn_proc.test_on_connect)
			{
				// If the process has been successful, run the first test call
				// this->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A_NULL);

				/* We go back to parent object to make it into the test loop such that dependent processes are tested as well*/
				jvxSize uId = JVX_SIZE_UNSELECTED;
				theParent->_uid_for_reference(this, &uId);
				this->theParent->_add_process_test(uId, nullptr, true); // Test it immediately
			}
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
};

jvxErrorType
CjvxDataConnectionsProcess::disconnect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_conn_proc.associated_master)
	{
#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
		std::cout << "++ disconnecting master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif
		
		// ======================================================================================
		// Here is an important change: when we request to disconnect a chain, the chain may even be 
		// in processing! In order to stop processing at first, we pass the control to the automation by 
		// sending a command request. In that, the connection must be stopped by the sequencer!!
		// if this fails the state of the chain will be in processing and the remove function will 
		// not succede.
		if (theParent->_common_set_data_connection.theReport)
		{
			CjvxReportCommandRequest_uid reportit(
				jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED,
				_common_set.theComponentType,
				_common_set_conn_comm.unique_id_system);
			reportit.set_immediate(true);
			if (_common_set_conn_comm.report_automation)
			{
				reportit.set_broadcast(jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);
			}

			theParent->_common_set_data_connection.theReport->request_command(reportit);
		}
		// ======================================================================================

		if (_common_set_min.theState != JVX_STATE_ACTIVE)
		{
			return JVX_ERROR_WRONG_STATE;
		}

		/*
		if (theParent->_common_set_data_connection.connReport)
		{
			if (_common_set_conn_comm.report_automation)
			{
				theParent->_common_set_data_connection.connReport->report_about_to_disconnect(
					_common_set_conn_comm.unique_id_system,
					_common_set_conn_comm.id_conn_category);
			}
		}
		*/

		res = _common_set_conn_proc.associated_master->disconnect_chain_master(jvxChainConnectArguments( _common_set_conn_comm.unique_id_system), 
				JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			CjvxDataConnectionProcess_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
			CjvxDataConnectionProcess_genpcg::deallocate_all();

			if (JVX_CHECK_SIZE_SELECTED(connRuleId))
			{
				IjvxDataConnectionRule* rule = nullptr;
				if (theParent)
				{
					theParent->_reference_connection_rule_uid(connRuleId, &rule);
					if (rule)
					{
						rule->update_connected(false);
						theParent->_return_reference_connection_rule(rule);
					}
				}
			}

			jvxErrorType resL = this->_deactivate();
			assert(resL == JVX_NO_ERROR);			
		}
		else
		{
			std::cout << __FUNCTION__ << ": Disconnect chaining not successful" << std::endl;
		}
		assert(res == JVX_NO_ERROR);

		res = _common_set_conn_proc.associated_master->set_connection_context(NULL);
		assert(res == JVX_NO_ERROR);		

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif

		// We do not rely on this too much - it is more a view thing
		if (theParent->_common_set_data_connection.theReport)
		{
			CjvxReportCommandRequest_uid reportit(
				jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_DISCONNECT_COMPLETE,
				_common_set.theComponentType,
				_common_set_conn_comm.unique_id_system);			
			theParent->_common_set_data_connection.theReport->request_command(reportit);
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxDataConnectionsProcess::match_master(jvxBool* doesMatchRet, jvxComponentIdentification tpMatch, const char* wildcardMatch)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnectionMasterFactory* masterFactory = NULL;
	IjvxObject* theObject = NULL;
	jvxComponentIdentification locCpId;
	jvxBool matchesLocal = false;

	if (_common_set_conn_proc.associated_master)
	{
		if (!jvx_compareStringsWildcard(wildcardMatch, _common_set_conn_comm.name))
		{
			return JVX_NO_ERROR;
		}

		// Here, the name matches the wildcard
		_common_set_conn_proc.associated_master->parent_master_factory(&masterFactory);
		assert(masterFactory);

		masterFactory->request_reference_object(&theObject);
		assert(theObject);
		theObject->request_specialization(NULL, &locCpId, NULL, NULL);
		if (tpMatch.tp == JVX_COMPONENT_UNKNOWN)
		{
			matchesLocal = true;
		}
		else
		{
			if (tpMatch.tp == locCpId.tp)
			{
				matchesLocal = true;
			}
		}

		if (matchesLocal)
		{
			matchesLocal = false;
			if (
				(tpMatch.slotid == JVX_SIZE_DONTCARE) ||
				(tpMatch.slotid == locCpId.slotid))
			{
				if (
					(tpMatch.slotsubid == JVX_SIZE_DONTCARE) ||
					(tpMatch.slotsubid == locCpId.slotsubid))
				{
					matchesLocal = true;
				}
			}
		}
		masterFactory->return_reference_object(theObject);

		if (doesMatchRet)
			*doesMatchRet = matchesLocal;

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxDataConnectionsProcess::prepare_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_conn_proc.associated_master)
	{
		prepare_local();

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
		std::cout << "++ preparing master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif			
		res = _common_set_conn_proc.associated_master->prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			jvxErrorType resL = this->_prepare();
			assert(resL == JVX_NO_ERROR);
		}
		else
		{
			std::cout << __FUNCTION__ << ": Connect chaining not successful" << std::endl;
		}

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxDataConnectionsProcess::postprocess_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_conn_proc.associated_master)
	{
		postprocess_local();

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
		std::cout << "++ postprocess master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif			
		res = _common_set_conn_proc.associated_master->postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			jvxErrorType resL = this->_postprocess();
			assert(resL == JVX_NO_ERROR);
		}
		else
		{
			std::cout << __FUNCTION__ << ": Postprocessing chain not successful" << std::endl;
		}

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxDataConnectionsProcess::start_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_conn_proc.associated_master)
	{
		start_local();

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
		std::cout << "++ starting master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif			
		res = _common_set_conn_proc.associated_master->start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			jvxErrorType resL = this->_start();
			assert(resL == JVX_NO_ERROR);
		}
		else
		{
			std::cout << __FUNCTION__ << ": Starting chaining not successful" << std::endl;
		}

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxDataConnectionsProcess::stop_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_conn_proc.associated_master)
	{
		stop_local();

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
		std::cout << "++ stopping master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif			
		res = _common_set_conn_proc.associated_master->stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			jvxErrorType resL = this->_stop();
			assert(resL == JVX_NO_ERROR);
		}
		else
		{
			std::cout << __FUNCTION__ << ": Stopping chaining not successful" << std::endl;
		}

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxDataConnectionsProcess::transfer_forward_chain(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_conn_proc.associated_master)
	{

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
		std::cout << "++ specific master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif
		res = _common_set_conn_proc.associated_master->transfer_chain_forward_master(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxDataConnectionsProcess::transfer_backward_chain(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_conn_proc.associated_master)
	{

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
		std::cout << "++ specific master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif
		res = _common_set_conn_proc.associated_master->transfer_chain_backward_master(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxDataConnectionsProcess::apply_update(jvxConnectionEventType evTp JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_conn_proc.associated_master)
	{

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
		std::cout << "++ apply changes master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif
		switch (evTp)
		{
		case JVX_CONNECTION_EVENT_LAYOUT_CHANGED:
			res = _common_set_conn_proc.associated_master->inform_changed_master(JVX_MASTER_UPDATE_EVENT_LAYOUT_CHANGED
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			break;
		default:
			assert(0);
		}
		assert(res == JVX_NO_ERROR);

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxDataConnectionsProcess::deassociate_master()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	if (_common_set_conn_proc.associated_master)
	{
		if (_common_set_min.theState >= JVX_STATE_ACTIVE)
		{
			_common_set_conn_proc.associated_master->disconnect_chain_master(jvxChainConnectArguments(_common_set_conn_comm.unique_id_system),
				JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}

		if (_common_set_conn_comm.bridges.size())
		{
			remove_all_bridges();
		}

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << "<" << std::endl;
		std::cout << "++ deassociating master " << std::flush;
		std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif
		res = _common_set_conn_proc.associated_master->unselect_master();
		assert(res == JVX_NO_ERROR);

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		_common_set_conn_proc.associated_master = NULL;
		jvxErrorType resL = this->_unselect();
		assert(resL == JVX_NO_ERROR);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void
CjvxDataConnectionsProcess::get_configuration_local(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section)
{
	jvxConfigData* datTmp = NULL;
	IjvxConnectionMasterFactory* masFac = NULL;
	jvxComponentIdentification tpId = JVX_COMPONENT_UNKNOWN;
	jvxApiString strMF, strMa;
	jvxValue val;

	theWriter->createAssignmentString(&datTmp, "JVX_CONNECTION_PROCESS_NAME", _common_set_conn_comm.name.c_str());
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	assert(_common_set_conn_proc.associated_master);
	_common_set_conn_proc.associated_master->parent_master_factory(&masFac);
	assert(masFac);

	theWriter->createAssignmentValue(&datTmp, "PROCESS_INTERCEPTORS", _common_set_conn_comm.interceptors);
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	theWriter->createAssignmentValue(&datTmp, "PROCESS_ESSENTIAL", _common_set_conn_proc.marked_essential_start);
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	jvx_request_interfaceToObject(masFac, NULL, &tpId, &strMF, NULL);
	std::string prefix = "MASTER_FACTORY_";

	theWriter->createAssignmentString(&datTmp, (prefix + "NAME").c_str(), strMF.c_str());
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	theWriter->createAssignmentString(&datTmp, (prefix + "COMPONENT_TYPE").c_str(), jvxComponentType_txt(tpId.tp));
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	val.assign((jvxSize)tpId.slotid);
	theWriter->createAssignmentValue(&datTmp, (prefix + "SLOT_ID").c_str(), val);
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	val.assign((jvxSize)tpId.slotsubid);
	theWriter->createAssignmentValue(&datTmp, (prefix + "SLOT_SUB_ID").c_str(), val);
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	// =================================================================

	_common_set_conn_proc.associated_master->descriptor_master(&strMa);
	theWriter->createAssignmentString(&datTmp, "MASTER_NAME", strMa.c_str());
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;
}

jvxErrorType
CjvxDataConnectionsProcess::create_bridge_check()
{
	if (_common_set_conn_proc.associated_master == NULL)
		return JVX_ERROR_WRONG_STATE;
	return JVX_NO_ERROR;
}

IjvxConnectionMaster*
CjvxDataConnectionsProcess::get_master_ref()
{
	return _common_set_conn_proc.associated_master;
}

jvxErrorType
CjvxDataConnectionsProcess::status_chain(jvxBool* lastTestOk JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (lastTestOk)
	{
		*lastTestOk = _common_set_conn_proc.lastTestOk;
	}
	JVX_CONNECTION_FEEDBACK_TYPE_ASSIGN(fdb, _common_set_conn_proc.fdb);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionsProcess::remove_bridge_local(jvxSize unique_id)
{
	return _remove_bridge_local(unique_id);
};

jvxErrorType 
CjvxDataConnectionsProcess::remove_all_bridges_local()
{
	return _remove_all_bridges_local();
}

// ============================================================================================

jvxErrorType
CjvxDataConnectionsProcess::request_hidden_interface(jvxInterfaceType tp, jvxHandle** ret)
{
	switch (tp)
	{
	case JVX_INTERFACE_PROPERTIES:
		if (ret)
		{
			*ret = static_cast<IjvxProperties*>(this);
		}
		return JVX_NO_ERROR;
		break;

	case JVX_INTERFACE_CONFIGURATION:
		if (ret)
		{
			*ret = static_cast<IjvxConfiguration*>(this);
		}
		return JVX_NO_ERROR;
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxDataConnectionsProcess::return_hidden_interface(jvxInterfaceType tp, jvxHandle* ret)
{
	switch (tp)
	{
	case JVX_INTERFACE_PROPERTIES:
		assert(ret == static_cast<IjvxProperties*>(this));
		return JVX_NO_ERROR;
		break;
	case JVX_INTERFACE_CONFIGURATION:
		assert(ret == static_cast<IjvxConfiguration*>(this));
		return JVX_NO_ERROR;
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxDataConnectionsProcess::object_hidden_interface(IjvxObject** objRef)
{
	if (objRef)
	{
		*objRef = _common_set.thisisme;
	}
	return JVX_NO_ERROR;
}

// ===============================================================================================

jvxErrorType
CjvxDataConnectionsProcess::report_event_interceptor(jvxDataChainReportType tp, jvxSize interceptorid)
{
	jvxTick timestamp_now = 0;
	int i;
	jvxSize sel = JVX_SIZE_UNSELECTED;
	if (log_data)
	{
		sel = jvx_bitfieldSelection2Id(
			CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.selection(),
			CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.entries.size());

		timestamp_now = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStampRun);
		switch (tp)
		{
			/*
			JVX_INTERCEPTOR_REPORT_PROCESS_START_ENTER,
			JVX_INTERCEPTOR_REPORT_PROCESS_START_LEAVE,
			JVX_INTERCEPTOR_REPORT_PROCESS_BUFFER_ENTER,
			JVX_INTERCEPTOR_REPORT_PROCESS_BUFFER_LEAVE,
			JVX_INTERCEPTOR_REPORT_PROCESS_STOP_ENTER,
			JVX_INTERCEPTOR_REPORT_PROCESS_STOP_LEAVE
			*/
		case JVX_INTERCEPTOR_REPORT_PROCESS_START_ENTER:
			timestamp_buffer_start_enter[interceptorid] = timestamp_now;
			break;
		case JVX_INTERCEPTOR_REPORT_PROCESS_START_LEAVE:
			timestamp_buffer_start_leave[interceptorid] = timestamp_now;
			break;
		case JVX_INTERCEPTOR_REPORT_PROCESS_BUFFER_ENTER:
			timestamp_buffer_process_enter[interceptorid] = timestamp_now;
			break;
		case JVX_INTERCEPTOR_REPORT_PROCESS_BUFFER_LEAVE:
			timestamp_buffer_process_leave[interceptorid] = timestamp_now;
			break;
		case JVX_INTERCEPTOR_REPORT_PROCESS_STOP_ENTER:
			timestamp_buffer_stop_enter[interceptorid] = timestamp_now;
			break;
		case JVX_INTERCEPTOR_REPORT_PROCESS_STOP_LEAVE:
			timestamp_buffer_stop_enter[interceptorid] = timestamp_now;
			break;
		default:
			assert(0);
			break;
		}

		if (
			(interceptorid == 0) &&
			(tp == JVX_INTERCEPTOR_REPORT_PROCESS_STOP_LEAVE))
		{
			std::cout << "Call duration Full = " << (timestamp_buffer_stop_enter[0] - timestamp_buffer_start_enter[0]) * 0.001 << " msecs." << std::endl;
			jvxData tmp = 0;
			jvxData accu = 0;
			for (i = log_num_channels-1; i >= 0; i--)
			{
				tmp = (jvxData)(timestamp_buffer_process_leave[i] - timestamp_buffer_process_enter[i]);
				accu = (tmp - accu) * 0.001;
				if (sel == i)
				{
					CjvxDataConnectionProcess_genpcg::properties.duration_msec.value =
						CjvxDataConnectionProcess_genpcg::properties.duration_msec.value * ALPHA + accu * (1 - ALPHA);
				}
				// std::cout << "Call duration [" << i << "] = " << accu << " msecs." << std::endl;
				accu = tmp;
			}
		}
	}
	return JVX_NO_ERROR;
}

// ==================================================================================

void
CjvxDataConnectionsProcess::prepare_local()
{
	log_data = false;

	log_data = (CjvxDataConnectionProcess_genpcg::properties.log_active.value == c_true);
	CjvxDataConnectionProcess_genpcg::properties.duration_msec.value = 0.0;
	CjvxProperties::_update_properties_on_start();
	if (log_data)
	{
		log_num_channels = theInterceptors.size();
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(timestamp_buffer_start_enter, jvxInt64, log_num_channels);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(timestamp_buffer_start_leave, jvxInt64, log_num_channels);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(timestamp_buffer_process_enter, jvxInt64, log_num_channels);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(timestamp_buffer_process_leave, jvxInt64, log_num_channels);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(timestamp_buffer_stop_enter, jvxInt64, log_num_channels);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(timestamp_buffer_stop_enter, jvxInt64, log_num_channels);
	}
}

void 
CjvxDataConnectionsProcess::start_local()
{
	JVX_GET_TICKCOUNT_US_SETREF(&tStampRun);
	timestamp_start_us = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStampRun);
}

void
CjvxDataConnectionsProcess::stop_local()
{
	timestamp_start_us = JVX_SIZE_UNSELECTED;
}

void
CjvxDataConnectionsProcess::postprocess_local()
{
	if (log_data)
	{
		JVX_DSP_SAFE_DELETE_FIELD(timestamp_buffer_start_enter);
		JVX_DSP_SAFE_DELETE_FIELD(timestamp_buffer_start_leave);
		JVX_DSP_SAFE_DELETE_FIELD(timestamp_buffer_process_enter);
		JVX_DSP_SAFE_DELETE_FIELD(timestamp_buffer_process_leave);
		JVX_DSP_SAFE_DELETE_FIELD(timestamp_buffer_stop_enter);
		JVX_DSP_SAFE_DELETE_FIELD(timestamp_buffer_stop_enter);
	}
	log_data = false;
	log_num_channels = 0;
	CjvxProperties::_update_properties_on_stop();
}

jvxErrorType
CjvxDataConnectionsProcess::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		jvxConfigData* cfg = NULL;
		processor->getReferenceSubsectionCurrentSection_name(sectionToContainAllSubsectionsForMe, &cfg, "CONNECTION_PROCESS_PROPERTIES");
		if (cfg)
		{

			CjvxDataConnectionProcess_genpcg::put_configuration_all(callConf, processor,
				sectionToContainAllSubsectionsForMe, &warns);
			for (i = 0; i < warns.size(); i++)
			{
			  std::cout << __FUNCTION__ << ": Warning when reading configuration: " << warns[i] << std::endl;
			}
			warns.clear();
		}
		else
		{
		  std::cout << __FUNCTION__ << ": Warning when reading configuration: Failed to read section <CONNECTION_PROCESS_PROPERTIES>." << std::endl;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionsProcess::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxDataConnectionCommon::_get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		jvxConfigData* cfg = NULL;
		processor->createEmptySection(&cfg, "CONNECTION_PROCESS_PROPERTIES");
		if (cfg)
		{
			CjvxDataConnectionProcess_genpcg::get_configuration_all(callConf, processor, cfg);
			processor->addSubsectionToSection(sectionWhereToAddAllSubsections, cfg);
		}
	}
	return res;
}

jvxErrorType 
CjvxDataConnectionsProcess::set_forward_test_depend(jvxBool test_on_connect)
{
	_common_set_conn_proc.fwd_test_depends = test_on_connect;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataConnectionsProcess::get_forward_test_depend(jvxBool* test_on_connect)
{
	if (test_on_connect)
	{
		*test_on_connect = _common_set_conn_proc.fwd_test_depends;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataConnectionsProcess::set_test_on_connect(jvxBool test_on_connect, const jvxLinkDataDescriptor_con_params* init_params)
{
	_common_set_conn_proc.test_on_connect = test_on_connect;
	if (init_params)
	{
		_common_set_conn_proc.init_params = *init_params;
		_common_set_conn_proc.init_params_ptr = &_common_set_conn_proc.init_params;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataConnectionsProcess::get_test_on_connect(jvxBool* test_on_connect, jvxLinkDataDescriptor_con_params* init_params)
{
	if (test_on_connect)
		*test_on_connect = _common_set_conn_proc.test_on_connect;
	if (init_params)
		*init_params = _common_set_conn_proc.init_params;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionsProcess::marked_essential_for_start(jvxBool* marked_essential)
{
	if (marked_essential)
		*marked_essential = _common_set_conn_proc.marked_essential_start;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionsProcess::check_process_ready(jvxApiString* reason_if_not_ready)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string errText;
	jvxApiString astr;

	// We enter the data transfer through the chain here. This should not be done with a nullptr
	// as the second argument since some modules test the pointer
	/* 
	if (reason_if_not_ready == nullptr)
	{
		reason_if_not_ready = &astr;
	}
	*/
	if(_common_set_conn_proc.lastTestOk)
	{
		_common_set_conn_proc.lastCheckReady = false;
		if (_common_set_conn_proc.associated_master)
		{
			res = _common_set_conn_proc.associated_master->transfer_chain_forward_master(JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY, reason_if_not_ready JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
			if (res == JVX_NO_ERROR)
			{
				// Store lastCheckReady success
				_common_set_conn_proc.lastCheckReady = true;
			}
		}
		else
		{
			res = JVX_ERROR_NOT_READY;
			if (reason_if_not_ready)
			{
				errText = _common_set_conn_comm.name + ": This process has no associated master.";
				reason_if_not_ready->assign(errText);
			}
		}
	}
	else
	{
		res = JVX_ERROR_NOT_READY;
		if (reason_if_not_ready)
		{
			errText = _common_set_conn_comm.name + ": Recent process chain test has failed, review connection dialog for details!";
			reason_if_not_ready->assign(errText);
		}
	}
	return res;
}

jvxErrorType 
CjvxDataConnectionsProcess::detail_connection_not_ready(jvxApiString* reason_if_not_ready)
{
	if (_common_set_conn_proc.lastTestOk)
	{
		if (reason_if_not_ready)
		{
			reason_if_not_ready->clear();
		}
	}
	else
	{
		jvxErrorType res = JVX_NO_ERROR;
		IjvxCallProt* fdbLastTest = _common_set_conn_proc.fdb;	
		getFirstErrorReason(fdbLastTest, &res, reason_if_not_ready);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionsProcess::getFirstErrorReason(IjvxCallProt* fdbElement, jvxErrorType* resOnReturn, jvxApiString* strOnReturn)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxApiString err;
	fdbElement->getSuccess(&resL, &err, nullptr);
	if (resL == JVX_NO_ERROR)
	{
		jvxSize num = 0, i;
		fdbElement->getNumberNext(&num);
		for (i = 0; i < num; i++)
		{
			IjvxCallProt* newNxt = nullptr;
			fdbElement->getNext(nullptr, &newNxt, i);
			if (newNxt)
			{
				
				res = getFirstErrorReason(newNxt, &resL, strOnReturn);
				if (resL != JVX_NO_ERROR)
				{
					if (resOnReturn)
						*resOnReturn = resL;
					break;
				}
			}
		}
	}
	else
	{
		jvxApiString nm;
		if (resOnReturn)
			*resOnReturn = resL;
		fdbElement->getIdentification(nullptr, &nm, nullptr, nullptr, nullptr, nullptr);
		std::string errDescr = "Component <";
		errDescr += nm.c_str();
		errDescr += ">: ";
		errDescr += err.c_str();
		if (strOnReturn)
		{
			strOnReturn->assign(errDescr);
		}
	}
	return res;
}

jvxErrorType
CjvxDataConnectionsProcess::iterator_chain(IjvxConnectionIterator** it)
{
	IjvxConnectionMaster* mas = nullptr;
	this->associated_master(&mas);
	if (mas)
	{
		return mas->iterator_chain(it);
	}
	return JVX_ERROR_NOT_READY;
}