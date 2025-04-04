#ifndef __CJVXFLEXIBLECONNECTIONTEXTDEVICE_H__
#define __CJVXFLEXIBLECONNECTIONTEXTDEVICE_H__

#define NUM_MESSAGES_IN_QUEUE_MAX 10
#define TIMEOUT_IF_MQUEUE_CAN_NOT_BE_HANDLED_MSEC 500

#include "CjvxFlexibleTextControlDevice.h"

// CjvxGenericRs232TextDevice
template <class T>
class CjvxFlexibleConnectionTextDevice : public T, public IjvxFlexibleTextControlDevice_interact
{
protected:
	CjvxFlexibleTextControlDevice theControl;
	jvx_propertyReferenceTriple theTrip;
	std::string fname_config;

	jvxBool output_sm_cout;
	jvxSize timeoutobserver_msec;
	jvxSize timeoutresponse_msec;
public:

	JVX_CALLINGCONVENTION CjvxFlexibleConnectionTextDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		T(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		// Name of config file
		fname_config = "myConfig.jif";

		// Specify verbose level
		jvx_bitSet(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_TIMING_SHIFT);
		jvx_bitSet(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_UNMATCHED_SHIFT);
		jvx_bitSet(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_SUBMODULE_OFFSET);
		// 	JVX_GENERIC_RS232_OUTPUT_ALLINCOMING_SHIFT
		// JVX_GENERIC_RS232_OUTPUT_ALLOUTGOING_SHIFT
		// JVX_GENERIC_RS232_OUTPUT_ERRORS_SHIFT 5

		T::output_flags = JVX_GENERIC_CONNECTION_OUTPUT_ALL;

		output_sm_cout = false; // <- output all sub module messages to cout
		timeoutobserver_msec = 1000; // Observer timeout
		timeoutresponse_msec = 10000; // Timeout to await response from device
		T::fullPingPong = true;		  // <- allows always only one message to be held in pending state
	};

	virtual JVX_CALLINGCONVENTION ~CjvxFlexibleConnectionTextDevice()
	{
		
	}

	// ==========================================================================

	virtual jvxErrorType translate_message_token_separator(oneMessage_hdr* mess, std::string& txtOut, const std::string& sep_token) override
	{
		jvxErrorType res = JVX_NO_ERROR;

		res = theControl.translate_message_token_separator(mess, txtOut, sep_token);

		if (res != JVX_NO_ERROR)
		{
			std::cout << __FUNCTION__ << ": Submodule reported error code <" << jvxErrorType_txt(res) << ">." << std::endl;
			res = JVX_ERROR_ABORT;
		}
		return res;
	};


	virtual jvxErrorType handle_single_received_text_token(const std::string& token, oneMessage_hdr* commContext, jvxSize* idIdentify) override
	{
		jvxSize uId = JVX_SIZE_UNSELECTED;
		jvxSize messType = JVX_GENERIC_CONNECTION_MESSAGE_OFFSET;

		jvxErrorType resL = theControl.process_incoming_message(token, &uId, &messType, idIdentify);
		if (resL == JVX_NO_ERROR)
		{
			commContext->uId = uId;
			commContext->tp = messType;
		}
		auto elm = theControl.lstSubTokensParse.begin();
		while (elm != theControl.lstSubTokensParse.end())
		{
			jvxSize idIdentifyLoc = 0;
			theControl.process_incoming_message(*elm, &uId, &messType, &idIdentifyLoc);
			theControl.lstSubTokensParse.erase(elm);
			elm = theControl.lstSubTokensParse.begin();
		}
		return resL;
	};

	virtual jvxErrorType handle_cancelled_message(oneMessage_hdr* commContext, jvxSize* idIdentify) override
	{
		jvxErrorType resL = theControl.process_cancelled_message(commContext, idIdentify);
		return resL;
	};

	virtual void activate_init_messages() override
	{
		theControl.messages_on_init();
	}

	virtual void report_observer_timeout() override
	{
		theControl.messages_on_observe();
	}

	//virtual jvxErrorType handle_incoming_data(jvxByte* ptr, jvxSize numRead, jvxSize offset, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis,
	//	oneMessage_hdr_load* theMess) override;

	virtual jvxErrorType activate_specific_connection()override
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

		res = T::activate_specific_connection();
		if (res != JVX_NO_ERROR)
		{
			return res;
		}

		T::_common_set.theToolsHost->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &cfProcObj, 0, NULL);
		if (cfProcObj)
		{
			cfProcObj->request_specialization(reinterpret_cast<jvxHandle**>(&hdlProcObj), NULL, NULL);
		}


		if (hdlProcObj)
		{
			jvxSize desired_mq_size = 0;
			resL = theControl.init(hdlProcObj, fname_config,
				static_cast<IjvxFlexibleTextControlDevice_interact*>(this),
				static_cast<CjvxProperties*>(this), errs,
				&desired_mq_size, true);
			if (resL == JVX_NO_ERROR)
			{
				std::cout << "::" << __FUNCTION__ << ": Successfully opened text device instruction file <" << fname_config << ">." << std::endl;
				theControl.print();

				// Setup the size of the message queue
				T::message_queue.sz_mqueue_elements = JVX_MAX(T::message_queue.sz_mqueue_elements, desired_mq_size);
				if (JVX_CHECK_SIZE_SELECTED(timeoutobserver_msec))
				{
					CjvxGenericConnectionDevice_pcg::mqueue_runtime.timeoutobserver_msec.value = timeoutobserver_msec;
				}
				if (JVX_CHECK_SIZE_SELECTED(timeoutresponse_msec))
				{
					CjvxGenericConnectionDevice_pcg::mqueue_runtime.timeoutresponse_msec.value = timeoutresponse_msec;
				}

				theControl.print();
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
			std::cout << __FUNCTION__ << ": Error: Failed to obtain file parser handle." << std::endl;
			JVX_ASSERT(0);
		}

		if (cfProcObj)
		{
			T::_common_set.theToolsHost->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, cfProcObj);
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
	};

	virtual jvxErrorType deactivate_specific_connection()override
	{
		jvxErrorType res = JVX_NO_ERROR;

		theTrip.theObj = NULL;
		theTrip.theHdl = NULL;
		theTrip.theProps = NULL;

		theControl.terminate();

		res = T::deactivate_specific_connection();

		return JVX_NO_ERROR;
	};

	virtual jvxErrorType goodbye_connection()override
	{
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType post_message_hook(jvxSize idIdentify) override
	{
		// Default behavior: forward to text control
		return JVX_NO_ERROR;
	}

	virtual jvxErrorType cleared_messages_hook() override
	{
		return theControl.cleared_messages();
	}


	// ==========================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION convert_property_2_txt(const jvx::propertyDescriptor::CjvxPropertyDescriptorCore* descr, jvxSize convert_id,
		std::string& txt_out, jvxSize callback_id, jvxSize num_digits) override
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxBool foundit = false;
		jvxHandle* propref = NULL;
		std::string txt_out_pp;
		jvxBool ctentonly = true;
		jvxSize offset = 0;
		jvxSize num_elms = JVX_SIZE_UNSELECTED;
		jvxCallManagerProperties callGate;
		// jvxSize num_digits = 4;
		jvxBool bin_list = false;
		jvxBool noEntry = false;
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
				&noEntry,
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

	virtual jvxErrorType JVX_CALLINGCONVENTION convert_txt_2_property(const jvx::propertyDescriptor::CjvxPropertyDescriptorCore* descr, jvxSize convert_id,
		const std::string& txt_in, const std::string& ref_token, 
		jvxSize callback_id, jvxCBitField convertFinetuning) override
	{
		jvxErrorType res = JVX_NO_ERROR;	
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
					convertFinetuning);
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

	virtual jvxErrorType JVX_CALLINGCONVENTION add_message_queue(oneMessage_hdr* load_plus, jvxHandle* priv, jvxBool expect_response) override
	{
		return T::add_message_queue(load_plus, priv, expect_response);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION device_check_pending_uid(jvxSize uId, jvxSize mTp, jvxTick* Stamp, oneMessage_hdr** ptr_hdr_ret) override
	{
		return T::check_pending_uid(uId, mTp, ptr_hdr_ret);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION report_message(const std::string& mess) override
	{
		if (output_sm_cout)
		{
			std::cout << "::--SUBMODULE: " << mess << std::endl;
		}
		else
		{
			if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_SUBMODULE_OFFSET))
			{
				jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
				if (T::embLog.jvxrtst_bkp.theTextLogger_hdl && T::embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					jvx_lock_text_log(T::embLog, logLev JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
					T::embLog.jvxrtst << "::SUBMODULE: " << mess << std::endl;
					jvx_unlock_text_log(T::embLog JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
				}
			}
		}
		return JVX_NO_ERROR;
	}

	virtual jvxErrorType send_direct(oneMessage_hdr* load_plus) override
	{
		return T::send_direct(load_plus);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_callback(jvxFlexibleControlEventType tp, jvxSize callback_id, jvxHandle* spec) override
	{
		if (output_sm_cout)
		{
			std::cout << "::--SUBMODULE: Reported callback with id <" << jvx_size2String(callback_id) << ">." << std::endl;
		}
		return JVX_NO_ERROR;
	}

	// =================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans) override
	{
		jvxErrorType res = T::set_property(
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

	jvxErrorType decide_quality(
		jvxRemoteConnectionQuality* quality,
			jvxSize num_incoming_messages,
			jvxSize num_skip_messages,
			jvxSize num_unmatched_messages,
			jvxSize num_unknown_messages) override
	{
		// Very simple default rule to derive quality
		*quality = jvxRemoteConnectionQuality::JVX_REMOTE_CONNECTION_QUALITY_NO_DATA;
		if (num_incoming_messages > 0)
		{
			*quality = jvxRemoteConnectionQuality::JVX_REMOTE_CONNECTION_QUALITY_GOOD;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType inform_identified_unmatched(const char* ruleUnmatched) override
	{
		return JVX_NO_ERROR;
	}
};

#endif