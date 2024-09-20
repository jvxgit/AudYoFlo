#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"
#include <qpixmap.h>

#include "mainCentral_host_ww.h"
#include "jvxQtSaGlWidget.h"

#include <QResizeEvent>

// header
class CjvxAspectRatioWidget : public QWidget
{
private:
	QBoxLayout *layout;
	float arWidth; // aspect ratio width
	float arHeight; // aspect ratio height

public:
	CjvxAspectRatioWidget(QWidget *parent = 0) : QWidget(parent), arWidth(0), arHeight(0)
	{
	}

	void init(QWidget *widget, float width, float height)
	{
		arWidth = width;
		arHeight = height;

		layout = new QBoxLayout(QBoxLayout::LeftToRight, this);

		// add spacer, then your widget, then spacer
		layout->addItem(new QSpacerItem(0, 0));
		layout->addWidget(widget);
		layout->addItem(new QSpacerItem(0, 0));
	};

	void resizeEvent(QResizeEvent *event)
	{
		float thisAspectRatio = (float)event->size().width() / event->size().height();
		int widgetStretch, outerStretch;

		if (thisAspectRatio > (arWidth / arHeight)) // too wide
		{
			layout->setDirection(QBoxLayout::LeftToRight);
			widgetStretch = height() * (arWidth / arHeight); // i.e., my width
			outerStretch = (width() - widgetStretch) / 2 + 0.5;
		}
		else // too tall
		{
			layout->setDirection(QBoxLayout::TopToBottom);
			widgetStretch = width() * (arHeight / arWidth); // i.e., my height
			outerStretch = (height() - widgetStretch) / 2 + 0.5;
		}

		layout->setStretch(0, outerStretch);
		layout->setStretch(1, widgetStretch);
		layout->setStretch(2, outerStretch);
	}
};


#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class myCentralWidget: public mainCentral_host_ww, public Ui::Form_myMainWidget
{
private:
	CjvxMyGlWidget *openGLWidget;
	IjvxAccessProperties* thePropRef_aun;
	IjvxAccessProperties* thePropRef_vin;

	CjvxMyGlWidget_config glCfg;

	Q_OBJECT

public:

	myCentralWidget(QWidget* parent): mainCentral_host_ww(parent)
	{
		openGLWidget = NULL;
		thePropRef_aun = NULL;
		thePropRef_vin = NULL;
		glCfg.prop_segx = "/system/segmentsizex";
		glCfg.prop_segy = "/system/segmentsizey";
		glCfg.prop_frmt = "/system/dataformat";
		glCfg.prop_sfrmt = "/system/datasubformat";
		glCfg.prop_omode = "/expose_visual_if/operation_mode";
		glCfg.prop_nbufs = "/expose_visual_if/number_buffers";
		glCfg.prop_rtrgt = "/expose_visual_if/rendering_target";
	};
	~myCentralWidget(){};

	virtual void init_submodule(IjvxHost* theHost)override;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps) override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	virtual void inform_sequencer_about_to_start()override;

	virtual void inform_sequencer_stopped()override;


	virtual void inform_update_window(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT)override;

	virtual void reportCloseEventTriggered();
	virtual void reportUpdateWindow();

	void preferredSize(jvxInt32& height, jvxInt32& width);


};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
