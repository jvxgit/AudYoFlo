#ifndef __IJVXQTAUDIOWIDGET_H__
#define __IJVXQTAUDIOWIDGET_H__

#include "interfaces/qt/IjvxQtSpecificWidget.h"

typedef enum
{
	JVX_QT_AUDIO_WIDGETS_OUTSIDE_CONTROL = 0x1,
	JVX_QT_AUDIO_WIDGETS_LOCK_REM_IR = 0x2
} jvxQtMeasureModeTypes;

JVX_INTERFACE IjvxQtMeasureAudio_ir: public IjvxQtSpecificHWidget
{
 public:
	virtual ~IjvxQtMeasureAudio_ir() {};
	virtual jvxErrorType number_measurements(jvxSize& num) = 0;
	virtual jvxErrorType name_measurement(jvxSize id, std::string& nm) = 0;
	virtual jvxErrorType lock_data_measurements(jvxSize id, jvxData** data, jvxSize* numEntries) = 0;
	virtual jvxErrorType unlock_data_measurements(jvxSize id) = 0;
	virtual jvxErrorType remove_measurement(jvxSize idRem) = 0;

};

// Qt Measure Audio Ir
JVX_QT_WIDGET_INIT_DECLARE(IjvxQtSpecificHWidget, jvxQtMeasureAudioJitter);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtSpecificHWidget, jvxQtMeasureAudioJitter);

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtMeasureAudio_ir, jvxQtMeasureAudioIr);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtMeasureAudio_ir, jvxQtMeasureAudioIr);

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtSpecificHWidget, jvxQtMeasureAudioSpectrum);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtSpecificHWidget, jvxQtMeasureAudioSpectrum);


#endif
