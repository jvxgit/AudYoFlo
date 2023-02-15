#include "CjvxQtSaWidgetWrapper.h"
#include "jvxSaWidgetWrapper_elements.h"
#include "CjvxMaWrapperElementTreeWidget.h"
#include <QVariant>
#include <QComboBox>
#include <QClipboard>
#include <QApplication>

#define JVX_USER_ROLE_TREEWIDGET_HOOK (Qt::UserRole + 1)
#define JVX_USER_ROLE_TREEWIDGET_PROPERTY_COMPONENT_TAG (Qt::UserRole + 2)
#define JVX_USER_ROLE_TREEWIDGET_PROPERTY_DESCRIPTOR_TAG (Qt::UserRole + 3)
#define JVX_USER_ROLE_TREEWIDGET_PROPERTY_PARAM_LIST (Qt::UserRole + 4)
#define JVX_USER_ROLE_TREEWIDGET_PROPERTY_DESCR (Qt::UserRole + 5)
#define JVX_USER_ROLE_TREEWIDGET_PROPERTY_PARAM_BASE (Qt::UserRole + 6)
#define JVX_USER_ROLE_TREEWIDGET_PROPERTY_ADDED_ON_ASSOCIATE (Qt::UserRole + 7)
#define JVX_USER_ROLE_TREEWIDGET_SUBWIDGET 1

Q_DECLARE_METATYPE(jvxPropertyDescriptor);
Q_DECLARE_METATYPE(QTreeWidgetItem*);
Q_DECLARE_METATYPE(CjvxQtSaWidgetWrapper_elementbase*);
Q_DECLARE_METATYPE(basePropInfos);

CjvxMaWrapperElementTreeWidget::CjvxMaWrapperElementTreeWidget(QTreeWidget* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
	IjvxAccessProperties* propRefIn, std::string propertyGetSetTagLoc, std::vector<std::string> paramLstLoc,
	const std::string& tag_cp, const std::string& tag_key, const std::string& uiObjectNameLoc, const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef):
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn, propertyGetSetTagLoc, 
		paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId, hostRef)
{
	uiRefTp = uiRefLoc;
	initializeUiElement();
}

CjvxMaWrapperElementTreeWidget::~CjvxMaWrapperElementTreeWidget()
{
	if(widgetStatus > JVX_STATE_NONE)
	{
		//disconnect(uiRefTp, SIGNAL(clicked()));
	}
	QVariant var = uiRefTp->property("JVX_THE_SUBWIDGET");
	if (var.isValid())
	{
		CjvxQtSaWidgetWrapper_elementbase* oldWidget = var.value<CjvxQtSaWidgetWrapper_elementbase*>();
		removeCurrentWidget(oldWidget, true);
	}
}

void 
CjvxMaWrapperElementTreeWidget::initializeUiElement(jvxPropertyCallContext ccontext)
{
	jvxSize i;
	std::string prefixL = "";
	numEntries = 0;
	numEntriesRTUpdate = 0;
	if (widgetStatus == JVX_STATE_NONE)
	{
		jvxErrorType res = this->initParameters_getMin(&backwardRef->keymaps.cb, paramLst);
		assert(res == JVX_NO_ERROR);
	}

	initializeProperty_core(false, ccontext);
	
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		uiRefTp->setContextMenuPolicy(Qt::CustomContextMenu);
		uiRefTp->expandAll();

		jvxSize numTLItems = uiRefTp->topLevelItemCount();
		for (i = 0; i < numTLItems; i++)
		{
			QTreeWidgetItem *theTLItem = uiRefTp->topLevelItem((int)i);
			if (theTLItem)
			{
				theTLItem->setExpanded(true);
				processLeefs(theTLItem, 0, prefixL, numEntries, numEntriesRTUpdate);
			}
		}

		QObject::connect(uiRefTp, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(treeWidgetItemDblClicked(QTreeWidgetItem*, int)));
		QObject::connect(uiRefTp, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(treeWidgetContextMenuRequest(const QPoint & )));

		//QObject::connect(uiRefTp, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(treeWidgetPressed(QTreeWidgetItem*, int)));


		/*
		if(
		(thePropDescriptor.format == JVX_DATAFORMAT_16BIT_LE) &&
		(thePropDescriptor.decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF))
		{
		tp = JVX_WW_PUSHBUTTON_CBOOL;
		}
		else if(
		(thePropDescriptor.format == JVX_DATAFORMAT_SELECTION_LIST))
		{
		tp = JVX_WW_PUSHBUTTON_SELLIST;
		}
		else if (
		(thePropDescriptor.format == JVX_DATAFORMAT_STRING))
		{
		tp = JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY;
		}
		else
		{
		std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor << ": Incorrect property type, expected JVX_DATAFORMAT_16BIT_LE + JVX_PROPERTY_DECODER_SIMPLE_ONOFF or JVX_DATAFORMAT_SELECTION_LIST" << std::endl;
		isReadyVar = false;
		}
		*/
	}

	if (widgetStatus == JVX_STATE_SELECTED)
	{
		/*
		for(i = 0; i < paramLst.size(); i++)
		{
		std::string assignment;
		std::string value;
		std::string oneToken = paramLst[i];
		size_t posi = oneToken.find("=");
		if(posi != std::string::npos)
		{
		assignment = oneToken.substr(0, posi);
		value = oneToken.substr(posi+1, std::string::npos);

		if(assignment == "PICON")
		{
		namePicOn = value;
		namePicOn = jvx_replaceStrInStr(namePicOn, "QRC", ":");
		}
		if(assignment == "PICOFF")
		{
		namePicOff = value;
		namePicOff = jvx_replaceStrInStr(namePicOff, "QRC", ":");
		}
		if(assignment == "PICPREFIX")
		{
		prefixPics = value;
		prefixPics = jvx_replaceStrInStr(prefixPics, "QRC", ":");
		}
		if (assignment == "BEHAVIOR")
		{
		if (tp == JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY)
		{
		if (value == "JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD")
		{
		tp = JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD;
		}
		if (value == "JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE")
		{
		tp = JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE;
		}
		}
		}
		}
		}*/
	}
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		/*
		if(!namePicOn.empty())
		{
		if(!pixmapOn.load(namePicOn.c_str()))
		{
		std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor << ": Unable to open picture " << namePicOn << " for icon." << std::endl;
		}
		}
		if(!namePicOff.empty())
		{
		if(!pixmapOff.load(namePicOff.c_str()))
		{
		std::cout << "--> " << uiRef->metaObject()->className() << " " << uiRef->objectName().toLatin1().data() << ":: Property " << propDescriptor << ": Unable to open picture " << namePicOff << " for icon." << std::endl;
		}
		}
		*/
		uiRefTp->setProperty("mySaWidgetRefTreeWidget", QVariant::fromValue<CjvxMaWrapperElementTreeWidget*>(this));
		//connect(uiRefTp, SIGNAL(clicked()), this, SLOT(button_pushed()));

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			assert(0);
		}

		updateWindowUiElement();

		assert(backwardRef);
		backwardRef->reportPropertyInitComplete(selector_lst.c_str(), assoc_id);
	}
}

void
CjvxMaWrapperElementTreeWidget::processLeefs(QTreeWidgetItem *theItem, int cnt, const std::string& prefix, jvxSize& numEntries, jvxSize& numEntriesRTUpdate)
{
	jvxSize i,j;
	QString hook_text_q;
	QString token;
	std::string prefixL;
	std::string propText;
	jvxApiString fldStr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxPropertyContext ctxt = JVX_PROPERTY_CONTEXT_UNKNOWN;
	jvxInt32 intVal;
	jvxBool foundit;
	std::string locTag;
	std::string locProps;
	std::vector<std::string> param;
	QVariant var;
	jvxCallManagerProperties callGate;
	int num = theItem->childCount();

	hook_text_q = theItem->text(1);
	propText = hook_text_q.toLatin1().data();
	//jvx_parsePropertyLinkDescriptor(propText, locTag, locProps, param);
	//propText = jvx_createPropertyLinkDescriptor(locTag, jvx_makePathExpr(propPrefix, locProps), param);
	prefixL = prefix + propText;
	theItem->setData(0, JVX_USER_ROLE_TREEWIDGET_HOOK, QVariant(hook_text_q));;

	if (num > 0)
	{
		theItem->setText(1, "");

		//std::cout << jvx_loopify("--", cnt) << "> Node " << theItem->text(0).toLatin1().data() << ":" << std::endl;

		for (i = 0; i < num; i++)
		{

			QTreeWidgetItem *theNext = theItem->child(i);
			if (theNext)
			{
				processLeefs(theNext, cnt + 1, prefixL, numEntries, numEntriesRTUpdate);
			}
		}
	}
	else
	{
		numEntries++;
		prefixL = prefix + propText;
		if (!prefixL.empty())
		{
			std::string propname, idtag;
			std::vector<std::string> paramlstloc;
			QStringList qlst;
			jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;

			jvx_parsePropertyLinkDescriptor(prefixL, idtag, propname, paramlstloc);

			jvxBool doLink = false;
			if (jvx_compareStringsWildcard(idtag, tag))
			{
				doLink = true;
				idtag = tag;
			}
			if (doLink)
			{
				theItem->setText(1, "");
				if (jvx_isStringsWildcard(prefixL))
				{
					// Add those tree widgets which match the wildcard
					jvxBool isValidPropRef = false;
					propRef->reference_status(NULL, &isValidPropRef);
					if (isValidPropRef)
					{
						jvxSize numP = 0;
						jvxErrorType resP = JVX_NO_ERROR;
						std::string newDescriptor, newDescription;
						QTreeWidgetItem* newItem = NULL;
						propRef->get_number_properties(callGate, &numP);

						std::string getSetTag = jvx_makePathExpr(propPrefix, propname);
						for (j = 0; j < numP; j++)
						{							
							jvx::propertyAddress::CjvxPropertyAddressLinear ident(j);
							resP = propRef->get_descriptor_property(callGate, theDescr, ident);
							if (resP == JVX_NO_ERROR)
							{
								newDescriptor = theDescr.descriptor.std_str();
								if (jvx_compareStringsWildcard(getSetTag, newDescriptor))
								{
									// Wildcard match, add an element here
									newItem = NULL;
									newItem = new QTreeWidgetItem(theItem);
									newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_ADDED_ON_ASSOCIATE, QVariant(true));
									newDescription = theDescr.description.std_str();
									if (newDescription.empty())
									{
										newDescription = "<" + newDescriptor + ">";
									}
									if (!theDescr.pTag.empty())
									{
										newDescription = theDescr.pTag.std_str() + ": " + newDescription;
									}
									newItem->setText(0, newDescription.c_str());

									newDescription.clear();
									jvxSize gId = JVX_SIZE_UNSELECTED;
									resP = propRef->get_property_extended_info(callGate, &gId, JVX_PROPERTY_INFO_GROUPID, ident);
									if (JVX_CHECK_SIZE_SELECTED(gId))
									{
										jvxApiString astr;
										propRef->translate_group_id(callGate, gId, astr);
										newDescription += astr.std_str();
									}
									else
									{
										newDescription += "--";
									}
									newItem->setText(3, newDescription.c_str());

									std::string fld = jvx_createPropertyLinkDescriptor(idtag, newDescriptor, paramlstloc);
									newItem->setText(1, fld.c_str());
									
									processLeefs(newItem, cnt + 1, "", numEntries, numEntriesRTUpdate);

									// All new items to be shown expanded
									newItem->setExpanded(true);
								}
							}

						}
					}
				}
				else
				{
					jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(propname.c_str());
					res = propRef->get_descriptor_property(callGate, theDescr, ident);
					if (res == JVX_NO_ERROR)
					{
						// Clear what is shown
						theItem->setText(1, "");

						token = tag.c_str();
						theItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_COMPONENT_TAG, QVariant(token));

						// Add prefix to property entry
						std::string getSetTag = jvx_makePathExpr(propPrefix, propname);

						// Special rule: If we have expanded link entries before, the prefix is already part of it
						var = theItem->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_ADDED_ON_ASSOCIATE);
						if (var.isValid())
						{
							if (var.toBool())
							{
								getSetTag = propname;
							}
						}

						token = getSetTag.c_str();
						theItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_DESCRIPTOR_TAG, QVariant(token));

						res = propRef->get_property_extended_info(callGate , &fldStr, JVX_PROPERTY_INFO_SHOWHINT, 
							jPAGID(theDescr.globalIdx, theDescr.category));
						if (res == JVX_NO_ERROR)
						{
							std::vector<std::string> addlst;
							jvx_parsePropertyStringToKeylist(fldStr.std_str(), addlst);
							paramlstloc.insert(paramlstloc.end(), addlst.begin(), addlst.end());
						}
						for (i = 0; i < paramlstloc.size(); i++)
						{
							qlst.push_back(paramlstloc[i].c_str());
						}
						theItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_PARAM_LIST, QVariant(qlst));

						theItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_DESCR, QVariant::fromValue(theDescr));
						
						res = propRef->get_property_extended_info(callGate,&fldStr, JVX_PROPERTY_INFO_MOREINFO, 
							jPAGID(theDescr.globalIdx, theDescr.category));
						if (res == JVX_NO_ERROR)
						{
							theItem->setToolTip(0, fldStr.c_str());
						}

						basePropInfos myBasePropIs;
						CjvxSaWrapperElementBase::resetBasePropInfos(myBasePropIs);

						res = backwardRef->keymaps.ao.parseEntryList(paramlstloc, prefixL);
						if (res == JVX_NO_ERROR)
						{
							backwardRef->keymaps.ao.getValueForKey("RTUPDATE", &myBasePropIs.updateRt, JVX_WW_KEY_VALUE_TYPE_BOOL);
							backwardRef->keymaps.ao.getValueForKey("VERBOSE", &myBasePropIs.verbose_out, JVX_WW_KEY_VALUE_TYPE_BOOL);
							backwardRef->keymaps.ao.getValueForKey("REPORT", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
							if (foundit)
							{
								myBasePropIs.reportTp = (jvxWwReportType)intVal;
							}
							theItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_PARAM_BASE, QVariant::fromValue(myBasePropIs));
						}

						if (myBasePropIs.updateRt)
						{
							myBaseProps.updateRt = true;
							numEntriesRTUpdate++;
						}
						if (myBasePropIs.verbose_out)
						{
							std::cout << "Treewidgetitem connected: " << "--> tag = " << tag << ";  dyn prefix = " << propertyGetSetTag << "; property = " << prefixL << std::endl;
						}
					}
				}
			}
		}
		//std::cout <<  << "X Leef " << theItem->text(0).toLatin1().data() << ": " << token.toLatin1().data() << std::endl;
	}
}


void
CjvxMaWrapperElementTreeWidget::reportPropertySetItem(const char* tag, const char* descror, jvxWwReportType tp, QTreeWidgetItem* thisisme, jvxErrorType res_in_call)
{
	jvxWwReportType tp_ss = JVX_MAX(myBaseProps.reportTp, tp);
	if (tp_ss == JVX_WW_REPORT_ELEMENT)
	{
		updateWindowUiElement(thisisme, 1, false);
	}
	else
	{
		backwardRef->reportPropertySet(tag, descror, myBaseProps.group_id_emit, tp, static_cast<CjvxSaWrapperElementBase*>(this), res_in_call);
	}
}

void
CjvxMaWrapperElementTreeWidget::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	std::string getSetTag = propertyGetSetTag;
	jvxBool condSet = false;
	jvxPropertyDescriptor descr;

	/*getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
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
		uiRefTp->setEnabled(false);
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
CjvxMaWrapperElementTreeWidget::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* globalId, jvxBool call_periodic_update)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBool valB;
	jvxSelectionList selLst;

	jvxSize numTLItems = uiRefTp->topLevelItemCount();

	auto qV = uiRefTp->property("search-tree");
	if (qV.isValid())
	{
		token_search = qV.toString().toLatin1().data();
	}

	for (i = 0; i < numTLItems; i++)
	{
		QTreeWidgetItem *theTLItem = uiRefTp->topLevelItem((int)i);
		if (theTLItem)
		{
			theTLItem->setExpanded(true);
			updateWindowUiElement(theTLItem, 0, call_periodic_update);
		}
	}
	return JVX_NO_ERROR;
}

jvxBool
CjvxMaWrapperElementTreeWidget::getAllTagInformation(QTreeWidgetItem* theItem, std::string& compTag, std::string& propName, std::vector<std::string>& lst, jvxPropertyDescriptor& propD, basePropInfos& myBasePropIs)
{
	jvxSize i;
	bool everythingOK = true;
	QVariant var;
	QString varQ;
	QStringList qlst;
	lst.clear();
	var = theItem->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_COMPONENT_TAG);
	if (var.isValid())
	{
		varQ = var.toString();
		compTag = varQ.toLatin1().data();
	}
	else
	{
		everythingOK = false;
	}

	var = theItem->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_DESCRIPTOR_TAG);
	if (var.isValid())
	{
		varQ = var.toString();
		propName = varQ.toLatin1().data();
	}
	else
	{
		everythingOK = false;
	}

	var = theItem->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_PARAM_LIST);
	if (var.isValid())
	{
		qlst = var.toStringList();
		for (i = 0; i < qlst.size(); i++)
		{
			lst.push_back(qlst[i].toLatin1().data());
		}
	}
	else
	{
		everythingOK = false;
	}

	var = theItem->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_DESCR);
	if (var.isValid())
	{
		propD = var.value<jvxPropertyDescriptor>();
	}
	else
	{
		everythingOK = false;
	}

	var = theItem->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_PARAM_BASE);
	if (var.isValid())
	{
		myBasePropIs = var.value<basePropInfos>();
	}
	else
	{
		everythingOK = false;
	}
	return everythingOK;
}

void
CjvxMaWrapperElementTreeWidget::updateWindowUiElement(QTreeWidgetItem *theItem, int cnt, jvxBool call_periodic_update)
{
	jvxSize i;
	std::string getSetTag;
	
	jvxApiString loc_descror;
	jvxApiString loc_descrion;
	jvxApiString loc_mdule;
	jvxComponentIdentification loc_cpid;
	std::string ttiptext;
	jvxCallManagerProperties callGate;
	if(theItem->isExpanded())
	{
		int num = theItem->childCount();
		if (num > 0)
		{
			for (i = 0; i < num; i++)
			{

				QTreeWidgetItem *theNext = theItem->child(i);
				if (theNext)
				{
					updateWindowUiElement(theNext, cnt + 1, call_periodic_update);
				}
			}
		}
		else
		{
			std::string compTag, propName;
			std::vector<std::string> lst;
			jvxPropertyDescriptor descrLoc;
			jvxPropertyDescriptor propD;
			bool everythingOk = true;
			jvxHandle* ptrVal = NULL;
			jvxData valD = 0;
			jvxInt16 valI16 = 0;
			jvxInt8 valI8 = 0;
			jvxInt32 valI32 = 0;
			jvxInt64 valI64 = 0;
			jvxInt16 valUI16 = 0;
			jvxInt8 valUI8 = 0;
			jvxInt32 valUI32 = 0;
			jvxInt64 valUI64 = 0;
			jvxSize valS = 0;
			jvxCBool valB = false;
			jvxSelectionList selLst;
			jvxApiString  fldStr;
			jvxValueInRange valR;
			jvxErrorType resP = JVX_NO_ERROR;
			jvxWwTransformValueType transtp = JVX_WW_SHOW_DIRECT;
			std::string unit;
			std::string txtShow;
			std::string txtShowPP;
			jvxInt32 numDigits = 4;
			jvxInt32 intVal;
			jvxBool foundit;
			jvxData minVal = JVX_DATA_MAX_NEG;
			jvxData maxVal = JVX_DATA_MAX_POS;
			basePropInfos myBasePropIs;
			jvxBool typeNothandled = false;
			jvxBool showThis = true;

			everythingOk = getAllTagInformation(theItem, compTag, propName, lst, propD, myBasePropIs);

			theItem->setHidden(false);
			if (!token_search.empty())
			{
				std::vector<std::string> allStrs;
				jvx_parseStringListIntoTokens(token_search, allStrs, ' ');
				theItem->setHidden(true);
				for (auto elm : allStrs)
				{
					if (!elm.empty())
					{
						std::string tokenWC = jvx_toUpper(elm);
						tokenWC = "*" + tokenWC + "*";
						std::string cmp = jvx_toUpper(propD.description.std_str());
						if (jvx_compareStringsWildcard(tokenWC, cmp))
						{
							theItem->setHidden(false);
							break;
						}
					}
				}
			}
			if (call_periodic_update && !myBasePropIs.updateRt)
			{
				showThis = false;
			}

			if (everythingOk)
			{
				if (showThis)
				{
					std::string getSetTag = propName; // The global tag was added at startup already!
					jvxErrorType res = JVX_NO_ERROR;
					jvxBool is_valid = true;
					jvxPropertyAccessType accTp = JVX_PROPERTY_ACCESS_NONE;
					callGate.s_push();

					callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_GET_ACCESS_DATA;
					callGate.on_get.is_valid = &is_valid;
					callGate.on_get.prop_access_type = &accTp;

					jvx::propertyAddress::CjvxPropertyAddressGlobalId identId(propD.globalIdx, propD.category);

					ident.reset(getSetTag.c_str());
					trans.reset(false, 0, JVX_PROPERTY_DECODER_NONE);
					propRef->get_property(callGate, jPRG( NULL, 0, JVX_DATAFORMAT_NONE), ident, trans );
					callGate.s_pop();

					if (res == JVX_NO_ERROR)
					{
						callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC;

						if (!getSetTag.empty())
						{
							if (propD.num == 1)
							{
								// Currently, only single elements are supported
								switch (propD.format)
								{
								case JVX_DATAFORMAT_SIZE:
									ptrVal = &valS;
									break;
								case JVX_DATAFORMAT_DATA:
									ptrVal = &valD;
									break;
								case JVX_DATAFORMAT_16BIT_LE:
									ptrVal = &valI16;
									break;
								case JVX_DATAFORMAT_8BIT:
									ptrVal = &valI8;
									break;
								case JVX_DATAFORMAT_32BIT_LE:
									ptrVal = &valI32;
									break;
								case JVX_DATAFORMAT_64BIT_LE:
									ptrVal = &valI64;
									break;
								case JVX_DATAFORMAT_U16BIT_LE:
									ptrVal = &valUI16;
									break;
								case JVX_DATAFORMAT_U8BIT:
									ptrVal = &valUI8;
									break;
								case JVX_DATAFORMAT_U32BIT_LE:
									ptrVal = &valUI32;
									break;
								case JVX_DATAFORMAT_U64BIT_LE:
									ptrVal = &valUI64;
									break;
								case JVX_DATAFORMAT_SELECTION_LIST:
									ptrVal = &selLst;
									break;
								case JVX_DATAFORMAT_STRING:
									ptrVal = &fldStr;
									break;
								case JVX_DATAFORMAT_STRING_LIST:
									ptrVal = &selLst.strList;
									break;
								case JVX_DATAFORMAT_VALUE_IN_RANGE:
									ptrVal = &valR;
									break;
								case JVX_DATAFORMAT_HANDLE:
								case JVX_DATAFORMAT_BYTE:
								case JVX_DATAFORMAT_LIMIT:
								case JVX_DATAFORMAT_NONE:
								case JVX_DATAFORMAT_POINTER:
								case JVX_DATAFORMAT_CALLBACK:
								case JVX_DATAFORMAT_INTERFACE:
									typeNothandled = true;
									break;
								default:
									assert(0);
								}

								if (!typeNothandled)
								{
									ident.reset(getSetTag.c_str());
									trans.reset(false, 0, JVX_PROPERTY_DECODER_NONE);
									resP = propRef->get_property(callGate, jPRG( ptrVal, 1, propD.format), ident, trans );

									if (resP == JVX_NO_ERROR)
									{
										switch (propD.format)
										{
										case JVX_DATAFORMAT_SIZE:
											if (JVX_CHECK_SIZE_SELECTED(valS))
											{
												txtShow = jvx_size2String(valS);
											}
											else
											{
												txtShow = "unselected";
											}
											break;

										case JVX_DATAFORMAT_DATA:

											backwardRef->keymaps.le.parseEntryList(lst, getSetTag);
											backwardRef->keymaps.le.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32);
											backwardRef->keymaps.le.getValueForKey("SCALEFAC", &transformScale, JVX_WW_KEY_VALUE_TYPE_DATA);
											backwardRef->keymaps.le.getValueForKey("LABELTRANSFORM", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
											if (foundit)
											{
												transtp = (jvxWwTransformValueType)intVal;
											}
											backwardRef->keymaps.le.getValueForKey("MINVAL", &minVal, JVX_WW_KEY_VALUE_TYPE_DATA);
											backwardRef->keymaps.le.getValueForKey("MAXVAL", &maxVal, JVX_WW_KEY_VALUE_TYPE_DATA);
											backwardRef->keymaps.le.getValueForKey("LABELUNIT", &unit, JVX_WW_KEY_VALUE_TYPE_STRING);

											valD = transformTo(valD, transtp, transformScale);
											getDisplayUnitToken(transtp, unit);
											txtShow = jvx_data2String(valD, numDigits) + " " + unit;
											break;
										case JVX_DATAFORMAT_16BIT_LE:
											if (propD.decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
											{
												txtShow = "yes";
												if (valI16 == c_false)
												{
													txtShow = "no";
												}
											}
											else if (propD.decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
											{
												txtShow = jvxDataFormat_txt(valI16);
											}
											else if (propD.decTp == JVX_PROPERTY_DECODER_SUBFORMAT_IDX)
											{
												txtShow = jvxDataFormatGroup_txt(valI16);
											}
											else if (propD.decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
											{
												if (valI16 < 0)
												{
													txtShow = "dontcare";
												}
												else
												{
													txtShow = jvx_int2String(valI16);
												}
											}
											else
											{
												txtShow = jvx_int2String(valI16);
											}
											break;
										case JVX_DATAFORMAT_U16BIT_LE:
											if (propD.decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
											{
												txtShow = "yes";
												if (valUI16 == c_false)
												{
													txtShow = "no";
												}
											}
											else if (propD.decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
											{
												txtShow = jvxDataFormat_txt(valUI16);
											}
											else if (propD.decTp == JVX_PROPERTY_DECODER_SUBFORMAT_IDX)
											{
												txtShow = jvxDataFormatGroup_txt(valUI16);
											}											
											else
											{
												txtShow = jvx_int2String(valUI16);
											}
											break;
										case JVX_DATAFORMAT_8BIT:
											txtShow = jvx_int2String(valI8);
											break;
										case JVX_DATAFORMAT_U8BIT:
											txtShow = jvx_int2String(valUI8);
											break;
										case JVX_DATAFORMAT_32BIT_LE:
											if (propD.decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
											{
												if (valI32 < 0)
												{
													txtShow = "dontcare";
												}
												else
												{
													txtShow = jvx_int2String(valI32);
												}
											}
											else
											{
												txtShow = jvx_int2String(valI32);
											}
											break;
										case JVX_DATAFORMAT_U32BIT_LE:
											txtShow = jvx_int2String(valUI32);											
											break;
										case JVX_DATAFORMAT_64BIT_LE:
											txtShow = jvx_int642String(valI64);
											break;
										case JVX_DATAFORMAT_U64BIT_LE:
											txtShow = jvx_uint642String(valUI64);
											break;
										case JVX_DATAFORMAT_SELECTION_LIST:
											txtShow = "-none-error-";
											for (i = 0; i < selLst.strList.ll(); i++)
											{
												if (jvx_bitTest(selLst.bitFieldSelected(), i))
												{
													txtShow = selLst.strList.std_str_at(i);
													break;
												}
											}
											break;
										case JVX_DATAFORMAT_STRING:
											txtShow = "-none-error-";
											txtShow = fldStr.std_str();
											break;
										case JVX_DATAFORMAT_STRING_LIST:
											if (selLst.strList.ll() == 0)
											{
												txtShow = "-none-error-";
											}
											else
											{
												txtShow = "";
											}

											for (i = 0; i < selLst.strList.ll(); i++)
											{
												if (i > 0)
												{
													txtShow += ", ";
												}
												txtShow += selLst.strList.std_str_at(i);
											}
											break;
										case JVX_DATAFORMAT_VALUE_IN_RANGE:
											backwardRef->keymaps.slpba.parseEntryList(fldStr.std_str(), getSetTag);
											backwardRef->keymaps.slpba.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32);
											backwardRef->keymaps.slpba.getValueForKey("SCALEFAC", &transformScale, JVX_WW_KEY_VALUE_TYPE_DATA);
											backwardRef->keymaps.slpba.getValueForKey("LABELTRANSFORM", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
											if (foundit)
											{
												transtp = (jvxWwTransformValueType)intVal;
											}
											valD = transformTo(valR.val(), transtp, transformScale);
											getDisplayUnitToken(transtp, unit);
											txtShow = jvx_data2String(valD, numDigits) + " " + unit;
											break;
										default:
											assert(0);
										}


										resP = propRef->get_descriptor_property(callGate, descrLoc, identId);
										propRef->get_reference_component_description(&loc_descror, &loc_descrion, &loc_mdule, &loc_cpid);
										ttiptext = loc_descror.std_str() + ":" + jvxComponentIdentification_txt(loc_cpid) + ":" + getSetTag;
										
										theItem->setText(1, txtShow.c_str());
										QFont ft = theItem->font(1);
										ft.setItalic(false);
										//if (descrLoc.accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
										if (!is_valid)
										{
											ft.setItalic(true);
											ttiptext += " -- property is not valid";
										}
										if (accTp == JVX_PROPERTY_ACCESS_READ_ONLY)
										{
											ft.setItalic(true);
											ttiptext += " -- property is read-only";
										}
										if (accTp == JVX_PROPERTY_ACCESS_WRITE_ONLY)
										{
											ft.setItalic(true);
											ttiptext += " -- property is write-only";
										}
										theItem->setToolTip(0, ttiptext.c_str());
										theItem->setFont(1, ft);
										QColor qc = Qt::black;
										switch (descrLoc.ctxt)
										{
										case JVX_PROPERTY_CONTEXT_INFO:
											qc = Qt::blue;
											break;
										case JVX_PROPERTY_CONTEXT_PARAMETER:
											qc = Qt::red;
											break;
										case JVX_PROPERTY_CONTEXT_RESULT:
											qc = Qt::green;
											break;
										}
										theItem->setForeground(0, QBrush(qc));
									}
									else
									{
									/*
									std::cout << __FILE__ << ", " << __LINE__ << ": Failed to read property <" << getSetTag << "> for component <" << tag << ">, error: " <<
										jvxErrorType_descr(resP) << std::endl;
										*/
										txtShow = "<Error on property access>";
										theItem->setText(1, txtShow.c_str());
										QFont ft = theItem->font(1);
										ft.setItalic(true);
										theItem->setFont(1, ft);
										QColor qc = Qt::gray;
										theItem->setForeground(0, QBrush(qc));
										propRef->get_descriptor_property(callGate, descrLoc, identId);
										propRef->get_reference_component_description(&loc_descror, &loc_descrion, &loc_mdule, &loc_cpid);
										ttiptext = loc_descror.std_str() + ":" + jvxComponentIdentification_txt(loc_cpid) + ":" + getSetTag;
										ttiptext += " -- ";
										ttiptext += jvxErrorType_descr(resP);
										theItem->setToolTip(0, ttiptext.c_str());
									}
								}
								else
								{
									txtShow = "<Type not handled>";
									theItem->setText(1, txtShow.c_str());
									QFont ft = theItem->font(1);
									ft.setItalic(true);
									theItem->setFont(1, ft);
									QColor qc = Qt::gray;
									theItem->setForeground(0, QBrush(qc));
									propRef->get_descriptor_property(callGate, descrLoc, identId);
									propRef->get_reference_component_description(&loc_descror, &loc_descrion, &loc_mdule, &loc_cpid);
									ttiptext = loc_descror.std_str() + ":" + jvxComponentIdentification_txt(loc_cpid) + ":" + getSetTag;
									ttiptext += " -- type not handled.";
									theItem->setToolTip(0, ttiptext.c_str());
								}
							}
							else
							{
								txtShow = "<array property not handled>"; // y- default output if the property conversion fails
								backwardRef->keymaps.slpba.parseEntryList(fldStr.std_str(), getSetTag);
								backwardRef->keymaps.slpba.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32);
								jvx::helper::properties::toString(propRef, callGate, propD, txtShow, txtShowPP, numDigits, false);// no need to use txtShowPP since binList is false
								theItem->setText(1, txtShow.c_str());
								QFont ft = theItem->font(1);
								ft.setItalic(true);
								theItem->setFont(1, ft);
								QColor qc = Qt::gray;
								theItem->setForeground(0, QBrush(qc));
								propRef->get_descriptor_property(callGate, descrLoc, identId);
								propRef->get_reference_component_description(&loc_descror, &loc_descrion, &loc_mdule, &loc_cpid);
								ttiptext = loc_descror.std_str() + ":" + jvxComponentIdentification_txt(loc_cpid) + ":" + getSetTag;
								theItem->setToolTip(0, ttiptext.c_str());
								/*
								ttiptext += " -- array of <";
								ttiptext += jvx_size2String(num);
								ttiptext += "> elements not handled.";
								*/
							}
						} // if (!getSetTag.empty())
					}
					else
					{
						txtShow = "--";
						theItem->setText(1, txtShow.c_str());
						theItem->setToolTip(1, (getSetTag + " -- property not readible").c_str());
						propRef->get_descriptor_property(callGate, descrLoc, identId);
						propRef->get_reference_component_description(&loc_descror, &loc_descrion, &loc_mdule, &loc_cpid);
						ttiptext = loc_descror.std_str() + ":" + jvxComponentIdentification_txt(loc_cpid) + ":" + getSetTag;
						ttiptext += " -- property not readible.";
						theItem->setToolTip(0, ttiptext.c_str());
					}

					if (uiRefTp->columnCount() >= 3)
					{
						if (myBasePropIs.updateRt)
						{
							theItem->setText(2, "yes");
						}
						else
						{
							theItem->setText(2, "no");
						}
					}
				}
			}
		}
	}
}

/*
void 
CjvxMaWrapperElementTreeWidget::button_pushed()
{
	std::string txt;
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	jvxStringList strLst;
	jvxString strFld;
	strLst.bStrings = NULL;
	strLst.lStrings = 0;
	selLst.strList = &strLst;
	selLst.bitFieldSelected = 0;
	selLst.bitFieldExclusive = 0;
	std::string getSetTag = propertyGetSetTag;
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	if(verboseMode)
	{
		std::cout << "QPushButton::clicked() callback triggered for " << uiObjectName << std::endl;
	}
	switch(tp)
	{
	case JVX_WW_PUSHBUTTON_CBOOL:
		res = propRef->get_property( &boolVar, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, getSetTag.c_str());
		if(res == JVX_NO_ERROR)
		{
			if(boolVar == c_true)
			{
				boolVar = c_false;
			}
			else
			{
				boolVar = c_true;
			}
			res = propRef->set_property( &boolVar, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, getSetTag.c_str());
		}
		break;
	case JVX_WW_PUSHBUTTON_SELLIST:
		res = propRef->get_property( &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, true, getSetTag.c_str());
		if(res == JVX_NO_ERROR)
		{
			jvxSize idx = jvx_bitfieldSelection2Id(selLst.bitFieldSelected, selLst.strList.ll());
			idx ++;
			idx = idx % selLst.strList.ll();
			selLst.bitFieldSelected = 0;
			jvx_bitSet(selLst.bitFieldSelected, idx);
			res = propRef->set_property( &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST, true, getSetTag.c_str());
		}
		break;
	case JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY:
		qstr = QFileDialog::getExistingDirectory(uiRefTp, tr("Open Folder"), tr(".\\"));
		txt = qstr.toLatin1().data();
		txt = jvx_replaceCharacter(txt, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
		txt = jvx_absoluteToRelativePath(txt, false);
		strFld.bString = (char*)txt.c_str();
		strFld.lString = txt.size();
		res = propRef->set_property( &strFld, 0, 1, JVX_DATAFORMAT_STRING, true, getSetTag.c_str());
		break;
	case JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD:
		qstr = QFileDialog::getOpenFileName(uiRefTp, tr("Open File"), tr(".\\"));
		txt = qstr.toLatin1().data();
		txt = jvx_replaceCharacter(txt, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
		txt = jvx_absoluteToRelativePath(txt, true);
		strFld.bString = (char*)txt.c_str();
		strFld.lString = txt.size();
		res = propRef->set_property( &strFld, 0, 1, JVX_DATAFORMAT_STRING, true, getSetTag.c_str());
		break;
	case JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE:
		qstr = QFileDialog::getSaveFileName(uiRefTp, tr("Save File"), tr(".\\"));
		txt = qstr.toLatin1().data();
		txt = jvx_replaceCharacter(txt, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
		txt = jvx_absoluteToRelativePath(txt, true);
		strFld.bString = (char*)txt.c_str();
		strFld.lString = txt.size();
		res = propRef->set_property( &strFld, 0, 1, JVX_DATAFORMAT_STRING, true, getSetTag.c_str());
		break;
	}
	if(res != JVX_NO_ERROR)
	{
		if(verboseMode)
		{
			std::cout << __FUNCTION__ << ": Failed to update object " << uiObjectName << " linked with property " << getSetTag << ", error reason: " << jvxErrorType_txt(res) << std::endl;
		}
	}
	else
	{
		backwardRef->reportPropertySet(tag, refIdElement);
	}
}
*/

void
checkEnableWidget(jvxErrorType res, jvxBool is_valid,
	jvxPropertyAccessType accTp, jvxDataFormat form,
	jvxBool& allowRead, jvxBool& allowWrite)
{
	allowRead = true;
	allowWrite = true;

	if (res != JVX_NO_ERROR)
	{
		allowRead = false;
		allowWrite = false;
		return;
	}
	if (!is_valid)
	{
		allowRead = false;
		allowWrite = false;
		return;
	}

	switch (accTp)
	{
	case JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE:
		break;
	case JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT:
		break;
	case JVX_PROPERTY_ACCESS_READ_ONLY:
		allowWrite = false;
		break;
	case JVX_PROPERTY_ACCESS_READ_ONLY_ON_START:
		break;
	case JVX_PROPERTY_ACCESS_WRITE_ONLY:
		allowRead = false;
	}
}

void
CjvxMaWrapperElementTreeWidget::treeWidgetContextMenuRequest(const QPoint& pos)
{
	std::string compTag, propName;
	std::vector<std::string> lst;
	jvxErrorType res = JVX_NO_ERROR;
	jvxPropertyDescriptor propD;
	basePropInfos myBasePropIs;
	QTreeWidgetItem* nd = uiRefTp->itemAt(pos);
	if(nd)
	{
		bool everythingOK = getAllTagInformation(nd, compTag, propName, lst, propD, myBasePropIs);
		if (everythingOK)
		{
			QClipboard* clipboard = QApplication::clipboard();
			if (propD.descriptor.std_str().size())
			{
				clipboard->setText(propD.descriptor.c_str());
			}
		}
	}
}

void
CjvxMaWrapperElementTreeWidget::treeWidgetItemDblClicked(QTreeWidgetItem* theItem, int col)
{
	QComboBox_fdb* newCB = NULL;
	QCheckBox_fdb* newCE = NULL; 
	QLineEdit_fdb* newLE = NULL;
	QSlider_fdb* newSL = NULL;
	std::string compTag, propName;
	std::vector<std::string> lst;
	jvxErrorType res = JVX_NO_ERROR;
	jvxPropertyDescriptor propD;
	bool everythingOk = true;
	QVariant var;
	CjvxQtSaWidgetWrapper_elementbase* oldWidget = NULL;
	jvxCBool valCB = c_false;
	basePropInfos myBasePropIs;
	jvxCallManagerProperties callGate;
	jvxBool is_valid = true;
	jvxPropertyAccessType accTp = JVX_PROPERTY_ACCESS_NONE;
	jvxBool allowRead = false;
	jvxBool allowWrite = false;

	bool everythingOK = getAllTagInformation(theItem, compTag, propName, lst, propD, myBasePropIs);
	if (everythingOk)
	{
		jvxBool addressRtUpdate = false;
		if (
			(uiRefTp->columnCount() >= 3) && (col == 2))
		{
			addressRtUpdate = true;
		}
		if(addressRtUpdate)
		{
			myBasePropIs.updateRt = !myBasePropIs.updateRt;
			if (myBasePropIs.updateRt)
			{
				numEntriesRTUpdate++;
			}
			else
			{
				numEntriesRTUpdate--;
			}
			theItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_PARAM_BASE, QVariant::fromValue(myBasePropIs));
			updateWindowUiElement(theItem, 1, false);
		}
		else
		{
			// If there is another widget, close this first
			if (compTag == tag)
			{
				QVariant var = uiRefTp->property("JVX_THE_SUBWIDGET");
				if (var.isValid())
				{
					oldWidget = var.value<CjvxQtSaWidgetWrapper_elementbase*>();
					removeCurrentWidget(oldWidget);
				}
				CjvxQtSaWidgetWrapper_elementbase* newWidget = NULL;

				callGate.s_push();

				callGate.on_get.is_valid = &is_valid;
				callGate.on_get.prop_access_type = &accTp;
				callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_GET_ACCESS_DATA;

				ident.reset(propD.descriptor.c_str());
				trans.reset(false, 0, JVX_PROPERTY_DECODER_NONE);
				res = propRef->get_property(callGate, jPRG( NULL, 0, JVX_DATAFORMAT_NONE), ident, trans );
				callGate.s_pop();

				checkEnableWidget(res, is_valid, accTp, propD.format, allowRead, allowWrite);
				switch (propD.format)
				{
				case JVX_DATAFORMAT_DATA:				
				case JVX_DATAFORMAT_32BIT_LE:
				case JVX_DATAFORMAT_64BIT_LE:
				case JVX_DATAFORMAT_8BIT:
				case JVX_DATAFORMAT_U32BIT_LE:
				case JVX_DATAFORMAT_U64BIT_LE:
				case JVX_DATAFORMAT_U8BIT:
				case JVX_DATAFORMAT_SIZE:
				case JVX_DATAFORMAT_STRING:
				case JVX_DATAFORMAT_VALUE_IN_RANGE:
					if (allowWrite)
					{
						newLE = new QLineEdit_fdb(theItem, this, uiRefTp, allowRead, allowWrite);
						uiRefTp->setItemWidget(theItem, 1, newLE);
						newLE->setProperty("JVX_MY_TREEWIDGET_ITEM", QVariant::fromValue<QTreeWidgetItem*>(theItem));
						newLE->update_window();
						newWidget = newLE;
					}
					break;
				case JVX_DATAFORMAT_16BIT_LE:
					if (propD.decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
					{
						if (allowWrite)
						{
							newCB = new QComboBox_fdb(theItem, this, uiRefTp, allowRead, allowWrite);
							uiRefTp->setItemWidget(theItem, 1, newCB);
							newCB->setProperty("JVX_MY_TREEWIDGET_ITEM", QVariant::fromValue<QTreeWidgetItem*>(theItem));
							newCB->update_window();
							newWidget = newCB;
						}
						break;
					}
					else
					{
						if (allowWrite)
						{
							newLE = new QLineEdit_fdb(theItem, this, uiRefTp, allowRead, allowWrite);
							uiRefTp->setItemWidget(theItem, 1, newLE);
							newLE->setProperty("JVX_MY_TREEWIDGET_ITEM", QVariant::fromValue<QTreeWidgetItem*>(theItem));
							newLE->update_window();
							newWidget = newLE;
						}
					}
					break;

				case JVX_DATAFORMAT_U16BIT_LE:
					if (propD.decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
					{

						if (allowRead)
						{
							ident.reset(propD.descriptor.c_str());
							trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
							res = propRef->get_property(callGate ,
								jPRG(&valCB, 1, propD.format), ident, trans);
						}
						else
						{
							valCB = false;
						}
						if (allowWrite)
						{
							if (valCB == c_false)
							{
								valCB = c_true;
							}
							else
							{
								valCB = c_false;
							}

							ident.reset(propD.descriptor.c_str());
							trans.reset(true, 0, thePropDescriptor.decTp, false);
							res = propRef->set_property(callGate,
								jPRG(&valCB, 1, propD.format),
								ident, trans);
							reportPropertySetItem(compTag.c_str(), propName.c_str(), myBasePropIs.reportTp, theItem, res);
						}
					}
					else
					{
						if (allowWrite)
						{
							newLE = new QLineEdit_fdb(theItem, this, uiRefTp, allowRead, allowWrite);
							uiRefTp->setItemWidget(theItem, 1, newLE);
							newLE->setProperty("JVX_MY_TREEWIDGET_ITEM", QVariant::fromValue<QTreeWidgetItem*>(theItem));
							newLE->update_window();
							newWidget = newLE;
						}
					}
					break;
				case JVX_DATAFORMAT_SELECTION_LIST:
					if (allowRead || allowWrite)
					{
						newCB = new QComboBox_fdb(theItem, this, uiRefTp, allowRead, allowWrite);
						uiRefTp->setItemWidget(theItem, 1, newCB);
						newCB->setProperty("JVX_MY_TREEWIDGET_ITEM", QVariant::fromValue<QTreeWidgetItem*>(theItem));
						newCB->update_window();
						newWidget = newCB;
					}
					break;
				case JVX_DATAFORMAT_STRING_LIST:
					if (allowWrite)
					{
						newSL = new QSlider_fdb(theItem, this, uiRefTp, allowRead, allowWrite);
						uiRefTp->setItemWidget(theItem, 1, newSL);
						newSL->setProperty("JVX_MY_TREEWIDGET_ITEM", QVariant::fromValue<QTreeWidgetItem*>(theItem));
						newSL->update_window();
						newWidget = newSL;
					}
					break;
				}

				var = QVariant::fromValue<CjvxQtSaWidgetWrapper_elementbase*>(newWidget);
				uiRefTp->setProperty("JVX_THE_SUBWIDGET", var);
			}
		}
	}
	
}

void 
CjvxMaWrapperElementTreeWidget::editingCompleted(CjvxQtSaWidgetWrapper_elementbase* theWidgetToClose)
{
	QVariant var = theWidgetToClose->getMyWidget()->property("JVX_MY_TREEWIDGET_ITEM");
	QTreeWidgetItem* theItem = NULL;
	if (var.isValid())
	{
		theItem = var.value<QTreeWidgetItem*>();
	}
	removeCurrentWidget(theWidgetToClose);
	if (theItem)
	{
		updateWindowUiElement(theItem, 1, false);
	}
	else
	{
		updateWindow_core();
	}
}

void 
CjvxMaWrapperElementTreeWidget::propertyReference(IjvxAccessProperties** propRefLoc)
{
	if (propRefLoc)
	{
		*propRefLoc = propRef;
	}
}

void
CjvxMaWrapperElementTreeWidget::removeCurrentWidget(CjvxQtSaWidgetWrapper_elementbase* oldWidget, bool forceImmediate)
{
	QVariant var;
	if (oldWidget)
	{
		var = oldWidget->getMyWidget()->property("JVX_MY_TREEWIDGET_ITEM");
		if (var.isValid())
		{
			QTreeWidgetItem* oldItem = var.value<QTreeWidgetItem*>();
			if (oldItem)
			{
				// We need to set the reference manually, otherwise the app will crash on shutdown..
				uiRefTp->setItemWidget(oldItem, 1, NULL);
			}
		}
		if (forceImmediate)
			delete oldWidget->getMyWidget();
		else
			oldWidget->getMyWidget()->deleteLater();

		oldWidget = NULL;
		var = QVariant::fromValue<CjvxQtSaWidgetWrapper_elementbase*>(oldWidget);
		uiRefTp->setProperty("JVX_THE_SUBWIDGET", var);
	}
}

bool
CjvxMaWrapperElementTreeWidget::hasValidTags(QTreeWidgetItem *theItem, const std::string& tag, int cnt, const std::string& prefix)
{
	bool res = false;
	int i;
	QString hook_text_q;
	QString token;
	std::string prefixL;
	int num = theItem->childCount();
	if (num > 0)
	{
		hook_text_q = theItem->text(1);
		prefixL = prefix + hook_text_q.toLatin1().data();
		for (i = 0; i < num; i++)
		{

			QTreeWidgetItem *theNext = theItem->child(i);
			if (theNext)
			{
				res = res || hasValidTags(theNext, tag, cnt + 1, prefixL);
			}
		}
	}
	else
	{
		token = theItem->text(1);
		prefixL = prefix + token.toLatin1().data();
		if (jvx_compareStringsWildcard(tag, token.toLatin1().data()))
		{
			res = true;
		}
	}
	return(res);
}

bool 
CjvxMaWrapperElementTreeWidget::hasValidTags(QTreeWidget* theTree, const std::string &tag)
{
	jvxSize i;
	bool res = false;
	std::string prefix;
	jvxSize numTLItems = theTree->topLevelItemCount();
	std::string wtag = tag + ":*";
	prefix = theTree->accessibleDescription().toLatin1().data();
	for (i = 0; i < numTLItems; i++)
	{
		QTreeWidgetItem *theTLItem = theTree->topLevelItem((int)i);
		if (theTLItem)
		{
			theTLItem->setExpanded(true);
			res = res ||hasValidTags(theTLItem, wtag, 0, prefix);
		}
	}
	return(res);
}

void
CjvxMaWrapperElementTreeWidget::clearUiElements()
{
	bool res = false;
	int i;
	QString hook_text_q;
	QString token;
	std::string prefixL;
	std::vector<int> remLst;

	// At forst, remove edit widget if one is attached
	QVariant var = uiRefTp->property("JVX_THE_SUBWIDGET");
	if (var.isValid())
	{
		CjvxQtSaWidgetWrapper_elementbase* oldWidget = var.value<CjvxQtSaWidgetWrapper_elementbase*>();
		removeCurrentWidget(oldWidget, true);
	}

	// Remove all added widgets
	jvxSize numTLItems = uiRefTp->topLevelItemCount();
	for (i = 0; i < numTLItems; i++)
	{
		QTreeWidgetItem *theTLItem = uiRefTp->topLevelItem((int)i);
		if (theTLItem)
		{
			bool removeme = false;
			clearUiElements(theTLItem, removeme);
			if (removeme)
			{
				remLst.push_back(i);
			}
		}
	}
	for (i = remLst.size(); i > 0; i--)
	{
		QTreeWidgetItem* theNext = uiRefTp->takeTopLevelItem(remLst[i-1]);
		delete theNext;
	}

	
}

CjvxQtSaWidgetWrapper*
CjvxMaWrapperElementTreeWidget::getBackwardReference()
{
	return(backwardRef);
}

void
CjvxMaWrapperElementTreeWidget::clearUiElements(QTreeWidgetItem* theItem, jvxBool& removeme)
{
	jvxSize i;
	QVariant var;
	QString hookupTxt;
	QTreeWidgetItem *theNext = NULL;
	std::vector<int> remLst;

	var = theItem->data(0, JVX_USER_ROLE_TREEWIDGET_HOOK);
	if (var.isValid())
	{
		hookupTxt = var.toString();
	}
	theItem->setText(1, hookupTxt);

	int num = theItem->childCount();
	if (num > 0)
	{
		for (i = 0; i < num; i++)
		{
			theNext = theItem->child(i);
			if (theNext)
			{
				jvxBool removeme = false;
				clearUiElements(theNext, removeme);
				if (removeme)
				{
					remLst.push_back(i);
				}
			}
		}

		for(i = remLst.size(); i > 0; i--)
		{
			theNext = theItem->takeChild(remLst[i-1]);
			delete theNext;
		}
	}
	else
	{
		removeme = false;
		var = theItem->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_ADDED_ON_ASSOCIATE);
		if (var.isValid())
		{
			removeme = var.toBool();
		}
	}
}

void
CjvxMaWrapperElementTreeWidget::trigger_updateWindow_periodic(jvxPropertyCallContext ccontext, jvxSize* passedId)
{
	updateWindowUiElement(ccontext, passedId, true);
}

void
CjvxMaWrapperElementTreeWidget::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	assert(0);
}

jvxErrorType
CjvxMaWrapperElementTreeWidget::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	return JVX_ERROR_UNSUPPORTED;
}

bool
CjvxMaWrapperElementTreeWidget::matchesWildcard(const std::string& wildcard, jvxSize groupid, jvxWwMatchIdType matchIdType)
{
	jvxSize i;
	bool resL = CjvxSaWrapperElementBase::matchesWildcard(wildcard, groupid, matchIdType);
	if (resL)
	{
		return resL;
	}
	else
	{
		jvxSize numTLItems = uiRefTp->topLevelItemCount();

		for (i = 0; i < numTLItems; i++)
		{
			QTreeWidgetItem *theTLItem = uiRefTp->topLevelItem((int)i);
			if (theTLItem)
			{
				resL = resL || matchesWildcard(theTLItem, wildcard, groupid, matchIdType);
			}
		}
	}
	return resL;
}

bool
CjvxMaWrapperElementTreeWidget::matchesWildcard(QTreeWidgetItem *theItem, const std::string& wildcard, jvxSize groupid, jvxWwMatchIdType matchIdType)
{
	jvxBool resL = false;
	jvxSize i;
	if (theItem->isExpanded())
	{
		int num = theItem->childCount();
		if (num > 0)
		{
			for (i = 0; i < num; i++)
			{

				QTreeWidgetItem *theNext = theItem->child(i);
				if (theNext)
				{
					resL = resL || matchesWildcard(theNext, wildcard, groupid, matchIdType);
					if (resL == true)
					{
						return true;
					}
				}
			}
		}
		else
		{
			resL = resL || matchesWildcardCore(theItem, wildcard, groupid, matchIdType);
			if (resL == true)
			{
				return true;
			}
		}
	}
	return resL;
}

bool
CjvxMaWrapperElementTreeWidget::matchesWildcardCore(QTreeWidgetItem *theItem, const std::string& wildcard, jvxSize groupid, jvxWwMatchIdType matchIdType)
{
	std::string compTag, propName;
	std::vector<std::string> lst;
	jvxPropertyDescriptor descrLoc;
	jvxPropertyDescriptor propD;
	bool everythingOk = true;
	basePropInfos myBasePropIs;

	bool everythingOK = getAllTagInformation(theItem, compTag, propName, lst, propD, myBasePropIs);
	if (everythingOk)
	{
		jvxBool doesMatch1 = true;
		jvxBool doesMatch2 = true;
		if (wildcard.empty())
		{
			doesMatch1 = true;
		}
		else
		{
			std::string fullPath = propName; // jvx_makePathExpr(propPrefix, propertyGetSetTag);
			doesMatch1 = jvx_compareStringsWildcard(wildcard, fullPath);
			/*
			for (i = 0; i < dependencies.size(); i++)
			{
				fullPath = jvx_makePathExpr(propPrefix, dependencies[i]);
				doesMatch1 = doesMatch1 || jvx_compareStringsWildcard(wildcard, fullPath);
			}
			*/
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
		if (doesMatch1 && doesMatch2)
		{
			return true;
		}
	}
	return false;
}

jvxBool 
CjvxMaWrapperElementTreeWidget::updateRealtime() 
{ 
	return(numEntriesRTUpdate > 0); 
}