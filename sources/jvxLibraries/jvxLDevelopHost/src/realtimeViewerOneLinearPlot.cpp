#include "realtimeViewerPlots.h"
#include "realtimeViewerOneLinearPlot.h"
#include "realtimeViewer_typedefs.h"
#include "realtimeViewer_helpers.h"
#include "jvx-qt-helpers.h"
#include "uMainWindow_defines.h"

/**
 * Constructor: Set section and group id and also the realtime update callbacks.
 *///==================================================================================================
realtimeViewerOneLinearPlot::realtimeViewerOneLinearPlot(realtimeViewerPlots* parent, IjvxHost* hostRef, CjvxRealtimeViewer* rtvRef, IjvxReport* report, 
	jvxSize sectionId, jvxSize groupId, jvxComponentIdentification* tpAllI):
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
	this->setProperty("BASE_REALTIMEVIEWER", QVariant::fromValue(reinterpret_cast<void*>(static_cast<realtimeViewer_base*>(this))));
}

/**
 * Destructor: Make sure all plots are removed properly.
 *///=================================================================================================
realtimeViewerOneLinearPlot::~realtimeViewerOneLinearPlot()
{
	this->cleanBeforeDelete();
}

/**
 * Init function to create UI elements and to create link to callback structure in real-time update.
 *///=================================================================================================
void
realtimeViewerOneLinearPlot::init()
{
	int i;

	// Create all ui elements
	this->setupUi(this);

	this->comboBox_context->addItem("All"); // JVX_PROPERTY_CONTEXT_UNKNOWN 
	this->comboBox_context->addItem("Parameter"); // JVX_PROPERTY_CONTEXT_PARAMETER 
	this->comboBox_context->addItem("Info"); // JVX_PROPERTY_CONTEXT_INFO 
	this->comboBox_context->addItem("Result"); // JVX_PROPERTY_CONTEXT_RESULT 
	this->comboBox_context->addItem("Command"); // JVX_PROPERTY_CONTEXT_COMMAND 
	this->comboBox_context->addItem("ViewBuf");// JVX_PROPERTY_CONTEXT_VIEWBUFFER


	// Set reference to callback
	this->references.theRef->_set_update_callback_view_group_in_section(references.sectionId, references.groupId, &myCallback);

	for(i = 0; i < JVX_REALTIME_PLOT_COLOR_NUM; i++)
	{
		this->comboBox_color->addItem(jvxRealtimePlotColorType_txt[i].c_str());
	}

	for(i = 0; i < JVX_REALTIME_PLOT_LINE_NUM; i++)
	{
		this->comboBox_style->addItem(jvxRealtimePlotLinestyleType_txt[i].c_str());
	}

	for(i = 0; i < JVX_REALTIME_PLOT_WIDTH_NUM; i++)
	{
		this->comboBox_width->addItem(jvxRealtimePlotWidthType_txt[i].c_str());
	}

	// Init all user selection fields
	selection.description = "No Description";
	selection.plotColorId = JVX_REALTIME_PLOT_COLOR_BLACK;
	selection.plotStyleId = JVX_REALTIME_PLOT_LINE_SOLID;
	selection.plotWidthId = JVX_REALTIME_PLOT_1PIXEL;
	//selection.tp = jvxCreateComponentIdentification( JVX_COMPONENT_UNKNOWN, 0, 0);
	selection.ctxt = JVX_PROPERTY_CONTEXT_UNKNOWN;
	selection.showDescriptors = false;
	selection.x.inPropIdComponent = JVX_SIZE_UNSELECTED;
	selection.y.inPropIdComponent = JVX_SIZE_UNSELECTED;
	selection.x.inPropIdShow = JVX_SIZE_UNSELECTED;
	selection.y.inPropIdShow = JVX_SIZE_UNSELECTED;
	selection.plotSelection = JVX_SIZE_UNSELECTED;

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

	updateAllPropertyDescriptors();

	// Redraw all curves
	updateWindow_redraw_plots();

	// Update user selection
	updateWindow();
}

/**
 * Function to remove all UI element references from the CjvxRealtimeViewer class lists.
 *///=================================================================================================
void
realtimeViewerOneLinearPlot::cleanBeforeDelete()
{
	jvxSize num = 0;
	jvxSize i;
	jvxRealtimeViewerPropertyPlotItem* theProp = NULL;

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
	for(i = 0; i < num; i++)
	{
		references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, i, reinterpret_cast<jvxHandle**>( &theProp), NULL, NULL);
		if(theProp->thePlotCurve)
		{
			theProp->thePlotCurve->detach();
			delete(theProp->thePlotCurve);
			theProp->thePlotCurve = NULL;
		}

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

		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, i, theProp);
	}
}

// ==================================================================================================================
// ==================================================================================================================

/**
 * Update windows function shows new content in UI elements but if does not redraw the elements frm scratch
 *///==========================================================================================================
void
realtimeViewerOneLinearPlot::updateWindow()
{
	jvxSize i;
	jvxRealtimeViewerPropertyPlotGroup* propsGroup = NULL;
	jvxRealtimeViewerPropertyPlotItem* propsItem = NULL;

	// Create a specific list which holds the right comp id
	jvxComponentIdentification tpAllC[JVX_COMPONENT_ALL_LIMIT];
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		tpAllC[i] = tpAll[i];
	}
	tpAllC[selection.tpV.tp] = selection.tpV;

	QwtText txtQwt;
	std::string txt;
	jvxSize num = 0;
	std::string descr;
	this->lineEdit_description->setText(selection.description.c_str());

	// Show valid selections in ComboBox elements
	jvx_qt_listComponentsInComboBox_(true, comboBox_components, references.theHostRef, selection.tpV);

	jvx_qt_listPropertiesInComboBox(selection.tpV, selection.ctxt, selection.showDescriptors, comboBox_y_properties, references.theHostRef, selection.y.inPropIdShow, 
		&selection.y.inPropIdComponent, JVX_PROPERTIES_FILTER_PLOT_FIELDS);

	// Get the data stored with the group and update UI elements
	references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&propsGroup), NULL, NULL);

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

	this->comboBox_reg_plots->clear();

	// Request private data field.
	references.theRef->_number_items_in_group_in_section(references.sectionId, references.groupId, &num);
	for(i = 0; i < num; i++)
	{
		references.theRef->_description_item_in_group_in_section(references.sectionId, references.groupId, i, NULL, NULL, descr);
		this->comboBox_reg_plots->addItem(descr.c_str());
	}

	if(selection.plotSelection >= num)
	{
		selection.plotSelection = num-1;
	}

	if(JVX_CHECK_SIZE_SELECTED(selection.plotSelection) && (selection.plotSelection < this->comboBox_reg_plots->count()))
	{
		this->comboBox_reg_plots->setCurrentIndex((int)selection.plotSelection);
	}

	this->comboBox_color->setCurrentIndex((int)selection.plotColorId);
	this->comboBox_style->setCurrentIndex((int)selection.plotStyleId);
	this->comboBox_width->setCurrentIndex((int)selection.plotWidthId);

	this->comboBox_context->setCurrentIndex(selection.ctxt);
	this->checkBox_showDescriptors->setChecked(selection.showDescriptors);

	// Return data pointer
	references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(propsGroup));

	if(JVX_CHECK_SIZE_SELECTED(selection.plotSelection))
	{
		references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle**>(&propsItem), NULL, NULL);
		this->checkBox_show->setChecked(propsItem->show);
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle*>(propsItem));
	}
}

/**
 * This function redraws all lots
 *///===========================================================================================================
void
realtimeViewerOneLinearPlot::updateWindow_redraw_plots()
{
	jvxSize i,j;
	jvxSize num = 0;
	jvxRealtimeViewerPropertyPlotItem* theProp  = NULL;
	QwtPlotCurve* theCurve = NULL;
	jvxSize idProp;
	jvxComponentIdentification tp;
	std::string descr;

	// Loop over all items
	references.theRef->_number_items_in_group_in_section(references.sectionId, references.groupId, &num);
	for(i = 0; i < num; i++)
	{
		// Get the entry description
		references.theRef->_description_item_in_group_in_section(references.sectionId, references.groupId, i, &idProp, &tp, descr);

		// Request private data field for this item
		references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, i, reinterpret_cast<jvxHandle**>(&theProp), NULL, NULL);

		// If thre is an item without plot curve, add one
		if(theProp->thePlotCurve == NULL)
		{
			theCurve = new QwtPlotCurve();
			theCurve->setTitle(descr.c_str());
			QPen thePen;
			switch(theProp->color)
			{
			case JVX_REALTIME_PLOT_COLOR_BLACK:
				thePen.setColor(Qt::black);
				break;
			case JVX_REALTIME_PLOT_COLOR_BLUE:
				thePen.setColor(Qt::blue);
				break;
			case JVX_REALTIME_PLOT_COLOR_BROWN:
				thePen.setColor(QColor("brown"));
				break;
			case JVX_REALTIME_PLOT_COLOR_GREEN:
				thePen.setColor(Qt::green);
				break;
			case JVX_REALTIME_PLOT_COLOR_ORANGE:
				thePen.setColor(QColor("orange"));
				break;
			case JVX_REALTIME_PLOT_COLOR_PURPLE:
				thePen.setColor(QColor("purple"));
				break;
			case JVX_REALTIME_PLOT_COLOR_RED:
				thePen.setColor(Qt::red);
				break;
			case JVX_REALTIME_PLOT_COLOR_YELLOW:
				thePen.setColor(Qt::yellow);
				break;
			default: assert(0);
			}

			switch(theProp->width)
			{
			case JVX_REALTIME_PLOT_1PIXEL:
				thePen.setWidth(1);
				break;
			case JVX_REALTIME_PLOT_2PIXEL:
				thePen.setWidth(2);
				break;
			case JVX_REALTIME_PLOT_3PIXEL:
				thePen.setWidth(3);
				break;
			}

			switch(theProp->linestyle)
			{
			case JVX_REALTIME_PLOT_LINE_DOTTED:
				thePen.setStyle(Qt::DotLine);
				break;
			case JVX_REALTIME_PLOT_LINE_DASHED:
				thePen.setStyle(Qt::DashLine);
				break;
			case JVX_REALTIME_PLOT_LINE_DASH_DOTTED:
				thePen.setStyle(Qt::DashDotLine);
				break;
			case JVX_REALTIME_PLOT_LINE_SOLID:
				thePen.setStyle(Qt::SolidLine);
				break;
			}
			theCurve->setPen(thePen);
			theCurve->attach(qwtPlot);

			/* Draw a line for dummy show */
			jvxData fldy[2];
			jvxData fldx[2];
			for(j = 0; j < 2; j++)
			{
				fldy[j] = 0;
				fldx[j] = i;
			}
			theCurve->setSamples(fldx, fldy, 2);

			qwtPlot->setAxisScale(QwtPlot::xBottom, 0, 1);
			qwtPlot->setAxisScale(QwtPlot::yLeft, -1, +1);

			// Plot some dummy data to activate legend
			theProp->thePlotCurve = theCurve;

			// Restore propertries
			references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, i, theProp);
		}
	}

	if(selection.plotSelection >= num)
	{
		selection.plotSelection  = num -1;
	}
	if(JVX_CHECK_SIZE_SELECTED(selection.plotSelection))
	{
		newSelection_reg_plots((int)selection.plotSelection);
	}
	qwtPlot->replot();

}

/**
 * Function to indicate new choice in plot selection combo box
 *///=================================================================================
void
realtimeViewerOneLinearPlot::newSelection_reg_plots(int sel)
{
	jvxRealtimeViewerPropertyPlotItem* propsItem = NULL;
	jvxSize idxProp = 0;
	jvxComponentIdentification tp;
	std::string nm;

	// Set new selection
	selection.plotSelection = sel;

	// Find which property is associated to this selection
	references.theRef->_description_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, &idxProp, &tp, nm);

	// Set all user selection fields accordingly
	selection.description = nm;
	selection.tpV = tp;
	selection.y.inPropIdComponent = idxProp;
	jvxComponentIdentification tpS((jvxComponentType) selection.y.inPropIdComponent, 0, 0);
	jvx_qt_propertiesIndexShowForIndexComponent(selection.tpV, references.theHostRef, selection.y.inPropIdShow, tpS,JVX_PROPERTIES_FILTER_PLOT_FIELDS);

	// Also the specific data
	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle**>(&propsItem), NULL, NULL);
	selection.plotColorId = propsItem->color;
	selection.plotStyleId = propsItem->linestyle;
	selection.plotWidthId = propsItem->width;
	selection.x.inPropIdComponent = propsItem->idPropX;
	references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle*>(propsItem));

	// Update the property list ndex
	tpS = jvxComponentIdentification((jvxComponentType)selection.x.inPropIdComponent, 0, 0);
	jvx_qt_propertiesIndexShowForIndexComponent(tp, references.theHostRef, selection.x.inPropIdShow, tpS, JVX_PROPERTIES_FILTER_PLOT_FIELDS);

	updateWindow();
}

/**
 * Check box to indicate that plot is shown
 *///========================================================================================
void
realtimeViewerOneLinearPlot::checkedBox_show_plot(bool tog)
{
	jvxRealtimeViewerPropertyPlotItem* props = NULL;
	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle**>(&props), NULL, NULL);
	props->show = tog;
	references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, selection.plotSelection, reinterpret_cast<jvxHandle*>(props));
	updateWindow();
}

bool
realtimeViewerOneLinearPlot::addPlot_core()
{
	jvxSize i;
	bool plotAdded = false;

	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyPlotItem propsItem;
	jvx_propertyReferenceTriple theTriple;
	jvxCallManagerProperties callGate;

	static_initData(&propsItem, sizeof(jvxRealtimeViewerPropertyPlotItem), JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_ITEM);

	propsItem.color = selection.plotColorId;
	propsItem.width = selection.plotWidthId;
	propsItem.linestyle = selection.plotStyleId;
	propsItem.show = true;
	propsItem.idPropX = selection.x.inPropIdComponent;

	if(JVX_CHECK_SIZE_SELECTED(selection.y.inPropIdComponent))
	{
		jvx_initPropertyReferenceTriple(&theTriple);
		if(selection.tpV.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, selection.tpV);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			propsItem.descriptory.reset();
			jvx::propertyAddress::CjvxPropertyAddressLinear ident(selection.y.inPropIdComponent);

			res = theTriple.theProps->description_property(callGate, propsItem.descriptory, ident);
			bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_ID(selection.y.inPropIdComponent), theTriple.theProps);
			assert(rr);

			propsItem.numValues = propsItem.descriptory.num;

			if(JVX_CHECK_SIZE_SELECTED(propsItem.idPropX))
			{
				propsItem.descriptorx.reset();
				ident.idx = propsItem.idPropX;

				res = theTriple.theProps->description_property(callGate, propsItem.descriptorx, ident);
				rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_ID(propsItem.idPropX), theTriple.theProps);
				assert(rr);

				propsItem.numValues = JVX_MAX(propsItem.numValues, propsItem.descriptorx.num);
			}

			if(propsItem.numValues > 0)
			{
				JVX_SAFE_NEW_FLD(propsItem.dataX, jvxData, propsItem.numValues);
				JVX_SAFE_NEW_FLD(propsItem.dataY, jvxData, propsItem.numValues);

				for(i = 0; i < propsItem.numValues; i++)
				{
					propsItem.dataX[i] = i;
					propsItem.dataY[i] = 0.0;
				}
			}
			res = references.theRef->_insert_item_in_group_in_section(references.sectionId, references.groupId, selection.tpV, 
				selection.y.inPropIdComponent, selection.plotSelection, selection.description,
				&propsItem, sizeof(jvxRealtimeViewerPropertyPlotItem), JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_ITEM);
			if(res != JVX_NO_ERROR)
			{
				references.report->report_simple_text_message("Failed to call <_insert_item_in_group_in_section> in <realtimeViewerOneLinearPlot::buttonPushed_edit_plot>", JVX_REPORT_PRIORITY_ERROR);
			}
			else
			{
				plotAdded = true;
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

/**
 * Add new plot current selection
 *///========================================================================================
void
realtimeViewerOneLinearPlot::buttonPushed_add_plot()
{
	addPlot_core();

	updateWindow_redraw_plots();
	updateWindow();
}

/**
 * Button has been pushed to edit current plot
 *///==========================================================================================
void
realtimeViewerOneLinearPlot::buttonPushed_edit_plot()
{
	jvxErrorType res = JVX_NO_ERROR;

	buttonPushed_remove_plot();
	buttonPushed_add_plot();
}

/**
 * Button has been pushed to edit current plot
 *///==========================================================================================
void
realtimeViewerOneLinearPlot::buttonPushed_remove_plot()
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

void
realtimeViewerOneLinearPlot::checkedBox_autoscale_x_plot(bool tog)
{
	jvxRealtimeViewerPropertyPlotGroup* props = NULL;
	references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&props), NULL, NULL);
	props->autox = tog;
	references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(props));
	updateWindow();
}

void
realtimeViewerOneLinearPlot::newText_description_plot()
{
	this->selection.description = lineEdit_description->text().toLatin1().constData();
	updateWindow();
}

void
realtimeViewerOneLinearPlot::newSelection_component(int sel)
{
	if (sel < comboBox_components->count())
	{
		QVariant var1 = comboBox_components->itemData(sel, JVX_USER_ROLE_COMPONENT_ID);
		QVariant var2 = comboBox_components->itemData(sel, JVX_USER_ROLE_COMPONENT_SLOT_ID);
		QVariant var3 = comboBox_components->itemData(sel, JVX_USER_ROLE_COMPONENT_SLOTSUB_ID);
		if (
			(var1.isValid()) &&
			(var2.isValid()) &&
			(var3.isValid()))
		{
			this->selection.tpV.tp = (jvxComponentType)var1.toInt();
			this->selection.tpV.slotid = var2.toInt();
			this->selection.tpV.slotsubid = var3.toInt();
		}
	}
	//this->selection.tpV = tpAll[(jvxComponentType)sel];
	updateWindow();
}

void
realtimeViewerOneLinearPlot::newSelection_y_property(int sel)
{
	selection.y.inPropIdShow = sel;
	updateWindow();
}

void
realtimeViewerOneLinearPlot::newSelection_x_property(int sel)
{
	selection.x.inPropIdShow = sel;
	updateWindow();
}

void
realtimeViewerOneLinearPlot::newSelection_plot_color(int sel)
{
	selection.plotColorId = sel;
	updateWindow();
}

void
realtimeViewerOneLinearPlot::newSelection_plot_width(int sel)
{
	selection.plotWidthId = sel;
	updateWindow();
}

void
realtimeViewerOneLinearPlot::newSelection_plot_linestyle(int sel)
{
	selection.plotStyleId = sel;
	updateWindow();
}

void
realtimeViewerOneLinearPlot::newText_y_min()
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
realtimeViewerOneLinearPlot::newText_x_min()
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
realtimeViewerOneLinearPlot::newText_x_max()
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
realtimeViewerOneLinearPlot::newText_y_max()
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
realtimeViewerOneLinearPlot::checkedBox_autoscale_y_plot(bool tog)
{
	jvxRealtimeViewerPropertyPlotGroup* props = NULL;
	references.theRef->_request_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle**>(&props), NULL, NULL);
	props->autoy = tog;
	references.theRef->_return_property_group_in_section(references.sectionId, references.groupId, reinterpret_cast<jvxHandle*>(props));
	updateWindow();

}

jvxErrorType
realtimeViewerOneLinearPlot::callbackUpdateItem(jvxSize SectionId, jvxSize groupId, jvxSize itemId, jvxHandle* privateData)
{
	if(privateData)
	{
		return(((realtimeViewerOneLinearPlot*)privateData)->cbUpdateItem(SectionId, groupId, itemId));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);

}

jvxErrorType
realtimeViewerOneLinearPlot::callbackUpdateGroup(jvxSize SectionId, jvxSize groupId, jvxBool callOnStart, jvxHandle* privateData)
{
	if(privateData)
	{
		return(((realtimeViewerOneLinearPlot*)privateData)->cbUpdateGroup(SectionId, groupId, callOnStart));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
realtimeViewerOneLinearPlot::cbUpdateItem(jvxSize SectionId, jvxSize groupId, jvxSize itemId)
{
	jvxSize i;
	jvxSize idxProp = 0;
	jvxComponentIdentification tp;
	std::string nm;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyPlotItem* theProp = NULL;
	jvx_propertyReferenceTriple theTriple;
	jvx_initPropertyReferenceTriple(&theTriple);
	bool isValid = false;
	jvxCallManagerProperties callGate;

	if(
		(SectionId == references.sectionId) &&
		(groupId == references.groupId))
	{
		res = this->references.theRef->_description_item_in_group_in_section(SectionId, groupId, itemId, &idxProp, &tp, nm);

		if(res == JVX_NO_ERROR)
		{
			if(tp.tp != JVX_COMPONENT_UNKNOWN)
			{
				res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, tp);
			}

			if((res == JVX_NO_ERROR) && theTriple.theProps)
			{
				this->references.theRef->_request_property_item_in_group_in_section(SectionId, groupId, itemId, reinterpret_cast<jvxHandle**>(&theProp), NULL, NULL);
				if(theProp)
				{
					jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
					jvx::propertyAddress::CjvxPropertyAddressLinear ident(idxProp);
					jvx::propertyAddress::CjvxPropertyAddressGlobalId identIdx;
					jPD trans;

					jvxSize numUsed = 0;

					if (theTriple.theProps->description_property(callGate,theDescr, ident) == JVX_NO_ERROR)
					{
						numUsed = theDescr.num;
						assert(theDescr.format == JVX_DATAFORMAT_DATA);
						if (JVX_CHECK_SIZE_SELECTED(theProp->idPropX))
						{
							ident.idx = theProp->idPropX;
							if (theTriple.theProps->description_property(callGate, theDescr, ident) == JVX_NO_ERROR)
							{
								numUsed = JVX_MAX(numUsed, theDescr.num);
								assert(theDescr.format == JVX_DATAFORMAT_DATA);
							}
						}

						if (theProp->numValues != numUsed)
						{
							JVX_SAFE_DELETE_FLD(theProp->dataX, jvxData);
							JVX_SAFE_DELETE_FLD(theProp->dataY, jvxData);

							theProp->numValues = numUsed;
							if (theProp->numValues > 0)
							{
								JVX_SAFE_NEW_FLD(theProp->dataX, jvxData, theProp->numValues);
								JVX_SAFE_NEW_FLD(theProp->dataY, jvxData, theProp->numValues);

								for (i = 0; i < theProp->numValues; i++)
								{
									theProp->dataX[i] = i;
									theProp->dataY[i] = 0.0;
								}
							}
						}

						// ======================================================================================
						callGate.on_get.prop_access_type = &theProp->descriptory.accessType;
						identIdx.reset(theProp->descriptory.globalIdx,
							theProp->descriptory.category);
						trans.reset(true);
						resL = theTriple.theProps->get_property(callGate, jPRG( theProp->dataY, theProp->numValues, theProp->descriptory.format), 
							identIdx, trans);
						if (resL == JVX_NO_ERROR)
						{
							if (JVX_CHECK_SIZE_SELECTED(theProp->idPropX))
							{
								callGate.on_get.prop_access_type = &theProp->descriptorx.accessType;
								identIdx.reset(theProp->descriptorx.globalIdx,
									theProp->descriptorx.category);
								trans.reset(true);
								resL = theTriple.theProps->get_property(callGate, jPRG( theProp->dataX, theProp->numValues, theProp->descriptorx.format),
									ident, trans);
								if (resL == JVX_NO_ERROR)
								{
									comboBox_x_properties->setToolTip("ok");
								}
								else
								{
									std::string txt = "Failed to obtain data for x-axis, reason: <";
									txt += jvxErrorType_descr(resL);
									comboBox_x_properties->setToolTip(txt.c_str());
								}
							}

							if (theProp->numValues > 0)
							{
								if (theProp->thePlotCurve)
								{
									theProp->thePlotCurve->setSamples(theProp->dataX, theProp->dataY, JVX_SIZE_INT(theProp->numValues));
								}
							}
							for (i = 0; i < theProp->numValues; i++)
							{
								theProp->maxX = JVX_MAX(theProp->maxX, theProp->dataX[i]);
								theProp->maxY = JVX_MAX(theProp->maxY, theProp->dataY[i]);
								theProp->minX = JVX_MIN(theProp->minX, theProp->dataX[i]);
								theProp->minY = JVX_MIN(theProp->minY, theProp->dataY[i]);
								theProp->minmaxValid = true;
							}
							comboBox_y_properties->setToolTip("ok");
						}
						else
						{
							std::string txt = "Failed to obtain data for y-axis, reason: <";
							txt += jvxErrorType_descr(resL);
							comboBox_y_properties->setToolTip(txt.c_str());
						}
					} // if (theTriple.theProps->description_property
					this->references.theRef->_return_property_item_in_group_in_section(SectionId, groupId, itemId, reinterpret_cast<jvxHandle*>(theProp));
				}
				else
				{
					assert(0);
				}
			}
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
realtimeViewerOneLinearPlot::cbUpdateGroup(jvxSize SectionId, jvxSize groupId, jvxBool callOnStart)
{
	jvxRealtimeViewerPropertyPlotGroup* thePropG = NULL;
	jvxRealtimeViewerPropertyPlotItem* thePropI = NULL;
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

		/*
		if(
			(SectionId == references.sectionId) &&
			(groupId == references.groupId))
		{
			this->references.theRef->_request_property_group_in_section(SectionId, groupId, reinterpret_cast<jvxHandle**>(&thePropG), NULL, NULL);
			assert(thePropG);

			references.theRef->_number_items_in_group_in_section(SectionId, groupId, &num);
			for(i = 0; i < num; i++)
			{
				res = this->references.theRef->_description_item_in_group_in_section(SectionId, groupId, itemId, &idxProp, &tp, nm);

				if(res == JVX_NO_ERROR)
				{
					if(tp != JVX_COMPONENT_UNKNOWN)
					{
						res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, tp);
					}
					theTriple.theProps->freeze_properties(


				this->references.theRef->_request_property_item_in_group_in_section(SectionId, groupId, i, reinterpret_cast<jvxHandle**>(&thePropI), NULL, NULL);
				assert(thePropI);
				this->references.theRef->_return_property_item_in_group_in_section(SectionId, groupId, i, reinterpret_cast<jvxHandle*>(thePropI));
			}
		}
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

			if(thePropG->xmax < thePropG->xmin)
			{
				thePropG->xmax = JVX_EPSILON/2;
				thePropG->xmin = JVX_EPSILON/2;
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
realtimeViewerOneLinearPlot::newText_yaxistitle()
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
realtimeViewerOneLinearPlot::newText_xaxistitle()
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
realtimeViewerOneLinearPlot::updateAllPropertyDescriptors()
{
	jvxSize i,j;
	bool plotAdded = false;
	jvxSize num = 0;
	jvxSize inPropId;
	jvxComponentIdentification tp ;
	std::string nm;
	jvxCallManagerProperties callGate;
	jvxErrorType res = JVX_NO_ERROR;
	jvxRealtimeViewerPropertyPlotItem* propsItem = NULL;
	jvx_propertyReferenceTriple theTriple;

	this->references.theRef->_number_items_in_group_in_section(references.sectionId,
		references.groupId, &num);
	for(i = 0; i < num; i++)
	{
		res = this->references.theRef->_description_item_in_group_in_section(references.sectionId,
			references.groupId,i, &inPropId, &tp, nm);
		assert(res == JVX_NO_ERROR);

		res = this->references.theRef->_request_property_item_in_group_in_section(references.sectionId,
			references.groupId,i, reinterpret_cast<jvxHandle**>(&propsItem), NULL, NULL);
		assert(res == JVX_NO_ERROR);

		if(propsItem)
		{
			jvx_initPropertyReferenceTriple(&theTriple);
			if(tp.tp != JVX_COMPONENT_UNKNOWN)
			{
				res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, tp);
			}

			if((res == JVX_NO_ERROR) && theTriple.theProps)
			{
				assert(propsItem->dataX == NULL);
				assert(propsItem->dataY == NULL);

				jvx::propertyAddress::CjvxPropertyAddressLinear ident(inPropId);
				res = theTriple.theProps->description_property(callGate, propsItem->descriptory, ident);
				bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_ID(inPropId), theTriple.theProps);
				assert(rr);

				propsItem->numValues = propsItem->descriptory.num;

				if(JVX_CHECK_SIZE_SELECTED(propsItem->idPropX))
				{
					ident.idx = propsItem->idPropX;
					res = theTriple.theProps->description_property(callGate, propsItem->descriptorx, ident);
					bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_ID(propsItem->idPropX), theTriple.theProps);
					assert(rr);

					propsItem->numValues = JVX_MAX(propsItem->numValues, propsItem->descriptorx.num);
				}

				if(propsItem->numValues > 0)
				{
					JVX_SAFE_NEW_FLD(propsItem->dataX, jvxData, propsItem->numValues);
					JVX_SAFE_NEW_FLD(propsItem->dataY, jvxData, propsItem->numValues);

					for(j = 0; j < propsItem->numValues; j++)
					{
						propsItem->dataX[j] = j;
						propsItem->dataY[j] = 0.0;
					}
				}
				jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, tp);
			}
			this->references.theRef->_return_property_item_in_group_in_section(references.sectionId,
			references.groupId,i, reinterpret_cast<jvxHandle*>(propsItem));
		}
	}
}

void 
realtimeViewerOneLinearPlot::newSelection_context(int val)
{
	selection.ctxt = (jvxPropertyContext) val;
	this->updateWindow();
}

void 
	realtimeViewerOneLinearPlot::toggle_showDescriptors(bool val)
{
	selection.showDescriptors = val;
	this->updateWindow();
}
