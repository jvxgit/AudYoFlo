#ifndef __SHOWINFORMATION_H__
#define __SHOWINFORMATION_H__

#include "jvx.h"
#include "ui_showInformation.h"

class showInformation: public QDialog, public Ui::Dialog_showInfo
{
	struct
	{
		IjvxObject* theObj;
	} runtime;

public:
	
	showInformation(IjvxObject* theObj);

	void init();
	void updateWindow();

};

#endif