#ifndef __TJVXEVENTLOOP_H__
#define __TJVXEVENTLOOP_H__

typedef enum
{
	JVX_FRONTEND_SUPPORTS_SHUTDOWN = 0
} jvxFrontendSupportRequestType;

typedef enum
{
	JVX_EVENTLOOP_EVENT_UNKNOWN,
	JVX_EVENTLOOP_EVENT_SELECT,
	JVX_EVENTLOOP_EVENT_ACTIVATE,
	JVX_EVENTLOOP_EVENT_TEXT_INPUT,
	JVX_EVENTLOOP_EVENT_TEXT_SHOW,
	JVX_EVENTLOOP_EVENT_DEACTIVATE,
	JVX_EVENTLOOP_EVENT_UNSELECTED,
	JVX_EVENTLOOP_EVENT_CLEAR_MESSAGE,
	JVX_EVENTLOOP_EVENT_CLEAR_ALL_MESSAGE_FE,
	JVX_EVENTLOOP_EVENT_CLEAR_ALL_MESSAGE_BE,
	JVX_EVENTLOOP_EVENT_FWD_REQUEST_COMMAND,
	JVX_EVENTLOOP_EVENT_SPECIFIC
} jvxOneEventType;

typedef enum
{
	JVX_EVENTLOOP_REQUEST_UNKNOWN,
	JVX_EVENTLOOP_REQUEST_TRIGGER,
	JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE,
	JVX_EVENTLOOP_REQUEST_CALL_BLOCKING,
	JVX_EVENTLOOP_REQUEST_IMMEDIATE
} jvxOneEventClass;
	
typedef enum
{
	JVX_EVENTLOOP_SPECIAL_EVENT_UNKNOWN,
	JVX_EVENTLOOP_SPECIAL_EVENT_SYSTEM_UPDATE,
	JVX_EVENTLOOP_SPECIAL_EVENT_SEQUENCER_UPDATE

} jvxSpecialEventClassEnum;

typedef enum
{
	JVX_EVENTLOOP_DATAFORMAT_NONE, // sends no specific data
	JVX_EVENTLOOP_DATAFORMAT_STDSTRING, // sends an (std::string*)
	JVX_EVENTLOOP_DATAFORMAT_JVXAPISTRING, // sends a jvxApiString
	JVX_EVENTLOOP_DATAFORMAT_CHAR, // sends a single char, convert to jvxHandle* (dirty!)
	JVX_EVENTLOOP_DATAFORMAT_SPECIAL_CHAR, // sends a single special char, convert to jvxHandle* (dirty!)
	JVX_EVENTLOOP_DATAFORMAT_CHAR_LINE_COMPLETE, // Sends the sign that a line is complete. It transfers a pointer to a jvxCBool which allows to return the continue status -> if return value is false, the blocking console frontend will stop. Requires a "blocking" request in the event loop 
	JVX_EVENTLOOP_DATAFORMAT_CHAR_REMOVE_LAST, // This data will cancel the last input character
	JVX_EVENTLOOP_DATAFORMAT_COMMAND_LINE, // Sends a struct that holds argv and argc
	JVX_EVENTLOOP_DATAFORMAT_IJVXHOST_REF, // Sends a pointer to the hostRef interface pointer stored in the backend
	JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS, // Passes a pointer to a json field struct
	JVX_EVENTLOOP_DATAFORMAT_SPECIAL_EVENT, // Passes a pointer to a jvxSpecialEventClass field
	JVX_EVENTLOOP_DATAFORMAT_INTPTR_T, // Passes an int variable in pointer, e.g. o indicate a specific id
	JVX_EVENTLOOP_DATAFORMAT_MEMORY_BUF,
	JVX_EVENTLOOP_DATAFORMAT_REQUEST_COMMAND_REQUEST, // Passes a pointer of class type CjvxReportCommandRequest. Should be auto deleted if rescheduled
	JVX_EVENTLOOP_DATAFORMAT_HANDLE_PTR,
	JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC
} jvxOneEventDatatypeId;

typedef enum
{
	JVX_EVENT_CONTINUE_WANT_SHUTDOWN_SHIFT = 0,
	JVX_EVENT_CONTINUE_POSTPONE_SHUTDOWN_SHIFT = 1,
	JVX_EVENT_CONTINUE_WANT_SHUTDOWN_RESTART_SHIFT = 2
} jvxOneEventContinueType;

typedef enum
{
	JVX_EVENTLOOP_PRIORITY_UNKNOWN,
	JVX_EVENTLOOP_PRIORITY_NORMAL,
	JVX_EVENTLOOP_PRIORITY_HIGH,
	JVX_EVENTLOOP_PRIORITY_TIMER
} jvxOneEventPriority;

typedef struct
{
	jvxSpecialEventClassEnum ev;
	jvxHandle* dat;
} jvxSpecialEventType;

typedef struct
{
	int argc;
	char** argv;
} jvx_argcv_struct;
	
	typedef struct
{
	JVX_NOTIFY_HANDLE notification;
	jvxErrorType result_block;
	jvxHandle* priv;
	jvxSize mId;
} TjvxEventLoopElementSync;

JVX_INTERFACE IjvxEventLoop_frontend;
JVX_INTERFACE IjvxEventLoop_backend;

class TjvxEventLoopElement
{
public:

	jvxSize eventType; //jvxOneEventType
	jvxOneEventClass eventClass;
	jvxOneEventPriority eventPriority;
	IjvxEventLoop_frontend* origin_fe;
	IjvxEventLoop_backend* target_be;
	jvxHandle* priv_fe;
	jvxHandle* priv_be;
	jvxHandle* param;
	jvxSize sz_param;
	jvxSize paramType;
	jvxTick timestamp_run_us;
	jvxSize message_id;
	jvxBool autoDeleteOnProcess;
	JVX_THREAD_ID thread_id;
	TjvxEventLoopElementSync* oneHdlBlock;
	jvxBool rescheduleEvent;
	jvxSize delta_t;

	TjvxEventLoopElement()
	{
		eventType = JVX_EVENTLOOP_EVENT_UNKNOWN;
		eventClass = JVX_EVENTLOOP_REQUEST_UNKNOWN;
		eventPriority = JVX_EVENTLOOP_PRIORITY_UNKNOWN;
		origin_fe = NULL;
		target_be = NULL;
		priv_fe = NULL;
		priv_be = NULL;
		param = NULL;
		sz_param = JVX_SIZE_UNSELECTED;

		paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;
		timestamp_run_us = 0;
		message_id = JVX_SIZE_UNSELECTED;
		autoDeleteOnProcess = false;
		thread_id = JVX_GET_CURRENT_THREAD_ID();
		oneHdlBlock = NULL;
		rescheduleEvent = 0;
		delta_t = 0;
	};

	~TjvxEventLoopElement()
	{
	};
};

#endif
