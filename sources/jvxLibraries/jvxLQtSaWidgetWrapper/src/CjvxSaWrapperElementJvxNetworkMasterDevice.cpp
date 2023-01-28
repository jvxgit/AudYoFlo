#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementJvxNetworkMasterDevice.h"
#include "jvx-qt-helpers.h"
#include <QVariant>

CjvxSaWrapperElementJvxNetworkMasterDevice::CjvxSaWrapperElementJvxNetworkMasterDevice(jvxQtSaNetworkMasterDevice* uiRefLoc,
	CjvxQtSaWidgetWrapper* backRefLoc, IjvxAccessProperties* propRefIn,
	std::string propertyGetSetTagLoc, std::vector<std::string> paramLstLoc, const std::string& tag_cp, const std::string& tag_key,
	const std::string& uiObjectNameLoc, const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef) :
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn,
		propertyGetSetTagLoc, paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId,
		hostRef)
{
	uiRefTp = uiRefLoc;
	initializeUiElement();
}

void 
CjvxSaWrapperElementJvxNetworkMasterDevice::initializeUiElement(jvxPropertyCallContext ccontext)
{
	std::vector<std::string> paramlst_add;
	jvxApiString  fldStr;
	initializeProperty_core(false, ccontext);

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

		le_host = NULL;
		le_port = NULL;
		pb_connect = NULL;
		le_perform = NULL;
		stats[0] = NULL;
		cb_auto = NULL;
		cb_start = NULL;
		cb_mode = NULL;
		cb_log = NULL;
		le_prof = NULL;
		le_tech = NULL;

		uiRefTp->getWidgetReferences(&le_host, &le_port, &pb_connect, &le_perform, stats, 
			&cb_auto, &cb_start, &cb_mode, &le_prof, &cb_log, &le_tech);

		if(le_host)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <host>" << std::endl;
			}
			connect(le_host, SIGNAL(editingFinished()), this, SLOT(newTextDestHost()));
		}
		if(le_port)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <port>" << std::endl;
			}
			connect(le_port , SIGNAL(editingFinished()), this, SLOT(newTextDestPort()));
		}
		if(pb_connect)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <connect>" << std::endl;
			}

			connect(pb_connect, SIGNAL(clicked()), this, SLOT(pushedButtonGo()));
		}
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
		updateWindowUiElement();
	
		uiRefTp->setProperty("mySaWidgetRefNetwork", QVariant::fromValue<CjvxSaWrapperElementJvxNetworkMasterDevice*>(this));

		//connect(uiRefTp, SIGNAL(clicked(bool)), this, SLOT(checkbox_toggled(bool)));

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			assert(0);
		}
		updateWindowUiElement();

		assert(backwardRef);
		backwardRef->reportPropertyInitComplete(selector_lst.c_str(), assoc_id);
	}
	//verboseLoc = this->myBaseProps.verbose_out;
}

CjvxSaWrapperElementJvxNetworkMasterDevice::~CjvxSaWrapperElementJvxNetworkMasterDevice()
{
	if (widgetStatus == JVX_STATE_SELECTED)
	{
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
	}
}

void
CjvxSaWrapperElementJvxNetworkMasterDevice::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
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
CjvxSaWrapperElementJvxNetworkMasterDevice::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* globalId, jvxBool call_periodic_update)
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
	setBackgroundLabelColor(Qt::gray, stats[0]);
	setBackgroundLabelColor(Qt::gray, stats[1]);
	setBackgroundLabelColor(Qt::gray, stats[2]);
	setBackgroundLabelColor(Qt::gray, stats[3]);
	setBackgroundLabelColor(Qt::gray, stats[4]);
	setBackgroundLabelColor(Qt::gray, stats[5]);

	pb_connect->setText("--");

	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_HOSTNAME");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &fldStr, 1, JVX_DATAFORMAT_STRING),ident, trans);
	if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		le_host->setText(fldStr.c_str());
		le_host->setEnabled(true);
	}
	else
	{
		if(myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		le_host->setEnabled(false);
	}

	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_HOSTPORT");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
	if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		le_port->setText(jvx_int2String(valI32).c_str());
		le_port->setEnabled(true);
	}
	else
	{
		if(myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		le_port->setEnabled(false);
	}

	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_CONNECTION_TYPE");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		le_tech->setText(fldStr.c_str());
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
	}

	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_CONN_STATE");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		pb_connect->setEnabled(true);
		if(jvx_bitTest(selLst.bitFieldSelected(), 0))
		{
			setBackgroundLabelColor(Qt::white, stats[0]);
			pb_connect->setText("Connect");
			le_host->setEnabled(true);
			le_port->setEnabled(true);
		}
		if(jvx_bitTest(selLst.bitFieldSelected(), 1))
		{
			setBackgroundLabelColor(Qt::white, stats[1]);
		}
		if(jvx_bitTest(selLst.bitFieldSelected(), 2))
		{
			setBackgroundLabelColor(Qt::white, stats[2]);
			pb_connect->setText("Started..");
			le_host->setEnabled(false);
			le_port->setEnabled(false);
		}
		if(jvx_bitTest(selLst.bitFieldSelected(), 3))
		{
			pb_connect->setText("Waiting..");
			le_host->setEnabled(false);
			le_port->setEnabled(false);
			setBackgroundLabelColor(Qt::white, stats[3]);
		}
		if(jvx_bitTest(selLst.bitFieldSelected(), 4))
		{
			pb_connect->setText("Disconnect");
			le_host->setEnabled(false);
			le_port->setEnabled(false);
			setBackgroundLabelColor(Qt::white, stats[4]);
		}
		if(jvx_bitTest(selLst.bitFieldSelected(), 5))
		{
			pb_connect->setText("Disconnect");
			le_host->setEnabled(false);
			le_port->setEnabled(false);
			pb_connect->setEnabled(false);
			setBackgroundLabelColor(Qt::white, stats[5]);
		}
	}
	else
	{
		if(myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
	}

	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_AUTO_CONNECT");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &valB, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		cb_auto->setChecked(valB == c_true);
		cb_auto->setEnabled(true);
	}
	else
	{
		if(myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		cb_auto->setChecked(false);
		cb_auto->setEnabled(false);
	}

	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_AUTO_START");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &valB, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		cb_start->setChecked(valB == c_true);
		cb_start->setEnabled(true);
	}
	else
	{
		if(myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		cb_start->setChecked(false);
		cb_start->setEnabled(false);
	}
	
	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_HW_MODES");
	ident.reset(propName.c_str());
	trans.reset(false, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &selLst,  1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		if (cb_mode)
		{
			cb_mode->clear();
			for (i = 0; i < selLst.strList.ll(); i++)
			{
				std::string txt = selLst.strList.std_str_at(i);
				if (jvx_bitTest(selLst.bitFieldSelected(), i))
				{
					txt += "*";
				}
				cb_mode->addItem(txt.c_str());
			}
		}
	}
	jvxPropertyDescriptor dd;
	jvx_initPropertyDescription(dd);
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(propName.c_str());
	res = propRef->get_descriptor_property(callGate, dd, ident);
	if (dd.accessType == JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE)
	{
		cb_mode->setEnabled(true);
	}
	else
	{
		cb_mode->setEnabled(false);
	}

	valI16 = c_false;
	getSetTag = propertyGetSetTag;
	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_LOG_TO_FILE");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	cb_log->setChecked(valI16 == c_true);

	le_perform->setText("--");
	le_perform->setEnabled(false);

	le_prof->setText("--");
	le_prof->setEnabled(false);

	return JVX_NO_ERROR;
}

void
CjvxSaWrapperElementJvxNetworkMasterDevice::newTextDestHost()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string txt = le_host->text().toLatin1().data();
	jvxApiString fldStr;
	std::string propName;
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
	fldStr.assign_const(txt.c_str(), txt.size());

	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_HOSTNAME");
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, jPRG(
		&fldStr, 1, JVX_DATAFORMAT_STRING), 
		ident, trans);
	backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
		myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
}

void
CjvxSaWrapperElementJvxNetworkMasterDevice::newTextDestPort()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 port = le_port->text().toInt();
	std::string getSetTag = propertyGetSetTag;
	jvxCallManagerProperties callGate;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	std::string propName;
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_HOSTPORT");
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, jPRG(
		&port, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans );
	backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
		myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
}

void
CjvxSaWrapperElementJvxNetworkMasterDevice::pushedButtonGo()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string command;
	jvxApiString fldStr;
	jvxApiString  fldStrP;
	jvxSelectionList selLst;
	std::string getSetTag = propertyGetSetTag;
	jvxCallManagerProperties callGate;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	std::string txtError;
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_CONN_STATE");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		if(jvx_bitTest(selLst.bitFieldSelected(), 0))
		{
			command = "connect();";
		}
		else
		{
			command = "disconnect();";
		}
		fldStr.assign_const(command.c_str(), command.size());
		propName = jvx_makePathExpr(propertyGetSetTag, "JVX_SOCKET_COMMAND");
		res = propRef->set_property(callGate, jPRG(
			&fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
		if(res != JVX_NO_ERROR)
		{
			txtError = "Error sending command " + command + " to audio device"; 
			if(res == JVX_ERROR_INVALID_SETTING)
			{
				ident.reset(propName.c_str());
				trans.reset(false, 0, JVX_PROPERTY_DECODER_NONE);
				propRef->get_property(callGate, jPRG( &fldStrP,  1, JVX_DATAFORMAT_STRING), ident, trans);
				txtError += ", reason: ";
				txtError += fldStrP.std_str();
			}
			else
			{
				txtError += jvxErrorType_descr(res);
			}
			std::cout << txtError << std::endl;
		}
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

void 
CjvxSaWrapperElementJvxNetworkMasterDevice::autoConnectClicked(bool tog)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBool val = c_false;
	std::string propName;
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
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_AUTO_CONNECT");
	if(tog)
	{
		val = c_true;
	}
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, jPRG(
		&val, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
		myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
}

void 
CjvxSaWrapperElementJvxNetworkMasterDevice::autoStartClicked(bool tog)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBool val = c_false;
	std::string propName;
	std::string getSetTag = propertyGetSetTag;
	jvxCallManagerProperties callGate;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_AUTO_START");
	if(tog)
	{
		val = c_true;
	}
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, jPRG(
		&val, 1, JVX_DATAFORMAT_16BIT_LE),ident, trans);
	backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
		myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
}

void 
CjvxSaWrapperElementJvxNetworkMasterDevice::trigger_updateWindow_periodic(jvxPropertyCallContext ccontext, jvxSize* passedId)
{
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
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
	if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		txtShow += jvx_int2String(valI32);
	}
	else
	{
		txtShow += "-";
	}

	txtShow += "/";

	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_UNSENTFRAMES");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &valI32, 1, JVX_DATAFORMAT_32BIT_LE),ident, trans);
	if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		txtShow += jvx_int2String(valI32);
	}
	else
	{
		txtShow += "-";
	}

	txtShow += "/";

	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_LOSTFRAMES_REMOTE");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
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
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
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
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		txtShow += jvx_data2String(valD, 1) + "%";
	}
	else
	{
		txtShow += "-";
	}

	le_prof->setText(txtShow.c_str());
	le_prof->setEnabled(true);
}

void
CjvxSaWrapperElementJvxNetworkMasterDevice::toggledMode(int sel)
{
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	jvxErrorType res = JVX_NO_ERROR;
	propName = jvx_makePathExpr(getSetTag, "JVX_NET_AUDEV_HW_MODES");
	ident.reset(propName.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		jvx_bitToggle(selLst.bitFieldSelected(), sel);
	}
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, jPRG(
		&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), 
		ident, trans);
	assert(res == JVX_NO_ERROR);
	
	updateWindowUiElement();
}

void
CjvxSaWrapperElementJvxNetworkMasterDevice::toggledLog(bool tog)
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
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, jPRG(
		&valI16, 1, JVX_DATAFORMAT_16BIT_LE), 
		ident, trans);
	updateWindowUiElement();

}

void
CjvxSaWrapperElementJvxNetworkMasterDevice::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	assert(0);
}

jvxErrorType
CjvxSaWrapperElementJvxNetworkMasterDevice::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	return JVX_ERROR_UNSUPPORTED;
}