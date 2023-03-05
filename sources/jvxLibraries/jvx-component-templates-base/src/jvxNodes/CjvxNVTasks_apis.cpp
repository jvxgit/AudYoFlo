#include "jvxNodes/CjvxNVTasks.h"
#include "CjvxJson.h"

#define JVX_ALPHA_RATE 0.95

void
CjvxNVTasks::accept_input_settings_vtask_start(jvxLinkDataDescriptor* datIn, 
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* & icon,
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon)
{
	jvxLinkDataDescriptor* datOut = NULL;
	auto elm = ocon.begin();
	for (; elm != ocon.end(); elm++)
	{
		// Check this. There are also input-only-devices!
		if ((*elm)->con)
		{
			datOut = &(*elm)->con->_common_set_ocon_nvtask.theData_out;
			icon->params = datIn->con_params;
			datOut->con_params = datIn->con_params;
			(*elm)->neg._push_constraints();
			(*elm)->neg._update_parameters_fixed(JVX_SIZE_UNSELECTED,
				datIn->con_params.buffersize,
				datIn->con_params.rate,
				datIn->con_params.format,
				datIn->con_params.caps.format_group,
				datOut);
		}
	}
	show_properties_io_task(NULL);
}

void
CjvxNVTasks::accept_input_settings_vtask_stop(
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon)
{
	auto elm = ocon.begin();
	for (; elm != ocon.end(); elm++)
	{
		// Push only happened if there is a following connection
		if ((*elm)->con)
		{
			(*elm)->neg._pop_constraints();
		}
	}
}


void 
CjvxNVTasks::accept_output_settings_vtask_start(jvxLinkDataDescriptor* datOut,
	oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>*& ocon,
	std::list<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* >& icon)
{
	ocon->params = datOut->con_params;
	show_properties_io_task(NULL);
}

void 
CjvxNVTasks::accept_output_settings_vtask_stop(
	std::list<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* >& icon)
{
}

jvxErrorType
CjvxNVTasks::find_i_map(jvxSize ctxtId, jvxSize ctxtSubId,
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>*>& ocon)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elmT = _common_set_node_vtask.variableTasks.begin();
	for (; elmT != _common_set_node_vtask.variableTasks.end(); elmT++)
	{
		auto elmR = elmT->second.activeRuntimeTasks.begin();
		for (; elmR != elmT->second.activeRuntimeTasks.end(); elmR++)
		{
			if (elmR->second.idCtxt == ctxtId)
			{
				icon = &elmR->second.icons[ctxtSubId];

				// Here is the simplification: return only the associated output connector in a one to one match
				ocon.push_back( &elmR->second.ocons[ctxtSubId]);
				res = JVX_NO_ERROR;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxNVTasks::find_o_map(jvxSize ctxtId, jvxSize ctxtSubId,
	std::list<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*>& icon,
	oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>*& ocon)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elmT = _common_set_node_vtask.variableTasks.begin();
	for (; elmT != _common_set_node_vtask.variableTasks.end(); elmT++)
	{
		auto elmR = elmT->second.activeRuntimeTasks.begin();
		for (; elmR != elmT->second.activeRuntimeTasks.end(); elmR++)
		{
			if (elmR->second.idCtxt == ctxtId)
			{
				ocon = &elmR->second.ocons[ctxtSubId];

				// Here is the simplification: return only the associated output connector in a one to one match
				icon.push_back(&elmR->second.icons[ctxtSubId]);
				res = JVX_NO_ERROR;
			}
		}
	}
	return res;
}

void
CjvxNVTasks::show_properties_io_task_vtask(const jvxOneConnectorTaskRuntime& task)
{
	auto elmi = task.icons.begin();
	auto elmo = task.ocons.begin();

	CjvxNVTasks_pcg::selected_vtask_properties.node.numberinputchannels.value = 0;
	CjvxNVTasks_pcg::selected_vtask_properties.node.numberoutputchannels.value = 0;
	CjvxNVTasks_pcg::selected_vtask_properties.node.buffersize.value = 0;
	CjvxNVTasks_pcg::selected_vtask_properties.node.format.value = JVX_DATAFORMAT_NONE;
	CjvxNVTasks_pcg::selected_vtask_properties.node.subformat.value = JVX_DATAFORMAT_GROUP_NONE;
	CjvxNVTasks_pcg::selected_vtask_properties.node.samplerate.value = 0;
	CjvxNVTasks_pcg::selected_vtask_properties.node.segmentsize_x.value = 0;
	CjvxNVTasks_pcg::selected_vtask_properties.node.segmentsize_y.value = 0;

	assert(elmi != task.icons.end());
	assert(elmo != task.ocons.end());

	CjvxNVTasks_pcg::selected_vtask_properties.node.numberinputchannels.value = elmi->params.number_channels;
	CjvxNVTasks_pcg::selected_vtask_properties.node.buffersize.value = (jvxInt32)elmi->params.buffersize;
	CjvxNVTasks_pcg::selected_vtask_properties.node.samplerate.value = (jvxInt32)elmi->params.rate;
	CjvxNVTasks_pcg::selected_vtask_properties.node.format.value = elmi->params.format;
	CjvxNVTasks_pcg::selected_vtask_properties.node.subformat.value = elmi->params.caps.format_group;
	CjvxNVTasks_pcg::selected_vtask_properties.node.segmentsize_x.value = elmi->params.segmentation_x;
	CjvxNVTasks_pcg::selected_vtask_properties.node.segmentsize_y.value = (jvxInt32)elmi->params.segmentation_y;
	CjvxNVTasks_pcg::selected_vtask_properties.node.numberoutputchannels.value = (jvxInt32)elmo->params.number_channels;
}

jvxErrorType
CjvxNVTasks::transfer_forward_icon_vtask_core(jvxLinkDataTransferType tp, jvxHandle* data,
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxBool isReady = false;
	jvxErrorType res = JVX_NO_ERROR;
	std::string locTxt;
	jvxApiString* str = (jvxApiString*)data;
	CjvxJsonElementList* jsonLst = (CjvxJsonElementList*)data;
	CjvxJsonElement jsonElm;
	jvxBool forwardStraight = false;

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COLLECT_LINK_STRING:

		locTxt = str->std_str();
		locTxt += "object input connector " + JVX_DISPLAY_CONNECTOR(icon->con) + "<" +
			jvx_size2String(icon->con->common_vtask._common_set_comnvtask.ctxtId) + "," +
			jvx_size2String(icon->con->common_vtask._common_set_comnvtask.ctxtSubId) + ">";

		if (icon->con->_common_nvtask->_common_set_comnvtask.associatedMaster)
		{
			locTxt += "<end>";
		}
		else
		{
			jvxSize cnt = 0;
			auto elm = ocon.begin();
			for (; elm != ocon.end(); elm++)
			{
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				locTxt += "-<int<" + jvx_size2String(cnt) + ">->";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				str->assign(locTxt);

				(*elm)->con->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

				locTxt = str->std_str();
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				locTxt += "<-<tni<" + jvx_size2String(cnt) + ">->";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				cnt++;
			}
		}
		str->assign(locTxt);
		break;

	case JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:

		jsonElm.makeAssignmentString("icon", JVX_DISPLAY_CONNECTOR(icon->con) + "<" +
			jvx_size2String(icon->con->common_vtask._common_set_comnvtask.ctxtId) + "," +
			jvx_size2String(icon->con->common_vtask._common_set_comnvtask.ctxtSubId) + ">");
		jsonLst->addConsumeElement(jsonElm);

		if (icon->con->_common_nvtask->_common_set_comnvtask.associatedMaster)
		{
			CjvxJsonElementList jsonLstRet;
			jsonElm.makeAssignmentString("connects_end", "here");
			jsonLst->addConsumeElement(jsonElm);
		}
		else
		{
			CjvxJsonArray jsonArr;
			jvxSize cnt = 0;
			auto elm = ocon.begin();
			for (; elm != ocon.end(); elm++)
			{
				CjvxJsonArrayElement jsonArrElm;
				CjvxJsonElementList jsonLstRet;

				res = (*elm)->con->transfer_forward_ocon(tp, &jsonLstRet JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res != JVX_NO_ERROR)
				{
					break;
				}
				jsonArrElm.makeSection(jsonLstRet);
				jsonArr.addConsumeElement(jsonArrElm);
			}
			if (res == JVX_NO_ERROR)
			{
				jsonElm.makeArray("connects_from", jsonArr);
			}
			else
			{
				jsonElm.makeAssignmentString("connects_error", jvxErrorType_txt(res));
			}
		}
		jsonLst->addConsumeElement(jsonElm);
		break;

	case JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY:
		this->is_ready(&isReady, (jvxApiString*)data);
		if (!isReady)
		{
			res = JVX_ERROR_NOT_READY;
		}
		else
		{
			forwardStraight = true;
		}
		break;
	default:

		forwardStraight = true;
	}

	if(forwardStraight)
	{
		if (!icon->con->_common_nvtask->_common_set_comnvtask.associatedMaster)
		{
			auto elm = ocon.begin();
			for (; elm != ocon.end(); elm++)
			{
				if ((*elm)->con)
				{
					res = (*elm)->con->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res != JVX_NO_ERROR)
					{
						break;
					}
				}
			}
		}
	}
	return res;
}

jvxErrorType
CjvxNVTasks::transfer_backward_ocon_vtask_core(jvxLinkDataTransferType tp, jvxHandle* data,
	oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>*& ocon,
	std::list<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* >& icon
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
	{
		jvxLinkDataDescriptor& datOut = ocon->con->_common_set_ocon_nvtask.theData_out;
		jvxLinkDataDescriptor* ld = (jvxLinkDataDescriptor*)data;

		res = ocon->neg._negotiate_transfer_backward_ocon(ld, &datOut,
			this, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res == JVX_NO_ERROR)
		{
			this->accept_output_settings_vtask_start(&datOut, ocon, icon);
			this->accept_output_settings_vtask_stop(icon);
		}
	}
	break;
	default:
	{
		auto elm = icon.begin();
		for (; elm != icon.end(); elm++)
		{
			res = (*elm)->con->transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
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
CjvxNVTasks::prepare_connect_icon_vtask_core(
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
			
	// Handle automatic prepare
	switch (_common_set_min.theState)
	{
	case JVX_STATE_ACTIVE:
		assert(_common_set.stateChecks.prepare_flags == 0);

		res = prepare();
		if (res != JVX_NO_ERROR)
		{
			std::string txt = "Implicit prepare failed on object " + _common_set_min.theDescription;
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), res);
			return res;
		}

		jvx_bitSet(_common_set.stateChecks.prepare_flags, _common_set_node_ntask.fixedTasks.size());
		break;
	case JVX_STATE_PREPARED:
	case JVX_STATE_PROCESSING:
		if (_common_set.stateChecks.prepare_flags)
		{
			jvxSize cnt = _common_set_node_ntask.fixedTasks.size();
			while (jvx_bitTest(_common_set.stateChecks.prepare_flags, cnt))
			{
				cnt++;
				assert(cnt < JVX_NUMBITS_CBITFIELD);
			}
			jvx_bitSet(_common_set.stateChecks.prepare_flags, cnt);
		}
		break;
	default:
		return JVX_ERROR_WRONG_STATE;
	}


	// =================================================================
	jvxLinkDataDescriptor* theData_in = icon->con->_common_set_icon_nvtask.theData_in;
	jvxSize addPipelineStage = 0;
	jvxSize num_min_buffers_in = 0;
	jvxSize num_min_buffers_out = 1;
	jvxBool zeroCopyBuffering_rt = false;

	// Accepts and modify some parameters
	theData_in->con_pipeline.num_additional_pipleline_stages = JVX_MAX(
		theData_in->con_pipeline.num_additional_pipleline_stages, addPipelineStage);
	theData_in->con_data.number_buffers = JVX_MAX(
		theData_in->con_data.number_buffers,
		theData_in->con_pipeline.num_additional_pipleline_stages + 1);
	theData_in->con_data.number_buffers = JVX_MAX(
		theData_in->con_data.number_buffers, num_min_buffers_in);

	if (
		(jvx_bitTest(theData_in->con_data.alloc_flags, 
			(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT)) ||
		(jvx_bitTest(theData_in->con_data.alloc_flags, 
			(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT)))
	{
		zeroCopyBuffering_rt = false;
	}

	if (ocon.size() > 1)
	{
		zeroCopyBuffering_rt = false;
	}

	auto elm = ocon.begin();
	for (; elm != ocon.end(); elm++)
	{
		// No connection is also an option!
		if ((*elm)->con)
		{
			jvxLinkDataDescriptor* theData_out = &(*elm)->con->_common_set_ocon_nvtask.theData_out;

			if (zeroCopyBuffering_rt)
			{
				// Set the number of buffers on the output identical to the input side
				theData_out->con_data.number_buffers = theData_in->con_data.number_buffers;
				theData_out->con_data.alloc_flags = theData_in->con_data.alloc_flags;
				///jvx_bitClear(_common_set_ldslave.theData_out.con_data.alloc_flags, JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT)
			}
			else
			{
				// Set the number of buffers as desired
				theData_out->con_data.number_buffers = num_min_buffers_out;
				theData_out->con_data.alloc_flags = theData_in->con_data.alloc_flags;
				jvx_bitClear(theData_in->con_data.alloc_flags, 
					(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT);
				jvx_bitClear(theData_in->con_data.alloc_flags, 
					(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT);
			}

			// We might attach user hints here!!!
			//theData_out->con_data.user_hints = theData_in->con_data.user_hints;
			res = (*elm)->con->prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res == JVX_NO_ERROR)
			{
				if (zeroCopyBuffering_rt)
				{
					// Link the buffers directly to output
					res = jvx_allocate_pipeline_and_buffers_prepare_to_zerocopy(theData_in, theData_out);
					assert(res == JVX_NO_ERROR);
					if (jvx_bitTest(theData_out->con_data.alloc_flags, 
						(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT))
					{
						jvx_bitSet(theData_in->con_data.alloc_flags, 
							(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);
					}
				}
			}
		}
		else
		{
			zeroCopyBuffering_rt = false;
		}
	}
	if(res == JVX_NO_ERROR)
	{
		if(!zeroCopyBuffering_rt)
		{
			res = jvx_allocate_pipeline_and_buffers_prepare_to(theData_in
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
				, _common_set.theModuleName.c_str()
#endif
			);
			assert(res == JVX_NO_ERROR);
			jvx_bitClear(theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);
		}
	}
	
	if (res != JVX_NO_ERROR)
	{
		// What to on error?
	}
	return res;
}

jvxErrorType
CjvxNVTasks::start_connect_icon_vtask_core(
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Handle automatic prepare
	switch (_common_set_min.theState)
	{
	case JVX_STATE_PREPARED:
		assert(_common_set.stateChecks.start_flags == 0);

		res = start();
		if (res != JVX_NO_ERROR)
		{
			std::string txt = "Implicit start failed on object " + _common_set_min.theDescription;
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), res);
			return res;
		}

		jvx_bitSet(_common_set.stateChecks.start_flags, _common_set_node_ntask.fixedTasks.size());
		break;
	case JVX_STATE_PROCESSING:
		if (_common_set.stateChecks.start_flags)
		{
			jvxSize cnt = _common_set_node_ntask.fixedTasks.size();
			while (jvx_bitTest(_common_set.stateChecks.start_flags, cnt))
			{
				cnt++;
				assert(cnt < JVX_NUMBITS_CBITFIELD);
			}
			jvx_bitSet(_common_set.stateChecks.start_flags, cnt);
		}
		break;
	default:
		return JVX_ERROR_WRONG_STATE;
	}

	// =================================================================
	// jvxLinkDataDescriptor* theData_in = icon->con->_common_set_icon_nvtask.theData_in;
	auto elm = ocon.begin();
	for (; elm != ocon.end(); elm++)
	{
		// No output connection is also an option!
		if ((*elm)->con)
		{
			jvxLinkDataDescriptor* theData_out = &(*elm)->con->_common_set_ocon_nvtask.theData_out;

			res = (*elm)->con->start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res != JVX_NO_ERROR)
			{
				break;
			}
		}
	}

	if (res != JVX_NO_ERROR)
	{
		// What to on error?
	}
	else
	{
		jvxApiString astr;
		oneEntryProcessingVTask newElm;

		jvxSize uid = JVX_SIZE_UNSELECTED;

		jvxLinkDataDescriptor* theData_in = nullptr;
		IjvxDataConnectionCommon* ctxt = nullptr;
		if (icon->con)
		{
			theData_in = icon->con->_common_set_icon_nvtask.theData_in;
		}
		if (theData_in)
		{
			if (theData_in->con_link.master)
			{
				theData_in->con_link.master->connection_context(&ctxt);
			}
		}
		if (ctxt)
		{
			ctxt->unique_id_connections(&uid);
		}

		newElm.icon = icon;
		newElm.ocon = ocon;
		newElm.connId = JVX_SIZE_UNSELECTED;

		if (icon)
		{
			if (icon->con)
			{
				if (icon->con->_common_set_icon_nvtask.theData_in)
				{
					newElm.connId = icon->con->_common_set_icon_nvtask.theData_in->con_link.uIdConn;
				}
			}
		}
		
		// Here we may update the connection associations
#if 0
		if (icon)
		{
			if (icon->con->_common_set_icon_nvtask.theData_in)
			{
				if (icon->con->_common_set_icon_nvtask.theData_in->con_link.master)
				{
					IjvxDataConnectionCommon* ctxt = nullptr;
					icon->con->_common_set_icon_nvtask.theData_in->con_link.master->connection_context(&ctxt);
					if (ctxt)
					{
						ctxt->connection_association(&astr, &icon->ctxt_cpTp);
						icon->ctxt_tag = astr.std_str();
					}
				}
			}
		}

		for (oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* elmLoc: ocon)
		{
			if (elmLoc->con)
			{
				if (elmLoc->con->_common_set_ocon_nvtask.theData_out.con_link.master)
				{
					IjvxDataConnectionCommon* ctxt = nullptr;
					elmLoc->con->_common_set_ocon_nvtask.theData_out.con_link.master->connection_context(&ctxt);
					if (ctxt)
					{
						ctxt->connection_association(&astr, &elmLoc->ctxt_cpTp);
						elmLoc->ctxt_tag = astr.std_str();
					}
				}
			}
		}
#endif
		JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
		newElm.uid = _common_set_nv_proc.uId_proc_tasks++;
		icon->con->in_proc.id = newElm.uid;
		_common_set_nv_proc.lst_in_proc_tasks[newElm.uid] = newElm;
		safe_callback_start_connection(newElm.connId);
		JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
	}
	return res;
}

jvxErrorType
CjvxNVTasks::stop_connect_icon_vtask_core(
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize uId = icon->con->in_proc.id;
	
	// Remove element from shortcut list
	JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
	auto elmC = _common_set_nv_proc.lst_in_proc_tasks.find(uId);
	assert(elmC != _common_set_nv_proc.lst_in_proc_tasks.end());
	
	safe_callback_stop_connection(elmC->second.connId);
	
	_common_set_nv_proc.lst_in_proc_tasks.erase(elmC);
	JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);

	// =================================================================
	// jvxLinkDataDescriptor* theData_in = icon->con->_common_set_icon_nvtask.theData_in;
	auto elm = ocon.begin();
	for (; elm != ocon.end(); elm++)
	{
		// No output is also an option
		if ((*elm)->con)
		{
			jvxLinkDataDescriptor* theData_out = &(*elm)->con->_common_set_ocon_nvtask.theData_out;

			res = (*elm)->con->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res != JVX_NO_ERROR)
			{
				break;
			}
		}
	}

	JVX_ASSERT(_common_set_min.theState == JVX_STATE_PROCESSING);
	if (_common_set.stateChecks.start_flags)
	{
		
		int cnt = JVX_NUMBITS_CBITFIELD - 1; // must have sign!!!
		while (cnt >= _common_set_node_ntask.fixedTasks.size())
		{
			if (jvx_bitTest(_common_set.stateChecks.start_flags, cnt))
			{
				break;
			}
			cnt--;
		}
		assert(cnt >= _common_set_node_ntask.fixedTasks.size());
		jvx_bitClear(_common_set.stateChecks.start_flags, cnt);

		// Very last instance shuts down state PROCESSING
		if (_common_set.stateChecks.start_flags == 0)
		{
			res = stop();
			if (res != JVX_NO_ERROR)
			{
				std::cout << "Implicit stop failed on object " << _common_set_min.theDescription << std::endl;
			}
		}
	}

	return res;
}

// ======================================================================================

void
CjvxNVTasks::safe_callback_start_connection(jvxSize uId)
{

}

void 
CjvxNVTasks::safe_callback_stop_connection(jvxSize uId)
{
}

// ======================================================================================

jvxErrorType
CjvxNVTasks::postprocess_connect_icon_vtask_core(
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool zeroCopyBuffering_rt = false;
	jvxLinkDataDescriptor* theData_in = icon->con->_common_set_icon_nvtask.theData_in;

	auto elm = ocon.begin();
	for (; elm != ocon.end(); elm++)
	{
		// No connector is also an option
		if ((*elm)->con)
		{
			jvxLinkDataDescriptor* theData_out = &(*elm)->con->_common_set_ocon_nvtask.theData_out;

			res = (*elm)->con->postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res != JVX_NO_ERROR)
			{
				break;
			}
		}
	}

	if (zeroCopyBuffering_rt)// <- modified HK from default_zeroCopyBuffering_rt
	{
		res = jvx_deallocate_pipeline_and_buffers_postprocess_to_zerocopy(theData_in);
	}
	else
	{
		// This covers all cases without zerocopy
		res = jvx_deallocate_pipeline_and_buffers_postprocess_to(theData_in);
	}

	theData_in->con_data.number_buffers = 0;
	theData_in->con_pipeline.num_additional_pipleline_stages = 0;

	// Handle automatic prepare
	assert(
		(_common_set_min.theState == JVX_STATE_PREPARED) ||
		(_common_set_min.theState == JVX_STATE_PROCESSING));

	if (_common_set.stateChecks.prepare_flags)
	{
		int cnt = JVX_NUMBITS_CBITFIELD - 1; // must have sign!!!
		while (cnt >= _common_set_node_ntask.fixedTasks.size())
		{
			if (jvx_bitTest(_common_set.stateChecks.prepare_flags, cnt))
			{
				break;
			}
			cnt--;
		}
		assert(cnt >= _common_set_node_ntask.fixedTasks.size());
		jvx_bitClear(_common_set.stateChecks.prepare_flags, cnt);

		// Very last instance shuts down state PROCESSING
		if (_common_set.stateChecks.prepare_flags == 0)
		{
			res = postprocess();
			if (res != JVX_NO_ERROR)
			{
				std::cout << "Implicit postprocess failed on object " << _common_set_min.theDescription << std::endl;
			}
		}
	}
	return res;
}

