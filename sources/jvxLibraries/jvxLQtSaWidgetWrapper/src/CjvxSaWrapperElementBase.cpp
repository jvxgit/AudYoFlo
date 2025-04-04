#include <cmath>
#include "CjvxSaWrapperElementBase.h"
#include "qvariant.h"

/*
#ifndef JVX_DEBUG_DELAYED_CONTROL
#define JVX_DEBUG_DELAYED_CONTROL
#endif
*/

// #define DEBUG_WRAPPER_ELEMENT_BASE

#define JVX_EPS_DB 1e-10
#define JVX_COBJECT_OS_BUF_SIZE 8192
#define JVX_LOOP_CHECK_COUNTER_MAX 10 

void
CjvxSaWrapperElementBase::init_propertyPrivate(jvxPropertyCallPrivate* prv, jvxPropertyDelayedCallPurpose purp,
	const std::string& selector_lst,
	const std::string& wildcard,
	jvxSize groupid_emit)
{
	prv->delayedCallPurpose = purp;
	prv->selector_lst = selector_lst;
	prv->wildcard = wildcard;
	prv->groupid_emit = groupid_emit;
	prv->prop_id = JVX_SIZE_UNSELECTED;
	prv->prop_cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
	prv->prop_conly = false;
	prv->externalUId = NULL;
	prv->passExternalUId = false;
}

void
CjvxSaWrapperElementBase::init_propertyPrivate(jvxPropertyCallPrivate* prv, jvxPropertyDelayedCallPurpose purp,
	const std::string& selector_lst,
	jvxSize pId,
	jvxPropertyCategoryType pCat,
	jvxBool content_only)
{
	prv->delayedCallPurpose = purp;
	prv->selector_lst = selector_lst;
	prv->wildcard = "";
	prv->groupid_emit = 0;
	prv->prop_id = pId;
	prv->prop_cat = pCat;
	prv->prop_conly = content_only;
	prv->externalUId = NULL;
	prv->passExternalUId = false;
}

CjvxSaWrapperElementBase::CjvxSaWrapperElementBase(QWidget* uiRefLoc,
	CjvxQtSaWidgetWrapper* backRefLoc,
	IjvxAccessProperties* propRefIn,
	std::string propertyNameLoc,
	std::vector<std::string> paramLstLoc,
	const std::string& tag_arg,
	const std::string& selector_lst_arg,
	const std::string& uiObjectNameLoc,
	const std::string& propertyPrefix,
	jvxBool verboseLoc,
	jvxSize uId, IjvxHiddenInterface* hostRef)
{
	theHostRef = hostRef;
	procId = 0;
	idPostpone = 0;
	procIdP = 0;
	widgetStatus = JVX_STATE_NONE;
	uiRef = uiRefLoc;
	backwardRef = backRefLoc;
	propRef = propRefIn;
	propRefIn->get_features(&this->thePropAFeatures);
	propertyGetSetTag = propertyNameLoc;
	propPrefix = propertyPrefix;
	paramLst = paramLstLoc;
	tag = tag_arg;
	selector_lst = selector_lst_arg;
	uiObjectName = uiObjectNameLoc;
	transformScale = 1.0;
	isUnchecked = true;
	jvx_initPropertyDescription(thePropDescriptor);
	resetBasePropInfos(myBaseProps);
	if (verboseLoc)
	{
		myBaseProps.verbose_out = verboseLoc;
	}
	assoc_id = uId;
	delayedActionOperations = 0;
	delayedActionPending = 0;
	loopCheckCnt = 0;
	errorState = false;

	connect(this, SIGNAL(emit_reenter_delayed_initialize()), this, SLOT(slot_reenter_delayed_initialize()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_trigger_updateWindow()), this, SLOT(slot_trigger_updateWindow()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_trigger_updateStatus()), this, SLOT(slot_trigger_updateStatus()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_send_content()), this, SLOT(slot_send_content()), Qt::QueuedConnection);

#ifdef JVX_OBJECTS_WITH_TEXTLOG

	if (theHostRef)
	{
		// Reset the logfile module
		jvx_init_text_log(embLog);

		embLog.jvxrtst_bkp.set_module_name(uiObjectName.c_str());

		theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&embLog.jvxrtst_bkp.theToolsHost));
		if (embLog.jvxrtst_bkp.theToolsHost)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(embLog.jvxrtst_bkp.jvxlst_buf, char, JVX_COBJECT_OS_BUF_SIZE);
			embLog.jvxrtst_bkp.jvxlst_buf_sz = JVX_COBJECT_OS_BUF_SIZE;
			jvx_request_text_log(embLog);
		}
	}
#endif
}

CjvxSaWrapperElementBase::~CjvxSaWrapperElementBase()
{
#ifdef JVX_OBJECTS_WITH_TEXTLOG

	if (embLog.jvxrtst_bkp.theToolsHost)
	{
		jvx_return_text_log(embLog);
		JVX_DSP_SAFE_DELETE_FIELD(embLog.jvxrtst_bkp.jvxlst_buf);

		embLog.jvxrtst_bkp.jvxlst_buf = NULL;
		embLog.jvxrtst_bkp.jvxlst_buf_sz = 0;

		theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(embLog.jvxrtst_bkp.jvxlst_buf));
	}
	embLog.jvxrtst_bkp.jvxlst_buf = NULL;
	jvx_terminate_text_log(embLog);
#endif
	if(uiRef)
	{
		uiRef->setToolTip("");
	}
	uiRef = NULL;
};

bool 
CjvxSaWrapperElementBase::matchesWildcard(const std::string& wildcard, jvxSize groupid, jvxWwMatchIdType matchIdType)
{
	jvxSize i;
	jvxBool doesMatch1 = true;
	jvxBool doesMatch2 = true;
	if (wildcard.empty())
	{
		doesMatch1 = true;
	}
	else
	{
		std::string fullPath = jvx_makePathExpr(propPrefix, propertyGetSetTag);
		doesMatch1 = jvx_compareStringsWildcard(wildcard, fullPath);
		for (i = 0; i < dependencies.size(); i++)
		{
			fullPath = jvx_makePathExpr(propPrefix, dependencies[i]);
			doesMatch1 = doesMatch1 || jvx_compareStringsWildcard(wildcard, fullPath);
		}
	}
	if (groupid == 0)
	{
		doesMatch2 = true; // If the group id is 0, only the wildcard is of importance
	}
	else
	{
		doesMatch2 = false;
		switch (matchIdType)
		{
		case JVX_WW_MATCH_GROUPID_VALUE:
			if (groupid == myBaseProps.group_id_trigger)
			{
				doesMatch2 = true;
			}
			break;
		case JVX_WW_MATCH_GROUPID_BITFIELD:
			if (groupid & myBaseProps.group_id_trigger)
			{
				doesMatch2 = true;
			}
			break;
		default:
			assert(0);
		}
	}
	return doesMatch1 && doesMatch2;
}

bool
CjvxSaWrapperElementBase::matchesIdCat(jvxSize propId, jvxPropertyCategoryType cat)
{
	jvxSize i;
	
	jvxBool doesMatch = false;
	doesMatch = (thePropDescriptor.globalIdx == propId);
	doesMatch = doesMatch && (thePropDescriptor.category == cat);
	/*
	if(doesMatch)
		std::cout << "Match for " << jvxPropertyCategoryType_txt(cat) << ", " << propId << "." << std::endl;
	*/
	return doesMatch;
}


void 
CjvxSaWrapperElementBase::resetBasePropInfos(basePropInfos& mbp)
{
	mbp.updateRt = false;
	mbp.verbose_out = false;
	mbp.report_get = false;
	mbp.tooltipDebug = false;
	mbp.supersede_enable = false;
	mbp.reportTp = JVX_WW_REPORT_ELEMENT;
	mbp.group_id_trigger = 0;
	mbp.group_id_emit = 0;
}

jvxErrorType
CjvxSaWrapperElementBase::initParameters_getMin(keyValueList* theLst, std::vector<std::string> & paramLst)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundit = false;
	jvxInt32 intVal = 0;
	std::string depString;

	res = theLst->parseEntryList(paramLst, "");
	if (res == JVX_NO_ERROR)
	{
		theLst->getValueForKey("SUPERSEDE_ENABLE", &myBaseProps.supersede_enable, JVX_WW_KEY_VALUE_TYPE_BOOL);
		theLst->getValueForKey("TTIPDEBUG", &myBaseProps.tooltipDebug, JVX_WW_KEY_VALUE_TYPE_BOOL);
		foundit = false;
		theLst->getValueForKey("GROUPID", &intVal, JVX_WW_KEY_VALUE_TYPE_INT32, &foundit);
		if (foundit)
		{
			myBaseProps.group_id_trigger = intVal;
			myBaseProps.group_id_emit = intVal;
		}

		foundit = false;
		theLst->getValueForKey("GROUPID_TRIGGER", &intVal, JVX_WW_KEY_VALUE_TYPE_INT32, &foundit);
		if (foundit)
		{
			myBaseProps.group_id_trigger = intVal;
		}

		foundit = false;
		theLst->getValueForKey("GROUPID_EMIT", &intVal, JVX_WW_KEY_VALUE_TYPE_INT32, &foundit);
		if (foundit)
		{
			myBaseProps.group_id_emit = intVal;
		}
		theLst->getValueForKey("RTUPDATE", &myBaseProps.updateRt, JVX_WW_KEY_VALUE_TYPE_BOOL);

		if (!myBaseProps.verbose_out)
		{
			theLst->getValueForKey("VERBOSE", &myBaseProps.verbose_out, JVX_WW_KEY_VALUE_TYPE_BOOL);
		}

		foundit = false;
		myBaseProps.dbgLevel = 0;
		if (myBaseProps.verbose_out)
		{
			myBaseProps.dbgLevel = 1;
		}
		theLst->getValueForKey("DBG_LEVEL", &intVal, JVX_WW_KEY_VALUE_TYPE_INT32, &foundit);
		if (foundit)
		{
			myBaseProps.dbgLevel = JVX_MAX(intVal, myBaseProps.dbgLevel);
		}

		theLst->getValueForKey("REPORT_GET", &myBaseProps.report_get, JVX_WW_KEY_VALUE_TYPE_BOOL);
		foundit = false; 
		theLst->getValueForKey("REPORT", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
		if (foundit)
		{
			myBaseProps.reportTp = (jvxWwReportType)intVal;
		}
		theLst->getValueForKey("DEPENDS", &depString, JVX_WW_KEY_VALUE_TYPE_STRING);
		while (1)
		{
			jvxSize posi = depString.find(";");
			if (posi != std::string::npos)
			{
				std::string locToken = depString.substr(0, posi);
				if (!locToken.empty())
					dependencies.push_back(locToken);
				depString = depString.substr(posi + 1, std::string::npos);
			}
			else
			{
				if (!depString.empty())
					dependencies.push_back(depString);
				break;
			}
		}

	}
	else
	{
		std::string err;
		theLst->lastError(err);
		std::cout << "Failed to identify parameter token " <<  err << " when parsing additional configuration tokens for " << uiObjectName << " linked with property " << propertyGetSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
	}
	return(res);
}


void
CjvxSaWrapperElementBase::getDisplayUnitToken(jvxWwTransformValueType tp, std::string& unit)
{
	unit.clear();
	switch (tp)
	{
	case JVX_WW_SHOW_DB10:
	case JVX_WW_SHOW_DB20:
		unit = "dB";
		break;
	}
}

jvxData 
CjvxSaWrapperElementBase::transformTo(jvxData valD, jvxWwTransformValueType tp, jvxData scaleFac)
{
	switch(tp)
	{
	case JVX_WW_SHOW_DIRECT:
		//valD = valD;
		break;
	case JVX_WW_SHOW_SCALED:
		valD = valD * scaleFac;
		break;
	case JVX_WW_SHOW_DB10:
		valD = valD*scaleFac;
		if (valD < JVX_EPS_DB)
			valD = JVX_EPS_DB;
		valD = 10*log10(valD);
		break;
	case JVX_WW_SHOW_DB20:
		valD = valD*scaleFac;
		if (valD < JVX_EPS_DB)
			valD = JVX_EPS_DB;
		valD = 20*log10(valD);
		break;
	}
	return(valD);
}

jvxData 
CjvxSaWrapperElementBase::transformFrom(jvxData valD, jvxWwTransformValueType tp, jvxData scaleFac)
{
	switch(tp)
	{
	case JVX_WW_SHOW_DIRECT:
		//valD = valD;
		break;
	case JVX_WW_SHOW_SCALED:
		valD /= scaleFac;
		break;
	case JVX_WW_SHOW_DB10:
		valD = pow(10.0, valD/10.0);
		valD /= scaleFac;
		break;
	case JVX_WW_SHOW_DB20:
		valD = pow(10.0, valD/20.0);
		valD /= scaleFac;
		break;
	}
	return(valD);
}

void 
CjvxSaWrapperElementBase::jvx_set_transform_scale(jvxData val)
{
	transformScale = val;
}

jvxData 
CjvxSaWrapperElementBase::jvx_get_transform_scale()
{
	return(transformScale);
}

// =============================================================================================
// =============================================================================================

jvxBool
CjvxSaWrapperElementBase::updateWindowUiElement_ifavail(jvxPropertyCallContext ccontext, jvxSize* passedId)
{
	jvxCBitField operDelayedAccess = (jvxCBitField)1 << JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool calledInDelayContext = CHECK_NONBLOCKING_RESPONSE(this->thePropAFeatures, ccontext);
	if (calledInDelayContext)
	{
		res = handleAccessDelayed_start(__FUNCTION__, operDelayedAccess);
	}
	if (res == JVX_NO_ERROR)
	{
		res = updateWindowUiElement(ccontext, passedId);
		if (res == JVX_NO_ERROR)
		{
			check_start_new_trigger_if_desired(ccontext);
			return true;
		}
		else
		{
			// Cancel start of this operaton since no update is possible
			handleAccessDelayed_stop(__FUNCTION__, operDelayedAccess);
		}
	}
	return false;
}

jvxBool
CjvxSaWrapperElementBase::updatePropertyStatus_ifavail(jvxPropertyCallContext ccontext, jvxSize* passedId)
{
	jvxCBitField operDelayedAccess = (jvxCBitField)1 << JVX_WIDGET_RWAPPER_OPERATION_UPDATE_STATUS_SHIFT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool calledInDelayContext = CHECK_NONBLOCKING_RESPONSE(this->thePropAFeatures, ccontext);
	if (calledInDelayContext)
	{
		res = handleAccessDelayed_start(__FUNCTION__, operDelayedAccess);
	}
	if (res == JVX_NO_ERROR)
	{
		updatePropertyStatus(ccontext, passedId);
		check_start_new_trigger_if_desired(ccontext);
		return true;
	}
	return false;
}

jvxBool 
CjvxSaWrapperElementBase::trigger_updateWindow_periodic_ifavail(jvxPropertyCallContext ccontext, jvxSize* passedId)
{
	jvxCBitField operDelayedAccess = (jvxCBitField)1 << JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool calledInDelayContext = CHECK_NONBLOCKING_RESPONSE(this->thePropAFeatures, ccontext);
	if (calledInDelayContext)
	{
		res = handleAccessDelayed_start(__FUNCTION__, operDelayedAccess);
	}
	if (res == JVX_NO_ERROR)
	{
		//updateWindowUiElement(ccontext, passedId);
		trigger_updateWindow_periodic(ccontext, passedId);
		check_start_new_trigger_if_desired(ccontext);
		return true;
	}
	return false;
}

void
CjvxSaWrapperElementBase::updateWindow_core(jvxPropertyCallContext ccontext )
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBitField operDelayedAccess = (jvxCBitField)1 << JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT;
	jvxBool calledInDelayContext = CHECK_NONBLOCKING_RESPONSE(this->thePropAFeatures, ccontext);
	jvxCallManagerProperties callGate;
	if (calledInDelayContext)
	{
		res = handleAccessDelayed_start(__FUNCTION__, operDelayedAccess);
		if (res != JVX_NO_ERROR)
		{
			return;
		}
		jvxPropertyCallPrivate* newPrivate = new jvxPropertyCallPrivate;
		init_propertyPrivate(newPrivate, JVX_WIDGET_RWAPPER_UPDATE_WINDOW, "", "", 0);// JVX_SIZE_UNSELECTED); <- correct?  Group id 0 means no association
		res = propRef->property_start_delayed_group(callGate, static_cast<IjvxAccessProperties_delayed_report*>(this), newPrivate,
			jPSCH(&procId));
		if (res == JVX_ERROR_COMPONENT_BUSY)
		{
			handleAccessDelayed_stop(__FUNCTION__ , operDelayedAccess);
			std::cout << __FUNCTION__ << ", ui element" << uiObjectName << ": warning: operation has been triggered before and is still waiting for results." << std::endl;
			return;
		}
		else if (res != JVX_NO_ERROR)
		{
			assert(0);
		}
		
	}

	this->updateWindowUiElement();
	
	if (calledInDelayContext)
	{
		propRef->property_stop_delayed_group(callGate, NULL, jPSCH(&this->procId));
	}
}

void
CjvxSaWrapperElementBase::updateStatus_core(jvxPropertyCallContext ccontext)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxCBitField operDelayedAccess = (jvxCBitField)1 << JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT;
	jvxBool calledInDelayContext = CHECK_NONBLOCKING_RESPONSE(this->thePropAFeatures, ccontext);
	if (calledInDelayContext)
	{
		res = handleAccessDelayed_start(__FUNCTION__, operDelayedAccess);
		if (res != JVX_NO_ERROR)
		{
			return;
		}
		jvxPropertyCallPrivate* newPrivate = new jvxPropertyCallPrivate;
		init_propertyPrivate(newPrivate, JVX_WIDGET_RWAPPER_UPDATE_WINDOW, "", "", 0);// JVX_SIZE_UNSELECTED); <- correct?  Group id 0 means no association
		res = propRef->property_start_delayed_group(callGate, static_cast<IjvxAccessProperties_delayed_report*>(this), newPrivate, jPSCH(&procId));
		if (res == JVX_ERROR_COMPONENT_BUSY)
		{
			handleAccessDelayed_stop(__FUNCTION__, operDelayedAccess);
			std::cout << __FUNCTION__ << ", ui element" << uiObjectName << ": warning: operation has been triggered before and is still waiting for results." << std::endl;
			return;
		}
		else if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

	}

	this->updatePropertyStatus(ccontext, &procId);

	if (calledInDelayContext)
	{
		propRef->property_stop_delayed_group(callGate, NULL, jPSCH(&this->procId));
	}
}
void
CjvxSaWrapperElementBase::setProperties_core_ui(jvxPropertyCallContext ccontext)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxCBitField operDelayed = (jvxCBitField)1 << JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT;
	jvxBool calledInDelayContext = CHECK_NONBLOCKING_RESPONSE(this->thePropAFeatures, ccontext);
	if (calledInDelayContext)
	{
		res = handleAccessDelayed_start(__FUNCTION__, operDelayed);
		if (res != JVX_NO_ERROR)
		{
			return;
		}
		jvxPropertyCallPrivate* newPrivate = new jvxPropertyCallPrivate;
		init_propertyPrivate(newPrivate, JVX_WIDGET_RWAPPER_SET_CONTENT, "", "", 0);// JVX_SIZE_UNSELECTED); <- correct?  Group id 0 means no association
		res = propRef->property_start_delayed_group(callGate, static_cast<IjvxAccessProperties_delayed_report*>(this), newPrivate, jPSCH(&procId));
		if (res == JVX_ERROR_COMPONENT_BUSY)
		{
			handleAccessDelayed_stop(__FUNCTION__, operDelayed);
			std::cout << __FUNCTION__ << ", ui element" << uiObjectName << ": warning: operation has been triggered before and is still waiting for results." << std::endl;
			return;
		}
		else if (res != JVX_NO_ERROR)
		{
			assert(0);
		}
	}

	setPropertiesUiElement(JVX_SIZE_UNSELECTED, JVX_WIDGET_RWAPPER_UPDATE_NORMAL);

	if (calledInDelayContext)
	{
		propRef->property_stop_delayed_group(callGate, NULL, jPSCH(&procId) );
	}
}

jvxErrorType 
CjvxSaWrapperElementBase::setProperties_core_direct(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, 
	jvxPropertyCallContext ccontext)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxCBitField operDelayed = (jvxCBitField)1 << JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT;
	jvxBool calledInDelayContext = CHECK_NONBLOCKING_RESPONSE(this->thePropAFeatures, ccontext);
	if (calledInDelayContext)
	{
		res = handleAccessDelayed_start(__FUNCTION__, operDelayed);
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
		jvxPropertyCallPrivate* newPrivate = new jvxPropertyCallPrivate;
		init_propertyPrivate(newPrivate, JVX_WIDGET_RWAPPER_SET_CONTENT, "", "", 0);// JVX_SIZE_UNSELECTED); <- correct?  Group id 0 means no association
		res = propRef->property_start_delayed_group(callGate, static_cast<IjvxAccessProperties_delayed_report*>(this), newPrivate, jPSCH(&procId) );
		if (res == JVX_ERROR_COMPONENT_BUSY)
		{
			handleAccessDelayed_stop(__FUNCTION__, operDelayed);
			std::cout << __FUNCTION__ << ", ui element" << uiObjectName << ": warning: operation has been triggered before and is still waiting for results." << std::endl;
			return res;
		}
		else if (res != JVX_NO_ERROR)
		{
			assert(0);
		}
	}

	res = setPropertiesDirect(prop, form, offset, numElements);

	if (calledInDelayContext)
	{
		propRef->property_stop_delayed_group(callGate, NULL, jPSCH(&procId));
	}
	return res;
}

jvxErrorType
CjvxSaWrapperElementBase::initializeProperty_core(jvxBool checkProperty, jvxPropertyCallContext ccontext)
{
	jvxCallManagerProperties callGate;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType res1 = JVX_NO_ERROR;
	jvxErrorType res2 = JVX_NO_ERROR;
	std::string getSetTag = propertyGetSetTag;
	jvxApiString fldStr;
	std::vector<std::string> pLstAdd;
	jvxCBitField operDelayed = 0;
	jvxBool calledInDelayContext = CHECK_NONBLOCKING_RESPONSE(this->thePropAFeatures, ccontext);

	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
	getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	if (checkProperty)
	{
		if (calledInDelayContext)
		{
			jvx_bitZSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_GET_DESCRIPTOR_SHIFT);
			jvx_bitSet(operDelayed, JVX_WIDGET_RWAPPER_OPERATION_GET_EXTENDED_INFO_SHIFT);
			res = handleAccessDelayed_start(__FUNCTION__, operDelayed);
			if (res != JVX_NO_ERROR)
			{
				return res;
			}
			jvxPropertyCallPrivate* newPrivate = new jvxPropertyCallPrivate;
			init_propertyPrivate(newPrivate, JVX_WIDGET_RWAPPER_INITIALIZE, "", "", 0); // Group id 0 means no association
			res = propRef->property_start_delayed_group(callGate, static_cast<IjvxAccessProperties_delayed_report*>(this), newPrivate, jPSCH(&procId));
			if (res == JVX_ERROR_COMPONENT_BUSY)
			{
				handleAccessDelayed_stop(__FUNCTION__, operDelayed);
				std::cout << __FUNCTION__ << ", ui element" << uiObjectName << ": warning: operation has been triggered before and is still waiting for results." << std::endl;
				return res;
			}
			else if (res != JVX_NO_ERROR)
			{
				assert(0);
			}	
		}

		jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(getSetTag.c_str());
		res1 = propRef->get_descriptor_property(callGate, thePropDescriptor, ident, jPSCH(&procId));
		res2 = propRef->get_property_extended_info(callGate, &fldStr, JVX_PROPERTY_INFO_SHOWHINT, 
			jPAD(getSetTag.c_str()), jPSCH(&procId));

		if (calledInDelayContext)
		{
			propRef->property_stop_delayed_group(callGate,NULL, jPSCH(&procId));

			if (
				(res1 == JVX_ERROR_POSTPONE) &&
				(res1 == JVX_ERROR_POSTPONE))
			{
				widgetStatus = JVX_STATE_INIT;
				return JVX_NO_ERROR;
			}
		}
		if (res1 == JVX_NO_ERROR)
		{
			if(res2 == JVX_NO_ERROR)
			{
				jvx_parsePropertyStringToKeylist(fldStr.std_str(), pLstAdd);
				// Add local elements to list
				paramLst.insert(paramLst.end(), pLstAdd.begin(), pLstAdd.end());
			}

			propDescription = thePropDescriptor.description.std_str();
			propDescriptor = thePropDescriptor.descriptor.std_str();
			propName = thePropDescriptor.name.std_str();
			ttip_core = propDescription;
			if (myBaseProps.tooltipDebug)
			{
				ttip_core += " <";
				ttip_core += uiRef->accessibleDescription().toLatin1().data();
				ttip_core += ">";
			}
			ttip_show = ttip_core;
			uiRef->setToolTip(ttip_show.c_str());
			widgetStatus = JVX_STATE_SELECTED;
			isUnchecked = false;

			updateUiDescriptor(callGate.access_protocol);
		}
		else
		{
			widgetStatus = JVX_STATE_NONE;
			std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << propertyGetSetTag << ", error reason: " << jvxErrorType_txt(res1) << std::endl;
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}

	}
	else
	{
		//std::cout << "Object " << uiObjectName << " which is linked with property " << propertyGetSetTag << " remains unchecked." << std::endl;
		isUnchecked = true;
		widgetStatus = JVX_STATE_SELECTED;
	}
	return res;
}

// ====================================================================================
// =============================================================================================

jvxErrorType
CjvxSaWrapperElementBase::report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr)
{
	jvxPropertyCallPrivate* myPriv = (jvxPropertyCallPrivate*)privatePtr;
	
	/* We end up here if we have initiated the HTTP request from within a single element */
	switch (myPriv->delayedCallPurpose)
	{
	case JVX_WIDGET_RWAPPER_UPDATE_WINDOW:
		emit emit_trigger_updateWindow();
		break;
	case JVX_WIDGET_RWAPPER_UPDATE_STATUS:
		emit emit_trigger_updateStatus();
		break;
	case JVX_WIDGET_RWAPPER_INITIALIZE:
		emit emit_reenter_delayed_initialize();
		break;
	case JVX_WIDGET_RWAPPER_SET_CONTENT:
		emit emit_send_content();
		break;
	case JVX_WIDGET_RWAPPER_GET_CONTENT:
		assert(0); // A single GET does not happen typically since it is always in updateWindow context
		break;

	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSaWrapperElementBase::report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr)
{
	jvxPropertyCallPrivate* myPriv = (jvxPropertyCallPrivate*)privatePtr;
	// This callback being called if a delayed update process is complete

	// Reset the process id.
	assert(uniqueId == procId);
	this->procId = 0;

	delete myPriv; // We can only delete this here since the privatePointer in report_nonblocking_delayed_update_complete is not emitted in the slot
	return JVX_NO_ERROR;
}

void
CjvxSaWrapperElementBase::slot_reenter_delayed_initialize()
{
#ifdef DEBUG_WRAPPER_ELEMENT_BASE
	std::cout << __FUNCTION__ << ": Tag = " << tag << std::endl;
#endif
	this->initializeUiElement(JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
	check_start_new_trigger_if_desired(JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
}

void
CjvxSaWrapperElementBase::slot_trigger_updateWindow()
{
#ifdef DEBUG_WRAPPER_ELEMENT_BASE
	std::cout << __FUNCTION__ << ": Tag = " << tag << std::endl;
#endif
	updateWindowUiElement(JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
	check_start_new_trigger_if_desired(JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
}

void
CjvxSaWrapperElementBase::slot_trigger_updateStatus()
{
#ifdef DEBUG_WRAPPER_ELEMENT_BASE
	std::cout << __FUNCTION__ << ": Tag = " << tag << std::endl;
#endif
	updatePropertyStatus(JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
	check_start_new_trigger_if_desired(JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
}

void
CjvxSaWrapperElementBase::slot_send_content()
{
	setPropertiesUiElement(JVX_SIZE_UNSELECTED, JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
	check_start_new_trigger_if_desired(JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
}

jvxErrorType 
CjvxSaWrapperElementBase::triggerDirect(const std::string& propertyDescr,
	jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	const std::string& uiFilterWildcard)
{
	if (propertyDescr != propertyGetSetTag)
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	if (!uiFilterWildcard.empty())
	{
		if (!jvx_compareStringsWildcard(uiFilterWildcard, uiObjectName))
		{ 
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}

	return setProperties_core_direct(prop, form, offset, numElements);
}

jvxErrorType 
CjvxSaWrapperElementBase::set_config(jvxWwConfigType tp, jvxHandle* load)
{
	return JVX_NO_ERROR;
}

void
CjvxSaWrapperElementBase::updateUiDescriptor(jvxAccessProtocol accProt)
{
	if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ( thePropDescriptor.accessType))
	{
		mainWidgetDisable();
		QFont ft = uiRef->font();
		ft.setItalic(true);
		uiRef->setFont(ft);
		ttip_show = ttip_core + " - read only -";
	}
	else
	{
		if (!myBaseProps.supersede_enable)
		{
			mainWidgetEnable();
		}
		QFont ft = uiRef->font();
		ft.setItalic(false);
		uiRef->setFont(ft);
		ttip_show = ttip_core;
	}

	// Check possible access errors
	if(accProt != JVX_ACCESS_PROTOCOL_OK)
	{
		mainWidgetDisable();
		QFont ft = uiRef->font();
		ft.setItalic(true);
		uiRef->setFont(ft);
		ttip_show = ttip_core + ", protocol = " + jvxAccessProtocol_txt(accProt);
		errorState = true;
	}
	uiRef->setToolTip(ttip_show.c_str());
}

void
CjvxSaWrapperElementBase::trigger_updateWindow_periodic(jvxPropertyCallContext ccontext, jvxSize* passedId)
{
	updateWindowUiElement(ccontext, passedId, true);
	// updateWindow_core();
}

jvxErrorType
CjvxSaWrapperElementBase::handleAccessDelayed_start(const char* callEnter, jvxCBitField oper)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool reportError = false;
	jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
	if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
	{
		embLog.jvxrtst << __FUNCTION__ << " entering from " << callEnter << ": Status on Enter: " << getDelayedStatusExpression(delayedActionOperations) << "::" <<
			getDelayedStatusExpression(delayedActionPending) << std::endl;
	}

	if(delayedActionOperations)
	{
		if (
			jvx_bitTest(delayedActionOperations, JVX_WIDGET_RWAPPER_OPERATION_GET_DESCRIPTOR_SHIFT) ||
			jvx_bitTest(delayedActionOperations, JVX_WIDGET_RWAPPER_OPERATION_GET_EXTENDED_INFO_SHIFT))
		{
			if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
			{
				embLog.jvxrtst << __FUNCTION__ << "--" << this->propertyGetSetTag << ": Request of parameter <" << this->propertyGetSetTag 
					<< "> not possible due to pending operation " <<
					getDelayedStatusExpression(oper) << "." << std::endl;
			}
			res = JVX_ERROR_UNEXPECTED;
		}
		if (
			jvx_bitTest(delayedActionOperations, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT) ||
			jvx_bitTest(delayedActionOperations, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_STATUS_SHIFT))
		{
			if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
			{
				embLog.jvxrtst << __FUNCTION__ << "--" << this->propertyGetSetTag << ": Request of parameter <" << this->propertyGetSetTag
					<< "> not possible due to pending operation " <<
					getDelayedStatusExpression(oper) << "." << std::endl;
			}
			if (
				jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT)||
				jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_STATUS_SHIFT))
			{
				if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					embLog.jvxrtst << __FUNCTION__ << "--" << this->propertyGetSetTag << ": Added update to be triggered once current operation is complete." << std::endl;
				}

				// We add the update to be done once the previous operation is complete
				// If another update is in progress, we will store another update but not more than one
				// to prevent too many updates. delayedActionPending is evaluated in 
				// check_start_new_trigger_if_desired which is caled after each completed transfer 
				delayedActionPending = delayedActionPending | oper;
				res = JVX_ERROR_POSTPONE;
			}
			if (jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT))
			{
				if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					embLog.jvxrtst << __FUNCTION__ << "--" << this->propertyGetSetTag << ": Operation to set a value is currently not possible." << std::endl;
				}
				res = JVX_ERROR_COMPONENT_BUSY;
			}
			if (
				jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_GET_DESCRIPTOR_SHIFT) ||
				jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_GET_EXTENDED_INFO_SHIFT))
			{
				if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					embLog.jvxrtst << __FUNCTION__ << "--" << this->propertyGetSetTag << ": Operation for initialization is unexpetected." << std::endl;
				}
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		if (
			jvx_bitTest(delayedActionOperations, JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT))
		{
			if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
			{
				embLog.jvxrtst << __FUNCTION__ << "--" << this->propertyGetSetTag << ": Request of parameter <" << this->propertyGetSetTag
					<< "> not possible due to pending operation " << 
					getDelayedStatusExpression(oper) << "." << std::endl;
			}
			if (
				jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT) ||
				jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_STATUS_SHIFT))
			{
				if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					embLog.jvxrtst << __FUNCTION__ << "--" << this->propertyGetSetTag << ": Added update to be triggered once current operation is complete." << std::endl;
				}
				delayedActionPending = delayedActionPending | oper;
				res = JVX_ERROR_POSTPONE;
			}
			if (jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT))
			{
				if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					embLog.jvxrtst << __FUNCTION__ << "--" << this->propertyGetSetTag << ": Operation to set a value is currently not possible." << std::endl;
				}
				res = JVX_ERROR_COMPONENT_BUSY;
			}
			if (
				jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_GET_DESCRIPTOR_SHIFT) ||
				jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_GET_EXTENDED_INFO_SHIFT))
			{
				if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					embLog.jvxrtst << __FUNCTION__ << "--" << this->propertyGetSetTag << ": Operation for initialization is unexpetected." << std::endl;
				}
				res = JVX_ERROR_UNEXPECTED;
			}
		}

#ifdef JVX_DEBUG_DELAYED_CONTROL
		std::cout << "Entering from " << callEnter << ":" << uiObjectName << "--" << this->propertyGetSetTag << ": Detected access to ui element which is already in use: pending operations: " << std::flush;
		std::cout << getDelayedStatusExpression(delayedActionOperations);
		std::cout << ", due to detected error, set pending flags to ";
		std::cout << getDelayedStatusExpression(delayedActionPending);
		std::cout << std::endl;
#endif

	}
	if (res == JVX_NO_ERROR)
	{
		delayedActionName = callEnter;
		delayedActionOperations |= oper;
		jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
		if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
		{
			embLog.jvxrtst << __FUNCTION__ << ": Status on Leave: " << getDelayedStatusExpression(delayedActionOperations) << "::" <<
				getDelayedStatusExpression(delayedActionPending) << std::endl;
		}

	}
	return res;
}

void
CjvxSaWrapperElementBase::handleAccessDelayed_stop(const char* callEnter, jvxCBitField oper)
{
	jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
	if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
	{
		embLog.jvxrtst << __FUNCTION__ << " entering from " << callEnter << ": Status on Enter: " << getDelayedStatusExpression(delayedActionOperations) << "::" <<
			getDelayedStatusExpression(delayedActionPending) << std::endl;
	}

	delayedActionName = "";
	delayedActionOperations &= ~oper;


	if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
	{
		embLog.jvxrtst <<  __FUNCTION__ << ": Status on Leave: " << getDelayedStatusExpression(delayedActionOperations) << "::" << 
			getDelayedStatusExpression(delayedActionPending) << std::endl;
	}

}

std::string
CjvxSaWrapperElementBase::getDelayedStatusExpression(jvxCBitField oper)
{
	std::string txt = "<"; 
	if (jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_GET_DESCRIPTOR_SHIFT))
	{
		txt += "d";
	}
	if (jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_GET_EXTENDED_INFO_SHIFT))
	{
		txt += "e";
	}
	if (jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT))
	{
		txt += "s";
	}
	if (jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT))
	{
		txt += "u";
	}
	if (jvx_bitTest(oper, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_STATUS_SHIFT))
	{
		txt += "p";
	}
	txt += ">";
	return txt;
}

void 
CjvxSaWrapperElementBase::check_start_new_trigger_if_desired(jvxPropertyCallContext ccontext)
{
	if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
	{
		jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
		if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
		{
			embLog.jvxrtst << __FUNCTION__ << ": Entered since delayed operation is complete, " <<
				getDelayedStatusExpression(delayedActionOperations) << "::" <<
				getDelayedStatusExpression(delayedActionPending) << std::endl;
		}
		if (delayedActionPending)
		{
			if (procId == 0)
			{
#ifdef JVX_DEBUG_DELAYED_CONTROL
				std::cout << __FUNCTION__ << ":" << uiObjectName << ": Post-running pending operations " << std::flush;
				std::cout << getDelayedStatusExpression(delayedActionPending);
				std::cout << ".";
				std::cout << std::endl;
#endif
				loopCheckCnt = 0;				
				// This moves the calling context from global to local if required
				if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					embLog.jvxrtst << __FUNCTION__ << ": Pending operation is awaiting service." << std::endl;
				}
				if (jvx_bitTest(delayedActionPending, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT))
				{
					/*
					std::cout << "Reentering pending operations due to pending action " << std::flush;
					std::cout << getDelayedStatusExpression(delayedActionOperations);
					std::cout << std::endl;
					*/
					if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
					{
						embLog.jvxrtst << __FUNCTION__ << " Reentering update window since it was rejected before." << std::endl;
					}
					jvx_bitClear(delayedActionPending, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT);
					updateWindow_core(JVX_WIDGET_RWAPPER_UPDATE_NORMAL);
				}
				if (jvx_bitTest(delayedActionPending, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_STATUS_SHIFT))
				{
					if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
					{
						embLog.jvxrtst << __FUNCTION__ << " Reentering update status since it was rejected before." << std::endl;
					}
					jvx_bitClear(delayedActionPending, JVX_WIDGET_RWAPPER_OPERATION_UPDATE_STATUS_SHIFT);
					updateStatus_core(JVX_WIDGET_RWAPPER_UPDATE_NORMAL);
				}
				if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					embLog.jvxrtst << __FUNCTION__ << ": Pending operation has been triggered." << std::endl;
				}
			}
			else
			{
				loopCheckCnt++;
			
#ifdef JVX_DEBUG_DELAYED_CONTROL
				std::cout << __FUNCTION__ << ":" << uiObjectName << ": Not able to post-run pending operations " << std::flush;
				std::cout << getDelayedStatusExpression(delayedActionPending);
				std::cout << ", loop counter = " << loopCheckCnt;
				std::cout << std::endl;
#endif

				if (loopCheckCnt >= JVX_LOOP_CHECK_COUNTER_MAX)
				{
					std::cout << __FUNCTION__ << ": Detected number of property access retries exception, exceded the number of <"
						<< JVX_LOOP_CHECK_COUNTER_MAX << " - giving up with pending " << getDelayedStatusExpression(delayedActionPending) <<
						std::endl;
					delayedActionPending = 0;
					loopCheckCnt = 0;
				}
				if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					embLog.jvxrtst << __FUNCTION__ << ": Pending operation is awaiting service but the channel is curerently not available, loop counter is." << std::endl;
				}
			}
		}
	}
}

void
CjvxSaWrapperElementBase::mainWidgetEnable()
{
	uiRef->setProperty("prop_enable", QVariant(true));
	if (!myBaseProps.supersede_enable)
	{
		uiRef->setEnabled(true);
	}
}

void
CjvxSaWrapperElementBase::mainWidgetDisable()
{
	uiRef->setProperty("prop_enable", QVariant(false));
	uiRef->setEnabled(false);
}

jvxBool
CjvxSaWrapperElementBase::local_command(std::string& local_command)
{
	std::cout << "Request to execute local command <local_command> - command is not valid." << std::endl;
	return false;
}