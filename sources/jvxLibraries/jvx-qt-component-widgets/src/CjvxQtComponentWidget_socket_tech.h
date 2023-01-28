#ifndef __CjvxQtComponentWidget_socket_tech_H__
#define __CjvxQtComponentWidget_socket_tech_H__

#include "jvx.h"
#include "jvxQtComponentWidgets.h"
#include <QObject>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include "jvxQtSaNetworkTechnology.h"
#include "CjvxQtSaWidgetWrapper.h"

class CjvxQtComponentWidget_socket_tech: public QObject, public IjvxQtComponentWidget,
	public IjvxQtSaWidgetWrapper_report
{
private:
	IjvxHost* theHostRef;
	jvxQtSaNetworkTechnology* theNetworkW;
	std::string addConfigToken;

	CjvxQtSaWidgetWrapper widgetWrapper;

	Q_OBJECT
public:
	CjvxQtComponentWidget_socket_tech(const char* additionalUiConfigTokens = "");
	~CjvxQtComponentWidget_socket_tech();
	
	void init_submodule(IjvxHost* theHost ) override;
	void terminate_submodule() override;

	virtual void start_show(IjvxAccessProperties* propRef, QFrame* placeHere,
		const char* propPrefix) override;
	virtual void stop_show(QFrame* placeHere) override;

	void updateWindow() override;
	void updateWindow_periodic() override;

	virtual jvxErrorType reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res_in_call) override;
	virtual jvxErrorType reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, jvxErrorType res_in_call)override;
	virtual jvxErrorType reportAllPropertiesAssociateComplete(const char* tag)override;
	virtual jvxErrorType reportPropertySpecific(jvxSaWidgetWrapperspecificReport, jvxHandle *)override;

public slots:

};

#endif