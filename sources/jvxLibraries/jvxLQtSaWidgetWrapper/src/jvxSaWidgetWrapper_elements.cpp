#include "jvxSaWidgetWrapper_elements.h"
#include "CjvxMaWrapperElementTreeWidget.h"
#include "CjvxQtSaWidgetWrapper.h"

CjvxQtSaWidgetWrapper_elementbase::CjvxQtSaWidgetWrapper_elementbase(QTreeWidgetItem* assoc, CjvxMaWrapperElementTreeWidget* cb, jvxBool allowReadArg, jvxBool allowWriteArg) :
	cbk(cb), it(assoc), theWidget(NULL)
{
	jvxSize idxArray = 0;
	jvxBool res = CjvxMaWrapperElementTreeWidget::getAllTagInformation(it, myTag, propName, paramlst, connectedProp, myBasePropIs, idxArray);
	myBackwardRef = cbk->getBackwardReference();
	allowRead = allowReadArg;
	allowWrite = allowWriteArg;
};

QComboBox_fdb::QComboBox_fdb(QTreeWidgetItem* assoc, CjvxMaWrapperElementTreeWidget* cb, QWidget* parent, jvxBool allowReadArg, jvxBool allowWriteArg) :
	QComboBox(parent), CjvxQtSaWidgetWrapper_elementbase(assoc, cb, allowReadArg, allowWriteArg)
{
  connect(this, SIGNAL(activated(int)), this, SLOT(newSelection(int)));

  theWidget = static_cast<QWidget*>(this);
};

bool
QComboBox_fdb::update_window()
{
	IjvxAccessProperties* propRefLoc = NULL;
	jvxSelectionList selLst;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i = 0;
	jvxCallManagerProperties callGate;

	std::string txt;
	jvxInt16 valI16 = 0;
	jvxSize idx = JVX_SIZE_UNSELECTED;
	std::string token;
	if (cbk)
	{
		cbk->propertyReference(&propRefLoc);
		if (propRefLoc)
		{
			switch(connectedProp.format)
			{
			case JVX_DATAFORMAT_SELECTION_LIST:
				if (allowRead)
				{
					token = connectedProp.descriptor.std_str();
					ident.reset(token.c_str());
					trans.reset(false, 0, connectedProp.decTp);
					res = propRefLoc->get_property(callGate, jPRG( 
						&selLst, 1, connectedProp.format), 
						ident, trans);
					if (res == JVX_NO_ERROR)
					{
						this->clear();
						for (i = 0; i < selLst.strList.ll(); i++)
						{
							txt = selLst.strList.std_str_at(i);
							if (jvx_bitTest(selLst.bitFieldSelected(), i))
							{
								if (JVX_CHECK_SIZE_UNSELECTED(idx))
								{
									idx = i;
								}
								// txt += "*";
							}
							if (!allowWrite)
							{
								txt += "(ro)";
							}
							this->addItem(txt.c_str());
						}

						for (i = 0; i < selLst.strList.ll(); i++)
						{
							QFont ft = font();
							if (jvx_bitTest(selLst.bitFieldSelected(), i))
							{
								ft.setBold(true);
							}
							else
							{
								ft.setBold(false);
							}
							this->setItemData(i, ft, Qt::FontRole);
						}

						if (JVX_CHECK_SIZE_SELECTED(idx))
						{
							this->setCurrentIndex(idx);
						}
					}
				}
				else
				{
					this->clear();
					this->addItem("(wo)");
				}
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				if (connectedProp.decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
				{
					if (allowRead)
					{
						token = connectedProp.descriptor.std_str();
						ident.reset(token.c_str());
						trans.reset(false, 0, connectedProp.decTp);
						res = propRefLoc->get_property(callGate, jPRG(&valI16,  1, connectedProp.format),ident, trans);
						if (res == JVX_NO_ERROR)
						{
							this->clear();
							for (i = 0; i < JVX_DATAFORMAT_LIMIT; i++)
							{
								txt = jvxDataFormat_txt(i);
								if (!allowWrite)
								{
									txt += "(ro)";
								}
								this->addItem(txt.c_str());
							}
							this->setCurrentIndex(valI16);
						}
					}
				}
				else
				{
					this->clear();
					this->addItem("INVALID SETTING");
				}
				break;
			}
		}
	}
	return true;
}

void
QComboBox_fdb::focusInEvent(QFocusEvent* e)
{
	if (e->reason() == Qt::MouseFocusReason)
	{
		
	}

	// You might also call the parent method.
	QComboBox::focusInEvent(e);
};

void 
QComboBox_fdb::focusOutEvent(QFocusEvent* e)
{
	if (e->reason() == Qt::MouseFocusReason)
	{
		if (cbk)
		{
			cbk->editingCompleted(static_cast<CjvxQtSaWidgetWrapper_elementbase*>(this));
		}
	}

	// You might also call the parent method.
	QComboBox::focusOutEvent(e);
};

void 
QComboBox_fdb::mousePressEvent ( QMouseEvent * event )
{
	/* This implementation allows to start a combobox with a right click */
	QMouseEvent mEvt(QEvent::MouseButtonPress, rect().center(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	if(event->button() == Qt::RightButton)
	{
		std::cout << __FUNCTION__ << std::endl;
		//startedWithRightClick = true;
	} 
	if(event->button() == Qt::LeftButton)
	{
		std::cout << __FUNCTION__ << std::endl;
		//startedWithRightClick = false;
	} 	
	return(QComboBox::mousePressEvent(&mEvt));

};

void 
QComboBox_fdb::newSelection(int sel)
{
	IjvxAccessProperties* propRefLoc = NULL;
	jvxSelectionList selLst;
	jvxInt16 valI16 = 0;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	std::string token;
	if (cbk)
	{
		cbk->propertyReference(&propRefLoc);
		if (propRefLoc)
		{
			switch (connectedProp.format)
			{
			case JVX_DATAFORMAT_SELECTION_LIST:
				if (allowRead && allowWrite)
				{
					token = connectedProp.descriptor.std_str();
					ident.reset(token.c_str());
					trans.reset(true, 0, connectedProp.decTp);
					res = propRefLoc->get_property(callGate, jPRG(&selLst, 1, connectedProp.format), ident, trans);
					switch (connectedProp.decTp)
					{
					case JVX_PROPERTY_DECODER_MULTI_SELECTION:
						if (jvx_bitTest(selLst.bitFieldExclusive, sel))
						{
							jvx_bitZSet(selLst.bitFieldSelected(), sel);
						}
						else
						{
							jvx_bitToggle(selLst.bitFieldSelected(), sel);
						}
						break;
					case JVX_PROPERTY_DECODER_SINGLE_SELECTION:
					case JVX_PROPERTY_DECODER_SIMPLE_ONOFF:
						jvx_bitZSet(selLst.bitFieldSelected(), sel);
						break;
					default:
						jvx_bitZSet(selLst.bitFieldSelected(), sel);
					}
					token = connectedProp.descriptor.std_str();
					ident.reset(token.c_str());
					trans.reset(true, 0, connectedProp.decTp, false);
					res = propRefLoc->set_property(callGate, 
						jPRG(&selLst, 1, connectedProp.format),
						ident, trans);
				}
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				if (connectedProp.decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
				{
					if (allowRead && allowWrite)
					{
						valI16 = sel;
						token = connectedProp.descriptor.std_str();
						ident.reset(token.c_str());
						trans.reset(true, 0, connectedProp.decTp, false);
						res = propRefLoc->set_property(callGate, 
							jPRG(&valI16, 1, connectedProp.format), ident, trans);
					}
				}
				else
				{
					std::cout << __FUNCTION__ << ": Error: ComboBox in tree widget with invalid decoder type for data format <JVX_DATAFORMAT_16BIT_LE>" << std::endl;
				}
				break;
			}
		}
		this->update_window();
		token = connectedProp.descriptor.std_str();
		cbk->reportPropertySetItem(myTag.c_str(), token.c_str(), myBasePropIs.reportTp, it, res);
	}
};

// =========================================================================================
// =========================================================================================

QSlider_fdb::QSlider_fdb(QTreeWidgetItem* assoc, CjvxMaWrapperElementTreeWidget* cb, QWidget* parent, jvxBool allowReadArg, jvxBool allowWriteArg):
	QSlider(Qt::Horizontal, parent), CjvxQtSaWidgetWrapper_elementbase(assoc, cb, allowReadArg, allowWriteArg)
{
  connect(this, SIGNAL(sliderReleased()), this, SLOT(newSliderPosition()));
  connect(this, SIGNAL(sliderPressed()), this, SLOT(sliderHasBeenPressed()));
  theWidget = static_cast<QWidget*>(this);
};

bool
QSlider_fdb::update_window()
{
	return true;
}

void 
QSlider_fdb::sliderHasBeenPressed()
{
};

void 
QSlider_fdb::newSliderPosition()
{
  int val = this->value();  
};

void
QSlider_fdb::focusInEvent(QFocusEvent* e)
{
	if (e->reason() == Qt::MouseFocusReason)
	{
	}

	// You might also call the parent method.
	QSlider::focusInEvent(e);
};

void
QSlider_fdb::focusOutEvent(QFocusEvent* e)
{
	if (cbk)
	{
		cbk->editingCompleted(static_cast<CjvxQtSaWidgetWrapper_elementbase*>(this));
	}

	// You might also call the parent method.
	QSlider::focusOutEvent(e);
};

// =========================================================================================
// =========================================================================================

QLineEdit_fdb::QLineEdit_fdb(QTreeWidgetItem* assoc, CjvxMaWrapperElementTreeWidget* cb, QWidget* parent, jvxBool allowReadArg, jvxBool allowWriteArg):
	QLineEdit(parent), CjvxQtSaWidgetWrapper_elementbase(assoc, cb, allowReadArg, allowWriteArg)
{
  connect(this, SIGNAL(editingFinished()), this, SLOT(newText()));
  theWidget = static_cast<QWidget*>(this);
};

bool
QLineEdit_fdb::update_window()
{
	IjvxAccessProperties* propRefLoc = NULL;

	jvxHandle* ptrVal = NULL;
	jvxData valD = 0;
	jvxInt16 valI16 = 0;
	jvxInt8 valI8 = 0;
	jvxInt32 valI32 = 0;
	jvxInt64 valI64 = 0;
	jvxSize valS = 0;
	jvxCBool valB = false;
	jvxSelectionList selLst;
	jvxApiString fldStr;
	jvxValueInRange valR;
	jvxErrorType resP = JVX_NO_ERROR;
	jvxWwTransformValueType transtp = JVX_WW_SHOW_DIRECT;
	std::string unit;
	std::string txtShow;
	int numDigits = 4;
	jvxInt32 intVal;
	jvxData minVal = JVX_DATA_MAX_NEG;
	jvxData maxVal = JVX_DATA_MAX_POS;
	jvxData transformScale;
	jvxBool foundit;
	jvxCallManagerProperties callGate;
	std::string token;
	if (cbk)
	{
		cbk->propertyReference(&propRefLoc);
		if (propRefLoc)
		{
			if (connectedProp.num == 1)
			{
				// Currently, only single elements are supported
				switch (connectedProp.format)
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
				case JVX_DATAFORMAT_STRING:
					ptrVal = &fldStr;
					break;
				case JVX_DATAFORMAT_VALUE_IN_RANGE:
					ptrVal = &valR;
					break;
				default:
					assert(0);
				}
				token = connectedProp.descriptor.std_str();
				ident.reset(token.c_str());
				trans.reset(false, 0, connectedProp.decTp);
				resP = propRefLoc->get_property(callGate, jPRG( ptrVal, 1, connectedProp.format), ident, trans);
				if (resP == JVX_NO_ERROR)
				{
					switch (connectedProp.format)
					{
					case JVX_DATAFORMAT_SIZE:
						txtShow = jvx_size2String(valS);
						break;

					case JVX_DATAFORMAT_DATA:

						myBackwardRef->keymaps.le.parseEntryList(paramlst, "");
						myBackwardRef->keymaps.le.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32);
						myBackwardRef->keymaps.le.getValueForKey("SCALEFAC", &transformScale, JVX_WW_KEY_VALUE_TYPE_DATA);
						myBackwardRef->keymaps.le.getValueForKey("LABELTRANSFORM", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
						if (foundit)
						{
							transtp = (jvxWwTransformValueType)intVal;
						}
						myBackwardRef->keymaps.le.getValueForKey("MINVAL", &minVal, JVX_WW_KEY_VALUE_TYPE_DATA);
						myBackwardRef->keymaps.le.getValueForKey("MAXVAL", &maxVal, JVX_WW_KEY_VALUE_TYPE_DATA);
						myBackwardRef->keymaps.le.getValueForKey("LABELUNIT", &unit, JVX_WW_KEY_VALUE_TYPE_STRING);

						valD = CjvxSaWrapperElementBase::transformTo(valD, transtp, transformScale);
						CjvxSaWrapperElementBase::getDisplayUnitToken(transtp, unit);
						valD = JVX_MAX(valD, minVal);
						valD = JVX_MIN(valD, maxVal);
						txtShow = jvx_data2String(valD, numDigits) + " " + unit;

						//valD = cbk->transformTo(valD, transtp, unit);
						// txtShow = jvx_data2String(valD, numDigits);
						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:

						myBackwardRef->keymaps.le.parseEntryList(paramlst, "");
						myBackwardRef->keymaps.le.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32);
						myBackwardRef->keymaps.le.getValueForKey("SCALEFAC", &transformScale, JVX_WW_KEY_VALUE_TYPE_DATA);
						myBackwardRef->keymaps.le.getValueForKey("LABELTRANSFORM", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
						if (foundit)
						{
							transtp = (jvxWwTransformValueType)intVal;
						}
						myBackwardRef->keymaps.le.getValueForKey("MINVAL", &minVal, JVX_WW_KEY_VALUE_TYPE_DATA);
						myBackwardRef->keymaps.le.getValueForKey("MAXVAL", &maxVal, JVX_WW_KEY_VALUE_TYPE_DATA);
						myBackwardRef->keymaps.le.getValueForKey("LABELUNIT", &unit, JVX_WW_KEY_VALUE_TYPE_STRING);

						valD = CjvxSaWrapperElementBase::transformTo(valR.val(), transtp, transformScale);
						CjvxSaWrapperElementBase::getDisplayUnitToken(transtp, unit);
						valD = JVX_MAX(valD, minVal);
						valD = JVX_MIN(valD, maxVal);
						txtShow = jvx_data2String(valD, numDigits) + " " + unit;

						//valD = cbk->transformTo(valD, transtp, unit);
						// txtShow = jvx_data2String(valD, numDigits);
						break;

					case JVX_DATAFORMAT_16BIT_LE:
						if (connectedProp.decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
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
						else if (connectedProp.decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
						{
							txtShow = jvxDataFormat_txt(valI16);
						}
						else
						{
							txtShow = jvx_int2String(valI16);
						}
						break;
					case JVX_DATAFORMAT_8BIT:
						txtShow = jvx_int2String(valI8);
						break;
					case JVX_DATAFORMAT_32BIT_LE:
						if (connectedProp.decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
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
					case JVX_DATAFORMAT_64BIT_LE:
						txtShow = jvx_int642String(valI64);
						break;
					case JVX_DATAFORMAT_STRING:
						txtShow = "-none-error-";
						txtShow = fldStr.std_str();
						break;
					default:
						assert(0);
					}
					this->setText(txtShow.c_str());
				}
			}
			else
			{
				std::string txtShowPP;
				jvx::helper::properties::toString(propRefLoc, callGate, connectedProp, txtShow, txtShowPP, numDigits, false);// no need to use txtShowPP since binList is false
				this->setText(txtShow.c_str());
			}
		}
	}
	return true;
}

void 
QLineEdit_fdb::focusInEvent(QFocusEvent* e)
{
	if (e->reason() == Qt::MouseFocusReason)
	{
	}

	// You might also call the parent method.
	QLineEdit::focusInEvent(e);
};

void 
QLineEdit_fdb::focusOutEvent(QFocusEvent* e)
{
	if (cbk)
	{
		cbk->editingCompleted(static_cast<CjvxQtSaWidgetWrapper_elementbase*>(this));
	}

	// You might also call the parent method.
	QLineEdit::focusOutEvent(e);
};


void 
QLineEdit_fdb::newText()
{
	QString txt = this->text();

	IjvxAccessProperties* propRefLoc = NULL;
	jvxHandle* ptrVal = NULL;
	jvxData valD = 0;
	jvxInt16 valI16 = 0;
	jvxInt8 valI8 = 0;
	jvxInt32 valI32 = 0;
	jvxInt64 valI64 = 0;
	jvxSize valS = 0;
	jvxCBool valB = false;
	jvxSelectionList selLst;
	jvxApiString fldStr;
	jvxValueInRange valR;
	jvxErrorType resP = JVX_NO_ERROR;
	jvxWwTransformValueType transtp = JVX_WW_SHOW_DIRECT;
	std::string unit;
	jvxCallManagerProperties callGate;

	std::string txtCpy;
	int numDigits = 4;
	std::string txtstr;
	jvxInt32 intVal;
	jvxData datVal = 0;
	jvxData minVal = JVX_DATA_MAX_NEG;
	jvxData maxVal = JVX_DATA_MAX_POS;
	jvxData transformScale;
	jvxBool foundit;
	std::string token;
	if (cbk)
	{
		cbk->propertyReference(&propRefLoc);
		if (propRefLoc)
		{
			if (connectedProp.num == 1)
			{
				// Currently, only single elements are supported
				switch (connectedProp.format)
				{
				case JVX_DATAFORMAT_SIZE:
					ptrVal = &valS;
					if (txt == "unselected")
						valS = JVX_SIZE_UNSELECTED;
					else
					{
						valS = txt.toInt();
					}
					break;
				case JVX_DATAFORMAT_DATA:
					
					ptrVal = &valD;

					myBackwardRef->keymaps.le.parseEntryList(paramlst, "");
					myBackwardRef->keymaps.le.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32); // if not found, use default
					myBackwardRef->keymaps.le.getValueForKey("SCALEFAC", &transformScale, JVX_WW_KEY_VALUE_TYPE_DATA);// if not found, use 1
					myBackwardRef->keymaps.le.getValueForKey("LABELTRANSFORM", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
					if (foundit)
					{
						transtp = (jvxWwTransformValueType)intVal;
					}
					myBackwardRef->keymaps.le.getValueForKey("MINVAL", &datVal, JVX_WW_KEY_VALUE_TYPE_DATA);
					myBackwardRef->keymaps.le.getValueForKey("MAXVAL", &maxVal, JVX_WW_KEY_VALUE_TYPE_DATA);
					myBackwardRef->keymaps.le.getValueForKey("LABELUNIT", &unit, JVX_WW_KEY_VALUE_TYPE_STRING);

					txtstr = txt.toLatin1().data();
					if (!unit.empty())
					{
						size_t poss = txtstr.rfind(unit);
						if (poss != std::string::npos)
						{
							txtstr = txtstr.substr(0, poss);
						}
					}
					valD = atof(txtstr.c_str());
					valD = JVX_MAX(minVal, valD);
					valD = JVX_MIN(maxVal, valD);

					valD = CjvxSaWrapperElementBase::transformFrom(valD, transtp, transformScale);

					break;
				case JVX_DATAFORMAT_16BIT_LE:
					ptrVal = &valI16;
					if (connectedProp.decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
					{
						if (txt == "dontcare")
						{
							valI16 = -1;
						}
						else
						{
							valI16 = txt.toInt();
						}
					}
					else if (connectedProp.decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
					{
						valI16 = jvxDataFormat_decode(txt.toLatin1().data());
					}
					else
					{
						valI16 = txt.toInt();
					}
					break;
				case JVX_DATAFORMAT_8BIT:
					ptrVal = &valI8;
					valI8 = txt.toInt();
					break;
				case JVX_DATAFORMAT_32BIT_LE:
					ptrVal = &valI32;
					if (connectedProp.decTp == JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE)
					{
						if (txt == "dontcare")
						{
							valI32 = -1;
						}
						else
						{
							valI32 = txt.toInt();
						}
					}
					else
					{
						valI32 = txt.toInt();
					}
					break;
				case JVX_DATAFORMAT_64BIT_LE:
					ptrVal = &valI64;
					valI64 = txt.toInt();
					break;
				case JVX_DATAFORMAT_STRING:
					ptrVal = &fldStr;
					txtCpy = txt.toLatin1().data();
					fldStr.assign_const(txtCpy.c_str(), txtCpy.size());
					break;
				case JVX_DATAFORMAT_VALUE_IN_RANGE:
					ptrVal = &valR;

					myBackwardRef->keymaps.le.parseEntryList(paramlst, "");
					myBackwardRef->keymaps.le.getValueForKey("NUMDIGITS", &numDigits, JVX_WW_KEY_VALUE_TYPE_INT32);
					myBackwardRef->keymaps.le.getValueForKey("SCALEFAC", &transformScale, JVX_WW_KEY_VALUE_TYPE_DATA);
					myBackwardRef->keymaps.le.getValueForKey("LABELTRANSFORM", &intVal, JVX_WW_KEY_VALUE_TYPE_ENUM, &foundit);
					if (foundit)
					{
						transtp = (jvxWwTransformValueType)intVal;
					}
					myBackwardRef->keymaps.le.getValueForKey("MINVAL", &minVal, JVX_WW_KEY_VALUE_TYPE_DATA);
					myBackwardRef->keymaps.le.getValueForKey("MAXVAL", &maxVal, JVX_WW_KEY_VALUE_TYPE_DATA);
					myBackwardRef->keymaps.le.getValueForKey("LABELUNIT", &unit, JVX_WW_KEY_VALUE_TYPE_STRING);

					txtstr = txt.toLatin1().data();
					if (!unit.empty())
					{
						size_t poss = txtstr.rfind(unit);
						if (poss != std::string::npos)
						{
							txtstr = txtstr.substr(0, poss);
						}
					}
					valD = atof(txtstr.c_str());
					valD = JVX_MAX(minVal, valD);
					valD = JVX_MIN(maxVal, valD);

					valR.val() = CjvxSaWrapperElementBase::transformFrom(valD, transtp, transformScale);
					break;
				default:
					assert(0);
				}
				token = connectedProp.descriptor.std_str();
				ident.reset(token.c_str());
				trans.reset(true, 0, connectedProp.decTp, false);
				resP = propRefLoc->set_property(callGate, 
					jPRG(ptrVal, 1, connectedProp.format),
					ident, trans);
			}
			else
			{
				jvx::helper::properties::fromString(propRefLoc, callGate, connectedProp, txt.toLatin1().data(), "", 0);// no need to use txtShowPP since binList is false
			}
		}
		token = connectedProp.descriptor.std_str();
		cbk->reportPropertySetItem(myTag.c_str(), token.c_str(), myBasePropIs.reportTp, it, resP);
		cbk->editingCompleted(static_cast<CjvxQtSaWidgetWrapper_elementbase*>(this));
	}
};

// =========================================================================================
// =========================================================================================

QCheckBox_fdb::QCheckBox_fdb(QTreeWidgetItem* assoc, CjvxMaWrapperElementTreeWidget* cb, QWidget* parent, jvxBool allowReadArg, jvxBool allowWriteArg) :
	QCheckBox(parent), CjvxQtSaWidgetWrapper_elementbase( assoc, cb, allowReadArg, allowWriteArg)
{
	connect(this, SIGNAL(clicked(bool)), this, SLOT(toggled(bool)));
	theWidget = static_cast<QWidget*>(this);
};

bool
QCheckBox_fdb::update_window()
{
	IjvxAccessProperties* propRefLoc = NULL;
	jvxSelectionList selLst;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	std::string token;
	jvxSize i = 0;
	std::string txt;
	jvxCBool valCB;

	if (cbk)
	{
		cbk->propertyReference(&propRefLoc);
		if (propRefLoc)
		{
			token = connectedProp.descriptor.std_str();
			assert(connectedProp.format == JVX_DATAFORMAT_16BIT_LE);
			ident.reset(token.c_str());
			trans.reset(false, 0, connectedProp.decTp);
			res = propRefLoc->get_property(callGate, jPRG( &valCB, 1, connectedProp.format), ident, trans);
			if(valCB == c_false)
			{ 
				this->setChecked(false);
			}
			else
			{
				this->setChecked(true);
			}
		}
	}
	return true;
}

void
QCheckBox_fdb::focusInEvent(QFocusEvent* e)
{
	if (e->reason() == Qt::MouseFocusReason)
	{

	}

	// You might also call the parent method.
	QCheckBox::focusInEvent(e);
};

void
QCheckBox_fdb::focusOutEvent(QFocusEvent* e)
{
	if (e->reason() == Qt::MouseFocusReason)
	{
		if (cbk)
		{
			cbk->editingCompleted(static_cast<CjvxQtSaWidgetWrapper_elementbase*>(this));
		}
	}

	// You might also call the parent method.
	QCheckBox::focusOutEvent(e);
};


void
QCheckBox_fdb::toggled(bool tog)
{
	IjvxAccessProperties* propRefLoc = NULL;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	jvxErrorType res = JVX_NO_ERROR;

	if (cbk)
	{
		cbk->propertyReference(&propRefLoc);
		if (propRefLoc)
		{
			jvxCBool valCB = c_false;
			if (tog)
			{
				valCB = c_true;
			}
			assert(connectedProp.format == JVX_DATAFORMAT_16BIT_LE);
			ident.reset(connectedProp.descriptor.c_str());
			trans.reset(true, 0, connectedProp.decTp, false);
			res = propRefLoc->set_property(callGate,
				jPRG(&valCB, 1, connectedProp.format), ident, trans);
			
		}
		update_window();
	}
};
