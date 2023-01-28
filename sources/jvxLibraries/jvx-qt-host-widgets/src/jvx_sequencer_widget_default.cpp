#include "jvx_sequencer_widget.h"
#include "allHostsStatic_common.h"

void
jvx_sequencer_widget::add_default_sequence(bool	onlyIfNoSequence, jvxOneSequencerStepDefinition* defaultSteps_run, 
	jvxSize numSteps_run, jvxOneSequencerStepDefinition* defaultSteps_leave, jvxSize numSteps_leave, const std::string& nm_default)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;

	if (theSequencer)
	{
		if ((!
			(numSteps_leave == 0) && (numSteps_run == 0)))
		{
			res = jvx_add_default_sequence_sequencer(theSequencer,
				onlyIfNoSequence, defaultSteps_run, numSteps_run,
				defaultSteps_leave, numSteps_leave, nm_default);
		}
	}

	if (res == JVX_NO_ERROR)
	{
		this->updateWindow_rebuild();
	}
}

void
jvx_sequencer_widget::remove_all_sequence()
{
	// First check if such a sequence is already there..
	theSequencer->reset_sequences();
}