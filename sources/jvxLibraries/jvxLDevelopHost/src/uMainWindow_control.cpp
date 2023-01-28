#include "uMainWindow.h"

jvxErrorType
uMainWindow::control_startSequencer()
{
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
	this->subWidgets.sequencer.theWidget->status_process(&stat, NULL);
	if (stat == JVX_SEQUENCER_STATUS_NONE)
	{
		if (subWidgets.main.theWidget)
		{
			subWidgets.main.theWidget->inform_sequencer_about_to_start();
		}

		jvxErrorType res = this->subWidgets.sequencer.theWidget->control_start_process(theHostFeatures.timeout_period_seq_ms, theHostFeatures.granularity_state_report);
		if (res == JVX_NO_ERROR)
		{
			jvxSize tout_viewer = theHostFeatures.timeout_viewer_maincentral_ms;
			if (subWidgets.main.theWidget)
			{
				subWidgets.main.theWidget->inform_viewer_about_to_start(&tout_viewer);
			}
			assert(subWidgets.main.timer == NULL);
			subWidgets.main.timer = new QTimer(this);
			connect(subWidgets.main.timer, SIGNAL(timeout()), this, SLOT(updateWindowPeriodic()));
			subWidgets.main.timer->start(tout_viewer);

			if (subWidgets.main.theWidget)
			{
				subWidgets.main.theWidget->inform_viewer_started();
			}
		}
		if (subWidgets.main.theWidget)
		{
			subWidgets.main.theWidget->inform_sequencer_started();
		}
		return(res);
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
uMainWindow::control_stopSequencer()
{
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
	this->subWidgets.sequencer.theWidget->status_process(&stat, NULL);
	if (stat != JVX_SEQUENCER_STATUS_NONE)
	{
		if (subWidgets.main.theWidget)
		{
			subWidgets.main.theWidget->inform_sequencer_about_to_stop();
		}
		jvxErrorType res = this->subWidgets.sequencer.theWidget->control_stop_process();
		assert(res == JVX_NO_ERROR);

		// Note: the confirmation that processing is over will come in via callback if all sequencer steps have been completed
		
		return(res);
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
uMainWindow::control_contSequencer()
{
	jvxSize stackDepth = 0;
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
	this->subWidgets.sequencer.theWidget->status_process(&stat, &stackDepth);
	if (stat != JVX_SEQUENCER_STATUS_NONE)
	{
		if (stackDepth == 1)
		{
			if (subWidgets.main.theWidget)
			{
				subWidgets.main.theWidget->inform_sequencer_about_to_stop();
			}
		}
		jvxErrorType res = this->subWidgets.sequencer.theWidget->control_continue_process();
		assert(res == JVX_NO_ERROR);

		// Note: the confirmation that processing is over will come in via callback if all sequencer steps have been completed

		return(res);
	}
	return JVX_ERROR_WRONG_STATE;
}
void
uMainWindow::sequencer_stopped()
{
	if (subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->inform_viewer_about_to_stop();
	}

	assert(subWidgets.main.timer);
	subWidgets.main.timer->stop();
	disconnect(subWidgets.main.timer, SIGNAL(timeout()), this, SLOT(updateWindowPeriodic()));
	delete(subWidgets.main.timer);
	subWidgets.main.timer = NULL;	

	if (subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->inform_viewer_stopped();
	}

	if(subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->inform_sequencer_stopped();
	}
} 

void 
uMainWindow::reportSequencerError_inThread(QString err, QString ferr)
{
	if (subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->inform_sequencer_error(err.toLatin1().data(), ferr.toLatin1().data());
	}
}
void 
uMainWindow::reportSequencerError_outThread(std::string err, std::string firstErr)
{
	emit emit_reportSequencerError(err.c_str(), firstErr.c_str());
}
