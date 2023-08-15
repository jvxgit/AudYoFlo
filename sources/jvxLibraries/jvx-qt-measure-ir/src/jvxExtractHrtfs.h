#ifndef __JVXEXTRACTHRTFS__H__
#define __JVXEXTRACTHRTFS__H__

#include "jvx.h"
#include "ui_jvxExtractHrtfs.h"
#include "jvxQtAcousticMeasurement.h"

extern "C"
{
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_fft_tools.h"
#include "jvx_math/jvx_complex.h"
#include "jvx_circbuffer/jvx_circbuffer.h"
}

enum class jvxIrOrderViewHrtf
{
	JVX_IR_VIEW_LEFT_FIRST,
	JVX_IR_VIEW_RIGHT_FIRST
};

class irDataSet
{
public:
	QVector<jvxData> irMeasured;
	std::string nameMeasurement;
	std::string nameChannel;
	jvxSize rate = JVX_SIZE_UNSELECTED;
};

class jvxExtractHrtfs: public QWidget, public IjvxQtAcousticMeasurement_process, public Ui::widgetExtractHrtfs
{
	IjvxQtAcousticMeasurement* refMeasure;

	Q_OBJECT

	jvxIrOrderViewHrtf viewOrder = jvxIrOrderViewHrtf::JVX_IR_VIEW_LEFT_FIRST; // -> ComboBox <>
	irDataSet setData1;
	irDataSet setData2;
	jvxSize lHrtfs = 256;
	jvxSize offsetHrtfs = 0;
	jvxBool autoComputeStart = true;
	jvxData startThresholdHrtf = 0.1;
	jvxInt32 showDisplacement = 1;
	std::string hrtfBufShow;

	QVector<jvxData> leftDataY;
	QVector<jvxData> rightDataY;
	QVector<jvxData> leftRightDataX;



public:
	jvxExtractHrtfs(QWidget* parent);
	~jvxExtractHrtfs();

	void init(IjvxQtAcousticMeasurement* refMeasure);
	void terminate();

	virtual jvxErrorType process_data(jvxMeasurementDataProcessorTask task, jvxHandle* fld) override;

	void compute_auto();
	void replot_hrtfs();
	void update_window();
	
signals:

public slots:

	// void trigger_recompute();
	void trigger_clipboard_ir();
	void update_parameter_show();
	void toggle_autoextract(bool);
	void changed_displacement();
	void change_threshold_offset();
	void change_l_hrtf();
	void change_show_offset();

};

#endif