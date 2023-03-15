#include "jvxNodes/CjvxBareNtask.h"

CjvxBareNtask::CjvxBareNtask(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxNodeBaseNtask(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	jvxOneConnectorTask theTask;
	jvxOneConnectorDefine theConnector;

	// ========================================================================

	theConnector.idCon = JVX_DEFAULT_CON_ID;
	theConnector.nmConnector = "default";

	theTask.inputConnector = theConnector;

	theConnector.idCon = JVX_DEFAULT_CON_ID;
	theConnector.nmConnector = "default";

	theTask.outputConnector = theConnector;

	_common_set_node_ntask.fixedTasks.push_back(theTask);

	// _common_set_node_ntask.numVariableTasks = JVX_SIZE_UNSELECTED;
	// _common_set_node_ntask.offVariableTasks = JVX_DEFAULT_CON_VAR_OFF;
	
	cfg_default_buffer.num_additional_pipleline_stages_cfg = 0;
	cfg_default_buffer.num_min_buffers_in_cfg = 1;
	cfg_default_buffer.num_min_buffers_out_cfg = 1;
	cfg_default_buffer.zeroCopyBuffering_cfg = false;

	if (_common_set.theComponentType.tp == JVX_COMPONENT_UNKNOWN)
	{
		std::cout << __FUNCTION__ << ": Error in configuration of component, no valid type specified in constructor." << std::endl;
	}
	connections = 0;
}
	
CjvxBareNtask::~CjvxBareNtask()
{
}

// ======================================================================
// ======================================================================

jvxErrorType
CjvxBareNtask::activate()
{
	jvxErrorType res = CjvxNodeBaseNtask::activate();
	if (res == JVX_NO_ERROR)
	{
		activate_local_tasks();

		res = CjvxConnectorFactory::_activate_factory(static_cast<IjvxObject*>(this));

		if (res != JVX_NO_ERROR)
		{
			CjvxNodeBaseNtask::deactivate();
		}
	}
	return res;
}



// ====================================================================================

jvxErrorType
CjvxBareNtask::deactivate()
{
	jvxSize i;
	jvxErrorType res = CjvxNodeBaseNtask::deactivate();
	if (res == JVX_NO_ERROR)
	{
		res = CjvxConnectorFactory::_deactivate_factory();

		_common_set_conn_factory.input_connectors.clear();
		_common_set_conn_factory.output_connectors.clear();

		deactivate_local_tasks();

	}
	return res;

}

// ====================================================================================
// ====================================================================================

void
CjvxBareNtask::activate_local_tasks()
{
	jvxSize i;
	for (i = 0; i < _common_set_node_ntask.fixedTasks.size(); i++)
	{
		CjvxInputConnectorNtask* theNewIcon = new CjvxInputConnectorNtask(
			_common_set_node_ntask.fixedTasks[i].inputConnector.idCon,
			_common_set_node_ntask.fixedTasks[i].inputConnector.nmConnector.c_str(),
			static_cast<IjvxConnectorFactory*>(this),
			static_cast<IjvxInputOutputConnectorNtask*>(this),
			_common_set_node_ntask.fixedTasks[i].inputConnector.assocMaster,
			static_cast<IjvxObject*>(this), 0);

		CjvxOutputConnectorNtask* theNewOcon = new CjvxOutputConnectorNtask(
			_common_set_node_ntask.fixedTasks[i].outputConnector.idCon,
			_common_set_node_ntask.fixedTasks[i].outputConnector.nmConnector.c_str(),
			static_cast<IjvxConnectorFactory*>(this),
			static_cast<IjvxInputOutputConnectorNtask*>(this),
			_common_set_node_ntask.fixedTasks[i].outputConnector.assocMaster,
			static_cast<IjvxObject*>(this), 0);

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
	}
}

void
CjvxBareNtask::deactivate_local_tasks()
{
	jvxSize i;
	for (i = 0; i < _common_set_node_ntask.fixedTasks.size(); i++)
	{
		delete(_common_set_node_ntask.fixedTasks[i].icon);
		_common_set_node_ntask.fixedTasks[i].icon = NULL;
		delete(_common_set_node_ntask.fixedTasks[i].ocon);
		_common_set_node_ntask.fixedTasks[i].ocon = NULL;
	}
}

// ======================================================================
// ======================================================================

jvxErrorType
CjvxBareNtask::prepare()
{
	jvxErrorType res = CjvxNodeBaseNtask::prepare();
	if (res == JVX_NO_ERROR)
	{
		_common_set_nvtask_proc.theData_out_master = NULL;
		_common_set_nvtask_proc.theData_in_master = NULL;
		_common_set_nvtask_proc.idx_stage_out_master = JVX_SIZE_UNSELECTED;
		_common_set_nvtask_proc.idx_stage_in_master = JVX_SIZE_UNSELECTED;
		_common_set_nvtask_proc.thread_id_master = JVX_NULLTHREAD;
	}
	return res;
}

jvxErrorType
CjvxBareNtask::postprocess()
{
	jvxErrorType res = CjvxNodeBaseNtask::postprocess();
	if (res == JVX_NO_ERROR)
	{
		_common_set_nvtask_proc.theData_out_master = NULL;
		_common_set_nvtask_proc.theData_in_master = NULL;
		_common_set_nvtask_proc.thread_id_master = JVX_NULLTHREAD;
	}
	return res;
}

// ======================================================================
// ======================================================================

jvxErrorType
CjvxBareNtask::test_connect_ocon_ntask(jvxLinkDataDescriptor* theData_out,
	jvxSize idCtxt, IjvxInputConnector* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string txt;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));
	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		if (refto)
		{
			/* Copy parameters from general node settings. Specify the number of output channels though */
			theData_out->con_params.buffersize = CjvxNode_genpcg::node.buffersize.value;
			theData_out->con_params.rate = CjvxNode_genpcg::node.samplerate.value;
			theData_out->con_params.format = (jvxDataFormat)CjvxNode_genpcg::node.format.value;
			theData_out->con_params.number_channels = CjvxNode_genpcg::node.numberoutputchannels.value;
			theData_out->con_params.segmentation.x = CjvxNode_genpcg::node.segmentsize_x.value;
			theData_out->con_params.segmentation.y = CjvxNode_genpcg::node.segmentsize_y.value;
			theData_out->con_params.format_group = (jvxDataFormatGroup)CjvxNode_genpcg::node.subformat.value;
			res = refto->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res == JVX_NO_ERROR)
			{
				CjvxNode_genpcg::node.buffersize.value = theData_out->con_params.buffersize;
				CjvxNode_genpcg::node.samplerate.value = theData_out->con_params.rate;
				CjvxNode_genpcg::node.format.value = (jvxInt16)theData_out->con_params.format;
				CjvxNode_genpcg::node.numberoutputchannels.value = theData_out->con_params.number_channels;
				CjvxNode_genpcg::node.segmentsize_x.value = theData_out->con_params.segmentation.x;
				CjvxNode_genpcg::node.segmentsize_y.value = theData_out->con_params.segmentation.y;
				CjvxNode_genpcg::node.subformat.value = theData_out->con_params.format_group;
			}
		}
		return res;
	}
	txt = "Object <CjvxBareNtaskentered> tested via secondary chain, this should be handled by derived class.";
	JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), JVX_ERROR_INVALID_SETTING);
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxBareNtask::test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, 
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;
	std::string txt;
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, this);

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		res = neg_primary_input._negotiate_connect_icon(
			theData_in, _common_set.thisisme, "CjvxAuNTask - default chain"
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res == JVX_NO_ERROR)
		{
			CjvxNode_genpcg::node.buffersize.value = neg_primary_input._latest_results.buffersize;
			CjvxNode_genpcg::node.samplerate.value = neg_primary_input._latest_results.rate;
			CjvxNode_genpcg::node.format.value = neg_primary_input._latest_results.format;
			CjvxNode_genpcg::node.numberinputchannels.value = neg_primary_input._latest_results.number_channels;
			CjvxNode_genpcg::node.subformat.value = neg_primary_input._latest_results.format_group;
			CjvxNode_genpcg::node.segmentsize_x.value = neg_primary_input._latest_results.segmentation.x;
			CjvxNode_genpcg::node.segmentsize_y.value = neg_primary_input._latest_results.segmentation.y;

			if (refto)
			{
				// Safe old parameters
				neg_primary_output._push_constraints();

				// Set the constraints for the output side
				neg_primary_output._update_parameters_fixed(
					JVX_SIZE_UNSELECTED,
					CjvxNode_genpcg::node.buffersize.value,
					CjvxNode_genpcg::node.samplerate.value,
					(jvxDataFormat)CjvxNode_genpcg::node.format.value,
					(jvxDataFormatGroup)CjvxNode_genpcg::node.subformat.value);
				
				// This should come out again in test_connect_ocon_id if a link is specified.
				res = refto->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				neg_primary_output._pop_constraints();

				if (res == JVX_NO_ERROR)
				{
					// Number of output channels is variable, all other parameters are fixed in this module
					CjvxNode_genpcg::node.numberoutputchannels.value = JVX_SIZE_INT32(theData_out->con_params.number_channels);
					
					// These are just consistency checks :-)
					assert(CjvxNode_genpcg::node.buffersize.value == JVX_SIZE_INT32(theData_out->con_params.buffersize));
					assert(CjvxNode_genpcg::node.samplerate.value == JVX_SIZE_INT32(theData_out->con_params.rate));
					assert(CjvxNode_genpcg::node.format.value == JVX_SIZE_INT16(theData_out->con_params.format));
					assert(CjvxNode_genpcg::node.subformat.value == JVX_SIZE_INT16(theData_out->con_params.format_group));
					assert(CjvxNode_genpcg::node.segmentsize_x.value == JVX_SIZE_INT32(theData_out->con_params.segmentation.x));
					assert(CjvxNode_genpcg::node.segmentsize_y.value == JVX_SIZE_INT32(theData_out->con_params.segmentation.y));
				}
			}
		}
		return res;
	}

	txt = "Object <CjvxBareNtaskentered> tested via secondary chain, this should be handled by derived class.";
	JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), JVX_ERROR_INVALID_SETTING);
	return JVX_ERROR_UNSUPPORTED;
}

// ==================================================================================================================

jvxErrorType 
CjvxBareNtask::prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, 
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		return prepare_connect_icon_ntask_core(theData_in, theData_out, idCtxt, refto, derived_zeroCopyBuffering_rt, cfg_default_buffer, 0 JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxBareNtask::postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, 
	jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		return postprocess_connect_icon_ntask_core(theData_in, theData_out, idCtxt, refto, derived_zeroCopyBuffering_rt, 0 JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return JVX_ERROR_UNSUPPORTED;
}

// ==================================================================================================================

jvxErrorType 
CjvxBareNtask::start_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, 
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		return start_connect_icon_ntask_core(theData_in, theData_out, idCtxt, refto, 0 JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxBareNtask::stop_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, 
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return stop_connect_icon_ntask_core(theData_in, theData_out, idCtxt, refto, 0 JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	//return refto->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

// ==================================================================================================================

jvxErrorType 
CjvxBareNtask::transfer_forward_icon_ntask(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_in, 
	jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxBool isReady = false;
	jvxErrorType res = JVX_NO_ERROR;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY:
		this->is_ready(&isReady, (jvxApiString*)data);
		if (!isReady)
		{
			res = JVX_ERROR_NOT_READY;
		}
		break;
	}
	if (res == JVX_NO_ERROR)
	{
		if (refto)
		{
			res = refto->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	return res;
}

jvxErrorType 
CjvxBareNtask::transfer_backward_ocon_ntask(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_out, 
	jvxLinkDataDescriptor* theData_in, jvxSize idCtxt, CjvxInputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld = (jvxLinkDataDescriptor*)data;
	jvxBool thereismismatch = false;
	jvxErrorType res = JVX_NO_ERROR;
	std::string txt;
	
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this),
		getConnectorName(idCtxt).c_str(),
		"Entering output connector for prameter negotiations.");

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this),
			_common_set_node_ntask.fixedTasks[0].outputConnector.nmConnector.c_str(), 
			"Entering CjvxAudioDevice default input connector");

		res = neg_primary_output._negotiate_transfer_backward_ocon(ld,
			theData_out, _common_set.thisisme, NULL
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if ((res == JVX_NO_ERROR) || (res == JVX_ERROR_COMPROMISE))
		{
			// If there was a change we need to update the output filter
			neg_primary_output._update_parameters_fixed(
				JVX_SIZE_UNSELECTED,
				theData_out->con_params.buffersize,
				theData_out->con_params.rate,
				(jvxDataFormat)theData_out->con_params.format,
				(jvxDataFormatGroup)theData_out->con_params.format_group);
		}
		// Do nothing here, we need to await the successful output
		return res;
	}
	else
	{
		assert(0);
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxBareNtask::process_buffers_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, 
	CjvxOutputConnectorNtask* refto, jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		// On first run store the thread id for master thread
		if (_common_set_nvtask_proc.thread_id_master == JVX_NULLTHREAD)
		{
			_common_set_nvtask_proc.thread_id_master = JVX_GET_CURRENT_THREAD_ID();
		}

		_common_set_nvtask_proc.idx_stage_in_master = idx_stage;
		if (JVX_CHECK_SIZE_UNSELECTED(_common_set_nvtask_proc.idx_stage_in_master))
		{
			_common_set_nvtask_proc.idx_stage_in_master = *theData_in->con_pipeline.idx_stage_ptr;
		}
		_common_set_nvtask_proc.idx_stage_out_master = *theData_out->con_pipeline.idx_stage_ptr;

		// Store the data sets for input and output in master thread
		_common_set_nvtask_proc.theData_out_master = theData_out;
		_common_set_nvtask_proc.theData_in_master = theData_in;

		res = process_buffers_icon_nvtask_master(theData_in, theData_out, mt_mask, idx_stage);

		_common_set_nvtask_proc.theData_out_master = NULL;
		_common_set_nvtask_proc.theData_in_master = NULL;
		_common_set_nvtask_proc.idx_stage_in_master = JVX_SIZE_UNSELECTED;
		_common_set_nvtask_proc.idx_stage_out_master = JVX_SIZE_UNSELECTED;
	}
	else
	{
		res = process_buffers_icon_ntask_attached(idCtxt, theData_in, theData_out, mt_mask, idx_stage);
	}

	// ===========================================================
	if (res == JVX_NO_ERROR)
	{
		res = refto->process_buffers_ocon(mt_mask, idx_stage);
	}
	return res;
}

jvxErrorType 
CjvxBareNtask::process_buffers_ocon_ntask(jvxSize idCtxt, jvxSize mt_mask, jvxSize idx_stage)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBareNtask::inform_chain_test(IjvxInputConnector* icon)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize processId = JVX_SIZE_UNSELECTED;
	IjvxDataConnectionCommon* dataConn = NULL;

	// Inform the secondary chain to be updated
	if (icon)
	{
		icon->associated_common_icon(&dataConn);
	}

	if (dataConn)
	{
		dataConn->unique_id_connections(&processId);
	}

	if (JVX_CHECK_SIZE_SELECTED(processId))
	{
		res = this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT, (jvxHandle*)(intptr_t)processId);
	}
	return res;
}

// ==============================================================================================0

jvxErrorType
CjvxBareNtask::prepare_connect_icon_ntask_core(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto, jvxBool& zeroCopyBuffering_rt, buffer_cfg& cfg_buffer, jvxSize id_task JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxHost* hh = nullptr;
	if (refto)
	{
		// Handle automatic prepare
		switch (_common_set_min.theState)
		{
		case JVX_STATE_ACTIVE:
			assert(_common_set.stateChecks.prepare_flags == 0);

			hh = reqInterface<IjvxHost>(_common_set_min.theHostRef);
			if (hh)
			{
				res = hh->switch_state_component(_common_set.theComponentType, JVX_STATE_SWITCH_PREPARE);
			}
			else
			{
				res = prepare();
			}

			if (hh)
			{
				retInterface<IjvxHost>(_common_set_min.theHostRef, hh);
			}

			if (res != JVX_NO_ERROR)
			{
				std::string txt = "Implicit prepare failed on object " + _common_set_min.theDescription;
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), res);
				return res;
			}

			jvx_bitSet(_common_set.stateChecks.prepare_flags, id_task);
			break;
		case JVX_STATE_PREPARED:
			if (_common_set.stateChecks.prepare_flags)
			{
				jvx_bitSet(_common_set.stateChecks.prepare_flags, id_task);
			}
			break;
		default:
			return JVX_ERROR_WRONG_STATE;
		}
	}

	// =================================================================

	// Accepts and modify some parameters
	theData_in->con_pipeline.num_additional_pipleline_stages = JVX_MAX(
		theData_in->con_pipeline.num_additional_pipleline_stages,
		cfg_buffer.num_additional_pipleline_stages_cfg);
	theData_in->con_data.number_buffers = JVX_MAX(
		theData_in->con_data.number_buffers,
		theData_in->con_pipeline.num_additional_pipleline_stages + 1);
	theData_in->con_data.number_buffers = JVX_MAX(
		theData_in->con_data.number_buffers,
		cfg_buffer.num_min_buffers_in_cfg);

	zeroCopyBuffering_rt = cfg_buffer.zeroCopyBuffering_cfg;

	if (refto == NULL)
	{
		zeroCopyBuffering_rt = false;
	}

	if (
		(jvx_bitTest(theData_in->con_data.alloc_flags, 
			(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT)) ||
		(jvx_bitTest(theData_in->con_data.alloc_flags, 
			(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT)))
	{
		zeroCopyBuffering_rt = false;
	}

	if (theData_out)
	{
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
			theData_out->con_data.number_buffers = cfg_buffer.num_min_buffers_out_cfg;
			theData_out->con_data.alloc_flags = theData_in->con_data.alloc_flags;
			jvx_bitClear(theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT);
			jvx_bitClear(theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT);
		}

		// We might attach user hints here!!!
		//theData_out->con_data.user_hints = theData_in->con_data.user_hints;
	}
	if (refto)
	{
		res = refto->prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
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
		else
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
	return res;
}

jvxErrorType
CjvxBareNtask::start_connect_icon_ntask_core(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto, jvxSize id_task JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxHost* hh = nullptr;

	if (refto)
	{
		// Handle automatic prepare
		switch (_common_set_min.theState)
		{
		case JVX_STATE_PREPARED:
			assert(_common_set.stateChecks.start_flags == 0);

			hh = reqInterface<IjvxHost>(_common_set_min.theHostRef);
			if (hh)
			{
				res = hh->switch_state_component(_common_set.theComponentType, JVX_STATE_SWITCH_START);
			}
			else
			{
				res = start();
			}
			if (hh)
			{
				retInterface<IjvxHost>(_common_set_min.theHostRef, hh);
			}
			
			if (res != JVX_NO_ERROR)
			{
				std::string txt = "Implicit start failed on object " + _common_set_min.theDescription;
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), res);
				return res;
			}

			jvx_bitSet(_common_set.stateChecks.start_flags, id_task);
			break;
		case JVX_STATE_PROCESSING:
			if (_common_set.stateChecks.start_flags)
			{
				jvx_bitSet(_common_set.stateChecks.start_flags, id_task);
			}
			break;
		default:
			return JVX_ERROR_WRONG_STATE;
		}
	}

	if (refto)
	{
		res = refto->start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxBareNtask::stop_connect_icon_ntask_core(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto, jvxSize id_task JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxHost* hh = nullptr;
	if (refto)
	{
		res = refto->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(res == JVX_NO_ERROR);
	}

	if (refto)
	{
		// Handle automatic prepare
		JVX_ASSERT(_common_set_min.theState == JVX_STATE_PROCESSING);
		if (_common_set.stateChecks.start_flags)
		{
			jvx_bitClear(_common_set.stateChecks.start_flags, id_task);
			
			// Very last instance shuts down state PROCESSING
			if (_common_set.stateChecks.start_flags == 0)
			{
				hh = reqInterface<IjvxHost>(_common_set_min.theHostRef);
				if (hh)
				{
					res = hh->switch_state_component(_common_set.theComponentType, JVX_STATE_SWITCH_STOP);
				}
				else
				{
					res = stop();
				}
				if (hh)
				{
					retInterface<IjvxHost>(_common_set_min.theHostRef, hh);
				}

				if (res != JVX_NO_ERROR)
				{
					std::cout << "Implicit stop failed on object " << _common_set_min.theDescription << std::endl;
				}
			}
		}
	}
	return res;
}

jvxErrorType
CjvxBareNtask::postprocess_connect_icon_ntask_core(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto, jvxBool& zeroCopyBuffering_rt, jvxSize id_task JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxHost* hh = nullptr;
	if (refto)
	{
		res = refto->postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	if (theData_out)
	{
		// We might remove user hints here!!!
		//theData_out->con_data.user_hints = NULL;

		// The only deviation may be the number of output channels - which is taken from the node parameter set
		theData_out->con_data.number_buffers = 0;
	}
	if (zeroCopyBuffering_rt)// <- modified HK from default_zeroCopyBuffering_rt
	{
		res = jvx_deallocate_pipeline_and_buffers_postprocess_to_zerocopy(theData_in);
	}
	else
	{
		res = jvx_deallocate_pipeline_and_buffers_postprocess_to(theData_in);
	}

	theData_in->con_data.number_buffers = 0;
	theData_in->con_pipeline.num_additional_pipleline_stages = 0;

	if (refto)
	{
		// Handle automatic prepare
		assert(_common_set_min.theState == JVX_STATE_PREPARED);
		if (_common_set.stateChecks.prepare_flags)
		{
			jvx_bitClear(_common_set.stateChecks.prepare_flags, id_task);
			if (_common_set.stateChecks.prepare_flags == 0)
			{
				hh = reqInterface<IjvxHost>(_common_set_min.theHostRef);
				if (hh)
				{
					res = hh->switch_state_component(_common_set.theComponentType, JVX_STATE_SWITCH_POSTPROCESS);
				}
				else
				{
					res = postprocess();
				}
				if (res != JVX_NO_ERROR)
				{
					std::cout << "Implicit postprocess failed on object " << _common_set_min.theDescription << std::endl;
				}
			}			
		}
	}
	return res;
}

std::string
CjvxBareNtask::getConnectorName(jvxSize idCTxt)
{
	jvxBool foundIt = false;
	std::string connName = "unknown connector";
	for (auto elm = _common_set_node_ntask.fixedTasks.begin(); elm != _common_set_node_ntask.fixedTasks.end(); elm++)
	{
		if (elm->outputConnector.idCon == idCTxt)
		{
			connName = elm->outputConnector.nmConnector;
			foundIt = true;
			break;
		}
	}
	if (!foundIt)
	{
		/*
		 -> TOBEDONE
		for (auto elm = _common_set_node_ntask.variableTasksDefines.begin(); elm != _common_set_node_ntask.variableTasksDefines.end(); elm++)
		{
			if (elm->outputConnector.idCon == idCTxt)
			{
				connName = elm->outputConnector.nmConnector;
				foundIt = true;
				break;
			}
		}
		*/
	}
	return connName;
}

jvxErrorType
CjvxBareNtask::is_ready(jvxBool* isReady, jvxApiString* reason)
{
	jvxBool isReadyLoc = false;
	jvxErrorType res = CjvxNodeBaseNtask::is_ready(&isReadyLoc, reason);
	if ((res == JVX_NO_ERROR) && (isReadyLoc))
	{
		isReadyLoc = false;
		jvxCBitField compMask = (1 << _common_set_node_ntask.fixedTasks.size()) - 1;
		if(connections == compMask)
		{
			isReadyLoc = true;
		}
		else
		{
			// all connections MUST be connected. 
			std::string txt = this->_common_set.theModuleName + ": Not all connectors connected.";
			if (reason)
				reason->assign(txt.c_str());			
		}
	}
	if (isReady)
		*isReady = isReadyLoc;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxBareNtask::connect_connect_ntask(
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out,
	jvxSize idTask)
{
	switch (idTask)
	{
	case JVX_DEFAULT_CON_ID:
		master.clear();
		jvx_bitSet(connections, 0);
		break;
	case JVX_DEFAULT_CON_VAR_OFF:
		jvx_bitSet(connections, 1);
		break;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxBareNtask::disconnect_connect_ntask( 
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, 
	jvxSize idTask)
{
	switch (idTask)
	{
	case JVX_DEFAULT_CON_ID:
		jvx_bitClear(connections, 0);
		master.clear();
		break;
	case JVX_DEFAULT_CON_VAR_OFF:
		jvx_bitClear(connections, 1);
		break;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBareNtask::process_buffers_icon_nvtask_master(
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out,
	jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

#ifdef VERBOSE_BUFFERING_REPORT
	jvxSize i;
	jvxApiString str;
	jvxSize cs = JVX_SIZE_UNSELECTED;
	std::cout << "Node input <-> output, bidx = " << theData_in->pipeline.idx_stage << "/"
		<< theData_in->con_data.number_buffers
		<< "<->" << theData_out->pipeline.idx_stage << "/"
		<< theData_out->con_data.number_buffers << std::endl;
	std::cout << "Input pipeline:" << std::endl;
	for (i = 0; i < theData_in->con_data.number_buffers; i++)
	{
		cs = theData_in->con_pipeline.reserve_buffer_pipeline_stage[i];
		if (JVX_CHECK_SIZE_SELECTED(cs))
		{
			_common_set.theToolsHost->descriptor_unique_id(cs, &str);
			std::cout << "[is#" << i << "->" << str.std_str() << "]" << std::flush;
		}
		else
		{
			std::cout << "[is#" << i << "->*]" << std::flush;
		}
	}
	std::cout << std::endl;
	std::cout << "Output pipeline:" << std::endl;
	for (i = 0; i < theData_out->con_data.number_buffers; i++)
	{
		cs = theData_out->con_pipeline.reserve_buffer_pipeline_stage[i];
		if (JVX_CHECK_SIZE_SELECTED(cs))
		{
			_common_set.theToolsHost->descriptor_unique_id(cs, &str);
			std::cout << "[os#" << i << "->" << str.std_str() << "]" << std::flush;
		}
		else
		{
			std::cout << "[os#" << i << "->*]" << std::flush;
		}
	}
	std::cout << std::endl;
#endif

	if (!derived_zeroCopyBuffering_rt)
	{
		jvxSize idx_stage_in = idx_stage;
		if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_in))
		{
			idx_stage_in = *theData_in->con_pipeline.idx_stage_ptr;
		}
		jvxSize idx_stage_out = *theData_out->con_pipeline.idx_stage_ptr;

		// Talkthrough
		jvxSize ii;
		jvxSize maxChans = JVX_MAX(theData_in->con_params.number_channels, theData_out->con_params.number_channels);
		jvxSize minChans = JVX_MIN(theData_in->con_params.number_channels, theData_out->con_params.number_channels);

		// This default function does not tolerate a lot of unexpected settings
		assert(theData_in->con_params.format == theData_out->con_params.format);
		assert(theData_in->con_params.buffersize == theData_out->con_params.buffersize);

		if (minChans)
		{
			for (ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % theData_in->con_params.number_channels;
				jvxSize idxOut = ii % theData_out->con_params.number_channels;

				assert(theData_in->con_params.format == theData_out->con_params.format);
				jvx_convertSamples_memcpy(
					theData_in->con_data.buffers[idx_stage_in][idxIn],
					theData_out->con_data.buffers[idx_stage_out][idxOut],
					jvxDataFormat_size[theData_in->con_params.format],
					theData_in->con_params.buffersize);
			}
		}

		if (res != JVX_NO_ERROR)
		{
			return res;
		}
	}
	return res;
}

jvxErrorType
CjvxBareNtask::process_buffers_icon_ntask_attached(
	jvxSize ctxtId,
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out,
	jvxSize mt_mask, jvxSize idx_stage)
{
	return JVX_NO_ERROR;
}