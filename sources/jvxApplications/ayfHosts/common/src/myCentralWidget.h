#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"  

#ifdef JVX_COMPILE_REMOTE_CONTROL
#include "mainCentral_hostfactory_ww_http.h"
#define JVX_MAINCENTRAL_CLASSNAME mainCentral_hostfactory_ww_http
#else
#include "mainCentral_host_ww.h"
#define JVX_MAINCENTRAL_CLASSNAME mainCentral_host_ww
//#include "templates/jvxHosts/qt-host-ui-extensions/mainCentral.h"
#endif

#ifndef JVX_COMPILE_REMOTE_CONTROL
#include "MjvxCentralWidgetMacros.h"
#include "jvxQtCentralWidgets.h" //jvx_property_tree_widget.h"
#endif

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#define JVX_CHECK_CONNECTIONS

class myCentralWidget: 
	public JVX_MAINCENTRAL_CLASSNAME,
	public IjvxConfigurationExtender_report, 
#ifndef JVX_COMPILE_REMOTE_CONTROL
	public IjvxQtSpecificWidget_report,
#endif
	public Ui::Form_myMainWidget
{
	IjvxAccessProperties* thePropRef_host;

#ifndef JVX_COMPILE_REMOTE_CONTROL

	JVX_MAIN_CENTRAL_DECLARE_TREE_WIDGET(propertyTreeWidget);/* tab_generic */	

#endif

	IjvxAccessProperties* thePropRef_loc;

private:

	Q_OBJECT


public:

	myCentralWidget(QWidget* parent);
	~myCentralWidget(){};

	void inform_internals_have_changed(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId,
			bool onlyContent, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose purpose) override;

#ifdef JVX_COMPILE_REMOTE_CONTROL
	virtual void init_submodule(IjvxFactoryHost* theHost) override;
#else
	virtual void init_submodule(IjvxHost* theHost) override;
#endif
	virtual void terminate_submodule() override;

	virtual void inform_update_window(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT) override;

	virtual void inform_update_window_periodic() override;

	virtual void inform_select(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_ext(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration_ext(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename, jvxInt32 lineno) override;

	virtual void inform_config_read_complete(const char* fName) override;

#ifdef JVX_COMPILE_REMOTE_CONTROL

	virtual jvxErrorType report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr) override;
	virtual jvxErrorType report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr) override;

	virtual void process_websocket_binary(jvxSize userData, jvxByte* payload, jvxPropertyTransferPriority prio) override;

#else

	virtual void report_widget_closed(QWidget* theClosedWidget) override;
	virtual void report_widget_specific(jvxSize id, jvxHandle* spec) override;
#endif

	virtual void inform_sequencer_about_to_start() override; 

	virtual void inform_sequencer_stopped() override;

	virtual void inform_about_to_shutdown() override;

	virtual jvxErrorType reportPropertyGet(const char *, const char *, jvxHandle *, jvxSize, jvxSize, jvxDataFormat, jvxErrorType res) override;
	virtual jvxErrorType reportPropertySet(const char* tag, const char* descror, jvxSize groupid, jvxErrorType res) override;

signals:

public slots:
	
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
