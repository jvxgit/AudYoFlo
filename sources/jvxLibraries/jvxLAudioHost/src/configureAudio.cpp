#include "uMainWindow.h"
#include "configureAudio.h"
#include "jvxQtAudioHost_common.h"
#include "jvxAudioHost_common.h"
#include <QtWidgets/QFileDialog>
#include <QtGui/QCloseEvent>

#include "jvxQtAudioHostHookupEntries.h"

static int listSamplerates[] =
{
	8000,
	11025,
	16000,
	22050,
	32000,
	44100,
	48000,
	88200,
	96000,
	192000,
	-1
};

static int listBuffersizes[] =
{
	64,
	96,
	128,
	160,
	256,
	320,
	480,
	512,
	1024,
	-1
};



#define UI_HEIGHT_SMALL 690
#define UI_HEIGHT_LARGE 990

#define JVX_MODES 2
typedef struct
{
	const char* prop_access_mode0;
	const char* prop_access_mode1;
	const char* prop_access_mode2;
} jvxOnePropDescriptor;

static std::string shortenStringName(int numLetters, std::string input)
{
	std::string ret = input;
	if(input.size() > numLetters)
	{
		ret = input.substr(0, numLetters/4) + "..." + input.substr(input.size()-3*numLetters/4, 3*numLetters/4);
	}
	return ret;
}

configureAudio::configureAudio(uMainWindow* thePar, jvxComponentIdentification* tpAllI): 
	QDialog(thePar), myParent(thePar), tpAll(tpAllI)

{
//	properties_started = false;
	//runtime.id_device;
	//runtime.id_technology =  0;
	runtime.id_inchan =  JVX_SIZE_UNSELECTED;
	runtime.id_outchan =  JVX_SIZE_UNSELECTED;
	runtime.in_all = true;
	runtime.out_all = true;

	/*
	properties.device.id_bsize = JVX_SIZE_UNSELECTED;
	properties.device.id_srate = JVX_SIZE_UNSELECTED;
	properties.device.id_formats = JVX_SIZE_UNSELECTED;
	properties.device.id_in_channels = JVX_SIZE_UNSELECTED;
	properties.device.id_out_channels = JVX_SIZE_UNSELECTED;

	properties.device.id_in_level_avrg = JVX_SIZE_UNSELECTED;
	properties.device.id_in_level_max = JVX_SIZE_UNSELECTED;
	properties.device.id_out_level_avrg = JVX_SIZE_UNSELECTED;
	properties.device.id_in_level_max = JVX_SIZE_UNSELECTED;
    properties.device.id_direct_in = JVX_SIZE_UNSELECTED;
    properties.device.id_out_level_gain = JVX_SIZE_UNSELECTED;
    properties.device.id_in_level_gain = JVX_SIZE_UNSELECTED;
	*/
	config.period_ms = 200;
	config.min_width = 640;
	config.min_height = 480;

	//spec_Class_dev = NULL;
	//spec_Class_tech = NULL;

	timerViewUpdate = NULL;

	currentPropsDev = NULL;
	currentPropsTech = NULL;

}

configureAudio::~configureAudio()
{
}

QSize 
configureAudio::sizeHint() const
{
	return QSize(config.min_width, config.min_height);
}

QSize 
configureAudio::minimumSizeHint() const
{
	return QSize(config.min_width, config.min_height);
}

jvxErrorType
configureAudio::init()
{
	jvxComponentIdentification& tpIdT = tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];
	checkMode();

	Ui::ConfigureAudio::setupUi(this);

	this->adjustSize();
	return JVX_NO_ERROR;
}

void
configureAudio::acceptSlotConfiguration()
{
	jvxSize i,j;
	jvxSize szSlots = 0, szSubSlots = 0;
	jvxSize szSlotsMax = 0, szSubSlotsMax = 0;

	// JVX_COMPONENT_AUDIO_TECHNOLOGY
	myParent->involvedHost.hHost->number_slots_component_system(tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY], &szSlots, &szSubSlots, &szSlotsMax, &szSubSlotsMax);
	runtime.id_technology.resize(szSlotsMax);
	for (i = 0; i < runtime.id_technology.size(); i++)
	{
		runtime.id_technology[i] = 0; // 0 is unselected
	}
	if (runtime.id_technology.size() > 1)
	{
		comboBox_techSlots->show();
		for (i = 0; i <runtime.id_technology.size(); i++)
		{
			comboBox_techSlots->addItem(("Slot #" + jvx_size2String(i)).c_str());
		}
	}
	else
	{
		comboBox_techSlots->hide();
	}
	
	// ========================================================================
	assert(JVX_CHECK_SIZE_SELECTED(szSlotsMax));
	assert(JVX_CHECK_SIZE_SELECTED(szSubSlotsMax));

	runtime.id_device.resize(szSlotsMax);
	for (i = 0; i < runtime.id_device.size(); i++)
	{
		runtime.id_device[i].resize(szSubSlotsMax);
		for (j = 0; j < runtime.id_device[i].size(); j++)
		{
			runtime.id_device[i][j] = 0; // 0 is unselected
		}
	}
	if (szSubSlotsMax > 1)
	{
		comboBox_devslot->show();
		for (i = 0; i < szSubSlotsMax /* runtime.id_device.size()*/; i++)
		{
			comboBox_devslot->addItem(("Slot #" + jvx_size2String(i)).c_str());
		}
	}
	else
	{
		comboBox_devslot->hide();
	}
}
#if 0
jvxErrorType
configureAudio::stopProperties()
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxComponentIdentification& tpIdT = tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];
	jvxComponentIdentification& tpIdD = tpAll[JVX_COMPONENT_AUDIO_DEVICE];

	assert(properties_started);
	
	/*
	jvx_componentIdentification_properties_erase(
		myParent->involvedHost.hHost,
		myParent->theRegisteredComponentReferenceTriples, 
		tpIdT);
		*/
	properties.theTechProps.reset();

	/*
	jvx_componentIdentification_properties_erase(
		myParent->involvedHost.hHost,
		myParent->theRegisteredComponentReferenceTriples,
		tpIdD);*/
	properties.theDevProps.reset();

	properties_started = false;

	return JVX_NO_ERROR;
}

jvxErrorType
configureAudio::startProperties()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;
	jvxBitField featureClass = 0;
	jvxComponentIdentification& tpIdT = tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];
	jvxComponentIdentification& tpIdD = tpAll[JVX_COMPONENT_AUDIO_DEVICE];

	assert(properties_started == false);

	/*
	res = jvx_componentIdentification_properties_direct_add(
		myParent->involvedHost.hHost,
		myParent->theRegisteredComponentReferenceTriples,
		tpIdT, properties.theTechProps);
		*/
	properties.theTechProps.reset();
	res = jvx_componentIdentification_properties_find(
			myParent->theRegisteredComponentReferenceTriples,
			tpIdT, properties.theTechProps);
	// assert(res == JVX_NO_ERROR);; < -this is a valid case if there is no technology


	properties.theDevProps.reset();
	/*
	res = jvx_componentIdentification_properties_direct_add(
		myParent->involvedHost.hHost,
		myParent->theRegisteredComponentReferenceTriples,
		tpIdD, properties.theDevProps);
		*/

	res = jvx_componentIdentification_properties_find(
		myParent->theRegisteredComponentReferenceTriples,
		tpIdD, properties.theDevProps);
	// assert(res == JVX_NO_ERROR); <- this is a valid case if there is no device

	// Check if a device has been loaded before from config file. If so, attach UI subelements right now
	myParent->involvedHost.hHost->state(&stat);
	if(stat == JVX_STATE_ACTIVE)
	{
		myParent->involvedHost.hHost->state_component(tpIdT, &stat);
		/*
		if(stat >= JVX_STATE_ACTIVE)
		{
			if(myParent->subWidgets.main.com_state.informed_tech == false)
			{
				myParent->subWidgets.main.theWidget->inform_active(JVX_COMPONENT_AUDIO_TECHNOLOGY);
				myParent->subWidgets.main.com_state.informed_tech = true;
			}
		}
		*/

		myParent->involvedHost.hHost->state_component(tpAll[JVX_COMPONENT_AUDIO_NODE], &stat);
		/*
		if(stat >= JVX_STATE_ACTIVE)
		{
			if(myParent->subWidgets.main.com_state.informed_node == false)
			{
				myParent->subWidgets.main.theWidget->inform_active(JVX_COMPONENT_AUDIO_NODE);
				myParent->subWidgets.main.com_state.informed_node = true;
			}
		}
		*/

		myParent->involvedHost.hHost->state_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
		if(stat >= JVX_STATE_ACTIVE)
		{
			/*
			if(myParent->subWidgets.main.com_state.informed_device == false)
			{
				myParent->subWidgets.main.theWidget->inform_active(JVX_COMPONENT_AUDIO_DEVICE);
				myParent->subWidgets.main.com_state.informed_device = true;
			}
			*/
		}
	}

	properties_started = true;
	res = refreshPropertyIds();

	return res;

}
#endif

jvxErrorType
configureAudio::refreshPropertyIds()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num;
	jvxSize hdlIdx;
	jvxBool foundit = false;
	std::string str;
	jvxSize numOutFiles = 0;
	jvxSize numInFiles = 0;
	jvxBool isValid = false;
	jvxBool tecDevIsGenWrapperFirst = false;
	jvxBool tecDevIsGenWrapper = false;

	jvxComponentIdentification& tpIdT = tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];
	if (tpIdT.slotid == 0)
	{
		tecDevIsGenWrapper = true;
		if (tpIdT.slotsubid == 0)
		{
			tecDevIsGenWrapperFirst = true;
		}
	}
	//if (!properties_started)
	//{
	//	return JVX_ERROR_NOT_READY;
	//}
	theHost = myParent->involvedHost.hHost;

	if(theHost)
	{
		theHost->state(&stat);
		/*
		properties.technology.id_issue_command = JVX_SIZE_UNSELECTED;
		properties.technology.id_technologies = JVX_SIZE_UNSELECTED;
		*/
		/*
		properties.device.id_in_channels = JVX_SIZE_UNSELECTED;
		properties.device.id_out_channels = JVX_SIZE_UNSELECTED;
		properties.device.id_srate = JVX_SIZE_UNSELECTED;
		properties.device.id_bsize = JVX_SIZE_UNSELECTED;
		properties.device.id_formats = JVX_SIZE_UNSELECTED;
		properties.device.id_auto_off = JVX_SIZE_UNSELECTED;
		*/
		if(stat == JVX_STATE_ACTIVE)
		{
			theHost->state_selected_component(tpIdT, &stat);
			if(stat >= JVX_STATE_SELECTED)
			{
				if (tecDevIsGenWrapper)
				{
					if (stat == JVX_STATE_SELECTED)
					{
						// Show property with descriptor "/JVX_GENW_TECHNOLOGIES"
						/*
						num = 0;
						foundit = false;
						hdlIdx = 0;
						isValid = false;

						FIND_PROPERTY_ID(currentPropsTech, hdlIdx, "/JVX_GENW_TECHNOLOGIES", isValid, foundit);
						if (foundit && isValid)
						{
							properties.technology.id_technologies = hdlIdx;
						}
						*/
					}

					if (stat >= JVX_STATE_SELECTED)
					{
						// Show property with descriptor "/JVX_GENW_TECHNOLOGIES"
						/*
						num = 0;
						foundit = false;
						hdlIdx = 0;
						isValid = false;

						FIND_PROPERTY_ID(currentPropsTech, hdlIdx, "/JVX_GENW_COMMAND", isValid, foundit);
						if (foundit && isValid)
						{
							properties.technology.id_issue_command = hdlIdx;
						}
						*/
					}
				}
				if (stat >= JVX_STATE_SELECTED)
				{

					if (stat >= JVX_STATE_ACTIVE)
					{
						res = theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
						assert(res == JVX_NO_ERROR);
						if (stat >= JVX_STATE_ACTIVE)
						{
							/*
							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/system/sel_input_channels", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_in_channels = hdlIdx;
							}

							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/system/sel_output_channels", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_out_channels = hdlIdx;
							}

							// Buffersizes, rates, formats
							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_AUDEV_SRATE", isValid, foundit);

							if (foundit && isValid)
							{
								properties.device.id_srate = hdlIdx;
							}
							
							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_AUDEV_BSIZE", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_bsize = hdlIdx;
							}

							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/system/sel_dataformat", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_formats = hdlIdx;
							}
							
							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_GENW_AUTO_OFF", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_auto_off = hdlIdx;
							}
							else
							{
								// May be selected in case of non-gen wrapper devices
							}
							*/
						}
					}
				}
			}
		}

		/*
		if(spec_Class_dev)
		{
			spec_Class_dev->refreshPropertyIds();
		}
		*/
		myParent->subWidgets.theAudioArgs->refreshPropertyIds(tecDevIsGenWrapper);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
configureAudio::refreshPropertyIds_run()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num;
	jvxSize hdlIdx;
	jvxBool isValid;
	jvxBool foundit = false;
	std::string str;
	jvxSize numOutFiles = 0;
	jvxSize numInFiles = 0;
	jvxSelectionList selList;
	jvxSize sz = 0;

	jvxBool tecDevIsGenWrapperFirst = false;
	jvxBool tecDevIsGenWrapper = false;
	jvxComponentIdentification& tpIdT = tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];

	if (tpIdT.slotid == 0)
	{
		tecDevIsGenWrapper = true;
		if (tpIdT.slotsubid == 0)
		{
			tecDevIsGenWrapperFirst = true;
		}
	}

	theHost = myParent->involvedHost.hHost;
	theHost->state( &stat);

	/*
	properties.device.id_in_level_avrg = JVX_SIZE_UNSELECTED;
	properties.device.id_in_level_max = JVX_SIZE_UNSELECTED;
	properties.device.id_out_level_avrg = JVX_SIZE_UNSELECTED;
	properties.device.id_out_level_max = JVX_SIZE_UNSELECTED;
	properties.device.id_direct_in = JVX_SIZE_UNSELECTED;
	properties.device.id_out_level_gain = JVX_SIZE_UNSELECTED;
	properties.device.id_in_level_gain = JVX_SIZE_UNSELECTED;
	*/

	if(stat == JVX_STATE_ACTIVE)
	{
		theHost->state_selected_component(tpIdT, &stat);
		if(stat >= JVX_STATE_SELECTED)
		{

			if(stat >= JVX_STATE_SELECTED)
			{
				if(tecDevIsGenWrapper)
				{
					/*
					num = 0;
					foundit = false;
					hdlIdx = 0;
					FIND_PROPERTY_ID(currentPropsTech, hdlIdx, "/JVX_GENW_INPUT_FILES");
					if(foundit && isValid)
					{
					properties.technology.id_input_files = hdlIdx;
					}
					else
					{
					assert(0);
					}

					num = 0;
					foundit = false;
					hdlIdx = 0;
					FIND_PROPERTY(currentPropsTech, "/JVX_GENW_OUTPUT_FILES");
					if(foundit && isValid)
					{
					properties.technology.id_output_files = hdlIdx;
					}
					else
					{
					assert(0);
					}
					*/

					if (stat >= JVX_STATE_ACTIVE)
					{
						res = theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
						assert(res == JVX_NO_ERROR);
						if (stat >= JVX_STATE_PREPARED)
						{
							/*
							// Activate level control
							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_GENW_ACT_ILEVEL", isValid, foundit);
							if (foundit && isValid)
							{
								selList.bitFieldSelected() = 0x1;
								res = currentPropsDev->set_property__descriptor(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST, hdlIdx, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true);
							}

							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_GENW_ACT_OLEVEL", isValid, foundit);
							if (foundit && isValid)
							{
								selList.bitFieldSelected() = 0x1;
								res = currentPropsDev->set_property__descriptor(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST, hdlIdx, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true);
							}


							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_GENW_DEV_ILEVEL", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_in_level_avrg = hdlIdx;
							}


							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_GENW_DEV_OLEVEL", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_out_level_avrg = hdlIdx;
							}


							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_GENW_DEV_ILEVEL_MAX", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_in_level_max = hdlIdx;
							}


							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_GENW_DEV_OLEVEL_MAX", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_out_level_max = hdlIdx;
							}

							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_GENW_DEV_IGAIN", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_in_level_gain = hdlIdx;
							}

							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_GENW_DEV_OGAIN", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_out_level_gain = hdlIdx;
							}

							num = 0;
							foundit = false;
							hdlIdx = 0;
							isValid = false;
							FIND_PROPERTY_ID(currentPropsDev, hdlIdx, "/JVX_GENW_DEV_IDIRECT", isValid, foundit);
							if (foundit && isValid)
							{
								properties.device.id_direct_in = hdlIdx;
							}
							*/
						}
					}
				}
			}
		}
	}

	/*
	if(spec_Class_dev)
	{
		spec_Class_dev->refreshPropertyIds_run();
	}
	*/
	myParent->subWidgets.theAudioArgs->refreshPropertyIds_run(tecDevIsGenWrapper);
	return JVX_NO_ERROR;
}

void
	configureAudio::start()
{
	/*
	this->technology_use_button();
	this->device_use_button();
	*/
}

void
configureAudio::inform_internals_have_changed(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId, 
	bool onlyContent, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose purpose)
{
	if((purpose == JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)||
		(purpose == JVX_PROPERTY_CALL_PURPOSE_TAKE_OVER))
	{
		updateWindow();
	}
}

void
configureAudio::updateWindow()
{
	jvxSize i;
	IjvxHost* theHost = NULL;
	jvxState statHost = JVX_STATE_INIT;
	jvxState statTech = JVX_STATE_INIT;
	jvxState statDev = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num;
	jvxSelectionList selList;
	jvxApiString strL;
	jvxBool foundit = false;
	std::string str;
	jvxInt32 valI32;
	jvxSize sel = JVX_SIZE_UNSELECTED;
	jvxSize szVal = 0;
	jvxSize szll = 0, szrr = 0;
	int cnt;
	jvxSize idxShow = JVX_SIZE_UNSELECTED;
	jvxData dblVal;
	jvxCBool boolVal = c_false;
	std::string fName_short;
	std::string cName_short;
	std::string fullName;
	IjvxQtComponentWidget* spec_Class = NULL;
	jvxAssignedFileTags tags;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxGenericWrapperChannelType purpChan = JVX_GENERIC_WRAPPER_CHANNEL_TYPE_NONE;
	std::string nmChan;
	std::string path;
	int id_in_device;
	jvxCallManagerProperties callGate;
	jvxCBool cboolVal = c_false;
	jvxComponentIdentification& tpIdT = tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];
	jvxComponentIdentification& tpIdD = tpAll[JVX_COMPONENT_AUDIO_DEVICE];

	// checkMode(); <- should have been set before

	theHost = myParent->involvedHost.hHost;

	this->comboBox_devices->clear();
	this->comboBox_technologies->clear();

	treeWidget_infiles->clear();
	treeWidget_outfiles->clear();

	treeWidget_inchans->clear();
	treeWidget_outchans->clear();

	comboBox_formats->clear();

	lineEdit_outwav_artist->setText("--");
	lineEdit_outwav_title->setText("--");

	/*
	QRect rr = this->geometry();
	std::cout << rr.width() << " -- " << rr.height() << std::endl;
	QSize sz = this->sizeHint();
	std::cout << sz.width() << " -:- " << sz.height() << std::endl;
	*/

	//this->adjustSize();
	//this->resize(0,0);
	/*	QRect geo = this->geometry();
	geo.setHeight(10);
	this->setGeometry(geo);
	*/
	//	this->checkBox_showMore->setChecked(this->widget_add_args->isVisible());
	setAllEnabled(false);

	//if (!properties_started)
	//{
	//	return;
	//}

	if(theHost)
	{
		theHost->state(&statHost);

		if (statHost == JVX_STATE_ACTIVE)
		{		
			comboBox_techSlots->setCurrentIndex(tpIdT.slotid);
			comboBox_devslot->setCurrentIndex(tpIdD.slotsubid);

			statTech = JVX_STATE_NONE;
			theHost->state_selected_component(tpIdT, &statTech);

			if (jvx_bitTest(modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
			{
				
				if (statTech >= JVX_STATE_SELECTED)
				{
					setAllEnabled(true);

					pushButton_inadd->setEnabled(false);
					pushButton_inrem->setEnabled(false);
					pushButton_outadd->setEnabled(false);
					pushButton_outrem->setEnabled(false);

					lineEdit_outwav_artist->setEnabled(false);
					lineEdit_outwav_title->setEnabled(false);
					pushButton_outwav_apply->setEnabled(false);
					pushButton_outwav_add->setEnabled(false);

					// ====================================================================
					// Display technologies
					// ====================================================================
					if (statTech <= JVX_STATE_SELECTED)
					{
						// Show property with descriptor "/JVX_GENW_TECHNOLOGIES"
						this->comboBox_technologies->addItem("None");
						ident.reset("/JVX_GENW_TECHNOLOGIES");
						trans.reset();
						res = currentPropsTech->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
						//	currentPropsTech->get_property(callGate, jPRG(&selList, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, false, "/JVX_GENW_TECHNOLOGIES");
						if (res == JVX_NO_ERROR)
						{
							for (i = 0; i < selList.strList.ll(); i++)
							{
								this->comboBox_technologies->addItem(selList.strList.std_str_at(i).c_str());
							}

							this->comboBox_technologies->setCurrentIndex(JVX_SIZE_INT(runtime.id_technology[tpIdT.slotid]));
						}
						else
						{
							assert(0);
						}

						if (JVX_CHECK_SIZE_SELECTED(runtime.id_technology[tpIdT.slotid]))
						{
							this->comboBox_technologies->setCurrentIndex(JVX_SIZE_INT(runtime.id_technology[tpIdT.slotid]));
						}

						//pushButton_techuse->setEnabled(true);
					}
					else if (statTech >= JVX_STATE_ACTIVE)
					{
						this->comboBox_technologies->addItem("Restart Selection Procedure");
						runtime.id_technology[tpIdT.slotid] = 0;
						//pushButton_techuse->setEnabled(true);
						if (statTech > JVX_STATE_ACTIVE)
						{
							this->comboBox_technologies->setEnabled(false);
						}
					}
				}
			}
			else
			{
				setAllEnabled(true);

				pushButton_inadd->setEnabled(false);
				pushButton_inrem->setEnabled(false);
				pushButton_outadd->setEnabled(false);
				pushButton_outrem->setEnabled(false);

				lineEdit_outwav_artist->setEnabled(false);
				lineEdit_outwav_title->setEnabled(false);
				pushButton_outwav_apply->setEnabled(false);
				pushButton_outwav_add->setEnabled(false);

				if (statTech == JVX_STATE_NONE)
				{
					res = theHost->number_components_system(tpIdT, &num);// technology slot ueq 0
					res = theHost->selection_component(tpIdT, &sel);
					if (res != JVX_NO_ERROR)
					{
						runtime.id_technology[tpIdT.slotid] = sel + 1; // +1 due to first "None" entry
					}

					this->comboBox_technologies->addItem("None");
					for (i = 0; i < num; i++)
					{
						res = theHost->description_component_system(tpIdT, i, &strL);
						this->comboBox_technologies->addItem(strL.c_str());
					}
					if (JVX_CHECK_SIZE_UNSELECTED(runtime.id_technology[tpIdT.slotid]) ||
						(runtime.id_technology[tpIdT.slotid] > (jvxInt32)num))
					{
						runtime.id_technology[tpIdT.slotid] = 0;
					}
					if (JVX_CHECK_SIZE_SELECTED(runtime.id_technology[tpIdT.slotid]))
					{
						this->comboBox_technologies->setCurrentIndex(JVX_SIZE_INT(runtime.id_technology[tpIdT.slotid]));
					}
				}
				else
				{
					this->comboBox_technologies->addItem("Restart Selection Procedure");
					runtime.id_technology[tpIdT.slotid] = 0;
					//pushButton_techuse->setEnabled(true);
					if (statTech > JVX_STATE_ACTIVE)
					{
						this->comboBox_technologies->setEnabled(false);
					}
				}
			}

			if (statTech >= JVX_STATE_ACTIVE)
			{
				num = 0;
				this->comboBox_devices->addItem("None");
				res = theHost->number_components_system(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &num);// technology slot 0
				res = theHost->selection_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &sel);

				if (res == JVX_NO_ERROR)
				{
					runtime.id_device[tpIdT.slotid][tpAll[JVX_COMPONENT_AUDIO_DEVICE].slotsubid] = sel + 1; // +1 due to first "None" entry
				}

				for (i = 0; i < num; i++)
				{					
					res = theHost->description_component_system(tpAll[JVX_COMPONENT_AUDIO_DEVICE], i, &strL);
					this->comboBox_devices->addItem(strL.c_str());
				}

				
				for (i = 0; i < num; i++)
				{
					QIcon icon;
					jvxDeviceCapabilities caps;
					res = theHost->capabilities_devices_component_system(tpIdT, i, caps);
					if (jvx_bitTest(caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_PROXY_SHIFT))
					{
						icon.addFile(QString::fromUtf8(":/himages/images/icons8-mic-audio-50.png"), QSize(), QIcon::Normal, QIcon::Off);
					}
					else if (
						(
							(jvx_bitTest(caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_INPUT_SHIFT)) &&
							(jvx_bitTest(caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_OUTPUT_SHIFT))) ||
						(jvx_bitTest(caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT)))
					{
						icon.addFile(QString::fromUtf8(":/himages/images/icons8-mic-audio-50.png"), QSize(), QIcon::Normal, QIcon::Off);
					}
					else if (jvx_bitTest(caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_OUTPUT_SHIFT))
					{
						icon.addFile(QString::fromUtf8(":/himages/images/icons8-audio-50.png"), QSize(), QIcon::Normal, QIcon::Off);
					}
					else if (jvx_bitTest(caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_INPUT_SHIFT))
					{
						icon.addFile(QString::fromUtf8(":/himages/images/icons8-mic-50.png"), QSize(), QIcon::Normal, QIcon::Off);
					}
					
					this->comboBox_devices->setItemIcon(i+1, icon);
					if (jvx_bitTest<jvxCBitField16>(caps.flags, (int)jvxDeviceCapabilityFlagsShift::JVX_DEVICE_CAPABILITY_FLAGS_DEFAULT_DEVICE_SHIFT))
					{
						auto var = this->comboBox_devices->itemData(i+1, Qt::FontRole);
						QFont ftT = var.value<QFont>();
						ftT.setItalic(true);
						this->comboBox_devices->setItemData(i+1, ftT, Qt::FontRole);
					}
				}				

				this->comboBox_devices->setCurrentIndex(sel + 1);
				res = theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &statDev);

				// Device processing
				if (statDev < JVX_STATE_ACTIVE)
				{
					// Device no longer selected/not yet selected
					treeWidget_inchans->setEnabled(true);
					treeWidget_outchans->setEnabled(true);
				}
				else if (statDev >= JVX_STATE_ACTIVE)
				{
					if (statDev == JVX_STATE_ACTIVE)
					{
						this->progressBar_inchan_level->setValue(0);
						this->progressBar_outchan_level->setValue(0);
					}

					treeWidget_inchans->setEnabled(true);
					treeWidget_outchans->setEnabled(true);

					//std::vector<props_onechan> in_channel_copy = this->runtime.in_channel_mapper;
					//std::vector<props_onechan> out_channel_copy = this->runtime.out_channel_mapper;

					this->runtime.in_channel_mapper.clear();
					this->runtime.out_channel_mapper.clear();

					pushButton_inup->setEnabled(true);
					pushButton_indown->setEnabled(true);
					pushButton_outup->setEnabled(true);
					pushButton_outdown->setEnabled(true);

					lineEdit_srate->setEnabled(true);
					lineEdit_bsize->setEnabled(true);
					comboBox_formats->setEnabled(true);

					checkBox_autooff->setChecked(false);

					if (jvx_bitTest(modeShow, JVX_UI_SHOW_MODE_GENW_SHIFT))
					{
						// Activate all file io elements - but only for slot 0
						pushButton_inadd->setEnabled(true);
						pushButton_inrem->setEnabled(true);
						pushButton_outadd->setEnabled(true);
						pushButton_outrem->setEnabled(true);

						lineEdit_outwav_artist->setEnabled(true);
						lineEdit_outwav_title->setEnabled(true);
						pushButton_outwav_apply->setEnabled(true);
						pushButton_outwav_add->setEnabled(true);
					}

					fullName = "";
					//currentPropsDev->get_property_description_simple__descriptor()
					//currentPropsDev->get_property_
					fullName = "";
					res = JVX_ERROR_NOT_READY;
					if (currentPropsDev)
					{
						ident.reset("/system/sel_input_channels");
						trans.reset();
						res = currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
					}
					// currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST, properties.device.id_in_channels, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, JVX_PROPERTY_DECODER_NONE, NULL, callGate);
					if (res == JVX_NO_ERROR)
					{
						if (runtime.id_inchan >= selList.strList.ll())
						{
							runtime.id_inchan = JVX_SIZE_UNSELECTED;
						}
						cnt = 0;
						for (i = 0; i < selList.strList.ll(); i++)
						{
							QTreeWidgetItem* topL = new QTreeWidgetItem;

							props_onechan val;
							ident.reset("/inputChannelGain");
							trans.reset(false, i);
							res = currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), ident, trans);
							if (res == JVX_NO_ERROR)
							{
								val.gain = dblVal;								
								val.valid = true;
							}

							ident.reset("/JVX_GENW_DEV_IDIRECT");
							trans.reset(false, i);
							res = currentPropsDev->get_property(callGate, jPRG(&boolVal, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
							if (res == JVX_NO_ERROR)
							{
								val.direct = boolVal;
							}

							topL->setData(0, Qt::UserRole, QVariant(JVX_SIZE_INT(i)));
							if (val.direct == 1)
							{
								topL->setText(0, (jvx_size2String(i) + "<d>").c_str());
							}
							else
							{
								topL->setText(0, (jvx_size2String(i)).c_str());
							}

							cName_short = selList.strList.std_str_at(i);
							resL = jvx_genw_decodeChannelName(cName_short, purpChan, nmChan, id_in_device);
							if (resL == JVX_NO_ERROR)
							{
								switch (purpChan)
								{
								case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:

									topL->setForeground(1, QBrush(Qt::blue));
									cName_short = nmChan;
									jvx_tagExprToTagStruct(cName_short, tags, path);

									if (JVX_EVALUATE_BITFIELD(tags.tagsSet))
									{
										cName_short = jvx_tagStructToUserView(tags);
									}
									else
									{
										cName_short = path;
									}
									cName_short += "(" + jvx_size2String(id_in_device) + ")";
									break;
								case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:
									topL->setForeground(1, QBrush(Qt::green));
									cName_short = nmChan + "(" + jvx_size2String(id_in_device) + ")";
									break;
								case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:
									topL->setForeground(1, QBrush(Qt::black));
									cName_short = nmChan + "(" + jvx_size2String(id_in_device) + ")";
									break;
								case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY:
									topL->setForeground(1, QBrush(Qt::magenta));
									cName_short = "Dummy";
									break;
								default:
									assert(0);
								}
							}
							cName_short = shortenStringName(NUMBER_LETTERS_CHANNELNAMES, cName_short);

							topL->setText(1, cName_short.c_str());
							if (jvx_bitTest(selList.bitFieldSelected(), i))
							{
								if (JVX_CHECK_SIZE_UNSELECTED(runtime.id_inchan))
								{
									runtime.id_inchan = i;
								}
								topL->setBackground(0, QBrush(Qt::green));

								val.mapped_to = cnt;
								cnt++;
							}
							else
							{
								topL->setBackground(0, QBrush(Qt::red));
								val.mapped_to = JVX_SIZE_UNSELECTED;

							}
							this->runtime.in_channel_mapper.push_back(val);

							if (i == runtime.id_inchan)
							{
								topL->setBackground(1, QBrush(Qt::gray));
								fullName = selList.strList.std_str_at(i);
							}
							treeWidget_inchans->addTopLevelItem(topL);
						}


						if (JVX_CHECK_SIZE_UNSELECTED(runtime.id_inchan))
						{
							if (selList.strList.ll() > 0)
							{
								runtime.id_inchan = 0;
							}
						}

						treeWidget_inchans->setToolTip(fullName.c_str());

						if (callGate.access_protocol != JVX_ACCESS_PROTOCOL_OK)
						{
							this->comboBox_technologies->setEnabled(false);
						}
					}
					else
					{
					this->comboBox_technologies->setEnabled(false);
					}
					


					fullName = "";
					res = JVX_ERROR_NOT_READY;
					if (currentPropsDev)
					{
						ident.reset("/system/sel_output_channels");
						trans.reset();

						res = currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
					}
					// currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST, XX, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, JVX_PROPERTY_DECODER_NONE, NULL, callGate);
					if (res == JVX_NO_ERROR)
					{
						if (runtime.id_outchan >= (jvxInt32)selList.strList.ll())
						{
							runtime.id_outchan = JVX_SIZE_UNSELECTED;
						}
						cnt = 0;
						for (i = 0; i < selList.strList.ll(); i++)
						{

							QTreeWidgetItem* topL = new QTreeWidgetItem;

							props_onechan val;
							ident.reset("/outputChannelGain");
							trans.reset(false, i);

							res = currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), ident, trans);
							if (res == JVX_NO_ERROR)
							{
								val.gain = dblVal;
								val.direct = 0;
								val.valid = true;
							}

							topL->setData(0, Qt::UserRole, QVariant(JVX_SIZE_INT(i)));
							topL->setText(0, (jvx_size2String(i)).c_str());

							cName_short = selList.strList.std_str_at(i);
							resL = jvx_genw_decodeChannelName(cName_short, purpChan, nmChan, id_in_device);
							if (resL == JVX_NO_ERROR)
							{
								switch (purpChan)
								{
								case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:

									topL->setForeground(1, QBrush(Qt::blue));
									cName_short = nmChan;
									jvx_tagExprToTagStruct(cName_short, tags, path);

									if (JVX_EVALUATE_BITFIELD(tags.tagsSet))
									{
										cName_short = jvx_tagStructToUserView(tags);
									}
									else
									{
										cName_short = path;
									}
									cName_short += "(" + jvx_int2String(id_in_device) + ")";
									break;
								case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:
									topL->setForeground(1, QBrush(Qt::green));
									cName_short = nmChan + "(" + jvx_int2String(id_in_device) + ")";
									break;
								case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:
									topL->setForeground(1, QBrush(Qt::black));
									cName_short = nmChan + "(" + jvx_int2String(id_in_device) + ")";
									break;
								case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY:
									topL->setForeground(1, QBrush(Qt::magenta));
									cName_short = "Dummy";
									break;
								default:
									assert(0);
								}
							}
							cName_short = shortenStringName(NUMBER_LETTERS_CHANNELNAMES, cName_short);

							topL->setText(1, cName_short.c_str());
							if (jvx_bitTest(selList.bitFieldSelected(), i))
							{
								if (JVX_CHECK_SIZE_UNSELECTED(runtime.id_outchan))
								{
									runtime.id_outchan = i;
								}

								topL->setBackground(0, QBrush(Qt::green));
								val.mapped_to = cnt;
								cnt++;
							}
							else
							{
								topL->setBackground(0, QBrush(Qt::red));
								val.mapped_to = JVX_SIZE_UNSELECTED;
							}

							this->runtime.out_channel_mapper.push_back(val);
							if (i == runtime.id_outchan)
							{
								topL->setBackground(1, QBrush(Qt::gray));
								fullName = selList.strList.std_str_at(i);
							}
							treeWidget_outchans->addTopLevelItem(topL);
						}
						if (JVX_CHECK_SIZE_UNSELECTED(runtime.id_outchan))
						{
							if (selList.strList.ll() > 0)
							{
								runtime.id_outchan = 0;
							}
						}
						treeWidget_outchans->setToolTip(fullName.c_str());
						if(callGate.access_protocol != JVX_ACCESS_PROTOCOL_OK)
						{
							this->comboBox_technologies->setEnabled(false);
						}
					}
					else
					{
						this->comboBox_technologies->setEnabled(false);
					}
					
					fullName = "";
					res = JVX_ERROR_NOT_READY;
					if (currentPropsDev)
					{
						ident.reset("/system/sel_rate");
						trans.reset();
						res = currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
					}
					//currentPropsDev->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE, properties.device.id_srate, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, callGate);
					if (jvx_check_property_access_ok(res, callGate.access_protocol))
					{
						jvxSize idSel = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), selList.strList.ll());

						QFont ft = comboBox_rates->font();
						if (callGate.access_protocol != JVX_ACCESS_PROTOCOL_OK)
						{
							ft.setItalic(true);
						}
						else
						{
							ft.setItalic(false);
						}
						comboBox_rates->setFont(ft);

						stackedWidget_rate->setCurrentIndex(1);
						comboBox_rates->clear();
						for (i = 0; i < selList.strList.ll(); i++)
						{
							std::string txt = selList.strList.std_str_at(i);
							if (i == idSel)
							{
								txt += "*";
							}
							comboBox_rates->addItem(txt.c_str());
						}
						if (JVX_CHECK_SIZE_SELECTED(idSel))
						{
							comboBox_rates->setCurrentIndex(idSel);
						}
					}
					else
					{
						fullName = "";
						res = JVX_ERROR_NOT_READY;
						if (currentPropsDev)
						{
							ident.reset("/system/rate");
							trans.reset();

							res = currentPropsDev->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
						}
						//currentPropsDev->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE, properties.device.id_srate, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, callGate);
						if (jvx_check_property_access_ok(res, callGate.access_protocol))
						{		
							stackedWidget_rate->setCurrentIndex(0);
							
							if (callGate.access_protocol != JVX_ACCESS_PROTOCOL_OK)
							{
								lineEdit_srate->setEnabled(false);
							}
							else
							{
								lineEdit_srate->setEnabled(true);
							}
							this->lineEdit_srate->show();
							this->lineEdit_srate->setText(jvx_int2String(valI32).c_str());
						}
						else
						{
							comboBox_rates->hide();
							this->lineEdit_srate->show();
							this->lineEdit_srate->setText("");
						}
					}

					fullName = "";
					res = JVX_ERROR_NOT_READY;
					if (currentPropsDev)
					{
						ident.reset("/system/sel_framesize");
						trans.reset();

						res = currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
					}
					if (jvx_check_property_access_ok(res, callGate.access_protocol))
					{
						jvxSize idSel = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), selList.strList.ll());
						QFont ft = comboBox_sizes->font();
						if (callGate.access_protocol != JVX_ACCESS_PROTOCOL_OK)
						{
							ft.setItalic(true);
						}
						else
						{
							ft.setItalic(false);
						}
						comboBox_sizes->setFont(ft);
						stackedWidget_size->setCurrentIndex(1);
						comboBox_sizes->clear();
						for (i = 0; i < selList.strList.ll(); i++)
						{
							std::string txt = selList.strList.std_str_at(i);
							if (i == idSel)
							{
								txt += "*";
							}
							comboBox_sizes->addItem(txt.c_str());
						}
						if (JVX_CHECK_SIZE_SELECTED(idSel))
						{
							comboBox_sizes->setCurrentIndex(idSel);
						}
					}
					else
					{
						fullName = "";
						res = JVX_ERROR_NOT_READY;
						if (currentPropsDev)
						{
							ident.reset("/system/framesize");
							trans.reset();

							res = currentPropsDev->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
						}
						// currentPropsDev->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE, properties.device.id_bsize, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, callGate);
						if (jvx_check_property_access_ok(res, callGate.access_protocol))
						{
							if (callGate.access_protocol != JVX_ACCESS_PROTOCOL_OK)
							{
								lineEdit_bsize->setEnabled(false);
							}
							else
							{
								lineEdit_bsize->setEnabled(true);
							}
							stackedWidget_size->setCurrentIndex(0);
							this->lineEdit_bsize->setText(jvx_int2String(valI32).c_str());
						}
						else
						{
							stackedWidget_size->setCurrentIndex(0);
							this->lineEdit_bsize->setText("");
						}
					}
					
					jvxSize selId = JVX_SIZE_UNSELECTED;

					fullName = "";
					res = JVX_ERROR_NOT_READY;
					if (currentPropsDev)
					{
						ident.reset("/system/sel_dataformat");
						trans.reset();

						res = currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
					}
					if (jvx_check_property_access_ok(res, callGate.access_protocol))
					{
						for (i = 0; i < selList.strList.ll(); i++)
						{
							if (jvx_bitTest(selList.bitFieldSelected(), i))
							{
								selId = i;
							}
							comboBox_formats->addItem(selList.strList.std_str_at(i).c_str());
						}

						if (JVX_CHECK_SIZE_SELECTED(selId))
						{
							comboBox_formats->setCurrentIndex(JVX_SIZE_INT(selId));
						}
					}
					else
					{
						comboBox_formats->clear();
					}

					this->checkBox_autooff->setEnabled(true);
					fullName = "";
					res = JVX_ERROR_NOT_READY;
					if (currentPropsDev)
					{
						ident.reset("/JVX_GENW_AUTO_OFF");
						trans.reset(true);

						res = currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
							ident, trans);
					}
					//currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST,
						//properties.device.id_auto_off, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true, callGate);
					if (res == JVX_NO_ERROR)
					{
						assert(callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK);

						// Note only content copied!!
						if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x2))
						{
							this->checkBox_autooff->setChecked(true);
						}
					}
					else
					{
						// This case may occur if a non-generic wr<pper device is selected
						this->checkBox_autooff->setChecked(false);
						this->checkBox_autooff->setEnabled(false);
					}

					this->checkBox_igainall->setChecked(runtime.in_all);
					this->checkBox_ogainall->setChecked(runtime.out_all);					

					idxShow = JVX_SIZE_UNSELECTED;

					if (runtime.in_channel_mapper.size() > 0)
					{
						idxShow = runtime.id_inchan;
						dblVal = 0;
						/*
						if (runtime.in_all)
						{
							idxShow = 0;
						}
						*/
						if (JVX_CHECK_SIZE_SELECTED(idxShow))
						{
							if (runtime.in_channel_mapper[idxShow].valid)
							{
								assert(idxShow < (jvxInt32)runtime.in_channel_mapper.size());
								dblVal = 20 * log10(runtime.in_channel_mapper[idxShow].gain);
								this->label_ingain->setText((jvx_data2String(dblVal, 1) + " dB").c_str());
								dblVal = JVX_MIN(50, dblVal);
								dblVal = JVX_MAX(-50, dblVal);
								this->horizontalSlider_igain->setValue(dblVal + 50);

								this->label_ingain->setEnabled(true);
								this->horizontalSlider_igain->setEnabled(true);
							}
							else
							{
								this->label_ingain->setEnabled(false);
								this->horizontalSlider_igain->setEnabled(false);
								this->label_ingain->setText("--");
							}
						}
					}
					else
					{
						this->label_ingain->setEnabled(false);
						this->horizontalSlider_igain->setEnabled(false);
						this->label_ingain->setText("--");
					}


					idxShow = JVX_SIZE_UNSELECTED;

					if (runtime.out_channel_mapper.size() > 0)
					{
						idxShow = runtime.id_outchan;
						dblVal = 0;
						/*
						if (runtime.out_all)
						{
							idxShow = 0;
						}
						*/

						if (JVX_CHECK_SIZE_SELECTED(idxShow))
						{
							if (runtime.out_channel_mapper[idxShow].valid)
							{
								assert(idxShow < (jvxInt32)runtime.out_channel_mapper.size());
								dblVal = 20 * log10(runtime.out_channel_mapper[idxShow].gain);
								this->label_outgain->setText((jvx_data2String(dblVal, 1) + " dB").c_str());
								dblVal = JVX_MIN(50, dblVal);
								dblVal = JVX_MAX(-50, dblVal);
								this->horizontalSlider_ogain->setValue(dblVal + 50);
								this->label_outgain->setEnabled(true);
								this->horizontalSlider_ogain->setEnabled(true);
							}
							else
							{
								this->label_outgain->setEnabled(false);
								this->horizontalSlider_ogain->setEnabled(false);
								this->label_outgain->setText("--");
							}
						}
					}
					else
					{
						this->label_outgain->setEnabled(false);
						this->horizontalSlider_ogain->setEnabled(false);
						this->label_outgain->setText("--");
					}

					fullName = "";
					res = JVX_ERROR_NOT_READY;
					if (currentPropsDev)
					{
						ident.reset("/fill_channels_from_last_out");
						trans.reset(true);

						res = currentPropsDev->get_property(callGate, jPRG(&cboolVal, 1, JVX_DATAFORMAT_BOOL),
							ident, trans);
					}
					this->checkBox_fflast_out->setChecked(cboolVal == c_true);

					fullName = "";
					res = JVX_ERROR_NOT_READY;
					if (currentPropsDev)
					{
						ident.reset("/fill_channels_from_last_in");
						trans.reset(true);

						res = currentPropsDev->get_property(callGate, jPRG(&cboolVal, 1, JVX_DATAFORMAT_BOOL),
							ident, trans);
						this->checkBox_fflast_in->setChecked(cboolVal == c_true);
					}
					if (statDev >= JVX_STATE_PREPARED)
					{
						// Deactivate all elements of the device
						pushButton_inadd->setEnabled(false);
						pushButton_inrem->setEnabled(false);
						pushButton_outadd->setEnabled(false);
						pushButton_outrem->setEnabled(false);

						pushButton_inup->setEnabled(false);
						pushButton_indown->setEnabled(false);
						pushButton_outup->setEnabled(false);
						pushButton_outdown->setEnabled(false);

						lineEdit_srate->setEnabled(false);
						lineEdit_bsize->setEnabled(false);
						comboBox_formats->setEnabled(false);

						lineEdit_outwav_artist->setEnabled(false);
						lineEdit_outwav_title->setEnabled(false);
						pushButton_outwav_apply->setEnabled(false);
						pushButton_outwav_add->setEnabled(false);
					}
				} // else if (statDev >= JVX_STATE_ACTIVE)
			} // if (statTech >= JVX_STATE_ACTIVE)

			if (jvx_bitTest(modeShow, JVX_UI_SHOW_MODE_GENW_SHIFT))
			{
				//if (statTech >= JVX_STATE_SELECTED)
				if (statDev >= JVX_STATE_SELECTED)
				{
					ident.reset("/select_files/input_file");
					trans.reset();

					res = currentPropsTech->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
					if (res == JVX_NO_ERROR)
					{
						std::string txt;
						jvxSize num_input_files = selList.strList.ll();
						jvxSize id_infile = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), num_input_files);
						for (i = 0; i < num_input_files; i++)
						{
							QTreeWidgetItem* topL = new QTreeWidgetItem;

							fName_short = selList.strList.std_str_at(i);
							ident.reset("/properties_selected_input_file/friendly_name");
							trans.reset(true, i);

							res = currentPropsTech->get_property(callGate, jPRG(&strL, 1, JVX_DATAFORMAT_STRING), ident, trans);
							if (res == JVX_NO_ERROR)
							{
								fName_short = strL.std_str();
							}

							resL = jvx_tagExprToTagStruct(fName_short, tags, path);
							if (resL == JVX_NO_ERROR)
							{
								if (JVX_EVALUATE_BITFIELD(tags.tagsSet))
								{
									fName_short = jvx_tagStructToUserView(tags);
								}
								else
								{
									fName_short = path;
								}
							}
							topL->setData(0, Qt::UserRole, QVariant(JVX_SIZE_INT(i)));
							topL->setText(0, (jvx_int2String(JVX_SIZE_INT(i)).c_str()));
							fName_short = shortenStringName(NUMBER_LETTERS_FILENAMES, fName_short);
							topL->setText(1, fName_short.c_str());
							if (i == id_infile)
							{
								topL->setBackground(0, QBrush(Qt::gray));
								topL->setBackground(1, QBrush(Qt::gray));
								fullName = selList.strList.std_str_at(i);
							}
							treeWidget_infiles->addTopLevelItem(topL);
						}
						if (JVX_CHECK_SIZE_SELECTED(id_infile))
						{
							txt = fullName + "::";
							szVal = JVX_SIZE_UNSELECTED;

							ident.reset("/properties_selected_input_file/file_length");
							trans.reset(true, id_infile);

							res = currentPropsTech->get_property(callGate, jPRG(&szll, 1, JVX_DATAFORMAT_SIZE), ident, trans);
							assert(res == JVX_NO_ERROR);
							txt += "Length: " + jvx_size2String(szll, 1);

							szVal = JVX_SIZE_UNSELECTED;

							ident.reset("/properties_selected_input_file/samplerate");
							trans.reset(true, id_infile);

							res = currentPropsTech->get_property(callGate, jPRG(&szrr, 1, JVX_DATAFORMAT_SIZE), ident, trans);
							assert(res == JVX_NO_ERROR);
							txt += "Rate: " + jvx_size2String(szrr) + " Hz";

							txt += " Secs:" + jvx_data2String((jvxData)szll / (jvxData)szrr, 1);

							ident.reset("/properties_selected_input_file/mode");
							trans.reset(false, id_infile);

							res = currentPropsTech->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
							assert(res == JVX_NO_ERROR);

							szVal = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), selList.strList.ll());
							if (JVX_CHECK_SIZE_SELECTED(szVal))
							{
								txt += "; Mode: <";
								txt += selList.strList.std_str_at(szVal);
								txt += ">";
							}
							else
							{
								txt += "Mode Error";
							}
						}
						treeWidget_infiles->setToolTip(txt.c_str());

						// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
						// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

						ident.reset("/select_files/output_file");
						trans.reset();

						res = currentPropsTech->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
						assert(res == JVX_NO_ERROR);
						jvxSize num_output_files = selList.strList.ll();
						jvxSize id_outfile = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), num_output_files);
						for (i = 0; i < num_output_files; i++)
						{
							//res = currentPropsTech->get_property(callGate, jPRG(&fNames, 1, JVX_DATAFORMAT_STRING_LIST, properties.technology.id_output_files_f, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, false, callGate);
							QTreeWidgetItem* topL = new QTreeWidgetItem;

							fName_short = selList.strList.std_str_at(i);
							jvx_initTagName(tags);
							jvx_tagExprToTagStruct(fName_short, tags, path);
							if (JVX_EVALUATE_BITFIELD(tags.tagsSet))
							{
								fName_short = jvx_tagStructToUserView(tags);
								localSettings.artist = tags.tags[jvx_cbitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_ARTIST)];
								localSettings.title = tags.tags[jvx_cbitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_TITLE)];
							}
							else
							{
								fName_short = path;
							}
							topL->setData(0, Qt::UserRole, QVariant(JVX_SIZE_INT(i)));
							topL->setText(0, (jvx_int2String(JVX_SIZE_INT(i)).c_str()));
							fName_short = shortenStringName(NUMBER_LETTERS_FILENAMES, fName_short);
							topL->setText(1, fName_short.c_str());
							if (i == id_outfile)
							{
								topL->setBackground(0, QBrush(Qt::gray));
								topL->setBackground(1, QBrush(Qt::gray));
								fullName = selList.strList.std_str_at(i);
							}
							treeWidget_outfiles->addTopLevelItem(topL);
						}

						if (JVX_CHECK_SIZE_SELECTED(id_outfile))
						{
							std::string txt;
							txt = fullName;
							treeWidget_outfiles->setToolTip(txt.c_str());
						}
						lineEdit_outwav_artist->setText(localSettings.artist.c_str());
						lineEdit_outwav_title->setText(localSettings.title.c_str());
					}
				}
			}
		}

		if(myParent->subWidgets.theAudioArgs)
		{
			myParent->subWidgets.theAudioArgs->updateWindow();
		}
	}

	if (currentPropsDev)
	{
		currentPropsDev->user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
			reinterpret_cast<jvxHandle**>(&spec_Class));

		if (spec_Class)
		{
			spec_Class->updateWindow();
		}
	}
	spec_Class = NULL;

	if (currentPropsTech)
	{
		currentPropsTech->user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
			reinterpret_cast<jvxHandle**>(&spec_Class));
		if (spec_Class)
		{
			spec_Class->updateWindow();
		}
	}
}

void
	configureAudio::setAllEnabled(bool val)
{
	pushButton_inadd->setEnabled(val);
	treeWidget_infiles->setEnabled(val);
	pushButton_outadd->setEnabled(val);
	pushButton_outrem->setEnabled(val);
	treeWidget_outfiles->setEnabled(val);
	comboBox_technologies->setEnabled(val);
	comboBox_devices->setEnabled(val);
	pushButton_outdown->setEnabled(val);
	pushButton_outup->setEnabled(val);
	treeWidget_outchans->setEnabled(val);
	pushButton_inup->setEnabled(val);
	pushButton_indown->setEnabled(val);
	treeWidget_inchans->setEnabled(val);
	//pushButton_techuse->setEnabled(val);
}

void
	configureAudio::closeEvent ( QCloseEvent * event )
{
    QDialog::closeEvent(event);
    if(myParent->involvedHost.hHost)
    {
        myParent->updateWindow();
    }
};

void
configureAudio::select_technology(int idx)
{
	jvxSize i;
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num;
	jvxSelectionList selList;
	jvxSize hdlIdx;
	jvxBool isValid;
	jvxBool foundit = false;
	std::string str;
	jvxCallManagerProperties callGate;
	jvxComponentIdentification& tpIdT = myParent->tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];

	//jvxApiStringList lstStr;
	theHost = myParent->involvedHost.hHost;

	theHost->state_selected_component(tpIdT, &stat);
	if(stat == JVX_STATE_ACTIVE)
	{
		// Save current selection status
		jvxComponentIdentification tpIdD = myParent->tpAll[JVX_COMPONENT_AUDIO_DEVICE];

		// Deactivate all devices in slots related to this technology
		for (i = 0; i < runtime.id_device[tpIdT.slotid].size(); i++)
		{
			new_dev_slot(i);

			// First, deactivate the audio device if one is selected (required for configureAudio_specific)
			this->select_device(0);
			runtime.id_device[tpIdT.slotid][i] = 0;
		}

		// Restore current selection of device
		myParent->tpAll[JVX_COMPONENT_AUDIO_DEVICE] = tpIdD;

		theHost->deactivate_selected_component(tpIdT);
		runtime.id_technology[tpIdT.slotid] = 0;

		theHost->state_selected_component(tpIdT, &stat);
	}
	
	if (jvx_bitTest(modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
	{
		if (stat == JVX_STATE_SELECTED)
		{
			if (idx > 0)
			{
				if (res == JVX_NO_ERROR)
				{
					ident.reset("/JVX_GENW_TECHNOLOGIES");
					trans.reset(true, 0, JVX_PROPERTY_DECODER_SINGLE_SELECTION, true);

					// Show property with descriptor "/JVX_GENW_TECHNOLOGIES"
					num = 0;
					foundit = false;
					hdlIdx = 0;
					isValid = false;

					ident.reset("/JVX_GENW_TECHNOLOGIES");
					trans.reset(true);

					res = currentPropsTech->get_property(callGate, jPRG(
							&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), 
						ident, trans);
					if (res == JVX_NO_ERROR)
					{
						if ((idx < (jvxInt32)selList.strList.ll()))
						{
							selList.bitFieldSelected() = (jvxBitField)1 << (idx - 1);
							ident.reset("/JVX_GENW_TECHNOLOGIES");
							trans.reset(true);
							currentPropsTech->set_property(
								callGate,
								jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
								ident, trans);
						}
					} 

					runtime.id_technology[tpIdT.slotid] = idx;
					res = theHost->activate_selected_component(tpIdT);
				}
			}
		}
	} 
	else
	{

		theHost->state_selected_component(tpIdT, &stat);
		if (stat == JVX_STATE_SELECTED)
		{
			res = theHost->unselect_selected_component(tpIdT);
			assert(res == JVX_NO_ERROR);
		}

		if (idx > 0)
		{
			res = theHost->select_component(tpIdT, idx - 1);
			assert(res == JVX_NO_ERROR);

			res = theHost->activate_selected_component(tpIdT);
			assert(res == JVX_NO_ERROR);
		}
	}
	
	myParent->updateWindow(); // This will also refresh the property ids
}

void 
configureAudio::add_dummy_in()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxSelectionList theOldList;
	std::vector<std::string> lstNew;
	jvxSelectionList sel;
	jvxApiString jstr;
	jvxCallManagerProperties callGate;

	theHost = myParent->involvedHost.hHost;

	std::string command = "addDummyInput();";

	jstr.assign_const(command.c_str(), command.size());

	ident.reset("/JVX_GENW_COMMAND");
	trans.reset(false);

	res = currentPropsTech->set_property(
		callGate,
		jPRG(&jstr, 1, JVX_DATAFORMAT_STRING), 
		ident, trans);
	if (res != JVX_NO_ERROR)
	{
		this->myParent->postMessage_inThread("Failed to add dummy channel.", JVX_REPORT_PRIORITY_ERROR);
	}

	this->myParent->updateWindow();
}

void
configureAudio::add_dummy_out()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxSelectionList theOldList;
	std::vector<std::string> lstNew;
	jvxSelectionList sel;
	jvxApiString jstr;
	jvxCallManagerProperties callGate;
	theHost = myParent->involvedHost.hHost;

	std::string command = "addDummyOutput();";

	jstr.assign_const(command.c_str(), command.size());

	ident.reset("/JVX_GENW_COMMAND");
	trans.reset(false);

	res = currentPropsTech->set_property(callGate, jPRG( &jstr, 1, JVX_DATAFORMAT_STRING), 
		ident, trans);
	if (res != JVX_NO_ERROR)
	{
		this->myParent->postMessage_inThread("Failed to add dummy channel.", JVX_REPORT_PRIORITY_ERROR);
	}

	this->myParent->updateWindow();
}

void 
configureAudio::rem_dummy_in()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxApiString jstr;
	jvxCallManagerProperties callGate;
	theHost = myParent->involvedHost.hHost;

	std::string command = "remDummyInput();";

	jstr.assign_const(command.c_str(), command.size());

	ident.reset("/JVX_GENW_COMMAND");
	trans.reset(false);

	res = currentPropsTech->set_property(callGate ,
		jPRG(&jstr, 1, JVX_DATAFORMAT_STRING),
		ident, trans);
	if (res != JVX_NO_ERROR)
	{
		this->myParent->postMessage_inThread("Failed to remove dummy channel.", JVX_REPORT_PRIORITY_ERROR);
	}

	this->myParent->updateWindow();
}


void 
configureAudio::rem_dummy_out()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxApiString jstr;
	jvxCallManagerProperties callGate;
	theHost = myParent->involvedHost.hHost;

	std::string command = "remDummyOutput();";

	jstr.assign_const(command.c_str(), command.size());

	ident.reset("/JVX_GENW_COMMAND");
	trans.reset(false);

	res = currentPropsTech->set_property(callGate, jPRG(
		&jstr, 1, JVX_DATAFORMAT_STRING), 
		ident, trans);
	if (res != JVX_NO_ERROR)
	{
		this->myParent->postMessage_inThread("Failed to rem dummy channel.", JVX_REPORT_PRIORITY_ERROR);
	}

	this->myParent->updateWindow();
}

void
	configureAudio::add_infile()
{
	jvxSize i;
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxSelectionList sel;
	jvxApiString jstr;
	jvxApiString fldStr;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
		
	theHost = myParent->involvedHost.hHost;

	QStringList lst = QFileDialog::getOpenFileNames(this, tr("Open Input File"), tr(".\\"), tr("(*.*)"));

	for (i = 0; i < (jvxSize)lst.size(); i++)
	{
		std::string fileName = (std::string)lst[JVX_SIZE_INT(i)].toLatin1().constData();
		
#ifdef JVX_OS_WINDOWS
		fileName = jvx_replaceCharacter(fileName, '/', (JVX_SEPARATOR_DIR)[0]);
#endif

		fileName = jvx_absoluteToRelativePath(fileName, true);

#ifdef JVX_OS_WINDOWS
		fileName = jvx_replaceCharacter(fileName, (JVX_SEPARATOR_DIR)[0], '/');
#endif

		std::string command = "addInputFile(" + fileName + ");";

		jstr.assign_const(command.c_str(), command.size());

		ident.reset("/JVX_GENW_COMMAND");
		trans.reset(false);
		res = currentPropsTech->set_property(callGate, jPRG( &jstr, 1, JVX_DATAFORMAT_STRING),
			ident, trans);
		if (res == JVX_NO_ERROR)
		{
			ident.reset("/select_files/input_file");
			trans.reset(true);

			res = currentPropsTech->get_property(callGate, jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if (sel.strList.ll() > 0)
			{
				// Highlight the added item
				jvx_bitZSet(sel.bitFieldSelected(), sel.strList.ll() - 1);
				ident.reset("/select_files/input_file");
				trans.reset(true);
				res = currentPropsTech->set_property(callGate, jPRG(
					&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), 
					ident, trans );
			}
		}
		else
		{
			ident.reset("/JVX_GENW_COMMAND");
			trans.reset();

			currentPropsTech->get_property(callGate, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
			this->myParent->postMessage_inThread(("Failed to open input file " + fileName + ": " + fldStr.std_str() + ".").c_str(), JVX_REPORT_PRIORITY_ERROR);
		}
	}

	this->myParent->updateWindow();
}

void
configureAudio::rem_infile()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selList;
	jvxApiString jstr;
	jvxApiString fldStr;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	ident.reset("/select_files/input_file");
	trans.reset(true);

	res = currentPropsTech->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	assert(res == JVX_NO_ERROR);
	std::string txt;
	jvxSize num_input_files = selList.strList.ll();
	jvxSize id_infile = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), num_input_files);
	if(JVX_CHECK_SIZE_SELECTED(id_infile))
	{
		std::string command = "remInputFile(" + jvx_size2String(id_infile) + ");";
		jstr.assign_const(command.c_str(), command.size());

		ident.reset("/JVX_GENW_COMMAND");
		trans.reset(false);
		res = currentPropsTech->set_property(callGate ,
			jPRG(&jstr, 1, JVX_DATAFORMAT_STRING),
			ident, trans);
		if (res != JVX_NO_ERROR)
		{
			ident.reset("/JVX_GENW_COMMAND");
			trans.reset();

			currentPropsTech->get_property(callGate, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING),
				ident, trans);
			this->myParent->postMessage_inThread(("Failed to close input file id " + jvx_size2String(id_infile) + ", reason: " + fldStr.std_str()).c_str(), JVX_REPORT_PRIORITY_ERROR);
		}
		this->myParent->updateWindow();
	}
}

void
configureAudio::add_outfile()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res1 = JVX_NO_ERROR, res2 = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	std::vector<std::string> lstNewNames;
	std::vector<std::string> lstNewFNames;
	jvxSelectionList sel;
	std::string txt;
	jvxAssignedFileTags tags;
	jvx_initTagName(tags);
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString jstr;
	jvxApiString  fldStr;
	jvxApiStringList lst;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	QString qstr = QFileDialog::getSaveFileName(this, tr("Open Output File"), tr(".\\"), tr("(*.*)"));
	std::string fileName = (std::string)qstr.toLatin1().constData();
#ifdef JVX_OS_WINDOWS
	fileName = jvx_replaceCharacter(fileName, '/', (JVX_SEPARATOR_DIR)[0]);
#endif
	fileName = jvx_absoluteToRelativePath(fileName, true);
#ifdef JVX_OS_WINDOWS
	fileName = jvx_replaceCharacter(fileName, (JVX_SEPARATOR_DIR)[0], '/');
#endif

	txt = "";
	if (fileName.empty())
	{
		if (!localSettings.artist.empty())
		{
			tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_ARTIST;
			tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_ARTIST, JVX_NUMBER_AUDIO_FILE_TAGS)] = localSettings.artist;
		}
		if (!localSettings.title.empty())
		{
			tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_TITLE;
			tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_TITLE, JVX_NUMBER_AUDIO_FILE_TAGS)] = localSettings.title;
		}
	}
	jvx_tagStructToTagExpr(txt, tags, fileName);

	std::string command;
	command = "addOutputFile(" + txt + ");";

	jstr.assign_const(command.c_str(), command.size());

	ident.reset("/JVX_GENW_COMMAND");
	trans.reset(false);
	res = currentPropsTech->set_property(callGate, jPRG( 
		&jstr, 1, JVX_DATAFORMAT_STRING), 
		ident, trans);
	if (res == JVX_NO_ERROR)
	{
		ident.reset("/select_files/output_file");
		trans.reset(true);

		res = currentPropsTech->get_property(callGate, jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		assert(res == JVX_NO_ERROR);
		jvx_bitZSet(sel.bitFieldSelected(), sel.strList.ll() - 1);
		ident.reset("/select_files/output_file");
		trans.reset(true);
		res = currentPropsTech->set_property(callGate, jPRG(
			&sel, 1, JVX_DATAFORMAT_SELECTION_LIST),
			ident, trans);
		assert(res == JVX_NO_ERROR);
	}
	else
	{
		ident.reset("/JVX_GENW_COMMAND");
		trans.reset();

		currentPropsTech->get_property(callGate, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING),
			ident, trans);
		this->myParent->postMessage_inThread(("Failed to open output file " + fileName + "<" + txt + ">, reason: " +
			fldStr.std_str()).c_str(), JVX_REPORT_PRIORITY_ERROR);
	}

#if 0

	if(properties.technology.id_output_files != 0)
	{
		if((!localSettings.artist.empty()) || (!localSettings.title.empty()))
		{
			res1 = currentPropsTech->get_property(callGate, jPRG(&theOldList, 1,JVX_DATAFORMAT_SELECTION_LIST, properties.technology.id_output_files, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, false, callGate);
			res2 = currentPropsTech->get_property(callGate, jPRG(&fNames, 1, JVX_DATAFORMAT_STRING_LIST, properties.technology.id_output_files_f, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, false, callGate);
			if((res1 == JVX_NO_ERROR) && (res2 == JVX_NO_ERROR))
			{
				assert(isValid);
				assert(theOldList.strList.ll() == fNames.ll());

				for(i = 0; i < theOldList.strList.ll(); i++)
				{
					lstNewNames.push_back(theOldList.strList.std_str_at(i));
					lstNewFNames.push_back(fNames.std_str_at(i));
				}
				currentPropsTech->theObj->deallocate(theOldList.strList);
				currentPropsTech->theObj->deallocate(fNames);

				// Select the new file
				runtime.id_outfile = lstNewNames.size();

				QString str = QFileDialog::getSaveFileName(this, tr("Open Output File"), tr(".\\"), tr("(*.*)"));

				std::string fileName = (std::string)str.toLatin1().constData();

				lstNewNames.push_back(fileName);

				txt = "";
				if(!localSettings.artist.empty())
				{
					tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_ARTIST;
					tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_ARTIST, JVX_NUMBER_AUDIO_FILE_TAGS)] = localSettings.artist;
				}
				if(!localSettings.title.empty())
				{
					tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_TITLE;
					tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_TITLE, JVX_NUMBER_AUDIO_FILE_TAGS)] = localSettings.title;
				}
				if(tags.tagsSet)
				{
					jvx_tagStructToTagExpr(txt, tags, "");
				}
				lstNewFNames.push_back(txt);

				theList.bitFieldSelected() = (jvxBitField)-1;
				HjvxObject_produceStringList(&theList.strList, lstNewNames);

				HjvxObject_produceStringList(&fNames, lstNewFNames);

				res1 = currentPropsTech->set_property__descriptor(fNames, 1, JVX_DATAFORMAT_STRING_LIST, properties.technology.id_output_files_f, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, false);
				res2 = currentPropsTech->set_property__descriptor(&theList, 1,JVX_DATAFORMAT_SELECTION_LIST, properties.technology.id_output_files, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, false);
				if((res1 == JVX_NO_ERROR) && (res1 == JVX_NO_ERROR))
				{
					// Select the new file
					runtime.id_outfile = lstNewNames.size()-1;
				}
				HjvxObject_deallocate(theList.strList);

				if(properties.device.id_out_channels > 0)
				{
					res1 = currentPropsDev->get_property(callGate, jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST,
						properties.device.id_out_channels, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, true, NULL, NULL);
					res1 = currentPropsDev->set_property__descriptor(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST,
						properties.device.id_out_channels, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, true);
				}
			}
		}
	}
#endif
	this->myParent->updateWindow();
}

void
configureAudio::rem_outfile()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res1 = JVX_NO_ERROR, res2 = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxSelectionList theList;
	jvxSelectionList sel;
	jvxApiString  fldStr;
	jvxApiString jstr;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	ident.reset("/select_files/output_file");
	trans.reset(true);

	jvxErrorType res = currentPropsTech->get_property(callGate, jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	assert(res == JVX_NO_ERROR);
	jvxSize num_output_files = sel.strList.ll();
	jvxSize id_outfile = jvx_bitfieldSelection2Id(sel.bitFieldSelected(), num_output_files);
	if (JVX_CHECK_SIZE_SELECTED(id_outfile))
	{
		ident.reset("/select_files/output_file");
		trans.reset(true, id_outfile);

		res = currentPropsTech->get_property(callGate, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
		assert(res == JVX_NO_ERROR);

		std::string command;
		command = "remOutputFile(" + jvx_size2String(id_outfile) + ");";
		//command += fldStr.std_str();
		//command += ", JVX_PROPERTY_DECODER_NONE, NULL, callGate)";
		jstr.assign_const(command.c_str(), command.size());

		ident.reset("/JVX_GENW_COMMAND");
		trans.reset(true);

		res = currentPropsTech->set_property(callGate ,
			jPRG(&jstr, 1, JVX_DATAFORMAT_STRING),
			ident, trans );
		if (res == JVX_NO_ERROR)
		{
			ident.reset("/select_files/output_file");
			trans.reset(true);

			res = currentPropsTech->get_property(callGate, jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			assert(res == JVX_NO_ERROR);
			if (sel.strList.ll() == 0)
			{
				jvx_bitZero(sel.bitFieldSelected());
			}
			else
			{
				jvx_bitZSet(sel.bitFieldSelected(), sel.strList.ll() - 1);
			}

			ident.reset("/select_files/output_file");
			trans.reset(true);
			res = currentPropsTech->set_property(callGate ,
				jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST),
				ident, trans);
			assert(res == JVX_NO_ERROR);
		}
	}
	this->myParent->updateWindow();
}

void
	configureAudio::select_device(int idx)
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_NONE;
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification tpHere;
	theHost = myParent->involvedHost.hHost;
	jvxComponentIdentification& tpIdD = myParent->tpAll[JVX_COMPONENT_AUDIO_DEVICE];

	theHost->state(&stat);

	if(stat == JVX_STATE_ACTIVE)
	{
		res = theHost->state_selected_component(tpIdD, &stat);
		assert(res == JVX_NO_ERROR);

		if(idx != runtime.id_device[tpIdD.slotid][tpIdD.slotsubid])
		{
			jvxBool mayRunDeactivate = false;
			if (tpIdD.slotid == 0 && tpIdD.slotsubid == 0)
			{
				// This is the master audio device
				if (stat == JVX_STATE_ACTIVE)
				{
					mayRunDeactivate = true;
				}
			}
			else
			{
				if (stat >= JVX_STATE_ACTIVE)
				{
					mayRunDeactivate = true;
				}
			}

			if(mayRunDeactivate)
			{
				// The deactivate may fail if tried to deactivate while processing
				res = theHost->deactivate_selected_component(tpIdD);
				if (res != JVX_NO_ERROR)
				{
					this->myParent->updateWindow();
					return;
				}

				res = theHost->state_selected_component(tpIdD, &stat);
				assert(res == JVX_NO_ERROR);
			}
			if(stat == JVX_STATE_SELECTED)
			{
				theHost->unselect_selected_component(tpIdD);
			}
		}
		runtime.id_device[tpIdD.slotid][tpIdD.slotsubid] = idx;
	}
	return this->device_use_button();

}

void
configureAudio::up_inchan()
{
	jvxSize i;
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxSelectionList theOldList;
	jvxSelectionList theNewList;
	std::vector<std::string> replaceList;
	jvxCallManagerProperties callGate;
	theHost = myParent->involvedHost.hHost;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	if(JVX_CHECK_SIZE_SELECTED(runtime.id_inchan) && (runtime.id_inchan > 0))
	{
		ident.reset("/system/sel_input_channels");
		trans.reset();

		res = currentPropsDev->get_property(callGate, jPRG(&theOldList, 1, JVX_DATAFORMAT_SELECTION_LIST), 
			ident, trans);

		// currentPropsDev->get_property(callGate, jPRG(&theOldList, 1,JVX_DATAFORMAT_SELECTION_LIST, properties.device.id_in_channels, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, callGate);
		if (res == JVX_NO_ERROR)
		{
			assert(callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK);
			theNewList.bitFieldSelected() = 0;
			theNewList.bitFieldExclusive = 0;

			for (i = 0; i < theOldList.strList.ll(); i++)
			{
				if (i == runtime.id_inchan)
				{
					replaceList.push_back(theOldList.strList.std_str_at(runtime.id_inchan - 1));
					if (jvx_bitTest(theOldList.bitFieldSelected(), (runtime.id_inchan - 1)))
					{
						jvx_bitSet(theNewList.bitFieldSelected(), runtime.id_inchan);
					}
					if (jvx_bitTest(theOldList.bitFieldExclusive, (runtime.id_inchan - 1)))
					{
						jvx_bitSet(theNewList.bitFieldExclusive, runtime.id_inchan);
					}
				}
				else
				{
					if (i == runtime.id_inchan - 1)
					{
						replaceList.push_back(theOldList.strList.std_str_at(runtime.id_inchan));
						if (jvx_bitTest(theOldList.bitFieldSelected(), (runtime.id_inchan)))
						{
							jvx_bitSet(theNewList.bitFieldSelected(), (runtime.id_inchan - 1));
						}
						if (jvx_bitTest(theOldList.bitFieldExclusive, (runtime.id_inchan)))
						{
							jvx_bitSet(theNewList.bitFieldExclusive, (runtime.id_inchan - 1));
						}
					}
					else
					{
						replaceList.push_back(theOldList.strList.std_str_at(i));
						if (jvx_bitTest(theOldList.bitFieldSelected(), i))
						{
							jvx_bitSet(theNewList.bitFieldSelected(), i);
						}
						if (jvx_bitTest(theOldList.bitFieldExclusive, i))
						{
							jvx_bitSet(theNewList.bitFieldExclusive, i);
						}
					}
				}
			}
			runtime.id_inchan--;
			theNewList.strList.assign(replaceList);

			ident.reset("/system/sel_input_channels");
			trans.reset(false);

			res = currentPropsDev->set_property(callGate ,
				jPRG(&theNewList, 1, JVX_DATAFORMAT_SELECTION_LIST),
				ident, trans );
			// currentPropsDev->set_property__descriptor(&theNewList, 1, JVX_DATAFORMAT_SELECTION_LIST, properties.device.id_in_channels, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false);
		}
		this->myParent->updateWindow();
	}
}

void
	configureAudio::down_inchan()
{
	jvxSize i;
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxSelectionList theOldList;
	jvxSelectionList theNewList;
	std::vector<std::string> replaceList;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	theHost = myParent->involvedHost.hHost;

	if(JVX_CHECK_SIZE_SELECTED(runtime.id_inchan))
	{
		ident.reset("/system/sel_input_channels");
		trans.reset();

		res = currentPropsDev->get_property(callGate, jPRG(&theOldList, 1, JVX_DATAFORMAT_SELECTION_LIST),
			ident, trans);
		// currentPropsDev->get_property(callGate, jPRG(&theOldList, 1,JVX_DATAFORMAT_SELECTION_LIST, properties.device.id_in_channels, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, callGate);
		if (res == JVX_NO_ERROR)
		{
			assert(callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK);

			theNewList.bitFieldSelected() = 0;
			theNewList.bitFieldExclusive = 0;

			if (runtime.id_inchan < theOldList.strList.ll() - 1)
			{
				for (i = 0; i < theOldList.strList.ll(); i++)
				{
					if (i == runtime.id_inchan)
					{
						replaceList.push_back(theOldList.strList.std_str_at(runtime.id_inchan + 1));
						if (jvx_bitTest(theOldList.bitFieldSelected(), (runtime.id_inchan + 1)))
						{
							jvx_bitSet(theNewList.bitFieldSelected(), runtime.id_inchan);
						}
						if (jvx_bitTest(theOldList.bitFieldExclusive, (runtime.id_inchan + 1)))
						{
							jvx_bitSet(theNewList.bitFieldExclusive, runtime.id_inchan);
						}
					}
					else
					{
						if (i == runtime.id_inchan + 1)
						{
							replaceList.push_back(theOldList.strList.std_str_at(runtime.id_inchan));
							if (jvx_bitTest(theOldList.bitFieldSelected(), (runtime.id_inchan)))
							{
								jvx_bitSet(theNewList.bitFieldSelected(), runtime.id_inchan + 1);
							}
							if (jvx_bitTest(theOldList.bitFieldExclusive, (runtime.id_inchan)))
							{
								jvx_bitSet(theNewList.bitFieldExclusive, runtime.id_inchan + 1);
							}
						}
						else
						{
							replaceList.push_back(theOldList.strList.std_str_at(i));
							if (jvx_bitTest(theOldList.bitFieldSelected(), i))
							{
								jvx_bitSet(theNewList.bitFieldSelected(), i);
							}
							if (jvx_bitTest(theOldList.bitFieldExclusive, i))
							{
								jvx_bitSet(theNewList.bitFieldExclusive, i);
							}
						}
					}
				}
				runtime.id_inchan++;
				theNewList.strList.assign(replaceList);
				
				ident.reset("/system/sel_input_channels");
				trans.reset(false);

				res = currentPropsDev->set_property(callGate ,
					jPRG(&theNewList, 1, JVX_DATAFORMAT_SELECTION_LIST),
					ident, trans );
			}
		}
	
		this->myParent->updateWindow();
	}
}

void
	configureAudio::up_outchan()
{
	jvxSize i;
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxSelectionList theOldList;
	jvxSelectionList theNewList;
	std::vector<std::string> replaceList;
	jvxCallManagerProperties callGate;
	theHost = myParent->involvedHost.hHost;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	if(JVX_CHECK_SIZE_SELECTED(runtime.id_outchan) && (runtime.id_outchan > 0))
	{
		ident.reset("/system/sel_output_channels");
		trans.reset();

		res = currentPropsDev->get_property(callGate, jPRG(&theOldList, 1, JVX_DATAFORMAT_SELECTION_LIST),
			ident, trans); ;
		// currentPropsDev->get_property(callGate, jPRG(&theOldList, 1, JVX_DATAFORMAT_SELECTION_LIST, properties.device.id_out_channels, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, callGate);
		if (res == JVX_NO_ERROR)
		{
			assert(callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK);

			theNewList.bitFieldSelected() = 0;
			theNewList.bitFieldExclusive = 0;

			for (i = 0; i < theOldList.strList.ll(); i++)
			{
				if (i == runtime.id_outchan)
				{
					replaceList.push_back(theOldList.strList.std_str_at(runtime.id_outchan - 1));
					if (jvx_bitTest(theOldList.bitFieldSelected(), (runtime.id_outchan - 1)))
					{
						jvx_bitSet(theNewList.bitFieldSelected(), runtime.id_outchan);
					}
					if (jvx_bitTest(theOldList.bitFieldExclusive, (runtime.id_outchan - 1)))
					{
						jvx_bitSet(theNewList.bitFieldExclusive, runtime.id_outchan);
					}
				}
				else
				{
					if (i == runtime.id_outchan - 1)
					{
						replaceList.push_back(theOldList.strList.std_str_at(runtime.id_outchan));
						if (jvx_bitTest(theOldList.bitFieldSelected(), (runtime.id_outchan)))
						{
							jvx_bitSet(theNewList.bitFieldSelected(), runtime.id_outchan - 1);
						}
						if (jvx_bitTest(theOldList.bitFieldExclusive, (runtime.id_outchan)))
						{
							jvx_bitSet(theNewList.bitFieldExclusive, runtime.id_outchan - 1);
						}
					}
					else
					{
						replaceList.push_back(theOldList.strList.std_str_at(i));
						if (jvx_bitTest(theOldList.bitFieldSelected(), i))
						{
							jvx_bitSet(theNewList.bitFieldSelected(), i);
						}
						if (jvx_bitTest(theOldList.bitFieldExclusive, i))
						{
							jvx_bitSet(theNewList.bitFieldExclusive, i);
						}
					}
				}
			}
			runtime.id_outchan--;
			theNewList.strList.assign(replaceList);
			ident.reset("/system/sel_output_channels");
			trans.reset(false);

			res = currentPropsDev->set_property(callGate, jPRG( 
				&theNewList, 1, JVX_DATAFORMAT_SELECTION_LIST), 
				ident, trans);
		}
		this->myParent->updateWindow();
	}
}

void
	configureAudio::down_outchan()
{
	jvxSize i;
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	
	jvxBool foundit = false;
	std::string str;
	jvxSelectionList theOldList;
	jvxSelectionList theNewList;
	std::vector<std::string> replaceList;
	jvxCallManagerProperties callGate;
	theHost = myParent->involvedHost.hHost;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	if(JVX_CHECK_SIZE_SELECTED(runtime.id_outchan))
	{
		ident.reset("/system/sel_output_channels");
		trans.reset();

		res = currentPropsDev->get_property(callGate, jPRG(&theOldList, 1, JVX_DATAFORMAT_SELECTION_LIST),
			ident, trans);
		// currentPropsDev->get_property(callGate, jPRG(&theOldList, 1, JVX_DATAFORMAT_SELECTION_LIST, properties.device.id_out_channels, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, callGate);
		if (res == JVX_NO_ERROR)
		{
			assert(callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK);

			theNewList.bitFieldSelected() = 0;
			theNewList.bitFieldExclusive = 0;

			if (runtime.id_outchan < theOldList.strList.ll() - 1)
			{
				for (i = 0; i < theOldList.strList.ll(); i++)
				{
					if (i == runtime.id_outchan)
					{
						replaceList.push_back(theOldList.strList.std_str_at(runtime.id_outchan + 1));
						if (jvx_bitTest(theOldList.bitFieldSelected(), (runtime.id_outchan + 1)))
						{
							jvx_bitSet(theNewList.bitFieldSelected(), runtime.id_outchan);
						}
						if (jvx_bitTest(theOldList.bitFieldExclusive, (runtime.id_outchan + 1)))
						{
							jvx_bitSet(theNewList.bitFieldExclusive, runtime.id_outchan);
						}
					}
					else
					{
						if (i == runtime.id_outchan + 1)
						{
							replaceList.push_back(theOldList.strList.std_str_at(runtime.id_outchan));
							if (jvx_bitTest(theOldList.bitFieldSelected(), (runtime.id_outchan)))
							{
								jvx_bitSet(theNewList.bitFieldSelected(), runtime.id_outchan + 1);
							}
							if (jvx_bitTest(theOldList.bitFieldExclusive, (runtime.id_outchan)))
							{
								jvx_bitSet(theNewList.bitFieldExclusive, runtime.id_outchan + 1);
							}
						}
						else
						{
							replaceList.push_back(theOldList.strList.std_str_at(i));
							if (jvx_bitTest(theOldList.bitFieldSelected(), i))
							{
								jvx_bitSet(theNewList.bitFieldSelected(), i);
							}
							if (jvx_bitTest(theOldList.bitFieldExclusive, i))
							{
								jvx_bitSet(theNewList.bitFieldExclusive, i);
							}
						}
					}
				}
				runtime.id_outchan++;
				theNewList.strList.assign(replaceList);
				ident.reset("/system/sel_output_channels");
				trans.reset(false);

				res = currentPropsDev->set_property(callGate ,
					jPRG(&theNewList, 1, JVX_DATAFORMAT_SELECTION_LIST),
					ident, trans);
			}
		}
		this->myParent->updateWindow();
	}
}

void
	configureAudio::select_format(int selection)
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	
	jvxBool foundit = false;
	std::string str;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	theHost = myParent->involvedHost.hHost;
	
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	ident.reset("/system/sel_dataformat");
	trans.reset(true);

	res = currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if (res == JVX_NO_ERROR)
	{
		if (selection < (jvxInt32)selList.strList.ll())
		{
			selList.bitFieldSelected() = ((jvxBitField)1 << selection);
			ident.reset("/system/sel_dataformat");
			trans.reset(true);

			res = currentPropsDev->set_property(callGate ,
				jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
				ident, trans);
			if (res != JVX_NO_ERROR)
			{
				assert(0);
			}
		}
		else
		{
			assert(0);
		}
		this->myParent->updateWindow();
	}
}

void
	configureAudio::selection_infiles(QTreeWidgetItem* it,int colId)
{
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	QVariant idSelect = it->data(0, Qt::UserRole);
	int idInFile = idSelect.toInt();

	ident.reset("/select_files/input_file");
	trans.reset(true);

	jvxErrorType res = currentPropsTech->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	assert(res == JVX_NO_ERROR);
	
	assert(idInFile < selList.strList.ll());

	jvx_bitZSet(selList.bitFieldSelected(), idInFile);

	ident.reset("/select_files/input_file");
	trans.reset(true);
	
	res = currentPropsTech->set_property(callGate, jPRG(
		&selList,  1, JVX_DATAFORMAT_SELECTION_LIST),
		ident, trans);
	assert(res == JVX_NO_ERROR);

	myParent->updateWindow();
}

void
	configureAudio::selection_outfiles(QTreeWidgetItem* it,int colId)
{
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	QVariant idSelect = it->data(0, Qt::UserRole);

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	int idOutFile = idSelect.toInt();

	ident.reset("/select_files/output_file");
	trans.reset(true);

	jvxErrorType res = currentPropsTech->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	assert(res == JVX_NO_ERROR);

	assert(idOutFile < selList.strList.ll());

	jvx_bitZSet(selList.bitFieldSelected(), idOutFile);

	ident.reset("/select_files/output_file");
	trans.reset(true);

	res = currentPropsTech->set_property(callGate, jPRG(
		&selList,  1, JVX_DATAFORMAT_SELECTION_LIST), 
		ident, trans );
	assert(res == JVX_NO_ERROR);

	myParent->updateWindow();
}

void
	configureAudio::selection_inchans(QTreeWidgetItem* it,int colId)
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	jvxBool foundit = false;
	std::string str;
	QVariant idSelect = it->data(0, Qt::UserRole);
	int idInChan = idSelect.toInt();
	theHost = myParent->involvedHost.hHost;
	theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	if (stat == JVX_STATE_ACTIVE)
	{
		if(colId == 0)
		{
			ident.reset("/system/sel_input_channels");
			trans.reset(true);

			currentPropsDev->get_property(callGate, jPRG(&selList, 1,
				JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if (res == JVX_NO_ERROR)
			{
				if (idInChan < (jvxInt32)selList.strList.ll())
				{
					jvxBitField fldSet = ((jvxBitField)1 << idInChan);
					selList.bitFieldSelected() = selList.bitFieldSelected()
						^ fldSet;

					currentPropsDev->property_start_delayed_group(callGate, NULL, NULL);
					ident.reset("/system/sel_input_channels");
					trans.reset(true);

					currentPropsDev->set_property(callGate, jPROSL(selList),
						ident, trans);
					currentPropsDev->property_stop_delayed_group(callGate, NULL);
				}
			}
		}
	}
	if (stat >= JVX_STATE_ACTIVE)
	{
		this->runtime.id_inchan = idInChan;
		myParent->updateWindow();
	}
}

void
configureAudio::selection_outchans(QTreeWidgetItem* it,int colId)
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selList;
	std::string str;
	jvxCallManagerProperties callGate;
	
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	QVariant idSelect = it->data(0, Qt::UserRole);
	int idOutChan = idSelect.toInt();

	theHost = myParent->involvedHost.hHost;
	theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
	if(stat == JVX_STATE_ACTIVE)
	{
		if(colId == 0)
		{
			ident.reset("/system/sel_output_channels");
			trans.reset(true);

			res = currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if (res == JVX_NO_ERROR)
			{
				if (idOutChan < (jvxInt32)selList.strList.ll())
				{
					jvxBitField fldSet = ((jvxBitField)1 << idOutChan);
					selList.bitFieldSelected() = selList.bitFieldSelected() ^ fldSet;
					currentPropsDev->property_start_delayed_group(callGate, NULL, NULL);
					
					ident.reset("/system/sel_output_channels");
					trans.reset(true);

					currentPropsDev->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
						ident, trans);
					currentPropsDev->property_stop_delayed_group(callGate, NULL);
				}
			}
		}
	}

	if(stat >= JVX_STATE_ACTIVE)
	{
		this->runtime.id_outchan = idOutChan;
		myParent->updateWindow();
	}
}

void
	configureAudio::technology_use_button()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;

	jvxComponentIdentification& tpIdT = myParent->tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];
	theHost = myParent->involvedHost.hHost;
	theHost->state( &stat);

	if(stat == JVX_STATE_ACTIVE)
	{
		theHost->state_selected_component(tpIdT, &stat);
		if(stat == JVX_STATE_SELECTED)
		{
			res = theHost->activate_selected_component(tpIdT);
			assert(res == JVX_NO_ERROR);
		}
		if(stat == JVX_STATE_ACTIVE)
		{
			res = theHost->deactivate_selected_component(tpIdT);
			assert(res == JVX_NO_ERROR);
		}
	}
	myParent->updateWindow();
}

void
	configureAudio::device_use_button()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_NONE;
	jvxErrorType res = JVX_NO_ERROR;

	theHost = myParent->involvedHost.hHost;
	theHost->state( &stat);
	jvxSize slotId = 0;
	jvxSize slotSubId = 0;
	jvxComponentIdentification& tpIdT = tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];
	jvxComponentIdentification& tpIdD = tpAll[JVX_COMPONENT_AUDIO_DEVICE];
	if(stat == JVX_STATE_ACTIVE)
	{
		res = theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
		assert(res == JVX_NO_ERROR);

		if(stat == JVX_STATE_NONE)
		{
			if(JVX_CHECK_SIZE_SELECTED(runtime.id_device[tpIdT.slotid][tpIdD.slotsubid]) && 
				(runtime.id_device[tpIdT.slotid][tpIdD.slotsubid] > 0))
			{
				res = theHost->select_component(tpIdD, runtime.id_device[tpIdT.slotid][tpAll[JVX_COMPONENT_AUDIO_DEVICE].slotsubid]-1);
				if(res != JVX_NO_ERROR)
				{
					REPORT_ERROR_MESSAGE(res, "Failed to select device");
				}
				
			}
		}

		res = theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
		assert(res == JVX_NO_ERROR);
		if(stat == JVX_STATE_SELECTED)
		{
			res = theHost->activate_selected_component(tpIdD);
			if(res != JVX_NO_ERROR)
			{
				REPORT_ERROR_MESSAGE(res, "Failed to activate device");
				res = theHost->unselect_selected_component(tpIdD);
				runtime.id_device[tpIdT.slotid][tpIdD.slotsubid] = 0;
			}
		}
	}
	myParent->updateWindow(); // this will also refreshPorpertyId
}

void
	configureAudio::startProcessing(jvxInt32 period_ms)
{
	if(period_ms > 0)
	{
		config.period_ms = period_ms;
	}

	if (myParent->subWidgets.main.theWidget)
	{
		myParent->subWidgets.main.theWidget->inform_viewer_about_to_start(&config.period_ms);
	}

	JVX_GET_TICKCOUNT_US_SETREF(&tStampRef);
	start_timer();
}

void
configureAudio::stopProcessing()
{
	if (this->timerViewUpdate)
	{
		stop_timer();
	}
}

void
	configureAudio::timerExpired()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxData dblVal;
	jvxInt32 idxSet;
	jvxComponentIdentification& tpIdT = tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];
	QPalette palette;
	QBrush brush_norm(Qt::white);
	QBrush brush_old(Qt::red);
	IjvxQtComponentWidget* spec_Class = NULL;
	jvxCallManagerProperties callGate;
	
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	theHost = myParent->involvedHost.hHost;
	res = theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);


	// Device processing
	if(stat >= JVX_STATE_PREPARED)
	{

		if(JVX_CHECK_SIZE_SELECTED(runtime.id_inchan))
		{
			ident.reset("/JVX_GENW_DEV_ILEVEL");
			trans.reset(true, runtime.id_inchan);

			res = currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), ident, trans);
			dblVal = 10 * log10(dblVal + EPS);
			idxSet = JVX_DATA2INT32(100.0 + dblVal);
			idxSet = JVX_MIN(idxSet, 100);
			idxSet = JVX_MAX(idxSet, 0);
			this->progressBar_inchan_level->setValue(idxSet);
			this->label_inchan_level->setText((jvx_data2String(dblVal, NUMBER_DIGITS_LEVEL) + " dB").c_str());

			ident.reset("/JVX_GENW_DEV_ILEVEL_MAX");
			trans.reset(true, runtime.id_inchan);

			res = currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), ident, trans);

			palette = this->label_inchan_level_max->palette();
			if (dblVal > THRESHOLD)
			{
				palette.setBrush(QPalette::Active, QPalette::WindowText, brush_old);
			}
			else
			{
				palette.setBrush(QPalette::Active, QPalette::WindowText, brush_norm);
			}
			this->label_inchan_level_max->setPalette(palette);

			dblVal = 10 * log10(dblVal + EPS);
			this->label_inchan_level_max->setText((jvx_data2String(dblVal, NUMBER_DIGITS_LEVEL) + " dB").c_str());

			dblVal = 0;
			ident.reset("/JVX_GENW_DEV_ILEVEL_MAX");
			trans.reset(true, runtime.id_inchan);

			res = currentPropsDev->set_property(callGate, jPRG(
				&dblVal, 1, JVX_DATAFORMAT_DATA),
				ident, trans);
		}
		else
		{
			this->progressBar_inchan_level->setValue(0);
			this->label_inchan_level->setText("");
			this->label_inchan_level_max->setText("");
		}
		
		if(JVX_CHECK_SIZE_SELECTED(runtime.id_outchan))
		{
			ident.reset("/JVX_GENW_DEV_OLEVEL");
			trans.reset(true, runtime.id_outchan);

			res = currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), ident, trans);
			dblVal = 10 * log10(dblVal + EPS);
			idxSet = JVX_DATA2INT32(100.0 + dblVal);
			idxSet = JVX_MIN(idxSet, 100);
			idxSet = JVX_MAX(idxSet, 0);
			this->progressBar_outchan_level->setValue(idxSet);
			this->label_outchan_level->setText((jvx_data2String(dblVal, NUMBER_DIGITS_LEVEL) + " dB").c_str());

			ident.reset("/JVX_GENW_DEV_OLEVEL_MAX");
			trans.reset(true, runtime.id_outchan);

			res = currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), ident, trans);

			palette = this->label_outchan_level_max->palette();
			if (dblVal > THRESHOLD)
			{
				palette.setBrush(QPalette::Active, QPalette::WindowText, brush_old);
			}
			else
			{
				palette.setBrush(QPalette::Active, QPalette::WindowText, brush_norm);
			}
			this->label_outchan_level_max->setPalette(palette);


			dblVal = 10 * log10(dblVal + EPS);

			this->label_outchan_level_max->setText((jvx_data2String(dblVal, NUMBER_DIGITS_LEVEL) + " dB").c_str());

			dblVal = 0;
			ident.reset("/JVX_GENW_DEV_OLEVEL_MAX");
			trans.reset(true, runtime.id_outchan);
			res = currentPropsDev->set_property(callGate, jPRG(
				&dblVal, 1, JVX_DATAFORMAT_DATA),
				ident, trans);
		}
		else
		{
			this->progressBar_outchan_level->setValue(0);
			this->label_outchan_level->setText("--");
			this->label_outchan_level_max->setText("--");
		}
	}

	if (currentPropsDev)
	{
		currentPropsDev->user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
			reinterpret_cast<jvxHandle**>(&spec_Class));
		if (spec_Class)
		{
			spec_Class->updateWindow_periodic();
		}
	}
	spec_Class = NULL;

	if (currentPropsTech)
	{
		currentPropsTech->user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
			reinterpret_cast<jvxHandle**>(&spec_Class));
		if (spec_Class)
		{
			spec_Class->updateWindow_periodic();
		}
	}
	this->myParent->updateWindow_periodic();
	myParent->subWidgets.theAudioArgs->updateWindow_periodic();
}

void
	configureAudio::edit_samplerate()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 valI32;
	QString txt = this->lineEdit_srate->text();
	jvxCallManagerProperties callGate;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	valI32 = txt.toInt();

	theHost = myParent->involvedHost.hHost;
	if (theHost)
	{
		theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
		if (stat == JVX_STATE_ACTIVE)
		{
			currentPropsDev->property_start_delayed_group(callGate, nullptr, nullptr );
			
			ident.reset("/system/rate");
			trans.reset(true);

			res = currentPropsDev->set_property(callGate ,
				jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE),
				ident, trans);
			assert(res == JVX_NO_ERROR);
			currentPropsDev->property_stop_delayed_group(callGate, nullptr);
			this->myParent->updateWindow();
		}
	}
}

void
	configureAudio::edit_buffersize()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 valI32;
	QString txt = this->lineEdit_bsize->text();
	valI32 = txt.toInt();
	jvxCallManagerProperties callGate;
	
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	

	theHost = myParent->involvedHost.hHost;
	if (theHost)
	{
		theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
		if (stat == JVX_STATE_ACTIVE)
		{
			currentPropsDev->property_start_delayed_group(callGate, NULL, NULL);
			
			ident.reset("/system/framesize");
			trans.reset(true);

			res = currentPropsDev->set_property(callGate, jPRG(
				&valI32, 1, JVX_DATAFORMAT_32BIT_LE), 
				ident, trans);
			assert(res == JVX_NO_ERROR);
			currentPropsDev->property_stop_delayed_group(callGate, NULL);
			this->myParent->updateWindow();
		}
	}
}

void
configureAudio::slider_in_reset()
{
	jvxSize i;
	jvxData valDbl = 1.0;

	if (runtime.in_all)
	{
		for (i = 0; i < runtime.in_channel_mapper.size(); i++)
		{
			runtime.in_channel_mapper[i].gain = valDbl;
		}
	}
	else
	{
		if (runtime.id_inchan < (jvxInt32)runtime.in_channel_mapper.size())
		{
			runtime.in_channel_mapper[runtime.id_inchan].gain = valDbl;
		}
	}

	// In right state, set value in generic wrapper module
	updateVariables_whileRunning();

	this->myParent->updateWindow();
}

void
configureAudio::slider_out_reset()
{
	jvxSize i;
	jvxData valDbl = 1.0;

	if (runtime.out_all)
	{
		for (i = 0; i < runtime.out_channel_mapper.size(); i++)
		{
			runtime.out_channel_mapper[i].gain = valDbl;
		}
	}
	else
	{
		if (runtime.id_outchan < (jvxInt32)runtime.out_channel_mapper.size())
		{
			runtime.out_channel_mapper[runtime.id_outchan].gain = valDbl;
		}
	}

	// In right state, set value in generic wrapper module
	updateVariables_whileRunning();

	this->myParent->updateWindow();
}

void
	configureAudio::new_input_gain()
{
	jvxSize i;
	int val = this->horizontalSlider_igain->value();
	jvxData valDbl = val - 50.0;
	valDbl = pow(10.0, valDbl/20.0);

	if (val == 0)
	{
		valDbl = 0.0;
	}
	if(runtime.in_all)
	{
		for(i = 0; i < runtime.in_channel_mapper.size(); i++)
		{
			runtime.in_channel_mapper[i].gain = valDbl;
		}
	}
	else
	{
		if(runtime.id_inchan < (jvxInt32)runtime.in_channel_mapper.size())
		{
			runtime.in_channel_mapper[runtime.id_inchan].gain = valDbl;
		}
	}

	// In right state, set value in generic wrapper module
	updateVariables_whileRunning();

	this->myParent->updateWindow();
}

void
configureAudio::updateVariablesSequencerNotification()
{
	refreshPropertyIds_run();
	updateVariables_whileRunning();
}

void
	configureAudio::toggle_inall()
{
	jvxSize i;
	if(runtime.in_all)
	{
		runtime.in_all = false;
	}
	else
	{
		runtime.in_all = true;
		/*
		if(runtime.id_inchan < (jvxInt32)runtime.in_channel_mapper.size())
		{
			jvxData val = runtime.in_channel_mapper[runtime.id_inchan].gain;
			for(i = 0; i < runtime.in_channel_mapper.size(); i++)
			{
				runtime.in_channel_mapper[i].gain = val;
			}
		}
		updateVariables_whileRunning();
		*/
	}

}

void
	configureAudio::toggle_outall()
{
	jvxSize i;
	if(runtime.out_all)
	{
		runtime.out_all = false;
	}
	else
	{
		runtime.out_all = true;
		/*
		if(runtime.id_outchan < (jvxInt32)runtime.out_channel_mapper.size())
		{
			jvxData val = runtime.out_channel_mapper[runtime.id_outchan].gain;
			for(i = 0; i < runtime.out_channel_mapper.size(); i++)
			{
				runtime.out_channel_mapper[i].gain = val;
			}
		}
		updateVariables_whileRunning();
		*/
	}
}

void
	configureAudio::new_output_gain()
{
	jvxSize i;
	int val = this->horizontalSlider_ogain->value();
	jvxData valDbl = val - 50.0;
	valDbl = pow(10.0, valDbl/20.0);

	if (val == 0)
	{
		valDbl = 0.0;
	}
	if(runtime.out_all)
	{
		for(i = 0; i < runtime.out_channel_mapper.size(); i++)
		{
			runtime.out_channel_mapper[i].gain = valDbl;
		}
	}
	else
	{
		if(runtime.id_outchan < runtime.out_channel_mapper.size())
		{
			runtime.out_channel_mapper[runtime.id_outchan].gain = valDbl;
		}
	}

	// In right state, set value in generic wrapper module
	updateVariables_whileRunning();

	this->myParent->updateWindow();
}

void
	configureAudio::toggle_direct()
{
	jvxCBool boolVal = c_false;
	jvxCallManagerProperties callGate;
	
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	if(runtime.id_inchan < runtime.in_channel_mapper.size())
	{
		if(runtime.in_channel_mapper[runtime.id_inchan].direct)
		{
			runtime.in_channel_mapper[runtime.id_inchan].direct = false;
		}
		else
		{
			runtime.in_channel_mapper[runtime.id_inchan].direct = true;
			boolVal = c_true;
		}
	}

	ident.reset("/JVX_GENW_DEV_IDIRECT");
	trans.reset(false, runtime.id_inchan);

	jvxErrorType res = currentPropsDev->set_property(callGate, jPRG(
		&boolVal,  1, JVX_DATAFORMAT_16BIT_LE), ident, trans);

	// In right state, set value in generic wrapper module
	updateVariables_whileRunning();

	this->myParent->updateWindow();
}

void
configureAudio::updateVariables_whileRunning()
{
	jvxSize i;
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valDbl;
	jvxInt16 valInt16;
	jvxCallManagerProperties callGate;
	
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	if (currentPropsDev)
	{
		for (i = 0; i < runtime.in_channel_mapper.size(); i++)
		{
			valDbl = runtime.in_channel_mapper[i].gain;
			ident.reset("/inputChannelGain");
			trans.reset(true, i);
			res = currentPropsDev->set_property(callGate, jPRG(
				&valDbl, 1, JVX_DATAFORMAT_DATA), 
				ident, trans);
		}

		for (i = 0; i < runtime.out_channel_mapper.size(); i++)
		{
			valDbl = runtime.out_channel_mapper[i].gain;
			ident.reset("/outputChannelGain");
			trans.reset(true, i);
			res = currentPropsDev->set_property(callGate, jPRG(
				&valDbl, 1, JVX_DATAFORMAT_DATA), 
				ident, trans );
		}
	}
	
	/*
	for (i = 0; i < runtime.in_channel_mapper.size(); i++)
	{
		valInt16 = runtime.in_channel_mapper[i].direct;
		res = currentPropsDev->set_property__descriptor(&valInt16, i, 1, JVX_DATAFORMAT_16BIT_LE, true, "/JVX_GENW_DEV_IDIRECT",
			JVX_PROPERTY_DECODER_NONE, NULL,false, callGate);
	}
	*/
}

void
configureAudio::toggle_autooff(bool tog)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList sel;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	if(tog)
	{
		sel.bitFieldSelected() = 0x2;
	}
	else
	{
		sel.bitFieldSelected() = 0x1;
	}

	ident.reset("/JVX_GENW_AUTO_OFF");
	trans.reset(true);

	res = currentPropsDev->set_property(callGate, jPRG( 
		&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), 
		ident, trans );
	assert(res == JVX_NO_ERROR);

	if(tog == false)
	{
		currentPropsDev->property_start_delayed_group(callGate, nullptr, nullptr);

		// Update
		this->edit_buffersize();
		this->edit_samplerate();

		ident.reset("/system/sel_dataformat");
		trans.reset(true);

		res = currentPropsDev->get_property(callGate, jPRG(&sel, 1, 
			JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		
		ident.reset("/system/sel_dataformat");
		trans.reset(true);
		
		res = currentPropsDev->set_property(callGate, 
			jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST),
			ident, trans );

		ident.reset("/system/sel_output_channels");
		trans.reset(true);
		
		res = currentPropsDev->get_property(callGate, jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		res = currentPropsDev->set_property(callGate,
			jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST),
			ident, trans);

		ident.reset("/system/sel_input_channels");
		trans.reset(true);
		res = currentPropsDev->get_property(callGate, jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);

		res = currentPropsDev->set_property(callGate,
			jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST),
			ident, trans );

		currentPropsDev->property_stop_delayed_group(callGate, nullptr);
	}
	this->myParent->updateWindow();
}

void
configureAudio::new_name_outwav_artist()
{
	localSettings.artist = lineEdit_outwav_artist->text().toLatin1().constData();
}

void
configureAudio::new_name_outwav_title()
{
	localSettings.title = lineEdit_outwav_title->text().toLatin1().constData();
}

void
configureAudio::button_outwav_add()
{
	jvxSelectionList theOldList;
	jvxBool isValid = false;
	std::vector<std::string> lstNewNames;
	std::vector<std::string> lstNewFNames;
	jvxAssignedFileTags tags;
	std::string txt;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	jvxSelectionList sel;
	jvxErrorType res1 = JVX_NO_ERROR, res2 = JVX_NO_ERROR;
	jvx_initTagName(tags);

	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString jstr;
	jvxApiString  fldstr;

	std::string fileName = "";
	txt = "";
	if (!localSettings.artist.empty())
	{
		tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_ARTIST;
		tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_ARTIST, JVX_NUMBER_AUDIO_FILE_TAGS)] = localSettings.artist;
	}
	else
	{
		tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_ARTIST;
		tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_ARTIST, JVX_NUMBER_AUDIO_FILE_TAGS)] = "unknown";
	}

	if (!localSettings.title.empty())
	{
		tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_TITLE;
		tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_TITLE, JVX_NUMBER_AUDIO_FILE_TAGS)] = localSettings.title;
	}
	else
	{
		tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_TITLE;
		tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_TITLE, JVX_NUMBER_AUDIO_FILE_TAGS)] = "unknown";
	}

	// Only add if tags are set
	jvx_tagStructToTagExpr(txt, tags, "");
	std::string command;
	command = "addOutputFile(" + txt + ");";
	jstr.assign_const(command.c_str(), command.size());

	ident.reset("/JVX_GENW_COMMAND");
	trans.reset(true);
	res = currentPropsTech->set_property(callGate,
		jPRG(&jstr, 1, JVX_DATAFORMAT_STRING),
		ident, trans);
	if (res == JVX_NO_ERROR)
	{
		ident.reset("/select_files/output_file");
		trans.reset(true);
		
		res = currentPropsTech->get_property(callGate, jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		assert(res == JVX_NO_ERROR);

		ident.reset("/select_files/output_file");
		trans.reset(true);
		jvx_bitZSet(sel.bitFieldSelected(), sel.strList.ll() - 1);
		res = currentPropsTech->set_property(callGate, jPRG( 
			&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), 
			ident, trans);
		assert(res == JVX_NO_ERROR);
	}
	else
	{
		ident.reset("/JVX_GENW_COMMAND");
		trans.reset(true);

		res = currentPropsTech->get_property(callGate, jPRG(&fldstr, 1, JVX_DATAFORMAT_STRING), ident, trans);
		this->myParent->postMessage_inThread(("Failed to open output file " + fileName + "<" + txt + ">, reason: " + fldstr.std_str()).c_str(), JVX_REPORT_PRIORITY_ERROR);
	}
	this->myParent->updateWindow();
}

void
configureAudio::button_outwav_apply()
{
	jvxBool isValid = false;
	jvxSelectionList theOldList;
	std::vector<std::string> newList;
	jvxAssignedFileTags tags;
	std::string txt;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	
	jvxErrorType res1 = JVX_NO_ERROR, res2 = JVX_NO_ERROR;
	jvx_initTagName(tags);

	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString jstr;
	jvxApiString  fldStr;

	jvxSelectionList sel;
	jvxApiStringList strLst;
	std::string oldtag;
	std::string newtag;

	ident.reset("/select_files/output_file");
	trans.reset(true);

	res = currentPropsTech->get_property(callGate, jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	assert(res == JVX_NO_ERROR);
	jvxSize num_output_files = sel.strList.ll();
	jvxSize id_outfile = jvx_bitfieldSelection2Id(sel.bitFieldSelected(), num_output_files);
	if (JVX_CHECK_SIZE_SELECTED(id_outfile))
	{
		ident.reset("/select_files/output_file");
		trans.reset(true, id_outfile);

		res = currentPropsTech->get_property(callGate, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
		assert(res == JVX_NO_ERROR);
		oldtag = fldStr.std_str();

		std::string fileName = "";
		txt = "";
		if (!localSettings.artist.empty())
		{
			tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_ARTIST;
			tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_ARTIST, JVX_NUMBER_AUDIO_FILE_TAGS)] = localSettings.artist;
		}
		else
		{
			tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_ARTIST;
			tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_ARTIST, JVX_NUMBER_AUDIO_FILE_TAGS)] = "unknown";
		}

		if (!localSettings.title.empty())
		{
			tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_TITLE;
			tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_TITLE, JVX_NUMBER_AUDIO_FILE_TAGS)] = localSettings.title;
		}
		else
		{
			tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_TITLE;
			tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_TITLE, JVX_NUMBER_AUDIO_FILE_TAGS)] = "unknown";
		}

		// Only add if tags are set
		jvx_tagStructToTagExpr(newtag, tags, "");

		std::string command;
		command = "renameOutputFile(" + oldtag + "," + newtag + ");";
		jstr.assign_const(command.c_str(), command.size());

		ident.reset("/JVX_GENW_COMMAND");
		trans.reset(true);

		res = currentPropsTech->set_property(callGate,
			jPRG(&jstr, 1, JVX_DATAFORMAT_STRING),
			ident, trans);
	}

	this->updateWindow();
}

void
configureAudio::pushed_inallon()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxApiString cmd;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	jvxSelectionList theSelList;

	theHost = myParent->involvedHost.hHost;

	ident.reset("/system/sel_input_channels");
	trans.reset(true);

	res = currentPropsDev->get_property(callGate, jPRG(&theSelList, 1,JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if (res == JVX_NO_ERROR)
	{
		theSelList.bitFieldSelected() = ((jvxBitField)1 << (jvxUInt32)theSelList.strList.ll()) - 1;

		ident.reset("/system/sel_input_channels");
		trans.reset(true);

		res = currentPropsDev->set_property(callGate,
			jPRG(&theSelList, 1, JVX_DATAFORMAT_SELECTION_LIST),
			ident, trans);

	}

	// Reset the output channel history
	cmd.assign("reset_channel_history_in();");
	ident.reset("/system/command_string");
	trans.reset(true);
	res = currentPropsDev->set_property(callGate, jPRG(
		&cmd, 1, JVX_DATAFORMAT_STRING), ident, trans);

	this->myParent->updateWindow();
}
	
void
configureAudio::pushed_inalloff()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxApiString cmd;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	

	jvxSelectionList theSelList;

	theHost = myParent->involvedHost.hHost;

	ident.reset("/system/sel_input_channels");
	trans.reset(true);

	res = currentPropsDev->get_property(callGate, jPRG(&theSelList, 1,JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if (res == JVX_NO_ERROR)
	{
		theSelList.bitFieldSelected() = 0;
		ident.reset("/system/sel_input_channels");
		trans.reset(true);
		res = currentPropsDev->set_property(callGate, jPRG(
			&theSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), 
			ident, trans );
	}

	// Reset the input channel history
	cmd.assign("reset_channel_history_in();");
	trans.reset(true);
	ident.reset("/system/command_string");
	res = currentPropsDev->set_property(callGate, jPRG(
		&cmd, 1, JVX_DATAFORMAT_STRING), 
		ident, trans );

	this->myParent->updateWindow();
}
	
void
configureAudio::pushed_outallon()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxApiString cmd;
	jvxCallManagerProperties callGate;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	jvxSelectionList theSelList;

	theHost = myParent->involvedHost.hHost;

	ident.reset("/system/sel_output_channels");
	trans.reset(true);

	res = currentPropsDev->get_property(callGate, jPRG(&theSelList, 1,JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if (res == JVX_NO_ERROR)
	{
		theSelList.bitFieldSelected() = ((jvxBitField)1 << (jvxUInt32)theSelList.strList.ll()) - 1;

		trans.reset(true);
		ident.reset("/system/sel_output_channels");
		res = currentPropsDev->set_property(callGate, jPRG( 
			&theSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), 
			ident, trans);

	}

	// Reset the output channel history
	cmd.assign("reset_channel_history_out();");

	ident.reset("/system/command_string");
	trans.reset(true);
	
	res = currentPropsDev->set_property(callGate, jPRG(
		&cmd, 1, JVX_DATAFORMAT_STRING), ident, trans );

	this->myParent->updateWindow();
}
	
void
configureAudio::pushed_outalloff()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	std::string str;
	jvxApiString cmd;
	jvxSelectionList theSelList;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	theHost = myParent->involvedHost.hHost;

	ident.reset("/system/sel_output_channels");
	trans.reset(true);

	res = currentPropsDev->get_property(callGate, jPRG(&theSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);

	ident.reset("/system/sel_output_channels");
	trans.reset(true);

	res = currentPropsDev->get_property(callGate, jPRG(&theSelList, 1,JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if (res == JVX_NO_ERROR)
	{
		theSelList.bitFieldSelected() = 0;
		ident.reset("/system/sel_output_channels");
		trans.reset(true);
		res = currentPropsDev->set_property(callGate, jPRG(
			&theSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), 
			ident, trans );

		// Reset the output channel history
		cmd.assign("reset_channel_history_out();");
		ident.reset("/system/command_string");
		trans.reset(true);
		res = currentPropsDev->set_property(callGate, jPRG(
			&cmd, 1, JVX_DATAFORMAT_STRING), 
			ident, trans);
	}

	this->myParent->updateWindow();
}


void
configureAudio::toggled_fillfromlast_out(bool tog)
{
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	jvxErrorType res = JVX_NO_ERROR;
	jvxCBool param = c_false;
	if (tog)
	{
		param = c_true;
	}
	ident.reset("/fill_channels_from_last_out");
	trans.reset(true);
	res = currentPropsDev->set_property(callGate, jPRG(
		&param, 1, JVX_DATAFORMAT_BOOL),
		ident, trans);
	this->myParent->updateWindow();
}

void
configureAudio::toggled_fillfromlast_in(bool tog)
{
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	jvxErrorType res = JVX_NO_ERROR;
	jvxCBool param = c_false;
	if (tog)
	{
		param = c_true;
	}
	ident.reset("/fill_channels_from_last_in");
	trans.reset(true);
	res = currentPropsDev->set_property(callGate, jPRG(
		&param, 1, JVX_DATAFORMAT_BOOL),
		ident, trans );
	this->myParent->updateWindow();

}


void
configureAudio::inform_component_select(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	IjvxQtComponentWidget* tmp = NULL;
	
	
	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_DEVICE:
		featureClass_dev = 0;
		myParent->involvedHost.hHost->feature_class_selected_component(tp, &featureClass_dev);
		
		
		checkMode();
		break;
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:
		
		// myParent->tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY] = tp;
		// myParent->tpAll[JVX_COMPONENT_AUDIO_DEVICE].slotid = myParent->tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY].slotid;
		
	
		checkMode();

		break;
	default:
		break;
	}

	myParent->reference_properties(this->tpAll[JVX_COMPONENT_AUDIO_DEVICE], &currentPropsDev);
	myParent->reference_properties(this->tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY], &currentPropsTech);
}
void
configureAudio::inform_component_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	IjvxQtComponentWidget* tmp = NULL;
	IjvxQtComponentWidget* spec_Class_dev = NULL;
	IjvxQtComponentWidget* spec_Class_tech = NULL;
	jvxCallManagerProperties callGate;
	featureClass_dev = 0;
	myParent->involvedHost.hHost->feature_class_selected_component(tp, &featureClass_dev);
	switch(tp.tp)
	{
	case JVX_COMPONENT_AUDIO_DEVICE:

		// myParent->tpAll[tp.tp] = tp;


		//assert(spec_Class_dev == NULL);
		IjvxQtComponentWidget_init(&spec_Class_dev, featureClass_dev, tp);
		if (spec_Class_dev)
		{
			spec_Class_dev->init_submodule(myParent->involvedHost.hHost);
			if (jvx_bitTest(modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
			{
				spec_Class_dev->start_show(theProps, frame_insert1, "/JVX_GENW");
			}
			else
			{
				spec_Class_dev->start_show(theProps, frame_insert1, "");
			}

			theProps->set_user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
				spec_Class_dev);
		}
		break;

	case JVX_COMPONENT_AUDIO_TECHNOLOGY:

		featureClass_tech = 0;
		myParent->involvedHost.hHost->feature_class_selected_component(tp, &featureClass_tech);

		IjvxQtComponentWidget_init(&spec_Class_tech, featureClass_tech, tp);
		if (spec_Class_tech)
		{
			spec_Class_tech->init_submodule(myParent->involvedHost.hHost);
			if (jvx_bitTest(modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
			{
				spec_Class_tech->start_show(theProps, frame_insert0, "/wrapped_tech");
			}
			else
			{
				spec_Class_tech->start_show(theProps, frame_insert0, "");
			}

			currentPropsTech->set_user_data(
				JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
				spec_Class_tech);
		}

		break;
	default:
		break;
	}
}

void 
configureAudio::inform_component_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* props)
{
	jvxHandle* tmp = NULL;
	jvxCallManagerProperties callGate;
	IjvxQtComponentWidget* spec_Class_dev = NULL;
	IjvxQtComponentWidget* spec_Class_tech = NULL;
	switch(tp.tp)
	{

	case JVX_COMPONENT_AUDIO_DEVICE:

		props->user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET, 
			reinterpret_cast<jvxHandle**>(&spec_Class_dev));
		
		if (spec_Class_dev)
		{
			spec_Class_dev->stop_show(frame_insert1);
			spec_Class_dev->terminate_submodule();
			IjvxQtComponentWidget_terminate(spec_Class_dev, featureClass_dev, tp);
			spec_Class_dev = NULL;

			currentPropsDev->set_user_data(
				JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
				spec_Class_dev);
		}

		break;
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:

		props->user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
			reinterpret_cast<jvxHandle**>(&spec_Class_tech));
		
		if (spec_Class_tech)
		{
			spec_Class_tech->stop_show(frame_insert0);
			spec_Class_tech->terminate_submodule();
			IjvxQtComponentWidget_terminate(spec_Class_tech, featureClass_tech, tp);
			spec_Class_tech = NULL;

			currentPropsTech->set_user_data(
				JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
				spec_Class_tech);
		}
		featureClass_tech = 0;
		break;
	default:
		break;
	}
}

void
configureAudio::inform_component_unselect(const jvxComponentIdentification& tp, IjvxAccessProperties* props)
{
	jvxHandle* tmp = NULL;
	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:

		// If there is no active device, we will reset this
		currentPropsTech = NULL;
		break;
	case JVX_COMPONENT_AUDIO_DEVICE:

		// If there is no active device, we will reset this
		currentPropsDev = NULL;				
		break;
	default:
		break;
	}
	myParent->reference_properties(this->tpAll[JVX_COMPONENT_AUDIO_DEVICE], &currentPropsDev);
	myParent->reference_properties(this->tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY], &currentPropsTech);
	
	// We must prevent that the currently removed reference is selected!
	if (props == currentPropsDev)
	{
		currentPropsDev = nullptr;
	}
	if (props == currentPropsTech)
	{
		currentPropsTech = nullptr;
	}
}

void
configureAudio::deactivate_slot(const jvxComponentIdentification& tpId, const jvxComponentIdentification& tpIdS)
{
	IjvxQtComponentWidget* spec_Class_dev = NULL;
	jvxCallManagerProperties callGate;
	IjvxQtComponentWidget* spec_Class_tech = NULL;
	switch (tpId.tp)
	{
	case JVX_COMPONENT_AUDIO_DEVICE:

		// Deactivate attached widget
		if (currentPropsDev)
		{
			currentPropsDev->user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
				reinterpret_cast<jvxHandle**>(&spec_Class_dev));
			if (spec_Class_dev)
			{
				spec_Class_dev->stop_show(frame_insert1);
			}
		}
		currentPropsDev = NULL;
		break;
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:

		if (currentPropsTech)
		{
			// Store slot id for current device
			currentPropsTech->set_user_data(JVX_USER_DATA_ID_SYSTEM, (jvxHandle*)intptr_t(tpIdS.slotsubid));
			currentPropsTech->user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET, reinterpret_cast<jvxHandle**>(&spec_Class_tech));
			if (spec_Class_tech)
			{
				spec_Class_tech->stop_show(frame_insert0);
			}
		}
		currentPropsTech = NULL;
		break;
	}
}

void
configureAudio::activate_slot(const jvxComponentIdentification& tpId, jvxComponentIdentification& tpIdS)
{
	IjvxQtComponentWidget* spec_Class_dev = NULL;
	IjvxQtComponentWidget* spec_Class_tech = NULL;
	jvxHandle* loadPtr = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize slotsubid = 0;
	jvxCallManagerProperties callGate;
	switch (tpId.tp)
	{
	case JVX_COMPONENT_AUDIO_DEVICE:

		currentPropsDev = NULL;
		myParent->reference_properties(tpId, &currentPropsDev);

		if (currentPropsDev)
		{
			checkMode();
			currentPropsDev->user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
				reinterpret_cast<jvxHandle**>(&spec_Class_dev));
			if (spec_Class_dev)
			{
				if (jvx_bitTest(modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
				{
					spec_Class_dev->start_show(currentPropsDev, frame_insert1, "/JVX_GENW");
				}
				else
				{
					spec_Class_dev->start_show(currentPropsDev, frame_insert1, "");
				}
			}
		}		

		break;
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:

		currentPropsTech = NULL;
		myParent->reference_properties(tpId, &currentPropsTech);
		if (currentPropsTech)
		{
			res = currentPropsTech->user_data(JVX_USER_DATA_ID_SYSTEM, &loadPtr);
			if (res == JVX_NO_ERROR)
			{
				slotsubid = (jvxSize)(intptr_t(loadPtr));
			}
		}
		tpIdS.slotid = tpId.slotid;
		tpIdS.slotsubid = slotsubid;

		checkMode();

		if (currentPropsTech)
		{
			currentPropsTech->user_data(JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET,
				reinterpret_cast<jvxHandle**>(&spec_Class_tech));
			if (spec_Class_tech)
			{
				if (jvx_bitTest(modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
				{
					spec_Class_tech->start_show(currentPropsTech, frame_insert0, "/JVX_GENW");
				}
				else
				{
					spec_Class_tech->start_show(currentPropsTech, frame_insert0, "");
				}
			}
		}

		break;
	}
}

void
configureAudio::new_dev_slot(int id)
{
	jvxComponentIdentification tpS = JVX_COMPONENT_UNKNOWN;
	deactivate_slot(myParent->tpAll[JVX_COMPONENT_AUDIO_DEVICE], tpS);
	refreshPropertyIds();

	// Do the actual switch
	myParent->tpAll[JVX_COMPONENT_AUDIO_DEVICE].slotsubid = id;
	
	activate_slot(myParent->tpAll[JVX_COMPONENT_AUDIO_DEVICE], tpS);
	refreshPropertyIds();
	myParent->updateWindow();
}

void
configureAudio::new_tech_slot(int id)
{
	jvxComponentIdentification tpS = JVX_COMPONENT_UNKNOWN;

	deactivate_slot(tpAll[JVX_COMPONENT_AUDIO_DEVICE], tpS);
	deactivate_slot(tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY], tpAll[JVX_COMPONENT_AUDIO_DEVICE]);
	refreshPropertyIds();

	//checkMode(tpIdT);
	tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY].slotid = id;

	acceptSlotConfiguration();

	activate_slot(tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY], tpAll[JVX_COMPONENT_AUDIO_DEVICE]);
	activate_slot(tpAll[JVX_COMPONENT_AUDIO_DEVICE], tpS);
	
	refreshPropertyIds();
	myParent->updateWindow();
}

void
configureAudio::reactivateComponent(const jvxComponentIdentification& cpTp)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idxsel = JVX_SIZE_UNSELECTED;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;

	if (cpTp.tp == JVX_COMPONENT_AUDIO_TECHNOLOGY)
	{
		if (jvx_bitTest(modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
		{
			ident.reset("/JVX_GENW_TECHNOLOGIES");
			trans.reset(true);

			res = currentPropsTech->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			idxsel = jvx_bitfieldSelection2Id(selLst.bitFieldSelected(), selLst.strList.ll());
		}
		else
		{
			res = myParent->involvedHost.hHost->selection_component(cpTp, &idxsel);
		}
		assert(res == JVX_NO_ERROR);

		if (JVX_CHECK_SIZE_SELECTED(idxsel))
		{
			select_technology(0);
			select_technology(idxsel+1);
		}
	}
}

void 
configureAudio::checkMode()
{
	assert(myParent->tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY].slotid == myParent->tpAll[JVX_COMPONENT_AUDIO_DEVICE].slotid);
	jvx_bitFClear(modeShow);
	if (myParent->tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY].slotid == 0)
	{
		jvx_bitSet(modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT);
		
		if (myParent->tpAll[JVX_COMPONENT_AUDIO_DEVICE].slotsubid == 0)
		{
			jvx_bitSet(modeShow, JVX_UI_SHOW_MODE_GENW_SHIFT);
		}
	}
}

void
configureAudio::select_rate(int idx)
{
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	myParent->involvedHost.hHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
	if (stat == JVX_STATE_ACTIVE)
	{
		currentPropsDev->property_start_delayed_group(callGate, nullptr, nullptr);
		jvx_bitFClear(selLst.bitFieldSelected());
		jvx_bitSet(selLst.bitFieldSelected(), idx);
		ident.reset("/system/sel_rate");
		trans.reset(true);
		res = currentPropsDev->set_property(callGate,
			jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST),
			ident, trans);
		assert(res == JVX_NO_ERROR);
		currentPropsDev->property_stop_delayed_group(callGate, nullptr );
		this->myParent->updateWindow();
	}
}

void
configureAudio::select_bsize(int idx)
{
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	myParent->involvedHost.hHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
	if (stat == JVX_STATE_ACTIVE)
	{
		currentPropsDev->property_start_delayed_group(callGate, nullptr, nullptr );
		jvx_bitFClear(selLst.bitFieldSelected());
		jvx_bitSet(selLst.bitFieldSelected(), idx);
		ident.reset("/system/sel_framesize");
		trans.reset(true);
		res = currentPropsDev->set_property(callGate,
			jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST),
			ident, trans);
		assert(res == JVX_NO_ERROR);
		currentPropsDev->property_stop_delayed_group(callGate, nullptr );
		this->myParent->updateWindow();
	}
}

void
configureAudio::start_timer()
{
	if (preAllowedTimer == false)
	{
		this->timerViewUpdate = new QTimer;
		connect(this->timerViewUpdate, SIGNAL(timeout()), this, SLOT(timerExpired()));
		this->timerViewUpdate->start(config.period_ms);

		if (myParent->subWidgets.main.theWidget)
		{
			myParent->subWidgets.main.theWidget->inform_viewer_started();
		}
	}
}

void
configureAudio::stop_timer()
{
	if (preAllowedTimer == false)
	{
		if (myParent->subWidgets.main.theWidget)
		{
			myParent->subWidgets.main.theWidget->inform_viewer_about_to_stop();
		}

		this->timerViewUpdate->stop();
		disconnect(this->timerViewUpdate, SIGNAL(timeout()));
		delete(this->timerViewUpdate);
		this->timerViewUpdate = nullptr;

		if (myParent->subWidgets.main.theWidget)
		{
			myParent->subWidgets.main.theWidget->inform_viewer_stopped();
		}
	}
}

void 
configureAudio::pre_allow_timer()
{
	start_timer();
	preAllowedTimer = true;
}

void 
configureAudio::post_allow_timer()
{
	preAllowedTimer = false;
	stop_timer();
}


void 
configureAudio::trigger_select_device(jvxComponentIdentification id, const std::string& ident)
{
	jvxSize num = 0;
	jvxApiString nmDev;

	tpAll[id.tp].slotid = id.slotid;
	myParent->involvedHost.hHost->number_components_system(tpAll[id.tp], &num);
	for (int i = 0; i < num; i++)
	{
		myParent->involvedHost.hHost->name_component_system(tpAll[id.tp], i, &nmDev);
		if (jvx_compareStringsWildcard(ident, nmDev.std_str()))
		{
			jvxErrorType res = myParent->involvedHost.hHost->select_component(tpAll[id.tp], i);
			if (res == JVX_NO_ERROR)
			{
				select_device(i);
			}
			break;
		}
	}
}
