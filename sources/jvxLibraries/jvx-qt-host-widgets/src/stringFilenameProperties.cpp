#include <QtWidgets/QFileDialog>
#include "stringFilenameProperties.h"

stringFilenameProperties::stringFilenameProperties(jvxComponentIdentification tp, jvxSize idx, jvxPropertyCategoryType category, jvxUInt64 allowStateMask,
		 jvxUInt64 allowThreadingMask, jvxDataFormat format, jvxSize num, jvxPropertyAccessType accessType, jvxPropertyDecoderHintType decTp,
		jvxSize handleIdx, IjvxProperties* theProps, IjvxObject* theObj): baseUiProperties(tp, idx, category, allowStateMask,
		 allowThreadingMask, format, num, accessType, decTp, handleIdx, theProps, theObj)
{
}

void
stringFilenameProperties::init()
{
	this->setupUi(static_cast<QDialog*>(this));
	updateWindow();
}

void
stringFilenameProperties::updateWindow()
{
	std::string descr = "Unknown description";
	jvxApiString fldStr;
	jvxCallManagerProperties callGate;

	std::string entry;

	// propertyParams.tp not shown
	//jvxComponentType tp;
	//jvxSize idx;

	lineEdit_propertyid->setText(jvx_size2String(propertyParams.handleIdx).c_str());

	lineEdit_category->setText(jvxPropertyCategoryType_txt((jvxSize)propertyParams.category));

	radioButton_state_init->setChecked(false);
	radioButton_state_selected->setChecked(false);
	radioButton_state_active->setChecked(false);
	radioButton_state_prepared->setChecked(false);
	radioButton_state_processing->setChecked(false);

	if(propertyParams.allowStateMask & JVX_STATE_INIT)
	{
		radioButton_state_init->setChecked(true);
	}
	if(propertyParams.allowStateMask & JVX_STATE_SELECTED)
	{
		radioButton_state_selected->setChecked(true);
	}
	if(propertyParams.allowStateMask & JVX_STATE_ACTIVE)
	{
		radioButton_state_active->setChecked(true);
	}
	if(propertyParams.allowStateMask & JVX_STATE_PREPARED)
	{
		radioButton_state_prepared->setChecked(true);
	}
	if(propertyParams.allowStateMask & JVX_STATE_PROCESSING)
	{
		radioButton_state_processing->setChecked(true);
	}

	radioButton_thread_init->setChecked(false);
	radioButton_thread_async->setChecked(false);
	radioButton_thread_sync->setChecked(false);
	if(propertyParams.allowThreadingMask & JVX_THREADING_INIT)
	{
		radioButton_thread_init->setChecked(true);
	}
	if(propertyParams.allowThreadingMask & JVX_THREADING_SYNC)
	{
		radioButton_thread_sync->setChecked(true);
	}
	if(propertyParams.allowThreadingMask & JVX_THREADING_ASYNC)
	{
		radioButton_thread_async->setChecked(true);
	}

	lineEdit_format->setText(jvxDataFormat_txt((jvxSize)propertyParams.format));

	lineEdit_numberElements->setText(jvx_size2String(propertyParams.num).c_str());

	radioButton_ronly->setChecked(false);
	radioButton_rwcontent->setChecked(false);
	radioButton_rwfull->setChecked(false);

	switch(this->propertyParams.accessType)
	{
	case JVX_PROPERTY_ACCESS_READ_ONLY:
		radioButton_ronly->setChecked(true);
		break;
	case JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT:
		radioButton_rwcontent->setChecked(true);
		break;
	case JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE:
		radioButton_rwfull->setChecked(true);
		break;
	}


	lineEdit_decoder_hint->setText(jvxPropertyDecoderHintType_txt((jvxSize)propertyParams.decTp));
//		jvxDecoderHintType decTp;
//		IjvxHost* theLink;

	if((propertyParams.theProps) && (propertyParams.theObj))
	{
		jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(propertyParams.idx);

		propertyParams.theProps->description_property(callGate, theDescr, ident);
		descr = jvxPropertyContext_txt(theDescr.ctxt);
		descr += ": ";
		descr += theDescr.description.std_str();
	}
	label_description->setText(descr.c_str());

	jvx_pullPropertyString(entry, propertyParams.theObj, propertyParams.theProps, propertyParams.idx, propertyParams.category, 
		callGate);
	lineEdit_value->setText(entry.c_str());
	lineEdit_value->setReadOnly(propertyParams.accessType == JVX_PROPERTY_ACCESS_READ_ONLY);
}

void
stringFilenameProperties::textEdited()
{
	jvxCallManagerProperties callGate;

	std::string entry = lineEdit_value->text().toLatin1().constData();;
	jvx_pushPullPropertyString(entry, propertyParams.theObj, propertyParams.theProps, propertyParams.idx, propertyParams.category,
		callGate);
	updateWindow();
}

void
stringFilenameProperties::selectFilename()
{
	std::string fName;
	QString fN;
	bool isFile = true;
	switch(propertyParams.decTp)
	{
	case JVX_PROPERTY_DECODER_FILENAME_OPEN:
		fN = QFileDialog::getOpenFileName(NULL, "Open File", "./", "*.*");
		fName = fN.toLatin1().constData();
		break;
	case JVX_PROPERTY_DECODER_DIRECTORY_SELECT:
		fN = QFileDialog::getExistingDirectory(NULL, "Open Directory");
		fName = fN.toLatin1().constData();
		isFile = false;
		break;
	case JVX_PROPERTY_DECODER_FILENAME_SAVE:
		fN = QFileDialog::getSaveFileName(NULL, "Save File", "./", "*.*");
		fName = fN.toLatin1().constData();
	}
#ifdef JVX_OS_WINDOWS
	fName = jvx_replaceDirectorySeparators_toWindows(fName, '/', "\\");
#endif
	fName = jvx_absoluteToRelativePath(fName, isFile);
	updateWindow();
}
