#include "jvxQtSaSpectrumViewer.h"
#include "ui_jvxQtSpectrumViewer.h"
#include "common/CjvxPropertiesSimple.h"
#include "qwt_legend.h"
#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_grid.h"
#include "qwt_scale_engine.h"
#include "qwt_text.h"
#include "qpen.h"

#define PIXEL_SIZE_FONTS 9

jvxQtSaSpectrumViewer::jvxQtSaSpectrumViewer(QWidget* parent,
                                         CjvxPropertiesSimple* propertyInterface,
                                         jvxData mindB,
                                         jvxData maxdB,
                                         const std::string& idTxt)
     : QWidget(parent)
{
     ui = new Ui::Form_jvxQtSpectrumViewer();
     ui->setupUi(this);

     plot = new QwtPlot();
     plot->setCanvasBackground(Qt::white);
     QwtText txt;
     QFont ft = txt.font();
     ft.setPixelSize(PIXEL_SIZE_FONTS);
     txt.setFont(ft);
     plot->setAxisFont(QwtPlot::xBottom, ft);
     plot->setAxisFont(QwtPlot::yLeft, ft);
     txt.setText("Frequency [Hz]");
     plot->setAxisTitle( QwtPlot::xBottom, txt );
     txt.setText("Magnitude Spectrum [dB]");
     plot->setAxisTitle( QwtPlot::yLeft, txt);
     plot->setAxisScaleEngine( QwtPlot::xBottom, new QwtLinearScaleEngine);
     grid = new QwtPlotGrid;
     grid->setMajorPen( QPen( Qt::gray, 1, Qt::DotLine ) );
     grid->setMinorPen( QPen( Qt::gray, 1 , Qt::DotLine ) );
     grid->attach(plot);
     this->mindB = mindB;
     this->maxdB = maxdB;
     plot->insertLegend(new QwtLegend, QwtPlot::BottomLegend, 1.0);
     ui->plotLayout->addWidget(plot);

     freqAxis = NULL;
     minFreqLogScale = 10; // fix to 10 Hz for now
     skipBins = 1; // just a default, recomputed below
     spectrumSize = -1;
     props = NULL;
     data = NULL;
     samplerate = -1;

     pprefix = idTxt;
     initPropertyAccess(propertyInterface);

     QObject::connect(ui->comboBox_spectrumEstimationMethod, SIGNAL(currentIndexChanged(int)),
                      this, SLOT(methodChanged(int)));
     QObject::connect(ui->comboBox_spectrumEstimationWindow, SIGNAL(currentIndexChanged(int)),
                      this, SLOT(windowChanged(int)));
     QObject::connect(ui->doubleSpinBox_spectrumEstimationWinArgA, SIGNAL(valueChanged(double)),
                      this, SLOT(winArgAChanged(double)));
     QObject::connect(ui->doubleSpinBox_spectrumEstimationWinArgB, SIGNAL(valueChanged(double)),
                      this, SLOT(winArgBChanged(double)));
     QObject::connect(ui->spinBox_spectrumEstimationFFTSize, SIGNAL(valueChanged(int)),
                      this, SLOT(FFTSizeChanged(int)));
     QObject::connect(ui->doubleSpinBox_spectrumEstimationAlpha, SIGNAL(valueChanged(double)),
                      this, SLOT(alphaChanged(double)));
     QObject::connect(ui->checkBox_spectrumEstimationLogarithmicAmplitude, SIGNAL(toggled(bool)),
                      this, SLOT(logarithmicAmplitudeToggled(bool)));
     QObject::connect(ui->checkBox_spectrumEstimationLogarithmicFrequency, SIGNAL(toggled(bool)),
                      this, SLOT(logarithmicFrequencyToggled(bool)));
     QObject::connect(ui->checkBox_enabled, SIGNAL(toggled(bool)),
                      this, SLOT(enabledToggled(bool)));
}


jvxQtSaSpectrumViewer::~jvxQtSaSpectrumViewer()
{
     if(freqAxis)
          delete[] freqAxis;
     if(data)
          delete[] data;
}

void jvxQtSaSpectrumViewer::initPropertyAccess(CjvxPropertiesSimple *props)
{

     // property access
     this->props = props;
}

void jvxQtSaSpectrumViewer::updatePropertyPrefix(const std::string& pre)
{
    this->pprefix = pre;
}

void jvxQtSaSpectrumViewer::updatePlot(std::string name,
                                     int channel,
                                     QColor color)
{
     jvxSize spectrumSizeNew;
     jvxInt16 enabled;
     jvxInt32 nChannels;
	 jvxCallManagerProperties callGate;

     // do we need a new curve?
     if(!curves[name])
          {
               curves[name] = new QwtPlotCurve(name.c_str());
               curves[name]->setRenderHint( QwtPlotItem::RenderAntialiased );
               curves[name]->setYAxis(QwtPlot::yLeft);
               curves[name]->setPen(QPen(color));
               curves[name]->attach(plot);
          }

     // check property interface
     if(!props)
          return;

     // get and check some basic properties
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/enabled");
     props->get_property(token, &enabled, 1, 0, callGate); //"
     if(!enabled)
          return;

     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/numberChannels");
     props->get_property(token, &nChannels, 1, 0, callGate); // "/"
     if(channel >= nChannels)
          {
               std::cerr<<"[jvxQtSaSpectrumViewer] ERROR: invalid channel number "<<channel<<" (max. " << nChannels-1<<")"<<std::endl;
               return;
          }

     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/spectrumSize");
     props->get_property(token, &spectrumSizeNew, callGate); // "/"
     if(!spectrumSizeNew)
          return;

     if(spectrumSizeNew != spectrumSize) // reallocations required?
          {
               spectrumSize = spectrumSizeNew;
               token = jvx_makePathExpr(pprefix, "/spectrumEstimation/samplerate");
               props->get_property(token, &samplerate, 1, 0, callGate); // "/"

               // reallocate frequency axis (0...fs/2)
               if (freqAxis)
                    delete[] freqAxis;
               freqAxis = new jvxData[spectrumSize];
               for(int i = 0; i < spectrumSize; i++)
                    freqAxis[i] = (jvxData)i/(jvxData)(spectrumSize-1) * ((jvxData)samplerate / 2.0);

               // reallocate data buffer
               if(data)
                    delete[] data;
               data = new jvxData[spectrumSize];
               for(int i = 0; i< spectrumSize; i++)
                    data[i] = -100.; // make sure we don't see curves in active state (whether log or not)

               // recalculate skipbins
               if(ui->checkBox_spectrumEstimationLogarithmicFrequency->isChecked())
                    skipBins = JVX_DATA2INT16(ceil(minFreqLogScale / samplerate * 2.0 * spectrumSize));
          }

     // get data & plot
     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/data");
     props->get_property(token, data, spectrumSize, channel*spectrumSize, callGate); // "/"
     curves[name]->setSamples(freqAxis+skipBins, data+skipBins, (int)(spectrumSize-skipBins));
     plot->replot();
}

void jvxQtSaSpectrumViewer::updateControls()
{
     jvxInt16 valInt16;
     jvxData valData;
	 jvxCallManagerProperties callGate;

     if(!props)
          return;

     ui->label_prefix->setText(pprefix.c_str());

     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/enabled");
     props->get_property(token, &valInt16, 1, 0, callGate); // "/"
     ui->checkBox_enabled->setChecked(valInt16);

     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/method");
     props->get_property(token, &valInt16, 1, 0, callGate); // "/"
     ui->comboBox_spectrumEstimationMethod->setCurrentIndex(valInt16);

     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/window");
     props->get_property(token, &valInt16, 1, 0, callGate); // "/"
     ui->comboBox_spectrumEstimationWindow->setCurrentIndex(valInt16);

     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/winArgA"); 
     props->get_property(token, &valData, 1, 0, callGate); // "/"
     ui->doubleSpinBox_spectrumEstimationWinArgA->setValue(valData);

     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/winArgB"); 
     props->get_property(token, &valData, 1, 0, callGate); // "/"
     ui->doubleSpinBox_spectrumEstimationWinArgB->setValue(valData);

     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/fftLengthLog2"); 
     props->get_property(token, &valInt16, 1, 0, callGate); // "/"
     ui->spinBox_spectrumEstimationFFTSize->setValue(valInt16);

     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/alpha"); 
     props->get_property(token, &valData, 1, 0, callGate); // "/"
     ui->doubleSpinBox_spectrumEstimationAlpha->setValue(valData);

     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/logarithmic"); 
     props->get_property(token, &valInt16, 1, 0, callGate); // "/"
     ui->checkBox_spectrumEstimationLogarithmicAmplitude->setCheckState(valInt16?Qt::Checked:Qt::Unchecked);

     token = jvx_makePathExpr(pprefix, "/spectrumEstimation/logFrequency"); 
     props->get_property(token, &valInt16, 1, 0, callGate); // "/"
     logarithmicFrequencyToggled(valInt16?true:false); // also update graph axis
     ui->checkBox_spectrumEstimationLogarithmicFrequency->setChecked(valInt16?true:false);
}

void jvxQtSaSpectrumViewer::setNotificationFlag()
{
     jvxInt16 valInt16 = 1;
	 jvxCallManagerProperties callGate; 
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/syncNotify");
     jvxErrorType res = props->set_property(token, &valInt16, 1, 0, callGate); // "/"
}

void jvxQtSaSpectrumViewer::methodChanged(int value)
{
     jvxInt16 valInt16= (jvxInt16) value;
	 jvxCallManagerProperties callGate;
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/method");
     props->set_property(token, &valInt16, 1 , 0, callGate); // "/"
     setNotificationFlag();
     updateControls();
}

void jvxQtSaSpectrumViewer::windowChanged(int value)
{
     jvxInt16 val = (jvxInt16) value;
	 jvxCallManagerProperties callGate;
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/window");
     props->set_property(token, &val, 1, 0, callGate); // "/"
     setNotificationFlag();
     updateControls();
}

void jvxQtSaSpectrumViewer::winArgAChanged(double value)
{
     jvxData valData = (jvxData) value;
	 jvxCallManagerProperties callGate;
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/winArgA");
     props->set_property(token, &valData, 1, 0, callGate); // "/"
     setNotificationFlag();
     updateControls();
}

void jvxQtSaSpectrumViewer::winArgBChanged(double value)
{
     jvxData valData = (jvxData) value;
	 jvxCallManagerProperties callGate;
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/winArgB");
     props->set_property(token, &valData, 1, 0, callGate); // "/"
     setNotificationFlag();
     updateControls();
}

void jvxQtSaSpectrumViewer::FFTSizeChanged(int value)
{
     jvxInt16 valInt16 = (jvxInt16) value;
	 jvxCallManagerProperties callGate;
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/fftLengthLog2");
     props->set_property(token, &valInt16, 1, 0, callGate); // "/"
     setNotificationFlag();
     updateControls();
}

void jvxQtSaSpectrumViewer::alphaChanged(double value)
{
     jvxData valData = (jvxData) value;
	 jvxCallManagerProperties callGate;
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/alpha");
     props->set_property(token, &valData, 1, 0, callGate); // "/"
     setNotificationFlag();
     updateControls();
}

void jvxQtSaSpectrumViewer::logarithmicAmplitudeToggled(bool value)
{
     jvxInt16 valInt16 = (jvxInt16) value;
	 jvxCallManagerProperties callGate;
     if(value)
          plot->setAxisScale(QwtPlot::yLeft, mindB, maxdB);
     else
          plot->setAxisAutoScale(QwtPlot::yLeft, true);
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/logarithmic");
     props->set_property(token, &valInt16, 1, 0, callGate); // "/"
     setNotificationFlag();
     updateControls();
}

void jvxQtSaSpectrumViewer::logarithmicFrequencyToggled(bool value)
{
     jvxInt16 valInt16 = (jvxInt16) value;
	 jvxCallManagerProperties callGate;
     if(value)
          {
               plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
               skipBins = JVX_DATA2INT16(ceil(minFreqLogScale / samplerate * 2.0 * spectrumSize));
          }
     else
          {
               plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
               skipBins = 0;
          }
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/logFrequency");
     props->set_property(token, &valInt16, 1, 0, callGate); // "/"
}

void jvxQtSaSpectrumViewer::enabledToggled(bool value)
{
     jvxInt16 valInt16 = (jvxInt16) value;
	 jvxCallManagerProperties callGate;
     std::string token = jvx_makePathExpr(pprefix, "/spectrumEstimation/enabled");
     props->set_property(token, &valInt16, 1, 0, callGate); // "/"
}
