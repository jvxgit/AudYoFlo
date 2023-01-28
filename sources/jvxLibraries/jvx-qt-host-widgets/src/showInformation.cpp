#include "showInformation.h"

showInformation::showInformation(IjvxObject* theObj)
{
	runtime.theObj = theObj;
}

void 
showInformation::init()
{
	this->setupUi(static_cast<QDialog*>(this));
	updateWindow();
}

void 
showInformation::updateWindow()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxApiString fldStr;
	if(runtime.theObj)
	{
		runtime.theObj->number_info_tokens(&num);
		for(i = 0; i < num; i++)
		{
			res = runtime.theObj->info_token(i, &fldStr);
			if(res == JVX_NO_ERROR)
			{
				this->listWidget->addItem(fldStr.c_str());
			}
			else
			{
				break;
			}
		}

	}
}
