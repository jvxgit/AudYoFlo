#define JVX_LOCAL_ASSERT_PROPERTIES_LEAVE true

#include "CjvxQtMeasureAudio_ir.h"
#include <iostream>
#include <QtWidgets/QFileDialog>

extern "C"
{
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_fft_tools.h"
}

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#define EPS_SHOW 1e-5
#define JVX_TOKEN_COUNTDOWN_OFF "--"

// ======================================================================
JVX_QT_WIDGET_INIT_DEFINE(IjvxQtMeasureAudio_ir, jvxQtMeasureAudioIr, CjvxQtMeasureAudio_ir)
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtMeasureAudio_ir, jvxQtMeasureAudioIr)

// ====================================================================================

CjvxQtMeasureAudio_ir::CjvxQtMeasureAudio_ir(QWidget* parent): QWidget(parent)
{
	Q_INIT_RESOURCE(jvx_audiotools);
	propRef = NULL;
	myMode = 0;

	availableColors.push_back(oneColorOption("black", Qt::black));
	availableColors.push_back(oneColorOption("green", Qt::green));
	availableColors.push_back(oneColorOption("red", Qt::red ));
	availableColors.push_back(oneColorOption("blue", Qt::blue ));
	availableColors.push_back(oneColorOption("gray", Qt::gray));
	availableColors.push_back(oneColorOption("yellow", Qt::yellow));
	availableColors.push_back(oneColorOption("magenta", Qt::magenta ));
}

CjvxQtMeasureAudio_ir::~CjvxQtMeasureAudio_ir()
{
	jvxSize i;
	for(i = 0; i < myMeasurements.theMeasurements.size(); i++)
	{
		JVX_SAFE_DELETE_FLD(myMeasurements.theMeasurements[i].dataOrigX, jvxData);
		JVX_SAFE_DELETE_FLD(myMeasurements.theMeasurements[i].dataOrigY, jvxData);
	}
}

jvxErrorType
CjvxQtMeasureAudio_ir::number_measurements(jvxSize& num)
{
	num = myMeasurements.theMeasurements.size();
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxQtMeasureAudio_ir::name_measurement(jvxSize id, std::string& nm)
{
	if (id < myMeasurements.theMeasurements.size())
	{
		nm = myMeasurements.theMeasurements[id].descriptor;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxQtMeasureAudio_ir::lock_data_measurements(jvxSize id, jvxData** data, jvxSize* numEntries)
{
	if (id < myMeasurements.theMeasurements.size())
	{
		if (data)
		{
			*data = myMeasurements.theMeasurements[id].dataOrigY;
		}
		if (numEntries)
		{
			*numEntries = myMeasurements.theMeasurements[id].dataL;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxQtMeasureAudio_ir::unlock_data_measurements(jvxSize id)
{
	if (id < myMeasurements.theMeasurements.size())
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxQtMeasureAudio_ir::remove_measurement(jvxSize id)
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	std::vector<oneMeasurement>::iterator elm = myMeasurements.theMeasurements.begin();
	for (i = 0; i < myMeasurements.theMeasurements.size(); i++, elm++)
	{
		if (elm == myMeasurements.theMeasurements.end())
		{
			break;
		}
		if (i == id)
		{
			JVX_SAFE_DELETE_FLD(elm->dataOrigX, jvxData);
			JVX_SAFE_DELETE_FLD(elm->dataOrigY, jvxData);
			res = JVX_NO_ERROR;
			myMeasurements.theMeasurements.erase(elm);
			break;
		}
	}
	return res;
}

void
CjvxQtMeasureAudio_ir::init(IjvxHost* theHostRef, jvxCBitField mode, jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd)
{
	jvxSize i;
	this->setupUi(this);
	theHost = theHostRef;
	//jvx_initPropertyReferenceTriple(&theTripleAuN);

	myMode = mode;

	comboBox_waveformType->addItem("Perfect Sweep");
	comboBox_waveformType->addItem("Logarithmic Sweep");
	comboBox_waveformType->addItem("Linear Sweep");
	comboBox_waveformType->setCurrentIndex(0);
	seqRunning = false;

	for (i = 0; i < availableColors.size(); i++)
	{
		comboBox_color->addItem(availableColors[i].nm.c_str());
	}

	// Init stuff for qwt
	QwtText txt;
	QFont ft = txt.font();
	ft.setPixelSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	txt.setFont(ft);

	//==============================================================
	// INIT PLOT ENGINE (Linear PLOT)
	//==============================================================
	qwtPlot_ir->setCanvasBackground(QColor(Qt::white));

	QFont font = qwtPlot_ir->axisFont(QwtPlot::xBottom);
	font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	qwtPlot_ir->setAxisFont(QwtPlot::xBottom, font);
	qwtPlot_ir->setAxisFont(QwtPlot::yLeft, font);

	font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	txt.setFont(font);
	txt.setText("Index");
	qwtPlot_ir->setAxisTitle(QwtPlot::xBottom, txt);
	txt.setText("Value");
	qwtPlot_ir->setAxisTitle(QwtPlot::yLeft, txt);

	qwtPlot_ir->setAutoDelete(false);

	// Setup a grid
	viewVars.theGrid_ir = new QwtPlotGrid;
	viewVars.theGrid_ir->setMajorPen(QPen(Qt::gray, 1, Qt::DotLine));
	viewVars.theGrid_ir->setMinorPen(QPen(Qt::gray, 1, Qt::DotLine));
	viewVars.theGrid_ir->attach(qwtPlot_ir);

	//==============================================================
// INIT PLOT ENGINE (Linear PLOT)
//==============================================================
	qwtPlot_other->setCanvasBackground(QColor(Qt::white));

	font = qwtPlot_other->axisFont(QwtPlot::xBottom);
	font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	qwtPlot_other->setAxisFont(QwtPlot::xBottom, font);
	qwtPlot_other->setAxisFont(QwtPlot::yLeft, font);

	font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	txt.setFont(font);
	txt.setText("Index");
	qwtPlot_other->setAxisTitle(QwtPlot::xBottom, txt);
	txt.setText("Value");
	qwtPlot_other->setAxisTitle(QwtPlot::yLeft, txt);

	qwtPlot_other->setAutoDelete(false);

	// Setup a grid
	viewVars.theGrid_other = new QwtPlotGrid;
	viewVars.theGrid_other->setMajorPen(QPen(Qt::gray, 1, Qt::DotLine));
	viewVars.theGrid_other->setMinorPen(QPen(Qt::gray, 1, Qt::DotLine));
	viewVars.theGrid_other->attach(qwtPlot_other);

	currentValues.ymaxir = 0;
	currentValues.yminir = 0;
	currentValues.xmaxir = 0;
	currentValues.xminir = 0;

	updatePlots();

	IjvxConfigurationExtender* cfgExt = NULL;
	if (theHost)
	{
		theHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&cfgExt));
		if (cfgExt)
		{
			cfgExt->register_extension(static_cast<IjvxConfigurationExtender_report*>(this), "JVX_AEC_HOST_EXPORTED_IRS");
			theHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle*>(cfgExt));
			cfgExt = NULL;
		}
	}
}

void
CjvxQtMeasureAudio_ir::getMyQWidget(QWidget**retWidget, jvxSize id)
{
	switch (id)
	{
	case 0:
		if (retWidget)
		{
			*retWidget = static_cast<QWidget*>(this);
		}
		break;
	default:
		if (retWidget)
		{
			*retWidget = NULL;
		}
	}
}

void
CjvxQtMeasureAudio_ir::terminate()
{
	IjvxConfigurationExtender* cfgExt = NULL;
	if (theHost)
	{
		theHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&cfgExt));
		if (cfgExt)
		{
			cfgExt->unregister_extension("JVX_AEC_HOST_EXPORTED_IRS");
			theHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle*>(cfgExt));
			cfgExt = NULL;
		}
		theHost = NULL;
	}
}

jvxErrorType
CjvxQtMeasureAudio_ir::addPropertyReference(IjvxAccessProperties* thePropRef, const std::string& prefixArg, const std::string&)
{
	if (propRef == NULL)
	{
		propRef = thePropRef;
		this->prefix = prefixArg;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void
CjvxQtMeasureAudio_ir::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	trigger_prepareMeasureIrMode();
}

void
CjvxQtMeasureAudio_ir::deactivate()
{
}

jvxErrorType
CjvxQtMeasureAudio_ir::removePropertyReference(IjvxAccessProperties*)
{
	if (propRef)
	{
		propRef = NULL;
		return JVX_NO_ERROR;
	}

	return JVX_ERROR_WRONG_STATE;
}

void
CjvxQtMeasureAudio_ir::processing_started()
{
	seqRunning = true;

	comboBox_selectChannelMeasure->setEnabled(false);
	comboBox_selectChannelTest->setEnabled(false);
	comboBox_allPlots->setEnabled(false);
	comboBox_waveformType->setEnabled(false);
	lineEdit_lengthMeasure->setEnabled(false);
	lineEdit_silenceStart->setEnabled(false);
	lineEdit_silenceStop->setEnabled(false);
	lineEdit_numberIterations->setEnabled(false);
	horizontalSlider_numberIterations->setEnabled(false);
	lineEdit_countdownstart->setEnabled(false);
	lineEdit_descriptionResult->setEnabled(false);
	lineEdit_descriptionLane->setEnabled(false);
}

void
CjvxQtMeasureAudio_ir::processing_stopped()
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i,j;
	jvxApiString  fldStr;
	jvxApiString str;
	std::string command;
	std::string txt;
	jvxCallManagerProperties callGate;

	seqRunning = false;
	comboBox_selectChannelMeasure->setEnabled(true);
	comboBox_selectChannelTest->setEnabled(true);
	comboBox_allPlots->setEnabled(true);
	comboBox_waveformType->setEnabled(true);
	lineEdit_lengthMeasure->setEnabled(true);
	lineEdit_silenceStart->setEnabled(true);
	lineEdit_silenceStop->setEnabled(true);
	lineEdit_numberIterations->setEnabled(true);
	horizontalSlider_numberIterations->setEnabled(true);
	lineEdit_countdownstart->setEnabled(true);
	lineEdit_descriptionResult->setEnabled(true);
	lineEdit_descriptionLane->setEnabled(true);

	if(propRef)
	{
		selList.bitFieldSelected() = 0;
		
		txt = jvx_makePathExpr(prefix, "/measureir/allmeasurements/selectMeasurement");
		ident.reset(txt.c_str());
		trans.reset(false);
		res = propRef->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);

		for (i = 0; i < selList.strList.ll(); i++)
		{
			oneMeasurement theMeasure;
			txt = jvx_makePathExpr(prefix, "/measureir/allmeasurements/selectMeasurement");
			jvx_bitZSet(selList.bitFieldSelected(), i);
			ident.reset(txt.c_str());
			trans.reset(true);
			res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);

			txt = jvx_makePathExpr(prefix, "/measureir/selectedmeasurement/ir_selMeasurement");
			jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(txt.c_str());
			jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
			res = propRef->get_descriptor_property(callGate, theDescr, ident);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);

			assert(theDescr.format == JVX_DATAFORMAT_DATA);
			theMeasure.dataL = theDescr.num;
			JVX_SAFE_NEW_FLD(theMeasure.dataOrigY, jvxData, theMeasure.dataL);
			JVX_SAFE_NEW_FLD(theMeasure.dataOrigX, jvxData, theMeasure.dataL);

			txt = jvx_makePathExpr(prefix, "/measureir/selectedmeasurement/ir_selMeasurement");
			ident.reset(txt.c_str());
			trans.reset(false);
			res = propRef->get_property(callGate, jPRG(theMeasure.dataOrigY, theMeasure.dataL, theDescr.format), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);

			for (j = 0; j < theMeasure.dataL; j++)
			{
				theMeasure.dataOrigX[j] = (jvxData)j;
			}

			theMeasure.descriptor = selList.strList.std_str_at(i);

			theMeasure.myCurveIr = NULL;
			theMeasure.myCurveOther = NULL;

			theMeasure.store = false;

			myMeasurements.theMeasurements.push_back(theMeasure);
		}

		command = "clearAllMeasurements()";
		str.assign_const(command.c_str(), command.size());
		txt = jvx_makePathExpr(prefix, "/common/specifyCommand");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&str, 1, JVX_DATAFORMAT_STRING), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
	}

	this->updatePlots(false, false, false, false, true, false, false);
}

void
CjvxQtMeasureAudio_ir::update_window(jvxCBitField prio, const char* propLst)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isMeasureIr = false;
	jvxData valD = 0;
	jvxInt32 valI32 = 0;
	jvxSize i;
	jvxApiString  fldStr;
	jvxSize selId = JVX_SIZE_UNSELECTED;
	std::string txt;
	jvxCallManagerProperties callGate;
	if(propRef)
	{
		selList.bitFieldSelected() = 0;

		txt = jvx_makePathExpr(prefix, "/JVX_AUDIONODE_SAMPLERATE");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG( &valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);

		// Show the propper output wave
		txt = jvx_makePathExpr(prefix, "/WaveGenerators/type_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x10))
		{
			// PSWEEP
			comboBox_waveformType->setCurrentIndex(0);
		}
		else if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x40))
		{
			// Log sweep
			comboBox_waveformType->setCurrentIndex(1);
		}
		else if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x80))
		{
			// Lin sweep
			comboBox_waveformType->setCurrentIndex(2);
		}
		else
		{
			assert(0);
		}

		txt = jvx_makePathExpr(prefix, "/WaveGenerators/lengthSeconds_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG( &valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		lineEdit_lengthMeasure->setText(jvx_data2String(valD, 3).c_str());

		txt = jvx_makePathExpr(prefix, "/WaveGenerators/silenceStartSeconds_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG( &valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		lineEdit_silenceStart->setText(jvx_data2String(valD, 3).c_str());

		txt = jvx_makePathExpr(prefix, "/WaveGenerators/silenceStopSeconds_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG( &valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		lineEdit_silenceStop->setText(jvx_data2String(valD, 3).c_str());

		txt = jvx_makePathExpr(prefix, "/WaveGenerators/loopCount_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG( &valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		lineEdit_numberIterations->setText(jvx_int2String(valI32).c_str());

		horizontalSlider_numberIterations->setValue(JVX_MIN(valI32, horizontalSlider_numberIterations->maximum()));

		/*
		txt = jvx_makePathExpr(prefix, "/measureir/noPauseBetweenIterations");
		res = propRef->get_property(callGate, jPRG( &selList, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, true, txt.c_str());
		JVX_LOCAL_ASSERT_PROPERTIES((res == JVX_NO_ERROR), txt, res);
		if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
		{
			checkBox_noPause->setChecked(false);
		}
		else
		{
			checkBox_noPause->setChecked(true);
		}
		*/
		txt = jvx_makePathExpr(prefix, "/measureir/countdownBeforeStart");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG( &valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		lineEdit_countdownstart->setText(jvx_data2String(valD, 3).c_str());

		txt = jvx_makePathExpr(prefix, "/WaveGenerators/amplitude_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG( &valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		lineEdit_amplitude->setText(jvx_data2String(valD, 3).c_str());

		valI32 = (int)(valD * 100.0);
		valI32 = JVX_MIN(100, valI32);
		horizontalSlider_amplitude->setValue(valI32);

		txt = jvx_makePathExpr(prefix, "/WaveGenerators/outputChannel_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(false);
		res = propRef->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
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

		// =================================================================
		// lanes
		// =================================================================

		txt = jvx_makePathExpr(prefix, "/measureir/alllanes/selectLane");
		ident.reset(txt.c_str());
		trans.reset(false);
		res = propRef->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		if (selList.strList.ll() != comboBox_lane->count())
			{
				comboBox_lane->clear();
				for (i = 0; i < selList.strList.ll(); i++)
				{
					comboBox_lane->addItem(selList.strList.std_str_at(i).c_str());
				}
			}
		selId = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), selList.strList.ll());
		if (JVX_CHECK_SIZE_SELECTED(selId))
		{
			comboBox_lane->setCurrentIndex(JVX_SIZE_INT(selId));
		}

		if (JVX_CHECK_SIZE_SELECTED(selId))
		{
			comboBox_selectChannelMeasure->setEnabled(true);
			comboBox_selectChannelTest->setEnabled(true);

			txt = jvx_makePathExpr(prefix, "/measureir/activelanes/testSelection_activeLane");
			ident.reset(txt.c_str());
			trans.reset(false);
			res = propRef->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
			selId = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), selList.strList.ll());
			comboBox_selectChannelTest->clear();
			comboBox_selectChannelTest->addItem("None");
			for (i = 0; i < selList.strList.ll(); i++)
			{
				std::string txt = selList.strList.std_str_at(i);
				if (i == selId)
				{
					txt += "*";
				}
				comboBox_selectChannelTest->addItem(txt.c_str());
			}

			if (JVX_CHECK_SIZE_SELECTED(selId))
			{
				comboBox_selectChannelTest->setCurrentIndex(JVX_SIZE_INT(selId + 1));
			}

			txt = jvx_makePathExpr(prefix, "/measureir/activelanes/measureSelection_activeLane");
			ident.reset(txt.c_str());
			trans.reset(false);
			res = propRef->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
			selId = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), selList.strList.ll());
			comboBox_selectChannelMeasure->clear();
			comboBox_selectChannelMeasure->addItem("None");
			for (i = 0; i < selList.strList.ll(); i++)
			{
				std::string txt = selList.strList.std_str_at(i);
				if (i == selId)
				{
					txt += "*";
				}

				comboBox_selectChannelMeasure->addItem(txt.c_str());
			}

			if (JVX_CHECK_SIZE_SELECTED(selId))
			{
				comboBox_selectChannelMeasure->setCurrentIndex(JVX_SIZE_INT(selId + 1));
			}

			txt = jvx_makePathExpr(prefix, "/measureir/activelanes/description_activeLane");
			ident.reset(txt.c_str());
			trans.reset(false);
			res = propRef->get_property(callGate, jPRG( &fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
			lineEdit_descriptionLane->setText(fldStr.c_str());
			
			txt = jvx_makePathExpr(prefix, "/measureir/allmeasurements/tagPrefix_Measurement");
			ident.reset(txt.c_str());
			trans.reset(false);
			res = propRef->get_property(callGate, jPRG( &fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
			lineEdit_descriptionResult->setText(fldStr.c_str());
			
		}
	}

	if (myMeasurements.selectionId < myMeasurements.theMeasurements.size())
	{
		if (myMeasurements.theMeasurements[myMeasurements.selectionId].store)
		{
			pushButton_4->setStyleSheet("background - color: rgba(255, 0, 0, 1); border: none;");
		}
		else
		{
			pushButton_4->setStyleSheet("background - color: rgba(220, 220, 220, 1); border: none;");
		}
		pushButton_4->update();
	}
	
	label_countdown->setText(JVX_TOKEN_COUNTDOWN_OFF);
	progressBar_all->setValue(0);
	progressBar_current->setValue(0);

	if (myMode & JVX_QT_AUDIO_WIDGETS_OUTSIDE_CONTROL)
	{
		pushButton_outputallon->setEnabled(false);
		pushButton_outputalloff->setEnabled(false);
		pushButton_addLane->setEnabled(false);
		pushButton_remLane->setEnabled(false);
		lineEdit_descriptionResult->setEnabled(false);
		lineEdit_descriptionLane->setEnabled(false);
		comboBox_lane->setEnabled(false);
	}
	if (myMode & JVX_QT_AUDIO_WIDGETS_LOCK_REM_IR)
	{
		pushButton_5->setEnabled(false);
	}
}


void
CjvxQtMeasureAudio_ir::updateDataLimits_oneMeasurement(oneMeasurement* theMeasure, jvxInt32 start, jvxInt32 stop)
{
	jvxSize i;
	jvxData maxVal = JVX_DATA_MAX_NEG;
	jvxData minVal = JVX_DATA_MAX_POS;

	theMeasure->xminir = JVX_MAX(0, start);
	theMeasure->xmaxir = JVX_SIZE_INT32(JVX_MIN(theMeasure->dataL-1, stop));

	for(i = theMeasure->xminir; i <= theMeasure->xmaxir; i++)
	{
		if(theMeasure->dataOrigY[i] > maxVal)
		{
			maxVal = theMeasure->dataOrigY[i];
		}
		if(theMeasure->dataOrigY[i] < minVal)
		{
			minVal = theMeasure->dataOrigY[i];
		}
	}
	theMeasure->ymaxir = maxVal;
	theMeasure->yminir = minVal;
}

void
CjvxQtMeasureAudio_ir::updatePlots(jvxBool updatePlotIr, jvxBool updateLimitsIr, jvxBool replotother, jvxBool setAxisLimitsother, jvxBool updateMeasurementList, jvxBool rescaleAxesir, jvxBool rescaleAxesother, jvxBool updateIrYOnly)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isMeasureIr = false;
	jvxData valD = 0;
	jvxInt32 valI32 = 0;
	jvxSize i;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxSize num = 0;
	jvxBool requiresReplot = false;

	if(updateMeasurementList)
	{
		if(myMeasurements.selectionId >= myMeasurements.theMeasurements.size())
		{
			myMeasurements.selectionId = myMeasurements.theMeasurements.size()-1;
		}

		if(myMeasurements.selectionId == JVX_SIZE_UNSELECTED)
		{
			myMeasurements.selectionId = myMeasurements.theMeasurements.size()-1;
		}

		comboBox_allPlots->clear();
		for(i = 0; i < myMeasurements.theMeasurements.size(); i++)
		{
			std::string txt = myMeasurements.theMeasurements[i].descriptor;
			if(myMeasurements.theMeasurements[i].myCurveIr)
			{
				txt = myMeasurements.theMeasurements[i].descriptor + "*";
			}
			comboBox_allPlots->addItem(txt.c_str());
		}
		if(JVX_CHECK_SIZE_SELECTED(myMeasurements.selectionId))
		{
			comboBox_allPlots->setCurrentIndex(JVX_SIZE_INT(myMeasurements.selectionId));
		}
	}

	currentValues.thereisMoreThanOne = false;
	for(i = 0; i < myMeasurements.theMeasurements.size(); i++)
	{
		if(myMeasurements.theMeasurements[i].myCurveIr)
		{
			currentValues.thereisMoreThanOne = true;
		}
	}

	if(currentValues.thereisMoreThanOne)
	{
		if(updateLimitsIr)
		{
			currentValues.ymaxir = JVX_DATA_MAX_NEG;
			currentValues.yminir = JVX_DATA_MAX_POS;

			jvxInt32 startIdxX = -1;
			jvxInt32 stopIdxX = INT_MAX;
			if(updateIrYOnly)
			{
				startIdxX = currentValues.xminir;
				stopIdxX = currentValues.xmaxir;
			}
			else
			{
				currentValues.xmaxir = INT_MIN;
				currentValues.xminir = INT_MAX;
			}


			for(i = 0; i < myMeasurements.theMeasurements.size(); i++)
			{
				if(myMeasurements.theMeasurements[i].myCurveIr)
				{
					updateDataLimits_oneMeasurement(&myMeasurements.theMeasurements[i], startIdxX, stopIdxX);
					currentValues.ymaxir = JVX_MAX(currentValues.ymaxir, myMeasurements.theMeasurements[i].ymaxir);
					currentValues.yminir = JVX_MIN(currentValues.yminir, myMeasurements.theMeasurements[i].yminir);
					if(!updateIrYOnly)
					{
						currentValues.xmaxir = JVX_MAX(currentValues.xmaxir, myMeasurements.theMeasurements[i].xmaxir);
						currentValues.xminir = JVX_MIN(currentValues.xminir, myMeasurements.theMeasurements[i].xminir);
					}
				}
			}

			rescaleAxesir = true;
		}

		// Now act

		if(updatePlotIr)
		{
			for(i = 0; i < myMeasurements.theMeasurements.size(); i++)
			{
				if(myMeasurements.theMeasurements[i].myCurveIr)
				{
					requiresReplot = true;
					myMeasurements.theMeasurements[i].myCurveIr->setSamples(myMeasurements.theMeasurements[i].dataOrigX,
						myMeasurements.theMeasurements[i].dataOrigY, JVX_SIZE_INT(myMeasurements.theMeasurements[i].dataL));
				}
			}
		}
	}
	if(rescaleAxesir)
	{
		if(currentValues.xmaxir- currentValues.xminir < 1)
		{
			currentValues.xmaxir = (currentValues.xmaxir + currentValues.xminir + 1) / 2.0;
			currentValues.xminir = (currentValues.xmaxir + currentValues.xminir - 1) / 2.0;
		}

		if(currentValues.ymaxir- currentValues.yminir < EPS_SHOW)
		{
			currentValues.ymaxir = (currentValues.ymaxir + currentValues.yminir + EPS_SHOW) / 2.0;
			currentValues.yminir = (currentValues.ymaxir + currentValues.yminir - EPS_SHOW) / 2.0;
		}
		qwtPlot_ir->setAxisScale(QwtPlot::xBottom, currentValues.xminir, currentValues.xmaxir);
		qwtPlot_ir->setAxisScale(QwtPlot::yLeft, currentValues.yminir, currentValues.ymaxir);
		requiresReplot = true;
	}

	if(requiresReplot)
	{
		qwtPlot_ir->replot();

		// Anything has changed, therefore, we need to replot the secondary plot
		updateSecondaryPlot();
		qwtPlot_other->replot();
	}
	if(currentValues.thereisMoreThanOne)
	{
		this->lineEdit_xmaxir->setText(jvx_data2String(currentValues.xmaxir,1).c_str());
		this->lineEdit_xminir->setText(jvx_data2String(currentValues.xminir,1).c_str());
		this->lineEdit_ymaxir->setText(jvx_data2String(currentValues.ymaxir,1).c_str());
		this->lineEdit_yminir->setText(jvx_data2String(currentValues.yminir,1).c_str());
		this->lineEdit_xmaxother->setText(jvx_data2String(currentValues.xmaxother,1).c_str());
		this->lineEdit_xminother->setText(jvx_data2String(currentValues.xminother,1).c_str());
		this->lineEdit_ymaxother->setText(jvx_data2String(currentValues.ymaxother,1).c_str());
		this->lineEdit_yminother->setText(jvx_data2String(currentValues.yminother,1).c_str());
	}
	else
	{
		this->lineEdit_xmaxir->setText("--");
		this->lineEdit_xminir->setText("--");
		this->lineEdit_ymaxir->setText("--");
		this->lineEdit_yminir->setText("--");
		this->lineEdit_xmaxother->setText("--");
		this->lineEdit_xminother->setText("--");
		this->lineEdit_ymaxother->setText("--");
		this->lineEdit_yminother->setText("--");
	}

}

jvxErrorType 
CjvxQtMeasureAudio_ir::request_sub_interface(jvxQtInterfaceType, jvxHandle**)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxQtMeasureAudio_ir::return_sub_interface(jvxQtInterfaceType, jvxHandle*)
{
	return JVX_ERROR_UNSUPPORTED;
}

void
CjvxQtMeasureAudio_ir::update_window_periodic()
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isMeasureIr = false;
	jvxData valD = 0;
	jvxInt32 valI321 = 0;
	jvxInt32 valI322 = 0;
	std::string txt;
	jvxCallManagerProperties callGate;

	txt = jvx_makePathExpr(prefix, "/measureir/inproc/currentStatus");
	ident.reset(txt.c_str());
	trans.reset(false);
	res = propRef->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);

	// The status may also be off if sequencer is still running but component is no longer prepared
	if (jvx_bitTest(selList.bitFieldSelected(), 0))
	{
		// Module is off due to inactive sequencer
		label_countdown->setText(JVX_TOKEN_COUNTDOWN_OFF);
	}
	else if (jvx_bitTest(selList.bitFieldSelected(), 1))
	{
		txt = jvx_makePathExpr(prefix, "/measureir/inproc/stillWaitCountdown");
		ident.reset(txt.c_str());
		trans.reset(false);
		res = propRef->get_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		label_countdown->setText((jvx_data2String(valD, 3) + " sec").c_str());		
	}
	else if (jvx_bitTest(selList.bitFieldSelected(), 2))
	{
		label_countdown->setText("Go!");
		txt = jvx_makePathExpr(prefix, "/WaveGenerators/loopCount_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG(&valI321, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		
		txt = jvx_makePathExpr(prefix, "/WaveGenerators/iterationCount_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG(&valI322, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		progressBar_all->setValue((jvxData)valI322 / (jvxData)valI321 * 100);

		txt = jvx_makePathExpr(prefix, "/WaveGenerators/progress_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		progressBar_current->setValue(valD * 100);
		
	}
	else if (jvx_bitTest(selList.bitFieldSelected(), 3))
	{
		// Module is off due to inactive sequencer
		label_countdown->setText("Complete");
	}
}



void
CjvxQtMeasureAudio_ir::trigger_prepareMeasureIrMode()
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isMeasureIr = false;
	jvxInt32 valI32 = 0;
	std::string txt;
	jvxCallManagerProperties callGate;

	// Switch to measure IR mode
	txt = jvx_makePathExpr(prefix, "/mode/currentMode");
	selList.bitFieldSelected() = 0x2; // If no propper mode is acvtive, switch mode quickly
	ident.reset(txt.c_str());
	trans.reset(true);
	res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);

	// Select the first wave generator
	txt = jvx_makePathExpr(prefix, "/WaveGenerators/selectGenerator");
	selList.bitFieldSelected() = 0x2; // <- first choice is NONE
	ident.reset(txt.c_str());
	trans.reset(true);
	res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);

	// Switch that generator wave to output psweep
	txt = jvx_makePathExpr(prefix, "/WaveGenerators/type_activeGenerator");
	selList.bitFieldSelected() = 0x10;
	ident.reset(txt.c_str());
	trans.reset(true);
	res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);

	// Set number iterations
	txt = jvx_makePathExpr(prefix, "/WaveGenerators/loopCount_activeGenerator");
	valI32 = 2;
	ident.reset(txt.c_str());
	trans.reset(true);
	res = propRef->set_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
	JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
}

void
CjvxQtMeasureAudio_ir::selected_waveformType(int idMode)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;

	std::string txt;

	if(propRef)
	{
		switch(idMode)
		{
		case 0:

			selList.bitFieldSelected() = 0x10;
			break;

		case 1:

			selList.bitFieldSelected() = 0x40;
			break;

		case 2:

			selList.bitFieldSelected() = 0x80;
			break;
		default:
			assert(0);

		}
		
		txt = jvx_makePathExpr(prefix, "/WaveGenerators/type_activeGenerator");
		ident.reset(txt.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, txt, propRef)), txt, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::changed_lengthSignal()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	jvxData valD = 0;
	jvxCallManagerProperties callGate;
	QString txt = lineEdit_lengthMeasure->text();
	valD = (jvxData)txt.toDouble();

	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/WaveGenerators/lengthSeconds_activeGenerator");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		lineEdit_lengthMeasure->setText(jvx_data2String(valD, 3).c_str());
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::changed_silenceStart()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD = 0;
	std::string token;
	jvxCallManagerProperties callGate;

	QString txt = lineEdit_silenceStart->text();
	valD = (jvxData)txt.toDouble();

	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/WaveGenerators/silenceStartSeconds_activeGenerator");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::changed_silenceStop()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD = 0;
	std::string token;
	jvxCallManagerProperties callGate;

	QString txt = lineEdit_silenceStop->text();
	valD = (jvxData)txt.toDouble();

	if (propRef)
	{
		token = jvx_makePathExpr(prefix, "/WaveGenerators/silenceStopSeconds_activeGenerator");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::changed_numberIterations()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	QString txt = lineEdit_numberIterations->text();
	jvxInt32 valI32 = txt.toInt();
	jvxCallManagerProperties callGate;

	valI32 = JVX_MAX(1,valI32);
	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/WaveGenerators/loopCount_activeGenerator");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::modify_numberIterations()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	jvxCallManagerProperties callGate;

	jvxInt32 valI32 = horizontalSlider_numberIterations->value();
	valI32 = JVX_MAX(1,valI32);
	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/WaveGenerators/loopCount_activeGenerator");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

/*
void
CjvxQtMeasureAudio_ir::toggled_noPause(bool tog)
{
	jvxSelectionList selList;
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	selList.strList = NULL;

	if(tog)
	{
		selList.bitFieldSelected() = 0x2;
	}
	else
	{
		selList.bitFieldSelected() = 0x1;
	}
	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/measureir/noPauseBetweenIterations");
		res = propRef->set_property(callGate, &selList, 1, JVX_DATAFORMAT_SELECTION_LIST, true, token.c_str());
		JVX_LOCAL_ASSERT_PROPERTIES((res == JVX_NO_ERROR), token, res)
		this->update_window();
	}
}
*/

void
CjvxQtMeasureAudio_ir::changed_amplitude()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	jvxData valD = 0;
	jvxCallManagerProperties callGate;

	QString txt = lineEdit_amplitude->text();
	valD = (jvxData)txt.toDouble();

	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/WaveGenerators/amplitude_activeGenerator");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::modify_amplitude()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	jvxCallManagerProperties callGate;

	jvxInt32 valI32 = horizontalSlider_amplitude->value();
	jvxData valD = valI32 / 100.0;

	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/WaveGenerators/amplitude_activeGenerator");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::changed_countdownBeforeStart()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	jvxData valD = 0;
	jvxCallManagerProperties callGate;

	QString txt = lineEdit_countdownstart->text();
	valD = (jvxData)txt.toDouble();
	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/measureir/countdownBeforeStart");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::selected_output(int id)
{
	jvxSelectionList selList;
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;

	if (myMode == 0)
	{
		if (propRef)
		{
			token = jvx_makePathExpr(prefix, "/WaveGenerators/outputChannel_activeGenerator");
			ident.reset(token.c_str());
			trans.reset(true);
			res = propRef->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

				selList.bitFieldSelected() ^= ((jvxBitField)1 << id);
			token = jvx_makePathExpr(prefix, "/WaveGenerators/outputChannel_activeGenerator");
			ident.reset(token.c_str());
			trans.reset(true);
			res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
				this->update_window();
		}
	}
}

void
CjvxQtMeasureAudio_ir::clicked_outputallon()
{
	jvxSelectionList selList;
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;

	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/WaveGenerators/outputChannel_activeGenerator");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		token = jvx_makePathExpr(prefix, "/WaveGenerators/outputChannel_activeGenerator");
		ident.reset(token.c_str());
		trans.reset(true);
		selList.bitFieldSelected() = ((jvxBitField)1 << JVX_SIZE_INT32(selList.strList.ll())) -1;
		res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::clicked_outputalloff()
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	jvxCallManagerProperties callGate;

	if(propRef)
	{
		selList.bitFieldSelected() = 0;
		token = jvx_makePathExpr(prefix, "/WaveGenerators/outputChannel_activeGenerator");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::selected_transferLine(int id)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	jvxCallManagerProperties callGate;

	if (myMode == 0)
	{

		if (propRef)
		{
			selList.bitFieldSelected() = (jvxBitField)1 << id;
			token = jvx_makePathExpr(prefix, "/measureir/alllanes/selectLane");
			ident.reset(token.c_str());
			trans.reset(true);
			res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

			this->update_window();
		}
	}
}

void
CjvxQtMeasureAudio_ir::clicked_addTransferLane()
{
	jvxApiString str;
	std::string command = "addLane";
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;

	QString txt = lineEdit_descriptionLane->text();
	if(txt.isEmpty())
	{
		command += "(lane_" + jvx_int2String(comboBox_lane->count()) + ")";
	}
	else
	{
		command += "(";
		command += txt.toLatin1().data();
		command += ")";
	}

	str.assign_const( command.c_str(),command.size());
	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/common/specifyCommand");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&str, 1, JVX_DATAFORMAT_STRING), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::clicked_removeTransferLane()
{
	jvxApiString str;
	std::string token;
	std::string command = "removeLane";
	jvxErrorType res = JVX_NO_ERROR;
	QString txt = lineEdit_descriptionLane->text();
	jvxApiString fldStr;
	jvxCallManagerProperties callGate;

	if(propRef)
	{
		token = jvx_makePathExpr(prefix, "/measureir/activelanes/description_activeLane");
		ident.reset(token.c_str());
		trans.reset(false);
		res = propRef->get_property(callGate, jPRG( &fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		command += "(";
		command += fldStr.std_str();
		command += ")";

		str.assign_const(command.c_str(), command.size());
		token = jvx_makePathExpr(prefix, "/common/specifyCommand");
		ident.reset(token.c_str());
		trans.reset(true);
		res = propRef->set_property(callGate, jPRG(&str, 1, JVX_DATAFORMAT_STRING), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);
		this->update_window();
	}
}

void
CjvxQtMeasureAudio_ir::changed_descriptionLane()
{
	// Nothing to do here..
	//std::cout << __FUNCTION__ << std::endl;
}

void
CjvxQtMeasureAudio_ir::selected_channelTest(int id)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	jvxCallManagerProperties callGate;

	if (myMode == 0)
	{
		if (propRef)
		{
			if (id == 0)
			{
				selList.bitFieldSelected() = 0;
			}
			else
			{
				selList.bitFieldSelected() = (jvxBitField)1 << (id - 1);
			}
			token = jvx_makePathExpr(prefix, "/measureir/activelanes/testSelection_activeLane");
			ident.reset(token.c_str());
			trans.reset(true);
			res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

				this->update_window();
		}
	}
}
void
CjvxQtMeasureAudio_ir::selected_channelMeasure(int id)
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	jvxCallManagerProperties callGate;

	if (myMode == 0)
	{
		if (propRef)
		{
			if (id == 0)
			{
				selList.bitFieldSelected() = 0;
			}
			else
			{
				selList.bitFieldSelected() = (jvxBitField)1 << (id - 1);
			}
			token = jvx_makePathExpr(prefix, "/measureir/activelanes/measureSelection_activeLane");
			ident.reset(token.c_str());
			trans.reset(true);
			res = propRef->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

				this->update_window();
		}
	}
}

void
CjvxQtMeasureAudio_ir::changed_descriptionMeasure()
{
	jvxApiString str;
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	QString txt = lineEdit_descriptionResult->text();
	jvxApiString * fldStr = NULL;
	std::string descr = txt.toLatin1().data();
	jvxCallManagerProperties callGate;

	if(propRef)
	{
		str.assign_const( descr.c_str(),descr.size());

		token = jvx_makePathExpr(prefix, "/measureir/allmeasurements/tagPrefix_Measurement");
		ident.reset(token.c_str());
		trans.reset(false);
		res = propRef->set_property(callGate, jPRG(&str, 1, JVX_DATAFORMAT_STRING), ident, trans);
		JVX_LOCAL_ASSERT_PROPERTIES((JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, token, propRef)), token, res);

		this->update_window();
	}}

void
CjvxQtMeasureAudio_ir::clicked_exportMeasure()
{
	jvxSize i,j,cnt = 0;
	IjvxToolsHost* theToolsHost = NULL;
	IjvxObject* theOutputObj = NULL;
	IjvxConfigProcessor* theOutputHandle = NULL;
	jvxConfigData* theSection = NULL;
	jvxConfigData* theSubSection = NULL;
	jvxConfigData* theData = NULL;


	QString str = QFileDialog::getSaveFileName(this, tr("Open Export Ir File"), tr(".\\"), tr("(*.jvx)"));
	std::string fName = (std::string)str.toLatin1().constData();

	std::string content;
	jvxApiString  fldStr;
	if(theHost)
	{
		theHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theToolsHost));
		if(theToolsHost)
		{
			jvxComponentIdentification cTp(JVX_COMPONENT_CONFIG_PROCESSOR);
			theToolsHost->reference_tool(cTp, &theOutputObj, 0, NULL);
			if(theOutputObj)
			{
				theOutputObj->request_specialization(reinterpret_cast<jvxHandle**>(&theOutputHandle), NULL, NULL);
				if(theOutputHandle)
				{
					theOutputHandle->createEmptySection(&theSection, "THE_IMPULSE_RESPONSES");
					if(theSection)
					{
						for(i = 0; i <myMeasurements.theMeasurements.size(); i++)
						{
							if(myMeasurements.theMeasurements[i].myCurveIr)
							{
								theOutputHandle->createEmptySection(&theSubSection, ("IR_RESPONSES_" + jvx_size2String(i)).c_str());
								theData = NULL;
								theOutputHandle->createAssignmentValueList(&theData, "IR_RESPONSES_DATA");

								jvxSize start = currentValues.xminir;
								jvxSize stop = currentValues.xmaxir;
								if(currentValues.xminir < 0)
								{
									start = 0;
								}
								if(currentValues.xmaxir >= myMeasurements.theMeasurements[i].dataL)
								{
									stop = myMeasurements.theMeasurements[i].dataL - 1;
								}
								for(j = start; j <= stop ; j++)
								{
									theOutputHandle->addAssignmentValueToList(theData, 0, myMeasurements.theMeasurements[i].dataOrigY[j]);
								}
								theOutputHandle->addSubsectionToSection(theSubSection, theData);

								theData = NULL;
								theOutputHandle->createAssignmentString(&theData, "IR_RESPONSES_TAG", myMeasurements.theMeasurements[i].descriptor.c_str());
								theOutputHandle->addSubsectionToSection(theSubSection, theData);

								theData = NULL;

								theOutputHandle->addSubsectionToSection(theSection, theSubSection);
								cnt++;
							}
						}
						theOutputHandle->printConfiguration(theSection, &fldStr, false);
						content = fldStr.std_str();
						jvx_writeContentToFile(fName, content);
						theOutputHandle->removeHandle(theSection);
					}
				}
				theToolsHost->return_reference_tool(cTp, theOutputObj);
			}
			theHost->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theToolsHost));
		}
	}
}

void
CjvxQtMeasureAudio_ir::clicked_removeMeasurement()
{
	std::vector<oneMeasurement>::iterator elm = myMeasurements.theMeasurements.begin();
	int cnt = 0;
	for(;elm != myMeasurements.theMeasurements.end(); elm++, cnt++)
	{
		if(cnt == myMeasurements.selectionId)
		{
			if(elm->myCurveIr)
			{
				myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr->detach();
				delete(myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr);
				myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr = NULL;
				myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveOther->detach();
				delete(myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveOther);
				myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveOther = NULL;
			}
			break;
		}
	}
	if(elm != myMeasurements.theMeasurements.end())
	{
		myMeasurements.theMeasurements.erase(elm);
	}
	updatePlots();
}

void
CjvxQtMeasureAudio_ir::changed_ymaxother()
{
	QString txt = lineEdit_ymaxother->text();
	jvxData val = txt.toDouble();
	currentValues.ymaxother = JVX_MAX(val, currentValues.yminother + EPS_SHOW);
	updateSecondaryPlot(false, true);
}

void
CjvxQtMeasureAudio_ir::changed_yminother()
{
	QString txt = lineEdit_yminother->text();
	jvxData val = txt.toDouble();
	currentValues.yminother = JVX_MIN(val, currentValues.ymaxother - EPS_SHOW);
	updateSecondaryPlot(false, true);
}

void
CjvxQtMeasureAudio_ir::changed_xminother()
{
	QString txt = lineEdit_xminother->text();
	jvxData val = txt.toDouble();
	currentValues.xminother = JVX_MIN(val, currentValues.xmaxother - EPS_SHOW);
	updateSecondaryPlot(false, true);
}

void
	CjvxQtMeasureAudio_ir::changed_xmaxother()
{
	QString txt = lineEdit_xmaxother->text();
	jvxData val = txt.toDouble();
	currentValues.xmaxother = JVX_MAX(val, currentValues.xminother + EPS_SHOW);
	updateSecondaryPlot(false, true);
}

void
CjvxQtMeasureAudio_ir::changed_ymaxir()
{
	QString txt = lineEdit_ymaxir->text();
	jvxData val = txt.toDouble();
	currentValues.ymaxir = JVX_MAX(val, currentValues.yminir + EPS_SHOW);
	updatePlots(false, false, false, false, false);
}

void
CjvxQtMeasureAudio_ir::changed_yminir()
{
	QString txt = lineEdit_yminir->text();
	jvxData val = txt.toDouble();
	currentValues.yminir = JVX_MIN(val, currentValues.ymaxir - EPS_SHOW);
	updatePlots(false, false, false, false, false);
}

void
CjvxQtMeasureAudio_ir::changed_xminir()
{
	QString txt = lineEdit_xminir->text();
	jvxData val = txt.toDouble();
	currentValues.xminir = JVX_MIN(val, currentValues.xmaxir - EPS_SHOW);
	updatePlots(false, false, false, false, false);
}

void
CjvxQtMeasureAudio_ir::changed_xmaxir()
{
	QString txt = lineEdit_xmaxir->text();
	jvxData val = txt.toDouble();
	currentValues.xmaxir = JVX_MAX(val, currentValues.xminir + EPS_SHOW);
	updatePlots(false, false, false, false, false);
}

void
CjvxQtMeasureAudio_ir::clicked_rescaleir()
{
	updatePlots(false, true, false, false, false);
}

void
CjvxQtMeasureAudio_ir::selected_measurement(int id)
{
	myMeasurements.selectionId = id;
	updatePlots(false, false, false, false, true, false, false);
}

void
CjvxQtMeasureAudio_ir::selected_whatToPlot(int id)
{
	std::cout << __FUNCTION__ << std::endl;
}

void
CjvxQtMeasureAudio_ir::clicked_toggleView()
{
	if(JVX_CHECK_SIZE_SELECTED(myMeasurements.selectionId))
	{
		if(myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr == NULL)
		{
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr = new QwtPlotCurve();
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr->attach(qwtPlot_ir);
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveOther = new QwtPlotCurve();
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveOther->attach(qwtPlot_other);
			QColor col = availableColors[comboBox_color->currentIndex()].col;
			std::string txt = myMeasurements.theMeasurements[myMeasurements.selectionId].descriptor;

			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr->setTitle(jvx_shortenStringName(12, txt, 3).c_str());
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveOther->setTitle(jvx_shortenStringName(12, txt, 3).c_str());

			QPen thePen;
			thePen.setColor(col);
			thePen.setWidth(2);
			thePen.setStyle(Qt::SolidLine);
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveOther->setPen(thePen);
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr->setPen(thePen);

			this->updatePlots();
		}
		else
		{
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr->detach();
			delete(myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr);
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveIr = NULL;
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveOther->detach();
			delete(myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveOther);
			myMeasurements.theMeasurements[myMeasurements.selectionId].myCurveOther = NULL;
			this->updatePlots();
		}
	}
}

void
CjvxQtMeasureAudio_ir::updateSecondaryPlot(jvxBool updateContent, jvxBool updateScale)
{
	jvxSize i,j;
	jvxFFTGlobal* globFft = NULL;
	jvxFFT* myFft = NULL;
	jvxData* inFld = NULL;
	jvxData* inFlds = NULL;
	jvxDataCplx* outFld = NULL;
	jvxSize N = 0;
	jvxSize sigL = currentValues.xmaxir - currentValues.xminir + 1;
	jvxSize sigShow = 0;
	jvxBool requiresReplot = false;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if(updateContent)
	{
		if(sigL % 2)
		{
			sigL += 1;
		}
		if(sigL < 16)
		{
			sigL = 16;
		}

		currentValues.ymaxother = JVX_DATA_MAX_NEG;
		currentValues.yminother = JVX_DATA_MAX_POS;
		currentValues.xmaxother = JVX_DATA_MAX_NEG;
		currentValues.xminother = JVX_DATA_MAX_POS;
		updateScale = true;
		requiresReplot = true;

		jvxFFTSize fftInt;
		res = jvx_get_nearest_size_fft(&fftInt, sigL, JVX_FFT_ROUND_UP, NULL);
		assert(res == JVX_DSP_NO_ERROR);

		jvx_create_fft_ifft_global(&globFft, fftInt, NULL JVX_FFT_GLOBAL_CONFIG_ADD_ARGUMENT_CALL);
		jvx_create_fft_real_2_complex(&myFft, globFft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&inFld, &outFld, &N, JVX_FFT_IFFT_EFFICIENT, NULL, NULL, sigL);
		JVX_SAFE_NEW_FLD(inFlds, jvxData, N/2+1);
		for(j = 0; j < N/2+1; j++)
		{
			inFlds[j] = (jvxData)j;
		}
		currentValues.xminother = 0;
		currentValues.xmaxother = N/2+1-1;

		for(i = 0; i < myMeasurements.theMeasurements.size(); i++)
		{
			if(myMeasurements.theMeasurements[i].myCurveOther)
			{
				memset(inFld, 0, sigL*sizeof(jvxData));
				jvxSize cpy = JVX_MIN(sigL, myMeasurements.theMeasurements[i].dataL - currentValues.xminir);
				memcpy(inFld, myMeasurements.theMeasurements[i].dataOrigY + currentValues.xminir, sizeof(jvxData) * cpy);
				jvx_execute_fft(myFft);
				for(j = 0; j < N/2+1; j++)
				{
					inFld[j] = outFld[j].re * outFld[j].re + outFld[j].im * outFld[j].im;
					inFld[j] = 10*log10(inFld[j]);
					if(inFld[j] > currentValues.ymaxother)
					{
						currentValues.ymaxother = inFld[j];
					}
					if(inFld[j] < currentValues.yminother)
					{
						currentValues.yminother = inFld[j];
					}
				}
				myMeasurements.theMeasurements[i].myCurveOther->setSamples(inFlds, inFld, JVX_SIZE_INT(N/2+1));
			}
		}
		JVX_SAFE_DELETE_FLD(inFlds, jvxData);
		jvx_destroy_fft(myFft);
		jvx_destroy_fft_ifft_global(globFft, nullptr);
	}


	if(updateScale)
	{
		if(!currentValues.thereisMoreThanOne)
		{
			currentValues.ymaxother = 0;
			currentValues.yminother  = 0;
		}

		if(currentValues.xmaxother- currentValues.xminother < EPS_SHOW)
		{
			currentValues.xmaxother = (currentValues.xmaxother + currentValues.xminother + 1) / 2.0;
			currentValues.xminother = (currentValues.xmaxother + currentValues.xminother - 1) / 2.0;
		}

		if(currentValues.ymaxother- currentValues.yminother < EPS_SHOW)
		{
			currentValues.ymaxother = (currentValues.ymaxother + currentValues.yminother + EPS_SHOW) / 2.0;
			currentValues.yminother = (currentValues.ymaxother + currentValues.yminother - EPS_SHOW) / 2.0;
		}
		qwtPlot_other->setAxisScale(QwtPlot::xBottom, currentValues.xminother, currentValues.xmaxother);
		qwtPlot_other->setAxisScale(QwtPlot::yLeft, currentValues.yminother, currentValues.ymaxother);
		requiresReplot = true;
	}
	if(requiresReplot)
	{
		qwtPlot_other->replot();
	}
}

void
CjvxQtMeasureAudio_ir::clicked_rescaleYir()
{
	updatePlots(true, true, false, false, false, true, false, true);
}

void
CjvxQtMeasureAudio_ir::clicked_rescaleother()
{
	std::cout << __FUNCTION__ << std::endl;
}

void
CjvxQtMeasureAudio_ir::toggle_store()
{
	if (myMeasurements.selectionId < myMeasurements.theMeasurements.size())
	{
		myMeasurements.theMeasurements[myMeasurements.selectionId].store = !myMeasurements.theMeasurements[myMeasurements.selectionId].store;
		update_window();
	}
}

jvxErrorType
CjvxQtMeasureAudio_ir::get_configuration_ext(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize i;
	jvxSize cnt = 0;
	jvxConfigData* theDat = NULL, *theDatSub = NULL;
	for (i = 0; i < this->myMeasurements.theMeasurements.size(); i++)
	{
		if (myMeasurements.theMeasurements[i].store)
		{
			processor->createEmptySection(&theDat, ("JVX_THE_IR_MEASURED_" + jvx_int2String(cnt)).c_str());
			if (theDat)
			{
				HjvxConfigProcessor_writeEntry_assignmentString(processor, theDat, "THE_MEASURE_TAG", &myMeasurements.theMeasurements[i].descriptor);
				HjvxConfigProcessor_writeEntry_1dList(processor, theDat, "THE_MEASURE_DATA", myMeasurements.theMeasurements[i].dataOrigY, myMeasurements.theMeasurements[i].dataL);
				processor->addSubsectionToSection(sectionWhereToAddAllSubsections, theDat);
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxQtMeasureAudio_ir::put_configuration_ext(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename, jvxInt32 lineno)
{
	jvxSize i,j;
	jvxSize num = 0;
	jvxHandle* theDat = NULL, *theDatLoc = NULL;
	jvxApiString  fldStr;
	jvxApiValueList lst;
	std::string tag;

	processor->getNumberEntriesCurrentSection(sectionToContainAllSubsectionsForMe, &num);
	for (i = 0; i < num; i++)
	{
		processor->getReferenceEntryCurrentSection_id(sectionToContainAllSubsectionsForMe, &theDat, i);
		if (theDat)
		{
			processor->getNameCurrentEntry(theDat, &fldStr);
			if (jvx_compareStringsWildcard("JVX_THE_IR_MEASURED_*", fldStr.std_str()))
			{
				theDatLoc = NULL;
				processor->getReferenceEntryCurrentSection_name(theDat, &theDatLoc, "THE_MEASURE_DATA");
				HjvxConfigProcessor_readEntry_assignmentString(processor, theDat, "THE_MEASURE_TAG", &tag);
				if (theDatLoc)
				{
					processor->getValueList_id(theDatLoc, &lst, 0);

						oneMeasurement theMeasure;
						theMeasure.descriptor = tag;

						theMeasure.myCurveIr = NULL;
						theMeasure.myCurveOther = NULL;

						theMeasure.store = true;
						theMeasure.dataL = lst.ll();
						JVX_SAFE_NEW_FLD(theMeasure.dataOrigX, jvxData, theMeasure.dataL);
						JVX_SAFE_NEW_FLD(theMeasure.dataOrigY, jvxData, theMeasure.dataL);
						for (j = 0; j < theMeasure.dataL; j++)
						{
							theMeasure.dataOrigX[j] = (jvxData)j;
							lst.elm_at(j).toContent(&theMeasure.dataOrigY[j]);
						}

						myMeasurements.theMeasurements.push_back(theMeasure);
					
				}
			}
		}
	}
	this->updatePlots(false, false, false, false, true, false, false);

	return JVX_NO_ERROR;
}


#ifdef JVX_PROJECT_NAMESPACE
}
#endif
