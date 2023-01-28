#include "jvxLibHost.h"

// ========================================================================
jvxErrorType
jvxLibHost::report_event(jvxCBitField event_mask, const char* description, jvxSize sequenceId, jvxSize stepId,
	jvxSequencerQueueType tp, jvxSequencerElementType stp, jvxSize fId, jvxSize current_state, jvxBool indicateFirstError)
{
	// Map this callback to the new generic API
	jvxErrorType res = JVX_NO_ERROR;
	TjvxSequencerEvent theEv;
	theEv.event_mask = event_mask;
	theEv.description.assign(description);
	theEv.sequenceId = sequenceId;
	theEv.stepId = stepId;
	theEv.tp = tp;
	theEv.stp = stp;
	theEv.fId = fId;
	theEv.current_state = current_state;
	theEv.indicateFirstError = indicateFirstError;
	CjvxReportCommandRequest_seq seqEvent(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_EVENT,
		JVX_COMPONENT_UNKNOWN, &theEv);
	res = this->request_command(seqEvent);

	return res;
}

// ========================================================================
jvxErrorType
jvxLibHost::sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId)
{
	// Map this callback to the new generic API
	jvxErrorType res = JVX_NO_ERROR;
	TjvxSequencerEvent theEv;
	
	theEv.sequenceId = sequenceId;
	theEv.stepId = stepId;
	theEv.tp = tp;
	theEv.fId = functionId;

	CjvxReportCommandRequest_seq seqEvent(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_CALLBACK,
		JVX_COMPONENT_UNKNOWN, &theEv);
	res = this->request_command(seqEvent);
	return res;
}
