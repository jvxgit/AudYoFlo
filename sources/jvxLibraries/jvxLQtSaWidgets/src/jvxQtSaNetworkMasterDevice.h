#ifndef __JVXQTSANETWORKMASTERDEVICE_H__
#define __JVXQTSANETWORKMASTERDEVICE_H__

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
     class myJvxNetwork_master;
}

class jvxQtSaNetworkMasterDevice : public QWidget, public jvxQtSaCustomBase
{
public:

	Ui::myJvxNetwork_master* ui;

    Q_OBJECT

public:

	jvxQtSaNetworkMasterDevice(QWidget *parent = 0);
	~jvxQtSaNetworkMasterDevice();

	void init(jvxQtSaWidgetsConfigure::jvxQtNetworkConfigure* myConfig);

	void getWidgetReferences(QLineEdit** le_host, QLineEdit** le_port, QPushButton** pb_connect, 
		QLineEdit** le_perform, QLabel** stats, QCheckBox** cb_auto, QCheckBox** cb_start, 
		QComboBox** cb_mode, QLineEdit** le_prof, QCheckBox** cb_log, QLineEdit** le_tech);

public slots:

protected:
private:
};

#endif
