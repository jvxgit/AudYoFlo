#ifndef __JVX_WIDGET_IN_FRAME_DIALOG_H__
#define __JVX_WIDGET_IN_FRAME_DIALOG_H__

#include "ui_jvx_widget_in_frame.h"
#include "jvx.h"
#include "IjvxQtWidgetFrameDialog.h"

class jvx_widget_in_frame_dialog: public QDialog, public Ui::WidgetInFrameDialog, public IjvxQtWidgetFrameDialog
{
	IjvxQtSpecificWidget_report* bwdReport;
	Q_OBJECT
protected:
	virtual void closeEvent(QCloseEvent * event)override;
public:
	jvx_widget_in_frame_dialog(QWidget* parent);
	~jvx_widget_in_frame_dialog();
	virtual void init(IjvxQtSpecificWidget_report* bwd) override;
	virtual void reset_bwd_reference(IjvxQtSpecificWidget_report* bwdRep)override;
	virtual void attachWidget(QWidget* theWidget) override;
	virtual void detachWidget(QWidget* theWidget) override;
	virtual void getMyQDialog(QDialog** retWidget, jvxSize id = 0)override;
};

#endif // ifndef __JVX_CONNECTIONS_WIDGET_H__
