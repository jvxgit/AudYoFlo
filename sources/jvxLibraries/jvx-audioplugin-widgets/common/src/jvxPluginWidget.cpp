#include "jvx-vst-widgets.h"

jvxPluginWidget::jvxPluginWidget(IjvxPluginWidgetClose* rep_close):
	report_close(rep_close)
{
}

jvxPluginWidget::~jvxPluginWidget()
{
	if (report_close)
	{
		report_close->report_close(this);
	}
}
