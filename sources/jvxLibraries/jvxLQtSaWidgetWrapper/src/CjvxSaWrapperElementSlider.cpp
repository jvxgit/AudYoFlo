#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementSlider.h"
#include <QVariant>

CjvxSaWrapperElementSlider::CjvxSaWrapperElementSlider(QSlider* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, IjvxAccessProperties* propRefIn, std::string propertyGetSetTagLoc,
	std::vector<std::string> paramLstLoc, const std::string& tag_cp, const std::string& tag_key, const std::string& uiObjectNameLoc,
	const std::string& prefix, QWidget* crossRefWidget, jvxBool verboseLoc, jvxSize uId, IjvxHiddenInterface* hostRef) :
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn, propertyGetSetTagLoc,
		paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId, hostRef)
{
	uiRefTp = uiRefLoc;
	std::string rtu;
	myCrossRefWidget = crossRefWidget;
	tp = JVX_WW_SLIDER_VALUE_RANGE;

	deltaSteps = 1;
	numDigits = 2;
	optLabelShowTp = JVX_WW_SHOW_DIRECT;
	optLabelShow = NULL;
	optLabelName = "";
	optLabelUnit = "";
	optLabelUnitSet = false;
	optLabelOn = "On";
	optLabelOff = "Off";

	initializeUiElement();
}
	
void
CjvxSaWrapperElementSlider::initializeUiElement(jvxPropertyCallContext ccontext)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvxInt32 intVal = 0;
	jvxBool foundit = false;
	jvxData minValR = 0;
	jvxData maxValR = 0;
	jvxBool activateMove = false;

	if (widgetStatus == JVX_STATE_NONE)
	{
		minVal = uiRefTp->minimum();
		maxVal = uiRefTp->maximum();

		res = this->initParameters_getMin(&backwardRef->keymaps.slpba, paramLst);

		backwardRef->keymaps.slpba.getValueForKey("LABELREF", &optLabelName, JVX_WW_KEY_VALUE_TYPE_STRING);
		backwardRef->keymaps.slpba.getValueForKey("LABELON", &optLabelOn, JVX_WW_KEY_VALUE_TYPE_STRING);
		backwardRef->keymaps.slpba.getValueForKey("LABELOFF", &optLabelOff, JVX_WW_KEY_VALUE_TYPE_STRING);
		backwardRef->keymaps.slpba.getValueForKey("DELTASTEPS", &deltaSteps, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		if (foundit)
		{
			deltaSteps = JVX_MAX(deltaSteps, JVX_DATA_EPS);
		}
		backwardRef->keymaps.slpba.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32);
		backwardRef->keymaps.slpba.getValueForKey("MINVAL", &minValR, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		if (foundit)
		{
			minVal = minValR;
		}
		backwardRef->keymaps.slpba.getValueForKey("MAXVAL", &maxValR, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		if (foundit)
		{
			maxVal = maxValR;
		}
		backwardRef->keymaps.slpba.getValueForKey("LABELUNIT", &optLabelUnit, JVX_WW_KEY_VALUE_TYPE_STRING, &foundit);
		if (foundit)
		{
			optLabelUnitSet = true;
		}
		backwardRef->keymaps.slpba.getValueForKey("SCALEFAC", &transformScale, JVX_WW_KEY_VALUE_TYPE_DATA);
		backwardRef->keymaps.slpba.getValueForKey("LABELTRANSFORM", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
		if (foundit)
		{
			optLabelShowTp = (jvxWwTransformValueType)intVal;
		}
		backwardRef->keymaps.slpba.getValueForKey("ACTIVEMOVE", &activateMove, JVX_WW_KEY_VALUE_TYPE_BOOL);


		uiRefTp->setMinimum(0);
		uiRefTp->setMaximum(maxVal - minVal);

		if (!optLabelName.empty())
		{
			assert(myCrossRefWidget);
			QList<QWidget *> widgets = myCrossRefWidget->findChildren<QWidget *>();
			for (i = 0; i < widgets.count(); i++)
			{
				QString name = widgets[JVX_SIZE_INT(i)]->objectName();
				if (name.toLatin1().data() == optLabelName)
				{
					optLabelShow = qobject_cast<QLabel*>(widgets[JVX_SIZE_INT(i)]);
					break;
				}
			}
		}
	}

	res = initializeProperty_core(true, ccontext);
	if (res != JVX_NO_ERROR)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*not connected*");
	}

	// Evaluate the property descriptors
	if(widgetStatus == JVX_STATE_SELECTED)
	{
		switch(thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			tp = JVX_WW_SLIDER_VALUE_RANGE;
			break;
		case JVX_DATAFORMAT_DATA:
		case JVX_DATAFORMAT_64BIT_LE:
		case JVX_DATAFORMAT_32BIT_LE:
		case JVX_DATAFORMAT_16BIT_LE:
		case JVX_DATAFORMAT_8BIT:
			tp = JVX_WW_SLIDER_VALUE;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			if (thePropDescriptor.decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
			{
				tp = JVX_WW_SLIDER_SWITCH;
			}
			else
			{
				tp = JVX_WW_SLIDER_VALUE;
			}
			break;
		case JVX_DATAFORMAT_SELECTION_LIST:
			tp = JVX_WW_SLIDER_SWITCH;
			break;
		default:
			std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor 
				<< ": Property is not of correct type, expected JVX_DATAFORMAT_VALUE_IN_RANGE or a numeric type." << std::endl;
			widgetStatus = JVX_STATE_NONE;
			break;
		}

		uiRefTp->setProperty("mySaWidgetRefSlider", QVariant::fromValue<CjvxSaWrapperElementSlider*>(this));
		connect(uiRefTp, SIGNAL(sliderReleased()), this, SLOT(slider_released()));
		if (activateMove)
		{
			connect(uiRefTp, SIGNAL(sliderMoved()), this, SLOT(slider_moved()));
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
	else if (widgetStatus == JVX_STATE_INIT)
	{
		if (optLabelShow)
		{
			optLabelShow->setText("*update*");
		}
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
		if (optLabelShow)
		{
			optLabelShow->setText("*failed*");
		}
		mainWidgetDisable();
		uiRefTp->setToolTip("*failed*");
	}

	//verboseLoc = this->myBaseProps.verbose_out;
}

CjvxSaWrapperElementSlider::~CjvxSaWrapperElementSlider()
{
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		disconnect(uiRefTp, SIGNAL(sliderReleased()));
	}
	if (optLabelShow)
	{
		optLabelShow->setText("");
	}
	if (uiRefTp)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("");
	}
}
	
void
CjvxSaWrapperElementSlider::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
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
		/*
		mainWidgetEnable();
		if (descr.accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
		{
			QFont ft = uiRefTp->font();
			ft.setItalic(true);
			uiRefTp->setFont(ft);
			ttip_show = ttip_core + " - read only -";
		}
		else
		{
			QFont ft = uiRefTp->font();
			ft.setItalic(false);
			uiRefTp->setFont(ft);
			ttip_show = ttip_core;
		}
		uiRefTp->setToolTip(ttip_show.c_str());*/
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
CjvxSaWrapperElementSlider::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* globalId, jvxBool call_periodic_update)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt8 val8 = 0;
	jvxInt16 val16 = 0;
	jvxInt32 val32 = 0;
	jvxInt64 val64 = 0;
	jvxUInt8 valU8 = 0;
	jvxUInt16 valU16 = 0;
	jvxUInt32 valU32 = 0;
	jvxUInt64 valU64 = 0;
	jvxData valD;
	jvxData valDShow;
	jvxSize valS = JVX_SIZE_UNSELECTED;
	jvxValueInRange valRange;
	jvxHandle* ptrFld = NULL;
	std::string unit;
	jvxSelectionList selLst;
	selLst.bitFieldSelected() = 0;
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
	case JVX_DATAFORMAT_SELECTION_LIST:
		ptrFld = &selLst;
		break;
	}

	switch(tp)
	{
	case JVX_WW_SLIDER_VALUE:

		ident.reset(getSetTag.c_str());
		trans.reset(true, 0, thePropDescriptor.decTp);

		if (globalId)
		{
			res = propRef->get_property(callGate, jPRG(ptrFld, 1, thePropDescriptor.format), ident, trans, jPSCH(globalId));
		}
		else
		{
			res = propRef->get_property(callGate, jPRG(ptrFld, 1, thePropDescriptor.format), ident, trans, jPSCH(&procId));
		}

		if (res == JVX_NO_ERROR)
		{
			switch(thePropDescriptor.format)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				valD = (jvxData)val16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				valD = (jvxData)val32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				valD = (jvxData)val64;
				break;
			case JVX_DATAFORMAT_8BIT:
				valD = val8;
				break;
			case JVX_DATAFORMAT_U16BIT_LE:
				valD = (jvxData)valU16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				valD = (jvxData)valU32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				valD = (jvxData)valU64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				valD = valU8;
				break;
			}

			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			int idx = valD / deltaSteps;
			idx = JVX_MAX(idx, minVal);
			idx = JVX_MIN(idx, maxVal);
			uiRefTp->setValue(idx - minVal);
			idx = uiRefTp->value();
			idx += minVal;
			valD = idx * deltaSteps;
			if(optLabelUnitSet)
			{
				unit = optLabelUnit;
			}
			if(optLabelShow)
			{
				optLabelShow->setText((jvx_data2String(valD, JVX_SIZE_INT(numDigits)) + " " + unit).c_str());
			}
		}
		break;
	case JVX_WW_SLIDER_VALUE_RANGE:
		valRange.minVal = minVal;
		valRange.maxVal = maxVal;
		ident.reset(getSetTag.c_str());
		trans.reset(false, 0, thePropDescriptor.decTp);
		if (globalId)
		{
			res = propRef->get_property(callGate, jPRG(&valRange, 1, thePropDescriptor.format), ident, trans, jPSCH(globalId));
		}
		else
		{
			res = propRef->get_property(callGate, jPRG(&valRange, 1, thePropDescriptor.format), ident, trans, jPSCH(&procId));
		}
		if(res == JVX_NO_ERROR)
		{
			mainWidgetEnable();
			int idx;
			valD = transformTo(valRange.minVal, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);		
			idx = (valD / deltaSteps);
			minVal = idx;

			valD = transformTo(valRange.maxVal, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			idx = (valD / deltaSteps);
			maxVal = idx;

			uiRefTp->setMaximum(maxVal - minVal);
			uiRefTp->setMinimum(0);
		
			valD = valRange.val();
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			idx =  valD/ deltaSteps;

			idx = JVX_MAX(idx, minVal);
			idx = JVX_MIN(idx, maxVal);
			idx -= minVal;
			uiRefTp->setValue(idx);

			idx = uiRefTp->value();
			idx += minVal;
			valDShow = idx;			
			valDShow *= deltaSteps;

			if(optLabelUnitSet)
			{
				unit = optLabelUnit;
			}

			if(optLabelShow)
			{
				optLabelShow->setText((jvx_data2String(valDShow, JVX_SIZE_INT(numDigits)) + " " + unit).c_str());
			}			
		}
		
		
		break;
	case JVX_WW_SLIDER_SWITCH:
		ident.reset(getSetTag.c_str());
		trans.reset(false, 0, thePropDescriptor.decTp);
		if (globalId)
		{
			res = propRef->get_property(callGate, jPRG(ptrFld, 1, thePropDescriptor.format), ident, trans, jPSCH(globalId));
		}
		else
		{
			res = propRef->get_property(callGate, jPRG(ptrFld, 1, thePropDescriptor.format), ident, trans, jPSCH(&procId));
		}
		if (res == JVX_NO_ERROR)
		{
			mainWidgetEnable();
			switch (thePropDescriptor.format)
			{
			case JVX_DATAFORMAT_BOOL:
				uiRefTp->setMaximum(1);
				uiRefTp->setMinimum(0);
				if (valU16 == c_false)
				{
					uiRefTp->setValue(0);
					if (optLabelShow)
					{
						optLabelShow->setText(optLabelOff.c_str());
					}
				}
				else
				{
					uiRefTp->setValue(1);
					if (optLabelShow)
					{
						optLabelShow->setText(optLabelOn.c_str());
					}
				}
				break;

			case JVX_DATAFORMAT_SELECTION_LIST:

				uiRefTp->setMaximum(selLst.strList.ll() - 1);
				uiRefTp->setMinimum(0);
				valS = jvx_bitfieldSelection2Id(selLst.bitFieldSelected(), selLst.strList.ll());
				if (JVX_CHECK_SIZE_SELECTED(valS))
				{
					uiRefTp->setValue((int)valS);
					if (optLabelShow)
					{
						optLabelShow->setText(selLst.strList.std_str_at(valS).c_str());
					}
				}
				else
				{
					optLabelShow->setText("invalid");
				}
				break;
			default:
				assert(0);
			}
		}
		
		break;
	}

	if (res == JVX_NO_ERROR)
	{
		// Handled before
		mainWidgetEnable();
		uiRefTp->setToolTip(ttip_show.c_str());

		updateUiDescriptor(callGate.access_protocol);

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__, operDelayed);
		}
		if (myBaseProps.report_get)
		{
			backwardRef->reportPropertyGet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), ptrFld, 0, 1, thePropDescriptor.format, static_cast<CjvxSaWrapperElementBase*>(this), res);
		}
	}
	else if (res == JVX_ERROR_POSTPONE)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*update*");
		if (optLabelShow)
		{
			optLabelShow->setText("*update*");
		}
	}
	else
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*failed*");
		if (optLabelShow)
		{
			optLabelShow->setText("*failed*");
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
CjvxSaWrapperElementSlider::slider_released()
{
	setProperties_core_ui();
}

void
CjvxSaWrapperElementSlider::slider_moved()
{
	// setProperties_core_ui(); <- need this
}

void
CjvxSaWrapperElementSlider::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD;
	jvxInt32 idx;
	jvxValueInRange valR;
	jvxInt8 val8 = 0;
	jvxInt16 val16 = 0;
	jvxInt32 val32 = 0;
	jvxInt64 val64 = 0;
	jvxUInt8 valU8 = 0;
	jvxUInt16 valU16 = 0;
	jvxUInt32 valU32 = 0;
	jvxUInt64 valU64 = 0;
	jvxCBool valB = c_false;
	jvxSelectionList selLst;
	jvxHandle* ptrFld = NULL;
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
	if(myBaseProps.verbose_out)
	{
		std::cout << "QSlider::sliderReleased() callback triggered for " << uiObjectName << std::endl;
	}

	ident.reset(getSetTag.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);

	switch (tp)
	{
	case JVX_WW_SLIDER_VALUE:
	case JVX_WW_SLIDER_VALUE_RANGE:
		idx = uiRefTp->value();
		idx += minVal;
		valD = idx;
		valD *= deltaSteps;
		valD = transformFrom(valD, optLabelShowTp, transformScale);

		switch (thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_8BIT:
			val8 = (jvxInt8)valD;
			ptrFld = &val8;
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			val16 = (jvxInt16)valD;
			ptrFld = &val16;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			val32 = (jvxInt32)valD;
			ptrFld = &val32;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			val64 = (jvxInt64)valD;
			ptrFld = &val64;
			break;
		case JVX_DATAFORMAT_U8BIT:
			valU8 = (jvxUInt8)valD;
			ptrFld = &valU8;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			valU16 = (jvxUInt16)valD;
			ptrFld = &valU16;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			valU32 = (jvxUInt32)valD;
			ptrFld = &valU32;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			valU64 = (jvxUInt64)valD;
			ptrFld = &valU64;
			break;
		case JVX_DATAFORMAT_DATA:
			ptrFld = &valD;
			break;
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			valR.val() = valD;
			ptrFld = &valR;
			break;
		}

		res = propRef->set_property(callGate, 
			jPRG(ptrFld, 1, thePropDescriptor.format),
			ident, trans, jPSCH(&procId));
		break;
	case JVX_WW_SLIDER_SWITCH:
		switch (thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_BOOL:
			valB = uiRefTp->value();
			ptrFld = &valB;
			res = propRef->set_property(callGate,
				jPRIO(valB),
				ident, trans, jPSCH(&procId));
			break;
		case JVX_DATAFORMAT_SELECTION_LIST:
			val16 = uiRefTp->value();
			selLst.bitFieldSelected() = 0;
			selLst.bitFieldExclusive = 0;
			jvx_bitSet(selLst.bitFieldSelected(), val16);
			res = propRef->set_property(callGate,
				jPRG(ptrFld, 1, thePropDescriptor.format),
				ident, trans, jPSCH(&procId));
			break;
		}
		break;
	default:
		assert(0);
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
CjvxSaWrapperElementSlider::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD;
	jvxInt32 idx;
	jvxValueInRange valR;
	jvxInt8 val8;
	jvxInt16 val16;
	jvxInt32 val32;
	jvxInt64 val64;
	jvxSelectionList selLst;
	jvxHandle* ptrFld = NULL;
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

	if (
		(form == thePropDescriptor.format) &&
		(offset == 0) &&
		(numElements == 1))
	{

		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << " callback triggered for " << uiObjectName << std::endl;
		}

		ident.reset(getSetTag.c_str());
		trans.reset(true, 0, thePropDescriptor.decTp, false);

		switch (tp)
		{
		case JVX_WW_SLIDER_VALUE:
		case JVX_WW_SLIDER_VALUE_RANGE:
			res = propRef->set_property(callGate,
				jPRG(ptrFld, 1, thePropDescriptor.format),
				ident, trans, jPSCH(&procId));
			break;
		case JVX_WW_SLIDER_SWITCH:

			res = propRef->set_property(callGate, jPRG(ptrFld, 1, thePropDescriptor.format), ident, trans, jPSCH(&procId));
			break;
		default:
			assert(0);
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
	}
	else
	{
		res = JVX_ERROR_INVALID_SETTING;
	}
	return res;
}
