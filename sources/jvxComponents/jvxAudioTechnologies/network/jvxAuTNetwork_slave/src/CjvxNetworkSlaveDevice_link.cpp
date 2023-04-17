#include "CjvxNetworkSlaveDevice.h"
#include <iostream>

//#define VERBOSE_OUTPUT

// ===================================================================================
// CALLBACKS CALLBACKS CALLBACKS CALLBACKS CALLBACKS CALLBACKS CALLBACKS CALLBACKS CALLBACKS
// ===================================================================================

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

jvxErrorType 
CjvxNetworkSlaveDevice::st_callback_link_report_startup_complete(jvxHandle* privateData, jvxSize ifidx)
{
	if(privateData)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData);

		return(this_ptr->ic_callback_link_report_startup_complete(ifidx));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxNetworkSlaveDevice::st_callback_link_provide_data_and_length( jvxHandle* privateData, jvxSize channel, char** fld, jvxSize* sz, jvxSize* offset, jvxSize ifidx, jvxHandle* add, jvxConnectionPrivateTypeEnum whatsthis)
{
	if(privateData)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData);

		return(this_ptr->ic_callback_link_provide_data_and_length( channel, fld,  sz, offset, ifidx, add, whatsthis));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);}

jvxErrorType 
CjvxNetworkSlaveDevice::st_callback_link_report_data_and_read( jvxHandle* privateData, jvxSize channel, char* fld, jvxSize sz, jvxSize offset, jvxSize ifidx, jvxHandle* add, jvxConnectionPrivateTypeEnum whatsthis)
{
	if(privateData)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData);

		return(this_ptr->ic_callback_link_report_data_and_read(channel, fld,  sz, offset, ifidx, add, whatsthis));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxNetworkSlaveDevice::st_callback_report_denied_incoming(jvxHandle* privateData, const char* description, jvxSize ifidx)
{
	if(privateData)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData);

		return(this_ptr->ic_callback_report_denied_incoming(description, ifidx));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxNetworkSlaveDevice::st_callback_report_connect_incoming(jvxHandle* privateData, jvxSize channelId, const char* description, jvxSize ifidx)
{
	if(privateData)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData);

		return(this_ptr->ic_callback_report_connect_incoming(channelId, description, ifidx));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
CjvxNetworkSlaveDevice::st_callback_report_disconnect_incoming(jvxHandle* privateData, jvxSize channelId, jvxSize ifidx)
{
	if (privateData)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData);

		return(this_ptr->ic_callback_report_disconnect_incoming(channelId, ifidx));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
CjvxNetworkSlaveDevice::st_callback_link_report_shutdown_complete(jvxHandle* privateData, jvxSize ifidx)
{
	if(privateData)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData);

		return(this_ptr->ic_callback_link_report_shutdown_complete(ifidx));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxNetworkSlaveDevice::st_callback_link_report_error(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx)
{
	if(privateData)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData);

		return(this_ptr->ic_callback_link_report_error(errCode, errDescription, ifidx));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
CjvxNetworkSlaveDevice::st_callback_link_report_specific(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx)
{
	if (privateData)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData);

		return(this_ptr->ic_callback_link_report_specific( idWhat,  fld,  szFld,  ifidx));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxNetworkSlaveDevice::ic_callback_link_report_startup_complete(jvxSize ifidx)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxNetworkSlaveDevice::ic_callback_link_provide_data_and_length(jvxSize chan, char** fldUse, jvxSize* szBestFit, jvxSize* offset, jvxSize ifidx, jvxHandle* add, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;
	*offset = 0;
	if(inConnection.channel.expect_further < 0)
	{
		assert(*szBestFit >=  (inConnection.fldPrepend + sizeof(jvxProtocolHeader)));
		*szBestFit = inConnection.fldPrepend + sizeof(jvxProtocolHeader);
		*fldUse = inConnection.fldCommunicateHandshake_in.ptr;
	}
	else
	{
		jvxProtocolHeader* hdr = (jvxProtocolHeader*)inConnection.fldCommunicateHandshake_in.shcut;
		jvxAdvLinkProtocolCommandType purpose_command = (jvxAdvLinkProtocolCommandType)(hdr->purpose & 0x3FFF);
		jvxAdvLinkProtocolMessagePurpose purpose_type = (jvxAdvLinkProtocolMessagePurpose)(hdr->purpose & 0xC000);			

		if(hdr->fam_hdr.proto_family == JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK)
		{
			if(purpose_command == JVX_PROTOCOL_ADVLINK_SEND_DATA) 
			{
				if(inConnection.oneChannelInOut.readBytes == false)
				{
					// Special rule for this
					*fldUse = (inConnection.fldCommunicateHandshake_in.shcut + sizeof(jvxProtocolHeader));
					*szBestFit = sizeof(jvxAdvProtocolDataChunkHeader);
					inConnection.oneChannelInOut.readBytes = true;
				}
				else
				{
					jvxAdvProtocolDataP* hdr = (jvxAdvProtocolDataP*)inConnection.fldCommunicateHandshake_in.shcut;
					jvxBitField tmp;
					tmp.setValue(hdr->dheader.adv_data_header.channel_mask);
					inConnection.oneChannelInOut.channelMask |= tmp;
					inConnection.oneChannelInOut.seq_id = hdr->dheader.adv_data_header.sequence_id;
					inConnection.oneChannelInOut.idxChannel = jvx_uint642Id(hdr->dheader.adv_data_header.channel_mask);
					
					JVX_LOG_TO_FILE(JVX_PROFILE_RECEIVE_CHANNEL_ENTER, JVX_SIZE_INT32(inConnection.oneChannelInOut.idxChannel), 0, inProcessing.logId, inConnection.oneChannelInOut.seq_id);

					JVX_LOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
					if(_common_set_min.theState == JVX_STATE_PROCESSING)
					{
						// Routing of input cannel
						assert(*szBestFit == jvxDataFormat_size[_common_set_ocon.theData_out.con_params.format] * _common_set_ocon.theData_out.con_params.buffersize);
						*fldUse = (char*)_common_set_ocon.theData_out.con_data.buffers[inProcessing.idSendIn][inConnection.oneChannelInOut.idxChannel];
					}
					else
					{
						JVX_UNLOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
						inConnection.channel.expect_further = -1;
						return(JVX_ERROR_ABORT);
					}
				}
			}
			else
			{
				assert(*szBestFit <=  (inConnection.fldCommunicateHandshake_in.sz - (inConnection.fldPrepend + sizeof(jvxProtocolHeader))));
				*fldUse = inConnection.fldCommunicateHandshake_in.ptr + inConnection.fldPrepend + sizeof(jvxProtocolHeader);
				inConnection.oneChannelInOut.readBytes = false;
			}
		}
		else
		{
			assert(0);
		}
	}
	return(JVX_NO_ERROR);
}

jvxUInt64 deltatmax = 0;

jvxErrorType 
CjvxNetworkSlaveDevice::ic_callback_link_report_data_and_read(jvxSize, char*, jvxSize sz, jvxSize offset,  jvxSize ifidx, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxSize j;
	jvxInt32 bufTimingInfo[JVX_MASTER_NETWORK_NUMBER_ENTRIES_TIMING] = {0};
	jvxInt32 * theBufPtr = bufTimingInfo;

	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isReadyForNext = false;
	//jvxInt32 sendBuffer_id;

	jvxProtocolHeader* header = (jvxProtocolHeader*)inConnection.fldCommunicateHandshake_in.shcut;
	jvxAdvLinkProtocolCommandType purpose_command = (jvxAdvLinkProtocolCommandType)(header->purpose & 0x3FFF);
	jvxAdvLinkProtocolMessagePurpose purpose_type = (jvxAdvLinkProtocolMessagePurpose)(header->purpose & 0xC000);			
	jvxUInt32 szPack = header->paketsize;

	inConnection.numBytesIn = szPack;

	if(inConnection.channel.expect_further < 0)
	{	
		inConnection.channel.expect_further = (szPack - sizeof(jvxProtocolHeader));
	}
	else
	{
		inConnection.channel.expect_further -= sz;
	}
	if(inConnection.channel.expect_further == 0)
	{
		inConnection.numPacketsIn++;

		// Packet is complete
		if(header->fam_hdr.proto_family == JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK)
		{

			if(purpose_command != JVX_PROTOCOL_ADVLINK_SEND_DATA)
			{
#ifdef VERBOSE_OUTPUT
				std::cout << "Received Message" << ((jvxData)JVX_GET_TICKCOUNT_US_GET(tStamp)/1000.0) << std::endl;
#endif
				JVX_LOCK_MUTEX(inConnection.channel.safeAccessChannel);
				
				// Incoming message
				if (!inConnection.channel.inUse)
				{
					inConnection.channel.inUse = true;
					jvxProtocolHeader* hdr = (jvxProtocolHeader*)((jvxByte*)inConnection.fldCommunicateHandshake_in.ptr + inConnection.fldPrepend);
					res = procedeStateMachine(hdr, priv, whatsthis);
				}
				else
				{
					assert(0);
				}
				JVX_UNLOCK_MUTEX(inConnection.channel.safeAccessChannel);
			}
			else
			{
				if(inConnection.oneChannelInOut.readBytes == true)
				{
					// Read in the bytes
					inConnection.oneChannelInOut.readBytes = false;

					if(
						(_common_set_min.theState == JVX_STATE_PROCESSING) && // Critical section from previous call
						(inConnection.localStateMachineConnect == JVX_STATE_PROCESSING))
					{
						JVX_LOG_TO_FILE(JVX_PROFILE_RECEIVE_CHANNEL_LEAVE, 0, 0, inProcessing.logId, 0);

						if(inConnection.oneChannelInOut.channelMask == inProcessing.channelMaskTarget)
						{
							// Evaluate frame sequence id
							if(inProcessing.idSequenceExpected < 0)
							{
								inProcessing.idSequenceExpected = inConnection.oneChannelInOut.seq_id;
							}
							else
							{
								jvxInt32 delta = (jvxInt32)(inConnection.oneChannelInOut.seq_id - inProcessing.idSequenceExpected);

								genNetworkSlave_device::integrateiplink.properties_running.numberLostFramesRemote.value += delta;
								if(delta != 0)
								{
									JVX_LOG_TO_FILE(JVX_PROFILE_EVENT_MISSED_FRAME, 0, delta, inProcessing.logId, 0);
								}
							}
							inProcessing.idSequenceExpected = inConnection.oneChannelInOut.seq_id + 1;

							// Try to send away the data suitable for output
							JVX_TRY_LOCK_MUTEX_RESULT_TYPE resM = JVX_TRY_LOCK_MUTEX_SUCCESS;
							JVX_TRY_LOCK_MUTEX(resM, inConnection.channel.safeAccessChannel);
							if (resM == JVX_TRY_LOCK_MUTEX_SUCCESS)
							{
								inConnection.channel.inUse = true;
								for (j = 0; j < _common_set_icon.theData_in->con_params.number_channels; j++)
								{
									jvxAdvProtocolDataP* hdr = (jvxAdvProtocolDataP*)(inProcessing.tx_fields[inProcessing.idSendOut][j] + inConnection.fldPrepend);
									/*
									jvxInt16* ptrDat = (jvxInt16*)inProcessing.theData.con_compat.from_receiver_buffer_allocated_by_sender[sendBuffer_id][j];
									for(i = 0; i < inProcessing.theData.con_compat.buffersize; i++)
									{
									ptrDat[i] = sendBuffer_id * 0x40 + j + i;
									}
									*/
									hdr->dheader.adv_data_header.sequence_id = inConnection.oneChannelInOut.seq_id;
									jvxSize szFld = inProcessing.tx_size;
#if defined JVX_PCAP_DIRECT_SEND || !defined JVX_WITH_PCAP
									jvxTick tim1 = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

									JVX_LOG_TO_FILE(JVX_PROFILE_SEND_CHANNEL_START, 0, 0, inProcessing.logId, 0);

									res = jvx_connect_server_send_st(this->socketServerIf, socketServerChannel,
													 inProcessing.tx_fields[inProcessing.idSendOut][j], &szFld,
													 NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
									assert(res == JVX_NO_ERROR);

									JVX_LOG_TO_FILE(JVX_PROFILE_SEND_CHANNEL_STOP, 0, 0, inProcessing.logId, 0);

									jvxTick tim2 = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
									tim2 -= tim1;
									/*
									  if (tim2 > deltatmax)
									  {
									  deltatmax = tim2;
									  std::cout << "Max " << deltatmax << std::endl;
									  }
									*/
#else
									res = jvx_connect_server_collect_st(this->socketServerIf, socketServerChannel, 
										inProcessing.tx_fields[inProcessing.idSendOut][j], &szFld, NULL, 
										JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_PCAP_TARGET);
									assert(res == JVX_NO_ERROR);
#endif

								}

#ifndef JVX_PCAP_DIRECT_SEND
								jvxTick tim1 = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

								JVX_LOG_TO_FILE(JVX_PROFILE_SEND_CHANNEL_START, 0, 0, inProcessing.logId, 0);

								res = jvx_connect_server_transmit_st(this->socketServerIf, socketServerChannel);
								assert(res == JVX_NO_ERROR);

								JVX_LOG_TO_FILE(JVX_PROFILE_SEND_CHANNEL_STOP, 0, 0, inProcessing.logId, 0);

								jvxTick tim2 = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
								tim2 -= tim1;
								/*
								if (tim2 > deltatmax)
								{
									deltatmax = tim2;
									std::cout << "Max " << deltatmax << std::endl;
								}
								*/

#endif
								inConnection.channel.inUse = false;
								JVX_UNLOCK_MUTEX(inConnection.channel.safeAccessChannel);
								//jvx_message_queue_trigger_queue(inConnection.mQueue_out); <- No, better handle this with a 1 sec timeout and a state machine!!!
							}
							else
							{
								JVX_LOG_TO_FILE(JVX_PROFILE_EVENT_UNSENT_FRAME, 0, 0, inProcessing.logId, 0);

								genNetworkSlave_device::integrateiplink.properties_running.numberUnsentFrames.value++;
							}

							
							if(inProcessing.fillHeight < inProcessing.numberBuffersMax -1)
							{
								_common_set_icon.theData_in->con_pipeline.do_lock(_common_set_icon.theData_in->con_pipeline.lock_hdl);
								inProcessing.fillHeight++;
								inProcessing.idSendIn = (inProcessing.idSendIn + 1) % _common_set_ocon.theData_out.con_data.number_buffers;
								inProcessing.idSendOut = (inProcessing.idSendOut + 1) % _common_set_icon.theData_in->con_data.number_buffers;
								_common_set_icon.theData_in->con_pipeline.do_unlock(_common_set_icon.theData_in->con_pipeline.lock_hdl);
								jvx_thread_wakeup(theThread.theThreadHandle);
							}
							else
							{
								// There is no space in inpout buffer, hence the latest buffer will be used for next buffer!!
								JVX_LOG_TO_FILE(JVX_PROFILE_EVENT_NOT_READY_PROCESSING, 0, 0, inProcessing.logId, 0);

								genNetworkSlave_device::integrateiplink.properties_running.numberLostFrames.value++;
							}


							// Reset channel collector
							inConnection.oneChannelInOut.channelMask = 0;
						}						
					}
					else
					{
						// No longer accepting incoming buffers
						std::cout << "Skip incoming packet" << std::endl;
					}
					JVX_UNLOCK_MUTEX(_common_set._safeAccessStateBound.the_lock);
				}
			}
		}
		else
		{
			assert(0);
		}
		inConnection.channel.expect_further = -1;

	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxNetworkSlaveDevice::ic_callback_report_denied_incoming(const char* description, jvxSize ifidx)
{
	return(JVX_ERROR_UNSUPPORTED);
}

jvxErrorType 
CjvxNetworkSlaveDevice::ic_callback_report_connect_incoming(jvxSize channelId, const char* description, jvxSize ifidx)
{
	jvxProtocolHeader* theHeader;
	jvxSize sz = 0;

	JVX_GET_TICKCOUNT_US_SETREF(&tStamp);

#if 0
	// =====================================================================
	theHeader = (jvxProtocolHeader*)inConnection.fldCommunicateHandshake_out.shcut;

	// Set DSP into initial state
	theHeader->paketsize = JVX_SIZE_INT32(inConnection.fldCommunicateHandshake_out.sz);
	theHeader->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_ERROR_RESET);
	theHeader->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
	sz = theHeader->paketsize;

	jvx_message_queue_add_message_to_queue(inConnection.mQueue_out, 0, inConnection.fldCommunicateHandshake_out.ptr,
		inConnection.fldCommunicateHandshake_out.sz, NULL);

	genNetworkSlave_device::integrateiplink.properties_running.connectionStatus.value.selection = 0x8;
	_report_property_has_changed(genNetworkSlave_device::integrateiplink.properties_running.connectionStatus.category,
		genNetworkSlave_device::integrateiplink.properties_running.connectionStatus.id,
		true, JVX_COMPONENT_UNKNOWN);

	jvxCBitField reportflags;
	jvx_bitFClear(reportflags);
	jvx_bitSet(reportflags, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	_report_command_request(reportflags);

#endif
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxNetworkSlaveDevice::ic_callback_report_disconnect_incoming(jvxSize channelId, jvxSize ifidx)
{
	// If we were connected before, we are no longer
	_lock_properties_local();
	CjvxProperties::_undo_update_property_access_type(genNetworkSlave_device::integrateiplink.properties_running.selectionHardwareModeActive);
	_unlock_properties_local();

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxNetworkSlaveDevice::ic_callback_link_report_shutdown_complete(jvxSize ifidx)
{
	jvxErrorType resL;

	inConnection.localStateMachineConnect = JVX_STATE_NONE;
	
	// If we are here, create channels, samplerates etc.
	if (theEventLoop)
	{
		TjvxEventLoopElement tElm;
		tElm.target_be = static_cast<IjvxEventLoop_backend*>(this);
		tElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
		tElm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		tElm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC+1;
		/*
		tElm.autoDeleteOnProcess = false;
		tElm.delta_t = JVX_SIZE_UNSELECTED;
		tElm.message_id = JVX_SIZE_UNSELECTED;
		tElm.oneHdlBlock = NULL;
		tElm.origin_fe = NULL;
		tElm.param = NULL;
		tElm.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;
		tElm.priv_be = NULL;
		tElm.priv_fe = NULL;
		tElm.rescheduleEvent = false;
		tElm.thread_id = 0;
		tElm.timestamp_run_us = 0;
		*/
		if (!forceDown)
		{
			theEventLoop->event_schedule(&tElm, &resL, NULL);
		}
	}
	else
	{
		std::cout << "Warning, this implementation is not thread safe!" << std::endl;
		update_parameters(false);
	}

	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxNetworkSlaveDevice::ic_callback_link_report_error(jvxInt32 errCode, const char* errDescription, jvxSize ifidx)
{
	jvx_connect_server_terminate_st(this->socketServerIf);
	this->socketServerIf = NULL;

	genNetworkSlave_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x1;
	_report_text_message(errDescription, JVX_REPORT_PRIORITY_ERROR);
	_report_property_has_changed(genNetworkSlave_device::integrateiplink.properties_running.connectionStatus.category,
		genNetworkSlave_device::integrateiplink.properties_running.connectionStatus.globalIdx,
		true, 0, 1, JVX_COMPONENT_UNKNOWN);
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxNetworkSlaveDevice::ic_callback_link_report_specific(jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx)
{
	return(JVX_NO_ERROR);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
