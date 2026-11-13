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
	
	myWidgetAudioSpectrum = NULL;
	init_jvxQtMeasureAudioSpectrum(&myWidgetAudioSpectrum, static_cast<QWidget*>(this));
	myWidgetAudioSpectrum->init(theHost);
	myWidgetAudioSpectrum->getMyQWidget(&theQWidget);
	// https://stackoverflow.com/questions/31323749/easiest-way-for-offscreen-rendering-with-qopenglwidget
	tabWidget->addTab(theQWidget, "Real-time Audio Spectrum");
	
	JVX_MAIN_CENTRAL_ATTACH_TREE_WIDGET(propertyTreeWidget, tabWidget);

	seqRunning = false;

//	widget_logFile->setup();

}

void
myCentralWidget::terminate_submodule()
{
	myWidgetAudioSpectrum->terminate();
	terminate_jvxQtMeasureAudioSpectrum(myWidgetAudioSpectrum);

	JVX_MAIN_CENTRAL_DETACH_TREE_WIDGET(propertyTreeWidget, tabWidget);
}

myCentralWidget::~myCentralWidget()
{
	
}

void 
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_host_ww_log::inform_active(tp, theProps);
	std::vector<std::string> propsCategories;

	JVX_MAIN_CENTRAL_TREE_WIDGET_ACTIVE(theProps, propertyTreeWidget);

	switch(tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:

		thePropRef_algo = theProps;
		if(thePropRef_algo)
		{
			jvxBool isValidPropRef = false;
			thePropRef_algo->reference_status(NULL, &isValidPropRef);
			if (isValidPropRef)
			{
				myWidgetAudioSpectrum->addPropertyReference(thePropRef_algo);
				myWidgetAudioSpectrum->activate();
				myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), thePropRef_algo,
					static_cast<IjvxQtSaWidgetWrapper_report*>(this), "audio_node", true,
					JVX_WW_WIDGET_SEARCH_ALL_CHILDREN); // Associate but step up in widget hierarchie first
			}
		}
		break;
	case JVX_COMPONENT_AUDIO_DEVICE:
		thePropRef_dev = theProps;
		if (thePropRef_dev)
		{
			jvxBool isValidPropRef = false;
			thePropRef_dev->reference_status(NULL, &isValidPropRef);
			if (isValidPropRef)
			{
				myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), thePropRef_dev,
					static_cast<IjvxQtSaWidgetWrapper_report*>(this), "audio_device", true,
					JVX_WW_WIDGET_SEARCH_ALL_CHILDREN); // Associate but step up in widget hierarchie first
			}
		}
		break;
	default:
		break;
	}
}

void 
myCentralWidget::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{		
	JVX_MAIN_CENTRAL_TREE_WIDGET_INACTIVE(theProps, propertyTreeWidget);
	switch(tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		myWidgetAudioSpectrum->deactivate();
		myWidgetAudioSpectrum->removePropertyReference(thePropRef_algo);
		myWidgetWrapper.deassociateAutoWidgets("audio_node");

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
		
	myWidgetAudioSpectrum->processing_started();

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
	myWidgetAudioSpectrum->processing_stopped();

	seqRunning = false;
}

void 
myCentralWidget::inform_update_window(jvxCBitField prio)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBool togLog = false;
	mainCentral_host_ww_log::inform_update_window(prio);

	JVX_MAIN_CENTRAL_TREE_WIDGET_UPDATE_WINDOW(propertyTreeWidget, prio);

	if(thePropRef_algo) //  jvx_isValidPropertyReferenceTriple(theTriples_ref(JVX_COMPONENT_AUDIO_NODE)))
	{
		std::string tag = "audio_node";
		myWidgetWrapper.trigger_updateWindow(tag);
	}
	
	if (thePropRef_dev) // jvx_isValidPropertyReferenceTriple(theTriples_ref(JVX_COMPONENT_AUDIO_DEVICE)))
	{
		std::string tag = "audio_device";
		myWidgetWrapper.trigger_updateWindow(tag);
	}

	if (thePropRef_algo) //  jvx_isValidPropertyReferenceTriple(theTriples_ref(JVX_COMPONENT_AUDIO_NODE)))
	{
		myWidgetAudioSpectrum->update_window();
	}
}

void 
myCentralWidget::inform_update_window_periodic()
{
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_host_ww::inform_update_window_periodic();

	JVX_MAIN_CENTRAL_TREE_WIDGET_UPDATE_WINDOW_PERIODIC(propertyTreeWidget);

	myWidgetAudioSpectrum->update_window_periodic();

	if(thePropRef_algo) //jvx_isValidPropertyReferenceTriple(theTriples_ref(JVX_COMPONENT_AUDIO_NODE)))
	{
		std::string tag = "audio_node";
		myWidgetWrapper.trigger_updateWindow_periodic(tag);
	}

	if (thePropRef_dev) // jvx_isValidPropertyReferenceTriple(theTriples_ref(JVX_COMPONENT_AUDIO_DEVICE)))
	{
		std::string tag = "audio_device";
		myWidgetWrapper.trigger_updateWindow_periodic(tag);
	}
}

void 
myCentralWidget::clicked_tabBarWidget(int id)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(thePropRef_algo) // jvx_isValidPropertyReferenceTriple(theTriples_ref(JVX_COMPONENT_AUDIO_NODE)))
	{
		if(!seqRunning)
		{
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

void
myCentralWidget::report_widget_closed(QWidget* theClosedWidget)
{
}

void
myCentralWidget::report_widget_specific(jvxSize id, jvxHandle* spec)
{
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
