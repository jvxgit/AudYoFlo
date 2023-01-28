#include "myCentralWidget.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

void
myCentralWidget::init_submodule(IjvxFactoryHost* theHost)
{
	this->setupUi(this);
	mainCentral_hostfactory_ww_http::init_submodule(theHost);
}

void
myCentralWidget::process_websocket_binary(jvxSize userData, jvxByte* payload, jvxPropertyTransferPriority prio)
{
}

jvxErrorType 
myCentralWidget::report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
myCentralWidget::report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr)
{
	return JVX_ERROR_UNSUPPORTED;
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
