#include "CjvxFlexibleRs232TextDevice.h"
#include <sstream>
#include "jvxGenericRS232Technologies/CjvxGenericRS232Device.h"

CjvxFlexibleRs232TextDevice::CjvxFlexibleRs232TextDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxGenericRS232TextDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	// Name of config file
	fname_config = "myConfig.jif";

	// Specify verbose level
	jvx_bitSet(output_flags, JVX_GENERIC_RS232_OUTPUT_TIMING_SHIFT );
	jvx_bitSet(output_flags, JVX_GENERIC_RS232_OUTPUT_UNMATCHED_SHIFT);
	jvx_bitSet(output_flags, JVX_GENERIC_RS232_OUTPUT_SUBMODULE_OFFSET);
	// 	JVX_GENERIC_RS232_OUTPUT_ALLINCOMING_SHIFT
	// JVX_GENERIC_RS232_OUTPUT_ALLOUTGOING_SHIFT
	// JVX_GENERIC_RS232_OUTPUT_ERRORS_SHIFT 5

	output_flags = JVX_GENERIC_RS232_OUTPUT_ALL;

	output_sm_cout = false; // <- output all sub module messages to cout
	timeoutobserver_msec = 1000; // Observer timeout
	timeoutresponse_msec = 10000; // Timeout to await response from device
	fullPingPong = true; // <- allows always only one message to be held in pending state

}

CjvxFlexibleRs232TextDevice::~CjvxFlexibleRs232TextDevice()
{
}

jvxErrorType
CjvxFlexibleRs232TextDevice::activate_specific_rs232()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	IjvxObject* cfProcObj = NULL;
	IjvxConfigProcessor* hdlProcObj = NULL;
	jvxErrorType resL = JVX_NO_ERROR;
	std::vector<std::string> errs;
	
	/*
	genPumpRs232_device::init_all();
	genPumpRs232_device::allocate_all();
	genPumpRs232_device::register_all(static_cast<CjvxProperties*>(this));
	*/

	res = CjvxGenericRS232TextDevice::activate_specific_rs232();
	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	_common_set.theToolsHost->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &cfProcObj, 0, NULL);
	if (cfProcObj)
	{
		cfProcObj->request_specialization(reinterpret_cast<jvxHandle**>(&hdlProcObj), NULL, NULL);
	}


	if (hdlProcObj)
	{
		jvxSize desired_mq_size = 0;
		resL = theControl.init(hdlProcObj, fname_config,
			static_cast<IjvxFlexibleTextControlDevice_interact*>(this),
			static_cast<IjvxProperties*>(this), errs,
			&desired_mq_size, true);
		if (resL == JVX_NO_ERROR)
		{
			std::cout << "::" << __FUNCTION__ << ": Successfully opened rs232 device instruction file <" << fname_config << ">." << std::endl;
			theControl.print();

			// Setup the size of the message queue
			message_queue.sz_mqueue_elements = JVX_MAX(message_queue.sz_mqueue_elements, desired_mq_size);
			if (JVX_CHECK_SIZE_SELECTED(timeoutobserver_msec))
			{
				CjvxGenericRs232Device_pcg::rs232_runtime.timeoutobserver_msec.value = timeoutobserver_msec;
			}
			if (JVX_CHECK_SIZE_SELECTED(timeoutresponse_msec))
			{
				CjvxGenericRs232Device_pcg::rs232_runtime.timeoutresponse_msec.value = timeoutresponse_msec;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;

			std::cout << "Failed to open device instruction file <" << fname_config << ">, reason: " << jvxErrorType_descr(resL) << std::endl;
			for (i = 0; i < errs.size(); i++)
			{
				std::cout << "Error: " << errs[i] << std::endl;
			}
			JVX_ASSERT(0);
		}
	}
	else
	{
		res = JVX_ERROR_INTERNAL;
		std::cout << "Failed to obtain file parser." << std::endl;
	}

	if (cfProcObj)
	{
		_common_set.theToolsHost->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, cfProcObj);
	}
	cfProcObj = NULL;
	hdlProcObj = NULL;

	if (res == JVX_NO_ERROR)
	{
		theTrip.theObj = static_cast<IjvxObject*>(this);
		theTrip.theHdl = static_cast<IjvxObject*>(this);
		theTrip.theProps = static_cast<IjvxProperties*>(this);
	}

	return res;
}

jvxErrorType
CjvxFlexibleRs232TextDevice::deactivate_specific_rs232()
{
	jvxErrorType res = JVX_NO_ERROR;

	theTrip.theObj = NULL;
	theTrip.theHdl = NULL;
	theTrip.theProps = NULL;

	theControl.terminate();

	res = CjvxGenericRS232TextDevice::deactivate_specific_rs232();

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFlexibleRs232TextDevice::goodbye_rs232()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFlexibleRs232TextDevice::post_message_hook(jvxSize idIdentify)
{
	// Default behavior: forward to text control
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFlexibleRs232TextDevice::cleared_messages_hook()
{
	return theControl.cleared_messages();
}

// ===========================================================================================================
// ===========================================================================================================

jvxErrorType 
CjvxFlexibleRs232TextDevice::translate_message_token_separator(oneMessage_hdr* mess, std::string& txtOut, const std::string& sep_token)
{
	jvxSize idSel;
	jvxErrorType res = JVX_NO_ERROR;

	res = theControl.translate_message_token_separator(mess, txtOut, sep_token);

	if (res != JVX_NO_ERROR)
	{
		std::cout << __FUNCTION__ << ": Submodule reported error code <" << jvxErrorType_txt(res) << ">." << std::endl;
 		res = JVX_ERROR_ABORT;
	}
	return res;
}

jvxErrorType 
CjvxFlexibleRs232TextDevice::handle_single_received_text_token(const std::string& token, oneMessage_hdr* commContext, jvxSize* idIdentify)
{
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxSize messType = JVX_GENERIC_RS232_MESSAGE_OFFSET;

	jvxErrorType resL = theControl.process_incoming_message(token, &uId, &messType, idIdentify);
	if (resL == JVX_NO_ERROR)
	{
		commContext->uId = uId;
		commContext->tp = messType;
	}
	return resL;
}

jvxErrorType 
CjvxFlexibleRs232TextDevice::handle_cancelled_message(oneMessage_hdr* commContext, jvxSize* idIdentify)
{
	jvxErrorType resL = theControl.process_cancelled_message(commContext, idIdentify);
	return resL;
}
void
CjvxFlexibleRs232TextDevice::activate_init_messages()
{
	theControl.messages_on_init();
}

void 
CjvxFlexibleRs232TextDevice::report_observer_timeout()
{
	theControl.messages_on_observe();
}

// ===========================================================================================================
// ===========================================================================================================

jvxErrorType 
CjvxFlexibleRs232TextDevice::convert_property_2_txt(const jvx::propertyDescriptor::CjvxPropertyDescriptorCore* descr, jvxSize convert_id, std::string& txt_out,
	jvxSize callback_id, jvxSize num_digits_token)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvxBool foundit = false;
	jvxHandle* propref = NULL;
	std::string txt_out_pp;
	jvxBool ctentonly = true;
	jvxSize offset = 0;
	jvxSize num_elms = JVX_SIZE_UNSELECTED;
	jvxCallManagerProperties callGate;
	jvxSize num_digits = 4;
	jvxBool bin_list = false;
	jvxCBitField fTuning = 0;
	jvx_bitSet(fTuning, JVX_STRING_PROPERTY_FINE_TUNING_SELECTIONLIST_SELID_SHIFT);

	if (jvx_isValidPropertyReferenceTriple(&theTrip))
	{
		res = jvx::helper::properties::toString(theTrip,
			callGate,
			*descr,
			txt_out,
			txt_out_pp,
			num_digits,
			bin_list,
			offset,
			num_elms,
			ctentonly,
			fTuning);
		switch (descr->format)
		{
		case JVX_DATAFORMAT_STRING_LIST:
			txt_out = txt_out_pp;
			break;
		case JVX_DATAFORMAT_SELECTION_LIST:
			if (!ctentonly)
			{
				txt_out += "::" + txt_out_pp;
			}
			break;

			// array with base64 encode should not be used here since bin_list is always "false"
		default:
			break;
		}
	}
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

jvxErrorType 
CjvxFlexibleRs232TextDevice::convert_txt_2_property(const jvx::propertyDescriptor::CjvxPropertyDescriptorCore* descr, jvxSize convert_id, const std::string& txt_in,
	const std::string& ref_token, jvxSize callback_id, jvxCBitField fTuning)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvxBool foundit = false;
	jvxHandle* propref = NULL;

	jvxBool ctentonly = true;
	jvxSize offset = 0;
	jvxSize num_digits = 4;
	jvxBool bin_list = false;
	jvxCallManagerProperties callGate;

	if (JVX_CHECK_SIZE_SELECTED(descr->globalIdx))
	{
		if (jvx_isValidPropertyReferenceTriple(&theTrip))
		{
			callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
			res = jvx::helper::properties::fromString(
				theTrip,
				callGate,
				*descr,
				txt_in,
				"",
				offset,
				fTuning);
		}
		else
		{
			res = JVX_ERROR_UNSUPPORTED;
		}
	}
	else
	{
		std::cout << "Property without associated reference, conversion id <" << convert_id << ">, callback id <" << callback_id << ">." << std::endl;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxFlexibleRs232TextDevice::add_message_queue(oneMessage_hdr* load_plus, jvxHandle* priv, jvxBool expect_response)
{
	return CjvxGenericRS232TextDevice::add_message_queue(load_plus, priv, expect_response);
}

jvxErrorType
CjvxFlexibleRs232TextDevice::send_direct(oneMessage_hdr* load_plus)
{
	return CjvxGenericRS232TextDevice::send_direct(load_plus);
}

jvxErrorType
CjvxFlexibleRs232TextDevice::device_check_pending_uid(jvxSize uId, jvxSize mTp, jvxTick* tStamp_sent, oneMessage_hdr** ptr_hdr_ret)
{
	return check_pending_uid(uId, mTp, ptr_hdr_ret);
}

jvxErrorType 
CjvxFlexibleRs232TextDevice::report_message(const std::string& mess)
{
	if (output_sm_cout)
	{
		std::cout << "::--SUBMODULE: " << mess << std::endl;
	}
	else
	{
		if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_SUBMODULE_OFFSET))
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::SUBMODULE: " << mess << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxFlexibleRs232TextDevice::trigger_callback(jvxFlexibleControlEventType tp, jvxSize callback_id, jvxHandle* spec)
{
	if (output_sm_cout)
	{
		std::cout << "::--SUBMODULE: Reported callback with id <" << jvx_size2String(callback_id) << ">." << std::endl;
	}
	return JVX_NO_ERROR;
}

// ======================================================================================
jvxErrorType 
CjvxFlexibleRs232TextDevice::set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = CjvxGenericRS232TextDevice::set_property(
		callGate, rawPtr, ident, trans);

	if (res == JVX_NO_ERROR)
	{
		
		if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)
		{
			JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
			theControl.property_set(category, propId);
		}
	}
	return res;
}
