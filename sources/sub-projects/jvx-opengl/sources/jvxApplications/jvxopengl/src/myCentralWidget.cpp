#include "myCentralWidget.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

void
myCentralWidget::init_submodule(IjvxHost* theHost)
{
	mainCentral_host_ww::init_submodule(theHost);
	this->setupUi(this);

	openGLWidget = new CjvxMyGlWidget(this);
	QWidget* widgetRef = NULL;
	openGLWidget->getWidgetReferences(&widgetRef);
	assert(widgetRef);
	QLayout* lay = frame_glW->layout();
	assert(lay == NULL);
	QHBoxLayout* layN = new QHBoxLayout;
	layN->insertWidget(0, widgetRef);
	frame_glW->setLayout(layN);
}

// ===================================================================================

void 
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	mainCentral_host_ww::inform_active(tp, theProps);
	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		thePropRef_aun = theProps;
		myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), thePropRef_aun, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "audio_node");
		break;
	case JVX_COMPONENT_VIDEO_DEVICE:
		thePropRef_vin = theProps;
		openGLWidget->setConnectLinks(thePropRef_vin, "expose_visual_if", jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL);
		break;
	default:
		break;
	}
}

void 
myCentralWidget::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		myWidgetWrapper.deassociateAutoWidgets("audio_node");
		thePropRef_aun = NULL;
		break;
	case JVX_COMPONENT_VIDEO_DEVICE:
		openGLWidget->clearConnectLinks();
		thePropRef_vin= NULL;
		break;
	default:
		break;
	}
	mainCentral_host_ww::inform_inactive(tp, theProps);
}

void 
myCentralWidget::inform_sequencer_about_to_start()
{
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_host_ww::inform_sequencer_about_to_start();
	res = this->openGLWidget->inform_about_to_start();
	assert(res == JVX_NO_ERROR);

}

void 
myCentralWidget::inform_sequencer_stopped()
{
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_host_ww::inform_sequencer_stopped();
	openGLWidget->inform_stopped();
	assert(res == JVX_NO_ERROR);
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
