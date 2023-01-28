#include "jvxQtSaLinearbufferViewer.h"
#include <QtWidgets/QSpacerItem>
#include "allHostsStatic_common.h"

jvxQtSaLinearbufferViewer::jvxQtSaLinearbufferViewer(QWidget* parent): QWidget(parent)
{
	myState = JVX_STATE_NONE;
	uiSetupComplete = false;
	myProperties.theProps = NULL;
	linkedProperties.id_xaxis_data = -1;
	linkedProperties.id_xaxis_title = -1;

	theCircHeader.fld = NULL;
	theCircHeader.isValid = false;
	theCircHeader.sz = 0;
}

jvxQtSaLinearbufferViewer::~jvxQtSaLinearbufferViewer()
{
}
 
jvxErrorType
	jvxQtSaLinearbufferViewer::init(jvxQtSaWidgetsConfigure::jvxQtWidgetsLinePlotConfigure* theViewerConfig)
{
	if(myState == JVX_STATE_NONE)
	{
		this->myConfig = *theViewerConfig;
		myState = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;

}

jvxErrorType
jvxQtSaLinearbufferViewer::associateProperties(configurePropertyInterface* thePropsConfig)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(myState == JVX_STATE_INIT)
	{
		this->myProperties = *thePropsConfig;
		jvxBool isValidPropRef = false;
		myProperties.theProps->reference_status(NULL, &isValidPropRef);
		if (!isValidPropRef)
		{
			std::cout << "Error: when associating widget " << this->objectName().toLatin1().data() << 
				" with property reference, the property reference is invalid!" << std::endl;
			return JVX_ERROR_INVALID_SETTING;
		}

		res = refresh_property_ids();

		updatePlotParameters(true);

		if(res == JVX_NO_ERROR)
		{
			myState = JVX_STATE_ACTIVE;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxErrorType
	jvxQtSaLinearbufferViewer::refresh_property_ids()
{
	jvxBool foundit = false;
	jvxBool isValid = true;
	jvxSize hdlIdx = 0;

	linkedProperties.id_circplot = -1;
	linkedProperties.id_command = -1;
	linkedProperties.id_num_in_channels = -1;
	linkedProperties.id_legend = -1;
	linkedProperties.id_circplot = -1;
	jvxCallManagerProperties callGate;

	jvxBool isValidPropRef = false;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (isValidPropRef)
	{
		// ===============================================================
		// ===============================================================
		foundit = false;
		hdlIdx = 0;
		isValid = false;
		FIND_PROPERTY_ID(myProperties.theProps, hdlIdx, myProperties.descr_circplot, isValid, foundit, callGate);
		if(foundit)
		{
			linkedProperties.id_circplot = (jvxInt32)hdlIdx;
		}
		else
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		// ===============================================================
		// ===============================================================
		foundit = false;
		hdlIdx = 0;
		isValid = false;
		FIND_PROPERTY_ID(myProperties.theProps, hdlIdx, myProperties.descr_legend, isValid, foundit, callGate);
		if(foundit && isValid)
		{
			linkedProperties.id_legend = (jvxInt32)hdlIdx;
		}
		else
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		foundit = false;
		hdlIdx = 0;
		isValid = false;
		FIND_PROPERTY_ID(myProperties.theProps, hdlIdx, myProperties.descr_num_in_channels, isValid, foundit, callGate);
		if(foundit && isValid)
		{
			linkedProperties.id_num_in_channels = (jvxInt32)hdlIdx;
		}
		else
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		foundit = false;
		hdlIdx = 0;
		isValid = false;
		FIND_PROPERTY_ID(myProperties.theProps, hdlIdx, myProperties.descr_command, isValid, foundit, callGate);

		if(foundit && isValid)
		{
			linkedProperties.id_command = (jvxInt32)hdlIdx;
		}
		else
		{
			return JVX_ERROR_INVALID_SETTING;
		}


		return JVX_NO_ERROR;
	}// if(res == JVX_NO_ERROR)
	return JVX_ERROR_UNEXPECTED;
}

jvxErrorType
	jvxQtSaLinearbufferViewer::deassociateProperties()
{
	if(myState == JVX_STATE_ACTIVE)
	{
		refresh_property_ids();
		myState = JVX_STATE_INIT;
		update_window();
		myProperties.theProps = NULL;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
	jvxQtSaLinearbufferViewer::terminate()
{
	if(myState == JVX_STATE_INIT)
	{
		myState = JVX_STATE_NONE;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ============================================================================
// ============================================================================

void
	jvxQtSaLinearbufferViewer::setupUi()
{
	jvxSize i;
	QPalette palette_white;
	QBrush brush_white(QColor(255, 255, 255, 255));
	QPalette palette_light;
	QBrush brush_light(QColor(240, 235, 225, 255));
	QPalette palette_gray;
	QBrush brush_gray(QColor(200, 200, 200, 200));
	QFont myFont;

	brush_white.setStyle(Qt::SolidPattern);
	palette_white.setBrush(QPalette::Active, QPalette::Base, brush_white);
	palette_white.setBrush(QPalette::Active, QPalette::Window, brush_white);
	palette_white.setBrush(QPalette::Inactive, QPalette::Base, brush_white);
	palette_white.setBrush(QPalette::Inactive, QPalette::Window, brush_white);
	palette_white.setBrush(QPalette::Disabled, QPalette::Base, brush_white);
	palette_white.setBrush(QPalette::Disabled, QPalette::Window, brush_white);

	brush_light.setStyle(Qt::SolidPattern);
	palette_light.setBrush(QPalette::Active, QPalette::Base, brush_light);
	palette_light.setBrush(QPalette::Active, QPalette::Window, brush_light);
	palette_light.setBrush(QPalette::Inactive, QPalette::Base, brush_light);
	palette_light.setBrush(QPalette::Inactive, QPalette::Window, brush_light);
	palette_light.setBrush(QPalette::Disabled, QPalette::Base, brush_light);
	palette_light.setBrush(QPalette::Disabled, QPalette::Window, brush_light);

	brush_gray.setStyle(Qt::SolidPattern);
	palette_gray.setBrush(QPalette::Active, QPalette::Base, brush_gray);
	palette_gray.setBrush(QPalette::Active, QPalette::Window, brush_gray);
	palette_gray.setBrush(QPalette::Inactive, QPalette::Base, brush_gray);
	palette_gray.setBrush(QPalette::Inactive, QPalette::Window, brush_gray);
	palette_gray.setBrush(QPalette::Disabled, QPalette::Base, brush_gray);
	palette_gray.setBrush(QPalette::Disabled, QPalette::Window, brush_gray);

	allContentsLayout = new QHBoxLayout(this);

	framePlots = new QFrame(this);
	framePlots->setPalette(palette_light);
	framePlots->setAutoFillBackground(true);
	framePlots->setFrameShape(QFrame::StyledPanel);
	framePlots->setFrameShadow(QFrame::Raised);

	vertLayoutPlots = new QVBoxLayout(framePlots);

	QwtText txt;
	title_xdata = myConfig.textX;
	
	thePlots.ui_elements.oneFrame = new QFrame(framePlots);
	thePlots.ui_elements.oneFrame->setPalette(palette_white);
	thePlots.ui_elements.oneFrame->setAutoFillBackground(true);
	thePlots.ui_elements.oneFrame->setFrameShape(QFrame::StyledPanel);
	thePlots.ui_elements.oneFrame->setFrameShadow(QFrame::Raised);

	gridLayout = new QGridLayout(thePlots.ui_elements.oneFrame);
	gridLayout->setSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);

	thePlots.ui_elements.le_ymax = new QLineEdit(thePlots.ui_elements.oneFrame);
	thePlots.ui_elements.le_ymax->setMaximumSize((int)myConfig.scale_maxw, (int)myConfig.scale_maxh);
	thePlots.ui_elements.le_ymax->setAlignment(Qt::AlignCenter);
	myFont = thePlots.ui_elements.le_ymax->font();
	myFont.setPixelSize((int)myConfig.fontPixelSize);
	thePlots.ui_elements.le_ymax->setFont(myFont);
	gridLayout->addWidget(thePlots.ui_elements.le_ymax, 0, 0, 1, 1);

	gridLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 1, 1, 1);

	thePlots.ui_elements.label_title = new QLabel(thePlots.ui_elements.oneFrame);
	thePlots.ui_elements.label_title->setText(myConfig.theTitle.c_str());
	thePlots.ui_elements.label_title->setAlignment(Qt::AlignCenter);
	myFont = thePlots.ui_elements.label_title->font();
	myFont.setPixelSize((int)myConfig.fontPixelSize);
	myFont.setItalic(true);
	thePlots.ui_elements.label_title->setFont(myFont);
	gridLayout->addWidget(thePlots.ui_elements.label_title, 0, 2, 1, 1);

	gridLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 3, 1, 1);

	thePlots.ui_elements.update = new QCheckBox(thePlots.ui_elements.oneFrame);
	thePlots.ui_elements.update->setText("Update");
	myFont = thePlots.ui_elements.update->font();
	myFont.setPixelSize((int)myConfig.fontPixelSize);
	thePlots.ui_elements.update->setFont(myFont);
	gridLayout->addWidget(thePlots.ui_elements.update,0, 4, 1, 1);

	gridLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 5, 1, 1);

	thePlots.ui_elements.auto_scale = new QCheckBox(thePlots.ui_elements.oneFrame);
	thePlots.ui_elements.auto_scale->setText("Auto Scale");
	myFont = thePlots.ui_elements.auto_scale->font();
	myFont.setPixelSize((int)myConfig.fontPixelSize);
	thePlots.ui_elements.auto_scale->setFont(myFont);
	gridLayout->addWidget(thePlots.ui_elements.auto_scale,0, 6, 1, 1);

	thePlots.ui_elements.thePlot = new QwtPlot(thePlots.ui_elements.oneFrame);
	thePlots.ui_elements.thePlot->setMaximumSize(QSize(16777215, (int)myConfig.curveHeight));

	gridLayout->addWidget(thePlots.ui_elements.thePlot, 1, 0, 1, 7);

	thePlots.ui_elements.le_ymin = new QLineEdit(thePlots.ui_elements.oneFrame);
	thePlots.ui_elements.le_ymin->setMaximumSize((int)myConfig.scale_maxw, (int)myConfig.scale_maxh);
	thePlots.ui_elements.le_ymin->setAlignment(Qt::AlignCenter);
	myFont = thePlots.ui_elements.le_ymin->font();
	myFont.setPixelSize((int)myConfig.fontPixelSize);
	thePlots.ui_elements.le_ymin->setFont(myFont);
	gridLayout->addWidget(thePlots.ui_elements.le_ymin, 2, 0, 1, 1);

	thePlots.ui_elements.le_xmin = new QLineEdit(thePlots.ui_elements.oneFrame);
	thePlots.ui_elements.le_xmin->setMaximumSize((int)myConfig.scale_maxw, (int)myConfig.scale_maxh);
	thePlots.ui_elements.le_xmin->setAlignment(Qt::AlignCenter);
	myFont = thePlots.ui_elements.le_xmin->font();
	myFont.setPixelSize((int)myConfig.fontPixelSize);
	thePlots.ui_elements.le_xmin->setFont(myFont);
	gridLayout->addWidget(thePlots.ui_elements.le_xmin, 2, 1, 1, 1);

	thePlots.ui_elements.le_xmax = new QLineEdit(thePlots.ui_elements.oneFrame);
	thePlots.ui_elements.le_xmax->setMaximumSize((int)myConfig.scale_maxw, (int)myConfig.scale_maxh);
	thePlots.ui_elements.le_xmax->setAlignment(Qt::AlignCenter);
	myFont = thePlots.ui_elements.le_xmax->font();
	myFont.setPixelSize((int)myConfig.fontPixelSize);
	thePlots.ui_elements.le_xmax->setFont(myFont);
	gridLayout->addWidget(thePlots.ui_elements.le_xmax, 2, 6, 1, 1);

	gridLayout->setColumnStretch(0,1);
	gridLayout->setColumnStretch(1,1);
	gridLayout->setColumnStretch(2,10);
	gridLayout->setColumnStretch(3,10);
	gridLayout->setColumnStretch(4,10);
	gridLayout->setColumnStretch(5,1);

	thePlots.ui_elements.oneFrame->setLayout(gridLayout);

	vertLayoutPlots->addWidget(thePlots.ui_elements.oneFrame);

	thePlots.ui_elements.thePlot->setCanvasBackground( QColor( Qt::white ) );

	QFont font = thePlots.ui_elements.thePlot->axisFont(QwtPlot::xBottom);
	font.setPixelSize((int)myConfig.fontPixelSize);
	thePlots.ui_elements.thePlot->setAxisFont (QwtPlot::xBottom, font);
	thePlots.ui_elements.thePlot->setAxisFont (QwtPlot::yLeft, font);

	font.setPixelSize((int)myConfig.axisPlotSize);
	txt.setFont(font);
	txt.setText(title_xdata.c_str());
	thePlots.ui_elements.thePlot->setAxisTitle( QwtPlot::xBottom, txt);
	txt.setText(myConfig.textY.c_str());
	thePlots.ui_elements.thePlot->setAxisTitle( QwtPlot::yLeft,txt );

	thePlots.ui_elements.thePlot->setAutoDelete(false);

	// Setup a grid
	thePlots.ui_elements.theGrid = new QwtPlotGrid;
	thePlots.ui_elements.theGrid->setMajorPen( QPen( Qt::gray, 1, Qt::DotLine ) );
	thePlots.ui_elements.theGrid->setMinorPen( QPen( Qt::gray, 1 , Qt::DotLine ) );
	thePlots.ui_elements.theGrid->attach( thePlots.ui_elements.thePlot );

	thePlots.ui_elements.theLegend = new QwtLegend;
	myFont = thePlots.ui_elements.theLegend->font();
	myFont.setPixelSize((int)myConfig.fontPixelSize);
	thePlots.ui_elements.theLegend->setFont(myFont);

	thePlots.ui_elements.thePlot->insertLegend(thePlots.ui_elements.theLegend,  QwtPlot::BottomLegend );

	JVX_SAFE_NEW_FLD(thePlots.ui_elements.theCurves, QwtPlotCurve*, myConfig.numPlots);
	JVX_SAFE_NEW_FLD(thePlots.plotY, jvxData*, myConfig.numPlots);
	for(i = 0; i < myConfig.numPlots; i++)
	{
		thePlots.ui_elements.theCurves[i] = new QwtPlotCurve(); //theCurves
		thePlots.ui_elements.theCurves[i]->setTitle(("signal #" + jvx_size2String(i)).c_str());

		if(myConfig.numPixelsPlot > 0)
		{
			JVX_SAFE_NEW_FLD(thePlots.plotY[i], jvxData, myConfig.numPixelsPlot);
			memset(thePlots.plotY[i], 0, sizeof(jvxData)*myConfig.numPixelsPlot);
		}

		QPen thePen;

		thePen.setColor(Qt::blue);

		thePen.setWidth((int)myConfig.plotPixelsize);

		thePen.setStyle(Qt::SolidLine);

		thePlots.ui_elements.theCurves[i]->setPen(thePen);
		thePlots.ui_elements.theCurves[i]->attach(thePlots.ui_elements.thePlot);
	}

	thePlots.ymin = -1.0;
	thePlots.ymax = 1.0;

	thePlots.xmin = 0;
	thePlots.xmax = 0;

	thePlots.auto_scale = false;
	thePlots.update = true;

	// Connect all slots with signals
	connect(thePlots.ui_elements.auto_scale, SIGNAL(clicked(bool)), this, SLOT(auto_scale_clicked(bool)));
	connect(thePlots.ui_elements.update, SIGNAL(clicked(bool)), this, SLOT(update_clicked(bool)));

	connect(thePlots.ui_elements.le_ymin, SIGNAL(editingFinished()), this, SLOT(ymin_edit()));
	connect(thePlots.ui_elements.le_ymax, SIGNAL(editingFinished()), this, SLOT(ymax_edit()));
	connect(thePlots.ui_elements.le_xmin, SIGNAL(editingFinished()), this, SLOT(xmin_edit()));
	connect(thePlots.ui_elements.le_xmax, SIGNAL(editingFinished()), this, SLOT(xmax_edit()));

	plotX = NULL;
	if(myConfig.numPixelsPlot > 0)
	{
		JVX_SAFE_NEW_FLD(plotX, jvxData, myConfig.numPixelsPlot);
		memset(plotX, 0, sizeof(jvxData)*myConfig.numPixelsPlot);

		updatePlotParameters(true);
	}

	framePlots->setLayout(vertLayoutPlots);
	allContentsLayout->addWidget(framePlots, 100);

	// ================================================================

	// ================================================================

	this->setLayout(allContentsLayout);
	
	uiSetupComplete = true;
	update_window();
}

jvxErrorType
	jvxQtSaLinearbufferViewer::startPlotting()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxInt32 valI32;

	jvxBool foundit = false;
	jvxBool isValid = true;
	jvxSize hdlIdx = 0;

	linkedProperties.id_xaxis_data = -1;
	linkedProperties.id_xaxis_title = -1;

	jvxBool isValidPropRef = false;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (isValidPropRef)
	{
		ident.reset(linkedProperties.id_num_in_channels, JVX_PROPERTY_CATEGORY_UNSPECIFIC);
		trans.reset(true);
		res = myProperties.theProps->get_property(callGate, jPRG(
			&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans );
		if(res == JVX_NO_ERROR)
		{

			theCircHeader.fld = jvx_allocate1DCircExternalBuffer(myConfig.numPixelsPlot, valI32,
				JVX_DATAFORMAT_DATA, JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED, jvx_static_lock, 
				jvx_static_try_lock, jvx_static_unlock, &theCircHeader.sz);
			theCircHeader.isValid = true;

			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(theCircHeader.fld, 
				&theCircHeader.isValid, 
				JVX_DATAFORMAT_DATA);
			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(linkedProperties.id_circplot, 
				JVX_PROPERTY_CATEGORY_UNSPECIFIC);

			res = myProperties.theProps->install_referene_property(callGate, ptrRaw, ident);
		}

		// ===============================================================
		// Obtain valid property ids
		// ===============================================================
		foundit = false;
		hdlIdx = 0;
		isValid = false;
		FIND_PROPERTY_ID(myProperties.theProps, hdlIdx, myProperties.descr_xaxis_data, isValid, foundit, callGate);
		if(foundit)
		{
			linkedProperties.id_xaxis_data = (jvxInt32)hdlIdx;
		}
		

		foundit = false;
		hdlIdx = 0;
		isValid = false;
		FIND_PROPERTY_ID(myProperties.theProps, hdlIdx, myProperties.descr_xaxis_title, isValid, foundit, callGate);
		if(foundit)
		{
			linkedProperties.id_xaxis_title = (jvxInt32)hdlIdx;
		}
		
		updatePlotParameters(true);
	}

	return res;
}

jvxErrorType
	jvxQtSaLinearbufferViewer::stopPlotting()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxBool isValidPropRef = false;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (isValidPropRef)
	{
		if(theCircHeader.isValid)
		{
			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(theCircHeader.fld,
				&theCircHeader.isValid,
				JVX_DATAFORMAT_DATA);
			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(linkedProperties.id_circplot, JVX_PROPERTY_CATEGORY_UNSPECIFIC);

			res = myProperties.theProps->uninstall_referene_property(callGate, ptrRaw, ident);
			theCircHeader.isValid = false;
			jvx_deallocateExternalBuffer(theCircHeader.fld);
			theCircHeader.fld = NULL;
			theCircHeader.sz = 0;
		}
		linkedProperties.id_xaxis_data = -1;
		linkedProperties.id_xaxis_title = -1;
	}
	updatePlotParameters(true);

	return res;
}

// ============================================================================
// ============================================================================

void
	jvxQtSaLinearbufferViewer::update_window()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	jvxSelectionList selList;


	if(!uiSetupComplete)
		return;

	for(i = 0; i < myConfig.numPlots; i++)
	{
		thePlots.ui_elements.le_ymin->setText(jvx_data2String(thePlots.ymin,1).c_str());
		thePlots.ui_elements.le_ymax->setText(jvx_data2String(thePlots.ymax,1).c_str());
		thePlots.ui_elements.le_xmin->setText(jvx_data2String(thePlots.xmin,1).c_str());
		thePlots.ui_elements.le_xmax->setText(jvx_data2String(thePlots.xmax,1).c_str());
		thePlots.ui_elements.update->setChecked(thePlots.update);
		thePlots.ui_elements.auto_scale->setChecked(thePlots.auto_scale);
	}

	if(!myProperties.theProps)
		return;
}

void
	jvxQtSaLinearbufferViewer::update_window_periodic()
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxSelectionList selList;

	jvxSize i,j;
	jvxByte* basePtr;

	jvxData local_min = JVX_DATA_MAX_POS;
	jvxData local_max = JVX_DATA_MAX_NEG;

	if(theCircHeader.fld)
	{
		theCircHeader.fld->safe_access.lockf(theCircHeader.fld->safe_access.priv);

		basePtr = (jvxByte*)theCircHeader.fld->ptrFld;

		for(i = 0; i < myConfig.numPlots; i++)
		{
			jvxData* in = (jvxData*)basePtr;
			in += i * theCircHeader.fld->length;
			memcpy(thePlots.plotY[i], in, theCircHeader.fld->length*sizeof(jvxData));
		}
		theCircHeader.fld->safe_access.unlockf(theCircHeader.fld->safe_access.priv);

		if(thePlots.update)
		{
			if(thePlots.auto_scale)
			{
				local_min = JVX_DATA_MAX_POS;
				local_max = JVX_DATA_MAX_NEG;
				for(i = 0; i < myConfig.numPlots; i++)
				{
					for(j = 0; j < myConfig.numPixelsPlot; j++)
					{
						if(thePlots.plotY[i][j] <= local_min)
						{
							local_min = thePlots.plotY[i][j];
						}

						if(thePlots.plotY[i][j] >= local_max)
						{
							local_max = thePlots.plotY[i][j];
						}
					}
					thePlots.ymin = local_min;
					thePlots.ymax = local_max;
					thePlots.ui_elements.le_ymin->setText(jvx_data2String(thePlots.ymin, 2).c_str());
					thePlots.ui_elements.le_ymax->setText(jvx_data2String(thePlots.ymax, 2).c_str());
					thePlots.ui_elements.le_xmin->setText(jvx_data2String(thePlots.xmin, 2).c_str());
					thePlots.ui_elements.le_xmax->setText(jvx_data2String(thePlots.xmax, 2).c_str());

					// Maybe in autoscale mode?
					// allPlots[i].xmin = plotX[0];
					//allPlots[i].xmax = plotX[myConfig.numPixelsPlot - 1];
				}
			}

			for(i = 0; i < myConfig.numPlots; i++)
			{
				thePlots.ui_elements.theCurves[i]->setSamples(plotX, thePlots.plotY[i], (int)myConfig.numPixelsPlot);
			}
			thePlots.ui_elements.thePlot->setAxisScale(QwtPlot::yLeft, thePlots.ymin, thePlots.ymax);
			thePlots.ui_elements.thePlot->setAxisScale(QwtPlot::xBottom, thePlots.xmin, thePlots.xmax);

			thePlots.ui_elements.thePlot->replot();

		}

		//	if(jvx_isValidPropertyReferenceTriple(&properties.theTripleNode))
		//	{
		//	}
	}
}

// ============================================================================
// ============================================================================

void
	jvxQtSaLinearbufferViewer::auto_scale_clicked(bool click)
{
	QVariant line;
	thePlots.auto_scale = click;
	this->update_window();
}

void
	jvxQtSaLinearbufferViewer::update_clicked(bool click)
{
	QVariant line;
	thePlots.update = click;
	this->update_window();
}

void
jvxQtSaLinearbufferViewer::ymin_edit()
{
	QVariant line;
	jvxSize lineId= 0;
	QString tt;

	tt = thePlots.ui_elements.le_ymin->text();
	thePlots.ymin = tt.toDouble();
	thePlots.ymax =  JVX_MAX(thePlots.ymax, (thePlots.ymin + myConfig.epsyplot));
	updatePlotParameters(false);
	this->update_window();
}

void
	jvxQtSaLinearbufferViewer::ymax_edit()
{
	QVariant line;
	jvxSize lineId= 0;
	QString tt;
	tt = thePlots.ui_elements.le_ymax->text();
	thePlots.ymax = tt.toDouble();
	thePlots.ymin = JVX_MIN(thePlots.ymin, (thePlots.ymax - myConfig.epsyplot));
	updatePlotParameters(false);
	this->update_window();
}


void
	jvxQtSaLinearbufferViewer::xmin_edit()
{
	QVariant line;
	jvxSize lineId= 0;
	QString tt;
	tt = thePlots.ui_elements.le_xmin->text();
	thePlots.xmin = tt.toDouble();
	thePlots.xmax =  JVX_MAX(thePlots.xmax, (thePlots.xmin + myConfig.epsyplot));
	updatePlotParameters(false);
	this->update_window();
}

void
jvxQtSaLinearbufferViewer::xmax_edit()
{
	QVariant line;
	jvxSize lineId= 0;
	QString tt;
	tt = thePlots.ui_elements.le_xmax->text();
	thePlots.xmax = tt.toDouble();
	thePlots.xmin = JVX_MIN(thePlots.xmin, (thePlots.xmax - myConfig.epsyplot));
	updatePlotParameters(false);
	this->update_window();
}


void
jvxQtSaLinearbufferViewer::updatePlotParameters(jvxBool refreshValues)
{
	jvxApiString fldStr;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	if(refreshValues)
	{
		if(linkedProperties.id_xaxis_data >= 0)
		{
			ident.reset(linkedProperties.id_xaxis_data,
				JVX_PROPERTY_CATEGORY_UNSPECIFIC);
			trans.reset(true);
			res = myProperties.theProps->get_property(callGate, jPRG(plotX, myConfig.numPixelsPlot, JVX_DATAFORMAT_DATA), ident, trans);
			if(res != JVX_NO_ERROR)
			{
				for(i = 0; i < myConfig.numPixelsPlot; i++)
				{
					plotX[i] = (jvxData) i;
				}
			}

			for(i = 0; i < myConfig.numPlots; i++)
			{
				thePlots.xmin = plotX[0];
				thePlots.xmax = plotX[myConfig.numPixelsPlot-1];
			}
		}

		if(linkedProperties.id_xaxis_title >= 0)
		{
			ident.reset(linkedProperties.id_xaxis_title,
				JVX_PROPERTY_CATEGORY_UNSPECIFIC);
			trans.reset();
			res = myProperties.theProps->get_property(callGate, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
			
			QwtText txt;
			
			if(res == JVX_NO_ERROR)
			{
				title_xdata = fldStr.std_str();
			}

			QFont font = thePlots.ui_elements.thePlot->axisFont(QwtPlot::xBottom);
			font.setPixelSize((int)myConfig.fontPixelSize);
			thePlots.ui_elements.thePlot->setAxisFont (QwtPlot::xBottom, font);

			txt.setFont(font);
			txt.setText(title_xdata.c_str());

			thePlots.ui_elements.thePlot->setAxisTitle( QwtPlot::xBottom, txt);
		}
	}
	for(i = 0; i < myConfig.numPlots; i++)
	{

		thePlots.ui_elements.theCurves[i]->setSamples(plotX, thePlots.plotY[i], (int)myConfig.numPixelsPlot);
	}

	thePlots.ui_elements.thePlot->setAxisScale(QwtPlot::xBottom, thePlots.xmin, thePlots.xmax);
	thePlots.ui_elements.thePlot->setAxisScale(QwtPlot::yLeft, thePlots.ymin, thePlots.ymax);
	thePlots.ui_elements.thePlot->replot();
}
