#ifndef __CJVXQTWEBCONTROLWIDGET_H__
#define __CJVXQTWEBCONTROLWIDGET_H__
#include "CjvxWebHostControl.h"
#include "ui_jvx-qt-webcontrol-widget.h"

#include "jvxQtWebControlWidget.h"

class CjvxQtWebControlWidget: public QWidget, public Ui::directWebControlWidget, public CjvxWebHostControl_hooks, 
	public IjvxQtWebControlWidget, public IjvxConfigurationExtender_report, public IjvxHttpApi_wsreport
{
private:
	jvxSize myWsId;

	Q_OBJECT

	CjvxWebHostControl* myControl;
	IjvxQtWebControlWidget_report* bwdRef;
	IjvxQtSpecificWidget_report* bwdMainUi;
	IjvxHost* theHost;

	virtual void init(IjvxFactoryHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL) override;
	virtual void getMyQWidget(QWidget** retWidget, jvxSize id = 0) override;
	virtual void terminate() override;
	virtual void activate() override;
	virtual void deactivate() override;
	virtual void processing_started() override;
	virtual void processing_stopped() override;
	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT) override;
	virtual void update_window_periodic() override;

	virtual void webcontrol_update_window() override;
	virtual void webcontrol_print_text(const std::string& txt) override;
	virtual void webcontrol_print_text_return(const std::string& txt) override;
	virtual void webcontrol_report_error() override;

	virtual jvxErrorType request_http_api(IjvxHttpApi** theHttpRef) override;
	virtual jvxErrorType return_http_api(IjvxHttpApi* theHttpRef) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_ext(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration_ext(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename, jvxInt32 lineno) override;

	virtual jvxErrorType request_configuration_ext_report(IjvxConfigurationExtender_report** theConfigExtenderReport) override;
	virtual jvxErrorType return_configuration_ext_report(IjvxConfigurationExtender_report* theConfigExtenderReport) override;
	virtual jvxErrorType set_report(IjvxQtWebControlWidget_report* ptr)override;

	virtual jvxErrorType websocket_connected(jvxSize uId, jvxHandle* priv)override;
	virtual jvxErrorType websocket_error(jvxSize uId, jvxHandle* priv, const char* reason)override;
	virtual jvxErrorType websocket_disconnected(jvxSize uId, jvxHandle* priv)override;
	virtual jvxErrorType websocket_data_binary(jvxSize uId, jvxByte* payload, jvxSize szFld, jvxHandle* priv) override;

public:
	CjvxQtWebControlWidget(QWidget* parent);
	~CjvxQtWebControlWidget();
	
	virtual void hideEvent(QHideEvent *)override;

public slots:

	void trigger_get();
	void trigger_put();
	void trigger_post();
	void set_address();
	void set_port();

	void trigger_openws();

	void edit_get_request();
	void edit_put_request();
	void edit_post_request();
	void edit_wsocket_request();
	void edit_put_content();
	void edit_post_content();

	void set_timeout_sec();

signals:
	
};

#endif
