#include "jvxQtSaNetworkSlaveDevice.h"
#include "ui_jvxQtNetworkSlaveDevice.h"
#include "jvx-qt-helpers.h"


jvxQtSaNetworkSlaveDevice::jvxQtSaNetworkSlaveDevice(QWidget *parent)
        : QWidget(parent)
{
	ui = NULL;
	//myPropPrefix = "/JVX_GENW";
	/* addConfigTokens = "";*/
}

jvxQtSaNetworkSlaveDevice::~jvxQtSaNetworkSlaveDevice()
{
	if(ui)
	{
		delete ui;
	}
	ui = NULL;
}

void 
jvxQtSaNetworkSlaveDevice::init(jvxQtSaWidgetsConfigure::jvxQtNetworkConfigure* myConfig)
{
	QFont font;
	QPalette pal;
	QBrush brush_gray(QColor(220, 220, 220, 255));
	QBrush brush_white(QColor(255, 255, 255, 255));
	QBrush brush_black(QColor(0, 0, 0, 255));
	QPalette palette;
    QBrush brush_txt(myConfig->colorTxt);

	ui = new Ui::myJvxNetwork_slave;
	ui->setupUi(this);

    brush_txt.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::WindowText, brush_txt);
	
	font.setPointSize(myConfig->fontSize);
    font.setBold(false);
    font.setItalic(true);

	ui->lineEdit_buffersize->setFont(font);
	ui->lineEdit_numberInChannels->setFont(font);
	ui->lineEdit_samplerate->setFont(font);
	ui->lineEdit_numberOutChannels->setFont(font);
	ui->comboBox_formats->setFont(font);
	ui->checkBox_autoaudio->setFont(font);

	ui->lineEdit_buffersize->setPalette(palette);
	ui->lineEdit_numberInChannels->setPalette(palette);
	ui->lineEdit_samplerate->setPalette(palette);
	ui->lineEdit_numberOutChannels->setPalette(palette);
	ui->comboBox_formats->setPalette(palette);
	ui->checkBox_autoaudio->setPalette(palette);

	ui->label_connType->setPalette(palette);
	ui->label_connType->setFont(font);

	ui->label_listenPort->setPalette(palette);
	ui->label_listenPort->setFont(font);

	ui->label_srcAddr->setPalette(palette);
	ui->label_srcAddr->setFont(font);

	ui->label_transMode->setPalette(palette);
	ui->label_transMode->setFont(font);

	ui->label_chanI->setPalette(palette);
	ui->label_chanI->setFont(font);

	ui->label_chanO->setPalette(palette);
	ui->label_chanO->setFont(font);

	ui->label_srate->setPalette(palette);
	ui->label_srate->setFont(font);

	ui->label_bsize->setPalette(palette);
	ui->label_bsize->setFont(font);

	ui->groupBox_conn->setPalette(palette);
	ui->groupBox_conn->setFont(font);

	ui->groupBox_audio->setPalette(palette);
	ui->groupBox_audio->setFont(font);

	ui->label_lost->setPalette(palette);
	ui->label_lost->setFont(font);

	ui->label_specload->setPalette(palette);
	ui->label_specload->setFont(font);

	ui->label_status->setPalette(palette);
	ui->label_status->setFont(font);

	ui->label_misc->setPalette(palette);
	ui->label_misc->setFont(font);

	ui->groupBox_connctrl->setPalette(palette);
	ui->groupBox_connctrl->setFont(font);

	ui->pushButton_listen->setPalette(palette);
	ui->pushButton_listen->setFont(font);

	// =======================================================================================

	font.setItalic(false);

	ui->lineEdit_connectionPort->setFont(font);
	ui->comboBox_mode->setFont(font);
	ui->lineEdit_connectionSource->setFont(font);
	ui->lineEdit_connectionType->setFont(font);
	ui->lineEdit_performance->setFont(font);
	ui->lineEdit_profile->setFont(font);
	ui->checkBox_log->setFont(font);
	ui->checkBox_autostart->setFont(font);

	ui->lineEdit_connectionPort->setPalette(palette);
	ui->comboBox_mode->setPalette(palette);
	ui->lineEdit_connectionSource->setPalette(palette);
	ui->lineEdit_connectionType->setPalette(palette);
	ui->lineEdit_performance->setPalette(palette);
	ui->lineEdit_profile->setPalette(palette);
	ui->checkBox_log->setPalette(palette);
	ui->checkBox_autostart->setPalette(palette);

	setBackgroundLabelColor(Qt::white, ui->label_stat0);
	setBackgroundLabelColor(Qt::white, ui->label_stat1);
	setBackgroundLabelColor(Qt::white, ui->label_stat2);
	setBackgroundLabelColor(Qt::white, ui->label_stat3);
	setBackgroundLabelColor(Qt::white, ui->label_stat4);
	setBackgroundLabelColor(Qt::white, ui->label_stat5);

}

void 
jvxQtSaNetworkSlaveDevice::getWidgetReferences(jvxQtSaNetworkSlaveDevice::theWidgetRefs* passalong)

{
	if (passalong)
	{
		memset(passalong, 0, sizeof(jvxQtSaNetworkSlaveDevice::theWidgetRefs));
		passalong->lineEdit_buffersize = ui->lineEdit_buffersize;
		passalong->lineEdit_numberInChannels = ui->lineEdit_numberInChannels;
		passalong->lineEdit_samplerate = ui->lineEdit_samplerate;
		passalong->lineEdit_numberOutChannels = ui->lineEdit_numberOutChannels;
		passalong->comboBox_formats = ui->comboBox_formats;
		passalong->checkBox_autoaudio = ui->checkBox_autoaudio;

		passalong->lineEdit_connectionPort = ui->lineEdit_connectionPort;
		passalong->comboBox_mode = ui->comboBox_mode;
		passalong->lineEdit_connectionSource = ui->lineEdit_connectionSource;
		passalong->lineEdit_connectionType = ui->lineEdit_connectionType;
		passalong->lineEdit_performance = ui->lineEdit_performance;
		passalong->lineEdit_profile = ui->lineEdit_profile;
		passalong->checkBox_log = ui->checkBox_log;
		passalong->checkBox_autostart = ui->checkBox_autostart;
		passalong->pushButton_listen = ui->pushButton_listen;

		passalong->stats[0] = ui->label_stat0;
		passalong->stats[1] = ui->label_stat1;
		passalong->stats[2] = ui->label_stat2;
		passalong->stats[3] = ui->label_stat3;
		passalong->stats[4] = ui->label_stat4;
		passalong->stats[5] = ui->label_stat5;
		passalong->stats[6] = ui->label_stat6;
	}
}

