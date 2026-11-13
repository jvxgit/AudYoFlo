#define JVX_LOCAL_ASSERT_PROPERTIES_LEAVE true

#include "CjvxQtMeasureAudio_jitter.h"
#include <iostream>
#include <QtWidgets/QFileDialog>

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#define EPS_SHOW 1e-5

#define NUM_COLORS 7
typedef struct
{
	QColor col;
	std::string descr;
} oneColor;

static oneColor givenColors[NUM_COLORS] =
{
	{Qt::black, "black"},
	{Qt::green, "green"},
	{Qt::red, "red"},
	{Qt::blue, "blue"},
	{Qt::gray, "gray"},
	{Qt::yellow, "yellow"},
	{Qt::magenta, "magenta"}
};

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtSpecificHWidget, jvxQtMeasureAudioJitter, CjvxQtMeasureAudio_jitter)
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
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtSpecificHWidget, jvxQtMeasureAudioJitter)

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

CjvxQtMeasureAudio_jitter::CjvxQtMeasureAudio_jitter(QWidget* parent): QWidget(parent)
{
	seqRunning = false;
	propRef = NULL;
}

CjvxQtMeasureAudio_jitter::~CjvxQtMeasureAudio_jitter()
{
}

void
CjvxQtMeasureAudio_jitter::init(IjvxHost* theHostRef, jvxCBitField mode, jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd )
{
	jvxSize i;
	this->setupUi(this);
	theHost = theHostRef;
	//jvx_initPropertyReferenceTriple(&theTripleAuN);
	seqRunning = false;
	comboBox_waveformType->addItem("Sine");
	comboBox_waveformType->addItem("Rect");
	comboBox_waveformType->addItem("Ramp");

#ifdef JVX_PLOTS_QWT

	// Init stuff for qwt
	QwtText txt;
	QFont ft = txt.font();
	ft.setPixelSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	txt.setFont(ft);

	//==============================================================
	// INIT PLOT ENGINE (Linear PLOT)
	//==============================================================
	qwtPlot_sig->setCanvasBackground( QColor( Qt::white ) );

	QFont font = qwtPlot_sig->axisFont(QwtPlot::xBottom);
    font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
    qwtPlot_sig->setAxisFont (QwtPlot::xBottom, font);
    qwtPlot_sig->setAxisFont (QwtPlot::yLeft, font);

	font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	txt.setFont(font);
	txt.setText("Index");
    qwtPlot_sig->setAxisTitle( QwtPlot::xBottom, txt);
	txt.setText("Value");
    qwtPlot_sig->setAxisTitle( QwtPlot::yLeft,txt );

	qwtPlot_sig->setAutoDelete(false);

	// Setup a grid
	rtView_channel.theGrid = new QwtPlotGrid;
	rtView_channel.theGrid->setMajorPen( QPen( Qt::gray, 1, Qt::DotLine ) );
    rtView_channel.theGrid->setMinorPen( QPen( Qt::gray, 1 , Qt::DotLine ) );
	rtView_channel.theGrid->attach( qwtPlot_sig );
	rtView_channel.theCurve = new QwtPlotCurve();
	rtView_channel.theCurve->attach(qwtPlot_sig);

	rtView_timing.theGrid = new QwtPlotGrid;
	rtView_timing.theGrid->setMajorPen( QPen( Qt::gray, 1, Qt::DotLine ) );
    rtView_timing.theGrid->setMinorPen( QPen( Qt::gray, 1 , Qt::DotLine ) );
	rtView_timing.theGrid->attach( qwtPlot_sig );
	rtView_timing.theCurve = new QwtPlotCurve();
	rtView_timing.theCurve->attach(qwtPlot_timing);

#else

#ifdef JVX_PERMIT_QCP_OPENGL
	widget_channel_qcp->setOpenGl(true);
	if (!widget_channel_qcp->openGl())
	{
		std::cout << "Failed to activate openGL rendering!" << std::endl;
	}
#endif
	widget_channel_qcp->addGraph();
	rtView_channel.secBufferY.resize(NUMBERPIXELS_PLOT_SIGNAL);
	rtView_channel.secBufferX.resize(NUMBERPIXELS_PLOT_SIGNAL);
	widget_channel_qcp->xAxis->setLabel("x(k)");
	widget_channel_qcp->yAxis->setLabel("Index k");
	widget_channel_qcp->xAxis->setRange(0, NUMBERPIXELS_PLOT_SIGNAL - 1);
	widget_channel_qcp->graph(0)->setPen(QPen(Qt::black));
	widget_channel_qcp->graph(0)->setName("Input Signal");
	widget_channel_qcp->legend->setVisible(true);

	// ===========================================================
#ifdef JVX_PERMIT_QCP_OPENGL
	widget_timing_qcp->setOpenGl(true);
	if (!widget_timing_qcp->openGl())
	{
		std::cout << "Failed to activate openGL rendering!" << std::endl;
	}
#endif

	widget_timing_qcp->addGraph();
	rtView_timing.secBufferY.resize(NUMBERPIXELS_PLOT_SIGNAL);
	rtView_timing.secBufferX.resize(NUMBERPIXELS_PLOT_SIGNAL);
	widget_timing_qcp->xAxis->setLabel("Detla T [msec]");
	widget_timing_qcp->yAxis->setLabel("Time");
	widget_timing_qcp->xAxis->setRange(0, NUMBERPIXELS_PLOT_SIGNAL-1);
	widget_timing_qcp->graph(0)->setPen(QPen(Qt::black));
	widget_timing_qcp->graph(0)->setName("Delta T Bufferswitch");
	widget_timing_qcp->legend->setVisible(true);
#endif

	for(i = 0; i < NUMBERPIXELS_PLOT_SIGNAL; i++)
	{
		rtView_channel.secBufferY[JVX_SIZE_INT(i)] = 0.0;
		rtView_channel.secBufferX[JVX_SIZE_INT(i)] = (jvxData)i;
	}

	for(i = 0; i < NUMBERPIXELS_PLOT_SIGNAL; i++)
	{
		rtView_timing.secBufferY[JVX_SIZE_INT(i)] = 0.0;
		rtView_timing.secBufferX[JVX_SIZE_INT(i)] = (jvxData)i;
	}
}

void
CjvxQtMeasureAudio_jitter::getMyQWidget(QWidget**retWidget, jvxSize id)
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
CjvxQtMeasureAudio_jitter::terminate()
{
	theHost = NULL;
}

jvxErrorType
CjvxQtMeasureAudio_jitter::addPropertyReference(IjvxAccessProperties* propRefIn, const std::string& prefixArg, const std::string& identArg)
{
	if (propRef == NULL)
	{
		propRef = propRefIn;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void
CjvxQtMeasureAudio_jitter::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	trigger_prepareMeasureTimingMode();
}

void
CjvxQtMeasureAudio_jitter::deactivate()
{

}

jvxErrorType
CjvxQtMeasureAudio_jitter::removePropertyReference(IjvxAccessProperties* propRefIn)
{
	if (propRef)
	{
		propRef = NULL;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void
CjvxQtMeasureAudio_jitter::processing_started()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw;
	
	std::string token;
	rtView_channel.fldPlot = jvx_allocate1DCircExternalBuffer(NUMBERPIXELS_PLOT_SIGNAL, 1, JVX_DATAFORMAT_DATA, JVX_DATAFORMAT_GROUP_AUDIO_PCM_NONINTERLEAVED,
		jvx_static_lock, jvx_static_try_lock, jvx_static_unlock, &rtView_channel.szPlot);
	rtView_channel.isValidPlot = true;
	rtView_timing.fldPlot = jvx_allocate1DCircExternalBuffer(NUMBERPIXELS_PLOT_SIGNAL, 1, JVX_DATAFORMAT_DATA, JVX_DATAFORMAT_GROUP_AUDIO_PCM_NONINTERLEAVED,
		jvx_static_lock, jvx_static_try_lock, jvx_static_unlock, &rtView_timing.szPlot);
	rtView_timing.isValidPlot = true;

	JVX_GET_TICKCOUNT_US_SETREF(&tStamp);

	if(propRef)
	{
		token = "/measuretiming/runtime/linearBuffer_channel";
		ptrRaw.reset(rtView_channel.fldPlot, &rtView_channel.isValidPlot, rtView_channel.fldPlot->formFld);
		ident.reset(token.c_str());

		res = propRef->install_referene_property(callGate, ptrRaw, ident);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		token = "/measuretiming/runtime/linearBuffer_timing";
		ptrRaw.reset(rtView_timing.fldPlot, &rtView_timing.isValidPlot, rtView_timing.fldPlot->formFld);
		ident.descriptor = token.c_str();
		res = propRef->install_referene_property(callGate, ptrRaw, ident);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
	}

	seqRunning = true;

	comboBox_waveformType->setEnabled(false);
}

void
CjvxQtMeasureAudio_jitter::processing_stopped()
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	std::string command;
	jvxCallManagerProperties callGate;
	std::string token;
	seqRunning = false;
	jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw;
	comboBox_waveformType->setEnabled(true);
	if(rtView_channel.isValidPlot)
	{
		if(propRef)
		{
			token = "/measuretiming/runtime/linearBuffer_channel";
			ptrRaw.reset(rtView_channel.fldPlot, &rtView_channel.isValidPlot,rtView_channel.fldPlot->formFld);
			ident.reset(token.c_str());
			res = propRef->uninstall_referene_property(callGate, ptrRaw, ident);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		}
		jvx_deallocateExternalBuffer(rtView_channel.fldPlot);
		rtView_channel.fldPlot = NULL;
		rtView_channel.isValidPlot = false;
		rtView_channel.szPlot = 0;
	}
	if(rtView_timing.isValidPlot)
	{
		if(propRef)
		{
			token = "/measuretiming/runtime/linearBuffer_timing";
			ptrRaw.reset(rtView_timing.fldPlot, &rtView_timing.isValidPlot, rtView_timing.fldPlot->formFld);
			ident.reset(token.c_str());
			res = propRef->uninstall_referene_property(callGate, ptrRaw, ident);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		}
		jvx_deallocateExternalBuffer(rtView_timing.fldPlot);
		rtView_timing.fldPlot = NULL;
		rtView_timing.isValidPlot = false;
		rtView_timing.szPlot = 0;
	}
}

void
CjvxQtMeasureAudio_jitter::update_window(jvxCBitField prio, const char* propLst )
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isMeasureIr = false;
	jvxData valD = 0;
	jvxInt32 valI32 = 0;
	jvxSize i;
	jvxCallManagerProperties callGate;
	std::string token;
	jvxSize selId = JVX_SIZE_UNSELECTED;
	if(propRef)
	{
		selList.bitFieldSelected() = 0;

			// Show the propper output wave
		token = "/WaveGenerators/type_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate,
			jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans );
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
			if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
			{
				// Sine
				comboBox_waveformType->setCurrentIndex(0);
			}
			else if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x2))
			{
				// Rect
				comboBox_waveformType->setCurrentIndex(1);
			}
			else if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x4))
			{
				// Ramp
				comboBox_waveformType->setCurrentIndex(2);
			}
			else
			{
				assert(0);
			}

			token = "/WaveGenerators/amplitude_activeGenerator";
			ident.reset(token.c_str());
			trans.reset(true);

			res = propRef->get_property(callGate, jPRG( &valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
			lineEdit_amplitude->setText(jvx_data2String(valD, 3).c_str());

			valI32 = (int)(valD * 100.0);
			valI32 = JVX_MIN(100, valI32);
			horizontalSlider_amplitude->setValue(valI32);

			token = "/JVX_AUDIONODE_SAMPLERATE";
			ident.reset(token.c_str());
			trans.reset(true);

			propRef->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

			token = "/WaveGenerators/frequency_activeGenerator";
			ident.reset(token.c_str());
			trans.reset(true);
			res = propRef->get_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
			lineEdit_frequency->setText(jvx_data2String(valD, 3).c_str());
			valD = 2 * valD / (jvxData)valI32;
			valI32 = JVX_DATA2INT32(valD * 100.0);
			valI32 = JVX_MIN(100, valI32);
			horizontalSlider_frequency->setValue(valI32);

			token = "/WaveGenerators/outputChannel_activeGenerator";
			ident.reset(token.c_str());
			trans.reset();
			res = propRef->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
			comboBox_outputchannel->clear();
			for (i = 0; i < selList.strList.ll(); i++)
			{
				std::string txt = selList.strList.std_str_at(i);
				if (jvx_bitTest(selList.bitFieldSelected(), i))
				{
					txt = txt + "*";
				}
				comboBox_outputchannel->addItem(txt.c_str());
			}

			token = "/measuretiming/runtime/selection_outputSignal";
			ident.reset(token.c_str());
			trans.reset();
			res = propRef->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
			comboBox_observeChannel->clear();
			comboBox_observeChannel->addItem("None");
			for (i = 0; i < selList.strList.ll(); i++)
			{
				std::string txt = selList.strList.std_str_at(i);
				if (jvx_bitTest(selList.bitFieldSelected(), i))
				{
					txt = txt + "*";
				}
				comboBox_observeChannel->addItem(txt.c_str());
			}
			selId = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), selList.strList.ll()) + 1;
			if (JVX_CHECK_SIZE_SELECTED(selId))
			{
				comboBox_observeChannel->setCurrentIndex(valI32);
			}
	}
}

jvxErrorType 
CjvxQtMeasureAudio_jitter::request_sub_interface(jvxQtInterfaceType, jvxHandle**)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxQtMeasureAudio_jitter::return_sub_interface(jvxQtInterfaceType, jvxHandle*)
{
	return JVX_ERROR_UNSUPPORTED;
}

static jvxUInt64 diffs[100];
static int cnt = 0;
void
CjvxQtMeasureAudio_jitter::update_window_periodic()
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isMeasureIr = false;
	jvxData valD1 = 0;
	jvxData valD2 = 0;
	jvxCBool valB = false;
	//jvxUInt64  tStart, tStop;
	jvxSize i;
	jvxCallManagerProperties callGate;
	std::string token;

	if(propRef)
	{
		token = "/measuretiming/results/mindeltat_Timing";
		ident.reset(token.c_str());
		trans.reset();
		res = propRef->get_property(callGate, jPRG(&valD1, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		token = "/measuretiming/results/maxdeltat_Timing";
		ident.reset(token.c_str());
		trans.reset();
		res = propRef->get_property(callGate, jPRG(&valD2, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		std::string txt = jvx_data2String(valD1,2) + "/" + jvx_data2String(valD2,2) + " msec";
		lineEdit_minmaxDeltaT->setText(txt.c_str());

		token = "/measuretiming/results/avrgdeltat_Timing";
		ident.reset(token.c_str());
		trans.reset();
		res = propRef->get_property(callGate, jPRG(&valD1, 01, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		txt = jvx_data2String(valD1,2)  + " msec";;
		lineEdit_avrgDeltaT->setText(txt.c_str());

		token = "/measuretiming/results/vardeltat_Timing";
		ident.reset(token.c_str());
		trans.reset();
		res = propRef->get_property(callGate, jPRG(&valD1, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		txt = jvx_data2String(valD1,2)  + " msec";;
		lineEdit_varianceDeltaT->setText(txt.c_str());

		token = "/measuretiming/results/theodeltat_Timing";
		ident.reset(token.c_str());
		trans.reset();
		res = propRef->get_property(callGate, jPRG(&valD1, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		txt = jvx_data2String(valD1,2)  + " msec";;
		lineEdit_theoDeltaT->setText(txt.c_str());

		while (1)
		{
			jvxSize numSamplesNew = rtView_channel.fldPlot->fill_height;
			jvxSize offsetCopy = rtView_channel.fldPlot->idx_read;
			jvxData* buf = (jvxData*)(rtView_channel.fldPlot->ptrFld);
			buf += offsetCopy;
			numSamplesNew = JVX_MIN(rtView_channel.fldPlot->length - offsetCopy, numSamplesNew);

			if (numSamplesNew)
			{
#ifdef JVX_PLOTS_QWT
				memmove(rtView_channel.secBufferY, rtView_channel.secBufferY + (numSamplesNew), (NUMBERPIXELS_PLOT_SIGNAL - numSamplesNew) * sizeof(jvxData));
				memcpy(rtView_channel.secBufferY + (NUMBERPIXELS_PLOT_SIGNAL - numSamplesNew), buf, numSamplesNew * sizeof(jvxData));
#else
				memmove(rtView_channel.secBufferY.data(), rtView_channel.secBufferY.data() + (numSamplesNew), (NUMBERPIXELS_PLOT_SIGNAL - numSamplesNew) * sizeof(double)); // It is double by intention!!
#ifdef JVX_SYSTEM_USE_DATA_FORMAT_FLOAT
				for (i = 0; i < (numSamplesNew); i++)
				{
					rtView_channel.secBufferY[i + NUMBERPIXELS_PLOT_SIGNAL - numSamplesNew] = (double)buf[i];
				}

#else
				memcpy(rtView_channel.secBufferY.data() + (NUMBERPIXELS_PLOT_SIGNAL - numSamplesNew), buf, numSamplesNew * sizeof(jvxData));
#endif
#endif
				rtView_channel.fldPlot->safe_access.lockf(rtView_channel.fldPlot->safe_access.priv);
				rtView_channel.fldPlot->fill_height -= numSamplesNew;
				rtView_channel.fldPlot->idx_read = (rtView_channel.fldPlot->idx_read + numSamplesNew) % rtView_channel.fldPlot->length;
				rtView_channel.fldPlot->safe_access.unlockf(rtView_channel.fldPlot->safe_access.priv);
			}
			else
			{
				break;
			}
		}

		jvxData ymax = JVX_DATA_MAX_NEG;
		jvxData ymin = JVX_DATA_MAX_POS;

		for (i = 0; i < NUMBERPIXELS_PLOT_SIGNAL; i++)
		{
			if (rtView_channel.secBufferY[JVX_SIZE_INT(i)] > ymax)
			{
				ymax = rtView_channel.secBufferY[JVX_SIZE_INT(i)];
			}
			if (rtView_channel.secBufferY[JVX_SIZE_INT(i)] < ymin)
			{
				ymin = rtView_channel.secBufferY[JVX_SIZE_INT(i)];
			}
		}

		if (checkBox_updateChannel->isChecked())
		{

#ifdef JVX_PLOTS_QWT
			rtView_channel.theCurve->setSamples(rtView_channel.secBufferX, rtView_channel.secBufferY, NUMBERPIXELS_PLOT_SIGNAL);
			qwtPlot_sig->setAxisScale(QwtPlot::xBottom, 0, NUMBERPIXELS_PLOT_SIGNAL - 1);
			ymin = -1;
			ymax = +1;
			//ymax = JVX_MAX(ymax, ymin + EPS_SHOW);
			qwtPlot_sig->setAxisScale(QwtPlot::yLeft, ymin, ymax);

			qwtPlot_sig->replot();
#else
			widget_channel_qcp->graph(0)->setData(rtView_channel.secBufferX, rtView_channel.secBufferY);
			ymin = -1;
			ymax = +1;
			widget_channel_qcp->yAxis->setRange(ymin, ymax);
			widget_channel_qcp->replot();
#endif
		}
	


		while (1)
		{
			jvxSize numSamplesNew = rtView_timing.fldPlot->fill_height;
			jvxSize offsetCopy = rtView_timing.fldPlot->idx_read;
			jvxData* buf = (jvxData*)(rtView_timing.fldPlot->ptrFld);
			buf += offsetCopy;
			numSamplesNew = JVX_MIN(rtView_timing.fldPlot->length - offsetCopy, numSamplesNew);

			if (numSamplesNew)
			{
#ifdef JVX_PLOTS_QWT
				memmove(rtView_timing.secBufferY, rtView_timing.secBufferY + (numSamplesNew), (NUMBERPIXELS_PLOT_SIGNAL - numSamplesNew) * sizeof(jvxData));
				memcpy(rtView_timing.secBufferY + (NUMBERPIXELS_PLOT_SIGNAL - numSamplesNew), buf, numSamplesNew * sizeof(jvxData));
#else
				memmove(rtView_timing.secBufferY.data(), rtView_timing.secBufferY.data() + (numSamplesNew), (NUMBERPIXELS_PLOT_SIGNAL - numSamplesNew) * sizeof(double));// It is double by intention!!

#ifdef JVX_SYSTEM_USE_DATA_FORMAT_FLOAT
				for (i = 0; i < (numSamplesNew); i++)
				{
					rtView_timing.secBufferY[i + NUMBERPIXELS_PLOT_SIGNAL - numSamplesNew] = (double)buf[i];
				}

#else
				memcpy(rtView_timing.secBufferY.data() + (NUMBERPIXELS_PLOT_SIGNAL - numSamplesNew), buf, numSamplesNew * sizeof(jvxData));
#endif
#endif
				rtView_timing.fldPlot->safe_access.lockf(rtView_timing.fldPlot->safe_access.priv);
				rtView_timing.fldPlot->fill_height -= numSamplesNew;
				rtView_timing.fldPlot->idx_read = (rtView_timing.fldPlot->idx_read + numSamplesNew) % rtView_timing.fldPlot->length;
				rtView_timing.fldPlot->safe_access.unlockf(rtView_timing.fldPlot->safe_access.priv);
			}
			else
			{
				break;
			}
		}
		
		ymax = JVX_DATA_MAX_NEG;
		ymin = JVX_DATA_MAX_POS;

		for (i = 0; i < NUMBERPIXELS_PLOT_SIGNAL; i++)
		{
			if (rtView_timing.secBufferY[JVX_SIZE_INT(i)] > ymax)
			{
				ymax = rtView_timing.secBufferY[JVX_SIZE_INT(i)];
			}
			if (rtView_timing.secBufferY[JVX_SIZE_INT(i)] < ymin)
			{
				ymin = rtView_timing.secBufferY[JVX_SIZE_INT(i)];
			}
		}
		ymin = JVX_MIN(0, ymin);

		if (checkBox_updateTiming->isChecked())
		{
#ifdef JVX_PLOTS_QWT
			rtView_timing.theCurve->setSamples(rtView_timing.secBufferX, rtView_timing.secBufferY, NUMBERPIXELS_PLOT_SIGNAL);
			qwtPlot_timing->setAxisScale(QwtPlot::xBottom, 0, NUMBERPIXELS_PLOT_SIGNAL - 1);

			ymin = 0;
			//ymax = JVX_MAX(ymax, ymin + EPS_SHOW);
			qwtPlot_timing->setAxisScale(QwtPlot::yLeft, ymin, ymax);

			qwtPlot_timing->replot();
#else
			//tStart = JVX_GET_TICKCOUNT_US_GET(tStamp);

			widget_timing_qcp->graph(0)->setData(rtView_timing.secBufferX, rtView_timing.secBufferY);
			widget_timing_qcp->yAxis->setRange(ymin, ymax);
			widget_timing_qcp->replot();

/*			while(1)
			{
				tStop = JVX_GET_TICKCOUNT_US_GET(tStamp);
				if(tStop >= tStart + 1000000)
				{
					break;
				}
			}
			
			tStop = JVX_GET_TICKCOUNT_US_GET(tStamp);
			//JVX_SLEEP_MS(200);
			tStop -= tStart;
			diffs[cnt] = tStop;
			cnt++;
			if(cnt == 100)
				cnt = 0;
				*/
#endif
		}

	}
}

void
CjvxQtMeasureAudio_jitter::trigger_prepareMeasureTimingMode()
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isMeasureIr = false;
	jvxCallManagerProperties callGate;
	std::string token;

	selList.bitFieldSelected() = 0x4;
	token = "/mode/currentMode";
	ident.reset(token.c_str());
	trans.reset(true);
	res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

	// Select the first wave generator
	selList.bitFieldSelected() = 0x2; // <- first choice is NONE
	token = "/WaveGenerators/selectGenerator";
	ident.reset(token.c_str());
	trans.reset(true);
	res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

	// Switch that generator wave to output sine
	selList.bitFieldSelected() = 0x1;
	token = "/WaveGenerators/type_activeGenerator";
	ident.reset(token.c_str());
	trans.reset(true);
	res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
}

void
CjvxQtMeasureAudio_jitter::selected_waveformType(int idMode)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	std::string token;
	if(propRef)
	{
		switch(idMode)
		{
		case 0:

			selList.bitFieldSelected() = 0x1;
			break;

		case 1:

			selList.bitFieldSelected() = 0x2;
			break;

		case 2:

			selList.bitFieldSelected() = 0x4;
			break;
		default:
			assert(0);

		}
		token = "/WaveGenerators/type_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}}

void
CjvxQtMeasureAudio_jitter::changed_amplitude()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD = 0;
	jvxCallManagerProperties callGate;
	std::string token;
	QString txt = lineEdit_amplitude->text();
	valD = (jvxData)txt.toDouble();

	if(propRef)
	{
		token = "/WaveGenerators/amplitude_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_jitter::modify_amplitude()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 valI32 = horizontalSlider_amplitude->value();
	jvxData valD = valI32 / 100.0;
	jvxCallManagerProperties callGate;
	std::string token;
	if(propRef)
	{
		token = "/WaveGenerators/amplitude_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_jitter::changed_frequency()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD = 0;
	jvxCallManagerProperties callGate;
	std::string token;
	QString txt = lineEdit_frequency->text();
	valD = (jvxData)txt.toDouble();

	if(propRef)
	{
		token = "/WaveGenerators/frequency_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_jitter::modify_frequency()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 valI32 = horizontalSlider_frequency->value();
	jvxData valD = valI32 / 100.0;
	jvxCallManagerProperties callGate;
	std::string token;
	if(propRef)
	{
		token = "/JVX_AUDIONODE_SAMPLERATE";
		ident.reset(token.c_str());
		trans.reset(true);
		propRef->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		valD = valD * valI32 /2;

		token = "/WaveGenerators/frequency_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_jitter::selected_output(int id)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	std::string token;
	if(propRef)
	{
		token = "/WaveGenerators/outputChannel_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		selList.bitFieldSelected() ^= ((jvxBitField)1 << id);
		token = "/WaveGenerators/outputChannel_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_jitter::clicked_outputallon()
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	std::string token;
	if(propRef)
	{
		token = "/WaveGenerators/outputChannel_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		selList.bitFieldSelected() = ((jvxBitField)1 << JVX_SIZE_INT32(selList.strList.ll())) -1;
		token = "/WaveGenerators/outputChannel_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		this->update_window();
	}
}

void
CjvxQtMeasureAudio_jitter::clicked_outputalloff()
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	std::string token;
	if(propRef)
	{
		selList.bitFieldSelected() = 0;
		token = "/WaveGenerators/outputChannel_activeGenerator";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		this->update_window();
	}
}

void
CjvxQtMeasureAudio_jitter::select_observationChannel(int id)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	std::string token;
	if(propRef)
	{
		if(id == 0)
		{
			selList.bitFieldSelected()  = 0;
		}
		else
		{
			selList.bitFieldSelected() = (jvxBitField)1 << (id-1);
		}
		token = "/measuretiming/runtime/selection_outputSignal";
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		this->update_window();
	}
}

void 
CjvxQtMeasureAudio_jitter::resizeEvent(QResizeEvent *event)
{
#ifndef JVX_PLOTS_QWT
	widget_channel_qcp->replot();
	widget_timing_qcp->replot();
#endif
}


#ifdef JVX_PROJECT_NAMESPACE
}
#endif
