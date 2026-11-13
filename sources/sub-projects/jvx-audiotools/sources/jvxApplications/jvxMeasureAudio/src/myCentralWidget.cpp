#include "myCentralWidget.h"
#include <QtWidgets/QFileDialog>

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

// ===============================================================
void
myCentralWidget::init_submodule(IjvxHost* theHost)
{
	QWidget* theQWidget = NULL;
	this->setupUi(this);

	mainCentral_host_ww_log::init_submodule(theHost);

	theLogFileRef->setWidgetReferences(this->comboBox_controllogfile, this->widget_logFile);

//	jvxQtSaLogFileWriter* theLogFileWriterWidget;

	myWidgetIrMeasure = NULL;
	init_jvxQtMeasureAudioIr(&myWidgetIrMeasure, static_cast<QWidget*>(this));
	myWidgetIrMeasure->init(theHost);
	myWidgetIrMeasure->getMyQWidget(&theQWidget);
	tabWidget->addTab(theQWidget, "Measure Ir");

	myWidgetJitterMeasure = NULL;
	init_jvxQtMeasureAudioJitter(&myWidgetJitterMeasure, static_cast<QWidget*>(this));
	myWidgetJitterMeasure ->init(theHost);
	myWidgetJitterMeasure ->getMyQWidget(&theQWidget);
	tabWidget->addTab(theQWidget, "Measure Device Jitter");

	currentOperation = JVX_AUNMEASURE_NO_OPERATION;
	seqRunning = false;

//	widget_logFile->setup();

/*
	CjvxBitFieldN myBitField;
	CjvxBitFieldN myBitFieldneg = -1;
	std::string tst;
	myBitField = (CjvxBitFieldN)1 << 3;
	std::cout << "Out1: " << myBitField.toString() << std::endl;
	myBitField = myBitField << 64;
	std::cout << "Out2: " << myBitField.toString() << std::endl;
	myBitField = 0x12345678;
	std::cout << "Out3: " << myBitField.toString() << std::endl;
	myBitField = myBitField << 13;
	tst = myBitField.toString();
	std::cout << "Out4: " << tst << std::endl;
	myBitField.fromString(tst);
	myBitField = myBitField >> 13;
	std::cout << "Out5: " << myBitField.toString() << std::endl;
	myBitField = myBitField + myBitFieldneg;
	std::cout << "Out5: " << myBitField.toString() << std::endl;
	if(myBitFieldneg.evaluateBool())
	{
		std::cout << "Out6: " << myBitFieldneg.toString() << std::endl;
	}
	myBitField = myBitField | (jvxBitField)0x87654321 << 67;
	std::cout << "Out7: " << myBitFieldneg.toString() << std::endl;
	*/
}

myCentralWidget::~myCentralWidget()
{
	myWidgetIrMeasure->terminate();
	terminate_jvxQtMeasureAudioIr(myWidgetIrMeasure);
	terminate_jvxQtMeasureAudioJitter(myWidgetJitterMeasure);
}

void
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_host_ww_log::inform_active(tp, theProps);
	std::vector<std::string> propsCategories;
	jvxCallManagerProperties callGate;
	switch(tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		thePropRef_algo = theProps;

		if(thePropRef_algo)
		{
			myWidgetIrMeasure->addPropertyReference(thePropRef_algo);
			myWidgetJitterMeasure->addPropertyReference(thePropRef_algo);
			ident.reset("/mode/currentMode");
			trans.reset(true);
			thePropRef_algo->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, "/mode/currentMode", thePropRef_algo), "/mode/currentMode", res)

			if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
			{
				// 0x1 means: currently no specific mode
				myWidgetIrMeasure->activate();
			}
			//associateAutoWidgets(static_cast<QWidget*>(this), &theTriples[JVX_COMPONENT_AUDIO_NODE], static_cast<IjvxQtWidgetWrapper_report*>(this), "audio_node");

		}
		break;
	case JVX_COMPONENT_AUDIO_DEVICE:
		thePropRef_dev = theProps;

		if (thePropRef_dev)
		{
			myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), thePropRef_dev,
				static_cast<IjvxQtSaWidgetWrapper_report*>(this), "audio_device", true, 
				JVX_WW_WIDGET_SEARCH_ALL_CHILDREN); // Associate but step up in widget hierarchie first
		}
		break;
	default:
		break;
	}
}

void
myCentralWidget::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{

	switch(tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		if(currentOperation == JVX_AUNMEASURE_MEASUREIR)
		{
			myWidgetIrMeasure->deactivate();
		}
		else
		{
				myWidgetJitterMeasure->deactivate();
		}

		myWidgetIrMeasure->removePropertyReference(thePropRef_algo);
		myWidgetJitterMeasure->removePropertyReference(thePropRef_algo);

		//deassociateAutoWidgets("audio-node");
		thePropRef_algo = NULL;
		break;

	case JVX_COMPONENT_AUDIO_DEVICE:

		myWidgetWrapper.deassociateAutoWidgets("audio_device");
		thePropRef_dev = NULL;
		break;
	default:
		break;
	}
	mainCentral_host_ww_log::inform_inactive(tp, theProps);

}

void
myCentralWidget::inform_sequencer_about_to_start()
{
	mainCentral_host_ww_log::inform_sequencer_about_to_start();

	if(currentOperation == JVX_AUNMEASURE_MEASUREIR)
	{
		tabWidget->setTabEnabled(1, false);
		myWidgetIrMeasure->processing_started();

	}
	else
	{
		tabWidget->setTabEnabled(0, false);
		myWidgetJitterMeasure->processing_started();
	}
	/*
	// This is all not needed
	JVX_THREAD_BACKGROUND_BEGIN(JVX_GET_CURRENT_THREAD_HANDLE());
	BOOL res = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	*/
	seqRunning = true;

}

void
myCentralWidget::inform_sequencer_stopped()
{
	/*
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
	JVX_THREAD_BACKGROUND_END(JVX_GET_CURRENT_THREAD_HANDLE());
	*/
	mainCentral_host_ww_log::inform_sequencer_stopped();
	if(currentOperation == JVX_AUNMEASURE_MEASUREIR)
	{
		myWidgetIrMeasure->processing_stopped();
	}
	else
	{
		myWidgetJitterMeasure->processing_stopped();
	}
	seqRunning = false;
	tabWidget->setTabEnabled(0, true);
	tabWidget->setTabEnabled(1, true);
}

void 
myCentralWidget::inform_update_window(jvxCBitField prio)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBool togLog = false;
	jvxCallManagerProperties callGate;
	mainCentral_host_ww_log::inform_update_window(prio);

	if(thePropRef_algo)
	{
		std::string tag = "audio_node";
		myWidgetWrapper.trigger_updateWindow(tag);
		ident.reset("/mode/currentMode");
		trans.reset(true);
		res = thePropRef_algo ->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, "/mode/currentMode", thePropRef_algo), "/mode/currentMode", res)

		if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x2))
		{
			tabWidget->setCurrentIndex(0);
			currentOperation = JVX_AUNMEASURE_MEASUREIR;
		}
		else if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x4))
		{
			tabWidget->setCurrentIndex(1);
			currentOperation = JVX_AUNMEASURE_MEASURETIMING;
		}
		else
		{
			assert(0);
		}
	}

	if (thePropRef_algo)
	{
		std::string tag = "audio_device";
		myWidgetWrapper.trigger_updateWindow(tag);
	}

	if(currentOperation == JVX_AUNMEASURE_MEASUREIR)
	{
		myWidgetIrMeasure->update_window();
	}
	else
	{
		myWidgetJitterMeasure->update_window();
	}
}

void
myCentralWidget::inform_update_window_periodic()
{
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_host_ww_log::inform_update_window_periodic();

	if(currentOperation == JVX_AUNMEASURE_MEASUREIR)
	{
		myWidgetIrMeasure->update_window_periodic();
	}
	else
	{
		myWidgetJitterMeasure->update_window_periodic();
	}

	if(thePropRef_algo)
	{
		std::string tag = "audio_node";
		myWidgetWrapper.trigger_updateWindow_periodic(tag);
	}

	if (thePropRef_algo)
	{
		std::string tag = "audio_device";
		myWidgetWrapper.trigger_updateWindow_periodic(tag);
	}
}

void
myCentralWidget::clicked_tabBarWidget(int id)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	if(thePropRef_algo)
	{
		if(!seqRunning)
		{
			ident.reset("/mode/currentMode");
			trans.reset(true);
			res = thePropRef_algo->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);

			if(id == 1)
			{
				// Switch to measurejitter mode
				if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x2))
				{
					myWidgetIrMeasure->deactivate();
					myWidgetJitterMeasure->activate();
				}
				currentOperation = JVX_AUNMEASURE_MEASURETIMING;
			}
			if(id == 0)
			{
				if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x4))
				{
					myWidgetJitterMeasure->deactivate();
					myWidgetIrMeasure->activate();
				}
				currentOperation = JVX_AUNMEASURE_MEASUREIR;
			}
		}
		else
		{
		}
	}
	this->inform_update_window();
}

void
myCentralWidget::preferredSize(jvxInt32& height, jvxInt32& width)
{
	QWidget* widg = NULL;
	QSize sz = this->size();
	//myWidgetJitterMeasure->getMyQWidget(&widg);
	//height = widg->minimumHeight();
	//width = widg->minimumWidth();
	height = sz.height();
	width = sz.width();
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
