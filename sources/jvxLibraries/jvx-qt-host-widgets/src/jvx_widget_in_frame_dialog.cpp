#include "jvx_widget_in_frame_dialog.h"
#include <cassert>
#include <iostream>
#include "IjvxQtSpecificWidget.h"
#include "QApplication"
#include <QWidget>
#include "jvx-qt-helpers.h"

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtWidgetFrameDialog, jvxQtWidgetFrameDialog, jvx_widget_in_frame_dialog)
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtWidgetFrameDialog, jvxQtWidgetFrameDialog)

jvx_widget_in_frame_dialog::jvx_widget_in_frame_dialog(QWidget* parent) : QDialog(parent)
{
	bwdReport = NULL;
}

jvx_widget_in_frame_dialog::~jvx_widget_in_frame_dialog()
{
}

void 
jvx_widget_in_frame_dialog::init(IjvxQtSpecificWidget_report* bwdRep)
{
	this->setupUi(this);
	bwdReport = bwdRep;
}

void
jvx_widget_in_frame_dialog::reset_bwd_reference(IjvxQtSpecificWidget_report* bwdRep)
{
	bwdReport = bwdRep;
}

void 
jvx_widget_in_frame_dialog::attachWidget(QWidget* theWidget)
{
	QLayout* layout = this->layout();
	QRect geom = theWidget->geometry();
	layout->addWidget(theWidget);
	this->setLayout(layout);
	this->setGeometry(geom);
	adjustSize();

	// Mode all widgets into the middle position
	QRect desktopRect = jvx_get_screen_geometry_qt();
	move(desktopRect.center() - this->rect().center());
}

void
jvx_widget_in_frame_dialog::detachWidget(QWidget* theWidget)
{
	QLayout* layout = this->layout();
	layout->removeWidget(theWidget);
}

void
jvx_widget_in_frame_dialog::closeEvent(QCloseEvent * event)
{
	QDialog::closeEvent(event);
	if (bwdReport)
	{
		bwdReport->report_widget_closed(static_cast<QWidget*>(this));
	}
}

void 
jvx_widget_in_frame_dialog::getMyQDialog(QDialog** retDialog, jvxSize id)
{
	switch (id)
	{
	case 0:
		*retDialog = static_cast<QDialog*>(this);
		break;
	default:
		*retDialog = NULL;
	}
}
