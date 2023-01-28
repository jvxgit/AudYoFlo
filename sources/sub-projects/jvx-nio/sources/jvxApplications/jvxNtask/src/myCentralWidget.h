#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"
//#include "jvxQtMixMatrixWidget.h"

#include "MjvxCentralWidgetMacros.h"
#include "jvxQtCentralWidgets.h" //jvx_property_tree_widget.h"

#include "mainCentral_host_ww.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#define JVX_CHECK_CONNECTIONS

class myCentralWidget: public mainCentral_host_ww, 
	public IjvxConfigurationExtender_report, 
	public IjvxQtSpecificWidget_report, 
	public Ui::Form_myMainWidget
{

private:

	Q_OBJECT

		JVX_MAIN_CENTRAL_DECLARE_TREE_WIDGET(propertyTreeWidget);
public:

	myCentralWidget(QWidget* parent);
	~myCentralWidget(){};

	virtual void init_submodule(IjvxHost* theHost) override;
	virtual void terminate_submodule() override;

	virtual void inform_update_window(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT) override;

	virtual void inform_update_window_periodic() override;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;


	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_ext(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration_ext(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename, jvxInt32 lineno) override;

	virtual void inform_config_read_complete(const char* fName) override;

	//jvxSize request_number_flat_slots_init() override;

	//jvxSize transform_type_location_to_flat_slot(const jvxComponentIdentification& tp) override;

	virtual void inform_sequencer_about_to_stop() override;

	virtual void inform_about_to_shutdown() override;

	virtual void report_widget_closed(QWidget* theClosedWidget) override;
	virtual void report_widget_specific(jvxSize id, jvxHandle* spec) override;
signals:

public slots:
	
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
