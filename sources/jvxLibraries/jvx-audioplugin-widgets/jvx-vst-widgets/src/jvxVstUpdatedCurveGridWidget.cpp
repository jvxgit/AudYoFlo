#include "jvx-vst-widgets.h"

namespace VSTGUI {

	jvxVstUpdatedCurveGridWidget::jvxVstUpdatedCurveGridWidget(
		const CRect& size, IjvxPluginWidgetClose* rep_close,
		IjvxPluginWidgetUpdate* bwdRef, jvxInt32 tag) :
		jvxVstCurveGridWidget(size, rep_close), control_ref(bwdRef), tag_report(tag)
	{
	}

	jvxErrorType
		jvxVstUpdatedCurveGridWidget::restart_update(jvxSize numSteps)
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if (stateUpdate == jvxUpdateState::JVX_VST_UPDATE_NONE)
		{
			res = JVX_NO_ERROR;
			numStepsUpdate = numSteps;
			cntUpdate = 0;
			stateUpdate = jvxUpdateState::JVX_VST_UPDATE_STARTED;

			if (control_ref)
			{
				res = control_ref->start_update(tag_report, numStepsUpdate);
			}

			if (res != JVX_NO_ERROR)
			{
				goto leave_error;
			}

			// The trigger function may be reached recursively
			if (cntUpdate < numStepsUpdate)
			{
				jvxSize curStep = cntUpdate;
				cntUpdate++;
				if (control_ref)
				{
					res = control_ref->trigger_step_update(tag_report, curStep);
				}
			}

			// Extra check required due to recursive call option
			if (cntUpdate < numStepsUpdate)
			{
				res = JVX_ERROR_POSTPONE;
			}

			if (!
				(res == JVX_NO_ERROR) || (res == JVX_ERROR_POSTPONE))
			{
				goto leave_error;
			}
		}

		return res;

	leave_error:

		stateUpdate = jvxUpdateState::JVX_VST_UPDATE_NONE;
		cntUpdate = 0;
		return res;
	}

	/*
	stateUpdate = jvxUpdateState::JVX_VST_UPDATE_UPDATE_PLOT;
	if (control_ref)
	{
		res = control_ref->finalize_update(tag_report);

		if (res == JVX_NO_ERROR)
		{
			// Set back to initial
			stateUpdate = jvxUpdateState::JVX_VST_UPDATE_NONE;
			cntUpdate = 0;
		}
	}
	*/
	jvxErrorType
		jvxVstUpdatedCurveGridWidget::trigger_next_update()
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if (stateUpdate == jvxUpdateState::JVX_VST_UPDATE_STARTED)
		{
			res = JVX_NO_ERROR;
			if (cntUpdate < numStepsUpdate)
			{
				jvxSize curStep = cntUpdate;
				cntUpdate++;
				if (control_ref)
				{
					res = control_ref->trigger_step_update(tag_report, curStep);
				}
			}

			if (cntUpdate < numStepsUpdate)
			{
				res = JVX_ERROR_POSTPONE;
			}

			if (!
				(res == JVX_NO_ERROR) || (res == JVX_ERROR_POSTPONE))
			{
				goto leave_error;
			}
			return res;
		}
	leave_error:

		stateUpdate = jvxUpdateState::JVX_VST_UPDATE_NONE;
		cntUpdate = 0;
		return res;
	};

	jvxErrorType
	jvxVstUpdatedCurveGridWidget::trigger_finalize()
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if (stateUpdate == jvxUpdateState::JVX_VST_UPDATE_STARTED)
		{
			stateUpdate = jvxUpdateState::JVX_VST_UPDATE_FINALIZE;
			if (control_ref)
			{
				res = control_ref->finalize_update(tag_report);
			}

		}
		return res;
	}

	void 
	jvxVstUpdatedCurveGridWidget::draw_inner(CDrawContext* context)
	{
		jvxVstCurveGridWidget::draw_inner(context);
		if (stateUpdate == jvxUpdateState::JVX_VST_UPDATE_FINALIZE)
		{
			// Set back to initial
			stateUpdate = jvxUpdateState::JVX_VST_UPDATE_NONE;
			cntUpdate = 0;
		}
	}
}
