#include "CjvxNetworkSlaveDevice.h"
#include <iostream>

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

jvxErrorType 
CjvxNetworkSlaveDevice::st_callback_message_queue_started(jvxHandle* privateData, jvxInt64 timestamp_us)
{
	if(privateData)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData);

		return(this_ptr->ic_callback_message_queue_started(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxNetworkSlaveDevice::st_callback_message_queue_message_in_queue_ready(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_queue, jvxHandle* privateData_message, jvxInt64 timestamp_us)
{
	if(privateData_queue)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData_queue);

		return(this_ptr->ic_callback_message_queue_message_in_queue_ready(contextId, fld, szFld, privateData_message,timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
	
jvxErrorType 
CjvxNetworkSlaveDevice::st_callback_message_queue_stopped(jvxHandle* privateData_queue, jvxInt64 timestamp_us)
{
	if(privateData_queue)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData_queue);

		return(this_ptr->ic_callback_message_queue_stopped(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
	
jvxErrorType 
CjvxNetworkSlaveDevice::st_callback_message_queue_message_in_queue_timeout(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_queue, jvxHandle* privateData_message, jvxInt64 timestamp_us)
{
	if(privateData_queue)
	{
		CjvxNetworkSlaveDevice* this_ptr = reinterpret_cast<CjvxNetworkSlaveDevice*>(privateData_queue);

		return(this_ptr->ic_callback_message_queue_message_in_queue_timeout(contextId, fld, szFld, privateData_message, timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
	
// ==========================================================================================================
jvxErrorType 
CjvxNetworkSlaveDevice::ic_callback_message_queue_started(jvxInt64 timestamp_us)
{
	return(JVX_ERROR_UNSUPPORTED);
}
jvxErrorType 
CjvxNetworkSlaveDevice::ic_callback_message_queue_message_in_queue_ready(jvxSize contextId, jvxHandle* fld, jvxSize szFld,  jvxHandle* privateData_message, jvxInt64 timestamp_us)
{
	jvxErrorType res = JVX_NO_ERROR;

	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resL = JVX_TRY_LOCK_MUTEX_SUCCESS;

#ifdef VERBOSE_OUTPUT
	std::cout << "Message in Queue ready" << std::endl;
#endif

	switch(contextId)
	{
	case 0:
		// Outgoing message
		JVX_LOCK_MUTEX(inConnection.channel.safeAccessChannel);
		if(inConnection.channel.inUse)
		{
			jvxProtocolHeader* hdr = (jvxProtocolHeader*) ((jvxByte*)fld + inConnection.fldPrepend);
			inConnection.channel.command_in_expected = (jvxAdvLinkProtocolCommandType) (hdr->purpose & 0x3FFF);
			inConnection.channel.purpose_in_expected = (jvxAdvLinkProtocolMessagePurpose) (hdr->purpose & 0xC000);
			switch(inConnection.channel.purpose_in_expected)
			{
			case JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST:
				inConnection.channel.purpose_in_expected = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
				break;
			case JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_SPECIFY:
				break;
			case JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER:
				break;
			}

			inConnection.channel.inUse = false;
			inConnection.channel.cnt_received_this_message = 0;
			inConnection.channel.expect_further = -1;

#ifdef VERBOSE_OUTPUT
			std::cout << "Send Message" << ((jvxData)JVX_GET_TICKCOUNT_US_GET(tStamp)/1000.0) << std::endl;
#endif
			res = jvx_connect_server_send_st(this->socketServerIf, socketServerChannel, (char*)fld, &szFld, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

#ifdef VERBOSE_OUTPUT
			std::cout << "Send Message Complete" << ((jvxData)JVX_GET_TICKCOUNT_US_GET(tStamp)/1000.0) << std::endl;
#endif
			if(res == JVX_ERROR_WRONG_STATE)
			{
				std::cout << "Failed to send message, possibly socket has already been closed." << std::endl;
			}
			else
			{
				assert(res == JVX_NO_ERROR);
			}
		}
		else
		{
			std::cout << "Currently busy" << std::endl;
			res = JVX_ERROR_COMPONENT_BUSY;
		}
		JVX_UNLOCK_MUTEX(inConnection.channel.safeAccessChannel);
		break;
	default:
		assert(0);
		break;		
	}
	return(res);
}
	
jvxErrorType 
CjvxNetworkSlaveDevice::ic_callback_message_queue_stopped(jvxInt64 timestamp_us)
{
	return(JVX_NO_ERROR);
}
	
jvxErrorType 
CjvxNetworkSlaveDevice::ic_callback_message_queue_message_in_queue_timeout(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_message, jvxInt64 timestamp_us)
{
	assert(0);
	return(JVX_NO_ERROR);
}	

#ifdef JVX_PROJECT_NAMESPACE
}
#endif