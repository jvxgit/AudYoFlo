#include "jvxQtSaNetworkMasterDevice.h"
#include "ui_jvxQtNetworkMasterDevice.h"
#include "jvx-qt-helpers.h"


jvxQtSaNetworkMasterDevice::jvxQtSaNetworkMasterDevice(QWidget *parent)
        : QWidget(parent)
{
	ui = NULL;
	//myPropPrefix = "/JVX_GENW";
	/* addConfigTokens = "";*/
}

jvxQtSaNetworkMasterDevice::~jvxQtSaNetworkMasterDevice()
{
	if(ui)
	{
		delete ui;
	}
	ui = NULL;
}

void 
jvxQtSaNetworkMasterDevice::init(jvxQtSaWidgetsConfigure::jvxQtNetworkConfigure* myConfig)
{
	QFont font;
	QPalette pal;
	QBrush brush_gray(QColor(220, 220, 220, 255));
	QBrush brush_white(QColor(255, 255, 255, 255));
	QBrush brush_black(QColor(0, 0, 0, 255));
	QPalette palette;
    QBrush brush_txt(myConfig->colorTxt);

	ui = new Ui::myJvxNetwork_master;
	ui->setupUi(this);

    brush_txt.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::WindowText, brush_txt);
	
	font.setPointSize(myConfig->fontSize);
    font.setBold(false);
    font.setItalic(true);
	ui->label->setFont(font);
	ui->label->setPalette(palette);
	ui->label_2->setFont(font);
	ui->label_2->setPalette(palette);
	ui->label_3->setFont(font);
	ui->label_3->setPalette(palette);
	ui->label_4->setFont(font);
	ui->label_4->setPalette(palette);
	ui->label_5->setFont(font);
	ui->label_5->setPalette(palette);
	ui->label_6->setFont(font);
	ui->label_6->setPalette(palette);
	ui->label_7->setFont(font);
	ui->label_7->setPalette(palette);
	ui->label_8->setFont(font);
	ui->label_8->setPalette(palette);
	ui->label_9->setFont(font);
	ui->label_9->setPalette(palette);
	ui->label_10->setFont(font);
	ui->label_10->setPalette(palette);

	font.setItalic(false);
	ui->lineEdit_host->setFont(font);
	ui->lineEdit_port->setFont(font);
	ui->pushButton_connect->setFont(font);
	ui->lineEdit_performance->setFont(font);
	ui->checkBox_autoconnect->setFont(font);
	ui->checkBox_autoconnect->setPalette(palette);
	ui->checkBox_autostart->setFont(font);
	ui->checkBox_autostart->setPalette(palette);
	ui->lineEdit_tech->setFont(font);
	ui->lineEdit_tech->setPalette(palette);

	setBackgroundLabelColor(Qt::white, ui->label_stat0);
	setBackgroundLabelColor(Qt::white, ui->label_stat1);
	setBackgroundLabelColor(Qt::white, ui->label_stat2);
	setBackgroundLabelColor(Qt::white, ui->label_stat3);
	setBackgroundLabelColor(Qt::white, ui->label_stat4);
	setBackgroundLabelColor(Qt::white, ui->label_stat5);

}

void 
jvxQtSaNetworkMasterDevice::getWidgetReferences(QLineEdit** le_host, QLineEdit** le_port, QPushButton** pb_connect, 
	QLineEdit** le_perform, QLabel** stats, QCheckBox** cb_auto, 
	QCheckBox** cb_start, QComboBox** cb_mode, QLineEdit** le_prof, QCheckBox** cb_log, QLineEdit** le_tech)

{
	if(le_host)
	{
		*le_host = ui->lineEdit_host;
	}
	if(le_port)
	{
		*le_port = ui->lineEdit_port;
	}
	if(pb_connect)
	{
		*pb_connect = ui->pushButton_connect;
	}
	if(le_perform)
	{
		*le_perform = ui->lineEdit_performance;
	}
	if(stats)
	{
		stats[0] = ui->label_stat0;
		stats[1] = ui->label_stat1;
		stats[2] = ui->label_stat2;
		stats[3] = ui->label_stat3;
		stats[4] = ui->label_stat4;
		stats[5] = ui->label_stat5;
	}
	if(cb_auto)
	{
		*cb_auto = ui->checkBox_autoconnect;
	}
	if(cb_start)
	{
		*cb_start = ui->checkBox_autostart;
	}
	if (cb_mode)
	{
		*cb_mode = ui->comboBox_mode;
	}
	if (le_prof)
	{
		*le_prof = ui->lineEdit_profile;
	}
	if(cb_log)
	{ 
		*cb_log = ui->checkBox_log;
	}
	if (le_tech) 
	{
		*le_tech = ui->lineEdit_tech;
	}
}

