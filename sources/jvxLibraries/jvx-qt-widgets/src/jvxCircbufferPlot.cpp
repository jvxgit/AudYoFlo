// ==============================================================================================
#include "jvxCircbufferPlot.h"
#include <cassert>

jvxData resamplerFactors[jvxCircbufferPlot::JVX_PLOT_RESAMPLER_LIMIT] =
{
	1.0,
	2.0,
	5.0,
	10.0,
	20.0,
	50.0,
	100.0,
	200.0,
	500.0,
	1000.0,
	2000.0,
	5000.0,
	10000.0,
	20000.0,
	50000.0
};

jvxCircbufferPlot::jvxCircbufferPlot()
{
	timer_data = NULL;
	timeout_circ_data_msec = 50;
	token = "/undefined";
	props = NULL;
	circplot = NULL;
	bSize = 0;
	nChans = 0;

	params.label_yaxis = "y(n)";
	params.label_xaxis = "x(n)";	
	params.show_legend = true;
	params.init_num_positions_plot = 1024;
	params.autox = false;
	params.autoy = false;
	params.ymin = -10.0;
	params.ymax = 10.0;
	params.xmin = -10.0;
	params.xmax = 10.0;
	params.resamplerStrat = JVX_PLOT_RESAMPLER_1X;
	params.scroll = false;
	params.min_width = 0.01;

	downsample = 1.0;
}

jvxCircbufferPlot::~jvxCircbufferPlot()
{
}
 
jvxErrorType
jvxCircbufferPlot::setPlotRef(
	QCustomPlot* plotter, 
	jvxSize nChansArg,
	const QVector<QColor>& cols)
{
	jvxSize i,j;
	onePlotInstance newItem;
	if (circplot == NULL)
	{
		circplot = plotter;
		nChans = nChansArg;
		numSamplesPlot = params.init_num_positions_plot;

#ifdef JVX_PERMIT_QCP_OPENGL
		std::cout << "Trying to activate openGL for QCP." << std::endl;
		circplot->setOpenGl(true);
		if (!circplot->openGl())
		{
			std::cout << "Failed to activate openGL rendering!" << std::endl;
		}
#else
		std::cout << "Running QCP without openGL." << std::endl;
#endif

		params.xmax = numSamplesPlot;
		params.xmin = 0;
		params.ymax = 10;
		params.ymin = -10;
		for (i = 0; i < nChans; i++)
		{
			circplot->addGraph();
			newItem.active = true;
			newItem.idPlot = i;
			newItem.show_x.resize((int)numSamplesPlot);
			newItem.show_y.resize((int)numSamplesPlot);
			for (j = 0; j < numSamplesPlot; j++)
			{
				newItem.show_x[(int)j] = j;
				newItem.show_y[(int)j] = i;
			}

			lanes.plotInstances.push_back(newItem);
		}

		for (i = 0; i < nChans; i++)
		{
			circplot->graph((int)i)->setLineStyle(QCPGraph::lsLine);
			QColor col = Qt::black;
			if (cols.size())
			{
				col = cols[i % cols.size()];
			}
			circplot->graph((int)i)->setPen(QPen(col));
		}

		circplot->yAxis->setLabel(params.label_yaxis.c_str());
		circplot->xAxis->setLabel(params.label_xaxis.c_str());
		circplot->legend->setVisible(params.show_legend);

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxCircbufferPlot::unsetPlotRef()
{
	jvxSize i;
	if (circplot)
	{
		for (i = 0; i < lanes.plotInstances.size(); i++)
		{
			circplot->removeGraph((int)lanes.plotInstances[i].idPlot);
			lanes.plotInstances[(int)i].show_x.clear();
			lanes.plotInstances[(int)i].show_y.clear();
			lanes.plotInstances[(int)i].newSamples.clear();
		}
		lanes.plotInstances.clear();

		nChans = 0;
		numSamplesPlot = 0;

		circplot = NULL;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ===================================================================================

jvxErrorType
jvxCircbufferPlot::startCircBuffer(
	const std::string& token_prop, 
	IjvxAccessProperties* accProps,
	jvxSize bSizeArg)
{
	jvxSize i;
	if (props == NULL)
	{
		token = token_prop;		
		bSize = bSizeArg;

		jvxCallManagerProperties callG;
		jvxErrorType resL = JVX_NO_ERROR;
		jvxExternalBuffer* theBuf = NULL;
		jvxBool* theValid = NULL;

		props = accProps;

		for (i = 0; i < lanes.plotInstances.size(); i++)
		{
			// One more value: if we resample by 2 we might see one additional sample 
			lanes.plotInstances[i].newSamples.resize((int)bSize+1);
			jvx_resampler_plotter_init(&lanes.plotInstances[i].theResampler, downsample);

		}

		resL = dataConnect.start(
			static_cast<HjvxCircbufferRead_report<jvxData>*>(this),
			bSize, nChans);
		if (resL == JVX_NO_ERROR)
		{
			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(
				dataConnect.theCircBufferFld, &dataConnect.theCircBufferValid, dataConnect.form);
			jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(token.c_str());

			resL = props->install_referene_property(callG, ptrRaw, ident);
			JVX_ASSERT_PROPERTY_ACCESS_RETURN(resL, token);
		}		

		timer_data = new QTimer();
		connect(timer_data, SIGNAL(timeout()), this, SLOT(timer_circ_data()));
		timer_data->start((int)timeout_circ_data_msec);

		currentPosition = 0;

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

// =============================================================================

jvxErrorType
jvxCircbufferPlot::stopCircBuffer()
{
	int i;
	jvxCallManagerProperties callG;

	if(props)
	{
		jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(
			dataConnect.theCircBufferFld,
			&dataConnect.theCircBufferValid, 
			dataConnect.theCircBufferFld->formFld);
		jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(token.c_str());

		props->uninstall_referene_property(callG, ptrRaw, ident);
		timer_data->stop();
		delete timer_data;
		timer_data = NULL;

		dataConnect.stop();

		token = "/undefined";
		bSize = 0;
		props = NULL;
		for (i = 0; i < lanes.plotInstances.size(); i++)
		{
			lanes.plotInstances[i].newSamples.clear();
			jvx_resampler_plotter_terminate(lanes.plotInstances[i].theResampler);
			lanes.plotInstances[i].theResampler = NULL;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void
jvxCircbufferPlot::timer_circ_data()
{
	jvxSize cnt = 0;
	jvxSize i;
	jvxSize j;
	jvxSize offr = 0;
	jvxSize offw = 0;
	jvxSize ll1 = 0;
	jvxSize ll2 = 0;

	jvxData minyG = JVX_DATA_MAX_POS;
	jvxData maxyG = JVX_DATA_MAX_NEG;
	jvxBool atleastOne = false;
	jvxBool scrollit = params.scroll;

	// Pull in new data - quickly
	dataConnect.trigger_new_data();

	jvxSize fH = fHeight;
	jvxSize posiStart = currentPosition;

	jvxSize offset = 0;
	jvxSize numSamples = fHeight;
	jvxSize numSamplesMove = 0;
	numSamples = JVX_MIN(numSamples, numSamplesPlot);
	offset += fHeight - numSamples;

	if (scrollit)
	{
		numSamplesMove = numSamplesPlot - numSamples;
	}

	for (i = 0; i < lanes.plotInstances.size(); i++)
	{
		jvxData* ptrIn = lanes.plotInstances[i].newSamples.data();
		qreal* ptrOut = lanes.plotInstances[i].show_y.data();

		if (scrollit)
		{
			memmove(ptrOut, ptrOut + numSamples, sizeof(qreal) * numSamplesMove);
			ptrOut += numSamplesMove;
			ptrIn += offset;
			for (j = 0; j < numSamples; j++)
			{
				*ptrOut = *ptrIn;
				ptrIn++;
				ptrOut++;
			}
		}
		else
		{
			ptrOut += posiStart;
			ll1 = numSamplesPlot - posiStart;
			ll1 = JVX_MIN(ll1, numSamples);
			ll2 = numSamples - ll1;
			for (j = 0; j < ll1; j++)
			{
				*ptrOut = *ptrIn;
				ptrIn++;
				ptrOut++;
			}
			ptrOut = lanes.plotInstances[i].show_y.data();
			for (j = 0; j < ll2; j++)
			{
				*ptrOut = *ptrIn;
				ptrIn++;
				ptrOut++;
			}
		}


		jvxData miny = JVX_DATA_MAX_POS;
		jvxData maxy = JVX_DATA_MAX_NEG;
		ptrOut = lanes.plotInstances[i].show_y.data();
		for (j = 0; j < numSamplesPlot; j++)
		{
			qreal tmp = *ptrOut;
			miny = JVX_MIN(tmp, miny);
			maxy = JVX_MAX(tmp, maxy);
			ptrOut++;
		}
	
		if (lanes.plotInstances[i].active)
		{
			atleastOne = true;
			circplot->graph((int)lanes.plotInstances[i].idPlot)->setData(lanes.plotInstances[i].show_x, lanes.plotInstances[i].show_y);
			minyG = JVX_MIN(minyG, miny);
			maxyG = JVX_MAX(maxyG, maxy);
		}
	}
	if (scrollit)
	{
		currentPosition = 0;
	}
	else
	{ 
		currentPosition = (currentPosition + numSamples) % numSamplesPlot;
	}
	fHeight = 0;

	circplot->xAxis->setRange(params.xmin, params.xmax);
	if (params.autoy)
	{
		if (atleastOne == false)
		{
			minyG = -1;
			maxyG = 1;
		}
	}
	else
	{
		minyG = params.ymin;
		maxyG = params.ymax;
	}

	if ((maxyG - minyG) < params.min_width)
	{
		jvxData mean = (maxyG + minyG) / 2.0;
		minyG = mean - params.min_width / 2.0;
		maxyG = mean + params.min_width / 2.0;
	}
	circplot->yAxis->setRange(minyG, maxyG);
	circplot->replot();
}

void 
jvxCircbufferPlot::report_data(jvxData** ptr, jvxSize numChannels, jvxSize numValues, jvxSize uId)
{
	jvxSize i;
	jvxSize j;
	static jvxData cmp = 0;
	jvxSize num = JVX_SIZE_UNSELECTED;
	// std::cout << "Num = " << numValues << std::endl;
	resamplerTp tpRes = params.resamplerStrat;
	downsample = resamplerFactors[tpRes];
	for (i = 0; i < lanes.plotInstances.size(); i++)
	{		
		jvxData* ptrOut = lanes.plotInstances[i].newSamples.data();
		jvxSize numLocal = 0;
		
		jvxData* ptrIn = NULL; 
		if (i < numChannels)
		{
			ptrIn = ptr[i];
		}
		
		for (j = 0; j < numValues; j++)
		{
			jvxData tmp = 0.0;
			if (ptrIn)
			{
				tmp = *ptrIn;
			}

			if (tpRes == JVX_PLOT_RESAMPLER_1X)
			{
				*ptrOut = tmp;
				ptrOut++;
				numLocal++;
			}
			else
			{
				jvxData out_min = 0;
				jvxData out_max = 0;
				jvxCBool outputProduced = 0;
				jvxCBool reconf = false;
				jvx_resampler_plotter_process_reconfig(
					(jvxData)tmp, &out_min, &out_max,
					&outputProduced,
					lanes.plotInstances[i].theResampler, downsample,
					&reconf);
				if (outputProduced)
				{
					*ptrOut = out_min;
					ptrOut++;
					*ptrOut = out_max;
					ptrOut++;
					numLocal++;
				}
			}
			ptrIn++;
		}
		if (JVX_CHECK_SIZE_UNSELECTED(num))
		{
			num = numLocal;
		}
		else
		{
			assert(num == numLocal);
		}
	}
	fHeight = num;
}

// ==================================================================================
// ==================================================================================

JVX_CLASS_SIMPLE_SET_DEFINE_START(jvxCircbufferPlot, std::string, params, label_yaxis)

	if (circplot)
	{
		circplot->yAxis->setLabel(in.c_str());
	}

JVX_CLASS_SIMPLE_SET_DEFINE_STOP

// ==================================================================================

JVX_CLASS_SIMPLE_SET_DEFINE_START(jvxCircbufferPlot, std::string, params, label_xaxis)
	if (circplot)
	{
		circplot->xAxis->setLabel(in.c_str());
	}
JVX_CLASS_SIMPLE_SET_DEFINE_STOP

// ==================================================================================

JVX_CLASS_SIMPLE_SET_DEFINE_START(jvxCircbufferPlot, jvxData, params, xmax)
if(params.xmax < params.xmin)
{ 
	params.xmin = params.xmax - 1;
}
JVX_CLASS_SIMPLE_SET_DEFINE_STOP

// ==================================================================================

JVX_CLASS_SIMPLE_SET_DEFINE_START(jvxCircbufferPlot, jvxData, params, xmin)
if (params.xmin > params.xmax)
{
	params.xmax = params.xmin + 1;
}
JVX_CLASS_SIMPLE_SET_DEFINE_STOP

// ==================================================================================

JVX_CLASS_SIMPLE_SET_DEFINE_START(jvxCircbufferPlot, jvxData, params, ymax)
if (params.ymax < params.ymin)
{
	params.ymin = params.ymax - params.min_width;
}
JVX_CLASS_SIMPLE_SET_DEFINE_STOP

// ==================================================================================

JVX_CLASS_SIMPLE_SET_DEFINE_START(jvxCircbufferPlot, jvxData, params, ymin)
if (params.ymin > params.ymax)
{
	params.ymax = params.ymin + params.min_width;
}
JVX_CLASS_SIMPLE_SET_DEFINE_STOP

// ==================================================================================

JVX_CLASS_SIMPLE_SET_DEFINE_START(jvxCircbufferPlot, jvxBool, params, show_legend)
	if (circplot)
	{
		if (in)
		{
			circplot->legend->setVisible(true);
		}
		else
		{
			circplot->legend->setVisible(false);
		}
	}
JVX_CLASS_SIMPLE_SET_DEFINE_STOP

JVX_CLASS_SIMPLE_SET_GET_DEFINE_SINGLE_START(jvxCircbufferPlot, jvxBool, lanes, plotInstances, active)
	if (circplot)
	{
		if (in)
		{
			circplot->graph((int)idx)->setVisible(true);
		}
		else
		{ 
			circplot->graph((int)idx)->setVisible(false);
		}
		circplot->replot();
	}
JVX_CLASS_SIMPLE_SET_DEFINE_SINGLE_STOP


// ==================================================================================