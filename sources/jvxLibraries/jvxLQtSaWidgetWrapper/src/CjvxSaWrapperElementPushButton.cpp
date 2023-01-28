#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementPushButton.h"
#include <QtWidgets/QFileDialog>
#include <QVariant>

CjvxSaWrapperElementPushButton::CjvxSaWrapperElementPushButton(QPushButton* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc,
	IjvxAccessProperties* propRefIn,
	std::string propertyGetSetTagLoc, std::vector<std::string> paramLstLoc,
	const std::string& tag_cp, const std::string& tag_key, const std::string& uiObjectNameLoc,
	const std::string&prefix, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef) :
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn, propertyGetSetTagLoc,
		paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId, hostRef)
{
	jvxSize i;
	uiRefTp = uiRefLoc;
	buttonPushState = false;
	buttonPushSel = 0;
	numSels = 0;
	modeRemote = false;
	validPicOn = false;
	validPicOff = false;
	tp = JVX_WW_PUSHBUTTON_DERIVE_TYPE;
	option_active = JVX_SIZE_UNSELECTED;	
	initializeUiElement();

}

void
CjvxSaWrapperElementPushButton::initializeUiElement(jvxPropertyCallContext ccontext)
{
	jvxBool foundit = false;
	std::string rtu;
	int myInt = 0;
	jvxInt32 valI32 = -1;
	jvxErrorType res = JVX_NO_ERROR;

	if (widgetStatus == JVX_STATE_NONE)
	{
		if (jvx_bitTest(this->thePropAFeatures, JVX_PROPERTY_ACCESS_DELAYED_RESPONSE_SHIFT))
		{
			modeRemote = true;
		}

		jvxErrorType res = this->initParameters_getMin(&backwardRef->keymaps.pb, paramLst);
		assert(res == JVX_NO_ERROR);

		backwardRef->keymaps.pb.getValueForKey("PICON", &namePicOn, JVX_WW_KEY_VALUE_TYPE_STRING);
		namePicOn = jvx_replaceStrInStr(namePicOn, "QRC", ":");

		backwardRef->keymaps.pb.getValueForKey("PICOFF", &namePicOff, JVX_WW_KEY_VALUE_TYPE_STRING);
		namePicOff = jvx_replaceStrInStr(namePicOff, "QRC", ":");

		backwardRef->keymaps.pb.getValueForKey("SSON", &onStyleSheet, JVX_WW_KEY_VALUE_TYPE_STRING);
		backwardRef->keymaps.pb.getValueForKey("SSOFF", &offStyleSheet, JVX_WW_KEY_VALUE_TYPE_STRING);

		backwardRef->keymaps.pb.getValueForKey("TXTON", &onShowText, JVX_WW_KEY_VALUE_TYPE_STRING);
		backwardRef->keymaps.pb.getValueForKey("TXTOFF", &offShowText, JVX_WW_KEY_VALUE_TYPE_STRING);

		backwardRef->keymaps.pb.getValueForKey("PICPREFIX", &prefixPics, JVX_WW_KEY_VALUE_TYPE_STRING);
		prefixPics = jvx_replaceStrInStr(prefixPics, "QRC", ":");

		backwardRef->keymaps.pb.getValueForKey("FILE_ENDING", &fileEnding, JVX_WW_KEY_VALUE_TYPE_STRING);
		
		backwardRef->keymaps.pb.getValueForKey("DEFAULT_FOLDER", &defaultFolder, JVX_WW_KEY_VALUE_TYPE_STRING);

		foundit = false;
		backwardRef->keymaps.pb.getValueForKey("OPTIONACTIVE", &valI32, JVX_WW_KEY_VALUE_TYPE_INT32, &foundit);
		if (foundit)
		{
			option_active = (jvxSize)valI32;
		}
		

		foundit = false;
		myInt = tp;
		backwardRef->keymaps.pb.getValueForKey("BEHAVIOR", &myInt, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
		if (foundit)
		{
			tp = (jvxWwConnectionType_pb)myInt;
		}
		if (!namePicOn.empty())
		{
			if (pixmapOn.load(namePicOn.c_str()))
			{
			  validPicOn = true;
			}
			else
			  {
				std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor << ": Unable to open picture " << namePicOn << " for icon." << std::endl;
			}
			
		}
		if (!namePicOff.empty())
		{
			if (pixmapOff.load(namePicOff.c_str()))
			{
			  validPicOff = true;
			}
			else
			  {
				std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor << ": Unable to open picture " << namePicOff << " for icon." << std::endl;
			}
		}
	}

	res = initializeProperty_core(true, ccontext);
	if (res != JVX_NO_ERROR)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*not connected*");
	}

	if (widgetStatus == JVX_STATE_SELECTED)
	{
		if (tp == JVX_WW_PUSHBUTTON_DERIVE_TYPE)
		{
			if (
				(thePropDescriptor.format == JVX_DATAFORMAT_BOOL) &&
				(thePropDescriptor.decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF))
			{
				tp = JVX_WW_PUSHBUTTON_CBOOL;
			}
			else if (
				thePropDescriptor.format == JVX_DATAFORMAT_SELECTION_LIST
				)
			{
				tp = JVX_WW_PUSHBUTTON_SELLIST;
			}
			else if (
				thePropDescriptor.format == JVX_DATAFORMAT_STRING
				)
			{
				switch (thePropDescriptor.decTp)
				{
				case JVX_PROPERTY_DECODER_FILENAME_OPEN:
					tp = JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD;
					break;
				case JVX_PROPERTY_DECODER_FILENAME_SAVE:
					tp = JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE;
					break;
				case JVX_PROPERTY_DECODER_DIRECTORY_SELECT:
					tp = JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY;
					break;
				}
			}
			else
			{
				std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " <<
					propDescriptor << ": Incorrect property type, expected JVX_DATAFORMAT_BOOL + JVX_PROPERTY_DECODER_SIMPLE_ONOFF, JVX_DATAFORMAT_SELECTION_LIST" << std::endl;
				widgetStatus = JVX_STATE_NONE;
			}
		}

		if (tp == JVX_WW_PUSHBUTTON_DERIVE_TYPE)
		{
			std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " <<
				propDescriptor << ": The button behavior is not specified, using default assumptions instead." << std::endl;
		}

		uiRefTp->setProperty("mySaWidgetRefPushButton", QVariant::fromValue<CjvxSaWrapperElementPushButton*>(this));
		connect(uiRefTp, SIGNAL(clicked()), this, SLOT(button_pushed()));

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_GET_DESCRIPTOR_SHIFT);
			jvx_bitSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_GET_EXTENDED_INFO_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__, operDelayed);
		}
		updateWindow_core();

		assert(backwardRef);
		backwardRef->reportPropertyInitComplete(selector_lst.c_str(), assoc_id);
	}
	else if (widgetStatus == JVX_STATE_INIT)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*update*");
	}
	else
	{
		// Let us free the channel to reuest initial parameters even if property failed
		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_GET_DESCRIPTOR_SHIFT);
			jvx_bitSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_GET_EXTENDED_INFO_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__, operDelayed);
		}
		mainWidgetDisable();
		uiRefTp->setToolTip("*failed*");
	}
	//verboseLoc = this->myBaseProps.verbose_out;
}

CjvxSaWrapperElementPushButton::~CjvxSaWrapperElementPushButton()
{
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		disconnect(uiRefTp, SIGNAL(clicked()));
	}
}
	
void
CjvxSaWrapperElementPushButton::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	std::string getSetTag = propertyGetSetTag;
	jvxBool condSet = false;
	jvxPropertyDescriptor descr;
	jvxCallManagerProperties callGate;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(getSetTag.c_str());
	if (globalId)
	{
		res = propRef->get_descriptor_property(callGate, thePropDescriptor, ident, jPSCH(globalId));
	}
	else
	{
		res = propRef->get_descriptor_property(callGate, thePropDescriptor, ident, jPSCH(&procId));
	}
	if (res == JVX_NO_ERROR)
	{
		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_STATUS_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__, operDelayed);
		}
		updateUiDescriptor(callGate.access_protocol);
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
	}
}

jvxErrorType
CjvxSaWrapperElementPushButton::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* globalId, jvxBool call_periodic_update)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBool valB = false;
	jvxBool ctOnly = true;
	jvxSelectionList selLst;
	std::string getSetTag = propertyGetSetTag;
	jvxBool isValid = false;
	jvxCallManagerProperties callGate;
	callGate.on_get.prop_access_type = &thePropDescriptor.accessType;

	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	
	if (widgetStatus != JVX_STATE_SELECTED)
	{
		return JVX_ERROR_NOT_READY;
	}

	switch (tp)
	{
	case JVX_WW_PUSHBUTTON_CBOOL:
		ident.reset(getSetTag.c_str());
		trans.reset(true, 0, thePropDescriptor.decTp);
		if (globalId)
		{
			res = propRef->get_property(callGate, jPRIO<jvxCBool>(valB), ident, trans, jPSCH(globalId));
		}
		else
		{
			res = propRef->get_property(callGate, jPRIO<jvxCBool>(valB), ident, trans, jPSCH(&procId));
		}

		if (res == JVX_NO_ERROR)
		{
			buttonPushState = (valB != c_false);

			if (valB == c_false)
			{
				if (validPicOff)
				{
					uiRefTp->setIcon(pixmapOff);
				}
				else
				{
					uiRefTp->setStyleSheet(offStyleSheet.c_str());
				}
				if (!(offShowText.empty() && onShowText.empty()))
				{
					uiRefTp->setText(offShowText.c_str());
				}
			}
			else
			{
				if (validPicOn)
				{
					uiRefTp->setIcon(pixmapOn);
				}
				else
				{
					uiRefTp->setStyleSheet(onStyleSheet.c_str());
				}
				if(! (offShowText.empty() && onShowText.empty()))
				{
					uiRefTp->setText(onShowText.c_str());
				}
			}
		}
		break;
	case JVX_WW_PUSHBUTTON_SELLIST:

		if (JVX_CHECK_SIZE_SELECTED(option_active))
		{
			ctOnly = false;
		}

		ident.reset(getSetTag.c_str());
		trans.reset(ctOnly, 0, thePropDescriptor.decTp);

		if (globalId)
		{
			res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans, jPSCH(globalId));
		}
		else
		{
			res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans, jPSCH(&procId));
		}
		if(res == JVX_NO_ERROR)
		{	
			if (!myBaseProps.supersede_enable)
			{
				numSels = selLst.strList.ll();
				if (JVX_CHECK_SIZE_SELECTED(option_active))
				{
					if (option_active < numSels)
					{
						uiRefTp->setText(selLst.strList.std_str_at(option_active).c_str());
					}
					else
					{
						uiRefTp->setText(("Option <" + jvx_size2String(option_active) + "> out of range").c_str());
					}
				}
				else
				{
					buttonPushSel = jvx_bitfieldSelection2Id(selLst.bitFieldSelected(), numSels);

					while (validPics.size() < numSels)
					{
						QPixmap pixmap;
						std::string picName = prefixPics + jvx_size2String(validPics.size());
						if (pixmap.load(picName.c_str()))
						{
							validPics.push_back(true);
						}
						else
						{
							std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor << ": Unable to open picture " << picName << " for icon." << std::endl;
							validPics.push_back(false);
						}
						pixmaps.push_back(pixmap);
					}
					while (validPics.size() > numSels)
					{
						validPics.pop_back();
						//pixmaps.push_back(
					}


					if (JVX_CHECK_SIZE_SELECTED(buttonPushSel))
					{
						if (buttonPushSel < validPics.size())
						{
							if (validPics[buttonPushSel])
							{
								uiRefTp->setIcon(pixmaps[buttonPushSel]);
							}
							else
							{
								uiRefTp->setText(jvx_size2String(buttonPushSel).c_str());
							}
						}
						else
						{
							uiRefTp->setText(jvx_size2String(buttonPushSel).c_str());
						}
					}
					else
					{
						if (myBaseProps.verbose_out)
						{
							std::cout << "Invalid selection in selection list for push button " << uiObjectName << " linked with property " << getSetTag << "." << std::endl;
						}
					}
				}
			}
		}
		break;
	case JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY:
	case JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD:
	case JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE:
		
		ident.reset(getSetTag.c_str());
		trans.reset(ctOnly, 0, thePropDescriptor.decTp);

		callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_GET_ACCESS_DATA; // to allow NULL pointer call
		if (globalId)
		{
			res = propRef->get_property(callGate, jPRG(NULL, 1, JVX_DATAFORMAT_NONE), ident, trans, jPSCH(globalId));
		}
		else
		{
			res = propRef->get_property(callGate, jPRG(NULL, 1, JVX_DATAFORMAT_NONE), ident, trans, jPSCH(&procId));
		}
		uiRefTp->setText("..");
		break;
	}
	if (res == JVX_NO_ERROR)
	{
		updateUiDescriptor(callGate.access_protocol);

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__, operDelayed);
		}
		// Forward the get result to listening application
		if (myBaseProps.report_get)
		{
			backwardRef->reportPropertyGet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), &valB, 0, 1, thePropDescriptor.format, static_cast<CjvxSaWrapperElementBase*>(this), res);
		}
	}
	else if (res == JVX_ERROR_POSTPONE)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*update*");
	}
	else
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*failed*");
		if (res != JVX_ERROR_WRONG_STATE)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
			}
		}
	}
	return JVX_NO_ERROR;
}

void 
CjvxSaWrapperElementPushButton::button_pushed()
{
	setProperties_core_ui();
}

void
CjvxSaWrapperElementPushButton::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	std::string txt;
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	jvxApiString strFld;
	selLst.bitFieldSelected() = 0;
	selLst.bitFieldExclusive = 0;
	std::string getSetTag = propertyGetSetTag;
	QString qstr;
	jvxCallManagerProperties callGate;

	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	ident.reset(getSetTag.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);

	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	if(myBaseProps.verbose_out)
	{
		std::cout << "QPushButton::clicked() callback triggered for " << uiObjectName << std::endl;
	}
	switch(tp)
	{
	case JVX_WW_PUSHBUTTON_CBOOL:
		if (modeRemote)
		{
			boolVar = c_true;
			if(!buttonPushState)
				boolVar = c_false;
		}
		else
		{
			ident.reset(getSetTag.c_str());
			trans.reset(true, 0, thePropDescriptor.decTp);

			res = propRef->get_property(callGate, jPRIO<jvxCBool>(boolVar), ident, trans, jPSCH(&procId));
		}

		if(res == JVX_NO_ERROR)
		{
			if(boolVar == c_true)
			{
				boolVar = c_false;
			}
			else
			{
				boolVar = c_true;
			}
			res = propRef->set_property(callGate, jPRIO<jvxCBool>(boolVar),
				ident, trans, jPSCH(&procId));
		}
		break;
	case JVX_WW_PUSHBUTTON_SELLIST:
		if (modeRemote)
		{
			if (JVX_CHECK_SIZE_SELECTED(option_active))
			{
				jvx_bitZSet(selLst.bitFieldSelected(), option_active);
				res = propRef->set_property(callGate ,
					jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST),
					ident, trans, jPSCH(&procId));
			}
		}
		else
		{
			ident.reset(getSetTag.c_str());
			trans.reset(true, 0, thePropDescriptor.decTp);

			res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans, jPSCH(&procId));
			if (res == JVX_NO_ERROR)
			{
				numSels = selLst.strList.ll();
				buttonPushSel = jvx_bitfieldSelection2Id(selLst.bitFieldSelected(), numSels);
			}
		
			buttonPushSel = (buttonPushSel + 1) % numSels;

			selLst.bitFieldSelected() = 0;
			jvx_bitSet(selLst.bitFieldSelected(), buttonPushSel);

			res = propRef->set_property(callGate, jPRG(
				&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), 
				ident, trans, jPSCH(&procId) );
		}
		break;
	case JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY:

		if (!modeRemote)
		{
			qstr = QFileDialog::getExistingDirectory(uiRefTp, tr("Open Folder"), defaultFolder.c_str());
			txt = qstr.toLatin1().data();
			txt = jvx_replaceCharacter(txt, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
			txt = jvx_absoluteToRelativePath(txt, false);
			strFld.assign_const(txt.c_str(), txt.size());

			res = propRef->set_property(callGate, jPRG(
				&strFld, 1, JVX_DATAFORMAT_STRING), 
				ident, trans, jPSCH(&procId));
		}
		else
		{
			std::cout << __FUNCTION__ << ": " << "remote open directory currently not implemented." << std::endl;
		}
		break;
	case JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD:
		if (!modeRemote)
		{
			qstr = QFileDialog::getOpenFileName(uiRefTp, tr("Open File"), defaultFolder.c_str(), fileEnding.c_str());
			txt = qstr.toLatin1().data();
			if (!txt.empty())
			{
				txt = jvx_replaceCharacter(txt, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
				txt = jvx_absoluteToRelativePath(txt, true);
				strFld.assign_const(txt.c_str(), txt.size());
				res = propRef->set_property(callGate ,
					jPRG(&strFld, 1, JVX_DATAFORMAT_STRING),
					ident, trans, jPSCH(&procId));
			}
		}
		else
		{
			std::cout << __FUNCTION__ << ": " << "remote open directory currently not implemented." << std::endl;
		}
		break;
	case JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE:
		if (!modeRemote)
		{
			qstr = QFileDialog::getSaveFileName(uiRefTp, tr("Save File"), tr(".\\"));
			txt = qstr.toLatin1().data();
			txt = jvx_replaceCharacter(txt, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
			txt = jvx_absoluteToRelativePath(txt, true);
			strFld.assign_const(txt.c_str(), txt.size());
			res = propRef->set_property(callGate ,
				jPRG(&strFld, 1, JVX_DATAFORMAT_STRING),
				ident, trans, jPSCH(&procId));
		}
		else
		{
			std::cout << __FUNCTION__ << ": " << "remote open directory currently not implemented." << std::endl;
		}
		break;
	}
	if (res == JVX_NO_ERROR)
	{
		updateUiDescriptor(callGate.access_protocol);

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__, operDelayed);
		}

		if (backwardRef)
		{
			backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
				myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
		}
	}
	else if (res == JVX_ERROR_POSTPONE)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*update*");

		updateUiDescriptor(callGate.access_protocol);
	}
	else
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*failed*");
		if(myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

jvxErrorType
CjvxSaWrapperElementPushButton::setPropertiesDirect(jvxHandle* prop, 
	jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	std::string txt;
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	jvxApiString strFld;
	selLst.bitFieldSelected() = 0;
	selLst.bitFieldExclusive = 0;
	std::string getSetTag = propertyGetSetTag;
	QString qstr;
	jvxCallManagerProperties callGate;

	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	ident.reset(getSetTag.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);

	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	if (myBaseProps.verbose_out)
	{
		std::cout << "QPushButton::clicked() callback triggered for " << uiObjectName << std::endl;
	}
	switch (tp)
	{
	case JVX_WW_PUSHBUTTON_CBOOL:

		ident.reset(getSetTag.c_str());
		trans.reset(true, offset, thePropDescriptor.decTp, false);

		res = propRef->set_property(callGate, jPRG(
			prop, numElements, form), 
			ident, trans, jPSCH(&procId));
		break;
	case JVX_WW_PUSHBUTTON_SELLIST:
		
		ident.reset(getSetTag.c_str());
		trans.reset(true, offset, thePropDescriptor.decTp, false);
		res = propRef->set_property(callGate, jPRG(
			prop, numElements, form), 
			ident, trans, jPSCH(&procId));
		break;
	case JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY:

		if (!modeRemote)
		{
			qstr = QFileDialog::getExistingDirectory(uiRefTp, tr("Open Folder"), tr(".\\"));
			txt = qstr.toLatin1().data();
			txt = jvx_replaceCharacter(txt, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
			txt = jvx_absoluteToRelativePath(txt, false);
			strFld.assign_const(txt.c_str(), txt.size());
			
			res = propRef->set_property(callGate ,
				jPRG(&strFld, 1, JVX_DATAFORMAT_STRING), ident, trans, jPSCH(&procId));
		}
		else
		{
			std::cout << __FUNCTION__ << ": " << "remote open directory currently not implemented." << std::endl;
			res = JVX_ERROR_UNSUPPORTED;
		}
		break;
	case JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD:
		if (!modeRemote)
		{
			qstr = QFileDialog::getOpenFileName(uiRefTp, tr("Open File"), tr(".\\"));
			txt = qstr.toLatin1().data();
			txt = jvx_replaceCharacter(txt, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
			txt = jvx_absoluteToRelativePath(txt, true);
			strFld.assign_const(txt.c_str(), txt.size());
			res = propRef->set_property(callGate, jPRG(
				&strFld, 1, JVX_DATAFORMAT_STRING), ident, trans, jPSCH(&procId));
		}
		else
		{
			std::cout << __FUNCTION__ << ": " << "remote open directory currently not implemented." << std::endl;
			res = JVX_ERROR_UNSUPPORTED;
		}
		break;
	case JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE:
		if (!modeRemote)
		{
			qstr = QFileDialog::getSaveFileName(uiRefTp, tr("Save File"), tr(".\\"));
			txt = qstr.toLatin1().data();
			txt = jvx_replaceCharacter(txt, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
			txt = jvx_absoluteToRelativePath(txt, true);
			strFld.assign_const(txt.c_str(), txt.size());
			res = propRef->set_property(callGate, jPRG(
				&strFld, 1, JVX_DATAFORMAT_STRING),
				ident, trans, jPSCH(&procId));
		}
		else
		{
			std::cout << __FUNCTION__ << ": " << "remote open directory currently not implemented." << std::endl;
			res = JVX_ERROR_UNSUPPORTED;
		}
		break;
	}
	if (res == JVX_NO_ERROR)
	{
		updateUiDescriptor(callGate.access_protocol);

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__, operDelayed);
		}

		if (backwardRef)
		{
			backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
				myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
		}
	}
	else if (res == JVX_ERROR_POSTPONE)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*update*");

		updateUiDescriptor(callGate.access_protocol);
	}
	else
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*failed*");
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
	return res;
}
