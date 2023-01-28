#ifndef __CjvxFlexibleRs232TextDevice_H__
#define __CjvxFlexibleRs232TextDevice_H__

#include "jvxGenericRS232Technologies/CjvxGenericRS232TextDevice.h"

#define NUM_MESSAGES_IN_QUEUE_MAX 10
#define TIMEOUT_IF_MQUEUE_CAN_NOT_BE_HANDLED_MSEC 500

#include "CjvxFlexibleTextControlDevice.h"

class CjvxFlexibleRs232TextDevice : public CjvxGenericRS232TextDevice, public IjvxFlexibleTextControlDevice_interact
{
protected:
	CjvxFlexibleTextControlDevice theControl;
	jvx_propertyReferenceTriple theTrip;
	std::string fname_config;

	jvxBool output_sm_cout;
	jvxSize timeoutobserver_msec ;
	jvxSize timeoutresponse_msec ;
public:

	JVX_CALLINGCONVENTION CjvxFlexibleRs232TextDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxFlexibleRs232TextDevice();

	// ==========================================================================

	virtual jvxErrorType translate_message_token_separator(oneMessage_hdr* mess, std::string& txtOut, const std::string& sepTok) override;

	virtual jvxErrorType handle_single_received_text_token(const std::string& token, oneMessage_hdr* commContext, jvxSize* idIdentify) override;
	virtual jvxErrorType handle_cancelled_message(oneMessage_hdr* commContext, jvxSize* idIdentify) override;
	virtual void activate_init_messages() override;
	virtual void report_observer_timeout() override;

	//virtual jvxErrorType handle_incoming_data(jvxByte* ptr, jvxSize numRead, jvxSize offset, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis,
	//	oneMessage_hdr_load* theMess) override;

	virtual jvxErrorType activate_specific_rs232()override;
	virtual jvxErrorType deactivate_specific_rs232()override;
	virtual jvxErrorType goodbye_rs232()override;
	virtual jvxErrorType post_message_hook(jvxSize idIdentify) override;
	virtual jvxErrorType cleared_messages_hook() override;

	// ==========================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION convert_property_2_txt(const jvx::propertyDescriptor::CjvxPropertyDescriptorCore* descr, jvxSize convert_id,
		std::string& txt_out, jvxSize callback_id, jvxSize num_digits) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION convert_txt_2_property(const jvx::propertyDescriptor::CjvxPropertyDescriptorCore* descr, jvxSize convert_id,
		const std::string& txt_in, const std::string& ref_token, 
		jvxSize callback_id, jvxCBitField convertFinetuning) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_message_queue(oneMessage_hdr* load_plus, jvxHandle* priv, jvxBool expect_response) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION device_check_pending_uid(jvxSize uId, jvxSize mTp, jvxTick* Stamp, oneMessage_hdr** ptr_hdr_ret) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_message(const std::string& mess) override;
	virtual jvxErrorType send_direct(oneMessage_hdr* load_plus) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_callback(jvxFlexibleControlEventType tp, jvxSize callback_id, jvxHandle* spec) override;
	// =================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans) override;
};

#endif