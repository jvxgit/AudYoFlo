#include "jvxVertLabel.h"

VertLabel::VertLabel(QString txt, QWidget *parent): QLabel(parent)
{
    text = txt;
	/*
	text = txt;
    QFont font("times",36);
    QFontMetrics fm(font);
    int pixelsWide = fm.width(txt);
    int pixelsHigh = fm.height();
    this->resize(pixelsHigh,pixelsWide);
	*/
}

void VertLabel::paintEvent(QPaintEvent * ev)
{
	QLabel::paintEvent(ev);
        //QPainter painter(this);
        //painter.setPen(Qt::black);
        //painter.rotate(90);
        //painter.drawText(QPoint(0,-15), text);
		
		/*
		QFont font("times",36);
        QPainter painter(this);
        painter.setFont(font);
        painter.setPen(Qt::black);
        painter.rotate(90);
        painter.drawText(QPoint(0,-15), text);
		*/
}

void VertLabel::changeText(QString txt)
{
    text = txt;
    update();
}