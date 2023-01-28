#include "jvx.h"
#include "jvxAudioNodes/CjvxAuNTasks.h"

#define ALPHA_SMOOTH 0.7

// =======================================================================

CjvxAuNTasks::CjvxAuNTasks(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): CjvxBareNtask(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	JVX_NBARETASK_ATTACH_TASK(JVX_DEFAULT_CON_VAR_OFF, "async-secondary-n", NULL);
	sec_input = NULL;
	pri_input = NULL;

	cfg_sec_buffer.num_additional_pipleline_stages_cfg = 0;
	cfg_sec_buffer.num_min_buffers_in_cfg = 1;
	cfg_sec_buffer.num_min_buffers_out_cfg = 1;
	cfg_sec_buffer.zeroCopyBuffering_cfg = false;
	sec_zeroCopyBuffering_rt = false;

	auto_parameters.buffersize_pri_to_sec = true; // <- requires also the asyncio component to be configured properly
	auto_parameters.rate_pri_to_sec = true;

	parameter_config = JVX_AUNTASKS_PRIMARY_MASTER;

}

CjvxAuNTasks::~CjvxAuNTasks()
{
}

// ===========================================================================================
// ===========================================================================================

jvxErrorType
CjvxAuNTasks::activate()
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
		sec_input = connector_input_name("async-secondary-n");
		pri_input = connector_input_name("default");

		CjvxAuNTasks_pcg::init_all();
		CjvxAuNTasks_pcg::allocate_all();
		CjvxAuNTasks_pcg::register_all(static_cast<CjvxProperties*>(this));

	}
	return res;
}

jvxErrorType
CjvxAuNTasks::deactivate()
{
	jvxErrorType res = CjvxBareNtask::_pre_check_deactivate();
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	std::vector<jvxSize> lst_uids_remove;

	if (res == JVX_NO_ERROR)
	{
		CjvxAuNTasks_pcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxAuNTasks_pcg::deallocate_all();

		sec_input = NULL;
		pri_input = NULL;
		res = CjvxBareNtask::deactivate();
	}
	return res;
}

// ===========================================================================================
// ===========================================================================================

jvxErrorType
CjvxAuNTasks::prepare()
{
	jvxErrorType res = CjvxBareNtask::prepare();
	if (res == JVX_NO_ERROR)
	{
		in_proc.numGlitchesProcessing = 0;
	}
	return res;
}

jvxErrorType
CjvxAuNTasks::postprocess()
{
	jvxErrorType res = CjvxBareNtask::postprocess();
	if (res == JVX_NO_ERROR)
	{
		in_proc.numGlitchesProcessing = 0;
	}
	return res;
}

// ===========================================================================================
// ===========================================================================================

jvxErrorType
CjvxAuNTasks::test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool request_chain = false;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));
	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		// This is the test function from the secondary chain. The secondary slave is slave from
		// primary chain, therefore, it will allow only "THE" single preferred parameter set
		// The primary configuration accepts just all.
		res = neg_secondary_input._negotiate_connect_icon(
			theData_in, _common_set.thisisme, "CjvxAuNTasks - Secondary Chain"
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res == JVX_NO_ERROR)
		{
			CjvxAuNTasks_pcg::secondary_async.node.buffersize.value = JVX_SIZE_INT32(neg_secondary_input._latest_results.buffersize);
			CjvxAuNTasks_pcg::secondary_async.node.samplerate.value = JVX_SIZE_INT32(neg_secondary_input._latest_results.rate);
			CjvxAuNTasks_pcg::secondary_async.node.format.value = (jvxInt16)neg_secondary_input._latest_results.format;
			CjvxAuNTasks_pcg::secondary_async.node.numberinputchannels.value = JVX_SIZE_INT32(neg_secondary_input._latest_results.number_channels);
			CjvxAuNTasks_pcg::secondary_async.node.subformat.value = (jvxInt16)neg_secondary_input._latest_results.format_group;
			CjvxAuNTasks_pcg::secondary_async.node.segmentsize_x.value = JVX_SIZE_INT32(neg_secondary_input._latest_results.segmentation_x);
			CjvxAuNTasks_pcg::secondary_async.node.segmentsize_y.value = JVX_SIZE_INT32(neg_secondary_input._latest_results.segmentation_y);

			if (refto)
			{
				// Store the output parameters
				neg_secondary_output._push_constraints();
	
				// Set current output constraints
				neg_secondary_output._update_parameters_fixed(
					JVX_SIZE_UNSELECTED,
					CjvxAuNTasks_pcg::secondary_async.node.buffersize.value,
					CjvxAuNTasks_pcg::secondary_async.node.samplerate.value,
					(jvxDataFormat)CjvxAuNTasks_pcg::secondary_async.node.format.value,
					(jvxDataFormatGroup)CjvxAuNTasks_pcg::secondary_async.node.subformat.value);

				res = refto->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				neg_secondary_output._pop_constraints();
				if (res == JVX_NO_ERROR)
				{
					// Output arguments
					CjvxAuNTasks_pcg::secondary_async.node.numberoutputchannels.value = JVX_SIZE_INT32(theData_out->con_params.number_channels);
					CjvxAuNTasks_pcg::secondary_async.node.segmentsize_y.value = JVX_SIZE_INT32(theData_out->con_params.number_channels);

					// Check that constraining did work out
					assert(CjvxAuNTasks_pcg::secondary_async.node.buffersize.value == JVX_SIZE_INT32(theData_out->con_params.buffersize));
					assert(CjvxAuNTasks_pcg::secondary_async.node.samplerate.value = JVX_SIZE_INT32(theData_out->con_params.rate));
					assert(CjvxAuNTasks_pcg::secondary_async.node.format.value == (jvxInt16)theData_out->con_params.format);
					assert(CjvxAuNTasks_pcg::secondary_async.node.subformat.value == (jvxInt16)theData_out->con_params.format_group);
					assert(CjvxAuNTasks_pcg::secondary_async.node.segmentsize_x.value == JVX_SIZE_INT32(theData_out->con_params.segmentation_x));
					assert(CjvxAuNTasks_pcg::secondary_async.node.segmentsize_y.value == JVX_SIZE_INT32(theData_out->con_params.segmentation_y));

					if (parameter_config == JVX_AUNTASKS_SECONDARY_MASTER)
					{
						if (neg_primary_input._update_parameters_fixed(
							JVX_SIZE_UNSELECTED,
							CjvxAuNTasks_pcg::secondary_async.node.buffersize.value,
							CjvxAuNTasks_pcg::secondary_async.node.samplerate.value,
							(jvxDataFormat)CjvxAuNTasks_pcg::secondary_async.node.format.value,
							(jvxDataFormatGroup)CjvxAuNTasks_pcg::secondary_async.node.subformat.value) ==
							JVX_NEGOTIATE_CONSTRAINT_CHANGE)
						{
							request_chain = true;
						}
					}
					if (request_chain)
					{
						inform_chain_test(pri_input);
					}
				}
			}
		}
	}
	else
	{
		// This is the test function from the primary chain
		res = CjvxBareNtask::test_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if(res == JVX_NO_ERROR)
		{
			if (parameter_config == JVX_AUNTASKS_PRIMARY_MASTER)
			{
				if (neg_secondary_input._update_parameters_fixed(
					JVX_SIZE_UNSELECTED,
					CjvxNode_genpcg::node.buffersize.value,
					CjvxNode_genpcg::node.samplerate.value,
					(jvxDataFormat)CjvxNode_genpcg::node.format.value,
					(jvxDataFormatGroup)CjvxNode_genpcg::node.subformat.value) ==
					JVX_NEGOTIATE_CONSTRAINT_CHANGE)
				{
					request_chain = true;
				}

				if (request_chain)
				{
					inform_chain_test(sec_input);
				}
			}
		}
	}
	return res;
}

jvxErrorType
CjvxAuNTasks::test_connect_ocon_ntask(jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	IjvxInputConnector* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));

	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		// Set the current values. These have been set before in test_connect_icon_id
		// What are we supposed to do with the output channels?

		// This function copies the number of input channels
		JVX_TEST_SETTING_PROP_PUT(theData_out, CjvxAuNTasks_pcg::secondary_async.node);

		// We may modify the number of output channels here!

		// Pass forward
		if (refto)
		{
			res = refto->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}
	else
	{
		res = CjvxBareNtask::test_connect_ocon_ntask(theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType 
CjvxAuNTasks::transfer_backward_ocon_ntask(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_out,
	jvxLinkDataDescriptor* theData_in, jvxSize idCtxt, CjvxInputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
		{
			res = neg_secondary_output._negotiate_transfer_backward_ocon((jvxLinkDataDescriptor*)data, theData_out,
				_common_set.thisisme, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			return res;
		}
		break;
	default:
		break;
	}
	res = CjvxBareNtask::transfer_backward_ocon_ntask(tp, data, theData_out,
		theData_in, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return res;
}

jvxErrorType
CjvxAuNTasks::prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		return prepare_connect_icon_ntask_core(theData_in, theData_out, idCtxt, refto, sec_zeroCopyBuffering_rt, cfg_sec_buffer, 1 JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{

		res = CjvxBareNtask::prepare_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));		
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxAuNTasks::start_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
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
CjvxAuNTasks::stop_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
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
CjvxAuNTasks::postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	if (idCtxt == JVX_DEFAULT_CON_VAR_OFF)
	{
		return postprocess_connect_icon_ntask_core(theData_in, theData_out, idCtxt, refto, derived_zeroCopyBuffering_rt, 1 
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		res = CjvxBareNtask::postprocess_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

// ===================================================================================================
jvxErrorType
CjvxAuNTasks::process_buffers_icon_nvtask_master(
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out,
	jvxSize mt_mask, jvxSize idx_stage)
{
	// Pull in secondary signals
	sec_input->transfer_backward_icon(JVX_LINKDATA_TRANSFER_REQUEST_DATA, NULL, 0);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNTasks::process_buffers_icon_ntask_attached(
	jvxSize ctxtId,
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out,
	jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idx_stage_out = *theData_out->con_pipeline.idx_stage_ptr;
	jvxSize idx_stage_in = idx_stage;
	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_in))
	{
		idx_stage_in = *theData_in->con_pipeline.idx_stage_ptr;
	}
	assert(_common_set_nvtask_proc.theData_out_master);
	assert(_common_set_nvtask_proc.theData_in_master);

	if (theData_in->con_data.attached_buffer_single[idx_stage_in])
	{
		jvxLinkDataAttached* ptr = theData_in->con_data.attached_buffer_single[idx_stage_in];
		while (1)
		{
			if (ptr)
			{
				if (ptr->tp == JVX_LINKDATA_ATTACHED_REPORT_UPDATE_NUMBER_LOST_FRAMES)
				{
					jvxLinkDataAttachedLostFrames* ptrL = (jvxLinkDataAttachedLostFrames*)ptr;
					in_proc.numGlitchesProcessing = JVX_MAX(in_proc.numGlitchesProcessing,
						ptrL->numLost);
				}
				ptr = ptr->next;
			}
			else
			{
				break;
			}
		}

		theData_out->con_data.attached_buffer_single[idx_stage_out] =
			theData_in->con_data.attached_buffer_single[idx_stage_in];
		theData_in->con_data.attached_buffer_single[idx_stage_in] = NULL;
	}
	
	res = process_io(_common_set_nvtask_proc.theData_in_master, 
		_common_set_nvtask_proc.idx_stage_in_master,
		theData_in, idx_stage_in,
		_common_set_nvtask_proc.theData_out_master, 
		theData_out);


	return res;
}

jvxErrorType
CjvxAuNTasks::process_io(jvxLinkDataDescriptor* pri_in, jvxSize idx_stage_pri_in,
	jvxLinkDataDescriptor* sec_in, jvxSize idx_stage_sec_in,
	jvxLinkDataDescriptor* pri_out, jvxLinkDataDescriptor* sec_out)
{
	jvxSize maxChans = 0;
	jvxSize minChans = 0;
	jvxSize ii;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idx_stage_pri_out = *pri_out->con_pipeline.idx_stage_ptr;
	jvxSize idx_stage_sec_out = *sec_out->con_pipeline.idx_stage_ptr;

	if (jvx_bitTest(CjvxAuNTasks_pcg::mode.output_copy.value.selection(), 1))
	{
		maxChans = JVX_MAX(sec_in->con_params.number_channels, pri_out->con_params.number_channels);
		minChans = JVX_MIN(sec_in->con_params.number_channels, pri_out->con_params.number_channels);

		// This default function does not tolerate a lot of unexpected settings
		assert(sec_in->con_params.format == pri_out->con_params.format);
		assert(sec_in->con_params.buffersize == pri_out->con_params.buffersize);

		// Cross copy mode
		if (minChans)
		{
			for (ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % sec_in->con_params.number_channels;
				jvxSize idxOut = ii % pri_out->con_params.number_channels;

				assert(sec_in->con_params.format == pri_out->con_params.format);
				jvx_convertSamples_memcpy(
					sec_in->con_data.buffers[idx_stage_sec_in][idxIn],
					pri_out->con_data.buffers[idx_stage_pri_out][idxOut],
					jvxDataFormat_size[sec_in->con_params.format],
					sec_in->con_params.buffersize);
			}
		}

		// =========================================================================================
		maxChans = JVX_MAX(sec_out->con_params.number_channels, pri_in->con_params.number_channels);
		minChans = JVX_MIN(sec_out->con_params.number_channels, pri_in->con_params.number_channels);

		// This default function does not tolerate a lot of unexpected settings
		assert(sec_out->con_params.format == pri_in->con_params.format);
		assert(sec_out->con_params.buffersize == pri_in->con_params.buffersize);

		// Cross copy mode
		if (minChans)
		{
			for (ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % sec_out->con_params.number_channels;
				jvxSize idxOut = ii % pri_in->con_params.number_channels;

				assert(sec_out->con_params.format == pri_in->con_params.format);
				jvx_convertSamples_memcpy(
					pri_in->con_data.buffers[idx_stage_pri_in][idxOut],
					sec_out->con_data.buffers[idx_stage_sec_out][idxIn],
					jvxDataFormat_size[sec_out->con_params.format],
					sec_out->con_params.buffersize);
			}
		}
	}
	if (jvx_bitTest(CjvxAuNTasks_pcg::mode.output_copy.value.selection(), 0))
	{
		maxChans = JVX_MAX(pri_in->con_params.number_channels, pri_out->con_params.number_channels);
		minChans = JVX_MIN(pri_in->con_params.number_channels, pri_out->con_params.number_channels);

		// This default function does not tolerate a lot of unexpected settings
		assert(pri_in->con_params.format == pri_out->con_params.format);
		assert(pri_in->con_params.buffersize == pri_out->con_params.buffersize);

		// Cross copy mode
		if (minChans)
		{
			for (ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % pri_in->con_params.number_channels;
				jvxSize idxOut = ii % pri_out->con_params.number_channels;

				assert(pri_in->con_params.format == pri_out->con_params.format);
				jvx_convertSamples_memcpy(
					pri_in->con_data.buffers[idx_stage_pri_in][idxIn],
					pri_out->con_data.buffers[idx_stage_pri_out][idxOut],
					jvxDataFormat_size[sec_in->con_params.format],
					pri_in->con_params.buffersize);
			}
		}

		// =========================================================================================
		maxChans = JVX_MAX(sec_in->con_params.number_channels, sec_out->con_params.number_channels);
		minChans = JVX_MIN(sec_in->con_params.number_channels, sec_out->con_params.number_channels);

		// This default function does not tolerate a lot of unexpected settings
		assert(sec_in->con_params.format == sec_out->con_params.format);
		assert(sec_in->con_params.buffersize == sec_out->con_params.buffersize);

		// Cross copy mode
		if (minChans)
		{
			for (ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % sec_out->con_params.number_channels;
				jvxSize idxOut = ii % pri_in->con_params.number_channels;

				assert(sec_out->con_params.format == sec_in->con_params.format);
				jvx_convertSamples_memcpy(
					sec_in->con_data.buffers[idx_stage_sec_in][idxOut],
					sec_out->con_data.buffers[idx_stage_sec_out][idxIn],
					jvxDataFormat_size[sec_out->con_params.format],
					sec_out->con_params.buffersize);
			}
		}
	}
	return res;
}

