#ifndef __CJVXQTPROJECT_H__
#define __CJVXQTPROJECT_H__

#include "jvx.h"
#include "ui_CjvxQtProject.h"
#include "jvxQtProject.h"


class CjvxQtProject : public QWidget, public Ui::widget_project, public IjvxQtSpecificHWidget
{

private:

	IjvxHost* theHost = nullptr;
	IjvxAccessProperties* theProps = nullptr;

	Q_OBJECT

public:

	CjvxQtProject(QWidget* parent);
	~CjvxQtHoa();

	// Interface <IjvxQtSpecificWidget_h_base>
	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL) override;

	// --> Interface <IjvxQtSpecificHWidget>
	virtual jvxErrorType addPropertyReference(IjvxAccessProperties* propRef, const std::string& prefixArg = "", const std::string& identifierArg = "") override;
	virtual jvxErrorType removePropertyReference(IjvxAccessProperties* propRef = NULL) override;
	
	// --> Interface <IjvxQtSpecificWidget_base>
	virtual void getMyQWidget(QWidget** retWidget, jvxSize id = 0) override;
	virtual void terminate() override;
	virtual void activate() override;
	virtual void deactivate() override;
	virtual void processing_started() override;
	virtual void processing_stopped() override;
	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT) override;
	virtual void update_window_periodic() override;

private:
	
signals:

public slots :

};

#endif