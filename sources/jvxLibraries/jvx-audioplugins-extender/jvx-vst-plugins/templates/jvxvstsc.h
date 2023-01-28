#ifndef __JVXVSTSC_H__
#define __JVXVSTSC_H__

#include "jvx.h"

#include "vstgui/plugin-bindings/vst3editor.h"
#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/uidescription/delegationcontroller.h"

namespace Steinberg {
namespace Vst {

JVX_INTERFACE jvxvstsc_report
{
public:
	~jvxvstsc_report() {};
	virtual void report_view_type_started(VSTGUI::CView* theView, jvxSize idTag) = 0;
	virtual void report_view_type_stopped(VSTGUI::CView* theView, jvxSize idTag) = 0;
	virtual void report_view_type_lost_focus(VSTGUI::CView* theView, jvxSize idTag) = 0;
	virtual void report_view_type_edit_end(VSTGUI::CControl* theCtrl, jvxSize idTag) = 0;
};

template <typename T>
class jvxvstsc : public VSTGUI::DelegationController, public VSTGUI::ViewListenerAdapter
{
public:

	jvxvstsc(IController* controller, jvxvstsc_report* lc, jvxSize tag): 
		VSTGUI::DelegationController(controller)
	{
		object_ptr = nullptr;
		view_ptr = nullptr;
		ctrl_ptr = nullptr;
		object_tag = tag;
		controllerRef = lc;
	}

	~jvxvstsc() override
	{
		if (view_ptr)
			viewWillDelete(view_ptr);
	};

	T* get_object_ptr()
	{
		return object_ptr;
	};

	//--- from IControlListener ----------------------
	void valueChanged(VSTGUI::CControl* pControl) override
	{
		VSTGUI::DelegationController::valueChanged(pControl);
		// I prefer to use the control end edit callback but this function is abstract otherwise
	};

	void controlEndEdit(VSTGUI::CControl* pControl) override
	{
		VSTGUI::DelegationController::controlEndEdit(pControl);
		if (pControl == ctrl_ptr)
		{
			if (controllerRef)
			{
				controllerRef->report_view_type_edit_end(ctrl_ptr, object_tag);
			}
		}
	};

		//--- from IControlListener ----------------------
		//--- is called when a view is created -----
	VSTGUI::CView* verifyView(VSTGUI::CView* view, const VSTGUI::UIAttributes& attributes,
		const VSTGUI::IUIDescription* description) override
	{
		VSTGUI::DelegationController::verifyView(view, attributes, description);

		// CAnimKnob* co = dynamic_cast<CAnimKnob*> (view);
		if (view_ptr == nullptr)
		{
			/* Only one pointer reported */

			T* t = dynamic_cast<T*> (view);
			if (t)
			{
				object_ptr = t;
				view_ptr = view;
				ctrl_ptr = dynamic_cast<VSTGUI::CControl*> (view);

				view_ptr->registerViewListener(this);
				if (ctrl_ptr)
				{
					// The listener may have been registered before - I do not know exactly why actually.
					// I tracked it back to a "registry" which somehow creates the link
					ctrl_registered = false;
					VSTGUI::IControlListener* list = ctrl_ptr->getListener();
					if (list != static_cast<VSTGUI::IControlListener*>(this))
					{
						ctrl_registered = true;
						ctrl_ptr->registerControlListener(static_cast<VSTGUI::IControlListener*>(this));
					}
				}

				if (controllerRef)
				{
					controllerRef->report_view_type_started(view_ptr, object_tag);
				}
			}
		}
		return view;
	};

	//--- from IViewListenerAdapter ----------------------
	//--- is called when a view will be deleted: the editor is closed -----
	void viewWillDelete(VSTGUI::CView* view) override
	{
		if (view == view_ptr)
		{
			if (controllerRef)
			{
				controllerRef->report_view_type_stopped(view_ptr, object_tag);
			}
			if (ctrl_ptr)
			{
				if (ctrl_registered)
				{
					ctrl_ptr->unregisterControlListener(this);
				}
			}
			view_ptr->unregisterViewListener(this);
			view_ptr = nullptr;
			object_ptr = nullptr;
		}
	}

	//--- is called when the view is loosing the focus -----------------
	void viewLostFocus(VSTGUI::CView* view) override
	{
		if (view == view_ptr)
		{
			if (controllerRef)
			{
				controllerRef->report_view_type_lost_focus(view_ptr, object_tag);
			}
		}
	}

private:
		T* object_ptr = nullptr;
		VSTGUI::CView* view_ptr = nullptr;
		VSTGUI::CControl* ctrl_ptr = nullptr;
		jvxSize object_tag = JVX_SIZE_UNSELECTED;
		jvxvstsc_report* controllerRef = nullptr;
		jvxBool ctrl_registered = false;
	};
	
//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

#endif
