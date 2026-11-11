#include "CjvxSpNDemultiplexer.h"

#include <cstring>

// =====================================================================================
// Prepare / postprocess: the output side is prepared on the arrival at the first input
// connector since the buffers on the output side are required to setup the input connectors.
// =====================================================================================

jvxErrorType
CjvxSpNDemultiplexer::icon_prepare(CjvxSpNDemultiplexerInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor* datIn = iconn->_common_set_icon.theData_in;
	jvxLinkDataDescriptor& datOut = _common_set_ocon.theData_out;

	// Store the allocation flags as passed by the previous component, the flags are modified on prepare
	jvxCBitField allocFlagsIn = datIn->con_data.alloc_flags;

	// The predecessor may not allow to pass other buffers than those which were allocated for the input
	jvxBool linkAllowed = !(
		jvx_bitTest(allocFlagsIn, (jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT) ||
		jvx_bitTest(allocFlagsIn, (jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT));

	if (!output.isPrepared)
	{
		// Run the implicit prepare of the base class
		prepare_autostart();
		if (_common_set_min.theState < JVX_STATE_PREPARED)
		{
			return JVX_ERROR_WRONG_STATE;
		}

		// The same setup as involved in the base class for an output which is not operated in zerocopy mode
		assert(datOut.con_data.buffers == NULL);
		datOut.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
		datOut.con_data.alloc_flags = allocFlagsIn;
		datOut.con_pipeline.num_additional_pipleline_stages = 0;
		datOut.con_sync.type_timestamp = datIn->con_sync.type_timestamp;

		res = prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			datOut.con_data.number_buffers = 0;
			postprocess_autostart();
			return res;
		}
		output.isPrepared = true;
		runtime.szChannelBytes = jvxDataFormat_getsize(datOut.con_params.format) * datOut.con_params.buffersize;
	}

	// Allocate the pipeline and a full set of buffers for this input connector
	res = iconn->CjvxSingleInputConnector::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		iconn->isPrepared = true;

		// This element is not a zerocopy element in the common sense
		jvx_bitClear(datIn->con_data.alloc_flags,
			(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);

		// The successor has allocated the buffers. Try to use these buffers on the input side
		link_buffers_input_connector(iconn, linkAllowed);
	}
	else
	{
		// Undo the prepare on the output side if no other connector is involved
		jvxBool anyPrepared = false;
		for (auto& elm : inputConnectors)
		{
			anyPrepared = anyPrepared || elm->isPrepared;
		}
		if (!anyPrepared)
		{
			JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
			postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb_loc));
			datOut.con_data.number_buffers = 0;
			output.isPrepared = false;
			postprocess_autostart();
		}
	}
	update_monitor();
	return res;
}

jvxErrorType
CjvxSpNDemultiplexer::icon_postprocess(CjvxSpNDemultiplexerInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool anyPrepared = false;

	if (iconn->isPrepared)
	{
		// Restore the buffer pointers on the input side and release the buffers
		unlink_buffers_input_connector(iconn);
		res = iconn->CjvxSingleInputConnector::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		iconn->isPrepared = false;
	}

	for (auto& elm : inputConnectors)
	{
		anyPrepared = anyPrepared || elm->isPrepared;
	}

	if (!anyPrepared && output.isPrepared)
	{
		// The last input connector has left, postprocess on the output side
		postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		_common_set_ocon.theData_out.con_data.number_buffers = 0;
		output.isPrepared = false;
		runtime.szChannelBytes = 0;

		// Run the implicit postprocess of the base class
		postprocess_autostart();
	}
	update_monitor();
	return res;
}

void
CjvxSpNDemultiplexer::link_buffers_input_connector(CjvxSpNDemultiplexerInputConnector* iconn, jvxBool linkAllowed)
{
	jvxSize i;
	jvxLinkDataDescriptor* datIn = iconn->_common_set_icon.theData_in;
	jvxLinkDataDescriptor& datOut = _common_set_ocon.theData_out;
	jvxSize numChannels = datIn->con_params.number_channels;

	iconn->isLinked = false;

	// The buffers can only be linked if both sides involve a single buffer in the pipeline: with
	// more than one pipeline stage, the association between the buffers on the input and
	// the output side is not fixed. In that case, the buffers are copied.
	if (
		linkAllowed &&
		(datIn->con_data.number_buffers == 1) &&
		(datOut.con_data.number_buffers == 1) &&
		datIn->con_data.buffers && datOut.con_data.buffers &&
		datIn->con_data.buffers[0] && datOut.con_data.buffers[0] &&
		((iconn->chanOffset + numChannels) <= datOut.con_params.number_channels))
	{
		for (i = 0; i < numChannels; i++)
		{
			if (datOut.con_data.buffers[0][iconn->chanOffset + i] == nullptr)
			{
				// Successor has no buffer for this channel - leave the remaining buffers as they are
				break;
			}
			datIn->con_data.buffers[0][i] = datOut.con_data.buffers[0][iconn->chanOffset + i];
		}
		iconn->isLinked = (i == numChannels);
		if (!iconn->isLinked)
		{
			unlink_buffers_input_connector(iconn);
		}
	}
}

void
CjvxSpNDemultiplexer::unlink_buffers_input_connector(CjvxSpNDemultiplexerInputConnector* iconn)
{
	jvxSize i;
	jvxLinkDataDescriptor* datIn = iconn->_common_set_icon.theData_in;

	// Restore the pointers to the buffers which were allocated on the input side
	if (
		datIn && datIn->con_data.buffers && datIn->con_data.buffers[0] &&
		datIn->con_data.bExt.raw && datIn->con_data.bExt.raw[0] &&
		datIn->con_data.bExt.offset && datIn->con_data.bExt.offset[0])
	{
		for (i = 0; i < datIn->con_params.number_channels; i++)
		{
			datIn->con_data.buffers[0][i] =
				((jvxByte*)datIn->con_data.bExt.raw[0][i] + datIn->con_data.bExt.offset[0][i]);
		}
	}
	iconn->isLinked = false;
}

// =====================================================================================
// Start / stop
// =====================================================================================

jvxErrorType
CjvxSpNDemultiplexer::icon_start(CjvxSpNDemultiplexerInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (!output.isStarted)
	{
		// Run the implicit start of the base class
		start_autostart();

		res = start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			stop_autostart();
			return res;
		}
		output.isStarted = true;
		output.inCycle = false;
		output.buffersForwarded = false;
	}

	iconn->seenProcessStart = false;
	iconn->seenProcessBuffers = false;
	iconn->seenProcessStop = false;

	// The connector reports the new state in <report_started_connector>
	res = iconn->CjvxSingleInputConnector::start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		jvxBool anyStarted = false;
		for (auto& elm : inputConnectors)
		{
			anyStarted = anyStarted || elm->isStarted;
		}
		if (!anyStarted)
		{
			JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
			stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb_loc));
			output.isStarted = false;
			stop_autostart();
		}
	}
	return res;
}

jvxErrorType
CjvxSpNDemultiplexer::icon_stop(CjvxSpNDemultiplexerInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool anyStarted = false;

	if (iconn->isStarted)
	{
		// The connector reports the new state in <report_stopped_connector>
		res = iconn->CjvxSingleInputConnector::stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	for (auto& elm : inputConnectors)
	{
		anyStarted = anyStarted || elm->isStarted;
	}

	if (!anyStarted && output.isStarted)
	{
		// The last input connector has left, stop on the output side
		if (output.inCycle)
		{
			finalize_output_cycle();
		}
		stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		output.isStarted = false;

		// Run the implicit stop of the base class
		stop_autostart();
	}
	return res;
}

// =====================================================================================
// Processing: one processing cycle on the output side covers the processing cycles
// of all input connectors in operation.
// =====================================================================================

void
CjvxSpNDemultiplexer::finalize_output_cycle()
{
	process_stop_ocon(JVX_SIZE_UNSELECTED, true, 0, NULL, NULL);
	output.inCycle = false;
	output.buffersForwarded = false;
	for (auto& elm : inputConnectors)
	{
		elm->seenProcessStart = false;
		elm->seenProcessBuffers = false;
		elm->seenProcessStop = false;
	}
}

jvxErrorType
CjvxSpNDemultiplexer::icon_process_start(CjvxSpNDemultiplexerInputConnector* iconn,
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (output.inCycle && iconn->seenProcessStart)
	{
		// This connector starts a new cycle but the previous cycle is still open: one of the
		// other connectors was not involved. Close the previous cycle here.
		finalize_output_cycle();
	}

	if (!output.inCycle)
	{
		// The first connector opens the cycle on the output side
		res = process_start_ocon(0, NULL, 0, NULL, NULL);
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
		output.inCycle = true;
		output.buffersForwarded = false;
	}

	// Reserve the buffer in the pipeline of this input connector
	res = iconn->CjvxSingleInputConnector::process_start_icon(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	if (res == JVX_NO_ERROR)
	{
		iconn->seenProcessStart = true;
	}
	else
	{
		jvxBool anySeen = false;
		for (auto& elm : inputConnectors)
		{
			anySeen = anySeen || elm->seenProcessStart;
		}
		if (!anySeen)
		{
			finalize_output_cycle();
		}
	}
	return res;
}

jvxErrorType
CjvxSpNDemultiplexer::icon_process_buffers(CjvxSpNDemultiplexerInputConnector* iconn, jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxBool allSeen = true;
	jvxBool isFirst = true;
	jvxLinkDataDescriptor* datIn = iconn->_common_set_icon.theData_in;
	jvxLinkDataDescriptor& datOut = _common_set_ocon.theData_out;

	if (!output.inCycle)
	{
		return JVX_ERROR_NOT_READY;
	}

	if (datIn->con_data.buffers && datOut.con_data.buffers)
	{
		jvxHandle** bufsIn = jvx_process_icon_extract_input_buffers<jvxHandle>(datIn, idx_stage);
		jvxHandle** bufsOut = jvx_process_icon_extract_output_buffers<jvxHandle>(datOut);
		if (bufsIn && bufsOut)
		{
			jvxSize numChannels = datIn->con_params.number_channels;
			if ((iconn->chanOffset + numChannels) > datOut.con_params.number_channels)
			{
				numChannels = 0;
				if (iconn->chanOffset < datOut.con_params.number_channels)
				{
					numChannels = datOut.con_params.number_channels - iconn->chanOffset;
				}
			}

			for (i = 0; i < numChannels; i++)
			{
				jvxHandle* bufFrom = bufsIn[i];
				jvxHandle* bufTo = bufsOut[iconn->chanOffset + i];

				// If source and target are identical, the previous component has written the
				// samples directly to the buffer of the successor - nothing to copy
				if (bufFrom != bufTo)
				{
					memcpy(bufTo, bufFrom, runtime.szChannelBytes);
				}
			}
		}
	}

	for (auto& elm : inputConnectors)
	{
		if (elm->isStarted && elm->seenProcessBuffers)
		{
			isFirst = false;
			break;
		}
	}
	if (isFirst)
	{
		// The admin data is taken from the first connector in each cycle
		datOut.admin = datIn->admin;
	}

	iconn->seenProcessBuffers = true;
	for (auto& elm : inputConnectors)
	{
		if (elm->isStarted && !elm->seenProcessBuffers)
		{
			allSeen = false;
			break;
		}
	}

	if (allSeen && !output.buffersForwarded)
	{
		// All input connectors have delivered, forward to next chain element
		output.buffersForwarded = true;
		return process_buffers_ocon(mt_mask, JVX_SIZE_UNSELECTED);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNDemultiplexer::icon_process_stop(CjvxSpNDemultiplexerInputConnector* iconn,
	jvxSize idx_stage,
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxBool allSeen = true;

	// Release the buffer in the pipeline of this input connector
	jvxErrorType res = iconn->CjvxSingleInputConnector::process_stop_icon(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);

	iconn->seenProcessStop = true;
	for (auto& elm : inputConnectors)
	{
		if (elm->isStarted && !elm->seenProcessStop)
		{
			allSeen = false;
			break;
		}
	}

	if (allSeen && output.inCycle)
	{
		// All input connectors have completed the cycle, close the cycle on the output side
		finalize_output_cycle();
	}
	return res;
}
