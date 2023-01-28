#include "uMainWindow.h"

void 
uMainWindow::start_process()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxSize num = 0;
	res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if((res == JVX_NO_ERROR) && sequencer)
	{
		if(sequencerStruct.extTrigger)
		{
			JVX_GET_TICKCOUNT_US_SETREF(sequencerStruct.tStamp);
			res = sequencer->run_process(200, static_cast<IjvxSequencer_report*>(this), -1, true);
			sequencerStruct.runtime.theTimer = new QTimer(this);
			connect(sequencerStruct.runtime.theTimer, SIGNAL(timeout()), this, SLOT(timerExpired()));
			sequencerStruct.runtime.theTimer->start(200);
			sequencerStruct.runtime.externalTrigger = true;
		}
		else
		{
			res = sequencer->run_process(200, static_cast<IjvxSequencer_report*>(this), -1, false);
			sequencerStruct.runtime.externalTrigger = false;
		}
		if(res != JVX_NO_ERROR)
		{
			this->postMessage_inThread(((std::string)"Starting process sequence failed, error: " + jvxErrorType_descr(res)).c_str(), JVX_QT_COLOR_ERROR);
		}
		this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	}

}

/**
 * User has pushed button to stop process
 *///====================================================================
void 
uMainWindow::stop_process()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxSize num = 0;
	res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if((res == JVX_NO_ERROR) && sequencer)
	{
		res = sequencer->trigger_abort_process();
		if(res != JVX_NO_ERROR)
		{
			this->postMessage_inThread(((std::string)"Stopping process sequence failed, error: " + jvxErrorType_descr(res)).c_str(), JVX_QT_COLOR_ERROR);
		}
		this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	}
}

jvxErrorType 
uMainWindow::wait_for_process_stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxSize num = 0;
	res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if((res == JVX_NO_ERROR) && sequencer)
	{
		res = sequencer->wait_completion_process(5000);
		if((res != JVX_NO_ERROR) && (res != JVX_ERROR_NOT_READY))
		{
			this->postMessage_inThread(((std::string)"Waiting for process to stop failed, error: " + jvxErrorType_descr(res)).c_str(), JVX_QT_COLOR_ERROR);
		}
		this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));

	}
	return(res);
}

jvxErrorType
uMainWindow::report_event(jvxUInt64 event_mask, const char* description, jvxInt16 sequenceId, jvxInt16 stepId, jvxSequencerQueueType tp, jvxSequencerElementType stp)
{
	std::string txt;
	JVX_THREAD_ID idT = JVX_GET_CURRENT_THREAD_ID();
	bool report_in_thread = false;
	/*
	if(idT == others.idQtThread)
	{
		report_in_thread = true;
	}*/
	if(event_mask & JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE)
	{
		txt += "Startup complete, message: ";
		txt += description;

		postMessage_outThread(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
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
		txt += "Successful completion sequence " + jvx_int2String(sequenceId) + ", step " + jvx_int2String(stepId) + ", ";
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
		postMessage_outThread(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		if(report_in_thread)
		{
			this->updateStateSequencer_inThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask, (int)sequenceId,(int)stepId, (int)tp);
		}
	}
	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR)
	{
		txt += "Incomplete completion sequence " + jvx_int2String(sequenceId) + ", step " + jvx_int2String(stepId) + ", ";
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
		postMessage_outThread(txt.c_str(), JVX_REPORT_PRIORITY_ERROR);
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
		txt += "Successful completion sequence " + jvx_int2String(sequenceId) + ", message ";
		txt += description;
		postMessage_outThread(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
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
		txt += "Incomplete completion sequence " + jvx_int2String(sequenceId) +" due to error, message: ";
		txt += description;
		postMessage_outThread(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
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
		txt += "Incomplete completion sequence "  + jvx_int2String(sequenceId) + " due to abort, message: ";
		txt += description;
		postMessage_outThread(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
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
		postMessage_outThread(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
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
		postMessage_outThread(txt.c_str(), JVX_REPORT_PRIORITY_ERROR);
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
		postMessage_outThread(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
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
		postMessage_outThread(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);

		if(report_in_thread)
		{
			// o copy and paste failure, we do this once returned from this function since we need to remove the timer
			this->updateStateSequencer_inThread(event_mask,(int)sequenceId,(int)stepId, (int)tp);
		}
		else
		{
			this->updateStateSequencer_outThread(event_mask,(int)sequenceId,(int)stepId, (int)tp);
		}
	}
	//JVX_SEQUENCER_EVENT_SEQUENCE_LEAVE_STEPS_STARTED = 0x80,
	//JVX_SEQUENCER_EVENT_PROCESS_TERMINATED	

	return(JVX_NO_ERROR);
}
void
uMainWindow::timerExpired()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxInt64 tStamp = JVX_GET_TICKCOUNT_US_GET(sequencerStruct.tStamp);
	res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if((res == JVX_NO_ERROR) && sequencer)
	{
		jvxErrorType res = sequencer->trigger_step_process_extern(tStamp);
		if(res == JVX_ERROR_ABORT)
		{
			sequencer->trigger_complete_process_extern(tStamp);
		}	
		 this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	}
}

void 
uMainWindow::updateStateSequencer_inThread(jvxUInt64 event_mask, int sequenceId, int stepId, int queuetp)
{
	if(event_mask == JVX_SEQUENCER_EVENT_PROCESS_TERMINATED)
	{
		jvxErrorType res = JVX_NO_ERROR;
		IjvxSequencer* sequencer = NULL;
		jvxSize num = 0;
		res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
		if((res == JVX_NO_ERROR) && sequencer)
		{
			sequencer->wait_completion_process(JVX_INFINITE);
			res = this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));

			if(sequencerStruct.runtime.externalTrigger)
			{
				// Stop the QTimer thread
				sequencerStruct.runtime.theTimer->stop();
				disconnect(this, SLOT(timerExpired()));
				sequencerStruct.runtime.theTimer = NULL;
			}

			readwrite.theBuffers.addRecordedEntry();
			this->updatePlotData();
			this->updatePlots();

			this->checkBox_listen->setEnabled(true);
			this->checkBox_loop->setEnabled(true);
		}
	}
	this->updateWindow();
}

void 
uMainWindow::updateStateSequencer_outThread(jvxUInt64 event_mask, int sequenceId, int stepId, int queuetp)
{
	jvxSequencerElementType elm;
	jvxComponentType tp;
	emit emit_updateStateSequencer(event_mask, sequenceId, stepId, queuetp);
	
	// Do some specific stuff during sequencer update
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxSize num = 0;
	res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if((res == JVX_NO_ERROR) && sequencer)
	{
		sequencer->description_step_sequence(sequenceId, stepId, (jvxSequencerQueueType)queuetp, NULL, &elm, &tp, NULL, NULL, NULL, NULL, NULL);
		if(
			(elm == JVX_SEQUENCER_TYPE_COMMAND_STD_PREPARE) && 
			(tp == JVX_COMPONENT_AUDIO_DEVICE))
		{
			IjvxProperties* theProps = NULL;
			IjvxObject* theObj = NULL;
			jvxData container[JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];
			jvxBool isValid = false;
			std::string txt;
			jvxErrorType res = this->involvedComponents.theHost.hHost->request_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
			if((res == JVX_NO_ERROR) && theProps)
			{
				if(!systemParams.in.mute)
				{
					this->systemParams.in.levelGain_inuse = this->systemParams.in.levelGain_user;
				}
				container[0] = this->systemParams.in.levelGain_inuse;
				container[1] = this->systemParams.in.levelGain_inuse;

				res = theProps->set_property(&container, 2, JVX_DATAFORMAT_DATA, 16, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true);

				if(!systemParams.out.mute)
				{
					this->systemParams.out.levelGain_inuse = this->systemParams.out.levelGain_user;
				}
				container[0] = this->systemParams.out.levelGain_inuse;
				container[1] = this->systemParams.out.levelGain_inuse;

				res = theProps->set_property(&container, 2, JVX_DATAFORMAT_DATA, 17, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true);

				this->involvedComponents.theHost.hHost->return_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
			}
		}
		this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	}
}

jvxErrorType
uMainWindow::sequencer_callback(jvxInt16 sequenceId, jvxInt16 stepId, jvxSequencerQueueType tp, jvxInt32 functionId)
{
	return(JVX_NO_ERROR);
}
