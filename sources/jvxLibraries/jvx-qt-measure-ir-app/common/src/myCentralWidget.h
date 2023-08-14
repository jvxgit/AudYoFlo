#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"
//#include "jvxQtMixMatrixWidget.h"

#include "MjvxCentralWidgetMacros.h"
#include "mainCentral_host_ww.h"
#include "jvxQtAcousticMeasurement.h"
#include "jvxQtCentralWidgets.h" //jvx_property_tree_widget.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE { 
#endif

#define JVX_CHECK_CONNECTIONS

class myCentralWidget: public mainCentral_host_ww, 
	public IjvxQtSpecificWidget_report, public Ui::Form_myMainWidget
{
	jvxBool measureIrActive = false;
	IjvxAccessProperties* thePropRef_measureIrNode = nullptr;

	jvxBool channelAnnounceActive = false;
	IjvxAccessProperties* thePropRef_channelAnnounceNode = nullptr;	

	// ======================================================================

	IjvxQtSpecificHWidget* widget_measure_ctrl = nullptr;
	IjvxQtSpecificHWidget_wwrapper_si* widget_measure_ctrl_wwrapper = nullptr;
	IjvxQtSpecificHWidget_config_si* widget_measure_ctrl_config = nullptr;

	QWidget* widget_measure_ctrl_qt = nullptr;

	JVX_MAIN_CENTRAL_DECLARE_TREE_WIDGET(propertyTreeWidget);

	IjvxConfigurationExtender* cfgExtender;

private:

	Q_OBJECT


public:

	myCentralWidget(QWidget* parent);
	~myCentralWidget(){};

	void inform_internals_have_changed(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId,
			bool onlyContent, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose purpose) override;

	// =========================================================================
	virtual void init_submodule(IjvxHost* theHost) override;

	virtual void terminate_submodule() override;


	// =========================================================================

	virtual void inform_update_window(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT) override;

	virtual void inform_update_window_periodic() override;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	jvxErrorType report_command_request(const CjvxReportCommandRequest& req) override;

	// ===========================================================================
	
	virtual void report_widget_closed(QWidget* theClosedWidget) override;
	
	virtual void report_widget_specific(jvxSize id, jvxHandle* spec) override;

	// ===========================================================================

signals:

public slots:
	

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
