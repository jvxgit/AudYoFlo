#ifndef __CJVXAUDIOSOCKETSTATEMASCHINE_H__
#define __CJVXAUDIOSOCKETSTATEMASCHINE_H__

#include "jvx.h"
#include "CjvxSockets.h"

// =========================================================

enum class jvxAudioSocketsState
{
	JVX_AUDIOSTATE_NONE,
	JVX_AUDIOSTATE_READY,
	JVX_AUDIOSTATE_CONNECTED,
	JVX_AUDIOSTATE_XCHANGE_INIT,
	JVX_AUDIOSTATE_XCHANGE_ESTABLISHED,
	JVX_AUDIOSTATE_DISCONNECTED
};

class jvxAudioSocketParameters
{
public:
	jvxSize bsize = 0;
	jvxSize srate = 0;
	jvxSize numChannelsSend = 0;
	jvxSize numChannelsReceive = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxDataFormatGroup formGroup = JVX_DATAFORMAT_GROUP_NONE;
	std::string nameConnection = "no-name";

	jvxAudioSocketParameters& operator = (const jvxAudioSocketParameters& arg)
	{
		bsize = arg.bsize;
		srate = arg.srate;
		numChannelsSend = arg.numChannelsSend;
		numChannelsReceive = arg.numChannelsReceive;
		format = arg.format;
		formGroup = arg.formGroup;
		nameConnection = arg.nameConnection;
		return *this;
	}
};

// =========================================================
// =========================================================

JVX_INTERFACE IjvxAudioSocketsStateMachine_common
{
public:
	virtual ~IjvxAudioSocketsStateMachine_common() {};

	virtual jvxErrorType report_new_frame_start(
		jvxSize bsize_data,
		jvxDataFormat format_data,
		jvxSize numchannels_data,
		jvxSize seq_id) = 0;

	virtual jvxErrorType report_channel_buffer(
		jvxHandle* buffer,		
		jvxSize chan_id_idx) = 0;

	virtual jvxErrorType report_frame_complete(jvxBool detectedError, jvxSize seq_id) = 0;

	virtual jvxErrorType report_state_switch(jvxAudioSocketsState newState) = 0;
};

/*
JVX_INTERFACE IjvxAudioSocketsStateMachine_slave
{
public:
	virtual ~IjvxAudioSocketsStateMachine_slave() {};
	virtual jvxErrorType connection_established() = 0;
};
*/

// ======================================================================

class CjvxAudioSocketsStateMachine :
	public IjvxSocketsConnection_report,
	public IjvxSocketsClient_report
{
public:

	IjvxSocketsConnection* connection = nullptr;

	enum class messageStatus
	{
		JVX_MESSAGE_STATUS_NONE,
		JVX_MESSAGE_STATUS_HEADER_OK,
		JVX_MESSAGE_STATUS_ERROR
	};

	jvxAudioSocketsState localState = jvxAudioSocketsState::JVX_AUDIOSTATE_NONE;
	IjvxAudioSocketsStateMachine_common* ptr_common = nullptr;
	JVX_MUTEX_HANDLE safeAccessConnection;	

	// Message parser
	jvxProtocolHeader hdr = { 0 };
	jvxByte* curPosition = nullptr;
	messageStatus messStat = messageStatus::JVX_MESSAGE_STATUS_NONE;
	jvxUInt16 messPurpose = 0;
	jvxSize numBytesFollow = 0;

	jvxBool masterMode = true;

	// Master mode
	jvxAudioSocketParameters params;

	// Slave mode
	jvxRemoteDeviceConfig cfg_remote;

	jvxByte* suck_unexpected_bytes = nullptr;
	jvxSize sz_suck_unexpected_bytes = 0;

	jvxByte* receive_field_buffer = nullptr;
	jvxSize sz_receiver_field_buffer = 0;
	jvxSize sz_receiver_field_buffer_data = 0;

	jvxByte* transmit_field_buffer = nullptr;
	jvxSize sz_transmit_field_buffer = 0;
	jvxSize sz_transmit_field_buffer_data = 0;

	jvxByte* dbg_pointer = nullptr;
	jvxSize monitor_num_error_packets = 0;
	jvxSize monitor_num_successful_packets = 0;
	jvxSize monitor_num_missed_packets = 0;

	jvxSize sequenceId = JVX_SIZE_UNSELECTED;
	jvxBool frameError = false;
public:

	CjvxAudioSocketsStateMachine();
	~CjvxAudioSocketsStateMachine();

	jvxErrorType initialize_master(
		jvxAudioSocketParameters* params,
		IjvxAudioSocketsStateMachine_common* report_common,
		jvxSize szBytesErrorBuffer = 1024);

	jvxErrorType initialize_slave(
		IjvxAudioSocketsStateMachine_common* report_common,
		IjvxSocketsConnection* connRef,
		jvxSize szBytesErrorBuffer = 1024);

	jvxErrorType data_slave(jvxAudioSocketParameters* params);
	jvxErrorType accept_slave(jvxAudioSocketParameters* params);
	jvxErrorType unlink_slave();

	jvxErrorType terminate();

	// ====================================================================
	
	void procede_state_machine();
	jvxErrorType send_message_no_lock(IjvxSocketsConnection_transfer* transfer, jvxSize* nums);

	// ====================================================================

	jvxErrorType send_buffer_response_start(jvxHandle** buffer_copyData,
		jvxSize* space, jvxDataFormat* form, jvxSize* numChannels);
	jvxErrorType send_buffer_response_data(jvxUInt64 chan_mask, jvxSize seq_id);
	
	// ====================================================================
		
	jvxErrorType send_message(IjvxSocketsConnection_transfer* transfer, jvxSize* nums);
	jvxErrorType disconnect();
	virtual jvxErrorType JVX_CALLINGCONVENTION provide_data_and_length(char** fld_use, jvxSize* lenField, jvxSize* offset, IjvxSocketsAdditionalInfoPacket* additionalInfo)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_data_and_read(char* fld_filled, jvxSize lenField, jvxSize offset, IjvxSocketsAdditionalInfoPacket* additionalInfo)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_connection_error(jvxSocketsErrorType errCode, const char* errDescription)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_connect(IjvxSocketsConnection* connectionRef)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_disconnect(IjvxSocketsConnection* connection)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_error(jvxSocketsErrorType err, const char* description)override;

	// ======================================================================
};

#endif
