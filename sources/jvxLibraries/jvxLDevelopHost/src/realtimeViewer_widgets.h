#ifndef __REAL_TIMEVIEWER_WIDGETS_H__
#define __REAL_TIMEVIEWER_WIDGETS_H__

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSlider>
#include "realtimeViewerOneProperty.h"

class QComboBox_ext: public QComboBox
{
	Q_OBJECT

private:
	realtimeViewerOneProperty* theParent;
	jvxBool oldUpdateState;
	jvxBool startedWithRightClick;
	jvxSize startedForMode;
public:

	QComboBox_ext(realtimeViewerOneProperty* parent, jvxSize mode = 0);
	virtual void focusInEvent(QFocusEvent* e);
	virtual void focusOutEvent(QFocusEvent* e);
	void mousePressEvent ( QMouseEvent * event );
public slots:
  void newSelection(int sel);
};


class QSlider_ext: public QSlider
{
	Q_OBJECT

private:
	realtimeViewerOneProperty* theParent;
	jvxBool oldUpdateState;

public:


 QSlider_ext(realtimeViewerOneProperty* parent);


public slots:

  void sliderHasBeenPressed();
  void newSliderPosition();
};


class QLineEdit_ext: public QLineEdit
{
	Q_OBJECT

private:
	realtimeViewerOneProperty* theParent;
	jvxBool oldUpdateState;

public:


        QLineEdit_ext(realtimeViewerOneProperty* parent);

        virtual void focusInEvent(QFocusEvent* e);
        virtual void focusOutEvent(QFocusEvent* e);

        public slots:
          void newText();
};

class QCheckBox_ext : public QCheckBox
{
	Q_OBJECT

private:
	realtimeViewerOneProperty* theParent;	
	jvxBool oldUpdateState;
public:


	QCheckBox_ext(realtimeViewerOneProperty* parent);

	virtual void focusInEvent(QFocusEvent* e);
	virtual void focusOutEvent(QFocusEvent* e);

public slots:
	void wasClicked(bool tog);
};

#endif
