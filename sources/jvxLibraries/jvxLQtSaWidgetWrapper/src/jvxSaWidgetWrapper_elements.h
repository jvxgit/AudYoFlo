#ifndef __JVXSAWIDGETWRAPPER_ELEMENTS_H__
#define __JVXSAWIDGETWRAPPER_ELEMENTS_H__

#include <QLineEdit>
#include <QFocusEvent>
#include <QComboBox>
#include <QCheckBox>
#include <QSlider>
#include "jvx.h"

#include "CjvxQtSaWidgetWrapper_keymap.h"
#include "CjvxQtSaWidgetWrapper_types.h"

class CjvxMaWrapperElementTreeWidget;
class QTreeWidgetItem;
class CjvxQtSaWidgetWrapper;
// ======================================================================
// ======================================================================

class CjvxQtSaWidgetWrapper_elementbase
{
protected:
	QWidget* theWidget;

	CjvxQtSaWidgetWrapper_elementbase(QTreeWidgetItem* assoc, CjvxMaWrapperElementTreeWidget* cb, jvxBool allowRead, jvxBool allowWrite);

	CjvxMaWrapperElementTreeWidget* cbk;
	jvxPropertyDescriptor connectedProp;
	QTreeWidgetItem* it;
	std::string myTag;
	std::string propName;
	std::vector<std::string> paramlst;
	basePropInfos myBasePropIs;
	CjvxQtSaWidgetWrapper* myBackwardRef;
	jvxBool allowRead;
	jvxBool allowWrite;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

public:
	QWidget* getMyWidget() { return(theWidget);}
	virtual bool update_window() = 0;
};

// ======================================================================

class QComboBox_fdb : public QComboBox, public CjvxQtSaWidgetWrapper_elementbase
{
	Q_OBJECT

public:

	QComboBox_fdb(QTreeWidgetItem* assoc, CjvxMaWrapperElementTreeWidget* cb, QWidget* parent, jvxBool allowRead, jvxBool allowWrite);
	virtual bool update_window();
	virtual void focusInEvent(QFocusEvent* e);
	virtual void focusOutEvent(QFocusEvent* e);
	void mousePressEvent(QMouseEvent * event);

	public slots:
	void newSelection(int sel);
};

// ======================================================================

class QSlider_fdb : public QSlider, public CjvxQtSaWidgetWrapper_elementbase
{
	Q_OBJECT

public:


	QSlider_fdb(QTreeWidgetItem* assoc, CjvxMaWrapperElementTreeWidget* cb, QWidget* parent, jvxBool allowRead, jvxBool allowWrite);

	virtual bool update_window();
	virtual void focusInEvent(QFocusEvent* e);
	virtual void focusOutEvent(QFocusEvent* e);

public slots:

	void sliderHasBeenPressed();
	void newSliderPosition();
};


class QLineEdit_fdb : public QLineEdit, public CjvxQtSaWidgetWrapper_elementbase
{
	Q_OBJECT

public:


	QLineEdit_fdb(QTreeWidgetItem* assoc, CjvxMaWrapperElementTreeWidget* cb, QWidget* parent, jvxBool allowRead, jvxBool allowWrite);
	virtual bool update_window();
	virtual void focusInEvent(QFocusEvent* e);
	virtual void focusOutEvent(QFocusEvent* e);

public slots:
	void newText();
};

class QCheckBox_fdb : public QCheckBox, public CjvxQtSaWidgetWrapper_elementbase
{
	Q_OBJECT

public:


	QCheckBox_fdb(QTreeWidgetItem* assoc, CjvxMaWrapperElementTreeWidget* cb, QWidget* parent, jvxBool allowRead, jvxBool allowWrite);

	virtual bool update_window();
	virtual void focusInEvent(QFocusEvent* e);
	virtual void focusOutEvent(QFocusEvent* e);

public slots:

	void toggled(bool);
};

#endif