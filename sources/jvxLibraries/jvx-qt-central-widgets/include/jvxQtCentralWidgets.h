#ifndef __JVXQTCENTRALWIDGETS_EXPORTS_H__
#define __JVXQTCENTRALWIDGETS_EXPORTS_H__

#include "IjvxQtSpecificWidget.h"

JVX_INTERFACE IjvxQtPropertyTreeWidget : public IjvxQtSpecificHWidget
{
public:
	virtual ~IjvxQtPropertyTreeWidget() {};
	virtual void fwd_command_request(const CjvxReportCommandRequest & req) = 0;
};

// ===============================================================================

// Mode flags. If we set this flag, all "set" operations will be reported to the
// main widget wrapper from where other widgets may be updated
#define JVX_LOCAL_PROPERTY_REPORT_SUPERSEDE_SHIFT 0

typedef struct
{
	jvxHandle* widget_wrapper_ptr;
	jvxHandle* report_ptr;
	const char* tag_name_ptr;
} IjvxQtPropertyTreeWidget_connect;

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtPropertyTreeWidget, jvxQtPropertyTreeWidget);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtPropertyTreeWidget, jvxQtPropertyTreeWidget);

#endif
