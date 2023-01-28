#ifndef __IJVXPLUGINWIDGETS_H__
#define __IJVXPLUGINWIDGETS_H__

class jvxPluginWidget;

JVX_INTERFACE IjvxPluginWidgetClose
{
public:
	virtual ~IjvxPluginWidgetClose() {};
	virtual void report_close(jvxPluginWidget* closeme) = 0;
};

JVX_INTERFACE IjvxPluginWidgetUpdate: public IjvxPluginWidgetClose
{
public:
	virtual ~IjvxPluginWidgetUpdate() {};

	virtual jvxErrorType start_update(jvxInt32 tag, jvxSize numSteps) = 0;
	virtual jvxErrorType trigger_step_update(jvxInt32 tag, jvxSize cntUpdate) = 0;
	virtual jvxErrorType finalize_update(jvxInt32 tag) = 0;
};

#endif
