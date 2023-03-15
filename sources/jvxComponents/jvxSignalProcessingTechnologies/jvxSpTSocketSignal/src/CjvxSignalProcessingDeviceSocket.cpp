#include "CjvxSignalProcessingTechnologySocket.h"
#include "CjvxSignalProcessingDeviceSocket.h"

#define JVX_CJVXSIGNALPROCESSINGDEVICESOCKET_ALPHA_RATE 0.995

CjvxSignalProcessingDeviceSocket::CjvxSignalProcessingDeviceSocket(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxFullMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	JVX_INITIALIZE_MUTEX(safeAccess_convertbufs);
};

CjvxSignalProcessingDeviceSocket::~CjvxSignalProcessingDeviceSocket()
{
	JVX_TERMINATE_MUTEX(safeAccess_convertbufs);
}

// ================================================================================

jvxErrorType 
CjvxSignalProcessingDeviceSocket::init_connection(IjvxSocketsConnection * conn,
		CjvxSignalProcessingTechnologySocket * parentRef)
{
	connection = conn;
	//devState = CjvxSignalProcessingDeviceSocket::deviceStatus::JVX_SOCKET_DEVICE_CONNECTED;
	connection->register_report(static_cast<IjvxSocketsConnection_report*>(&theSMachine));
	parent = parentRef;
	theSMachine.initialize_slave(
		static_cast<IjvxAudioSocketsStateMachine_common*>(this),
		connection);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingDeviceSocket::terminate_connection()
{
	connection->unregister_report(static_cast<IjvxSocketsConnection_report*>(&theSMachine));
	connection = NULL;
	theSMachine.unlink_slave();
	theSMachine.terminate();
	_common_set.iamalive = false;
	return JVX_NO_ERROR;
}

// ================================================================================

jvxErrorType 
CjvxSignalProcessingDeviceSocket::activate()
{
	jvxSize i;
	jvxErrorType res = CjvxFullMasterDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		CjvxSignalProcessingDeviceSocket_genpcg::init_all();
		CjvxSignalProcessingDeviceSocket_genpcg::allocate_all();
		CjvxSignalProcessingDeviceSocket_genpcg::register_all(static_cast<CjvxProperties*>(this));
		
		CjvxSignalProcessingDeviceSocket_genpcg::associate__sync__derived(
			static_cast<CjvxProperties*>(this),
			&runtime_sync.jitter_buffer_size, 1);
		
		/* associate__sync__monitor(CjvxProperties* theProps, 
			jvxSize* ptrExt_number_overflows, jvxSize num_number_overflows, 
			jvxSize* ptrExt_number_underflows, jvxSize num_number_underflows, 
			jvxSize* ptrExt_number_aborts, jvxSize num_number_aborts, 
			jvxData* ptrExt_fillheight_avrg, jvxSize num_fillheight_avrg, 
			jvxData* ptrExt_fillheight_min, jvxSize num_fillheight_min, 
			jvxData* ptrExt_fillheight_max, jvxSize num_fillheight_max)
			*/
		CjvxSignalProcessingDeviceSocket_genpcg::associate__sync__monitor(
			static_cast<CjvxProperties*>(this),
			&runtime_sync.number_overflows, 1,
			&runtime_sync.number_underflows, 1,
			&runtime_sync.number_aborts, 1,
			&runtime_sync.fillheight_avrg, 1,
			&runtime_sync.fillheight_min, 1,
			&runtime_sync.fillheight_max, 1);

		CjvxSignalProcessingDeviceSocket_genpcg::associate__sync__parameter(
			static_cast<CjvxProperties*>(this),
			&config_sync.number_buffers, 1,
			&config_sync.log_timing, 1);

		//CjvxSignalProcessingDeviceSocket_genpcg::register_callbacks(static_cast<CjvxProperties*>(this),
		//	get_interface_ptr, reinterpret_cast<jvxHandle*>(this),
		//	NULL);

		CjvxFullMasterDevice::CjvxFullMasterDevice_genpcg::proc_params.bsize.value =
			theSMachine.params.bsize;
		CjvxFullMasterDevice::CjvxFullMasterDevice_genpcg::proc_params.srate.value =
			theSMachine.params.srate;
		CjvxFullMasterDevice::CjvxFullMasterDevice_genpcg::proc_params.num_out_channels.value =
			theSMachine.params.numChannelsSend;
		CjvxFullMasterDevice::CjvxFullMasterDevice_genpcg::proc_params.num_in_channels.value =
			theSMachine.params.numChannelsReceive;
		CjvxFullMasterDevice::CjvxFullMasterDevice_genpcg::proc_params.format.value = 
			theSMachine.params.format;
		CjvxFullMasterDevice::CjvxFullMasterDevice_genpcg::proc_params.subformat.value =
			theSMachine.params.formGroup;
		CjvxFullMasterDevice::CjvxFullMasterDevice_genpcg::proc_params.seg_x.value = 
			theSMachine.params.bsize;
		CjvxFullMasterDevice::CjvxFullMasterDevice_genpcg::proc_params.seg_y.value =
			1;

		JVX_LOCK_MUTEX(safeAccess_convertbufs);
		sz_buffers_convert = theSMachine.params.bsize;
		num_buffers_convert = theSMachine.params.numChannelsSend;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld_buffers_convert, jvxData*, num_buffers_convert);
		for (i = 0; i < num_buffers_convert; i++)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld_buffers_convert[i], jvxData, sz_buffers_convert);
		}
		JVX_UNLOCK_MUTEX(safeAccess_convertbufs);

		JVX_GET_TICKCOUNT_US_SETREF(&tStampRates_extern);
		tStampRates_last_out_extern = JVX_SIZE_UNSELECTED;
		delta_t_smoothed_extern = -1;
	}
	return res;
}

jvxErrorType 
CjvxSignalProcessingDeviceSocket::deactivate()
{
	jvxSize i;
	jvxErrorType res = CjvxFullMasterDevice::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		JVX_LOCK_MUTEX(safeAccess_convertbufs);
		for (i = 0; i < num_buffers_convert; i++)
		{
			JVX_DSP_SAFE_DELETE_FIELD(fld_buffers_convert[i]);
		}
		JVX_DSP_SAFE_DELETE_FIELD(fld_buffers_convert);
		sz_buffers_convert = 0;
		num_buffers_convert = 0;
		JVX_UNLOCK_MUTEX(safeAccess_convertbufs);

		CjvxSignalProcessingDeviceSocket_genpcg::deassociate__sync__parameter(
			static_cast<CjvxProperties*>(this));
		CjvxSignalProcessingDeviceSocket_genpcg::deassociate__sync__monitor(
			static_cast<CjvxProperties*>(this));
		CjvxSignalProcessingDeviceSocket_genpcg::deassociate__sync__derived(
			static_cast<CjvxProperties*>(this));

		CjvxSignalProcessingDeviceSocket_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxSignalProcessingDeviceSocket_genpcg::deallocate_all();

		res = CjvxFullMasterDevice::deactivate();
	}
	return res;
}

// =============================================================
jvxErrorType
CjvxSignalProcessingDeviceSocket::prepare()
{
	jvxErrorType res = _prepare_lock();
	//;CjvxFullMasterDevice::prepare();
	// we can use the "lock version directly since prepare is not implemented inbase classes.
	// Otherwise, be careful!
	if (res == JVX_NO_ERROR)
	{
		jvxSize bsize_used = _common_set_ldslave.theData_out.con_params.buffersize;
		bsize_used = JVX_MAX(bsize_used, theSMachine.params.bsize);
		jvxSize srate_used = _common_set_ldslave.theData_out.con_params.rate;

		config_sync.reset();
		runtime_sync.reset();
		fheight_sync.reset();

		syncio.startSync(
			bsize_used,
			srate_used,
			_common_set_ldslave.theData_out.con_params.format,
			_common_set_ldslave.theData_out.con_params.number_channels,
			_common_set_ldslave.theData_in->con_params.number_channels,
			&config_sync,
			&runtime_sync,
			&fheight_sync);
		_unlock_state();

		JVX_GET_TICKCOUNT_US_SETREF(&tStampRates_local);
		tStampRates_last_out_local = JVX_SIZE_UNSELECTED;
		delta_t_smoothed_local = -1;
	}
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceSocket::postprocess()
{
	jvxErrorType res = CjvxFullMasterDevice::_postprocess_lock();
	if (res == JVX_NO_ERROR)
	{
		syncio.stopSync();

		config_sync.reset();
		runtime_sync.reset();
		fheight_sync.reset();
		_unlock_state();
	}
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceSocket::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	outputParams.buffersize = JVX_SIZE_UNSELECTED; // Any buffersize
	outputParams.number_channels = JVX_SIZE_UNSELECTED; // Any number of channels
	outputParams.format = JVX_DATAFORMAT_NONE; // Any 
	outputParams.format_group = JVX_DATAFORMAT_GROUP_NONE; // Any subformat
	outputParams.segmentation.x = outputParams.buffersize;

	outputParams.rate =
		CjvxFullMasterDevice::CjvxFullMasterDevice_genpcg::proc_params.srate.value; // Should be of the same rate

	inputParams.buffersize = JVX_SIZE_UNSELECTED; // Any buffersize
	inputParams.number_channels = 0; // Any number of channels
	inputParams.format = JVX_DATAFORMAT_NONE; // Any 
	inputParams.format_group = JVX_DATAFORMAT_GROUP_NONE; // Any subformat
	inputParams.segmentation.x = inputParams.buffersize;
	inputParams.rate = outputParams.rate;

	res = CjvxFullMasterDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceSocket::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxFullMasterDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceSocket::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxFullMasterDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceSocket::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxFullMasterDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceSocket::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	// End up here only if prepare was called before
	jvxLinkDataDescriptor* theData_in = nullptr;
	jvxLinkDataDescriptor* theData_out = nullptr;

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_REQUEST_DATA:

		theData_out = &_common_set_ldslave.theData_out;
		theData_in = _common_set_ldslave.theData_in;

		jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStampRates_local);
		if (JVX_CHECK_SIZE_SELECTED(tStampRates_last_out_local))
		{
			jvxTick deltat = tt - tStampRates_last_out_local;
			jvxData deltaf = (jvxData)deltat / (jvxSize)theData_out->con_params.buffersize;
			if (delta_t_smoothed_local < 0)
			{
				delta_t_smoothed_local = deltaf;
			}
			else
			{
				delta_t_smoothed_local = delta_t_smoothed_local * JVX_CJVXSIGNALPROCESSINGDEVICESOCKET_ALPHA_RATE + 
					deltaf * (1.0 - JVX_CJVXSIGNALPROCESSINGDEVICESOCKET_ALPHA_RATE);
			}
			jvxData rate_current = 1000000.0 / delta_t_smoothed_local;
			CjvxSignalProcessingDeviceSocket_genpcg::sync.monitor.output_rate.value = rate_current;
		}
		tStampRates_last_out_local = tt;

		// Make sure the access to the secondary link is fully setup before actually using it
		if (theData_out->con_link.connect_to)
		{
			_common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();


			/*
			jvxSize numLostNow = runtime.number_aborts +
				+genSPAsyncio_node::rt_info.output.number_overflows.value
				+ genSPAsyncio_node::rt_info.output.number_underflows.value;
			*/
			/*
			if (numLostNow != inProcessing.cntLost)
			{
				jvxLinkDataAttached* newPtr = NULL;
				if (jvxLinkDataAttachedLostFrames_updatePrepare(
					inProcessing.attachedData,
					numLostNow,
					newPtr))
				{
					inProcessing.cntLost = numLostNow;
					theData_out->con_data.attached_buffer_single[*theData_out->con_pipeline.idx_stage_ptr] = newPtr;
				}
			}
			*/

			_common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();

			jvxHandle** buf_to_software = NULL;
			jvxHandle** buf_from_software = NULL;
			if (_common_set_ldslave.theData_out.con_data.buffers)
			{
				buf_to_software = theData_out->con_data.buffers[*theData_out->con_pipeline.idx_stage_ptr];
			}
			if (theData_in->con_data.buffers)
			{
				buf_from_software = theData_in->con_data.buffers[*theData_in->con_pipeline.idx_stage_ptr];
			}
			jvxErrorType res = syncio.io_software(buf_to_software, buf_from_software,
				theData_out->con_params.buffersize);

			theData_out->con_link.connect_to->process_stop_icon();
		}
		return JVX_NO_ERROR;
		break;
	}
	return CjvxFullMasterDevice::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxSignalProcessingDeviceSocket::data_slave(jvxAudioSocketParameters* params)
{
	jvxErrorType res = theSMachine.data_slave(params);
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceSocket::accept_slave(jvxAudioSocketParameters* params)
{
	jvxErrorType res = theSMachine.accept_slave(params);
	if (res == JVX_NO_ERROR)
	{
		std::string defName = theSMachine.params.nameConnection;
		if (!defName.empty())
		{
			_common_set_min.theDescription = _common_set_min.theDescription + "<" + defName + ">";
			_common_set.theDescriptor = defName;
		}
	}
	return res;
}

/*
JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSignalProcessingDeviceSocket, get_interface_ptr)
{
	IjvxSignalProcessingDeploy** fldIf = (IjvxSignalProcessingDeploy**)fld;
	if (fldIf)
		*fldIf = static_cast<IjvxSignalProcessingDeploy*>(this);

	return JVX_NO_ERROR;
}
*/

jvxErrorType 
CjvxSignalProcessingDeviceSocket::report_new_frame_start(
	jvxSize bsize_data,
	jvxDataFormat format_data,
	jvxSize numchannels_data,
	jvxSize seq_id)
{
	jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStampRates_extern);
	if (JVX_CHECK_SIZE_SELECTED(tStampRates_last_out_extern))
	{
		jvxTick deltat = tt - tStampRates_last_out_extern;
		jvxData deltaf = (jvxData)deltat / (jvxSize)bsize_data;
		if (delta_t_smoothed_extern < 0)
		{
			delta_t_smoothed_extern = deltaf;
		}
		else
		{
			delta_t_smoothed_extern = delta_t_smoothed_extern * JVX_CJVXSIGNALPROCESSINGDEVICESOCKET_ALPHA_RATE + 
				deltaf * (1.0 - JVX_CJVXSIGNALPROCESSINGDEVICESOCKET_ALPHA_RATE);
		}
		jvxData rate_current = 1000000.0 / delta_t_smoothed_extern;
		CjvxSignalProcessingDeviceSocket_genpcg::sync.monitor.input_rate.value = rate_current;
	}
	tStampRates_last_out_extern = tt;

	flags_channels = 0;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSignalProcessingDeviceSocket::report_channel_buffer(
	jvxHandle* buffer,
	jvxSize chan_id_idx)
{
	JVX_LOCK_MUTEX(safeAccess_convertbufs);
	if (fld_buffers_convert)
	{
		if (chan_id_idx < num_buffers_convert)
		{
			flags_channels |= chan_id_idx;
			jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, jvxData>(
				(jvxInt16*) buffer, fld_buffers_convert[chan_id_idx],
				sz_buffers_convert,
				JVX_MAX_INT_16_DIV);
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess_convertbufs);

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSignalProcessingDeviceSocket::report_frame_complete(jvxBool detectedError, jvxSize seq_id)
{
	// Copy to cross thread buffer
	_lock_state();
	if (_common_set_min.theState >= JVX_STATE_PREPARED)
	{
		JVX_LOCK_MUTEX(safeAccess_convertbufs);
		if (fld_buffers_convert)
		{
			syncio.io_hardware((jvxHandle**)fld_buffers_convert,
				NULL,
				sz_buffers_convert);
		}

		JVX_UNLOCK_MUTEX(safeAccess_convertbufs);
	}
	_unlock_state();

	jvxSize i;
	jvxHandle* buffer_copyData = NULL;
	jvxSize space = 0;
	jvxDataFormat form = JVX_DATAFORMAT_NONE;
	jvxSize numChannels = 0;
	jvxUInt64 channel_mask = 0;

	theSMachine.send_buffer_response_start(&buffer_copyData, &space, &form, &numChannels);

	JVX_LOCK_MUTEX(safeAccess_convertbufs);

	// Check that we have the right number of channels
	if (numChannels == 0)
	{
		channel_mask = ((jvxUInt64)-1);
		theSMachine.send_buffer_response_data(channel_mask, seq_id);
	}
	else
	{
		for (i = 0; i < numChannels; i++)
		{
			// Not implemented at this time
			channel_mask = i;
			if (i == 0)
			{
				channel_mask |= ((jvxUInt64)1 << 62);
			}
			if (i == numChannels - 1)
			{
				channel_mask |= ((jvxUInt64)1 << 63);
			}
			theSMachine.send_buffer_response_data(channel_mask, seq_id);
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess_convertbufs);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingDeviceSocket::report_state_switch(jvxAudioSocketsState stat)
{

	switch (stat)
	{
	case jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_INIT:
		parent->report_new_connection(connection);
		break;
	}
	return JVX_NO_ERROR;
}
