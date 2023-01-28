#ifndef __JVX_QT_BEAMPATTERN_VIEWER__H__
#define __JVX_QT_BEAMPATTERN_VIEWER__H__

#include "jvx.h"
#include <map>
#include <QWidget>

// forward declarations
namespace Ui {
     class Form_jvxQtBeampatternViewer;
}
class CjvxPropertiesSimple;
class QwtPlot;
class QwtPlotSpectrogram;
class jvxQwtRasterData;
class QwtScaleWidget;

// class declaration
class jvxQtBeampatternViewer : public QWidget 
{
    std::string pprefix;

     Q_OBJECT

public:

     jvxQtBeampatternViewer(QWidget* parent,
                            CjvxPropertiesSimple *propertyInterface,
                            jvxData mindB,
                            jvxData maxdB,
                            std::string idTxt = "");
     ~jvxQtBeampatternViewer();

     void updatePlot();
     void updateControls();
     void updatePropertyPrefix(const std::string& pprep = "");

public slots:

     void enabledToggled(bool);
     void numberAnglesChanged(int);
     void minAzimuthChanged(double);
     void maxAzimuthChanged(double);
     void FFTSizeChanged(int);
     void fracDelayFilterDelayChanged(int);
     void logarithmicFrequencyToggled(bool);

private:
     void setNotificationFlag();

     void initPropertyAccess(CjvxPropertiesSimple *props);

     jvxData *data;

     QwtPlot *plot;
     QwtPlotSpectrogram *spectrogram;
     jvxQwtRasterData *rasterData;
     QwtScaleWidget *rightAxis;

     jvxData mindB;
     jvxData maxdB;

     jvxData minAzimuth;
     jvxData maxAzimuth;

     jvxInt16 spectrumSize;
     jvxInt16 numAngles;
     jvxInt32 samplerate;

     jvxData minFreqLogScale;

     Ui::Form_jvxQtBeampatternViewer *ui;

     CjvxPropertiesSimple *props;
     //std::string propSamplerate;
     //std::string propSpectrumSize;
     //std::string propMinAzimuth;
     //std::string propMaxAzimuth;
     // std::string propNumAngles;
     // std::string propEnabled;
     //std::string propFFTLengthLog2;
     //std::string propFracDelayFilterDelay;
     //std::string propNotificationFlag;
     //std::string propData;
     //std::string propLogFrequency;
};


#endif
