#include "jvxQtSaLightOnOffWidget.h"

jvxQtSaLightOnOffWidget::jvxQtSaLightOnOffWidget(QWidget *parent)
        : QWidget(parent), m_on(false) 
{
	m_color_on = Qt::green;
	m_color_off = Qt::white;
	this->setMinimumSize(QSize(20, 20));
}

void 
	jvxQtSaLightOnOffWidget::setColorOn(const QColor &color)
{
	m_color_on = color;
}

void 
	jvxQtSaLightOnOffWidget::setColorOff(const QColor &color)
{
	m_color_off = color;
}

bool 
jvxQtSaLightOnOffWidget::isOn()
{ 
	return m_on; 
}
 
 void 
 jvxQtSaLightOnOffWidget::setOn(bool on)
 {
        if (on == m_on)
            return;
        m_on = on;
        update();
    }

void 
jvxQtSaLightOnOffWidget::turnOff() 
{ 
	setOn(false); 
}

void 
jvxQtSaLightOnOffWidget::turnOn() 
{ 
	setOn(true); 
}

void 
jvxQtSaLightOnOffWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	if(m_on)
	{
		painter.setBrush(m_color_on);
	}
	else
	{
		painter.setBrush(m_color_off);
	}
	int minVal = JVX_MIN(width(), height());
	int xval = width()-minVal;
	xval = xval /2;
	int yval = height()-minVal;
	yval = yval /2;
	painter.drawEllipse(xval, yval, minVal, minVal);
}
