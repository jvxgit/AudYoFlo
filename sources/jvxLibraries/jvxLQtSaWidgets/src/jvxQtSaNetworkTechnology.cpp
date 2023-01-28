#include "jvxQtSaNetworkTechnology.h"
#include "ui_jvxQtNetworkTechnology.h"
#include "jvx-qt-helpers.h"


jvxQtSaNetworkTechnology::jvxQtSaNetworkTechnology(QWidget *parent)
        : QWidget(parent)
{
	ui = NULL;
	//myPropPrefix = "/JVX_GENW";
	/* addConfigTokens = "";*/
}

jvxQtSaNetworkTechnology::~jvxQtSaNetworkTechnology()
{
	if(ui)
	{
		delete ui;
	}
	ui = NULL;
}

void 
jvxQtSaNetworkTechnology::init(jvxQtSaWidgetsConfigure::jvxQtNetworkConfigure* myConfig)
{
	QFont font;
	QPalette pal;
	QBrush brush_gray(QColor(220, 220, 220, 255));
	QBrush brush_white(QColor(255, 255, 255, 255));
	QBrush brush_black(QColor(0, 0, 0, 255));
	QPalette palette;
    QBrush brush_txt(myConfig->colorTxt);

	ui = new Ui::myJvxNetworkTechnology;
	ui->setupUi(this);

    brush_txt.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::WindowText, brush_txt);
	
	font.setPointSize(myConfig->fontSize);
    font.setBold(false);
    font.setItalic(true);
	ui->comboBox->setFont(font);
	ui->comboBox->setPalette(palette);
	ui->label->setFont(font);
	ui->label->setPalette(palette);
	ui->label_7->setFont(font);
	ui->label_7->setPalette(palette);
}

void 
jvxQtSaNetworkTechnology::getWidgetReferences(QComboBox** cb_tech)

{
	if(cb_tech)
	{
		*cb_tech = ui->comboBox;
	}
}

