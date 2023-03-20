#ifndef __MAINWINDOW_UIEXTENSION_H__
#define __MAINWINDOW_UIEXTENSION_H__

#include "jvx.h"
class QWidget;

// Enum to report an event
typedef enum
{
	JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_NONE,
	JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_HTTP_CONNECTED, // jvxConfigWeb* , JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_TYPE_HTTP_CONNECT_WEB
	JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_HTTP_DISCONNECTED,
	JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_HTTP_OBSERVATION_ABOUT_TO_START,
	JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_HTTP_OBSERVATION_ABOUT_TO_STOP,
	JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_REPORT_SEQUENCER_OPERATION_STATE,
	JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_REPORT_SEQUENCER_UPDATE_WINDOW
} jvxMainWindowSpecificRequestEnum;

// Enum to describe a specific type of variable passed as void pointer
typedef enum
{
	JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_TYPE_NONE,
	JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_TYPE_HTTP_CONNECT_WEB,
	JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_TYPE_SIZE
} jvxMainWindowSpecificTypeEnum;

JVX_INTERFACE jvxWebSocket_properties_install
{
public:
	~jvxWebSocket_properties_install() {};
	virtual jvxErrorType register_properties(jvxComponentIdentification tp, jvxOneRemotePropertySpecification* props, jvxSize numProps) = 0;
	virtual jvxErrorType unregister_properties(jvxComponentIdentification tp) = 0;
};

JVX_INTERFACE jvxWebSocket_properties_report
{
public:
	~jvxWebSocket_properties_report() {};
	virtual jvxErrorType report_property_binary_data(jvxComponentIdentification tp, jvxSize uId, jvxByte* payload, 
		jvxPropertyTransferPriority prio) = 0;
	virtual jvxErrorType report_property_registered(jvxComponentIdentification tp, jvxSize uId) = 0;
	virtual jvxErrorType report_property_unregistered(jvxComponentIdentification tp, jvxSize uId) = 0;
};

/*
JVX_INTERFACE jvxHttp_sequencer_startstop
{
public:
	~jvxHttp_sequencer_startstop(){};
	virtual jvxErrorType trigger_seq_start() = 0;
	virtual jvxErrorType trigger_seq_stop() = 0;
};
*/

typedef struct
{
	jvxBool autoStartSequencer;
//	jvxHttp_sequencer_startstop* startStopHdl;
} jvxConfigWebFlow;

typedef struct
{
	jvxWebSocket_properties_install* useSocket;
	jvxSize ticksUpdateWebSocket_msec;
	jvxSize numTicksUpdateWebSocket;
	jvxWebSocket_properties_report* reportData;
	jvxBool verbose_out;
} jvxConfigWebSocket;

typedef struct
{
	jvxConfigWebSocket ws;
	jvxConfigWebFlow flow;
}jvxConfigWeb;

JVX_INTERFACE IjvxQtSpecificWidget_report;

JVX_INTERFACE mainWindow_UiExtension_common
{
public:
	mainWindow_UiExtension_common() {};
	
	~mainWindow_UiExtension_common() {};

	virtual void init_extend_specific_widgets(IjvxMainWindowControl* ctrl, IjvxQtSpecificWidget_report* rep_spec_widgets) = 0;

	virtual void terminate_extend_specific_widgets() = 0;

	virtual void terminate_submodule() = 0;

	virtual void return_widget(QWidget** refWidget) = 0;

	virtual void inform_bootup_complete(jvxBool* wantsToAdjustSize) = 0;

	virtual void inform_about_to_shutdown() = 0;

	virtual void inform_select(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) = 0;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) = 0;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) = 0;

	virtual void inform_unselect(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) = 0;

	virtual void inform_sequencer_about_to_start() = 0;

	virtual void inform_sequencer_started() = 0;

	virtual void inform_sequencer_about_to_stop() = 0;

	virtual void inform_sequencer_stopped() = 0;

	virtual void inform_viewer_about_to_start(jvxSize* tout_viewer) = 0;

	virtual void inform_viewer_started() = 0;

	virtual void inform_viewer_about_to_stop() = 0;

	virtual void inform_viewer_stopped() = 0;

	virtual void inform_sequencer_error(const char* err, const char* ferr) = 0;

	virtual void inform_request_shutdown(jvxBool* requestHandled) = 0;

	virtual void inform_update_window(jvxCBitField prio) = 0;

	virtual void inform_update_window_periodic() = 0;

	virtual void inform_update_properties(jvxComponentIdentification& tp) = 0;

	// virtual void inform_update_properties_ident(jvxComponentIdentification& tp, const char* ident) = 0;
	virtual jvxErrorType report_command_request(const CjvxReportCommandRequest& req) = 0;

	virtual void inform_internals_have_changed(const jvxComponentIdentification& tp,
		IjvxObject* theObj,
		jvxPropertyCategoryType cat,
		jvxSize propId,
		bool onlyContent,
		const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose purpose) = 0;

	virtual void inform_config_read_complete(const char* fName) = 0;

	virtual jvxErrorType inform_sequencer_callback(jvxSize functionId) = 0;

	virtual jvxErrorType inform_close_triggered() = 0;

	virtual jvxErrorType inform_specific(jvxSize idSpec, jvxHandle* fldSpecific, jvxSize fldTypeId) = 0;

	virtual bool is_ready() = 0;

};

JVX_INTERFACE mainWindow_UiExtension_hostfactory: public mainWindow_UiExtension_common
{
public:
	mainWindow_UiExtension_hostfactory() {};
	virtual ~mainWindow_UiExtension_hostfactory() {};

	virtual void init_submodule(IjvxFactoryHost* theHost) = 0;

};

/*
 * Delegate interface for extension of the host for socket connections
 *///================================================================
JVX_INTERFACE mainWindow_UiExtension_host: public mainWindow_UiExtension_common
{
public:
	mainWindow_UiExtension_host(){};
	virtual ~mainWindow_UiExtension_host(){};
	virtual void init_submodule(IjvxHost* theHost) = 0;
	virtual void get_reference_feature(jvxHandle** priv) = 0;
};

#endif
