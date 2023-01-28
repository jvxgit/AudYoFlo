#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementProgressBar.h"
#include <QVariant>

CjvxSaWrapperElementProgressBar::CjvxSaWrapperElementProgressBar(QProgressBar* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc,
	IjvxAccessProperties* propRefIn,
	std::string propertyGetSetTagLoc, std::vector<std::string> paramLstLoc, const std::string& tag_cp, const std::string& tag_key,
	const std::string& uiObjectNameLoc, const std::string& prefix, QWidget* crossRefWidget, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef) :
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn, propertyGetSetTagLoc,
		paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId, hostRef)
{
	uiRefTp = uiRefLoc;
	myCrossRefWidget = crossRefWidget;
	initializeUiElement();
}

void 
CjvxSaWrapperElementProgressBar::initializeUiElement(jvxPropertyCallContext ccontext)
{
	jvxSize i;
	jvxBool foundit = false;
	jvxInt32 intVal = 0;
	initializeProperty_core(true,  ccontext);

	if (widgetStatus == JVX_STATE_SELECTED)
	{
		deltaSteps = 1;
		numDigits = 2;
		minVal = uiRefTp->minimum();
		maxVal = uiRefTp->maximum();
		optLabelShowTp = JVX_WW_SHOW_DIRECT;
		optLabelShow = NULL;
		optLabelName = "";
		optLabelUnit = "";
		optLabelUnitSet = false;

		switch(thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_DATA:
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			break;
		default:
			std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor 
				<< ": Property is not of correct type, expected JVX_DATAFORMAT_VALUE_IN_RANGE or JVX_DATAFORMAT_DATA." << std::endl;
			widgetStatus = JVX_STATE_NONE;
			break;
		}
	}
	if (widgetStatus == JVX_STATE_SELECTED)
	{

		jvxErrorType res = this->initParameters_getMin(&backwardRef->keymaps.slpba, paramLst);
		assert(res == JVX_NO_ERROR);

		backwardRef->keymaps.slpba.getValueForKey("DELTASTEPS", &deltaSteps, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		if (foundit)
		{
			deltaSteps = JVX_MAX(deltaSteps, JVX_DATA_EPS);
		}
		backwardRef->keymaps.slpba.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32);
		backwardRef->keymaps.slpba.getValueForKey("MINVAL", &minVal, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		backwardRef->keymaps.slpba.getValueForKey("MAXVAL", &maxVal, JVX_WW_KEY_VALUE_TYPE_DATA, &foundit);
		backwardRef->keymaps.slpba.getValueForKey("LABELREF", &optLabelName, JVX_WW_KEY_VALUE_TYPE_STRING);
		backwardRef->keymaps.slpba.getValueForKey("LABELUNIT", &optLabelUnit, JVX_WW_KEY_VALUE_TYPE_STRING, &foundit);

		contentOnly = false;
		backwardRef->keymaps.slpba.getValueForKey("CONTENT_ONLY", &contentOnly, JVX_WW_KEY_VALUE_TYPE_BOOL, &foundit);		

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

		uiRefTp->setMaximum(maxVal-minVal);
		uiRefTp->setMinimum(0);
			
		if(!optLabelName.empty())
		{
			assert(myCrossRefWidget);
			QList<QWidget *> widgets = myCrossRefWidget->findChildren<QWidget *>();
			for(i = 0; i < widgets.count(); i++)
			{
				QString name = widgets[JVX_SIZE_INT(i)]->objectName();
				if(name.toLatin1().data() == optLabelName)
				{
					optLabelShow = qobject_cast<QLabel*>(widgets[JVX_SIZE_INT(i)]);
					break;
				}
			}
		}
		uiRefTp->setProperty("mySaWidgetRefProgressBar", QVariant::fromValue<CjvxSaWrapperElementProgressBar*>(this));
		
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

CjvxSaWrapperElementProgressBar::~CjvxSaWrapperElementProgressBar()
{
	if (widgetStatus == JVX_STATE_SELECTED)
	{
	}
}
	
void
CjvxSaWrapperElementProgressBar::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
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
CjvxSaWrapperElementProgressBar::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* passedId, jvxBool call_periodic_update)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD;
	jvxValueInRange valVR;
	jvxHandle* ptrFld = NULL;
	std::string unit;
	std::string getSetTag = propertyGetSetTag;
	jvxBool isValid = false;
	jvxCallManagerProperties callGate;
	callGate.on_get.prop_access_type = &thePropDescriptor.accessType;

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
	case JVX_DATAFORMAT_VALUE_IN_RANGE:
		ptrFld = &valVR;
		break;
	}

	ident.reset(getSetTag.c_str());
	trans.reset(contentOnly, 0, thePropDescriptor.decTp);
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
		int idx = 0;
		switch (thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			minVal = valVR.minVal;
			maxVal = valVR.maxVal;
			uiRefTp->setMinimum(minVal);
			uiRefTp->setMaximum(maxVal / deltaSteps);
			valD = valVR.val();
			// No break here!
		case JVX_DATAFORMAT_DATA:
			valD = transformTo(valD, optLabelShowTp, transformScale);
			getDisplayUnitToken(optLabelShowTp, unit);
			idx = valD / deltaSteps;
			idx = JVX_MAX(idx, minVal);
			idx = JVX_MIN(idx, maxVal);
			break;
		}

		uiRefTp->setValue(idx - minVal);
		idx = uiRefTp->value();
		idx += minVal;

		valD = idx * deltaSteps;
		if (optLabelUnitSet)
		{
			unit = optLabelUnit;
		}
		if (optLabelShow)
		{
			optLabelShow->setText((jvx_data2String(valD, JVX_SIZE_INT(numDigits)) + " " + unit).c_str());
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
	else if (res == JVX_ERROR_POSTPONE)
	{
		// No idea how to show..
	}
	else
	{
		mainWidgetDisable();
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
CjvxSaWrapperElementProgressBar::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	assert(0);
}

jvxErrorType
CjvxSaWrapperElementProgressBar::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	return JVX_ERROR_UNSUPPORTED;
}