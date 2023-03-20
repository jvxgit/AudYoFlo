#include "mainCentral_hostfactory.h"

#include <QScrollArea>
#include <QMessageBox>
#include "jvx.h"
#include <iostream>

Q_DECLARE_METATYPE(jvxHandle*);
Q_DECLARE_METATYPE(jvxSize);
Q_DECLARE_METATYPE(jvxBool);

mainCentral_hostfactory::mainCentral_hostfactory(QWidget* parent): QWidget(parent)
{
	
	qRegisterMetaType<jvxHandle*>("jvxHandle*");
	qRegisterMetaType<jvxWebServerHandlerPurpose>("jvxWebServerHandlerPurpose");
	qRegisterMetaType<jvxSize>("jvxSize");
	qRegisterMetaType<jvxBool>("jvxBool");

	theHostRef = NULL;
	mwCtrlRef = NULL;
	theReport = NULL;

	theToolsHost = NULL;
}

// #################################################################
void
mainCentral_hostfactory::init_submodule(IjvxFactoryHost* theHost)
{
	jvxSize i,j;
	theHostRef = theHost;

	if (theHostRef)
	{
		theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
	}

	assert(theHostRef);
	assert(theReport);
}

void
mainCentral_hostfactory::terminate_submodule( )
{
	theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theToolsHost));
	theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReport));
	theReport = NULL;
	theHostRef = NULL;
	mwCtrlRef = NULL;
}

void
mainCentral_hostfactory::init_extend_specific_widgets(IjvxMainWindowControl* ctrl, IjvxQtSpecificWidget_report* rep_spec_widgets)
{
	mwCtrlRef = ctrl;
}

void
mainCentral_hostfactory::terminate_extend_specific_widgets()
{
	mwCtrlRef = NULL;
}

void
mainCentral_hostfactory::inform_select(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
}


void 
mainCentral_hostfactory::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
}

void
mainCentral_hostfactory::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
}

void
mainCentral_hostfactory::inform_unselect(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
}


void 
mainCentral_hostfactory::inform_sequencer_about_to_start()
{
}

void
mainCentral_hostfactory::inform_sequencer_started()
{
}

void 
mainCentral_hostfactory::inform_sequencer_stopped()
{
}

void
mainCentral_hostfactory::inform_sequencer_about_to_stop()
{

}

void
mainCentral_hostfactory::inform_viewer_about_to_start(jvxSize* tout_viewer)
{
}

void
mainCentral_hostfactory::inform_viewer_started()
{
}

void
mainCentral_hostfactory::inform_viewer_stopped()
{
}

void
mainCentral_hostfactory::inform_viewer_about_to_stop()
{

}

void
mainCentral_hostfactory::return_widget(QWidget** refWidget)
{
	*refWidget = static_cast<QWidget*>(this);
}

/*
 * Callback to indiate that the main window is supposed to be redrawn
 *///=======================================================================

void
mainCentral_hostfactory::inform_update_window(jvxCBitField prio)
{

}

/*
 * Callback to make periodic updates of the window (in case it is running)
 *///=====================================================================
void
mainCentral_hostfactory::inform_update_window_periodic()
{

}

void 
mainCentral_hostfactory::inform_update_properties(jvxComponentIdentification& tp)
{
}

jvxErrorType
mainCentral_hostfactory::report_command_request(const CjvxReportCommandRequest& req)
{
	return JVX_ERROR_UNSUPPORTED;
}


/*
 * Callback to indicate that booting has been completed
 *///=====================================================================
void
mainCentral_hostfactory::inform_bootup_complete(jvxBool* wantsToAdjust)
{
	if (theHostRef)
	{
		theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theToolsHost));
	}
}

void
mainCentral_hostfactory::inform_about_to_shutdown()
{
	if (theHostRef)
	{
		theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theToolsHost));
	}
	theToolsHost = NULL;
}

jvxErrorType
mainCentral_hostfactory::inform_close_triggered()
{
	return(JVX_NO_ERROR);
}

void
mainCentral_hostfactory::inform_config_read_complete(const char* fName)
{

}

void
mainCentral_hostfactory::inform_sequencer_error(const char* err, const char* ferr) 
{
}

void
mainCentral_hostfactory::inform_request_shutdown(jvxBool* requestHandled)
{
}

void
mainCentral_hostfactory::inform_internals_have_changed(const jvxComponentIdentification& tp,
	IjvxObject* theObj,
	jvxPropertyCategoryType cat,
	jvxSize propId,
	bool onlyContent,
	const jvxComponentIdentification& tpTo,
	jvxPropertyCallPurpose purpose)
{

}

jvxErrorType
mainCentral_hostfactory::inform_sequencer_callback(jvxSize functionId)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
mainCentral_hostfactory::inform_specific(jvxSize idSpec, jvxHandle* fldSpecific, jvxSize fldTypeId)
{
	return JVX_ERROR_UNSUPPORTED;
}

bool 
mainCentral_hostfactory::is_ready()
{
	return true;
}

