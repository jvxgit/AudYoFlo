#include "mainCentral_host.h"

#include <QScrollArea>
#include <QMessageBox>

#include "jvx.h"

#include <iostream>

Q_DECLARE_METATYPE(jvxHandle*);
Q_DECLARE_METATYPE(jvxSize);
Q_DECLARE_METATYPE(jvxBool);

mainCentral_host::mainCentral_host(QWidget* parent): QWidget(parent)
{
	qRegisterMetaType<jvxHandle*>("jvxHandle*");
	qRegisterMetaType<jvxWebServerHandlerPurpose>("jvxWebServerHandlerPurpose");
	qRegisterMetaType<jvxSize>("jvxSize");
	qRegisterMetaType<jvxBool>("jvxBool");

	theHostRef = NULL;
	mwCtrlRef = NULL;
	theReport = NULL;

	theToolsHost = NULL;
	seq_started = false;
}

// #################################################################
void
mainCentral_host::init_submodule(IjvxHost* theHost)
{
	jvxSize i,j;
	theHostRef = theHost;
	theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));

	num_flat_slots_init = request_number_flat_slots_init();
	thePropReferenes.resize(num_flat_slots_init);
	for (i = 0; i < num_flat_slots_init; i++)
	{
		thePropReferenes[i].resize(JVX_COMPONENT_ALL_LIMIT);
		for (j = 0; j < JVX_COMPONENT_ALL_LIMIT; j++)
		{
			thePropReferenes[i][j] = NULL;
		}
	}

	assert(theHostRef);
	assert(theReport);
}

void 
mainCentral_host::get_reference_feature(jvxHandle** priv)
{

}

void 
mainCentral_host::init_extend_specific_widgets(IjvxMainWindowControl* ctrl, IjvxQtSpecificWidget_report* rep_spec_widgets)
{
	mwCtrlRef = ctrl;
}

void
mainCentral_host::terminate_extend_specific_widgets()
{
	mwCtrlRef = NULL;
}

void
mainCentral_host::terminate_submodule( )
{
	theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReport));
	theReport = NULL;
	theHostRef = NULL;
	mwCtrlRef = NULL;
}

void
mainCentral_host::return_widget(QWidget** refWidget)
{
	*refWidget = static_cast<QWidget*>(this);
}

void
mainCentral_host::inform_select(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
}

/*
 * Callback right after a component has been activated.
 *///=======================================================================
void
mainCentral_host::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	jvxSize id_ref = transform_type_location_to_flat_slot(tp);
	// std::cout << "Component activated: " << jvxComponentIdentification_txt(tp) << std::endl;

	thePropReferenes[id_ref][tp.tp] = theProps;
}

/*
 * Callback right after a component has been deactivated.
 *///=======================================================================
void
mainCentral_host::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	jvxSize id_ref = transform_type_location_to_flat_slot(tp);
	thePropReferenes[id_ref][tp.tp] = NULL;
	// std::cout << "Component deactivated: " << jvxComponentIdentification_txt(tp) << std::endl;
}

void
mainCentral_host::inform_unselect(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
}

/*
 * Callback right after the sequencer has been started
 *///=======================================================================
void
mainCentral_host::inform_sequencer_about_to_start()
{
}

void
mainCentral_host::inform_sequencer_started()
{
	seq_started = true;
}

void
mainCentral_host::inform_sequencer_about_to_stop()
{
}

/*
 * Callback right after the sequencer has been stopped
 *///=======================================================================
void
mainCentral_host::inform_sequencer_stopped()
{
	seq_started = false;
}

/*
 * Callback right after the sequencer has been started
 *///=======================================================================
void
mainCentral_host::inform_viewer_about_to_start(jvxSize* tout_viewer)
{
}

void
mainCentral_host::inform_viewer_started()
{
}

void
mainCentral_host::inform_viewer_about_to_stop()
{
}

/*
 * Callback right after the sequencer has been stopped
 *///=======================================================================
void
mainCentral_host::inform_viewer_stopped()
{
}
void
mainCentral_host::inform_sequencer_error(const char* err, const char* ferr)
{
	/*
	QMessageBox reportBox(this);
	reportBox.setText("Error reported by sequencer.");
	reportBox.setInformativeText(((std::string)"First error: " + ferr).c_str());
	reportBox.exec();
	*/
	QMessageBox *reportBox = new QMessageBox(this);
	reportBox->setText("Error reported by sequencer.");
	reportBox->setInformativeText(((std::string)"First error: " + ferr).c_str());
	reportBox->show();
}

/*
 * Callback to indiate that the main window is supposed to be redrawn
 *///=======================================================================

void
mainCentral_host::inform_update_window(jvxCBitField prio)
{
}

void
mainCentral_host::inform_update_properties(jvxComponentIdentification& tp)
{
}

/*
 * Callback to make periodic updates of the window (in case it is running)
 *///=====================================================================
void
mainCentral_host::inform_update_window_periodic()
{
}

jvxErrorType
mainCentral_host::inform_specific(jvxSize idSpec, jvxHandle* fldSpecific, jvxSize fldTypeId)
{
	return JVX_ERROR_UNSUPPORTED;
}

/*
 * Callback to indicate that booting has been completed
 *///=====================================================================
void
mainCentral_host::inform_bootup_complete(jvxBool* wantsToAdjust)
{
	jvxErrorType resL = JVX_NO_ERROR;
	if(theHostRef)
	{
		resL = theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theToolsHost));
	}
}

void
mainCentral_host::inform_about_to_shutdown()
{
}

void 
mainCentral_host::inform_request_shutdown(jvxBool* requestHandled)
{

}

jvxErrorType
mainCentral_host::inform_close_triggered()
{
	return(JVX_NO_ERROR);
}

/*
 * Callback to indicate that one of the properties has changed internally
 *///=====================================================================
void
mainCentral_host::inform_internals_have_changed(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId,
	bool onlyContent, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose purpose)
{

}

jvxErrorType
mainCentral_host::inform_sequencer_callback(jvxSize functionId)
{
	return(JVX_NO_ERROR);
}

void
mainCentral_host::inform_config_read_complete(const char* fName)
{

}

jvxSize
mainCentral_host::request_number_flat_slots_init()
{
	return 1;
}

jvxSize
mainCentral_host::transform_type_location_to_flat_slot(const jvxComponentIdentification& tp)
{
	return 0;
}

