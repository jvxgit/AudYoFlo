#ifndef __CJVXFLEXIBLETEXTCONTROLDEVICE__H__
#define __CJVXFLEXIBLETEXTCONTROLDEVICE__H__

#include "jvx.h"
#include "common/CjvxProperties.h"

#include "pcg_CjvxFlexibleTextControlDevice.h"

typedef enum
{
	JVX_FLEXIBLE_CONTROL_EVENT_COMMAND_UNKNOWN,
	JVX_FLEXIBLE_CONTROL_EVENT_COMMAND_COMPLETE
} jvxFlexibleControlEventType;


JVX_INTERFACE IjvxFlexibleTextControlDevice_interact
{

public:
	virtual JVX_CALLINGCONVENTION ~IjvxFlexibleTextControlDevice_interact(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION convert_property_2_txt(const jvx::propertyDescriptor::CjvxPropertyDescriptorCore* descr, jvxSize convert_id, std::string& txt_out, jvxSize callback_id, jvxSize num_digits) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION convert_txt_2_property(const jvx::propertyDescriptor::CjvxPropertyDescriptorCore* descr, jvxSize convert_id, const std::string& txt_in, const std::string& ref_token,
		jvxSize callback_id, jvxCBitField fineTuningConvert = ((jvxCBitField)1 << JVX_STRING_PROPERTY_FINE_TUNING_SELECTIONLIST_SELID_SHIFT)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_message_queue(oneMessage_hdr* load_plus, jvxHandle* priv, jvxBool expect_response) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION device_check_pending_uid(jvxSize uId, jvxSize mTp, jvxInt64* Stamp_sent, oneMessage_hdr** ptr_hdr_ret) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION send_direct(oneMessage_hdr* load_plus) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_message(const std::string& mess) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_callback(jvxFlexibleControlEventType tp, jvxSize callback_id, jvxHandle* spec) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION decide_quality(
		jvxRemoteConnectionQuality* quality,
		jvxSize num_incoming_messages,
		jvxSize num_skip_messages,
		jvxSize num_unmatched_messages,
		jvxSize num_unknown_messages) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION inform_identified_unmatched(const char* ruleUnmatched) = 0;
};

class CjvxFlexibleTextControlDevice : public CjvxFlexibleTextControlDevice_genpcg
{
public:
	struct oneDefinitionSeperatorChar
	{
		char sep_define;
		std::string replaceby;
	};

private:

	typedef enum
	{
		JVX_CONTROL_DEVICE_UNDEFINED = 0,
		JVX_CONTROL_DEVICE_TEXT = 1,
		JVX_CONTROL_DEVICE_SEPERATOR = 2,
		JVX_CONTROL_DEVICE_REFERENCE_TOKEN = 3,
		JVX_CONTROL_DEVICE_REFERENCE_PROPERTY = 4,
		JVX_CONTROL_DEVICE_REFERENCE_COMMAND = 5,
		JVX_CONTROL_DEVICE_REFERENCE_SEQUENCE = 6,
		JVX_CONTROL_DEVICE_REFERENCE = 7,
		JVX_CONTROL_DEVICE_WILDCARD = 8,
		JVX_CONTROL_DEVICE_INVALIDATED = 9
	} jvxElementType;

	// ========================================================================
	struct oneElementList
	{
		jvxElementType tp;
		std::string txt;
		std::vector<std::string> txtl;
		jvxSize ref_id;
		jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;
		std::string orig;
		jvxBool expects_response;
		jvxSize conversion_id;
		jvxSize length;
		oneElementList()
		{
			length = JVX_SIZE_UNSELECTED;
			expects_response = true;
			tp = JVX_CONTROL_DEVICE_UNDEFINED;
			ref_id = JVX_SIZE_UNSELECTED;
			conversion_id = JVX_SIZE_UNSELECTED;
		}
	};

	struct reportOneProperty
	{
		std::string convertme;
		std::string txtref;
		jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;
		jvxSize conversion_id;
		reportOneProperty()
		{
			conversion_id = JVX_SIZE_UNSELECTED;
		}
	};

	// ========================================================================
	struct oneDefinitionPropRef
	{
		jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;
		std::string entry;
		std::string prop_name;
		jvxSize conversion_id;
		jvxBool verbose;
		jvxSize length;
		oneDefinitionPropRef()
		{
			verbose = false;
			length = JVX_SIZE_UNSELECTED;
			conversion_id = JVX_SIZE_UNSELECTED;
		};
	};

	// ========================================================================
	struct oneDefinitionCommandOnSet
	{
		jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;
		std::string entry;
		std::vector<std::string> set_commands;
		std::string set_sequence;
		jvxBool verbose;
		oneElementList ref;

		oneDefinitionCommandOnSet()
		{
			verbose = false;
		};
	};

	// ========================================================================
	struct oneDefinitionToken
	{
		std::string token;
		std::string expr;
	};
	
	// ========================================================================
	struct oneDefinitionCommand
	{
		std::string entry;
		std::string command;
		std::list<std::string> responses;
		jvxBool verbose;

		std::list<oneElementList> cmd_ref_lst;
		std::list<std::list<oneElementList>> resp_ref_lst;
		jvxSize id;
		jvxSize callback_id;

		jvxSize pre_timeout_msec;
		jvxSize post_timeout_msec;

		oneDefinitionCommand()
		{
			verbose = false;
			id = 0;
			callback_id = JVX_SIZE_UNSELECTED;
			pre_timeout_msec = 0;
			post_timeout_msec = 0;
		};
	};

	// ========================================================================
	struct oneDefinitionUnmatched
	{
		std::string entry;
		std::string response;
		jvxBool verbose;
		jvxSize callback_id;
		std::list<oneElementList> resp_ref_lst;

		oneDefinitionUnmatched()
		{
			callback_id = JVX_SIZE_UNSELECTED;
			verbose = false;
		};
	};

	// ========================================================================

	struct oneDefinitionSequence
	{
		std::string entry;
		jvxSize id;
		jvxBool verbose;
		std::list<oneElementList> lst_commands;

		jvxBool status_operate;
		jvxBitField returned_response_flags;
		jvxBitField complete_response_flags;
		jvxBitField error_response_flags;

		oneDefinitionSequence()
		{
			verbose = false;
			status_operate = false;

			id = JVX_SIZE_UNSELECTED;
		};
	};

	// ========================================================================

	IjvxFlexibleTextControlDevice_interact* interact;
	std::string fname;
	CjvxProperties* propRef;
	jvxBool verbose_out;

	std::map<std::string, oneDefinitionPropRef> lst_proprefs;
	std::map<std::string, oneDefinitionCommandOnSet> lst_setprops;
	std::map<std::string, oneDefinitionToken> lst_tokens;
	std::map<std::string, oneDefinitionCommand> lst_commands;
	std::map<std::string, oneDefinitionSequence> lst_sequences;
	std::map<std::string, oneDefinitionUnmatched> lst_unmatched;

	std::list<oneDefinitionSeperatorChar> lst_seps;

	std::list<std::string> skip_tokens;
	jvxBool skip_tokens_verbose;

	jvxSize timeout_slow_for_device_msecs;

	// ================================================================
	struct
	{
		jvxSize num_incoming_messages = 0;
		jvxSize num_skip_messages = 0;
		jvxSize num_unmatched_messages = 0;
		jvxSize num_unknown_messages = 0;
		jvxRemoteConnectionQuality quality = jvxRemoteConnectionQuality::JVX_REMOTE_CONNECTION_QUALITY_NOT_CONNECTED;
	} monitor;
	// ================================================================

public:
	JVX_CALLINGCONVENTION CjvxFlexibleTextControlDevice();
	virtual JVX_CALLINGCONVENTION ~CjvxFlexibleTextControlDevice();	

	jvxErrorType JVX_CALLINGCONVENTION init(
		IjvxConfigProcessor* confProc, 
		const std::string& fname_config, 
		IjvxFlexibleTextControlDevice_interact* report, 
		CjvxProperties* propRef,
		std::vector<std::string>& errs, jvxSize* desired_mqueue_size,
		jvxBool verbose_out,
		oneDefinitionSeperatorChar* elms_sep = NULL, 
		jvxSize nums_seps = 0);

	jvxErrorType JVX_CALLINGCONVENTION terminate();

	jvxErrorType JVX_CALLINGCONVENTION property_set(jvxPropertyCategoryType category, 
		jvxSize handleIdx, 
		jvxSize id_prop_command = JVX_SIZE_UNSELECTED);

	void print_ref_list(const std::list<oneElementList>& ref_lst);
	void print();

	jvxErrorType process_incoming_message(const std::string& token, jvxSize* uId, jvxSize* messType, jvxSize* idIdentify);
	jvxErrorType process_cancelled_message(oneMessage_hdr* mess, jvxSize* idIdentify);
	jvxErrorType translate_message_token_separator(oneMessage_hdr* mess, std::string& txtOut, const std::string& sepTken);

	void messages_on_init();
	void messages_on_observe();

	jvxErrorType trigger_send_command(const std::string& seq_name, jvxSize id_prop_override = JVX_SIZE_UNSELECTED,
		jvxSize id_report_single = JVX_SIZE_UNSELECTED);
	jvxErrorType trigger_send_seq(const std::string& seq_name, jvxBool track_response, jvxBool send_direct = false);
	jvxErrorType check_set_status_seq(const std::string& seq_name, jvxBitField* error_bfld, jvxBitField* returned_bfld, jvxBitField* complete_bfld);

	jvxErrorType cleared_messages();
	void report_observer_timeout();

private:
	jvxErrorType parse_input(IjvxConfigProcessor* confProc, std::vector<std::string>& errs);
	jvxErrorType process_input(CjvxProperties* propRef, std::vector<std::string>& errs);
	jvxErrorType link_references(std::vector<std::string>& errs);

	bool check_sep_character(char c, const std::list<oneDefinitionSeperatorChar> & lst_seps, std::string& replace);
	jvxErrorType jvx_parseSequenceIntoToken(std::string input, std::list<CjvxFlexibleTextControlDevice::oneElementList>& args,
		const std::list<CjvxFlexibleTextControlDevice::oneDefinitionSeperatorChar> & lst_seps);
	jvxErrorType jvx_expandTokens(std::list<oneElementList>& args);

	jvxErrorType command_2_string(const std::list<oneElementList>& lst, std::string& txtOut, jvxSize callback_id, jvxSize ext_conversion_id);
	jvxBool parse_text_message(std::string message, const std::list<oneElementList>& lst, jvxSize command_id);

	jvxBool check_verbose(jvxConfigData* datTmp, IjvxConfigProcessor* confProc);
	jvxSize check_callback_id(jvxConfigData* datTmp, IjvxConfigProcessor* confProc);
	jvxSize check_length(jvxConfigData* datTmp, IjvxConfigProcessor* confProc);

	std::string repair_backslash(const std::string& in);

	jvxErrorType inform_identified(oneMessage_hdr* mess, jvxBool error_flag = false, jvxSize* idIdentify = NULL);
};

#endif
