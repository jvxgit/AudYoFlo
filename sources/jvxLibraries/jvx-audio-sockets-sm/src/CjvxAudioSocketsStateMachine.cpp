#include "CjvxAudioSocketsStateMachine.h"

// ======================================================================

CjvxAudioSocketsStateMachine::CjvxAudioSocketsStateMachine()
{
	JVX_INITIALIZE_MUTEX(safeAccessConnection);
	memset(&cfg_remote, 0, sizeof(cfg_remote));
}

CjvxAudioSocketsStateMachine::~CjvxAudioSocketsStateMachine()
{
	JVX_TERMINATE_MUTEX(safeAccessConnection);
}

jvxErrorType
CjvxAudioSocketsStateMachine::initialize_master(
	jvxAudioSocketParameters* paramsArg,
	IjvxAudioSocketsStateMachine_common* report_common,
	jvxSize szBytesErrorBuffer)
{
	if (localState == jvxAudioSocketsState::JVX_AUDIOSTATE_NONE)
	{
		masterMode = true;
		params = *paramsArg;		
		ptr_common = report_common;
		localState = jvxAudioSocketsState::JVX_AUDIOSTATE_READY;

		sz_suck_unexpected_bytes = JVX_MAX(16, szBytesErrorBuffer);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(suck_unexpected_bytes, jvxByte, sz_suck_unexpected_bytes);

		// ===================================================================================
		// Allocate receiver buffer
		// ===================================================================================
		sz_receiver_field_buffer_data = 0;
		if (params.numChannelsReceive)
		{
			sz_receiver_field_buffer_data = (params.bsize * jvxDataFormat_getsize(params.format));
		}
		sz_receiver_field_buffer = sizeof(jvxAdvProtocolDataHeader) + sz_receiver_field_buffer_data;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(receive_field_buffer, jvxByte, sz_receiver_field_buffer + 1);
		dbg_pointer = receive_field_buffer + sz_receiver_field_buffer;

		return JVX_NO_ERROR;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioSocketsStateMachine::initialize_slave(
	IjvxAudioSocketsStateMachine_common* report_common,
	IjvxSocketsConnection* connRef,
	jvxSize szBytesErrorBuffer)
{
	if (localState == jvxAudioSocketsState::JVX_AUDIOSTATE_NONE)
	{		
		connection = connRef;
		masterMode = false;
		ptr_common = report_common;
		localState = jvxAudioSocketsState::JVX_AUDIOSTATE_CONNECTED;

		sz_suck_unexpected_bytes = JVX_MAX(16, szBytesErrorBuffer);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(suck_unexpected_bytes, jvxByte, sz_suck_unexpected_bytes);

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxAudioSocketsStateMachine::accept_slave(jvxAudioSocketParameters* paramsArg )
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (masterMode == false)
	{
		JVX_LOCK_MUTEX(safeAccessConnection);
		if (localState == jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_INIT)
		{
			if (paramsArg)
			{
				params = *paramsArg;
			}			

			jvxRemoteDeviceConfig sdData;
			sdData.header.paketsize = sizeof(jvxRemoteDeviceConfig);
			sdData.header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_SIMPLE_AUDIO_DEVICE_LINK;
			sdData.header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE | JVX_PROTOCOL_SIMPLEDEVICE_CONFIGURE;

			sdData.bsize = params.bsize;
			sdData.srate = params.srate;
			sdData.format = params.format;
			sdData.format_group = params.formGroup;
			sdData.channels_send = params.numChannelsSend;
			sdData.channels_recv = params.numChannelsReceive;
			sdData.cpOptionFlags = (jvxUInt16)1 << JVX_FLOAT_DATAFORMAT_SHIFT;
			memset(sdData.description, 0, JVX_ADV_DECRIPTION_LENGTH);
			jvx_fillCharStr((char*)sdData.description, JVX_ADV_DECRIPTION_LENGTH, params.nameConnection.c_str());
			jvxSize numS = 0;

			// ===================================================================================
			// Allocate receiver buffer
			// ===================================================================================
			sz_receiver_field_buffer_data = 0;
			if (params.numChannelsSend)
			{
				sz_receiver_field_buffer_data = (params.bsize * jvxDataFormat_getsize(params.format));
			}
			sz_receiver_field_buffer = sizeof(jvxAdvProtocolDataHeader) + sz_receiver_field_buffer_data;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(receive_field_buffer, jvxByte, sz_receiver_field_buffer + 1);
			dbg_pointer = receive_field_buffer + sz_receiver_field_buffer;

			// ===================================================================================
			// Allocate transmit buffer
			// ===================================================================================
			sz_transmit_field_buffer_data = 0;
			if (params.numChannelsReceive)
			{
				sz_transmit_field_buffer_data = (params.bsize * jvxDataFormat_getsize(params.format));
			}
			sz_transmit_field_buffer = sizeof(jvxAdvProtocolDataHeader) + sz_transmit_field_buffer_data;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(transmit_field_buffer, jvxByte, sz_transmit_field_buffer);
			jvxAdvProtocolDataHeader* ptrTo = (jvxAdvProtocolDataHeader*)transmit_field_buffer;
			ptrTo->adv_data_header.channel_mask = 0;
			ptrTo->adv_data_header.sequence_id = 0;

			ptrTo->header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_SIMPLE_AUDIO_DEVICE_LINK;
			ptrTo->header.paketsize = sz_transmit_field_buffer;
			ptrTo->header.purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_SPECIFY | JVX_PROTOCOL_SIMPLEDEVICE_SEND_DATA);

			// ===================================================================================
			// ===================================================================================

			// Reset monitoring variables
			monitor_num_error_packets = 0;
			monitor_num_successful_packets = 0;
			monitor_num_missed_packets = 0;
			sequenceId = JVX_SIZE_UNSELECTED;

			CjvxSocketsConnection_transfer myTrans((jvxByte*)&sdData, sdData.header.paketsize);
			res = send_message_no_lock(&myTrans, &numS);
			if (res == JVX_NO_ERROR)
			{
				localState = jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_ESTABLISHED;
			}
		}
		JVX_UNLOCK_MUTEX(safeAccessConnection);
	}
	return res;
}

jvxErrorType
CjvxAudioSocketsStateMachine::data_slave(jvxAudioSocketParameters* paramsArg)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (paramsArg)
	{
		paramsArg->bsize = cfg_remote.bsize;
		paramsArg->srate = cfg_remote.srate;
		paramsArg->format = (jvxDataFormat)cfg_remote.format;
		paramsArg->formGroup = (jvxDataFormatGroup)cfg_remote.format_group;
		paramsArg->nameConnection = (char*)cfg_remote.description;
		paramsArg->numChannelsSend = cfg_remote.channels_send;
		paramsArg->numChannelsReceive = cfg_remote.channels_recv;
	}
	return res;
}

jvxErrorType
CjvxAudioSocketsStateMachine::unlink_slave()
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (masterMode == false)
	{
		JVX_LOCK_MUTEX(safeAccessConnection);
		if (localState != jvxAudioSocketsState::JVX_AUDIOSTATE_NONE)
		{
			localState = jvxAudioSocketsState::JVX_AUDIOSTATE_DISCONNECTED;
			res = JVX_NO_ERROR;
		}
		JVX_UNLOCK_MUTEX(safeAccessConnection);
	}
	return res;
}

jvxErrorType
CjvxAudioSocketsStateMachine::terminate()
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	JVX_LOCK_MUTEX(safeAccessConnection);
	if (
		(localState == jvxAudioSocketsState::JVX_AUDIOSTATE_NONE) ||
		(localState == jvxAudioSocketsState::JVX_AUDIOSTATE_READY))
	{
		// These cases are for non-successful connects and 
		localState = jvxAudioSocketsState::JVX_AUDIOSTATE_NONE;		
		res = JVX_NO_ERROR;
	}
	JVX_UNLOCK_MUTEX(safeAccessConnection);

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		// The state machine may still be waiting for the connection to end
		jvxSize cnt = 0;
		while (localState != jvxAudioSocketsState::JVX_AUDIOSTATE_DISCONNECTED)
		{
			JVX_SLEEP_MS(200);
			cnt++;
			if (cnt > 10)
			{
				assert(0);
			}
		}

		if (localState == jvxAudioSocketsState::JVX_AUDIOSTATE_DISCONNECTED)
		{
			localState = jvxAudioSocketsState::JVX_AUDIOSTATE_NONE;
			res = JVX_NO_ERROR;
		}

		if (sz_receiver_field_buffer)
		{
			JVX_DSP_SAFE_DELETE_FIELD(receive_field_buffer);
		}

		sz_receiver_field_buffer = 0;

		if (sz_suck_unexpected_bytes)
		{
			JVX_DSP_SAFE_DELETE_FIELD(suck_unexpected_bytes);
		}
		sz_suck_unexpected_bytes = 0;
	}
	return res;
}

// ====================================================================
void
CjvxAudioSocketsStateMachine::procede_state_machine()
{
	JVX_LOCK_MUTEX(safeAccessConnection);
	switch (localState)
	{
	case jvxAudioSocketsState::JVX_AUDIOSTATE_CONNECTED:
	{
		jvxRemoteDeviceConfig sdData;

		monitor_num_error_packets = 0;
		monitor_num_successful_packets = 0;
		monitor_num_missed_packets = 0;
		sequenceId = JVX_SIZE_UNSELECTED;

		sdData.header.paketsize = sizeof(jvxRemoteDeviceConfig);
		sdData.header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_SIMPLE_AUDIO_DEVICE_LINK;
		sdData.header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_SPECIFY | JVX_PROTOCOL_SIMPLEDEVICE_CONFIGURE;

		sdData.bsize = params.bsize;
		sdData.srate = params.srate;
		sdData.format = params.format;
		sdData.format_group = params.formGroup;
		sdData.channels_send = params.numChannelsSend;
		sdData.channels_recv = params.numChannelsReceive;
		sdData.cpOptionFlags = (jvxUInt16)1 << JVX_FLOAT_DATAFORMAT_SHIFT;
		memset(sdData.description, 0, JVX_ADV_DECRIPTION_LENGTH);
		jvx_fillCharStr((char*)sdData.description, JVX_ADV_DECRIPTION_LENGTH, params.nameConnection);
		jvxSize numS = 0;
		CjvxSocketsConnection_transfer myTrans((jvxByte*)&sdData, sdData.header.paketsize);
		send_message_no_lock(&myTrans, &numS);
		localState = jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_INIT;
	}
	break;
	}
	JVX_UNLOCK_MUTEX(safeAccessConnection);
}

jvxErrorType
CjvxAudioSocketsStateMachine::send_message_no_lock(IjvxSocketsConnection_transfer * transfer, jvxSize * nums)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (connection)
	{
		res = connection->transfer(transfer, nums);
	}
	return res;
}

jvxErrorType
CjvxAudioSocketsStateMachine::send_message(IjvxSocketsConnection_transfer* transfer, jvxSize* nums)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccessConnection);
	if (connection)
	{
		connection->transfer(transfer, nums);
	}
	else
	{
		if (localState == jvxAudioSocketsState::JVX_AUDIOSTATE_DISCONNECTED)
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		else
		{
			if (localState > jvxAudioSocketsState::JVX_AUDIOSTATE_NONE)
			{
				res = JVX_ERROR_NOT_READY;
			}
		}
	}
	JVX_UNLOCK_MUTEX(safeAccessConnection);
	return res;
}

jvxErrorType
CjvxAudioSocketsStateMachine::disconnect()
{
	JVX_LOCK_MUTEX(safeAccessConnection);
	if (connection)
	{
		connection->disconnect();
	}
	JVX_UNLOCK_MUTEX(safeAccessConnection);
	return JVX_NO_ERROR;
}

/*
struct oneBufferTrans
{
	jvxHandle* ptr = nullptr;
	jvxSize sz = 0;
	jvxSize state = JVX_SIZE_UNSELECTED;
};

static oneBufferTrans storeDebug[1024];
static jvxSize cntDbg = 0;
*/

jvxErrorType
CjvxAudioSocketsStateMachine::provide_data_and_length(char** fld_use, jvxSize* lenField, jvxSize* offset, IjvxSocketsAdditionalInfoPacket* additionalInfo)
{
	switch (messStat)
	{
	case messageStatus::JVX_MESSAGE_STATUS_NONE:
		*fld_use = (jvxByte*)&hdr;
		*lenField = sizeof(hdr);
		break;
	case messageStatus::JVX_MESSAGE_STATUS_HEADER_OK:
		*fld_use = curPosition;
		*lenField = numBytesFollow;	
		break;
	case messageStatus::JVX_MESSAGE_STATUS_ERROR:
		*fld_use = curPosition;
		*lenField = JVX_MIN(numBytesFollow, sz_suck_unexpected_bytes);
		break;
	default:
		assert(0);
	}

	/*
	storeDebug[cntDbg].ptr = *fld_use;
	storeDebug[cntDbg].sz = *lenField;
	storeDebug[cntDbg].state = (jvxSize)messStat;
	cntDbg = (cntDbg + 1) % 1024;
	*/

	return JVX_NO_ERROR;
};

jvxErrorType
CjvxAudioSocketsStateMachine::report_data_and_read(char* fld_filled, jvxSize lenField, jvxSize offset, IjvxSocketsAdditionalInfoPacket* additionalInfo)
{
	jvxBool handleUnexpectedMessage = false;
	jvxBool reportStateSwitch = false;
	jvxBool reportIncomingBuffer = false;
	jvxAudioSocketsState newState = jvxAudioSocketsState::JVX_AUDIOSTATE_NONE;

	/*
	if (JVX_CHECK_SIZE_UNSELECTED(lenField))
	{
		// Report something
		int a = 0;
	}
	*/
	JVX_LOCK_MUTEX(safeAccessConnection);
	switch (messStat)
	{
	case messageStatus::JVX_MESSAGE_STATUS_NONE:

		// Here, we direct the signal to a specific buffer
		if (masterMode)
		{
			switch (localState)
			{
				// In INIT mode, the only accepted message is the configuration message
			case jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_INIT:
				if (
					(hdr.fam_hdr.proto_family == JVX_PROTOCOL_TYPE_SIMPLE_AUDIO_DEVICE_LINK) &&
					(hdr.purpose == (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE | JVX_PROTOCOL_SIMPLEDEVICE_CONFIGURE)))
				{
					if (hdr.paketsize == sizeof(cfg_remote))
					{
						cfg_remote.header = hdr;
						curPosition = (jvxByte*)&cfg_remote;
						curPosition += sizeof(hdr);
						numBytesFollow = hdr.paketsize - sizeof(hdr);
						messStat = messageStatus::JVX_MESSAGE_STATUS_HEADER_OK;
						messPurpose = hdr.purpose;
					}
					else
					{
						handleUnexpectedMessage = true;
					}
				}
				else
				{
					handleUnexpectedMessage = true;
				}
				break;
			case jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_ESTABLISHED:
				if (
					(hdr.fam_hdr.proto_family == JVX_PROTOCOL_TYPE_SIMPLE_AUDIO_DEVICE_LINK) &&
					(hdr.purpose == (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_SPECIFY | JVX_PROTOCOL_SIMPLEDEVICE_SEND_DATA)))
				{
					jvxAdvProtocolDataHeader* ptrStore = (jvxAdvProtocolDataHeader*)receive_field_buffer;
					ptrStore->header = hdr;
					curPosition = (jvxByte*)receive_field_buffer;
					curPosition += sizeof(hdr);
					assert(sz_receiver_field_buffer == hdr.paketsize);
					numBytesFollow = hdr.paketsize - sizeof(hdr);
					if (numBytesFollow)
					{
						messStat = messageStatus::JVX_MESSAGE_STATUS_HEADER_OK;
						messPurpose = hdr.purpose;
					}
					else
					{
						// This is a framing info with 0 channels. Report incoming buffer immediately
						reportIncomingBuffer = true;
						messPurpose = 0;
						messStat = messageStatus::JVX_MESSAGE_STATUS_NONE;
					}
				}
				break;
			default:
				handleUnexpectedMessage = true;
			}
		}
		else
		{ 
			switch (localState)
			{
			case jvxAudioSocketsState::JVX_AUDIOSTATE_CONNECTED:

				// The only expected message is the configure message
				if (
					(hdr.fam_hdr.proto_family == JVX_PROTOCOL_TYPE_SIMPLE_AUDIO_DEVICE_LINK) &&
					(hdr.purpose == (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_SPECIFY | JVX_PROTOCOL_SIMPLEDEVICE_CONFIGURE)))
				{
					if (hdr.paketsize == sizeof(cfg_remote))
					{
						cfg_remote.header = hdr;
						curPosition = (jvxByte*)&cfg_remote;
						curPosition += sizeof(hdr);
						numBytesFollow = hdr.paketsize - sizeof(hdr);
						messStat = messageStatus::JVX_MESSAGE_STATUS_HEADER_OK;
						messPurpose = hdr.purpose;
					}
					else
					{
						handleUnexpectedMessage = true;
					}
				}
				else
				{
					handleUnexpectedMessage = true;
				}

				break;
			case jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_ESTABLISHED:
				if (
					(hdr.fam_hdr.proto_family == JVX_PROTOCOL_TYPE_SIMPLE_AUDIO_DEVICE_LINK) &&
					(hdr.purpose == (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_SPECIFY | JVX_PROTOCOL_SIMPLEDEVICE_SEND_DATA)))
				{
					jvxAdvProtocolDataHeader* ptrStore = (jvxAdvProtocolDataHeader*)receive_field_buffer;
					ptrStore->header = hdr;
					curPosition = (jvxByte*)receive_field_buffer;
					curPosition += sizeof(hdr);
					assert(sz_receiver_field_buffer == hdr.paketsize);
					numBytesFollow = hdr.paketsize - sizeof(hdr);
					if (numBytesFollow)
					{
						messStat = messageStatus::JVX_MESSAGE_STATUS_HEADER_OK;
						messPurpose = hdr.purpose;
					}
					else
					{
						// This is a framing info with 0 channels. Report incoming buffer immediately
						reportIncomingBuffer = true;
						messPurpose = 0;
						messStat = messageStatus::JVX_MESSAGE_STATUS_NONE;
					}
				}
				else
				{
					handleUnexpectedMessage = true;
				}
				break;
			}
		}
		break;
	case messageStatus::JVX_MESSAGE_STATUS_HEADER_OK:

		numBytesFollow -= lenField;
		if (numBytesFollow == 0)
		{
			curPosition = NULL;
			switch (localState)
			{
			case jvxAudioSocketsState::JVX_AUDIOSTATE_CONNECTED:
				if (messPurpose == (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_SPECIFY | JVX_PROTOCOL_SIMPLEDEVICE_CONFIGURE))
				{
					localState = jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_INIT;
					newState = localState;
					reportStateSwitch = true;
					assert(!masterMode);
				}
				break;
			case jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_INIT:
				if (messPurpose == (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE | JVX_PROTOCOL_SIMPLEDEVICE_CONFIGURE))
				{
					localState = jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_ESTABLISHED;
					newState = localState;
					reportStateSwitch = true;
					assert(masterMode);
				}
				break;
			case jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_ESTABLISHED:
				if (messPurpose == (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_SPECIFY | JVX_PROTOCOL_SIMPLEDEVICE_SEND_DATA))
				{
					reportIncomingBuffer = true;
				}
			}
			messPurpose = 0;
			messStat = messageStatus::JVX_MESSAGE_STATUS_NONE;
		}
		else
		{
			curPosition += lenField;
		}
		break;
	case messageStatus::JVX_MESSAGE_STATUS_ERROR:
		numBytesFollow -= lenField;
		curPosition = suck_unexpected_bytes;
		if (numBytesFollow == 0)
		{
			// Error case complete
			messStat = messageStatus::JVX_MESSAGE_STATUS_NONE;
			monitor_num_error_packets ++;
			curPosition = nullptr;
		}
		break;
	}

	JVX_UNLOCK_MUTEX(safeAccessConnection);

	if (handleUnexpectedMessage)
	{
		messStat = messageStatus::JVX_MESSAGE_STATUS_ERROR;
		curPosition = (jvxByte*)suck_unexpected_bytes;
		numBytesFollow = hdr.paketsize - sizeof(hdr);
	}

	if (reportStateSwitch)
	{
		if (ptr_common)
		{
			ptr_common->report_state_switch(newState);
		}
	}

	if (reportIncomingBuffer)
	{
		jvxBool firstBuffer = false;
		jvxBool lastBuffer = false;
		jvxAdvProtocolDataHeader* ptrStore = (jvxAdvProtocolDataHeader*)receive_field_buffer;

		monitor_num_successful_packets++;

		if (ptrStore->adv_data_header.channel_mask & ((jvxUInt64)1 << 62))
		{
			firstBuffer = true;
		}
		if (ptrStore->adv_data_header.channel_mask & ((jvxUInt64)1 << 63))
		{
			lastBuffer = true;
		}

		// Now process
		if (firstBuffer)
		{
			if (JVX_CHECK_SIZE_SELECTED(sequenceId))
			{
				int deltaSeq = (int)ptrStore->adv_data_header.sequence_id - (int)sequenceId;
				if(deltaSeq != 1)
				{
					monitor_num_missed_packets += deltaSeq;
				}
			}	
			sequenceId = ptrStore->adv_data_header.sequence_id;

			// Inform about new buffer
			if (ptr_common)
			{
				if (masterMode)
				{
					ptr_common->report_new_frame_start(
						params.bsize,
						params.format,
						params.numChannelsReceive,
						sequenceId);
				}
				else
				{
					ptr_common->report_new_frame_start(
						params.bsize,
						params.format,
						params.numChannelsSend,
						sequenceId);
					frameError = false;
				}
			}
		}
		else
		{
			if (JVX_CHECK_SIZE_SELECTED(sequenceId))
			{
				int deltaSeq = (int)ptrStore->adv_data_header.sequence_id - (int)sequenceId;
				if (deltaSeq > 0)
				{
					monitor_num_missed_packets += deltaSeq;
					frameError = true;
				}
			}
		}

		// Copy content 
		if (ptr_common)
		{
			jvxSize idxChannel = JVX_SIZE_UNSELECTED;
			if (ptrStore->adv_data_header.channel_mask == (jvxUInt64)-1)
			{
				// This would be an empty buffer! It is nevertheless valid if 0 channels are transferred!
			}
			else
			{
				idxChannel = ptrStore->adv_data_header.channel_mask & 0x3FFFFFFF;
				jvxByte* buf = receive_field_buffer;
				buf += sizeof(jvxAdvProtocolDataHeader);
				ptr_common->report_channel_buffer(
					buf,
					idxChannel);
			}
		}
		// Check last frame
		if (lastBuffer)
		{
			// Inform one frame compete
			if (ptr_common)
			{
				ptr_common->report_frame_complete(frameError, sequenceId);
			}
		}

	}

	return JVX_NO_ERROR;
};

// ==========================================================================================================

jvxErrorType 
CjvxAudioSocketsStateMachine::send_buffer_response_start(jvxHandle** buffer_copyData,
	jvxSize* space, jvxDataFormat* form, jvxSize* numChannels)
{
	if (buffer_copyData)
	{
		if (sz_transmit_field_buffer_data)
		{
			*buffer_copyData = transmit_field_buffer;
		}
		else
		{
			*buffer_copyData = nullptr;
		}
	}
	if (space)
	{
		*space = sz_transmit_field_buffer_data;
	}
	if (form)
	{
		*form = params.format;
	}
	if (numChannels)
	{
		*numChannels = params.numChannelsReceive;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioSocketsStateMachine::send_buffer_response_data(jvxUInt64 channel_mask, jvxSize seq_id)
{
	jvxSize numS;
	jvxAdvProtocolDataHeader* ptrStore = (jvxAdvProtocolDataHeader*)transmit_field_buffer;
	ptrStore->adv_data_header.channel_mask = channel_mask;
	ptrStore->adv_data_header.sequence_id = seq_id;
	CjvxSocketsConnection_transfer tf(transmit_field_buffer, sz_transmit_field_buffer);

	// Tyically, we are within the receive loop at this point, hence the connection may not have 
	// been removed!
	this->send_message_no_lock(&tf, &numS);
	return JVX_NO_ERROR;
}

// ==========================================================================================================

jvxErrorType
CjvxAudioSocketsStateMachine::report_connection_error(jvxSocketsErrorType errCode, const char* errDescription)
{
	std::cout << __FUNCTION__ << std::endl;
	return JVX_NO_ERROR;
};

// ==========================================================================================================

jvxErrorType
CjvxAudioSocketsStateMachine::report_client_connect(IjvxSocketsConnection* connectionRef)
{
	jvxBool trigger_state_machine = false;
	jvxAudioSocketsState newState = jvxAudioSocketsState::JVX_AUDIOSTATE_NONE;

	JVX_LOCK_MUTEX(safeAccessConnection);
	assert(localState == jvxAudioSocketsState::JVX_AUDIOSTATE_READY);
	connection = connectionRef;
	connection->register_report(static_cast<IjvxSocketsConnection_report*>(this));
	localState = jvxAudioSocketsState::JVX_AUDIOSTATE_CONNECTED;
	newState = localState;
	trigger_state_machine = true;
	JVX_UNLOCK_MUTEX(safeAccessConnection);

	if (trigger_state_machine)
	{
		if (ptr_common)
		{
			ptr_common->report_state_switch(newState);
		}
	}
	// Send initialize message if master
	return JVX_NO_ERROR;
};

jvxErrorType
CjvxAudioSocketsStateMachine::report_client_disconnect(IjvxSocketsConnection* connectionArg)
{
	std::cout << __FUNCTION__ << std::endl;

	JVX_LOCK_MUTEX(safeAccessConnection);
	if (
		(localState == jvxAudioSocketsState::JVX_AUDIOSTATE_CONNECTED) ||
		(localState == jvxAudioSocketsState::JVX_AUDIOSTATE_READY) ||
		(localState == jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_INIT) ||
		(localState == jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_ESTABLISHED))
	{
		localState = jvxAudioSocketsState::JVX_AUDIOSTATE_DISCONNECTED;
		connection->unregister_report(static_cast<IjvxSocketsConnection_report*>(this));
		connection = NULL;
	}
	JVX_UNLOCK_MUTEX(safeAccessConnection);

	return JVX_NO_ERROR;
};

jvxErrorType
CjvxAudioSocketsStateMachine::report_client_error(jvxSocketsErrorType err, const char* description)
{
	std::cout << __FUNCTION__ << std::endl;
	return JVX_NO_ERROR;
};

// ======================================================================
