#include "CjvxSpNMultiplexer.h"

#include <cstring>

// =====================================================================================
// Prepare / postprocess
// =====================================================================================

jvxErrorType
CjvxSpNMultiplexer::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Store the allocation flags as passed by the previous component, the base class modifies the flags
	jvxCBitField allocFlagsIn = _common_set_icon.theData_in->con_data.alloc_flags;

	// The predecessor may not allow to pass other buffers than those which were allocated for the input
	jvxBool linkAllowed = !(
		jvx_bitTest(allocFlagsIn, (jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT) ||
		jvx_bitTest(allocFlagsIn, (jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT));

	// The base class allocates the pipeline and a full set of buffers on the input side. The output connector
	// "default" is not connected. Therefore, the base class always runs in "no zerocopy" mode.
	jvxErrorType res = CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvxLinkDataDescriptor* datIn = _common_set_icon.theData_in;
		runtime.szChannelBytes = jvxDataFormat_getsize(datIn->con_params.format) * datIn->con_params.buffersize;

		for (auto& elm : outputConnectors)
		{
			if (elm->inChain)
			{
				jvxLinkDataDescriptor& datOut = elm->_common_set_ocon.theData_out;

				// The same setup as involved in the base class for an output which is not operated in zerocopy mode
				datOut.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
				datOut.con_data.alloc_flags = allocFlagsIn;
				datOut.con_pipeline.num_additional_pipleline_stages = 0;
				datOut.con_sync.type_timestamp = datIn->con_sync.type_timestamp;

				res = elm->prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				if (res != JVX_NO_ERROR)
				{
					break;
				}
				elm->isPrepared = true;

				// The successor has allocated the buffers. Try to use these buffers on the input side
				link_buffers_output_connector(elm, linkAllowed);
			}
		}

		if (res != JVX_NO_ERROR)
		{
			JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
			postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb_loc));
		}
		update_monitor();
	}
	return res;
}

jvxErrorType
CjvxSpNMultiplexer::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	for (auto& elm : outputConnectors)
	{
		if (elm->isPrepared)
		{
			// Restore the buffer pointers on the input side before the successor releases its buffers
			unlink_buffers_output_connector(elm);

			elm->postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			elm->_common_set_ocon.theData_out.con_data.number_buffers = 0;
			elm->isPrepared = false;
		}
	}
	runtime.szChannelBytes = 0;
	update_monitor();

	return CjvxBareNode1ioRearrange::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

void
CjvxSpNMultiplexer::link_buffers_output_connector(CjvxSpNMultiplexerOutputConnector* oconn, jvxBool linkAllowed)
{
	jvxSize i;
	jvxLinkDataDescriptor* datIn = _common_set_icon.theData_in;
	jvxLinkDataDescriptor& datOut = oconn->_common_set_ocon.theData_out;

	oconn->isLinked = false;

	// The buffers can only be linked if both sides involve a single buffer in the pipeline: with
	// more than one pipeline stage, the association between the buffers on the input and
	// the output side is not fixed. In that case, the buffers are copied.
	if (
		linkAllowed &&
		(datIn->con_data.number_buffers == 1) &&
		(datOut.con_data.number_buffers == 1) &&
		datIn->con_data.buffers && datOut.con_data.buffers &&
		datIn->con_data.buffers[0] && datOut.con_data.buffers[0] &&
		((oconn->chanOffset + oconn->chanNum) <= datIn->con_params.number_channels) &&
		(oconn->chanNum <= datOut.con_params.number_channels))
	{
		for (i = 0; i < oconn->chanNum; i++)
		{
			if (datOut.con_data.buffers[0][i] == nullptr)
			{
				// Successor has no buffer for this channel - leave the remaining buffers as they are
				break;
			}
			datIn->con_data.buffers[0][oconn->chanOffset + i] = datOut.con_data.buffers[0][i];
		}
		oconn->isLinked = (i == oconn->chanNum);
		if (!oconn->isLinked)
		{
			unlink_buffers_output_connector(oconn);
		}
	}
}

void
CjvxSpNMultiplexer::unlink_buffers_output_connector(CjvxSpNMultiplexerOutputConnector* oconn)
{
	jvxSize i;
	jvxLinkDataDescriptor* datIn = _common_set_icon.theData_in;

	// Restore the pointers to the buffers which were allocated on the input side
	if (
		datIn && datIn->con_data.buffers && datIn->con_data.buffers[0] &&
		datIn->con_data.bExt.raw && datIn->con_data.bExt.raw[0] &&
		datIn->con_data.bExt.offset && datIn->con_data.bExt.offset[0])
	{
		for (i = 0; i < oconn->chanNum; i++)
		{
			jvxSize idxChan = oconn->chanOffset + i;
			if (idxChan < datIn->con_params.number_channels)
			{
				datIn->con_data.buffers[0][idxChan] =
					((jvxByte*)datIn->con_data.bExt.raw[0][idxChan] + datIn->con_data.bExt.offset[0][idxChan]);
			}
		}
	}
	oconn->isLinked = false;
}

// =====================================================================================
// Start / stop
// =====================================================================================

jvxErrorType
CjvxSpNMultiplexer::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		for (auto& elm : outputConnectors)
		{
			if (elm->isPrepared)
			{
				// The connector reports the new state in <report_started_connector>
				res = elm->start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				if (res != JVX_NO_ERROR)
				{
					break;
				}
			}
		}

		if (res != JVX_NO_ERROR)
		{
			JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
			stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb_loc));
		}
	}
	return res;
}

jvxErrorType
CjvxSpNMultiplexer::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	for (auto& elm : outputConnectors)
	{
		if (elm->isStarted)
		{
			// The connector reports the new state in <report_stopped_connector>
			elm->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		elm->inCycle = false;
	}
	return CjvxBareNode1ioRearrange::stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

// =====================================================================================
// Processing
// =====================================================================================

jvxErrorType
CjvxSpNMultiplexer::process_start_icon(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Propagate to all successors first as it is the convention in the default implementation
	for (auto& elm : outputConnectors)
	{
		if (elm->isStarted)
		{
			jvxErrorType resL = elm->process_start_ocon(0, NULL, 0, NULL, NULL);
			if (resL == JVX_NO_ERROR)
			{
				elm->inCycle = true;
			}
			else
			{
				res = resL;
				break;
			}
		}
	}

	if (res == JVX_NO_ERROR)
	{
		res = CjvxBareNode1ioRearrange::process_start_icon(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	}

	if (res != JVX_NO_ERROR)
	{
		// Release what has been reserved
		for (auto& elm : outputConnectors)
		{
			if (elm->inCycle)
			{
				elm->process_stop_ocon(JVX_SIZE_UNSELECTED, true, 0, NULL, NULL);
				elm->inCycle = false;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxSpNMultiplexer::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor* datIn = _common_set_icon.theData_in;
	jvxHandle** bufsIn = jvx_process_icon_extract_input_buffers<jvxHandle>(datIn, idx_stage);

	for (auto& elm : outputConnectors)
	{
		if (elm->inCycle)
		{
			jvxLinkDataDescriptor& datOut = elm->_common_set_ocon.theData_out;
			if (bufsIn && datOut.con_data.buffers)
			{
				jvxHandle** bufsOut = jvx_process_icon_extract_output_buffers<jvxHandle>(datOut);
				if (bufsOut)
				{
					for (i = 0; i < elm->chanNum; i++)
					{
						jvxHandle* bufFrom = bufsIn[elm->chanOffset + i];
						jvxHandle* bufTo = bufsOut[i];

						// If source and target are identical, the previous component has written the
						// samples directly to the buffer of the successor - nothing to copy
						if (bufFrom != bufTo)
						{
							memcpy(bufTo, bufFrom, runtime.szChannelBytes);
						}
					}
				}
			}

			// Copy the admin data and forward to next chain element
			datOut.admin = datIn->admin;
			jvxErrorType resL = elm->process_buffers_ocon(mt_mask, JVX_SIZE_UNSELECTED);
			if (res == JVX_NO_ERROR)
			{
				res = resL;
			}
		}
	}

	// The base class has nothing to copy since the output connector "default" is not connected
	jvxErrorType resL = CjvxBareNode1ioRearrange::process_buffers_icon(mt_mask, idx_stage);
	if (res == JVX_NO_ERROR)
	{
		res = resL;
	}
	return res;
}

jvxErrorType
CjvxSpNMultiplexer::process_stop_icon(
	jvxSize idx_stage,
	jvxBool operate_first_call,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	for (auto& elm : outputConnectors)
	{
		if (elm->inCycle)
		{
			elm->process_stop_ocon(JVX_SIZE_UNSELECTED, true, 0, NULL, NULL);
			elm->inCycle = false;
		}
	}
	return CjvxBareNode1ioRearrange::process_stop_icon(idx_stage, operate_first_call, tobeAccessedByStage, clbk, priv_ptr);
}
