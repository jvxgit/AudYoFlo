#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementFrame.h"
#include <QVariant>
#include "jvx-qt-helpers.h"
#include <QGridLayout>
#include <QLineEdit>
CjvxSaWrapperElementFrame::CjvxSaWrapperElementFrame(QWidget* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc,
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
	tp = JVX_WW_COMBOBOX_READ_ONCE;
	initialSelectionRead = false;
	wrapAroundCount = 0;
	index_selection = JVX_SIZE_UNSELECTED;
	elmTp = JVX_WW_FRAME_ELEMENTS_LABELS;
	minSize_w = 15;
	minSize_h = 15;
	maxSize_w = 15;
	maxSize_h = 15;

	jvx_bitFSet(maskSelection);
	initializeUiElement();
}

void
CjvxSaWrapperElementFrame::initializeUiElement(jvxPropertyCallContext ccontext)
{
	jvxBool foundit = false;
	jvxInt32 intVal = 0;
	jvxErrorType res = JVX_NO_ERROR;
	std::string strRead;

	if (widgetStatus == JVX_STATE_NONE)
	{
		jvxErrorType res = this->initParameters_getMin(&backwardRef->keymaps.fr, paramLst);
		assert(res == JVX_NO_ERROR);

		backwardRef->keymaps.fr.getValueForKey("BEHAVIOR", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
		if (foundit)
		{
			tp = (jvxWwConnectionType_cb)intVal;
		}

		foundit = false;
		backwardRef->keymaps.fr.getValueForKey("WRAPAROUNDCNT", &intVal, JVX_WW_KEY_VALUE_TYPE_INT32, &foundit);
		if (foundit)
		{
			wrapAroundCount = intVal;
		}

		backwardRef->keymaps.fr.getValueForKey("UIELEMENTTP", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
		if (foundit)
		{
			elmTp = (jvxWwUiElementType_fr)intVal;
		}

		backwardRef->keymaps.fr.getValueForKey("VIEWSELECTIONBITMASK", &strRead, JVX_WW_KEY_VALUE_TYPE_STRING, &foundit);
		if (foundit)
		{
			jvxBool err = false;
			jvxUInt64 valU64 = jvx_string2Int64(strRead, err);
			if (err == false)
			{
				maskSelection = valU64;
			}
		}
		switch (elmTp)
		{
		case JVX_WW_FRAME_ELEMENTS_LABELS:
			minSize_w = 15;
			minSize_h = 15;
			maxSize_w = 15;
			maxSize_h = 15;
			break;
		case JVX_WW_FRAME_ELEMENTS_BUTTONS:
		case JVX_WW_FRAME_ELEMENTS_CHECKBOXES:
			minSize_w = 50;
			minSize_h = 20;
			maxSize_w = 50;
			maxSize_h = 20;
			break;
		}

		backwardRef->keymaps.fr.getValueForKey("MINSIZEW", &intVal, JVX_WW_KEY_VALUE_TYPE_INT32, &foundit);
		if (foundit)
		{
			minSize_w = intVal;
		}
		backwardRef->keymaps.fr.getValueForKey("MINSIZEH", &intVal, JVX_WW_KEY_VALUE_TYPE_INT32, &foundit);
		if (foundit)
		{
			minSize_h = intVal;
		}
		backwardRef->keymaps.fr.getValueForKey("MAXSIZEW", &intVal, JVX_WW_KEY_VALUE_TYPE_INT32, &foundit);
		if (foundit)
		{
			maxSize_w = intVal;
		}
		backwardRef->keymaps.fr.getValueForKey("MAXSIZEH", &intVal, JVX_WW_KEY_VALUE_TYPE_INT32, &foundit);
		if (foundit)
		{
			maxSize_h = intVal;
		}	
	}

	res = initializeProperty_core(true, ccontext);
	if (res != JVX_NO_ERROR)
	{
		/*
		mainWidgetDisable();
		uiRefTp->setText("*not connected*");
		*/
	}

	// Evaluate the property descriptors
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		switch(thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_SELECTION_LIST:
			break;
		default:
			std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor 
				<< ": Incorrect type, expected JVX_DATAFORMAT_SELECTION_LIST." << std::endl;
			widgetStatus = JVX_STATE_NONE;
			break;
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

CjvxSaWrapperElementFrame::~CjvxSaWrapperElementFrame()
{
	jvxSize i;
	for (i = 0; i < lstSelLabels.size(); i++)
	{
		delete lstSelLabels[i];
	}
	lstSelLabels.clear();
	for (i = 0; i < lstSelButtons.size(); i++)
	{
		delete lstSelButtons[i];
	}
	lstSelButtons.clear();
	for (i = 0; i < lstSelCheckboxes.size(); i++)
	{
		delete lstSelCheckboxes[i];
	}
}

void
CjvxSaWrapperElementFrame::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
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
CjvxSaWrapperElementFrame::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* globalId, jvxBool call_periodic_update)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	std::string txt;
	std::string unit;
	jvxBool contOnly = true;
	QLabel* nL = NULL;
	QPushButton* nB = NULL;
	QCheckBox* nC = NULL;
	jvxBool isValid = false;
	jvxSize cntx = 0;
	jvxSize cnty = 0;
	jvxCallManagerProperties callGate;
	callGate.on_get.prop_access_type = &thePropDescriptor.accessType;

	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	
	if (widgetStatus != JVX_STATE_SELECTED)
	{
		return JVX_ERROR_NOT_READY;
	}

	// std::cout << __FUNCTION__ << "-> " << getSetTag << std::endl;
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

	ident.reset(getSetTag.c_str());
	trans.reset(contOnly, 0, thePropDescriptor.decTp);
	if (globalId)
	{
		res = propRef->get_property(callGate, jPRG(&selLst, 1, thePropDescriptor.format), ident, trans, jPSCH(globalId));
	}
	else
	{
		res = propRef->get_property(callGate, jPRG(&selLst, 1, thePropDescriptor.format), ident, trans, jPSCH(&procId));
	}

	if(res == JVX_NO_ERROR)
	{
		if (!contOnly)
		{
			jvxSize cnt = 0;
			QGridLayout* lay = NULL;
			QLayout* layW = uiRefTp->layout();
			lay = dynamic_cast<QGridLayout*>(layW);
			if (lay == NULL)
			{
				lay = new QGridLayout;
				lay->setContentsMargins(5, 0, 5, 0);
				lay->setHorizontalSpacing(5);
				uiRefTp->setLayout(lay);
			}
			else
			{
				switch (elmTp)
				{
				case JVX_WW_FRAME_ELEMENTS_LABELS:
					for (i = 0; i < lstSelLabels.size(); i++)
					{
						delete lstSelLabels[i];
					}
					lstSelLabels.clear();
					break;
				case JVX_WW_FRAME_ELEMENTS_BUTTONS:
					for (i = 0; i < lstSelButtons.size(); i++)
					{
						delete lstSelButtons[i];
					}
					lstSelButtons.clear();
					break;
				case JVX_WW_FRAME_ELEMENTS_CHECKBOXES:
					for (i = 0; i < lstSelCheckboxes.size(); i++)
					{
						delete lstSelCheckboxes[i];
					}
					lstSelCheckboxes.clear();
					break;
				}
			}
			for (i = 0; i < selLst.strList.ll(); i++)
			{
				switch (elmTp)
				{
				case JVX_WW_FRAME_ELEMENTS_LABELS:
					
					if (jvx_bitTest(maskSelection, i))
					{
						nL = new QLabel(uiRefTp);
						nL->setProperty("selection", QVariant((int)i));
						lstSelLabels.push_back(nL);

						nL->setText(("#" + jvx_size2String(i)).c_str());
						nL->setToolTip(selLst.strList.std_str_at(i).c_str());

						cntx = cnt;
						cnty = 0;
						if (wrapAroundCount > 0)
						{
							cntx = cnt % wrapAroundCount;
							cnty = cnt / wrapAroundCount;
						}
						lay->addWidget(nL, cnty, cntx, 1, 1);

						nL->setMinimumSize(QSize(minSize_w, minSize_h));
						nL->setMaximumSize(QSize(maxSize_w, maxSize_h));
						nL->setAutoFillBackground(true);
						nL->setFrameShape(QFrame::StyledPanel);

						nL->show();
						cnt++;
					}
					break;

				case JVX_WW_FRAME_ELEMENTS_BUTTONS:

					if (jvx_bitTest(maskSelection, i))
					{
						nB = new QPushButton(uiRefTp);
						connect(nB, SIGNAL(clicked()), this, SLOT(slot_button_pushed()));
						nB->setProperty("selection", QVariant((int)i));

						lstSelButtons.push_back(nB);

						nB->setText(selLst.strList.std_str_at(i).c_str());

						cntx = cnt;
						cnty = 0;
						if (wrapAroundCount > 0)
						{
							cntx = cnt % wrapAroundCount;
							cnty = cnt / wrapAroundCount;
						}
						lay->addWidget(nB, cnty, cntx, 1, 1);

						nB->setMinimumSize(QSize(minSize_w, minSize_h));
						nB->setMaximumSize(QSize(maxSize_w, maxSize_h));

						nB->show();
						cnt++;
					}
					break;

				case JVX_WW_FRAME_ELEMENTS_CHECKBOXES:

					if (jvx_bitTest(maskSelection, i))
					{
						nC = new QCheckBox(uiRefTp);
						connect(nC, SIGNAL(clicked(bool)), this, SLOT(slot_checkbox_toggled(bool)));
						nC->setProperty("selection", QVariant((int)i));
						
						lstSelCheckboxes.push_back(nC);

						nC->setText(selLst.strList.std_str_at(i).c_str());
						nC->setToolTip(selLst.strList.std_str_at(i).c_str());

						cntx = cnt;
						cnty = 0;
						if (wrapAroundCount > 0)
						{
							cntx = cnt % wrapAroundCount;
							cnty = cnt / wrapAroundCount;
						}
						lay->addWidget(nC, cnty, cntx, 1, 1);

						nC->setMinimumSize(QSize(minSize_w, minSize_h));
						nC->setMaximumSize(QSize(maxSize_w, maxSize_h));
						nC->show();
						cnt++;
					}
					break;
				}
			}
			uiRefTp->setLayout(lay);

			/*
			uiRefTp->clear();
			for (i = 0; i < selLst.strList.ll(); i++)
			{
				uiRefTp->addItem(selLst.strList.std_str_at(i).c_str());
			}*/
			initialSelectionRead = true;
		}

		switch (elmTp)
		{
		case JVX_WW_FRAME_ELEMENTS_LABELS:
			for(i = 0; i < lstSelLabels.size(); i++)
			{
				QVariant val = lstSelLabels[i]->property("selection");
				if (val.isValid())
				{
					int idx = val.toInt();

					if (idx < selLst.strList.ll())
					{
						if (jvx_bitTest(selLst.bitFieldSelected(), idx))
						{
							setBackgroundLabelColor(Qt::green, lstSelLabels[i]);
						}
						else
						{
							setBackgroundLabelColor(Qt::gray, lstSelLabels[i]);
						}
					}
				}
			}
			break;
		case  JVX_WW_FRAME_ELEMENTS_BUTTONS:
			for (i = 0; i < lstSelButtons.size(); i++)
			{
				QVariant val = lstSelButtons[i]->property("selection");
				if (val.isValid())
				{
					int idx = val.toInt();

					if (idx < selLst.strList.ll())
					{
						if (jvx_bitTest(selLst.bitFieldSelectable, idx))
						{
							lstSelButtons[i]->setEnabled(true);
						}
						else
						{
							lstSelButtons[i]->setEnabled(false);
						}
					}
				}
			}
			break;
		case  JVX_WW_FRAME_ELEMENTS_CHECKBOXES:
			for (i = 0; i < lstSelCheckboxes.size(); i++)
			{
				QVariant val = lstSelCheckboxes[i]->property("selection");
				if (val.isValid())
				{
					int idx = val.toInt();

					if (idx < selLst.strList.ll())
					{
						if (jvx_bitTest(selLst.bitFieldSelected(), idx))
						{
							lstSelCheckboxes[i]->setChecked(true);
						}
						else
						{
							lstSelCheckboxes[i]->setChecked(false);
						}
					}
				}
			}
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
			backwardRef->reportPropertyGet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), &selLst, 0, 1, thePropDescriptor.format, static_cast<CjvxSaWrapperElementBase*>(this), res);
		}

	}

	if (res == JVX_NO_ERROR)
	{
		mainWidgetEnable();
		//uiRefTp->setText(txt.c_str());
		
	}
	else if (res == JVX_ERROR_POSTPONE)
	{
		mainWidgetDisable();
		/*
		uiRefTp->setText("*update*");
		*/
	}
	else
	{
		mainWidgetDisable();
		/*
		uiRefTp->setText("*failed*");
		*/
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
CjvxSaWrapperElementFrame::slot_button_pushed()
{
	QObject* sender = QObject::sender();
	QVariant var = sender->property("selection");
	if (var.isValid())
	{
		index_selection = var.toInt();
		std::cout << "Button with index <" << index_selection << "> pushed." << std::endl;
		setProperties_core_ui();
	}
}

void
CjvxSaWrapperElementFrame::slot_checkbox_toggled(bool tog)
{
	QObject* sender = QObject::sender();
	QVariant var = sender->property("selection");
	if (var.isValid())
	{
		index_selection = var.toInt();
		std::cout << "Checkbox with index <" << index_selection << "> toggled." << std::endl;
		setProperties_core_ui();
	}
}

void
CjvxSaWrapperElementFrame::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	// In a label, there is no "set"
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
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/

	if (
		(elmTp == JVX_WW_FRAME_ELEMENTS_BUTTONS) ||
		(elmTp == JVX_WW_FRAME_ELEMENTS_CHECKBOXES))
	{
		
		if (myBaseProps.verbose_out)
		{
			if (elmTp == JVX_WW_FRAME_ELEMENTS_BUTTONS)
			{
				std::cout << "QPushButton::clicked() callback triggered for " << uiObjectName << std::endl;
			}
			if (elmTp == JVX_WW_FRAME_ELEMENTS_CHECKBOXES)
			{
				std::cout << "QCheckBox::toggled(bool) callback triggered for " << uiObjectName << std::endl;
			}
		}

		if (elmTp == JVX_WW_FRAME_ELEMENTS_BUTTONS)
		{
			selLst.bitFieldSelected() = 0;
			jvx_bitSet(selLst.bitFieldSelected(), index_selection);
		}
		if (elmTp == JVX_WW_FRAME_ELEMENTS_CHECKBOXES)
		{
			jvxSize i;
			jvx_bitFClear(selLst.bitFieldSelected());
			for (i = 0; i < lstSelCheckboxes.size(); i++)
			{
				if (lstSelCheckboxes[i]->isChecked())
				{
					jvx_bitSet(selLst.bitFieldSelected(), i);
				}
				else
				{
					jvx_bitClear(selLst.bitFieldSelected(), i);
				}
			}
		}

		ident.reset(getSetTag.c_str());
		trans.reset(true, 0, thePropDescriptor.decTp, false);

		res = propRef->set_property(callGate , 
			jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST),
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
	}
	else
	{
		assert(0);
	}
}

jvxErrorType
CjvxSaWrapperElementFrame::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD;
	jvxInt8 val8;
	jvxInt16 val16;
	jvxInt32 val32;
	jvxInt64 val64;
	/*
	QString qtxt = uiRefTp->text();
	*/
	jvxApiString fldStr;
	std::string txt;
	jvxHandle* ptrFld = NULL;
	std::string unit;
	jvxCallManagerProperties callGate;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);

	assert(0);
#if 0
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

		res = propRef->set_property__descriptor(ptrFld, 0, 1, thePropDescriptor.format, true, getSetTag.c_str(), thePropDescriptor.decTp, &procId,
			false, callGate);
		if (res == JVX_NO_ERROR)
		{
			backwardRef->reportPropertySet(tag.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id, 
				myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
			updateUiDescriptor(accProt );
		}
		else if (res == JVX_ERROR_POSTPONE)
		{
			/*
			mainWidgetDisable();
			uiRefTp->setText("*update*");
			*/
		}
		else
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
			}
			backwardRef->reportPropertySet(tag.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id, 
				myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_SETTING;
	}
#endif
	return res;
}