#include "jvxMeasureControl.h"

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtAcousticMeasurement, jvxQtAcousticMeasurement);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtAcousticMeasurement, jvxQtAcousticMeasurement);

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtSpecificHWidget, jvxQtMeasureControl, jvxMeasureControl)
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtSpecificHWidget, jvxQtMeasureControl)

// =====================================================================
// =====================================================================
// =====================================================================


jvxMeasureControl::jvxMeasureControl(QWidget* parent) : QWidget(parent)
{
	cb_priv.cb = callback_report_evaluation;
	cb_priv.cb_priv = reinterpret_cast<jvxHandle*>(this);
}

jvxMeasureControl::~jvxMeasureControl()
{
	
}

// ========================================================================	

void
jvxMeasureControl::getMyQWidget(QWidget** retWidget, jvxSize id)
{
	*retWidget = NULL;
	switch (id)
	{
	case 0:
		*retWidget = static_cast<QWidget*>(this);
		break;
	default:
			*retWidget = nullptr;
		break;
	}
}

// =====================================================================

void
jvxMeasureControl::init(IjvxHost* theHost, jvxCBitField mode,
	jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd)
{
	jvxSize i;
	oneEvaluation newElm;


	this->setupUi(this);
	// this->setWindowTitle("Acoustic Measument - base");

	theHostRef = theHost;

	widget_ac_measure = NULL;
	init_jvxQtAcousticMeasurement(&widget_ac_measure, this);
	newElm.widget = widget_ac_measure;
	// jvxAcousticMeasure* measureAcoustics = NULL;
	// JVX_DSP_SAFE_ALLOCATE_OBJECT(measureAcoustics, jvxAcousticMeasure());
	// newElm.widget = measureAcoustics;
	newElm.showName = "Acoustic Measure";
	allSubWidgets.push_back(newElm);

	auto elm = allSubWidgets.begin();
	for (; elm != allSubWidgets.end(); elm++)
	{
		jvxSize cnt = 0;
		elm->widget->init(theHostRef, 0, elm->widget, static_cast<IjvxQtSpecificWidget_report*>(this));
		while (1)
		{
			QWidget* oneWidget = NULL;
			elm->widget->getMyQWidget(&oneWidget, cnt);
			if (oneWidget)
			{
				elm->showName = oneWidget->windowTitle().toLatin1().data();
				elm->qwidgets.push_back(oneWidget);
				tabWidget_evaluation->addTab(oneWidget, elm->showName.c_str());
				cnt++;
			}
			else
			{
				break;
			}
		}
	}
}

void
jvxMeasureControl::terminate()
{
	auto elm = allSubWidgets.begin();
	for (; elm != allSubWidgets.end(); elm++)
	{
		if (elm->widget == widget_ac_measure)
		{
			elm->widget->terminate();
			elm->qwidgets.clear();
			widget_ac_measure = NULL;
		}
	}
	allSubWidgets.clear();

	terminate_jvxQtAcousticMeasurement(widget_ac_measure);
	widget_ac_measure = nullptr;
	theHostRef = nullptr;
}

void
jvxMeasureControl::activate()
{
	if (!thePropRefAnnounce)
	{
		groupBox_announce->hide();
	}
	auto elm = allSubWidgets.begin();
	for (; elm != allSubWidgets.end(); elm++)
	{
		elm->widget->activate();
	}
}

void
jvxMeasureControl::deactivate()
{
	auto elm = allSubWidgets.begin();
	for (; elm != allSubWidgets.end(); elm++)
	{
		elm->widget->deactivate();
	}
}

void
jvxMeasureControl::processing_started()
{
}

void
jvxMeasureControl::processing_stopped()
{
}

void
jvxMeasureControl::update_window(jvxCBitField prio, const char* propLst )
{
	myWidgetWrapper.trigger_updateWindow("measure");
	myWidgetWrapper.trigger_updateWindow("measure_rep");
	myWidgetWrapper.trigger_updateWindow("mmonitor");
	myWidgetWrapper.trigger_updateWindow("mresults");
	auto elm = allSubWidgets.begin();
	for (; elm != allSubWidgets.end(); elm++)
	{
		elm->widget->update_window(prio);
	}
	myWidgetWrapper.trigger_updateWindow("chanAnn_node");
}

void
jvxMeasureControl::update_window_periodic()
{
	myWidgetWrapper.trigger_updateWindow_periodic("measure");
	myWidgetWrapper.trigger_updateWindow_periodic("measure_rep");
	myWidgetWrapper.trigger_updateWindow_periodic("mmonitor");
	myWidgetWrapper.trigger_updateWindow_periodic("mresults");
	auto elm = allSubWidgets.begin();
	for (; elm != allSubWidgets.end(); elm++)
	{
		elm->widget->update_window_periodic();
	}
	myWidgetWrapper.trigger_updateWindow_periodic("chanAnn_node");
}

// =====================================================================

jvxErrorType
jvxMeasureControl::request_sub_interface(jvxQtInterfaceType ifTp, jvxHandle** retHdl)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch(ifTp)
	{ 
	case jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_CONFIG:
		if (retHdl)
		{
			*retHdl = static_cast<IjvxQtSpecificHWidget_config_si*>(this);
		}
		res = JVX_NO_ERROR;
		break;
	case jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_WWRAPPER:
		if (retHdl)
		{
			*retHdl = static_cast<IjvxQtSpecificHWidget_wwrapper_si*>(this);
		}
		res = JVX_NO_ERROR;
		break;
	default:
		break;
	}
	return res;
}

jvxErrorType
jvxMeasureControl::return_sub_interface(jvxQtInterfaceType ifTp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (ifTp)
	{
	case jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_CONFIG:
		if (hdl == static_cast<IjvxQtSpecificHWidget_config_si*>(this))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_WWRAPPER:
		if (hdl == static_cast<IjvxQtSpecificHWidget_wwrapper_si*>(this))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	default:
		break;
	}
	return res;
}

// =====================================================================

jvxErrorType
jvxMeasureControl::addPropertyReference(IjvxAccessProperties* propRefArg, const std::string& prefixArg, const std::string& identArg)
{
	if (identArg == "jvxAuNMeasure")
	{
		assert(propRefMeasure == NULL);
		propRefMeasure = propRefArg;

		myWidgetWrapper.associateAutoWidgets(this,
			propRefMeasure, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "mmonitor");

		myWidgetWrapper.associateAutoWidgets(this,
			propRefMeasure, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "measure");

		myWidgetWrapper.associateAutoWidgets(this,
			propRefMeasure, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "measure_rep");

		myWidgetWrapper.associateAutoWidgets(this,
			propRefMeasure, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "mresults");

		auto elm = allSubWidgets.begin();
		for (; elm != allSubWidgets.end(); elm++)
		{
			elm->widget->addPropertyReference(propRefMeasure);
		}

		ident.reset("/report_evaluate");
		trans.reset(true);
		jvxCallManagerProperties callGate;
		jvxErrorType res = propRefMeasure->install_referene_property(
			callGate,
			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxCallbackPrivate>(&cb_priv),
			ident);
		assert(res == JVX_NO_ERROR);
	}
	if (identArg == "jvxAuNChannelAnnounce")
	{
		assert(thePropRefAnnounce == nullptr);
		thePropRefAnnounce = propRefArg;
		myWidgetWrapper.associateAutoWidgets(this,
			thePropRefAnnounce, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "chanAnn_node");
	}
	return JVX_NO_ERROR;
	
}

jvxErrorType
jvxMeasureControl::removePropertyReference(IjvxAccessProperties* propRefIn)
{
	if (propRefMeasure == propRefIn)
	{
		jvxCallManagerProperties callGate;
		ident.reset("/report_evaluate");
		trans.reset(true);

		jvxErrorType res = propRefMeasure->uninstall_referene_property(
			callGate, jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxCallbackPrivate>(&cb_priv),
			ident);
		assert(res == JVX_NO_ERROR);

		auto elm = allSubWidgets.begin();
		for (; elm != allSubWidgets.end(); elm++)
		{		
			elm->widget->removePropertyReference(propRefMeasure);
		}
		myWidgetWrapper.deassociateAutoWidgets("mmonitor");
		myWidgetWrapper.deassociateAutoWidgets("measure");
		myWidgetWrapper.deassociateAutoWidgets("measure_rep");
		myWidgetWrapper.deassociateAutoWidgets("mresults");
		propRefMeasure = nullptr;
	}
	if (thePropRefAnnounce == propRefIn)
	{
		myWidgetWrapper.deassociateAutoWidgets("chanAnn_node");
		thePropRefAnnounce = nullptr;
	}
	return JVX_NO_ERROR;
}

// ==========================================================================

jvxErrorType
jvxMeasureControl::reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res_in_call)
{
	if (fwdReports)
	{
		fwdReports->reportPropertySet(tag, propDescrptior, groupid, res_in_call);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvxMeasureControl::reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, jvxErrorType res_in_call)
{
	if (fwdReports)
	{
		fwdReports->reportPropertyGet(tag, propDescrptior, ptrFld, offset, numElements, format, res_in_call);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvxMeasureControl::reportAllPropertiesAssociateComplete(const char* tag)
{
	if (fwdReports)
	{
		fwdReports->reportAllPropertiesAssociateComplete(tag);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvxMeasureControl::reportPropertySpecific(jvxSaWidgetWrapperspecificReport rr, jvxHandle* props)
{
	if (fwdReports)
	{
		fwdReports->reportPropertySpecific(rr, props);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvxMeasureControl::setWidgetWrapperReportReference(IjvxQtSaWidgetWrapper_report* fwdArg)
{
	fwdReports = fwdArg;
	return JVX_NO_ERROR;
}

jvxErrorType
jvxMeasureControl::registerConfigExtensions(IjvxConfigurationExtender* cfgExt)
{
	auto elm = allSubWidgets.begin();
	for (; elm != allSubWidgets.end(); elm++)
	{
		// elm->rep = NULL;
		IjvxQtSpecificHWidget_config_si* theIf = nullptr;
		elm->widget->request_sub_interface(jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_CONFIG, reinterpret_cast<jvxHandle**>(&theIf));
		if (theIf)
		{
			theIf->registerConfigExtensions(cfgExt);
			elm->widget->return_sub_interface(jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_CONFIG, reinterpret_cast<jvxHandle*>(theIf));
		}
	}

	cfgExt->register_extension(static_cast<IjvxConfigurationExtender_report*>(this), "WHISTLE2_MAIN");

	return JVX_NO_ERROR;
}

jvxErrorType
jvxMeasureControl::unregisterConfigExtensions(IjvxConfigurationExtender* cfgExt)
{
	auto elm = allSubWidgets.begin();
	for (; elm != allSubWidgets.end(); elm++)
	{
		IjvxQtSpecificHWidget_config_si* theIf = nullptr;
		elm->widget->request_sub_interface(jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_CONFIG, reinterpret_cast<jvxHandle**>(&theIf));
		if (theIf)
		{
			theIf->unregisterConfigExtensions(cfgExt);
			elm->widget->return_sub_interface(jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_CONFIG, reinterpret_cast<jvxHandle*>(theIf));
		}
	}
	return JVX_NO_ERROR;
}

void
jvxMeasureControl::report_widget_closed(QWidget* theClosedWidget)
{
}

void
jvxMeasureControl::report_widget_specific(jvxSize id, jvxHandle* spec)
{

}

jvxErrorType
jvxMeasureControl::get_configuration_ext(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxMeasureControl::put_configuration_ext(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename, jvxInt32 lineno)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_DIRECT_C_CALLBACK_EXECUTE_FULL(jvxMeasureControl, callback_report_evaluation)
{
	if (content->tagid == JVX_FLOATINGPOINTER_UPDATE_PROPERTY_REPORT_IMMEDIATE)
	{
		jvxFloatingPointer* ptr = (jvxFloatingPointer*)content;

		/*
		listWidget_report->addItem(txt.c_str());
		listWidget_report->setCurrentRow(listWidget_report->count()-1);
		*/
		if (propRefMeasure)
		{
			myWidgetWrapper.trigger_updateWindow("measure");
			myWidgetWrapper.trigger_updateWindow("measure_rep");
			myWidgetWrapper.trigger_updateWindow("mmonitor");
		}
		qApp->processEvents();
	}

	return JVX_NO_ERROR;
}

// Need to put this function here to use the QT META TYPE

