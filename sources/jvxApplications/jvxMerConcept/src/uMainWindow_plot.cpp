#include "uMainWindow.h"

#define EPS 1e-5
void
uMainWindow::updatePlots()
{
	jvxSize i;
	jvxData minY = JVX_DATA_MAX_POS;
	jvxData maxY = JVX_DATA_MAX_NEG;

	for(i = 0; i < JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT; i++)
	{
		if(plot.y_left[i] <= minY)
		{
			minY = plot.y_left[i];
		}
		if(plot.y_left[i] >= maxY)
		{
			maxY = plot.y_left[i];
		}
		if(plot.y_right[i] <= minY)
		{
			minY = plot.y_right[i];
		}
		if(plot.y_right[i] >= maxY)
		{
			maxY = plot.y_right[i];
		}
	}
	plot.theCurve_left->setSamples(this->plot.x_left, plot.y_left, JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT);
	plot.theCurve_right->setSamples(this->plot.x_right, plot.y_right, JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT);
			
	qwtPlot_left->setAxisScale(QwtPlot::xBottom, plot.x_left[0], plot.x_left[JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT-1]);
	qwtPlot_right->setAxisScale(QwtPlot::xBottom, plot.x_right[0], plot.x_right[JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT-1]);
			
	if(maxY - minY < EPS)
	{
		jvxData tmp = maxY + minY;
		maxY = (tmp + EPS)*0.5;
		minY = (tmp - EPS)*0.5;
	}
	qwtPlot_left->setAxisScale(QwtPlot::yLeft, minY, maxY);
	qwtPlot_right->setAxisScale(QwtPlot::yLeft, minY, maxY);

	qwtPlot_left->replot();
	qwtPlot_right->replot();
}

void
uMainWindow::updatePlotData()
{
	jvxData* bufsY[JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];
	jvxData* bufsX[JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];

	bufsY[0] = plot.y_left;
	bufsY[1] = plot.y_right;
	bufsX[0] = plot.x_left;
	bufsX[1] = plot.x_right;

	this->readwrite.theBuffers.fill_view_buffer(bufsY, bufsX, JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT);
}

void
uMainWindow::updatePlotData_online()
{
	jvxData* bufsY[JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];
	jvxData* bufsX[JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];

	bufsY[0] = plot.y_left;
	bufsY[1] = plot.y_right;
	bufsX[0] = plot.x_left;
	bufsX[1] = plot.x_right;

	this->readwrite.theBuffers.fill_view_buffer_online(bufsY, bufsX, JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT, JVX_STANDALONE_HOST_SAMPLERATE * JVX_STANDALONE_HOST_SECONDS_SHOW_ONLINE);
}