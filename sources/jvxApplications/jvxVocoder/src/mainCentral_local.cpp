#include "mainCentral_local.h"

#include <QScrollArea>
#include <QMainWindow>

#include "jvx.h"

#include "jvxAudioHost_common.h"
#include "realtimeViewer_helpers.h"

#define WARPING_MAX 0.1

// =================================================================================
void
mainCentral::init_submodule(IjvxHost* theHost)
{
	theHostRef = theHost;
	this->setupUi(this);

	this->processing = false;

	reset_property_ids();

}

void
mainCentral::terminate_submodule( )
{
}

void 
mainCentral::init_extend_specific_widgets(IjvxMainWindowControl* ctrl, IjvxQtSpecificWidget_report* rep_spec_widgets)
{
}

void mainCentral::terminate_extend_specific_widgets()
{
}

void
mainCentral::return_widget(QWidget** wdg)
{
	*wdg = static_cast<QWidget*>(this);
}

void 
mainCentral::get_reference_feature(jvxHandle** priv)
{
}

void 
mainCentral::inform_select(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{

}

void 
mainCentral::inform_unselect(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
}

/* Some project specific simplifying callbacks */
void
mainCentral::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{

	switch(tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:

		/* Obtain all ids for properties */
		this->refresh_property_ids();
		break;
	default:
		break;
	}
}

void
mainCentral::inform_inactive(const jvxComponentIdentification&  tp, IjvxAccessProperties* propRef)
{
	switch(tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:

		/* Obtain all ids for properties */
		this->reset_property_ids();

		jvxCBitField prio;
		jvx_bitFClear(prio);
		jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
		this->update_window_user(prio);

		break;
	default:
		break;
	}
}

void
mainCentral::inform_sequencer_about_to_start()
{
	this->processing = true;
	this->refresh_property_ids();
}

void
mainCentral::inform_sequencer_started()
{
}

void
mainCentral::inform_sequencer_about_to_stop()
{
}

void
mainCentral::inform_sequencer_stopped()
{
	this->processing = false;
	this->refresh_property_ids();
}

void
mainCentral::inform_viewer_about_to_start(jvxSize* tout_msec)
{
}

void
mainCentral::inform_viewer_started()
{
}

void
mainCentral::inform_viewer_about_to_stop()
{
}

void
mainCentral::inform_viewer_stopped()
{
}
void
mainCentral::inform_request_shutdown(jvxBool *requestHandled)
{
	if (requestHandled)
		*requestHandled = false;
}

void
mainCentral::inform_update_window(jvxCBitField prio)
{
	update_window_user(prio);
}

void
mainCentral::inform_update_window_periodic()
{
	update_window_periodic();
}

void
mainCentral::inform_bootup_complete(jvxBool* wantsToAdjustSize)
{
	if(wantsToAdjustSize)
	{
		*wantsToAdjustSize = true;
	}
	//theParentRef->adjustSize();
}

void
mainCentral::inform_internals_have_changed(const jvxComponentIdentification&  tp, IjvxObject* theObj, jvxPropertyCategoryType cat, 
	jvxSize propId, bool onlyContent, const jvxComponentIdentification&  tpTo, jvxPropertyCallPurpose purp)
{
}

jvxErrorType
mainCentral::inform_sequencer_callback(jvxSize functionId)
{
	return(JVX_NO_ERROR);
}


// =============================================================
// USER INTERFACE
// =============================================================

void
mainCentral::button_out1()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	jvxSelectionList selLst;
	IjvxHost* theHostRef = NULL;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_outputSelect))
		{
			selLst.bitFieldSelected() = 0x1;

			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), 
					ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
				jvx_returnReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			}
		}
	}
}

void
mainCentral::button_out2()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_outputSelect))
		{
			selLst.bitFieldSelected() = 0x2;

			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
				jvx_returnReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			}
		}
	}
}

void
mainCentral::button_out3()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_outputSelect))
		{
			selLst.bitFieldSelected() = 0x4;

			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
				jvx_returnReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			}
		}
	}
}

void
mainCentral::button_out4()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_outputSelect))
		{
			selLst.bitFieldSelected() = 0x8;

			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
				jvx_returnReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			}
		}
	}
}

void
mainCentral::button_out5()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_outputSelect))
		{
			selLst.bitFieldSelected() = 0x10;

			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
				jvx_returnReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			}
		}
	}
}

void
mainCentral::toggle_pitch(bool tog)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_autoModePitch))
		{
			if(tog)
			{
				selLst.bitFieldSelected() = 0x1;
			}
			else
			{
				selLst.bitFieldSelected() = 0x2;
			}

			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					properties.node.id_autoModePitch,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_autoModePitch,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
				jvx_returnReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			}
		}
	}
}

void
mainCentral::toggle_autovuv(bool tog)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_autoModeVuv))
		{
			if(tog)
			{
				selLst.bitFieldSelected() = 0x1;
			}
			else
			{
				selLst.bitFieldSelected() = 0x2;
			}

			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					properties.node.id_autoModeVuv,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_autoModeVuv,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
				jvx_returnReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			}
		}
	}
}

void
mainCentral::select_in(int idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_inputSelect))
		{
			if(idx == 0)
			{
				selLst.bitFieldSelected() = 0x2;
			}
			else
			{
				selLst.bitFieldSelected() = 0x1;
			}

			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					properties.node.id_inputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_inputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
				jvx_returnReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			}
		}
	}
}

void
mainCentral::changed_mix(int sel)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valDbl = 0.0;
	jvx_propertyReferenceTriple theTripleNode;
	valDbl = sel * 0.01;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_MixingCrit))
		{
			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					properties.node.id_MixingCrit,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&valDbl, 1, JVX_DATAFORMAT_DATA), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_MixingCrit,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
			}
		}
	}
}

void
mainCentral::changed_pitch_scale(int val)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valDbl = 0.0;
	jvx_propertyReferenceTriple theTripleNode;
	jvxCallManagerProperties callGate;
	valDbl = val - 50;
	valDbl = valDbl / 5.0;
	valDbl = pow(10.0, valDbl/10.0);

	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_pitchScale))
		{
			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					properties.node.id_pitchScale,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(false);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&valDbl, 1, JVX_DATAFORMAT_DATA), ident, trans );
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_pitchScale,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
			}
		}
	}
}

void
mainCentral::newValue_pitch_max()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	QString txt = this->lineEdit_pitchmax->text();
	jvxInt32 val = txt.toInt();
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_pitchFrequencyMax))
		{
			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					properties.node.id_pitchFrequencyMax,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(false);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&val, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_pitchFrequencyMax,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
			}
		}
	}
}

void
mainCentral::newValue_pitch_min()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	QString txt = this->lineEdit_pitchmin->text();
	jvxInt32 val = txt.toInt();
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_pitchFrequencyMin))
		{
			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					properties.node.id_pitchFrequencyMin,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(false);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&val, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_pitchFrequencyMin,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
			}
		}
	}
}

void
mainCentral::newValue_lpc_order()
{
	int order = horizontalSlider_lporder->value();

	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	jvxValueInRange rg;
	rg.val() = order;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_lpcOrder))
		{
			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					properties.node.id_lpcOrder,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&rg, 1, JVX_DATAFORMAT_VALUE_IN_RANGE), ident, trans );
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_lpcOrder,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
			}
		}
	}
}

void
mainCentral::newValue_form_warp(int val)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valDbl = 0.0;
	jvx_propertyReferenceTriple theTripleNode;
	valDbl = val - 50;
	valDbl = valDbl / 51.0;
	valDbl *= WARPING_MAX;
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_formantWarp))
		{
			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					properties.node.id_formantWarp,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(false);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&valDbl, 1, JVX_DATAFORMAT_DATA), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_formantWarp,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
			}
		}
	}
}

void
mainCentral::newValue_pitch()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTripleNode;
	QString txt = this->lineEdit_pitch->text();
	jvxInt32 val = txt.toInt();
	jvxCallManagerProperties callGate;
	if(theHostRef)
	{
		if(JVX_CHECK_SIZE_SELECTED(properties.node.id_pitchFrequency))
		{
			jvx_initPropertyReferenceTriple(&theTripleNode);
			res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			if(res == JVX_NO_ERROR)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					properties.node.id_pitchFrequency,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				jvx::propertyDetail::CjvxTranferDetail trans(false);

				res = theTripleNode.theProps->set_property(callGate, 
					jPRG(&val, 1,JVX_DATAFORMAT_32BIT_LE), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_pitchFrequency,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					this->update_window_user();
				}
			}
		}
	}
}

#include "templates/jvxProperties/CjvxAccessProperties_direct.h"
void
mainCentral::refresh_property_ids()
{
	jvx_propertyReferenceTriple theTripleNode;
	CjvxAccessProperties_direct thePropRef;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxBool foundit = false;
	jvxBool isValid = true;
	jvxSize hdlIdx = 0;

	this->reset_property_ids();

	if(theHostRef)
	{
		jvx_initPropertyReferenceTriple(&theTripleNode);
		res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
		thePropRef.initialize(&theTripleNode);

		if(res == JVX_NO_ERROR)
		{
			if(jvx_isValidPropertyReferenceTriple(&theTripleNode))
			{
				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_MIXINGCRIT", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_MixingCrit = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_PITCHFREQUENCY", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_pitchFrequency = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_PITCHFREQUENCYMIN", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_pitchFrequencyMin = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_PITCHFREQUENCYMAX", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_pitchFrequencyMax = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_OUTPUTSELECT", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_outputSelect = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_INPUTSELECT", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_inputSelect = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_HARDVUVDECISION", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_hardVuVDecision = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_AUTOMODEVUV", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_autoModeVuv = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_AUTOMODEPITCH", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_autoModePitch = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_LPCORDER", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_lpcOrder = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_VOLUME", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_volume = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_PITCH_SCALE", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_pitchScale = hdlIdx;
				}
				else
				{
					assert(0);
				}

				// ===============================================================
				// ===============================================================
				foundit = false;
				hdlIdx = 0;
				isValid = false;
				FIND_PROPERTY_ID((&thePropRef), hdlIdx, "/JVX_VOCODER_FORMAT_WARP", isValid, foundit, callGate);
				if(foundit && isValid)
				{
					properties.node.id_formantWarp = hdlIdx;
				}
				else
				{
					assert(0);
				}
				thePropRef.terminate();
				jvx_returnReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
			}//if(jvx_isValidPropertyReferenceTriple((&theTripleNode)))

		}// if(res == JVX_NO_ERROR)
	} // if(theHostRef)
}

void
mainCentral::reset_property_ids()
{
	properties.node.id_MixingCrit = JVX_SIZE_UNSELECTED;
	properties.node.id_pitchFrequency = JVX_SIZE_UNSELECTED;
	properties.node.id_pitchFrequencyMin = JVX_SIZE_UNSELECTED;
	properties.node.id_pitchFrequencyMax = JVX_SIZE_UNSELECTED;
	properties.node.id_outputSelect = JVX_SIZE_UNSELECTED;
	properties.node.id_inputSelect = JVX_SIZE_UNSELECTED;
	properties.node.id_hardVuVDecision = JVX_SIZE_UNSELECTED;
	properties.node.id_autoModeVuv = JVX_SIZE_UNSELECTED;
	properties.node.id_autoModePitch = JVX_SIZE_UNSELECTED;
	properties.node.id_lpcOrder = JVX_SIZE_UNSELECTED;
	properties.node.id_volume = JVX_SIZE_UNSELECTED;
	properties.node.id_formantWarp = JVX_SIZE_UNSELECTED;
	properties.node.id_pitchScale = JVX_SIZE_UNSELECTED;
}

void
mainCentral::update_window_user(jvxCBitField prio)
{
	jvx_propertyReferenceTriple theTripleNode;
	jvxErrorType res = JVX_NO_ERROR;

	jvxSelectionList selList;
	

	jvxInt32 valI32;
	jvxData valDbl;
	jvxValueInRange valIR;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	if(!this->processing)
	{
		this->lineEdit_pitch->setText("--");
	}
	else
	{
	}

	if(theHostRef)
	{
		jvx_initPropertyReferenceTriple(&theTripleNode);
		res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
		if(res == JVX_NO_ERROR)
		{
			if(jvx_isValidPropertyReferenceTriple(&theTripleNode))
			{
				/*
				if(this->properties.node.id_MixingCrit >= 0) <- only while running
				if(this->properties.node.id_pitchFrequency >= 0)
				if(this->properties.node.id_pitchFrequencyMin >= 0)
				if(this->properties.node.id_pitchFrequencyMax >= 0)
				if(this->properties.node.id_outputSelect >= 0)
				if(this->properties.node.id_inputSelect >= 0)
				if(this->properties.node.id_hardVuVDecision >= 0)
				if(this->properties.node.id_autoModeVuv >= 0)
				if(this->properties.node.id_autoModePitch >= 0)
				if(this->properties.node.id_lpcOrder >= 0)
				if(this->properties.node.id_volume >= 0)
				res = theTripleNode.theProps->get_property(callGate, jPRG( &selList, 1,
					JVX_DATAFORMAT_SELECTION_LIST, properties.technology.id_technologies,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, false, &isValid, &accessTp);
				*/
				ident.reset(properties.node.id_MixingCrit,JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &valDbl, 1,
					JVX_DATAFORMAT_DATA),ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_MixingCrit,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					valI32 = JVX_DATA2INT32(valDbl * 100);
					valI32 = JVX_MAX(valI32, 0);
					valI32 = JVX_MIN(valI32, 100);
					verticalSlider_vuv->setValue(valI32);
				}
				else
				{
				}

				ident.reset(properties.node.id_pitchFrequency,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();

				res = theTripleNode.theProps->get_property(callGate, jPRG( &valI32, 1,
					JVX_DATAFORMAT_32BIT_LE),ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_pitchFrequency,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					lineEdit_pitch->setText(jvx_int2String(valI32).c_str());
				}
				else
				{
					lineEdit_pitch->setText("--");
				}

				ident.reset(properties.node.id_pitchFrequencyMin,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &valI32, 1,
					JVX_DATAFORMAT_32BIT_LE), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_pitchFrequencyMin,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					lineEdit_pitchmin->setText(jvx_int2String(valI32).c_str());
				}
				else
				{
					lineEdit_pitchmin->setText("--");
				}

				ident.reset(properties.node.id_pitchFrequencyMax,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &valI32, 1,
					JVX_DATAFORMAT_32BIT_LE), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_pitchFrequencyMax,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					lineEdit_pitchmax->setText(jvx_int2String(valI32).c_str());
				}
				else
				{
					lineEdit_pitchmax->setText("--");
				}

				QPalette paletten;
		        QBrush brushn(QColor(120, 120, 120, 255));
		        brushn.setStyle(Qt::SolidPattern);
				paletten.setBrush(QPalette::Active, QPalette::ButtonText, brushn);
				paletten.setBrush(QPalette::Inactive, QPalette::ButtonText, brushn);
				pushButton_out1->setPalette(paletten);
				pushButton_out2->setPalette(paletten);
				pushButton_out3->setPalette(paletten);
				pushButton_out4->setPalette(paletten);
				pushButton_out5->setPalette(paletten);

				ident.reset(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &selList, 1,
					JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_outputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					QPalette paletter;
					QBrush brushr(QColor(255, 0, 0, 255));
					brushr.setStyle(Qt::SolidPattern);
					paletter.setBrush(QPalette::Active, QPalette::ButtonText, brushr);
					paletter.setBrush(QPalette::Inactive, QPalette::ButtonText, brushr);
					if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
						pushButton_out1->setPalette(paletter);
					if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x2))
						pushButton_out2->setPalette(paletter);
					if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x4))
						pushButton_out3->setPalette(paletter);
					if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x8))
						pushButton_out4->setPalette(paletter);
					if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x10))
						pushButton_out5->setPalette(paletter);
				}
				else
				{
					//
				}

				ident.reset(properties.node.id_inputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &selList, 1,
					JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_inputSelect,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
					{
						verticalSlider_in->setValue(1);
					}
					else
					{
						verticalSlider_in->setValue(0);
					}
				}
				else
				{
					//
				}

				ident.reset(properties.node.id_autoModePitch,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &selList, 1,
					JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_autoModePitch,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
					{
						checkBox_autopitch->setChecked(true);
					}
					else
					{
						checkBox_autopitch->setChecked(false);
					}
				}
				else
				{
					//
				}

				ident.reset(properties.node.id_autoModeVuv,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &selList, 1,
					JVX_DATAFORMAT_SELECTION_LIST),ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_autoModeVuv,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
					{
						checkBox_autovuv->setChecked(true);
					}
					else
					{
						checkBox_autovuv->setChecked(false);
					}
				}
				else
				{
					//
				}

				ident.reset(properties.node.id_lpcOrder,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &valIR, 1,
					JVX_DATAFORMAT_VALUE_IN_RANGE), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_lpcOrder,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					horizontalSlider_lporder->setValue(valIR.val());
				}
				else
				{
				}

				ident.reset(properties.node.id_pitchScale,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &valDbl, 1,
					JVX_DATAFORMAT_DATA), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_pitchScale,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					valDbl = 10*log10(valDbl);
					valDbl *=5;
					valDbl += 50.0;
					valI32 = JVX_DATA2INT32(valDbl);

					horizontalSlider_pitch_scale->setValue(valI32);
				}
				else
				{
				}

				ident.reset(properties.node.id_formantWarp,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &valDbl, 1,
					JVX_DATAFORMAT_DATA), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_formantWarp,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					valDbl /= WARPING_MAX;
					valDbl = valDbl * 51;
					valDbl += 50.0;
					valI32 = JVX_DATA2INT32(valDbl);

					horizontalSlider_formant_warp->setValue(valI32);
				}
				else
				{
				}
			}
		}
	}
}

void
mainCentral::update_window_periodic()
{
	jvx_propertyReferenceTriple theTripleNode;
	jvxErrorType res = JVX_NO_ERROR;

	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	jvxInt32 valI32;
	jvxData valDbl;

	if(!this->processing)
	{
		this->lineEdit_pitch->setText("--");
	}
	else
	{
	}

	if(theHostRef)
	{
		jvx_initPropertyReferenceTriple(&theTripleNode);
		res = jvx_getReferencePropertiesObject(theHostRef, &theTripleNode, JVX_COMPONENT_AUDIO_NODE);
		if(res == JVX_NO_ERROR)
		{
			if(jvx_isValidPropertyReferenceTriple(&theTripleNode))
			{
				ident.reset(properties.node.id_autoModeVuv,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &selList, 1,
					JVX_DATAFORMAT_SELECTION_LIST),ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_autoModeVuv,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
					{
						ident.reset(properties.node.id_MixingCrit,
							JVX_PROPERTY_CATEGORY_UNSPECIFIC);
						trans.reset();
						res = theTripleNode.theProps->get_property(callGate, jPRG( &valDbl, 1,
							JVX_DATAFORMAT_DATA), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_MixingCrit,
							JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
						{
							valI32 = JVX_DATA2INT32(valDbl * 100);
							valI32 = JVX_MAX(valI32, 0);
							valI32 = JVX_MIN(valI32, 100);
							verticalSlider_vuv->setValue(valI32);
						}
					}
				}

				ident.reset(properties.node.id_autoModePitch,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC);
				trans.reset();
				res = theTripleNode.theProps->get_property(callGate, jPRG( &selList, 1,
					JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_autoModePitch,
					JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
				{
					if(JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
					{
						ident.reset(properties.node.id_pitchFrequency,
							JVX_PROPERTY_CATEGORY_UNSPECIFIC);
						trans.reset();

						res = theTripleNode.theProps->get_property(callGate, jPRG( &valI32, 1,
							JVX_DATAFORMAT_32BIT_LE),ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(properties.node.id_pitchFrequency,
							JVX_PROPERTY_CATEGORY_UNSPECIFIC), theTripleNode.theProps))
						{
							lineEdit_pitch->setText(jvx_int2String(valI32).c_str());
						}
					}
				}
			}
		}
	}
}
