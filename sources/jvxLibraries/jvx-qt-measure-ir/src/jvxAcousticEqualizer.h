#ifndef __JVXACOUSTICEQUALIZER__H__
#define __JVXACOUSTICEQUALIZER__H__

#include "jvx.h"
#include "ui_jvxAcousticEqualizer.h"
#include "jvxQtAcousticMeasurement.h"

extern "C"
{
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_fft_tools.h"
#include "jvx_math/jvx_complex.h"
#include "jvx_circbuffer/jvx_circbuffer.h"
}

class jvxAcousticEqualizer: public QWidget, public IjvxQtAcousticMeasurement_process, public Ui::widgetEqualize
{
	IjvxQtAcousticMeasurement* refMeasure;

	Q_OBJECT

	jvxBool plotFdLinearx;
	jvxSize rate;
	jvxSize lIrMeasured;
	
	QVector<jvxData> irProcessed;
	QVector<jvxData> irModification;
	QVector<jvxData> irMeasured;

	QVector<jvxData> marker_meas_x;
	QVector<jvxData> marker_meas_y;
	QVector<jvxData> marker_desired_x;
	QVector<jvxData> marker_desired_y;

	jvxSize lIrEqualizer;
	jvxSize irDelay;
	jvxSize lFFTEqualizer;
	jvxSize lFFTShow;

	jvxData gaindB;
	jvxSize winId;

	std::string dataTag;
	std::string measureName;
	std::string chanName;

	jvxFFTSize szFftTypeEq;
	jvxSize szFftEq;

	jvxFFTSize szFftTypeShow;
	jvxSize szFftShow;

	QVector<jvxData> transfer_desired_dB;
	QVector<jvxData> transfer_desired_f;

	QVector<jvxData> equalizer_ir_y;
	QVector<jvxData> equalizer_ir_gain_y;
	QVector<jvxData> equalizer_ir_x;

	QVector<jvxData> magnitudeProcessed_x;
	QVector<jvxData> magnitudeProcessed_y;

	QVector<jvxData> magnitudeUnprocessed_x;
	QVector<jvxData> magnitudeUnprocessed_y;

	QVector<jvxData> magnitudeIr_x;
	QVector<jvxData> magnitudeIr_y;

public:
	jvxAcousticEqualizer(QWidget* parent);
	~jvxAcousticEqualizer();

	void init(IjvxQtAcousticMeasurement* refMeasure);
	void terminate();

	virtual jvxErrorType process_data(jvxMeasurementDataProcessorTask task, jvxHandle* fld) override;

	void q_marker_idx(QVector<jvxData>& marker_x, jvxSize& idxMarkLow, jvxSize& idxMarkUp, jvxData f);

	void replot_f_ir();
	void replot_t_ir();
	void replot_f_tf();

	void compute_all();
	void update_window();
	void update_eq_ir(jvxBool recompute = true);
	
	void compute_equalizer();
	void compute_proc_ir();

signals:

public slots:

	void trigger_recompute();
	void trigger_clipboard_ir();
	void changed_l_ir_eq();
	void activated_fft_ir(int idx);
	void activated_fft_show(int idx);
	void activated_freq_sale(int idx);
	void changed_gaindB();
	void activated_win(int idx);
};

#endif