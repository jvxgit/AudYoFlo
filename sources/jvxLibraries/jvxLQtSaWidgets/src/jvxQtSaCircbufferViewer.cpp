#include "jvxQtSaCircbufferViewer.h" 
#include <QtWidgets/QSpacerItem>
#include "allHostsStatic_common.h"

jvxQtSaCircbufferViewer::jvxQtSaCircbufferViewer(QWidget* parent): QWidget(parent)
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

jvxQtSaCircbufferViewer::~jvxQtSaCircbufferViewer()
{
} 

jvxErrorType
	jvxQtSaCircbufferViewer::init(jvxQtSaWidgetsConfigure::jvxQtWidgetsLinePlotConfigure* theViewerConfig)
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
	jvxQtSaCircbufferViewer::associateProperties(configurePropertyInterface* thePropsConfig)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(myState == JVX_STATE_INIT)
	{
		this->myProperties = *thePropsConfig;

		jvxBool isValidPropRef = false;
		myProperties.theProps->reference_status(NULL, &isValidPropRef);
		if(!isValidPropRef)
		{
			std::cout << "Error: when associating widget " << this->objectName().toLatin1().data() << 
				" with property reference, the property reference is invalid!" << std::endl;
			return JVX_ERROR_INVALID_SETTING;
		}

		res = refresh_property_ids();

		res = read_properties_shortcut();

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
	jvxQtSaCircbufferViewer::read_properties_shortcut()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selList;
	jvxBool isValidPropRef = false;
	jvxCallManagerProperties callGate;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (isValidPropRef)
	{
		if(linkedProperties.id_plot_mode >= 0)
		{
			ident.reset(linkedProperties.id_plot_mode,
				JVX_PROPERTY_CATEGORY_UNSPECIFIC);
			trans.reset(true);

			res = myProperties.theProps->get_property(callGate, 
				jPRG(&selList, 1,JVX_DATAFORMAT_SELECTION_LIST), 
				ident, trans);
			if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(linkedProperties.id_plot_mode, JVX_PROPERTY_CATEGORY_UNSPECIFIC),
				myProperties.theProps))
			{
				myDataCollectionMode = (jvxCircBufferDataCollectorMode)jvx_bitFieldValue32(selList.bitFieldSelected());
				return JVX_NO_ERROR;
			}
		}
	}
	return JVX_ERROR_UNEXPECTED;
}

jvxErrorType
	jvxQtSaCircbufferViewer::refresh_property_ids()
{
	jvxBool foundit = false;
	jvxBool isValid = true;
	jvxSize hdlIdx = 0;
	jvxCallManagerProperties callGate;

	linkedProperties.id_circplot = -1;
	linkedProperties.id_command = -1;
	linkedProperties.id_line_trigger = -1;
	linkedProperties.id_num_in_channels = -1;
	linkedProperties.id_plot_mode = -1;
	linkedProperties.id_showFactor = -1;
	linkedProperties.id_trigger_amplitude = -1;
	linkedProperties.id_plot_behavior_type = -1;

	jvxBool isValidPropRef = false;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (!isValidPropRef)
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
		FIND_PROPERTY_ID(myProperties.theProps, hdlIdx, myProperties.descr_showFactor, isValid, foundit, callGate);
		if(foundit && isValid)
		{
			linkedProperties.id_showFactor = (jvxInt32)hdlIdx;
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
		FIND_PROPERTY_ID(myProperties.theProps, hdlIdx, myProperties.descr_plot_mode, isValid, foundit, callGate);
		if(foundit && isValid)
		{
			linkedProperties.id_plot_mode = (jvxInt32)hdlIdx;
		}
		else
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		foundit = false;
		hdlIdx = 0;
		isValid = false;
		FIND_PROPERTY_ID(myProperties.theProps, hdlIdx, myProperties.descr_line_trigger, isValid, foundit, callGate);
		if(foundit && isValid)
		{
			linkedProperties.id_line_trigger = (jvxInt32)hdlIdx;
		}
		else
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		foundit = false;
		hdlIdx = 0;
		isValid = false;
		FIND_PROPERTY_ID(myProperties.theProps, hdlIdx, myProperties.descr_trigger_amplitude, isValid, foundit, callGate);

		if(foundit && isValid)
		{
			linkedProperties.id_trigger_amplitude = (jvxInt32)hdlIdx;
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
	jvxQtSaCircbufferViewer::deassociateProperties()
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
	jvxQtSaCircbufferViewer::terminate()
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
	jvxQtSaCircbufferViewer::setupUi()
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

	// Create all subwidgets
	scrollArea = new QScrollArea(this);
	scrollArea->setPalette(palette_white);
	scrollArea->setAutoFillBackground(true);
	scrollArea->setWidgetResizable(true);
	scrollAreaWidgetContents = new QWidget();
	//   scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1691, 1129));

	allContentsLayout = new QHBoxLayout(scrollAreaWidgetContents);

	framePlots = new QFrame(scrollAreaWidgetContents);
	framePlots->setPalette(palette_light);
	framePlots->setAutoFillBackground(true);
	framePlots->setFrameShape(QFrame::StyledPanel);
	framePlots->setFrameShadow(QFrame::Raised);

	vertLayoutPlots = new QVBoxLayout(framePlots);

	JVX_SAFE_NEW_FLD(allPlots, onePlotLane, myConfig.numPlots);

	QwtText txt;
	title_xdata = myConfig.textX;

	for(i = 0; i < myConfig.numPlots; i++)
	{
		allPlots[i].ui_elements.oneFrame = new QFrame(framePlots);
		allPlots[i].ui_elements.oneFrame->setPalette(palette_white);
		allPlots[i].ui_elements.oneFrame->setAutoFillBackground(true);
		allPlots[i].ui_elements.oneFrame->setFrameShape(QFrame::StyledPanel);
		allPlots[i].ui_elements.oneFrame->setFrameShadow(QFrame::Raised);

		gridLayout = new QGridLayout(allPlots[i].ui_elements.oneFrame);
		gridLayout->setSpacing(0);
		gridLayout->setContentsMargins(0, 0, 0, 0);

		allPlots[i].ui_elements.le_ymax = new QLineEdit(allPlots[i].ui_elements.oneFrame);
		allPlots[i].ui_elements.le_ymax->setMaximumSize((int)myConfig.scale_maxw, (int)myConfig.scale_maxh);
		allPlots[i].ui_elements.le_ymax->setAlignment(Qt::AlignCenter);

		myFont = allPlots[i].ui_elements.le_ymax->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		allPlots[i].ui_elements.le_ymax->setFont(myFont);
		gridLayout->addWidget(allPlots[i].ui_elements.le_ymax, 0, 0, 1, 1);

		gridLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 1, 1, 1);

		allPlots[i].ui_elements.label_title = new QLabel(allPlots[i].ui_elements.oneFrame);
		allPlots[i].ui_elements.label_title->setText((myConfig.theTitle + " [" + jvx_size2String(i) + "]").c_str());
		allPlots[i].ui_elements.label_title->setAlignment(Qt::AlignCenter);
		myFont = allPlots[i].ui_elements.label_title->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		myFont.setItalic(true);
		allPlots[i].ui_elements.label_title->setFont(myFont);
		gridLayout->addWidget(allPlots[i].ui_elements.label_title, 0, 2, 1, 1);

		gridLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 3, 1, 1);

		allPlots[i].ui_elements.update = new QCheckBox(allPlots[i].ui_elements.oneFrame);
		allPlots[i].ui_elements.update->setText("Update");
		myFont = allPlots[i].ui_elements.update->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		allPlots[i].ui_elements.update->setFont(myFont);
		gridLayout->addWidget(allPlots[i].ui_elements.update,0, 4, 1, 1);

		gridLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 5, 1, 1);

		allPlots[i].ui_elements.auto_scale = new QCheckBox(allPlots[i].ui_elements.oneFrame);
		allPlots[i].ui_elements.auto_scale->setText("Auto Scale");
		myFont = allPlots[i].ui_elements.auto_scale->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		allPlots[i].ui_elements.auto_scale->setFont(myFont);
		gridLayout->addWidget(allPlots[i].ui_elements.auto_scale,0, 6, 1, 1);

		allPlots[i].ui_elements.thePlot = new QwtPlot(allPlots[i].ui_elements.oneFrame);
		allPlots[i].ui_elements.thePlot->setMaximumSize(QSize(16777215, (int)myConfig.curveHeight));

		gridLayout->addWidget(allPlots[i].ui_elements.thePlot, 1, 0, 1, 7);

		allPlots[i].ui_elements.le_ymin = new QLineEdit(allPlots[i].ui_elements.oneFrame);
		allPlots[i].ui_elements.le_ymin->setMaximumSize((int)myConfig.scale_maxw, (int)myConfig.scale_maxh);
		allPlots[i].ui_elements.le_ymin->setAlignment(Qt::AlignCenter);

		myFont = allPlots[i].ui_elements.le_ymin->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		allPlots[i].ui_elements.le_ymin->setFont(myFont);
		gridLayout->addWidget(allPlots[i].ui_elements.le_ymin, 2, 0, 1, 1);

		allPlots[i].ui_elements.le_xmin = new QLineEdit(allPlots[i].ui_elements.oneFrame);
		allPlots[i].ui_elements.le_xmin->setMaximumSize((int)myConfig.scale_maxw, (int)myConfig.scale_maxh);
		allPlots[i].ui_elements.le_xmin->setAlignment(Qt::AlignCenter);
		myFont = allPlots[i].ui_elements.le_xmin->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		allPlots[i].ui_elements.le_xmin->setFont(myFont);
		gridLayout->addWidget(allPlots[i].ui_elements.le_xmin, 2, 1, 1, 1);

		allPlots[i].ui_elements.le_xmax = new QLineEdit(allPlots[i].ui_elements.oneFrame);
		allPlots[i].ui_elements.le_xmax->setMaximumSize((int)myConfig.scale_maxw, (int)myConfig.scale_maxh);
		allPlots[i].ui_elements.le_xmax->setAlignment(Qt::AlignCenter);

		myFont = allPlots[i].ui_elements.le_xmax->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		allPlots[i].ui_elements.le_xmax->setFont(myFont);
		gridLayout->addWidget(allPlots[i].ui_elements.le_xmax, 2, 6, 1, 1);

		gridLayout->setColumnStretch(0,1);
		gridLayout->setColumnStretch(1,1);
		gridLayout->setColumnStretch(2,10);
		gridLayout->setColumnStretch(3,10);
		gridLayout->setColumnStretch(4,10);
		gridLayout->setColumnStretch(5,1);

		allPlots[i].ui_elements.oneFrame->setLayout(gridLayout);

		vertLayoutPlots->addWidget(allPlots[i].ui_elements.oneFrame);

		// Link the lane number to the objects
		allPlots[i].ui_elements.auto_scale->setProperty("ID_IDENTIFY_PLOT", QVariant((int)i));
		allPlots[i].ui_elements.update->setProperty("ID_IDENTIFY_PLOT", QVariant((int)i));
		allPlots[i].ui_elements.le_ymin->setProperty("ID_IDENTIFY_PLOT", QVariant((int)i));
		allPlots[i].ui_elements.le_ymax->setProperty("ID_IDENTIFY_PLOT", QVariant((int)i));
		allPlots[i].ui_elements.le_xmin->setProperty("ID_IDENTIFY_PLOT", QVariant((int)i));
		allPlots[i].ui_elements.le_xmax->setProperty("ID_IDENTIFY_PLOT", QVariant((int)i));

		allPlots[i].ui_elements.thePlot->setCanvasBackground( QColor( Qt::white ) );

		QFont font = allPlots[i].ui_elements.thePlot->axisFont(QwtPlot::xBottom);
		font.setPixelSize((int)myConfig.fontPixelSize);
		allPlots[i].ui_elements.thePlot->setAxisFont (QwtPlot::xBottom, font);
		allPlots[i].ui_elements.thePlot->setAxisFont (QwtPlot::yLeft, font);

		font.setPixelSize((int)myConfig.fontPixelSize);
		txt.setFont(font);
		txt.setText(title_xdata.c_str());
		allPlots[i].ui_elements.thePlot->setAxisTitle( QwtPlot::xBottom, txt);
		txt.setText(myConfig.textY.c_str());
		allPlots[i].ui_elements.thePlot->setAxisTitle( QwtPlot::yLeft,txt );

		allPlots[i].ui_elements.thePlot->setAutoDelete(false);

		// Setup a grid
		allPlots[i].ui_elements.theGrid = new QwtPlotGrid;
		allPlots[i].ui_elements.theGrid->setMajorPen( QPen( Qt::gray, 1, Qt::DotLine ) );
		allPlots[i].ui_elements.theGrid->setMinorPen( QPen( Qt::gray, 1 , Qt::DotLine ) );
		allPlots[i].ui_elements.theGrid->attach( allPlots[i].ui_elements.thePlot );

		//allPlots[i].theLegend = new QwtLegend;
		//allPlots[i].thePlot->insertLegend(allPlots[i].theLegend,  QwtPlot::BottomLegend );

		allPlots[i].ui_elements.theCurve = new QwtPlotCurve();
		allPlots[i].ui_elements.theCurve->setTitle(("signal #" + jvx_size2String(i)).c_str());
		QPen thePen;

		thePen.setColor(Qt::blue);

		thePen.setWidth((int)myConfig.plotPixelsize);

		thePen.setStyle(Qt::SolidLine);

		allPlots[i].ui_elements.theCurve->setPen(thePen);
		allPlots[i].ui_elements.theCurve->attach(allPlots[i].ui_elements.thePlot);

		allPlots[i].bufIdx_x = 0;
		allPlots[i].bufIdx_y = i;

		allPlots[i].ymin = -1.0;
		allPlots[i].ymax = 1.0;

		allPlots[i].xmin = 0;
		allPlots[i].xmax = 0;

		allPlots[i].auto_scale = false;
		allPlots[i].update = true;

		// Connect all slots with signals
		connect(allPlots[i].ui_elements.auto_scale, SIGNAL(clicked(bool)), this, SLOT(auto_scale_clicked(bool)));
		connect(allPlots[i].ui_elements.update, SIGNAL(clicked(bool)), this, SLOT(update_clicked(bool)));

		connect(allPlots[i].ui_elements.le_ymin, SIGNAL(editingFinished()), this, SLOT(ymin_edit()));
		connect(allPlots[i].ui_elements.le_ymax, SIGNAL(editingFinished()), this, SLOT(ymax_edit()));
		connect(allPlots[i].ui_elements.le_xmin, SIGNAL(editingFinished()), this, SLOT(xmin_edit()));
		connect(allPlots[i].ui_elements.le_xmax, SIGNAL(editingFinished()), this, SLOT(xmax_edit()));

		allPlots[i].plotY = NULL;

		if(myConfig.numPixelsPlot > 0)
		{
			JVX_SAFE_NEW_FLD(allPlots[i].plotY, jvxData, myConfig.numPixelsPlot);
			memset(allPlots[i].plotY, 0, sizeof(jvxData)*myConfig.numPixelsPlot);
		}

	}

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

	frameControl = new QFrame(scrollAreaWidgetContents);
	frameControl ->setPalette(palette_gray);
	frameControl ->setAutoFillBackground(true);
	frameControl ->setFrameShape(QFrame::StyledPanel);
	frameControl ->setFrameShadow(QFrame::Raised);

	vertLayoutControl = new QVBoxLayout(frameControl);

	vertLayoutControl->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	label_plotMode = new QLabel(frameControl);
	label_plotMode->setText("Plot Mode");
	myFont = label_plotMode->font();
	myFont.setPixelSize((int)myConfig.fontPixelSize);
	label_plotMode->setFont(myFont);
	label_plotMode->setAlignment(Qt::AlignCenter);
	vertLayoutControl->addWidget(label_plotMode);

	comboBox_plotMode = new QComboBox(frameControl);
	myFont = comboBox_plotMode->font();
	myFont.setPixelSize((int)myConfig.fontPixelSize);
	comboBox_plotMode->setFont(myFont);
	//comboBox_plotMode->setAlignment(Qt::AlignCenter);
	vertLayoutControl->addWidget(comboBox_plotMode);
	comboBox_plotMode->setMaximumSize((int)myConfig.ctrWidth, (int)myConfig.ctrHeight);

	vertLayoutControl->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	label_plotSpeed = new QLabel(frameControl);
	label_plotSpeed->setText("Plot Speed");
	label_plotSpeed->setAlignment(Qt::AlignCenter);
	myFont = label_plotSpeed->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		label_plotSpeed->setFont(myFont);
	vertLayoutControl->addWidget(label_plotSpeed);

	comboBox_plotSpeed = new QComboBox(frameControl);
	//comboBox_plotSpeed->setAlignment(Qt::AlignCenter);
	myFont = comboBox_plotSpeed->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		comboBox_plotSpeed->setFont(myFont);
	vertLayoutControl->addWidget(comboBox_plotSpeed);
	comboBox_plotSpeed->setMaximumSize((int)myConfig.ctrWidth, (int)myConfig.ctrHeight);

	vertLayoutControl->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	pushButton_clearPlots = new QPushButton(frameControl);
	pushButton_clearPlots ->setText("Clear");
	myFont = pushButton_clearPlots->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		pushButton_clearPlots->setFont(myFont);
	//pushButton_clearPlots->setAlignment(Qt::AlignCenter);
	vertLayoutControl->addWidget(pushButton_clearPlots );
	pushButton_clearPlots->setMaximumSize((int)myConfig.ctrWidth, (int)myConfig.ctrHeight);

	vertLayoutControl->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	label_triggerAmplt = new QLabel(frameControl);
	label_triggerAmplt->setText("Trigger Level");
	myFont = label_triggerAmplt->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		label_triggerAmplt->setFont(myFont);
	label_triggerAmplt->setAlignment(Qt::AlignCenter);
	vertLayoutControl->addWidget(label_triggerAmplt);

	lineEdit_triggerAmplt = new QLineEdit(frameControl);
	lineEdit_triggerAmplt->setAlignment(Qt::AlignCenter);
	myFont = lineEdit_triggerAmplt->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		lineEdit_triggerAmplt->setFont(myFont);
	vertLayoutControl->addWidget(lineEdit_triggerAmplt);
	lineEdit_triggerAmplt->setMaximumSize((int)myConfig.ctrWidth, (int)myConfig.ctrHeight);

	vertLayoutControl->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	label_triggerSelect = new QLabel(frameControl);
	label_triggerSelect->setText("Trigger Line Select");
	label_triggerSelect->setAlignment(Qt::AlignCenter);
	myFont = label_triggerSelect->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		label_triggerSelect->setFont(myFont);
	vertLayoutControl->addWidget(label_triggerSelect);

	comboBox_lineTriggerSelect = new QComboBox(frameControl);
	myFont = comboBox_lineTriggerSelect->font();
		myFont.setPixelSize((int)myConfig.fontPixelSize);
		comboBox_lineTriggerSelect->setFont(myFont);
	//comboBox_lineTriggerSelect->setAlignment(Qt::AlignCenter);
	vertLayoutControl->addWidget(comboBox_lineTriggerSelect);
	comboBox_lineTriggerSelect->setMaximumSize((int)myConfig.ctrWidth, (int)myConfig.ctrHeight);

	vertLayoutControl->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	vertLayoutControl->setStretch(0, 10);
	vertLayoutControl->setStretch(1, 1);
	vertLayoutControl->setStretch(2, 1);
	vertLayoutControl->setStretch(3, 1);
	vertLayoutControl->setStretch(4, 1);
	vertLayoutControl->setStretch(5, 1);
	vertLayoutControl->setStretch(6, 1);
	vertLayoutControl->setStretch(7, 1);
	vertLayoutControl->setStretch(8, 1);
	vertLayoutControl->setStretch(9, 1);
	vertLayoutControl->setStretch(10, 1);
	vertLayoutControl->setStretch(11, 1);
	vertLayoutControl->setStretch(12, 1);
	vertLayoutControl->setStretch(13, 1);
	vertLayoutControl->setStretch(14, 10);

	frameControl->setLayout(vertLayoutControl);
	allContentsLayout->addWidget(frameControl, 1);

	// ================================================================

	scrollAreaWidgetContents->setLayout(allContentsLayout);

	scrollArea->setWidget(scrollAreaWidgetContents);
	QHBoxLayout* lay = new QHBoxLayout(this);
	lay->addWidget(scrollArea);
	this->setLayout(lay);
	
	comboBox_lineTriggerSelect->clear(); 
	for(i = 0; i < myConfig.numPlots; i++)
	{
		comboBox_lineTriggerSelect->addItem(("Line #" + jvx_size2String(i)).c_str());
	}

	connect(comboBox_plotSpeed, SIGNAL(activated(int)), this, SLOT(newSelectionPlotSpeed(int)));
	connect(comboBox_plotMode, SIGNAL(activated(int)),  this, SLOT(newSelectionPlotMode(int)));
	connect(comboBox_lineTriggerSelect, SIGNAL(activated(int)),  this, SLOT(newSelectionLineTrigger(int)));
	connect(pushButton_clearPlots, SIGNAL(clicked()), this, SLOT(clearAllPlots()));
	connect(lineEdit_triggerAmplt, SIGNAL(editingFinished()), this, SLOT(newValueTriggerAmpl()));
	
	uiSetupComplete = true;
	update_window();
}

jvxErrorType
	jvxQtSaCircbufferViewer::startPlotting()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 valI32;
	jvxCallManagerProperties callGate;
	jvxBool foundit = false;
	jvxBool isValid = true;
	jvxSize hdlIdx = 0;

	// After prepared
	fillHeightLastPlot = 0;

	linkedProperties.id_xaxis_data = -1;
	linkedProperties.id_xaxis_title = -1;

	myConfig_supersede = myConfig;
	jvxBool isValidPropRef = false;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (!isValidPropRef)
	{
		ident.reset(linkedProperties.id_num_in_channels, JVX_PROPERTY_CATEGORY_PREDEFINED);
		trans.reset(true);
		res = myProperties.theProps->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
		if(res == JVX_NO_ERROR)
		{
			myConfig_supersede.numPlots = valI32;
			theCircHeader.fld = jvx_allocate1DCircExternalBuffer(myConfig.numPixelsPlot, valI32, JVX_DATAFORMAT_DATA, JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED,
				jvx_static_lock, jvx_static_try_lock, jvx_static_unlock, &theCircHeader.sz);
			theCircHeader.isValid = true;

			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(theCircHeader.fld, &theCircHeader.isValid, JVX_DATAFORMAT_DATA);
			ident.reset(linkedProperties.id_circplot, JVX_PROPERTY_CATEGORY_UNSPECIFIC);

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
	jvxQtSaCircbufferViewer::stopPlotting()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isValidPropRef = false;
	jvxCallManagerProperties callGate;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (!isValidPropRef)
	{
		if(theCircHeader.isValid)
		{
			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(theCircHeader.fld, &theCircHeader.isValid, JVX_DATAFORMAT_DATA);
			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(linkedProperties.id_circplot, JVX_PROPERTY_CATEGORY_UNSPECIFIC);

			res = myProperties.theProps->uninstall_referene_property(callGate, ptrRaw, ident);

			jvx_deallocateExternalBuffer(theCircHeader.fld);
			theCircHeader.isValid = false;
			theCircHeader.fld = NULL;
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
	jvxQtSaCircbufferViewer::update_window()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxSelectionList selList;
	
	jvxData valDbl;

	comboBox_plotSpeed->setEnabled(false);
	comboBox_lineTriggerSelect->setEnabled(false);
	lineEdit_triggerAmplt->setEnabled(false);

	if(!uiSetupComplete)
		return;

	for(i = 0; i < myConfig.numPlots; i++)
	{
		allPlots[i].ui_elements.le_ymin->setText(jvx_data2String(allPlots[i].ymin,1).c_str());
		allPlots[i].ui_elements.le_ymax->setText(jvx_data2String(allPlots[i].ymax,1).c_str());
		allPlots[i].ui_elements.le_xmin->setText(jvx_data2String(allPlots[i].xmin,1).c_str());
		allPlots[i].ui_elements.le_xmax->setText(jvx_data2String(allPlots[i].xmax,1).c_str());
		allPlots[i].ui_elements.update->setChecked(allPlots[i].update);
		allPlots[i].ui_elements.auto_scale->setChecked(allPlots[i].auto_scale);
	}

	if (!myProperties.theProps)
	{
		return;
	}

	read_properties_shortcut();
	jvxSize selId = jvx_bitfieldSelection2Id(myDataCollectionMode, sizeof(jvxBitField));
	if(JVX_CHECK_SIZE_SELECTED(selId))
	{
		comboBox_plotMode->setCurrentIndex(JVX_SIZE_INT(selId));
	}

	if(linkedProperties.id_plot_mode >= 0)
	{
		ident.reset(linkedProperties.id_plot_mode,
			JVX_PROPERTY_CATEGORY_UNSPECIFIC);
		trans.reset();
		res = myProperties.theProps->get_property(callGate, jPRG(&selList, 1,
			JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(linkedProperties.id_plot_mode, JVX_PROPERTY_CATEGORY_UNSPECIFIC),
			myProperties.theProps))
		{
			if (selList.strList.ll() != comboBox_plotMode->count())
			{
				comboBox_plotMode->clear();
				for (i = 0; i < selList.strList.ll(); i++)
				{
					comboBox_plotMode->addItem(selList.strList.std_str_at(i).c_str());
				}
			}
		}
	}

	if(linkedProperties.id_showFactor >= 0)
	{
		ident.reset(linkedProperties.id_showFactor,
			JVX_PROPERTY_CATEGORY_UNSPECIFIC);
		trans.reset();

		res = myProperties.theProps->get_property(callGate, jPRG(&selList, 1,
			JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(linkedProperties.id_showFactor, JVX_PROPERTY_CATEGORY_UNSPECIFIC),
			myProperties.theProps))
		{
			if (selList.strList.ll() != comboBox_plotSpeed->count())
			{
				comboBox_plotSpeed->clear();
				for (i = 0; i < selList.strList.ll(); i++)
				{
					comboBox_plotSpeed->addItem(selList.strList.std_str_at(i).c_str());
				}
			}

			selId = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), comboBox_plotSpeed->count());
			if(JVX_CHECK_SIZE_SELECTED(selId))
			{
				comboBox_plotSpeed->setCurrentIndex(JVX_SIZE_INT(selId));
			}
		}
	}

	if(linkedProperties.id_line_trigger >= 0)
	{
		ident.reset(linkedProperties.id_line_trigger,
			JVX_PROPERTY_CATEGORY_UNSPECIFIC);
		trans.reset();
		res = myProperties.theProps->get_property(callGate, jPRG(&selList, 1,
			JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(linkedProperties.id_line_trigger, JVX_PROPERTY_CATEGORY_UNSPECIFIC),
			myProperties.theProps))
		{
			if (selList.strList.ll() != comboBox_lineTriggerSelect->count())
			{
				comboBox_lineTriggerSelect->clear();
				for (i = 0; i < selList.strList.ll(); i++)
				{
					comboBox_lineTriggerSelect->addItem(selList.strList.std_str_at(i).c_str());
				}
			}

			selId = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), comboBox_lineTriggerSelect->count());
			if(JVX_CHECK_SIZE_SELECTED(selId))
			{
				comboBox_lineTriggerSelect->setCurrentIndex(JVX_SIZE_INT(selId));
			}
		}
	}



	if(linkedProperties.id_trigger_amplitude >= 0)
	{
		ident.reset(linkedProperties.id_trigger_amplitude,
			JVX_PROPERTY_CATEGORY_UNSPECIFIC);
		trans.reset();
		res = myProperties.theProps->get_property(callGate, jPRG(&valDbl, 1,
			JVX_DATAFORMAT_DATA), ident, trans);
		if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(linkedProperties.id_trigger_amplitude, JVX_PROPERTY_CATEGORY_UNSPECIFIC),
			myProperties.theProps))
		{
			lineEdit_triggerAmplt->setText(jvx_data2String(valDbl,2).c_str());
		}
	}


	// Case specific activations
	if(
		(myDataCollectionMode & JVX_CIRCBUFFER_TRIGGER_INBAND) ||
		(myDataCollectionMode & JVX_CIRCBUFFER_TRIGGER_EXPLICIT))
	{
		comboBox_lineTriggerSelect->setEnabled(true);
		lineEdit_triggerAmplt->setEnabled(true);
	}
	if(myDataCollectionMode & JVX_CIRCBUFFER_VISUAL_RESAMPLER)
	{
		comboBox_plotSpeed->setEnabled(true);
	}
}

void
	jvxQtSaCircbufferViewer::update_window_periodic()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize chanLim;

	jvxSize i,j;
	jvxSize numSamplesCopy;
	jvxSize idxRead;
	jvxSize ll1, ll2;
	jvxByte* basePtr;

	jvxData local_min = JVX_DATA_MAX_POS;
	jvxData local_max = JVX_DATA_MAX_NEG;

	if(theCircHeader.fld)
	{
		theCircHeader.fld->safe_access.lockf(theCircHeader.fld->safe_access.priv);

		numSamplesCopy = theCircHeader.fld->fill_height;
		idxRead = theCircHeader.fld->idx_read;

		theCircHeader.fld->safe_access.unlockf(theCircHeader.fld->safe_access.priv);

		basePtr = (jvxByte*)theCircHeader.fld->ptrFld;
		assert(theCircHeader.fld->formFld == JVX_DATAFORMAT_DATA);

		chanLim = myConfig.numPlots;
		chanLim = JVX_MIN(chanLim, myConfig_supersede.numPlots);
		chanLim = JVX_MIN(chanLim, theCircHeader.fld->number_channels);

		if(
			(myDataCollectionMode & JVX_CIRCBUFFER_TRIGGER_INBAND) ||
			(myDataCollectionMode & JVX_CIRCBUFFER_TRIGGER_EXPLICIT))
		{
			for(i = 0; i < chanLim; i++)
			{
				jvxData* in = (jvxData*)basePtr;
				in += i * theCircHeader.fld->length;
				memcpy(allPlots[i].plotY, in, theCircHeader.fld->length*sizeof(jvxData));
			}
		}
		else
		{

			ll1 = numSamplesCopy;
			ll1 = JVX_MIN(ll1, theCircHeader.fld->length - idxRead);
			ll2 = numSamplesCopy - ll1;
			for(i = 0; i < chanLim; i++)
			{
				jvxSize tt = (myConfig.numPixelsPlot - numSamplesCopy);
				jvxData* in = (jvxData*)basePtr;
				in += i * theCircHeader.fld->length;
				memmove(allPlots[i].plotY, allPlots[i].plotY + numSamplesCopy, tt * sizeof(jvxData));
				if(ll1)
				{
					memcpy(allPlots[i].plotY + tt, in + idxRead, ll1*sizeof(jvxData));
				}
				if(ll2)
				{
					memcpy(allPlots[i].plotY + tt + ll1, in, ll2*sizeof(jvxData));
				}
			}

			theCircHeader.fld->safe_access.lockf(theCircHeader.fld->safe_access.priv);

			theCircHeader.fld->fill_height -= numSamplesCopy;
			theCircHeader.fld->idx_read = (theCircHeader.fld->idx_read + numSamplesCopy) % theCircHeader.fld->length;

			theCircHeader.fld->safe_access.unlockf(theCircHeader.fld->safe_access.priv);
		}

		for(i = 0; i < myConfig.numPlots; i++)
		{
			if(allPlots[i].update)
			{
				if(allPlots[i].auto_scale)
				{
					local_min = JVX_DATA_MAX_POS;
					local_max = JVX_DATA_MAX_NEG;
					for(j = 0; j < myConfig.numPixelsPlot; j++)
					{
						if(allPlots[i].plotY[j] <= local_min)
						{
							local_min = allPlots[i].plotY[j];
						}

						if(allPlots[i].plotY[j] >= local_max)
						{
							local_max = allPlots[i].plotY[j];
						}
					}
					allPlots[i].ymin = local_min;
					allPlots[i].ymax = local_max;
					allPlots[i].ui_elements.le_ymin->setText(jvx_data2String(allPlots[i].ymin, 2).c_str());
					allPlots[i].ui_elements.le_ymax->setText(jvx_data2String(allPlots[i].ymax, 2).c_str());
					allPlots[i].ui_elements.le_xmin->setText(jvx_data2String(allPlots[i].xmin, 2).c_str());
					allPlots[i].ui_elements.le_xmax->setText(jvx_data2String(allPlots[i].xmax, 2).c_str());

					// Maybe in autoscale mode?
					// allPlots[i].xmin = plotX[0];
					//allPlots[i].xmax = plotX[myConfig.numPixelsPlot - 1];
				}


				allPlots[i].ui_elements.theCurve->setSamples(plotX, allPlots[i].plotY, (int)myConfig.numPixelsPlot);

				allPlots[i].ui_elements.thePlot->setAxisScale(QwtPlot::yLeft, allPlots[i].ymin, allPlots[i].ymax);
				allPlots[i].ui_elements.thePlot->setAxisScale(QwtPlot::xBottom, allPlots[i].xmin, allPlots[i].xmax);

				allPlots[i].ui_elements.thePlot->replot();

			}
		}

		//	if(jvx_isValidPropertyReferenceTriple(&properties.theTripleNode))
		//	{
		//	}
	}
}

// ============================================================================
// ============================================================================

void
	jvxQtSaCircbufferViewer::auto_scale_clicked(bool click)
{
	QVariant line;
	jvxSize lineId= 0;
	QObject* obj = sender();
	if(obj)
	{
		line = obj->property("ID_IDENTIFY_PLOT");
		if(line.isValid())
		{
			lineId = line.toInt();
			if(lineId < myConfig.numPlots)
			{
				allPlots[lineId].auto_scale = click;
				this->update_window();
			}
		}
	}
}

void
	jvxQtSaCircbufferViewer::update_clicked(bool click)
{
	QVariant line;
	jvxSize lineId= 0;
	QObject* obj = sender();
	if(obj)
	{
		line = obj->property("ID_IDENTIFY_PLOT");
		if(line.isValid())
		{
			lineId = line.toInt();
			if(lineId < myConfig.numPlots)
			{
				allPlots[lineId].update = click;
				this->update_window();
			}
		}
	}
}

void
	jvxQtSaCircbufferViewer::ymin_edit()
{
	QVariant line;
	jvxSize lineId= 0;
	QString tt;
	QObject* obj = sender();
	if(obj)
	{
		line = obj->property("ID_IDENTIFY_PLOT");
		if(line.isValid())
		{
			lineId = line.toInt();
			if(lineId < myConfig.numPlots)
			{
				tt = allPlots[lineId].ui_elements.le_ymin->text();
				allPlots[lineId].ymin = tt.toDouble();
				allPlots[lineId].ymax =  JVX_MAX(allPlots[lineId].ymax, (allPlots[lineId].ymin + myConfig.epsyplot));
				updatePlotParameters(false);
				this->update_window();
			}
		}
	}
}

void
	jvxQtSaCircbufferViewer::ymax_edit()
{
	QVariant line;
	jvxSize lineId= 0;
	QString tt;
	QObject* obj = sender();
	if(obj)
	{
		line = obj->property("ID_IDENTIFY_PLOT");
		if(line.isValid())
		{
			lineId = line.toInt();
			if(lineId < myConfig.numPlots)
			{
				tt = allPlots[lineId].ui_elements.le_ymax->text();
				allPlots[lineId].ymax = tt.toDouble();
				allPlots[lineId].ymin = JVX_MIN(allPlots[lineId].ymin, (allPlots[lineId].ymax - myConfig.epsyplot));
				updatePlotParameters(false);
				this->update_window();
			}
		}
	}
}


void
	jvxQtSaCircbufferViewer::xmin_edit()
{
	QVariant line;
	jvxSize lineId= 0;
	QString tt;
	QObject* obj = sender();
	if(obj)
	{
		line = obj->property("ID_IDENTIFY_PLOT");
		if(line.isValid())
		{
			lineId = line.toInt();
			if(lineId < myConfig.numPlots)
			{
				tt = allPlots[lineId].ui_elements.le_xmin->text();
				allPlots[lineId].xmin = tt.toDouble();
				allPlots[lineId].xmax =  JVX_MAX(allPlots[lineId].xmax, (allPlots[lineId].xmin + myConfig.epsyplot));
				updatePlotParameters(false);
				this->update_window();
			}
		}
	}
}

void
	jvxQtSaCircbufferViewer::xmax_edit()
{
	QVariant line;
	jvxSize lineId= 0;
	QString tt;
	QObject* obj = sender();
	if(obj)
	{
		line = obj->property("ID_IDENTIFY_PLOT");
		if(line.isValid())
		{
			lineId = line.toInt();
			if(lineId < myConfig.numPlots)
			{
				tt = allPlots[lineId].ui_elements.le_xmax->text();
				allPlots[lineId].xmax = tt.toDouble();
				allPlots[lineId].xmin = JVX_MIN(allPlots[lineId].xmin, (allPlots[lineId].xmax - myConfig.epsyplot));
				updatePlotParameters(false);
				this->update_window();
			}
		}
	}
}


void
	jvxQtSaCircbufferViewer::newSelectionPlotSpeed(int select)
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;

	jvxBool isValidPropRef = false;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (!isValidPropRef)
	{

		if(linkedProperties.id_showFactor)
		{
			ident.reset(linkedProperties.id_showFactor,
				JVX_PROPERTY_CATEGORY_UNSPECIFIC);
			trans.reset(true);
			res = myProperties.theProps->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(linkedProperties.id_showFactor, JVX_PROPERTY_CATEGORY_UNSPECIFIC),
				myProperties.theProps))
			{
				selList.bitFieldSelected() = (jvxBitField)1 << select;
				ident.reset(linkedProperties.id_showFactor,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset(true);
				res = myProperties.theProps->set_property(callGate , jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
					ident, trans);
				assert(res == JVX_NO_ERROR);

				//updateTimeBasePlots(selList.bitFieldSelected);
				updatePlotParameters(true);
				update_window();
			}
		}
	}
}

void
	jvxQtSaCircbufferViewer::newSelectionPlotMode(int selId)
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;

	jvxBool isValidPropRef = false;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (!isValidPropRef)
	{

		if(linkedProperties.id_plot_mode)
		{
			ident.reset(linkedProperties.id_plot_mode,
				JVX_PROPERTY_CATEGORY_UNSPECIFIC);
			trans.reset(true);
			res = myProperties.theProps->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(linkedProperties.id_plot_mode, JVX_PROPERTY_CATEGORY_UNSPECIFIC),
				myProperties.theProps))
			{
				selList.bitFieldSelected() = (jvxBitField)1 << selId;
				ident.reset(linkedProperties.id_plot_mode,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset(true);
				res = myProperties.theProps->set_property(callGate,
					jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				assert(res == JVX_NO_ERROR);

				updatePlotParameters(true);
				update_window();
			}
		}
	}
}

void
	jvxQtSaCircbufferViewer::clearAllPlots()
{
	jvxSize i,j;
	jvxErrorType res = JVX_NO_ERROR;
	std::string command = "resetTriggeredCapture();";
	jvxApiString txt;
	jvxCallManagerProperties callGate;
	txt.assign_const(command.c_str(), command.size());

	jvxBool isValidPropRef = false;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (!isValidPropRef)
	{

		if(linkedProperties.id_command >= 0)
		{
			ident.reset(linkedProperties.id_command,
				JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset(false);
			res = myProperties.theProps->set_property(callGate,
				jPRG(&txt, 1, JVX_DATAFORMAT_STRING),ident, trans);
			assert(res == JVX_NO_ERROR);
		}
	}

	for(i = 0; i < myConfig.numPlots; i++)
	{
		for(j = 0; j < myConfig.numPixelsPlot; j++)
		{
			allPlots[i].plotY[j] = 0.0;
		}
	}
}

void
	jvxQtSaCircbufferViewer::newSelectionLineTrigger(int selId)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;


	jvxBool isValidPropRef = false;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (!isValidPropRef)
	{

		if(linkedProperties.id_plot_mode)
		{
			ident.reset(linkedProperties.id_line_trigger,
				JVX_PROPERTY_CATEGORY_UNSPECIFIC);
			trans.reset(true);
			res = myProperties.theProps->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if(res == JVX_NO_ERROR)
			{
				selList.bitFieldSelected() = (jvxBitField)1 << selId;
				ident.reset(linkedProperties.id_line_trigger,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset(true);
				res = myProperties.theProps->set_property(callGate ,
					jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
					ident, trans);
				assert(res == JVX_NO_ERROR);

				update_window();
			}
		}
	}
}

void
	jvxQtSaCircbufferViewer::newValueTriggerAmpl()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	QString txt = lineEdit_triggerAmplt->text();
	jvxData val = txt.toDouble();

	jvxBool isValidPropRef = false;
	myProperties.theProps->reference_status(NULL, &isValidPropRef);
	if (!isValidPropRef)
	{

		if(linkedProperties.id_trigger_amplitude >= 0)
		{
			ident.reset(linkedProperties.id_trigger_amplitude,
				JVX_PROPERTY_CATEGORY_UNSPECIFIC);
			trans.reset(true);

			res = myProperties.theProps->set_property(callGate ,
				jPRG(&val, 1, JVX_DATAFORMAT_DATA),
			ident, trans);
		}
	}
	//JVX_GEST_SENSE_TRIGGER_AMPLITUDE
	this->update_window();
}

void
jvxQtSaCircbufferViewer::updatePlotParameters(jvxBool refreshValues)
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
				allPlots[i].xmin = plotX[0];
				allPlots[i].xmax = plotX[myConfig.numPixelsPlot-1];
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

			for(i = 0; i < myConfig.numPlots; i++)
			{
				QFont font = allPlots[i].ui_elements.thePlot->axisFont(QwtPlot::xBottom);
				font.setPixelSize((int)myConfig.fontPixelSize);
				allPlots[i].ui_elements.thePlot->setAxisFont (QwtPlot::xBottom, font);

				txt.setFont(font);
				txt.setText(title_xdata.c_str());

				allPlots[i].ui_elements.thePlot->setAxisTitle( QwtPlot::xBottom, txt);
			}
		}
	}
	for(i = 0; i < myConfig.numPlots; i++)
	{

		allPlots[i].ui_elements.theCurve->setSamples(plotX, allPlots[i].plotY, (int)myConfig.numPixelsPlot);
		allPlots[i].ui_elements.thePlot->setAxisScale(QwtPlot::xBottom, allPlots[i].xmin, allPlots[i].xmax);
		allPlots[i].ui_elements.thePlot->setAxisScale(QwtPlot::yLeft, allPlots[i].ymin, allPlots[i].ymax);
		allPlots[i].ui_elements.thePlot->replot();
	}
}
