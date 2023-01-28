#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementLabel.h"
#include <QVariant>
#include "jvx-qt-helpers.h"

typedef struct
{
	const char* name;
	QColor col;
} oneColorDefine;

#define JVX_NUM_COLORS_LABEL 9
static oneColorDefine allColors[JVX_NUM_COLORS_LABEL] =
{
	{"green", Qt::green},
	{ "red", Qt::red },
	{ "yellow", Qt::yellow },
	{ "blue", Qt::blue },
	{ "cyan", Qt::cyan },
	{ "black", Qt::black},
	{ "gray", Qt::gray },
	{ "lgray", Qt::lightGray},
	{ "magenta", Qt::magenta }
};

CjvxSaWrapperElementLabel::CjvxSaWrapperElementLabel(QLabel* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc,
	IjvxAccessProperties* propRefIn, std::string propertyGetSetTagLoc,
	std::vector<std::string> paramLstLoc, const std::string& tag_cp, const std::string& tag_key, const std::string& uiObjectNameLoc,
	const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef) :
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn, propertyGetSetTagLoc,
		paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId, hostRef)
{
	jvxSize i;
	uiRefTp = uiRefLoc;
	std::string rtu;
	tp = JVX_WW_LABEL_TEXT;
	initializeUiElement();
}

void
CjvxSaWrapperElementLabel::initializeUiElement(jvxPropertyCallContext ccontext)
{
	jvxSize i,j;
	jvxBool foundit = false;
	jvxInt32 intVal = 0;
	jvxErrorType res = JVX_NO_ERROR;

	if (widgetStatus == JVX_STATE_NONE)
	{
		jvxErrorType res = this->initParameters_getMin(&backwardRef->keymaps.la, paramLst);
		assert(res == JVX_NO_ERROR);

		minVal = JVX_DATA_MAX_NEG;
		maxVal = JVX_DATA_MAX_POS;
		optLabelShowTp = JVX_WW_SHOW_DIRECT;
		numDigits = 2;
		tp = JVX_WW_LABEL_TEXT;

		backwardRef->keymaps.la.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32);
		// backwardRef->keymaps.la.getValueForKey("LABELUNIT", &unit, JVX_WW_KEY_VALUE_TYPE_STRING, &foundit);
		backwardRef->keymaps.la.getValueForKey("MINVAL", &minVal, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		backwardRef->keymaps.la.getValueForKey("MAXVAL", &maxVal, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		backwardRef->keymaps.la.getValueForKey("SCALEFAC", &transformScale, JVX_WW_KEY_VALUE_TYPE_DATA);
		backwardRef->keymaps.la.getValueForKey("LABELTRANSFORM", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
		if (foundit)
		{
			optLabelShowTp = (jvxWwTransformValueType)intVal;
		}
		backwardRef->keymaps.la.getValueForKey("BEHAVIOR", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
		if (foundit)
		{
			tp = (jvxWwConnectionType_la)intVal;
		}
		backwardRef->keymaps.la.getValueForKey("COLORS", &colorNames, JVX_WW_KEY_VALUE_TYPE_STRING, &foundit);
		if (foundit)
		{
			std::vector<std::string> lst;
			// std::cout << "Color names = <" << colorNames << ">." << std::endl;
			jvx_parseStringListIntoTokens(colorNames, lst, '-');
			for (i = 0; i < lst.size(); i++)
			{
				jvxBool foundit = false;
				for (j = 0; j < JVX_NUM_COLORS_LABEL; j++)
				{
					if (lst[i] == allColors[j].name)
					{
						colorList.push_back(allColors[j].col);
						foundit = true;
						break;
					}
				}
				if (!foundit)
				{
					std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor
						<< ": Unknown color format <" << lst[i] << "> for label." << std::endl;
				}
			}
		}

	}

	res = initializeProperty_core(true, ccontext);
	if (res != JVX_NO_ERROR)
	{
		mainWidgetDisable();
		uiRefTp->setText("*not connected*");
	}

	// Evaluate the property descriptors
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		switch(thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_SELECTION_LIST:
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
			break;
		default:
			std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor 
				<< ": Incorrect type, expected JVX_DATAFORMAT_STRING, JVX_DATAFORMAT_SELECTION_LIST, JVX_DATAFORMAT_DATA, JVX_DATAFORMAT_64BIT_LE, JVX_DATAFORMAT_32BIT_LE, JVX_DATAFORMAT_16BIT_LE or JVX_DATAFORMAT_8BIT." << std::endl;
			widgetStatus = JVX_STATE_NONE;
			break;
		}
			
		uiRefTp->setProperty("mySaWidgetRefLabel", QVariant::fromValue<CjvxSaWrapperElementLabel*>(this));

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

CjvxSaWrapperElementLabel::~CjvxSaWrapperElementLabel()
{
	if(uiRefTp) 
		uiRefTp->setText("");
}

void
CjvxSaWrapperElementLabel::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
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
CjvxSaWrapperElementLabel::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* globalId, jvxBool call_periodic_update)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt8 val8 = 0;
	jvxInt16 val16 = 0;
	jvxInt32 val32 = 0;
	jvxInt64 val64 = 0;
	jvxSize valSz = 0;
	jvxData valD = 0;
	jvxUInt8 valU8 = 0;
	jvxUInt16 valU16 = 0;
	jvxUInt32 valU32 = 0;
	jvxUInt64 valU64 = 0;
	
	jvxSelectionList slst;
	jvxHandle* ptrFld = NULL;
	jvxApiString  str;
	std::string txt;
	jvxBool contentOnly = true;
	std::string unit;
	jvxBool noText = false;
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
		ptrFld = &valSz;
		break;
	case JVX_DATAFORMAT_STRING:
		ptrFld = &str;
		break;
	case JVX_DATAFORMAT_SELECTION_LIST:
		ptrFld = &slst;
		break;
	}

	ident.reset(getSetTag.c_str());
	trans.reset(contentOnly, 0, thePropDescriptor.decTp);
	if (globalId)
	{
		res = propRef->get_property(callGate, jPRG(ptrFld, 1, thePropDescriptor.format), ident, trans);
	}
	else
	{
		res = propRef->get_property(callGate, jPRG(ptrFld, 1, thePropDescriptor.format),ident, trans, jPSCH(&procId));
	}

#if 0
	if (thePropDescriptor.format == JVX_DATAFORMAT_16BIT_LE)
	{
		if (thePropDescriptor.decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
		{
			if (
				(tp == JVX_WW_LABEL_HIDE_ON_TRUE) ||
				(tp == JVX_WW_LABEL_GREEN_GRAY) ||
				(tp == JVX_WW_LABEL_CUSTOM_COLOR))
			{
				noText = true;
			}
		}
	}
	if (thePropDescriptor.format == JVX_DATAFORMAT_SELECTION_LIST)
	{
		if (thePropDescriptor.decTp == JVX_PROPERTY_DECODER_SINGLE_SELECTION)
		{
			if (
				(tp == JVX_WW_LABEL_HIDE_ON_TRUE) ||
				(tp == JVX_WW_LABEL_GREEN_GRAY) ||
				(tp == JVX_WW_LABEL_CUSTOM_COLOR))
			{
				noText = true;
			}
		}
	}
#endif

	if (res == JVX_NO_ERROR)
	{ 
		
		switch(thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_SELECTION_LIST:
			if (tp == JVX_WW_LABEL_CUSTOM_COLOR)
			{
				jvxSize selId = jvx_bitfieldSelection2Id(
					slst.bitFieldSelected(), slst.strList.ll());
				noText = true;
				QColor col = Qt::gray;
				if (selId < colorList.size())
				{
					col = colorList[selId];
				}
				setBackgroundLabelColor(col, uiRefTp);
			}
			else
			{
				noText = false;
				txt = jvx_bitField2String(slst.bitFieldSelected());
			}
			break;
		case JVX_DATAFORMAT_8BIT:
			valD = JVX_INT32_DATA(val8);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;

		case JVX_DATAFORMAT_U8BIT:
			valD = JVX_INT32_DATA(valU8);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;

		case JVX_DATAFORMAT_16BIT_LE:
			valD = JVX_INT32_DATA(val16);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;

		case JVX_DATAFORMAT_U16BIT_LE:
			if (thePropDescriptor.decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
			{
				noText = true;
				txt = "";
				if (tp == JVX_WW_LABEL_HIDE_ON_TRUE)
				{

					if (valU16 == c_false)
					{
						uiRefTp->show();
					}
					else
					{
						uiRefTp->hide();
					}
				}
				else if (tp == JVX_WW_LABEL_GREEN_GRAY)
				{
					noText = true;
					if (valU16 == c_true)
					{
						setBackgroundLabelColor(Qt::green, uiRefTp);
					}
					else
					{
						setBackgroundLabelColor(Qt::gray, uiRefTp);
					}
				}
				else if (tp == JVX_WW_LABEL_ALERT)
				{
					noText = true;
					if (valU16 == c_true)
					{
						setBackgroundLabelColor(Qt::red, uiRefTp);
					}
					else
					{
						setBackgroundLabelColor(Qt::green, uiRefTp);
					}
				}
				else if (tp == JVX_WW_LABEL_CUSTOM_COLOR)
				{
					noText = true;
					QColor col = Qt::gray;
					if (valU16 < colorList.size())
					{
						col = colorList[val16];
					}
					setBackgroundLabelColor(col, uiRefTp);
				}
				else if (tp == JVX_WW_LABEL_TEXT)
				{
					if (valU16 == c_false)
					{
						txt = "off";
					}
					else
					{
						txt = "on";
					}
				}
			}
			else
			{
				valD = JVX_INT32_DATA(valU16);
				valD = transformTo(valD, optLabelShowTp, transformScale);
				getDisplayUnitToken(optLabelShowTp, unit);
				txt = jvx_data2String(valD, numDigits) + unit;
				break;
			}
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			valD = JVX_INT32_DATA(val32);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			valD = JVX_UINT32_DATA(valU32);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;

		case JVX_DATAFORMAT_64BIT_LE:
			valD = JVX_INT64_DATA(val64);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			valD = JVX_UINT64_DATA(valU64);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_DATA:
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_SIZE:
			valD = JVX_SIZE_DATA(valSz);
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			txt = jvx_data2String(valD, numDigits) + unit;
			break;
		case JVX_DATAFORMAT_STRING:
			txt = "ERROR";
			txt = str.std_str();
			break;
		}

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

	if (res == JVX_NO_ERROR)
	{
		mainWidgetEnable();
		if (noText)
		{
			uiRefTp->setToolTip(txt.c_str());
			uiRefTp->setText("");
		}
		else
		{
			uiRefTp->setText(txt.c_str());
		}
	}
	else if (res == JVX_ERROR_POSTPONE)
	{
		mainWidgetDisable();
		if (noText)
		{
			uiRefTp->setToolTip("*update*");
		}
		else
		{
			uiRefTp->setText("*update*");
		}
	}
	else
	{
		mainWidgetDisable();
		if (noText)
		{
			uiRefTp->setToolTip("*failed*");
		}
		else
		{
			uiRefTp->setText("*failed*");
		}

		if(res != JVX_ERROR_WRONG_STATE)
		{
			if(myBaseProps.verbose_out)
			{
				std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
			}
		}
	}
	return JVX_NO_ERROR;
}

void
CjvxSaWrapperElementLabel::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	// In a label, there is no "set"
	assert(0);
}

jvxErrorType
CjvxSaWrapperElementLabel::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD;
	jvxInt8 val8;
	jvxInt16 val16;
	jvxInt32 val32;
	jvxInt64 val64;
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
		case JVX_DATAFORMAT_8BIT:
		case JVX_DATAFORMAT_16BIT_LE:
		case JVX_DATAFORMAT_32BIT_LE:
		case JVX_DATAFORMAT_64BIT_LE:
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
			backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
				myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
			updateUiDescriptor(callGate.access_protocol);
		}
		else if (res == JVX_ERROR_POSTPONE)
		{
			mainWidgetDisable();
			uiRefTp->setToolTip("*update*");
		}
		else
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
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
