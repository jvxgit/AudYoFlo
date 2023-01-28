#ifndef __IJVXQTMIXMATRIXWIDGET_H__
#define __IJVXQTMIXMATRIXWIDGET_H__

#include "IjvxQtSpecificWidget.h"

JVX_INTERFACE IjvxQtMixMatrixWidget: public IjvxQtSpecificHWidget
{
public:

	virtual ~IjvxQtMixMatrixWidget() {};

	virtual void setNamesColorsChannels(QStringList& lstIn, QStringList& lstOut, QList<QColor>& colorsIn, QList<QColor>& colorsOut) = 0;
};

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtMixMatrixWidget, jvxQtMixMatrix);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtMixMatrixWidget, jvxQtMixMatrix);

#endif