#include <QtWidgets/QFileDialog>
#include <QKeyEvent>

#include "myCentralWidget.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include <stdio.h>

#include <QMimeData>
#include <QSignalMapper>
#include <QDateTime>
#include <QMimeDatabase>
// #include <qdebug.h>
#include <QMessageBox>


#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

	myCentralWidget::myCentralWidget(QWidget* parent) : mainCentral_host_ww(parent)
	{
		this->timeDisplayUpdateTimer = new QTimer(this);
		this->clippingUpdateTimer = new QTimer(this);
		this->clippingReleaseTimer = new QTimer(this);

		this->lastSelectFileDir = QDir::homePath();

		thePropRef_algo = NULL;
		thePropRef_dev = NULL;
		parent->installEventFilter(this);
	}

	myCentralWidget::~myCentralWidget() {
		delete this->timeDisplayUpdateTimer;
		delete this->clippingUpdateTimer;
		delete this->clippingReleaseTimer;
	};

	void myCentralWidget::init_submodule(IjvxHost* theHost)
	{
		this->setupUi(this);
		mainCentral_host_ww::init_submodule(theHost);


		horizontalSlider_loopEnd->setRange(0, timeSliderMaxValue);
		horizontalSlider_loopStart->setRange(0, timeSliderMaxValue);
		horizontalSlider_curPlayPos->setRange(0, timeSliderMaxValue);

		horizontalSlider_loopEnd->setDisabled(true);
		horizontalSlider_loopStart->setDisabled(true);
		horizontalSlider_curPlayPos->setDisabled(true);

		// create one input handler per possible item
		IjvxExtInputChannelHandler* tmpInputHandler;
		for (int i = 0; i < this->maxNumOfItems; i++)
		{
			tmpInputHandler = NULL;
			jvx_factory_allocate_jvxExtInputChannelHandler(&tmpInputHandler);
			vecOfInputHandlers.push_back(tmpInputHandler);
		}


		outputHandler = NULL;
		jvx_factory_allocate_jvxExtOutputChannelHandler(&outputHandler);

		assert(outputHandler);


		// connect(this, SIGNAL(retrigger_start()), this, SLOT(start_retriggered()), Qt::QueuedConnection);

		this->timeDisplayUpdateTimer->setInterval(this->updateTimerInterval);
		connect(this->timeDisplayUpdateTimer, SIGNAL(timeout()), this, SLOT(updateTimeDisplay()));

		this->clippingReleaseTimer->setSingleShot(true);
		this->clippingReleaseTimer->setInterval(this->clippingReleaseTime);
		connect(this->clippingReleaseTimer, SIGNAL(timeout()), this, SLOT(resetClippingIndicator()));

		this->clippingUpdateTimer->setInterval(this->updateTimerInterval);
		connect(this->clippingUpdateTimer, SIGNAL(timeout()), this, SLOT(updateClippingIndicator()));

		this->init_listArrays();


		connect(horizontalSlider_loopStart, SIGNAL(valueChanged(int)), this, SLOT(loopStartSliderMoved(int)));
		connect(horizontalSlider_loopEnd, SIGNAL(valueChanged(int)), this, SLOT(loopEndSliderMoved(int)));
		connect(horizontalSlider_curPlayPos, SIGNAL(valueChanged(int)), this, SLOT(curPlayPosSliderMoved(int)));


		connect(checkBox_playMonoAsStereo, SIGNAL(stateChanged(int)), this, SLOT(updatePlayMonoAsStereoClicked(int)));

		this->connectButtons();
		this->connectVolume();


		for (int i = 0; i < this->maxNumOfItems; i++) {
			arrayOfButtons[i]->setFocusPolicy(Qt::NoFocus);
			arrayOfFileButtons[i]->setFocusPolicy(Qt::NoFocus);
			arrayOfVolumeSlider[i]->setFocusPolicy(Qt::NoFocus);
			arrayOfRemoveItemButtons[i]->setFocusPolicy(Qt::NoFocus);
			arrayOfVolumeSpinBox[i]->installEventFilter(this);
		}

		this->buttonPlayPause->setFocusPolicy(Qt::NoFocus);
		this->buttonStop->setFocusPolicy(Qt::NoFocus);
		this->button_hideShow->setFocusPolicy(Qt::NoFocus);
		this->button_shuffle->setFocusPolicy(Qt::NoFocus);
		this->horizontalSlider_curPlayPos->setFocusPolicy(Qt::NoFocus);
		this->horizontalSlider_loopStart->setFocusPolicy(Qt::NoFocus);
		this->horizontalSlider_loopEnd->setFocusPolicy(Qt::NoFocus);

		this->setFocusPolicy(Qt::StrongFocus);

		this->resetHost();

	}


	void myCentralWidget::init_listArrays()
	{

		this->arrayOfLabels[0] = this->labelItemState0;
		this->arrayOfLabels[1] = this->labelItemState1;
		this->arrayOfLabels[2] = this->labelItemState2;
		this->arrayOfLabels[3] = this->labelItemState3;
		this->arrayOfLabels[4] = this->labelItemState4;
		this->arrayOfLabels[5] = this->labelItemState5;
		this->arrayOfLabels[6] = this->labelItemState6;
		this->arrayOfLabels[7] = this->labelItemState7;
		this->arrayOfLabels[8] = this->labelItemState8;

		this->arrayOfButtons[0] = this->buttonItem0;
		this->arrayOfButtons[1] = this->buttonItem1;
		this->arrayOfButtons[2] = this->buttonItem2;
		this->arrayOfButtons[3] = this->buttonItem3;
		this->arrayOfButtons[4] = this->buttonItem4;
		this->arrayOfButtons[5] = this->buttonItem5;
		this->arrayOfButtons[6] = this->buttonItem6;
		this->arrayOfButtons[7] = this->buttonItem7;
		this->arrayOfButtons[8] = this->buttonItem8;

		this->arrayOfVolumeSlider[0] = this->volumeSliderItem0;
		this->arrayOfVolumeSlider[1] = this->volumeSliderItem1;
		this->arrayOfVolumeSlider[2] = this->volumeSliderItem2;
		this->arrayOfVolumeSlider[3] = this->volumeSliderItem3;
		this->arrayOfVolumeSlider[4] = this->volumeSliderItem4;
		this->arrayOfVolumeSlider[5] = this->volumeSliderItem5;
		this->arrayOfVolumeSlider[6] = this->volumeSliderItem6;
		this->arrayOfVolumeSlider[7] = this->volumeSliderItem7;
		this->arrayOfVolumeSlider[8] = this->volumeSliderItem8;

		this->arrayOfVolumeSpinBox[0] = this->spinBox_volume0;
		this->arrayOfVolumeSpinBox[1] = this->spinBox_volume1;
		this->arrayOfVolumeSpinBox[2] = this->spinBox_volume2;
		this->arrayOfVolumeSpinBox[3] = this->spinBox_volume3;
		this->arrayOfVolumeSpinBox[4] = this->spinBox_volume4;
		this->arrayOfVolumeSpinBox[5] = this->spinBox_volume5;
		this->arrayOfVolumeSpinBox[6] = this->spinBox_volume6;
		this->arrayOfVolumeSpinBox[7] = this->spinBox_volume7;
		this->arrayOfVolumeSpinBox[8] = this->spinBox_volume8;

		this->arrayOfFileButtons[0] = this->buttonFileItem0;
		this->arrayOfFileButtons[1] = this->buttonFileItem1;
		this->arrayOfFileButtons[2] = this->buttonFileItem2;
		this->arrayOfFileButtons[3] = this->buttonFileItem3;
		this->arrayOfFileButtons[4] = this->buttonFileItem4;
		this->arrayOfFileButtons[5] = this->buttonFileItem5;
		this->arrayOfFileButtons[6] = this->buttonFileItem6;
		this->arrayOfFileButtons[7] = this->buttonFileItem7;
		this->arrayOfFileButtons[8] = this->buttonFileItem8;

		this->arrayOfFilenameLabels[0] = this->label_filename0;
		this->arrayOfFilenameLabels[1] = this->label_filename1;
		this->arrayOfFilenameLabels[2] = this->label_filename2;
		this->arrayOfFilenameLabels[3] = this->label_filename3;
		this->arrayOfFilenameLabels[4] = this->label_filename4;
		this->arrayOfFilenameLabels[5] = this->label_filename5;
		this->arrayOfFilenameLabels[6] = this->label_filename6;
		this->arrayOfFilenameLabels[7] = this->label_filename7;
		this->arrayOfFilenameLabels[8] = this->label_filename8;


		this->arrayOfdBLables[0] = this->label_dB_0;
		this->arrayOfdBLables[1] = this->label_dB_1;
		this->arrayOfdBLables[2] = this->label_dB_2;
		this->arrayOfdBLables[3] = this->label_dB_3;
		this->arrayOfdBLables[4] = this->label_dB_4;
		this->arrayOfdBLables[5] = this->label_dB_5;
		this->arrayOfdBLables[6] = this->label_dB_6;
		this->arrayOfdBLables[7] = this->label_dB_7;
		this->arrayOfdBLables[8] = this->label_dB_8;



		this->arrayOfRemoveItemButtons[0] = this->buttonRemoveItem_0;
		this->arrayOfRemoveItemButtons[1] = this->buttonRemoveItem_1;
		this->arrayOfRemoveItemButtons[2] = this->buttonRemoveItem_2;
		this->arrayOfRemoveItemButtons[3] = this->buttonRemoveItem_3;
		this->arrayOfRemoveItemButtons[4] = this->buttonRemoveItem_4;
		this->arrayOfRemoveItemButtons[5] = this->buttonRemoveItem_5;
		this->arrayOfRemoveItemButtons[6] = this->buttonRemoveItem_6;
		this->arrayOfRemoveItemButtons[7] = this->buttonRemoveItem_7;
		this->arrayOfRemoveItemButtons[8] = this->buttonRemoveItem_8;


		for (int i = 0; i < this->maxNumOfItems; i++) {
			this->listDisableOnPlayback << this->arrayOfFileButtons[i];
			this->listDisableOnPlayback << this->arrayOfRemoveItemButtons[i];
		}
		this->listDisableOnPlayback << this->button_shuffle;


	}


	void myCentralWidget::connectButtons()
	{

		// buttons for item selection
		QSignalMapper *signalMapperButtonItem = new QSignalMapper(this);
		connect(signalMapperButtonItem, SIGNAL(mapped(int)), this, SIGNAL(buttonItemEvent(int)));

		for (int i = 0; i < this->maxNumOfItems; ++i) {
			signalMapperButtonItem->setMapping(this->arrayOfButtons[i], i);
			connect(this->arrayOfButtons[i], SIGNAL(clicked()), signalMapperButtonItem, SLOT(map()));
		}

		connect(this, SIGNAL(buttonItemEvent(int)), this, SLOT(buttonItemClicked(int)));


		// buttons for file selection
		QSignalMapper *signalMapperButtonFileItem = new QSignalMapper(this);
		connect(signalMapperButtonFileItem, SIGNAL(mapped(int)), this, SIGNAL(buttonFileItemEvent(int)));

		for (int i = 0; i < this->maxNumOfItems; ++i) {
			signalMapperButtonFileItem->setMapping(this->arrayOfFileButtons[i], i);
			connect(this->arrayOfFileButtons[i], SIGNAL(clicked()), signalMapperButtonFileItem, SLOT(map()));
		}

		connect(this, SIGNAL(buttonFileItemEvent(int)), this, SLOT(selectFileItem(int)));


		// buttons for removel of items
		QSignalMapper *signalMapperButtonRemoveItem = new QSignalMapper(this);
		connect(signalMapperButtonRemoveItem, SIGNAL(mapped(int)), this, SIGNAL(buttonRemoveItemEvent(int)));

		for (int i = 0; i < this->maxNumOfItems; ++i) {
			signalMapperButtonRemoveItem->setMapping(this->arrayOfRemoveItemButtons[i], i);
			connect(this->arrayOfRemoveItemButtons[i], SIGNAL(clicked()), signalMapperButtonRemoveItem, SLOT(map()));
		}

		connect(this, SIGNAL(buttonRemoveItemEvent(int)), this, SLOT(buttonRemoveItemClicked(int)));



		setAcceptDrops(true);

		connect(this->buttonPlayPause, SIGNAL(clicked()), this, SLOT(buttonPlayPauseClicked()));

		connect(this->buttonStop, SIGNAL(clicked()), this, SLOT(buttonStopClicked()));

		connect(this->button_hideShow, SIGNAL(clicked()), this, SLOT(button_hideShow_clicked()));

		connect(this->button_shuffle, SIGNAL(clicked()), this, SLOT(shuffleTestItems()));

	}


	void myCentralWidget::connectVolume()
	{
		for (int i = 0; i < this->maxNumOfItems; i++)
		{
			this->arrayOfVolumeSpinBox[i]->setValue(this->defaultVolumeUI);
		}

		// VolumeSpinBox
		QSignalMapper *signalMapperVolumeSpinBox = new QSignalMapper(this);
		connect(signalMapperVolumeSpinBox, SIGNAL(mapped(int)), this, SIGNAL(volumeSpinBoxEvent(int)));

		for (int i = 0; i < this->maxNumOfItems; ++i) {
			signalMapperVolumeSpinBox->setMapping(this->arrayOfVolumeSpinBox[i], i);
			connect(this->arrayOfVolumeSpinBox[i], SIGNAL(valueChanged(int)), signalMapperVolumeSpinBox, SLOT(map()));
		}

		connect(this, SIGNAL(volumeSpinBoxEvent(int)), this, SLOT(volumeSpinboxChanged(int)));


		// VolumeSlider
		QSignalMapper *signalMapperVolumeSlider = new QSignalMapper(this);
		connect(signalMapperVolumeSlider, SIGNAL(mapped(int)), this, SIGNAL(volumeSliderEvent(int)));

		for (int i = 0; i < this->maxNumOfItems; ++i) {
			signalMapperVolumeSlider->setMapping(this->arrayOfVolumeSlider[i], i);
			connect(this->arrayOfVolumeSlider[i], SIGNAL(valueChanged(int)), signalMapperVolumeSlider, SLOT(map()));
		}

		connect(this, SIGNAL(volumeSliderEvent(int)), this, SLOT(volumeSliderChanged(int)));
	}


	void myCentralWidget::inform_bootup_complete(jvxBool* wantsToAdjustSize)
	{
		jvxCallManagerProperties callGate;
		jvxSize i;
		jvxSize selId;
		jvxState stat = JVX_STATE_NONE;
		jvxErrorType resL = JVX_NO_ERROR;
		jvx_propertyReferenceTriple theTmpTriple;
		jvxSelectionList selLst;
		jvxSize num = 0;
		jvxSize idx = 0;
		std::string txt, txt2;
		jvxApiString fldStr;
		mainCentral_host_ww::inform_bootup_complete(wantsToAdjustSize);
		IjvxReport* theReport = NULL;
		jvxSize idxPlanB = JVX_SIZE_UNSELECTED;
		jvxCBitField prio;

		resL = this->theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
		assert(resL == JVX_NO_ERROR);

		// ===================================================================================
		// Make sure that audio node component is in initial state
		stat = JVX_STATE_INIT;
		resL = this->theHostRef->state_selected_component(JVX_COMPONENT_AUDIO_NODE, &stat);
		while (stat != JVX_STATE_NONE)
		{
			jvxComponentIdentification tp(JVX_COMPONENT_AUDIO_NODE);
			this->theHostRef->unselect_selected_component(tp);
			resL = this->theHostRef->state_selected_component(JVX_COMPONENT_AUDIO_NODE, &stat);
			assert(resL == JVX_NO_ERROR);
		}

		// ===================================================================================

		// Technology into selected state so that we can select sub-technolology
		resL = this->theHostRef->state_selected_component(JVX_COMPONENT_AUDIO_TECHNOLOGY, &stat);
		if (stat == JVX_STATE_ACTIVE)
		{
			this->theHostRef->deactivate_selected_component(JVX_COMPONENT_AUDIO_TECHNOLOGY);
		}

		resL = this->theHostRef->state_selected_component(JVX_COMPONENT_AUDIO_TECHNOLOGY, &stat);
		assert(stat == JVX_STATE_SELECTED);

		// ===================================================================================
		// Addrss audio technology properties
		jvx_getReferencePropertiesObject(theHostRef, &theTmpTriple, JVX_COMPONENT_AUDIO_TECHNOLOGY);

		// Auto setup port audio technology 
		if (jvx_isValidPropertyReferenceTriple(&theTmpTriple))
		{
			resL = jvx_get_property(theTmpTriple.theProps, &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, false, "/JVX_GENW_TECHNOLOGIES",
				callGate);
			bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(resL, callGate.access_protocol, "/JVX_GENW_TECHNOLOGIES", theTmpTriple.theProps);
			assert(rr);

			idx = JVX_SIZE_UNSELECTED;
			txt = "JVX Port Audio Technology*";
			for (i = 0; i < selLst.strList.ll(); i++)
			{
				txt2 = selLst.strList.std_str_at(i);
				if (jvx_compareStringsWildcard(txt, txt2))
				{
					idx = i;
					break;
				}
			}

			assert(JVX_CHECK_SIZE_SELECTED(idx));
			jvx_bitZSet(selLst.bitFieldSelected(), idx);
			resL = jvx_set_property(theTmpTriple.theProps, &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, true, "/JVX_GENW_TECHNOLOGIES", callGate);

			jvx_returnReferencePropertiesObject(theHostRef, &theTmpTriple, JVX_COMPONENT_AUDIO_TECHNOLOGY);
		}

		// ===================================================================================
		// Activate audio technology
		resL = this->theHostRef->activate_selected_component(JVX_COMPONENT_AUDIO_TECHNOLOGY);
		assert(resL == JVX_NO_ERROR);

		jvx_bitFClear(prio);
		//jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTIES_SHIFT);
		jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);

		theReport->report_command_request(prio);

		// Select the default audio device on this platform
		this->theHostRef->number_components_system(JVX_COMPONENT_AUDIO_DEVICE, &num);
		assert(resL == JVX_NO_ERROR);

		idx = JVX_SIZE_UNSELECTED;
#ifdef JVX_OS_WINDOWS
		txt = "*Microsoft*Sound*apper*Output*";
#else
		txt = "Unknown Device";
#endif
		for (i = 0; i < num; i++)
		{
			resL = this->theHostRef->description_component_system(JVX_COMPONENT_AUDIO_DEVICE, i, &fldStr);
			assert(resL == JVX_NO_ERROR);

			txt2 = fldStr.std_str();
			if (jvx_compareStringsWildcard(txt, txt2))
			{
				idx = i;
			}
			if (txt2 == "File Only")
			{
				idxPlanB = i;
			}
		}

		if (JVX_CHECK_SIZE_UNSELECTED(idx))
			idx = idxPlanB;

		if (JVX_CHECK_SIZE_UNSELECTED(idx))
		{
			// If no valid audio device was found, we printout all and exit
			std::cout << "Unable to find a valid output device matching wildcard " << txt << ". The following were found: " << std::endl;
			for (i = 0; i < num; i++)
			{
				resL = this->theHostRef->description_component_system(JVX_COMPONENT_AUDIO_DEVICE, i, &fldStr);
				assert(resL == JVX_NO_ERROR);

				txt2 = fldStr.std_str();
				std::cout << "Device #" << i << ": " << txt2 << std::endl;
			}
			assert(0);
		}

		// Select the default audio device
		jvxComponentIdentification tp(JVX_COMPONENT_AUDIO_DEVICE);

		this->theHostRef->select_component(tp, idx);
		assert(resL == JVX_NO_ERROR);

		// Activate the default audio device
		this->theHostRef->activate_selected_component(JVX_COMPONENT_AUDIO_DEVICE);
		assert(resL == JVX_NO_ERROR);

		jvx_bitFClear(prio);
		//jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTIES_SHIFT);
		jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
		theReport->report_command_request(prio);

		// ===================================================================
		// Select and activate the target audio node
		resL = this->theHostRef->number_components_system(JVX_COMPONENT_AUDIO_NODE, &num);
		assert(resL == JVX_NO_ERROR);

		idx = JVX_SIZE_UNSELECTED;

		// Name of the algorithm component
		txt = "Algorithm Multichannel Audio Studio";
		for (i = 0; i < num; i++)
		{
			resL = this->theHostRef->description_component_system(JVX_COMPONENT_AUDIO_NODE, i, &fldStr);
			assert(resL == JVX_NO_ERROR);

			txt2 = fldStr.std_str();
			if (jvx_compareStringsWildcard(txt, txt2))
			{
				idx = i;
				break;
			}
		}

		assert(JVX_CHECK_SIZE_SELECTED(idx));
		tp.tp = JVX_COMPONENT_AUDIO_NODE;
		this->theHostRef->select_component(tp, idx);
		assert(resL == JVX_NO_ERROR);

		this->theHostRef->activate_selected_component(JVX_COMPONENT_AUDIO_NODE);
		assert(resL == JVX_NO_ERROR);

		jvx_bitFClear(prio);
		//jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTIES_SHIFT);
		jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
		theReport->report_command_request(prio);

		// =============================================================
		// Application specific stuff
		// =============================================================

		theHostRef->request_hidden_interface_selected_component(JVX_COMPONENT_AUDIO_TECHNOLOGY, JVX_INTERFACE_EXTERNAL_AUDIO_CHANNELS, reinterpret_cast<jvxHandle**>(&theExtRefChannels));

		// this->activateAndUpdateAppChannels();

		this->turnOffDeviceInputChannels();
		// this->turnOnAllDeviceOutputChannels();

		jvx_bitFClear(prio);
		jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
		theReport->report_command_request(prio);
	}


	void myCentralWidget::attachAudioInputReferences()
	{
		for (int item = 0; item < vectorOfItems.size(); item++)
		{
			jvxSize numInChannels = vectorOfItems.at(item).numChannels;
			jvxSize sampleRate = vectorOfItems.at(item).sampleRate;
			this->vecOfInputHandlers.at(item)->connectIf(theExtRefChannels, numInChannels, sampleRate, "App Input Channel");
		}
	}


	void myCentralWidget::detachAudioInputReferences()
	{
		for (int item = 0; item < vectorOfItems.size(); item++)
		{
			this->vecOfInputHandlers.at(item)->disconnectIf();
		}
	}



	void myCentralWidget::turnOnAllInternalInputChannels()
	{
		jvxSize i;
		jvxErrorType resL = JVX_NO_ERROR;
		jvxBool thereisOutput = false;
		jvxSelectionList selLst;
		jvxCallManagerProperties callGate;
		ident.reset("/system/sel_input_channels");
		trans.reset();
		resL = thePropRef_dev->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(resL, callGate.access_protocol, "/system/sel_input_channels", thePropRef_dev);
		assert(rr);


		jvx_bitZero(selLst.bitFieldSelected());

		std::vector<std::string> newLst;
		jvxApiStringList newJvxStrLst;
		for (i = 0; i < selLst.strList.ll(); i++)
		{
			std::string cName_short = selLst.strList.std_str_at(i);
			std::string nmChan;
			int idDev = -1;
			jvxGenericWrapperChannelType purpChan = JVX_GENERIC_WRAPPER_CHANNEL_TYPE_NONE;
			resL = jvx_genw_decodeChannelName(cName_short, purpChan, nmChan, idDev);
			if (purpChan == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL)
			{
				jvx_bitSet(selLst.bitFieldSelected(), i);
			}
		}

		ident.reset("/system/sel_input_channels");
		trans.reset(true);
		resL = thePropRef_dev->set_property(callGate, jPRG(
			&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		rr = JVX_CHECK_PROPERTY_ACCESS_OK(resL, callGate.access_protocol, "/system/sel_input_channels", thePropRef_dev);
		assert(rr);
	}


	void myCentralWidget::turnOnAllDeviceOutputChannels()
	{

		jvxSize i;
		jvxErrorType resL = JVX_NO_ERROR;
		jvxBool thereisOutput = false;
		jvxSelectionList selLst;
		jvxCallManagerProperties callGate;
		ident.reset("/system/sel_output_channels");
		trans.reset();
		resL = thePropRef_dev->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(resL, callGate.access_protocol, "/system/sel_output_channels", thePropRef_dev);
		assert(rr);

		std::vector<std::string> newLst;
		jvxApiStringList newJvxStrLst;
		for (i = 0; i < selLst.strList.ll(); i++)
		{
			std::string cName_short = selLst.strList.std_str_at(i);
			std::string nmChan;
			int idDev = -1;
			jvxGenericWrapperChannelType purpChan = JVX_GENERIC_WRAPPER_CHANNEL_TYPE_NONE;
			resL = jvx_genw_decodeChannelName(cName_short, purpChan, nmChan, idDev);
			if (purpChan == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
			{
				jvx_bitSet(selLst.bitFieldSelected(), i);
			}
		}
		
		ident.reset("/system/sel_output_channels");
		trans.reset(true);
		resL = thePropRef_dev->set_property(callGate, jPRG(
			&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST),ident, trans);
		rr = JVX_CHECK_PROPERTY_ACCESS_OK(resL, callGate.access_protocol, "/system/sel_output_channels", thePropRef_dev);
		assert(rr);

	}


	void 
		myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
	{
		mainCentral_host_ww::inform_active(tp, theProps);
		switch (tp.tp)
		{
		case JVX_COMPONENT_AUDIO_NODE:
			
			thePropRef_algo = theProps;
			myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), thePropRef_algo, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "audio_node");
			break;
		case JVX_COMPONENT_AUDIO_DEVICE:
			
			thePropRef_dev = theProps;
			break;
		default:
			break;
		}
	}


	void 
myCentralWidget::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
	{
		switch (tp.tp)
		{
		case JVX_COMPONENT_AUDIO_NODE:
			
			myWidgetWrapper.deassociateAutoWidgets("audio_node");
			thePropRef_algo = NULL;
			break;
		case JVX_COMPONENT_AUDIO_DEVICE:
			thePropRef_dev = NULL;
			break;
		default:
			break;
		}

		// Must follow, not precede!
		mainCentral_host_ww::inform_inactive(tp, theProps);
	}


	void 
	myCentralWidget::inform_update_window(jvxCBitField prio)
	{
		std::string propDescr;
		jvxErrorType res = JVX_NO_ERROR;
		mainCentral_host_ww::inform_update_window(prio);
		if (jvx_bitTest(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT))
		{
			if (thePropRef_algo)
			{
				myWidgetWrapper.deassociateAutoWidgets("audio_node");
				myWidgetWrapper.associateAutoWidgets(static_cast<QWidget*>(this), thePropRef_algo, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "audio_node");
				//update_window();
			}
		}
		myWidgetWrapper.trigger_updateWindow("audio_node");
	}


	void myCentralWidget::inform_update_window_periodic()
	{
		mainCentral_host_ww::inform_update_window_periodic();

	}


	void myCentralWidget::inform_about_to_shutdown()
	{
		mainCentral_host_ww::inform_about_to_shutdown();
		myWidgetWrapper.deassociateAutoWidgets("audio_node");
	}


	jvxSize myCentralWidget::getNumberOfActiveOutputChannels()
	{

		jvxSelectionList selLst;
		jvxCallManagerProperties callGate;
		ident.reset("/system/sel_output_channels");
		trans.reset(true);
		jvxErrorType res = thePropRef_dev->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		assert(res == JVX_NO_ERROR);

		jvxSize numChannels = selLst.strList.ll();
		jvxSize numActiveChannels = 0;
		for (int i = 0; i < numChannels; i++) {
			numActiveChannels += jvx_bitTest(selLst.bitFieldSelected(), i);
		}

		return numActiveChannels;
	}


	void myCentralWidget::turnOffDeviceInputChannels()
	{
		jvxSelectionList selLst;
		jvxCallManagerProperties callGate;
		if (thePropRef_dev)
		{
			ident.reset("/system/sel_input_channels");
			trans.reset(true);
			jvxErrorType res = thePropRef_dev->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);

			assert(res == JVX_NO_ERROR);

			jvxSize numChannels = selLst.strList.ll();

			for (int i = 0; i < numChannels; i++) {
				jvx_bitClear(selLst.bitFieldSelected(), i);
			}

			ident.reset("/system/sel_input_channels");
			trans.reset(true);
			res = res = thePropRef_dev->set_property(callGate, jPRG( 
				&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST),ident, trans);
			assert(res == JVX_NO_ERROR);
		}
	}


	void myCentralWidget::inform_sequencer_about_to_start()
	{

		jvxCBitField prio;
		mainCentral_host_ww::inform_sequencer_about_to_start();

		this->setFocus();

		jvx_bitFClear(prio);
		jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
		this->inform_update_window(prio);

		this->turnOffDeviceInputChannels();
		this->attachAudioInputReferences();
		
		// If we have change the settings for the channels, we need to test all the chains to inform
		// all connected components IMMEDIATELY.
		// This also happens automatically BUT it is always delayed if caused by the master itself.
		// Since we have already started the process by pushing the button a delayed test would be too late
		// to take effect.
		mwCtrlRef->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_IMMEDIATE_TEST_ALL_CHAINS, NULL);

		for (int i = 0; i < this->vectorOfItems.size(); i++) {

			this->vecOfInputHandlers.at(i)->setPassOnlyFullFrames(false);
		}


		for (int item = 0; item < this->vectorOfItems.size(); item++) {
			jvxSize const readOffset  = this->vecOfInputHandlers.at(item)->getReadOffset();
			this->vecOfInputHandlers.at(item)->setBuffer((jvxHandle**)&(vectorOfItems.at(item).pointersToItemSignalChannels[0]), JVX_DATAFORMAT_DATA, vectorOfItems[item].lengthSignal);
			this->vecOfInputHandlers.at(item)->setReadOffset(readOffset);
		}


		this->setCurrentPlayTime(this->lastCurrentPlayTimeInSec);

		this->updateInputHandlerLoops();

		this->setPlayProperties(this->currentTestItem);

		this->setPlayMode(true);

	}


	jvxSize myCentralWidget::getInputChannelOffset(int indexItem)
	{
		assert(indexItem < vectorOfItems.size());

		jvxSize channel_offset = 0;

		for (int i = 0; i < indexItem; i++)
		{
			channel_offset += vectorOfItems.at(i).numChannels;
		}
		return channel_offset;
	}


	void myCentralWidget::inform_sequencer_stopped()
	{


		mainCentral_host_ww::inform_sequencer_stopped();
		jvxSize readOffset[MAX_NUMBER_OF_ITEMS];


		// save readOffset
		for (int i = 0; i < this->vectorOfItems.size(); i++) {
			readOffset[i] = vecOfInputHandlers.at(i)->getReadOffset();
		}

		detachAudioInputReferences();
		
		for (int i = 0; i < this->vectorOfItems.size(); i++) {
			 vecOfInputHandlers.at(i)->setReadOffset(readOffset[i]);
		}

		outputHandler->resetBuffer();

		setPlayMode(false);
	}




	void myCentralWidget::dragEnterEvent(QDragEnterEvent* e)
	{

		if (e->mimeData()->hasUrls() && vectorOfItems.size() < this->maxNumOfItems && !this->audioIsPlaying)
		{
			e->acceptProposedAction();
		}
	}


	void myCentralWidget::dropEvent(QDropEvent* e)
	{

		// set hourglass cursor 

		QApplication::setOverrideCursor(Qt::WaitCursor);



		foreach(const QUrl &url, e->mimeData()->urls()) {

			if (vectorOfItems.size() >= this->maxNumOfItems)
				break;

			QString const fileName = url.toLocalFile();

			if (isWavFile(fileName)) {
				int const itemIndex = vectorOfItems.size();
				addNewTestItem(itemIndex, fileName);

			}
			this->updateEnableDisableItemWidgets();

		}

		// reset to normal cursor
		QApplication::restoreOverrideCursor();
		this->updateLoopAndPlaySliderValues();
	}



	bool myCentralWidget::isWavFile(QString fileName)
	{
		QMimeDatabase db;

		QMimeType mimeType = db.mimeTypeForFile(fileName, QMimeDatabase::MatchContent);
		if (mimeType.inherits("audio/x-wav")) {
			return true;
		}
		else {
			return false;
		}
	}



	void myCentralWidget::updateEnableDisableItemWidgets() {

		// enable all elements for stopped mode

		for (int itemIndex = 0; itemIndex < this->vectorOfItems.size(); itemIndex++) {
			// enable buttons in UI
			this->arrayOfButtons[itemIndex]->setEnabled(true);
			this->arrayOfVolumeSpinBox[itemIndex]->setEnabled(true);
			this->arrayOfVolumeSlider[itemIndex]->setEnabled(true);
			this->arrayOfdBLables[itemIndex]->setEnabled(true);
			this->arrayOfRemoveItemButtons[itemIndex]->setEnabled(true);
			this->arrayOfFileButtons[itemIndex]->setEnabled(true);
		}

		for (int itemIndex = this->vectorOfItems.size(); itemIndex < this->maxNumOfItems; itemIndex++) {
			this->arrayOfButtons[itemIndex]->setEnabled(false);
			this->arrayOfVolumeSpinBox[itemIndex]->setEnabled(false);
			this->arrayOfVolumeSlider[itemIndex]->setEnabled(false);
			this->arrayOfdBLables[itemIndex]->setEnabled(false);
			this->arrayOfRemoveItemButtons[itemIndex]->setEnabled(false);
			this->arrayOfFileButtons[itemIndex]->setEnabled(false);
		}

		// select file button for next (unloaded) item
		if (this->vectorOfItems.size() < this->maxNumOfItems)
			this->arrayOfFileButtons[this->vectorOfItems.size()]->setEnabled(true);

		this->button_shuffle->setEnabled(true);


		// disable elements in play mode
		if (this->audioIsPlaying) {
			foreach(QWidget *w, this->listDisableOnPlayback)
				w->setEnabled(false);
		}
	}

	void myCentralWidget::addNewTestItem(int itemIndex, QString fileName)
	{
		TestItem newItem;
		newItem.itemIndex = itemIndex;
		newItem.filepath = fileName;
		newItem.volume = 1;

		ItemSignals newSignals;
		int signalsIndex;

		assert(itemIndex <= vectorOfItems.size());

		// check if item already exists
		if (itemIndex == vectorOfItems.size())
		{
			// create new item

			this->vectorOfItems.push_back(newItem);

			if (this->currentTestItem == -1) {
				// this is the first item
				this->currentTestItem = 0;
			}

		}
		else
		{
			// remove old signal from list
			listOfItemSignals.erase(vectorOfItems.at(itemIndex).iterItemSignal);

			// replace old item
			vectorOfItems.at(itemIndex) = newItem;
		}


		ItemSignals newSignal;

		this->readAudioFile(vectorOfItems.at(itemIndex), newSignal);
		
		this->listOfItemSignals.push_back(newSignal);

		std::list<ItemSignals>::iterator iterNewItemSignal = this->listOfItemSignals.end();
		iterNewItemSignal--;

		vectorOfItems.at(itemIndex).iterItemSignal = iterNewItemSignal;

		jvxData const playPos = getCurrentPlayTime();
		jvxSize readOffset = playPos * vectorOfItems.at(itemIndex).sampleRate;
		this->vecOfInputHandlers.at(itemIndex)->setReadOffset(readOffset);

		this->updateMaxSignalLength();
		this->updateInputHandlerLoops();

		this->updatePointersToSignals();

		this->updatePlaybackControlWidgets();
		this->updateActiveLabels();
		this->updateFilenameLabelDisplay();
		this->updateVolumeDisplay();
		this->updateMaxSignalLength();

	}


	void myCentralWidget::updatePointersToSignals() {
		// update pointers to signal arrays in vectors
		 
		for (int itemIndex = 0; itemIndex < vectorOfItems.size(); itemIndex++) {
			vectorOfItems.at(itemIndex).pointersToItemSignalChannels.clear();

			for (int channel = 0; channel < vectorOfItems.at(itemIndex).numChannels; channel++) {

				jvxData* ptrSignal = &((*vectorOfItems.at(itemIndex).iterItemSignal)[channel][0]);
				vectorOfItems.at(itemIndex).pointersToItemSignalChannels.push_back(ptrSignal);
			}
		}
	}


	void myCentralWidget::updateActiveLabels() {

		for (int itemIndex = 0; itemIndex < this->maxNumOfItems; itemIndex++) {
			this->arrayOfLabels[itemIndex]->setText(tr(""));
		}

		if (this->currentTestItem >= 0) {
			this->arrayOfLabels[currentTestItem]->setText(tr("Active"));
		}
	}



	void myCentralWidget::updateMaxSignalLength()
	{
		// get duration of longest signal

		jvxData maxDuration = 0;
		longestSignalItem = -1;

		for (int i = 0; i < this->vectorOfItems.size(); i++)
		{
			maxDuration = JVX_MAX(vectorOfItems.at(i).durationInSec, maxDuration);
			longestSignalItem = i;
		}

		

		this->maxSignalLengthInSec = maxDuration;

		// set Text of label for Signal Length
		this->label_signalLength->setText(createQStringForTime(maxDuration));

		// set B to maxDuration if slider is maximized
		const int sliderValue = this->horizontalSlider_loopEnd->value();
		if (sliderValue == this->horizontalSlider_loopEnd->maximum()) {
			setLoopEnd(maxDuration);
		}
		else if (this->loopEndInSec > this->maxSignalLengthInSec) {
			setLoopEnd(maxSignalLengthInSec);
		}

		if (this->loopStartInSec > this->maxSignalLengthInSec) {
			setLoopStart(0);
		}

		// updateLoopSliderValues();
		
	}


	void myCentralWidget::updateLoopAndPlaySliderValues() {

		horizontalSlider_loopStart->blockSignals(true);
		horizontalSlider_loopStart->setValue(getSliderValueFromDuration(this->loopStartInSec));
		horizontalSlider_loopStart->blockSignals(false);

		if (this->maxSignalLengthInSec > 0.0) {
			horizontalSlider_loopEnd->blockSignals(true);
			horizontalSlider_loopEnd->setValue(getSliderValueFromDuration(this->loopEndInSec));
			horizontalSlider_loopEnd->blockSignals(false);
		}
		else
		{
			horizontalSlider_loopEnd->setValue(horizontalSlider_loopEnd->maximum());
		}

		horizontalSlider_curPlayPos->blockSignals(true);
		horizontalSlider_curPlayPos->setValue(getSliderValueFromDuration(this->getCurrentPlayTime()));
		horizontalSlider_curPlayPos->blockSignals(false);
		
	}



	void myCentralWidget::updateFilenameLabelDisplay()
	{
		for (int j = 0; j < this->maxNumOfItems; j++)
		{
			arrayOfFilenameLabels[j]->setText("");
		}

		for (int i = 0; i < vectorOfItems.size(); i++)
		{
			if (blindModeActivated == false)
			{
				arrayOfFilenameLabels[i]->setText(vectorOfItems.at(i).filepath);
			}
			else
			{
				arrayOfFilenameLabels[i]->setText("?");
			}
		}
	}



	void myCentralWidget::updateVolumeDisplay()
	{
		int value;

		for (int i = 0; i < this->maxNumOfItems; i++) {

			if (i < this->vectorOfItems.size()) {
				value = (int)20 * log10(vectorOfItems[i].volume);
			}
			else {
				value = this->defaultVolumeUI;
			}

			bool blockState = arrayOfVolumeSlider[i]->blockSignals(true);
			this->arrayOfVolumeSlider[i]->setValue(value);
			arrayOfVolumeSlider[i]->blockSignals(blockState);

			blockState = arrayOfVolumeSpinBox[i]->blockSignals(true);
			this->arrayOfVolumeSpinBox[i]->setValue(value);
			arrayOfVolumeSpinBox[i]->blockSignals(blockState);

			// hide/show volume control widgets
			arrayOfVolumeSpinBox[i]->setVisible(!blindModeActivated);
			arrayOfVolumeSlider[i]->setVisible(!blindModeActivated);
			arrayOfdBLables[i]->setVisible(!blindModeActivated);

		}
	}



	void myCentralWidget::setPlayMode(bool on)
	{
		this->audioIsPlaying = on;

		if (on) {
			this->timeDisplayUpdateTimer->start();
			this->clippingUpdateTimer->start();
		}
		else {
			this->timeDisplayUpdateTimer->stop();
			this->clippingUpdateTimer->stop();
			this->lastCurrentPlayTimeInSec = getDurationFromSliderValue(horizontalSlider_curPlayPos->value());
		}



		updateEnableDisableItemWidgets();
	}


	QString	myCentralWidget::createQStringForTime(jvxData durationInSec)
	{
		return QDateTime::fromTime_t(durationInSec).toString("mm:ss");
	}


	bool myCentralWidget::eventFilter(QObject *ob, QEvent *e)
	{

		if (e->type() == QEvent::KeyPress) {
			const QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);


			switch (keyEvent->key())
			{
			case Qt::Key_Space:

				buttonPlayPauseClicked();
				return true;

			case Qt::Key_A:

				if (keyEvent->modifiers() == Qt::ShiftModifier) {
					resetLoopStartKeyPress();
				}
				else {
					setLoopStartKeyPress(); // set loop start
				}
				return true;

			case Qt::Key_B:
				if (keyEvent->modifiers() == Qt::ShiftModifier) {
					resetLoopEndKeyPress();
				}
				else {
					setLoopEndKeyPress();
				}
				return true;
			}
		}
		return QWidget::eventFilter(ob, e);
	}



	void myCentralWidget::keyPressEvent(QKeyEvent* event)
	{

		int key = event->key();

		switch (key)
		{
		case Qt::Key_1:
			buttonItemClicked(0);
			break;
		case Qt::Key_2:
			buttonItemClicked(1);
			break;
		case Qt::Key_3:
			buttonItemClicked(2);
			break;
		case Qt::Key_4:
			buttonItemClicked(3);
			break;
		case Qt::Key_5:
			buttonItemClicked(4);
			break;
		case Qt::Key_6:
			buttonItemClicked(5);
			break;
		case Qt::Key_7:
			buttonItemClicked(6);
			break;
		case Qt::Key_8:
			buttonItemClicked(7);
			break;
		case Qt::Key_9:
			buttonItemClicked(8);
			break;

		case Qt::Key_Space:
			buttonPlayPauseClicked();
			break;

		case Qt::Key_A:

			if (event->modifiers() == Qt::ShiftModifier) {
				resetLoopStartKeyPress();
			}
			else {
				setLoopStartKeyPress(); // set loop start
			}
			break;

		case Qt::Key_B:
			if (event->modifiers() == Qt::ShiftModifier) {
				resetLoopEndKeyPress();
			}
			else {
				setLoopEndKeyPress();
			}
			break;
		}
	}



	void myCentralWidget::resetHost()
	{

		vectorOfItems.clear();

		this->buttonPlayPause->setEnabled(false);
		this->buttonStop->setEnabled(false);

		lastCurrentPlayTimeInSec = 0;
		currentTestItem = -1;

		for (int i = 0; i < this->maxNumOfItems; i++)
		{
			arrayOfButtons[i]->setEnabled(false);
			arrayOfdBLables[i]->setEnabled(false);
			arrayOfVolumeSlider[i]->setEnabled(false);
			arrayOfVolumeSlider[i]->setValue(this->defaultVolumeUI);
			arrayOfVolumeSpinBox[i]->setEnabled(false);
			arrayOfVolumeSpinBox[i]->setValue(this->defaultVolumeUI);
			arrayOfRemoveItemButtons[i]->setEnabled(false);

			

			if (i != 0)
			{
				arrayOfFileButtons[i]->setEnabled(false);
			}
		}

		updateActiveLabels();
		updateFilenameLabelDisplay();
		updateVolumeDisplay();
	}


	void myCentralWidget::updatePlaybackControlWidgets()
	{
		bool activateControls = (vectorOfItems.size() > 0);

		this->buttonPlayPause->setEnabled(activateControls);
		this->buttonStop->setEnabled(activateControls);

		horizontalSlider_loopEnd->setEnabled(activateControls);
		horizontalSlider_loopStart->setEnabled(activateControls);
		horizontalSlider_curPlayPos->setEnabled(activateControls);
	}


	void myCentralWidget::buttonItemClicked(int itemIndex)
	{
		jvxCBitField prio;
		if (!this->arrayOfButtons[itemIndex]->isEnabled())
			return;

		if (this->currentTestItem == itemIndex)
			return;

		this->currentTestItem = itemIndex;


		this->setPlayProperties(itemIndex);
		
		this->updateActiveLabels();
		
		jvx_bitFClear(prio);
		jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
		inform_update_window(prio);

	}


	void myCentralWidget::setPlayProperties(int itemIndex)
	{
		jvxCallManagerProperties callGate;
		jvxSize props_dirty = 1;

		// Wait (if necessary) till former call of setProperties has been handled by library.
		//while (props_dirty && this->audioIsPlaying)
		//{
		//	jvxErrorType const res = thePropRef_algo->get_property(callGate, jPRG(&props_dirty, 0, 1, JVX_DATAFORMAT_SIZE, true, "/play_props_dirty");
		//	assert(res == JVX_NO_ERROR);
		//}

		props_dirty = 1;
		jvxData volume = 0;
		jvxSize num_channels = 0;
		jvxSize input_channel_offset = 0;


		if (itemIndex >= 0)
		{
			volume = this->vectorOfItems.at(this->currentTestItem).volume;
			num_channels = this->vectorOfItems.at(this->currentTestItem).numChannels;
			input_channel_offset = getInputChannelOffset(this->currentTestItem);
		}

		jvxErrorType res;
		ident.reset("/volume");
		trans.reset(true);
		res = thePropRef_algo->set_property(callGate, jPRG(
			&volume, 1, JVX_DATAFORMAT_DATA),ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, "/volume", thePropRef_algo);
		assert(rr);

		ident.reset("/input_channel_offset");
		trans.reset(true);
		res = thePropRef_algo->set_property(callGate, jPRG(
			&input_channel_offset, 1, JVX_DATAFORMAT_SIZE),ident, trans);
		rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, "/input_channel_offset", thePropRef_algo);
		assert(rr);

		ident.reset("/num_channels");
		trans.reset(true);
		res = thePropRef_algo->set_property(callGate, jPRG(
			&num_channels, 1, JVX_DATAFORMAT_SIZE),ident, trans);
		rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, "/num_channels", thePropRef_algo);
		assert(rr);

		ident.reset("/play_props_dirty");
		trans.reset(true);
		res = thePropRef_algo->set_property(callGate, jPRG(
			&props_dirty, 1, JVX_DATAFORMAT_SIZE),ident, trans);
		rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, "/play_props_dirty", thePropRef_algo);
		assert(rr);

	}


	void myCentralWidget::volumeSpinboxChanged(int itemIndex)
	{
		jvxData volume = this->arrayOfVolumeSpinBox[itemIndex]->value();

		this->arrayOfVolumeSlider[itemIndex]->setValue((int)volume);

		if (this->vectorOfItems.size() > itemIndex)
			this->newVolume(itemIndex, volume);
	}


	void myCentralWidget::volumeSliderChanged(int itemIndex)
	{
		jvxData volume = this->arrayOfVolumeSlider[itemIndex]->value();

		this->arrayOfVolumeSpinBox[itemIndex]->setValue((int)volume);

		if (this->vectorOfItems.size() > itemIndex)
			this->newVolume(itemIndex, volume);
	}


	void myCentralWidget::buttonRemoveItemClicked(int itemIndexRemove)
	{
		assert(itemIndexRemove < this->vectorOfItems.size());

		

		std::vector<TestItem>::iterator iterItem = vectorOfItems.begin() + itemIndexRemove;

		this->listOfItemSignals.erase(iterItem->iterItemSignal);
		this->vectorOfItems.erase(iterItem);

		for (int itemIndex = itemIndexRemove; itemIndex < this->vectorOfItems.size(); itemIndex++)
		{
			vectorOfItems.at(itemIndex).itemIndex--;
		}


		// update currentTestItem 
		if (vectorOfItems.size() == 0) {
			currentTestItem = -1;
		}
		else if (currentTestItem == itemIndexRemove) {
			currentTestItem = 0;
		}
		else if (currentTestItem > itemIndexRemove)
		{
			currentTestItem--;
		}

		// updatePointersToSignals();

		updateEnableDisableItemWidgets();
		updateMaxSignalLength();
		updateLoopAndPlaySliderValues();
		updateFilenameLabelDisplay();
		updateVolumeDisplay();
		updateActiveLabels();
		updatePlaybackControlWidgets();
	}



	void myCentralWidget::newVolume(int itemIndex, jvxData uiVolume)
	{
		jvxData volume = pow(10, uiVolume / 20);

		this->vectorOfItems.at(itemIndex).volume = volume;

		if (this->currentTestItem == itemIndex)
		{
			setPlayProperties(itemIndex);
		}
	}


	void myCentralWidget::readAudioFile(TestItem& newItem, ItemSignals& newSignals) {

		IjvxToolsHost* theToolsHost = NULL;
		IjvxObject* theAudioReader_obj = NULL;
		IjvxRtAudioFileReader* theAudioReader_hdl = NULL;
		jvxFileDescriptionEndpoint_open fdescro;
		jvxFileDescriptionEndpoint_prepare fdescrp;
		jvxData pp = 0;
		jvxSize i;

		this->theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theToolsHost));
		if (theToolsHost)
		{
			theToolsHost->instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_READER, &theAudioReader_obj, 0, NULL);
			if (theAudioReader_obj)
			{
				theAudioReader_obj->request_specialization(reinterpret_cast<jvxHandle**>(&theAudioReader_hdl), NULL, NULL, NULL);
				if (theAudioReader_hdl)
				{
					theAudioReader_hdl->initialize(theHostRef);
					theAudioReader_hdl->activate(newItem.filepath.toLatin1().data(),
						JVX_FILETYPE_WAV, &fdescro);
					fdescrp.dFormatAppl = JVX_DATAFORMAT_DATA;

					int readBufSize = 1024;

					fdescrp.numSamplesFrameMax_moveBgrd = readBufSize;
					theAudioReader_hdl->prepare(readBufSize, JVX_FILECONTINUE_NONE, false,
						&fdescrp, JVX_FILEOPERATION_BATCH);

					int lengthIn = fdescro.lengthFile;
					int const numChannelsToRead = fdescro.numberChannels;

					for (i = 0; i < numChannelsToRead; i++)
					{
						std::vector<jvxData> tmpSignalChannel(lengthIn, 0.0);
						// newItem.vectorOfSignals.push_back(tmpSignalChannel);
						newSignals.push_back(tmpSignalChannel);
					}



					jvxHandle** buffersTmp;
					JVX_DSP_SAFE_ALLOCATE_FIELD(buffersTmp, jvxHandle*, numChannelsToRead);

					for (i = 0; i < numChannelsToRead; i++)
					{
						buffersTmp[i] = &newSignals[i][0];
					}

					int bufIdx = 0;
					theAudioReader_hdl->start();
					while (1)
					{
						if (theAudioReader_hdl->get_audio_buffer(buffersTmp, numChannelsToRead, readBufSize, false, &pp) != JVX_NO_ERROR)
						{
							break;
						}
						bufIdx += readBufSize;

						for (i = 0; i < numChannelsToRead; i++)
						{
							buffersTmp[i] = (jvxHandle*)&newSignals[i][bufIdx];
						}

						readBufSize = JVX_MIN(lengthIn - bufIdx, readBufSize);
					}
					theAudioReader_hdl->stop();

					theAudioReader_hdl->postprocess();
					theAudioReader_hdl->deactivate();
					theAudioReader_hdl->terminate();


					newItem.lengthSignal = lengthIn;
					newItem.numChannels = numChannelsToRead;
					newItem.sampleRate = fdescro.samplerate;
					newItem.durationInSec = (jvxData)lengthIn / (jvxData)fdescro.samplerate;

				}
				this->theToolsHost->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_READER, theAudioReader_obj, 0, NULL);
			}
			this->theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theToolsHost));
		}
	}



	void myCentralWidget::selectFileItem(int itemIndex)
	{

		assert(itemIndex >= 0);
		assert(itemIndex <= this->maxNumOfItems);

		QFileDialog dialog(this);
		dialog.setNameFilter(tr("Sound Files (*.wav)"));
		dialog.setFileMode(QFileDialog::ExistingFiles);
		dialog.setDirectory(this->lastSelectFileDir);
		int dialogCode = dialog.exec();

		this->lastSelectFileDir = dialog.directory();

		if (dialogCode == QDialog::Rejected) {
			return;
		}
		QStringList filenames = dialog.selectedFiles();

		foreach(const QString &filename, filenames) {
			if (filename.isEmpty()) {
			}

			else if (!isWavFile(filename)) {
				QMessageBox messageBox;
				QString errormsg = tr("Selected file ") + filename + tr(" is not a valid wav file or cannot be read.");

				messageBox.critical(0, "Error Reading File", errormsg);
				messageBox.setFixedSize(500, 200);
			}
			else
			{

				// set hourglass cursor 
				QApplication::setOverrideCursor(Qt::WaitCursor);
				QApplication::processEvents();

				this->addNewTestItem(itemIndex, filename);

				QApplication::restoreOverrideCursor();
				itemIndex++;

				if (itemIndex >= this->maxNumOfItems) {
					break;
				}
			}
		}
		this->updateLoopAndPlaySliderValues();
		this->updateEnableDisableItemWidgets();
		this->updateActiveLabels();
	}



	void myCentralWidget::buttonPlayPauseClicked()
	{
		if (this->buttonPlayPause->isEnabled())
		{
			if (!this->audioIsPlaying)
			{
				startPlaying();
			}
			else
			{
				if (this->audioIsPaused) {
					unpausePlaying();
				}
				else {
					pausePlaying();
				}
			}
		}
	}


	void myCentralWidget::buttonStopClicked() {
		stopPlaying();
	}


	void myCentralWidget::startPlaying() {

		if (this->getNumberOfActiveOutputChannels() == 0) {
			return;
		}

		this->buttonPlayPause->setText("||");
		this->audioIsPaused = false;
		this->mwCtrlRef->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_START, NULL);
	}

	void myCentralWidget::stopPlaying() {
		this->buttonPlayPause->setText(">");
		this->mwCtrlRef->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_STOP, NULL);
	}

	void myCentralWidget::pausePlaying() {
		this->buttonPlayPause->setText(">");

		for (int i = 0; i < vectorOfItems.size(); i++)
		{
			vecOfInputHandlers.at(i)->setPause(true);
		}
		this->audioIsPaused = true;
	}

	void myCentralWidget::unpausePlaying() {
		this->buttonPlayPause->setText("||");

		for (int i = 0; i < vectorOfItems.size(); i++)
		{
			vecOfInputHandlers.at(i)->setPause(false);
		}
		this->audioIsPaused = false;
	}


	void myCentralWidget::button_hideShow_clicked()
	{
		if (blindModeActivated == false)
		{
			blindModeActivated = true;
			hideItems();
			this->button_hideShow->setText("Show Items");
		}
		else
		{
			blindModeActivated = false;
			showItems();
			this->button_hideShow->setText("Hide Items");
		}
	}


	void myCentralWidget::hideItems()
	{
		blindModeActivated = true;
		updateFilenameLabelDisplay();
		updateVolumeDisplay();
	}


	void myCentralWidget::showItems()
	{
		blindModeActivated = false;
		updateFilenameLabelDisplay();
		updateVolumeDisplay();
	}


	void myCentralWidget::shuffleTestItems()
	{
		std::vector<TestItem> shuffledList;

		jvxSize newItemIndex = 0;
		while (vectorOfItems.size() != 0)
		{
			// The item at this random position in the orignal list is going to be inserted at the end of the shuffled list
			int rndPos = rand() % vectorOfItems.size();

			std::vector<TestItem>::iterator const iterRnd = vectorOfItems.begin() + rndPos;


			//insert element in shuffled list
			shuffledList.push_back(*iterRnd);

			// erase from original list

			vectorOfItems.erase(iterRnd);

			newItemIndex++;
		}

		vectorOfItems = shuffledList;

		
		updateFilenameLabelDisplay();
		updateVolumeDisplay();
	}


	void myCentralWidget::updateTimeDisplay()
	{
		jvxData playPositionInSec;

		if (vectorOfItems.size() > 0) {
			jvxData const readPositionSamples = this->vecOfInputHandlers.at(longestSignalItem)->getReadOffset();
			jvxData const srate = (jvxData) this->vectorOfItems.at(longestSignalItem).sampleRate;
			playPositionInSec = readPositionSamples / srate;
		}
		else {
			playPositionInSec = 0;
		}

		label_playCurPos->setText(createQStringForTime(playPositionInSec));
		horizontalSlider_curPlayPos->blockSignals(true);
		horizontalSlider_curPlayPos->setValue(getSliderValueFromDuration(playPositionInSec));
		horizontalSlider_curPlayPos->blockSignals(false);
	}


	void myCentralWidget::setLoopStart(jvxData positionInSec)
	{
		// set the starting point of the loop

		this->loopStartInSec = JVX_MIN(positionInSec, this->loopEndInSec);

		const int value = this->getSliderValueFromDuration(this->loopStartInSec);

		horizontalSlider_loopStart->blockSignals(true);
		horizontalSlider_loopStart->setValue(value);
		horizontalSlider_loopStart->blockSignals(false);

		this->label_loopStart->setText(createQStringForTime(this->loopStartInSec));
		updateInputHandlerLoops();
	}


	void myCentralWidget::setLoopEnd(jvxData positionInSec)
	{
		// set the ending point of the loop

		this->loopEndInSec = JVX_MAX(positionInSec, this->loopStartInSec);

		
		int value;
		if (this->maxSignalLengthInSec > 0.0) {
			value = this->getSliderValueFromDuration(this->loopEndInSec);
			this->label_loopEnd->setText(createQStringForTime(this->loopEndInSec));
		}
		else {
			value = horizontalSlider_loopEnd->maximum();
			this->label_loopEnd->setText(createQStringForTime(0.0));
		}
	

		horizontalSlider_loopEnd->blockSignals(true);
		horizontalSlider_loopEnd->setValue(value);
		horizontalSlider_loopEnd->blockSignals(false);

		updateInputHandlerLoops();

	}

	void myCentralWidget::setLoopStartKeyPress()
	{
		if (this->longestSignalItem == -1)
			return;

		jvxData const playPositionInSec = (jvxData) this->vecOfInputHandlers.at(this->longestSignalItem)->getReadOffset() / (jvxData) this->vectorOfItems.at(this->longestSignalItem).sampleRate;

		this->setLoopStart(playPositionInSec);
	}

	void myCentralWidget::resetLoopStartKeyPress()
	{
		this->setLoopStart(0.0);
	}

	void myCentralWidget::setLoopEndKeyPress()
	{
		if (this->longestSignalItem == -1)
			return;

		jvxData const playPositionInSec = (jvxData) this->vecOfInputHandlers.at(this->longestSignalItem)->getReadOffset() / (jvxData) this->vectorOfItems.at(this->longestSignalItem).sampleRate;

		this->setLoopEnd(playPositionInSec);
	}


	void myCentralWidget::resetLoopEndKeyPress()
	{
		this->setLoopEnd(this->maxSignalLengthInSec);
	}


	jvxData myCentralWidget::getDurationFromSliderValue(int value)
	{
		jvxData duration;
		if ((jvxData) this->timeSliderMaxValue > 0) {
			duration = (jvxData)value / (jvxData) this->timeSliderMaxValue * this->maxSignalLengthInSec;
		}
		else
		{
			duration = 0;
		}
		return duration;
	}


	int myCentralWidget::getSliderValueFromDuration(jvxData duration)
	{
		int value;
		if (this->maxSignalLengthInSec > 0.0) {
			value = (int)round((jvxData) this->timeSliderMaxValue * duration / this->maxSignalLengthInSec);
		}
		else
		{
			value = 0;
		}
		return value;
	}


	void myCentralWidget::loopStartSliderMoved(int value) {
		setLoopStart(getDurationFromSliderValue(value));
	}


	void myCentralWidget::loopEndSliderMoved(int value) {
		setLoopEnd(getDurationFromSliderValue(value));
	}

	void myCentralWidget::curPlayPosSliderMoved(int value) {
		jvxData currentPlayPos = this->getDurationFromSliderValue(value);
		jvxData const safety_delta = 0.1;
		currentPlayPos = JVX_MAX(currentPlayPos, this->loopStartInSec + safety_delta);
		currentPlayPos = JVX_MIN(currentPlayPos, this->loopEndInSec - safety_delta);
		setCurrentPlayTime(currentPlayPos);
	}

	void myCentralWidget::setCurrentPlayTime(jvxData playTime) {

		for (int i = 0; i < vectorOfItems.size(); i++)
		{
			jvxSize const readPosition = playTime * (jvxData)vectorOfItems.at(i).sampleRate;

			vecOfInputHandlers.at(i)->setReadOffset(readPosition);
			
		}
		updateTimeDisplay();
		lastCurrentPlayTimeInSec = playTime;

	}

	jvxData myCentralWidget::getCurrentPlayTime()
	{
		if (vectorOfItems.size() == 0)
			return 0;

		if (longestSignalItem == -1)
			updateMaxSignalLength();

		jvxData currentPlayTime_in_sec;
		jvxData const readOffset = (jvxData)vecOfInputHandlers.at(longestSignalItem)->getReadOffset();
		currentPlayTime_in_sec = readOffset / (jvxData)vectorOfItems.at(longestSignalItem).sampleRate;


		return currentPlayTime_in_sec;
	}


	void myCentralWidget::updateInputHandlerLoops() {
		for (int i = 0; i < vectorOfItems.size(); i++)
		{
			jvxSize const sampleStart = (jvxSize)((jvxData)loopStartInSec * (jvxData)vectorOfItems.at(i).sampleRate);
			jvxSize const sampleEnd = (jvxSize)((jvxData)loopEndInSec * (jvxData)vectorOfItems.at(i).sampleRate);

			vecOfInputHandlers.at(i)->setLoopStartStop(sampleStart, sampleEnd);
			jvxSize readPosition = vecOfInputHandlers.at(i)->getReadOffset();
			if (readPosition < sampleStart || readPosition > sampleEnd) {
				vecOfInputHandlers.at(i)->setReadOffset(sampleStart);
				// updateTimeDisplay();
			}

		}
		updateTimeDisplay();
	}



	void myCentralWidget::updateClippingIndicator() {
		jvxSize clipping_detected;
		jvxCallManagerProperties callGate;

		ident.reset("/clipping_detected");
		trans.reset(true);
		jvxErrorType res = thePropRef_algo->get_property(callGate, jPRG(&clipping_detected, 1, JVX_DATAFORMAT_SIZE), ident, trans);
		assert(res == JVX_NO_ERROR);

		if (clipping_detected)
		{
			setClippingIndicator();

			this->clippingReleaseTimer->stop();
			this->clippingReleaseTimer->start();

			clipping_detected = false;
			ident.reset("/clipping_detected");
			trans.reset(true);
			jvxErrorType const res = thePropRef_algo->set_property(callGate , jPRG(&clipping_detected, 1, JVX_DATAFORMAT_SIZE),ident, trans);
			assert(res == JVX_NO_ERROR);
		}
	}

	void myCentralWidget::setClippingIndicator() {
		this->horizontalSlider_curPlayPos->setStyleSheet("QSlider::groove:horizontal {border: 1px solid #999999; height:18px; border-radius:1px;} QSlider::add-page:qlineargradient{background:red; border-top-right-radius: 0px; border-bottom-right-radius:0px; border-top-left-radius:0px; border-bottom-left-radius:0px;} QSlider::handle:horizontal{width:5px; background:grey;}  QSlider::sub-page:qlineargradient{background:darkred; border-top-right-radius: 0px; border-bottom-right-radius: 0px; border-top-left-radius:0px; border-bottom-left-radius:0px;}");

	}

	void myCentralWidget::resetClippingIndicator() {
		this->horizontalSlider_curPlayPos->setStyleSheet("QSlider::groove:horizontal {\nborder: 1px solid #999999;\nheight: 18px;\n\nborder-radius: 1px;\n}\n\n\nQSlider::add-page:qlineargradient {\nbackground: lightgrey;\nborder-top-right-radius: 0px;\nborder-bottom-right-radius: 0px;\nborder-top-left-radius: 0px;\nborder-bottom-left-radius: 0px;\n}\n\n\nQSlider::handle:horizontal {\nwidth: 5px;\nbackground: grey;\n}\n\nQSlider::sub-page:qlineargradient {\nbackground: lightblue;\nborder-top-right-radius: 0px;\nborder-bottom-right-radius: 0px;\nborder-top-left-radius: 0px;\nborder-bottom-left-radius: 0px;\n}");

	}

	void myCentralWidget::updatePlayMonoAsStereoClicked(int state)
	{
		jvxCallManagerProperties callGate;
		jvxSize mono_to_stereo_playback = (jvxSize)state;
		ident.reset("/mono_to_stereo_playback");
		trans.reset(true);
		jvxErrorType res = thePropRef_algo->set_property(callGate, jPRG(
			&mono_to_stereo_playback, 1, JVX_DATAFORMAT_SIZE),ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, "/mono_to_stereo_playback", thePropRef_dev);
		assert(rr);

		jvxSize props_dirty = 1;
		ident.reset("/play_props_dirty");
		trans.reset(true);
		res = thePropRef_algo->set_property(callGate, jPRG(
			&props_dirty, 01, JVX_DATAFORMAT_SIZE),ident, trans);
		rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, "/play_props_dirty", thePropRef_dev);
		assert(rr);
	}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
