#ifndef __UMAINWINDOW_H__
#define __UMAINWINDOW_H__

#include "ui_jvxPacketFilterUi.h"
#include "jvx.h"
#include "commandline/CjvxCommandLine.h"

#include "jvx_connect_client_st.h"

#include <qtimer.h>

//#include <QtWidgets/QDockWidget>
#include "CjvxPacketCaptureEndpoint.h"
#include "CjvxOneQueueGateway.h"
#include "CjvxAppFactoryHostBase.h"

#define JVX_NUMBER_ENTRIES_QUEUE 100
#define JVX_MTU_SIZE_MAX 1550
#define JVX_DEFAULT_TIMEOUT_PACKET_DELAY_MS 20
#define JVX_PCAP_NUM_FLDS_QUEUE 5

class uMainWindow: public QMainWindow, public Ui::MainWindow, public IjvxReport, 
	public JVX_APP_FACTORY_HOST_CLASSNAME
{

	Q_OBJECT
private:
	typedef enum
	{
		JVX_PACKETFILTER_LEFT,
		JVX_PACKETFILTER_RIGHT
	} laneAssociation;

	struct oneLane
	{
		laneAssociation association;
		uMainWindow* thisisme;
	};

	struct oneFilterInstance
	{
		IjvxObject* theRule_obj = NULL;
		IjvxPacketFilterRule* thRule_core;
		IjvxProperties* theRule_properties;
	} ;

	struct
	{
		jvx_connect_client_family* theSocketFamily;

		jvx_connect_client_if* deviceLeft;
		jvxSize idLeft;
		JVX_MUTEX_HANDLE saveSendLeft;

		jvx_connect_client_if* deviceRight;
		jvxSize idRight;
		JVX_MUTEX_HANDLE saveSendRight;

		jvxBool started;

		jvxSize num;

		oneLane laneLeft;
		oneLane laneRight;
	} devices;

	struct
	{
		CjvxOneQueueGateway leftrightqueue;
		CjvxOneQueueGateway rightleftqueue;
	} queues;

	jvx_connect_client_callback_type myCallbacks;

	CjvxPacketCaptureEndpoint epLeft;
	CjvxPacketCaptureEndpoint epRight;

	jvxTimeStampData tStamp;

	QTimer* myTimer;

	struct
	{
		jvxSize numAvailableFilters;
		jvxSize idSelect;
		std::list<oneFilterInstance> fRules;

	} filters;
	CjvxCommandLine commLine;
public:

	uMainWindow();
	~uMainWindow(void);
	
	jvxErrorType initialize(QApplication* hdlApp, char*[], int);

	static jvxErrorType callback_report_startup_complete(jvxHandle* privateData, jvxSize ifidx);
	static jvxErrorType callback_report_shutdown_complete(jvxHandle* privateData, jvxSize ifidx);
	static jvxErrorType callback_provide_data_and_length(jvxHandle* privateData, jvxSize channelId, char**, jvxSize*, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);
	static jvxErrorType callback_report_data_and_read(jvxHandle* privateData, jvxSize channelId, char*, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);
	static jvxErrorType callback_report_error(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx);
	static jvxErrorType callback_report_specific(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx);
	static jvxErrorType callback_report_unsuccesful_outgoing(jvxHandle* privateData, jvx_connect_fail_reason reason, jvxSize ifidx);
	static jvxErrorType callback_report_connect_outgoing(jvxHandle* privateData, jvxSize ifidx);
	static jvxErrorType callback_report_disconnect_outgoing(jvxHandle* privateData, jvxSize ifidx);

	void reportOneInPackage(jvxHandle* buf, jvxSize szFld, laneAssociation assoc);


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
	virtual void inform_config_read_stop(jvxErrorType resRead, const char* nmfile, jvxHandle* context)override;

	virtual void get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)override;
	virtual void get_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data) override;
	virtual void put_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName) override;
	virtual void put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext) override;

	virtual void report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification&, IjvxObject*, jvxPropertyCategoryType cat,
		jvxSize propId, bool ctOnly, jvxSize offs, jvxSize num, const jvxComponentIdentification&, jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(
		jvxCBitField request, 
		jvxHandle* caseSpecificData, 
		jvxSize szSpecificData) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;

	void closeEvent(QCloseEvent* event);
	
	jvxErrorType send_to_left(jvxByte* fld, jvxSize fldSize);
	jvxErrorType send_to_right(jvxByte* fld, jvxSize fldSize);

	jvxErrorType filter_packet_left_to_right(jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_add_tag, jvxSize if_tag_add, jvxBool* packet_remove);
	jvxErrorType filter_packet_right_to_left(jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_add_tag, jvxSize if_tag_add, jvxBool* packet_remove);
private:

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_ui.cpp */
	/* = = = = = = = = = = = = = = = = = */
	void fatalStop(const char* str1, const char* str2);
	void updateWindow();

public slots:

	void updateWindowPeriodic();
	void select_cb_ifs_left(int);
	void select_cb_ifs_right(int);
	void pushed_start();
	void select_frule(int);
	void add_rule();
	void rem_rule();

signals:


};

#endif
