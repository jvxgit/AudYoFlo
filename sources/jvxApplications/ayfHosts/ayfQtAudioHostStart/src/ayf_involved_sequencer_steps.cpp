#include "jvx.h"

#define JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_AUDIO_RUN 6
static jvxOneSequencerStepDefinition defaultSteps_audio_run[JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_AUDIO_RUN] =
{
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE, JVX_COMPONENT_UNKNOWN, JVX_SIZE_DONTCARE, JVX_SIZE_DONTCARE, "Prepare all connections", "PrepConnections", 0, 1000, "*", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback after all prepared", "AfterAllPrep", 0, 1000, "no label", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START, JVX_COMPONENT_UNKNOWN, JVX_SIZE_DONTCARE, JVX_SIZE_DONTCARE, "Start all connections", "StartConnections", 0, 1000, "*", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback after all started", "AfterAllStarted", 1, 1000, "no label", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER, JVX_COMPONENT_UNKNOWN, 0, 0, "Update Viewer", "UpdateViewer", 0, 1000, "no label", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_BREAK, JVX_COMPONENT_UNKNOWN, 0, 0, "Break sequencer steps", "Break", 0, -1, "no label", "no label" }
};

#define JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_AUDIO_LEAVE 4
static jvxOneSequencerStepDefinition defaultSteps_audio_leave[JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_AUDIO_LEAVE] =
{
	{ JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback before all stop", "BeforeAllStop", 16, 1000, "no label", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP, JVX_COMPONENT_UNKNOWN, JVX_SIZE_DONTCARE, JVX_SIZE_DONTCARE, "Stop all connections", "StopConnections", 0, 1000, "*", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback before all postprocess", "BeforeAllPost", 17, 1000, "no label", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS, JVX_COMPONENT_UNKNOWN, JVX_SIZE_DONTCARE, JVX_SIZE_DONTCARE, "Postprocess all connections", "PostProcessConnections", 0, 1000, "*", "no label" }
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
			"default sequencer spec audio");
			
		return res;
	};
}
