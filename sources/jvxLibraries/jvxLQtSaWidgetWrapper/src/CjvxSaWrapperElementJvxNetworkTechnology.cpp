#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementJvxNetworkTechnology.h"
#include "jvx-qt-helpers.h"
#include <QVariant>

CjvxSaWrapperElementJvxNetworkTechnology::CjvxSaWrapperElementJvxNetworkTechnology(jvxQtSaNetworkTechnology* uiRefLoc,
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
CjvxSaWrapperElementJvxNetworkTechnology::initializeUiElement(jvxPropertyCallContext ccontext)
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

		cb_tech = NULL;
		

		uiRefTp->getWidgetReferences(&cb_tech);

		if(cb_tech)
		{
			if (myBaseProps.verbose_out)
			{
				std::cout << "Found reference to element for <technology>" << std::endl;
			}

			connect(cb_tech, SIGNAL(activated(int)), this, SLOT(selectedTechnology(int)));
		}
		
		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			assert(0); // To be done
		}
		updateWindowUiElement();
	
		uiRefTp->setProperty("mySaWidgetRefNetworkTechnology", QVariant::fromValue<CjvxSaWrapperElementJvxNetworkTechnology*>(this));

		//connect(uiRefTp, SIGNAL(clicked(bool)), this, SLOT(checkbox_toggled(bool)));
		updateWindowUiElement();

		assert(backwardRef);
		backwardRef->reportPropertyInitComplete(selector_lst.c_str(), assoc_id);
	}
	//verboseLoc = this->myBaseProps.verbose_out;
}

CjvxSaWrapperElementJvxNetworkTechnology::~CjvxSaWrapperElementJvxNetworkTechnology()
{
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		if(cb_tech)
		{
			disconnect(cb_tech, SIGNAL(activated(int)));
		}
	}
}

void
CjvxSaWrapperElementJvxNetworkTechnology::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
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
CjvxSaWrapperElementJvxNetworkTechnology::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* globalId, jvxBool call_periodic_update )
{
	jvxSize i;
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	jvxApiString  fldStr;
	jvxInt32 valI32;
	jvxCallManagerProperties callGate;
	jvxBool anyError = false;
	jvxCBool valB = false;
	jvxInt16 valI16 = c_false;

	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_SOCK_TYPES");
	ident.reset(propName.c_str());
	trans.reset(true);
	res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if(res == JVX_NO_ERROR)
	{
		jvxSize idx = 0;
		cb_tech->clear();
		for (i = 0; i < selLst.strList.ll(); i++)
		{
			std::string txt = selLst.strList.std_str_at(i);
			if (jvx_bitTest(selLst.bitFieldSelected(), i))
			{
				txt += "*";
				idx = i;
			}
			cb_tech->addItem(txt.c_str());
		}
		cb_tech->setCurrentIndex(idx);
	}
	else
	{
		if(myBaseProps.verbose_out)
		{
			std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << propName << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
		cb_tech->setEnabled(false);
	}
	return JVX_NO_ERROR;
}

void
CjvxSaWrapperElementJvxNetworkTechnology::selectedTechnology(int idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString fldStr;
	std::string propName;
	jvxCallManagerProperties callGate;
	std::string getSetTag = propertyGetSetTag;
	jvxSelectionList selLst;

	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	jvx_bitSet(selLst.bitFieldSelected(), idx);
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_SOCK_TYPES");
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate,
		jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST),
		ident, trans);
	backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
}

void 
CjvxSaWrapperElementJvxNetworkTechnology::trigger_updateWindow_periodic(jvxPropertyCallContext ccontext, jvxSize* passedId)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 valI32;
	jvxData valD = 0;
	std::string txtShow;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	
}

void
CjvxSaWrapperElementJvxNetworkTechnology::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	assert(0);
}

jvxErrorType
CjvxSaWrapperElementJvxNetworkTechnology::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	return JVX_ERROR_UNSUPPORTED;
}