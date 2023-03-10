#include "CjvxSignalProcessingDeviceDeploy.h"

jvxErrorType 
CjvxSignalProcessingDeviceDeploy::setDataInteractReference(IjvxSignalProcessingDeploy_interact* interact)
{
	if (interact)
	{
		if (interactRef == NULL)
		{
			interactRef = interact;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ALREADY_IN_USE;
	}
	if (interactRef)
	{
		interactRef = NULL;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxSignalProcessingDeviceDeploy::setOutputParameters(jvxLinkDataDescriptor_con_params* params)
{
	outputParams = *params;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::setInputParameters(jvxLinkDataDescriptor_con_params* params) 
{
	// = _common_set_ldslave.theData_in->con_params;

	jvxErrorType res = JVX_NO_ERROR;
	jvxSize processId = JVX_SIZE_UNSELECTED;
	IjvxDataConnectionCommon* dataConn = NULL;

	inputParams = *params;

	// Inform the secondary chain to be updated
	this->associated_common_icon(&dataConn);

	if (dataConn)
	{
		dataConn->unique_id_connections(&processId);
	}

	if (JVX_CHECK_SIZE_SELECTED(processId))
	{
		res = this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT, (jvxHandle*)(intptr_t)processId);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::triggerTest() 
{
	//
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::exchangeData(IjvxSignalProcessingDeploy_data* data,
	jvxLinkDataAttachedChain* single_arg)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resW = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX(resW, safeAccessChain);
	if (resW == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		if (_common_set_ld_master.state == JVX_STATE_PROCESSING)
		{
			jvxLinkDataAttachedChain* ptr = NULL;
			if (single_arg)
			{
				jvxLinkDataAttachedLostFrames* ptrLost = reinterpret_cast<jvxLinkDataAttachedLostFrames*>(single_arg->if_specific(JVX_LINKDATA_ATTACHED_REPORT_UPDATE_NUMBER_LOST_FRAMES));
				if (ptrLost)
				{
					lost_cnt_new = ptrLost->numLost;
				}
			}

			res = _common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
			jvxSize idxBuf = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
			jvxByte** bufs = (jvxByte**)_common_set_ldslave.theData_out.con_data.buffers[idxBuf];

			// Forward number of lost buffers
			if (lost_cnt_new != lost_cnt_reported)
			{
				jvxLinkDataAttachedLostFrames* newPtr = NULL;
				if (jvxLinkDataAttachedLostFrames_updatePrepare(
					lst_lost,
					lost_cnt_new,
					newPtr))
				{
					_common_set_ldslave.theData_out.con_data.attached_buffer_single[idxBuf] = newPtr;
					lost_cnt_reported = lost_cnt_new;
				}
			}


			JVX_ASSERT(data->bsize == _common_set_ldslave.theData_out.con_params.buffersize);
			JVX_ASSERT(data->srate == _common_set_ldslave.theData_out.con_params.rate);
			JVX_ASSERT(data->form == _common_set_ldslave.theData_out.con_params.format);
			JVX_ASSERT(data->format_group == _common_set_ldslave.theData_out.con_params.format_group);

			jvxSize numBytes = sizeof(jvxByte) * jvxDataFormat_getsize(data->form) * data->bsize;

			for (i = 0; i < _common_set_ldslave.theData_out.con_params.number_channels; i++)
			{
				if (i < data->bufs_received_num)
				{
					memcpy(bufs[i], data->bufs_received_fld[i], numBytes);
				}
			}

			res = _common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();

			idxBuf = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
			bufs = (jvxByte**)_common_set_ldslave.theData_in->con_data.buffers[idxBuf];
			for (i = 0; i < _common_set_ldslave.theData_in->con_params.number_channels; i++)
			{
				if (i < data->bufs_to_emit_num)
				{
					memcpy(data->bufs_to_emit_fld[i], bufs[i], numBytes);
				}
			}

			res = _common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
		}
		JVX_UNLOCK_MUTEX(safeAccessChain);
	}
	return JVX_NO_ERROR;
}

/*
jvxErrorType
CjvxSignalProcessingDeviceDeploy::triggerPrepare()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::triggerStart()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::triggerStop()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::triggerPostprocess() 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::triggerProcess() 
{
	return JVX_NO_ERROR;
}*/