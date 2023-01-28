#include "jvx.h"
#include "jvx_newvalue_dialog.h"

jvx_newvalue_dialog::jvx_newvalue_dialog(QWidget* parent, std::string txt, jvxInt32* value): QDialog(parent)
{
	this->setupUi(this);
	this->valueModify_int = value;
	this->updateType = JVX_DATAFORMAT_32BIT_LE;
	this->groupBox_what->setTitle(txt.c_str());
	this->setWindowTitle("Specify new value");
	this->lineEdit->setText(jvx_int2String(*value).c_str());
}

jvx_newvalue_dialog::jvx_newvalue_dialog(QWidget* parent, std::string txt, jvxData* value, jvxSize comma): QDialog(parent)
{
	this->setupUi(this);
	this->valueModify_dbl = value;
	this->updateType = JVX_DATAFORMAT_DATA;
	this->groupBox_what->setTitle(txt.c_str());
	this->setWindowTitle("Specify new value");
	this->lineEdit->setText(jvx_data2String(*value, (int)comma).c_str());
}

jvx_newvalue_dialog::jvx_newvalue_dialog(QWidget* parent, std::string txt, std::string* value): QDialog(parent)
{
	this->setupUi(this);
	this->valueModify_txt = value;
	this->updateType = JVX_DATAFORMAT_STRING;
	this->groupBox_what->setTitle(txt.c_str());
	this->setWindowTitle("Specify new value");
	this->lineEdit->setText(value->c_str());
}

void
jvx_newvalue_dialog::setupUi(QDialog *Dialog_newValueLineedit)
{
	if (Dialog_newValueLineedit->objectName().isEmpty())
		Dialog_newValueLineedit->setObjectName(QStringLiteral("Dialog_newValueLineedit"));
	Dialog_newValueLineedit->resize(182, 56);
	groupBox_what = new QGroupBox(Dialog_newValueLineedit);
	groupBox_what->setObjectName(QStringLiteral("groupBox_what"));
	groupBox_what->setGeometry(QRect(2, 2, 179, 51));
	groupBox_what->setAutoFillBackground(true);
	lineEdit = new QLineEdit(groupBox_what);
	lineEdit->setObjectName(QStringLiteral("lineEdit"));
	lineEdit->setGeometry(QRect(8, 22, 165, 20));

	QObject::connect(lineEdit, SIGNAL(returnPressed()), Dialog_newValueLineedit, SLOT(newValueReturnPressed()));
} 

void 
jvx_newvalue_dialog::newValueReturnPressed()
{
	int newVal_int = 0;
	jvxData newVal_dbl = 0.0;
	QString newVal_txt;
	switch(updateType)
	{
	case JVX_DATAFORMAT_32BIT_LE:
		newVal_int = this->lineEdit->text().toInt();
		if(valueModify_int)
		{
			*valueModify_int = newVal_int;
		}
		break;
	case JVX_DATAFORMAT_DATA:
		newVal_dbl = this->lineEdit->text().toData();
		if(valueModify_dbl)
		{
			*valueModify_dbl = newVal_dbl;
		}
		break;
	case JVX_DATAFORMAT_STRING:
		newVal_txt = this->lineEdit->text();
		if(valueModify_txt)
		{
			*valueModify_txt = newVal_txt.toLatin1().data();
		}
		break;
	}
	this->accept();
}