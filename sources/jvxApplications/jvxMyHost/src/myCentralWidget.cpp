#include "myCentralWidget.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

void
myCentralWidget::init_submodule(IjvxHost* theHost)
{
	this->setupUi(this);
	mainCentral_host::init_submodule(theHost);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
