#ifndef __CJVXGENERICCONNECTIONTEXTDEVICE_H__
#define __CJVXGENERICCONNECTIONTEXTDEVICE_H__

#include "common/CjvxProperties.h"
#include "jvxGenericConnectionTechnologies/CjvxGenericConnectionDevice.h"

// SOme default parameters
#define JVX_CONNECTION_TEXTLOG_SHIFT 0

#include "pcg_CjvxGenericConnectionTextDevice_pcg.h"

#define JVX_SIZE_GENERIC_CONNECTION_MESSAGE 128

#include <sstream>

#define JVX_NUM_MESSAGE_SEP_TOKENS 5
static const char* message_separator_token[JVX_NUM_MESSAGE_SEP_TOKENS] =
{
	"\r",
	"\n",
	"\r\n",
	"\r\r\n",
	"\r\n\xFF"
};

template <class T>
class CjvxGenericConnectionTextDevice : public T, public CjvxGenericConnectionTextDevice_pcg
{
	JVX_MUTEX_HANDLE safeTextBuffer;
	std::string receivedTextBuffer;

	JVX_MUTEX_HANDLE safeAccessDataAvail;
	jvxCBitField reportEventDataAvail;

	struct
	{
		jvxSize szLocalTextLog;
		jvxBool useLocalTextLog;

		IjvxObject* theLocalTextLog_obj;
		IjvxLocalTextLog* theLocalTextLog;

	} localTxtLog;

	struct
	{
		std::string separatorToken_rcv;
		std::string separatorToken_send;
	} runtime;

	union oneMessageContent
	{
		char generic_message[JVX_SIZE_GENERIC_CONNECTION_MESSAGE];
	};

	typedef struct
	{
		oneMessage_hdr hdr;
		oneMessageContent ct;
	} oneGenericMessage;

public:
	CjvxGenericConnectionTextDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		T(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
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

	~CjvxGenericConnectionTextDevice()
	{
		JVX_TERMINATE_MUTEX(safeAccessDataAvail);
		JVX_TERMINATE_MUTEX(safeTextBuffer);
	}


	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno) override
	{
		jvxSize i;
		std::vector<std::string> warns;

		jvxErrorType res = T::put_configuration(callConf, processor,
			sectionToContainAllSubsectionsForMe,
			filename, lineno);
		if (res == JVX_NO_ERROR)
		{
			if (T::_common_set_min.theState >= JVX_STATE_SELECTED)
			{
				CjvxGenericConnectionTextDevice_pcg::put_configuration_all(callConf, processor,
					sectionToContainAllSubsectionsForMe, &warns);
				for (i = 0; i < warns.size(); i++)
				{
					std::cout << "::" << __FUNCTION__ << ": Warning when loading confiugration from file " << filename << ": " << warns[i] << std::endl;
				}
			}
		}
		return(JVX_NO_ERROR);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections) override
	{
		jvxErrorType res = T::get_configuration(callConf, processor,
			sectionWhereToAddAllSubsections);
		if (res == JVX_NO_ERROR)
		{
			if (T::_common_set_min.theState >= JVX_STATE_SELECTED)
			{
				CjvxGenericConnectionTextDevice_pcg::get_configuration_all(callConf, processor,
					sectionWhereToAddAllSubsections);
			}
		}
		return(JVX_NO_ERROR);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override
	{
		jvxErrorType res = T::prepare();
		if (res == JVX_NO_ERROR)
		{
			CjvxProperties::_update_properties_on_start();
		}
		return(res);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override
	{
		jvxErrorType res = T::postprocess();
		if (res == JVX_NO_ERROR)
		{
			CjvxProperties::_update_properties_on_stop();
		}
		return(res);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override
	{
		jvxErrorType res = T::select(theOwner);
		if (res == JVX_NO_ERROR)
		{
			CjvxGenericConnectionTextDevice_pcg::init_all();
			CjvxGenericConnectionTextDevice_pcg::allocate_all();
			CjvxGenericConnectionTextDevice_pcg::register_all(static_cast<CjvxProperties*>(this));
			CjvxGenericConnectionTextDevice_pcg::register_callbacks(static_cast<CjvxProperties*>(this),
				cb_message_local_text_log_set, cb_message_generic_message_set, reinterpret_cast<jvxHandle*>(this), NULL);
		}
		return(res);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override
	{
		jvxErrorType res = T::_pre_check_unselect();
		if (res == JVX_NO_ERROR)
		{
			CjvxGenericConnectionTextDevice_pcg::unregister_all(static_cast<CjvxProperties*>(this));
			CjvxGenericConnectionTextDevice_pcg::deallocate_all();
			CjvxGenericConnectionTextDevice_pcg::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);
			T::unselect();
		}
		return(res);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override
	{
		jvxErrorType res = T::_pre_check_activate();
		if (res == JVX_NO_ERROR)
		{
			T::message_queue.sz_mqueue_elements = sizeof(oneMessage_hdr) + sizeof(oneMessageContent);
			jvx_bitFClear(reportEventDataAvail);
			res = T::activate();
		}
		return(res);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override
	{
		jvxErrorType res = T::deactivate();
		if (res == JVX_NO_ERROR)
		{
			jvx_bitFClear(reportEventDataAvail);
		}
		return(res);
	}


	virtual jvxErrorType add_generic_message_queue(const std::string& myText, jvxSize uId = JVX_SIZE_UNSELECTED)
	{
		jvxErrorType res = JVX_NO_ERROR;
		oneMessage_hdr* mess;
		oneGenericMessage mess_q;
		jvxByte* bPtr = NULL;
		jvxSize szAlloc = sizeof(oneMessage_hdr);

		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bPtr, jvxByte, szAlloc);
		mess = (oneMessage_hdr*)bPtr;

		mess->expectResponse = false;
		mess->stat = JVX_GENERIC_CONNECTION_STATUS_INIT;
		mess->tp = JVX_GENERIC_CONNECTION_GENERIC_MESSAGE;
		mess->retrans_cnt = 0;

		T::lock_channel();
		mess->mId = T::mIdMessages++;
		T::unlock_channel();

		mess->uId = uId; // Only one type: generic message
		mess->sz_elm = szAlloc;
		jvx_message_queue_get_timestamp_us(T::theMQueue, &mess->timestamp_enter_us);

		mess_q.hdr = *mess;
		mess_q.hdr.sz_elm = sizeof(oneGenericMessage);
		char* loadPtr = (char*)&mess_q;
		loadPtr += sizeof(oneMessage_hdr);
		jvx_fillCharStr(loadPtr, JVX_SIZE_GENERIC_CONNECTION_MESSAGE, myText);

		if (JVX_CHECK_SIZE_SELECTED(mess->uId))
		{
			// Only add messages with valid uIds
			T::lock_channel();
			auto elmL = T::mpMessages.find(mess->uId);
			if (elmL == T::mpMessages.end())
			{
				if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_TIMING_SHIFT))
				{
					jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
					if (T::jvxrtst_bkp.theTextLogger_hdl && T::jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
					{
						jvx_lock_text_log(T::jvxrtst_bkp, logLev);
						T::jvxrtst << "::" << __FUNCTION__ << "-> T0 => Adding message uid <" << mess->uId << "> - mid <" <<
							mess->mId << "> to list of pending messages." << std::endl;
						jvx_unlock_text_log(T::jvxrtst_bkp);
					}
				}
				T::mpMessages[mess->uId] = mess;
			}
			else
			{
				if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ERRORS_SHIFT))
				{
					jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
					if (T::jvxrtst_bkp.theTextLogger_hdl && T::jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
					{
						jvx_lock_text_log(T::jvxrtst_bkp, logLev);
						T::jvxrtst << "::" << __FUNCTION__ << "-> T0 => Failed to add message uid <" << mess->uId << "> - mid <" <<
							mess->mId << "> to list of pending messages, a message with the same uid is already pending." << std::endl;
						jvx_unlock_text_log(T::jvxrtst_bkp);
					}
				}
				res = JVX_ERROR_ALREADY_IN_USE;
			}
			T::unlock_channel();
		}

		if (res == JVX_NO_ERROR)
		{
			res = jvx_message_queue_add_message_to_queue(T::theMQueue, mess->uId, &mess_q, mess_q.hdr.sz_elm, reinterpret_cast<jvxHandle*>(this));
		}
		return res;
	}

	virtual jvxErrorType add_message_queue(oneMessage_hdr* load_plus, jvxHandle* priv, jvxBool expect_response)
	{
		jvxErrorType res = JVX_NO_ERROR;
		oneMessage_hdr* mess = NULL;
		//jvxSize szfMessage = sizeof(oneMessage_hdr) + sz_load;

		jvxSize szAlloc = JVX_MAX(sizeof(oneMessage_hdr), load_plus->sz_elm);
		jvxByte* ptrB = NULL;

		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrB, jvxByte, szAlloc);
		mess = (oneMessage_hdr*)ptrB;

		memcpy(mess, load_plus, szAlloc);

		mess->expectResponse = expect_response;
		mess->stat = JVX_GENERIC_CONNECTION_STATUS_INIT;
		mess->retrans_cnt = 0;

		T::lock_channel();
		mess->mId = T::mIdMessages++;
		T::unlock_channel();

		mess->sz_elm = szAlloc;

		jvx_message_queue_get_timestamp_us(T::theMQueue, &mess->timestamp_enter_us);

		if (JVX_CHECK_SIZE_SELECTED(mess->uId))
		{
			// Only add messages with valid uIds
			T::lock_channel();
			auto elmL = T::mpMessages.find(mess->uId);
			if (elmL == T::mpMessages.end())
			{
				if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_TIMING_SHIFT))
				{
					jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
					if (T::jvxrtst_bkp.theTextLogger_hdl && T::jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
					{
						jvx_lock_text_log(T::jvxrtst_bkp, logLev);
						T::jvxrtst << "::" << __FUNCTION__ << "-> T0 => Adding message uid <" << mess->uId << "> - mid <" << mess->mId <<
							"> to list of pending messages." << std::endl;
						jvx_unlock_text_log(T::jvxrtst_bkp);
					}
				}
				T::mpMessages[mess->uId] = mess;
			}
			else
			{
				if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ERRORS_SHIFT))
				{
					jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
					if (T::jvxrtst_bkp.theTextLogger_hdl && T::jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
					{
						jvx_lock_text_log(T::jvxrtst_bkp, logLev);
						T::jvxrtst << "::" << __FUNCTION__ << "-> T0 => Failed to add message uid <" << mess->uId << "> - mid >" << mess->mId <<
							"> to list of pending messages, a message with the same uid is already pending." << std::endl;
						jvx_unlock_text_log(T::jvxrtst_bkp);
					}
				}
				res = JVX_ERROR_ALREADY_IN_USE;
			}
			T::unlock_channel();
		}

		if (res == JVX_NO_ERROR)
		{
			res = jvx_message_queue_add_message_to_queue(T::theMQueue, load_plus->uId, load_plus, load_plus->sz_elm, reinterpret_cast<jvxHandle *>(this));
		}
		return res;
	}

	virtual jvxErrorType send_direct(oneMessage_hdr* load_plus)
	{
		std::string txtOut;
		jvxErrorType res = JVX_NO_ERROR;
		this->translate_message_token(load_plus, txtOut);

		if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ALLOUTGOING_SHIFT))
		{
			jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
			if (T::jvxrtst_bkp.theTextLogger_hdl && T::jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
			{
				jvx_lock_text_log(T::jvxrtst_bkp, logLev);
				T::jvxrtst << "::" << __FUNCTION__ << "-> sending message direct, uid <" << load_plus->uId << "> - mid <" <<
					load_plus->mId << "> bypassing message queue, context <" << txtOut << ">." << std::endl;
				jvx_unlock_text_log(T::jvxrtst_bkp);
			}
		}

		jvxByte* ptr = (jvxByte*)txtOut.c_str();
		jvxSize sz = txtOut.size();
		T::theConnectionTool->sendMessage_port(T::idDevice, ptr, &sz, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
		return res;
	}

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

	virtual jvxErrorType activate_specific_connection()override
	{
		jvxErrorType resL = JVX_NO_ERROR;
		jvxErrorType res = JVX_NO_ERROR;
		if (localTxtLog.useLocalTextLog)
		{
			if (T::_common_set.theToolsHost)
			{
				resL = T::_common_set.theToolsHost->instance_tool(JVX_COMPONENT_LOCAL_TEXT_LOG, &localTxtLog.theLocalTextLog_obj,
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
					T::_common_set.theModuleName << "> since tools host reference is not vailable." << std::endl;
			}

			jvxSize idSel = jvx_bitfieldSelection2Id(
				CjvxGenericConnectionTextDevice_pcg::message_sep_token.receiver.value.selection(),
				CjvxGenericConnectionTextDevice_pcg::message_sep_token.receiver.value.entries.size());

			assert(idSel < JVX_NUM_MESSAGE_SEP_TOKENS);
			runtime.separatorToken_rcv = message_separator_token[idSel];

			idSel = jvx_bitfieldSelection2Id(
				CjvxGenericConnectionTextDevice_pcg::message_sep_token.send.value.selection(),
				CjvxGenericConnectionTextDevice_pcg::message_sep_token.send.value.entries.size());

			assert(idSel < JVX_NUM_MESSAGE_SEP_TOKENS);
			runtime.separatorToken_send = message_separator_token[idSel];

			if (localTxtLog.useLocalTextLog)
			{

				localTxtLog.theLocalTextLog->initialize(T::_common_set_min.theHostRef);
				localTxtLog.theLocalTextLog->configure(JVX_LOCAL_TEXT_LOG_CONFIG_SEPARATOR_CHAR, (jvxHandle*)runtime.separatorToken_rcv.c_str());

				localTxtLog.theLocalTextLog->start(localTxtLog.szLocalTextLog);
				localTxtLog.theLocalTextLog->attach_entry(("Starting local text log" + runtime.separatorToken_rcv).c_str());

				jvxCallManagerProperties callMan;
				T::install_property_reference(callMan, jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<IjvxLocalTextLog>(localTxtLog.theLocalTextLog),
											  jPAD("/text_log_collect"));
				//CjvxGenericRs232TextDevice_pcg::associate__rs232_log(static_cast<CjvxProperties*>(this), localTxtLog.theLocalTextLog);
			}
		}
		return res;
	}

	virtual jvxErrorType deactivate_specific_connection()override
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (localTxtLog.theLocalTextLog)
		{
			jvxCallManagerProperties callMan;
			T::uninstall_property_reference(callMan, jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<IjvxLocalTextLog>(localTxtLog.theLocalTextLog),
											jPAD("/text_log_collect"));
			// CjvxGenericRs232TextDevice_pcg::deassociate__rs232_log(static_cast<CjvxProperties*>(this));

			localTxtLog.theLocalTextLog->stop();
			localTxtLog.theLocalTextLog->terminate();
			T::_common_set.theToolsHost->return_instance_tool(JVX_COMPONENT_LOCAL_TEXT_LOG, localTxtLog.theLocalTextLog_obj, 0, NULL);
		}
		localTxtLog.theLocalTextLog = NULL;
		localTxtLog.theLocalTextLog_obj = NULL;
		return res;
	}

	virtual jvxErrorType translate_message_token_separator(oneMessage_hdr* mess, std::string& txtOut, const std::string& sepToken)
	{
		return JVX_ERROR_UNSUPPORTED;
	}

	virtual jvxErrorType post_message_hook(jvxSize idIdentify) override
	{
		return JVX_NO_ERROR;
	}

	virtual jvxErrorType prepare_retransmit()override
	{
		return clear_input_buffer();
	}

	// =======================================================================

	virtual void activate_init_messages() override
	{
		std::string myText = "123-Test";
		add_generic_message_queue(myText);
	}

	virtual jvxErrorType translate_message_token(oneMessage_hdr* mess, std::string& txtOut) override
	{
		jvxByte* ct_ptr = NULL;
		jvxErrorType res = JVX_NO_ERROR;
		jvxSize idSel;

		res = translate_message_token_separator(mess, txtOut, runtime.separatorToken_send);

		if (res == JVX_ERROR_UNSUPPORTED)
		{
			switch (mess->tp)
			{
			case JVX_GENERIC_CONNECTION_GENERIC_MESSAGE:

				ct_ptr = (jvxByte*)mess;
				ct_ptr += sizeof(oneMessage_hdr);
				txtOut = std::string((char*)ct_ptr);
				// Add separator token
				idSel = jvx_bitfieldSelection2Id(CjvxGenericConnectionTextDevice_pcg::message_sep_token.send.value.selection(),
					CjvxGenericConnectionTextDevice_pcg::message_sep_token.send.value.entries.size());
				assert(idSel < JVX_NUM_MESSAGE_SEP_TOKENS);
				txtOut += runtime.separatorToken_send;
				res = JVX_NO_ERROR;
				break;
			default:
				res = JVX_ERROR_UNSUPPORTED;
			}
		}
		return res;
	}

	virtual jvxErrorType handle_incoming_data(jvxByte* ptr, jvxSize numRead, jvxSize offset,
			jvxSize id_port, jvxHandle* addInfo,
			jvxConnectionPrivateTypeEnum whatsthis,
			oneMessage_hdr* theMess, jvxSize* idIdentify) override
	{
		jvxErrorType res = JVX_NO_ERROR;
		std::string token;
		size_t posi_sep = std::string::npos;

		JVX_LOCK_MUTEX(safeTextBuffer);

		// The first call brings in new data, all following calls do not
		if (numRead)
		{
			receivedTextBuffer += std::string((char*)ptr, numRead);

			if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ALLINCOMING_SHIFT))
			{
				jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
				if (T::jvxrtst_bkp.theTextLogger_hdl && T::jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					jvx_lock_text_log(T::jvxrtst_bkp, logLev);
					T::jvxrtst << "::" << __FUNCTION__ << ": Textbuffer with new input is <" <<
						jvx_prepareStringForLogfile(receivedTextBuffer) << ">." << std::endl;
					jvx_unlock_text_log(T::jvxrtst_bkp);
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

			if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ALLINCOMING_SHIFT))
			{
				jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
				if (T::jvxrtst_bkp.theTextLogger_hdl && T::jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					jvx_lock_text_log(T::jvxrtst_bkp, logLev);
					T::jvxrtst << "::" << __FUNCTION__ << ": Text token to seperator character is <" << jvx_prepareStringForLogfile(token)
						<< ">, sep index is " << posi_sep << "." << std::endl;
					jvx_unlock_text_log(T::jvxrtst_bkp);
				}
			}

			receivedTextBuffer = receivedTextBuffer.substr(posi_sep + runtime.separatorToken_rcv.size(), std::string::npos);

			if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ALLINCOMING_SHIFT))
			{
				jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
				if (T::jvxrtst_bkp.theTextLogger_hdl && T::jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					jvx_lock_text_log(T::jvxrtst_bkp, logLev);
					T::jvxrtst << "::" << __FUNCTION__ << ": Textbuffer with removed message is <"
						<< jvx_prepareStringForLogfile(receivedTextBuffer)
						<< ">." << std::endl;
					jvx_unlock_text_log(T::jvxrtst_bkp);
				}
			}

			if (token.size())
			{
				if (CjvxGenericConnectionTextDevice_pcg::message_log.text_log_active.value)
				{
					if (localTxtLog.theLocalTextLog)
					{
						localTxtLog.theLocalTextLog->attach_entry((token + runtime.separatorToken_rcv).c_str());
					}
					JVX_LOCK_MUTEX(safeAccessDataAvail);
					jvx_bitSet(reportEventDataAvail, JVX_CONNECTION_TEXTLOG_SHIFT);
					JVX_UNLOCK_MUTEX(safeAccessDataAvail);
				}
			}

			// Here we may process the message on the next higher API level
			res = this->handle_single_received_text_token(token, reinterpret_cast<oneMessage_hdr*>(theMess), idIdentify);
			if (res == JVX_ERROR_ABORT)
			{
				if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_ERRORS_SHIFT))
				{
					jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
					if (T::jvxrtst_bkp.theTextLogger_hdl && T::jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
					{
						jvx_lock_text_log(T::jvxrtst_bkp, logLev);
						T::jvxrtst << "::" << __FUNCTION__ << ": Failed to match message <" <<
							jvx_prepareStringForLogfile(token) << "> in submodule, error reason: " << jvxErrorType_txt(res) << "." << std::endl;
						jvx_unlock_text_log(T::jvxrtst_bkp);
					}
				}
			}
			// return res;
		}

		JVX_UNLOCK_MUTEX(safeTextBuffer);

		return res;
	}

	virtual void report_observer_timeout() override
	{
		JVX_LOCK_MUTEX(safeAccessDataAvail);
		if (jvx_bitTest(reportEventDataAvail, JVX_CONNECTION_TEXTLOG_SHIFT))
		{
			T::_report_property_has_changed(
				CjvxGenericConnectionTextDevice_pcg::message_log.text_log_collect.category,
				CjvxGenericConnectionTextDevice_pcg::message_log.text_log_collect.globalIdx,
				true, 0,
				CjvxGenericConnectionTextDevice_pcg::message_log.text_log_collect.num,
				JVX_COMPONENT_UNKNOWN);
			jvx_bitClear(reportEventDataAvail, JVX_CONNECTION_TEXTLOG_SHIFT);
		}
		JVX_UNLOCK_MUTEX(safeAccessDataAvail);
	}

	virtual jvxErrorType handle_single_received_text_token(const std::string& token, oneMessage_hdr* commContext, jvxSize* idIdentify)
	{
		return JVX_NO_ERROR;
	}

	virtual jvxErrorType clear_input_buffer()
	{
		if (jvx_bitTest(T::output_flags, JVX_GENERIC_CONNECTION_OUTPUT_TIMING_SHIFT))
		{
			jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
			if (T::jvxrtst_bkp.theTextLogger_hdl && T::jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
			{
				jvx_lock_text_log(T::jvxrtst_bkp, logLev);
				T::jvxrtst << "::" << __FUNCTION__ << "Clearing input text buffer." << std::endl;
				jvx_unlock_text_log(T::jvxrtst_bkp);
			}
		}
		JVX_LOCK_MUTEX(safeTextBuffer);
		receivedTextBuffer.clear();
		JVX_UNLOCK_MUTEX(safeTextBuffer);
		return JVX_NO_ERROR;
	}

	//JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_ALL;
	JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL_H(CjvxGenericConnectionTextDevice<T>, cb_message_generic_message_set)
	{
		std::string txt_mess = CjvxGenericConnectionTextDevice_pcg::message_transfer.genmessage.value;
		CjvxGenericConnectionTextDevice_pcg::message_transfer.genmessage.value.clear();
		add_generic_message_queue(txt_mess);
		CjvxGenericConnectionTextDevice_pcg::message_transfer.genmessage.value.clear();
		return JVX_NO_ERROR;
	}

	JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL_H(CjvxGenericConnectionTextDevice<T>, cb_message_local_text_log_set)
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
		CjvxGenericConnectionTextDevice_pcg::message_log.text_log_clear.value = c_false;
		return JVX_NO_ERROR;
	}

};

#endif