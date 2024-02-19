#include "myCentralWidget.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

void
myCentralWidget::init_submodule(IjvxHost* theHost)
{
	mainCentral_host_ww::init_submodule(theHost);
	this->setupUi(this);
}

// ===================================================================================

void 
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	mainCentral_host_ww::inform_active(tp, theProps);
	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		thePropRef_algo = theProps;
		myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), thePropRef_algo, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "audio_node");
		break;
	default:
		break;
	}
}

void 
myCentralWidget::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	mainCentral_host_ww::inform_inactive(tp, theProps);
	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		myWidgetWrapper.deassociateAutoWidgets("audio_node");
		thePropRef_algo = NULL;

		break;
	default:
		break;
	}
}

void 
myCentralWidget::inform_sequencer_about_to_start()
{
	mainCentral_host_ww::inform_sequencer_about_to_start();
}

void 
myCentralWidget::inform_sequencer_stopped()
{
	mainCentral_host_ww::inform_sequencer_stopped();
}


void 
myCentralWidget::inform_update_window(jvxCBitField prio)
{
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_host_ww::inform_update_window(prio);
}

void 
myCentralWidget::reportCloseEventTriggered()
{
	jvxCBitField prio;
	jvx_bitFClear(prio);
	jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	inform_update_window(prio);
}

void myCentralWidget::reportUpdateWindow()
{
	jvxCBitField prio;
	jvx_bitFClear(prio);
	jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	inform_update_window(prio);
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
