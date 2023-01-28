#ifndef __UMAINWINDOW_H__
#define __UMAINWINDOW_H__

#include "ui_mainApplication.h"
#include "jvx.h"
#include "messagesOutput.h"
#include "realtimeViewer.h"
#include <QtWidgets/QDockWidget>
#include "uMainWindow_defines.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "commandline/CjvxCommandLine.h"
#include "interfaces/qt-develop-host/configureQtDevelopHost_features.h"
#include "allHostsStatic_common.h"
#include "jvxQtHostWidgets.h"
#include "CjvxAppHostProduct.h"
#include "jvx_mainwindow_control.h"
#include "jvx_eventloop_object.h"

class QDockWidget_ext;
class mainCentral_local;

class uMainWindow : public QMainWindow, public Ui::MainWindow,
	public IjvxReport, public IjvxReportOnConfig,
	public IjvxQtSequencerWidget_report,
	public IjvxQtSpecificWidget_report,
	public IjvxMainWindowControl_connect,
	public IjvxMainWindowController_report,
	public jvx_menu_host_bridge_components_report,
	public JVX_APPHOST_PRODUCT_CLASSNAME
{
  	friend QDockWidget_ext;

	Q_OBJECT
private:

	struct
	{
		struct
		{
			QDockWidget_ext* theWidgetD;
			messagesOutput* theWidget;
			bool added;
		} messages;

		/*
		struct
		{
			QDockWidget_ext* theWidgetD;
			sequenceEditor* theWidget;
			bool added;
		} sequences;
		*/
		struct
		{
			struct
			{
				QDockWidget_ext* theWidgetD;
				realtimeViewer* theWidget;
				bool added;
			} props;

			struct
			{
				QDockWidget_ext* theWidgetD;
				realtimeViewer* theWidget;
				bool added;
			} plots;
		} realtimeViewer;

		struct
		{
			QDockWidget_ext* theWidgetD;
			IjvxQtConnectionWidget* theWidget;
			QWidget* theQWidget;
			bool added;
		} connections;

		struct
		{
			QDockWidget_ext* theWidgetD;
			IjvxQtSequencerWidget* theWidget;
			QWidget* theQWidget;
			bool added;
		} sequencer;

		/*		struct
		{
			QDockWidget* theWidgetD;
			messagesOutput* theWidget;
			bool added;
		} messages2;
*/
		struct
		{
			mainWindow_UiExtension_host* theWidget;
			QTimer* timer;
		} main;

		jvxBool bootupComplete;

		struct
		{
			jvxErrorType returnVal;
			JVX_THREAD_ID theQtThreadId;
		} qthost;

		IjvxQtMenuComponentsBridge* bridgeMenuComponents;
	} subWidgets;

	struct
	{
		jvxBool auto_start;
		jvxBool skipStateSelected;
		jvxBool encryptionConfigFiles;
	} systemParams;


	configureQtDevelopHost_features theHostFeatures;

	CjvxCommandLine commLine;
	CjvxMainWindowControl theControl;
	CjvxQtEventLoop theEventLoop;
	
	jvxComponentIdentification tpAll[JVX_COMPONENT_ALL_LIMIT];

	jvxSize id_autoc_id;
	jvxSize id_rtview_period_props_ms;
	jvxSize id_rtview_period_plots_ms;
	jvxSize id_period_seq_ms;
	jvxSize id_rtview_period_maincentral_ms;
	jvxSize id_skip_ssel_id;
	jvxSize id_enc_config_id;

	jvxBool lockKeyShortcuts = false;

public:
	uMainWindow();

	~uMainWindow(void);

	virtual jvxErrorType JVX_CALLINGCONVENTION get_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION set_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id) override;

	jvxErrorType connect_specific();
	jvxErrorType boot_negotiate_specific();
	jvxErrorType shutdown_specific();
	jvxErrorType postbootup_specific();
	void bootDelayed_specific();

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_bootup.cpp */
	/* = = = = = = = = = = = = = = = = = */

	jvxErrorType initSystem(QApplication* hdlApp, char*[], int);
	//void resetJvxReferences();

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow.cpp */
	/* = = = = = = = = = = = = = = = = = */
	void postMessage_outThread(const char* txt, QColor col);
	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification&, 
		IjvxObject*, jvxPropertyCategoryType cat,
		jvxSize propId , bool ctOnly,
		jvxSize offset_start, jvxSize numElements,const jvxComponentIdentification&,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(
		jvxCBitField request, 
		jvxHandle* caseSpecificData, 
		jvxSize szSpecificData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
                                                                             jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
									     jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo,
									     jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context)override;


	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_active_on_config(const jvxComponentIdentification& tp)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_inactive_on_config(const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_select_on_config(const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_unselect_on_config(const jvxComponentIdentification& tp) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc) override;
	// ===========================================================
	// IjvxMainWindowControl
	// ===========================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION forward_trigger_operation(
		jvxMainWindowController_trigger_operation_ids id_operation,
		jvxHandle* theData = 0) override;

	/* = = = = = = = = = = = = = = = = = */
	/* = = = = = = = = = = = = = = = = = */

	virtual void report_widget_closed(QWidget *)override;
	virtual void report_widget_specific(jvxSize, jvxHandle *) override;

	/* = = = = = = = = = = = = = = = = = */
	/* = = = = = = = = = = = = = = = = = */

	virtual void report_sequencer_callback(jvxSize fId) override;
	virtual void report_sequencer_stopped() override;
	virtual void report_sequencer_error(const char* str1, const char* str2) override;
	virtual void report_sequencer_update_timeout() override;

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_shutdown.cpp */
	/* = = = = = = = = = = = = = = = = = */
	//jvxErrorType terminateHost();
	//jvxErrorType terminateSystem();
	void fatalStop(const char* str1, const char* str2);

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_config.cpp */
	/* = = = = = = = = = = = = = = = = = */
	//jvxErrorType configureFromFile(std::string fName, jvxFlagTag flagtag);
	//jvxErrorType configurationToFile(std::string& fName, jvxFlagTag flagtag);
	void openConfigFile_core( const char* fName, jvxBool allowOverlayOnly_config);

	void dockWidgetReturned(Qt::DockWidgetArea area);

	jvxErrorType control_startSequencer();
	jvxErrorType control_stopSequencer();
	jvxErrorType control_contSequencer();

	void sequencer_stopped();

	void closeEvent(QCloseEvent* event)override;

	void reportSequencerError_outThread(std::string, std::string);

	void updateSingleAction(QAction* act);

	// Start and stop the dock widgets
	void setup_dock_widgets();
	void finalize_dock_widgets();
	void postbootup_dock_widgets();

	void pre_release_dock_widgets();
	void release_dock_widgets();

	// Load and unload the host
	//void load_host();
	//void unload_host();

	jvxErrorType boot_initialize_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_specify_slots_specific() override;
	jvxErrorType boot_prepare_specific(jvxApiString* errloc)override;
	jvxErrorType boot_start_specific(jvxApiString* errloc)override;
	jvxErrorType boot_activate_specific(jvxApiString* errloc)override;
	jvxErrorType shutdown_terminate_specific(jvxApiString* errloc)override;
	jvxErrorType shutdown_deactivate_specific(jvxApiString* errloc)override;
	jvxErrorType shutdown_postprocess_specific(jvxApiString* errloc)override;
	jvxErrorType shutdown_stop_specific(jvxApiString* errloc)override;
	void report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context)override;
	void inform_config_read_start(const char* nmfile, jvxHandle* context)override;
	void inform_config_read_stop(jvxErrorType resRead, const char* nmfile, jvxHandle* context)override;
	void get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)override;
	void get_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data)override;
	void put_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName)override;
	void put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)override;
	virtual void report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName)override;

private:

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_ui.cpp */
	/* = = = = = = = = = = = = = = = = = */

	void updateWindow(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	void setTabPositionRight(int idTab);

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_menu.cpp */
	/* = = = = = = = = = = = = = = = = = */

	void setup_ext_menu_system();
	void free_ext_menu_system();

	/*void menuUpdate_oneComponentType(QMenu* lstNames, QMenu* lstStates, QMenu* lstProperties, QMenu* lstCommands,
		QMenu* lstConditions, QAction* lstInformation, QMenu* lstSlots, const jvxComponentIdentification& tp, jvxBool isPriCp);

	void menuUpdate_tools(QMenu* lstNames);
	*/
	void report_internals_have_changed_outThread(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurp);

	void arrangeTabsRightDock();

	jvxErrorType register_action_widget(const std::string& descr, IjvxQtSpecificWidget_base* widget, IjvxQtWidgetFrameDialog* fwidget);
	jvxErrorType unregister_action_widget(const std::string& descr, IjvxQtSpecificWidget_base* widget, IjvxQtWidgetFrameDialog* fwidget);

	virtual void updateAllWidgetsOnStateChange() override;
	virtual void postMessageError(const char* mess) override;

public slots:

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_ui.cpp */
	/* = = = = = = = = = = = = = = = = = */

	void triggered_viewMessages();
	void triggered_viewParameters();

	void triggered_viewSequencer();

	void triggered_viewRealtime_props();
	void triggered_viewRealtime_plots();

	void triggered_skipStateSelected();
	void triggered_toggleCryptConfig();

	void triggered_viewConnections();

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_menu.cpp */
	/* = = = = = = = = = = = = = = = = = */



	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow.cpp */
	/* = = = = = = = = = = = = = = = = = */

	void postMessage_inThread(QString txt, QColor col);

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_config.cpp */
	/* = = = = = = = = = = = = = = = = = */
	void saveConfigFile();
	void saveConfigFileAs();
	void openConfigFile();

	void bootDelayed();

	void report_internals_have_changed_inThread(jvxComponentIdentification, IjvxObject* , jvxPropertyCategoryType, jvxSize, bool, jvxComponentIdentification, jvxPropertyCallPurpose);
	void report_command_request_inThread(
		jvxCBitField request, 
		jvxHandle* caseSpecificData, 
		jvxSize szSpecificData);

	void request_command_inThread(CjvxReportCommandRequest* request);

	void updateWindowPeriodic();


	void close_app();
	void reopen_config(QString fName, bool allowOverlayOnly_config);

	void reportSequencerError_inThread(QString, QString);


signals:
	void emit_postMessage(QString txt, QColor col);
	void emit_bootDelayed();
	void emit_report_internals_have_changed_inThread(jvxComponentIdentification, IjvxObject*, jvxPropertyCategoryType, jvxSize, bool , 
		jvxComponentIdentification, jvxPropertyCallPurpose);
	void emit_close_app();
	void emit_report_command_request(
		jvxCBitField, 
		jvxHandle*, jvxSize);
	void emit_request_command(CjvxReportCommandRequest* request);
	void emit_reportSequencerError(QString, QString);
	void emit_reopen_config(QString fName, bool allowOverlayOnly_config);


};

#endif
