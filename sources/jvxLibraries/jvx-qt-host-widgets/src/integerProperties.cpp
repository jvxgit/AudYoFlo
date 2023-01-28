#include "integerProperties.h"

integerProperties::integerProperties(jvxComponentIdentification tp, jvxSize idx, jvxPropertyCategoryType category, jvxUInt64 allowStateMask,
		 jvxUInt64 allowThreadingMask, jvxDataFormat format, jvxSize num, jvxPropertyAccessType accessType, jvxPropertyDecoderHintType decTp,
		jvxSize handleIdx, IjvxProperties* theProps, IjvxObject* theObj): baseUiProperties(tp, idx, category, allowStateMask,
		 allowThreadingMask, format, num, accessType, decTp, handleIdx, theProps, theObj)
{
}

void
integerProperties::init()
{
	this->setupUi(static_cast<QDialog*>(this));
	updateWindow();
}

void
integerProperties::updateWindow()
{
	jvxCallManagerProperties callGate;
	jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
	jvx::propertyAddress::CjvxPropertyAddressLinear ident;
	jvx::propertyAddress::CjvxPropertyAddressGlobalId identId;
	jvx::propertyDetail::CjvxTranferDetail trans;

	std::string descr = "Unknown description";
	jvxApiString fldStr;
	std::string txt;

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
		ident.reset(propertyParams.idx);

		propertyParams.theProps->description_property(callGate, theDescr, ident);

		descr = jvxPropertyContext_txt(theDescr.ctxt);
		descr += ": ";
		descr = theDescr.description.std_str();
	}
	label_description->setText(descr.c_str());

	identId.reset(propertyParams.idx, propertyParams.category);
	trans.reset();
	if(propertyParams.format == JVX_DATAFORMAT_8BIT)
	{
		jvxInt8 theVal = 0.0;
		propertyParams.theProps->get_property(callGate, jPRG(&theVal, 1, JVX_DATAFORMAT_8BIT),identId, trans);
		txt = jvx_int2String(theVal);
	}
	else if(propertyParams.format == JVX_DATAFORMAT_16BIT_LE)
	{
		jvxInt16 theVal = 0.0;
		propertyParams.theProps->get_property(callGate, jPRG(&theVal, 1, JVX_DATAFORMAT_16BIT_LE), identId, trans);
		txt = jvx_int2String(theVal);
	}
	else if(propertyParams.format == JVX_DATAFORMAT_32BIT_LE)
	{
		jvxInt32 theVal = 0.0;
		propertyParams.theProps->get_property(callGate, jPRG(&theVal, 1, JVX_DATAFORMAT_32BIT_LE), identId, trans);
		txt = jvx_int2String(theVal);
	}
	else if(propertyParams.format == JVX_DATAFORMAT_64BIT_LE)
	{
		jvxInt16 theVal = 0.0;
		propertyParams.theProps->get_property(callGate, jPRG(&theVal, 1, JVX_DATAFORMAT_64BIT_LE), identId, trans);
		txt = jvx_int2String(theVal);
	}

	this->lineEdit_value->setText(txt.c_str());
	this->lineEdit_value->setReadOnly( propertyParams.accessType == JVX_PROPERTY_ACCESS_READ_ONLY);
}

void
integerProperties::textEdited()
{
	jvxCallManagerProperties callGate;
	QString txtQ = lineEdit_value->text();

	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(propertyParams.idx, propertyParams.category);
	jvx::propertyDetail::CjvxTranferDetail trans(false);

	if(propertyParams.format == JVX_DATAFORMAT_8BIT)
	{
		jvxInt8 theVal = 0;
		theVal = (jvxInt8)txtQ.toInt();

		propertyParams.theProps->set_property(callGate, jPRG(&theVal, 1, JVX_DATAFORMAT_8BIT), 
			ident, trans);
	}
	else if(propertyParams.format == JVX_DATAFORMAT_16BIT_LE)
	{
		jvxInt16 theVal = 0;
		theVal = txtQ.toShort();
		propertyParams.theProps->set_property(callGate, jPRG(&theVal, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	}
	else if(propertyParams.format == JVX_DATAFORMAT_32BIT_LE)
	{
		jvxInt32 theVal = 0;
		theVal = txtQ.toInt();
		propertyParams.theProps->set_property(callGate, jPRG(&theVal, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
	}
	else if(propertyParams.format == JVX_DATAFORMAT_64BIT_LE)
	{
		jvxInt64 theVal = 0;
		theVal = txtQ.toLongLong();
		propertyParams.theProps->set_property(callGate, jPRG(&theVal, 1, JVX_DATAFORMAT_64BIT_LE), ident, trans);
	}
	updateWindow();
}
