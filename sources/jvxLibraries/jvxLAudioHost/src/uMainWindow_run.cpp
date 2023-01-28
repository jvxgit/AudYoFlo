#include "uMainWindow.h"
#include "jvxQtAudioHost_common.h"
#include "jvxAudioHost_common.h"

void
uMainWindow::buttonPushed_startstop()
{
	on_request_startstop(JVX_START_STOP_INTENDED_TOGGLE);
}

void
uMainWindow::on_request_startstop(jvxStartStopIntendedOperation intendedOperation)
{
	jvxErrorType res;
	jvxBool doStart = false;
	jvxBool doStop = false;
	jvxSequencerStatus  seqStat = JVX_SEQUENCER_STATUS_NONE;

	subWidgets.theSequencerWidget->status_process(&seqStat, NULL);

	switch (intendedOperation)
	{
	case JVX_START_STOP_INTENDED_START:
		if (seqStat == JVX_SEQUENCER_STATUS_NONE)
		{
			doStart = true;
		}
		break;
	case JVX_START_STOP_INTENDED_STOP:
		if (seqStat != JVX_SEQUENCER_STATUS_NONE)
		{
			doStop = true;
		}
		break;
	case JVX_START_STOP_INTENDED_TOGGLE:
		if (seqStat == JVX_SEQUENCER_STATUS_NONE)
		{
			doStart = true;
		}
		else
		{
			doStop = true;
		}
		break;
	}

	if (doStart)
	{
		progress = 0;
		JVX_GET_TICKCOUNT_US_SETREF(&tstamp);

		// Inform mainCentral class object
		if (this->subWidgets.main.theWidget)
		{
			this->subWidgets.main.theWidget->inform_sequencer_about_to_start();
		}

		res = subWidgets.theSequencerWidget->control_start_process(theHostFeatures.timeout_period_seq_ms, JVX_SIZE_UNSELECTED);
		if (res == JVX_NO_ERROR)
		{
			this->subWidgets.theAudioDialog->startProcessing(theHostFeatures.timeout_viewer_maincentral_ms);
			if (this->subWidgets.main.theWidget)
			{
				this->subWidgets.main.theWidget->inform_sequencer_started();
			}
		}
		else
		{
			if (this->subWidgets.main.theWidget)
			{
				this->subWidgets.main.theWidget->inform_sequencer_stopped();
			}
		}
	}
	else if(doStop)
	{ 
		// We may arrive here if the component asks to stop and I push the button at the same time
		if (seqStat == JVX_SEQUENCER_STATUS_IN_OPERATION)
		{
			std::cout << "Sequencer in operation, stopping." << std::endl;
			if (this->subWidgets.main.theWidget)
			{
				this->subWidgets.main.theWidget->inform_sequencer_about_to_stop();
			}
			// this->subWidgets.theAudioDialog->stopProcessing(); <- I have moved this call into the sequencer end of process callback. Otherwise the timer in the audio dialog keeps on running in case of an error.
			res = subWidgets.theSequencerWidget->control_stop_process();
			assert(res == JVX_NO_ERROR);
		}
		else
		{
			std::cout << "Sequencer not in operation, ignoring stop." << std::endl;
		}
		// The rest will be done in the sequencer callbacks..
	}

	/*
	if(seq.isRunning == false)
	{
		if (
			(intendedOperation == JVX_START_STOP_INTENDED_START)||
			(intendedOperation == JVX_START_STOP_INTENDED_TOGGLE))
		{
			seq.theSeq = NULL;
			res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&seq.theSeq));
			if (res == JVX_NO_ERROR)
			{
				assert(res == JVX_NO_ERROR);
				assert(seq.theSeq);

				// Inform mainCnetral class object
				this->subWidgets.main.theWidget->inform_sequencer_about_to_start();

				res = seq.theSeq->run_process(200, this, -1, true); // Timeout etc. not used in operation mode
				if (res != JVX_NO_ERROR)
				{
					res = this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(seq.theSeq));
					return;
				}

				seq.isRunning = true;
				seq.progress = 0;
				JVX_GET_TICKCOUNT_US_SETREF(&seq.tsStart);

				res = seq.theSeq->trigger_step_process_extern(0);
				if (res == JVX_NO_ERROR)
				{
					this->subWidgets.theAudioDialog->startProcessing(theHostFeatures.timeout_viewer_ms);
				}
				else
				{
					this->report_simple_text_message("Failed to start sequencer.", JVX_REPORT_PRIORITY_ERROR);

					res = this->seq.theSeq->wait_completion_process(JVX_INFINITE_MS);
					assert(res == JVX_NO_ERROR);
					seq.isRunning = false;
					// Inform mainCnetral class object
					this->subWidgets.main.theWidget->inform_sequencer_stopped();

					this->subWidgets.theAudioDialog->refreshPropertyIds_run();
					this->subWidgets.theAudioDialog->refreshPropertyIds();
				}
			}
		}
	}
	else
	{
		if (
			(intendedOperation == JVX_START_STOP_INTENDED_STOP) ||
			(intendedOperation == JVX_START_STOP_INTENDED_TOGGLE))
		{
			this->subWidgets.main.theWidget->inform_sequencer_about_to_stop();
			this->subWidgets.theAudioDialog->stopProcessing();

			assert(seq.theSeq);
			res = seq.theSeq->trigger_abort_process();
			assert(res == JVX_NO_ERROR);
			while (res == JVX_NO_ERROR)
			{
				res = seq.theSeq->trigger_step_process_extern(0);
			}
			res = this->seq.theSeq->wait_completion_process(JVX_INFINITE_MS);
			assert(res == JVX_NO_ERROR);
			seq.isRunning = false;

			// Inform mainCnetral class object
			this->subWidgets.main.theWidget->inform_sequencer_stopped();

			this->subWidgets.theAudioDialog->refreshPropertyIds_run();
			this->subWidgets.theAudioDialog->refreshPropertyIds();
		}
	}
	*/
	this->updateWindow((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void
uMainWindow::updateWindow_periodic()
{
	QColor col = Qt::gray;
	jvxTick ts = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tstamp);

	int lprogress = JVX_DATA2INT32(ts * 0.000001) % 6;
	if (lprogress != progress)
	{
		setBackgroundLabelColor(Qt::gray, label_run0);
		setBackgroundLabelColor(Qt::gray, label_run1);
		setBackgroundLabelColor(Qt::gray, label_run2);
		setBackgroundLabelColor(Qt::gray, label_run3);
		setBackgroundLabelColor(Qt::gray, label_run4);
		setBackgroundLabelColor(Qt::gray, label_run5);
		switch (progress)
		{
		case 0:
			setBackgroundLabelColor(Qt::yellow, label_run0);
			break;
		case 1:
			setBackgroundLabelColor(Qt::yellow, label_run1);
			break;
		case 2:
			setBackgroundLabelColor(Qt::yellow, label_run2);
			break;
		case 3:
			setBackgroundLabelColor(Qt::yellow, label_run3);
			break;
		case 4:
			setBackgroundLabelColor(Qt::yellow, label_run4);
			break;
		case 5:
			setBackgroundLabelColor(Qt::yellow, label_run5);
			break;
		}
		progress = lprogress;
	}

	this->subWidgets.main.theWidget->inform_update_window_periodic();
	
	theControl.update_window_periodic();
}
