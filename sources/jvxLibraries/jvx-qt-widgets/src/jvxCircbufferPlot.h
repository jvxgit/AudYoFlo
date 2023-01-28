#ifndef __CIRCBUFFERPLOT_H__
#define __CIRCBUFFERPLOT_H__

#include "jvx.h"
#include "jvx-helpers-cpp.h"
#include <QtCore/QTimer>
#include "qcustomplot.h"

#include "jvx_resampler/jvx_resampler_plotter.h"

class jvxCircbufferPlot: public QObject, public HjvxCircbufferRead_report<jvxData>
{
public:
	typedef enum
	{
		JVX_PLOT_RESAMPLER_1X,
		JVX_PLOT_RESAMPLER_2X,
		JVX_PLOT_RESAMPLER_5X,
		JVX_PLOT_RESAMPLER_10X,
		JVX_PLOT_RESAMPLER_20X,
		JVX_PLOT_RESAMPLER_50X,
		JVX_PLOT_RESAMPLER_100X,
		JVX_PLOT_RESAMPLER_200X,
		JVX_PLOT_RESAMPLER_500X,
		JVX_PLOT_RESAMPLER_1000X,
		JVX_PLOT_RESAMPLER_2000X,
		JVX_PLOT_RESAMPLER_5000X,
		JVX_PLOT_RESAMPLER_10000X,
		JVX_PLOT_RESAMPLER_20000X,
		JVX_PLOT_RESAMPLER_50000X,
		JVX_PLOT_RESAMPLER_LIMIT
	} resamplerTp;

	class onePlotInstance
	{
	public:
		jvxSize idPlot;
		QVector<jvxData> newSamples;
		QVector<qreal> show_x;
		QVector<qreal> show_y;
		jvxSize active;
		resampler_plotter_private* theResampler;
		onePlotInstance()
		{
			theResampler = NULL;
			active = true;
		};
	};

	QTimer* timer_data;
	HjvxCircbufferRead<jvxData> dataConnect;
	jvxSize timeout_circ_data_msec;
	
	std::string token;
	IjvxAccessProperties* props;

	jvxSize bSize;
	jvxSize nChans;
	jvxSize numSamplesPlot;

	QCustomPlot* circplot;
	struct
	{
		std::string label_yaxis;
		std::string label_xaxis;
		jvxBool show_legend;
		jvxSize init_num_positions_plot;
		jvxBool autox;
		jvxData xmax;
		jvxData xmin;
		jvxBool autoy;
		jvxData ymax;
		jvxData ymin;
		jvxBool scroll;
		resamplerTp resamplerStrat;
		jvxData min_width;
	} params;

	struct
	{
		std::vector<onePlotInstance> plotInstances;
	} lanes;

	jvxData downsample;
	jvxSize fHeight;
	jvxSize currentPosition;

	Q_OBJECT

public:
	jvxCircbufferPlot();
	~jvxCircbufferPlot();

	jvxErrorType setPlotRef(QCustomPlot* plotter,
		jvxSize nChansArg,
		const QVector<QColor>& cols);
	jvxErrorType unsetPlotRef();

	jvxErrorType startCircBuffer(
		const std::string& token_prop, 
		IjvxAccessProperties* accProps,
		jvxSize bSize);
	jvxErrorType stopCircBuffer();

	// Report new data in circular buffer from component
	virtual void JVX_CALLINGCONVENTION report_data(jvxData** ptr, jvxSize numChannels, jvxSize numValues, jvxSize uId) override;

	// ===============================================================
	JVX_CLASS_SIMPLE_SET_GET_DECLARE(std::string, params, label_yaxis);
	JVX_CLASS_SIMPLE_SET_GET_DECLARE(std::string, params, label_xaxis);
	JVX_CLASS_SIMPLE_SET_GET_DECLARE(jvxBool, params, show_legend);

	JVX_CLASS_SIMPLE_SET_GET_DECLARE(jvxData, params, xmax);
	JVX_CLASS_SIMPLE_SET_GET_DECLARE(jvxData, params, xmin);
	JVX_CLASS_SIMPLE_SET_GET_DECLARE(jvxData, params, ymax);
	JVX_CLASS_SIMPLE_SET_GET_DECLARE(jvxData, params, ymin);

	JVX_CLASS_SIMPLE_SET_GET_DEFINE(jvxSize, params, init_num_positions_plot);
	JVX_CLASS_SIMPLE_SET_GET_DEFINE(jvxBool, params, autox);
	JVX_CLASS_SIMPLE_SET_GET_DEFINE(jvxBool, params, autoy);
	JVX_CLASS_SIMPLE_SET_GET_DEFINE(resamplerTp, params, resamplerStrat);
	JVX_CLASS_SIMPLE_SET_GET_DEFINE(jvxBool, params, scroll);
	JVX_CLASS_SIMPLE_SET_GET_DEFINE(jvxData, params, min_width);

	JVX_CLASS_SIMPLE_SET_GET_DECLARE_SINGLE(jvxBool, lanes, plotInstances, active);

	// ===============================================================

public slots:
	void timer_circ_data();
};

#endif
