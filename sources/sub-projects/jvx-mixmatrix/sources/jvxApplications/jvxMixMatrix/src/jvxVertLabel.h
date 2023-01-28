#ifndef VERTLABEL_H
#define VERTLABEL_H

#include <QLabel>
#include <QPaintEvent>
#include <QPainter>

class VertLabel : public QLabel
{
	Q_OBJECT
public:
	VertLabel(QString txt, QWidget *parent = 0);
	
	void changeText(QString txt);
signals:

protected:
	void paintEvent(QPaintEvent *);

public slots:

protected:
	QString text;

};

#endif // VERTLABEL_H
