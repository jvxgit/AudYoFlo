#include "jvxHosts/CjvxDataConnectionsGroup.h"
#include "common/CjvxConnectorMasterFactory.h"

#define ALPHA 0.9
// =====================================================================================================
// =====================================================================================================

CjvxDataConnectionsGroup::CjvxDataConnectionsGroup(
	CjvxDataConnections* parentArg,
	const std::string& nm, 
	jvxSize unique_id_system, 
	const std::string& descr, 
	jvxBool verbose_out):
	CjvxDataConnectionCommon(
		parentArg,
		nm, 
		unique_id_system, 
		true, 
		descr, 
		verbose_out,
		false), CjvxProperties(nm, *this) // Always interceptors
{
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	
	_common_set.theComponentType.tp = JVX_COMPONENT_PROCESSING_GROUP;
	_common_set.theComponentType.slotid = JVX_SIZE_SLOT_OFF_SYSTEM;
	_common_set.theComponentType.slotsubid = JVX_SIZE_SLOT_OFF_SYSTEM;
	_common_set_min.theDescription = nm;
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(this);

	theDataConnectionGroup = static_cast<IjvxDataConnectionGroup*>(this);

	// =================================================================================0

	timestamp_start_us = JVX_SIZE_UNSELECTED;

	timestamp_buffer_start_enter = NULL;
	timestamp_buffer_start_leave = NULL;
	timestamp_buffer_process_enter = NULL;
	timestamp_buffer_process_leave = NULL;
	timestamp_buffer_stop_enter = NULL;
	timestamp_buffer_stop_leave = NULL;
	log_data = false;
	log_num_channels = 0;

#ifdef JVX_BUFFERING_LOGFILE
	logFileObj = NULL;
	logFileHdl = NULL;
#endif

}

CjvxDataConnectionsGroup::~CjvxDataConnectionsGroup()
{
	
}

void
CjvxDataConnectionsGroup::set_name_unique_descriptor(IjvxCallProt* fdb)
{
	jvxApiString uDescStr;
	if (fdb)
	{
		if (theContext)
			theContext->unique_descriptor(&uDescStr);

		fdb->setIdentification(JVX_COMPONENT_PROCESSING_GROUP,
			"CjvxDataConnectionsGroup", "--",
			JVX_CONNECTION_ORIGIN,
			uDescStr.c_str());
	}
}

jvxErrorType
CjvxDataConnectionsGroup::create_bridge(IjvxOutputConnectorSelect* conn_from, 
	IjvxInputConnectorSelect* conn_to, const char* nm, jvxSize* unique_id, jvxBool dedicatedThread, jvxBool boostThread,
	jvxSize oconIdTrigger, jvxSize iconIdTrigger)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resf = JVX_NO_ERROR;
	jvxErrorType rest = JVX_NO_ERROR;
	std::string bname;
	jvxApiString txt;
	jvxBool alreadyRegistered = false;
	jvxSize uId_to = 0;
	jvxSize uId_from = 0;
	oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > newElm_from;
	oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > newElm_to;
	oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > newElm_orig;

	if (!conn_from)
	{
		uId_from = JVX_SIZE_UNSELECTED;
		// conn_from = static_cast<IjvxOutputConnector*>(this->my_local_master);
	}
	if (!conn_to)
	{
		uId_to = JVX_SIZE_UNSELECTED;
		// conn_to = static_cast<IjvxInputConnector*>(this->my_local_master);
	}

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
		jvxBool noErr = true;

		// One uId for both bridges
		if (unique_id)
		{
			*unique_id = newElm_orig.uId;
		}

		oneInterceptor myInterceptor;
		CjvxDataChainInterceptorGroup* theInterceptor = NULL;

		JVX_DSP_SAFE_ALLOCATE_OBJECT(theInterceptor,
			CjvxDataChainInterceptorGroup(static_cast<IjvxConnectorFactory*>(&exposeConnectors),
				static_cast<IjvxObject*>(this), 
			_common_set_conn_comm.name, bname,
				theInterceptors.size(),
				static_cast<IjvxDataChainInterceptorReport*>(this),
				dedicatedThread, boostThread, this));
		myInterceptor.ptr = theInterceptor;

		// ====================================================================================
		newElm_orig.ptr = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm_orig.ptr, CjvxConnectorBridge<CjvxDataConnectionCommon>(conn_from, conn_to, this, bname, 
			dedicatedThread, boostThread, false, oconIdTrigger, iconIdTrigger));
		newElm_orig.refCnt = 0;
		// ====================================================================================

		jvxSize idCnt = _common_set_conn_comm.unique_id_bridge;
		std::vector<jvxSize> idLnk;

		newElm_from.ptr = NULL;
		newElm_from.uId = 0;
		if (conn_from)
		{

			newElm_from.uId = idCnt;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm_from.ptr, CjvxConnectorBridge<CjvxDataConnectionCommon>(conn_from,
				myInterceptor.ptr, this, bname + "_i_to", dedicatedThread, boostThread, true,
				oconIdTrigger, iconIdTrigger));
			myInterceptor.fromConnected = true;
			newElm_from.refCnt = 0;

			resf = newElm_from.ptr->init(get_master_ref(), theDataConnectionGroup/*static_cast<IjvxDataConnectionCommon*>(this)*/);
			if (resf != JVX_NO_ERROR)
			{
				noErr = false;
			}
			idLnk.push_back(idCnt);
			idCnt++;
		}
		
		newElm_to.ptr = NULL;
		newElm_to.uId = 0;
		if (conn_to)
		{
			newElm_to.uId = idCnt;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm_to.ptr, CjvxConnectorBridge<CjvxDataConnectionCommon>(myInterceptor.ptr, conn_to,
				this, bname + "_from_o", dedicatedThread, boostThread, true,
				oconIdTrigger, iconIdTrigger));
			myInterceptor.toConnected = true;
			newElm_to.refCnt = 0;

			rest = newElm_to.ptr->init(get_master_ref(), theDataConnectionGroup/*static_cast<IjvxDataConnectionCommon*>(this)*/);
			if (rest != JVX_NO_ERROR)
			{
				noErr = false;
			}
			idLnk.push_back(idCnt);
			idCnt++;
		}

		if (noErr)
		{
			std::string append_from;
			std::string append_to;

			_common_set_conn_comm.unique_id_bridge = idCnt;
			
			if (myInterceptor.fromConnected == false)
			{
				oneChainEntry elmEntry;
				elmEntry.ptr = theInterceptor;
				interceptors_in[elmEntry.ptr] = elmEntry;
			}
			if (myInterceptor.toConnected == false)
			{
				interceptors_out.push_back(theInterceptor);
			}

			if (newElm_from.ptr)
			{
				_common_set_conn_comm.bridges[newElm_from.uId] = newElm_from;
			}
			else
			{
				assert(myInterceptor.fromConnected == false);
				append_to = "_i";
				myInterceptor.ptr->tag(JVX_PROCESS_MARK_ENTER_CONNECTOR);
			}

			if (newElm_to.ptr)
			{
				_common_set_conn_comm.bridges[newElm_to.uId] = newElm_to;
			}
			else
			{
				assert(myInterceptor.toConnected == false);
				append_from = "_o";
				myInterceptor.ptr->tag(JVX_PROCESS_MARK_LEAVE_CONNECTOR);
			}

			if (newElm_from.ptr)
			{
				newElm_from.ptr->append_name_postfix(append_from.c_str());
			}
			if (newElm_to.ptr)
			{
				newElm_to.ptr->append_name_postfix(append_to.c_str());
			}

			_common_set_conn_comm.bridges_orig[newElm_orig.uId] = newElm_orig;
			mapped_ids[newElm_orig.uId] = idLnk;
			theInterceptors.push_back(myInterceptor);
		}
		else
		{
			if (newElm_from.ptr)
			{
				newElm_from.ptr->terminate();
				JVX_DSP_SAFE_DELETE_OBJECT(newElm_from.ptr);
			}
			if (newElm_to.ptr)
			{
				newElm_to.ptr->terminate();
				JVX_DSP_SAFE_DELETE_OBJECT(newElm_to.ptr);
			}
			if (newElm_orig.ptr)
			{
				JVX_DSP_SAFE_DELETE_OBJECT(newElm_orig.ptr);
			}
			JVX_DSP_SAFE_DELETE_OBJECT(theInterceptor);

			return JVX_ERROR_INTERNAL;
		}

#if 0


		IjvxConnectionMaster* master = NULL;
		newElm.ptr = NULL;
		
		JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm.ptr, CjvxConnectorBridge<CjvxDataConnectionCommon>(conn_from, conn_to, this, bname));
		newElm.refCnt = 0;
		newElm.ptr->set_uid_from_to( uId_from,  uId_to);

		res = newElm.ptr->init(get_master_ref(), theDataConnectionGroup/*static_cast<IjvxDataConnectionCommon*>(this)*/);
		if (res != JVX_NO_ERROR)
		{
			newElm.ptr->terminate();
			JVX_DSP_SAFE_DELETE_OBJECT(newElm.ptr);
			return JVX_ERROR_INTERNAL;
		}

		_common_set_conn_comm.bridges[newElm.uId] = newElm;
		_common_set_conn_comm.unique_id_bridge++;

		if (unique_id)
		{
			*unique_id = newElm.uId;
		}
#endif
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
};

jvxErrorType
CjvxDataConnectionsGroup::connect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	assert(_common_set_min.theState = JVX_STATE_INIT);

#ifdef JVX_DATACONNECTIONS_VERBOSE
	std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
	std::cout << "DataConnectionGroup <" << _common_set_conn_comm.name << ">" << std::endl;
	std::cout << "++ connecting master " << std::flush;
	std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif		
	
	// This part would be in associate_master in processes
	jvxErrorType resL = this->_select();
	assert(resL == JVX_NO_ERROR); 

	resL = CjvxDataConnectionCommon::_activate();
	assert(resL == JVX_NO_ERROR);

	auto elm = theInterceptors.begin();
	for (; elm != theInterceptors.end(); elm++)
	{
		elm->ptr->set_tools_host(_common_set.theToolsHost);
	}

	CjvxDataConnectionProcess_genpcg::init_all();
	CjvxDataConnectionProcess_genpcg::allocate_all();
	CjvxDataConnectionProcess_genpcg::register_all(static_cast<CjvxProperties*>(this));

	CjvxDataConnectionGroup_genpcg::init_all();
	CjvxDataConnectionGroup_genpcg::allocate_all();
	CjvxDataConnectionGroup_genpcg::register_all(static_cast<CjvxProperties*>(this));
	CjvxDataConnectionGroup_genpcg::register_callbacks(
		static_cast<CjvxProperties*>(this), 
		get_data_prehook, 
		get_chain_prehook,
		set_chain_posthook,
		reinterpret_cast<jvxHandle*>(this),
		NULL);

	_lock_properties_local();
	CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.entries.clear();
	jvx_bitFClear(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.selection());
	jvx_bitFClear(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.selectable);
	jvx_bitFClear(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.exclusive);
	
	elm = theInterceptors.begin();
	jvxSize cnt = 0;
	for (; elm != theInterceptors.end(); elm++)
	{
		jvxApiString astr;
		//(*elm)->descriptor_connector(&astr);
		elm->ptr->descriptor_connector(&astr);
		CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.entries.push_back(astr.std_str());
		jvx_bitSet(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.selectable, cnt);
		jvx_bitSet(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.exclusive, cnt);
	}
	if (CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.entries.size())
	{
		jvx_bitSet(CjvxDataConnectionProcess_genpcg::properties.interceptor_list.value.selection(), 0);
	}

	// =============================================================================

	_unlock_properties_local();

	// Register new factory
	exposeConnectors.set_object_ref(static_cast<IjvxObject*>(this));
	
	cnt = 0;
	auto elmii = interceptors_in.begin();
	for (; elmii != interceptors_in.end(); elmii++)
	{
		std::string newNm = "input_connector_" + jvx_size2String(cnt);
		elmii->second.ptr->updateName(newNm.c_str());
		exposeConnectors.add_outside_input_connector(elmii->second.ptr);
		cnt++;
	}

	cnt = 0;
	auto elmio = interceptors_out.begin();
	for (; elmio != interceptors_out.end(); elmio++)
	{
		std::string newNm = "output_connector_" + jvx_size2String(cnt);
		(*elmio)->updateName(newNm.c_str());
		exposeConnectors.add_outside_output_connector(*elmio);
		cnt++;
	}
	theContext->register_connection_factory(&exposeConnectors);

#ifdef JVX_DATACONNECTIONS_VERBOSE
	std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif

	CjvxDataConnectionGroup_genpcg::properties.select_chain.value.entries.clear();
	auto elmi = interceptors_in.begin();
	for (; elmi != interceptors_in.end(); elmi++)
	{
		jvxApiString astr;
		elmi->second.ptr->descriptor_connector(&astr);
		CjvxDataConnectionGroup_genpcg::properties.select_chain.value.entries.push_back(astr.std_str());
	}
	jvx_bitFClear(CjvxDataConnectionGroup_genpcg::properties.select_chain.value.selection());
	if (CjvxDataConnectionGroup_genpcg::properties.select_chain.value.entries.size())
	{
		jvx_bitZSet(CjvxDataConnectionGroup_genpcg::properties.select_chain.value.selection(), 0);
	}

	// =============================================================================
	updateTaskList();
	CjvxDataConnectionGroup_genpcg::properties.avrg_duration_task.value = 0.0;

	return res;

};

void
CjvxDataConnectionsGroup::updateTaskList()
{
	jvxSize i;
	jvxSize idSelect = jvx_bitfieldSelection2Id(
		CjvxDataConnectionGroup_genpcg::properties.select_chain.value.selection(),
		CjvxDataConnectionGroup_genpcg::properties.select_chain.value.entries.size());

	jvx_bitFClear(CjvxDataConnectionGroup_genpcg::properties.select_task.value.selection());
	CjvxDataConnectionGroup_genpcg::properties.select_task.value.entries.clear();
	if (idSelect < interceptors_in.size())
	{
		auto elmi = interceptors_in.begin();
		std::advance(elmi, idSelect);
		if (elmi->second.obj_id)
		{
			std::string chain_nm;
			jvxApiString astr;
			std::string txt;	
			std::vector<std::string> tasks;

			// Get the name
			elmi->first->descriptor_connector(&astr);
			chain_nm = astr.std_str(); 
			tasks.resize(elmi->second.obj_id->num_tasks);
			
			txt = "full-inout";
			i = 0;
			if (i < tasks.size())
			{
				tasks[i] = txt;
			}

			i = 1;
			txt = "input-proc";
			if (i < tasks.size())
			{
				tasks[i] = txt;
			}

			jvxErrorType res = elmi->first->transfer_forward_icon(
				JVX_LINKDATA_TRANSFER_SPECIFY_TASKNAMES_CHAIN_IN_GROUP,
				&tasks JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
			assert(res == JVX_NO_ERROR);
			
			assert(tasks.size() == elmi->second.obj_id->num_tasks);
			CjvxDataConnectionGroup_genpcg::properties.select_task.value.entries = tasks;
		}
	}

	if (CjvxDataConnectionGroup_genpcg::properties.select_task.value.entries.size())
	{
		jvx_bitZSet(CjvxDataConnectionGroup_genpcg::properties.select_task.value.selection(), 0);
	}
}

jvxErrorType
CjvxDataConnectionsGroup::disconnect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	assert(_common_set_min.theState == JVX_STATE_ACTIVE);

	theContext->unregister_connection_factory(&exposeConnectors);
	auto elm = interceptors_in.begin();
	for (; elm != interceptors_in.end(); elm++)
	{
		exposeConnectors.rem_outside_input_connector(elm->second.ptr);
	}
	auto elmo = interceptors_out.begin();
	for (; elmo != interceptors_out.end(); elmo++)
	{
		exposeConnectors.rem_outside_output_connector(*elmo);
	}
	exposeConnectors.set_object_ref(NULL);

	CjvxDataConnectionGroup_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
	CjvxDataConnectionGroup_genpcg::deallocate_all();

	CjvxDataConnectionProcess_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
	CjvxDataConnectionProcess_genpcg::deallocate_all();

#ifdef JVX_DATACONNECTIONS_VERBOSE
	std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
	std::cout << "DataConnectionProcess <" << _common_set_conn_comm.name << ">" << std::endl;
	std::cout << "++ disconnecting master " << std::flush;
	std::cout << JVX_DISPLAY_MASTER(static_cast<IjvxConnectionMaster*>(_common_set_conn_comm.associated_master)) << std::endl;
#endif

	auto elmi = theInterceptors.begin();
	for (; elmi != theInterceptors.end(); elmi++)
	{
		elmi->ptr->set_tools_host(NULL);
	}

	jvxErrorType resL = CjvxDataConnectionCommon::_deactivate();
	assert(resL == JVX_NO_ERROR);

	// This part would be in deassociate_master in processes
	resL = this->_unselect();
	assert(resL == JVX_NO_ERROR);

#ifdef JVX_DATACONNECTIONS_VERBOSE
	std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
	return JVX_NO_ERROR;

}

void
CjvxDataConnectionsGroup::get_configuration_local(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section)
{
	jvxConfigData* datTmp = NULL;
	IjvxConnectionMasterFactory* masFac = NULL;
	jvxComponentIdentification tpId = JVX_COMPONENT_UNKNOWN;
	jvxApiString strMF, strMa;
	jvxValue val;

	theWriter->createAssignmentString(&datTmp, "JVX_CONNECTION_GROUP_NAME", _common_set_conn_comm.name.c_str());
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	
}

jvxErrorType
CjvxDataConnectionsGroup::create_bridge_check()
{
	return JVX_NO_ERROR;
}

IjvxConnectionMaster*
CjvxDataConnectionsGroup::get_master_ref()
{
	return static_cast<IjvxConnectionMaster*>(NULL);
}

jvxErrorType
CjvxDataConnectionsGroup::remove_bridge_local(jvxSize unique_id)
{
	return _remove_bridge_local(unique_id);
}

jvxErrorType
CjvxDataConnectionsGroup::remove_all_bridges_local()
{
	return _remove_all_bridges_local();
}
void 
CjvxDataConnectionsGroup::about_release_intereptor(CjvxDataChainInterceptor* ptr)
{
	auto elmi = interceptors_in.begin(); 
	for (; elmi != interceptors_in.end(); elmi++)
	{
		if (elmi->second.ptr== ptr)
		{
			interceptors_in.erase(elmi);
			break;
		}
	}

	auto elmo = interceptors_out.begin();
	for (; elmo != interceptors_out.end(); elmo++)
	{
		if (*elmo == ptr)
		{
			interceptors_out.erase(elmo);
			break;
		}
	}
}


// ===========================================================================

jvxErrorType
CjvxDataConnectionsGroup::request_hidden_interface(jvxInterfaceType tp, jvxHandle** ret)
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
CjvxDataConnectionsGroup::return_hidden_interface(jvxInterfaceType tp, jvxHandle* ret)
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
CjvxDataConnectionsGroup::object_hidden_interface(IjvxObject** objRef)
{
	if (objRef)
	{
		*objRef = _common_set.thisisme;
	}
	return JVX_NO_ERROR;
}

// ===============================================================================================

jvxErrorType
CjvxDataConnectionsGroup::link_triggers_connection()
{
	return _link_triggers_connection();
}

jvxErrorType
CjvxDataConnectionsGroup::unlink_triggers_connection()
{
	return _unlink_triggers_connection();
}


jvxErrorType
CjvxDataConnectionsGroup::report_event_interceptor(jvxDataChainReportType tp, jvxSize interceptorid)
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
			for (i = (int)log_num_channels - 1; i >= 0; i--)
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
CjvxDataConnectionsGroup::prepare_local()
{
	log_data = false;

	log_data = (CjvxDataConnectionProcess_genpcg::properties.log_active.value == c_true);
	CjvxDataConnectionProcess_genpcg::properties.duration_msec.value = 0.0;

	/*
	threadMode.myThreadMode = JVX_THREAD_PROC_GROUP_MODE_SEQ_CHAIN_THREADS;
	if (jvx_bitTest(CjvxDataConnectionGroup_genpcg::properties.thread_model.value.selection(), 1))
	{
		threadMode.myThreadMode = JVX_THREAD_PROC_GROUP_MODE_FULL_THREADS_COUPLED; //;JVX_THREAD_PROC_GROUP_MODE_FULL_THREADS;
	}
	else 
	{
		threadMode.myThreadMode = JVX_THREAD_PROC_GROUP_MODE_SEQ_CHAIN_THREADS;
	}
	threadMode.numExtraBuffers = CjvxDataConnectionGroup_genpcg::properties.num_extra_buffers.value;
	*/

	// ===============================================================================================

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

#ifdef JVX_BUFFERING_LOGFILE
		_common_set.theToolsHost->instance_tool(JVX_COMPONENT_DATALOGGER, &logFileObj, 0, NULL);
		if (logFileObj)
		{
			logFileObj->request_specialization(reinterpret_cast<jvxHandle**>(&logFileHdl), NULL, NULL, NULL);
		}
		HjvxDataLogger::_activate(logFileHdl, 100, false, JVX_SIZE_UNSELECTED_INT32);
#endif
	}
}

void
CjvxDataConnectionsGroup::start_local()
{
	JVX_GET_TICKCOUNT_US_SETREF(&tStampRun);
	timestamp_start_us = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStampRun);

#ifdef JVX_BUFFERING_LOGFILE
	std::vector<std::string> errMess;
	std::string fname = CjvxDataConnectionProcess_genpcg::properties.log_fileprefix.value;
	fname = jvx_produceFilenameDateClock(fname);
	fname += ".jvx";
	HjvxDataLogger::_start(fname, errMess, _common_set_min.theHostRef);
#endif
}

void
CjvxDataConnectionsGroup::stop_local()
{
#ifdef JVX_BUFFERING_LOGFILE
	std::vector<std::string> errMess;
	HjvxDataLogger::_stop(NULL, NULL, errMess);
#endif

	timestamp_start_us = JVX_SIZE_UNSELECTED;
}

void
CjvxDataConnectionsGroup::postprocess_local()
{
	if (log_data)
	{
#ifdef JVX_BUFFERING_LOGFILE
		HjvxDataLogger::_deactivate();
		_common_set.theToolsHost->return_instance_tool(JVX_COMPONENT_DATALOGGER, logFileObj, 0, NULL);
		logFileObj = NULL;
		logFileHdl = NULL;
#endif
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
CjvxDataConnectionsGroup::connector_factory_is_involved(IjvxConnectorFactory* rem_this)
{
	if (rem_this == static_cast<IjvxConnectorFactory*>(&exposeConnectors))
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return  _connector_factory_is_involved(rem_this);
}

/*
jvxErrorType
CjvxDataConnectionsGroup::trigger_thread_interceptors(IjvxConnectionMaster* master)
{
	jvxErrorType res = JVX_NO_ERROR;
	while (1)
	{
		jvxBool foundOne = false;
		auto elm = theInterceptors.begin();
		for (; elm != theInterceptors.end(); elm++)
		{
			jvxErrorType resL = elm->ptr->try_run_processing(master);
			if (JVX_NO_ERROR)
			{
				foundOne = true;
				break;
			}
		}
		if (foundOne == false)
		{
			break;
		}
	}
	return res;
}

jvxErrorType 
CjvxDataConnectionsGroup::get_thread_mode(jvxDataConnectionsThreadParameters* threadParameters)
{
	if (threadParameters)
		*threadParameters = threadMode;
	return JVX_NO_ERROR;
}
*/

// ====================================================================================

jvxErrorType
CjvxDataConnectionsGroup::prepare_thread_pipeline(CjvxDataChainInterceptorGroup* entry,
	jvxDataConnectionsObjectIdentification** ptrConnectionId)
{
	jvxErrorType res = JVX_ERROR_UNEXPECTED;
	jvxSize szb = 0;

	auto elm = interceptors_in.begin();
	elm = interceptors_in.find(entry);
	if (elm != interceptors_in.end())
	{
		jvxApiString astr;
		std::string chain_nm;

		// Get the name
		entry->descriptor_connector(&astr);
		chain_nm = astr.std_str();

		std::cout << "Threads in group <" << _common_set.theDescriptor << ">, interceptor <" << astr.std_str() << ">." << std::endl;
		std::cout << " ---> Thread #" << 0 << " (enter thread)." << std::endl;

		JVX_DSP_SAFE_ALLOCATE_OBJECT(elm->second.obj_id, jvxDataConnectionsObjectIdentification);
		elm->second.obj_id->component_id = 0;
		jvx_bitFClear(elm->second.obj_id->thread_id);
		jvx_bitFClear(elm->second.obj_id->thread_id_complete);
		elm->second.obj_id->num_tasks = 2; /* Task 1: end-to-end, Task 2: entry task*/

		res = entry->transfer_forward_icon(
			JVX_LINKDATA_TRANSFER_ENUMERATE_THREADS_CHAIN_IN_GROUP,
			elm->second.obj_id JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		assert(res == JVX_NO_ERROR);

		if (elm->second.obj_id->num_tasks)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(elm->second.obj_id->duration_arvrg_task, jvxData, elm->second.obj_id->num_tasks);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(elm->second.obj_id->duration_max_task, jvxData, elm->second.obj_id->num_tasks);
		}

		threadMode.max_wait_msec = CjvxDataConnectionGroup_genpcg::properties.max_wait_timeout.value;

		// Transfer thread parameters
		res = entry->transfer_forward_icon(
			JVX_LINKDATA_TRANSFER_BROADCAST_THREADING_PARAMETERS_IN_GROUP,
			&threadMode JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		assert(res == JVX_NO_ERROR);

		if (ptrConnectionId)
		{
			*ptrConnectionId = elm->second.obj_id;
		}

		// Copy initial thread strategy and thread model
		elm->second.obj_id->thread_strat = elm->second.thread_strat_set;
		elm->second.obj_id->myThreadMode = elm->second.thread_model_set;

		this->_update_properties_on_start();

		res = JVX_NO_ERROR;
	}

	updateTaskList();

	return res;
}

jvxErrorType
CjvxDataConnectionsGroup::postprocess_thread_pipeline(CjvxDataChainInterceptorGroup* entry)
{
	jvxErrorType res = JVX_ERROR_UNEXPECTED;
	jvxApiString astr;
	jvxSize szb = 0;

	auto elm = interceptors_in.begin();
	elm = interceptors_in.find(entry);
	if (elm != interceptors_in.end())
	{
		/*
		/ * This is not needed when stopping * /
		// Transfer thread parameters
		res = entry->transfer_forward_icon(
			JVX_LINKDATA_TRANSFER_BROADCAST_THREADING_PARAMETERS_IN_GROUP,
			&threadMode JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		assert(res == JVX_NO_ERROR);
		*/

		/*
		/ * This is not needed when stopping * /
		// Find out thread bitfield
		jvx_bitFClear(elm->second.thread_trigger);
		// jvx_bitZSet(elm->second.thread_trigger, 0); // <- thread id 0 for the sync thread on entry and exit
		res = entry->transfer_forward_icon(
			JVX_LINKDATA_TRANSFER_REGISTER_THREADS_CHAIN_IN_GROUP,
			&elm->second.thread_trigger JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		assert(res == JVX_NO_ERROR);
		*/
		updateTaskList();

		if (elm->second.obj_id->num_tasks)
		{
			JVX_DSP_SAFE_DELETE_FIELD(elm->second.obj_id->duration_arvrg_task);
			elm->second.obj_id->duration_arvrg_task = NULL;
			JVX_DSP_SAFE_DELETE_FIELD(elm->second.obj_id->duration_max_task);
			elm->second.obj_id->duration_max_task = NULL;
		}

		jvx_bitFClear(elm->second.obj_id->thread_id);
		elm->second.obj_id->trigger_pts_output.clear();
		elm->second.obj_id->trigger_pts_dedicated.clear();
		JVX_DSP_SAFE_DELETE_OBJECT(elm->second.obj_id);
		elm->second.obj_id = NULL;

		this->_update_properties_on_stop();

		res = JVX_NO_ERROR;
	}

	updateTaskList();

	return res;
}

// ======================================================================================

/*
jvxErrorType
CjvxDataConnectionsGroup::start_thread_pipeline(CjvxDataChainInterceptorGroup* entry)
{
	jvxErrorType res = JVX_ERROR_UNEXPECTED;
	jvxSize szb = 0;
	auto elm = interceptors_in.begin();
	elm = interceptors_in.find(entry);
	if (elm != interceptors_in.end())
	{
		szb = SIZE_MAX;
		res = entry->transfer_forward_icon(
			JVX_LINKDATA_TRANSFER_COUNT_OVERALL_BUFFERS_ADD_PIPELINE,
			&szb JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		assert(res == JVX_NO_ERROR);

		elm->second.num_buffer = szb;
		if (elm->second.num_buffer)
		{
			// Do not set to zero, it is a class with constructors
			JVX_DSP_SAFE_ALLOCATE_FIELD(elm->second.operation_ids, jvxDataConnectionsThreadIdentification, elm->second.num_buffer);
			elm->second.globalIdxx_read = 0;
			elm->second.num_current = 0;
		}
	}
	return res;
}

jvxErrorType
CjvxDataConnectionsGroup::stop_thread_pipeline(CjvxDataChainInterceptorGroup* entry)
{
	jvxErrorType res = JVX_ERROR_UNEXPECTED;
	jvxSize szb = 0;
	auto elm = interceptors_in.begin();
	elm = interceptors_in.find(entry);
	if (elm != interceptors_in.end())
	{
		if (elm->second.num_buffer)
		{
			JVX_DSP_SAFE_DELETE_FIELD(elm->second.operation_ids);
			elm->second.globalIdxx_read = 0;
			elm->second.num_current = 0;
			elm->second.num_buffer = 0;
		}

		/ *
		
		/ * This is only a "get", we need not run it here * /

		res = entry->transfer_forward_icon(
			JVX_LINKDATA_TRANSFER_COUNT_OVERALL_BUFFERS_ADD_PIPELINE,
			&szb JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		assert(res == JVX_NO_ERROR);
		* /

		elm->second.num_buffer = 0;
		res = JVX_NO_ERROR;
	}
	return res;
}
*/

// ===================================================================================
// ===========================================================================================

jvxErrorType
CjvxDataConnectionsGroup::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,  const char* filename, jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		jvxConfigData* cfg = NULL;
		processor->getReferenceSubsectionCurrentSection_name(sectionToContainAllSubsectionsForMe, &cfg, "CONNECTION_GROUP_PROPERTIES");
		if (cfg)
		{
			CjvxDataConnectionGroup_genpcg::put_configuration__properties(callConf, processor,
				cfg, &warns);
			
			for (i = 0; i < warns.size(); i++)
			{
			  std::cout << __FUNCTION__ << ": Warning when reading configuration: " << warns[i] << std::endl;
			}
			warns.clear();

			CjvxDataConnectionProcess_genpcg::put_configuration_all(callConf, processor,
				cfg, &warns);
			for (i = 0; i < warns.size(); i++)
			{
			  std::cout << __FUNCTION__ << ": Warning when reading configuration: " << warns[i] << std::endl;
			}
			warns.clear();

			jvxSize cnt = 0;
			auto elm = interceptors_in.begin();
			while (elm != interceptors_in.end())
			{
				jvxConfigData* theLocalSec = NULL;
				std::string nm = "CONNECTION_GROUP_PROPERTIES_CHAIN_" + jvx_size2String(cnt);

				jvxErrorType res = processor->getReferenceEntryCurrentSection_name(
					sectionToContainAllSubsectionsForMe,
					&theLocalSec, nm.c_str());
				if (res == JVX_NO_ERROR)
				{
					CjvxDataConnectionGroup_genpcg::put_configuration__properties_chain(
						callConf, processor, theLocalSec, &warns);

					elm->second.thread_strat_set =
						CjvxDataConnectionGroup_genpcg::translate__properties_chain__processing_strategy_from();
					elm->second.thread_model_set =
						CjvxDataConnectionGroup_genpcg::translate__properties_chain__thread_model_from();
				}

				elm++;
				cnt++;
			}
		}
		else
		{
		  std::cout << __FUNCTION__ << ": Warning when reading configuration: Failed to read section <CONNECTION_GROUP_PROPERTIES>." << std::endl;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionsGroup::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (JVX_CHECK_SIZE_SELECTED(this->connRuleId))
	{
		jvxValue val;
		jvxConfigData* autoCreate = NULL;
		jvxConfigData* nm = NULL;
		val.assign(true);
		processor->createAssignmentValue(&autoCreate, "CONNECTION_FROM_AUTOSETUP", val);
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, autoCreate);

		processor->createAssignmentString(&nm, "CONNECTION_NAME_REFERENCE", this->_common_set_conn_comm.name.c_str());
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, nm);
	}
	else
	{
		res = CjvxDataConnectionCommon::_get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	}

	if (res == JVX_NO_ERROR)
	{	
		jvxConfigData* cfg = NULL;
		jvxSize cnt = 0;
		processor->createEmptySection(&cfg, "CONNECTION_GROUP_PROPERTIES");
		if (cfg)
		{
			auto elm = interceptors_in.begin();
			while (elm != interceptors_in.end())
			{
				jvxConfigData* theLocalSec = NULL;
				std::string nm = "CONNECTION_GROUP_PROPERTIES_CHAIN_" + jvx_size2String(cnt);
				processor->createEmptySection(&theLocalSec, nm.c_str());

				CjvxDataConnectionGroup_genpcg::translate__properties_chain__processing_strategy_to(
					elm->second.thread_strat_set);
				CjvxDataConnectionGroup_genpcg::translate__properties_chain__thread_model_to(
					elm->second.thread_model_set);
				/*
				CjvxDataConnectionGroup_genpcg::translate__properties_chain__(
					elm->second.thread_strat_set);
				*/

				CjvxDataConnectionGroup_genpcg::get_configuration__properties_chain(
					callConf, processor, theLocalSec);
				processor->addSubsectionToSection(sectionWhereToAddAllSubsections,
					theLocalSec);

				elm++;
				cnt++;
			}
			CjvxDataConnectionGroup_genpcg::get_configuration__properties(callConf, processor,
				cfg);
			CjvxDataConnectionProcess_genpcg::get_configuration_all(callConf, processor,
				cfg);

			processor->addSubsectionToSection(sectionWhereToAddAllSubsections, cfg);
		}
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxDataConnectionsGroup, get_data_prehook)
{
	jvxSize idChain = jvx_bitfieldSelection2Id(CjvxDataConnectionGroup_genpcg::properties.select_chain.value.selection(),
		CjvxDataConnectionGroup_genpcg::properties.select_chain.value.entries.size());

	CjvxDataConnectionGroup_genpcg::properties.avrg_duration_task.value = 0.0;
	CjvxDataConnectionGroup_genpcg::properties.max_duration_task.value = 0.0;
	if (idChain < interceptors_in.size())
	{
		auto elm = interceptors_in.begin();
		std::advance(elm, idChain);
		if (elm->second.obj_id)
		{
			if (ident.id == CjvxDataConnectionGroup_genpcg::properties.avrg_duration_task.globalIdx)
			{
				jvxSize idTask = jvx_bitfieldSelection2Id(CjvxDataConnectionGroup_genpcg::properties.select_task.value.selection(),
					CjvxDataConnectionGroup_genpcg::properties.select_task.value.entries.size());

				if (idTask < elm->second.obj_id->num_tasks)
				{
					CjvxDataConnectionGroup_genpcg::properties.avrg_duration_task.value =
						elm->second.obj_id->duration_arvrg_task[idTask];
				}
			}
			else if (ident.id == CjvxDataConnectionGroup_genpcg::properties.max_duration_task.globalIdx)
			{
				jvxSize idTask = jvx_bitfieldSelection2Id(CjvxDataConnectionGroup_genpcg::properties.select_task.value.selection(),
					CjvxDataConnectionGroup_genpcg::properties.select_task.value.entries.size());

				if (idTask < elm->second.obj_id->num_tasks)
				{
					CjvxDataConnectionGroup_genpcg::properties.max_duration_task.value =
						elm->second.obj_id->duration_max_task[idTask];
				}
			}
			else if (ident.id == CjvxDataConnectionGroup_genpcg::properties.num_fails.globalIdx)
			{
				CjvxDataConnectionGroup_genpcg::properties.num_fails.value = elm->second.obj_id->numRejected;
			}
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxDataConnectionsGroup, get_chain_prehook)
{
	jvxSize idChain = jvx_bitfieldSelection2Id(CjvxDataConnectionGroup_genpcg::properties.select_chain.value.selection(),
		CjvxDataConnectionGroup_genpcg::properties.select_chain.value.entries.size());

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat,
		CjvxDataConnectionGroup_genpcg::properties_chain.processing_strategy))
	{
		// CjvxDataConnectionGroup_genpcg::properties.avrg_duration_task.value = 0.0;
		if (idChain < interceptors_in.size())
		{
			auto elm = interceptors_in.begin();
			std::advance(elm, idChain);
			CjvxDataConnectionGroup_genpcg::translate__properties_chain__processing_strategy_to(elm->second.thread_strat_set);
		}
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat,
		CjvxDataConnectionGroup_genpcg::properties_chain.thread_model))
	{
		// CjvxDataConnectionGroup_genpcg::properties.avrg_duration_task.value = 0.0;
		if (idChain < interceptors_in.size())
		{
			auto elm = interceptors_in.begin();
			std::advance(elm, idChain);
			CjvxDataConnectionGroup_genpcg::translate__properties_chain__thread_model_to(elm->second.thread_model_set);
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxDataConnectionsGroup, set_chain_posthook)
{
	jvxSize idChain = jvx_bitfieldSelection2Id(CjvxDataConnectionGroup_genpcg::properties.select_chain.value.selection(),
		CjvxDataConnectionGroup_genpcg::properties.select_chain.value.entries.size());

	if(JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat,
		CjvxDataConnectionGroup_genpcg::properties_chain.processing_strategy))
	{
		// CjvxDataConnectionGroup_genpcg::properties.avrg_duration_task.value = 0.0;
		if (idChain < interceptors_in.size())
		{
			auto elm = interceptors_in.begin();
			std::advance(elm, idChain);
			elm->second.thread_strat_set = CjvxDataConnectionGroup_genpcg::translate__properties_chain__processing_strategy_from();
			/*
			// Never set directly!
			if (elm->second.obj_id)
			{
				elm->second.obj_id->thread_strat = elm->second.thread_strat_set;
			}*/
		}
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat,
		CjvxDataConnectionGroup_genpcg::properties_chain.thread_model))
	{
		// CjvxDataConnectionGroup_genpcg::properties.avrg_duration_task.value = 0.0;
		if (idChain < interceptors_in.size())
		{
			auto elm = interceptors_in.begin();
			std::advance(elm, idChain);
			elm->second.thread_model_set = CjvxDataConnectionGroup_genpcg::translate__properties_chain__thread_model_from();
			/*
			// Never set directly!
			if (elm->second.obj_id)
			{
				elm->second.obj_id->myThreadMode = elm->second.thread_model_set;
			}*/
		}
	}
	return JVX_NO_ERROR;
}

// ==============================================================================
// ==============================================================================

#ifdef JVX_BUFFERING_LOGFILE

jvxErrorType 
CjvxDataConnectionsGroup::register_instance(const std::string& descr, jvxSize& id_write)
{
	jvxErrorType res = HjvxDataLogger::_add_line(descr, JVX_LOG_PROC_GROUPS_NUM_ENTRIES_UINT32_LOG, JVX_DATAFORMAT_U32BIT_LE, 0, id_write);
	return res;
}

jvxErrorType
CjvxDataConnectionsGroup::memory_instance(jvxUInt32** buffer, jvxSize* num_entries, jvxSize id_write)
{
	jvxErrorType res = HjvxDataLogger::_field_reference(id_write, reinterpret_cast<jvxByte**>(buffer));
	if (num_entries)
		*num_entries = JVX_LOG_PROC_GROUPS_NUM_ENTRIES_UINT32_LOG;
	return res;
}
jvxErrorType 
CjvxDataConnectionsGroup::write_single_instance(jvxSize id_write)
{
	std::string errD;
	jvxErrorType res = HjvxDataLogger::_add_data_reference(id_write, 0, 0, errD);
	return res;
}

#endif
