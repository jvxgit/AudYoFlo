#ifndef __MAINCENTRAL_HOSTFACTORY_WW_H__
#define __MAINCENTRAL_HOSTFACTORY_WW_H__

#include <QWidget>
#include <QComboBox>
#include "CjvxQtSaWidgetWrapper.h"
#include "mainCentral_hostfactory.h"

class mainCentral_hostfactory_ww : public mainCentral_hostfactory, public IjvxQtSaWidgetWrapper_report
{

protected:
	CjvxQtSaWidgetWrapper myWidgetWrapper;
	jvxBool output_failed_setget;

private:

	Q_OBJECT

public:

	mainCentral_hostfactory_ww(QWidget* parent);

	virtual jvxErrorType reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res) override;
	virtual jvxErrorType reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, jvxErrorType res) override;
	virtual jvxErrorType reportAllPropertiesAssociateComplete(const char* tag)override;
	virtual jvxErrorType reportPropertySpecific(jvxSaWidgetWrapperspecificReport, jvxHandle* props)override;

};

#endif
