#ifndef __MESSAGESOUTPUT_H__
#define __MESSAGESOUTPUT_H__

#include "ui_messagesOutput.h"

class messagesOutput: public QWidget, public Ui::Form_messagesOutput
{
	Q_OBJECT

public:
	
	messagesOutput(QWidget* parent): QWidget(parent){};

	void init();

	void  postMessage_inThread(QString txt, QColor col);

public slots:
	void trigger_clear_messages();
};

#endif
	