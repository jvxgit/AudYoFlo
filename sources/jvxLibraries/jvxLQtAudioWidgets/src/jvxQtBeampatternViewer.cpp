#include "jvxQtBeampatternViewer.h"
#include "ui_jvxQtBeampatternViewer.h"
#include "common/CjvxPropertiesSimple.h"
#include "qwt_plot_layout.h"
#include <qwt_scale_widget.h>
#include "qwt_plot_spectrogram.h"
#include "qwt_plot.h"
#include "qwt_color_map.h"
#include "qwt_raster_data.h"
#include "qwt_scale_engine.h"

#define PIXEL_SIZE_FONTS 9
class jvxQwtColorMap: public QwtLinearColorMap
{
public:
	jvxQwtColorMap():
	  QwtLinearColorMap(Qt::blue, Qt::red)
	  {
		  addColorStop(0.25, Qt::cyan);
		  addColorStop(0.5, Qt::green);
		  addColorStop(0.75, Qt::yellow);
	  }
};

class jvxQwtRasterData: public QwtRasterData
{
public:
     jvxQwtRasterData(jvxData minX,
                      jvxData maxX,
                      jvxData minY,
                      jvxData maxY,
                      jvxData minZ,
                      jvxData maxZ)
     {
          /*
          setInterval(Qt::XAxis, QwtInterval(minX, maxX));
          setInterval( Qt::YAxis, QwtInterval( minY, maxY ) );
          setInterval( Qt::ZAxis, QwtInterval( minZ, maxZ ) );
          */
          this->minX = minX;
          this->maxX = maxX;
          this->minY = minY;
          this->maxY = maxY;
          this->minZ = minZ;
          this->maxZ = maxZ;
          data = NULL;
     }

     virtual QwtInterval interval(Qt::Axis ax) const override
     {
         switch (ax)
         {
         case Qt::XAxis:
             return QwtInterval(minX, maxX);
             break;
         case Qt::YAxis:
             return QwtInterval(minY, maxY);
             break;
         case Qt::ZAxis:
             return QwtInterval(minZ, maxZ);
             break;
        }
         return QwtInterval(0, 0);
     }

     void set(jvxData *x, jvxSize numX, jvxSize numY)
     {
          data = x;
          this->numX = numX;
          this->numY = numY;
     }

     virtual double value( double x, double y ) const override
     {
          if(!data || !numX || !numY)
               return -40;

          jvxInt32 xIdx = JVX_DATA2INT32((x-minX)/(maxX-minX) * (numX - 1));
          jvxInt32 yIdx = JVX_DATA2INT32((y-minY)/(maxY-minY) * (numY - 1));
          xIdx = JVX_SIZE_INT32(JVX_MAX(JVX_MIN(xIdx, numX-1), 0));
          yIdx = JVX_SIZE_INT32(JVX_MAX(JVX_MIN(yIdx, numY-1), 0));
          return data[xIdx*numY+yIdx];
     }


private:
     jvxData *data;
     jvxData minX;
     jvxData maxX;
     jvxData minY;
     jvxData maxY;
     jvxData minZ;
     jvxData maxZ;
     jvxSize numX;
     jvxSize numY;
};

jvxQtBeampatternViewer::jvxQtBeampatternViewer(QWidget* parent,
                                               CjvxPropertiesSimple* propertyInterface,
                                               jvxData mindB,
                                               jvxData maxdB,
                                               std::string idTxt)
     : QWidget(parent)
{
     ui = new Ui::Form_jvxQtBeampatternViewer();
     ui->setupUi(this);

     plot = new QwtPlot;
     plot->setCanvasBackground(Qt::white);
     QwtText txt;
     QFont ft = txt.font();
     ft.setPixelSize(PIXEL_SIZE_FONTS);
     txt.setFont(ft);
     plot->setAxisFont(QwtPlot::xBottom, ft);
     plot->setAxisFont(QwtPlot::yLeft, ft);
     plot->setAxisFont(QwtPlot::yRight, ft);
     txt.setText("Azimuth Angle [°]");
     plot->setAxisTitle( QwtPlot::xBottom, txt );
     plot->setAxisScale(QwtPlot::xBottom, 0, 360);
     txt.setText("Frequency [Hz]");
     plot->setAxisTitle(QwtPlot::yLeft, txt);
     plot->setAxisScale(QwtPlot::yLeft, 0, 8000);
     spectrogram = new QwtPlotSpectrogram;
     spectrogram->attach(plot);
     spectrogram->setColorMap(new jvxQwtColorMap);
     rightAxis = plot->axisWidget( QwtPlot::yRight );
     txt.setText("Attenuation [dB]");
     rightAxis->setTitle(txt);
     rightAxis->setColorBarEnabled(true);
     rightAxis->setColorBarWidth(20);
     rightAxis->setColorMap(QwtInterval(-40,10), new jvxQwtColorMap);
     plot->setAxisScale( QwtPlot::yRight, -40, 10 );
     plot->enableAxis( QwtPlot::yRight );
     plot->plotLayout()->setAlignCanvasToScales( true );
     rasterData = NULL;
     ui->plotLayout->addWidget(plot);

     this->mindB = mindB;
     this->maxdB = maxdB;

     minFreqLogScale = 10; // fix to 10 Hz for now

     data = NULL;
     spectrumSize = -1;
     minAzimuth = -1;
     maxAzimuth = -1;
     numAngles = -1;
     props = NULL;

     pprefix = idTxt;
     initPropertyAccess(propertyInterface);

     QObject::connect(ui->checkBox_logarithmicFrequency, SIGNAL(toggled(bool)),
                      this, SLOT(logarithmicFrequencyToggled(bool)));
     QObject::connect(ui->checkBox_enabled, SIGNAL(toggled(bool)),
                      this, SLOT(enabledToggled(bool)));
     QObject::connect(ui->spinBox_numberAngles, SIGNAL(valueChanged(int)),
                      this, SLOT(numberAnglesChanged(int)));
     QObject::connect(ui->spinBox_FFTSize, SIGNAL(valueChanged(int)),
                      this, SLOT(FFTSizeChanged(int)));
     QObject::connect(ui->spinBox_fracDelayFilterDelay, SIGNAL(valueChanged(int)),
                      this, SLOT(fracDelayFilterDelayChanged(int)));
     QObject::connect(ui->doubleSpinBox_minAzimuth, SIGNAL(valueChanged(double)),
                      this, SLOT(minAzimuthChanged(double)));
     QObject::connect(ui->doubleSpinBox_maxAzimuth, SIGNAL(valueChanged(double)),
                      this, SLOT(maxAzimuthChanged(double)));
}


jvxQtBeampatternViewer::~jvxQtBeampatternViewer()
{
     if(data)
          delete[] data;
}
void jvxQtBeampatternViewer::initPropertyAccess(CjvxPropertiesSimple *props)
{
     // property access
     this->props = props;     
}

void jvxQtBeampatternViewer::updatePropertyPrefix(const std::string& pre)
{
    this->pprefix = pre;
}

void jvxQtBeampatternViewer::updatePlot()
{
     jvxInt16 newSpectrumSize, newNumAngles;
     jvxData newMinAzimuth, newMaxAzimuth;
     jvxInt16 enabled;
     jvxInt16 logFreq;
     jvxInt32 newSamplerate;
	 jvxCallManagerProperties callGate;

     // check property interface
     if(!props)
          return;

     // check if plot update is desired
     std::string token;
     
     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/enabled");
     props->get_property(token, &enabled, 1, 0, callGate);
     if(!enabled)
          return;

     // update dimensions
     // FIXME/BG: notification flag instead of checking all props in every frame?
     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/samplerate");
     props->get_property(token, &newSamplerate, 1, 0, callGate); // 

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/spectrumSize");
     props->get_property(token, &newSpectrumSize, 1, 0, callGate); // 

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/numAngles");
     props->get_property(token, &newNumAngles, 1, 0, callGate);

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/azimuthMin");
     props->get_property(token, &newMinAzimuth, 1, 0, callGate); // 

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/azimuthMax");
     props->get_property(token, &newMaxAzimuth, 1, 0, callGate); //

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/logFrequency");
     props->get_property(token, &logFreq, 1, 0, callGate); // 
     if(newSpectrumSize != spectrumSize || newNumAngles != numAngles || samplerate != newSamplerate
        || newMaxAzimuth != maxAzimuth || newMinAzimuth != minAzimuth )
          {
               numAngles = newNumAngles;
               spectrumSize = newSpectrumSize;
               samplerate = newSamplerate;
               maxAzimuth = newMaxAzimuth;
               minAzimuth = newMinAzimuth;
               plot->setAxisScale(QwtPlot::xBottom, minAzimuth, maxAzimuth);
               rightAxis->setColorMap(QwtInterval(mindB, maxdB), new jvxQwtColorMap);
               plot->setAxisScale( QwtPlot::yRight, mindB, maxdB );
               rasterData = new jvxQwtRasterData(minAzimuth, maxAzimuth, 0, samplerate/2, mindB, maxdB);
               spectrogram->setData(rasterData);
               logarithmicFrequencyToggled(logFreq?true:false); // also update graph axis

               // reallocate data buffer
               if(data)
                    delete[] data;
               data = new jvxData[numAngles * spectrumSize];
          }

     // get data & plot
     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/data");
     if(props->get_property(token, data, numAngles * spectrumSize, 0, callGate) == JVX_NO_ERROR) // 
          {
               rasterData->set(data, numAngles, spectrumSize);
               plot->replot();
          }
}

void jvxQtBeampatternViewer::updateControls()
{
     jvxInt16 valInt16;
     jvxData valData;
	 jvxCallManagerProperties callGate;
     std::string token;     
     
     ui->label_prefix->setText(pprefix.c_str());

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/enabled");
     props->get_property(token, &valInt16, 1, 0, callGate);
     ui->checkBox_enabled->setChecked(valInt16);

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/numAngles");
     props->get_property(token, &valInt16, 1, 0, callGate);
     ui->spinBox_numberAngles->setValue(valInt16);

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/azimuthMin");
     props->get_property(token, &valData, 1, 0, callGate); // 
     ui->doubleSpinBox_minAzimuth->setValue(valData);

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/azimuthMax");
     props->get_property(token, &valData, 1, 0, callGate); // 
     ui->doubleSpinBox_maxAzimuth->setValue(valData);

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/fftLengthLog2");
     props->get_property(token, &valInt16, 1, 0, callGate);// 
     ui->spinBox_FFTSize->setValue(valInt16);

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/fracDelayFilterDelay");
     props->get_property(token, &valInt16, 1, 0, callGate); // 
     ui->spinBox_fracDelayFilterDelay->setValue(valInt16);

     token = jvx_makePathExpr(this->pprefix, "/beampatternSimulator/logFrequency");
     props->get_property(token, &valInt16, 1, 0, callGate); // 

     logarithmicFrequencyToggled(valInt16?true:false); // also update graph axis
     ui->checkBox_logarithmicFrequency->setChecked(valInt16?true:false);
}

void jvxQtBeampatternViewer::setNotificationFlag()
{
     jvxInt16 valInt16 = 1;
	 jvxCallManagerProperties callGate;
     std::string token = jvx_makePathExpr(pprefix, "/beampatternSimulator/syncNotify");
     props->set_property(token, &valInt16, 1, 0, callGate); // 
}

void jvxQtBeampatternViewer::enabledToggled(bool value)
{
     jvxInt16 valInt16 = (jvxInt16) value;
	 jvxCallManagerProperties callGate; 
     std::string token = jvx_makePathExpr(pprefix, "/beampatternSimulator/enabled");
	 props->set_property(token, &valInt16, 1, 0, callGate);
}

void jvxQtBeampatternViewer::numberAnglesChanged(int value)
{
     jvxInt16 valInt16 = (jvxInt16) value;
	 jvxCallManagerProperties callGate; 
     std::string token = jvx_makePathExpr(pprefix, "/beampatternSimulator/numAngles");
	 props->set_property(token, &valInt16, 1, 0, callGate);
     setNotificationFlag();
     updateControls();
}

void jvxQtBeampatternViewer::minAzimuthChanged(double value)
{
     jvxData valData = (jvxData) value;
	 jvxCallManagerProperties callGate; 
     std::string token = jvx_makePathExpr(pprefix, "/beampatternSimulator/azimuthMin");
	 props->set_property(token, &valData, 1, 0, callGate); // 
     setNotificationFlag();
     updateControls();
}

void jvxQtBeampatternViewer::maxAzimuthChanged(double value)
{
     jvxData valData = (jvxData) value;
	 jvxCallManagerProperties callGate; 
     std::string token = jvx_makePathExpr(pprefix, "/beampatternSimulator/azimuthMax");
	 props->set_property(token, &valData, 1, 0, callGate); // 
     setNotificationFlag();
     updateControls();
}

void jvxQtBeampatternViewer::FFTSizeChanged(int value)
{
     jvxInt16 valInt16 = (jvxInt16) value;
	 jvxCallManagerProperties callGate;
     std::string token = jvx_makePathExpr(pprefix, "/beampatternSimulator/fftLengthLog2");
     props->set_property(token, &valInt16, 1, 0, callGate); // 
     setNotificationFlag();
     updateControls();
}

void jvxQtBeampatternViewer::fracDelayFilterDelayChanged(int value)
{
     jvxInt16 valInt16 = (jvxInt16) value;
	 jvxCallManagerProperties callGate;
     std::string token = jvx_makePathExpr(pprefix, "/beampatternSimulator/fracDelayFilterDelay");
     props->set_property(token, &valInt16, 1, 0, callGate); // 
     setNotificationFlag();
     updateControls();
}

void jvxQtBeampatternViewer::logarithmicFrequencyToggled(bool value)
{
     jvxInt16 valInt16 = (jvxInt16) value;
	 jvxCallManagerProperties callGate;
     if(value)
          {
               plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
               plot->setAxisScale(QwtPlot::yLeft, minFreqLogScale, samplerate/2);
          }
     else
          {
               plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
               plot->setAxisScale(QwtPlot::yLeft, 0, samplerate/2);
          }
     std::string token = jvx_makePathExpr(pprefix, "/beampatternSimulator/logFrequency");
     props->set_property(token, &valInt16, 1, 0, callGate); // 
}
