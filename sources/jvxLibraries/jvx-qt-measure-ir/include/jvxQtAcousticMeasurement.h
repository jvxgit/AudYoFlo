#ifndef __JVXQTACOUSTICMEASUREMENT_H__
#define __JVXQTACOUSTICMEASUREMENT_H__

#include "interfaces/qt/IjvxQtSpecificWidget.h"

typedef enum
{
	JVX_ACOUSTIC_MEASURE_TASK_NONE = 0,
	JVX_ACOUSTIC_MEASURE_TASK_EQUALIZE /* jvxMeasurementTaskEqualize */,
	JVX_ACOUSTIC_MEASURE_TASK_DELAY,
	JVX_ACOUSTIC_MEASURE_TASK_STORE_DELAY,
	JVX_ACOUSTIC_MEASURE_TASK_STORE_EQUALIZER
} jvxMeasurementDataProcessorTask;

struct jvxMeasurementTaskEqualize
{
	jvxData* ir_measured;
	jvxSize len_ir;
	jvxData* ir_processed;

	jvxSize len_markers_measured;
	jvxData* markers_measured_x;
	jvxData* markers_measured_y;

	jvxSize len_markers_desired;
	jvxData* markers_desired_x;
	jvxData* markers_desired_y;

	const char* dataTag;
	const char* measName;
	const char* chanName;

	jvxSize samplerate;
};

JVX_INTERFACE IjvxQtAcousticMeasurement_process
{
public:
	virtual ~IjvxQtAcousticMeasurement_process() {};

	virtual jvxErrorType process_data(jvxMeasurementDataProcessorTask task, jvxHandle* fld) = 0;
};

JVX_INTERFACE IjvxQtAcousticMeasurement : public IjvxQtSpecificHWidget
{
public:
	virtual ~IjvxQtAcousticMeasurement() {};

	virtual jvxErrorType register_data_processor(
		const char* descr,
		jvxMeasurementDataProcessorTask task,
		IjvxQtAcousticMeasurement_process* fld) = 0;

	virtual jvxErrorType unregister_data_processor(
		jvxMeasurementDataProcessorTask task,
		IjvxQtAcousticMeasurement_process* fld) = 0;

	// =================================================

	virtual jvxErrorType invite_processor(
		IjvxQtAcousticMeasurement* fld) = 0;

	virtual jvxErrorType goodbye_processor(
		IjvxQtAcousticMeasurement* fld) = 0;

	// =================================================
};

// ===================================================================================================

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtSpecificHWidget, jvxQtMeasureControl);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtSpecificHWidget, jvxQtMeasureControl);

#endif
