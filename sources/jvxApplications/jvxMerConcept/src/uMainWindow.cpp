#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include "uMainWindow.h"


uMainWindow::uMainWindow()
{
	resetJvxReferences();

	this->systemParams.rtv_period_ms = 500;

	sequencerStruct.extTrigger = true;
	sequencerStruct.idQtThread = JVX_GET_CURRENT_THREAD_ID();
	sequencerStruct.sequenceComplete = false;

	sequencerStruct.runtime.externalTrigger = false;
	sequencerStruct.runtime.theTimer = NULL;

	externalChannels.theObject = NULL;
	externalChannels.theTech = NULL;
	externalChannels.theChannels = NULL;
	externalChannels.inId = 0;
	externalChannels.outId = 0;

	systemParams.in.levelGain_user = 1.0;
	systemParams.in.levelGain_inuse = 1.0;
	systemParams.in.mute = false;

	systemParams.out.levelGain_user = 1.0;
	systemParams.out.levelGain_inuse = 1.0;
	systemParams.out.mute = false;

	systemParams.listen = false;
	systemParams.inSlider = 0;

	rtPlot.theTimer = NULL;
	rtViewer.theTimer = NULL;

	myState = JVX_STANDALONE_HOST_STATE_NONE;
}

uMainWindow::~uMainWindow(void)
{
}

void
uMainWindow::postMessage_outThread(const char* txt, QColor col)
{
	QString txtq = txt;
	emit emit_postMessage(txtq, col);
}

void
uMainWindow::postMessage_inThread(QString txt, QColor col)
{
	QListWidgetItem* item = new QListWidgetItem(txt);
	item->setTextColor(col);
	this->listWidget_messages->addItem(item);
	this->listWidget_messages->setCurrentRow(this->listWidget_messages->count()-1);
//	this->subWidgets.messages.theWidget->postMessage_inThread(txt, col);
}

jvxErrorType
uMainWindow::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	switch(prio)
	{
	case JVX_REPORT_PRIORITY_ERROR:
		postMessage_outThread(txt, JVX_QT_COLOR_ERROR);
		break;
	case JVX_REPORT_PRIORITY_WARNING:
		postMessage_outThread(txt, JVX_QT_COLOR_WARNING);
		break;
	case JVX_REPORT_PRIORITY_INFO:
		postMessage_outThread(txt, JVX_QT_COLOR_INFORM);
		break;
	case JVX_REPORT_PRIORITY_SUCCESS:
		postMessage_outThread(txt, JVX_QT_COLOR_SUCCESS);
		break;
	default:
		postMessage_outThread(txt, Qt::black);
		break;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::report_command_request(jvxCommandRequestType request,jvxHandle *,jvxSize)
{
	emit emit_report_command_request((short)request);
	return(JVX_NO_ERROR);
}

void
uMainWindow::report_command_request_inThread(short request)
{
	jvxCommandRequestType theRequest = (jvxCommandRequestType)request;
	switch(theRequest)
	{
	case JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER:

		switch(myState)
		{
		case JVX_STANDALONE_HOST_STATE_WAITING:
			// Do nothing
			break;
		case JVX_STANDALONE_HOST_STATE_PROCESSING:
			this->pushed_Process();
			break;
		case JVX_STANDALONE_HOST_STATE_LISTENING:
			this->pushed_Listen();
			break;
		case JVX_STANDALONE_HOST_STATE_RECORDING:
			this->pushed_Record();
			break;
		case JVX_STANDALONE_HOST_STATE_PLAYING:
			this->pushed_Play();
			break;
		}

		break;
	default:
		assert(0);
	}
}


jvxErrorType
uMainWindow::report_internals_have_changed(jvxComponentType tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxInt16 propId, bool onlyContent, jvxComponentType tpTo,
													jvxPropertyCallPurpose callpurpose)
{
	// Check config lines..
	jvxErrorType res = JVX_NO_ERROR;

	// Follow deployment of properties as specified in the configuration lines
	IjvxConfigurationLine* theConfigLine = NULL;

	// Config parameters to config line
	res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle**>(&theConfigLine));

	if((res == JVX_NO_ERROR) && theConfigLine)
	{
		theConfigLine->report_property_was_set(tp, cat, propId, onlyContent, tpTo);

		// Report to update all UIs since important stuff has changed
		report_internals_have_changed_outThread(tp, theObj,  cat,  propId, onlyContent, tpTo, callpurpose);

		this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle*>(theConfigLine));
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::report_take_over_property(jvxComponentType thisismytype, IjvxObject* thisisme,
		 jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		 jvxPropertyCategoryType cat, jvxInt16 propId, jvxComponentType tpTo, jvxPropertyCallPurpose callpurp)
{
	return(JVX_ERROR_UNSUPPORTED);
}

jvxErrorType
uMainWindow::report_os_specific(jvxSize commandId, void* context)
{
	return(JVX_ERROR_UNSUPPORTED);
}

void
uMainWindow::report_internals_have_changed_outThread(jvxComponentType tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxInt16 propId, bool onlyContent, jvxComponentType tpTo, jvxPropertyCallPurpose callpurp)
{
	emit emit_report_internals_have_changed_inThread(tp, theObj, cat, propId, onlyContent, tpTo, callpurp);
}

void
uMainWindow::report_internals_have_changed_inThread(jvxComponentType tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxInt16 propId, bool onlyContent, jvxComponentType tpTo, jvxPropertyCallPurpose callpurp)
{
	/* Just do nothing here .. */
	//this->inform_internals_have_changed(tp, theObjectFrom, cat, propId, onlyContent, tpTo);
}
