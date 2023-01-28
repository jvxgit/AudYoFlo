#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>

#include "uMainWindow.h"

// Includes for main host application
#include "jvxHJvx.h"

JVX_DECLARE_QT_METATYPES

/*
void
uMainWindow::setTabPositionRight(int idTab)
{
	int i;
	int idFound = -1;

	QList<QTabBar *> tabList = findChildren<QTabBar *>();
	for(i = 0; i < tabList.size(); i++)
	{
		if(!tabList.isEmpty())
		{
			QTabBar *tabBar = tabList.at(i);
			QString txt = tabBar->tabText(i);

			// tabBar->currentIndex()
			if((txt == "Seqeunce Editor") || (txt == "Realtime Viewer"))
			{
				idFound = i;
			}
		}
	}
	if(idFound >= 0)
	{
		QTabBar *tabBar = tabList.at(idFound);
		tabBar->setCurrentIndex(idTab);
	}
}
*/
void
uMainWindow::resetJvxReferences()
{
	this->involvedComponents.theHost.hobject = NULL;
	this->involvedComponents.theHost.hHost = NULL;
	this->involvedComponents.theTools.hTools = NULL;

}

jvxErrorType
uMainWindow::initSystem(QApplication* hdlApp, char* clStr[], int clNum)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	JVX_REGISTER_QT_METATYPES

	this->setupUi(this);

	this->subWidgets.bootupComplete = false;

	this->treeWidget->setColumnCount(3);
	QStringList lst;
	lst.push_back("Entry #");
	lst.push_back("Description");
	lst.push_back("Length");
	treeWidget->setHeaderLabels(lst);
	treeWidget->setSortingEnabled(false);

	pushButton_play->setStyleSheet("background-color:gray;");
	pushButton_record->setStyleSheet("background-color:gray;");
	pushButton_enhance->setStyleSheet("background-color:gray;");
	pushButton_process->setStyleSheet("background-color:gray;");
	pushButton_save->setStyleSheet("background-color:gray;");

	// Init stuff for qwt
	QwtText txt;
	QFont ft = txt.font();
	ft.setPixelSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	txt.setFont(ft);

	//==============================================================
	// INIT PLOT ENGINE (Linear PLOT)
	//==============================================================
	qwtPlot_left->setCanvasBackground( QColor( Qt::white ) );
	qwtPlot_right->setCanvasBackground( QColor( Qt::white ) );

	QFont font = qwtPlot_left->axisFont(QwtPlot::xBottom);
    font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
    qwtPlot_left->setAxisFont (QwtPlot::xBottom, font);
    qwtPlot_right->setAxisFont (QwtPlot::yLeft, font);

	font.setPointSize(JVX_PIXEL_SIZE_FONTS_PLOT_AXIS);
	txt.setFont(font);
	txt.setText("Time [sec]");
    qwtPlot_left->setAxisTitle( QwtPlot::xBottom, txt);
	qwtPlot_right->setAxisTitle( QwtPlot::xBottom, txt);
	txt.setText("x(k)");
    qwtPlot_left->setAxisTitle( QwtPlot::yLeft,txt );
    qwtPlot_right->setAxisTitle( QwtPlot::yLeft,txt );

	qwtPlot_left->setAutoDelete(false);
	qwtPlot_right->setAutoDelete(false);

	// Setup a grid
	plot.theGrid_left = new QwtPlotGrid;
	plot.theGrid_left->setMajorPen( QPen( Qt::gray, 1, Qt::DotLine ) );
    plot.theGrid_left->setMinorPen( QPen( Qt::gray, 1 , Qt::DotLine ) );
	plot.theGrid_left->attach(qwtPlot_left);
	plot.theGrid_right = new QwtPlotGrid;
	plot.theGrid_right->setMajorPen( QPen( Qt::gray, 1, Qt::DotLine ) );
    plot.theGrid_right->setMinorPen( QPen( Qt::gray, 1 , Qt::DotLine ) );
	plot.theGrid_right->attach(qwtPlot_right);

	plot.theLegend_left = new QwtLegend;
	qwtPlot_left->insertLegend(plot.theLegend_left,  QwtPlot::BottomLegend );
	plot.theLegend_right = new QwtLegend;
	qwtPlot_right->insertLegend(plot.theLegend_right,  QwtPlot::BottomLegend );

	for(i = 0; i < JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT; i++)
	{
		plot.x_left[i] = 0;
		plot.x_right[i] = 0;
		plot.y_left[i] = 0;
		plot.y_right[i] = 0;
	}

	plot.theCurve_left = new QwtPlotCurve();
	plot.theCurve_right = new QwtPlotCurve();

	//theCurve->setTitle(descr.c_str());
	QPen thePen;
	thePen.setColor(Qt::black);
	thePen.setWidth(2);
	thePen.setStyle(Qt::SolidLine);
	plot.theCurve_left->setPen(thePen);
	plot.theCurve_left->attach(qwtPlot_left);
	plot.theCurve_right->setPen(thePen);
	plot.theCurve_right->attach(qwtPlot_right);

	this->updatePlots();

	// Connect for different purposes
	connect(this, SIGNAL(emit_postMessage(QString, QColor)), this, SLOT(postMessage_inThread(QString, QColor)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_bootDelayed()), this, SLOT(bootDelayed()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_report_internals_have_changed_inThread(jvxComponentType, IjvxObject*, jvxPropertyCategoryType, jvxInt16, bool, jvxComponentType, jvxPropertyCallPurpose)), this,
		SLOT(report_internals_have_changed_inThread(jvxComponentType, IjvxObject*, jvxPropertyCategoryType, jvxInt16, bool, jvxComponentType, jvxPropertyCallPurpose)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_report_command_request(short)), this, SLOT(report_command_request_inThread(short)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_close_app()), this, SLOT(close_app()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_updateStateSequencer(jvxUInt64, int, int, int)), this, SLOT(updateStateSequencer_inThread(jvxUInt64, int, int, int)), Qt::QueuedConnection);

	connect_specific();



	// Release dock widgets to be handles as all parts are complete
	this->subWidgets.bootupComplete = true;

	// Config file and dll directories are hardcoded
	/*
	CjvxCommandline::reset_command_line();
	CjvxCommandline::parse_command_line(clStr, clNum);
	*/
	_command_line_parameters.dllsDirectories.push_back(JVX_STANDALONE_HOST_DLL_DIR);
	_command_line_parameters.configFilename = JVX_STANDALONE_HOST_CONFIGFILE;

	this->initializeHost();

	if(!this->_command_line_parameters.configFilename.empty())
	{
		res = this->configureFromFile(this->_command_line_parameters.configFilename);
	}

	if(res == JVX_NO_ERROR)
	{
		res = startExternalChannels();
	}

	if(res == JVX_NO_ERROR)
	{
		res = readwrite.theBuffers.configure_type(JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS, JVX_STANDALONE_HOST_SAMPLERATE);
		if(res == JVX_NO_ERROR)
		{
			res = readwrite.theBuffers.configure_session(".session", ".session.tmp");
		}
	}

	if(res == JVX_NO_ERROR)
	{
		IjvxProperties* theProps = NULL;
		IjvxObject* theObj = NULL;
		jvxData container[JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];
		jvxBool isValid = false;
		std::string txt;
		jvxSelectionList theList;
		jvxStringList lstStr;
		theList.strList = &lstStr;

		jvxErrorType res = this->involvedComponents.theHost.hHost->request_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
		if((res == JVX_NO_ERROR) && theProps)
		{
			// Allow level control on input side
			res = jvx_get_property(theProps, &theList, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, true, "JVX_GENW_ACT_ILEVEL");
			if(res == JVX_NO_ERROR)
			{
				theList.bitFieldSelected = 1;
			}
			jvx_set_property(theProps, &theList, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, true, "JVX_GENW_ACT_ILEVEL");

			// Allow level control on input side
			res = jvx_get_property(theProps, &theList, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, true, "JVX_GENW_ACT_OLEVEL");
			if(res == JVX_NO_ERROR)
			{
				theList.bitFieldSelected = 1;
			}
			jvx_set_property(theProps, &theList, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, true, "JVX_GENW_ACT_OLEVEL");

		}
		this->involvedComponents.theHost.hHost->return_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
	}
	if(res != JVX_NO_ERROR)
	{
		this->fatalStop("Bootup failed", "Fatal Error");
	}


	// Update ui
	this->updateWindow();

	emit emit_bootDelayed();

	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::initializeHost()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentType cpType = JVX_COMPONENT_UNKNOWN;
	bool multObj = false;

	resetJvxReferences();

	res = jvxHJvx_init(&this->involvedComponents.theHost.hobject);
	if((res != JVX_NO_ERROR) || (this->involvedComponents.theHost.hobject == NULL))
	{
		this->fatalStop("Fatal Error", "Could not load main host component!");
	}

	postMessage_inThread("Successfully opened host", Qt::green);

	this->involvedComponents.theHost.hobject->request_specialization((jvxHandle**)&this->involvedComponents.theHost.hHost, &cpType, &multObj);

	// Check component type of specialization
	if(this->involvedComponents.theHost.hHost && (cpType == JVX_COMPONENT_HOST))
	{
		if(!_command_line_parameters.changeWorkDir.empty())
		{
			JVX_CHDIR(_command_line_parameters.changeWorkDir.c_str());
		}

		// Initialize the host..
		this->involvedComponents.theHost.hHost->initialize(NULL);
		this->involvedComponents.theHost.hHost->select();

		// Set the predefined property for dll path for subcomponents
		jvxSize id = 0;
		if(jvx_findPropertyDescriptor("jvx_component_path_str_list", &id, NULL, NULL, NULL))
		{
			IjvxProperties* theProps = NULL;
			jvxBool isValid = false;
			res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);
			if((res == JVX_NO_ERROR) && theProps)
			{
				res = jvx_pushPullPropertyStringList(_command_line_parameters.dllsDirectories, this->involvedComponents.theHost.hobject, theProps, id, JVX_PROPERTY_CATEGORY_PREDEFINED, &isValid);
				res = this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);
			}
		}

		this->involvedComponents.theHost.hHost->set_external_report_target(static_cast<IjvxReport*>(this));

		res = this->bootup_specific();

		// =================================================================
		// Activate the host
		// =================================================================
		this->involvedComponents.theHost.hHost->activate();

	}
	else
	{
		this->fatalStop("Fatal Error", "Invalid object specialization!");
	}

	// Pass host reference to sequencer ui
	//this->subWidgets.sequences.theWidget->setHostRef(this->involvedComponents.theHost.hHost, static_cast<IjvxReport*>(this));
	//this->subWidgets.realtimeViewer.theWidget->setHostRef(this->involvedComponents.theHost.hHost, static_cast<IjvxReport*>(this));

	//this->subWidgets.sequences.theWidget->updateWindow_rebuild();

	// Tools interfaces
	res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&this->involvedComponents.theTools.hTools));
	if(res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", "No access to JVX tools!");
	}

	return(JVX_NO_ERROR);
}


void
uMainWindow::bootDelayed()
{
}
