#include "uMainWindow.h"
#include <QtCore/QTimer>
#include "jvx-qt-helpers.h"

#if 0
jvxErrorType
uMainWindow::report_event(jvxCBitField event_mask, const char* description, jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSequencerElementType stp)
{
	std::string txt;
	JVX_THREAD_ID idT = JVX_GET_CURRENT_THREAD_ID();
	bool report_in_thread = false;

	if(idT == idQtThread)
	{
		report_in_thread = true;
	}
	if(event_mask & JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE)
	{
		txt += "Startup complete, message: ";
		txt += description;

		this->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		if(report_in_thread)
		{
			this->updateStateSequencer_inThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
	}

	if(event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP)
	{
		txt += "Successful completion sequence " + jvx_size2String(sequenceId) + ", step " + jvx_size2String(stepId) + ", ";
		switch(tp)
		{
		case JVX_SEQUENCER_QUEUE_TYPE_RUN:
			txt += "run queue, message: ";
			break;
		case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
			txt += "leave queue, message: ";
			break;
		default:
			txt += "no queue, message: ";
			break;
		}

		txt += description;
		this->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		if(report_in_thread)
		{
			this->updateStateSequencer_inThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}

		if(stp == JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER)
		{
			this->subWidgets.theAudioDialog->refreshPropertyIds_run();
			this->subWidgets.theAudioDialog->updateVariables_whileRunning();
		}
	}
	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR)
	{
		txt += "Incomplete completion sequence " + jvx_size2String(sequenceId) + ", step " + jvx_size2String(stepId) + ", ";
		switch(tp)
		{
		case JVX_SEQUENCER_QUEUE_TYPE_RUN:
			txt += "run queue, due to error, message: ";
			break;
		case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
			txt += "leave queue, due to error, message: ";
			break;
		default:
			txt += "no queue, message: ";
			break;
		}
		txt += description;
		this->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_ERROR);
		if(report_in_thread)
		{
			this->updateStateSequencer_inThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
	}

	if(event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE)
	{
		txt += "Successful completion sequence " + jvx_size2String(sequenceId) + ", message ";
		txt += description;
		report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		if(report_in_thread)
		{
			this->updateStateSequencer_inThread(event_mask,(int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask,(int)sequenceId,(int)stepId, (int)tp);
		}
	}
	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR)
	{
		txt += "Incomplete completion sequence " + jvx_size2String(sequenceId) +" due to error, message: ";
		txt += description;
		report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
		if(report_in_thread)
		{
			this->updateStateSequencer_inThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
	}

	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT)
	{
		txt += "Incomplete completion sequence "  + jvx_size2String(sequenceId) + " due to abort, message: ";
		txt += description;
		report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
		if(report_in_thread)
		{
			this->updateStateSequencer_inThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
	}

	if(event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS)
	{
		txt += "Successful completion process.";
		report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		if(report_in_thread)
		{
			this->updateStateSequencer_inThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
	}
	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR)
	{
		txt += "Incomplete completion process error, message: ";
		txt += description;
		report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_ERROR);
		if(report_in_thread)
		{
			this->updateStateSequencer_inThread(event_mask,(int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask,(int)sequenceId,(int)stepId, (int)tp);
		}
	}
	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT)
	{
		txt += "Process aborted.";
		report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
		if(report_in_thread)
		{
			this->updateStateSequencer_inThread(event_mask,(int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask,(int)sequenceId,(int)stepId, (int)tp);
		}
	}
	if(event_mask & JVX_SEQUENCER_EVENT_PROCESS_TERMINATED)
	{
		txt += "Process terminated.";
		report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);

		if(report_in_thread)
		{
			// o copy and paste failure, we do this once returned from this function since we need to remove the timer
			this->updateStateSequencer_outThread(event_mask,(int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask,(int)sequenceId,(int)stepId, (int)tp);
		}
	}

	if (event_mask & JVX_SEQUENCER_EVENT_INFO_TEXT)
	{
		txt += description;
		report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
	}
	//JVX_SEQUENCER_EVENT_SEQUENCE_LEAVE_STEPS_STARTED = 0x80,
	//JVX_SEQUENCER_EVENT_PROCESS_TERMINATED

	return(JVX_NO_ERROR);
}

/**
 * Map event callback to QT thread.
 *///====================================================================
void
uMainWindow::updateStateSequencer_outThread(jvxUInt64 event_mask, int sequenceId, int stepId, int queuetp)
{
	emit emit_updateStateSequencer(event_mask, sequenceId, stepId, queuetp);
}

/**
 * Event callback within QT thread context.
 *///====================================================================
void
uMainWindow::updateStateSequencer_inThread(jvxUInt64 event_mask, int sequenceId, int stepId, int queuetp)
{
	if(event_mask == JVX_SEQUENCER_EVENT_PROCESS_TERMINATED)
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxSize num = 0;
		this->seq.theSeq->wait_completion_process(JVX_INFINITE_MS);
		// Stop the QTimer thread

		this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(seq.theSeq));
		seq.isRunning = false;
		seq.theSeq = NULL;
	}

	this->subWidgets.theAudioDialog->refreshPropertyIds();
	this->updateWindow((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

jvxErrorType 
uMainWindow::sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId)
{
	if(subWidgets.main.theWidget)
	{
		// Report to main widget
		this->subWidgets.theAudioDialog->updateVariablesSequencerNotification();
		return(this->subWidgets.main.theWidget->inform_sequencer_callback(functionId));
	}
	return(JVX_NO_ERROR);
}
#endif

void 
uMainWindow::report_sequencer_callback(jvxSize fId)
{
	if (subWidgets.main.theWidget)
	{
		// Report to main widget
		this->subWidgets.theAudioDialog->updateVariablesSequencerNotification();
		if (this->subWidgets.main.theWidget)
		{
			this->subWidgets.main.theWidget->inform_sequencer_callback(fId);
		}
	}
}

void 
uMainWindow::report_sequencer_stopped()
{
	// Inform mainCnetral class object
	if (this->subWidgets.main.theWidget)
	{
		this->subWidgets.main.theWidget->inform_sequencer_stopped();
	}

	this->subWidgets.theAudioDialog->stopProcessing();

	this->subWidgets.theAudioDialog->refreshPropertyIds_run();
	this->subWidgets.theAudioDialog->refreshPropertyIds();

	setBackgroundLabelColor(Qt::yellow, label_run0);
	setBackgroundLabelColor(Qt::yellow, label_run1);
	setBackgroundLabelColor(Qt::yellow, label_run2);
	setBackgroundLabelColor(Qt::yellow, label_run3);
	setBackgroundLabelColor(Qt::yellow, label_run4);
	setBackgroundLabelColor(Qt::yellow, label_run5);
	if (systemParams.auto_stop)
	{
		emit emit_autoStop();
	}
}

void 
uMainWindow::report_sequencer_error(const char* str1, const char* str2)
{
	std::string txt = str1;
	txt += str2;
	report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_ERROR);
}

void 
uMainWindow::report_sequencer_update_timeout()
{
	this->updateWindow_periodic();
}


void
uMainWindow::run_autostop()
{
	this->close();
}