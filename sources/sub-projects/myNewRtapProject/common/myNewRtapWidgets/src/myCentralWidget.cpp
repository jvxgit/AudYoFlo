#include "myCentralWidget.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

	void
#ifdef JVX_COMPILE_REMOTE_CONTROL
		myCentralWidget::init_submodule(IjvxFactoryHost* theHost)
#else
		myCentralWidget::init_submodule(IjvxHost* theHost)
#endif
{
	JVX_MAINCENTRAL_CLASSNAME::init_submodule(theHost);
	this->setupUi(this);
}

// ===================================================================================

void 
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	JVX_MAINCENTRAL_CLASSNAME::inform_active(tp, theProps);
	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		thePropRef = theProps;
		myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), thePropRef,
			static_cast<IjvxQtSaWidgetWrapper_report*>(this), "audio_node");
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
		thePropRef = NULL;
		break;
	default:
		break;
	}
	JVX_MAINCENTRAL_CLASSNAME::inform_inactive(tp, theProps);
}

void 
myCentralWidget::inform_sequencer_about_to_start()
{
	JVX_MAINCENTRAL_CLASSNAME::inform_sequencer_about_to_start();
}

void 
myCentralWidget::inform_sequencer_stopped()
{
	JVX_MAINCENTRAL_CLASSNAME::inform_sequencer_stopped();
}


void 
myCentralWidget::inform_update_window(jvxCBitField prio )
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;

	JVX_MAINCENTRAL_CLASSNAME::inform_update_window(prio);

	myWidgetWrapper.trigger_updateWindow("audio_node");
}

void 
myCentralWidget::reportCloseEventTriggered()
{
	inform_update_window();
}

void myCentralWidget::reportUpdateWindow()
{
	inform_update_window();
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

#ifdef JVX_COMPILE_REMOTE_CONTROL

/*
jvxErrorType
myCentralWidget::report_property_binary_data(jvxComponentIdentification tp, jvxSize uId, jvxByte* payload,
	jvxPropertyTransferPriority prio)
{
	return JVX_NO_ERROR;
}

jvxErrorType
myCentralWidget::report_property_registered(jvxComponentIdentification tp, jvxSize uId)
{
	return JVX_NO_ERROR;
}

jvxErrorType
myCentralWidget::report_property_unregistered(jvxComponentIdentification tp, jvxSize uId)
{
	return JVX_NO_ERROR;
}
*/

void
myCentralWidget::process_websocket_binary(jvxSize userData, jvxByte* payload, jvxPropertyTransferPriority prio)
{
}

jvxErrorType
myCentralWidget::report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr)
{
	return JVX_NO_ERROR;
}

jvxErrorType
myCentralWidget::report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr)
{
	return JVX_NO_ERROR;
}

#endif
#ifdef JVX_PROJECT_NAMESPACE
}
#endif
