#include "uLoadSaveDialog.h"

uLoadSaveDialog::uLoadSaveDialog(QWidget* parent): QDialog(parent)
{
	stopVariable = NULL;
}

uLoadSaveDialog::~uLoadSaveDialog()
{
}

void 
uLoadSaveDialog::setupUi()
{
	Ui::Dialog::setupUi(static_cast<QDialog*>(this));
}

void 
uLoadSaveDialog::setStopVariable(jvxBool* stop)
{
	stopVariable = stop;
}

void 
uLoadSaveDialog::setText(std::string & fileName)
{
	label_post->setText(("Reading " + fileName).c_str());
}
			
void 
uLoadSaveDialog::setProgress(jvxData val)
{
	this->progressBar->setValue(val* 100.0);
}

void 
uLoadSaveDialog::accept()
{
	if(stopVariable)
	{
		*stopVariable = false;
	}
	QDialog::accept();
}

