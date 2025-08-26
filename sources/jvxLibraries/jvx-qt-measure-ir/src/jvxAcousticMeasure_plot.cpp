#include "jvxAcousticMeasure.h"

extern "C"
{
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_fft_tools.h"
#include "jvx_math/jvx_complex.h"
}

// ======================================================================================

void 
jvxAcousticMeasure::replot_diagrams(jvxBool updateDataPrimary, jvxBool updateDataSecondary )
{
	jvxSize i;
	jvxData xmax = JVX_DATA_MAX_NEG;
	jvxData xmin = JVX_DATA_MAX_POS;
	jvxData ymax = JVX_DATA_MAX_NEG;
	jvxData ymin = JVX_DATA_MAX_POS;

	QVector<jvxData>* ploty1 = NULL;
	QVector<jvxData>* ploty2 = NULL;
	switch (modeTd)
	{
	case JVX_PLOT_MODE_TD_IR_LOG:
		ploty1 = &dataPlot1.data.ir;
		ploty2 = &dataPlot2.data.ir;
		break;
	case JVX_PLOT_MODE_TD_IR:
		ploty1 = &dataPlot1.data.ir;
		ploty2 = &dataPlot2.data.ir;
		break;
	case JVX_PLOT_MODE_TD_MEAS:
		ploty1 = &dataPlot1.data.meas;
		ploty2 = &dataPlot2.data.meas;
		break;
	case JVX_PLOT_MODE_TD_TEST:
		ploty1 = &dataPlot1.data.test;
		ploty2 = &dataPlot2.data.test;
		break;
	}

	if (updateDataPrimary)
	{
		// Plot 1
		qcp_timedomain->graph(0)->setData(dataPlot1.data.ind_timedomain, *ploty1, true);
		// Plot 1
		qcp_timedomain->graph(1)->setData(dataPlot2.data.ind_timedomain, *ploty2, true);
	}

	if (td.autox)
	{
		for (i = 0; i < dataPlot1.data.ind_timedomain.size(); i++)
		{
			if (dataPlot1.data.ind_timedomain[(int)i] < xmin)
			{
				xmin = dataPlot1.data.ind_timedomain[(int)i];
			}
			if (dataPlot1.data.ind_timedomain[(int)i] > xmax)
			{
				xmax = dataPlot1.data.ind_timedomain[(int)i];
			}
		}
		for (i = 0; i < dataPlot2.data.ind_timedomain.size(); i++)
		{
			if (dataPlot2.data.ind_timedomain[(int)i] < xmin)
			{
				xmin = dataPlot2.data.ind_timedomain[(int)i];
			}
			if (dataPlot2.data.ind_timedomain[(int)i] > xmax)
			{
				xmax = dataPlot2.data.ind_timedomain[(int)i];
			}
		}
	}
	else
	{
		xmax = td.maxx;
		xmin = td.minx;
	}

	if (td.autoy)
	{
		for (i = 0; i < ploty1->size(); i++)
		{
			if ((*ploty1)[(int)i] < ymin)
			{
				ymin = (*ploty1)[(int)i];
			}
			if ((*ploty1)[(int)i] > ymax)
			{
				ymax = (*ploty1)[(int)i];
			}
		}
		for (i = 0; i < ploty2->size(); i++)
		{
			if ((*ploty2)[(int)i] < ymin)
			{
				ymin = (*ploty2)[(int)i];
			}
			if ((*ploty2)[(int)i] > ymax)
			{
				ymax = (*ploty2)[(int)i];
			}
		}
	}
	else
	{
		ymax = td.maxy;
		ymin = td.miny;
	}

	td.latest.maxy = ymax;
	td.latest.miny = ymax;
	td.latest.maxx = xmax;
	td.latest.minx = xmax;

	qcp_timedomain->xAxis->setRange(xmin, xmax);
	qcp_timedomain->yAxis->setRange(ymin, ymax);

	qcp_timedomain->replot();

	// ================================================================================
	replot_freqdomain(updateDataSecondary, ploty1, ploty2, floor(xmin), ceil(xmax));
}

void
jvxAcousticMeasure::replot_freqdomain(
	jvxBool updateDataSecondary,
	QVector<jvxData>* ploty1,
	QVector<jvxData>* ploty2, 
	jvxSize xmin_td, jvxSize xmax_td)
{
	jvxSize i;
	jvxData xmax = JVX_DATA_MAX_NEG;
	jvxData xmin = JVX_DATA_MAX_POS;
	jvxData ymax = JVX_DATA_MAX_NEG;
	jvxData ymin = JVX_DATA_MAX_POS;
	jvxBool no_minmax = true;
	jvxSize sigLi = xmax_td - xmin_td + 1;
	jvxSize sigLu = sigLi;
	if (sigLu % 2)
	{
		sigLu += 1;
	}
	if (sigLu < 16)
	{
		sigLu = 16;
	}
	jvxFFTSize fftInt;
	jvxFFTGlobal* globFft = NULL;
	jvxFFT* myFft = NULL;
	jvxData* inFld = NULL;
	jvxData* inFlds = NULL;
	jvxDataCplx* outFld = NULL;
	jvxSize N = 0;
	jvxSize N2P1 = 0;
	jvxSize N2 = 0;
	jvxData phase_unwrap_offset = 0;
	jvxData phase_unwrap_previous = 0;

	if (updateDataSecondary)
	{
		jvxDspBaseErrorType res = jvx_get_nearest_size_fft(&fftInt, sigLu, JVX_FFT_ROUND_UP, NULL);
		assert(res == JVX_DSP_NO_ERROR);

		label_est_delay_plot1->setText("--");
		label_est_delay_plot2->setText("--");

		jvx_create_fft_ifft_global(&globFft, fftInt, nullptr);
		jvx_create_fft_real_2_complex(&myFft, globFft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&inFld, &outFld, &N, JVX_FFT_IFFT_EFFICIENT, NULL, NULL, sigLu);
		N2 = N / 2;
		N2P1 = N2 + 1;

		if (ploty1->size())
		{
			dataPlot1.data.show_freqdomain_valid = true;
			if (dataPlot1.data.show_freqdomain_x.size() != N2P1)
			{
				dataPlot1.data.show_freqdomain_x.resize((int)N2P1);
				dataPlot1.data.show_freqdomain_y.resize((int)N2P1);
			}

			jvxSize mlmax = 0;
			if ((*ploty1).size())
			{
				mlmax = JVX_MIN(xmax_td, (*ploty1).size() - 1);
			}
			memset(inFld, 0, sizeof(jvxData) * sigLu);
			for (i = xmin_td; i <= mlmax; i++)
			{
				inFld[i - xmin_td] = (*ploty1)[(int)(i)];
			}

			// ===========================================================================================

			jvx_execute_fft(myFft);

			// ===========================================================================================

			switch (modeSec)
			{
			case JVX_PLOT_MODE_SEC_MAG:

				for (i = 0; i < N2P1; i++)
				{
					dataPlot1.data.show_freqdomain_x[(int)i] = dataPlot1.oneChan.rate / (jvxData)N * (jvxData)i;
					dataPlot1.data.show_freqdomain_y[(int)i] = outFld[i].re * outFld[i].re + outFld[i].im * outFld[i].im;
					dataPlot1.data.show_freqdomain_y[(int)i] = 10 * log10(dataPlot1.data.show_freqdomain_y[(int)i] + JVX_LOG10_EPS);
				}
				break;
			case  JVX_PLOT_MODE_SEC_GROUP_DELAY:
			case  JVX_PLOT_MODE_SEC_HIST_DELAY:
			case JVX_PLOT_MODE_SEC_PHASE:
				for (i = 0; i < N2P1; i++)
				{
					jvxData absOut = 0;
					jvxData angleOut = 0;
					dataPlot1.data.show_freqdomain_x[(int)i] = dataPlot1.oneChan.rate / (jvxData)N * (jvxData)i;
					jvx_complex_cart_2_polar(outFld[i].re, outFld[i].im, &absOut, &angleOut);
					dataPlot1.data.show_freqdomain_y[(int)i] = angleOut;
				}
				phase_unwrap_previous = dataPlot1.data.show_freqdomain_y[(int)0];
				phase_unwrap_offset = 0;
				for (i = 1; i < N2P1; i++)
				{
					jvxData diff = dataPlot1.data.show_freqdomain_y[(int)i] - phase_unwrap_previous;
					phase_unwrap_previous = dataPlot1.data.show_freqdomain_y[(int)i];
					if (diff > M_PI)
					{
						phase_unwrap_offset -= 2.0 * M_PI;
					}
					if (diff < -M_PI)
					{
						phase_unwrap_offset += 2.0 * M_PI;
					}
					dataPlot1.data.show_freqdomain_y[(int)i] += phase_unwrap_offset;
				}
				// According to https://www.ljmu.ac.uk/~/media/files/ljmu/about-us/faculties-and-schools/fet/geri/onedimensionalphaseunwrapping_finalpdf.pdf
				// Matlab Code: xu = xw; for i = 2:length(xw)    difference = xw(i) - xw(i - 1);    if difference > pi        xu(i:end) = xu(i:end) - 2 * pi;    elseif difference < -pi        xu(i:end) = xu(i:end) + 2 * pi;         endend
				//

				if (
					(modeSec == JVX_PLOT_MODE_SEC_GROUP_DELAY) ||
					(modeSec == JVX_PLOT_MODE_SEC_HIST_DELAY))
				{
					// Group delay computed as n_delay = (delta_phi * FFTL) / (2 M_PI) as a float value
					phase_unwrap_previous = dataPlot1.data.show_freqdomain_y[(int)0];
					for (i = 1; i < N2P1; i++)
					{
						jvxData deltaPhi = -(dataPlot1.data.show_freqdomain_y[(int)i] - phase_unwrap_previous);
						phase_unwrap_previous = dataPlot1.data.show_freqdomain_y[(int)i];
						jvxData delay = (deltaPhi * (jvxData)N) / (2.0 * M_PI);
						dataPlot1.data.show_freqdomain_y[(int)i] = delay;
					}

					if (modeSec == JVX_PLOT_MODE_SEC_HIST_DELAY)
					{
						// in case we compute the histogram for the delay, re-arrange buffers and count delay values
						QVector<jvxData> tmpBuf = dataPlot1.data.show_freqdomain_y;
						for (i = 0; i < N2P1; i++)
						{
							// Consider offset from the selection in time domain diagram
							dataPlot1.data.show_freqdomain_x[(int)i] = i + xmin_td;
							dataPlot1.data.show_freqdomain_y[(int)i] = 0;
						}
						for (i = 0; i < N2P1; i++)
						{
							jvxSize idxRnd = JVX_DATA2SIZE(tmpBuf[(int)i]);
							if (idxRnd < dataPlot1.data.show_freqdomain_y.size())
							{
								dataPlot1.data.show_freqdomain_y[(int)idxRnd] += 1.0;
							}
						}

						// Re-normalize the hist buffer
						jvxData accu = 0;
						jvxSize maxIdx = 0;
						jvxSize maxVal = 0;
						for (i = 0; i < N2P1; i++)
						{
							if (dataPlot1.data.show_freqdomain_y[(int)i] >= maxVal)
							{
								maxIdx = i;
								maxVal = dataPlot1.data.show_freqdomain_y[(int)i];
							}
							accu += dataPlot1.data.show_freqdomain_y[(int)i];
						}
						for (i = 0; i < N2P1; i++)
						{
							dataPlot1.data.show_freqdomain_y[(int)i] /= accu;
						}
						maxIdx += xmin_td;
						label_est_delay_plot1->setText(jvx_size2String(maxIdx).c_str());
					}
				}
				break;
			}
			qcp_secondary->graph(0)->setData(dataPlot1.data.show_freqdomain_x, dataPlot1.data.show_freqdomain_y, true);
		}
		else
		{
			dataPlot1.data.show_freqdomain_valid = false;
		}

		// ===========================================================================================

		if (ploty2->size())
		{
			dataPlot2.data.show_freqdomain_valid = true;
			if (dataPlot2.data.show_freqdomain_x.size() != N2P1)
			{
				dataPlot2.data.show_freqdomain_x.resize((int)N2P1);
				dataPlot2.data.show_freqdomain_y.resize((int)N2P1);
			}

			jvxSize mlmax = 0;
			if ((*ploty2).size())
			{
				mlmax = JVX_MIN(xmax_td, (*ploty2).size() - 1);
			}
			memset(inFld, 0, sizeof(jvxData) * sigLu);
			for (i = xmin_td; i <= mlmax; i++)
			{
				inFld[i - xmin_td] = (*ploty2)[(int)(i)];
			}
			jvx_execute_fft(myFft);

			switch (modeSec)
			{
			case JVX_PLOT_MODE_SEC_MAG:
				for (i = 0; i < N2P1; i++)
				{
					dataPlot2.data.show_freqdomain_x[(int)i] = dataPlot2.oneChan.rate / (jvxData)N * (jvxData)i;
					dataPlot2.data.show_freqdomain_y[(int)i] = outFld[i].re * outFld[i].re + outFld[i].im * outFld[i].im;
					dataPlot2.data.show_freqdomain_y[(int)i] = 10 * log10(dataPlot2.data.show_freqdomain_y[(int)i] + JVX_LOG10_EPS);
				}
				break;

			case  JVX_PLOT_MODE_SEC_GROUP_DELAY:
			case  JVX_PLOT_MODE_SEC_HIST_DELAY:
			case JVX_PLOT_MODE_SEC_PHASE:
				for (i = 0; i < N2P1; i++)
				{
					jvxData absOut = 0;
					jvxData angleOut = 0;
					dataPlot2.data.show_freqdomain_x[(int)i] = dataPlot2.oneChan.rate / (jvxData)N * (jvxData)i;
					jvx_complex_cart_2_polar(outFld[i].re, outFld[i].im, &absOut, &angleOut);
					dataPlot2.data.show_freqdomain_y[(int)i] = angleOut;
				}
				phase_unwrap_previous = dataPlot2.data.show_freqdomain_y[(int)0];
				phase_unwrap_offset = 0;
				for (i = 1; i < N2P1; i++)
				{
					jvxData diff = dataPlot2.data.show_freqdomain_y[(int)i] - phase_unwrap_previous;
					phase_unwrap_previous = dataPlot2.data.show_freqdomain_y[(int)i];
					if (diff > M_PI)
					{
						phase_unwrap_offset -= 2.0 * M_PI;
					}
					if (diff < -M_PI)
					{
						phase_unwrap_offset += 2.0 * M_PI;
					}
					dataPlot2.data.show_freqdomain_y[(int)i] += phase_unwrap_offset;
				}
				// According to https://www.ljmu.ac.uk/~/media/files/ljmu/about-us/faculties-and-schools/fet/geri/onedimensionalphaseunwrapping_finalpdf.pdf
				// Matlab Code: xu = xw; for i = 2:length(xw)    difference = xw(i) - xw(i - 1);    if difference > pi        xu(i:end) = xu(i:end) - 2 * pi;    elseif difference < -pi        xu(i:end) = xu(i:end) + 2 * pi;         endend
				//

				if (
					(modeSec == JVX_PLOT_MODE_SEC_GROUP_DELAY) ||
					(modeSec == JVX_PLOT_MODE_SEC_HIST_DELAY))
				{
					// Group delay computed as n_delay = (delta_phi * FFTL) / (2 M_PI) as a float value
					phase_unwrap_previous = dataPlot2.data.show_freqdomain_y[(int)0];
					for (i = 1; i < N2P1; i++)
					{
						jvxData deltaPhi = -(dataPlot2.data.show_freqdomain_y[(int)i] - phase_unwrap_previous);
						phase_unwrap_previous = dataPlot2.data.show_freqdomain_y[(int)i];
						jvxData delay = (deltaPhi * (jvxData)N) / (2.0 * M_PI);
						dataPlot2.data.show_freqdomain_y[(int)i] = delay;
					}

					if (modeSec == JVX_PLOT_MODE_SEC_HIST_DELAY)
					{
						// in case we compute the histogram for the delay, re-arrange buffers and count delay values
						QVector<jvxData> tmpBuf = dataPlot2.data.show_freqdomain_y;
						for (i = 0; i < N2P1; i++)
						{
							// Consider offset from the selection in time domain diagram
							dataPlot2.data.show_freqdomain_x[(int)i] = i + xmin_td;
							dataPlot2.data.show_freqdomain_y[(int)i] = 0;
						}
						for (i = 0; i < N2P1; i++)
						{
							jvxSize idxRnd = JVX_DATA2SIZE(tmpBuf[(int)i]);
							if (idxRnd < dataPlot2.data.show_freqdomain_y.size())
							{
								dataPlot2.data.show_freqdomain_y[(int)idxRnd] += 1.0;
							}
						}

						// Re-normalize the hist buffer
						jvxData accu = 0;
						jvxSize maxIdx = 0;
						jvxSize maxVal = 0;
						for (i = 0; i < N2P1; i++)
						{
							if (dataPlot2.data.show_freqdomain_y[(int)i] >= maxVal)
							{
								maxIdx = i;
								maxVal = dataPlot2.data.show_freqdomain_y[(int)i];
							}
							accu += dataPlot2.data.show_freqdomain_y[(int)i];
						}
						for (i = 0; i < N2P1; i++)
						{
							dataPlot2.data.show_freqdomain_y[(int)i] /= accu;
						}
						maxIdx += xmin_td;
						label_est_delay_plot2->setText(jvx_size2String(maxIdx).c_str());
					}
				}
				break;
			}

			qcp_secondary->graph(1)->setData(dataPlot2.data.show_freqdomain_x, dataPlot2.data.show_freqdomain_y, true);			
		}
		else
		{
			dataPlot2.data.show_freqdomain_valid = false;
		}

		jvx_destroy_fft(myFft);
		jvx_destroy_fft_ifft_global(globFft);
		inFld = NULL;
		outFld = NULL;
	}

	if (dataPlot1.data.show_freqdomain_valid)
	{
		for (i = 0; i < dataPlot1.data.show_freqdomain_x.size(); i++)
		{
			if (dataPlot1.data.show_freqdomain_x[(int)i] < xmin)
			{
				xmin = dataPlot1.data.show_freqdomain_x[(int)i];
			}
			if (dataPlot1.data.show_freqdomain_x[(int)i] > xmax)
			{
				xmax = dataPlot1.data.show_freqdomain_x[(int)i];
			}
		}

		for (i = 0; i < dataPlot1.data.show_freqdomain_y.size(); i++)
		{
			if (dataPlot1.data.show_freqdomain_y[(int)i] < ymin)
			{
				ymin = dataPlot1.data.show_freqdomain_y[(int)i];
			}
			if (dataPlot1.data.show_freqdomain_y[(int)i] > ymax)
			{
				ymax = dataPlot1.data.show_freqdomain_y[(int)i];
			}
		}
		no_minmax = false;
	}

	if (dataPlot2.data.show_freqdomain_valid)
	{
		for (i = 0; i < dataPlot2.data.show_freqdomain_x.size(); i++)
		{
			if (dataPlot2.data.show_freqdomain_x[(int)i] < xmin)
			{
				xmin = dataPlot2.data.show_freqdomain_x[(int)i];
			}
			if (dataPlot2.data.show_freqdomain_x[(int)i] > xmax)
			{
				xmax = dataPlot2.data.show_freqdomain_x[(int)i];
			}
		}

		for (i = 0; i < dataPlot2.data.show_freqdomain_y.size(); i++)
		{
			if (dataPlot2.data.show_freqdomain_y[(int)i] < ymin)
			{
				ymin = dataPlot2.data.show_freqdomain_y[(int)i];
			}
			if (dataPlot2.data.show_freqdomain_y[(int)i] > ymax)
			{
				ymax = dataPlot2.data.show_freqdomain_y[(int)i];
			}
		}
		no_minmax = false;
	}

	if (!sec.autox || no_minmax)
	{
		xmax = sec.maxx;
		xmin = sec.minx;
	}

	if (!sec.autoy || no_minmax)
	{
		ymax = sec.maxy;
		ymin = sec.miny;
	}

	sec.latest.maxy = ymax;
	sec.latest.miny = ymax;
	sec.latest.maxx = xmax;
	sec.latest.minx = xmax;

	qcp_secondary->xAxis->setRange(xmin, xmax);
	qcp_secondary->yAxis->setRange(ymin, ymax);

	if (sec.plotLinearx)
	{
		qcp_secondary->xAxis->setScaleType(QCPAxis::stLinear);
		QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		qcp_secondary->xAxis->setTicker(linTicker);
		qcp_secondary->xAxis->setNumberFormat("f"); // e = exponential, b = beautiful decimal powers
		qcp_secondary->xAxis->setNumberPrecision(0);
	}
	else
	{
		qcp_secondary->xAxis->setScaleType(QCPAxis::stLogarithmic);
		QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
		qcp_secondary->xAxis->setTicker(logTicker);
		qcp_secondary->xAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
		qcp_secondary->xAxis->setNumberPrecision(0);
	}
	qcp_secondary->replot();
}
