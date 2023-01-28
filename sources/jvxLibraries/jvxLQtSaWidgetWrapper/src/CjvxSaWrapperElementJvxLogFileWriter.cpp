#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementJvxLogFileWriter.h"
#include <QtWidgets/QFileDialog>
#include <QVariant>

CjvxSaWrapperElementJvxLogFileWriter::CjvxSaWrapperElementJvxLogFileWriter(jvxQtSaLogFileWriter* uiRefLoc,
	CjvxQtSaWidgetWrapper* backRefLoc, IjvxAccessProperties* propRefIn, std::string propertyPrefix,
	std::vector<std::string> paramLstLoc, const std::string& tag_cp, const std::string& tag_key, const std::string& uiObjectNameLoc,
	const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
	IjvxHiddenInterface* hostRef) :
	CjvxSaWrapperElementBase(static_cast<QWidget*>(uiRefLoc), backRefLoc, propRefIn, propertyPrefix,
		paramLstLoc, tag_cp, tag_key, uiObjectNameLoc, prefix, verboseLoc, uId, hostRef)
{
	uiRefTp = uiRefLoc;
	initializeUiElement();
}

void
CjvxSaWrapperElementJvxLogFileWriter::initializeUiElement(jvxPropertyCallContext ccontext)
{
	jvxSize i;
	jvxApiString  fldStr;
	jvxApiString str;
	std::string txt;
	jvxCallManagerProperties callGate;

	str.assign_const(txt.c_str(), txt.size());

	theCb = NULL;
	theLePrefix = NULL;
	theLeFolder = NULL;
	thePbFolder = NULL;
	thePb = NULL;
	theGb = NULL;

	initializeProperty_core(false, ccontext);

	if (widgetStatus == JVX_STATE_SELECTED)
	{

		uiRefTp->getPropsPrefix(&fldStr);
		std::string lPrefix = fldStr.std_str();
		if (!lPrefix.empty())
		{
			propertyGetSetTag = jvx_makePathExpr(lPrefix, propertyGetSetTag, true, true);
		}
	
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

				if(assignment == "WIDGET_TITLE")
				{
					myTitle = value;
				}
				if(assignment == "DEFAULT_PREFIX")
				{
					defaultPrefix = value;
				}
			}
		}
		if(!defaultPrefix.empty())
		{
			str.assign_const(defaultPrefix.c_str(), txt.size());
			jvxErrorType res = JVX_NO_ERROR;
			std::string prop;

			prop = propertyGetSetTag + "/logFilePrefix";
			ident.reset(prop.c_str());
			trans.reset(true, 0, thePropDescriptor.decTp, false);
			res = propRef->set_property(callGate,
				jPRG(&str, 1, JVX_DATAFORMAT_STRING),
				ident, trans);
		}
		uiRefTp->getWidgetReferences(&theCb, &theLePrefix, &theLeFolder, &thePbFolder, &thePb, &theGb);
		uiRefTp->setProperty("mySaWidgetRefLogFileWriter", QVariant::fromValue<CjvxSaWrapperElementJvxLogFileWriter*>(this));

		if(theCb)
		{
			connect(theCb, SIGNAL(clicked(bool)), this, SLOT(checkbox_toggled(bool)));
		}
		if(theLePrefix)
		{
			connect(theLePrefix , SIGNAL(editingFinished()), this, SLOT(editing_finished_prefix()));
		}
		//theLeFolder: no callback
		if(thePbFolder)
		{
			connect(thePbFolder, SIGNAL(clicked()), this, SLOT(button_pushed_folder()));
		}
		//thePb

		if (!myTitle.empty())
		{
			theGb->setTitle(myTitle.c_str());
		}

		clearUiElements();

		if (ccontext == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			assert(0);
		}

		updateWindowUiElement();

		assert(backwardRef);
		backwardRef->reportPropertyInitComplete(selector_lst.c_str(), assoc_id);
	}
	//verboseLoc = this->myBaseProps.verbose_out;
}

CjvxSaWrapperElementJvxLogFileWriter::~CjvxSaWrapperElementJvxLogFileWriter()
{
	if (widgetStatus == JVX_STATE_SELECTED)
	{
		if(theCb)
		{
			disconnect(theCb, SIGNAL(clicked(bool)));
		}
		if(theLePrefix)
		{
			disconnect(theLePrefix , SIGNAL(editingFinished()));
		}
		//theLeFolder: no callback
		if(thePbFolder)
		{
			disconnect(thePbFolder, SIGNAL(clicked()));	
		}
		theCb = NULL;
		theLePrefix = NULL;
		theLeFolder = NULL;
		thePbFolder = NULL;
		thePb = NULL;
	}
}

void 
CjvxSaWrapperElementJvxLogFileWriter::clearUiElements()
{
	if (uiRefTp)
	{
		uiRefTp->clearUiElements();
	}
}

void
CjvxSaWrapperElementJvxLogFileWriter::updatePropertyStatus(jvxPropertyCallContext ccontext, jvxSize* globalId)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	std::string getSetTag = propertyGetSetTag;
	jvxBool condSet = false;
	jvxPropertyDescriptor descr;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
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
	}
	*/
}


jvxErrorType 
CjvxSaWrapperElementJvxLogFileWriter::updateWindowUiElement(jvxPropertyCallContext ccontext, jvxSize* globalId, jvxBool call_periodic_update)
{
	jvxCBool boolVar = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selLst;
	std::string prop;
	jvxApiString  str;
	jvxCBool togLog;
	std::string txtT;
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
	if(myTitle.empty())
	{
		prop = jvx_makePathExpr(getSetTag, "/groupTitle", false, true);
		ident.reset(prop.c_str());
		trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
		res = propRef->get_property(callGate, jPRG( &str,  1, JVX_DATAFORMAT_STRING), ident, trans);
		if (res == JVX_NO_ERROR)
		{
			txtT = str.std_str();
		}
	}

	uiRefTp->setWidgetTitle(txtT.c_str());

	prop = jvx_makePathExpr(getSetTag, "/activateLogToFile", false, true);
	ident.reset(prop.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &togLog, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	if(res == JVX_NO_ERROR)
	{
		this->theCb->setChecked(togLog != 0);
		this->theCb->setEnabled(true);
	}
	else
	{
		this->theCb->setEnabled(false);
		std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << prop << ", error reason: " << jvxErrorType_txt(res) << std::endl;
	}


	prop = jvx_makePathExpr(getSetTag, "/logFilePrefix", false, true);
	ident.reset(prop.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &str, 1, JVX_DATAFORMAT_STRING), ident, trans);
	if(res == JVX_NO_ERROR)
	{
			this->theLePrefix->setText(str.c_str());
			this->theLePrefix->setEnabled(true);
	}
	else
	{
		this->theLePrefix->setText("ERROR");
		std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << prop << ", error reason: " << jvxErrorType_txt(res) << std::endl;
	}

	prop = jvx_makePathExpr(getSetTag, "/logFileFolder", false, true);
	ident.reset(prop.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &str, 1, JVX_DATAFORMAT_STRING), ident, trans);
	if(res == JVX_NO_ERROR)
	{
		this->theLeFolder->setText(str.c_str());
	}
	else
	{
		this->theLePrefix->setText("ERROR");
		std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << prop << ", error reason: " << jvxErrorType_txt(res) << std::endl;
	}
	thePb->setValue(0.0);
	thePb->setEnabled(false);
	return JVX_NO_ERROR;
}

void 
CjvxSaWrapperElementJvxLogFileWriter::trigger_updateWindow_periodic(jvxPropertyCallContext ccontext, jvxSize* passedId)
{
	jvxValueInRange range;
	jvxErrorType res = JVX_NO_ERROR;
	std::string prop;
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
	prop = jvx_makePathExpr(getSetTag, "/fillHeightBuffer", false, true);
	ident.reset(prop.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &range, 1, JVX_DATAFORMAT_VALUE_IN_RANGE), ident, trans);
	if(res == JVX_NO_ERROR)
	{
		thePb->setValue(range.val());
		thePb->setEnabled(true);
	}
	else
	{
		std::cout << __FUNCTION__ << ": Failed to read object " << uiObjectName << " linked with property " << prop << ", error reason: " << jvxErrorType_txt(res) << std::endl;
	}
}

void 
CjvxSaWrapperElementJvxLogFileWriter::checkbox_toggled(bool toggle)
{
	jvxCallManagerProperties callGate;
	jvxCBool val = toggle;
	jvxErrorType res = JVX_NO_ERROR;
	std::string prop;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	prop = jvx_makePathExpr(getSetTag, "/activateLogToFile");
	ident.reset(prop.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate,
		jPRG(&val, 1, JVX_DATAFORMAT_16BIT_LE),
		ident, trans);
	backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
		myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
}

void 
CjvxSaWrapperElementJvxLogFileWriter::editing_finished_prefix()
{
	jvxCallManagerProperties callGate;
	jvxApiString str;
	std::string txt;
	txt = theLePrefix->text().toLatin1().data();
	str.assign_const(txt.c_str(), txt.size());
	jvxErrorType res = JVX_NO_ERROR;
	std::string prop;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	/*
	QString qstr = uiRef->accessibleName();
	if (!qstr.isEmpty())
	{
		getSetTag = jvx_makePathExpr(qstr.toLatin1().data(), getSetTag);
	}
	*/
	prop = jvx_makePathExpr(getSetTag, "/logFilePrefix");
	ident.reset(prop.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, 
		jPRG(&str, 1, JVX_DATAFORMAT_STRING), ident, trans );
	backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
		myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);

}

void 
CjvxSaWrapperElementJvxLogFileWriter::button_pushed_folder()
{
	jvxApiString  str;
	jvxApiString str2;
	std::string txt;
	jvxErrorType res = JVX_NO_ERROR;
	std::string prop;
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
	prop = jvx_makePathExpr(getSetTag, "/logFileFolder");
	ident.reset(prop.c_str());
	trans.reset(true, 0, JVX_PROPERTY_DECODER_NONE);
	res = propRef->get_property(callGate, jPRG( &str, 1, JVX_DATAFORMAT_STRING), ident, trans);
	if(res == JVX_NO_ERROR)
	{
		QString dir = QFileDialog::getExistingDirectory(uiRefTp, tr("Open Directory"), str.c_str(), QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);

		txt = dir.toLatin1().data();

		txt = jvx_replaceDirectorySeparators_toWindows(txt, '/', JVX_SEPARATOR_DIR);
		txt = jvx_absoluteToRelativePath(txt, false);

		str2.assign_const(txt.c_str(), txt.size());
		ident.reset(prop.c_str());
		trans.reset(true, 0, thePropDescriptor.decTp, false);
		res = propRef->set_property(callGate,
			jPRG(&str2, 1, JVX_DATAFORMAT_STRING),
			ident, trans);
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

void
CjvxSaWrapperElementJvxLogFileWriter::setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	assert(0);
}

jvxErrorType
CjvxSaWrapperElementJvxLogFileWriter::setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements,
	jvxCBitField whattoset, jvxPropertyCallContext ccontext)
{
	return JVX_ERROR_UNSUPPORTED;
}

