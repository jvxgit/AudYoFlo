#ifndef __IJVXQTWIDGETFRAMEDIALOG_H__
#define __IJVXQTWIDGETFRAMEDIALOG_H__

JVX_INTERFACE IjvxQtSpecificWidget_report;

JVX_INTERFACE IjvxQtWidgetFrameDialog
{
public:
	virtual ~IjvxQtWidgetFrameDialog() {};

	virtual void init(IjvxQtSpecificWidget_report* bwd) = 0;
	virtual void reset_bwd_reference(IjvxQtSpecificWidget_report* bwdRep) = 0;
	virtual void attachWidget(QWidget* theWidget) = 0;
	virtual void detachWidget(QWidget* theWidget) = 0;
	virtual void getMyQDialog(QDialog** retWidget, jvxSize id = 0) = 0;
};

#endif