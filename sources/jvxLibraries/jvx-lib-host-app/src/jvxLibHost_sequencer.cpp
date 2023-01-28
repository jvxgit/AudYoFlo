#include "jvxLibHost.h"

jvxErrorType
jvxLibHost::sequencer_status(jvxSequencerStatus* stat)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (synchronize_thread_enter())
	{
		IjvxSequencer* seqPtr = nullptr;
		if (involvedHost.hHost)
		{
			seqPtr = reqInterface<IjvxSequencer>(involvedHost.hHost);
			if (seqPtr)
			{
				res = seqPtr->status_process(stat, nullptr, nullptr, nullptr, nullptr);
				retInterface<IjvxSequencer>(involvedHost.hHost, seqPtr);
			}
		}
		synchronize_thread_leave();
	}
	return res;
}

jvxErrorType
jvxLibHost::sequencer_start(jvxSize granularity_state_report)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (synchronize_thread_enter())
	{
		IjvxSequencer* seqPtr = nullptr;
		jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
		if (involvedHost.hHost)
		{
			seqPtr = reqInterface<IjvxSequencer>(involvedHost.hHost);
			if (seqPtr)
			{
				res = seqPtr->run_process(static_cast<IjvxSequencer_report*>(this),
					JVX_SEQUENCER_EVENT_REPORT_ALL,
					granularity_state_report);
				JVX_GET_TICKCOUNT_US_GET_CURRENT(&seq_timestap_data);
				retInterface<IjvxSequencer>(involvedHost.hHost, seqPtr);
			}
		}
		synchronize_thread_leave();
	}
	return res;
}

jvxErrorType 
jvxLibHost::sequencer_stop()
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (synchronize_thread_enter())
	{
		IjvxSequencer* seqPtr = nullptr;
		if (involvedHost.hHost)
		{
			seqPtr = reqInterface<IjvxSequencer>(involvedHost.hHost);
			if (seqPtr)
			{
				res = seqPtr->trigger_abort_process();
				retInterface<IjvxSequencer>(involvedHost.hHost, seqPtr);
			}
		}
		synchronize_thread_leave();
	}
	return res;
}

jvxErrorType
jvxLibHost::sequencer_ack_stop()
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (synchronize_thread_enter())
	{
		IjvxSequencer* seqPtr = nullptr;
		if (involvedHost.hHost)
		{
			seqPtr = reqInterface<IjvxSequencer>(involvedHost.hHost);
			if (seqPtr)
			{
				res = seqPtr->acknowledge_completion_process();
				retInterface<IjvxSequencer>(involvedHost.hHost, seqPtr);
			}
		}
		synchronize_thread_leave();
	}
	return res;
}

jvxErrorType
jvxLibHost::sequencer_trigger()
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (synchronize_thread_enter())
	{
		IjvxSequencer* seqPtr = nullptr;
		if (involvedHost.hHost)
		{
			jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
			seqPtr = reqInterface<IjvxSequencer>(involvedHost.hHost);
			if (seqPtr)
			{
				res = JVX_ERROR_WRONG_STATE;
				seqPtr->status_process(&stat, nullptr, nullptr, nullptr, nullptr);
				if (stat != JVX_SEQUENCER_STATUS_NONE)
				{
					jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&seq_timestap_data);
					res = seqPtr->trigger_step_process_extern(tt);
					if ((res == JVX_ERROR_PROCESS_COMPLETE) || (res == JVX_ERROR_ABORT))
					{
						seqPtr->trigger_complete_process_extern(tt);
					}
				}
				retInterface<IjvxSequencer>(involvedHost.hHost, seqPtr);
			}
		}
		synchronize_thread_leave();
	}
	return res;
}
