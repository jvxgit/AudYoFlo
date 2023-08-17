#ifndef __JVXQTACOUSTICMEASUREMENT_H__
#define __JVXQTACOUSTICMEASUREMENT_H__

#include "interfaces/qt/IjvxQtSpecificWidget.h"

typedef enum
{
	JVX_ACOUSTIC_MEASURE_TASK_NONE = 0,
	JVX_ACOUSTIC_MEASURE_TASK_EQUALIZE /* jvxMeasurementTaskEqualize */,
	JVX_ACOUSTIC_MEASURE_TASK_DELAY,
	JVX_ACOUSTIC_MEASURE_TASK_STORE_DELAY,
	JVX_ACOUSTIC_MEASURE_TASK_STORE_EQUALIZER,
	JVX_ACOUSTIC_MEASURE_TASK_PASS_TWO_MEASURED_IRS /* jvxMeasurementTaskPass2Ir */
} jvxMeasurementDataProcessorTask;

struct jvxMeasurementTaskEqualize
{
	jvxData* ir_measured = nullptr;
	jvxSize len_ir = 0;
	jvxData* ir_processed = nullptr;

	jvxSize len_markers_measured = 0;
	jvxData* markers_measured_x = nullptr;
	jvxData* markers_measured_y = nullptr;

	jvxSize len_markers_desired = 0;
	jvxData* markers_desired_x = nullptr;
	jvxData* markers_desired_y = nullptr;

	const char* dataTag = nullptr;
	const char* measName = nullptr;
	const char* chanName = nullptr;

	jvxSize samplerate = JVX_SIZE_UNSELECTED;
};

struct jvxMeasurementTaskPass2Ir
{
	jvxData* ir_data1 = nullptr;
	jvxSize ir_data1_len = 0;
	jvxSize ir_data1_rate = JVX_SIZE_UNSELECTED;
	const char* ir_data1_meas_name = nullptr;
	const char* ir_data1_chan_name = nullptr;

	jvxData* ir_data2 = nullptr;
	jvxSize ir_data2_len = 0;
	jvxSize ir_data2_rate = JVX_SIZE_UNSELECTED;
	const char* ir_data2_meas_name = nullptr;
	const char* ir_data2_chan_name = nullptr;

};

JVX_INTERFACE  IjvxQtAcousticMeasurement;

JVX_INTERFACE IjvxQtAcousticMeasurement_process
{
public:
	virtual ~IjvxQtAcousticMeasurement_process() {};

	virtual void init(IjvxQtAcousticMeasurement* refMeasure) = 0;
	virtual void terminate() = 0;
	virtual jvxErrorType process_data(jvxMeasurementDataProcessorTask task, jvxHandle* fld, jvxSize tagId) = 0;

	virtual QWidget* my_widget() = 0;
};

JVX_INTERFACE IjvxQtAcousticMeasurement : public IjvxQtSpecificHWidget
{
public:
	virtual ~IjvxQtAcousticMeasurement() {};

	virtual jvxErrorType register_data_processor(
		const char* descr,
		jvxMeasurementDataProcessorTask task,
		IjvxQtAcousticMeasurement_process* fld,
		jvxSize tagId) = 0;

	virtual jvxErrorType unregister_data_processor(
		jvxMeasurementDataProcessorTask task,
		IjvxQtAcousticMeasurement_process* fld, 
		jvxSize tagId) = 0;

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
