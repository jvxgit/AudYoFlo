#ifndef __JVX_QT_SPECTRUM_VIEWER__H__
#define __JVX_QT_SPECTRUM_VIEWER__H__

#include "jvx.h"
#include <map>
#include <QWidget>
#include "jvxQtSaCustomBase.h"

// forward declarations
namespace Ui {
     class Form_jvxQtSpectrumViewer;
}
class CjvxPropertiesSimple;
class QwtLinearScaleEngine;
class QwtLogScaleEngine;
class QwtPlot;
class QwtPlotGrid;
class QwtPlotCurve;

// class declaration
class jvxQtSaSpectrumViewer : public QWidget, public jvxQtSaCustomBase
{
    std::string pprefix;
     
    Q_OBJECT
        
public:

     jvxQtSaSpectrumViewer(QWidget* parent,
                         CjvxPropertiesSimple *propertyInterface,
                         jvxData mindB,
                         jvxData maxdB,
                         const std::string& idTxt = "");
     ~jvxQtSaSpectrumViewer();


     void updatePlot(std::string name,
                     int channel,
                     QColor color);
     void updateControls();
     void updatePropertyPrefix(const std::string& pre);


public slots:

     void methodChanged(int value);
     void windowChanged(int value);
     void winArgAChanged(double value);
     void winArgBChanged(double value);
     void FFTSizeChanged(int value);
     void alphaChanged(double value);
     void logarithmicAmplitudeToggled(bool value);
     void logarithmicFrequencyToggled(bool value);
     void enabledToggled(bool value);

private:
     void setNotificationFlag();
     void initPropertyAccess(CjvxPropertiesSimple *props);

     QwtPlot *plot;
     QwtPlotGrid *grid;
     std::map<std::string, QwtPlotCurve*> curves;
     jvxSize spectrumSize;
     jvxData *freqAxis;
     jvxInt32 samplerate;
     jvxData minFreqLogScale;
     jvxInt16 skipBins;
     jvxData *data;
     jvxData mindB, maxdB;
     Ui::Form_jvxQtSpectrumViewer *ui; 

     CjvxPropertiesSimple *props;
     /*
     std::string propNumberChannels;
     std::string propSamplerate;
     std::string propData;
     std::string propSize;
     std::string propMethod;
     std::string propEnabled;
     std::string propSyncNotify;
     std::string propWindow;
     std::string propWinArgA;
     std::string propWinArgB;
     std::string propFFTLengthLog2;
     std::string propAlpha;
     std::string propLogarithmic;
     std::string propLogFrequency;
     */
};


#endif
