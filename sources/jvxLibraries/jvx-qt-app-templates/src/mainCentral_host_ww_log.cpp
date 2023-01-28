#include "mainCentral_host_ww_log.h"


mainCentral_host_ww_log::mainCentral_host_ww_log(QWidget* parent): mainCentral_host_ww(parent)
{
	theLogFileRef = new jvxQtMaLogFileWriter;
}

// #################################################################
void
mainCentral_host_ww_log::init_submodule(IjvxHost* theHost)
{
	mainCentral_host_ww::init_submodule(theHost);

	// Set first reference for MA widget for logfile writer
	theLogFileRef->setWidgetWrapperReferences(&myWidgetWrapper, static_cast<IjvxQtSaWidgetWrapper_report*>(this));
}

/*
 * Callback right after a component has been activated.
 *///=======================================================================
void
mainCentral_host_ww_log::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	mainCentral_host_ww::inform_active(tp, theProps);
	theLogFileRef->inform_active_subwidget(tp, theProps);
}

/*
 * Callback right after a component has been deactivated.
 *///=======================================================================
void
mainCentral_host_ww_log::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	theLogFileRef->inform_inactive_subwidget(tp, NULL);
	mainCentral_host_ww::inform_inactive(tp, theProps);
}

void
mainCentral_host_ww_log::inform_update_window(jvxCBitField prio)
{
	mainCentral_host_ww::inform_update_window(prio);
	theLogFileRef->update_window_subwidget();
}

/*
 * Callback to make periodic updates of the window (in case it is running)
 *///=====================================================================
void
mainCentral_host_ww_log::inform_update_window_periodic()
{
	theLogFileRef->update_window_subwidget_periodic();
}

