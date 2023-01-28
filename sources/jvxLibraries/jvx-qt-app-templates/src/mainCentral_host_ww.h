#ifndef __MAINCENTRAL_HOST_WW_H__
#define __MAINCENTRAL_HOST_WW_H__

#include "mainCentral_host.h"
#include "CjvxQtSaWidgetWrapper.h"

class mainCentral_host_ww: public mainCentral_host, public IjvxQtSaWidgetWrapper_report
{

protected:

	jvxBool output_failed_setget;
	CjvxQtSaWidgetWrapper myWidgetWrapper;

public:

	mainCentral_host_ww(QWidget* parent);

	void init_submodule(IjvxHost* theHost)override;
	virtual jvxErrorType reportPropertySet(const char* tag, const char* descror, jvxSize groupid, jvxErrorType res) override;
	virtual jvxErrorType reportAllPropertiesAssociateComplete(const char* tag) override;
	virtual jvxErrorType reportPropertyGet(const char *, const char *, jvxHandle *, jvxSize, jvxSize, jvxDataFormat, jvxErrorType res) override;
	jvxErrorType reportPropertySpecific(jvxSaWidgetWrapperspecificReport, jvxHandle *) override;
};

#endif
