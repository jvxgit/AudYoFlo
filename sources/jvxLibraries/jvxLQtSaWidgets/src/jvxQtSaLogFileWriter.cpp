#include "jvxQtSaLogFileWriter.h"
#include "ui_jvxQtLogFileWriter.h"

jvxQtSaLogFileWriter::jvxQtSaLogFileWriter(QWidget *parent)
        : QWidget(parent)
{
	ui = new Ui::myJvxFileWriterWidget;
	ui->setupUi(this);
	this->clearUiElements();
}

jvxQtSaLogFileWriter::~jvxQtSaLogFileWriter()
{
}

jvxErrorType
jvxQtSaLogFileWriter::clearUiElements()
{	
	ui->checkBox_activate->setEnabled(false);
	ui->checkBox_activate->setChecked(false);
	ui->lineEdit_prefix->setText("--");
	ui->lineEdit_prefix->setEnabled(false);
	ui->lineEdit_folder->setText("--");
	ui->lineEdit_folder->setEnabled(false);
	ui->progressBar_buffer->setValue(0);
	ui->progressBar_buffer->setEnabled(false);
	return(JVX_NO_ERROR);
}

void 
jvxQtSaLogFileWriter::getWidgetReferences(QCheckBox** activateCb, QLineEdit** filePrefixLe, QLineEdit** folderLe, QPushButton** dirPb, QProgressBar** progrPb, QGroupBox** gb)
{
	if(activateCb)
	{
		*activateCb = ui->checkBox_activate;
	}
	if(filePrefixLe)
	{
		*filePrefixLe = ui->lineEdit_prefix;
	}
	if(folderLe)
	{
		*folderLe = ui->lineEdit_folder;
	}
	if(dirPb)
	{
		*dirPb = ui->pushButton_folder;
	}
	if(progrPb)
	{
		*progrPb = ui->progressBar_buffer;
	}
	if (gb)
	{
		*gb = ui->groupBox;
	}
}


jvxErrorType 
jvxQtSaLogFileWriter::setWidgetTitle(const char* tit)
{
	ui->groupBox->setTitle(tit);
	return(JVX_NO_ERROR);
}
