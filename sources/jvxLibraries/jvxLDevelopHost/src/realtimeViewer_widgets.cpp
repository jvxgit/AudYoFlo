#ifndef __REALTIMEVIEWER_WIDGETS_H__
#define __REALTIMEVIEWER_WIDGETS_H__

#include "realtimeViewer_widgets.h"

QComboBox_ext::QComboBox_ext(realtimeViewerOneProperty* parent, jvxSize mode): QComboBox(parent)
{
  theParent = parent;
  startedForMode = mode;
  connect(this, SIGNAL(activated(int)), this, SLOT(newSelection(int)));
  startedWithRightClick = false;
};

void QComboBox_ext::focusInEvent(QFocusEvent* e)
{
  if (e->reason() == Qt::MouseFocusReason)
    {
      theParent->references.theRef->_update_item_in_group_in_section(
                                                                     theParent->references.sectionId, theParent->references.groupId,
                                                                     theParent->references.itemId, &oldUpdateState);

      // Deactivate update for this field...
			theParent->references.theRef->_set_update_item_in_group_in_section(
                                                                                           theParent->references.sectionId, theParent->references.groupId,
                                                                                           theParent->references.itemId, false);
			theParent->updateWindow_update();
    }

  // You might also call the parent method.
  QComboBox::focusInEvent(e);
};

void QComboBox_ext::focusOutEvent(QFocusEvent* e)
{
  if (e->reason() == Qt::MouseFocusReason)
  {
	  theParent->references.theRef->_set_update_item_in_group_in_section(
		  theParent->references.sectionId, theParent->references.groupId,
		  theParent->references.itemId, oldUpdateState);
	  theParent->updateWindow_update();
  }

  // You might also call the parent method.
  QComboBox::focusOutEvent(e);
};

void 
QComboBox_ext::mousePressEvent ( QMouseEvent * event )
{
	/* This implementation allows to start a combobox with a right click */
	QMouseEvent mEvt(QEvent::MouseButtonPress, rect().center(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	if(event->button() == Qt::RightButton)
	{
		startedWithRightClick = true;
	} 
	if(event->button() == Qt::LeftButton)
	{
		startedWithRightClick = false;
	} 	
	return(QComboBox::mousePressEvent(&mEvt));

};

void QComboBox_ext::newSelection(int sel)
{
	jvxBitField bits;
	jvx_bitSet(bits, sel);
	theParent->newSelection_combo(bits, startedWithRightClick, startedForMode);
};


QSlider_ext::QSlider_ext(realtimeViewerOneProperty* parent): QSlider(Qt::Horizontal, parent)
{
  theParent = parent;
  connect(this, SIGNAL(sliderReleased()), this, SLOT(newSliderPosition()));
  connect(this, SIGNAL(sliderPressed()), this, SLOT(sliderHasBeenPressed()));
};


void QSlider_ext::sliderHasBeenPressed()
{
  theParent->references.theRef->_update_item_in_group_in_section(
                                                                 theParent->references.sectionId, theParent->references.groupId,
                                                                 theParent->references.itemId, &oldUpdateState);

  // Deactivate update for this field...
  theParent->references.theRef->_set_update_item_in_group_in_section(
                                                                     theParent->references.sectionId, theParent->references.groupId,
                                                                     theParent->references.itemId, false);
  theParent->updateWindow_update();
};

void QSlider_ext::newSliderPosition()
{
  int val = this->value();

  theParent->references.theRef->_set_update_item_in_group_in_section(
                                                                     theParent->references.sectionId, theParent->references.groupId,
                                                                     theParent->references.itemId, oldUpdateState);
  theParent->updateWindow_update();
  theParent->newPosition_slider(val);

};


QLineEdit_ext::QLineEdit_ext(realtimeViewerOneProperty* parent): QLineEdit(parent)
{
  theParent = parent;
  //connect(this, SIGNAL(returnPressed()), this, SLOT(newText()));
  connect(this, SIGNAL(editingFinished()), this, SLOT(newText()));
};

void QLineEdit_ext::focusInEvent(QFocusEvent* e)
{
  if (e->reason() == Qt::MouseFocusReason)
    {
      theParent->references.theRef->_update_item_in_group_in_section(
                                                                     theParent->references.sectionId, theParent->references.groupId,
                                                                     theParent->references.itemId, &oldUpdateState);

      // Deactivate update for this field...
      theParent->references.theRef->_set_update_item_in_group_in_section(
                                                                         theParent->references.sectionId, theParent->references.groupId,
                                                                         theParent->references.itemId, false);

      theParent->updateWindow_update();
    }

  // You might also call the parent method.
  QLineEdit::focusInEvent(e);
};

void QLineEdit_ext::focusOutEvent(QFocusEvent* e)
{
  if (e->reason() == Qt::MouseFocusReason)
	    {
			theParent->references.theRef->_set_update_item_in_group_in_section(
				theParent->references.sectionId, theParent->references.groupId,
				theParent->references.itemId, oldUpdateState);
			theParent->updateWindow_update();
		}

  // You might also call the parent method.
  QLineEdit::focusOutEvent(e);
};

void QLineEdit_ext::newText()
{
	jvxBool doReset = false;
	QString txt = this->text();

	theParent->newText_lineEdit(txt, doReset);
	if (doReset)
	{
		this->setText("");
	}
};


QCheckBox_ext::QCheckBox_ext(realtimeViewerOneProperty* parent) : QCheckBox(parent)
{
	theParent = parent;
	//connect(this, SIGNAL(returnPressed()), this, SLOT(newText()));
	connect(this, SIGNAL(clicked(bool)), this, SLOT(wasClicked(bool)));
};

void QCheckBox_ext::focusInEvent(QFocusEvent* e)
{
	if (e->reason() == Qt::MouseFocusReason)
	{
		theParent->references.theRef->_update_item_in_group_in_section(
			theParent->references.sectionId, theParent->references.groupId,
			theParent->references.itemId, &oldUpdateState);

		// Deactivate update for this field...
		theParent->references.theRef->_set_update_item_in_group_in_section(
			theParent->references.sectionId, theParent->references.groupId,
			theParent->references.itemId, false);

		theParent->updateWindow_update();
	}

	// You might also call the parent method.
	QCheckBox::focusInEvent(e);
};

void QCheckBox_ext::focusOutEvent(QFocusEvent* e)
{
	if (e->reason() == Qt::MouseFocusReason)
	{
		theParent->references.theRef->_set_update_item_in_group_in_section(
			theParent->references.sectionId, theParent->references.groupId,
			theParent->references.itemId, oldUpdateState);
		theParent->updateWindow_update();
	}

	// You might also call the parent method.
	QCheckBox::focusOutEvent(e);
};

void QCheckBox_ext::wasClicked(bool tog)
{
	theParent->toggled_checkBox(tog);

};


#endif
