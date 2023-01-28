#ifndef __JVXPLUGINWIDGET_H__
#define __JVXPLUGINWIDGET_H__

#include "jvx-vst-widgets.h"

class jvxPluginWidget
{
protected:
	IjvxPluginWidgetClose* report_close = nullptr;

public:
	jvxPluginWidget(IjvxPluginWidgetClose* rep_close);
	~jvxPluginWidget();
};

#endif
