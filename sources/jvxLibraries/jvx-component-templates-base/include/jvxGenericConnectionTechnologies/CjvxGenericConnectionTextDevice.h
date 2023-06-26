#ifndef __CJVXGENERICRS232TEXTDEVICE_H__
#define __CJVXGENERICRS232TEXTDEVICE_H__

#include "jvxGenericRS232Technologies/CjvxGenericRS232Device.h"

// SOme default parameters
#define JVX_RS232_TEXTLOG_SHIFT 0

#include "pcg_CjvxGenericConnectionTextDevice_pcg.h"

#define JVX_SIZE_GENERIC_RS232_MESSAGE 128

class CjvxGenericConnectionTextDevice : public CjvxGenericRS232Device, public CjvxGenericConnectionTextDevice_pcg
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
		char generic_message[JVX_SIZE_GENERIC_RS232_MESSAGE];
	};

	typedef struct
	{
		oneMessage_hdr hdr;
		oneMessageContent ct;
	} oneGenericMessage;

public:
	CjvxGenericConnectionTextDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxGenericConnectionTextDevice();

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "", jvxInt32 lineno = -1)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType add_generic_message_queue(const std::string& txt, jvxSize uId = JVX_SIZE_UNSELECTED);
	virtual jvxErrorType add_message_queue(oneMessage_hdr* load_plus, jvxHandle* priv, jvxBool expect_response);
	virtual jvxErrorType send_direct(oneMessage_hdr* load_plus);

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

	virtual jvxErrorType activate_specific_connection()override;
	virtual jvxErrorType deactivate_specific_connection()override;
	virtual jvxErrorType translate_message_token_separator(oneMessage_hdr* mess, std::string& txtOut, const std::string& sepToken);
	virtual jvxErrorType post_message_hook(jvxSize idIdentify) override;
	virtual jvxErrorType prepare_retransmit()override;
	// =======================================================================

	virtual void activate_init_messages() override;
	virtual jvxErrorType translate_message_token(oneMessage_hdr* mess, std::string& txtOut) override;
	virtual jvxErrorType handle_incoming_data(jvxByte* ptr, jvxSize numRead, jvxSize offset, 
		jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis, 
		oneMessage_hdr* theMess, jvxSize* idIdentify) override;
	virtual void report_observer_timeout() override;
	virtual jvxErrorType handle_single_received_text_token(const std::string& token, oneMessage_hdr* commContext, jvxSize* idIdentify);

	virtual jvxErrorType clear_input_buffer();

	//JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_ALL;
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_rs232_generic_message_set);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_rs232_local_text_log_set);

};

#endif