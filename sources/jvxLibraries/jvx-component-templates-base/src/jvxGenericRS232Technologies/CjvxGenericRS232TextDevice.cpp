#include "jvxGenericRS232Technologies/CjvxGenericRS232TextDevice.h"

#include <sstream>

#define JVX_NUM_RS232_SEP_TOKENS 5
static const char * rs2323text_separator_token[JVX_NUM_RS232_SEP_TOKENS] =
{
	"\r",
	"\n",
	"\r\n",
	"\r\r\n",
	"\r\n\xFF"
};

CjvxGenericRS232TextDevice::CjvxGenericRS232TextDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxGenericRS232Device(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{	
	localTxtLog.useLocalTextLog = true;
	localTxtLog.szLocalTextLog = 128;
	localTxtLog.theLocalTextLog_obj = NULL;
	localTxtLog.theLocalTextLog = NULL;

	JVX_INITIALIZE_MUTEX(safeAccessDataAvail);
	jvx_bitFClear(reportEventDataAvail);
	JVX_INITIALIZE_MUTEX(safeTextBuffer);
	// receivedTextBuffer;

	// Specify number of bytes in input string field
}

CjvxGenericRS232TextDevice::~CjvxGenericRS232TextDevice()
{
	JVX_TERMINATE_MUTEX(safeAccessDataAvail);
	JVX_TERMINATE_MUTEX(safeTextBuffer);
}

// =================================================================================================

jvxErrorType
CjvxGenericRS232TextDevice::select(IjvxObject* theOwner)
{
	jvxErrorType res = CjvxGenericRS232Device::select(theOwner);
	if (res == JVX_NO_ERROR)
	{
		CjvxGenericRs232TextDevice_pcg::init_all();
		CjvxGenericRs232TextDevice_pcg::allocate_all();
		CjvxGenericRs232TextDevice_pcg::register_all(static_cast<CjvxProperties*>(this));
		CjvxGenericRs232TextDevice_pcg::register_callbacks(static_cast<CjvxProperties*>(this),
			cb_rs232_local_text_log_set, cb_rs232_generic_message_set, reinterpret_cast<jvxHandle*>(this), NULL);
	}
	return(res);
}

jvxErrorType
CjvxGenericRS232TextDevice::unselect()
{
	jvxErrorType res = CjvxGenericRS232Device::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		CjvxGenericRs232TextDevice_pcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxGenericRs232TextDevice_pcg::deallocate_all();
		CjvxGenericRs232TextDevice_pcg::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);


		CjvxGenericRS232Device::unselect();
	}
	return(res);
}

// =================================================================================================

jvxErrorType
CjvxGenericRS232TextDevice::prepare()
{
	jvxErrorType res = CjvxGenericRS232Device::prepare();
	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_update_properties_on_start();
	}
	return(res);
}

jvxErrorType
CjvxGenericRS232TextDevice::postprocess()
{
	jvxErrorType res = CjvxGenericRS232Device::postprocess();
	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_update_properties_on_stop();
	}
	return(res);
}

// ===========================================================================================

// LOCAL TeXT LOG AND MESSAGE QUEUE MUST BE READY BEFORE THE DEVICE IS CONNECTED SINCE THE DEVIE MAY START IMMEDIATELY
// Get the local text log
jvxErrorType
CjvxGenericRS232TextDevice::activate_specific_rs232()
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = JVX_NO_ERROR;
	if (localTxtLog.useLocalTextLog)
	{
		if (_common_set.theToolsHost)
		{
			resL = _common_set.theToolsHost->instance_tool(JVX_COMPONENT_LOCAL_TEXT_LOG, &localTxtLog.theLocalTextLog_obj,
				0, NULL);
			if ((resL == JVX_NO_ERROR) && localTxtLog.theLocalTextLog_obj)
			{
				resL = localTxtLog.theLocalTextLog_obj->request_specialization(reinterpret_cast<jvxHandle**>(
					&localTxtLog.theLocalTextLog), NULL, NULL);
			}
			else
			{
				localTxtLog.useLocalTextLog = false;
				std::cout << "::" << __FUNCTION__ << ": Error: Failed to open logger object, reason: " << jvxErrorType_txt(resL) << std::endl;
			}
		}
		else
		{
			localTxtLog.useLocalTextLog = false;
			std::cout << "::" << __FUNCTION__ << "Error: Failed to open logger object for component< " <<
				_common_set.theModuleName << "> since tools host reference is not vailable." << std::endl;
		}

		jvxSize idSel = jvx_bitfieldSelection2Id(
			CjvxGenericRs232TextDevice_pcg::rs232_sep_token.receiver.value.selection(),
			CjvxGenericRs232TextDevice_pcg::rs232_sep_token.receiver.value.entries.size());

		assert(idSel < JVX_NUM_RS232_SEP_TOKENS);
		runtime.separatorToken_rcv = rs2323text_separator_token[idSel];

		idSel = jvx_bitfieldSelection2Id(
			CjvxGenericRs232TextDevice_pcg::rs232_sep_token.send.value.selection(),
			CjvxGenericRs232TextDevice_pcg::rs232_sep_token.send.value.entries.size());

		assert(idSel < JVX_NUM_RS232_SEP_TOKENS);
		runtime.separatorToken_send = rs2323text_separator_token[idSel];

		if (localTxtLog.useLocalTextLog)
		{

			localTxtLog.theLocalTextLog->initialize(_common_set_min.theHostRef);
			localTxtLog.theLocalTextLog->configure(JVX_LOCAL_TEXT_LOG_CONFIG_SEPARATOR_CHAR, (jvxHandle*)runtime.separatorToken_rcv.c_str());

			localTxtLog.theLocalTextLog->start(localTxtLog.szLocalTextLog);
			localTxtLog.theLocalTextLog->attach_entry(("Starting local text log" + runtime.separatorToken_rcv).c_str());

			jvxCallManagerProperties callMan;
			install_property_reference(callMan, jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<IjvxLocalTextLog>(localTxtLog.theLocalTextLog),
				jPAD("/text_log_collect"));
			//CjvxGenericRs232TextDevice_pcg::associate__rs232_log(static_cast<CjvxProperties*>(this), localTxtLog.theLocalTextLog);
		}
	}
	return res;
}

jvxErrorType
CjvxGenericRS232TextDevice::deactivate_specific_rs232()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (localTxtLog.theLocalTextLog)
	{
		jvxCallManagerProperties callMan;
		uninstall_property_reference(callMan, jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<IjvxLocalTextLog>(localTxtLog.theLocalTextLog),
			jPAD("/text_log_collect"));
		// CjvxGenericRs232TextDevice_pcg::deassociate__rs232_log(static_cast<CjvxProperties*>(this));

		localTxtLog.theLocalTextLog->stop();
		localTxtLog.theLocalTextLog->terminate();
		_common_set.theToolsHost->return_instance_tool(JVX_COMPONENT_LOCAL_TEXT_LOG, localTxtLog.theLocalTextLog_obj, 0, NULL);
	}
	localTxtLog.theLocalTextLog = NULL;
	localTxtLog.theLocalTextLog_obj = NULL;
	return res;
}

// ==========================================================================================================
jvxErrorType
CjvxGenericRS232TextDevice::activate()
{
	jvxErrorType res = CjvxGenericRS232Device::_pre_check_activate();
	if (res == JVX_NO_ERROR)
	{		
		message_queue.sz_mqueue_elements = sizeof(oneMessage_hdr) + sizeof(oneMessageContent);
		jvx_bitFClear(reportEventDataAvail);
		res = CjvxGenericRS232Device::activate();
	}
	return(res);
}

jvxErrorType
CjvxGenericRS232TextDevice::deactivate()
{
	jvxErrorType res = CjvxGenericRS232Device::deactivate();
	if (res == JVX_NO_ERROR)
	{
		jvx_bitFClear(reportEventDataAvail);
	}
	return(res);
}

// ===========================================================================================

jvxErrorType
CjvxGenericRS232TextDevice::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;

	jvxErrorType res = CjvxGenericRS232Device::put_configuration(callConf, processor,
		sectionToContainAllSubsectionsForMe,
		filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState >= JVX_STATE_SELECTED)
		{
			CjvxGenericRs232TextDevice_pcg::put_configuration_all(callConf, processor,
				sectionToContainAllSubsectionsForMe, &warns);
			for (i = 0; i < warns.size(); i++)
			{
				std::cout << "::" << __FUNCTION__ << ": Warning when loading confiugration from file " << filename << ": " << warns[i] << std::endl;
			}
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericRS232TextDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxGenericRS232Device::get_configuration(callConf, processor,
		sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState >= JVX_STATE_SELECTED)
		{
			CjvxGenericRs232TextDevice_pcg::get_configuration_all(callConf, processor,
				sectionWhereToAddAllSubsections);
		}
	}
	return(JVX_NO_ERROR);
}

// ====================================================================================

jvxErrorType 
CjvxGenericRS232TextDevice::translate_message_token_separator(oneMessage_hdr* mess, std::string& txtOut, const std::string& sepToken)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxGenericRS232TextDevice::translate_message_token(oneMessage_hdr* mess, std::string& txtOut)
{
	jvxByte* ct_ptr = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idSel;

	res = translate_message_token_separator(mess, txtOut, runtime.separatorToken_send);
	
	if (res == JVX_ERROR_UNSUPPORTED)
	{
		switch (mess->tp)
		{
		case JVX_GENERIC_RS232_GENERIC_MESSAGE:
			
			ct_ptr = (jvxByte*)mess;
			ct_ptr += sizeof(oneMessage_hdr);
			txtOut = std::string((char*)ct_ptr);
			// Add separator token
			idSel = jvx_bitfieldSelection2Id(CjvxGenericRs232TextDevice_pcg::rs232_sep_token.send.value.selection(), CjvxGenericRs232TextDevice_pcg::rs232_sep_token.send.value.entries.size());
			assert(idSel < JVX_NUM_RS232_SEP_TOKENS);
			txtOut += runtime.separatorToken_send;
			res = JVX_NO_ERROR;
			break;
		default:
			res = JVX_ERROR_UNSUPPORTED;
		}
	}
	return res;
}

/*
 * Send all initial messages on connect - use the message queue to do so!!!
 */
void
CjvxGenericRS232TextDevice::activate_init_messages()
{
	std::string myText = "123-Test";
	add_generic_message_queue(myText);
}

/*
 * Handle incoming data
 */
jvxErrorType
CjvxGenericRS232TextDevice::handle_incoming_data(jvxByte* ptr, jvxSize numRead, jvxSize offset, 
	jvxSize id_port, jvxHandle* addInfo, 
	jvxConnectionPrivateTypeEnum whatsthis, 
	oneMessage_hdr* theMess, jvxSize* idIdentify)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	size_t posi_sep = std::string::npos;

	JVX_LOCK_MUTEX(safeTextBuffer);

	// The first call brings in new data, all following calls do not
	if (numRead)
	{
		receivedTextBuffer += std::string((char*)ptr, numRead);

		if (jvx_bitTest(output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ALLINCOMING_SHIFT))
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::" << __FUNCTION__ << ": Textbuffer with new input is <" << 
					jvx_prepareStringForLogfile(receivedTextBuffer) << ">." << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}
		}
	}

	if (runtime.separatorToken_rcv.empty())
	{
		posi_sep = receivedTextBuffer.size();
	}
	else
	{
		posi_sep = receivedTextBuffer.find(runtime.separatorToken_rcv);
	}
	if (posi_sep == std::string::npos)
	{
		res = JVX_ERROR_POSTPONE;
	}
	else
	{
		//token = receivedText.substr(0, posi_sep + runtime.separatorToken_rcv.size());
		token = receivedTextBuffer.substr(0, posi_sep); // <- separator token removed

		if (jvx_bitTest(output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ALLINCOMING_SHIFT))
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::" << __FUNCTION__ << ": Text token to seperator character is <" << jvx_prepareStringForLogfile(token) 
					<< ">, sep index is " << posi_sep << "." <<  std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}
		}

		receivedTextBuffer = receivedTextBuffer.substr(posi_sep + runtime.separatorToken_rcv.size(), std::string::npos);

		if (jvx_bitTest(output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ALLINCOMING_SHIFT))
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::" << __FUNCTION__ << ": Textbuffer with removed message is <" 
					<< jvx_prepareStringForLogfile(receivedTextBuffer) 
					<< ">." << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}
		}

		if (token.size())
		{
			if (CjvxGenericRs232TextDevice_pcg::rs232_log.text_log_active.value)
			{
				if (localTxtLog.theLocalTextLog)
				{
					localTxtLog.theLocalTextLog->attach_entry((token + runtime.separatorToken_rcv).c_str());
				}
				JVX_LOCK_MUTEX(safeAccessDataAvail);
				jvx_bitSet(reportEventDataAvail, JVX_RS232_TEXTLOG_SHIFT);
				JVX_UNLOCK_MUTEX(safeAccessDataAvail);
			}
		}

		// Here we may process the message on the next higher API level
		res = this->handle_single_received_text_token(token, reinterpret_cast<oneMessage_hdr*>(theMess), idIdentify);
		if (res == JVX_ERROR_ABORT)
		{
			if (jvx_bitTest(output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ERRORS_SHIFT))
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvx_lock_text_log(jvxrtst_bkp);
					jvxrtst << "::" << __FUNCTION__ << ": Failed to match message <" << 
						jvx_prepareStringForLogfile(token) << "> in submodule, error reason: " << jvxErrorType_txt(res) << "." << std::endl;
					jvx_unlock_text_log(jvxrtst_bkp);
				}
			}
		}
		// return res;
	}
	
	JVX_UNLOCK_MUTEX(safeTextBuffer);

	return res;
}

/*
 * In observer, report incoming messages to application on a slower basis than each incoming messages
 */
void 
CjvxGenericRS232TextDevice::report_observer_timeout()
{
	JVX_LOCK_MUTEX(safeAccessDataAvail);
	if (jvx_bitTest(reportEventDataAvail, JVX_RS232_TEXTLOG_SHIFT))
	{
		_report_property_has_changed(
			CjvxGenericRs232TextDevice_pcg::rs232_log.text_log_collect.category,
			CjvxGenericRs232TextDevice_pcg::rs232_log.text_log_collect.globalIdx,
			true, 0, 
			CjvxGenericRs232TextDevice_pcg::rs232_log.text_log_collect.num,
			JVX_COMPONENT_UNKNOWN);
		jvx_bitClear(reportEventDataAvail, JVX_RS232_TEXTLOG_SHIFT);
	}
	JVX_UNLOCK_MUTEX(safeAccessDataAvail);
}

/*
 * Callback to pass a generic text message to the rs232 device. Just add the separator token
 */
JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericRS232TextDevice, cb_rs232_generic_message_set)
{
	std::string txt_mess = CjvxGenericRs232TextDevice_pcg::rs232_transfer.genmessage.value;
	CjvxGenericRs232TextDevice_pcg::rs232_transfer.genmessage.value.clear();
	add_generic_message_queue(txt_mess);
	CjvxGenericRs232TextDevice_pcg::rs232_transfer.genmessage.value.clear();
	return JVX_NO_ERROR;
}

/*
 * Clear local text log
 */
JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericRS232TextDevice, cb_rs232_local_text_log_set)
{
	if (localTxtLog.theLocalTextLog)
	{
		localTxtLog.theLocalTextLog->clear();

		/*
		 This is not required if the requester clears its local buffer
		_report_property_has_changed(
			CjvxGenericRs232TextDevice_pcg::rs232_log.text_log_collect.cat,
			CjvxGenericRs232TextDevice_pcg::rs232_log.text_log_collect.globalIdx,
			true,
			JVX_COMPONENT_UNKNOWN);
		*/
	}
	CjvxGenericRs232TextDevice_pcg::rs232_log.text_log_clear.value = c_false;
	return JVX_NO_ERROR;
}

/*
 * Adde a text message to quque, add
 */
jvxErrorType
CjvxGenericRS232TextDevice::add_generic_message_queue(const std::string& myText, jvxSize uId)
{
	jvxErrorType res = JVX_NO_ERROR;
	oneMessage_hdr* mess;
	oneGenericMessage mess_q;
	jvxByte* bPtr = NULL;
	jvxSize szAlloc = sizeof(oneMessage_hdr);

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bPtr, jvxByte, szAlloc);
	mess = (oneMessage_hdr*)bPtr;
	
	mess->expectResponse = false;
	mess->stat = JVX_GENERIC_RS232_STATUS_INIT;
	mess->tp = JVX_GENERIC_RS232_GENERIC_MESSAGE;
	mess->retrans_cnt = 0;

	JVX_LOCK_MUTEX(safeAccessChannel);
	mess->mId = mIdMessages++;
	JVX_UNLOCK_MUTEX(safeAccessChannel);

	mess->uId = uId; // Only one type: generic message
	mess->sz_elm = szAlloc;
	jvx_message_queue_get_timestamp_us(theMQueue, &mess->timestamp_enter_us);

	mess_q.hdr = *mess;
	mess_q.hdr.sz_elm = sizeof(oneGenericMessage);
	char* loadPtr = (char*)&mess_q;
	loadPtr += sizeof(oneMessage_hdr);
	jvx_fillCharStr(loadPtr, JVX_SIZE_GENERIC_RS232_MESSAGE, myText);

	if (JVX_CHECK_SIZE_SELECTED(mess->uId))
	{
		// Only add messages with valid uIds
		JVX_LOCK_MUTEX(safeAccessChannel);
		auto elmL = mpMessages.find(mess->uId);
		if (elmL == mpMessages.end())
		{
			if (jvx_bitTest(output_flags, JVX_GENERIC_CONNECTION_OUTPUT_TIMING_SHIFT))
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvx_lock_text_log(jvxrtst_bkp);
					jvxrtst << "::" << __FUNCTION__ << "-> T0 => Adding message uid <" << mess->uId << "> - mid <" << 
						mess->mId << "> to list of pending messages." << std::endl;
					jvx_unlock_text_log(jvxrtst_bkp);
				}
			}
			mpMessages[mess->uId] = mess;
		}
		else
		{
			if (jvx_bitTest(output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ERRORS_SHIFT))
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvx_lock_text_log(jvxrtst_bkp);
					jvxrtst << "::" << __FUNCTION__ << "-> T0 => Failed to add message uid <" << mess->uId << "> - mid <" << 
						mess->mId << "> to list of pending messages, a message with the same uid is already pending." << std::endl;
					jvx_unlock_text_log(jvxrtst_bkp);
				}
			}
			res = JVX_ERROR_ALREADY_IN_USE;
		}
		JVX_UNLOCK_MUTEX(safeAccessChannel);
	}

	if (res == JVX_NO_ERROR)
	{
		res = jvx_message_queue_add_message_to_queue(theMQueue, mess->uId, &mess_q, mess_q.hdr.sz_elm, reinterpret_cast<jvxHandle*>(this));
	}
	return res;
}

jvxErrorType
CjvxGenericRS232TextDevice::add_message_queue(oneMessage_hdr* load_plus, jvxHandle* priv, jvxBool expect_response)
{
	jvxErrorType res = JVX_NO_ERROR;
	oneMessage_hdr* mess = NULL;
	//jvxSize szfMessage = sizeof(oneMessage_hdr) + sz_load;

	jvxSize szAlloc = JVX_MAX(sizeof(oneMessage_hdr), load_plus->sz_elm);
	jvxByte * ptrB = NULL;

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrB, jvxByte, szAlloc);
	mess = (oneMessage_hdr*)ptrB;

	memcpy(mess, load_plus, szAlloc);

	mess->expectResponse = expect_response;
	mess->stat = JVX_GENERIC_RS232_STATUS_INIT;
	mess->retrans_cnt = 0;

	JVX_LOCK_MUTEX(safeAccessChannel);
	mess->mId = mIdMessages++;
	JVX_UNLOCK_MUTEX(safeAccessChannel);

	mess->sz_elm = szAlloc;

	jvx_message_queue_get_timestamp_us(theMQueue, &mess->timestamp_enter_us);

	if (JVX_CHECK_SIZE_SELECTED(mess->uId))
	{
		// Only add messages with valid uIds
		JVX_LOCK_MUTEX(safeAccessChannel);
		auto elmL = mpMessages.find(mess->uId);
		if (elmL == mpMessages.end())
		{
			if (jvx_bitTest(output_flags, JVX_GENERIC_CONNECTION_OUTPUT_TIMING_SHIFT))
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvx_lock_text_log(jvxrtst_bkp);
					jvxrtst << "::" << __FUNCTION__ << "-> T0 => Adding message uid <" << mess->uId << "> - mid <" << mess->mId << 
						"> to list of pending messages." << std::endl;
					jvx_unlock_text_log(jvxrtst_bkp);
				}
			}
			mpMessages[mess->uId] = mess;
		}
		else
		{
			if (jvx_bitTest(output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ERRORS_SHIFT))
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvx_lock_text_log(jvxrtst_bkp);
					jvxrtst << "::" << __FUNCTION__ << "-> T0 => Failed to add message uid <" << mess->uId << "> - mid >" << mess->mId << 
						"> to list of pending messages, a message with the same uid is already pending." << std::endl;
					jvx_unlock_text_log(jvxrtst_bkp);
				}
			}
			res = JVX_ERROR_ALREADY_IN_USE;
		}
		JVX_UNLOCK_MUTEX(safeAccessChannel);
	}

	if (res == JVX_NO_ERROR)
	{
		res = jvx_message_queue_add_message_to_queue(theMQueue, load_plus->uId, load_plus, load_plus->sz_elm, reinterpret_cast<jvxHandle*>(this));
	}
	return res;
}

jvxErrorType
CjvxGenericRS232TextDevice::send_direct(oneMessage_hdr* load_plus)
{
	std::string txtOut;
	jvxErrorType res = JVX_NO_ERROR;
	this->translate_message_token(load_plus, txtOut);

	if (jvx_bitTest(output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ALLOUTGOING_SHIFT))
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << "-> sending message direct, uid <" << load_plus->uId << "> - mid <" <<
				load_plus->mId << "> bypassing message queue, context <" << txtOut << ">." << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}
	}

	jvxByte* ptr = (jvxByte*)txtOut.c_str();
	jvxSize sz = txtOut.size();
	theConnectionTool->sendMessage_port(idDevice, ptr, &sz, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
	return res;
}

jvxErrorType
CjvxGenericRS232TextDevice::handle_single_received_text_token(const std::string& token, oneMessage_hdr* commContext, jvxSize* idIdentify)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxGenericRS232TextDevice::post_message_hook(jvxSize idIdentify)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxGenericRS232TextDevice::clear_input_buffer()
{
	if (jvx_bitTest(output_flags, JVX_GENERIC_CONNECTION_OUTPUT_TIMING_SHIFT))
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << "Clearing input text buffer." << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}
	}
	JVX_LOCK_MUTEX(safeTextBuffer);
	receivedTextBuffer.clear();
	JVX_UNLOCK_MUTEX(safeTextBuffer);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxGenericRS232TextDevice::prepare_retransmit()
{
	return clear_input_buffer();
}
