#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementListWidget.h"
#include <QVariant>

CjvxSaWrapperElementListWidget::CjvxSaWrapperElementListWidget(QListWidget* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
	IjvxAccessProperties* propRefIn, std::string propertyGetSetTagLoc,
	std::vector<std::string> paramLstLoc, const std::string& tag_cp, const std::string& tag_key, const std::string& uiObjectNameLoc,
	const std::string & prefix, QWidget* crossRefWidget, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef):
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn, propertyGetSetTagLoc,
		paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId, hostRef)
{
	jvxSize i;

	transformScale = 1.0;
	uiRefTp = uiRefLoc;
	ctxt.id = JVX_SIZE_UNSELECTED;
	ctxt.valid = true;
	lineSeparatorType = JVX_WW_LINE_SEPARATE_LINEFEED;
	myCrossRefWidget = crossRefWidget;

	optButtonTrigger = NULL;
	optCheckBoxClick = NULL;

	numLinesShow = 10;


	// Evaluate the property descriptors
	initializeUiElement();

	//verboseLoc = this->myBaseProps.verbose_out;
}

CjvxSaWrapperElementListWidget::~CjvxSaWrapperElementListWidget()
{
	if (widgetStatus > JVX_STATE_NONE)
	{
		if (optButtonTrigger)
		{
			disconnect(optButtonTrigger, SIGNAL(clicked()));
		}
		if (optCheckBoxClick)
		{
			disconnect(optCheckBoxClick, SIGNAL(clicked()));
		}
	}
}
	
void 
CjvxSaWrapperElementListWidget::clearUiElements()
{
	if (uiRefTp)
		uiRefTp->clear();
}

void
CjvxSaWrapperElementListWidget::initializeUiElement(jvxPropertyCallContext ccontext)
{
	std::string rtu;
	jvxBool foundit = false;
	jvxInt32 intVal = 0;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (widgetStatus == JVX_STATE_NONE)
	{
		// First time enter nitialize - the properties are scanned here.
		res = this->initParameters_getMin(&backwardRef->keymaps.lw, paramLst);
		assert(res == JVX_NO_ERROR);

		transformScale = 1.0;
		int idType = JVX_WW_LINE_SEPARATE_LINEFEED;

		backwardRef->keymaps.lw.getValueForKey("CHECKBOXREF", &optCheckBoxName, JVX_WW_KEY_VALUE_TYPE_STRING);
		backwardRef->keymaps.lw.getValueForKey("BUTTONREF", &optButtonName, JVX_WW_KEY_VALUE_TYPE_STRING);
		backwardRef->keymaps.lw.getValueForKey("LINESEPTP", &idType, JVX_WW_KEY_VALUE_TYPE_ENUM);
		lineSeparatorType = (jvxWwLineSeparation_lw)idType;
		backwardRef->keymaps.lw.getValueForKey("NUM_LINES_SHOW", &numLinesShow, JVX_WW_KEY_VALUE_TYPE_INT32);

		uiRefTp->setProperty("mySaWidgetRefListWidget", QVariant::fromValue<CjvxSaWrapperElementListWidget*>(this));

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
		if (!optCheckBoxName.empty())
		{
			assert(myCrossRefWidget);
			QList<QWidget *> widgets = myCrossRefWidget->findChildren<QWidget *>();
			for (i = 0; i < widgets.count(); i++)
			{
				QString name = widgets[JVX_SIZE_INT(i)]->objectName();
				if (name.toLatin1().data() == optCheckBoxName)
				{
					optCheckBoxClick = qobject_cast<QCheckBox*>(widgets[JVX_SIZE_INT(i)]);
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
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		mainWidgetDisable();

		switch (thePropDescriptor.format)
		{
		case JVX_DATAFORMAT_STRING:
	
			// Text log stored as STRING property with a lot more behind
			if (thePropDescriptor.decTp != JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG)
			{
				std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor
					<< ": Incorrect decoder type, expected JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG." << std::endl;
				widgetStatus = JVX_STATE_NONE;
			}
			break;
		default:
			std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor
				<< ": Incorrect type, expected JVX_DATAFORMAT_STRING." << std::endl;
			widgetStatus = JVX_STATE_NONE;
			break;
		}

		if (optButtonTrigger)
		{
			connect(optButtonTrigger, SIGNAL(clicked()), this, SLOT(trigger_clear()));
		}
		if (optCheckBoxClick)
		{
			connect(optCheckBoxClick, SIGNAL(clicked()), this, SLOT(clicked_follow()));
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
	// -> verboseLoc = this->myBaseProps.verbose_out;
}

void
CjvxSaWrapperElementListWidget::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
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
		updateUiDescriptor(callGate.access_protocol );
	}
	else if (res == JVX_ERROR_POSTPONE)
	{
		mainWidgetDisable();
		uiRefTp->setToolTip("*update*");
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
CjvxSaWrapperElementListWidget::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* passedId, jvxBool call_periodic_update)
{
	jvxErrorType res = JVX_NO_ERROR;
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
	case JVX_DATAFORMAT_STRING:
		ptrFld = &str;
		break;
	}

	callGate.on_get.prop_access_type = &thePropDescriptor.accessType;
	callGate.context = &ctxt;

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
		case JVX_DATAFORMAT_STRING:
			txt = str.std_str();
			break;
		}

		collectData += txt;
		
		updateShowListWidget();

		// TODO uiRefTp->setText(txt.c_str());

		updateUiDescriptor(callGate.access_protocol );

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
		uiRefTp->setToolTip("*update*");
	}
	else
	{
		mainWidgetDisable();
		uiRefTp->setToolTip(jvxErrorType_txt(res));
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

jvxErrorType 
CjvxSaWrapperElementListWidget::set_config(jvxWwConfigType tp, jvxHandle* load)
{
	return JVX_ERROR_UNSUPPORTED;
}

void
CjvxSaWrapperElementListWidget::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	// This function is not supported
	return;
}

jvxErrorType
CjvxSaWrapperElementListWidget::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, 
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{ 
	return JVX_ERROR_UNSUPPORTED;
}

void
CjvxSaWrapperElementListWidget::trigger_clear()
{
	uiRefTp->clear();
}

void
CjvxSaWrapperElementListWidget::clicked_follow()
{
	follow_list();
}
void
CjvxSaWrapperElementListWidget::updateShowListWidget()
{
	jvxSize i;
	jvxSize numEntries = 0;
	jvxSize toClear = 0;
	std::string token;
	jvxSize increment = 1;
	jvxBool followNewest = false;

	while (1)
	{
		size_t posi = std::string::npos;
		switch (lineSeparatorType)
		{
		case JVX_WW_LINE_SEPARATE_LINEFEED:
			posi = collectData.find("\n");
			increment = 1;
			break;
		case JVX_WW_LINE_SEPARATE_CARRRETURN:
			posi = collectData.find("\r");
			increment = 1;
			break;
		case JVX_WW_LINE_SEPARATE_CARRRETURN_LINEFEED:
			posi = collectData.find("\r\n");
			increment = 2;
			break;
		}
		if (posi != std::string::npos)
		{
			token = collectData.substr(0, posi);
			uiRefTp->addItem(token.c_str());
			collectData = collectData.substr(posi + increment, std::string::npos);
		}
		else
		{
			break;
		}
	}

	// Now, reduce the size of the shown messages
	numEntries = uiRefTp->count();
	if (numEntries > numLinesShow)
	{
		toClear = numEntries - numLinesShow;
		for (i = 0; i < toClear; i++)
		{
			QListWidgetItem *it = uiRefTp->takeItem(0);
			delete it;
		}
	}

	follow_list();
}

void
CjvxSaWrapperElementListWidget::follow_list()
{
	jvxBool followNewest = false;

	if (optCheckBoxClick)
	{
		if (optCheckBoxClick->isChecked())
		{
			followNewest = true;
		}
	}
	if (followNewest)
	{
		// Always jump to end of line
		uiRefTp->setCurrentRow(uiRefTp->count() - 1);
	}
}

jvxBool
CjvxSaWrapperElementListWidget::local_command(std::string& local_command)
{
	if (local_command == "listWidget::clear")
	{
		this->uiRefTp->clear();
	}
	return true;
}