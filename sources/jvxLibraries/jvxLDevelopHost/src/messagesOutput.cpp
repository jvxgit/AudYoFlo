#include "messagesOutput.h"

void 
messagesOutput::init()
{
	this->setupUi(this);
}

void 
messagesOutput::postMessage_inThread(QString txt, QColor col)
{
	if (checkBox_update->isChecked())
	{
		QListWidgetItem* item = new QListWidgetItem(txt);
		item->setForeground(QBrush(col));
		this->listWidget_messages->addItem(item);
		this->listWidget_messages->setCurrentRow(this->listWidget_messages->count() - 1);
	}
}

void
messagesOutput::trigger_clear_messages()
{
	this->listWidget_messages->clear();
}
