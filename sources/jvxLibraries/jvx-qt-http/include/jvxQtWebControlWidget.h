#ifndef __IJVXQTWEBCONTROLWIDGET_H__
#define __IJVXQTWEBCONTROLWIDGET_H__

#include "IjvxQtSpecificWidget.h"

JVX_INTERFACE IjvxHttpApi;

JVX_INTERFACE IjvxQtWebControlWidget_report
{
public:
	virtual ~IjvxQtWebControlWidget_report() {};
	virtual jvxErrorType report_error() = 0;
};

JVX_INTERFACE IjvxQtWebControlWidget: public IjvxQtSpecificWidget_f_base
{
public:
	virtual ~IjvxQtWebControlWidget() {};

	virtual jvxErrorType request_http_api(IjvxHttpApi** theHttpRef) = 0;
	virtual jvxErrorType return_http_api(IjvxHttpApi* theHttpRef) = 0;

	virtual jvxErrorType request_configuration_ext_report(IjvxConfigurationExtender_report** theConfigExtenderReport) = 0;
	virtual jvxErrorType return_configuration_ext_report(IjvxConfigurationExtender_report* theConfigExtenderReport) = 0;

	virtual jvxErrorType set_report(IjvxQtWebControlWidget_report* ptr) = 0;

};

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtWebControlWidget, jvxQtWebControl);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtWebControlWidget, jvxQtWebControl);

#endif
