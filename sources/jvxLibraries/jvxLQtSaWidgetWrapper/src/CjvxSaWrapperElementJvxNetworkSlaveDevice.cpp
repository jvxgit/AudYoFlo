#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementJvxNetworkSlaveDevice.h"
#include "jvx-qt-helpers.h"
#include <QVariant>

/*
	"/integrateiplink/JVX_SOCKET_COMMAND"
	"/integrateiplink/JVX_SOCKET_AUTO_CONNECT"
	"/integrateiplink/JVX_SOCKET_AUTO_START"
	"/integrateiplink/JVX_SOCKET_HOSTNAME"
	"/integrateiplink/JVX_SOCKET_PCAP_CONNECT_REPORT_DELAY"
	"/integrateiplink/JVX_SOCKET_HOSTPORT"
	"/integrateiplink/JVX_SOCKET_CONNECTION_TYPE"
	"/integrateiplink/JVX_SOCKET_CONN_STATE"
	"/integrateiplink/JVX_NET_AUDEV_LOSTFRAMES"
	"/integrateiplink/JVX_NET_AUDEV_LOSTFRAMES_REMOTE"
	"/integrateiplink/JVX_NET_AUDEV_UNSENTFRAMES"
	"/integrateiplink/JVX_NET_AUDEV_HW_MODES"
	"/integrateiplink/JVX_NET_AUDEV_LOAD_CONVERT"
	"/integrateiplink/JVX_NET_AUDEV_LOAD_PROCESS"*/

CjvxSaWrapperElementJvxNetworkSlaveDevice::CjvxSaWrapperElementJvxNetworkSlaveDevice(jvxQtSaNetworkSlaveDevice* uiRefLoc,
	CjvxQtSaWidgetWrapper* backRefLoc, IjvxAccessProperties* propRefIn,
	std::string propertyGetSetTagLoc, std::vector<std::string> paramLstLoc, const std::string& tag_cp, const std::string& tag_key,
	const std::string& uiObjectNameLoc, const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef) :
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn,
		propertyGetSetTagLoc, paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId, hostRef)
{
	memset(&theReferences, 0, sizeof(theReferences));

	uiRefTp = uiRefLoc;
	initializeUiElement();
}

void 
CjvxSaWrapperElementJvxNetworkSlaveDevice::initializeUiElement(jvxPropertyCallContext ccontext)
{
	std::vector<std::string> paramlst_add;
	jvxApiString  fldStr;
	initializeProperty_core(false, ccontext);
	jvxSize i;

	if (widgetStatus == JVX_STATE_SELECTED)
	{
		// Evaluate the property descriptors

		// Prepend custom prefix
		uiRefTp->getPropsPrefix(&fldStr);
		std::string lPrefix = fldStr.std_str();
		if (!lPrefix.empty())
		{
			propertyGetSetTag = jvx_makePathExpr(lPrefix, propertyGetSetTag, true, true);
		}

		uiRefTp->getAdditonalConfigTokens(&fldStr);
		std::string addParamsString = fldStr.std_str();
		if (!addParamsString.empty())
		{
			jvx_parsePropertyStringToKeylist(addParamsString, paramlst_add);
			paramLst.insert(paramLst.end(), paramlst_add.begin(), paramlst_add.end());
		}

		jvxErrorType res = this->initParameters_getMin(&backwardRef->keymaps.ao, paramLst);

		uiRefTp->getWidgetReferences(&theReferences);

		if (theReferences.lineEdit_connectionPort)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <port>" << std::endl;
			}
			connect(theReferences.lineEdit_connectionPort, SIGNAL(editingFinished()), this, SLOT(onEditPort()));
		}
		if (theReferences.pushButton_listen)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <listen>" << std::endl;
			}

			connect(theReferences.pushButton_listen, SIGNAL(clicked()), this, SLOT(onButtonListen()));
		}

		if (theReferences.lineEdit_buffersize)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <buffersize>" << std::endl;
			}
			connect(theReferences.lineEdit_buffersize, SIGNAL(editingFinished()), this, SLOT(onNewBuffersize()));
		}

		if (theReferences.lineEdit_samplerate)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <samplerate>" << std::endl;
			}
			connect(theReferences.lineEdit_samplerate, SIGNAL(editingFinished()), this, SLOT(onNewSamplerate()));
		}

		if (theReferences.lineEdit_numberInChannels)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <input channels>" << std::endl;
			}
			connect(theReferences.lineEdit_numberInChannels, SIGNAL(editingFinished()), this, SLOT(onNewNumInChannels()));
		}

		if (theReferences.lineEdit_numberOutChannels)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <output channels>" << std::endl;
			}
			connect(theReferences.lineEdit_numberOutChannels, SIGNAL(editingFinished()), this, SLOT(onNewNumOutChannels()));
		}

		if (theReferences.comboBox_formats)
		{
			theReferences.comboBox_formats->clear();
			for (i = 0; i < JVX_DATAFORMAT_LIMIT; i++)
			{
				theReferences.comboBox_formats->addItem(jvxDataFormat_txt(i));
			}
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <format>" << std::endl;
			}
			connect(theReferences.comboBox_formats, SIGNAL(activated(int)), this, SLOT(onNewFormat(int)));
		}

		if (theReferences.checkBox_autoaudio)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <format>" << std::endl;
			}
			connect(theReferences.checkBox_autoaudio, SIGNAL(clicked()), this, SLOT(onClickAudioAuto()));
		}

#ifdef JVX_CONNECT_ELEMENTS
		
		if(cb_auto)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <auto>" << std::endl;
			}

			connect(cb_auto, SIGNAL(clicked(bool)), this, SLOT(autoConnectClicked(bool)));
		}
		if(cb_start)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <start>" << std::endl;
			}

			connect(cb_start, SIGNAL(clicked(bool)), this, SLOT(autoStartClicked(bool)));
		}
		if (cb_mode)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <mode>" << std::endl;
			}

			connect(cb_mode, SIGNAL(activated(int)), this, SLOT(toggledMode(int)));
		}
		if (cb_log)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <log>" << std::endl;
			}

			connect(cb_log, SIGNAL(clicked(bool)), this, SLOT(toggledLog(bool)));

		}

		if (le_tech)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <tech>" << std::endl;
			}
		}
#endif

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			assert(0);
		}
		updateWindowUiElement();
	
		uiRefTp->setProperty("mySaWidgetRefNetwork", QVariant::fromValue<CjvxSaWrapperElementJvxNetworkSlaveDevice*>(this));

		//connect(uiRefTp, SIGNAL(clicked(bool)), this, SLOT(checkbox_toggled(bool)));
		updateWindowUiElement();

		assert(backwardRef);
		backwardRef->reportPropertyInitComplete(selector_lst.c_str(), assoc_id);
	}
	//verboseLoc = this->myBaseProps.verbose_out;
}

CjvxSaWrapperElementJvxNetworkSlaveDevice::~CjvxSaWrapperElementJvxNetworkSlaveDevice()
{
	if (widgetStatus == JVX_STATE_SELECTED)
	{
#ifdef JVX_CONNECT_ELEMENTS
		if(le_host)
		{
			disconnect(le_host, SIGNAL(editingFinished()));
		}
		if(le_port)
		{
			disconnect(le_port , SIGNAL(editingFinished()));
		}
		if(pb_connect)
		{
			disconnect(pb_connect, SIGNAL(clicked()));
		}
		if(cb_auto)
		{
			disconnect(cb_auto, SIGNAL(clicked(bool)));
		}
		if(cb_start)
		{
			disconnect(cb_start, SIGNAL(clicked(bool)));
		}
#endif
	}
}

void
CjvxSaWrapperElementJvxNetworkSlaveDevice::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	std::string getSetTag = propertyGetSetTag;
	jvxBool condSet = false;
	jvxPropertyDescriptor descr;
	/*
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	if (globalId)
	{
		res = propRef->get_property_descriptor__descriptor(descr, getSetTag.c_str(), globalId);
	}
	else
	{
		res = propRef->get_property_descriptor__descriptor(descr, getSetTag.c_str(), &procId);
	}
	if (res == JVX_NO_ERROR)
	{
		if (descr.accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
		{
			QFont ft = uiRefTp->font();
			ft.setItalic(true);
			uiRefTp->setFont(ft);
		}
		else
		{
			QFont ft = uiRefTp->font();
			ft.setItalic(false);
			uiRefTp->setFont(ft);
		}
	}
	else if (res == JVX_ERROR_POSTPONE)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*update*");
	}
	else
	{
		QFont ft = uiRefTp->font();
		ft.setItalic(true);
		uiRefTp->setFont(ft);
		std::cout << "Failed to get status update for object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
	}*/
}

jvxErrorType 
CjvxSaWrapperElementJvxNetworkSlaveDevice::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* globalId, jvxBool call_periodic_update)
{
	jvxSize i;
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	jvxApiString  fldStr;
	jvxInt32 valI32;
	jvxBool anyError = false;
	jvxCBool valB = false;
	jvxInt16 valI16 = c_false;
	jvxSize valS = 0;
	std::string hostName = "-unknown-";
	jvxCallManagerProperties callGate;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/

	setBackgroundLabelColor(Qt::gray, theReferences.stats[0]);
	setBackgroundLabelColor(Qt::gray, theReferences.stats[1]);
	setBackgroundLabelColor(Qt::gray, theReferences.stats[2]);
	setBackgroundLabelColor(Qt::gray, theReferences.stats[3]);
	setBackgroundLabelColor(Qt::gray, theReferences.stats[4]);
	setBackgroundLabelColor(Qt::gray, theReferences.stats[5]);

	theReferences.pushButton_listen->setText("--");

	// ====================================================================================

	propName = jvx_makePathExpr(getSetTag, "audioconfig/buffersize");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&valS, 1, JVX_DATAFORMAT_SIZE), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		theReferences.lineEdit_buffersize->setText(jvx_size2String(valS).c_str());
		theReferences.lineEdit_buffersize->setEnabled(true);
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		theReferences.lineEdit_buffersize->setText("--");
		theReferences.lineEdit_buffersize->setEnabled(false);
	}

	// ====================================================================================

	propName = jvx_makePathExpr(getSetTag, "audioconfig/samplerate");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&valS, 1, JVX_DATAFORMAT_SIZE), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		theReferences.lineEdit_samplerate->setText(jvx_size2String(valS).c_str());
		theReferences.lineEdit_samplerate->setEnabled(true);
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		theReferences.lineEdit_samplerate->setText("--");
		theReferences.lineEdit_samplerate->setEnabled(false);
	}
	
	// ====================================================================================

	propName = jvx_makePathExpr(getSetTag, "audioconfig/numberinputchannels");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&valS, 1, JVX_DATAFORMAT_SIZE), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		theReferences.lineEdit_numberInChannels->setText(jvx_size2String(valS).c_str());
		theReferences.lineEdit_numberInChannels->setEnabled(true);
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		theReferences.lineEdit_numberInChannels->setText("--");
		theReferences.lineEdit_numberInChannels->setEnabled(false);
	}

	// ====================================================================================

	propName = jvx_makePathExpr(getSetTag, "audioconfig/numberoutputchannels");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&valS, 1, JVX_DATAFORMAT_SIZE),ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		theReferences.lineEdit_numberOutChannels->setText(jvx_size2String(valS).c_str());
		theReferences.lineEdit_numberOutChannels->setEnabled(true);
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		theReferences.lineEdit_numberOutChannels->setText("--");
		theReferences.lineEdit_numberOutChannels->setEnabled(false);
	}

	propName = jvx_makePathExpr(getSetTag, "audioconfig/audioformat");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		if (valI16 >= JVX_DATAFORMAT_LIMIT)
		{
			valI16 = JVX_DATAFORMAT_DATA;
		}
		theReferences.comboBox_formats->setCurrentIndex(valI16);
		theReferences.comboBox_formats->setEnabled(true);
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		theReferences.comboBox_formats->setCurrentIndex(0);
		theReferences.comboBox_formats->setEnabled(false);
	}

	propName = jvx_makePathExpr(getSetTag, "audioconfig/automode");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		theReferences.checkBox_autoaudio->setChecked((valI16 == c_true));
		if (valI16 == c_true)
		{
			theReferences.comboBox_formats->setEnabled(false);
			theReferences.lineEdit_numberInChannels->setEnabled(false);
			theReferences.lineEdit_numberOutChannels->setEnabled(false);
			theReferences.lineEdit_buffersize->setEnabled(false);
			theReferences.lineEdit_samplerate->setEnabled(false);
		}
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		theReferences.comboBox_formats->setEnabled(false);
		theReferences.lineEdit_numberInChannels->setEnabled(false);
		theReferences.lineEdit_numberOutChannels->setEnabled(false);
		theReferences.lineEdit_buffersize->setEnabled(false);
		theReferences.lineEdit_samplerate->setEnabled(false);
	}

	// ====================================================================================
	// ====================================================================================
	// ====================================================================================

	// Print port at which to listen
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_HOSTPORT");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		theReferences.lineEdit_connectionPort->setText(jvx_int322String(valI32).c_str());
		theReferences.lineEdit_connectionPort->setEnabled(true);
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		theReferences.lineEdit_connectionPort->setText("--");
		theReferences.lineEdit_connectionPort->setEnabled(false);
	}

	// Print name of connected entity
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_HOSTNAME");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		hostName = fldStr.c_str();
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		hostName = "--";
	}
	
	// Print connection type (only for view)
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_CONNECTION_TYPE");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		theReferences.lineEdit_connectionType->setText(fldStr.c_str());
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		theReferences.lineEdit_connectionType->setText("--");
	}

	// Print connection type (only for view)
	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_HW_MODES");
	ident.reset(propName.c_str());
	trans.reset(false, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		theReferences.comboBox_mode->clear();
		for (i = 0; i < selLst.strList.ll(); i++)
		{
			std::string entry = selLst.strList.std_str_at(i);
			if (jvx_bitTest(selLst.bitFieldSelected(), i))
			{
				entry += "*";
			}
			theReferences.comboBox_mode->addItem(entry.c_str());
		}
		theReferences.comboBox_mode->setEnabled(true);
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		theReferences.comboBox_mode->clear();
		theReferences.comboBox_mode->setEnabled(false);
	}


	// =============================================================================00
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_CONN_STATE");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		/*
		 * States:
		 * 0: not listening, no incoming allowed
		 * 1: Listening, no packet received yet
		 * 2: First packet received, no other sources allowed
		 * 3: Status is active
		 * 4: Connection complete, staring is possible
		 * 5: In processing
		 * 6: shutodwn in progress
		 */

		if (jvx_bitTest(selLst.bitFieldSelected(), 0))
		{
			setBackgroundLabelColor(Qt::white, theReferences.stats[0]);
			theReferences.pushButton_listen->setText("start");
			theReferences.pushButton_listen->setEnabled(true);
			theReferences.lineEdit_connectionSource->setText("--");
			theReferences.lineEdit_connectionPort->setEnabled(true);
		}
		if (jvx_bitTest(selLst.bitFieldSelected(), 1))
		{
			theReferences.pushButton_listen->setText("listening");
			theReferences.pushButton_listen->setEnabled(true);
			theReferences.lineEdit_connectionSource->setText("--");
			theReferences.lineEdit_connectionPort->setEnabled(false);
			setBackgroundLabelColor(Qt::white, theReferences.stats[1]);
			theReferences.lineEdit_samplerate->setEnabled(false);
			theReferences.lineEdit_buffersize->setEnabled(false);
			theReferences.lineEdit_numberInChannels->setEnabled(false);
			theReferences.lineEdit_numberOutChannels->setEnabled(false);
			theReferences.comboBox_formats->setEnabled(false);
			theReferences.comboBox_mode->setEnabled(false);
		}
		if (jvx_bitTest(selLst.bitFieldSelected(), 2))
		{
			theReferences.pushButton_listen->setText("connected");
			theReferences.pushButton_listen->setEnabled(true);
			theReferences.lineEdit_connectionSource->setText(hostName.c_str());
			theReferences.lineEdit_connectionPort->setEnabled(false);
			setBackgroundLabelColor(Qt::white, theReferences.stats[2]);
			theReferences.lineEdit_samplerate->setEnabled(false);
			theReferences.lineEdit_buffersize->setEnabled(false);
			theReferences.lineEdit_numberInChannels->setEnabled(false);
			theReferences.lineEdit_numberOutChannels->setEnabled(false);
			theReferences.comboBox_formats->setEnabled(false);
			theReferences.comboBox_mode->setEnabled(false);
		}
		if (jvx_bitTest(selLst.bitFieldSelected(), 3))
		{
			theReferences.pushButton_listen->setText("active");
			theReferences.pushButton_listen->setEnabled(true);
			theReferences.lineEdit_connectionSource->setText(hostName.c_str());
			theReferences.lineEdit_connectionPort->setEnabled(false);
			setBackgroundLabelColor(Qt::white, theReferences.stats[3]);
			theReferences.lineEdit_samplerate->setEnabled(false);
			theReferences.lineEdit_buffersize->setEnabled(false);
			theReferences.lineEdit_numberInChannels->setEnabled(false);
			theReferences.lineEdit_numberOutChannels->setEnabled(false);
			theReferences.comboBox_formats->setEnabled(false);
			theReferences.comboBox_mode->setEnabled(false);
		}
		if (jvx_bitTest(selLst.bitFieldSelected(), 4))
		{
			theReferences.pushButton_listen->setText("ready");
			theReferences.pushButton_listen->setEnabled(true);
			theReferences.lineEdit_connectionSource->setText(hostName.c_str());
			theReferences.lineEdit_connectionPort->setEnabled(false);
			setBackgroundLabelColor(Qt::white, theReferences.stats[4]);
			theReferences.lineEdit_samplerate->setEnabled(false);
			theReferences.lineEdit_buffersize->setEnabled(false);
			theReferences.lineEdit_numberInChannels->setEnabled(false);
			theReferences.lineEdit_numberOutChannels->setEnabled(false);
			theReferences.comboBox_formats->setEnabled(false);
			theReferences.comboBox_mode->setEnabled(false);
		}
		if (jvx_bitTest(selLst.bitFieldSelected(), 5))
		{
			theReferences.pushButton_listen->setText("processing");
			theReferences.pushButton_listen->setEnabled(false);
			theReferences.lineEdit_connectionSource->setText(hostName.c_str());
			theReferences.lineEdit_connectionPort->setEnabled(false);
			setBackgroundLabelColor(Qt::white, theReferences.stats[5]);
			theReferences.lineEdit_samplerate->setEnabled(false);
			theReferences.lineEdit_buffersize->setEnabled(false);
			theReferences.lineEdit_numberInChannels->setEnabled(false);
			theReferences.lineEdit_numberOutChannels->setEnabled(false);
			theReferences.comboBox_formats->setEnabled(false);
			theReferences.comboBox_mode->setEnabled(false);
		}
		if (jvx_bitTest(selLst.bitFieldSelected(), 6))
		{
			theReferences.pushButton_listen->setText("shutdown");
			theReferences.pushButton_listen->setEnabled(false);
			theReferences.lineEdit_connectionSource->setText(hostName.c_str());
			theReferences.lineEdit_connectionPort->setEnabled(false);
			setBackgroundLabelColor(Qt::white, theReferences.stats[6]);
			theReferences.lineEdit_samplerate->setEnabled(false);
			theReferences.lineEdit_buffersize->setEnabled(false);
			theReferences.lineEdit_numberInChannels->setEnabled(false);
			theReferences.lineEdit_numberOutChannels->setEnabled(false);
			theReferences.comboBox_formats->setEnabled(false);
			theReferences.comboBox_mode->setEnabled(false);
		}
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
	}
	return JVX_NO_ERROR;
}
void 
CjvxSaWrapperElementJvxNetworkSlaveDevice::trigger_updateWindow_periodic(jvxPropertyCallContext ccontext, jvxSize* passedId)
{
#ifdef JVX_CONNECT_ELEMENTS
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 valI32;
	jvxData valD = 0;
	std::string txtShow;
	jvxCallManagerProperties callGate;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	std::string propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_LOSTFRAMES");
	res = propRef->get_property(callGate, jPRG( &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, true, propName.c_str(),
		JVX_PROPERTY_DECODER_NONE, NULL, JVX_PROPERTY_FLAG_FULL_ALLOW, callGate.access_protocol);
	if(JVX_CHECK_PROPERTY_ACCESS_OK(res, accProt))
	{
		txtShow += jvx_int2String(valI32);
	}
	else
	{
		txtShow += "-";
	}

	txtShow += "/";

	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_UNSENTFRAMES");
	res = propRef->get_property(callGate, jPRG( &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, true, propName.c_str(),
		JVX_PROPERTY_DECODER_NONE, NULL, JVX_PROPERTY_FLAG_FULL_ALLOW, callGate.access_protocol);
	if(JVX_CHECK_PROPERTY_ACCESS_OK(res, accProt))
	{
		txtShow += jvx_int2String(valI32);
	}
	else
	{
		txtShow += "-";
	}

	txtShow += "/";

	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_LOSTFRAMES_REMOTE");
	res = propRef->get_property(callGate, jPRG( &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, true, propName.c_str(),
		JVX_PROPERTY_DECODER_NONE, NULL, JVX_PROPERTY_FLAG_FULL_ALLOW, callGate.access_protocol);
	if(res == JVX_NO_ERROR)
	{
		txtShow += jvx_int2String(valI32);
	}
	else
	{
		txtShow += "-";
	}
	le_perform->setText(txtShow.c_str());
	le_perform->setEnabled(true);

	txtShow = "";
	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_LOAD_CONVERT");
	res = propRef->get_property(callGate, jPRG( &valD, 0, 1, JVX_DATAFORMAT_DATA, true, propName.c_str(),
		JVX_PROPERTY_DECODER_NONE, NULL, JVX_PROPERTY_FLAG_FULL_ALLOW, callGate.access_protocol);
	if (res == JVX_NO_ERROR)
	{
		txtShow += jvx_data2String(valD,1) + "%";
	}
	else
	{
		txtShow += "-";
	}
	txtShow += "/";
	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_LOAD_PROCESS");
	res = propRef->get_property(callGate, jPRG( &valD, 0, 1, JVX_DATAFORMAT_DATA, true, propName.c_str(),
		JVX_PROPERTY_DECODER_NONE, NULL, JVX_PROPERTY_FLAG_FULL_ALLOW, callGate.access_protocol);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, accProt))
	{
		txtShow += jvx_data2String(valD, 1) + "%";
	}
	else
	{
		txtShow += "-";
	}

	le_prof->setText(txtShow.c_str());
	le_prof->setEnabled(true);
#endif
}

#ifdef JVX_CONNECT_ELEMENTS
void
CjvxSaWrapperElementJvxNetworkSlaveDevice::toggledMode(int sel)
{
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	jvxErrorType res = JVX_NO_ERROR;
	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_HW_MODES");
	res = propRef->get_property(callGate, jPRG( &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, true, propName.c_str(), 
		JVX_PROPERTY_DECODER_NONE, NULL, JVX_PROPERTY_FLAG_FULL_ALLOW, callGate.access_protocol);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, accProt))
	{
		jvx_bitToggle(selLst.bitFieldSelected(), sel);
	}
	res = propRef->set_property__descriptor( &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, true, propName.c_str(),
		JVX_PROPERTY_DECODER_NONE, NULL, false,JVX_PROPERTY_FLAG_FULL_ALLOW, callGate.access_protocol);
	assert(res == JVX_NO_ERROR);
	
	updateWindowUiElement();
}

void
CjvxSaWrapperElementJvxNetworkSlaveDevice::toggledLog(bool tog)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt16 valI16 = c_false;
	jvxCallManagerProperties callGate;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_LOG_TO_FILE");
	if (tog)
	{
		valI16 = c_true;
	}
	res = propRef->set_property__descriptor( &valI16, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, propName.c_str(),
		JVX_PROPERTY_DECODER_NONE, NULL, false, JVX_PROPERTY_FLAG_FULL_ALLOW, callGate.access_protocol);
	updateWindowUiElement();

}
#endif

void
CjvxSaWrapperElementJvxNetworkSlaveDevice::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	assert(0);
}

jvxErrorType
CjvxSaWrapperElementJvxNetworkSlaveDevice::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	return JVX_ERROR_UNSUPPORTED;
}