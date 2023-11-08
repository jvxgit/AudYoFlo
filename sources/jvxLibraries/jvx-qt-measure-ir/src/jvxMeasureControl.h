#ifndef __JVXMEASURECONTROL__H__
#define __JVXMEASURECONTROL__H__

#include "jvx.h"
#include "ui_jvxMeasureControl.h"
#include "jvxQtAcousticMeasurement.h"
#include "CjvxQtSaWidgetWrapper.h"

// #include "jvxSpNMeasureIr_props.h"

//#include "jvxAcousticEqualizer.h"
//#include "jvx-qcp-qt-helpers.h"

//#define JVX_LOG10_EPS 1e-10
//#define JVX_EPS_XY 0.01

class jvxMeasureControl: 
	public QWidget, 
	public Ui::measureControl,
	public IjvxQtSpecificHWidget,
	public IjvxQtSpecificHWidget_wwrapper_si,
	public IjvxQtSpecificHWidget_config_si,
	public IjvxQtSaWidgetWrapper_report,
	public IjvxQtSpecificWidget_report,
	public IjvxConfigurationExtender_report
	//public IjvxConfigurationExtender_report
{
public:
private:
	class oneEvaluation
	{
	public:
		IjvxQtAcousticMeasurement* widget = nullptr;
		std::list<QWidget*> qwidgets;
		std::string showName;
		std::string cfgName;
		// IjvxConfigurationExtender_report* rep;

		oneEvaluation()
		{
			widget = NULL;
			// rep = NULL;
		};
	};

	CjvxQtSaWidgetWrapper myWidgetWrapper;
	IjvxHost* theHostRef = nullptr;

	IjvxAccessProperties* propRefMeasure = nullptr;
	IjvxQtSaWidgetWrapper_report* fwdReports = nullptr;

	std::list<oneEvaluation> allSubWidgets;
	IjvxQtAcousticMeasurement* widget_ac_measure;

	IjvxAccessProperties* thePropRefAnnounce = nullptr;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	jvxCallbackPrivate cb_priv;

	Q_OBJECT 

public:

	jvxMeasureControl(QWidget* parent = NULL);
	virtual ~jvxMeasureControl();

	// ========================================================================	

	virtual void getMyQWidget(QWidget** retWidget, jvxSize id = 0) override;

	virtual void terminate() override;

	virtual void activate() override;

	virtual void deactivate() override;

	virtual void processing_started() override;

	virtual void processing_stopped() override;

	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT, const char* propLst = nullptr) override;

	virtual void update_window_periodic() override;

	virtual jvxErrorType request_sub_interface(jvxQtInterfaceType, jvxHandle**) override;

	virtual jvxErrorType return_sub_interface(jvxQtInterfaceType, jvxHandle*) override;

	// =====================================================================
	
	virtual jvxErrorType addPropertyReference(IjvxAccessProperties* propRef, const std::string&  prefixArg = "", const std::string& identArg = "") override;

	virtual jvxErrorType removePropertyReference(IjvxAccessProperties* propRef) override;

	// =====================================================================

	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL) override;

	// =====================================================================

	virtual jvxErrorType reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res_in_call) override;
	virtual jvxErrorType reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, 
		jvxDataFormat format, jvxErrorType res_in_call) override;
	virtual jvxErrorType reportAllPropertiesAssociateComplete(const char* tag) override;
	virtual jvxErrorType reportPropertySpecific(jvxSaWidgetWrapperspecificReport, jvxHandle* props)override;
	// virtual jvxErrorType request_configuration_ext_report(IjvxConfigurationExtender_report** theConfigExtenderReport) override;
	// virtual jvxErrorType return_configuration_ext_report(IjvxConfigurationExtender_report* theConfigExtenderReport)override;

	// =====================================================================

	// ================ IjvxQtSpecificHWidget_wwrapper_si ==================
	virtual jvxErrorType setWidgetWrapperReportReference(IjvxQtSaWidgetWrapper_report* fwd = nullptr) override;

	// =====================================================================
	// ================ IjvxQtSpecificHWidget_config_si ======================
	virtual jvxErrorType registerConfigExtensions(IjvxConfigurationExtender* cfgExt) override;
	virtual jvxErrorType unregisterConfigExtensions(IjvxConfigurationExtender* cfgExt) override;

	// =====================================================================

	virtual void report_widget_closed(QWidget* theClosedWidget) override;
	virtual void report_widget_specific(jvxSize id, jvxHandle* spec) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename, jvxInt32 lineno) override;

	// =====================================================================

	JVX_PROPERTIES_DIRECT_C_CALLBACK_DECLARE(callback_report_evaluation);

signals:

public slots:

	
};

#endif
