#include "CjvxApplication.h"
#include <QTouchEvent>
#include <QMouseEvent>
#include <QDebug>
#include <iostream>

CjvxApplication::CjvxApplication(int &argc, char **argv) : QApplication(argc, argv)
{

}

bool 
CjvxApplication::notify(QObject* target, QEvent* event)
{
	QMouseEvent* mev = NULL;

	switch (event->type())
	{
		/*
	case QEvent::MouseMove:
		mev = static_cast<QMouseEvent*>(event);
		qDebug << mev->pos() << std::endl;
		break;
		*/
		/*
		case QEvent::TouchBegin:
		case QEvent::TouchUpdate:
		case QEvent::TouchEnd:
		{
			QTouchEvent* te = static_cast<QTouchEvent*>(event);

			if (te->device()->type() == QTouchDevice::TouchScreen)
			{
				QList<QTouchEvent::TouchPoint> tps = te->touchPoints();

				if(tps.count() != 1)
				{
					qDebug() << "Touch points != 1";
					return true;
				}

				qreal tx = tps.first().pos().x();
				qreal ty = tps.first().pos().y();

				qreal mx = 480.0/800 * ty;
				qreal my = 800 - 800.0/480 * tx;

				qDebug() << tx << "," << ty << " => " << mx << "," << my;

				QPointF mp(mx, my);

				switch (event->type())
				{
				case QEvent::TouchBegin:
				{
					qDebug() << "BEGIN";
					QMouseEvent ee(QEvent::MouseButtonPress, mp, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
					QCoreApplication::sendEvent(target, &ee);
					break;
				}
				case QEvent::TouchUpdate:
				{
					qDebug() << "UPDATE";
					QMouseEvent ee(QEvent::MouseMove, mp, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
					QCoreApplication::sendEvent(target, &ee);
					break;
				}
				case QEvent::TouchEnd:
				{
					qDebug() << "END";
					QMouseEvent ee(QEvent::MouseButtonRelease, mp, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
					QCoreApplication::sendEvent(target, &ee);
					break;
				}
				default:
					qDebug() << "Unhandled touch event";
					return true;
				}

				return true;
			}
			break;
		}*/
	case QEvent::MouseButtonPress:
	{
		QMouseEvent *k = (QMouseEvent *)event;
		qDebug() << "MouseButtonPress:" << k->pos();
		break;
	}
	case QEvent::MouseMove:
	{
		QMouseEvent *k = (QMouseEvent *)event;
		
		Qt::MouseEventSource src = k->source();
		qDebug() << "MouseMove:" << k->localPos() << k->windowPos() << k->globalPos() << k->type();
		switch (src)
		{
		case Qt::MouseEventNotSynthesized:
			qDebug() << "Qt::MouseEventNotSynthesized";
			break;
		case Qt::MouseEventSynthesizedBySystem:
			qDebug() << "Qt::MouseEventSynthesizedBySystem";
			break;
		case Qt::MouseEventSynthesizedByQt:
			qDebug() << "Qt::MouseEventSynthesizedByQt";
			break;
		case Qt::MouseEventSynthesizedByApplication:
			qDebug() << "Qt::MouseEventSynthesizedByApplication";
			break;
		}

		QPointF lpt = k->localPos();
		QPointF spt = k->screenPos();
		QPointF wpt = k->windowPos();
		int x = wpt.x();
		int y = wpt.y();
		x = 100 - x;
		if (x < 0)
			x = 0;
		wpt.setX(x);
		x = lpt.x();
		y = lpt.y();
		x = 100 - x;
		if (x < 0)
			x = 0;
		lpt.setX(x);
		x = spt.x();
		y = spt.y();
		x = 100 - x;
		if (x < 0)
			x = 0;
		spt.setX(x);

		if (src == Qt::MouseEventNotSynthesized)
		{
			QMouseEvent ee(QEvent::MouseMove, lpt, wpt, spt, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier, Qt::MouseEventSynthesizedByApplication);
			QCoreApplication::sendEvent(target, &ee);
			return true;
		}
		break;
	}
	case QEvent::MouseButtonRelease:
	{
		QMouseEvent *k = (QMouseEvent *)event;
		qDebug() << "MouseButtonRelease:" << k->pos();
		break;
	}
	default:
		;
	}

    return QApplication::notify(target, event);
 }