#ifndef __jvxQtSaNetworkTechnology_H__
#define __jvxQtSaNetworkTechnology_H__

#include <QtWidgets/QWidget>

/*
#include <QtGui/QPainter>
*/
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>

#include "jvx.h"
#include "jvxQtSaCustomBase.h"
#include "jvxQtSaWidgetsConfigure.h"

// forward declarations
namespace Ui {
     class myJvxNetworkTechnology;
}

class jvxQtSaNetworkTechnology : public QWidget, public jvxQtSaCustomBase
{
public:

	Ui::myJvxNetworkTechnology* ui;

    Q_OBJECT

public:

    jvxQtSaNetworkTechnology(QWidget *parent = 0);
	~jvxQtSaNetworkTechnology();

	void init(jvxQtSaWidgetsConfigure::jvxQtNetworkConfigure* myConfig);

	void getWidgetReferences(QComboBox** cb_tech);

public slots:

protected:
private:
};

#endif
