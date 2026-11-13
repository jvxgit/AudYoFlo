#define JVX_LOCAL_ASSERT_PROPERTIES_LEAVE true

#include "CjvxQtMeasureAudio_spectrum.h"
#include <iostream>

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtSpecificHWidget, jvxQtMeasureAudioSpectrum, CjvxQtMeasureAudio_spectrum)
/*
jvxErrorType init_IjvxQtMeasureAudio_jitter(IjvxQtSpecificWidget** returnWidget, QWidget* parent)
{
	if(returnWidget)
	{
		*returnWidget = new CjvxQtMeasureAudio_jitter(parent);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
*/
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtSpecificHWidget, jvxQtMeasureAudioSpectrum)

/*jvxErrorType terminate_IjvxQtMeasureAudio_jitter(IjvxQtSpecificWidget* returnWidget)
{
	if(returnWidget)
	{
		delete(returnWidget);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
*/
// ====================================================================================

CjvxQtMeasureAudio_spectrum::CjvxQtMeasureAudio_spectrum(QWidget* parent): QWidget(parent)
{
	seqRunning = false;
	propRef = NULL;

	// "expose_plot"
	glCfg_pri.prop_segx = "/expose_plot/segmentsize_x";
	glCfg_pri.prop_segy = "/expose_plot/segmentsize_y";
	glCfg_pri.prop_frmt = "/expose_plot/format";
	glCfg_pri.prop_sfrmt = "/expose_plot/subformat";
	glCfg_pri.prop_omode = "/expose_plot/operation_mode";
	glCfg_pri.prop_nbufs = "/expose_plot/number_buffers";
	glCfg_pri.prop_rtrgt = "/expose_plot/rendering_target";

	glCfg_sec.prop_segx = "/expose_plot/segmentsize_x";
	glCfg_sec.prop_segy = "/expose_plot/segmentsize_y";
	glCfg_sec.prop_frmt = "/expose_plot/format";
	glCfg_sec.prop_sfrmt = "/expose_plot/subformat";
	glCfg_sec.prop_omode = "/expose_plot/operation_mode";
	glCfg_sec.prop_nbufs = "/expose_plot/number_buffers";
	glCfg_sec.prop_rtrgt = "/expose_plot/rendering_target_sec";
}

CjvxQtMeasureAudio_spectrum::~CjvxQtMeasureAudio_spectrum()
{
}

void
CjvxQtMeasureAudio_spectrum::init(IjvxHost* theHostRef, jvxCBitField mode, jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd )
{
	jvxSize i;
	this->setupUi(this);

	theHost = theHostRef;
	seqRunning = false;

	openGLWidget_pri = new CjvxMyGlWidget(this);
	openGLWidget_sec = new CjvxMyGlWidget(this);

	// Axis grid overlay: 8 divisions in x, 4 divisions in y
	jvxVideoOverlayConfig cfgOvl = openGLWidget_pri->overlayConfig();
	cfgOvl.active = true;
	cfgOvl.num_div_x = 8;
	cfgOvl.num_div_y = 4;

#if 0
	// We will get these entries lateron
	// 
	// Tick labels: num_div + 1 entries label every tick. x: left to right, y: bottom to top.
	// Normalized values for now - physical values (fs, fft size, time span) are only known at runtime
	cfgOvl.labels_x = { "0", "0.125", "0.25", "0.375", "0.5", "0.625", "0.75", "0.875", "1" };
	cfgOvl.labels_y = { "0", "0.25", "0.5", "0.75", "1" };

	cfgOvl.axis_label_x = "x (normalized)";
	cfgOvl.axis_label_y = "y (normalized)";
#endif

	// Primary display: "Waveform View" (lower group box), the signal is plotted in white.
	// Only this display shows a legend.
	cfgOvl.title = "Waveform";
	cfgOvl.legend = { { { 255, 255, 255, 255 }, "Signal x(k)" } };
	cfgOvl.legend_pos = JVX_RENDER_LEGEND_RIGHT_TOP;
	openGLWidget_pri->setOverlayConfig(cfgOvl);

	// Secondary display: "Spectral View" (upper group box), no legend
	cfgOvl.title = "Spectrum";
	cfgOvl.legend.clear();
	openGLWidget_sec->setOverlayConfig(cfgOvl);

	// Legend position selection, order as in newSelectionLegendPos
	comboBox_legend->addItem("Off");
	comboBox_legend->addItem("Top left");
	comboBox_legend->addItem("Top right");
	comboBox_legend->addItem("Bottom left");
	comboBox_legend->addItem("Bottom right");
	comboBox_legend->setCurrentIndex(2);
	connect(comboBox_legend, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &CjvxQtMeasureAudio_spectrum::newSelectionLegendPos);

	// Grid on / off for both views
	checkBox_grid->setChecked(cfgOvl.show_grid);
	connect(checkBox_grid, &QCheckBox::toggled, this, &CjvxQtMeasureAudio_spectrum::toggledGrid);

	QWidget* widgetRef_pri = NULL;
	openGLWidget_pri->getWidgetReferences(&widgetRef_pri);
	assert(widgetRef_pri);	
	QLayout* lay_pri = frame_glW_pri->layout();
	assert(lay_pri == NULL);
	QHBoxLayout* layN_pri = new QHBoxLayout;
	layN_pri->insertWidget(0, widgetRef_pri);
	frame_glW_pri->setLayout(layN_pri);
	
	QWidget* widgetRef_sec = NULL;
	openGLWidget_sec->getWidgetReferences(&widgetRef_sec);
	assert(widgetRef_sec);	
	QLayout* lay_sec = frame_glW_sec->layout();
	assert(lay_sec == NULL);
	QHBoxLayout* layN_sec = new QHBoxLayout;
	layN_sec->insertWidget(0, widgetRef_sec);
	frame_glW_sec->setLayout(layN_sec);
	checkBox_update->setChecked(true);

	// We need this function to trigger the initializeGL function as the window is
	// not visible when we start
	//openGLWidget->init_manually();
}

void
CjvxQtMeasureAudio_spectrum::getMyQWidget(QWidget**retWidget, jvxSize id)
{
	if(retWidget)
	{
		if (id == 0)
		{
			*retWidget = static_cast<QWidget*>(this);
		}
		else
		{
			*retWidget = NULL;
		}
	}
}

void
CjvxQtMeasureAudio_spectrum::terminate()
{
	theHost = NULL;
}

jvxErrorType
CjvxQtMeasureAudio_spectrum::addPropertyReference(IjvxAccessProperties* propRefIn, const std::string& prefixArg, const std::string& ident)
{
	if (propRef == NULL)
	{
		propRef = propRefIn;
		prefix = prefixArg;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void
CjvxQtMeasureAudio_spectrum::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string myPrefix = jvx_makePathExpr(prefix, "expose_plot");
	openGLWidget_pri->setConnectLinks(propRef, &glCfg_pri, jvx::JVX_EXTERNAL_BUFFER_SUB_2D_INC);
	openGLWidget_sec->setConnectLinks(propRef, &glCfg_sec, jvx::JVX_EXTERNAL_BUFFER_SUB_2D_INC);

}

void
CjvxQtMeasureAudio_spectrum::deactivate()
{
	openGLWidget_pri->clearConnectLinks();
	openGLWidget_sec->clearConnectLinks();
}

jvxErrorType
CjvxQtMeasureAudio_spectrum::removePropertyReference(IjvxAccessProperties* propRefIn)
{
	if (propRef)
	{
		propRef = NULL;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
	//jvx_initPropertyReferenceTriple(&theTripleAuN);
}

void
CjvxQtMeasureAudio_spectrum::processing_started()
{
	jvxErrorType res = JVX_NO_ERROR;

	// Primary
	res = this->openGLWidget_pri->inform_about_to_start([&](jvxVideoOverlayConfig& ovlConfig, jvxSize ll) -> bool
		{
			jvxCallManagerProperties callGate;
			jPAD ident;
			jPD trans;
			jvxData unit_x = 1;
			jvxData max_y_pri = 1;
			jvxData min_y_pri = 0;
			jvxApiString descr_x;
			jvxApiString descr_y_pri;

			std::string propStr = "/unit_x";
			propStr = jvx_makePathExpr(prefix, propStr);
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			propRef->get_property(callGate, jPRIO<jvxData>(unit_x), ident, trans);
		
			propStr = "/descr_x";
			propStr = jvx_makePathExpr(prefix, propStr);
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			propRef->get_property(callGate, jPROS(descr_x), ident, trans);

			propStr = "/max_y_pri";
			propStr = jvx_makePathExpr(prefix, propStr);
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			propRef->get_property(callGate, jPRIO<jvxData>(max_y_pri), ident, trans);

			propStr = "/min_y_pri";
			propStr = jvx_makePathExpr(prefix, propStr);
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			propRef->get_property(callGate, jPRIO<jvxData>(min_y_pri), ident, trans);

			propStr = "/descr_y_pri";
			propStr = jvx_makePathExpr(prefix, propStr);
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			propRef->get_property(callGate, jPROS(descr_y_pri), ident, trans);

			jvxSize nLabels = (ovlConfig.num_div_x + 1);
			ovlConfig.labels_x.resize(nLabels);
			auto elm = ovlConfig.labels_x.begin();
			for (jvxSize i = 0; i < nLabels; i++)
			{
				jvxData axisFull_sec = ll * unit_x;
				jvxData entryVal = axisFull_sec * i / (nLabels - 1);
				entryVal = -(axisFull_sec - entryVal);
				*elm = jvx_data2String(entryVal);
				elm++;
			}
			ovlConfig.axis_label_x = descr_x.std_str();

			nLabels = ovlConfig.num_div_y + 1;
			ovlConfig.labels_y.resize(nLabels);
			elm = ovlConfig.labels_y.begin();
			for (jvxSize i = 0; i < nLabels; i++)
			{
				jvxData axisFull = max_y_pri - min_y_pri;
				jvxData entryVal = axisFull * i / (nLabels - 1);
				entryVal = min_y_pri + entryVal;
				*elm = jvx_data2String(entryVal);
				elm++;
			}
			ovlConfig.axis_label_y = descr_y_pri.std_str();
			return true;
		});

	res = this->openGLWidget_sec->inform_about_to_start([&](jvxVideoOverlayConfig& ovlConfig, jvxSize ll) -> bool
		{
			jvxCallManagerProperties callGate;
			jPAD ident;
			jPD trans;
			jvxData unit_x = 1;
			jvxData max_y_pri = 1;
			jvxData min_y_pri = 0;
			jvxApiString descr_x;
			jvxApiString descr_y_pri;

			std::string propStr = "/unit_x";
			propStr = jvx_makePathExpr(prefix, propStr);
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			propRef->get_property(callGate, jPRIO<jvxData>(unit_x), ident, trans);

			propStr = "/descr_x";
			propStr = jvx_makePathExpr(prefix, propStr);
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			propRef->get_property(callGate, jPROS(descr_x), ident, trans);

			propStr = "/max_y_sec";
			propStr = jvx_makePathExpr(prefix, propStr);
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			propRef->get_property(callGate, jPRIO<jvxData>(max_y_pri), ident, trans);

			propStr = "/min_y_sec";
			propStr = jvx_makePathExpr(prefix, propStr);
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			propRef->get_property(callGate, jPRIO<jvxData>(min_y_pri), ident, trans);

			propStr = "/descr_y_sec";
			propStr = jvx_makePathExpr(prefix, propStr);
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			propRef->get_property(callGate, jPROS(descr_y_pri), ident, trans);

			jvxSize nLabels = ovlConfig.num_div_x;
			ovlConfig.labels_x.resize(nLabels);
			auto elm = ovlConfig.labels_x.begin();
			for (jvxSize i = 0; i < nLabels; i++)
			{
				jvxData axisFull_sec = ll * unit_x;
				jvxData entryVal = axisFull_sec * i / (nLabels - 1);
				entryVal = -(axisFull_sec - entryVal);
				*elm = jvx_data2String(entryVal);
				elm++;
			}
			ovlConfig.axis_label_x = descr_x.std_str();

			nLabels = ovlConfig.num_div_y + 1;
			ovlConfig.labels_y.resize(nLabels);
			elm = ovlConfig.labels_y.begin();
			for (jvxSize i = 0; i < nLabels; i++)
			{
				jvxData axisFull = max_y_pri - min_y_pri;
				jvxData entryVal = axisFull * i / (nLabels - 1);
				entryVal = min_y_pri + entryVal;
				*elm = jvx_data2String(entryVal);
				elm++;
			}
			ovlConfig.axis_label_y = descr_y_pri.std_str();
			return true;
		});
	
	seqRunning = true;
}

void
CjvxQtMeasureAudio_spectrum::processing_stopped()
{
	jvxSelectionList selList;
	openGLWidget_pri->inform_stopped();
	openGLWidget_sec->inform_stopped();
}

void
CjvxQtMeasureAudio_spectrum::update_window(jvxCBitField prio, const char* propLst)
{
	jvxSelectionList selList;

}

jvxErrorType 
CjvxQtMeasureAudio_spectrum::request_sub_interface(jvxQtInterfaceType, jvxHandle**)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxQtMeasureAudio_spectrum::return_sub_interface(jvxQtInterfaceType, jvxHandle*)
{
	return JVX_ERROR_UNSUPPORTED;
}

void
CjvxQtMeasureAudio_spectrum::update_window_periodic()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (checkBox_update->isChecked())
	{
		this->openGLWidget_pri->inform_update_window_periodic();
		this->openGLWidget_sec->inform_update_window_periodic();
	}
}

void
CjvxQtMeasureAudio_spectrum::newSelectionLegendPos(int idx)
{
	// Same order as the entries of comboBox_legend
	static const jvxRenderLegendPosition posLst[] =
	{
		JVX_RENDER_LEGEND_NOT_SHOWN,
		JVX_RENDER_LEGEND_LEFT_TOP,
		JVX_RENDER_LEGEND_RIGHT_TOP,
		JVX_RENDER_LEGEND_LEFT_BOTTOM,
		JVX_RENDER_LEGEND_RIGHT_BOTTOM
	};
	if ((idx >= 0) && (idx < (int)(sizeof(posLst) / sizeof(posLst[0]))))
	{
		jvxVideoOverlayConfig cfgOvl = openGLWidget_pri->overlayConfig();
		cfgOvl.legend_pos = posLst[idx];
		openGLWidget_pri->setOverlayConfig(cfgOvl);
	}
}

void
CjvxQtMeasureAudio_spectrum::toggledGrid(bool show)
{
	jvxVideoOverlayConfig cfgOvl = openGLWidget_pri->overlayConfig();
	cfgOvl.show_grid = show;
	openGLWidget_pri->setOverlayConfig(cfgOvl);

	cfgOvl = openGLWidget_sec->overlayConfig();
	cfgOvl.show_grid = show;
	openGLWidget_sec->setOverlayConfig(cfgOvl);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
