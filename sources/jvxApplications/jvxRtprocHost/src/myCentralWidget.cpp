#include "myCentralWidget.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

void
myCentralWidget::init_submodule(IjvxHost* theHost)
{
	this->setupUi(this);
	mainCentral_host_ww::init_submodule(theHost);
	audioNodeActive = false;
}

void
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	mainCentral_host_ww::inform_active(tp, theProps);
	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		thePropRef_algo = theProps;
		audioNodeActive = true;
		myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), thePropRef_algo, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "audio_node");
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
		audioNodeActive = false;
		myWidgetWrapper.deassociateAutoWidgets("audio_node");
		thePropRef_algo = NULL;
		break;
	default:
		break;
	}

	// Must follow, not precede!
	mainCentral_host_ww::inform_inactive(tp, theProps);
}

void 
myCentralWidget::inform_update_window(jvxCBitField prio)
{
	std::string propDescr;
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_host_ww::inform_update_window(prio);
	if (jvx_bitTest(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT))
	{
		if (audioNodeActive)
		{
			myWidgetWrapper.deassociateAutoWidgets("audio_node");
			myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), thePropRef_algo, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "audio_node");
		}
	}
	myWidgetWrapper.trigger_updateWindow("audio_node");

}

void 
myCentralWidget::inform_update_window_periodic()
{
	mainCentral_host_ww::inform_update_window_periodic();
}

void
myCentralWidget::inform_about_to_shutdown()
{
	mainCentral_host_ww::inform_about_to_shutdown();
	myWidgetWrapper.deassociateAutoWidgets("audio_node");
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
