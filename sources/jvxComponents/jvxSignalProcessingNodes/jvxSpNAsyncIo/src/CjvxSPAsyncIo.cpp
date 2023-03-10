#include "jvx.h"
#include "CjvxSPAsyncIo.h"

//#include "mcg_exports_matlab.h"

JVX_ASYNCIO_CLASSNAME::JVX_ASYNCIO_CLASSNAME(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxBareNtask(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{	
	JVX_NBARETASK_ATTACH_TASK(JVX_DEFAULT_CON_VAR_OFF, "async-secondary", this);
	sec_output = NULL;
	sec_input = NULL;
	pri_input = NULL;
	pri_output = NULL;

	cfg_sec_buffer.num_additional_pipleline_stages_cfg = 0;
	cfg_sec_buffer.num_min_buffers_in_cfg = 1;
	cfg_sec_buffer.num_min_buffers_out_cfg = 1;
	cfg_sec_buffer.zeroCopyBuffering_cfg = false;
	sec_zeroCopyBuffering_rt = false;

	jvxDspBaseErrorType resL = jvx_audio_stack_sample_dispenser_cont_initCfg(&inProcessing.crossThreadTransfer.myAudioDispenser);
	assert(resL == JVX_DSP_NO_ERROR);

	inProcessing.fHeight.numberEventsConsidered_perMinMaxSection = 10;
	inProcessing.fHeight.num_MinMaxSections = 5;
	inProcessing.fHeight.recSmoothFactor = 0.95;
	inProcessing.fHeight.numOperations = 0;

	JVX_GET_TICKCOUNT_US_SETREF(&inProcessing.tstamp);
	inProcessing.tstamp_init = false;

	inProcessing.prof_prim.fHeightOnEnter = JVX_SIZE_UNSELECTED;
	inProcessing.prof_prim.fHeightOnLeave = JVX_SIZE_UNSELECTED;
	inProcessing.prof_prim.state_onEnter = JVX_SIZE_UNSELECTED;
	inProcessing.prof_prim.state_onLeave = JVX_SIZE_UNSELECTED;
	inProcessing.prof_prim.tStamp = JVX_SIZE_UNSELECTED;

	inProcessing.prof_sec.fHeightOnEnter = JVX_SIZE_UNSELECTED;
	inProcessing.prof_sec.fHeightOnLeave = JVX_SIZE_UNSELECTED;
	inProcessing.prof_sec.state_onEnter = JVX_SIZE_UNSELECTED;
	inProcessing.prof_sec.state_onLeave = JVX_SIZE_UNSELECTED;
	inProcessing.prof_sec.tStamp = JVX_SIZE_UNSELECTED;


	JVX_INITIALIZE_MUTEX(inProcessing.crossThreadTransfer.safeAccess_lock);
	JVX_INITIALIZE_MUTEX(inProcessing.crossThreadTransfer.safeAccessSecLink);
	inProcessing.crossThreadTransfer.secLinkRead = false;

#ifdef JVX_ASYNCIO_MASTER_PRI
	auto_mode = autoConfigurationMode::JVX_ASYNC_MODE_PRIMARY_MASTER;
#else
	auto_mode = autoConfigurationMode::JVX_ASYNC_MODE_SECONDARY_MASTER;
#endif

}

JVX_ASYNCIO_CLASSNAME::~JVX_ASYNCIO_CLASSNAME()
{
	JVX_TERMINATE_MUTEX(inProcessing.crossThreadTransfer.safeAccessSecLink);
	JVX_TERMINATE_MUTEX(inProcessing.crossThreadTransfer.safeAccess_lock);
}

// ========================================================================================
// ========================================================================================

jvxErrorType
JVX_ASYNCIO_CLASSNAME::activate()
{
#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif
	//std::vector<std::string> warns;
	jvxSize i;
	jvxApiString str;
	jvxErrorType res = CjvxBareNtask::activate();
	if (res == JVX_NO_ERROR)
	{
		genSPAsyncio_node::init_all();
		genSPAsyncio_node::allocate_all();
		genSPAsyncio_node::register_all(static_cast<CjvxProperties*>(this));
		genSPAsyncio_node::register_callbacks(
			static_cast<CjvxProperties*>(this),
			set_control_flow_param,
			static_cast<jvxHandle*>(this),
			NULL);

#ifdef JVX_NTASK_ENABLE_LOGFILE
		resL = logWriter_main_signal.lfc_activate(static_cast<IjvxObject*>(this),
			static_cast<CjvxProperties*>(this), _common_set_min.theHostRef, "main_log", 100, 500,
			"- Main Signal Logfile -", "log_main", "Activate - Main Signal Logfile", "Prefix - Main Signal Logfile");

		if (resL != JVX_NO_ERROR)
		{
			_report_text_message("Failed to activate main logfile writer!", JVX_REPORT_PRIORITY_WARNING);
		}
		resL = logWriter_timing.lfc_activate(static_cast<IjvxObject*>(this), 
			static_cast<CjvxProperties*>(this), _common_set_min.theHostRef, "timing_log", 100, 500,
			"- Timing Logfile -", "log_timing", "Activate - Timing Logfile", "Prefix - Timing Logfile");
		if (resL != JVX_NO_ERROR)
		{
			_report_text_message("Failed to activate timing logfile writer!", JVX_REPORT_PRIORITY_WARNING);
		}
#endif
		std::map<IjvxOutputConnector*, oneOutputConnectorElement>::iterator elm = _common_set_conn_factory.output_connectors.begin();
		std::map<IjvxInputConnector*, oneInputConnectorElement>::iterator elm_in = _common_set_conn_factory.input_connectors.begin();

		for (;elm != _common_set_conn_factory.output_connectors.end(); elm++)
		{
			elm->second.theConnector->descriptor_connector(&str);
			if (str.std_str() == "default")
			{
				pri_output = elm->second.theConnector;
			}
			if (str.std_str() == "async-secondary")
			{
				sec_output = elm->second.theConnector;
			}
		}

		for (; elm_in != _common_set_conn_factory.input_connectors.end(); elm_in++)
		{
			elm_in->second.theConnector->descriptor_connector(&str);
			
			if (str.std_str() == "default")
			{
				pri_input = elm_in->second.theConnector;
			}
			if (str.std_str() == "async-secondary")
			{
				sec_input = elm_in->second.theConnector;
			}
		}
		assert(sec_input);
		assert(sec_output);
		assert(pri_input);
		assert(pri_output);
		JVX_ACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER(static_cast<CjvxObject*>(this), "async-secondary");
	}
	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::deactivate()
{
#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif
	jvxErrorType res = CjvxBareNtask::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		sec_input = NULL;
		sec_output = NULL;
		pri_input = NULL;
		pri_output = NULL;

		genSPAsyncio_node::unregister_all(static_cast<CjvxProperties*>(this));
		genSPAsyncio_node::deallocate_all();

#ifdef JVX_NTASK_ENABLE_LOGFILE
		resL = logWriter_main_signal.lfc_deactivate();
		resL = logWriter_timing.lfc_deactivate();
#endif
		res = CjvxBareNtask::deactivate();
	}
	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::prepare()
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = CjvxBareNtask::prepare();
	if (res == JVX_NO_ERROR)
	{
		inProcessing.tstamp_init = false;

		inProcessing.prof_prim.fHeightOnEnter = JVX_SIZE_UNSELECTED;
		inProcessing.prof_prim.fHeightOnLeave = JVX_SIZE_UNSELECTED;
		inProcessing.prof_prim.state_onEnter = JVX_SIZE_UNSELECTED;
		inProcessing.prof_prim.state_onLeave = JVX_SIZE_UNSELECTED;
		inProcessing.prof_prim.tStamp = JVX_SIZE_UNSELECTED;

		inProcessing.prof_sec.fHeightOnEnter = JVX_SIZE_UNSELECTED;
		inProcessing.prof_sec.fHeightOnLeave = JVX_SIZE_UNSELECTED;
		inProcessing.prof_sec.state_onEnter = JVX_SIZE_UNSELECTED;
		inProcessing.prof_sec.state_onLeave = JVX_SIZE_UNSELECTED;
		inProcessing.prof_sec.tStamp = JVX_SIZE_UNSELECTED;

		// The allocation must be within the prepare function - before ANY of the chains is connected!!
		resL = startAudioSync_nolock(CjvxNode_genpcg::node.buffersize.value, CjvxNode_genpcg::node.samplerate.value, (jvxDataFormat)CjvxNode_genpcg::node.format.value,
			CjvxNode_genpcg::node.numberinputchannels.value,
			CjvxNode_genpcg::node.numberoutputchannels.value);
		assert(resL == JVX_NO_ERROR);

		jvxLinkDataAttachedLostFrames_allocateRuntime(inProcessing.attachedData,
			reinterpret_cast<jvxHandle*>(this),
			static_release_attached);
		inProcessing.cntLost = 0;

#ifdef JVX_NTASK_ENABLE_LOGFILE
		if (res == JVX_NO_ERROR)
		{
			resL = logWriter_timing.lfc_prepare_one_channel(1,
				JVX_NUMBER_ENTRIES_TIMING, JVX_SIZE_UNSELECTED,
				JVX_DATAFORMAT_SIZE, "timing", idTiming);

			resL = logWriter_timing.lfc_start_all_channels();
		}
#endif
	}
	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::postprocess()
{
#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif
	jvxErrorType res = CjvxBareNtask::postprocess();
	if (res == JVX_NO_ERROR)
	{
#ifdef JVX_NTASK_ENABLE_LOGFILE
		resL = logWriter_timing.lfc_stop_all_channels();
		resL = logWriter_timing.lfc_postprocess_all_channels();
#endif
		jvxLinkDataAttachedLostFrames_deallocateRuntime(
			inProcessing.attachedData);
		inProcessing.cntLost = 0;

		jvxErrorType resL = stopAudioSync_nolock();
		assert(resL == JVX_NO_ERROR);
	}
	return res;
}
// ========================================================================================
// ========================================================================================

jvxErrorType 
JVX_ASYNCIO_CLASSNAME::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{

	case JVX_INTERFACE_MANIPULATE:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxManipulate*>(this));
		break;

	case JVX_INTERFACE_CONNECTOR_MASTER_FACTORY:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectionMasterFactory*>(this));
		break;

	default:
		res = CjvxBareNtask::request_hidden_interface(tp, hdl);
		break;
	}
	return(res);
}

jvxErrorType 
JVX_ASYNCIO_CLASSNAME::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_MANIPULATE:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_INTERFACE_CONNECTOR_MASTER_FACTORY:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectionMasterFactory*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	default:
		res = CjvxBareNtask::return_hidden_interface(tp, hdl);
		break;
	}
	return(res);
};

// ========================================================================================
// ========================================================================================

jvxErrorType 
JVX_ASYNCIO_CLASSNAME::set_manipulate_value(jvxSize id, jvxVariant* varray)
{
	jvxApiString* ptrString = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	if (!varray)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (id)
	{
	case JVX_MANIPULATE_DESCRIPTION:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			_common_set_min.theDescription = ptrString->std_str();
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	case JVX_MANIPULATE_DESCRIPTOR:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			_common_set_min.theDescription = ptrString->std_str();
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	default: 
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::get_manipulate_value(jvxSize id, jvxVariant* varray)
{
	jvxApiString* ptrString = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	if (!varray)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (id)
	{
	case JVX_MANIPULATE_DESCRIPTION:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			 ptrString->assign(_common_set_min.theDescription);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	case JVX_MANIPULATE_DESCRIPTOR:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			ptrString->assign(_common_set_min.theDescription);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

// ========================================================================================
// ========================================================================================

jvxErrorType
JVX_ASYNCIO_CLASSNAME::connect_connect_ntask(
	jvxLinkDataDescriptor* theData_in,	
	jvxLinkDataDescriptor* theData_out,
	jvxSize idTask)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString astr;

	IjvxDataConnectionCommon* ctxt = nullptr;
	theData_in->con_link.master->connection_context(&ctxt);
	switch (idTask)
	{
	case JVX_DEFAULT_CON_ID:

		// Provide the connection association to the synchrnized chain
		ctxt->connection_association(&astr, &_common_set_ld_master.cpTpAss);
		_common_set_ld_master.tagAss = astr.std_str();
		ctxtMain = ctxt;
		jvx_bitSet(connections, 0);
		break;
	case JVX_DEFAULT_CON_VAR_OFF:

		ctxtSec = ctxt;
		if (ctxtMain)
		{
			ctxtMain->add_dependency(ctxtSec);
			jvx_bitSet(connections, 1);
		}
		else
		{
			// If we end up here, it was tried to connect the secondary chain before the primary.
			// This should prevent the connect!
			res = JVX_ERROR_NOT_READY;
		}
		break;
	}

	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::disconnect_connect_ntask(
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, 
	jvxSize idTask)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (idTask)
	{
	case JVX_DEFAULT_CON_ID:
		jvx_bitClear(connections, 0);
		ctxtMain = nullptr;
		break;
	case JVX_DEFAULT_CON_VAR_OFF:
		if (ctxtMain)
		{
			ctxtMain->remove_dependency(ctxtSec);
		}
		jvx_bitClear(connections, 1);
		break;
	}
	return res;
}

// ========================================================================================
// ========================================================================================

jvxErrorType
JVX_ASYNCIO_CLASSNAME::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Output parameters can not be specified here since we do not have access to the connector
	return _test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool param_overwritten = false;
	jvxBool test_chain_now = false;
	jvxSize num_channels = JVX_SIZE_UNSELECTED;
	jvxConstraintSetResult resU = JVX_NEGOTIATE_CONSTRAINT_NO_CHANGE;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this),
		this->getConnectorName(idCtxt).c_str(), "Entering input ntask connector in JVX_ASYNCIO_CLASSNAME.");

	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		// This is the test function from the secondary chain. 
		res = neg_secondary_input._negotiate_connect_icon(
			theData_in, _common_set.thisisme, "CjvxAuNTask - secondary chain"
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		if (res == JVX_NO_ERROR)
		{
			// CHaining hould always end here in sec chain!
			assert(refto == NULL);

			num_channels = JVX_SIZE_UNSELECTED;
			// Primary constraints have been set according to sec chain. Let us als derive the number of output channels
			if (genSPAsyncio_node::control.flows.control_channels_pri_output.value)
			{
				num_channels = theData_in->con_params.number_channels;
			}
			resU = neg_primary_output._update_parameters_fixed(num_channels);
			if (resU == JVX_NEGOTIATE_CONSTRAINT_CHANGE)
			{
				test_chain_now = true;
			}

			if(connections != 0x3)
			{
				// This is only if the connection is not yet complete and we start with the 
				// secondary connection. There is no constraint for the input channels yet if
				// the primary connection is not yet established!
				genSPAsyncio_node::secondary_async.node.numberinputchannels.value = theData_in->con_params.number_channels;
			}
			// There should be no mode in which the secondary chain modifies the channel numbers 
			assert(genSPAsyncio_node::secondary_async.node.numberinputchannels.value == theData_in->con_params.number_channels);

			if (test_chain_now)
			{
				inform_chain_test(pri_input);
			}

		}
	}
	else
	{
		res = CjvxBareNtask::test_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res == JVX_NO_ERROR)
		{
			jvxBool inform_chain = false;

			// If we derive the number of channels from primary chain, we need to copy value to properties AND set a constraint
			// genSPAsyncio_node::secondary_async.node.numberinputchannels.value = JVX_SIZE_INT32(theData_out->con_params.number_channels);
			resU = neg_secondary_input._update_parameters_fixed(CjvxNode_genpcg::node.numberoutputchannels.value);
			if (resU == JVX_NEGOTIATE_CONSTRAINT_CHANGE)
			{
				inform_chain = true;
			}
			genSPAsyncio_node::secondary_async.node.numberinputchannels.value = CjvxNode_genpcg::node.numberoutputchannels.value;

			if (!genSPAsyncio_node::control.flows.control_channels_pri_input.value)
			{
				// This forces the secondary chain to have identical number of output as the primary chain
				resU = neg_secondary_output._update_parameters_fixed(CjvxNode_genpcg::node.numberinputchannels.value);
				if (resU == JVX_NEGOTIATE_CONSTRAINT_CHANGE)
				{
					inform_chain = true;
				}

				// This is necessary since the secondary output is not driven from extern
				genSPAsyncio_node::secondary_async.node.numberoutputchannels.value = CjvxNode_genpcg::node.numberinputchannels.value;
			}
			switch (auto_mode)
			{
			case autoConfigurationMode::JVX_ASYNC_MODE_SECONDARY_MASTER:
				/*
				neg_secondary._common_set_node_params_neg.preferred_simple.number_input_channels.max =
					CjvxNode_genpcg::node.numberoutputchannels.value;
				neg_secondary._common_set_node_params_neg.preferred_simple.number_input_channels.min =
					CjvxNode_genpcg::node.numberoutputchannels.value;
				genSPAsyncio_node::secondary_async.node.numberinputchannels.value = CjvxNode_genpcg::node.numberoutputchannels.value;
				*/
				break;
			case autoConfigurationMode::JVX_ASYNC_MODE_PRIMARY_MASTER:

				// In this mode, parameters from primary chain are accepted in secundary chain
				genSPAsyncio_node::secondary_async.node.buffersize.value = CjvxNode_genpcg::node.buffersize.value;
				genSPAsyncio_node::secondary_async.node.samplerate.value = CjvxNode_genpcg::node.samplerate.value;
				genSPAsyncio_node::secondary_async.node.format.value = CjvxNode_genpcg::node.format.value;
				genSPAsyncio_node::secondary_async.node.subformat.value = CjvxNode_genpcg::node.subformat.value;
				genSPAsyncio_node::secondary_async.node.segmentsize_x.value = CjvxNode_genpcg::node.segmentsize_x.value;
				genSPAsyncio_node::secondary_async.node.segmentsize_y.value = CjvxNode_genpcg::node.segmentsize_y.value;

				resU = neg_secondary_input._update_parameters_fixed(
					JVX_SIZE_UNSELECTED,
					CjvxNode_genpcg::node.buffersize.value,
					CjvxNode_genpcg::node.samplerate.value,
					(jvxDataFormat)CjvxNode_genpcg::node.format.value,
					(jvxDataFormatGroup)CjvxNode_genpcg::node.subformat.value);
				if (resU == JVX_NEGOTIATE_CONSTRAINT_CHANGE)
				{
					inform_chain = true;

				}
				break;
			}

			if (inform_chain)
			{
				inform_chain_test(sec_input);
			}
		}
	}
	return res;
}

jvxErrorType 
JVX_ASYNCIO_CLASSNAME::test_connect_ocon_ntask(jvxLinkDataDescriptor* theData_out,
	jvxSize idCtxt, IjvxInputConnector* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool param_overwritten = false;
	jvxSize num_channels = JVX_SIZE_UNSELECTED;
	jvxConstraintSetResult resU = JVX_NEGOTIATE_CONSTRAINT_NO_CHANGE;
	jvxBool test_chain_now = false;
	
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this),
		this->getConnectorName(idCtxt).c_str(), "Entering output ntask connector in JVX_ASYNCIO_CLASSNAME.");

	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		/*
		if(auto_mode == JVX_ASYNC_MODE_PRIMARY_MASTER)
		{
			if (connections != 3)
			{
				return JVX_ERROR_NOT_READY;
			}		
		}
		*/

		// Note: on the secondary side we will start with the output side!!!
		if (refto)
		{
			// We need to specify the connection parameters here!
			theData_out->con_params.buffersize = genSPAsyncio_node::secondary_async.node.buffersize.value;
			theData_out->con_params.rate = genSPAsyncio_node::secondary_async.node.samplerate.value;
			theData_out->con_params.format = (jvxDataFormat)genSPAsyncio_node::secondary_async.node.format.value;
			theData_out->con_params.number_channels = genSPAsyncio_node::secondary_async.node.numberoutputchannels.value;
			theData_out->con_params.segmentation_x = genSPAsyncio_node::secondary_async.node.segmentsize_x.value;
			theData_out->con_params.segmentation_y = genSPAsyncio_node::secondary_async.node.segmentsize_y.value;
			theData_out->con_params.format_group = (jvxDataFormatGroup)genSPAsyncio_node::secondary_async.node.subformat.value;
			
			// Push all old values
			// neg_secondary_input._push_constraints();

			// Set the parameter constraints for the input side!
			neg_secondary_input._update_parameters_fixed(
				JVX_SIZE_UNSELECTED,
				theData_out->con_params.buffersize,
				theData_out->con_params.rate,
				theData_out->con_params.format,
				theData_out->con_params.format_group);

			// Here, forward to next object
			res = refto->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res == JVX_NO_ERROR)
			{
				// There might a lot happen while testing, hence, accept all parameters here to track modifications
				genSPAsyncio_node::secondary_async.node.numberoutputchannels.value = theData_out->con_params.number_channels;
				genSPAsyncio_node::secondary_async.node.buffersize.value = theData_out->con_params.buffersize;
				genSPAsyncio_node::secondary_async.node.samplerate.value = theData_out->con_params.rate;
				genSPAsyncio_node::secondary_async.node.format.value = (jvxInt16)theData_out->con_params.format;
				genSPAsyncio_node::secondary_async.node.subformat.value = (jvxInt16)theData_out->con_params.format_group;
				genSPAsyncio_node::secondary_async.node.segmentsize_x.value = theData_out->con_params.segmentation_x;
				genSPAsyncio_node::secondary_async.node.segmentsize_y.value = theData_out->con_params.segmentation_y;

				// Now, update current constraints
				switch (auto_mode)
				{
				case autoConfigurationMode::JVX_ASYNC_MODE_SECONDARY_MASTER:

					// If pri input channels shall be identical to secondary output channel set it here!
					num_channels = JVX_SIZE_UNSELECTED;
					if (genSPAsyncio_node::control.flows.control_channels_pri_input.value)
					{
						num_channels = theData_out->con_params.number_channels;
					}
					// All parameters from secondary chain except for channels - taken from primary chain
					resU = neg_primary_input._update_parameters_fixed(
						num_channels,
						genSPAsyncio_node::secondary_async.node.buffersize.value,
						genSPAsyncio_node::secondary_async.node.samplerate.value,
						(jvxDataFormat)genSPAsyncio_node::secondary_async.node.format.value,
						(jvxDataFormatGroup)genSPAsyncio_node::secondary_async.node.subformat.value);
					if (resU == JVX_NEGOTIATE_CONSTRAINT_CHANGE)
					{
						test_chain_now = true;
					}
					
					if (test_chain_now)
					{
						inform_chain_test(pri_input);
					}
					break;
				case autoConfigurationMode::JVX_ASYNC_MODE_PRIMARY_MASTER:

					// nothing to do here!!!
					break;
				}
			}
		}
	}
	else
	{
		/*
		if (auto_mode == JVX_ASYNC_MODE_SECONDARY_MASTER)
		{
			if (connections != 3)
			{
				return JVX_ERROR_NOT_READY;
			}
		}
		*/

		// This call will fill in theData_out parameters
		res = CjvxBareNtask::test_connect_ocon_ntask(theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		if (!genSPAsyncio_node::control.flows.control_channels_pri_output.value)
		{
			// This forces the secondary chain to have identical number of input as the primary chain output channels
			resU = neg_secondary_input._update_parameters_fixed(CjvxNode_genpcg::node.numberoutputchannels.value);
			if (resU == JVX_NEGOTIATE_CONSTRAINT_CHANGE)
			{
				test_chain_now = true;
			}

			// This is necessary since the secondary output is not driven from extern
			genSPAsyncio_node::secondary_async.node.numberinputchannels.value = CjvxNode_genpcg::node.numberoutputchannels.value;
			
			if (test_chain_now)
			{
				// Bug fixed by HK 25.10.2021: If we are on primary chain, we need to trigger a test on the secondary chain
				//inform_chain_test(pri_input);
				inform_chain_test(sec_input);

			}
		}
		/*
		if (res == JVX_NO_ERROR)
		{

		}
		*/
	}
	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::transfer_backward_ocon_ntask(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_out,
	jvxLinkDataDescriptor* theData_in, jvxSize idCtxt, CjvxInputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxDataConnectionCommon* dataConn = NULL;
	jvxSize processId = JVX_SIZE_UNSELECTED;

	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resM = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;

	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this),
		getConnectorName(idCtxt).c_str(),
		"Entering output connector for prameter negotiations.");

	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		jvxLinkDataDescriptor* ld = (jvxLinkDataDescriptor*)data;
		jvxBool thereismismatch = false;
		jvx::propertyCallCompactList* propCallCompact = nullptr; 
		std::string txt;

		switch (tp)
		{
		case JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES:

			propCallCompact = (jvx::propertyCallCompactList*)data;
			if (propCallCompact)
			{
				jvxBool forwardChain = false;
				for (auto& elm : propCallCompact->propReqs)
				{
					if (elm->resCall == JVX_ERROR_ELEMENT_NOT_FOUND)
					{
						elm->resCall = this->get_property((elm->callMan), elm->rawPtr,
							elm->ident, elm->detail);
					}
					if (elm->resCall == JVX_ERROR_ELEMENT_NOT_FOUND)
					{
						forwardChain = true;
					}
				}

				if (forwardChain)
				{
					if (pri_input)
					{
						res = pri_input->transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					}
				}
				return res;
			}
			break;

		case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

			// This is the place where we see new parameters in connected node. Those are stored
			// as only accepted preferred parameters. Afterwards, the new prefered paraaters will be 
			// transferred to primary chain by requesting test_master
			res = neg_secondary_output._negotiate_transfer_backward_ocon(ld,
				theData_out,
				_common_set.thisisme, NULL
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if ((res == JVX_NO_ERROR)|| (res == JVX_ERROR_COMPROMISE))
			{
				jvxSize numChannels = JVX_SIZE_UNSELECTED;

				// Update the input processing parameters secondary input from secondary output
				neg_secondary_input._update_parameters_fixed(
					numChannels,
					theData_out->con_params.buffersize,
					theData_out->con_params.rate,
					theData_out->con_params.format,
					theData_out->con_params.format_group);

				/*
				Principally: Constrain the secondary input channel number to follow the primary output channel number.
				However, this is managed already in test_icon und test_ocon				
				*/
			}
			break;

		case JVX_LINKDATA_TRANSFER_REQUEST_DATA:

			// Make sure the access to the secondary link is fully setup before actually using it
			JVX_TRY_LOCK_MUTEX(resM, inProcessing.crossThreadTransfer.safeAccessSecLink);
			if (resM == JVX_TRY_LOCK_MUTEX_SUCCESS)
			{
				if (inProcessing.crossThreadTransfer.secLinkRead)
				{
					if (theData_out->con_link.connect_to)
					{
						theData_out->con_link.connect_to->process_start_icon();
					}

					jvxSize numLostNow = genSPAsyncio_node::rt_info.output.number_aborts.value +
						+genSPAsyncio_node::rt_info.output.number_overflows.value
						+ genSPAsyncio_node::rt_info.output.number_underflows.value;
					if (numLostNow != inProcessing.cntLost)
					{
						jvxLinkDataAttachedLostFrames* newPtr = NULL;
						if(jvxLinkDataAttachedLostFrames_updatePrepare(
							inProcessing.attachedData,
							numLostNow,
							newPtr))
						{
							inProcessing.cntLost = numLostNow;
							theData_out->con_data.attached_buffer_single[*theData_out->con_pipeline.idx_stage_ptr] = newPtr;
						}
					}
					if (theData_out->con_link.connect_to)
					{
						theData_out->con_link.connect_to->process_buffers_icon();

						jvxHandle** buf_to_software = NULL;
						jvxHandle** buf_from_software = NULL;
						if (theData_out->con_data.buffers)
						{
							buf_to_software = theData_out->con_data.buffers[
								*theData_out->con_pipeline.idx_stage_ptr];
						}
						if(theData_in->con_data.buffers)
						{
							buf_from_software = theData_in->con_data.buffers[
								*theData_in->con_pipeline.idx_stage_ptr];
						}
						res = audio_io_software(buf_to_software, buf_from_software,
							theData_out->con_params.buffersize);

						theData_out->con_link.connect_to->process_stop_icon();
					}
				}
				JVX_UNLOCK_MUTEX(inProcessing.crossThreadTransfer.safeAccessSecLink);
			}
			break;

		default:
			if (refto)
			{
				res = refto->transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
		}
	}
	else
	{
		res = CjvxBareNtask::transfer_backward_ocon_ntask(tp, data, theData_out, theData_in, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}


jvxErrorType 
JVX_ASYNCIO_CLASSNAME::transfer_forward_icon_ntask(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this),
		getConnectorName(idCtxt).c_str(),
		"Entering input connector for forward data transfer.");

	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
		{
			jvx::propertyCallCompactList* propCallCompact = nullptr;

			switch (tp)
			{
			case JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES:

				propCallCompact = (jvx::propertyCallCompactList*)data;
				if (propCallCompact)
				{
					jvxBool forwardChain = false;
					for (auto& elm : propCallCompact->propReqs)
					{
						if (elm->resCall == JVX_ERROR_ELEMENT_NOT_FOUND)
						{
							elm->resCall = this->get_property((elm->callMan), elm->rawPtr,
								elm->ident, elm->detail);
						}
						if (elm->resCall == JVX_ERROR_ELEMENT_NOT_FOUND)
						{
							forwardChain = true;
						}
					}

					if (forwardChain)
					{
						if (pri_output)
						{
							res = pri_output->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
						}
					}
					return res;
				}
				break;
			}
		}
	}
	res = CjvxBareNtask::transfer_forward_icon_ntask(tp, data, theData_in,
		theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return res;
}



jvxErrorType 
JVX_ASYNCIO_CLASSNAME::connect_chain_master(const jvxChainConnectArguments& args,
	const jvxLinkDataDescriptor_con_params* i_params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_NOT_READY;

	// Check if the primary connection has been established, otherwise do not connect!
	if (jvx_bitTest(connections, 0))
	{
		res = _activate_master(JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR, static_cast<IjvxConnectionMaster*>(this));
		assert(res == JVX_NO_ERROR);

		res = _connect_chain_master(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{
			jvxErrorType resL = _deactivate_master(JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR, static_cast<IjvxConnectionMaster*>(this));
			assert(resL == JVX_NO_ERROR);
		}
	}
	
	return res;
}

jvxErrorType 
JVX_ASYNCIO_CLASSNAME::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize id_task = 1;

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

		jvx_bitSet(_common_set.stateChecks.prepare_flags, id_task);
		break;
	case JVX_STATE_PREPARED:
		if (_common_set.stateChecks.prepare_flags)
		{
			jvx_bitSet(_common_set.stateChecks.prepare_flags, id_task);
		}
		break;
	default:

		// 
		// return JVX_ERROR_WRONG_STATE;
		break;
	}

	return _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

JVX_ATTACHED_LINK_DATA_RELEASE_CALLBACK_DEFINE(JVX_ASYNCIO_CLASSNAME, release_attached)
{
	jvxLinkDataAttachedLostFrames* bufPtr = reinterpret_cast<jvxLinkDataAttachedLostFrames*>(elm->if_specific(JVX_LINKDATA_ATTACHED_REPORT_UPDATE_NUMBER_LOST_FRAMES));
	jvxLinkDataAttachedLostFrames_updateComplete(inProcessing.attachedData, bufPtr);
	return JVX_NO_ERROR;
}


jvxErrorType
JVX_ASYNCIO_CLASSNAME::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize id_task = 1;

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

		jvx_bitSet(_common_set.stateChecks.start_flags, id_task);
		break;
	case JVX_STATE_PROCESSING:
		if (_common_set.stateChecks.start_flags)
		{
			jvx_bitSet(_common_set.stateChecks.start_flags, id_task);
		}
		break;
	default:
		//return JVX_ERROR_WRONG_STATE;
		break;
	}
	return _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize id_task = 1;

	res = _stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	// Handle automatic stop
	// In case we hookup while running, there may be a sequence of prepare -> start -> prepare -> start.
	// This is fine and should not yield any exception
	if(_common_set.stateChecks.start_flags)
	{
		jvx_bitClear(_common_set.stateChecks.start_flags, id_task);
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

jvxErrorType
JVX_ASYNCIO_CLASSNAME::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize id_task = 1;

	res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	// Handle automatic prepare
	// In case we hookup while running, there may be a sequence of prepare -> start -> prepare -> start.
	// This is fine and should not yield any exception
	if (_common_set.stateChecks.prepare_flags)
	{
		jvx_bitClear(_common_set.stateChecks.prepare_flags, id_task);

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
// =======================================================================================0

jvxErrorType
JVX_ASYNCIO_CLASSNAME::prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{

#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif

	jvxErrorType res = JVX_NO_ERROR;
	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		return prepare_connect_icon_ntask_core(theData_in, theData_out, idCtxt, refto, sec_zeroCopyBuffering_rt, cfg_sec_buffer, 1 JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		res = CjvxBareNtask::prepare_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
#ifdef JVX_NTASK_ENABLE_LOGFILE
		if (res == JVX_NO_ERROR)
		{
			resL = logWriter_main_signal.lfc_prepare_one_channel(theData_in->con_params.number_channels,
				theData_in->con_params.buffersize, theData_in->con_params.rate,
				theData_in->con_params.format, "audio_in_main", idMainIn);
			resL = logWriter_main_signal.lfc_prepare_one_channel(theData_out->con_params.number_channels,
				theData_out->con_params.buffersize, theData_out->con_params.rate,
				theData_out->con_params.format, "audio_in_main", idMainOut);
			resL = logWriter_main_signal.lfc_start_all_channels();
		}
#endif
	}
	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::start_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		return start_connect_icon_ntask_core(theData_in, theData_out, idCtxt, refto, 1 JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		return CjvxBareNtask::start_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::stop_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		return stop_connect_icon_ntask_core(theData_in, theData_out, idCtxt, refto, 1 JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		return CjvxBareNtask::stop_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif
	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		return postprocess_connect_icon_ntask_core(theData_in, theData_out, idCtxt, refto, sec_zeroCopyBuffering_rt, 1 JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
#ifdef JVX_NTASK_ENABLE_LOGFILE
		resL = logWriter_main_signal.lfc_stop_all_channels();
		resL = logWriter_main_signal.lfc_postprocess_all_channels();
#endif

		return CjvxBareNtask::postprocess_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::process_buffers_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto, jvxSize mt_mask, jvxSize idx_stage)
{
#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif

	/* Store samples in audio sample buffers and pull out samples to be copied to output. */
	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
#ifdef JVX_NTASK_ENABLE_LOGFILE
		resL = logWriter_main_signal.lfc_write_all_channels(
			theData_in->con_data.buffers[
				*theData_in->con_pipeline.idx_stage_ptr],
			theData_in->con_params.number_channels, theData_in->con_params.buffersize,
			theData_in->con_params.format, idMainIn);
#endif
		jvxHandle** buf_from_hw = NULL;
		jvxHandle** buf_to_hw = NULL;
		if (theData_in->con_data.buffers)
		{
			buf_from_hw = theData_in->con_data.buffers[*theData_in->con_pipeline.idx_stage_ptr];
		}
		if (theData_out->con_data.buffers)
		{
			buf_to_hw = theData_out->con_data.buffers[*theData_out->con_pipeline.idx_stage_ptr];
		}

		audio_io_hardware(buf_from_hw, buf_to_hw, theData_in->con_params.buffersize);
			
#ifdef JVX_NTASK_ENABLE_LOGFILE
		resL = logWriter_main_signal.lfc_write_all_channels(
			theData_out->con_data.buffers[
				*theData_out->con_pipeline.idx_stage_ptr],
			theData_out->con_params.number_channels, theData_out->con_params.buffersize,
			theData_out->con_params.format, idMainOut);
#endif
	}
	else if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		jvxSize idx_stage_in = idx_stage;
		if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_in))
		{
			idx_stage_in = *theData_in->con_pipeline.idx_stage_ptr;
		}
		if (theData_in->con_data.attached_buffer_single[idx_stage_in])
		{
			jvxLinkDataAttachedChain* ptr = theData_in->con_data.attached_buffer_single[idx_stage_in];
			jvxLinkDataAttachedBuffer* ptrBuf = ptr->if_buffer();
			theData_in->con_data.attached_buffer_single[idx_stage_in] = NULL;
			if (ptrBuf)
			{
				ptrBuf->cb_release(ptrBuf->priv, ptrBuf);
			}
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(JVX_ASYNCIO_CLASSNAME, set_control_flow_param)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genSPAsyncio_node::control.flows.control_channels_pri_input))
	{
		// Here, the control flow has been modified
		if (genSPAsyncio_node::control.flows.control_channels_pri_input.value)
		{
			neg_secondary_output._clear_parameters_fixed(true);
		}
		else
		{
			neg_secondary_output._update_parameters_fixed(CjvxNode_genpcg::node.numberinputchannels.value);
		}
		inform_chain_test(sec_input);
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genSPAsyncio_node::control.flows.control_channels_pri_output))
	{
		// Here, the control flow has been modified
		if (genSPAsyncio_node::control.flows.control_channels_pri_output.value)
		{
			neg_secondary_input._clear_parameters_fixed(true);
		}
		else
		{
			neg_secondary_input._update_parameters_fixed(CjvxNode_genpcg::node.numberoutputchannels.value);
		}
		inform_chain_test(sec_input);
	}
	return JVX_NO_ERROR;
}
/*
void
JVX_ASYNCIO_CLASSNAME::constraint_parameters_primary_chain(const jvxLinkDataDescriptor* ld, jvxBool* requires_update_chain)
{
	jvxSize num_in = JVX_SIZE_UNSELECTED;
	jvxInt32 bs = JVX_SIZE_UNSELECTED;
	jvxInt32 ra = JVX_SIZE_UNSELECTED;
	jvxDataFormat fo = JVX_DATAFORMAT_NONE;
	jvxDataFormatGroup sfo = JVX_DATAFORMAT_GROUP_NONE;
	jvxBool requiresupdate = false;

	if (genSPAsyncio_node::control.flows.update_bsize.value)
	{
		bs = JVX_SIZE_INT32(ld->con_params.buffersize);
		requiresupdate = true;
	}
	if (genSPAsyncio_node::control.flows.update_rate.value)
	{
		ra = JVX_SIZE_INT32(ld->con_params.rate);
		requiresupdate = true;
	}
	if (genSPAsyncio_node::control.flows.update_format.value)
	{
		fo = (jvxDataFormat)ld->con_params.format;
		requiresupdate = true;
	}
	if (genSPAsyncio_node::control.flows.update_subformat.value)
	{
		sfo = (jvxDataFormatGroup)ld->con_params.format_group;
		requiresupdate = true;
	}

	if (requiresupdate)
	{
		/ *
		 Constrain the primary chain parameters
		 * /
		neg_primary._set_parameters_fixed_simple(
			JVX_SIZE_UNSELECTED,
			bs,
			ra,
			fo,
			sfo);
	}
	if (requires_update_chain) 
		*requires_update_chain = requiresupdate;
}
*/
// =====================================================================================================0

#ifdef JVX_NTASK_ENABLE_LOGFILE

jvxErrorType
JVX_ASYNCIO_CLASSNAME::put_configuration(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename, jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNtask::put_configuration(callConf, processor,
		sectionToContainAllSubsectionsForMe, 
		filename,
		lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			logWriter_main_signal.lfc_put_configuration_active(callConf, processor,
				sectionToContainAllSubsectionsForMe,
				filename,
				lineno,
				warns);
			logWriter_timing.lfc_put_configuration_active(callConf, processor,
				sectionToContainAllSubsectionsForMe,
				filename,
				lineno,
				warns);

			genSPAsyncio_node::put_configuration_all(callConf,
				processor, sectionToContainAllSubsectionsForMe,
				&warns);

			for (i = 0; i < warns.size(); i++)
			{
				_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
			}
		}
	}
	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNtask::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	logWriter_main_signal.lfc_get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	logWriter_timing.lfc_get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	genSPAsyncio_node::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);
	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = CjvxBareNtask::set_property(
		callGate, rawPtr,
		ident, trans);

	if (res == JVX_NO_ERROR)
	{
		res = logWriter_main_signal.lfc_set_property(
			callGate, rawPtr,
			ident, trans,
			callGate.call_purpose);
		res = logWriter_timing.lfc_set_property(callGate,
			rawPtr,
			ident, trans,
			callGate.call_purpose);
	}
	return res;
}


#endif