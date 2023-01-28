#ifndef __JVX_MESSAGE_QUEUE_H__
#define __JVX_MESSAGE_QUEUE_H__

#include "jvx.h"

typedef enum
{
	JVX_MESSAGE_QUEUE_STATIC_OBJECTS = 0,
	JVX_MESSAGE_QUEUE_DYNAMIC_OBJECTS = 1
} jvxMessageQueueObjectStrategyType;

typedef struct
{
	jvxErrorType (*callback_message_queue_started)(jvxHandle* privateData_queue, jvxInt64 timestamp_us);
	jvxErrorType (*callback_message_queue_message_in_queue_ready)(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_queue, jvxHandle* privateData_message, jvxInt64 timestamp_us);
	jvxErrorType (*callback_message_queue_stopped)(jvxHandle* privateData_queue, jvxInt64 timestamp_us);
	jvxErrorType (*callback_message_queue_message_in_queue_timeout)(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_queue, jvxHandle* privateData_message, jvxInt64 timestamp_us);
} messageQueueHandler;

jvxErrorType jvx_message_queue_initialize(jvxHandle** hdlOnReturn, jvxMessageQueueObjectStrategyType type, messageQueueHandler* report_callback, jvxHandle* privData, 
		jvxSize numMessagesInQueueMax, jvxSize timeoutMax_ms, jvxSize maxSizeContainerMessage);

jvxErrorType jvx_message_queue_terminate(jvxHandle* hdl);

jvxErrorType jvx_message_queue_start(jvxHandle* hdl);

jvxErrorType jvx_message_queue_stop(jvxHandle* hdl);

jvxErrorType jvx_message_queue_trigger_stop(jvxHandle* hdl);

jvxErrorType jvx_message_queue_wait_for_stop(jvxHandle* hdl);

jvxErrorType jvx_message_queue_add_message_to_queue(jvxHandle* hdl, jvxSize contextId, jvxHandle* data, jvxSize szFld, jvxHandle* privateData_message);

jvxErrorType jvx_message_queue_trigger_queue(jvxHandle* hdl);

jvxErrorType jvx_message_queue_trigger_clear_messages_queue(jvxHandle* hdl);

jvxErrorType jvx_message_queue_num_message_in_queue(jvxHandle* hdl, jvxSize* numMessages);

jvxErrorType jvx_message_queue_get_timestamp_us(jvxHandle* hdl, jvxInt64* timestamp_us);

#endif