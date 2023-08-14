#ifndef __IJVXQTSPECIFICWIDGET_H__
#define __IJVXQTSPECIFICWIDGET_H__

#include "interfaces/properties_hosts/IjvxAccessProperties.h"

typedef enum
{
	JVX_QTWIDGET_SPECIFIC_REPORT_NONE = 0,
	JVX_QTWIDGET_SPECIFIC_REPORT_UDPATE_WINDOW,
	JVX_QTWIDGET_SPECIFIC_REPORT_USER_OFFSET = 256
} jvxQtSpecificWidget_reportEnum;

enum class jvxQtInterfaceType
{
	JVX_QT_SUB_INTERFACE_CONFIG,
	JVX_QT_SUB_INTERFACE_WWRAPPER
};

// ====================================================================================================
JVX_INTERFACE IjvxQtSpecificWidget_report
{
public:
	virtual ~IjvxQtSpecificWidget_report() {};

	virtual void report_widget_closed(QWidget* theClosedWidget) = 0;
	virtual void report_widget_specific(jvxSize id, jvxHandle* spec) = 0;
};

// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================

// ====================================================================================================
JVX_INTERFACE IjvxQtSpecificWidget_base
{
public:
	virtual ~IjvxQtSpecificWidget_base() {};
	virtual void getMyQWidget(QWidget** retWidget, jvxSize id = 0) = 0;
	virtual void terminate() = 0;
	virtual void activate() = 0;
	virtual void deactivate() = 0;
	virtual void processing_started() = 0;
	virtual void processing_stopped() = 0;
	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT) = 0;
	virtual void update_window_periodic() = 0;	
};
// ====================================================================================================
// ====================================================================================================

// ====================================================================================================
// Qt specific window if initialized with factory host reference
JVX_INTERFACE IjvxQtSpecificWidget_f_base: public IjvxQtSpecificWidget_base
{
public:
	virtual ~IjvxQtSpecificWidget_f_base() {};
	virtual void init(IjvxFactoryHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL) = 0;
};
// ====================================================================================================
// ====================================================================================================

// ====================================================================================================
// Qt specific window if initialized with host reference
JVX_INTERFACE IjvxQtSpecificWidget_h_base : public IjvxQtSpecificWidget_base
{
public:
	virtual ~IjvxQtSpecificWidget_h_base() {};
	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL) = 0;
};
// ====================================================================================================
// ====================================================================================================


// ====================================================================================================
// Mainly used qt widget in case host
JVX_INTERFACE IjvxQtSpecificHWidget: public IjvxQtSpecificWidget_h_base
{
public:

	virtual ~IjvxQtSpecificHWidget(){};

	//virtual void setPropertyReference(jvx_propertyReferenceTriple* theNodeTriple, std::string prefixArg = "") = 0;
	virtual jvxErrorType addPropertyReference(IjvxAccessProperties* propRef, const std::string& prefixArg = "", const std::string& identifierArg = "") = 0;
	virtual jvxErrorType removePropertyReference(IjvxAccessProperties* propRef = NULL) = 0;

	virtual jvxErrorType request_sub_interface(jvxQtInterfaceType, jvxHandle**) = 0;
	virtual jvxErrorType return_sub_interface(jvxQtInterfaceType, jvxHandle*) = 0;
};
// ====================================================================================================
// ====================================================================================================

/*
JVX_INTERFACE IjvxQtSpecificHWidget_config : public IjvxQtSpecificHWidget
{
public:

	virtual ~IjvxQtSpecificHWidget_config() {};

	virtual jvxErrorType request_configuration_ext_report(IjvxConfigurationExtender_report** theConfigExtenderReport) = 0;
	virtual jvxErrorType return_configuration_ext_report(IjvxConfigurationExtender_report* theConfigExtenderReport) = 0;

};
*/

// ====================================================================================================
// interfaces to forward the callbacks from the widget wrapper
JVX_INTERFACE IjvxQtSaWidgetWrapper_report;
JVX_INTERFACE IjvxQtSpecificHWidget_wwrapper_si 
{
public:
	virtual ~IjvxQtSpecificHWidget_wwrapper_si() {};
	virtual jvxErrorType setWidgetWrapperReportReference(IjvxQtSaWidgetWrapper_report* fwd = nullptr) = 0;
};
// ====================================================================================================
// ====================================================================================================

// ====================================================================================================
// Sub interface for configuration
JVX_INTERFACE IjvxQtSpecificHWidget_config_si 
{
public:
	virtual ~IjvxQtSpecificHWidget_config_si() {};
	virtual jvxErrorType registerConfigExtensions(IjvxConfigurationExtender* cfgExt) = 0;
	virtual jvxErrorType unregisterConfigExtensions(IjvxConfigurationExtender* cfgExt) = 0;
};
// ====================================================================================================
// ====================================================================================================

// ====================================================================================================
// Special case for qt sub widget with implementation as mainWindow widget
JVX_INTERFACE IjvxQtSpecificWidget_mainWindow : public IjvxQtSpecificHWidget
{
public:
	virtual void init_extend_specific_widgets(IjvxMainWindowControl* ctrl, IjvxQtSpecificWidget_report* rep_spec_widgets) = 0;
	virtual void terminate_extend_specific_widgets() = 0;
};
// ====================================================================================================
// ====================================================================================================

// ====================================================================================================
// Some macros to generate and deallocate qt widgets
#define JVX_QT_WIDGET_INIT_DECLARE(retType, fpostfix) \
	jvxErrorType init_ ## fpostfix(retType** returnWidget, QWidget* parent)

#define JVX_QT_WIDGET_TERMINATE_DECLARE(retType, fpostfix) \
	jvxErrorType terminate_ ## fpostfix(retType* returnWidget)

#define JVX_QT_WIDGET_INIT_DEFINE(retType, fpostfix, cname) \
	jvxErrorType init_ ## fpostfix(retType** returnWidget, QWidget* parent) \
	{ \
		if (returnWidget) \
		{ \
			*returnWidget = new cname(parent); \
			return(JVX_NO_ERROR); \
		} \
		return(JVX_ERROR_INVALID_ARGUMENT); \
	}
#define JVX_QT_WIDGET_TERMINATE_DEFINE(retType, fpostfix) \
	jvxErrorType terminate_ ## fpostfix(retType* returnWidget) \
	{ \
		if (returnWidget) \
		{ \
			delete(returnWidget); \
			return(JVX_NO_ERROR); \
		} \
		return(JVX_ERROR_INVALID_ARGUMENT); \
	}
// ====================================================================================================
// ====================================================================================================


#endif
