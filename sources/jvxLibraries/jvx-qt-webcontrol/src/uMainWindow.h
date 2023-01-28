#ifndef __UMAINWINDOW_H__
#define __UMAINWINDOW_H__

#include "ui_mainApplication.h"
#include "jvx.h"
#include "commandline/CjvxCommandLine.h"
#include <QtGui/QCloseEvent>
//#include "CjvxQtWebControlWidget.h"
#include <QDialog>
#include "jvxQtWebControlWidget.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "CjvxAccessProperties_Qhttp.h"
#include "interfaces/qt-factory-host/configureQtFactoryHost_features.h"
#include "allHostsStatic_common.h"
#include "CjvxAppFactoryHostProduct.h"
#include "jvx_mainwindow_control.h"
#include "jvx_eventloop_object.h"
#include "jvx-net-helpers.h"

#define JVX_QT_COLOR_WARNING Qt::blue
#define JVX_QT_COLOR_ERROR Qt::red
#define JVX_QT_COLOR_SUCCESS Qt::green
#define JVX_QT_COLOR_INFORM Qt::black
#define JVX_QT_DERIVE_FROM_TYPE QMainWindow

#define JVX_NUM_HEARTBEAT_MAX 2

#define JVX_WEBCONTROL_AUTO_CONNECT "JVX_WEBCONTROL_AUTO_CONNECT"
#define JVX_WEBCONTROL_AUTO_OBSERVE "JVX_WEBCONTROL_AUTO_OBSERVE"
#define JVX_WEBCONTROL "JVX_WEBCONTROL"

// #define JVX_WEBCONTROL_UDP_VERBOSE

class jvxSyncRemoteHttp
{
protected:
	class onePartnerComponent
	{
	public:
		jvxComponentIdentification tp;
		CjvxAccessProperties_Qhttp* theHttpControl;
		jvxSize uId;
		std::string description;
		std::string descriptor;
		std::string tag;
		jvxBool temp_flag;
		jvxState fromState;
		jvxState toState;
		jvxBool removeMe;

		onePartnerComponent()
		{
			tp = JVX_COMPONENT_UNKNOWN;
			theHttpControl = NULL;
			uId = JVX_SIZE_UNSELECTED;
			temp_flag = false;
			fromState = JVX_STATE_NONE;
			toState = JVX_STATE_NONE;
			removeMe = false;
			tag = "unsupported";
		};
	};

	std::map< jvxComponentIdentification, onePartnerComponent> registeredPartners;
	mainWindow_UiExtension_hostfactory* theMainWidget;
	IjvxHttpApi* httpRef;
	jvxState myState;
	jvxSequencerStatus seqStat;
	jvxSize heartbeatState;
	jvxInt16 auto_connect;
	jvxInt16 auto_observe;
	IjvxFactoryHost* hFHost = nullptr;
public:

	jvxSyncRemoteHttp() 
	{
		theMainWidget = NULL;
		httpRef = NULL;
		myState = JVX_STATE_NONE;
		seqStat = JVX_SEQUENCER_STATUS_NONE;
		hFHost = nullptr;
	};

	jvxErrorType initialize(mainWindow_UiExtension_hostfactory* theMainWidget, IjvxHttpApi* httpRef,
		IjvxFactoryHost* hFHostRef);
	jvxErrorType terminate();

	jvxErrorType sync_remote(CjvxJsonElementList& lst, 
		jvxBool& is_system_state_transitio,
		jvxBool& is_state_transition_process);

	jvxErrorType sync_remote_disconnect();
	jvxErrorType version_to_tag(CjvxJsonElementList& lst, std::string &version_tag);
	jvxErrorType sync_out();
	jvxErrorType match_new_old_lists(std::map< jvxComponentIdentification, onePartnerComponent>& receivedList,
		jvxBool& is_system_state_transition);
};

typedef enum
{
	JVX_WEBSOCK_STATE_NONE,
	JVX_WEBSOCK_STATE_CONNECTING_PENDING,
	JVX_WEBSOCK_STATE_CONNECTING,
	JVX_WEBSOCK_STATE_CONNECTED,
	JVX_WEBSOCK_STATE_SETUP_PENDING,
	JVX_WEBSOCK_STATE_READY,
	JVX_WEBSOCK_STATE_WAIT_FOR_CLOSE
} jvxStateWebSockSetup;

class uMainWindow: public QMainWindow, public Ui::MainWindow, 
	public IjvxReport, 
	public IjvxQtWebControlWidget_report, 
	public IjvxQtSpecificWidget_report, 
	public IjvxMainWindowController_report,
	public IjvxMainWindowControl_connect,
	public IjvxHttpApi_httpreport,
	public jvxSyncRemoteHttp,
	public IjvxHttpApi_wsreport,
	public jvxWebSocket_properties_install,
	//public jvxHttp_sequencer_startstop,
	public HjvxPropertyStreamUdpSocket_report,
	public JVX_APP_FACTORY_HOST_PRODUCT_CLASSNAME
{
	typedef enum
	{
		JVX_CONNECTION_STATE_INIT,
		JVX_CONNECTION_STATE_INIT_VERSION,
		JVX_CONNECTION_STATE_UPDATE,
		JVX_CONNECTION_STATE_OPERATE,
		JVX_CONNECTION_STATE_STOPPED,
		JVX_CONNECTION_STATE_ERROR
	} jvxConnectionState;

	Q_OBJECT
private:
	jvxBool  bootupComplete;
	CjvxQtEventLoop theEventLoop;
	struct
	{
		struct
		{
			mainWindow_UiExtension_hostfactory* theWidget;
		} main;
	} subWidgets;

	IjvxQtWidgetFrameDialog* myHttpWidget_frame;
	IjvxQtWebControlWidget* myHttpWidget_class;
	QWidget* myHttpWidget_wid;
	QDialog* myHttpWidget_dialog;
	IjvxConfigurationExtender_report* myHttpWidget_cfg;
	jvxSize myHttpWidget_trig_id;
	jvxSize myHttpWidget_autoc_id;
	jvxSize myHttpWidget_autoo_id;

	jvxComponentIdentification tp;
	CjvxAccessProperties_Qhttp* theHttpControl;

	configureQtFactoryHost_features theHostFeatures;
	//jvxBool ws_started;
	jvxBool viewer_started;
	IjvxHttpApi* httpRef;

	QTimer* runTimer;
	CjvxCommandLine commLine;

	CjvxMainWindowControl theControl;

	// For system specific requests
	jvxSize uIdGroup;
	std::map<jvxSize, std::string> requestedGets;
	std::map<jvxSize, std::string> requestedPosts;
	jvxConnectionState theConnState;
	jvxBool reportedConnect;

	jvxConfigWeb theWebCfg;

	struct propertiesOneComponent
	{
		jvxSize count;
		std::map<jvxSize, jvxOneRemotePropertySpecification> registeredProperties;
		std::map<jvxSize, jvxOneRemotePropertySpecification> propertiesToRegister;
		propertiesOneComponent()
		{
			count = 0;
		};
	};

	struct
	{
		jvxSize idWebSock;
		jvxStateWebSockSetup stateWebSock;
		struct
		{
			std::map<jvxComponentIdentification, propertiesOneComponent> registeredProperties;

			// We need this lock at that place where properties arrive within the UDP threads - these are not in sync to QT
			JVX_MUTEX_HANDLE safeAccess;
		} mt;
		std::map<jvxComponentIdentification, propertiesOneComponent> tobereleasedProperties;
		jvxSize theWSId;
	}webSockRuntime;

	std::string version_info;
	std::string connection_info;
	jvxSize pendigSystemUpdates;

	jvxBool initial_setup_request;

	struct
	{
		std::map<jvxInt32, HjvxPropertyStreamUdpSocket*> theUdpSockets;
		jvxSize uId;
	} high_prio_transfer;

public:
	uMainWindow();
	~uMainWindow();

	void fatalStop(const char* str1, const char* str2);
	
	jvxErrorType trigger_config_websocket(jvxSize ticks_base_cnt_msec, jvxSize ping_cnt);

	jvxErrorType initSystem(QApplication* hdlApp, char*[], int);
	void closeEvent(QCloseEvent* event)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize off, jvxSize num,
		const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
									     jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart,
									     jvxBool onlyContent,
									     jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo,
									     jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(
		jvxCBitField request, 
		jvxHandle* caseSpecificData = NULL,
		jvxSize szSpecificData = 0) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;
	// =============================================================================

	virtual jvxErrorType report_error()override;

	// =============================================================================
	// IjvxMainWindowControl
	virtual jvxErrorType JVX_CALLINGCONVENTION forward_trigger_operation(
		jvxMainWindowController_trigger_operation_ids id_operation,
		jvxHandle* theData = 0) override;

	// =============================================================================
	
	virtual void report_widget_closed(QWidget* theClosedWidget) override;
	virtual void report_widget_specific(jvxSize id, jvxHandle* spec) override;

	// =============================================================================

	//jvxErrorType configureFromFile(std::string fName, jvxFlagTag flagtag);
	//jvxErrorType configurationToFile(std::string& fName, jvxFlagTag flagtag);
	void openConfigFile_core(const char* fName, jvxBool allowOverlayOnly_config);

	void updateWindow(jvxCBitField prio /* = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT*/);

	void postMessage_outThread(const char* txt, QColor col);

	// Entries for higher level functions
	virtual jvxErrorType boot_initialize_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_specify_slots_specific() override;
	virtual jvxErrorType boot_prepare_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_start_specific(jvxApiString* errloc) override;
	virtual jvxErrorType boot_activate_specific(jvxApiString* errloc) override;

	virtual jvxErrorType shutdown_terminate_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_deactivate_specific(jvxApiString* errloc) override;
	virtual jvxErrorType shutdown_postprocess_specific(jvxApiString* errloc)override;
	virtual jvxErrorType shutdown_stop_specific(jvxApiString* errloc) override;


	virtual void report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context) override;
	virtual void inform_config_read_start(const char* nmfile, jvxHandle* context) override;
	virtual void inform_config_read_stop(jvxErrorType resRead, const char* nmfile, jvxHandle* context) override;

	virtual void get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext) override;
	virtual void get_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data) override;
	virtual void put_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName) override;
	virtual void put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext) override;

	virtual void report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName) override;

	jvxErrorType get_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id)override;
	jvxErrorType set_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id)override;

	virtual jvxErrorType complete_transfer_http(jvxSize uniqueIdGroup, jvxSize uniqueId_ingroup, const char* response, jvxHandle* priv, jvxCBitField reqId) override;
	virtual jvxErrorType failed_transfer_http(jvxSize uniqueIdGroup, jvxSize uniqueId_ingroup, jvxErrorType errRes, jvxHandle* priv, jvxCBitField reqId)override;

	virtual jvxErrorType websocket_connected(jvxSize uId, jvxHandle* priv)override;
	virtual jvxErrorType websocket_error(jvxSize uId, jvxHandle* priv, const char* descrErr) override;
	virtual jvxErrorType websocket_disconnected(jvxSize uId, jvxHandle* priv) override;
	virtual jvxErrorType websocket_data_binary(jvxSize uId, jvxByte* payload, jvxSize szFld, jvxHandle* priv) override;
	virtual jvxErrorType register_properties(jvxComponentIdentification tp, jvxOneRemotePropertySpecification* props, jvxSize numProps) override;
	virtual jvxErrorType unregister_properties(jvxComponentIdentification tp) override;

	jvxErrorType trigger_setup_next_prop();
	jvxErrorType trigger_release_props(jvxComponentIdentification tp);
	void request_status();

	virtual jvxErrorType trigger_seq_start();
	virtual jvxErrorType trigger_seq_stop();

	void stop_timer_viewer();
	void update_heartbeat();
	void http_disconnect_finalize();

	jvxErrorType try_attach_udp_socket(jvxInt32 port, jvxSize uniqueId_property);
	jvxErrorType try_detach_udp_socket(jvxInt32 port, jvxSize uniqueId_property);

	virtual jvxErrorType report_incoming_packet(jvxByte* fld, jvxSize sz, HjvxPropertyStreamUdpSocket* resp_socket) override;
	virtual jvxErrorType report_connection_error(const char* errorText) override;

public slots:

	void close_app();
	void open_preferences();
	void open();
	void http_connect();

	void saveConfigFile();
	void saveConfigFileAs();
	void openConfigFile();
	void postMessage_inThread(QString txt, QColor col);
	void trigger_ws_startstop();
	void trigger_viewer_start();
	void trigger_viewer_stop();
	void trigger_seq_startstop();
	void trigger_seq_break();
	void periodic_timeout();
	void boot_delayed();

	void slot_setup_websocket();
	void slot_state_update(QString strq);

signals:
	void emit_postMessage(QString txt, QColor col);
	void emit_close_app();
	void emit_boot_delayed();
	void remit_http_connect();

	void emit_setup_websocket();
	void emit_state_update(QString strq);

};

#endif
