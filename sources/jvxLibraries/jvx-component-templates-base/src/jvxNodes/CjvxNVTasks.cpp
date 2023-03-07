#include "jvx.h"
#include "jvxNodes/CjvxNVTasks.h"

#define ALPHA_SMOOTH 0.7

// =======================================================================

/*
	CjvxInputConnectorNtask* theNewIcon = new CjvxInputConnectorNtask(
		_common_set_node_ntask.fixedTasks[i].inputConnector.idCon,
		_common_set_node_ntask.fixedTasks[i].inputConnector.nmConnector.c_str(),
		static_cast<IjvxConnectorFactory*>(this),
		static_cast<CjvxCommonInputOutputConnectorNtask_report*>(this),
		_common_set_node_ntask.fixedTasks[i].inputConnector.assocMaster,
		static_cast<IjvxObject*>(this));

	CjvxOutputConnectorNtask* theNewOcon = new CjvxOutputConnectorNtask(
		_common_set_node_ntask.fixedTasks[i].outputConnector.idCon,
		_common_set_node_ntask.fixedTasks[i].outputConnector.nmConnector.c_str(),
		static_cast<IjvxConnectorFactory*>(this),
		static_cast<CjvxCommonInputOutputConnectorNtask_report*>(this),
		_common_set_node_ntask.fixedTasks[i].outputConnector.assocMaster,
		static_cast<IjvxObject*>(this));

	theNewIcon->set_reference(theNewOcon);
	theNewOcon->set_reference(theNewIcon);

	_common_set_node_ntask.fixedTasks[i].icon = theNewIcon;
	_common_set_node_ntask.fixedTasks[i].ocon = theNewOcon;

	oneInputConnectorElement newElmIn;
	newElmIn.theConnector = theNewIcon;
	_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn;

	oneOutputConnectorElement newElmOut;
	newElmOut.theConnector = theNewOcon;
	_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut;
*/

CjvxNVTasks::CjvxNVTasks(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): CjvxBareNtask(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	jvxOneVariableConnectorTaskDefinition theVarTask;
	
	theVarTask.name = "async-sec";
	theVarTask.descriptor_in.push_back("async-secondary-v");
	theVarTask.descriptor_out.push_back("async-secondary-v");
	theVarTask.idOffThisTasks = JVX_NVTASK_OFFSET_VARIABLE_TASKS;
	theVarTask.withMaster = false;
	theVarTask.reportSelect = static_cast<IjvxInputOutputConnectorVtask*>(this);

	_common_set_node_vtask.variableTasks[theVarTask.idOffThisTasks] = theVarTask;

	JVX_INITIALIZE_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
	_common_set_nv_proc.uId_proc_tasks = 1;

	/*
	jvxOneConnectorTask theTask;
	jvxOneConnectorDefine theConnector;

	theConnector.idCon = JVX_DEFAULT_CON_VAR_OFF;
	theConnector.nmConnector = "async-secondary";

	theTask.inputConnector = theConnector;

	theConnector.idCon = JVX_DEFAULT_CON_VAR_OFF;
	theConnector.nmConnector = "async-secondary";

	theTask.outputConnector = theConnector;
	_common_set_node_ntask.fixedTasks.push_back(theTask);
	*/

	sec_input = NULL;
	pri_input = NULL;

	cfg_sec_buffer.num_additional_pipleline_stages_cfg = 0;
	cfg_sec_buffer.num_min_buffers_in_cfg = 1;
	cfg_sec_buffer.num_min_buffers_out_cfg = 1;
	cfg_sec_buffer.zeroCopyBuffering_cfg = false;
	sec_zeroCopyBuffering_rt = false;

	auto_parameters.buffersize_pri_to_sec = true; // <- requires also the asyncio component to be configured properly
	auto_parameters.rate_pri_to_sec = true;

	combinSignalMode = jvxCombineSignalsMode::JVX_COMBINE_SIGNALS_MODE_MIX;

	parameter_config = JVX_AUNTASKS_PRIMARY_MASTER;

}

CjvxNVTasks::~CjvxNVTasks()
{
	JVX_TERMINATE_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
}

// ===========================================================================================
// ===========================================================================================

#ifdef JVX_CJVXNVTASKS_UPDATE_MASTER_RATE
jvxErrorType
CjvxNVTasks::prepare()
{
	jvxErrorType res = CjvxBareNtask::prepare();
	if (res == JVX_NO_ERROR)
	{
		JVX_GET_TICKCOUNT_US_SETREF(&tStampRates);
		tStampRates_last_out = JVX_SIZE_UNSELECTED;
	}
	return res;
}

jvxErrorType
CjvxNVTasks::postprocess()
{
	return CjvxBareNtask::postprocess();
}
#endif

// ===================================================================

jvxErrorType
CjvxNVTasks::activate()
{
	jvxErrorType res = CjvxBareNtask::activate();
	jvxSize num = 0;
	jvxApiString strDescr;
	jvxApiString strDescror;
	jvxBool multInst = false;
	jvxComponentIdentification tpId;
	IjvxDataConnections* theConnections = NULL;
	jvxVariant var;
	jvxApiString apiStr;
	std::string txt;
	
	if (res == JVX_NO_ERROR)
	{
		sec_input = connector_input_name("async-secondary-v");
		pri_input = connector_input_name("default");

		CjvxNVTasks_pcg::init_all();
		CjvxNVTasks_pcg::allocate_all();
		CjvxNVTasks_pcg::register_all(static_cast<CjvxProperties*>(this));
		CjvxNVTasks_pcg::register_callbacks(static_cast<CjvxProperties*>(this),
			select_task, reinterpret_cast<jvxHandle*>(this), NULL);

		update_exposed_tasks();	
	}
	return res;
}

void 
CjvxNVTasks::activate_local_tasks()
{
	CjvxBareNtask::activate_local_tasks();

	auto elmT = _common_set_node_vtask.variableTasks.begin();
	for (; elmT != _common_set_node_vtask.variableTasks.end(); elmT++)
	{
		jvxSize cnt = 0;
		std::list< oneConnectorVTask_con<CjvxInputConnectorVtask> > newLstI;
		std::list< oneConnectorVTask_con<CjvxOutputConnectorVtask> > newLstO;
		auto elms = elmT->second.descriptor_in.begin();
		for (; elms != elmT->second.descriptor_in.end(); elms++)
		{
			oneConnectorVTask_con<CjvxInputConnectorVtask> newIn;
			newIn.con = new CjvxInputConnectorVtask(
				elmT->second.idOffThisTasks,
				elms->c_str(),
				static_cast<IjvxConnectorFactory*>(this),
				static_cast<IjvxInputOutputConnectorVtask*>(this),
				NULL, 
				static_cast<IjvxObject*>(this),
				cnt++,
				CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_EXPOSE);
			newLstI.push_back(newIn);
		}

		cnt = 0;
		elms = elmT->second.descriptor_out.begin();
		for (; elms != elmT->second.descriptor_out.end(); elms++)
		{
			oneConnectorVTask_con<CjvxOutputConnectorVtask> newOut;
			newOut.con = new CjvxOutputConnectorVtask(
				elmT->second.idOffThisTasks,
				elms->c_str(),
				static_cast<IjvxConnectorFactory*>(this),
				static_cast<IjvxInputOutputConnectorVtask*>(this),
				NULL,
				static_cast<IjvxObject*>(this),
				cnt++,
				CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_EXPOSE);
			newLstO.push_back(newOut);
		}

		elmT->second.icon = newLstI;
		elmT->second.ocon = newLstO;

		// Add to connector fractory

		auto elmli = elmT->second.icon.begin();
		for (; elmli != elmT->second.icon.end(); elmli++)
		{
			oneInputConnectorElement newElmIn;
			newElmIn.theConnector = elmli->con;
			_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn;
		}

		auto elmlo = elmT->second.ocon.begin();
		for (; elmlo != elmT->second.ocon.end(); elmlo++)
		{
			oneOutputConnectorElement newElmOut;
			newElmOut.theConnector = elmlo->con;
			_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut;
		}
		elmT->second.numCurrentTasks = 0;
	}
}

// ==========================================================================================

jvxErrorType
CjvxNVTasks::deactivate()
{
	jvxErrorType res = CjvxBareNtask::_pre_check_deactivate();
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	std::vector<jvxSize> lst_uids_remove;

	if (res == JVX_NO_ERROR)
	{
		CjvxNVTasks_pcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxNVTasks_pcg::deallocate_all();

		sec_input = NULL;
		pri_input = NULL;
		res = CjvxBareNtask::deactivate();
	}
	return res;
}

void
CjvxNVTasks::deactivate_local_tasks()
{
	// Once we are here, the connector factory has been completely cleared
	CjvxBareNtask::deactivate_local_tasks();
	auto elmT = _common_set_node_vtask.variableTasks.begin();
	for (; elmT != _common_set_node_vtask.variableTasks.end(); elmT++)
	{
		auto elmi = elmT->second.icon.begin();
		for (; elmi != elmT->second.icon.end(); elmi++)
		{
			delete elmi->con;
			elmi->con = NULL;
		}
		elmT->second.icon.clear();

		auto elmo = elmT->second.ocon.begin();
		for (; elmo != elmT->second.ocon.end(); elmo++)
		{
			delete elmo->con;
			elmo->con = NULL;
		}
		elmT->second.ocon.clear();
	}
}

// ===========================================================================================
// ===========================================================================================

jvxErrorType 
CjvxNVTasks::connect_connect_icon_vtask(
	jvxSize idTask,
	jvxSize subIdTask
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	//jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxErrorType res = JVX_NO_ERROR;
	auto elmT = _common_set_node_vtask.variableTasks.begin();
	for (; elmT != _common_set_node_vtask.variableTasks.end(); elmT++)
	{
		auto elmR = elmT->second.activeRuntimeTasks.begin();
		for (; elmR != elmT->second.activeRuntimeTasks.end(); elmR++)
		{
			if (elmR->second.idCtxt == idTask)
			{
				assert(
					(subIdTask < elmR->second.ocons.size()) && 
					(subIdTask < elmR->second.icons.size()));
				CjvxOutputConnectorVtask* ocon = (elmR->second.ocons[subIdTask].con);
				CjvxInputConnectorVtask* icon = (elmR->second.icons[subIdTask].con);
				
				elmR->second.icons[subIdTask].neg._update_parameters_fixed(
						JVX_SIZE_UNSELECTED, // No channel specification
						master.bsize,
						master.rate,
						master.format,
						master.format_group,
						master.data_flow,
						nullptr);
				elmR->second.ocons[subIdTask].neg._update_parameters_fixed(
					JVX_SIZE_UNSELECTED, // No channel specification
					master.bsize,
					master.rate,
					master.format,
					master.format_group,
					master.data_flow,
					nullptr);

				if (icon)
				{
					jvxApiString astr;
					IjvxDataConnectionCommon* ctxt = nullptr;
					jvxLinkDataDescriptor* datIn = icon->_common_set_icon_nvtask.theData_in;
					if (datIn->con_link.master)
					{			
						// Check the association via the connection context - it may differ from the master name
						datIn->con_link.master->connection_context(&ctxt);
						if (ctxt)
						{
							ctxt->connection_association(&astr, &elmR->second.icons[subIdTask].ctxt_cpTp);
							elmR->second.icons[subIdTask].ctxt_tag = astr.std_str();
						}
					}
					if (ocon)
					{
						ctxt = nullptr;
						
						res = elmR->second.ocons[subIdTask].con->connect_connect_ocon(jvxChainConnectArguments(datIn->con_link.uIdConn, datIn->con_link.master)
							JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
						if (res != JVX_NO_ERROR)
						{
							break;
						}
						if (ocon->_common_set_ocon_nvtask.theData_out.con_link.master)
						{
							// Check the association via the connection context - it may differ from the master name
							ocon->_common_set_ocon_nvtask.theData_out.con_link.master->connection_context(&ctxt);
							if (ctxt)
							{
								ctxt->connection_association(&astr, &elmR->second.ocons[subIdTask].ctxt_cpTp);
								elmR->second.ocons[subIdTask].ctxt_tag = astr.std_str();
							}
						}
					}
				}
			}
		}
	}
	return res;
}

jvxErrorType 
CjvxNVTasks::disconnect_connect_icon_vtask(
	jvxSize idTask,
	jvxSize subIdTask
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elmT = _common_set_node_vtask.variableTasks.begin();
	for (; elmT != _common_set_node_vtask.variableTasks.end(); elmT++)
	{
		auto elmR = elmT->second.activeRuntimeTasks.begin();
		for (; elmR != elmT->second.activeRuntimeTasks.end(); elmR++)
		{
			if (elmR->second.idCtxt == idTask)
			{
				assert(
					(subIdTask < elmR->second.ocons.size()) && 
					(subIdTask < elmR->second.icons.size()));
				CjvxOutputConnectorVtask* ocon = (elmR->second.ocons[subIdTask].con);
				CjvxInputConnectorVtask* icon = (elmR->second.icons[subIdTask].con);
				if (icon && ocon)
				{
					jvxLinkDataDescriptor* datIn = icon->_common_set_icon_nvtask.theData_in;
					res = elmR->second.ocons[subIdTask].con->disconnect_connect_ocon(jvxChainConnectArguments(datIn->con_link.uIdConn, datIn->con_link.master)
						JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					return res;
				}
			}
		}
	}
	return JVX_NO_ERROR;
}

// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxNVTasks::test_connect_icon_vtask(
	jvxSize ctxtId,
	jvxSize ctxtSubId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;

	res = find_i_map(ctxtId, ctxtSubId, icon, ocon);
	if (res == JVX_NO_ERROR)
	{
		jvxLinkDataDescriptor* datIn = icon->con->_common_set_icon_nvtask.theData_in;		
		if(datIn->con_params.format_group == JVX_DATAFORMAT_GROUP_TRIGGER_ONLY)
		{ 
			// Trigger only mode means that no input data is expected, only the chain is 
			// requested to trigger the output processing
			icon->trigger_only_mode = true;
			auto elm = ocon.begin();
			for (; elm != ocon.end(); elm++)
			{
				// VTasks accept also input-only connections!
				if ((*elm)->con)
				{
					jvxLinkDataDescriptor* datOut = &(*elm)->con->_common_set_ocon_nvtask.theData_out;

					// All processing parameters are taken from primary chain
					datOut->con_params.buffersize = (*elm)->neg.preferred.buffersize.min;
					datOut->con_params.rate = (*elm)->neg.preferred.samplerate.min;
					datOut->con_params.segmentation_x = (*elm)->neg.preferred.dimX.min;
					datOut->con_params.segmentation_y = (*elm)->neg.preferred.dimY.min;
					datOut->con_params.format = (*elm)->neg.preferred.format.min;
					datOut->con_params.format_group = (*elm)->neg.preferred.subformat.min;

					// Taking the number of output channels from the master chain (copy)
					// We may setup a different number of channels afterwards
					datOut->con_params.number_channels = CjvxNode_genpcg::node.numberoutputchannels.value;

					this->test_set_output_parameters(ctxtId, ctxtSubId, datOut);

					res = (*elm)->con->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
					if (res != JVX_NO_ERROR)
					{
						break;
					}
				}
			}
		}
		else
		{
			res = icon->neg._negotiate_connect_icon(datIn, this, "descr" JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res == JVX_NO_ERROR)
			{
				this->accept_input_settings_vtask_start(datIn, icon, ocon);
				auto elm = ocon.begin();
				for (; elm != ocon.end(); elm++)
				{
					// VTasks accept also input-only connections!
					if ((*elm)->con)
					{
						res = (*elm)->con->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
						if (res != JVX_NO_ERROR)
						{
							break;
						}
					}
				}
				this->accept_input_settings_vtask_stop(ocon);
			}
		}
		return res;
	}
	return res;
}

jvxErrorType
CjvxNVTasks::test_connect_ocon_vtask(
	jvxSize ctxtId,
	jvxSize ctxtSubId,
	jvxLinkDataDescriptor* theDataOut
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (theDataOut->con_link.connect_to)
	{
		res = theDataOut->con_link.connect_to->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

void 
CjvxNVTasks::test_set_output_parameters(jvxSize ctxtId, jvxSize ctxtSubId, jvxLinkDataDescriptor* theDataOut)
{
}

// ===========================================================================================
// ===========================================================================================

jvxErrorType 
CjvxNVTasks::transfer_forward_icon_vtask(
	jvxLinkDataTransferType tp,
	jvxHandle* data,
	jvxSize ctxtId,
	jvxSize ctxtSubId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;

	res = find_i_map(ctxtId, ctxtSubId, icon, ocon);
	if (res == JVX_NO_ERROR)
	{
		res = this->transfer_forward_icon_vtask_core(tp, data, icon, ocon
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType 
CjvxNVTasks::transfer_backward_ocon_vtask(jvxLinkDataTransferType tp, jvxHandle* data, 
	jvxSize ctxtId,
	jvxSize ctxtSubId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));

	oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>*  ocon = NULL;
	std::list<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* > icon;

	res = find_o_map(ctxtId, ctxtSubId, icon, ocon);
	if (res == JVX_NO_ERROR)
	{
		res = this->transfer_backward_ocon_vtask_core(tp, data, ocon, icon
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType
CjvxNVTasks::prepare_connect_icon_vtask(
	jvxSize ctxtId,
	jvxSize ctxtSubId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));

	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;

	res = find_i_map(ctxtId, ctxtSubId, icon, ocon);
	if (res == JVX_NO_ERROR)
	{
		res = this->prepare_connect_icon_vtask_core(icon, ocon
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType
CjvxNVTasks::start_connect_icon_vtask(
	jvxSize ctxtId,
	jvxSize ctxtSubId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));

	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;

	res = find_i_map(ctxtId, ctxtSubId, icon, ocon);
	if (res == JVX_NO_ERROR)
	{
		res = this->start_connect_icon_vtask_core(icon, ocon
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType
CjvxNVTasks::stop_connect_icon_vtask(
	jvxSize ctxtId,
	jvxSize ctxtSubId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));

	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;

	res = find_i_map(ctxtId, ctxtSubId, icon, ocon);
	if (res == JVX_NO_ERROR)
	{
		res = this->stop_connect_icon_vtask_core(icon, ocon
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType
CjvxNVTasks::postprocess_connect_icon_vtask(
	jvxSize idCtxt,
	jvxSize ctxtSubId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));

	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;

	res = find_i_map(idCtxt, ctxtSubId, icon, ocon);
	if (res == JVX_NO_ERROR)
	{
		res = this->postprocess_connect_icon_vtask_core(icon, ocon
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

// ===================================================================================================

jvxErrorType
CjvxNVTasks::process_start_icon_vtask(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr,
	jvxSize procId)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	// -> _common_set_nv_proc.safeAcces_proc_tasks); no need to lock here, we are triggered from within the protected loop
	auto elm = _common_set_nv_proc.lst_in_proc_tasks.find(procId);
	auto elmO = elm->second.ocon.begin();
	for (; elmO != elm->second.ocon.end(); elmO++)
	{
		if ((*elmO)->con)
		{
			res = (*elmO)->con->process_start_ocon(pipeline_offset,
				idx_stage,
				tobeAccessedByStage,
				clbk,
				priv_ptr);
			if (res != JVX_NO_ERROR)
				break;
		}
		else
		{
			res = JVX_NO_ERROR;
		}
	}
	return res;
}

jvxErrorType
CjvxNVTasks::process_stop_icon_vtask(
	jvxSize idx_stage,
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr,
	jvxSize procId)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	// -> _common_set_nv_proc.safeAcces_proc_tasks); no need to lock here, we are triggered from within the protected loop
	auto elm = _common_set_nv_proc.lst_in_proc_tasks.find(procId);
	auto elmO = elm->second.ocon.begin();
	for (; elmO != elm->second.ocon.end(); elmO++)
	{
		if ((*elmO)->con)
		{
			res = (*elmO)->con->process_stop_ocon(
				idx_stage,
				shift_fwd,
				tobeAccessedByStage,
				clbk,
				priv_ptr);
			if (res != JVX_NO_ERROR)
				break;
		}
		else
		{
			res = JVX_NO_ERROR;
		}
	}
	return res;
}

// ===================================================================================================
// ===================================================================================================

/*
 * VTasks need not be connected. Therefore, this module is ready if the NTask condition is met.
 */
jvxErrorType
CjvxNVTasks::is_ready(jvxBool* isReady, jvxApiString* reason)
{
	jvxBool isReadyLoc = false;

	// There is a different constraint for VTask than for NTask:
	// At least the main task must be connected for startup
	jvxErrorType res = CjvxBareNtask::is_ready(&isReadyLoc, reason);
	if ((res == JVX_NO_ERROR) && (isReadyLoc))
	{
		isReadyLoc = false;
		if (connections & 0x1)
		{
			isReadyLoc = true;
		}
		else
		{
			std::string txt = this->_common_set.theModuleName + ": at least the main chain must be connected.";
			if (reason)
				reason->assign(txt.c_str());
		}
	}
	if (isReady)
		*isReady = isReadyLoc;
	return JVX_NO_ERROR;
}

void 
CjvxNVTasks::update_exposed_tasks()
{
	jvxSize selOld = jvx_bitfieldSelection2Id(
		CjvxNVTasks_pcg::selected_vtask_properties.variable_tasks.select.value.selection(),
		CjvxNVTasks_pcg::selected_vtask_properties.variable_tasks.select.value.entries.size()
	);

	CjvxNVTasks_pcg::selected_vtask_properties.variable_tasks.select.value.entries.clear();

	auto elmT = _common_set_node_vtask.variableTasks.begin();
	for (; elmT != _common_set_node_vtask.variableTasks.end(); elmT++)
	{
		auto elmR = elmT->second.activeRuntimeTasks.begin();
		for (; elmR != elmT->second.activeRuntimeTasks.end(); elmR++)
		{
			jvxSize id = elmR->second.idCtxt - elmT->second.idOffThisTasks;
			std::string txtShow = elmT->second.name;
			if (id > 0)
			{
				txtShow += "<" + jvx_size2String(id) + ">";
			}
			CjvxNVTasks_pcg::selected_vtask_properties.variable_tasks.select.value.entries.push_back(txtShow);
		}
	}
	if (selOld >= CjvxNVTasks_pcg::selected_vtask_properties.variable_tasks.select.value.entries.size())
	{
		selOld = (jvxSize)JVX_MIN((int)CjvxNVTasks_pcg::selected_vtask_properties.variable_tasks.select.value.entries.size() - 1, 0);
	}
	if(JVX_CHECK_SIZE_SELECTED(selOld))
	{
		jvx_bitZSet(CjvxNVTasks_pcg::selected_vtask_properties.variable_tasks.select.value.selection(), selOld);
	}
}

jvxErrorType
CjvxNVTasks::test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
	jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	// This function is called once the main audio device 
	// is connected to the main (ntask) connector
	// You may see the input and the output parameters ad you may 
	// correct those
	jvxErrorType res = CjvxBareNtask::test_connect_icon_ntask(theData_in,
		theData_out,
		idCtxt, refto,
		JVX_CONNECTION_FEEDBACK_CALL(fdb));

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		if (res == JVX_NO_ERROR)
		{
			// The buffersize may be aligned but does not necessarily have to be
			master.bsize = JVX_SIZE_UNSELECTED;
			if (CjvxNVTasks_pcg::configuration.align_buffersize_0Vtask.value == c_true)
			{
				master.bsize = theData_out->con_params.buffersize;
			}
			master.rate = theData_out->con_params.rate;
			master.format = theData_out->con_params.format;
			master.format_group = theData_out->con_params.format_group;
		}
		// Dependend settings: if on primary channel the parameters were changed we will need the same 
		// settings on all secondary channels
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elmT = _common_set_node_vtask.variableTasks.begin();
		for (; elmT != _common_set_node_vtask.variableTasks.end(); elmT++)
		{

			auto elmR = elmT->second.activeRuntimeTasks.begin();
			for (; elmR != elmT->second.activeRuntimeTasks.end(); elmR++)
			{
				// Input connectors
				for (auto& elmI : elmR->second.icons)
				{
					elmI.neg._update_parameters_fixed(
						JVX_SIZE_UNSELECTED, // No channel specification
						master.bsize,
						master.rate,
						master.format,
						master.format_group,
						master.data_flow,
						nullptr);
				}

				// Output connectors
				for (auto& elmO : elmR->second.ocons)
				{
					elmO.neg._update_parameters_fixed(
						JVX_SIZE_UNSELECTED, // No channel specification
						master.bsize,
						master.rate,
						master.format,
						master.format_group,
						master.data_flow,
						nullptr);
				}

				for (auto& elmI : elmR->second.icons)
				{
					// Update the connected chains
					inform_chain_test(elmI.con);
				}
			}
		}
	}
	return res;
}
