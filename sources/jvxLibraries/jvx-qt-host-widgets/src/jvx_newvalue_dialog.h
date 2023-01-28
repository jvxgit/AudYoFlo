#ifndef _JVX_NEWVALUE_DIALOG_H__
#define _JVX_NEWVALUE_DIALOG_H__

#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>

#include "jvx.h"

class jvx_newvalue_dialog: public QDialog
{
	Q_OBJECT

private:
	QGroupBox *groupBox_what;
	QLineEdit *lineEdit;

	jvxInt32* valueModify_int;
	jvxData* valueModify_dbl;
	std::string* valueModify_txt;

	jvxDataFormat updateType;

	void setupUi(QDialog *Dialog_newValueLineedit);

public:

	jvx_newvalue_dialog(QWidget* parent, std::string text, jvxInt32* value);
	jvx_newvalue_dialog(QWidget* parent, std::string txt, jvxData* value, jvxSize comma);
	jvx_newvalue_dialog(QWidget* parent, std::string txt, std::string* value);

public slots:
	void newValueReturnPressed();
	
};

#endif
