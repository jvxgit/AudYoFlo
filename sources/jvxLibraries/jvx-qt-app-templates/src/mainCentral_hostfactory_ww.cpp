#include "mainCentral_hostfactory_ww.h"

#include <QScrollArea>
#include <QMessageBox>

#include "jvx.h"

#include <iostream>

mainCentral_hostfactory_ww::mainCentral_hostfactory_ww(QWidget* parent): 
	mainCentral_hostfactory(parent)
{
	output_failed_setget = true;
}


jvxErrorType 
mainCentral_hostfactory_ww::reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res)
{
	if (res != JVX_NO_ERROR)
	{
		if (output_failed_setget)
		{
			std::cout << "<set_property failed: " << "<" << tag << ":" << propDescrptior << ">." << std::endl;
		}
	}

	return JVX_NO_ERROR;
}

jvxErrorType 
mainCentral_hostfactory_ww::reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, jvxErrorType res)
{
	if (res != JVX_NO_ERROR)
	{
		if (output_failed_setget)
		{
			std::cout << "<get_property failed: " << "<" << tag << ":" << propDescrptior << ">." << std::endl;
		}
	}

	return JVX_NO_ERROR;
}

jvxErrorType 
mainCentral_hostfactory_ww::reportAllPropertiesAssociateComplete(const char* tag)
{
	std::cout << "Association of properties for " << tag << "complete." << std::endl;
	return JVX_NO_ERROR;
}

jvxErrorType
mainCentral_hostfactory_ww::reportPropertySpecific(jvxSaWidgetWrapperspecificReport what, jvxHandle* props)
{
	std::cout << __FUNCTION__ << "Reported specific event." << std::endl;
	return JVX_NO_ERROR;
}
