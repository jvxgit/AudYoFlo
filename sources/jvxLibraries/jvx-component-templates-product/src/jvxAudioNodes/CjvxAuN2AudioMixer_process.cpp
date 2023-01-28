#include "jvx.h"
#include "jvxAudioNodes/CjvxAuN2AudioMixer.h"
#include "jvx-helpers-cpp.h"

jvxErrorType
CjvxAuN2AudioMixer::prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
	jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxNVTasks::prepare_connect_icon_ntask(theData_in, theData_out,
		idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvxSize i;

		JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
		for (auto& elm : mixBuffers)
		{
			if (elm.second.allocatOnPrepare)
			{
				elm.second.bufSize = theData_out->con_params.buffersize;

				elm.second.numChannels = theData_out->con_params.number_channels;
				if (JVX_CHECK_SIZE_SELECTED(elm.second.numChannels_fixed))
				{
					elm.second.numChannels = elm.second.numChannels_fixed;
				}

				if (elm.second.numChannels)
				{
					elm.second.szElm = jvxDataFormat_getsize(elm.second.format);
					assert(elm.second.szElm);
					elm.second.szElmsChannel = elm.second.szElm * elm.second.bufSize;
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(elm.second.mixBuffer, jvxHandle*, elm.second.numChannels);

					for (i = 0; i < elm.second.numChannels; i++)
					{
						JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(elm.second.mixBuffer[i], jvxByte, elm.second.szElmsChannel);
					}
				}
				elm.second.ready = true;
			}
		}
		JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
	}
	return res;
}

jvxErrorType
CjvxAuN2AudioMixer::postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
	jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxNVTasks::postprocess_connect_icon_ntask(theData_in, theData_out,
		idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvxSize i;
		JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
		for (auto& elm : mixBuffers)
		{
			if (elm.second.allocatOnPrepare)
			{
				if (elm.second.ready)
				{
					elm.second.ready = false;
					for (i = 0; i < elm.second.numChannels; i++)
					{
						JVX_DSP_SAFE_DELETE_FIELD(elm.second.mixBuffer[i]);
					}
					JVX_DSP_SAFE_DELETE_FIELD(elm.second.mixBuffer);
					elm.second.bufSize = 0;
					elm.second.numChannels = 0;
					elm.second.szElm = 0;
					elm.second.szElmsChannel = 0;
				}
			}
		}
		JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
	}
	return res;
}

jvxErrorType
CjvxAuN2AudioMixer::process_buffers_icon_vtask(
	jvxSize mt_mask, jvxSize idx_stage, jvxSize procId)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor* theData_in = theGlobalIterator->second.icon->con->_common_set_icon_nvtask.theData_in;
	assert(theGlobalIterator != _common_set_nv_proc.lst_in_proc_tasks.end());

	if (theData_in->con_params.format_group != JVX_DATAFORMAT_GROUP_TRIGGER_ONLY)
	{		
		// COMMENT #1: We do not need a lock here since this call is always triggeredfrom
		// within the ntask call with id = 0 - hence the master. All other
		// process callbacks are driven from within the lock and will popup here
		
		// JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
		for (auto& elmB : mixBuffers)
		{
			if (elmB.second.ready)
			{
				mix_data_to_mix_buffers(theData_in->con_link.uIdConn, elmB.second, theData_in, idx_stage);
			}
		}

		// No lock required here, check <COMMENT #1>
		// JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
	}

	std::map<jvxSize, oneEntryProcessingVTask>::iterator theGlobalIterator_bwd = theGlobalIterator;
	theGlobalIterator++;

	recursive_vtask_processing();

	/* =======================================================================
	 * Forward the output chain routes
	 * =======================================================================
	 */
	auto elm = theGlobalIterator_bwd->second.ocon.begin();
	for (; elm != theGlobalIterator_bwd->second.ocon.end(); elm++)
	{
		if ((*elm)->con)
		{
			jvxLinkDataDescriptor* data_out_vtask = &(*elm)->con->_common_set_ocon_nvtask.theData_out;
			
			// No lock required here, check <COMMENT #1>
			// JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
			for (auto& elmB : mixBuffers)
			{
				if (elmB.second.ready)
				{
					copy_data_from_mix_buffers(data_out_vtask->con_link.uIdConn, elmB.second, data_out_vtask);
				}
			}
			// No lock required here, check <COMMENT #1>
			// JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);

			res = (*elm)->con->process_buffers_ocon();

			if (res != JVX_NO_ERROR)
				break;
		}
	}
	return res;
}

void
CjvxAuN2AudioMixer::mix_data_to_mix_buffers(jvxSize uidconn, const oneBufferDefinition& oneBuffer,
	jvxLinkDataDescriptor* theData_in, jvxSize idx_stage,
	jvxSize toOffset, jvxSize fromOffset, jvxSize toNumberLimit )
{
	/*
	jvxSize mixBufferSize = 0;
	jvxSize mixBufferChannels = 0;
	jvxData** primaryMixBuffer = nullptr;
	jvxData** secondaryMixBuffer = nullptr;
	*/
	jvxSize i;
	jvxSize numChans = theData_in->con_params.number_channels;
	jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(theData_in, idx_stage);
	
	jvxSize mixBufferChannels = oneBuffer.numChannels;
	jvxSize mixBufferSize = oneBuffer.bufSize;
	assert(oneBuffer.format == JVX_DATAFORMAT_DATA);
	jvxData** mixBuffer = reinterpret_cast<jvxData**>(oneBuffer.mixBuffer);

	jvxSize numChansMax = JVX_MAX(mixBufferChannels, numChans);
	numChansMax = JVX_MIN(toNumberLimit, numChansMax);

	// We need to assure that neithe of both is zero to prevent exception with modulo
	if (numChans && mixBufferChannels)
	{
		for (i = 0; i < numChansMax; i++)
		{
			jvxSize idxIn = (i + fromOffset) % numChans;
			jvxSize idxOut = (i + toOffset) % mixBufferChannels;
			jvx_mixSamples_flp(
				bufsIn[idxIn],
				mixBuffer[idxOut],
				mixBufferSize);
		}
	}
}

void
CjvxAuN2AudioMixer::copy_data_from_mix_buffers(jvxSize uidconn, const oneBufferDefinition& oneBuffer, jvxLinkDataDescriptor* data_out)
{
	jvxSize i;
	jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(data_out);
	jvxSize numChans = data_out->con_params.number_channels;

	jvxSize mixBufferChannels = oneBuffer.numChannels;
	jvxSize mixBufferSize = oneBuffer.bufSize;
	assert(oneBuffer.format == JVX_DATAFORMAT_DATA);
	jvxData** mixBuffer = reinterpret_cast<jvxData**>(oneBuffer.mixBuffer);

	jvxSize numChansMax = JVX_MIN(mixBufferChannels, numChans);
	if (numChansMax)
	{
		for (i = 0; i < numChansMax; i++)
		{
			jvxSize idxIn = i % mixBufferChannels;
			jvxSize idxOut = i % numChans;
			jvx_convertSamples_memcpy(
				mixBuffer[idxOut],
				bufsOut[idxIn],
				sizeof(jvxData),
				mixBufferSize);
		}
	}
}

jvxErrorType
CjvxAuN2AudioMixer::process_buffers_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto, jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize i;
	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		// When we are here, the trigger has come from within the master device.
		// We start the mixbuffer fill and get data from all connected no-master devices
		// 
		// On first run store the thread id for master thread
		if (_common_set_nvtask_proc.thread_id_master == JVX_NULLTHREAD)
		{
			_common_set_nvtask_proc.thread_id_master = JVX_GET_CURRENT_THREAD_ID();
		}

		// We need to hold this lock here:
		// If additional connections come in, both, the mixbuffers as well as the 
		// connection list may change. Typically, the lock MUST be in use when adding a connection
		// but the mixbuffer list does not nessecarily require a lock if no mix buffer is added.
		// However, we never know.
		// The lock may take longer here but the place where it is used to add an element to one
		// of the list should be really fast as it only hooksup the new element.
		JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
		for (auto& elmB : mixBuffers)
		{
			for (i = 0; i < elmB.second.numChannels; i++)
			{
				memset(elmB.second.mixBuffer[i], 0, elmB.second.szElmsChannel);
			}
		}
		
		if (theData_in->con_params.number_channels)
		{
			// The following functions check that the mixBufferChannels fit
			for (auto& elmB : mixBuffers)
			{
				mix_data_to_mix_buffers(theData_in->con_link.uIdConn, elmB.second, theData_in, idx_stage);
			}
		}

		// Lock the tasks to prevent add or remove while in processing
		// JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);

		theGlobalIterator = _common_set_nv_proc.lst_in_proc_tasks.begin();
		recursive_vtask_processing();
		
		// JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
		// Store the data sets for input and output in master thread

		for (auto& elmB : mixBuffers)
		{
			copy_data_from_mix_buffers(theData_out->con_link.uIdConn, elmB.second, theData_out);
		}

		JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
		res = JVX_NO_ERROR;
	}
	else
	{
		// How does this integrate into the mixer? I think, this code is never reached since we always use 
		// 1 ntask (the master) and vtasks - no one uses a secondary ntask at the moment)
		res = process_buffers_icon_ntask_attached(idCtxt, theData_in, theData_out, mt_mask, idx_stage);
	}

	// ===========================================================
	if (res == JVX_NO_ERROR)
	{
		res = refto->process_buffers_ocon(mt_mask, idx_stage);
	}
	return res;
}

void
CjvxAuN2AudioMixer::recursive_vtask_processing()
{
	// This call is always from the 0-ntask process function and inside the connection lock!
	if (theGlobalIterator != _common_set_nv_proc.lst_in_proc_tasks.end())
	{
		jvxErrorType resL = JVX_NO_ERROR;
		resL = theGlobalIterator->second.icon->con->transfer_backward_icon(JVX_LINKDATA_TRANSFER_REQUEST_DATA, NULL, 0);
	}
}
