#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementLineEdit.h"
#include <QVariant>

CjvxSaWrapperElementLineEdit::CjvxSaWrapperElementLineEdit(QLineEdit* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
	IjvxAccessProperties* propRefIn, std::string propertyGetSetTagLoc,
	std::vector<std::string> paramLstLoc, const std::string& tag_cp, const std::string& tag_key, const std::string& uiObjectNameLoc,
	const std::string & prefix, QWidget* crossRefWidget, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef):
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn, propertyGetSetTagLoc,
		paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId, hostRef)
{
	jvxSize i;

	optLabelShowTp = JVX_WW_SHOW_DIRECT;
	transformScale = 1.0;
	numDigits = 2;
	minVal = JVX_DATA_MAX_NEG;
	maxVal = JVX_DATA_MAX_POS;
	granularity = 0.0;
	uiRefTp = uiRefLoc;
	myCrossRefWidget = crossRefWidget;
	returnModeOnly = false;
	optButtonTrigger = NULL;

	// Evaluate the property descriptors
	initializeUiElement();

	//verboseLoc = this->myBaseProps.verbose_out;
}

CjvxSaWrapperElementLineEdit::~CjvxSaWrapperElementLineEdit()
{
	if (widgetStatus > JVX_STATE_NONE)
	{
		disconnect(uiRefTp, SIGNAL(editingFinished()));
	}
}
	
void 
CjvxSaWrapperElementLineEdit::clearUiElements()
{
	if (uiRefTp)
		uiRefTp->setText("");
}

void
CjvxSaWrapperElementLineEdit::initializeUiElement(jvxPropertyCallContext ccontext)
{
	std::string rtu;
	jvxBool foundit = false;
	jvxInt32 intVal = 0;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (widgetStatus == JVX_STATE_NONE)
	{
		// First time enter nitialize - the properties are scanned here.
		res = this->initParameters_getMin(&backwardRef->keymaps.le, paramLst);
		assert(res == JVX_NO_ERROR);

		optLabelShowTp = JVX_WW_SHOW_DIRECT;
		transformScale = 1.0;
		optButtonName = "";
		optButtonTrigger = NULL;
		numDigits = 2;
		minVal = JVX_DATA_MAX_NEG;
		maxVal = JVX_DATA_MAX_POS;

		backwardRef->keymaps.le.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32);
		backwardRef->keymaps.le.getValueForKey("SCALEFAC", &transformScale, JVX_WW_KEY_VALUE_TYPE_DATA);
		backwardRef->keymaps.le.getValueForKey("MINVAL", &minVal, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		backwardRef->keymaps.le.getValueForKey("MAXVAL", &maxVal, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		backwardRef->keymaps.le.getValueForKey("GRANULARITY", &granularity, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		foundit = false;
		backwardRef->keymaps.le.getValueForKey("LABELTRANSFORM", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
		if (foundit)
		{
			optLabelShowTp = (jvxWwTransformValueType)intVal;
		}
		backwardRef->keymaps.le.getValueForKey("EDITRETURNONLY", &returnModeOnly, JVX_WW_KEY_VALUE_TYPE_BOOL);
		backwardRef->keymaps.le.getValueForKey("BUTTONREF", &optButtonName, JVX_WW_KEY_VALUE_TYPE_STRING);
		uiRefTp->setProperty("mySaWidgetRefLineEdit", QVariant::fromValue<CjvxSaWrapperElementLineEdit*>(this));

		if (!optButtonName.empty())
		{
			assert(myCrossRefWidget);
			QList<QWidget *> widgets = myCrossRefWidget->findChildren<QWidget *>();
			for (i = 0; i < widgets.count(); i++)
			{
				QString name = widgets[JVX_SIZE_INT(i)]->objectName();
				if (name.toLatin1().data() == optButtonName)
				{
					optButtonTrigger = qobject_cast<QPushButton*>(widgets[JVX_SIZE_INT(i)]);
					break;
				}
			}
		}
		setBgPal();
	}

	res = initializeProperty_core(true, ccontext);
	if (res != JVX_NO_ERROR)
	{
		mainWidgetDisable();
		uiRefTp->setText("*not connected*");
	}
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		mainWidgetDisable();

		switch (thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_STRING:
		case JVX_DATAFORMAT_DATA:
		case JVX_DATAFORMAT_64BIT_LE:
		case JVX_DATAFORMAT_32BIT_LE:
		case JVX_DATAFORMAT_16BIT_LE:
		case JVX_DATAFORMAT_8BIT:
		case JVX_DATAFORMAT_U64BIT_LE:
		case JVX_DATAFORMAT_U32BIT_LE:
		case JVX_DATAFORMAT_U16BIT_LE:
		case JVX_DATAFORMAT_U8BIT:
		case JVX_DATAFORMAT_SIZE:
			break;
		default:
			std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor
				<< ": Incorrect type, expected JVX_DATAFORMAT_STRING, JVX_DATAFORMAT_DATA, JVX_DATAFORMAT_64BIT_LE, JVX_DATAFORMAT_32BIT_LE, JVX_DATAFORMAT_16BIT_LE or JVX_DATAFORMAT_8BIT." << std::endl;
			widgetStatus = JVX_STATE_NONE;
			break;
		}

		if (optButtonTrigger)
		{
			connect(optButtonTrigger, SIGNAL(clicked()), this, SLOT(transfer_triggered()));
		}
		else
		{
			connect(uiRefTp, SIGNAL(editingFinished()), this, SLOT(editing_finished()));
			connect(uiRefTp, SIGNAL(returnPressed()), this, SLOT(return_pressed()));
		}
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
	else if(widgetStatus == JVX_STATE_INIT)
	{
		mainWidgetDisable();
#ifdef JVX_WIDGET_WRAPPER_FIELD_INFO_UPDATE			
		uiRefTp->setText("*update i*");
#endif
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
		uiRefTp->setText("*failed*");
	}
	// -> verboseLoc = this->myBaseProps.verbose_out;
}

void
CjvxSaWrapperElementLineEdit::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	std::string getSetTag = propertyGetSetTag;
	jvxCallManagerProperties callGate;
	jvxBool condSet = false;
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
			handleAccessDelayed_stop(__FUNCTION__ , operDelayed);
		}
		updateUiDescriptor(callGate.access_protocol);
	}
	else if (res == JVX_ERROR_POSTPONE)
	{
		mainWidgetDisable();
#ifdef JVX_WIDGET_WRAPPER_FIELD_INFO_UPDATE			
		uiRefTp->setText("*update st*");
#endif
	}
	else
	{
		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_STATUS_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__ , operDelayed);
		}
		QFont ft = uiRefTp->font();
		ft.setItalic(true);
		uiRefTp->setFont(ft);
		std::cout << "Failed to get status update for object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
	}
}

jvxErrorType
CjvxSaWrapperElementLineEdit::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* passedId, jvxBool call_periodic_update)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt8 val8;
	jvxInt16 val16;
	jvxInt32 val32;
	jvxInt64 val64;
	jvxUInt8 valU8;
	jvxUInt16 valU16;
	jvxUInt32 valU32;
	jvxUInt64 valU64;
	jvxData valD;
	jvxSize valS;
	jvxHandle* ptrFld = NULL;
	jvxApiString  str;
	std::string txt;
	std::string unit;
	jvxBool isValid = false;
	jvxCallManagerProperties callGate;
	callGate.on_get.prop_access_type = &thePropDescriptor.accessType;

	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	
	if (widgetStatus != JVX_STATE_SELECTED)
	{
		return JVX_ERROR_NOT_READY;
	}

	switch(thePropDescriptor.format)
	{
	case JVX_DATAFORMAT_DATA:
		ptrFld = &valD;
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		ptrFld = &val16;
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		ptrFld = &val32;
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		ptrFld = &val64;
		break;
	case JVX_DATAFORMAT_8BIT:
		ptrFld = &val8;
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		ptrFld = &valU16;
		break;
	case JVX_DATAFORMAT_U32BIT_LE:
		ptrFld = &valU32;
		break;
	case JVX_DATAFORMAT_U64BIT_LE:
		ptrFld = &valU64;
		break;
	case JVX_DATAFORMAT_U8BIT:
		ptrFld = &valU8;
		break;
	case JVX_DATAFORMAT_SIZE:
		ptrFld = &valS;
		break;
	case JVX_DATAFORMAT_STRING:
		ptrFld = &str;
		break;
	}

	ident.reset(getSetTag.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp);
	if (passedId)
	{
		res = propRef->get_property(callGate, jPRG(ptrFld, 1, thePropDescriptor.format), ident, trans, jPSCH(passedId));
	}
	else
	{
		res = propRef->get_property(callGate, jPRG(ptrFld, 1, thePropDescriptor.format), ident, trans, jPSCH(&procId));
	}

	if(res == JVX_NO_ERROR)
	{
		mainWidgetEnable();

		switch(thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_8BIT:
			valD = JVX_INT32_DATA(val8);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			//valD = JVX_MAX(minVal, valD);
			//valD = JVX_MIN(maxVal, valD);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_U8BIT:
			valD = JVX_UINT32_DATA(valU8);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			//valD = JVX_MAX(minVal, valD);
			//valD = JVX_MIN(maxVal, valD);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_SIZE:
			if (JVX_CHECK_SIZE_SELECTED(valS))
			{
				valD = JVX_SIZE_DATA(valS);
				valD = transformTo(valD, optLabelShowTp, transformScale);
				getDisplayUnitToken(optLabelShowTp, unit);
				//valD = JVX_MAX(minVal, valD);
				//valD = JVX_MIN(maxVal, valD);
				txt = jvx_data2String(valD, numDigits) + unit;
			}
			else
			{
				txt = jvx_size2String(valS, 2);
			}
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			valD = JVX_INT32_DATA(val16);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			//valD = JVX_MAX(minVal, valD);
			//valD = JVX_MIN(maxVal, valD);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			valD = JVX_UINT32_DATA(valU16);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			//valD = JVX_MAX(minVal, valD);
			//valD = JVX_MIN(maxVal, valD);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			valD = JVX_INT32_DATA(val32);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			//valD = JVX_MAX(minVal, valD);
			//valD = JVX_MIN(maxVal, valD);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			valD = JVX_UINT32_DATA(valU32);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			//valD = JVX_MAX(minVal, valD);
			//valD = JVX_MIN(maxVal, valD);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			valD = JVX_INT64_DATA(val64);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			//valD = JVX_MAX(minVal, valD);
			//valD = JVX_MIN(maxVal, valD);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			valD = JVX_UINT64_DATA(valU64);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			//valD = JVX_MAX(minVal, valD);
			//valD = JVX_MIN(maxVal, valD);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_DATA:
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			//valD = JVX_MAX(minVal, valD);
			//valD = JVX_MIN(maxVal, valD);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_STRING:
			txt = str.std_str();
			break;
		}
		uiRefTp->setText(txt.c_str());
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
			backwardRef->reportPropertyGet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), ptrFld, 0, 1, thePropDescriptor.format, static_cast<CjvxSaWrapperElementBase*>(this), res);
		}
	}
	else if (res == JVX_ERROR_POSTPONE)
	{
		mainWidgetDisable();
#ifdef JVX_WIDGET_WRAPPER_FIELD_INFO_UPDATE			
		uiRefTp->setText("*update g*");
#endif
	}
	else
	{
		mainWidgetDisable();
		uiRefTp->setText(jvxErrorType_txt(res));
		if(res != JVX_ERROR_WRONG_STATE)
		{
			if(myBaseProps.verbose_out)
			{
				std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
			}
		}
		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__ , operDelayed);
		}
		backwardRef->reportPropertyGet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), ptrFld, 0, 1, thePropDescriptor.format, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
	return JVX_NO_ERROR;
}

void
CjvxSaWrapperElementLineEdit::setBgPal()
{
	QPalette palette;
	QBrush brush(Qt::white);

	if (returnModeOnly)
	{
		brush.setColor(QColor(255, 255, 220, 255)); // Light yellow brd
	}
	brush.setStyle(Qt::SolidPattern);
	palette.setBrush(QPalette::Active, QPalette::Base, brush);
	palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
	palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
	uiRef->setPalette(palette);
}

jvxErrorType
CjvxSaWrapperElementLineEdit::set_config(jvxWwConfigType tp, jvxHandle* load)
{
	switch (tp)
	{
	case JVX_WW_CONFIG_LINEEDIT_REACT_MODE_RETURN_ONLY:
		returnModeOnly = *((jvxBool*)load);
		setBgPal();
		break;
	}
	return JVX_NO_ERROR;
}

void 
CjvxSaWrapperElementLineEdit::editing_finished()
{
	if (!returnModeOnly)
	{
		setProperties_core_ui();
	}
}

void
CjvxSaWrapperElementLineEdit::return_pressed()
{
	if (returnModeOnly)
	{
		setProperties_core_ui();
	}
}

void
CjvxSaWrapperElementLineEdit::transfer_triggered()
{
	if (!returnModeOnly)
	{
		setProperties_core_ui();
	}
}

void
CjvxSaWrapperElementLineEdit::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD;
	jvxSize valS;
	jvxInt8 val8;
	jvxInt16 val16;
	jvxInt32 val32;
	jvxInt64 val64;
	jvxUInt8 valU8;
	jvxUInt16 valU16;
	jvxUInt32 valU32;
	jvxUInt64 valU64;
	jvxBool err = false;
	QString qtxt = uiRefTp->text();
	jvxApiString fldStr;
	std::string txt;
	jvxHandle* ptrFld = NULL;
	std::string unit;
	jvxCallManagerProperties callGate;

	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
	getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}*/
	if (myBaseProps.verbose_out)
	{
		std::cout << "QLineEdit::editing_finished() callback triggered for " << uiObjectName << std::endl;
	}

	if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_NORMAL)
	{
		// Get string
		txt = qtxt.toLatin1().data();

		switch (thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_8BIT:

			// Remove possible unit part
			getDisplayUnitToken(optLabelShowTp, unit);
			if (!unit.empty())
			{
				size_t poss = txt.rfind(unit);
				if (poss != std::string::npos)
				{
					txt = txt.substr(0, poss);
				}
			}

			// Transform string to value
			valD = jvx_string2Data(txt.c_str(), err);
			if (err)
			{
				std::cout << __FUNCTION__ << ":" << __LINE__ << ": Error when converting expression <" << txt << "> into a data value." << std::endl;
			}
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);

			valD = transformFrom(valD, optLabelShowTp, transformScale);
			if (granularity > 0.0)
			{
				valD = valD / granularity;
				valD = granularity * (jvxData)(JVX_DATA2INT64(valD));
			}
			val8 = JVX_DATA2INT8(valD);
			ptrFld = &val8;
			break;

		case JVX_DATAFORMAT_U8BIT:

			// Remove possible unit part
			getDisplayUnitToken(optLabelShowTp, unit);
			if (!unit.empty())
			{
				size_t poss = txt.rfind(unit);
				if (poss != std::string::npos)
				{
					txt = txt.substr(0, poss);
				}
			}

			// Transform string to value
			valD = jvx_string2Data(txt.c_str(), err);
			if (err)
			{
				std::cout << __FUNCTION__ << ":" << __LINE__ << ": Error when converting expression <" << txt << "> into a data value." << std::endl;
			}
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);

			valD = transformFrom(valD, optLabelShowTp, transformScale);

			if (granularity > 0.0)
			{
				valD = valD / granularity;
				valD = granularity * (jvxData)(JVX_DATA2INT64(valD));
			}

			valU8 = JVX_DATA2UINT8(valD);
			ptrFld = &valU8;
			break;
		case JVX_DATAFORMAT_SIZE:

			// Remove possible unit part
			getDisplayUnitToken(optLabelShowTp, unit);
			if (!unit.empty())
			{
				size_t poss = txt.rfind(unit);
				if (poss != std::string::npos)
				{
					txt = txt.substr(0, poss);
				}
			}

			// Transform string to value
			valD = jvx_string2Data(txt.c_str(), err);
			if (err)
			{
				std::cout << __FUNCTION__ << ":" << __LINE__ << ": Error when converting expression <" << txt << "> into a data value." << std::endl;
			}
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);

			valD = transformFrom(valD, optLabelShowTp, transformScale);

			if (granularity > 0.0)
			{
				valD = valD / granularity;
				valD = granularity * (jvxData)(JVX_DATA2INT64(valD));
			}

			valS = JVX_DATA2SIZE(valD);
			ptrFld = &valS;
			break;

		case JVX_DATAFORMAT_16BIT_LE:

			// Remove possible unit part
			getDisplayUnitToken(optLabelShowTp, unit);
			if (!unit.empty())
			{
				size_t poss = txt.rfind(unit);
				if (poss != std::string::npos)
				{
					txt = txt.substr(0, poss);
				}
			}

			// Transform string to value
			valD = jvx_string2Data(txt.c_str(), err);
			if (err)
			{
				std::cout << __FUNCTION__ << ":" << __LINE__ << ": Error when converting expression <" << txt << "> into a data value." << std::endl;
			}
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);

			valD = transformFrom(valD, optLabelShowTp, transformScale);

			if (granularity > 0.0)
			{
				valD = valD / granularity;
				valD = granularity * (jvxData)(JVX_DATA2INT64(valD));
			}

			val16 = JVX_DATA2INT16(valD);
			ptrFld = &val16;
			break;

		case JVX_DATAFORMAT_U16BIT_LE:

			// Remove possible unit part
			getDisplayUnitToken(optLabelShowTp, unit);
			if (!unit.empty())
			{
				size_t poss = txt.rfind(unit);
				if (poss != std::string::npos)
				{
					txt = txt.substr(0, poss);
				}
			}

			// Transform string to value
			valD = jvx_string2Data(txt.c_str(), err);
			if (err)
			{
				std::cout << __FUNCTION__ << ":" << __LINE__ << ": Error when converting expression <" << txt << "> into a data value." << std::endl;
			}
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);

			valD = transformFrom(valD, optLabelShowTp, transformScale);

			if (granularity > 0.0)
			{
				valD = valD / granularity;
				valD = granularity * (jvxData)(JVX_DATA2INT64(valD));
			}

			valU16 = JVX_DATA2UINT16(valD);
			ptrFld = &valU16;
			break;
		case JVX_DATAFORMAT_32BIT_LE:

			// Remove possible unit part
			getDisplayUnitToken(optLabelShowTp, unit);
			if (!unit.empty())
			{
				size_t poss = txt.rfind(unit);
				if (poss != std::string::npos)
				{
					txt = txt.substr(0, poss);
				}
			}

			// Transform string to value
			valD = jvx_string2Data(txt.c_str(), err);
			if (err)
			{
				std::cout << __FUNCTION__ << ":" << __LINE__ << ": Error when converting expression <" << txt << "> into a data value." << std::endl;
			}
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);

			valD = transformFrom(valD, optLabelShowTp, transformScale);

			if (granularity > 0.0)
			{
				valD = valD / granularity;
				valD = granularity * (jvxData)(JVX_DATA2INT64(valD));
			}

			val32 = JVX_DATA2INT32(valD);
			ptrFld = &val32;
			break;

		case JVX_DATAFORMAT_U32BIT_LE:

			// Remove possible unit part
			getDisplayUnitToken(optLabelShowTp, unit);
			if (!unit.empty())
			{
				size_t poss = txt.rfind(unit);
				if (poss != std::string::npos)
				{
					txt = txt.substr(0, poss);
				}
			}

			// Transform string to value
			valD = jvx_string2Data(txt.c_str(), err);
			if (err)
			{
				std::cout << __FUNCTION__ << ":" << __LINE__ << ": Error when converting expression <" << txt << "> into a data value." << std::endl;
			}
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);

			valD = transformFrom(valD, optLabelShowTp, transformScale);

			if (granularity > 0.0)
			{
				valD = valD / granularity;
				valD = granularity * (jvxData)(JVX_DATA2INT64(valD));
			}

			valU32 = JVX_DATA2UINT32(valD);
			ptrFld = &valU32;
			break;
		case JVX_DATAFORMAT_64BIT_LE:

			// Remove possible unit part
			getDisplayUnitToken(optLabelShowTp, unit);
			if (!unit.empty())
			{
				size_t poss = txt.rfind(unit);
				if (poss != std::string::npos)
				{
					txt = txt.substr(0, poss);
				}
			}

			// Transform string to value
			valD = jvx_string2Data(txt.c_str(), err);
			if (err)
			{
				std::cout << __FUNCTION__ << ":" << __LINE__ << ": Error when converting expression <" << txt << "> into a data value." << std::endl;
			}
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);

			valD = transformFrom(valD, optLabelShowTp, transformScale);

			if (granularity > 0.0)
			{
				valD = valD / granularity;
				valD = granularity * (jvxData)(JVX_DATA2INT64(valD));
			}

			val64 = JVX_DATA2INT64(valD);
			ptrFld = &val64;
			break;

		case JVX_DATAFORMAT_U64BIT_LE:

			// Remove possible unit part
			getDisplayUnitToken(optLabelShowTp, unit);
			if (!unit.empty())
			{
				size_t poss = txt.rfind(unit);
				if (poss != std::string::npos)
				{
					txt = txt.substr(0, poss);
				}
			}

			// Transform string to value
			valD = jvx_string2Data(txt.c_str(), err);
			if (err)
			{
				std::cout << __FUNCTION__ << ":" << __LINE__ << ": Error when converting expression <" << txt << "> into a data value." << std::endl;
			}
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);

			valD = transformFrom(valD, optLabelShowTp, transformScale);

			if (granularity > 0.0)
			{
				valD = valD / granularity;
				valD = granularity * (jvxData)(JVX_DATA2UINT64(valD));
			}

			valU64 = JVX_DATA2UINT64(valD);
			ptrFld = &valU64;
			break;

		case JVX_DATAFORMAT_DATA:

			// Remove possible unit part
			getDisplayUnitToken(optLabelShowTp, unit);
			if (!unit.empty())
			{
				size_t poss = txt.rfind(unit);
				if (poss != std::string::npos)
				{
					txt = txt.substr(0, poss);
				}
			}

			// Transform string to value
			valD = atof(txt.c_str());
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);

			valD = transformFrom(valD, optLabelShowTp, transformScale);

			if (granularity > 0.0)
			{
				valD = valD / granularity;
				valD = granularity * (jvxData)(JVX_DATA2INT64(valD));
			}

			ptrFld = &valD;
			break;
		case JVX_DATAFORMAT_STRING:

			// Add quotes
			//txt = "\"";
			txt = qtxt.toLatin1().data();
			//txt += "\"";

			fldStr.assign_const(txt.c_str(), txt.size());
			ptrFld = &fldStr;
			break;
		}
	}
	
	ident.reset(getSetTag.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);

	res = propRef->set_property(callGate,
		jPRG(ptrFld, 1, thePropDescriptor.format),
		ident,trans, jPSCH(&procId));
	if (res == JVX_NO_ERROR)
	{
		updateUiDescriptor(callGate.access_protocol);
		uiRefTp->setText("*set complete*"); // This must be overwritten by a get operation!

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__ , operDelayed);
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
#ifdef JVX_WIDGET_WRAPPER_FIELD_INFO_UPDATE			
		uiRefTp->setText("*update s*");
#endif
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__ , operDelayed);
		}
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

jvxErrorType
CjvxSaWrapperElementLineEdit::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, 
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD;
	QString qtxt = uiRefTp->text();
	jvxApiString fldStr;
	std::string txt;
	jvxHandle* ptrFld = NULL;
	std::string unit;
	jvxCallManagerProperties callGate;

	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);

	if (prop == NULL)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	if (myBaseProps.verbose_out)
	{
		std::cout << "QLineEdit::setPropertiesDirect() callback triggered for " << uiObjectName << std::endl;
	}

	if (
		(form == thePropDescriptor.format) &&
		(offset == 0) &&
		(numElements == 1))
	{
		switch (thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_SIZE:
		case JVX_DATAFORMAT_8BIT:
		case JVX_DATAFORMAT_16BIT_LE:
		case JVX_DATAFORMAT_32BIT_LE:
		case JVX_DATAFORMAT_64BIT_LE:
		case JVX_DATAFORMAT_U8BIT:
		case JVX_DATAFORMAT_U16BIT_LE:
		case JVX_DATAFORMAT_U32BIT_LE:
		case JVX_DATAFORMAT_U64BIT_LE:
			ptrFld = prop;
			break;
		case JVX_DATAFORMAT_DATA:
			valD = *((jvxData*)prop);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			valD = JVX_MAX(minVal, valD);
			valD = JVX_MIN(maxVal, valD);
			ptrFld = &valD;
			break;
		case JVX_DATAFORMAT_STRING:
			ptrFld = prop;
			break;
		}

		ident.reset(getSetTag.c_str());
		trans.reset(true, 0, thePropDescriptor.decTp, false);

		res = propRef->set_property(callGate,
			jPRG(ptrFld, 1, thePropDescriptor.format),
			ident, trans, jPSCH(&procId));
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
#ifdef JVX_WIDGET_WRAPPER_FIELD_INFO_UPDATE			
			uiRefTp->setText("*update sd*");
#endif
		}
		else
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
			}
			if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
			{
				jvxCBitField operDelayed = 0;
				jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT);
				handleAccessDelayed_stop(__FUNCTION__, operDelayed);
			}
			backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
				myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_SETTING;
	}
	return res;
}