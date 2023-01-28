#ifndef __JVXQTSANETWORKSLAVEDEVICE_H__
#define __JVXQTSANETWORKSLAVEDEVICE_H__

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
     class myJvxNetwork_slave;
}

class jvxQtSaNetworkSlaveDevice : public QWidget, public jvxQtSaCustomBase
{
public:

	Ui::myJvxNetwork_slave* ui;

	typedef struct
	{
		QLineEdit* lineEdit_buffersize;
		QLineEdit* lineEdit_numberInChannels;
		QLineEdit* lineEdit_samplerate;
		QLineEdit* lineEdit_numberOutChannels;
		QComboBox* comboBox_formats;
		QCheckBox* checkBox_autoaudio;
		
		QLineEdit* lineEdit_connectionPort;
		QComboBox* comboBox_mode;
		QLineEdit* lineEdit_connectionSource;
		QLineEdit* lineEdit_connectionType;
		QLineEdit* lineEdit_performance;
		QLineEdit* lineEdit_profile;
		QCheckBox* checkBox_log;
		QCheckBox* checkBox_autostart;
		QPushButton* pushButton_listen;

		QLabel *stats[7];
	}theWidgetRefs;

    Q_OBJECT

public:

	jvxQtSaNetworkSlaveDevice(QWidget *parent = 0);
	~jvxQtSaNetworkSlaveDevice();

	void init(jvxQtSaWidgetsConfigure::jvxQtNetworkConfigure* myConfig);

	void getWidgetReferences(jvxQtSaNetworkSlaveDevice::theWidgetRefs* passalong);

public slots:

protected:
private:
};

#endif
