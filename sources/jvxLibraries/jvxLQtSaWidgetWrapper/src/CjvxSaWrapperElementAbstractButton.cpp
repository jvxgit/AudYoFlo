#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementAbstractButton.h"
#include <QVariant>

CjvxSaWrapperElementAbstractButton::CjvxSaWrapperElementAbstractButton(QAbstractButton* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc,
	IjvxAccessProperties* propRefIn, std::string propertyGetSetTagLoc, std::vector<std::string> paramLstLoc, const std::string& tag_cp, const std::string& tag_key,
	const std::string& uiObjectNameLoc, const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef) :
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn, propertyGetSetTagLoc,
		paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId, hostRef)
{
	uiRefTp = uiRefLoc;
	// Evaluate the property descriptors
	initializeUiElement();
}

void
CjvxSaWrapperElementAbstractButton::initializeUiElement(jvxPropertyCallContext ccontext)
{
	std::string rtu;
	jvxBool foundit = false;
	jvxInt32 intVal = 0;
	jvxErrorType res = JVX_NO_ERROR;
	if (widgetStatus == JVX_STATE_NONE)
	{
		jvxErrorType res = this->initParameters_getMin(&backwardRef->keymaps.cb, paramLst);
		assert(res == JVX_NO_ERROR);

		uiRefTp->setProperty("mySaWidgetRefCheckBox", QVariant::fromValue<CjvxSaWrapperElementAbstractButton*>(this));
	}

	res = initializeProperty_core(true, ccontext);
	if (res != JVX_NO_ERROR)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*not connected*");
	}
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		if(
			(thePropDescriptor.format == JVX_DATAFORMAT_BOOL) &&
			(thePropDescriptor.decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF))
		{
			tp = JVX_WW_CHECKBOX_CBOOL;
		}
		else if(
			thePropDescriptor.format == JVX_DATAFORMAT_SELECTION_LIST
				)
		{
			tp = JVX_WW_CHECKBOX_SELLIST;
		}
		else
		{
			std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor << ": Incorrect type, expected JVX_DATAFORMAT_BOOL + JVX_PROPERTY_DECODER_SIMPLE_ONOFF or JVX_DATAFORMAT_SELECTION_LIST" << std::endl;
			widgetStatus = JVX_STATE_NONE;
		}

		connect(uiRefTp, SIGNAL(clicked(bool)), this, SLOT(checkbox_toggled(bool)));

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

CjvxSaWrapperElementAbstractButton::~CjvxSaWrapperElementAbstractButton()
{
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		disconnect(uiRefTp, SIGNAL(clicked(bool)));
	}
	if(uiRefTp)
		mainWidgetDisable();
}

void 
CjvxSaWrapperElementAbstractButton::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
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
		std::cout << __FUNCTION__ << ": Failed to get status update for object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
	}
}

jvxErrorType 
CjvxSaWrapperElementAbstractButton::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* passedId, jvxBool call_periodic_update)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	std::string getSetTag = propertyGetSetTag;
	jvxBool condSet = false;
	jvxCallManagerProperties callGate;
	callGate.on_get.prop_access_type = &thePropDescriptor.accessType;

	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	
	if (widgetStatus != JVX_STATE_SELECTED)
	{
		return JVX_ERROR_NOT_READY;
	}

	ident.reset(getSetTag.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp);

	switch(tp)
	{
	case JVX_WW_CHECKBOX_CBOOL:
		if (passedId)
		{
			res = propRef->get_property(callGate, jPRIO<jvxCBool>(boolVar), ident, trans, jPSCH(passedId));
		}
		else
		{
			res = propRef->get_property(callGate, jPRIO<jvxCBool>(boolVar), ident, trans, jPSCH(&procId));

		}
		condSet = (boolVar != c_false);

		break;
	case JVX_WW_CHECKBOX_SELLIST:
		if (passedId)
		{
			res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans, jPSCH(passedId));
		}
		else
		{
			res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans, jPSCH(&procId));
		}
		condSet = jvx_bitTest(selLst.bitFieldSelected(), 0);
		break;
	}
	if (res == JVX_NO_ERROR)
	{		
		uiRefTp->setChecked(condSet);
		
		updateUiDescriptor(callGate.access_protocol);

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			jvxCBitField operDelayed = 0;
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT);
			handleAccessDelayed_stop(__FUNCTION__, operDelayed);
		}

		if (myBaseProps.report_get)
		{
			switch (tp)
			{
			case JVX_WW_CHECKBOX_CBOOL:
				backwardRef->reportPropertyGet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), &boolVar, 0, 1, thePropDescriptor.format,
					static_cast<CjvxSaWrapperElementBase*>(this), res);
				break;
			case JVX_WW_CHECKBOX_SELLIST:
				backwardRef->reportPropertyGet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), &selLst, 0, 1, thePropDescriptor.format,
					static_cast<CjvxSaWrapperElementBase*>(this), res);
				break;
			}
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
CjvxSaWrapperElementAbstractButton::checkbox_toggled(bool toggle)
{
	setProperties_core_ui();
}

void
CjvxSaWrapperElementAbstractButton::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	selLst.bitFieldSelected() = 0;
	selLst.bitFieldExclusive = 0;
	std::string getSetTag = propertyGetSetTag;
	jvxCallManagerProperties callGate;
	bool toggle = uiRefTp->isChecked();
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	if(myBaseProps.verbose_out)
	{
		std::cout << "QCheckBox::clicked() callback triggered for " << uiObjectName << std::endl;
	}
	switch(tp)
	{
	case JVX_WW_CHECKBOX_CBOOL:
		if(toggle)
		{
			boolVar = c_true;
		}
		ident.reset(getSetTag.c_str());
		trans.reset(true, 0, thePropDescriptor.decTp, false);
		res = propRef->set_property(callGate, jPRIO<jvxCBool>(boolVar), ident, trans, jPSCH(&procId));
		break;
	case JVX_WW_CHECKBOX_SELLIST:
		if(toggle)
		{
			// In a selectionlist, option "0" is related to a "true"
			jvx_bitZSet(selLst.bitFieldSelected(), 0);
		}
		else
		{
			// In a selectionlist, option "1" is related to a "false"
			jvx_bitZSet(selLst.bitFieldSelected(), 1);
		}
		ident.reset(getSetTag.c_str());
		trans.reset(true, 0, thePropDescriptor.decTp, false);
		res = propRef->set_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans, jPSCH(&procId));
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
	}
	else
	{
		if(myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(),
			myBaseProps.group_id_emit, myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

jvxErrorType
CjvxSaWrapperElementAbstractButton::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	std::string getSetTag = propertyGetSetTag;
	jvxBool condSet = false;
	jvxBool isValid = false;
	jvxCallManagerProperties callGate;

	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);

	if (
		(form == thePropDescriptor.format) &&
		(offset == 0) &&
		(numElements == 1))
	{
		ident.reset(getSetTag.c_str());
		trans.reset(true, offset, thePropDescriptor.decTp, false);
		res = propRef->set_property(callGate, jPRG(prop, numElements, form), ident, trans, jPSCH(&procId));

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
			if (myBaseProps.verbose_out)
			{
				std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
			}
			backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(),
				myBaseProps.group_id_emit, myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_SETTING;
	}
	return res;
}

/*
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	selLst.bitFieldSelected = 0;
	selLst.bitFieldExclusive = 0;
	std::string getSetTag = propertyGetSetTag;
	jvxCallManagerProperties callGate;
	bool toggle = uiRefTp->isChecked();
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	* /
getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
if (myBaseProps.verbose_out)
{
	std::cout << "QCheckBox::clicked() callback triggered for " << uiObjectName << std::endl;
}

if (res == JVX_NO_ERROR)
{
	updateUiDescriptor(accProt);

	if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
	{
		jvxCBitField operDelayed = 0;
		jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT);
		handleAccessDelayed_stop(__FUNCTION__, operDelayed);
	}

	if (backwardRef)
	{
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id,
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
	if (myBaseProps.verbose_out)
	{
		std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
	}
	backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(),
		myBaseProps.group_id, myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
}
*/