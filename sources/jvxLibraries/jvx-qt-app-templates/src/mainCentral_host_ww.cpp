#include "mainCentral_host_ww.h"


mainCentral_host_ww::mainCentral_host_ww(QWidget* parent): mainCentral_host(parent)
{
	output_failed_setget = true;
}

// #################################################################
void
mainCentral_host_ww::init_submodule(IjvxHost* theHost)
{
	mainCentral_host::init_submodule(theHost);
	output_failed_setget = true;
}

jvxErrorType
mainCentral_host_ww::reportPropertySet(const char* tag, const char* descror, jvxSize groupid, jvxErrorType res)
{
	if (res != JVX_NO_ERROR)
	{
		if (output_failed_setget)
		{
			std::cout << "<set_property> failed: " << "<" << tag << ":" << descror << ">." << std::endl;
		}
	}

	return(JVX_NO_ERROR);
}

jvxErrorType
mainCentral_host_ww::reportAllPropertiesAssociateComplete(const char* tag)
{
	return JVX_NO_ERROR;
}

jvxErrorType
mainCentral_host_ww::reportPropertySpecific(jvxSaWidgetWrapperspecificReport, jvxHandle *)
{
	return JVX_NO_ERROR;
}

jvxErrorType
mainCentral_host_ww::reportPropertyGet(const char *tag, const char *descror, jvxHandle *, jvxSize, jvxSize, jvxDataFormat, jvxErrorType res)
{
	if (res != JVX_NO_ERROR)
	{
		if (output_failed_setget)
		{
			std::cout << __FILE__ << "::" << __LINE__ << ": <get_property> failed: " << "<" << tag << ":" << descror << ">, reason: <" << jvxErrorType_txt(res) << ">." << std::endl;
		}
	}
	return JVX_NO_ERROR;
}
