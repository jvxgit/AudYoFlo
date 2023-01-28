#ifndef __JVXVSTC_H__
#define __JVXVSTC_H__

#include "vstgui/plugin-bindings/vst3editor.h"
#include "public.sdk/source/vst/vsteditcontroller.h"

#include <vector>

// ===============================================
// jvxrt includes
#include "CjvxPluginProcessor.h"
#include "CjvxVstPluginController.h"
#include "jvxvstsc.h"

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// loudnessController
//------------------------------------------------------------------------
class jvxvstc : public EditControllerEx1, public IMidiMapping, 
	public VSTGUI::VST3EditorDelegate, public IjvxPluginParameterAllocator, 
	public CjvxPluginVstController
{
public:

	using UTF8StringPtr = VSTGUI::UTF8StringPtr;
	using IUIDescription = VSTGUI::IUIDescription;
	using IController = VSTGUI::IController;
	using VST3Editor = VSTGUI::VST3Editor;
	
	//--- ---------------------------------------------------------------------
	// create function required for plug-in factory,
	// it will be called to create new instances of this controller
	//--- ---------------------------------------------------------------------
	

	//---from IPluginBase--------
	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API terminate () SMTG_OVERRIDE;

	//---from EditController-----
	tresult PLUGIN_API setComponentState (IBStream* state) SMTG_OVERRIDE;
	IPlugView* PLUGIN_API createView (const char* name) SMTG_OVERRIDE;
	tresult PLUGIN_API setState (IBStream* state) SMTG_OVERRIDE;
	tresult PLUGIN_API getState (IBStream* state) SMTG_OVERRIDE;
	tresult PLUGIN_API setParamNormalized (ParamID tag, ParamValue value) SMTG_OVERRIDE;
	tresult PLUGIN_API getParamStringByValue (ParamID tag, ParamValue valueNormalized,
	                                          String128 string) SMTG_OVERRIDE;
	tresult PLUGIN_API getParamValueByString (ParamID tag, TChar* string,
	                                          ParamValue& valueNormalized) SMTG_OVERRIDE;
	 
	//---from ComponentBase-----
	tresult receiveText (const char* text) SMTG_OVERRIDE;
	tresult notify(IMessage* message)SMTG_OVERRIDE;
	
	//---from IMidiMapping-----------------
	tresult PLUGIN_API getMidiControllerAssignment (int32 busIndex, int16 channel,
	                                                CtrlNumber midiControllerNumber,
	                                                ParamID& tag) SMTG_OVERRIDE;

	//---from VST3EditorDelegate-----------
	VSTGUI::CView* createCustomView(VSTGUI::UTF8StringPtr name,
		const VSTGUI::UIAttributes& attributes,
		const VSTGUI::IUIDescription* description,
		VSTGUI::VST3Editor* editor) SMTG_OVERRIDE;

	//---from Programme logic-----------
	Steinberg::int32 PLUGIN_API getProgramListCount() SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getProgramListInfo(Steinberg::int32 listIndex, Steinberg::Vst::ProgramListInfo& info /*out*/) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getProgramName(Steinberg::Vst::ProgramListID listId, Steinberg::int32 programIndex, Steinberg::Vst::String128 name /*out*/) SMTG_OVERRIDE;


	DELEGATE_REFCOUNT (EditController)
	tresult PLUGIN_API queryInterface (const char* iid, void** obj) SMTG_OVERRIDE;

	//---Internal functions-------
	/*
	void addUIMessageController (UIMessageController* controller);
	void removeUIMessageController (UIMessageController* controller);
	*/

	void setDefaultMessageText (String128 text);
	TChar* getDefaultMessageText ();
//------------------------------------------------------------------------

	virtual tresult beginEdit(ParamID tag) SMTG_OVERRIDE;	///< to be called before a serie of performEdit
	virtual tresult performEdit(ParamID tag, ParamValue valueNormalized) SMTG_OVERRIDE; ///< will inform the host about the value change
	virtual tresult endEdit(ParamID tag) SMTG_OVERRIDE;


	virtual jvxErrorType allocate_unit(jvxSize id, jvxSize mappedId, const char* nm) override;

	virtual jvxErrorType allocate_parameter_normalized(const char* nm, jvxInt32 id,
		jvxCBitField flags, jvxData initval, jvxSize unit_id) override;

	virtual jvxErrorType allocate_parameter_norm_so(const char* nm, jvxInt32 id,
		jvxCBitField flags, const char* unit, jvxData offset, jvxData scalefac,
		jvxData initval, jvxSize unit_id, jvxSize num_digits) override;

	virtual jvxErrorType allocate_parameter_norm_strlist(const char* nm, jvxInt32 id,
		jvxApiStringList* lstEntries,
		jvxCBitField flags, jvxData initval, 
		jvxSize unit_id, const char* empty_arg = nullptr)override;

	void check_subcomponent_status(jvxCBitField check_this, jvxCBitField vs_this);

	virtual void report_end_edit(jvxInt32 tag) = 0;
	virtual void parameter_update_full() = 0;
	virtual void create_parameters() = 0;
	virtual void startup() = 0;
	virtual std::string get_ui_description_name() = 0;
	virtual void ui_attached() = 0;
	virtual void ui_detached() = 0; 

private:

	// using UIMessageControllerList = std::vector<UIMessageController*>;
	// UIMessageControllerList uiMessageControllers;
protected:
	jvxInt32 preset_param_id = -1;
	String128 defaultMessageText;
	VSTGUI::VST3Editor* theEditor = nullptr;
	jvxBool ui_has_been_attached = false;
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

#endif

