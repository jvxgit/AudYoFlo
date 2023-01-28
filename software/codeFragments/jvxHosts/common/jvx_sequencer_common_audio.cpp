#include "jvx.h"

#define JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_AUDIO_RUN 10
static jvxOneSequencerStepDefinition defaultSteps_audio_run[JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_AUDIO_RUN] =
{
	{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE, JVX_COMPONENT_AUDIO_NODE, 0, 0, "Prepare Audio Node", "PrepANode", 0, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Prepare Audio Device", "PrepADevice", 0, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Prepare link connections", "ConnAudio", 0, 1000, "*", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback after all prepared", "AfterAllPrep", 0, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START, JVX_COMPONENT_AUDIO_NODE, 0, 0, "Start Audio Node", "StartANode", 0, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Start Audio Device", "StartADevice", 0, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Start link connections", "StartAudio", 0, 1000, "*", "no label" },
	{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback after all started", "AfterAllStarted", 1, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER, JVX_COMPONENT_UNKNOWN, 0, 0, "Update Viewer", "UpdateViewer", 0, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_BREAK, JVX_COMPONENT_UNKNOWN, 0, 0, "Break sequencer steps", "Break", 0, -1, "no label", "no label"}
};

#define JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_AUDIO_LEAVE 8
static jvxOneSequencerStepDefinition defaultSteps_audio_leave[JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_AUDIO_LEAVE] =
{
	{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback before all stop", "BeforeAllStop", 16, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Stop link connections", "StopAudio", 0, 1000, "*", "no label" },
	{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Stop Audio Device", "StopADevice", 0, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP, JVX_COMPONENT_AUDIO_NODE, 0, 0, "Stop Audio Node", "StopANode", 0, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback before all postprocess", "BeforeAllPost", 17, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Postprocess link connections", "PostProcAudio", 0, 1000, "*", "no label" },
	{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Postprocess Audio Device", "PostADevice", 0, 1000, "no label", "no label"},
	{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS, JVX_COMPONENT_AUDIO_NODE, 0, 0, "Postprocess Audio Node", "PostANode", 0, 1000, "no label", "no label"}
};

extern "C"
{
	jvxErrorType jvx_default_sequence_add(IjvxSequencer* theSeq)
	{
		std::cout << __FUNCTION__ << ": Custom sequencer hook function, located in " << __FILE__ << std::endl;

		jvxErrorType res = JVX_NO_ERROR;
		//	featuresA->useDefaultStepsOnlyIfNoneConfig = false;
		//	featuresA->nm_default_seq = "jvxInspireSequencerSet";

		res = jvx_add_default_sequence_sequencer(theSeq,
			false, 
			defaultSteps_audio_run, JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_AUDIO_RUN,
			defaultSteps_audio_leave, JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_AUDIO_LEAVE,
			"default sequencer spec");
			
		return res;
	};
}
