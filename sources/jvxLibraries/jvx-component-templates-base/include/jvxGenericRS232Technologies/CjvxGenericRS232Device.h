#ifndef __CJVXGENERICRS232DEVICE_H__
#define __CJVXGENERICRS232DEVICE_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxDevice.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"

#include "jvx_threads.h"
#include "jvx_message_queue.h"
#include "pcg_CjvxGenericRS232Device_pcg.h"

#define JVX_SIZE_GENERIC_RS232_MESSAGE 128

#define JVX_GENERIC_RS232_OUTPUT_TIMING_SHIFT 1
#define JVX_GENERIC_RS232_OUTPUT_UNMATCHED_SHIFT 2
#define JVX_GENERIC_RS232_OUTPUT_ALLINCOMING_SHIFT 3
#define JVX_GENERIC_RS232_OUTPUT_ALLOUTGOING_SHIFT 4
#define JVX_GENERIC_RS232_OUTPUT_ERRORS_SHIFT 5
#define JVX_GENERIC_RS232_OUTPUT_SUBMODULE_OFFSET 6
#define JVX_GENERIC_RS232_OUTPUT_TIMEOUT_MESSAGES_COUT 7
#define JVX_GENERIC_RS232_OUTPUT_ALL 0xFFFF

// #define JVX_SHOW_QUICK_TIMING

// Macro to output timing messages about the communication on the std::cout channel
// #define JVX_RS232_OUTPUT_MESSAGE_TIMING 

class CjvxGenericRS232Device : public IjvxDevice, public CjvxDevice,
	public CjvxObject, public IjvxProperties, 
	public CjvxProperties, public IjvxSequencerControl,
	public CjvxSequencerControl, public IjvxConnection_report, 
	public IjvxConfiguration,
	public CjvxGenericRs232Device_pcg
{

protected:

	JVX_MUTEX_HANDLE safeAccessChannel;
	jvxSize mIdMessages;
	jvxBool fullPingPong;

	IjvxObject* theRs232Obj;
	IjvxConnection* theRs232Tool;
	jvxSize idDevice;

	jvxHandle* theObserverThread;
	jvx_thread_handler callbacks;

	jvxHandle* theMQueue;
	messageQueueHandler theMQueueCallbacks;

	jvxBool channel_fully_locked;
	jvxBool i_am_ready;

	jvxSize num_retrans_max;

	jvxBool output_rs232_on_stdcout;

	std::map<jvxSize, oneMessage_hdr*> mpMessages;

	struct
	{
		jvxByte* mem_incoming;
		jvxSize sz_mem_incoming;
	} runtime;

	struct
	{
		jvxSize sz_mqueue_elements;
		jvxSize num_messages_in_queue;
		jvxSize timeout_messages_in_queue_msec;
	} message_queue;

	struct
	{
		jvxSize timeout_observer_msec;
		jvxSize timeout_message_response;
	} observer;

	jvxTimeStampData tStampData;

	jvxCBitField output_flags;
	
#ifdef JVX_SHOW_QUICK_TIMING
	jvxTimeStampData myTStamp;
	jvxTick leave;
	jvxTick enter;
#endif

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxGenericRS232Device(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxGenericRS232Device();

#define JVX_STATEMACHINE_FULL_SKIP_ACTIVATE_DEACTIVATE
#define JVX_STATEMACHINE_FULL_SKIP_SELECT_UNSELECT
#include "codeFragments/simplify/jvxStateMachineFull_simplify.h"
#undef JVX_STATEMACHINE_FULL_SKIP_SELECT_UNSELECT
#undef JVX_STATEMACHINE_FULL_SKIP_ACTIVATE_DEACTIVATE

#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxSequencerControl_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

#include "codeFragments/simplify/jvxDevice_simplify.h"

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "", jvxInt32 lineno = -1)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ========================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION provide_data_and_length(jvxByte**ptr, jvxSize* maxNumCopy, jvxSize* offset, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_data_and_read(jvxByte* ptr, jvxSize numRead, jvxSize offset, jvxSize id_port, jvxHandle* addInfo, 
		jvxConnectionPrivateTypeEnum whatsthis)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxBitField eventMask, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)override;

	void setPortId(int id, IjvxConnection* theRs232Tool, IjvxObject* theRs232Obj);

	/* MESSAGE QUEUE FOR RS-232 MESSAGES*/
	// Static member functions for MESSAGE QUEUE
	static jvxErrorType callback_message_queue_message_in_queue_ready(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_queue, jvxHandle* privateData_message, jvxInt64 timestamp_us);
	static jvxErrorType callback_message_queue_message_in_queue_timeout(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_queue, jvxHandle* privateData_message, jvxInt64 timestamp_us);

	// Non-Static member functions for MESSAGE QUEUE
	jvxErrorType cb_message_queue_message_in_queue_ready(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_message, jvxInt64 timestamp_us);
	jvxErrorType cb_message_queue_message_in_queue_timeout(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_message, jvxInt64 timestamp_us);

	/* THREASD FOR PERIDODIC UPDATES OF RS-232 CONNECTED DEVICE */
	static jvxErrorType cb_thread_timer_expired(jvxHandle* privateData_queue, jvxInt64 timestamp_us, jvxSize* timeout_next);
	jvxErrorType callback_thread_timer_expired(jvxInt64 timestamp_us);

	virtual jvxErrorType remove_data_from_map(oneMessage_hdr* theMess);
	virtual jvxErrorType check_pending_uid(jvxSize uId, jvxSize mTp, oneMessage_hdr** ptr_hdr_ret);
	virtual jvxErrorType clear_send_messages();
	virtual jvxErrorType clear_inout_matching();

	// =======================================================================
	// Higher level API
	// =======================================================================

	// Specific activation/deactivation
	virtual jvxErrorType activate_specific_rs232() = 0;
	virtual jvxErrorType deactivate_specific_rs232() = 0;

	virtual jvxErrorType goodbye_rs232() = 0;

	// From message queue to port
	virtual jvxErrorType translate_message_token(oneMessage_hdr* mess, std::string& txtOut) = 0;

	// Prepare all init messages on port start
	virtual void activate_init_messages() = 0;

	virtual jvxErrorType handle_incoming_data(jvxByte* ptr, jvxSize numRead, jvxSize offset, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis, 
		oneMessage_hdr* theMess, jvxSize* idIdentify) = 0;
	virtual jvxErrorType handle_cancelled_message(oneMessage_hdr* mess, jvxSize* idIdentify) = 0;

	virtual void report_observer_timeout() = 0;

	virtual jvxErrorType post_message_hook(jvxSize idIdentify) = 0;
	virtual jvxErrorType cleared_messages_hook() = 0;

	virtual jvxErrorType prepare_retransmit() = 0;
};

#endif
