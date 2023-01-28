#ifndef __jvxQtSaLightOnOffWidget_H__
#define __jvxQtSaLightOnOffWidget_H__

#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include "jvx.h"
#include "jvxQtSaCustomBase.h"


class jvxQtSaLightOnOffWidget : public QWidget, public jvxQtSaCustomBase
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
public:
    jvxQtSaLightOnOffWidget(QWidget *parent = 0);

	void setColorOn(const QColor &color);
	void setColorOff(const QColor &color);

	bool isOn();
    void setOn(bool on);

public slots:
    void turnOff();
    void turnOn();

protected:
    virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
private:
    QColor m_color_on;
    QColor m_color_off;
    bool m_on;
};

#endif
