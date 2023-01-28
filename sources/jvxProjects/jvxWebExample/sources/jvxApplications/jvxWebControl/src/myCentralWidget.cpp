#include "myCentralWidget.h"
#include <QtWidgets/QFileDialog>
#include "jvx-qt-helpers.h"

#ifndef JVX_ADD_WIDGET_WRAPPER
#error "This module will compile only with widget wrapper!"
#endif

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#define JVX_ID_TRANSFER_ALIVE_TRIGGER 0
#define JVX_EPSMINMAX_ASCAN 1e-2
#define JVX_EPSMINMAX_RAWRAM 1e-2
#define JVX_EPS_LOG 1e-7
//#define JVX_TIMEOUT_ASCAN_PLOT_MSEC 1000
	//#define JVX_INSPIRE_CENTRAL_WIDGET_VERBOSE

/* 
	* Bahnkurve: We see raw values, x- and y offset and a scalefactor.
	* The processing is as follows:
	* function -> raw values (1) -> +=offset x/y -> *=scaleFac_bahnkurve -> plot
	*                                                            -> transfer
	* and
	* 
	* transfer -> /= scaleFac_bahnkurve -> -= offset x/y -> raw values (from here: (1))
*/
#define JVX_NUM_REMOTE_PROPERTIES 2
static jvxOneRemotePropertySpecification the_properties_websocket[JVX_NUM_REMOTE_PROPERTIES] =
{
	//   property name ,			component identification,				format             , numElms (-1: unknown),  decoder hint type,									offset, param0 (num channels),  param1,  state to activate property on,		update condition,					param_cond_update (number timeslices),	user id, cnt_report_disconnect
	{	"/field1/memory",			{JVX_COMPONENT_CUSTOM_DEVICE, 0, 0},	JVX_DATAFORMAT_32BIT_LE, JVX_SIZE_UNSELECTED,	JVX_PROPERTY_DECODER_NONE,							0     , 1,						128,	JVX_STATE_ACTIVE,					JVX_PROP_STREAM_UPDATE_TIMEOUT,		1,										0      , 10},
	{   "/field2/memory",			{JVX_COMPONENT_CUSTOM_DEVICE, 0, 0},	JVX_DATAFORMAT_32BIT_LE, JVX_SIZE_UNSELECTED,	JVX_PROPERTY_DECODER_NONE,							0     , 1,						128,	JVX_STATE_ACTIVE,					JVX_PROP_STREAM_UPDATE_TIMEOUT,		1,										1      , 10}
};

myCentralWidget::myCentralWidget(QWidget* parent) : mainCentral_hostfactory_ww_http(parent)
{
	propRef_customdvice = NULL;

	localProcid_al = 0;
	uiReady = false;

	//webSocketsReady = false;

	cfgExtender = NULL;

	memset(ascanBuffer32, 0, sizeof(ascanBuffer32));

	plotOptionsAscan.minx = 0;
	plotOptionsAscan.maxx = JVX_NUMBER_INT32_ASCANS-1;
	plotOptionsAscan.autox = true;
	plotOptionsAscan.miny = 0;
	plotOptionsAscan.maxy = 1;
	plotOptionsAscan.autoy = true;
	plotOptionsAscan.autoupdate = false;
	plotOptionsAscan.log10 = true;

	//ascanUpdateInProgress = false;
	ascan_bufx.resize(JVX_NUMBER_INT32_ASCANS);
	ascan_bufy.resize(JVX_NUMBER_INT32_ASCANS);
	reset_ascan_buffers();

	memset(rawramBuffer32, 0, sizeof(rawramBuffer32));
	plotOptionsRawram.minx = 0;
	plotOptionsRawram.maxx = JVX_NUMBER_INT32_RAWRAM - 1;
	plotOptionsRawram.autox = true;
	plotOptionsRawram.miny = 0;
	plotOptionsRawram.maxy = 1;
	plotOptionsRawram.autoy = true;
	plotOptionsRawram.autoupdate = false;
	plotOptionsRawram.log10 = true;
	rawram_bufx.resize(JVX_NUMBER_INT32_RAWRAM);
	rawram_bufy.resize(JVX_NUMBER_INT32_RAWRAM);
	reset_rawram_buffers();

}

myCentralWidget::~myCentralWidget()
{
}

void
myCentralWidget::init_submodule(IjvxFactoryHost* theHost)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;// , j;
	QVariant varlane;
	QVariant varid;

	Q_INIT_RESOURCE(myCentralWidget);

	this->setupUi(this);
	mainCentral_hostfactory_ww_http::init_submodule(theHost);

	/* ==========================================================*/
	/*
	 * These lines lead to a crash of qcustomplot on MY ONE computer.
	 * The solution was to install an updated intel driver of the graphics card..
	 */
#ifdef JVX_PERMIT_QCP_OPENGL
	qcp_ascan->setOpenGl(true);
	if (!qcp_ascan->openGl())
	{
		std::cout << "Failed to activate openGL rendering!" << std::endl;
	}
#endif
	qcp_ascan->addGraph();
	qcp_ascan->yAxis->setLabel("y(t)");
	qcp_ascan->xAxis->setLabel("x(t)");
	qcp_ascan->xAxis->setRange(0, 1);
	qcp_ascan->graph(0)->setPen(QPen(Qt::black));
	qcp_ascan->graph(0)->setName("ASCANs");
	//qcp_ascan->graph(0)->setLineStyle(QCPGraph::lsNone);
	//qcp_ascan->graph(0)->setScatterStyle(QCPScatterStyle::ssSquare);

#ifdef JVX_PERMIT_QCP_OPENGL	
	qcp_rawram->setOpenGl(true);
	if (!qcp_rawram->openGl())
	{
		std::cout << "Failed to activate openGL rendering!" << std::endl;
	}
#endif
	qcp_rawram->addGraph();
	qcp_rawram->yAxis->setLabel("y(t)");
	qcp_rawram->xAxis->setLabel("x(t)");
	qcp_rawram->xAxis->setRange(0, 1);
	qcp_rawram->graph(0)->setPen(QPen(Qt::black));
	qcp_rawram->graph(0)->setName("RAWRAM");
#ifdef JVX_SHOW_LEGENDS
	qcp_ascan->legend->setVisible(true);
#endif
	// ==================================================================
	// ==================================================================
	// ==================================================================

	res = theHostRef->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&cfgExtender));
	if ((res == JVX_NO_ERROR) && cfgExtender)
	{
		res = cfgExtender->register_extension(static_cast<IjvxConfigurationExtender_report*>(this), "INSPIRE_MAIN");
	}

	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);

	/*
	theAutoAscanTimer = new QTimer(this);
	connect(theAutoOsziTimer, SIGNAL(timeout()), this, SLOT(oszi_timer_expired()));
	theAutoOsziTimer->setSingleShot(true);
	*/

	theAliveTimer = new QTimer(this);
	connect(theAliveTimer, SIGNAL(timeout()), this, SLOT(alive_timer_expired()));
	/*
	ctrl->register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Timout Http Transfer Oszi [msec]",
		& id_ui_timout_oszi_msec);

	ctrl->register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Length Circular Buffer [bytes]",
		&id_ereignis_cbuffer_length);

	ctrl->register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_DATA_VALUE,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Bahnkurven-index scale factor",
		&id_ereignis_scale);
		*/

	for (i = 0; i < ascan_bufx.size(); i++)
	{
		ascan_bufx[(int)i] = (double)i;
	}
	reset_ascan_buffers();
	recompute_ascans();
	replot_ascans();

	for (i = 0; i < rawram_bufx.size(); i++)
	{
		rawram_bufx[(int)i] = (double)i;
	}
	reset_rawram_buffers();
	recompute_rawram();
	replot_rawram();
}

jvxErrorType
myCentralWidget::get_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id)
{
	jvxErrorType res = JVX_NO_ERROR;
	/*
	if (id == id_ui_timout_oszi_msec)
	{
		var.assign(oszi_min_timer_msec);
		return JVX_NO_ERROR;
	}
	else if (id == id_ereignis_cbuffer_length)
	{
		var.assign(the_properties_websocket[1].param1);
		return JVX_NO_ERROR;
	}
	
	else if (id == id_ereignis_scale)
	{
		var.assign(scaleFac_bahnkurve);
		return JVX_NO_ERROR;
	}*/
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
myCentralWidget::set_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id)
{
	jvxErrorType res = JVX_NO_ERROR;
	/*
	if (id == id_ui_timout_oszi_msec)
	{
		var.toContent(&oszi_min_timer_msec);
		return JVX_NO_ERROR;
	}
	else if (id == id_ereignis_cbuffer_length)
	{
		var.toContent(&the_properties_websocket[1].param1);
		return JVX_NO_ERROR;
	}
	else if (id == id_ereignis_scale)
	{
		var.toContent(&scaleFac_bahnkurve);
		recomputeBahnkurve();
		return JVX_NO_ERROR;
	}*/
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

void
myCentralWidget::terminate_submodule()
{
	if (cfgExtender)
	{
		theHostRef->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle*>(cfgExtender));
		cfgExtender = NULL;
	}
}

void
myCentralWidget::inform_update_window(jvxCBitField prio)
{
	QString txt;
	jvxSize j = 0; 
	jvxSize i = 0;
	QFont theFont;

	lineEdit_xmax_ascans->setText(jvx_data2String(plotOptionsAscan.maxx, 1).c_str());
	lineEdit_xmin_ascans->setText(jvx_data2String(plotOptionsAscan.minx, 1).c_str());
	checkBox_autox_ascans->setChecked(plotOptionsAscan.autox);
	if (plotOptionsAscan.autox)
	{
		lineEdit_xmax_ascans->setEnabled(false);
		lineEdit_xmin_ascans->setEnabled(false);
	}
	else
	{
		lineEdit_xmax_ascans->setEnabled(true);
		lineEdit_xmin_ascans->setEnabled(true);
	}
	
	lineEdit_ymax_ascans->setText(jvx_data2String(plotOptionsAscan.maxy, 1).c_str());
	lineEdit_ymin_ascans->setText(jvx_data2String(plotOptionsAscan.miny, 1).c_str());
	checkBox_autoy_ascans->setChecked(plotOptionsAscan.autoy);
	if (plotOptionsAscan.autoy)
	{
		lineEdit_ymax_ascans->setEnabled(false);
		lineEdit_ymin_ascans->setEnabled(false);
	}
	else
	{
		lineEdit_ymax_ascans->setEnabled(true);
		lineEdit_ymin_ascans->setEnabled(true);
	}

	
	checkBox_log10_ascans->setChecked(plotOptionsAscan.log10);

	// ================================================================

	lineEdit_xmax_rawram->setText(jvx_data2String(plotOptionsRawram.maxx, 1).c_str());
	lineEdit_xmin_rawram->setText(jvx_data2String(plotOptionsRawram.minx, 1).c_str());
	checkBox_autox_rawram->setChecked(plotOptionsRawram.autox);
	if (plotOptionsRawram.autox)
	{
		lineEdit_xmax_rawram->setEnabled(false);
		lineEdit_xmin_rawram->setEnabled(false);
	}
	else
	{
		lineEdit_xmax_rawram->setEnabled(true);
		lineEdit_xmin_rawram->setEnabled(true);
	}

	lineEdit_ymax_rawram->setText(jvx_data2String(plotOptionsRawram.maxy, 1).c_str());
	lineEdit_ymin_rawram->setText(jvx_data2String(plotOptionsRawram.miny, 1).c_str());
	checkBox_autoy_rawram->setChecked(plotOptionsRawram.autoy);
	if (plotOptionsRawram.autoy)
	{
		lineEdit_ymax_rawram->setEnabled(false);
		lineEdit_ymin_rawram->setEnabled(false);
	}
	else
	{
		lineEdit_ymax_rawram->setEnabled(true);
		lineEdit_ymin_rawram->setEnabled(true);
	}

	checkBox_log10_rawram->setChecked(plotOptionsRawram.log10);

	/*
	if (webSockRuntime.stateWebSock != JVX_WEBSOCK_STATE_COMPLETE)
	{
		pushButton_oszi_trigger->setEnabled(false);
		comboBox_oszi_mode->setEnabled(false);
	}
	else
	{
		comboBox_oszi_mode->setEnabled(true);
		inform_update_window_oszi();
	}
	*/
}

jvxErrorType
myCentralWidget::get_configuration_ext(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	/*
	jvxConfigData* datTmp = NULL;
	jvxConfigData* datTmpA = NULL;
	jvxConfigData* datTmpB = NULL;
	jvxSize i;
	*/
	/*
	processor->createAssignmentString(&datTmp, "Haukes_1_Param", "Hallo");
	processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
	datTmp = NULL;

	processor->createAssignmentString(&datTmp, "Haukes_2_Param", "Hallo");
	processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
	datTmp = NULL;
	*/
	return JVX_NO_ERROR;
};

jvxErrorType
myCentralWidget::put_configuration_ext(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename, jvxInt32 lineno)
{
	/*
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* datTmp = NULL;
	std::string token;
	jvxSize i;
	*/
	
	/*
	res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe,
		&datTmp, token.c_str());
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Failed to read config entry <" << token << ">: " << jvxErrorType_txt(res) << std::endl;
	}

	datTmp = NULL;
	res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe,
		&datTmp, "Haukes_2_Param");
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Failed to read config entry <" << token << ">: " << jvxErrorType_txt(res) << std::endl;
	}
	*/

	return JVX_NO_ERROR;
};

void
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
	mainCentral_hostfactory_ww_http::inform_active(tp, propRef);
	switch (tp.tp)
	{
	case JVX_COMPONENT_CUSTOM_DEVICE:
		//setBackgroundLabelColor(Qt::yellow, label_connected);

		propRef_customdvice = propRef;
		myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), propRef_customdvice,
			static_cast<IjvxQtSaWidgetWrapper_report*>(this), "custom_device");
		localProcid_al = 0;
		//localProcid_ascan = 0;
		reset_ascan_buffers();
		recompute_ascans();
		replot_ascans();

		reset_rawram_buffers();
		recompute_rawram();
		replot_rawram();
		break;
	}
}


void
myCentralWidget::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	switch (tp.tp)
	{
	case JVX_COMPONENT_CUSTOM_DEVICE:
		myWidgetWrapper.deassociateAutoWidgets("custom_device");
		// setBackgroundLabelColor(Qt::white, label_connected);

		propRef_customdvice = NULL;
		uiReady = false;

		// The process ids must be reset. This is required since the
		// operations that caused the timeout otherwise do not see that the process
		// is over!!
		//localProcid_ascan = 0;
		localProcid_al = 0;

		theAliveTimer->stop();
		reset_ascan_buffers();
		recompute_ascans();
		replot_ascans();

		reset_rawram_buffers();
		recompute_rawram();
		replot_rawram();
		break;
	}
	mainCentral_hostfactory_ww_http::inform_inactive(tp, theProps);
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void
myCentralWidget::inform_sequencer_about_to_start() 
{
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_hostfactory_ww_http::inform_sequencer_about_to_start();
}

void
myCentralWidget::inform_sequencer_about_to_stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_hostfactory_ww_http::inform_sequencer_stopped();
}

bool
myCentralWidget::is_ready()
{
	return true;
}

jvxErrorType
myCentralWidget::reportPropertySet(const char* tag, const char* propDescrptor, jvxSize groupid, jvxErrorType res)
{
	myWidgetWrapper.trigger_updateWindow(tag, propDescrptor);
	mainCentral_hostfactory_ww_http::reportPropertySet(tag, propDescrptor, groupid, res);
	return JVX_NO_ERROR;
}

jvxErrorType
myCentralWidget::reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, jvxErrorType res)
{
	/*
	if (strcmp(tag, "custom_device") == 0)
	{
		if (strcmp(propDescrptior, "/ereignis/crit_threshold") == 0)
		{
			if (
				(format == JVX_DATAFORMAT_32BIT_LE) &&
				(numElements == 1) &&
				(offset == 0))
			{
				copyCriticalValue = *((jvxInt32*)ptrFld);
				replot_histogram_buffer();
				return JVX_NO_ERROR;
			}
			else
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		if (strcmp(propDescrptior, "/ereignis/segmentgroesse") == 0)
		{
			if (
				(format == JVX_DATAFORMAT_32BIT_LE) &&
				(numElements == 1) &&
				(offset == 0))
			{
				copySegmentGroesse = *((jvxInt32*)ptrFld);
				replot_ereignis_buffer();
				return JVX_NO_ERROR;
			}
			else
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
		}
	}
	*/
	mainCentral_hostfactory_ww_http::reportPropertyGet(tag, propDescrptior, ptrFld, offset, numElements, format, res);
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
myCentralWidget::reportAllPropertiesAssociateComplete(const char* tag)
{
	jvxBitField features;
	
	if (std::string(tag) == "custom_device")
	{
		//setBackgroundLabelColor(Qt::green, label_connected);

		uiReady = true;
		inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);

		propRef_customdvice->get_features(&features);
		assert(jvx_bitTest(features, JVX_PROPERTY_ACCESS_DELAYED_RESPONSE_SHIFT));

		theAliveTimer->start(5000);
		//autoUpdateAscanTimer->start(JVX_TIMEOUT_ASCAN_PLOT_MSEC);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
myCentralWidget::report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr)
{
	intptr_t id = (intptr_t)privatePtr;
	jvxInt16 valB;
	jvxErrorType res = JVX_NO_ERROR;
	//jvxSize i;
	jvxCallManagerProperties callGate;
	switch (id)
	{
	case JVX_ID_TRANSFER_ALIVE_TRIGGER:
		ident.reset("/alive/check");
		trans.reset(true);
		res = propRef_customdvice->get_property(callGate, jPRG(&valB, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans, jPSCH(&localProcid_al));
		// The result may be an error. It is important that it responses, even if property does not exist
		break;
	/*
	case JVX_ID_TRANSFER_ASCAN_TRIGGER:
		res = propRef_customdvice->get_property__descriptor(ascanBuffer32, 0, JVX_NUMBER_INT32_ASCANS, JVX_DATAFORMAT_32BIT_LE, true, "/ascans/memory", JVX_PROPERTY_DECODER_NONE, &localProcid_ascan);
		recompute_ascans();
		replot_ascans();
		// The result may be an error. It is important that it responses, even if property does not exist
		break;
		*/
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
myCentralWidget::report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr)
{
	intptr_t id = (intptr_t)privatePtr;
	/*
	if (localProcid_ascan == uniqueId)
	{
		localProcid_ascan = 0;
	}
	*/
	if (localProcid_al == uniqueId)
	{
		localProcid_al = 0;
	}
	switch (id)
	{
	case JVX_ID_TRANSFER_ALIVE_TRIGGER:
		break;
	/*
	case JVX_ID_TRANSFER_ASCAN_TRIGGER:
		//ascanUpdateInProgress = false;
		inform_update_window_ascan();
		break;
		*/
	}

	return JVX_NO_ERROR;
}

void
myCentralWidget::alive_timer_expired()
{
	core_init_alive_retrigger();
}

void
myCentralWidget::inform_update_window_periodic()
{
	jvxErrorType res = JVX_NO_ERROR;
}

void 
myCentralWidget::process_websocket_binary(jvxSize userData, jvxByte* payload, jvxPropertyTransferPriority prio)
{
	jvxPropertyStreamHeader* shdr = (jvxPropertyStreamHeader*)payload;
	jvxUInt8* load8 = (jvxUInt8*)(payload + sizeof(jvxPropertyStreamHeader));
	jvxInt32* load32 = (jvxInt32*)(payload + sizeof(jvxPropertyStreamHeader));
	jvxUInt8* ptr_load8_start = NULL;
	jvxSize num_load8_start = 0;
	jvxData minVal = JVX_DATA_MAX_POS;
	jvxData maxVal = JVX_DATA_MAX_NEG;
	jvxData minValShow = 0;
	jvxData maxValShow = 0;
	jvxSize numElmDetected = 0;
	jvxSize idxLim = 0;
	//jvxSize i;
	jvxDspBaseErrorType resDSPL = JVX_DSP_NO_ERROR;

	switch (prio)
	{
	case JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO:
		switch (userData)
		{
		case 0:

			if (shdr->property_num_elements == JVX_NUMBER_INT32_ASCANS)
			{
				memcpy(ascanBuffer32, load32, sizeof(jvxUInt32) * JVX_NUMBER_INT32_ASCANS);
			}
			recompute_ascans();
			replot_ascans();
			break;
		case 1:

			if (shdr->property_num_elements == JVX_NUMBER_INT32_RAWRAM)
			{
				memcpy(rawramBuffer32, load32, sizeof(jvxUInt32) * JVX_NUMBER_INT32_RAWRAM);
			}
			recompute_rawram();
			replot_rawram();
			break;
		}
		break;
	case JVX_PROP_CONNECTION_TYPE_HIGH_PRIO:
		break;
	}
}

void
myCentralWidget::core_init_alive_retrigger()
{
	jvxInt16 valB = 1;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	// Trigger get property on custom device. Values will be available later
	res = propRef_customdvice->property_start_delayed_group(callGate,
		static_cast<IjvxAccessProperties_delayed_report*>(this),
		(jvxHandle*)intptr_t(JVX_ID_TRANSFER_ALIVE_TRIGGER), jPSCH(&localProcid_al));
	if (res == JVX_ERROR_COMPONENT_BUSY)
	{
		std::cout << "Live process not triggered since the latest request is still ongoing." << std::endl;
		return;
	}

	ident.reset("/alive/check");
	trans.reset(true);
	res = propRef_customdvice->get_property(callGate, jPRG(&valB, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans, jPSCH(&localProcid_al));
	assert(res == JVX_ERROR_POSTPONE);

	res = propRef_customdvice->property_stop_delayed_group(callGate, nullptr, jPSCH(&localProcid_al));
	assert(res == JVX_NO_ERROR);
}

// ===================================================================

void
myCentralWidget::recompute_ascans()
{
	jvxSize i;

	for (i = 0; i < JVX_NUMBER_INT32_ASCANS; i++)
	{
		ascan_bufy[(int)i] = (double)ascanBuffer32[i];
	}

	if (plotOptionsAscan.log10)
	{
		for (i = 0; i < JVX_NUMBER_INT32_ASCANS; i++)
		{
			ascan_bufy[(int)i] = log10(ascan_bufy[(int)i] + JVX_EPS_LOG);
			ascan_bufy[(int)i] = JVX_MAX(0, ascan_bufy[(int)i]);
		}
	}
}

void
myCentralWidget::recompute_rawram()
{
	jvxSize i;

	for (i = 0; i < JVX_NUMBER_INT32_RAWRAM; i++)
	{
		rawram_bufy[(int)i] = (double)rawramBuffer32[i];
	}

	if (plotOptionsRawram.log10)
	{
		for (i = 0; i < JVX_NUMBER_INT32_RAWRAM; i++)
		{
			rawram_bufy[(int)i] = log10(rawram_bufy[(int)i] + JVX_EPS_LOG);
			rawram_bufy[(int)i] = JVX_MAX(0, rawram_bufy[(int)i]);
		}
	}
}
void
myCentralWidget::replot_ascans()
{
	jvxSize i;
	double minY = JVX_DATA_MAX_POS;
	double maxY = JVX_DATA_MAX_NEG;
	double minX = 0;
	double maxX = 0;
	for (i = 0; i < ascan_bufy.size(); i++)
	{
		if (ascan_bufy[(int)i] < minY)
		{
			minY = ascan_bufy[(int)i];
		}
		if (ascan_bufy[(int)i] > maxY)
		{
			maxY = ascan_bufy[(int)i];
		}
	}
	qcp_ascan->graph(0)->setData(ascan_bufx, ascan_bufy);

	minX = ascan_bufx[0];
	maxX = ascan_bufx[ascan_bufx.size()-1];

	if (!plotOptionsAscan.autox)
	{
		minX = plotOptionsAscan.minx;
		maxX = plotOptionsAscan.maxx;
	}

	if (!plotOptionsAscan.autoy)
	{
		minY = plotOptionsAscan.miny;
		maxY = plotOptionsAscan.maxy;
	}

	if (maxY < minY + JVX_EPSMINMAX_ASCAN)
	{
		maxY = minY + JVX_EPSMINMAX_ASCAN;
	}

	if (maxX < minX + JVX_EPSMINMAX_ASCAN)
	{
		maxX = minX + JVX_EPSMINMAX_ASCAN;
	}

	qcp_ascan->yAxis->setRange(minY, maxY);
	qcp_ascan->xAxis->setRange(minX, maxX);
	qcp_ascan->replot();
}

void
myCentralWidget::replot_rawram()
{
	jvxSize i;
	double minY = JVX_DATA_MAX_POS;
	double maxY = JVX_DATA_MAX_NEG;
	double minX = 0;
	double maxX = 0;
	for (i = 0; i < rawram_bufy.size(); i++)
	{
		if (rawram_bufy[(int)i] < minY)
		{
			minY = rawram_bufy[(int)i];
		}
		if (rawram_bufy[(int)i] > maxY)
		{
			maxY = rawram_bufy[(int)i];
		}
	}
	qcp_rawram->graph(0)->setData(rawram_bufx, rawram_bufy);

	minX = rawram_bufx[0];
	maxX = rawram_bufx[rawram_bufx.size() - 1];

	if (!plotOptionsRawram.autox)
	{
		minX = plotOptionsRawram.minx;
		maxX = plotOptionsRawram.maxx;
	}

	if (!plotOptionsRawram.autoy)
	{
		minY = plotOptionsRawram.miny;
		maxY = plotOptionsRawram.maxy;
	}

	if (maxY < minY + JVX_EPSMINMAX_RAWRAM)
	{
		maxY = minY + JVX_EPSMINMAX_RAWRAM;
	}

	if (maxX < minX + JVX_EPSMINMAX_RAWRAM)
	{
		maxX = minX + JVX_EPSMINMAX_RAWRAM;
	}

	qcp_rawram->yAxis->setRange(minY, maxY);
	qcp_rawram->xAxis->setRange(minX, maxX);
	qcp_rawram->replot();
}

void
myCentralWidget::reset_ascan_buffers()
{
	memset(ascanBuffer32, 0, sizeof(ascanBuffer32));
}

void
myCentralWidget::reset_rawram_buffers()
{
	memset(rawramBuffer32, 0, sizeof(rawramBuffer32));
}

// ===================================================================

void 
myCentralWidget::changed_miny_ascan()
{
	QString txt = lineEdit_ymin_ascans->text();
	plotOptionsAscan.miny = txt.toData();
	plotOptionsAscan.maxy = JVX_MAX(plotOptionsAscan.maxy, plotOptionsAscan.miny + JVX_EPSMINMAX_ASCAN);
	replot_ascans();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void 
myCentralWidget::toggled_autoy_ascan(bool tog)
{
	plotOptionsAscan.autoy = tog;
	recompute_ascans();
	replot_ascans();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void 
myCentralWidget::toggled_autox_ascan(bool tog)
{
	plotOptionsAscan.autox = tog;
	recompute_ascans();
	replot_ascans();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void 
myCentralWidget::toggled_log10_ascan(bool tog)
{
	plotOptionsAscan.log10 = tog;
	recompute_ascans();
	replot_ascans();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void
myCentralWidget::changed_minx_ascan()
{
	QString txt = lineEdit_xmin_ascans->text();
	plotOptionsAscan.minx = txt.toData();
	plotOptionsAscan.maxx = JVX_MAX(plotOptionsAscan.maxx, plotOptionsAscan.minx + JVX_EPSMINMAX_ASCAN);
	replot_ascans();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void 
myCentralWidget::changed_maxy_ascan()
{
	QString txt = lineEdit_ymax_ascans->text();
	plotOptionsAscan.maxy = txt.toData();
	plotOptionsAscan.miny = JVX_MIN(plotOptionsAscan.miny, plotOptionsAscan.maxy - JVX_EPSMINMAX_ASCAN);
	replot_ascans();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void 
myCentralWidget::changed_maxx_ascan()
{
	QString txt = lineEdit_xmax_ascans->text();
	plotOptionsAscan.maxx = txt.toData();
	plotOptionsAscan.minx = JVX_MIN(plotOptionsAscan.minx, plotOptionsAscan.maxx - JVX_EPSMINMAX_ASCAN);
	replot_ascans();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

// ++++++++

void
myCentralWidget::changed_miny_rawram()
{
	QString txt = lineEdit_ymin_rawram->text();
	plotOptionsRawram.miny = txt.toData();
	plotOptionsRawram.maxy = JVX_MAX(plotOptionsRawram.maxy, plotOptionsRawram.miny + JVX_EPSMINMAX_RAWRAM);
	replot_rawram();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void
myCentralWidget::toggled_autoy_rawram(bool tog)
{
	plotOptionsRawram.autoy = tog;
	recompute_rawram();
	replot_rawram();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void
myCentralWidget::toggled_autox_rawram(bool tog)
{
	plotOptionsRawram.autox = tog;
	recompute_rawram();
	replot_rawram();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void
myCentralWidget::toggled_log10_rawram(bool tog)
{
	plotOptionsRawram.log10 = tog;
	recompute_rawram();
	replot_rawram();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void
myCentralWidget::changed_minx_rawram()
{
	QString txt = lineEdit_xmin_rawram->text();
	plotOptionsRawram.minx = txt.toData();
	plotOptionsRawram.maxx = JVX_MAX(plotOptionsRawram.maxx, plotOptionsRawram.minx + JVX_EPSMINMAX_RAWRAM);
	replot_rawram();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void
myCentralWidget::changed_maxy_rawram()
{
	QString txt = lineEdit_ymax_rawram->text();
	plotOptionsRawram.maxy = txt.toData();
	plotOptionsRawram.miny = JVX_MIN(plotOptionsRawram.miny, plotOptionsRawram.maxy - JVX_EPSMINMAX_RAWRAM);
	replot_rawram();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

void
myCentralWidget::changed_maxx_rawram()
{
	QString txt = lineEdit_xmax_rawram->text();
	plotOptionsRawram.maxx = txt.toData();
	plotOptionsRawram.minx = JVX_MIN(plotOptionsRawram.minx, plotOptionsRawram.maxx - JVX_EPSMINMAX_RAWRAM);
	replot_rawram();
	inform_update_window((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
