#ifndef __JVXVSTUPDATEDCURVEGRIDWIDGET_H__
#define __JVXVSTUPDATEDCURVEGRIDWIDGET_H__

#include "jvx-vst-widgets.h"


namespace VSTGUI {

	class jvxVstUpdatedCurveGridWidget : public jvxVstCurveGridWidget
	{
		enum class jvxUpdateState
		{
			JVX_VST_UPDATE_NONE,
			JVX_VST_UPDATE_STARTED,
			JVX_VST_UPDATE_FINALIZE
		};

		jvxSize numStepsUpdate = 0;
		jvxSize cntUpdate = 0;
		jvxUpdateState stateUpdate = jvxUpdateState::JVX_VST_UPDATE_NONE;
		jvxInt32 tag_report = -1;

	protected:
		
		IjvxPluginWidgetUpdate* control_ref = nullptr;

	public:
		jvxVstUpdatedCurveGridWidget(const CRect& size, IjvxPluginWidgetClose* rep_close, IjvxPluginWidgetUpdate* bwdRef, jvxInt32 tag);

		jvxErrorType restart_update(jvxSize numSteps);
		jvxErrorType trigger_next_update();
		jvxErrorType trigger_finalize();

		void draw_inner(CDrawContext* context)override;

	protected:
	};
}

#endif

