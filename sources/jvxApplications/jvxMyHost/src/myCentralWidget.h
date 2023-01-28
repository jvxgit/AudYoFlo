#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"

#include "mainCentral_host.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class myCentralWidget: public mainCentral_host, public Ui::Form_myMainWidget
{
private:

	Q_OBJECT

public:

	myCentralWidget(QWidget* parent): mainCentral_host(parent){};
	~myCentralWidget(){};
	virtual void init_submodule(IjvxHost* theHost)override;

signals:

public slots:
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
