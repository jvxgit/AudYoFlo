#include "jvx_audio_stack.h"
#include "jvx_dsp_base.h"

#define JVX_PRESET_PACKAGE_SEND(theHeader, TYPE, PURPOSE, COMMAND) \
	sz = JVX_SIZE_INT32(sizeof(TYPE)); \
	assert(myPriv->work.minsize_buf >= myPriv->work.offset_buf); \
	sz = JVX_MAX(sz, myPriv->work.minsize_buf - myPriv->work.offset_buf); \
	assert((sz + myPriv->work.offset_buf) <= myPriv->work.size_buf); \
	theHeader->paketsize = JVX_SIZE_UINT32( sz); \
	sz = theHeader->paketsize + myPriv->work.offset_buf; \
	theHeader->purpose = ( PURPOSE | COMMAND); \
	theHeader->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK; \
	myPriv->expected_command = COMMAND


typedef struct
{
	jvxAudioStack init;

	jvxAudioStack_state state;

	struct
	{
		jvxAudioStack_state state;
		jvxAudioDataParams_master params;
	} other;

	struct
	{
		jvxByte* buf;
		jvxSize size_buf;
		jvxSize offset_buf;
		jvxSize minsize_buf;
	} work;

	jvxUInt16 expected_type;
	jvxAdvLinkProtocolCommandType expected_command;

} jvxAudioStack_prv;

jvxDspBaseErrorType 
jvx_audio_stack_initCfg(jvxAudioStack* hdl_stack)
{
	hdl_stack->prv = NULL;

	hdl_stack->common.allocate_callback = NULL;
	hdl_stack->common.reset_callback = NULL;
	hdl_stack->common.error_callback = NULL;
	hdl_stack->common.prv_callbacks = NULL;
	hdl_stack->common.sendfail_callback = NULL;
	hdl_stack->common.send_callback = NULL;
	hdl_stack->common.started_callback = NULL;
	hdl_stack->common.stopped_callback = NULL;
	hdl_stack->common.data_callback = NULL;

	hdl_stack->delayed_send = false;
	hdl_stack->master.hw_mode = JVX_PROTOCOL_ADVLINK_MODE_PCM_IN_PCM_OUT;

	hdl_stack->operation = JVX_AUDIO_STACK_SLAVE;

	hdl_stack->slave.mode_callback = NULL;
	hdl_stack->slave.params.bsize = 128;
	hdl_stack->slave.params.srate = 128;
	hdl_stack->slave.params.format = JVX_DATAFORMAT_16BIT_LE;
	memset(hdl_stack->slave.params.description, 0, JVX_ADV_DECRIPTION_LENGTH);
	hdl_stack->slave.params.mode = JVX_PROTOCOL_ADVLINK_MODE_PCM_IN_PCM_OUT;
	hdl_stack->slave.params.frametype = JVX_PROTOCOL_ADV_CHANNEL_FRAME_TYPE_NONE;
	hdl_stack->slave.params.num_channels_from_external = 1;
	hdl_stack->slave.params.num_channels_to_external = 1;
	hdl_stack->slave.params.trigRole = JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_SOURCE;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_audio_stack_init(jvxAudioStack* hdl_stack)
{
	if (hdl_stack->prv == NULL)
	{
		jvxAudioStack_prv* myPriv = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(myPriv, jvxAudioStack_prv);

		myPriv->work.buf = NULL;
		myPriv->work.size_buf = 0;
		myPriv->work.offset_buf = 0;
		myPriv->work.minsize_buf = 0;

		myPriv->init = *hdl_stack;

		hdl_stack->prv = myPriv;

		myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType
jvx_audio_stack_terminate(jvxAudioStack* hdl_stack)
{
	if (hdl_stack->prv != NULL)
	{
	    jvxAudioStack_prv* delMe = (jvxAudioStack_prv*)hdl_stack->prv;
		JVX_DSP_SAFE_DELETE_OBJECT(delMe);
		hdl_stack->prv = NULL;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType
jvx_audio_stack_packet_input(jvxAudioStack* hdl_stack, jvxByte* fld, jvxSize offset, jvxSize fldSize, jvxCBool* need_more_input, jvxByte** send_ptr, jvxSize* send_sz)
{
	jvxProtocolHeader* header = NULL;
	//jvxAdvLinkProtocolCommandType purpose_command = JVX_PROTOCOL_ADVLINK_ERROR_RESET;
	//jvxAdvLinkProtocolMessagePurpose purpose_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST;
	jvxDspBaseErrorType resL;
	jvxUInt32 sz = 0;

	jvxCBool reportConnectionReset = c_false;
	jvxCBool reportPrototcolError = c_false;
	jvxCBool reportSendFailed = c_false;
	jvxCBool reportStarted = c_false;
	jvxCBool reportStopped = c_false;
	jvxCBool triggerSend = c_false;

	jvxProtocolHeader* theHeaderOut = NULL;
	if(hdl_stack->prv)
	{
		jvxAudioStack_prv* myPriv = (hdl_stack->prv);

		if ((fldSize - offset) >= sizeof(jvxProtocolHeader))
		{
			header = (jvxProtocolHeader*)(fld + offset);
			jvxAdvLinkProtocolCommandType purpose_command = (jvxAdvLinkProtocolCommandType)(header->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK);
			jvxAdvLinkProtocolMessagePurpose purpose_type = (jvxAdvLinkProtocolMessagePurpose)(header->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK);
			jvxUInt32 szPack = header->paketsize;

			if ((fldSize - offset) >= szPack)
			{
                // Packet is complete
                theHeaderOut = (jvxProtocolHeader*)(myPriv->work.buf + myPriv->work.offset_buf);

				switch (purpose_command)
				{
				case JVX_PROTOCOL_ADVLINK_ERROR_RESET:

					if (purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST)
					{
						JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_ERROR_RESET);
						myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;
						triggerSend = true;
						myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
						reportConnectionReset = true;
					}				
					break;
                case JVX_PROTOCOL_ADVLINK_SEND_DATA:
                    if (myPriv->state == JVX_AUDIOSTACK_STATE_PROCESSING)
                    {
                        jvxAdvProtocolDataP* data = (jvxAdvProtocolDataP*)header;
                        jvxByte* dataDir = (jvxByte*)&data->first_byte_transfer;
                        if (myPriv->init.common.data_callback)
                        {
                            resL = myPriv->init.common.data_callback(myPriv->init.common.prv_callbacks, dataDir, header->paketsize - (sizeof(jvxAdvProtocolDataP) - sizeof(jvxUInt8)), 
								data->dheader.adv_data_header.channel_mask, 
								data->dheader.adv_data_header.sequence_id);
                        }
                    }
                    break;
				}
				
				switch (myPriv->init.operation)
				{
#ifndef JVX_AUDIO_STACK_SLAVE_ONLY
				case JVX_AUDIO_STACK_MASTER:

					switch (purpose_command)
					{
					case JVX_PROTOCOL_ADVLINK_ERROR_RESET:

						if (purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)
						{
							if (
								(
								(
									(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
									) ||
									(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
									) && (myPriv->state == JVX_AUDIOSTACK_STATE_INIT))
							{
								JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_STATUS);
								myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
								triggerSend = true;

								myPriv->state = JVX_AUDIOSTACK_STATE_STATUS;
							}
							else
							{
								myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
								reportPrototcolError = true;
							}
						}
						else if (purpose_type != JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST)
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;
					case JVX_PROTOCOL_ADVLINK_STATUS:

						// Status connected partner is reported
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_STATUS))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							myPriv->other.state = (jvxAudioStack_state)hdrInT->first_transfer_load;

							// If we are in state to connect, send next message in sequence
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_INIT);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_CONNECTION_INIT;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;
					case JVX_PROTOCOL_ADVLINK_INIT:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_CONNECTION_INIT))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_DESCR);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_CONNECTION_DESCR;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_DESCR:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_CONNECTION_DESCR))
						{

							jvxAdvProtocolDescriptionP* hdrInD = (jvxAdvProtocolDescriptionP*)header;
							memcpy(myPriv->other.params.description, hdrInD->description, JVX_ADV_DECRIPTION_LENGTH);

							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_MASK);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_CONNECTION_ROLE;

						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_MASK:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_CONNECTION_ROLE))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							myPriv->other.params.trigRole = (jvxAdvLinkProtocolClockTriggerRole)hdrInT->first_transfer_load;

							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxAdvProtocolTransferP, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_SPECIFY_MODE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;

							jvxAdvProtocolTransferP* theHeaderOutPlus = (jvxAdvProtocolTransferP*)theHeaderOut;
							theHeaderOutPlus->first_transfer_load = (jvxUInt32)myPriv->init.master.hw_mode;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_CONNECTION_MODE;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_SPECIFY_MODE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_CONNECTION_MODE))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							myPriv->other.params.mode = hdrInT->first_transfer_load;

							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_ACTIVATE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_INIT;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_ACTIVATE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_INIT))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_SRATE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_SRATE;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_SRATE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_SRATE))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							myPriv->other.params.srate = JVX_SIZE_INT32(hdrInT->first_transfer_load);

							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_BSIZE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_BSIZE;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_BSIZE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_BSIZE))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							myPriv->other.params.bsize = JVX_SIZE_INT32(hdrInT->first_transfer_load);

							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_FORMAT);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_FORMAT;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_FORMAT:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_FORMAT))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							myPriv->other.params.format = (jvxDataFormat)JVX_SIZE_INT32(hdrInT->first_transfer_load);

							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_SLAVE_TO_MASTER);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_INCHANS;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_SLAVE_TO_MASTER:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && 
								(myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_INCHANS))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							myPriv->other.params.num_channels_slave_to_master = JVX_SIZE_INT32(hdrInT->first_transfer_load);

							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_MASTER_TO_SLAVE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_OUTCHANS;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_MASTER_TO_SLAVE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_OUTCHANS))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							myPriv->other.params.num_channels_master_to_slave = JVX_SIZE_INT32(hdrInT->first_transfer_load);

							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_CHANNEL_FRAME_TYPE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_FRAMETYPE;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_CHANNEL_FRAME_TYPE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_FRAMETYPE))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							myPriv->other.params.frametype = (jvxAdvLinkProtocolChannelFrameType)JVX_SIZE_INT32(hdrInT->first_transfer_load);
							myPriv->state = JVX_AUDIOSTACK_STATE_AWAIT_START;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_START:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_PROCESSING))
						{
							reportStarted = true;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;
					case JVX_PROTOCOL_ADVLINK_STOP:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_AWAIT_START))
						{
							reportStopped = true;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;
					default:
					    break;
					}
					break;
#endif
					// =========================================================================
					// =========================================================================
					// =========================================================================

				case JVX_AUDIO_STACK_SLAVE:

					switch (purpose_command)
					{
					
					case JVX_PROTOCOL_ADVLINK_STATUS:

						// Status connected partner is reported
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_NONE))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							myPriv->other.state = (jvxAudioStack_state)hdrInT->first_transfer_load;

							// If we are in state to connect, send next message in sequence
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_STATUS);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_CONNECTION_INIT;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;
					case JVX_PROTOCOL_ADVLINK_INIT:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_CONNECTION_INIT))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_INIT);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_CONNECTION_DESCR;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_DESCR:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_CONNECTION_DESCR))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxAdvProtocolDescriptionP, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_DESCR);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;

							jvxAdvProtocolDescriptionP* hdrOutD = (jvxAdvProtocolDescriptionP*)theHeaderOut;
							memcpy(hdrOutD->description, myPriv->init.slave.params.description, JVX_ADV_DECRIPTION_LENGTH);
							triggerSend = true;
                            myPriv->state = JVX_AUDIOSTACK_STATE_CONNECTION_ROLE;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_MASK:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_CONNECTION_ROLE))
						{

							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxAdvProtocolTransferP, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_MASK);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;

							jvxAdvProtocolTransferP* hdrOutT = (jvxAdvProtocolTransferP*)theHeaderOut;
							hdrOutT->first_transfer_load = (jvxUInt32)myPriv->init.slave.params.trigRole;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_CONNECTION_MODE;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_SPECIFY_MODE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_CONNECTION_MODE))
						{
							jvxAdvProtocolTransferP* hdrInT = (jvxAdvProtocolTransferP*)header;
							jvxSize mode = hdrInT->first_transfer_load;

							if (myPriv->init.slave.mode_callback)
							{
								myPriv->init.slave.mode_callback(myPriv->init.common.prv_callbacks, &mode);
							}

							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxAdvProtocolTransferP, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_SPECIFY_MODE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;

							jvxAdvProtocolTransferP* hdrOutT = (jvxAdvProtocolTransferP*)theHeaderOut;
							hdrOutT->first_transfer_load = mode;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_INIT;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_ACTIVATE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_INIT))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_ACTIVATE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_SRATE;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_SRATE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_SRATE))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxAdvProtocolTransferP, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_SRATE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;

							jvxAdvProtocolTransferP* hdrOutT = (jvxAdvProtocolTransferP*)theHeaderOut;
							hdrOutT->first_transfer_load = JVX_SIZE_UINT32(myPriv->init.slave.params.srate);
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_BSIZE;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_BSIZE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_BSIZE))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxAdvProtocolTransferP, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_BSIZE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;

							jvxAdvProtocolTransferP* hdrOutT = (jvxAdvProtocolTransferP*)theHeaderOut;
							hdrOutT->first_transfer_load = JVX_SIZE_UINT32(myPriv->init.slave.params.bsize);
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_FORMAT;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_FORMAT:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_FORMAT))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxAdvProtocolTransferP, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_FORMAT);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;

							jvxAdvProtocolTransferP* hdrOutT = (jvxAdvProtocolTransferP*)theHeaderOut;
							hdrOutT->first_transfer_load = myPriv->init.slave.params.format;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_INCHANS;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_SLAVE_TO_MASTER:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_INCHANS))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxAdvProtocolTransferP, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_SLAVE_TO_MASTER);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;

							jvxAdvProtocolTransferP* hdrOutT = (jvxAdvProtocolTransferP*)theHeaderOut;
							hdrOutT->first_transfer_load = JVX_SIZE_UINT32(myPriv->init.slave.params.num_channels_to_external);
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_OUTCHANS;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_MASTER_TO_SLAVE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_OUTCHANS))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxAdvProtocolTransferP, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_MASTER_TO_SLAVE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;

							jvxAdvProtocolTransferP* hdrOutT = (jvxAdvProtocolTransferP*)theHeaderOut;
							hdrOutT->first_transfer_load = JVX_SIZE_UINT32(myPriv->init.slave.params.num_channels_from_external);
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_ACTIVE_FRAMETYPE;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;

					case JVX_PROTOCOL_ADVLINK_CHANNEL_FRAME_TYPE:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
								( 
									(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
							)&&
							(myPriv->state == JVX_AUDIOSTACK_STATE_ACTIVE_FRAMETYPE))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxAdvProtocolTransferP, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_CHANNEL_FRAME_TYPE);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;

							jvxAdvProtocolTransferP* hdrOutT = (jvxAdvProtocolTransferP*)theHeaderOut;
							hdrOutT->first_transfer_load = JVX_SIZE_UINT32(myPriv->init.slave.params.frametype);
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_AWAIT_START;
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;
					case JVX_PROTOCOL_ADVLINK_START:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_AWAIT_START))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_START);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;
							reportStarted = true;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_PROCESSING;
							
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;
					case JVX_PROTOCOL_ADVLINK_STOP:
						if (
							(purpose_type == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
							(
							(
								(myPriv->expected_command == purpose_command) && (myPriv->expected_type == purpose_type)
								) ||
								(myPriv->expected_type == JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK)
								) && (myPriv->state == JVX_AUDIOSTACK_STATE_PROCESSING))
						{
							JVX_PRESET_PACKAGE_SEND(theHeaderOut, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE, JVX_PROTOCOL_ADVLINK_STOP);
							myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;
							reportStopped = true;
							triggerSend = true;
							myPriv->state = JVX_AUDIOSTACK_STATE_AWAIT_START;
							
						}
						else
						{
							myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
							reportPrototcolError = true;
						}
						break;
					default:
					    break;
					}
					break;
				} // switch (myPriv->init.operation)

				if (triggerSend)
				{
					if (myPriv->init.delayed_send)
					{
						assert(send_ptr);
						assert(send_sz);
						*send_ptr = myPriv->work.buf;
						*send_sz = sz;
					}
					else
					{
						resL = JVX_DSP_ERROR_CALL_SUB_COMPONENT_FAILED;
						if (myPriv->init.common.send_callback)
						{
							resL = myPriv->init.common.send_callback(myPriv->init.common.prv_callbacks, myPriv->work.buf, sz);
						}
						if (resL != JVX_DSP_NO_ERROR)
						{
							reportSendFailed = c_true;
						}
					}
				}

				// ====================================================

				if (reportPrototcolError)
				{
					if (myPriv->init.common.error_callback)
					{
						myPriv->init.common.error_callback(myPriv->init.common.prv_callbacks);
					}
					
				}
				if(reportSendFailed)
				{ 
					if (myPriv->init.common.sendfail_callback)
					{
						myPriv->init.common.sendfail_callback(myPriv->init.common.prv_callbacks);
					}
				}
				if (reportStarted)
				{
					if (myPriv->init.common.started_callback)
					{
						myPriv->init.common.started_callback(myPriv->init.common.prv_callbacks);
					}
				}
				if (reportStopped)
				{
					if (myPriv->init.common.stopped_callback)
					{
						myPriv->init.common.stopped_callback(myPriv->init.common.prv_callbacks);
					}
				}


                if (reportSendFailed || reportPrototcolError )
				{
					return JVX_DSP_ERROR_INTERNAL;
				}

                if (reportConnectionReset)
                {
                    if (myPriv->init.common.reset_callback)
                    {
                        myPriv->init.common.reset_callback(myPriv->init.common.prv_callbacks);
                    }
                }
				return JVX_DSP_NO_ERROR;
			}
			else
			{
				*need_more_input = false;
				return JVX_DSP_ERROR_ABORT;
			}
		}
		else
		{
			*need_more_input = false;
			return JVX_DSP_ERROR_ABORT;
		}	
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

// ==============================================================================================
jvxDspBaseErrorType
jvx_audio_stack_connect(jvxAudioStack* hdl_stack, jvxByte** send_ptr, jvxSize* send_sz)
{
	jvxSize sz;
	if (hdl_stack->prv)
	{
		jvxAudioStack_prv* myPriv = (hdl_stack->prv);

		if (myPriv->state == JVX_AUDIOSTACK_STATE_NONE)
		{
			myPriv->other.state = JVX_AUDIOSTACK_STATE_UNKNOWN;
			memset(myPriv->other.params.description, 0, JVX_ADV_DECRIPTION_LENGTH);
			myPriv->other.params.trigRole = JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_SOURCE;

			myPriv->other.params.srate = JVX_SIZE_UNSELECTED;
			myPriv->other.params.bsize = JVX_SIZE_UNSELECTED;
			myPriv->other.params.format = JVX_DATAFORMAT_NONE;
			myPriv->other.params.frametype = JVX_PROTOCOL_ADV_CHANNEL_FRAME_TYPE_NONE;
			myPriv->other.params.mode = JVX_SIZE_UNSELECTED;
			myPriv->other.params.num_channels_slave_to_master = JVX_SIZE_UNSELECTED;
			myPriv->other.params.num_channels_master_to_slave = JVX_SIZE_UNSELECTED;

			jvxDspBaseErrorType resL = JVX_DSP_ERROR_INVALID_SETTING;
			if (myPriv->init.common.allocate_callback)
			{
				resL = myPriv->init.common.allocate_callback(myPriv->init.common.prv_callbacks,
					&myPriv->work.buf, &myPriv->work.size_buf, &myPriv->work.offset_buf,
					&myPriv->work.minsize_buf, sizeof(jvxAdvProtocolUnion));
			}
			if (resL != JVX_DSP_NO_ERROR)
			{
				return resL;
			}

			myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK;

			if (myPriv->init.operation == JVX_AUDIO_STACK_MASTER)
			{
				// =====================================================================
				jvxProtocolHeader* theHeader = (jvxProtocolHeader*)(myPriv->work.buf + myPriv->work.offset_buf);

				// Set DSP into initial state
				JVX_PRESET_PACKAGE_SEND(theHeader, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_ERROR_RESET);
				myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;

				if (myPriv->init.delayed_send)
				{
					assert(send_ptr);
					assert(send_sz);
					*send_ptr = myPriv->work.buf;
					*send_sz = sz;
				}
				else
				{
					jvxDspBaseErrorType resL = JVX_DSP_ERROR_INVALID_SETTING;
					if (myPriv->init.common.send_callback)
					{
						resL = myPriv->init.common.send_callback(myPriv->init.common.prv_callbacks, myPriv->work.buf, sz);
					}
					if (resL != JVX_DSP_NO_ERROR)
					{
						return JVX_DSP_ERROR_INTERNAL;
					}
				}
			}
			myPriv->state = JVX_AUDIOSTACK_STATE_INIT;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType
jvx_audio_stack_disconnect(jvxAudioStack* hdl_stack)
{
	if (hdl_stack->prv)
	{
		jvxAudioStack_prv* myPriv = (hdl_stack->prv);

		if (myPriv->state > JVX_AUDIOSTACK_STATE_NONE)
		{
			jvxDspBaseErrorType resL = JVX_DSP_ERROR_INVALID_SETTING;
			if (myPriv->init.common.deallocate_callback)
			{
				resL = myPriv->init.common.deallocate_callback(myPriv->init.common.prv_callbacks,
					myPriv->work.buf);
				assert(resL == JVX_DSP_NO_ERROR);
			}
			myPriv->work.buf = NULL;
			myPriv->work.size_buf = 0;
			myPriv->work.offset_buf = 0;
			myPriv->work.minsize_buf = 0;

			myPriv->state = JVX_AUDIOSTACK_STATE_NONE;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_state(jvxAudioStack* hdl_stack, jvxAudioStack_state* theStat)
{
	if (hdl_stack->prv)
	{
		jvxAudioStack_prv* myPriv = (hdl_stack->prv);

		if (theStat)
		{
			*theStat = myPriv->state;
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_connected_params(jvxAudioStack* hdl_stack, jvxAudioDataParams_master* params)
{
	if (hdl_stack->prv)
	{
		jvxAudioStack_prv* myPriv = (hdl_stack->prv);

		if(myPriv->state >= JVX_AUDIOSTACK_STATE_AWAIT_START)
		{
			if(params)
			{
				*params = myPriv->other.params;
			}
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType 
jvx_audio_stack_trigger_start(jvxAudioStack* hdl_stack, jvxByte** send_ptr, jvxSize* send_sz)
{
	jvxSize sz;
	if (hdl_stack->prv)
	{
		jvxAudioStack_prv* myPriv = (hdl_stack->prv);
		if (myPriv->state == JVX_AUDIOSTACK_STATE_AWAIT_START)
		{
			// =====================================================================
			jvxProtocolHeader* theHeader = (jvxProtocolHeader*)(myPriv->work.buf + myPriv->work.offset_buf);

			// Set DSP into initial state
			JVX_PRESET_PACKAGE_SEND(theHeader, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_START);
			myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;

			if (myPriv->init.delayed_send)
			{
				assert(send_ptr);
				assert(send_sz);
				*send_ptr = myPriv->work.buf;
				*send_sz = sz;
			}
			else
			{
				jvxDspBaseErrorType resL = JVX_DSP_ERROR_INVALID_SETTING;
				if (myPriv->init.common.send_callback)
				{
					resL = myPriv->init.common.send_callback(myPriv->init.common.prv_callbacks, myPriv->work.buf, sz);
				}
				if (resL != JVX_DSP_NO_ERROR)
				{
					return JVX_DSP_ERROR_INTERNAL;
				}
			}

			myPriv->state = JVX_AUDIOSTACK_STATE_PROCESSING;
			return JVX_DSP_NO_ERROR;
		}
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType
jvx_audio_stack_trigger_stop(jvxAudioStack* hdl_stack, jvxByte** send_ptr, jvxSize* send_sz)
{
	jvxSize sz;
	if (hdl_stack->prv)
	{
		jvxAudioStack_prv* myPriv = (hdl_stack->prv);

		if (myPriv->init.operation == JVX_AUDIO_STACK_MASTER)
		{
			if (myPriv->state == JVX_AUDIOSTACK_STATE_PROCESSING)
			{
				// =====================================================================
				jvxProtocolHeader* theHeader = (jvxProtocolHeader*)(myPriv->work.buf + myPriv->work.offset_buf);

				// Set DSP into initial state
				JVX_PRESET_PACKAGE_SEND(theHeader, jvxProtocolHeader, JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST, JVX_PROTOCOL_ADVLINK_STOP);
				myPriv->expected_type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;

				if (myPriv->init.delayed_send)
				{
					assert(send_ptr);
					assert(send_sz);
					*send_ptr = myPriv->work.buf;
					*send_sz = sz;
				}
				else
				{
					jvxDspBaseErrorType resL = JVX_DSP_ERROR_INVALID_SETTING;
					if (myPriv->init.common.send_callback)
					{
						resL = myPriv->init.common.send_callback(myPriv->init.common.prv_callbacks, myPriv->work.buf, sz);
					}
					if (resL != JVX_DSP_NO_ERROR)
					{
						return JVX_DSP_ERROR_INTERNAL;
					}
				}

				myPriv->state = JVX_AUDIOSTACK_STATE_AWAIT_START;
				return JVX_DSP_NO_ERROR;
			}
		}
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}
