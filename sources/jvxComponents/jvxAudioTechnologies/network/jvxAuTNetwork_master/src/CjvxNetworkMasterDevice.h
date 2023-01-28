#ifndef __CJVXNETWORKMASTERDEVICE_H__
#define __CJVXNETWORKMASTERDEVICE_H__

#include "jvx.h"
#include "jvx_connect_client_st.h"
#include "jvx_message_queue.h"
#include "jvx_threads.h"

#include "jvxAudioTechnologies/CjvxAudioDevice.h"

#include "pcg_exports_device.h"
#include "CjvxLogSignalsToFile.h"

#include "jvx_pdm/jvx_fir_lut.h"

#include "jvx-helpers.h"

#define JVX_MASTER_TIMEOUT 1000

#define JVX_MASTER_NETWORK_NUMBER_ENTRIES_TIMING 5
#define JVX_PROFILE_SEND_CHANNEL_START 1
#define JVX_PROFILE_SEND_CHANNEL_STOP 2
#define JVX_PROFILE_RECEIVE_CHANNEL_ENTER 3
#define JVX_PROFILE_RECEIVE_CHANNEL_LEAVE 4
#define JVX_PROFILE_PROCESS_BUFFER_START 5
#define JVX_PROFILE_PROCESS_BUFFER_STOP 6
#define JVX_PROFILE_EVENT_UNSENT_FRAME 7
#define JVX_PROFILE_EVENT_MISSED_FRAME 8
#define JVX_PROFILE_EVENT_NOT_READY_PROCESSING 9
#define JVX_PROFILE_EVENT_CHANNEL_BUSY 10

#define JVX_TIMING_TO_LOGFILE

#define JVX_LOG_TO_FILE(p0, p1, p2, p3, p4) \
	{ \
		JVX_TRY_LOCK_MUTEX_RESULT_TYPE rr = JVX_TRY_LOCK_MUTEX_SUCCESS; \
		JVX_TRY_LOCK_MUTEX(rr, safeAccess_Otf); \
		if (rr == JVX_TRY_LOCK_MUTEX_SUCCESS) \
		{ \
			if (HjvxDataLogger_Otf::status == JVX_STATE_PROCESSING) \
			{ \
				std::string errD; \
				jvxData tmp = 0; \
				if (dataLogging.fldWrite_timing) \
				{ \
					jvxInt32* bufTimingInfo = (jvxInt32*)dataLogging.fldWrite_timing; \
					bufTimingInfo[0] = p0; \
					bufTimingInfo[1] = p1; \
					bufTimingInfo[2] = p2; \
					bufTimingInfo[3] = p3; \
					bufTimingInfo[4] = p4; \
					p3++; \
					HjvxDataLogger::_add_data_reference(0, 0, (jvxInt32)dataLogging.idxWrite_timing, errD, false); \
					HjvxDataLogger::_fill_height(tmp); \
				} \
			} \
			else \
			{ \
			}  \
			JVX_UNLOCK_MUTEX(safeAccess_Otf); \
		} \
	}

// 					genCommonFramegrabber_device::properties_active_higher.logDataFillHeight.value = tmp * 100; 
//				genCommonFramegrabber_device::properties_active_higher.logDataFillHeight.value = 0.0; 

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#if defined(JVX_OS_WINDOWS) && defined(JVX_WITH_PCAP)
#define JVX_MASTER_AUDIO_SOCKET_TYPE JVX_CONNECT_CLIENT_SOCKET_TYPE_PCAP
#else
#define JVX_MASTER_AUDIO_SOCKET_TYPE JVX_CONNECT_CLIENT_SOCKET_TYPE_TCP
#endif

class CjvxNetworkMasterTechnology;

class CjvxNetworkMasterDevice: public CjvxAudioDevice, 
	public HjvxDataLogger, public HjvxDataLogger_Otf,
	public IjvxDataLogger_Otf_report,
	public IjvxEventLoop_backend,
	public CjvxComponentTempConfig,
	public genNetworkMaster_device
{
private:
	struct oneModeSpecifier
	{
		jvxAdvLinkOperationMode mode;
		jvxDataFormatGroup subformat_in;
		jvxDataFormatGroup subformat_out;
	};

	struct oneField
	{
		jvxByte* ptr;
		jvxByte* shcut;
		jvxSize sz;
	};

	CjvxNetworkMasterTechnology* parentRef;
	jvx_connect_client_if* socketClientIf;
	jvxSize idIface;
	
	jvx_connect_client_callback_type report_link_cbs;
	messageQueueHandler report_mqueue_cbs;

	jvxTimeStampData tStamp;

	jvx_connect_socket_type tp;
	struct
	{
		jvxSize numBytesIn;
		jvxSize numPacketsIn;

		jvxSize fldPrepend;
		jvxSize fldMinSize;
		jvxSize fldMaxSize;

		oneField fldCommunicateHandshake_out;
		oneField fldCommunicateHandshake_in;

		//oneField fldCommunicateReceive_growme;

		jvxState localStateMachineConnect;

		jvxHandle* mQueue_out;

		jvxBool isReady;
		struct
		{
			JVX_MUTEX_HANDLE safeAccessChannel;
			jvxAdvLinkProtocolCommandType command_in_expected;
			jvxAdvLinkProtocolMessagePurpose purpose_in_expected;
			jvxBool inUse;
			jvxSize cnt_received_this_message;
			jvxInt64 expect_further;
		} channel;

		struct
		{
			std::string description;
			jvxInt32 srate;
			jvxInt32 bsize;
			jvxDataFormat format;
			jvxSize inChannels;
			jvxSize outChannels;
			jvxAdvLinkProtocolClockTriggerRole triggerRole;
			jvxAdvLinkProtocolChannelFrameType frameFormat;
			jvxSize idTestMode;
			std::vector<oneModeSpecifier> lstAllowedModes;
			struct
			{
				jvxDataFormatGroup use;				
			} subformat_in;
			struct
			{
				jvxDataFormatGroup use;
			} subformat_out;
		} connectedPartner;

		struct
		{
			jvxBool readBytes;
			jvxBitField channelMask;
			jvxInt32 seq_id;
			jvxSize idxChannel;
		} oneChannelInOut;
	} inConnection;

	struct
	{
		std::vector<int> channelLink_in;
		std::vector<int> channelLink_out;

		jvxTimeStampData theTimestamp;
		jvxInt64 timestamp_previous;
		jvxData deltaT_average_us;
		jvxData deltaT_theory_us;

		jvxSize numberBuffers;

		jvxBitField channelMaskTarget;
		jvxInt64 idSequenceExpected;

		jvxByte*** tx_fields;
		jvxSize tx_size;
		jvxInt32 logId;

		jvxSize idSendIn;
		jvxSize idSendOut;
		jvxSize fillHeight;
		jvxSize numberBuffersMax;
	} inProcessing;

	struct
	{
		jvx_thread_handler theThreadHandler;
		jvxHandle* theThreadHandle;
		//jvxInt32 statusVar;
		//jvxSize circBufferId;
		jvxBool inProcessing;
	} theThread;

	struct
	{
		jvx_fir_lut* pdm2PcmConverter;
		jvxTimeStampData tStamp;
		jvxUInt64 timeOld;
		jvxData deltaT_theo;
	} converters;

#ifdef JVX_TIMING_TO_LOGFILE
	struct
	{
		jvxSize numFramesLog;
		jvxSize timeoutMsec;
	} 	_common_set_timing_log_to_file;

	struct
	{
		CjvxLogSignalsToFile theLogger_;
		std::string logger_prop_prefix;
		JVX_MUTEX_HANDLE safeWriteLog;
		jvxSize idTiming;
	} timingToFile;
#endif

	struct
	{
		IjvxToolsHost* theToolsHost;
		IjvxObject* theDataLogger_obj;
		IjvxDataLogger* theDataLogger_hdl;
		jvxByte** fldWrite_rawaudio;
		jvxSize* idxWrite_rawaudio;
		jvxByte* fldWrite_timing;
		jvxSize idxWrite_timing;
	} dataLogging;

	IjvxEventLoop* theEventLoop;
	jvxBool forceDown;
public:

	CjvxNetworkMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxNetworkMasterDevice();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
								     jvxHandle* sectionToContainAllSubsectionsForMe,
								     const char* filename = "",
								     jvxInt32 lineno = -1)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	// ===================================================================================
	// New linkdata connection interface
	// ===================================================================================

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_trigger(jvxLinkDataDescriptor* theData);
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_trigger(jvxLinkDataDescriptor* theData);
	virtual jvxErrorType JVX_CALLINGCONVENTION start_trigger(jvxLinkDataDescriptor* theData);
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_trigger(jvxLinkDataDescriptor* theData);
    */

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset, 
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
		jvxSize idx_stage, 
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr) override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION report_register_be_commandline(IjvxCommandLine* comLine)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_readout_be_commandline(IjvxCommandLine* comLine) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_event(TjvxEventLoopElement* theQueueElement) override;


	// END OF INTERFACES

	void setReferences(CjvxNetworkMasterTechnology* parentRef, jvxSize idIf, jvx_connect_socket_type tp);

	//jvxErrorType sendMessage_handshake(jvxAdvLinkProtocolCommandType tp);
	//jvxErrorType growMultiPurposeReceiveField(jvxSize newSize);

	jvxErrorType startLink(jvxHandle*, jvxConnectionPrivateTypeEnum whatsthis);
	jvxErrorType stopLink();
	jvxErrorType procedeStateMachine(jvxProtocolHeader* hdr);
	void update_parameters(jvxBool connected);

	// CALLBACKS IP CLIENT
	static jvxErrorType st_callback_link_report_startup_complete(jvxHandle* privateData, jvxSize ifidx);
	static jvxErrorType st_callback_link_report_shutdown_complete(jvxHandle* privateData, jvxSize ifidx);
	static jvxErrorType st_callback_link_provide_data_and_length( jvxHandle* privateData, jvxSize channelid, char**, jvxSize*, jvxSize*, jvxSize ifidx, jvxHandle*, jvxConnectionPrivateTypeEnum whatsthis);
	static jvxErrorType st_callback_link_report_data_and_read( jvxHandle* privateData, jvxSize channelid, char*, jvxSize, jvxSize, jvxSize ifidx, jvxHandle*, jvxConnectionPrivateTypeEnum whatsthis);
	static jvxErrorType st_callback_link_report_unsuccesful_outgoing( jvxHandle* privateData, jvx_connect_fail_reason reason, jvxSize ifidx);
	static jvxErrorType st_callback_link_report_connect_outgoing( jvxHandle* privateData, jvxSize ifidx);
	static jvxErrorType st_callback_link_report_disconnect_outgoing(jvxHandle* privateData, jvxSize ifidx);
	static jvxErrorType st_callback_link_report_error(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx);
	static jvxErrorType st_callback_link_report_specific(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx);

	jvxErrorType ic_callback_link_report_startup_complete(jvxSize ifidx);
	jvxErrorType ic_callback_link_provide_data_and_length(jvxSize chan, char**, jvxSize*, jvxSize* offset, jvxSize ifidx, jvxHandle*, jvxConnectionPrivateTypeEnum whatsthis);
	jvxErrorType ic_callback_link_report_data_and_read(jvxSize chan, char*, jvxSize, jvxSize offset, jvxSize ifidx, jvxHandle*, jvxConnectionPrivateTypeEnum whatsthis);
	jvxErrorType ic_callback_link_report_unsuccesful_outgoing( jvx_connect_fail_reason reason, jvxSize ifidx);
	jvxErrorType ic_callback_link_report_connect_outgoing( jvxSize ifidx);
	jvxErrorType ic_callback_link_report_disconnect_outgoing(jvxSize ifidx);
	jvxErrorType ic_callback_link_report_shutdown_complete(jvxSize ifidx);
	jvxErrorType ic_callback_link_report_error(jvxInt32 errCode, const char* errDescription, jvxSize ifidx);
	jvxErrorType ic_callback_link_report_specific(jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx);

	// CALLBACKS MESSAGE QUEUE
	static jvxErrorType st_callback_message_queue_started(jvxHandle* privateData_queue, jvxInt64 timestamp_us);
	static jvxErrorType st_callback_message_queue_message_in_queue_ready(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_queue, jvxHandle* privateData_message, jvxInt64 timestamp_us);
	static jvxErrorType st_callback_message_queue_stopped(jvxHandle* privateData_queue, jvxInt64 timestamp_us);
	static jvxErrorType st_callback_message_queue_message_in_queue_timeout(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_queue, jvxHandle* privateData_message, jvxInt64 timestamp_us);

	jvxErrorType ic_callback_message_queue_started(jvxInt64 timestamp_us);
	jvxErrorType ic_callback_message_queue_message_in_queue_ready(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_message, jvxInt64 timestamp_us);
	jvxErrorType ic_callback_message_queue_stopped(jvxInt64 timestamp_us);
	jvxErrorType ic_callback_message_queue_message_in_queue_timeout(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_message, jvxInt64 timestamp_us);

	// CALLBACKS THREADS
	static jvxErrorType st_callback_thread_startup(jvxHandle* privateData_thread, jvxInt64 timestamp_us);
	static jvxErrorType st_callback_thread_timer_expired(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	static jvxErrorType st_callback_thread_wokeup(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	static jvxErrorType st_callback_thread_stopped(jvxHandle* privateData_thread, jvxInt64 timestamp_us);

	jvxErrorType ic_callback_thread_startup(jvxInt64 timestamp_us);
	jvxErrorType ic_callback_thread_timer_expired(jvxInt64 timestamp_us);
	jvxErrorType ic_callback_thread_wokeup(jvxInt64 timestamp_us);
	jvxErrorType ic_callback_thread_stopped(jvxInt64 timestamp_us);

	void constructPropertiesAfterConnect();
	void resetPropertiesAfterDisonnect();

	// ======================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION report_prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_start(std::string fName)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_stop(std::string fName)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_postprocess()override;

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
