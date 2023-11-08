#include "jvxAcousticMeasure.h"
#include "jvxAcousticEqualizer.h"
#include "jvxExtractHrtfs.h"

#define JVX_QT_NUMBER_COLORS 5
#define JVX_QT_NUMBER_STYLES 3
#define JVX_QT_NUMBER_MARKERS 3
#define JVX_QT_NUMBER_WIDTHS 3

Q_DECLARE_METATYPE(jvxAcousticMeasure::oneRegisteredProcessor*)

extern "C"
{

#define FUNC_CORE_PROT_DECLARE jvx_acoustic_measure_init_submodule
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_acoustic_measure_init_submodule_local
#define FUNC_CORE_PROT_ARGS IjvxQtAcousticMeasurement_process** procOnReturn, QWidget* parentWidget, jvxSize idAlloc
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_acoustic_measure_init_submodule=jvx_acoustic_measure_init_submodule_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_acoustic_measure_init_submodule=_jvx_acoustic_measure_init_submodule_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		switch (idAlloc)
		{
		case 0:
			*procOnReturn = new jvxAcousticEqualizer(parentWidget);
			return JVX_NO_ERROR;
			break;
		case 1:
			*procOnReturn = new jvxExtractHrtfs(parentWidget);
			return JVX_NO_ERROR;
			break;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define FUNC_CORE_PROT_DECLARE jvx_acoustic_measure_terminate_submodule
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_acoustic_measure_terminate_submodule_local
#define FUNC_CORE_PROT_ARGS IjvxQtAcousticMeasurement_process* procOnReturn, jvxSize idAlloc
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_acoustic_measure_terminate_submodule=jvx_acoustic_measure_terminate_submodule_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_acoustic_measure_terminate_submodule=_jvx_acoustic_measure_terminate_submodule_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		switch (idAlloc)
		{
		case 0:
			delete procOnReturn;
			return JVX_NO_ERROR;
			break;
		case 1:
			delete procOnReturn;
			return JVX_NO_ERROR;
			break;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL
}


// ==============================================================================

typedef struct
{
	std::string nm;
	QColor col;
} oneColor;

static oneColor plot_colors[JVX_QT_NUMBER_COLORS] =
{
	{"black", Qt::black},
	{"gray", Qt::gray},
	{"green", Qt::green},
	{"cyan", Qt::darkCyan},
	{"magenta", Qt::magenta}
};

// =========================================================================

typedef struct
{
	std::string nm;
	QCPGraph::LineStyle style;
} oneStyle;

static oneStyle plot_styles[JVX_QT_NUMBER_STYLES] =
{
	{"line", QCPGraph::lsLine},
	{"impulse", QCPGraph::lsImpulse},
	{"none", QCPGraph::lsNone}
};

// =========================================================================

typedef struct
{
	std::string nm;
	QCPScatterStyle::ScatterShape marker;
} oneMarker;

static oneMarker plot_markers[JVX_QT_NUMBER_MARKERS] =
{
	{"none", QCPScatterStyle::ssNone},
	{"circle", QCPScatterStyle::ssCircle},
	{"square", QCPScatterStyle::ssSquare}
};

// =========================================================================

typedef struct
{
	std::string nm;
	jvxData width;
} oneWidth;

static oneWidth plot_widths[JVX_QT_NUMBER_WIDTHS] =
{
	{"1", 1},
	{"2", 2},
	{"3", 3}
};

// ======================================================================
// =========================================================================

jvxAcousticMeasure::jvxAcousticMeasure(QWidget* parent) : QWidget(parent)
{
	propRef = NULL;
	modeTd = JVX_PLOT_MODE_TD_IR;
	modeSec = JVX_PLOT_MODE_SEC_MAG;
	mouseMode = JVX_MOUSE_MODE_NONE;	
}

jvxAcousticMeasure::~jvxAcousticMeasure()
{
}

// ========================================================================	

void 
jvxAcousticMeasure::getMyQWidget(QWidget** retWidget, jvxSize id )
{
	*retWidget = NULL;
	switch (id)
	{
	case 0:
		*retWidget = static_cast<QWidget*>(this);
		break;
	default:
	{		
		auto elm = lstSubProcessors.begin();
		std::advance(elm, id - 1);
		if (elm != lstSubProcessors.end())
		{
			*retWidget = elm->proc->my_widget();
		}		
	}
	/*
	case 1:
		*retWidget = equalizerWidget->my_widget();
		break;
	case 2:
		*retWidget = hrtfWidget->my_widget();// static_cast<QWidget*> (hrtfWidget);
		break;
	*/
	}
}

// =====================================================================

void
jvxAcousticMeasure::init(IjvxHost* theHost, jvxCBitField mode, 
	jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd)
{
	jvxSize i;

	this->setupUi(this);

	this->setWindowTitle("Acoustic Measument - base");
	
	jvxSize cnt = 0;
	while (1)
	{
		IjvxQtAcousticMeasurement_process* retPtr = nullptr;
		jvxErrorType loopRes =
			jvx_acoustic_measure_init_submodule(&retPtr, this, cnt);
		if (loopRes == JVX_NO_ERROR)
		{
			if (retPtr)
			{
				oneProcessorAllocated newElm;
				newElm.proc = retPtr;
				newElm.idAlloc = cnt;
				lstSubProcessors.push_back(newElm);
			}
		}
		else
		{
			break;
		}
		cnt++;
	}

	for (i = 0; i < JVX_QT_NUMBER_COLORS; i++)
	{
		comboBox_col_p1->addItem(plot_colors[i].nm.c_str());
		comboBox_col_p2->addItem(plot_colors[i].nm.c_str());
	}

	for (i = 0; i < JVX_QT_NUMBER_STYLES; i++)
	{
		comboBox_style_p1->addItem(plot_styles[i].nm.c_str());
		comboBox_style_p2->addItem(plot_styles[i].nm.c_str());
	}

	for (i = 0; i < JVX_QT_NUMBER_MARKERS; i++)
	{
		comboBox_mar_p1->addItem(plot_markers[i].nm.c_str());
		comboBox_mar_p2->addItem(plot_markers[i].nm.c_str());
	}

	for (i = 0; i < JVX_QT_NUMBER_WIDTHS; i++)
	{
		comboBox_width_p1->addItem(plot_widths[i].nm.c_str());
		comboBox_width_p2->addItem(plot_widths[i].nm.c_str());
	}

	// ===========================================================

	qcp_timedomain->addGraph();
	qcp_timedomain->addGraph();

	// markers 1 and 2
	qcp_timedomain->addGraph();
	qcp_timedomain->addGraph();

	qcp_secondary->addGraph();
	qcp_secondary->addGraph();

	// markers 1 and 2
	qcp_secondary->addGraph();
	qcp_secondary->addGraph();

	qcp_timedomain->yAxis->setLabel("y(k)");
	qcp_timedomain->xAxis->setLabel("x(k)");
	qcp_timedomain->graph(0)->setName("Plot 1");
	qcp_timedomain->graph(1)->setName("Plot 2");
	qcp_timedomain->graph(2)->setName("Markers 1");
	qcp_timedomain->graph(2)->setVisible(false);
	qcp_timedomain->graph(3)->setName("Markers 2");
	qcp_timedomain->graph(3)->setVisible(false);
	qcp_timedomain->legend->setVisible(true);

	jvx_qcp_set_color_graph(qcp_timedomain, 2, Qt::red);
	jvx_qcp_set_color_graph(qcp_timedomain, 3, Qt::blue);

	qcp_secondary->yAxis->setLabel("y(k)");
	qcp_secondary->xAxis->setLabel("x(k)");
	qcp_secondary->graph(0)->setName("Plot 1");
	qcp_secondary->graph(1)->setName("Plot 2");
	qcp_secondary->graph(2)->setName("Markers 1");
	qcp_secondary->graph(2)->setVisible(false);
	qcp_secondary->graph(3)->setName("Markers 2");
	qcp_secondary->graph(3)->setVisible(false);
	qcp_secondary->legend->setVisible(true);

	jvx_qcp_set_color_graph(qcp_secondary, 2, Qt::red);
	jvx_qcp_set_color_graph(qcp_secondary, 3, Qt::blue);
	
	set_params_plots();
	
	// Update legends
	update_window();
	qcp_timedomain->replot();
	qcp_secondary->replot();

	comboBox_plot_mode_td->addItem("IR");
	comboBox_plot_mode_td->addItem("IR-LOG");
	comboBox_plot_mode_td->addItem("MEAS");
	comboBox_plot_mode_td->addItem("TEST");

	checkBox_show_plot1->setChecked(plot1.plotActive);
	checkBox_show_plot2->setChecked(plot2.plotActive);

	connect(qcp_timedomain, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mouse_press_td(QMouseEvent*)));
	connect(qcp_timedomain, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouse_release_td(QMouseEvent*)));
	connect(qcp_timedomain, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouse_move_td(QMouseEvent*)));
	connect(qcp_timedomain, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(mouse_dblclick_td(QMouseEvent*)));
	connect(qcp_timedomain, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouse_wheel_td(QWheelEvent*)));

	connect(qcp_secondary, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mouse_press_sec(QMouseEvent*)));
	connect(qcp_secondary, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouse_release_sec(QMouseEvent*)));
	connect(qcp_secondary, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouse_move_sec(QMouseEvent*)));
	connect(qcp_secondary, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(mouse_dblclick_sec(QMouseEvent*)));
	connect(qcp_secondary, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouse_wheel_sec(QWheelEvent*)));


	comboBox_scalex_type->addItem("Linear");
	comboBox_scalex_type->addItem("Log");

	comboBox_plot_sec->addItem("Mag");
	comboBox_plot_sec->addItem("Phase");
	comboBox_plot_sec->addItem("Delay");
	comboBox_plot_sec->addItem("Hist Delay");
	// comboBox_plot_sec->addItem("Phase");

	dataPlot1.data.show_freqdomain_valid = false;
	dataPlot2.data.show_freqdomain_valid = false;

	timedomain_markers.markers_1.show = false;
	timedomain_markers.markers_2.show = false;

	secondary_markers.markers_1.show = false;
	secondary_markers.markers_2.show = false;

	JVX_GET_TICKCOUNT_US_SETREF(&tStamp);
	switchMouseMode(mouseMode);
	reset_marker_td_edit();
	reset_marker_sec_edit();

	for (auto& elm : lstSubProcessors)
	{
		elm.proc->init(static_cast<IjvxQtAcousticMeasurement*>(this));
	}
	theHostRef = theHost;
}

void 
jvxAcousticMeasure::terminate()
{
	theHostRef = nullptr;

	jvxSize cnt = 0;
	for (auto& elm : lstSubProcessors)
	{
		elm.proc->terminate();
		jvx_acoustic_measure_terminate_submodule(elm.proc, elm.idAlloc);
	}
	lstSubProcessors.clear();
}

void 
jvxAcousticMeasure::activate()
{

}

void 
jvxAcousticMeasure::deactivate()
{

}

void 
jvxAcousticMeasure::processing_started()
{
}

void 
jvxAcousticMeasure::processing_stopped()
{
}

void 
jvxAcousticMeasure::update_window(jvxCBitField prio, const char* propLst )
{
	lineEdit_dataTag->setText(dataTag.c_str());
	
	lineEdit_td_xmax->setText(jvx_data2String(td.maxx, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
	lineEdit_td_xmin->setText(jvx_data2String(td.minx, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
	lineEdit_td_ymax->setText(jvx_data2String(td.maxy, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
	lineEdit_td_ymin->setText(jvx_data2String(td.miny, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());

	checkBox_td_xauto->setChecked(td.autox);
	checkBox_td_yauto->setChecked(td.autoy);

	if (td.autox)
	{
		lineEdit_td_xmax->setEnabled(false);
		lineEdit_td_xmin->setEnabled(false);
	}
	else
	{
		lineEdit_td_xmax->setEnabled(true);
		lineEdit_td_xmin->setEnabled(true);
	}
	if (td.autoy)
	{
		lineEdit_td_ymax->setEnabled(false);
		lineEdit_td_ymin->setEnabled(false);
	}
	else
	{
		lineEdit_td_ymax->setEnabled(true);
		lineEdit_td_ymin->setEnabled(true);
	}

	// ===========================================================================================

	lineEdit_fd_xmax->setText(jvx_data2String(sec.maxx, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
	lineEdit_fd_xmin->setText(jvx_data2String(sec.minx, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
	lineEdit_fd_ymax->setText(jvx_data2String(sec.maxy, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
	lineEdit_fd_ymin->setText(jvx_data2String(sec.miny, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());

	checkBox_fd_xauto->setChecked(sec.autox);
	checkBox_fd_yauto->setChecked(sec.autoy);

	if (sec.autox)
	{
		lineEdit_fd_xmax->setEnabled(false);
		lineEdit_fd_xmin->setEnabled(false);
	}
	else
	{
		lineEdit_fd_xmax->setEnabled(true);
		lineEdit_fd_xmin->setEnabled(true);
	}
	if (sec.autoy)
	{
		lineEdit_fd_ymax->setEnabled(false);
		lineEdit_fd_ymin->setEnabled(false);
	}
	else
	{
		lineEdit_fd_ymax->setEnabled(true);
		lineEdit_fd_ymin->setEnabled(true);
	}

	// ===========================================================================================

	comboBox_plot_mode_td->setCurrentIndex(modeTd);

	if (sec.plotLinearx)
	{
		comboBox_scalex_type->setCurrentIndex(0);
	}
	else
	{
		comboBox_scalex_type->setCurrentIndex(1);
	}

	comboBox_plot_sec->setCurrentIndex(modeSec);

	checkBox_sec_show_m1->setChecked(secondary_markers.markers_1.show);
	checkBox_sec_show_m2->setChecked(secondary_markers.markers_2.show);

	checkBox_td_legend->setChecked(td.showLegend);
	checkBox_sec_legend->setChecked(sec.showLegend);

	comboBox_process_select->clear();
	auto elm = registeredProcessors.begin();
	for (; elm != registeredProcessors.end(); elm++)
	{
		if (showThisProcessor(elm->first))
		{
			auto elmL = elm->second.begin();
			for (; elmL != elm->second.end(); elmL++)
			{
				oneRegisteredProcessor* elmPtr = &(*elmL);
				comboBox_process_select->addItem(elmL->descr.c_str(), QVariant::fromValue<oneRegisteredProcessor*>(elmPtr));
			}
		}
	}
}

void
jvxAcousticMeasure::update_window_periodic()
{
}

jvxErrorType
jvxAcousticMeasure::request_sub_interface(jvxQtInterfaceType ifTp, jvxHandle** retHdl)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (ifTp)
	{
	case jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_CONFIG:
		if (retHdl)
		{
			*retHdl = static_cast<IjvxQtSpecificHWidget_config_si*>(this);
		}
		res = JVX_NO_ERROR;
		break;	
	default:
		break;
	}
	return res;
}

jvxErrorType
jvxAcousticMeasure::return_sub_interface(jvxQtInterfaceType ifTp, jvxHandle* hdl)
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
	default:
		break;
	}
	return res;
}

// =====================================================================

jvxErrorType
jvxAcousticMeasure::addPropertyReference(IjvxAccessProperties* propRefArg, const std::string& prefixArg, const std::string& identArg)
{
	if (propRef == NULL)
	{
		propRef = propRefArg;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxAcousticMeasure::removePropertyReference(IjvxAccessProperties* propRefIn)
{
	propRef = NULL;
	return JVX_NO_ERROR;
}

// ==========================================================================

void
jvxAcousticMeasure::set_params_plots()
{
	QPen pen;
	QBrush br;
	QColor col = Qt::black;
	QCPGraph::LineStyle lstyle = QCPGraph::lsNone;
	jvxData lineWidth = 1;
	QCPScatterStyle sstyle = QCPScatterStyle::ssCircle;
	jvxSize markerSize = 5;

	col = plot_colors[plot1.optionColor].col;
	lstyle = plot_styles[plot1.optionStyle].style;
	sstyle = plot_markers[plot1.optionMarker].marker;
	lineWidth = plot_widths[plot1.optionWidth].width;
	sstyle.setSize(markerSize);

	pen = qcp_timedomain->graph(0)->pen();
	br = pen.brush();
	br.setColor(col);
	pen.setBrush(br);
	pen.setWidthF(lineWidth);
	qcp_timedomain->graph(0)->setPen(pen);
	qcp_timedomain->graph(0)->setLineStyle(lstyle);
	qcp_timedomain->graph(0)->setScatterStyle(sstyle);

	qcp_timedomain->graph(0)->setVisible(plot1.plotActive);

	pen = qcp_secondary->graph(0)->pen();
	br = pen.brush();
	br.setColor(col);
	pen.setBrush(br);
	pen.setWidthF(lineWidth);
	qcp_secondary->graph(0)->setPen(pen);
	qcp_secondary->graph(0)->setLineStyle(lstyle);
	qcp_secondary->graph(0)->setScatterStyle(sstyle);

	qcp_secondary->graph(0)->setVisible(plot1.plotActive);

	// =============================================================

	col = plot_colors[plot2.optionColor].col;
	lstyle = plot_styles[plot2.optionStyle].style;
	sstyle = plot_markers[plot2.optionMarker].marker;
	lineWidth = plot_widths[plot2.optionWidth].width;

	sstyle.setSize(markerSize);

	pen = qcp_timedomain->graph(0)->pen();
	br = pen.brush();
	br.setColor(col);
	pen.setBrush(br);
	pen.setWidthF(lineWidth);
	qcp_timedomain->graph(1)->setPen(pen);
	qcp_timedomain->graph(1)->setLineStyle(lstyle);
	qcp_timedomain->graph(1)->setScatterStyle(sstyle);

	qcp_timedomain->graph(1)->setVisible(plot2.plotActive);

	pen = qcp_secondary->graph(1)->pen();
	br = pen.brush();
	br.setColor(col);
	pen.setBrush(br);
	pen.setWidthF(lineWidth);
	qcp_secondary->graph(1)->setPen(pen);
	qcp_secondary->graph(1)->setLineStyle(lstyle);
	qcp_secondary->graph(1)->setScatterStyle(sstyle);

	qcp_secondary->graph(1)->setVisible(plot2.plotActive);

	qcp_timedomain->replot();
	qcp_secondary->replot();
}

jvxErrorType
jvxAcousticMeasure::registerConfigExtensions(IjvxConfigurationExtender* cfgExt)
{
	if (cfgExt)
	{
		nmRegConfig = this->windowTitle().toLatin1().data();
		nmRegConfig = jvx_replaceCharacter(nmRegConfig, ' ', '_');
		nmRegConfig = jvx_replaceCharacter(nmRegConfig, '-', '_');
		cfgExt->register_extension(static_cast<IjvxConfigurationExtender_report*>(this), nmRegConfig.c_str());
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvxAcousticMeasure::unregisterConfigExtensions(IjvxConfigurationExtender* cfgExt)
{
	if (cfgExt)
	{		
		cfgExt->unregister_extension(nmRegConfig.c_str());
		nmRegConfig.clear();
	}
	return JVX_NO_ERROR;

}

/*
jvxErrorType
jvxAcousticMeasure::request_configuration_ext_report(IjvxConfigurationExtender_report** theConfigExtenderReport)
{
	if (theConfigExtenderReport)
	{
		*theConfigExtenderReport = static_cast<IjvxConfigurationExtender_report*>(this);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
jvxAcousticMeasure::return_configuration_ext_report(IjvxConfigurationExtender_report* theConfigExtenderReport)
{
	if (theConfigExtenderReport == static_cast<IjvxConfigurationExtender_report*>(this))
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}
*/

// Need to put this function here to use the QT META TYPE

void
jvxAcousticMeasure::trigger_processor()
{
	jvxSize idx = comboBox_process_select->currentIndex();
	QVariant var = comboBox_process_select->itemData(idx);
	if (var.isValid())
	{
		oneRegisteredProcessor* proc = var.value<oneRegisteredProcessor*>();
		if (proc)
		{
			switch (proc->task)
			{
			case JVX_ACOUSTIC_MEASURE_TASK_EQUALIZE:
				trigger_proc_equalizer(proc->proc, proc->tagId);
				break;
			case JVX_ACOUSTIC_MEASURE_TASK_DELAY:
			case JVX_ACOUSTIC_MEASURE_TASK_STORE_DELAY:
			case JVX_ACOUSTIC_MEASURE_TASK_STORE_EQUALIZER:
				break;
			case JVX_ACOUSTIC_MEASURE_TASK_PASS_TWO_MEASURED_IRS:
				trigger_proc_pass2ir(proc->proc, proc->tagId);
				break;
			}
		}
	}
}

void
jvxAcousticMeasure::trigger_proc_equalizer(IjvxQtAcousticMeasurement_process* proc, jvxSize tagId)
{
	jvxMeasurementTaskEqualize taskData;

	dataPlot12 = dataPlot1; 
	dataPlot12.relink();

	taskData.ir_processed = dataPlot12.data.ir.data();
	taskData.ir_measured = dataPlot1.oneChan.bufIr;
	taskData.len_ir = dataPlot1.oneChan.lBuf;

	taskData.len_markers_measured = secondary_markers.markers_1.markers_x.size();
	taskData.markers_measured_x = secondary_markers.markers_1.markers_x.data();
	taskData.markers_measured_y = secondary_markers.markers_1.markers_y.data();

	taskData.len_markers_desired = secondary_markers.markers_2.markers_x.size();
	taskData.markers_desired_x = secondary_markers.markers_2.markers_x.data();
	taskData.markers_desired_y = secondary_markers.markers_2.markers_y.data();

	taskData.dataTag = dataTag.c_str();
	taskData.measName = dataPlot1.measurement_name.c_str();
	taskData.chanName = dataPlot1.channel_name.c_str();

	taskData.samplerate = dataPlot1.oneChan.rate;
	proc->process_data(JVX_ACOUSTIC_MEASURE_TASK_EQUALIZE, &taskData, tagId);
}

void
jvxAcousticMeasure::trigger_proc_pass2ir(IjvxQtAcousticMeasurement_process* proc, jvxSize tagId)
{
	jvxMeasurementTaskPass2Ir taskData;
	jvxBool readyForProcess = true;

	if (!dataPlot1.oneChan.lBuf)
	{
		readyForProcess = false;
	}
	taskData.ir_data1 = dataPlot1.oneChan.bufIr;
	taskData.ir_data1_len = dataPlot1.oneChan.lBuf;
	taskData.ir_data1_rate = dataPlot1.oneChan.rate;
	taskData.ir_data1_meas_name = dataPlot1.measurement_name.c_str();
	taskData.ir_data1_chan_name = dataPlot1.channel_name.c_str();

	if (!dataPlot2.oneChan.lBuf)
	{
		readyForProcess = false;
	}
	taskData.ir_data2 = dataPlot2.oneChan.bufIr;
	taskData.ir_data2_len = dataPlot2.oneChan.lBuf;
	taskData.ir_data2_rate = dataPlot2.oneChan.rate;
	taskData.ir_data2_meas_name = dataPlot2.measurement_name.c_str();
	taskData.ir_data2_chan_name = dataPlot2.channel_name.c_str();

	if (readyForProcess)
	{
		proc->process_data(JVX_ACOUSTIC_MEASURE_TASK_PASS_TWO_MEASURED_IRS, &taskData, tagId);
	}
}
		