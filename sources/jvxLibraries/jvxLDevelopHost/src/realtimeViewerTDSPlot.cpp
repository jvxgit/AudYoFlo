#include "realtimeViewerPlots.h"
#include "realtimeViewerTDSPlot.h"
#include "realtimeViewer_typedefs.h"
#include "realtimeViewer_helpers.h"
#include "jvx-qt-helpers.h"
#include "uMainWindow_defines.h"

static QColor jvxRealtimePlotColorType_translate[JVX_REALTIME_PLOT_COLOR_NUM] =
{
	Qt::black,
	QColor("brown"),
	Qt::blue,
	QColor("purple"),
	Qt::red,
	QColor("orange"),
	Qt::yellow,
	Qt::green
};

/**
 * Constructor: Set section and group id and also the realtime update callbacks.
 *///==================================================================================================
realtimeViewerTDSPlot::realtimeViewerTDSPlot(realtimeViewerPlots* parent, IjvxHost* hostRef, CjvxRealtimeViewer* rtvRef, IjvxReport* report, jvxSize sectionId,
	jvxSize groupId, jvxComponentIdentification* tpAllI) :
	QWidget(parent), realtimeViewer_base("realtimeViewerOneProperty", hostRef, rtvRef, report), tpAll(tpAllI)
{
	references.sectionId = sectionId;
	references.groupId = groupId;

	// Prepare callback struct, will be passed in init()
	myCallback.callback_section.callback_item = callbackUpdateItem;
	myCallback.callback_section.privData = reinterpret_cast<jvxHandle*>(this);
	myCallback.callback_group = callbackUpdateGroup;

	// No grid yet.
	plot.theGrid = NULL;
	plot.theLegend = NULL;

	// Set reference for low level function calls
	this->setProperty("BASE_REALTIMEVIEWER", qVariantFromValue(reinterpret_cast<void*>(static_cast<realtimeViewer_base*>(this))));
}

/**
 * Destructor: Make sure all plots are removed properly.
 *///=================================================================================================
realtimeViewerTDSPlot::~realtimeViewerTDSPlot()
{
	//this->cleanBeforeDelete(); <- clean function called before from outside
}

/**
 * Init function to create UI elements and to create link to callback structure in real-time update.
 *///=================================================================================================
void 
realtimeViewerTDSPlot::init()
{
	int i;

	// Create all ui elements
	this->setupUi(this);

	// Set reference to callback
	this->references.theRef->_set_update_callback_view_group_in_section(references.sectionId, references.groupId, &myCallback);

	for(i = 0; i < JVX_REALTIME_PLOT_WIDTH_NUM; i++)
	{
		this->comboBox_width->addItem(jvxRealtimePlotWidthType_txt[i].c_str());
	}

	for(i = 0; i < JVX_REALTIME_PLOT_WIDTH_NUM; i++)
	{
		this->comboBox_width->addItem(jvxRealtimePlotWidthType_txt[i].c_str());
	}

	// Init all user selection fields
	selection.description = "No Description";
	selection.channelselection_y = 0;
	selection.channelselection_x = 0;
	selection.plotWidthId = 2;
	selection.lengthBuffer = 1024;
	selection.numChannels = 1;
	selection.propIdShow = JVX_SIZE_UNSELECTED;

	selection.tp.reset();

	// Init stuff for qwt
	QwtText txt;
	QFont ft = txt.font();
	ft.setPixelSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	txt.setFont(ft);

	//==============================================================
	// INIT PLOT ENGINE (Linear PLOT)
	//==============================================================
	qwtPlot->setCanvasBackground( QColor( Qt::white ) );

	QFont font = qwtPlot->axisFont(QwtPlot::xBottom);
    font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
    qwtPlot->setAxisFont (QwtPlot::xBottom, font);
    qwtPlot->setAxisFont (QwtPlot::yLeft, font);
	
	font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	txt.setFont(font);
	txt.setText("Index");
    qwtPlot->setAxisTitle( QwtPlot::xBottom, txt);
	txt.setText("Value");
    qwtPlot->setAxisTitle( QwtPlot::yLeft,txt );

	qwtPlot->setAutoDelete(false);

	// Setup a grid
	plot.theGrid = new QwtPlotGrid;
	plot.theGrid->setMajorPen( QPen( Qt::gray, 1, Qt::DotLine ) );
    plot.theGrid->setMinorPen( QPen( Qt::gray, 1 , Qt::DotLine ) );
	plot.theGrid->attach( qwtPlot );

	plot.theLegend = new QwtLegend;
	qwtPlot->insertLegend(plot.theLegend,  QwtPlot::BottomLegend );

	// Redraw all curves
	updateWindow_redraw_plots();

	// Update user selection
	updateWindow();
}

/**
 * Function to remove all UI element references from the CjvxRealtimeViewer class lists.
 *///=================================================================================================
void
realtimeViewerTDSPlot::cleanBeforeDelete()
{
	jvxSize num = 0;
	jvxRealtimeViewerPropertyCircPlotItem* theProp = NULL;

	// Set the callback references for realtime update to NULL (block updates)
	this->references.theRef->_set_update_callback_view_group_in_section(references.sectionId, references.groupId, NULL);

	// Delete grid
	if(plot.theGrid)
	{
		plot.theGrid->detach();
		delete(plot.theGrid);
	}
	plot.theGrid = NULL;

	// Get rid of legend, seems that we better not delete it.
	if(plot.theLegend)
	{
		// delete(plot.theLegend ); <- it seems that qwt deletes the legend
	}
	plot.theLegend = NULL;

	// Remove all plot curves from CjvxRealtimeViewer class lists
	references.theRef->_number_items_in_group_in_section(references.sectionId, references.groupId, &num);
	if(num >= 1)
	{
		remPlot_core();
	}
}

// ==================================================================================================================
// ==================================================================================================================

/** 
 * Update windows function shows new content in UI elements but if does not redraw the elements frm scratch
 *///==========================================================================================================
void 
realtimeViewerTDSPlot::updateWindow()
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyPlotGroup* propsGroup = NULL;
	jvxRealtimeViewerPropertyCircPlotItem* propsItem = NULL;

	QwtText txtQwt;
	std::string txt;
	jvxSize num = 0;
	jvxSize i;
	std::string descr;
	jvxSize propId = 0;
	jvxBool plotEntryEngaged = false;
	this->lineEdit_description->setText(selection.description.c_str());

	comboBox_channel_selection_yaxis->clear();
	comboBox_channel_selection_xaxis->clear();
	
	this->comboBox_width->setCurrentIndex((int)selection.plotWidthId);


	// Find out if there is a sub item
	references.theRef->_number_items_in_group_in_section(references.sectionId, references.groupId, &num);
	if(num == 0)
	{
		plotEntryEngaged = false;
	}
	else if(num == 1)
	{
		plotEntryEngaged = true;
	}
	else
	{
		assert(0);
	}

	if(!plotEntryEngaged)
	{
		lineEdit_description->setText(selection.description.c_str());
		pushButton_engage->setText("Engage");
		lineEdit_length->setEnabled(true);
		spinBox_channels->setEnabled(true);

		comboBox_channel_selection_yaxis->setEnabled(false);
		comboBox_channel_selection_xaxis->setEnabled(false);
		comboBox_components->setEnabled(true);
		comboBox_property->setEnabled(true);

		selection.idYChannel = JVX_SIZE_UNSELECTED;
	}
	else
	{
		pushButton_engage->setText("Disengage");
		lineEdit_length->setEnabled(false);
		spinBox_channels->setEnabled(false);

		/* Get number channels from private data */
		comboBox_channel_selection_yaxis->setEnabled(true);
		comboBox_channel_selection_xaxis->setEnabled(true);

		res = references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, 0,
			reinterpret_cast<jvxHandle**>(&propsItem), NULL, NULL);
		assert(res == JVX_NO_ERROR);

		if(JVX_CHECK_SIZE_UNSELECTED(propsItem->idChannelX ))
		{
			comboBox_channel_selection_xaxis->addItem("None*");
		}
		else
		{
			comboBox_channel_selection_xaxis->addItem("None");
		}

		for(i = 0; i < propsItem->ptrCircBuffer->number_channels; i++)
		{
			if(jvx_bitTest(propsItem->selection, i))
			{
				propsItem->thePlotCurves[i]->attach(qwtPlot);
				comboBox_channel_selection_yaxis->addItem(("Channel #" + jvx_size2String(i) + "*").c_str());
			}
			else
			{
				propsItem->thePlotCurves[i]->detach();
				comboBox_channel_selection_yaxis->addItem(("Channel #" + jvx_size2String(i)).c_str());
			}

			if(propsItem->idChannelX == i)
			{
				comboBox_channel_selection_xaxis->addItem(("Channel #" + jvx_size2String(i) + "*").c_str());
			}
			else
			{
				comboBox_channel_selection_xaxis->addItem(("Channel #" + jvx_size2String(i)).c_str());
			}
		}
		comboBox_components->setEnabled(false);
		comboBox_property->setEnabled(false);
			
		selection.lengthBuffer = propsItem->length;
		selection.numChannels = propsItem->num_chans;
		selection.tp = propsItem->descriptor.tp;
		selection.propIdShow = propsItem->descriptor.inPropIdx;
		
		if(selection.numChannels > 0)
		{
			if(JVX_CHECK_SIZE_SELECTED(selection.idYChannel))
			{
				selection.idYChannel = 0;
			}
		}

		comboBox_channel_selection_yaxis->setCurrentIndex((int)selection.idYChannel);
		comboBox_channel_selection_xaxis->setCurrentIndex((int)propsItem->idChannelX+1);

		/*
		for(i = 0; i < num; i++)
		{
			references.theRef->_description_item_in_group_in_section(references.sectionId, references.groupId, i, NULL, NULL, descr);
			this->comboBox_reg_plots->addItem(descr.c_str());
		}
		*/
	}

		// Show valid selections in ComboBox elements
	jvx_qt_listComponentsInComboBox(true, comboBox_components, references.theHostRef, selection.tp, tpAll);

	jvx_qt_listPropertiesInComboBox(selection.tp, JVX_PROPERTY_CONTEXT_UNKNOWN, false, comboBox_property, references.theHostRef, selection.propIdShow, &selection.propIdComponent, 
		JVX_PROPERTIES_FILTER_CIRC_PLOT_FIELDS);

	lineEdit_length->setText(jvx_size2String(selection.lengthBuffer).c_str());
	spinBox_channels->setValue((int)selection.numChannels);

	// Get the data stored with the group and update UI elements
	res = references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&propsGroup), NULL, NULL);
	if((res == JVX_NO_ERROR) && propsGroup)
	{
		QFont font = qwtPlot->axisFont(QwtPlot::xBottom);
		font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
		font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
		txtQwt.setFont(font);
		txtQwt.setText(propsGroup->xaxistxt.c_str());
		qwtPlot->setAxisTitle( QwtPlot::xBottom, txtQwt);
		txtQwt.setText(propsGroup->yaxistxt.c_str());
		qwtPlot->setAxisTitle( QwtPlot::yLeft,txtQwt );
		this->lineEdit_title_xaxis->setText(propsGroup->xaxistxt.c_str());
		this->lineEdit_title_yaxis->setText(propsGroup->yaxistxt.c_str());
		res = references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(propsGroup));

		txt = jvx_data2String(propsGroup->xmax, JVX_NUM_DIGITS_PARAMS);
		this->lineEdit_xmax->setText(txt.c_str());

		txt = jvx_data2String(propsGroup->xmin, JVX_NUM_DIGITS_PARAMS);
		this->lineEdit_xmin->setText(txt.c_str());

		txt = jvx_data2String(propsGroup->ymax, JVX_NUM_DIGITS_PARAMS);
		this->lineEdit_ymax->setText(txt.c_str());

		txt = jvx_data2String(propsGroup->ymin, JVX_NUM_DIGITS_PARAMS);
		this->lineEdit_ymin->setText(txt.c_str());

		if(propsGroup->autox)
		{
			lineEdit_xmax->setReadOnly(true);
			lineEdit_xmin->setReadOnly(true);
			checkBox_autoscale_x->setChecked(true);
		}
		else
		{
			lineEdit_xmax->setReadOnly(false);
			lineEdit_xmin->setReadOnly(false);
			checkBox_autoscale_x->setChecked(false);
		}
		if(propsGroup->autoy)
		{
			lineEdit_ymax->setReadOnly(true);
			lineEdit_ymin->setReadOnly(true);
			checkBox_autoscale_y->setChecked(true);
		}
		else
		{
			lineEdit_ymax->setReadOnly(false);
			lineEdit_ymin->setReadOnly(false);
			checkBox_autoscale_y->setChecked(false);
		}
	}

	qwtPlot->replot();
}

/**
 * This function redraws all lots
 *///===========================================================================================================
void 
realtimeViewerTDSPlot::updateWindow_redraw_plots()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxRealtimeViewerPropertyCircPlotItem* theProp  = NULL;
	jvxSize idProp;
	jvxComponentIdentification tp;
	std::string descr;
	jvx_propertyReferenceTriple theTriple;
	jvxBool removeThisFigure = false;
	jvxCallManagerProperties callGate;
	// Loop over all items
	references.theRef->_number_items_in_group_in_section(references.sectionId, references.groupId, &num);
	if(num == 1)
	{
		// Get the entry description
		references.theRef->_description_item_in_group_in_section(references.sectionId, references.groupId, 0, &idProp, &tp, descr);
			
		// Request private data field for this item
		references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, 0, reinterpret_cast<jvxHandle**>(&theProp), NULL, NULL);
		
		jvx_initPropertyReferenceTriple(&theTriple);
		if(tp.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, tp);
		}
		if(jvx_isValidPropertyReferenceTriple(&theTriple))
		{
		
			if(theProp->ptrCircBuffer == NULL)
			{
				theProp->descriptor.reset();
				jvx::propertyAddress::CjvxPropertyAddressLinear ident(idProp);
				/* Set all relevant property specific values in descriptor */
				res = theTriple.theProps->description_property(callGate, theProp->descriptor, ident);
				theProp->descriptor.tp = tp;
				theProp->descriptor.inPropIdx = idProp;

				if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_ID(idProp), theTriple.theProps))
				{
					theProp->ptrCircBuffer = jvx_allocate1DCircExternalBuffer(theProp->length, theProp->num_chans,
						JVX_DATAFORMAT_DATA, JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED, jvx_static_lock, jvx_static_try_lock,
						jvx_static_unlock, &theProp->szCircBuffer);
	
					theProp->isValid = true;

					theProp->thePlotCurves = NULL;
					JVX_SAFE_NEW_FLD(theProp->thePlotCurves, QwtPlotCurve*, theProp->ptrCircBuffer->number_channels);
					theProp->dataCpy = NULL;
					JVX_SAFE_NEW_FLD(theProp->dataCpy, jvxData*, theProp->ptrCircBuffer->number_channels);

					// theProp->idChannelX = -1; <- x channel selection may result from configuration
					//theProp->selection = 0; <- channel selection may result from configuration

					QPen thePen;

					for(i = 0; i < theProp->ptrCircBuffer->number_channels; i++)
					{
						theProp->thePlotCurves[i] = new QwtPlotCurve();
						theProp->thePlotCurves[i]->setTitle(("Channel #" + jvx_size2String(i)).c_str());

						int cntColor = (i % JVX_REALTIME_PLOT_COLOR_NUM);
						thePen.setColor(jvxRealtimePlotColorType_translate[cntColor]);
						thePen.setWidth((int)theProp->lineWidth);

						thePen.setStyle(Qt::SolidLine);
						theProp->thePlotCurves[i]->setPen(thePen);
						/* 
						theProp->thePlotCurves[i]->attach(qwtPlot);
						*/
						JVX_SAFE_NEW_FLD(theProp->dataCpy[i], jvxData, theProp->ptrCircBuffer->length);
						memset(theProp->dataCpy[i], 0, sizeof(jvxData) * theProp->ptrCircBuffer->length);
					}

					theProp->dataInc = NULL;
					JVX_SAFE_NEW_FLD(theProp->dataInc, jvxData, theProp->ptrCircBuffer->length);
					for(i = 0; i < theProp->ptrCircBuffer->length; i++)
					{
						theProp->dataInc[i] = (jvxData)i;
					}
					theProp->expectRescaleX = true;

					jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(theProp->ptrCircBuffer, &theProp->isValid, 
						theProp->ptrCircBuffer->formFld);
					jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(theProp->descriptor.globalIdx, 
						theProp->descriptor.category);

					res = theTriple.theProps->install_property_reference(
						callGate,
						ptrRaw, ident);
					assert(res == JVX_NO_ERROR);


					// Restore propertries
					references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, i, theProp);
				}
			} // if(theProp->ptrCircBuffer == NULL)
			else
			{
				if(!theProp->isValid)
				{
					// If user of circ buffer indicates the circ buffer to be invalid, drop that buffer
					remPlot_core();
				}
			}
		}
		else
		{
			// Failed to get reference to the component, remove entry
			if(theProp->ptrCircBuffer != NULL)
			{
				remPlot_core();
			}
			else
			{
				assert(0);
			}
		}
	}
	
	qwtPlot->replot();
}

void 
realtimeViewerTDSPlot::newSelection_toggle_channel_y(int sel)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyCircPlotItem* propsItem = NULL;
	jvxSize num = 0;

	res = references.theRef->_number_items_in_group_in_section(references.sectionId, references.groupId, &num);
	if(num == 1)
	{
		// Get the data stored with the group and update UI elements
		res = references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, 0, reinterpret_cast<jvxHandle**>(&propsItem), NULL, NULL);
		if((res == JVX_NO_ERROR) && propsItem)
		{
			if(jvx_bitTest(propsItem->selection,sel))
			{
				propsItem->selection = propsItem->selection & (~((jvxBitField)1 << sel));
			}
			else
			{
				propsItem->selection = propsItem->selection | ((jvxBitField)1 << sel);

				if(propsItem->idChannelX == sel)
				{
					propsItem->idChannelX = JVX_SIZE_UNSELECTED;
				}
			}
			references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, 0, reinterpret_cast<jvxHandle*>(propsItem));
		}
		selection.idYChannel = sel;
		updateWindow();
	}
}

void 
realtimeViewerTDSPlot::checkedBox_show_plot(bool)
{
}

void 
realtimeViewerTDSPlot::checkedBox_autoscale_x_plot(bool tog)
{
	jvxRealtimeViewerPropertyPlotGroup* props = NULL;
	references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&props), NULL, NULL);
	props->autox = tog;
	references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(props));
	updateWindow();
}

void 
realtimeViewerTDSPlot::newText_description_plot()
{
	this->selection.description = lineEdit_description->text().toLatin1().constData();
	updateWindow();
}

void 
realtimeViewerTDSPlot::newSelection_component(int sel)
{
	this->selection.tp.tp = (jvxComponentType)sel;
	updateWindow();
}

void 
realtimeViewerTDSPlot::newSelection_property(int sel)
{
	selection.propIdShow = sel;
	updateWindow();
}






void 
realtimeViewerTDSPlot::newText_y_min()
{
	jvxData newYMin = lineEdit_ymin->text().toData();

	jvxRealtimeViewerPropertyPlotGroup* props = NULL;
	references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&props), NULL, NULL);
	props->ymin = newYMin;
	props->ymax = JVX_MAX(props->ymin + JVX_EPSILON, props->ymax);
	references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(props));
	updateWindow();
}

void 
realtimeViewerTDSPlot::newText_x_min()
{
	jvxData newXMin = lineEdit_xmin->text().toData();

	jvxRealtimeViewerPropertyPlotGroup* props = NULL;
	references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&props), NULL, NULL);
	props->xmin = newXMin;
	props->xmax = JVX_MAX(props->xmin + JVX_EPSILON, props->xmax);
	references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(props));
	updateWindow();
}

void 
realtimeViewerTDSPlot::newText_x_max()
{
	jvxData newXMax = lineEdit_xmax->text().toData();

	jvxRealtimeViewerPropertyPlotGroup* props = NULL;
	references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&props), NULL, NULL);
	props->xmax = newXMax;
	props->xmin = JVX_MIN(props->xmax - JVX_EPSILON, props->xmin);
	references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(props));
	updateWindow();
}

void 
realtimeViewerTDSPlot::newText_y_max()
{
	jvxData newYMax = lineEdit_ymax->text().toData();

	jvxRealtimeViewerPropertyPlotGroup* props = NULL;
	references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&props), NULL, NULL);
	props->ymax = newYMax;
	props->ymin = JVX_MIN(props->ymax - JVX_EPSILON, props->ymin);
	references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(props));
	updateWindow();
}
	
void 
realtimeViewerTDSPlot::checkedBox_autoscale_y_plot(bool tog)
{
	jvxRealtimeViewerPropertyPlotGroup* props = NULL;
	references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&props), NULL, NULL);
	props->autoy = tog;
	references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(props));
	updateWindow();

}




void  
realtimeViewerTDSPlot::newText_yaxistitle()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyPlotGroup* propsGroup = NULL;

	// Get the data stored with the group and update UI elements
	res = references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&propsGroup), NULL, NULL);
	if((res == JVX_NO_ERROR) && propsGroup)
	{
		propsGroup->yaxistxt = lineEdit_title_yaxis->text().toLatin1().constData();
		res = references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(propsGroup));
	}
	updateWindow();
}

void  
realtimeViewerTDSPlot::newText_xaxistitle()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyPlotGroup* propsGroup = NULL;

	// Get the data stored with the group and update UI elements
	res = references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&propsGroup), NULL, NULL);
	if((res == JVX_NO_ERROR) && propsGroup)
	{
		propsGroup->xaxistxt = lineEdit_title_xaxis->text().toLatin1().constData();
		res = references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(propsGroup));
	}
	updateWindow();
}

void 
realtimeViewerTDSPlot::newSelection_toggle_channel_x(int sel)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyCircPlotItem* propsItem = NULL;
	jvxSize num = 0;

	res = references.theRef->_number_items_in_group_in_section(references.sectionId, references.groupId, &num);
	if(num == 1)
	{
		// Get the data stored with the group and update UI elements
		res = references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, 0, reinterpret_cast<jvxHandle**>(&propsItem), NULL, NULL);
		if((res == JVX_NO_ERROR) && propsItem)
		{
			sel -= 1;
			propsItem->idChannelX = sel;
			if(JVX_CHECK_SIZE_SELECTED(sel ))
			{
				propsItem->selection = propsItem->selection & (~((jvxBitField)1 << sel));
			}

			if(JVX_CHECK_SIZE_UNSELECTED(propsItem->idChannelX ))
			{
				if(propsItem->dataInc)
				{
					for(i = 0; i < propsItem->length; i++)
					{
						propsItem->dataInc[i] = i;
					}
				}
			}
			else
			{
				propsItem->expectRescaleX = true;
			}
			references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, 0, reinterpret_cast<jvxHandle*>(propsItem));
		}
		updateWindow();
	}
}

jvxErrorType 
realtimeViewerTDSPlot::callbackUpdateItem(jvxSize SectionId, jvxSize groupId, jvxSize itemId, jvxHandle* privateData)
{
	if(privateData)
	{
		return(((realtimeViewerTDSPlot*)privateData)->cbUpdateItem(SectionId, groupId, itemId));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);

}

jvxErrorType 
realtimeViewerTDSPlot::callbackUpdateGroup(jvxSize SectionId, jvxSize groupId, jvxBool callOnStart, jvxHandle* privateData)
{
	if(privateData)
	{
		return(((realtimeViewerTDSPlot*)privateData)->cbUpdateGroup(SectionId, groupId, callOnStart));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
realtimeViewerTDSPlot::cbUpdateItem(jvxSize SectionId, jvxSize groupId, jvxSize itemId)
{
	jvxSize i,j;
	jvxSize idxProp = 0;
	jvxComponentType tp = JVX_COMPONENT_UNKNOWN;
	std::string nm;
	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyCircPlotItem* theProp = NULL;
	jvx_propertyReferenceTriple theTriple;
	jvx_initPropertyReferenceTriple(&theTriple);
	bool isValid = false;
	jvxSize numCopy = 0;
	jvxSize numOld = 0;
	jvxBool requiresPlot = false;
	jvxData ymax = 0;
	jvxData ymin = 0;
	jvxData xmax = 0;
	jvxData xmin = 0;
	jvxSize ll1 = 0;
	jvxSize ll2 = 0;

	if(
		(SectionId == references.sectionId) &&
		(groupId == references.groupId) &&
		(itemId == 0))
	{
		this->references.theRef->_request_property_item_in_group_in_section(SectionId, groupId, itemId, reinterpret_cast<jvxHandle**>(&theProp), NULL, NULL);
		//this->references.theRef->_request_property_group_in_section(SectionId, groupId, reinterpret_cast<jvxHandle**>(&thePropG), NULL, NULL);
		if(theProp)// && thePropG)
		{
			if(theProp->isValid)
			{
				if(theProp->ptrCircBuffer)
				{
					numCopy = theProp->ptrCircBuffer->fill_height;
					numCopy = (numCopy>>1)<<1;
					numOld = theProp->ptrCircBuffer->length - numCopy;
					ll1 = JVX_MIN(numCopy, theProp->ptrCircBuffer->length - theProp->ptrCircBuffer->idx_read);
					ll2 = numCopy - ll1;

					jvxData* inPtr = (jvxData*)(theProp->ptrCircBuffer->ptrFld);
					jvxData* inPtrLl2 = NULL;
					jvxData* inPtrLl1 = NULL;
					jvxData* outPtr = NULL;
					for(i = 0; i < theProp->ptrCircBuffer->number_channels; i++)
					{
						inPtrLl2 = inPtr;
						if(jvx_bitTest(theProp->selection, i))
						{
							inPtrLl2 += theProp->ptrCircBuffer->length * i;
							inPtrLl1 = inPtrLl2 + theProp->ptrCircBuffer->idx_read;
							outPtr = theProp->dataCpy[i] + numOld;

							memmove(theProp->dataCpy[i], theProp->dataCpy[i] + numCopy, sizeof(jvxData) * numOld);
							if(ll1 > 0)
							{
								memcpy(outPtr, inPtrLl1, sizeof(jvxData) * ll1);
							}
							if(ll2 > 0)
							{
								outPtr += ll1;
								memcpy(outPtr, inPtrLl2, sizeof(jvxData) * ll2);
							}
							requiresPlot = true;
						}
					
						if(i == theProp->idChannelX)
						{
							inPtrLl2 += theProp->ptrCircBuffer->length * i;
							inPtrLl1 = inPtrLl2 + theProp->ptrCircBuffer->idx_read;
							jvxData* outPtr = theProp->dataInc + (theProp->ptrCircBuffer->length - numCopy);

							if(theProp->expectRescaleX)
							{
								if(ll1 >= 1)
								{
									for(j = 0; j < theProp->ptrCircBuffer->length; j++)
									{
										theProp->dataInc[j] = inPtrLl1[0];
									}
									theProp->expectRescaleX = false;
								}
								if(ll2 >= 1)
								{
									for(j = 0; j < theProp->ptrCircBuffer->length; j++)
									{
										theProp->dataInc[j] = inPtrLl2[0];
									}
									theProp->expectRescaleX = false;
								}
							}

							memmove(theProp->dataInc, theProp->dataInc + numCopy, sizeof(jvxData) * numOld);
							if(ll1 > 0)
							{
								memcpy(outPtr, inPtrLl1, sizeof(jvxData) * ll1);
							}
							if(ll2 > 0)
							{
								outPtr += ll1;
								memcpy(outPtr, inPtrLl2, sizeof(jvxData) * ll2);
							}
							requiresPlot = true;
							/*
							jvxData minV = theProp->dataInc[0];
							for(j = 0; j < theProp->ptrCircBuffer->bLength; j++)
							{
								if(theProp->dataInc[j] < minV)
								{
									int a = 0;
								}
							}
							*/
						}
					}

					theProp->ptrCircBuffer->safe_access.lockf(theProp->ptrCircBuffer->safe_access.priv);
					theProp->ptrCircBuffer->fill_height -= numCopy;
					theProp->ptrCircBuffer->idx_read = (theProp->ptrCircBuffer->idx_read + numCopy) % theProp->ptrCircBuffer->length;
					theProp->ptrCircBuffer->safe_access.unlockf(theProp->ptrCircBuffer->safe_access.priv);
				}
			}

			if(requiresPlot)
			{
				ymax = JVX_DATA_MAX_NEG;
				ymin = JVX_DATA_MAX_POS;
				xmax = JVX_DATA_MAX_NEG;
				xmin = JVX_DATA_MAX_POS;

				jvxData* ptr = theProp->dataInc;
				for(i = 0; i < theProp->ptrCircBuffer->length; i++)
				{
					theProp->maxX = JVX_MAX(theProp->maxX, ptr[i]);
					theProp->minX = JVX_MIN(theProp->minX, ptr[i]);
				}

				for(i = 0; i < theProp->ptrCircBuffer->number_channels; i++)
				{
					if(jvx_bitTest(theProp->selection, i))
					{
						jvxData* ptr = theProp->dataCpy[i];
						for(j = 0; j < theProp->ptrCircBuffer->length; j++)
						{
							theProp->maxY = JVX_MAX(theProp->maxY, ptr[j]);
							theProp->minY = JVX_MIN(theProp->minY, ptr[j]);
							theProp->minmaxValid = true;
						}
					}
					theProp->thePlotCurves[i]->setSamples(theProp->dataInc, theProp->dataCpy[i], theProp->ptrCircBuffer->length);
				}
			}

			//this->references.theRef->_return_property_group_in_section(SectionId, groupId, reinterpret_cast<jvxHandle*>(thePropG));
			this->references.theRef->_return_property_item_in_group_in_section(SectionId, groupId, itemId, reinterpret_cast<jvxHandle*>(theProp));
		}


	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
realtimeViewerTDSPlot::cbUpdateGroup(jvxSize SectionId, jvxSize groupId, jvxBool callOnStart)
{
	jvxRealtimeViewerPropertyPlotGroup* thePropG = NULL;
	jvxRealtimeViewerPropertyCircPlotItem* thePropI = NULL;
	jvxSize num = 0;
	jvxSize i;
	jvxData xmax;
	jvxData xmin;
	jvxData ymax;
	jvxData ymin;
	jvxBool oneValid = false;
	std::string txt;

	if(callOnStart)
	{
		/*
		// Todo: select all properties for which this is relevant and freeze those 
		*/
	}
	else
	{
		if(
			(SectionId == references.sectionId) &&
			(groupId == references.groupId))
		{
			this->references.theRef->_request_property_group_in_section(SectionId, groupId, reinterpret_cast<jvxHandle**>(&thePropG), NULL, NULL);
			assert(thePropG);
			xmax = JVX_DATA_MAX_NEG;
			xmin = JVX_DATA_MAX_POS;
			ymax = JVX_DATA_MAX_NEG;
			ymin = JVX_DATA_MAX_POS;

			references.theRef->_number_items_in_group_in_section(SectionId, groupId, &num);
			for(i = 0; i < num; i++)
			{
				this->references.theRef->_request_property_item_in_group_in_section(SectionId, groupId, i, reinterpret_cast<jvxHandle**>(&thePropI), NULL, NULL);
				assert(thePropI);
				if(thePropI->minmaxValid)
				{
					xmax = JVX_MAX(thePropI->maxX, xmax);
					xmin = JVX_MIN(thePropI->minX, xmin);
					ymax = JVX_MAX(thePropI->maxY, ymax);
					ymin = JVX_MIN(thePropI->minY, ymin);
					oneValid = true;
				}
				thePropI->minmaxValid = false;
				thePropI->maxX = JVX_DATA_MAX_NEG;
				thePropI->maxY = JVX_DATA_MAX_NEG;
				thePropI->minX = JVX_DATA_MAX_POS;
				thePropI->minY = JVX_DATA_MAX_POS;
				this->references.theRef->_return_property_item_in_group_in_section(SectionId, groupId, i, reinterpret_cast<jvxHandle*>(thePropI));
			}

			if(oneValid)
			{
				if(thePropG->autox)
				{
					thePropG->xmax = xmax;
					thePropG->xmin = xmin;
				}
				if(thePropG->autoy)
				{
					thePropG->ymax = ymax;
					thePropG->ymin = ymin;
				}
			}

			if(fabs(thePropG->xmax - thePropG->xmin) < JVX_EPSILON)
			{
				thePropG->xmax += JVX_EPSILON/2;
				thePropG->xmin -= JVX_EPSILON/2;
			}
			if(fabs(thePropG->ymax - thePropG->ymin) < JVX_EPSILON)
			{
				thePropG->ymax += JVX_EPSILON/2;
				thePropG->ymin -= JVX_EPSILON/2;
			}

			qwtPlot->setAxisScale(QwtPlot::xBottom, thePropG->xmin, thePropG->xmax);
			qwtPlot->setAxisScale(QwtPlot::yLeft, thePropG->ymin, thePropG->ymax);

			if(thePropG->autox)
			{
				txt = jvx_data2String(thePropG->xmax, JVX_NUM_DIGITS_PARAMS);
				this->lineEdit_xmax->setText(txt.c_str());
				txt = jvx_data2String(thePropG->xmin, JVX_NUM_DIGITS_PARAMS);
				this->lineEdit_xmin->setText(txt.c_str());
			}
			if(thePropG->autoy)
			{
				txt = jvx_data2String(thePropG->ymax, JVX_NUM_DIGITS_PARAMS);
				this->lineEdit_ymax->setText(txt.c_str());
				txt = jvx_data2String(thePropG->ymin, JVX_NUM_DIGITS_PARAMS);
				this->lineEdit_ymin->setText(txt.c_str());
			}
			this->qwtPlot->replot();
			this->references.theRef->_return_property_group_in_section(SectionId, groupId, reinterpret_cast<jvxHandle*>(thePropG));
		}
	}
	return(JVX_NO_ERROR);
}

void 
realtimeViewerTDSPlot::newSelection_plot_width(int sel)
{
	selection.plotWidthId = sel;

	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyCircPlotItem* propsItem = NULL;
	jvxSize num = 0;

	res = references.theRef->_number_items_in_group_in_section(references.sectionId, references.groupId, &num);
	if(num == 1)
	{
		// Get the data stored with the group and update UI elements
		res = references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, 0, reinterpret_cast<jvxHandle**>(&propsItem), NULL, NULL);
		if((res == JVX_NO_ERROR) && propsItem)
		{
			QPen thePen;
			assert(propsItem->ptrCircBuffer);
			propsItem->lineWidth = jvxRealtimePlotWidthType_translate[selection.plotWidthId];
			for(i = 0; i < propsItem->ptrCircBuffer->number_channels; i++)
			{
				thePen = propsItem->thePlotCurves[i]->pen();
				thePen.setWidth((int)propsItem->lineWidth);
				propsItem->thePlotCurves[i]->setPen(thePen);
			}
			references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, 0, reinterpret_cast<jvxHandle*>(propsItem));
		}
		updateWindow();
	}
	updateWindow();
}

void 
realtimeViewerTDSPlot::buttonPushed_engage_disengage()
{
	jvxSize num;
	
	// Find out if there is a sub item
	references.theRef->_number_items_in_group_in_section(references.sectionId, references.groupId, &num);
	if(num == 0)
	{
		jvxBool plotAdded = addPlot_core(selection.tp, selection.propIdComponent, selection.numChannels, selection.lengthBuffer, selection.plotWidthId, selection.description);
		if(plotAdded)
		{
			updateWindow_redraw_plots();
			updateWindow();
		}

	}
	else
	{
		jvxBool plotRemoved = remPlot_core();
		if(plotRemoved)
		{
			updateWindow();
		}
	}
}

bool 
realtimeViewerTDSPlot::addPlot_core(jvxComponentIdentification tp, jvxSize propId, jvxSize numChannels, jvxSize lengthBuffer, jvxSize plotWidthId, std::string description)
{
	bool plotAdded = false;

	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyCircPlotItem propsItem;
	jvx_propertyReferenceTriple theTriple;
	jvxCallManagerProperties callGate;
	static_initData(&propsItem, sizeof(jvxRealtimeViewerPropertyCircPlotItem), JVX_REALTIME_PRIVATE_MEMORY_LINEAR_SPLOT_SINGLE_DIAG_ITEM);

	if(JVX_CHECK_SIZE_SELECTED(propId))
	{
		jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(propId);

		jvx_initPropertyReferenceTriple(&theTriple);
		if(tp.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, tp);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			// Chek wether property selection is valid
			/* Set all relevant property specific values in descriptor */
			res = theTriple.theProps->description_property(callGate,theDescr, ident);
			if(res == JVX_NO_ERROR)
			{
				if(
					(theDescr.decTp == JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER) &&
					(theDescr.format == JVX_DATAFORMAT_DATA))
				{
					propsItem.descriptor.tp = tp;
					propsItem.length = lengthBuffer;
					propsItem.num_chans = numChannels;
					propsItem.lineWidth = jvxRealtimePlotWidthType_translate[plotWidthId];

					res = references.theRef->_insert_item_in_group_in_section(references.sectionId, references.groupId, selection.tp, selection.propIdComponent, 
						JVX_SIZE_UNSELECTED, description, &propsItem, sizeof(jvxRealtimeViewerPropertyCircPlotItem), JVX_REALTIME_PRIVATE_MEMORY_LINEAR_SPLOT_SINGLE_DIAG_ITEM);
					if(res != JVX_NO_ERROR)
					{
						references.report->report_simple_text_message("Failed to call <_insert_item_in_group_in_section> in <realtimeViewerTDSPlot::addPlot_core>", JVX_REPORT_PRIORITY_ERROR);
					}
					else
					{
						plotAdded = true;
					}
				}
				else
				{
					references.report->report_simple_text_message("Engaging item failed since property is not of correct type.", JVX_REPORT_PRIORITY_WARNING);
				}
			}
			else
			{
				references.report->report_simple_text_message("Engaging item failed due to invalid propety id.", JVX_REPORT_PRIORITY_WARNING);
			}
		}
		else
		{
			references.report->report_simple_text_message("Editing item failed since there is no selection of a valid property.", JVX_REPORT_PRIORITY_WARNING);
		}
	}
	else
	{
		references.report->report_simple_text_message("Editing item failed since there is no selection of a property.", JVX_REPORT_PRIORITY_WARNING);
	}
	return(plotAdded);
}

bool 
realtimeViewerTDSPlot::remPlot_core()
{
	jvxSize i;
	bool plotAdded = false;

	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyCircPlotItem* propsItem;
	jvx_propertyReferenceTriple theTriple;
	jvxCallManagerProperties callGate;
	jvxRealtimeViewerPropertyPlotItem* theProp = NULL;

	// There is only one item
	res = references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, 0, reinterpret_cast<jvxHandle**>( &propsItem), NULL, NULL);
	assert(res == JVX_NO_ERROR);		

	if(propsItem->ptrCircBuffer->safe_access.allowRemove)
	{
		jvx_initPropertyReferenceTriple(&theTriple);
		res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, propsItem->descriptor.tp);
		if((res == JVX_NO_ERROR) && (theTriple.theProps))
		{
			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(propsItem->ptrCircBuffer, &propsItem->isValid,
				propsItem->ptrCircBuffer->formFld);
			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(propsItem->descriptor.globalIdx,
				propsItem->descriptor.category);

			// Take ext referfence from component property handler
			res = theTriple.theProps->uninstall_property_reference(callGate, ptrRaw, ident);
			assert(res == JVX_NO_ERROR);
		}

		// Finally, remove the fields
		for(i = 0; i < propsItem->ptrCircBuffer->number_channels; i++)
		{
			propsItem->thePlotCurves[i]->detach();
			delete(propsItem->thePlotCurves[i]);

			JVX_SAFE_DELETE_FLD(propsItem->dataCpy[i], jvxData);
		}

		JVX_SAFE_DELETE_FLD(propsItem->thePlotCurves, QwtPlotCurve*);
		propsItem->thePlotCurves = NULL;
		JVX_SAFE_DELETE_FLD(propsItem->dataCpy, jvxData*);
		propsItem->dataCpy = NULL;
		JVX_SAFE_DELETE_FLD(propsItem->dataInc, jvxData);
		propsItem->dataInc = NULL;

		propsItem->lineWidth = 0;
		jvx_deallocateExternalBuffer(propsItem->ptrCircBuffer);
		propsItem->ptrCircBuffer = NULL;

		propsItem->numElmsCircBuffer = 0;

		propsItem->szCircBuffer = 0;

		propsItem->length = 0;
		propsItem->num_chans = 0;

		// This will remove the private item
		res = references.theRef->_remove_item_in_group_in_section(references.sectionId, references.groupId, 0);
		assert(res == JVX_NO_ERROR);
	}
	return(true);
}
void 
realtimeViewerTDSPlot::newText_length()
{
	int val = lineEdit_length->text().toInt();
	if(val > 0)
	{
		selection.lengthBuffer = val;
	}
	updateWindow();
}

void 
realtimeViewerTDSPlot::newSetting_numchannels(int sel)
{
	if(sel == -1 ||  sel == 0)
	{
		sel = 1;
	}
	if(sel != selection.numChannels)
	{
		selection.numChannels = sel;
		updateWindow();
	}
}

void 
realtimeViewerTDSPlot::newText_numChannels()
{
	int val = spinBox_channels->text().toInt();
	newSetting_numchannels(val);
}

#if 0
/**
 * Function to indicate new choice in plot selection combo box
 *///=================================================================================
void 
realtimeViewerTDSPlot::newSelection_reg_plots(int sel)
{
	jvxRealtimeViewerPropertyPlotItem* propsItem = NULL;
	jvxSize idxProp = 0;
	jvxComponentType tp = JVX_COMPONENT_UNKNOWN;
	std::string nm;

	// Set new selection
	selection.plotSelection = sel;

	// Find which property is associated to this selection
	references.theRef->_description_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, &idxProp, &tp, nm);

	// Set all user selection fields accordingly
	selection.description = nm;
	selection.tp = tp;
	selection.y.inPropIdComponent = idxProp;	
	jvx_qt_listPropertiesIndexShowForIndexComponent(selection.tp, references.theHostRef, selection.y.inPropIdShow, selection.y.inPropIdComponent,JVX_PROPERTIES_FILTER_PLOT_FIELDS);

	// Also the specific data
	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle**>(&propsItem), NULL, NULL);
	selection.plotColorId = propsItem->color;
	selection.plotStyleId = propsItem->linestyle;
	selection.plotWidthId = propsItem->width;
	selection.x.inPropIdComponent = propsItem->idPropX;
	references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle*>(propsItem));

	// Update the property list ndex
	jvx_qt_listPropertiesIndexShowForIndexComponent(tp, references.theHostRef, selection.x.inPropIdShow, selection.x.inPropIdComponent, JVX_PROPERTIES_FILTER_PLOT_FIELDS);

	updateWindow();
}

/**
 * Check box to indicate that plot is shown
 *///========================================================================================
void 
realtimeViewerTDSPlot::checkedBox_show_plot(bool tog)
{
	jvxRealtimeViewerPropertyPlotItem* props = NULL;
	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle**>(&props), NULL, NULL);
	props->show = tog;
	references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle*>(props));
	updateWindow();
}



/**
 * Add new plot current selection
 *///========================================================================================
void 
realtimeViewerTDSPlot::buttonPushed_add_plot()
{
	addPlot_core();

	updateWindow_redraw_plots();
	updateWindow();
}

/**
 * Button has been pushed to edit current plot
 *///==========================================================================================
void 
realtimeViewerTDSPlot::buttonPushed_edit_plot()
{
	jvxErrorType res = JVX_NO_ERROR;

	buttonPushed_remove_plot();
	buttonPushed_add_plot();
}

/**
 * Button has been pushed to edit current plot
 *///==========================================================================================
void 
realtimeViewerTDSPlot::buttonPushed_remove_plot()
{
	jvxSize num = 0;
	jvxRealtimeViewerPropertyPlotItem* theProp = NULL;
	references.theRef->_number_items_in_group_in_section(references.sectionId, references.groupId, &num);
	if(selection.plotSelection < (jvxInt16)num)
	{
		references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle**>( &theProp), NULL, NULL);
		if(theProp->thePlotCurve)
		{
			theProp->thePlotCurve->detach();
			delete(theProp->thePlotCurve);
			theProp->thePlotCurve = NULL;

			// Remove the buffer for input data
			if(theProp->dataX)
			{
				JVX_SAFE_DELETE_FLD(theProp->dataX, jvxData);
			}
			if(theProp->dataY)
			{
				JVX_SAFE_DELETE_FLD(theProp->dataY, jvxData);
			}
			theProp->numValues = 0;
		}
		qwtPlot->replot();
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, theProp);
	}

	// Data will be removed in the remove function call
	references.theRef->_remove_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection);
	updateWindow();
}



#endif

void  
realtimeViewerTDSPlot::updateAllPropertyDescriptors()
{
	/*
	jvxSize i,j;
	bool plotAdded = false;
	jvxSize num = 0;
	jvxSize inPropId;
	jvxComponentType tp = JVX_COMPONENT_UNKNOWN;
	std::string nm;
	return;*/
}

