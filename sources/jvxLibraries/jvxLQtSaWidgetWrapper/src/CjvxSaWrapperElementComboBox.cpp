#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementComboBox.h"
#include <QVariant>

CjvxSaWrapperElementComboBox::CjvxSaWrapperElementComboBox(QComboBox* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc,
	IjvxAccessProperties* propRefIn, std::string propertyGetSetTagLoc,
	std::vector<std::string> paramLstLoc,
	const std::string& tag_cp, const std::string& tag_key,
	const std::string& uiObjectNameLoc,
	const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef) :
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn,
		propertyGetSetTagLoc, paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId,
		hostRef)
{
	jvxSize i;
	uiRefTp = uiRefLoc;
	tp = JVX_WW_COMBOBOX_READ_ONCE;
	initialSelectionRead = false;
	std::string rtu;
	initializeUiElement();
}

void
CjvxSaWrapperElementComboBox::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
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

void
CjvxSaWrapperElementComboBox::initializeUiElement(jvxPropertyCallContext ccontext)
{
	jvxInt32 intVal;
	jvxErrorType res = JVX_NO_ERROR;
	// Evaluate the property descriptors
	if (widgetStatus == JVX_STATE_NONE)
	{
		jvxErrorType res = this->initParameters_getMin(&backwardRef->keymaps.cb, paramLst);
		assert(res == JVX_NO_ERROR);

		backwardRef->keymaps.cb.getValueForKey("BEHAVIOR", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM);
		tp = (jvxWwConnectionType_cb)intVal;
	}

	res = initializeProperty_core(true, ccontext);
	if (res != JVX_NO_ERROR)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*not connected*");
	}
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		jvxSize i;
		jvxCBool boolVar = false;
		jvxErrorType res = JVX_NO_ERROR;
		jvxSelectionList selLst;
		std::string getSetTag = propertyGetSetTag;
		getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
		/*QString qstr = uiRef->accessibleName();
		if (!qstr.isEmpty())
		{
			getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
		}
		*/


		/*
		res = propRef->get_property(callGate, jPRG( &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, false, getSetTag.c_str());
		if(res == JVX_NO_ERROR)
		{
		*/
		if (
			thePropDescriptor.format != JVX_DATAFORMAT_SELECTION_LIST
			)
		{
			std::cout << "--> Properety is not of correct type, expected JVX_DATAFORMAT_SELECTION_LIST" << std::endl;
			widgetStatus = JVX_STATE_NONE;
			mainWidgetDisable();
			uiRefTp->clear();
			uiRefTp->addItem("*error*");
			uiRefTp->setCurrentIndex(0);
		}


		/*else
			{
				uiRefTp->clear();
				for (i = 0; i < selLst.strList.ll(); i++)
				{
					uiRefTp->addItem(selLst.strList.std_str_at(i).c_str());
				}
				jvxSize idxSel = jvx_bitfieldSelection2Id(selLst.bitFieldSelected(), selLst.strList.ll());
				if (JVX_CHECK_SIZE_SELECTED(idxSel))
				{
					if (idxSel < uiRefTp->count())
					{
						uiRefTp->setCurrentIndex(JVX_SIZE_INT(idxSel));
					}
					else
					{
						std::cout << "Warning: current index value for property is out of valid range in QComboBox " << uiObjectName << ", " << idxSel << " selected whereas number of entries is " << uiRefTp->count() << "." << std::endl;
					}
				}
				else
				{
					if (uiRefTp->count())
					{
						std::cout << "Warning: no selection in property for QComboBox " << uiObjectName << "." << uiRefTp->count() << std::endl;
					}
				}
			}
		}
		*/

		uiRefTp->setProperty("mySaWidgetRefComboBox", QVariant::fromValue<CjvxSaWrapperElementComboBox*>(this));

		connect(uiRefTp, SIGNAL(activated(int)), this, SLOT(combobox_activated(int)));

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
		uiRefTp->clear();
		uiRefTp->addItem("*update*");
		uiRefTp->setCurrentIndex(0);
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
		uiRefTp->clear();
		uiRefTp->addItem("*failed*");
		uiRefTp->setCurrentIndex(0);
	}

	//verboseLoc = this->myBaseProps.verbose_out;
}

CjvxSaWrapperElementComboBox::~CjvxSaWrapperElementComboBox()
{
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		disconnect(uiRefTp, SIGNAL(activated(int)));
	}
	if (uiRefTp)
	{
		uiRefTp->clear();
	}
}
	
jvxErrorType
CjvxSaWrapperElementComboBox::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* passedId, jvxBool call_periodic_update)
{
	jvxSize idxSel;
	jvxSize i;
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	std::string getSetTag = propertyGetSetTag;
	jvxBool contOnly = true;
	jvxCallManagerProperties callGate;
	callGate.on_get.prop_access_type = &thePropDescriptor.accessType;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	
	if (myBaseProps.verbose_out && (myBaseProps.dbgLevel >= 2))
	{
		std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Updating UI element for tag <" << getSetTag << "> -- " << uiRefTp->count() << std::endl;
	}

	if (widgetStatus != JVX_STATE_SELECTED)
	{
		return JVX_ERROR_NOT_READY;
	}


	switch (tp)
	{
	case JVX_WW_COMBOBOX_ONLY_SELECTION:
	case JVX_WW_COMBOBOX_READ_ONCE:

		if (!initialSelectionRead)
		{
			contOnly = false;
		}
		break;
	case JVX_WW_COMBOBOX_READ_ALWAYS:
		contOnly = false;
		break;
	}

	// If recovering from an error, request full update
	if (errorState)
	{
		errorState = false;
		contOnly = false;
	}

	ident.reset(getSetTag.c_str());
	trans.reset(contOnly, 0, thePropDescriptor.decTp);

	if (passedId)
	{
		res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans, jPSCH(passedId));
	}
	else
	{
		res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans, jPSCH(&procId));
	}

	if (res == JVX_NO_ERROR)
	{
		if (!contOnly)
		{
			if (myBaseProps.verbose_out && (myBaseProps.dbgLevel >= 3))
			{
				std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Populating ui element." << std::endl;
			}
			uiRefTp->clear();
			for (i = 0; i < selLst.strList.ll(); i++)
			{
				std::string entry = selLst.strList.std_str_at(i);
				if (myBaseProps.verbose_out && (myBaseProps.dbgLevel >= 3))
				{
					std::cout << "--*--> " << entry << std::endl;
				}
				uiRefTp->addItem(entry.c_str());
			}
			initialSelectionRead = true;
		}
		switch (thePropDescriptor.decTp)
		{
		case JVX_PROPERTY_DECODER_BITFIELD:
		case JVX_PROPERTY_DECODER_MULTI_SELECTION:
			latestValue = selLst.bitFieldSelected();
			for (i = 0; i < selLst.strList.ll(); i++)
			{
				QVariant qv = uiRefTp->itemData(i, Qt::FontRole);
				QFont ft = qvariant_cast<QFont>(qv);
				if (jvx_bitTest(selLst.bitFieldSelected(), i))
				{
					ft.setBold(true);
				}
				else
				{
					ft.setBold(false);
				}
				uiRefTp->setItemData(i, ft, Qt::FontRole);
			}
			break;
		default:
			idxSel = jvx_bitfieldSelection2Id(selLst.bitFieldSelected(), selLst.strList.ll());
			if (JVX_CHECK_SIZE_SELECTED(idxSel))
			{
				if (idxSel < uiRefTp->count())
				{
					uiRefTp->setCurrentIndex(JVX_SIZE_INT(idxSel));
				}
				else
				{
					if (myBaseProps.verbose_out)
					{
						std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Warning: current index value for property is out of valid range in QComboBox " << uiObjectName << ", " << idxSel << " selected whereas number of entries is " << uiRefTp->count() << "." << std::endl;
					}
				}
			}
			else
			{
				if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
				{
					if (myBaseProps.verbose_out)
					{
						std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor << ": Warning: no selection in property for QComboBox " << uiObjectName << "." << uiRefTp->count() << std::endl;
					}
				}
			}
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
			backwardRef->reportPropertyGet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), &selLst, 0, 1, thePropDescriptor.format, static_cast<CjvxSaWrapperElementBase*>(this), res);
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
CjvxSaWrapperElementComboBox::combobox_activated(int sel)
{
	setProperties_core_ui();
}

void
CjvxSaWrapperElementComboBox::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	selLst.bitFieldSelected() = 0;
	selLst.bitFieldExclusive = 0;
	std::string getSetTag = propertyGetSetTag;
	jvxCallManagerProperties callGate;

	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	int sel = uiRefTp->currentIndex();

	if (myBaseProps.verbose_out)
	{
		std::cout << "QComboBox::clicked() callback triggered for " << uiObjectName << std::endl;
	}

	switch (thePropDescriptor.decTp)
	{
	case JVX_PROPERTY_DECODER_BITFIELD:
	case JVX_PROPERTY_DECODER_MULTI_SELECTION:
		selLst.bitFieldSelected() = latestValue;
		if (jvx_bitTest(selLst.bitFieldSelected(), sel))
		{
			jvx_bitClear(selLst.bitFieldSelected(), sel);
		}
		else
		{
			jvx_bitSet(selLst.bitFieldSelected(), sel);
		}
		break;
	default:
		jvx_bitSet(selLst.bitFieldSelected(), sel);
		break;
	}

	ident.reset(getSetTag.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate ,
		jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST),
		ident, trans, jPSCH(&procId));
	if (res == JVX_NO_ERROR)
	{
		mainWidgetEnable();
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
	}
	else
	{
		if (myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		mainWidgetDisable();
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit, myBaseProps.reportTp,
			static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

jvxErrorType
CjvxSaWrapperElementComboBox::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList* selLst = (jvxSelectionList*)prop;
	jvxCallManagerProperties callGate;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);

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
		res = propRef->set_property(callGate ,
			jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans, jPSCH(&procId));
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
