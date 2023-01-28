#ifndef __CJVXREPORTMESSAGEQUEUE_H__
#define __CJVXREPORTMESSAGEQUEUE_H__

#include "jvx.h"
#include <list>

typedef jvxErrorType (*CjvxMessageQueue_process_object)(jvxHandle* theField, jvxSize theFieldSz, jvxCBitField elementType, jvxHandle* theContext);

typedef jvxErrorType (*CjvxMessageQueue_remove_object)(jvxHandle* theField, jvxSize theFieldSz, jvxCBitField elementType, jvxHandle* theContext);

class CjvxReportMessageQueue
{
private:

	struct oneMessageInQueue
	{
		jvxCBitField elementType;
		jvxHandle* mess_content_remove_me_after_post;
		jvxSize sz_mess_content;
		CjvxMessageQueue_remove_object remove_cb;
		jvxHandle* remove_ctxt;
		
	};

	struct
	{
		jvxState state;
		std::list<oneMessageInQueue> theQueue;
		JVX_MUTEX_HANDLE safeAccessQueue;

		CjvxMessageQueue_process_object cb_process;
		jvxHandle* pv_process;
	} runtime;

public:

	CjvxReportMessageQueue();

	~CjvxReportMessageQueue();

	jvxErrorType initialize_queue(CjvxMessageQueue_process_object cb_process, jvxHandle* pv_process);

	jvxErrorType start_queue();

	jvxErrorType stop_queue();

	jvxErrorType terminate_queue();

	jvxErrorType trigger_process_element_remove_queue();

	jvxErrorType add_element_queue(jvxCBitField elementType, jvxHandle* mess_content_remove_me_after_post, jvxSize sz_mess, CjvxMessageQueue_remove_object remove_cb, jvxHandle* ctxt);
};

#endif
