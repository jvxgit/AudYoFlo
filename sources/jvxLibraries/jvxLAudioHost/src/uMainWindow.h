#ifndef __UMAINWINDOW_H__
#define __UMAINWINDOW_H__

#include "ui_mainApplication.h"
#include "jvx.h"
#include "jvx-helpers.h"
#include "uMainWindow_defines.h"
#include "commandline/CjvxCommandLine.h"
#include "configureAudio.h"
#include "common/CjvxRealtimeViewer.h"
#include "realtimeViewer_helpers.h"
#include "allHostsStatic_common.h"
#include "uMainWindow_specific.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"

//#include "jvx_connections_widget.h"
#include "jvx_mainwindow_control.h"
#include "jvx_eventloop_object.h"

#include "CjvxAppHostProduct.h"

#define JVX_DESCRIPTION_CONNECTION_WIDGET "Open Connection Control"
#define JVX_DESCRIPTION_SEQUENCER_WIDGET "Open Sequencer Control"

class uMainWindow:
	public uMainWindow_specific, public Ui::MainWindow,
	public IjvxReport, public IjvxReportOnConfig,
	public IjvxQtSpecificWidget_report,
	public IjvxQtSequencerWidget_report,
	public IjvxMainWindowControl_connect,
	public IjvxMainWindowController_report,
	public jvx_menu_host_bridge_components_report,
	public CjvxHandleRequestCommands_callbacks,
	public JVX_APPHOST_PRODUCT_CLASSNAME
{
	friend class mainCentral;
	friend class configureAudio;
	friend class additionalArgsWidget;
	friend class uMainWindow_specific;

	Q_OBJECT
private:

	typedef enum
	{
		JVX_START_STOP_INTENDED_TOGGLE,
		JVX_START_STOP_INTENDED_START,
		JVX_START_STOP_INTENDED_STOP
	}jvxStartStopIntendedOperation;

	struct
	{
		struct
		{
			mainWindow_UiExtension_host* theWidget;
		} main;

		struct
		{
			jvxErrorType returnVal;
			JVX_THREAD_ID theQtThreadId;
		} qthost;

		configureAudio* theAudioDialog;
		additionalArgsWidget* theAudioArgs;

		IjvxQtWidgetFrameDialog* theConnectionsFrame;
		QDialog* theConnectionsFrameDialog;
		IjvxQtConnectionWidget* theConnectionsWidget;
		QWidget* theConnectionsQWidget;

		IjvxQtWidgetFrameDialog* theSequencerFrame;
		QDialog* theSequencerFrameDialog;
		IjvxQtSequencerWidget* theSequencerWidget;
		QWidget* theSequencerQWidget;

		IjvxQtMenuComponentsBridge* bridgeMenuComponents;
		// menuComponents
	} subWidgets;


	CjvxRealtimeViewer viewer_props;
	CjvxRealtimeViewer viewer_plots;

	configureQtAudioHost_features theHostFeatures;

	struct
	{
		jvxBool auto_start;
		jvxBool auto_stop;
		jvxBool skipStateSelected;
	} systemParams;

	jvxInt32 progress;
	jvxTimeStampData tstamp;

	CjvxCommandLine commLine;

	CjvxMainWindowControl theControl;
	CjvxQtEventLoop theEventLoop;

	jvxSize id_period_seq_ms;
	jvxSize id_rtview_period_maincentral_ms;
	jvxSize id_autostart_id;
	jvxSize id_autostop_id;
	jvxBool allowStart = false;

	jvxCallbackPrivate theCallback_saveconfig;
	JVX_PROPERTIES_CALLBACK_DECLARE(cb_save_config);

	jvxCallbackPrivate theCallback_shutdown;
	JVX_PROPERTIES_CALLBACK_DECLARE(cb_shutdown);

	jvxCallbackPrivate theCallback_restart;
	JVX_PROPERTIES_CALLBACK_DECLARE(cb_restart);

	jvxCallbackPrivate theCallback_loadconfig;
	JVX_PROPERTIES_CALLBACK_DECLARE(cb_load_config);

	jvxCallbackPrivate theCallback_exchg_property;
	JVX_PROPERTIES_CALLBACK_DECLARE(cb_xchg_property);

public:
	uMainWindow();

	~uMainWindow(void);

	// Project specific entry points
	/*
	jvxErrorType connect_specific();
	jvxErrorType bootup_specific();
	jvxErrorType shutdown_specific();
	jvxErrorType postbootup_specific();
	jvxErrorType pre_shutdown_specific();
	*/
	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_bootup.cpp */
	/* = = = = = = = = = = = = = = = = = */

	jvxErrorType initSystem(QApplication* hdlApp, char*[], int);
	void resetJvxReferences();

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow.cpp */
	/* = = = = = = = = = = = = = = = = = */
	void postMessage_outThread(const char* txt, jvxReportPriority prio);
	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification&, IjvxObject*, jvxPropertyCategoryType cat, 
		jvxSize propId , bool ctOnly, jvxSize offs, jvxSize num,
		const jvxComponentIdentification&, jvxPropertyCallPurpose callpurp)override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(
		jvxCBitField request, 
		jvxHandle* caseSpecificData, 
		jvxSize szSpecificDataoverride)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurp)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_select_on_config(const jvxComponentIdentification& tp)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_inactive_on_config(const jvxComponentIdentification& tp)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_active_on_config(const jvxComponentIdentification& tp)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_unselect_on_config(const jvxComponentIdentification& tp)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION forward_trigger_operation(
		jvxMainWindowController_trigger_operation_ids id_operation,
		jvxHandle* theData = 0)override;

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxCBitField event_mask, const char* description, jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSequencerElementType stp)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId)override;
	*/

	jvxErrorType rtv_get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* theReader, jvxHandle* ir, IjvxHost* theHost, CjvxRealtimeViewer& theViewer, jvxRealtimeViewerType configurationType);
	jvxErrorType rtv_put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* theReader, jvxHandle* ir, IjvxHost* theHost, const char* fName, int lineno, std::vector<std::string>& warnings, CjvxRealtimeViewer& theViewer, jvxRealtimeViewerType configurationType);

	virtual void report_widget_closed(QWidget* theClosedWidget) override;
	virtual void report_widget_specific(jvxSize id, jvxHandle* spec) override;

	virtual void report_sequencer_callback(jvxSize fId)override;
	virtual void report_sequencer_stopped() override;
	virtual void report_sequencer_error(const char* str1, const char* str2) override;
	virtual void report_sequencer_update_timeout() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION on_components_before_configure() override;

	/* = = = = = = = = = = = = = = = = = */
	/* = = = = = = = = = = = = = = = = = */

	virtual void updateAllWidgetsOnStateChange() override;
	virtual void postMessageError(const char* mess) override;

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_shutdown.cpp */
	/* = = = = = = = = = = = = = = = = = */
	void fatalStop(const char* str1, const char* str2);

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_config.cpp */
	/* = = = = = = = = = = = = = = = = = */
	
	jvxErrorType control_startSequencer();
	jvxErrorType control_stopSequencer();

	void closeEvent(QCloseEvent* event)override;

	void updateWindow_periodic();

	void setup_widgets();
	void finalize_widgets();
	void postbootup_widgets();

	void preshutdown_widgets();
	void shutdown_widgets();

	// =========================================================================
	// Callback from lower class hierarchies
	// =========================================================================
	virtual jvxErrorType boot_initialize_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_specify_slots_specific() override;
	virtual jvxErrorType boot_prepare_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_start_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_activate_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_terminate_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_deactivate_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_postprocess_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_stop_specific(jvxApiString* errloc) override;
	virtual void report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context) override;
	virtual void inform_config_read_start(const char* nmfile, jvxHandle* context) override;
	virtual void inform_config_read_stop(jvxErrorType resRead, const char* nmFile, jvxHandle* context) override;
	virtual void get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext) override;
	virtual void get_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data) override;
	virtual void put_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName) override;
	void put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext) override;
	void report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName)override;

	virtual jvxErrorType get_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id) override;
	virtual jvxErrorType set_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id) override;

	virtual void keyPressEvent(QKeyEvent* event) override;

	virtual void trigger_immediate_sequencerStep() override;
	virtual void trigger_threadChange_forward(CjvxReportCommandRequest* ptr) override;

	virtual void run_mainthread_triggerTestChainDone() override;
	virtual void run_mainthread_updateComponentList(jvxComponentIdentification cpId) override;
	virtual void run_mainthread_updateProperties(jvxComponentIdentification cpId) override;
	virtual void run_mainthread_updateSystemStatus() override;
	virtual void run_immediate_rescheduleRequest(const CjvxReportCommandRequest& request) override;
	virtual void report_error(jvxErrorType resError, const CjvxReportCommandRequest& request) override;

private:

	void updateWindow(jvxCBitField prio = ((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT));
	//void restartProperties();
	void updatePropertyViewer();
	void updatePropertyViewerFull();

	void report_internals_have_changed_outThread(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId, 
	bool onlyContent, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurp);
	void updateStateSequencer_outThread(jvxUInt64 event_mask, int sequenceId, int stepId, int queuetp);
	void on_request_startstop(jvxStartStopIntendedOperation intendedOperation);

	void load_icon(QLabel* lab, const std::string coretxt);

public slots:


	void trigger_openAudioConfiguration();
	void trigger_openAudioArgs();
	void trigger_openVideoConfiguration();

	void postMessage_inThread(QString txt, jvxReportPriority prio);

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_config.cpp */
	/* = = = = = = = = = = = = = = = = = */
	void saveConfigFile();
	void saveConfigFileAs();
	void openConfigFile();

	void bootDelayed();

	void buttonPushed_startstop();

	void report_internals_have_changed_inThread(jvxComponentIdentification, IjvxObject*, jvxPropertyCategoryType, jvxSize, bool, jvxComponentIdentification, jvxPropertyCallPurpose);
	void report_command_request_inThread(jvxCBitField, jvxHandle* , jvxSize);
	void request_command_inThread(CjvxReportCommandRequest* request);
	//void updateStateSequencer_inThread(jvxUInt64 event_mask, int sequenceId, int stepId, int queuetp);

	void trigger_test_chain_inThread();

	void close_app();
	
	void reopen_config();
	void run_autostop();

signals:

	void emit_postMessage(QString txt, jvxReportPriority prio);
	void emit_bootDelayed();
	void emit_report_internals_have_changed_inThread(jvxComponentIdentification, IjvxObject*, jvxPropertyCategoryType, jvxSize, bool, jvxComponentIdentification, jvxPropertyCallPurpose);
	void emit_report_command_request(jvxCBitField, jvxHandle*, jvxSize);
	void emit_request_command(CjvxReportCommandRequest* request);
	void emit_triggerViewer();
	void emit_close_app();
	void emit_autoStart();
	void emit_autoStop();
	void emit_reopen_config();

	void emit_saveConfigFile();
	void emit_openConfigFile();

	void emit_triggerTestChain();

};

#endif
