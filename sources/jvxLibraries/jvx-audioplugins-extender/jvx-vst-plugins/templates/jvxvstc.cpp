#include "jvxvstc.h"

#include "CjvxVstPluginCommon.h"
#include "CjvxVstPluginParameter.h"

#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h"

#include "base/source/fstring.h"
#include "base/source/fstreamer.h"

#include "vstgui/uidescription/delegationcontroller.h"

using namespace VSTGUI;

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// loudnessController Implementation
//------------------------------------------------------------------------
	tresult PLUGIN_API jvxvstc::initialize(FUnknown* context)
	{
		tresult result = EditControllerEx1::initialize(context);
		if (result != kResultOk)
		{
			return result;
		}

		// Assign pointer for all further message handling
		theController = static_cast<EditController*>(this);

		startup();

		return result;
	}

	//------------------------------------------------------------------------
	tresult PLUGIN_API jvxvstc::terminate()
	{
		return EditControllerEx1::terminate();
	}

	//------------------------------------------------------------------------
	tresult PLUGIN_API jvxvstc::setComponentState(IBStream* state)
	{
		jvxBool err = false;
		// we receive the current state of the component (processor part)
		// we read only the gain and bypass value...
		if (!state)
			return kResultFalse;

		IBStreamer streamer(state, kLittleEndian);

		std::string txtIn = CjvxVstPluginCommon::readConfiguration(streamer, err);

#if 0
		float savedGain = 0.f;
		if (streamer.readFloat (savedGain) == false)
			return kResultFalse;
		setParamNormalized (kInputGainId, savedGain);
		// jump the GainReduction
		streamer.seek(sizeof(float), kSeekCurrent);

		int32 bypassState = 0;
		if (streamer.readInt32(bypassState) == false)
			return kResultFalse;
		setParamNormalized(kBypassId, bypassState ? 1 : 0);
#endif

		// We trigger the mechanism to receive the data from the processor directly
		parameter_update_full();

		return kResultOk;
	}

	//------------------------------------------------------------------------
	IPlugView* PLUGIN_API jvxvstc::createView(const char* _name)
	{
		// someone wants my editor
		ConstString name(_name);
		if (name == ViewType::kEditor)
		{
			std::string nmUiDescr = get_ui_description_name();
			// Start the viewer by passing in the uidesc file
			theEditor = new VST3Editor(this, "view", nmUiDescr.c_str());
			return theEditor;
		}
		return nullptr;
	}

	//------------------------------------------------------------------------
	CView* jvxvstc::createCustomView(VSTGUI::UTF8StringPtr name,
		const VSTGUI::UIAttributes& attributes,
		const VSTGUI::IUIDescription* description,
		VSTGUI::VST3Editor* editor)
	{
		return nullptr;
	}

	//------------------------------------------------------------------------
	tresult PLUGIN_API jvxvstc::setState(IBStream* state)
	{
		IBStreamer streamer(state, kLittleEndian);

		int8 byteOrder;
		if (streamer.readInt8(byteOrder) == false)
			return kResultFalse;
		if (streamer.readRaw(defaultMessageText, 128 * sizeof(TChar)) == false)
			return kResultFalse;

		// if the byteorder doesn't match, byte swap the text array ...
		if (byteOrder != BYTEORDER)
		{
			for (int32 i = 0; i < 128; i++)
			{
				SWAP_16(defaultMessageText[i])
			}
		}

		// update our editors
		/*
		for (auto& uiMessageController : uiMessageControllers)
			uiMessageController->setMessageText(defaultMessageText);
		*/

		return kResultTrue;
	}

	//------------------------------------------------------------------------
	tresult PLUGIN_API jvxvstc::getState(IBStream* state)
	{
		// here we can save UI settings for example

		// as we save a Unicode string, we must know the byteorder when setState is called

		IBStreamer streamer(state, kLittleEndian);

		int8 byteOrder = BYTEORDER;
		if (streamer.writeInt8(byteOrder) == false)
			return kResultFalse;

		if (streamer.writeRaw(defaultMessageText, 128 * sizeof(TChar)) == false)
			return kResultFalse;

		return kResultTrue;
	}

	//------------------------------------------------------------------------
	tresult
		jvxvstc::notify(IMessage* message)
	{
		CjvxPluginVstController::handle_message_from_processor(message);
		return ComponentBase::notify(message);
	}

	tresult jvxvstc::receiveText(const char* text)
	{
		// received from Component
		// This function is called with the ComponentBase::notify function
		if (text)
		{
			fprintf(stderr, "[jvxvstc] received: ");
			fprintf(stderr, "%s", text);
			fprintf(stderr, "\n");
		}
		return kResultOk;
	}

	//------------------------------------------------------------------------
	tresult PLUGIN_API jvxvstc::setParamNormalized(ParamID tag, ParamValue value)
	{
		// called from host to update our parameters state
		tresult result = EditControllerEx1::setParamNormalized(tag, value);		
		return result;
	}

	//------------------------------------------------------------------------
	tresult PLUGIN_API jvxvstc::getParamStringByValue(ParamID tag, ParamValue valueNormalized,
		String128 string)
	{
		// Here, the parameter listener requests a value to string conversion.
		// The default case addresses the Parameter class directly
		return EditControllerEx1::getParamStringByValue(tag, valueNormalized, string);
	}

	//------------------------------------------------------------------------
	tresult PLUGIN_API jvxvstc::getParamValueByString(ParamID tag, TChar* string,
		ParamValue& valueNormalized)
	{
		// Here, the parameter listener requests a value to string conversion.
		// The default case addresses the Parameter class directly
		return EditControllerEx1::getParamValueByString(tag, string, valueNormalized);
	}

	//------------------------------------------------------------------------
	/*
	void jvxvstc::addUIMessageController(UIMessageController* controller)
	{
		uiMessageControllers.push_back(controller);
	}

	//------------------------------------------------------------------------
	void jvxvstc::removeUIMessageController(UIMessageController* controller)
	{
		UIMessageControllerList::const_iterator it =
			std::find(uiMessageControllers.begin(), uiMessageControllers.end(), controller);
		if (it != uiMessageControllers.end())
			uiMessageControllers.erase(it);
	}
	*/
	//------------------------------------------------------------------------
	void jvxvstc::setDefaultMessageText(String128 text)
	{
		String tmp(text);
		tmp.copyTo16(defaultMessageText, 0, 127);
	}

	//------------------------------------------------------------------------
	TChar* jvxvstc::getDefaultMessageText()
	{
		return defaultMessageText;
	}

	//------------------------------------------------------------------------
	tresult PLUGIN_API jvxvstc::queryInterface(const char* iid, void** obj)
	{
		QUERY_INTERFACE(iid, obj, IMidiMapping::iid, IMidiMapping)
			return EditControllerEx1::queryInterface(iid, obj);
	}

	//------------------------------------------------------------------------
	tresult PLUGIN_API jvxvstc::getMidiControllerAssignment(int32 busIndex,
		int16 /*midiChannel*/,
		CtrlNumber midiControllerNumber,
		ParamID& tag)
	{
		// we support for the Gain parameter all MIDI Channel but only first bus (there is only one!)
		if (busIndex == 0 && midiControllerNumber == kCtrlVolume)
		{
			// tag = kInputGainId;
			return kResultTrue;
		}
		return kResultFalse;
	}

	tresult PLUGIN_API jvxvstc::beginEdit(ParamID tag)
	{
		return EditControllerEx1::beginEdit(tag);
	}

	tresult PLUGIN_API jvxvstc::performEdit(ParamID tag, ParamValue valueNormalized)
	{
		return EditControllerEx1::performEdit(tag, valueNormalized);
		//kResultTrue;
	}

	jvxErrorType
		jvxvstc::allocate_unit(jvxSize id, jvxSize mappedId, const char* nm)
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType
		jvxvstc::allocate_parameter_normalized(const char* nm, jvxInt32 id,
			jvxCBitField flags, jvxData initval, jvxSize unit_id)
	{
		std::string nm_id = nm;
		nm_id += "<" + jvx_int322String(id) + ">";
		parameters.addParameter(USTRING(nm_id.c_str()), nullptr, 0, initval, flags, id, (Steinberg::Vst::UnitID)unit_id);
		return JVX_NO_ERROR;
	}

	jvxErrorType
		jvxvstc::allocate_parameter_norm_so(const char* nm, jvxInt32 id,
			jvxCBitField flags, const char* unit, jvxData offset, jvxData scalefac,
			jvxData initval, jvxSize unit_id, jvxSize num_digits)
	{
		//---Gain parameter--	
		std::string nm_id = nm;
		nm_id += "<" + jvx_int322String(id) + ">";
		auto* myNewParameter = new jvxVstGainParameter(flags, id, nm_id.c_str(), offset, scalefac, unit, num_digits);
		parameters.addParameter(myNewParameter);
		myNewParameter->setNormalized(initval);
		myNewParameter->setUnitID((Steinberg::Vst::UnitID)unit_id);
		return JVX_NO_ERROR;
	}

	jvxErrorType
		jvxvstc::allocate_parameter_norm_strlist(const char* nm, jvxInt32 id,
			jvxApiStringList* lstEntries,
			jvxCBitField flags, jvxData initval, 
			jvxSize unit_id, const char* empty_arg)
	{
		jvxSize i;
		std::string nm_id = nm;
		nm_id += "<" + jvx_int322String(id) + ">";
		auto* myNewParameter = new jvxVstStringListParameter(USTRING(nm_id.c_str()), id, empty_arg);
		for (i = 0; i < lstEntries->ll(); i++)
		{
			myNewParameter->appendString(USTRING(lstEntries->std_str_at(i).c_str()));
		}
		parameters.addParameter(myNewParameter);
		myNewParameter->setNormalized(initval);
		myNewParameter->setUnitID((Steinberg::Vst::UnitID)unit_id);
		return JVX_NO_ERROR;
	}

	tresult PLUGIN_API jvxvstc::endEdit(ParamID tag)
	{
		report_end_edit(tag);
		return EditControllerEx1::endEdit(tag);
		// kResultTrue;
	}
	
	Steinberg::int32 PLUGIN_API jvxvstc::getProgramListCount()
	{
		if (parameters.getParameter(preset_param_id))
			return 1;
		return 0;
	}

	//-----------------------------------------------------------------------------
	Steinberg::tresult PLUGIN_API jvxvstc::getProgramListInfo(Steinberg::int32 listIndex,
		Steinberg::Vst::ProgramListInfo& info /*out*/)
	{
		Steinberg::Vst::Parameter* param = parameters.getParameter(preset_param_id);
		if (param && listIndex == 0)
		{
			info.id = preset_param_id;
			info.programCount = (Steinberg::int32)param->toPlain(1) + 1;
			Steinberg::UString name(info.name, 128);
			name.fromAscii("Presets");
			return Steinberg::kResultTrue;
		}
		return Steinberg::kResultFalse;
	}

	//-----------------------------------------------------------------------------
	Steinberg::tresult PLUGIN_API jvxvstc::getProgramName(Steinberg::Vst::ProgramListID listId, Steinberg::int32 programIndex,
		Steinberg::Vst::String128 name /*out*/)
	{
		if (listId == preset_param_id)
		{
			Steinberg::Vst::Parameter* param = parameters.getParameter(preset_param_id);
			if (param)
			{
				Steinberg::Vst::ParamValue normalized = param->toNormalized(programIndex);
				param->toString(normalized, name);
				return Steinberg::kResultTrue;
			}
		}
		return Steinberg::kResultFalse;
	}

	void 
	jvxvstc::check_subcomponent_status(jvxCBitField check_this, jvxCBitField vs_this)
	{
		if (!ui_has_been_attached)
		{
			if (check_this == vs_this)
			{
				ui_has_been_attached = true;
				ui_attached();
			}
		}
		else
		{
			if (check_this != vs_this)
			{
				ui_detached();
				ui_has_been_attached = false;
			}
		}
	}

	//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg