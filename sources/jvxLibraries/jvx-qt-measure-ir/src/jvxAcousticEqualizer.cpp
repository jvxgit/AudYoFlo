#include "jvxAcousticEqualizer.h"
#include "QClipboard"
#include "jvx-qcp-qt-helpers.h"

#define JVX_LOG10_EPS 1e-10

extern "C"
{
#include "jvx_windows/jvx_windows.h"
}

typedef struct
{
	jvxInt16 winTp;
	const char* descr;
} oneWinType;

#define JVX_NUMBER_WIN_TYPES 5
static oneWinType windowTypes[JVX_NUMBER_WIN_TYPES] =
{
	{JVX_WINDOW_RECT, "RECT"},
	{JVX_WINDOW_HAMMING, "HAMMING" },
	{JVX_WINDOW_HANN, "HANN"},
	{JVX_WINDOW_BLACKMAN, "BLACKMAN"},
	{JVX_WINDOW_SQRT_FLATTOP_HANN, "SQRT HANN"}
};

jvxAcousticEqualizer::jvxAcousticEqualizer(QWidget* parent) : QWidget(parent)
{
	refMeasure = NULL;

	rate = 48000;
	lIrMeasured = 0;
	lIrEqualizer = 511;
	irDelay = lIrEqualizer/2;
	szFftTypeShow = JVX_FFT_TOOLS_FFT_SIZE_1024;
	szFftShow = 1024;
	gaindB = 0;
	winId = 0;
	plotFdLinearx = true;
	update_eq_ir(false);
	/*
	irDelay = lIrEqualizer / 2;
	jvx_get_nearest_size_fft(&szFftTypeEq, lIrEqualizer, JVX_FFT_ROUND_UP, &szFftEq);
	*/
}

jvxAcousticEqualizer::~jvxAcousticEqualizer()
{
}

JVX_FFT_TOOLS_DEFINE_FFT_SIZES

void
jvxAcousticEqualizer::init(IjvxQtAcousticMeasurement* refMeasureArg)
{
	jvxSize i;

	refMeasure = refMeasureArg;

	this->setupUi(static_cast<QWidget*>(this));
	
	this->setWindowTitle("Acoustic Measument - equalizer");

	if (refMeasure)
	{
		refMeasure->register_data_processor("Equalize IR",
			JVX_ACOUSTIC_MEASURE_TASK_EQUALIZE,
			static_cast<IjvxQtAcousticMeasurement_process*>(this), 
			JVX_SIZE_UNSELECTED);
	}

	for (i = 0; i < JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE; i++)
	{
		comboBox_fft_eq->addItem(("FFT SIZE " + jvx_size2String(jvxFFTSize_sizes[i])).c_str());
		comboBox_fft_show->addItem(("FFT SIZE " + jvx_size2String(jvxFFTSize_sizes[i])).c_str());
	}
	qcp_display_ir_fd->addGraph();
	qcp_display_ir_fd->addGraph();
	qcp_display_ir_fd->graph(0)->setVisible(true);
	qcp_display_ir_fd->graph(1)->setVisible(true);
	jvx_qcp_set_color_graph(qcp_display_ir_fd, 0, Qt::black, Qt::SolidLine, QCPGraph::lsLine, QCPScatterStyle::ssNone);
	jvx_qcp_set_color_graph(qcp_display_ir_fd, 1, Qt::green, Qt::DotLine, QCPGraph::lsLine, QCPScatterStyle::ssNone);

	qcp_display_eq_td->addGraph();
	qcp_display_eq_td->graph(0)->setVisible(true);

	qcp_resulting_mag->addGraph();
	qcp_resulting_mag->addGraph();
	qcp_resulting_mag->graph(0)->setVisible(true);
	qcp_resulting_mag->graph(1)->setVisible(true);
	jvx_qcp_set_color_graph(qcp_resulting_mag, 0, Qt::blue, Qt::SolidLine, QCPGraph::lsLine, QCPScatterStyle::ssNone);
	jvx_qcp_set_color_graph(qcp_resulting_mag, 1, Qt::red, Qt::DotLine, QCPGraph::lsLine, QCPScatterStyle::ssNone);

	comboBox_freqscale->addItem("Linear");
	comboBox_freqscale->addItem("LogScale");

	for (i = 0; i < JVX_NUMBER_WIN_TYPES; i++)
	{
		comboBox_window->addItem(windowTypes[i].descr);
	}
}

void
jvxAcousticEqualizer::terminate()
{
	if (refMeasure)
	{
		refMeasure->unregister_data_processor(
			JVX_ACOUSTIC_MEASURE_TASK_EQUALIZE,
			static_cast<IjvxQtAcousticMeasurement_process*>(this), JVX_SIZE_UNSELECTED);
	}
	refMeasure = NULL;
}

jvxErrorType
jvxAcousticEqualizer::process_data(jvxMeasurementDataProcessorTask task, jvxHandle* fld, jvxSize tagId)
{
	if (task != jvxMeasurementDataProcessorTask::JVX_ACOUSTIC_MEASURE_TASK_EQUALIZE)
	{
		return JVX_ERROR_UNSUPPORTED;
	}
	jvxMeasurementTaskEqualize* eq_data = (jvxMeasurementTaskEqualize*)fld;
	lIrMeasured = eq_data->len_ir;
	irMeasured.resize(lIrMeasured);
	memcpy(irMeasured.data(), eq_data->ir_measured, sizeof(jvxData)* lIrMeasured);

	marker_meas_x.resize(eq_data->len_markers_measured);
	marker_meas_y.resize(eq_data->len_markers_measured);
	memcpy(marker_meas_x.data(), eq_data->markers_measured_x, sizeof(jvxData)* marker_meas_x.size());
	memcpy(marker_meas_y.data(), eq_data->markers_measured_y, sizeof(jvxData)* marker_meas_y.size());

	marker_desired_x.resize(eq_data->len_markers_desired);
	marker_desired_y.resize(eq_data->len_markers_desired);
	memcpy(marker_desired_x.data(), eq_data->markers_desired_x, sizeof(jvxData)* marker_desired_x.size());
	memcpy(marker_desired_y.data(), eq_data->markers_desired_y, sizeof(jvxData)* marker_desired_y.size());

	dataTag = eq_data->dataTag;
	measureName = eq_data->measName;
	chanName = eq_data->chanName;

	compute_all();
	update_window();

	if (eq_data->ir_processed)
	{
		assert(irProcessed.size() == eq_data->len_ir);
		memcpy(eq_data->ir_processed, irProcessed.data(), sizeof(jvxData) * irProcessed.size());
	}
	return JVX_NO_ERROR;
}

QWidget*
jvxAcousticEqualizer::my_widget()
{
	return static_cast<QWidget*>(this);
}

// ====================================================================================

void
jvxAcousticEqualizer::update_window()
{
	jvxSize i;
	lineEdit_srate->setText(jvx_size2String(rate).c_str());
	lineEdit_l_ir_measured->setText(jvx_size2String(lIrMeasured).c_str());
	lineEdit_l_ir_equalization->setText(jvx_size2String(lIrEqualizer).c_str());

	lineEdit_datatag->setText(dataTag.c_str());
	label_chan->setText(chanName.c_str());
	label_meas->setText(measureName.c_str());

	i = 0;
	for (; i < JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE; i++)
	{
		if (i == szFftTypeEq)
		{
			break;
		}
	}
	if( i < comboBox_fft_eq->count())
	{ 
		comboBox_fft_eq->setCurrentIndex(i);
	}

	i = 0;
	for (; i < JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE; i++)
	{
		if (i == szFftTypeShow)
		{
			break;
		}
	}
	if (i < comboBox_fft_show->count())
	{
		comboBox_fft_show->setCurrentIndex(i);
	}

	lineEdit_gaindB->setText(jvx_data2String(gaindB, 2).c_str());
	comboBox_window->setCurrentIndex(winId);
}

void
jvxAcousticEqualizer::compute_all()
{

	compute_equalizer();
	replot_t_ir();

	// =====================================================================================
	
	compute_proc_ir();
	replot_f_ir();
	replot_f_tf();
}

void
jvxAcousticEqualizer::compute_equalizer()
{
	jvxSize i;
	jvxFFTGlobal* globFft = NULL;
	jvxFFT* myFft = NULL;
	jvxDataCplx* inFld = NULL;
	jvxData* outFld = NULL;
	jvxSize N;

	// jvx_fft_direct_apply_fixed_size(jvxData * inBuf_n, jvxData * outBuf_n2p1, jvxFFTSize szFft, jvxSize n, jvxCBool outlog10)

	jvx_get_nearest_size_fft(&szFftTypeEq, lIrEqualizer, JVX_FFT_ROUND_UP, &szFftEq);

	std::cout << "Length FIR Equalize: " << lIrEqualizer << ", derived FFT size: " << szFftEq << std::endl;
	equalizer_ir_y.resize(lIrEqualizer);
	equalizer_ir_gain_y.resize(lIrEqualizer);
	equalizer_ir_x.resize(lIrEqualizer);

	jvxSize fftSz2P1 = szFftEq / 2 + 1;

	transfer_desired_dB.resize(fftSz2P1);
	transfer_desired_f.resize(fftSz2P1);

	jvx_create_fft_ifft_global(&globFft, szFftTypeEq);
	jvx_create_ifft_complex_2_real(&myFft, globFft, szFftTypeEq,
		&inFld, &outFld, &N, JVX_FFT_IFFT_EFFICIENT, NULL, NULL, 0);

	for (i = 0; i < fftSz2P1; i++)
	{
		jvxData scalef = ((jvxData)rate / 2.0) / (jvxData)(fftSz2P1 - 1);
		jvxSize f = (jvxData)i * scalef;
		transfer_desired_f[i] = f;

		jvxSize idxMarkMeasLow = JVX_SIZE_UNSELECTED;
		jvxSize idxMarkMeasUp = JVX_SIZE_UNSELECTED;
		jvxSize idxMarkDesiredLow = JVX_SIZE_UNSELECTED;
		jvxSize idxMarkDesiredUp = JVX_SIZE_UNSELECTED;

		transfer_desired_f[i] = f;

		q_marker_idx(marker_meas_x, idxMarkMeasLow, idxMarkMeasUp, f);
		q_marker_idx(marker_desired_x, idxMarkDesiredLow, idxMarkDesiredUp, f);

		jvxData lowValMeasy = 0;
		jvxData lowValDesiredy = 0;
		jvxData highValMeasy = 0;
		jvxData highValDesiredy = 0;

		jvxData lowValMeasx = 0;
		jvxData lowValDesiredx = 0;
		jvxData highValMeasx = rate / 2;
		jvxData highValDesiredx = rate / 2;

		if (JVX_CHECK_SIZE_SELECTED(idxMarkMeasLow))
		{
			lowValMeasy = marker_meas_y[idxMarkMeasLow];
			lowValMeasx = marker_meas_x[idxMarkMeasLow];
		}
		if (JVX_CHECK_SIZE_SELECTED(idxMarkDesiredLow))
		{
			lowValDesiredy = marker_desired_y[idxMarkDesiredLow];
			lowValDesiredx = marker_desired_x[idxMarkDesiredLow];
		}
		if (JVX_CHECK_SIZE_SELECTED(idxMarkMeasUp))
		{
			highValMeasy = marker_meas_y[idxMarkMeasUp];
			highValMeasx = marker_meas_x[idxMarkMeasUp];
		}
		if (JVX_CHECK_SIZE_SELECTED(idxMarkDesiredUp))
		{
			highValDesiredy = marker_desired_y[idxMarkDesiredUp];
			highValDesiredx = marker_desired_x[idxMarkDesiredUp];
		}

		jvxData valMeasy = lowValMeasy + (f - lowValMeasx) / (highValMeasx - lowValMeasx) * (highValMeasy - lowValMeasy);
		jvxData valDesiredy = lowValDesiredy + (f - lowValDesiredx) / (highValDesiredx - lowValDesiredx) * (highValDesiredy - lowValDesiredy);

		transfer_desired_dB[i] = valDesiredy - valMeasy;
		inFld[i].re = pow(10, transfer_desired_dB[i] / 20.0);
		inFld[i].im = 0;
	}

	jvx_execute_ifft(myFft);

	jvxData scaleFFT = 1.0 / (jvxData)szFftEq;
	jvxData gainLin = pow(10.0, gaindB / 20.0);
	for (i = 0; i < lIrEqualizer; i++)
	{
		jvxSize idx = (i + szFftEq - irDelay) % szFftEq;
		equalizer_ir_y[i] = outFld[idx] * scaleFFT;
		equalizer_ir_x[i] = i;
	}

	switch (winId)
	{
	case JVX_WINDOW_HAMMING:
		jvx_hamming_window_it(equalizer_ir_y.data(), lIrEqualizer);
		break;
	case JVX_WINDOW_HANN:
		jvx_hann_window_it(equalizer_ir_y.data(), lIrEqualizer);
		break;
	case JVX_WINDOW_BLACKMAN:
		jvx_blackman_window_it(equalizer_ir_y.data(), lIrEqualizer);
		break;
	case JVX_WINDOW_SQRT_FLATTOP_HANN:
		jvx_sqrt_hann_window_it(equalizer_ir_y.data(), lIrEqualizer);
		break;
	}

	for (i = 0; i < lIrEqualizer; i++)
	{
		equalizer_ir_gain_y[i] = equalizer_ir_y[i] * gainLin;
	}

	jvx_destroy_ifft(myFft);
	inFld = NULL;
	outFld = NULL;
	jvx_destroy_fft_ifft_global(globFft);
}

void 
jvxAcousticEqualizer::trigger_recompute()
{
	compute_all();
	update_window();
}

void
jvxAcousticEqualizer::compute_proc_ir()
{
	jvxSize i;
	jvx_circbuffer* theFilterModul = NULL;
	jvxSize N = lIrMeasured;
	jvxSize N2P1 = N / 2 + 1;	

	irProcessed = irMeasured;

	// =====================================================================================
	// Filter unprocessed with eq IR
	// =====================================================================================
	jvxData* bufIn = irMeasured.data();
	jvxData* bufOut = irProcessed.data();
	jvx_circbuffer_allocate_1chan(&theFilterModul, lIrEqualizer - 1);
	jvx_circbuffer_fir_1can_2io(theFilterModul, equalizer_ir_gain_y.data(),
		&bufIn, &bufOut, irMeasured.size());
	jvx_circbuffer_deallocate(theFilterModul);
	theFilterModul = NULL;

	// =====================================================================================
	// Remove the delay
	memmove(bufOut, bufOut + irDelay, sizeof(jvxData) * (irProcessed.size() - irDelay));
	
	// =====================================================================================
	// Plot solely the modification
	irModification.clear();
	irModification.resize(lIrMeasured);
	jvxSize minCp = JVX_MIN(lIrMeasured, lIrEqualizer);
	memcpy(irModification.data(), equalizer_ir_gain_y.data(), sizeof(jvxData) * minCp);

	// =====================================================================================
	// Compute the magnitudes	
	magnitudeUnprocessed_x.resize(N2P1);
	magnitudeUnprocessed_y.resize(N2P1);
	magnitudeProcessed_x.resize(N2P1);
	magnitudeProcessed_y.resize(N2P1);
	magnitudeIr_x.resize(N2P1);
	magnitudeIr_y.resize(N2P1);

	jvx_fft_direct_apply_arbitrary_size(irMeasured.data(), magnitudeUnprocessed_y.data(), N, c_true);
	jvx_fft_direct_apply_arbitrary_size(irProcessed.data(), magnitudeProcessed_y.data(), N, c_true);
	jvx_fft_direct_apply_arbitrary_size(irModification.data(), magnitudeIr_y.data(), N, c_true);

	for (i = 0; i < N2P1; i++)
	{
		magnitudeUnprocessed_x[(int)i] = rate / (jvxData)N * (jvxData)i;
		magnitudeProcessed_x[(int)i] = rate / (jvxData)N * (jvxData)i;
		magnitudeIr_x[(int)i] = rate / (jvxData)N * (jvxData)i;
	}
}

void
jvxAcousticEqualizer::q_marker_idx(QVector<jvxData>& marker_x, jvxSize& idxMarkLow, jvxSize& idxMarkUp, jvxData f)
{
	idxMarkLow = 0;
	idxMarkUp = marker_x.size() - 1;

	if (marker_x.size() == 0)
	{
		idxMarkLow = JVX_SIZE_UNSELECTED;
		idxMarkUp = JVX_SIZE_UNSELECTED;
		return;
	}

	if (f < marker_x[0])
	{
		idxMarkLow = JVX_SIZE_UNSELECTED;
		idxMarkUp = 0;
		return;
	}
	if (f > marker_x[marker_x.size() - 1])
	{
		idxMarkLow = marker_x.size() - 1;
		idxMarkUp = JVX_SIZE_UNSELECTED;
		return;
	}
	while (idxMarkUp > idxMarkLow + 1)
	{
		jvxSize idxBiSection = (idxMarkUp + idxMarkLow) / 2;
		if (f <= marker_x[idxBiSection])
		{
			idxMarkUp = idxBiSection;
		}
		else
		{
			idxMarkLow = idxBiSection;
		}
	}
}

void
jvxAcousticEqualizer::replot_f_ir()
{
	jvxSize i;
	jvxData xlimmin = transfer_desired_f[0];
	jvxData xlimmax = transfer_desired_f[transfer_desired_f.size() - 1];

	jvxData ylimmin = JVX_DATA_MAX_POS;
	jvxData ylimmax = JVX_DATA_MAX_NEG;

	for (i = 0; i < transfer_desired_dB.size(); i++)
	{
		if (transfer_desired_dB[i] > ylimmax)
		{
			ylimmax = transfer_desired_dB[i];
		}
		if (transfer_desired_dB[i] < ylimmin)
		{
			ylimmin = transfer_desired_dB[i];
		}
	}

	for (i = 0; i < magnitudeIr_y.size(); i++)
	{
		if (magnitudeIr_y[i] > ylimmax)
		{
			ylimmax = magnitudeIr_y[i];
		}
		if (magnitudeIr_y[i] < ylimmin)
		{
			ylimmin = magnitudeIr_y[i];
		}
	}

	qcp_display_ir_fd->graph(0)->setData(transfer_desired_f, transfer_desired_dB, true);
	qcp_display_ir_fd->graph(1)->setData(magnitudeIr_x, magnitudeIr_y, true);

	qcp_display_ir_fd->xAxis->setRange(xlimmin, xlimmax);
	qcp_display_ir_fd->yAxis->setRange(ylimmin, ylimmax);

	if (plotFdLinearx)
	{
		qcp_display_ir_fd->xAxis->setScaleType(QCPAxis::stLinear);
		QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		qcp_display_ir_fd->xAxis->setTicker(linTicker);
		qcp_display_ir_fd->xAxis->setNumberFormat("f"); // e = exponential, b = beautiful decimal powers
		qcp_display_ir_fd->xAxis->setNumberPrecision(0);
	}
	else
	{
		qcp_display_ir_fd->xAxis->setScaleType(QCPAxis::stLogarithmic);
		QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
		qcp_display_ir_fd->xAxis->setTicker(logTicker);
		qcp_display_ir_fd->xAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
		qcp_display_ir_fd->xAxis->setNumberPrecision(0);
	}
	qcp_display_ir_fd->replot();
}

void
jvxAcousticEqualizer::replot_t_ir()
{
	jvxSize i;
	jvxData xlimmin = equalizer_ir_x[0];
	jvxData xlimmax = equalizer_ir_x[equalizer_ir_x.size() - 1];

	jvxData ylimmin = JVX_DATA_MAX_POS;
	jvxData ylimmax = JVX_DATA_MAX_NEG;

	for (i = 0; i < equalizer_ir_y.size(); i++)
	{
		if (equalizer_ir_y[i] > ylimmax)
		{
			ylimmax = equalizer_ir_y[i];
		}
		if (equalizer_ir_y[i] < ylimmin)
		{
			ylimmin = equalizer_ir_y[i];
		}
	}
	qcp_display_eq_td->graph(0)->setData(equalizer_ir_x, equalizer_ir_y, true);
	qcp_display_eq_td->xAxis->setRange(xlimmin, xlimmax);
	qcp_display_eq_td->yAxis->setRange(ylimmin, ylimmax);
	qcp_display_eq_td->replot();
}

void
jvxAcousticEqualizer::replot_f_tf()
{
	jvxSize i;
	jvxData xlimmin = magnitudeProcessed_x[0];
	jvxData xlimmax = magnitudeProcessed_x[magnitudeProcessed_x.size() - 1];

	jvxData ylimmin = JVX_DATA_MAX_POS;
	jvxData ylimmax = JVX_DATA_MAX_NEG;

	for (i = 0; i < magnitudeProcessed_y.size(); i++)
	{
		if (magnitudeProcessed_y[i] > ylimmax)
		{
			ylimmax = magnitudeProcessed_y[i];
		}
		if (magnitudeProcessed_y[i] < ylimmin)
		{
			ylimmin = magnitudeProcessed_y[i];
		}
		if (magnitudeUnprocessed_y[i] > ylimmax)
		{
			ylimmax = magnitudeUnprocessed_y[i];
		}
		if (magnitudeUnprocessed_y[i] < ylimmin)
		{
			ylimmin = magnitudeUnprocessed_y[i];
		}
	}

	qcp_resulting_mag->graph(0)->setData(magnitudeUnprocessed_x, magnitudeUnprocessed_y, true);
	qcp_resulting_mag->graph(1)->setData(magnitudeProcessed_x, magnitudeProcessed_y, true);
	qcp_resulting_mag->xAxis->setRange(xlimmin, xlimmax);
	qcp_resulting_mag->yAxis->setRange(ylimmin, ylimmax);
	if (plotFdLinearx)
	{
		qcp_resulting_mag->xAxis->setScaleType(QCPAxis::stLinear);
		QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		qcp_resulting_mag->xAxis->setTicker(linTicker);
		qcp_resulting_mag->xAxis->setNumberFormat("f"); // e = exponential, b = beautiful decimal powers
		qcp_resulting_mag->xAxis->setNumberPrecision(0);
	}
	else
	{
		qcp_resulting_mag->xAxis->setScaleType(QCPAxis::stLogarithmic);
		QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
		qcp_resulting_mag->xAxis->setTicker(logTicker);
		qcp_resulting_mag->xAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
		qcp_resulting_mag->xAxis->setNumberPrecision(0);
	}
	qcp_resulting_mag->replot();
}

void 
jvxAcousticEqualizer::trigger_clipboard_ir()
{
	QClipboard* clipboard = QGuiApplication::clipboard();
	if (equalizer_ir_y.size())
	{
		std::string txt = jvx_valueList2String(equalizer_ir_y.data(), JVX_DATAFORMAT_DATA,
			equalizer_ir_y.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
		clipboard->setText(txt.c_str());
	}
}

void
jvxAcousticEqualizer::changed_l_ir_eq()
{
	lIrEqualizer = lineEdit_l_ir_equalization->text().toInt();
	lIrEqualizer = JVX_MIN(8191, lIrEqualizer);
	
	update_eq_ir(true);
	update_window();

}

void
jvxAcousticEqualizer::activated_fft_ir(int idx)
{
	update_eq_ir(false);
	update_window();
}

void 
jvxAcousticEqualizer::activated_fft_show(int idx)
{
	update_eq_ir();
	update_window();
}

void
jvxAcousticEqualizer::update_eq_ir(jvxBool recompute)
{
	irDelay = lIrEqualizer / 2;
	jvx_get_nearest_size_fft(&szFftTypeEq, lIrEqualizer, JVX_FFT_ROUND_UP, &szFftEq);
	if (szFftTypeShow < szFftTypeEq)
	{
		szFftTypeShow = szFftTypeEq;
	}

	if (recompute)
	{
		compute_all();
	}
}

void 
jvxAcousticEqualizer::activated_freq_sale(int idx)
{
	if (idx == 0)
	{
		plotFdLinearx = true;
	}
	else
	{
		plotFdLinearx = false;
	}
	replot_f_ir();
	replot_f_tf();
}

void
jvxAcousticEqualizer::changed_gaindB()
{
	gaindB = lineEdit_gaindB->text().toData();
	update_window();
	compute_all();
}

void 
jvxAcousticEqualizer::activated_win(int idx)
{
	winId = idx;
	update_window();
	compute_all();
}
