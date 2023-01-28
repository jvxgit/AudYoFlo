#ifndef __jvxQtSaLogFileWriter_H__
#define __jvxQtSaLogFileWriter_H__

#include <QtWidgets/QWidget>
#include <QtGui/QPainter>

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QGroupBox>

#include "jvx.h"
#include "jvxQtSaCustomBase.h"

// forward declarations
namespace Ui {
     class myJvxFileWriterWidget;
}

class jvxQtSaLogFileWriter : public QWidget, public jvxQtSaCustomBase
{
	Ui::myJvxFileWriterWidget* ui;

    Q_OBJECT

public:

    jvxQtSaLogFileWriter(QWidget *parent = 0);
	~jvxQtSaLogFileWriter();

	void getWidgetReferences(QCheckBox** activateCb, QLineEdit** filePrefixLe, QLineEdit** folderLe, QPushButton** dirPb, QProgressBar** progrPb, QGroupBox** gb);
	
	virtual jvxErrorType setWidgetTitle(const char* tit);
	virtual jvxErrorType clearUiElements();


public slots:

protected:
private:
};

#endif
