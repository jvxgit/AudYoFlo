#include "jvx_dsp_base.h"
#include "CjvxNetworkMasterDevice.h"
#include "CjvxNetworkMasterTechnology.h"

#define JVX_NUM_MESSAGES_QUEUE 10
#define JVX_TIMEOUT_MQ_MAX 50000

#define LOGFILE_NUMBER_FRAMES_MEMORY 10
#define LOGFILE_STOP_TIMEOUT_MS 5000

#include <iostream>

#include "jvx_fdesign_I_octConfig.h"
#include "jvx_fdesign_II_octConfig.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif


CjvxNetworkMasterDevice::CjvxNetworkMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	parentRef = NULL;
	socketClientIf = NULL;;
	idIface = JVX_SIZE_UNSELECTED;
	
	JVX_INITIALIZE_MUTEX(timingToFile.safeWriteLog);

	report_link_cbs.common.callback_provide_data_and_length = st_callback_link_provide_data_and_length;
	report_link_cbs.common.callback_report_data_and_read = st_callback_link_report_data_and_read;
	report_link_cbs.common.callback_report_shutdown_complete = st_callback_link_report_shutdown_complete;
	report_link_cbs.common.callback_report_error = st_callback_link_report_error;
	report_link_cbs.common.callback_report_startup_complete = st_callback_link_report_startup_complete;
	report_link_cbs.common.callback_report_specific = st_callback_link_report_specific; 
	
	report_link_cbs.callback_report_connect_outgoing = st_callback_link_report_connect_outgoing;
	report_link_cbs.callback_report_disconnect_outgoing = st_callback_link_report_disconnect_outgoing;
	report_link_cbs.callback_report_unsuccesful_outgoing = st_callback_link_report_unsuccesful_outgoing;

	report_mqueue_cbs.callback_message_queue_started = st_callback_message_queue_started;
	report_mqueue_cbs.callback_message_queue_message_in_queue_ready = st_callback_message_queue_message_in_queue_ready;
	report_mqueue_cbs.callback_message_queue_stopped = st_callback_message_queue_stopped;
	report_mqueue_cbs.callback_message_queue_message_in_queue_timeout = st_callback_message_queue_message_in_queue_timeout;

	theThread.theThreadHandler.callback_thread_startup = st_callback_thread_startup;
	theThread.theThreadHandler.callback_thread_stopped = st_callback_thread_stopped;
	theThread.theThreadHandler.callback_thread_timer_expired = st_callback_thread_timer_expired;
	theThread.theThreadHandler.callback_thread_wokeup = st_callback_thread_wokeup;

	JVX_INITIALIZE_MUTEX(inConnection.channel.safeAccessChannel);

	//theSelector.theType.tp = JVX_COMPONENT_AUDIO_NODE;
	//theSelector.theType.slotid = JVX_SIZE_UNSELECTED;
	//theSelector.theType.slotsubid = JVX_SIZE_UNSELECTED;

	//theSelector.thePurpose = JVX_PROCESSING_PURPOSE_AUDIO_IO_N0_7;
	//_common_set_dataprocessor_selector.entries.push_back(theSelector);

#ifdef JVX_TIMING_TO_LOGFILE
	_common_set_timing_log_to_file.numFramesLog = 100;
	_common_set_timing_log_to_file.timeoutMsec = 5000;
#endif
	inProcessing.logId = 0;
	oneModeSpecifier oneMode;

	oneMode.mode = JVX_PROTOCOL_ADVLINK_MODE_AUDIO_CODED_OPUS;
	oneMode.subformat_in = JVX_DATAFORMAT_GROUP_AUDIO_CODED_OPUS_NO_CONTAINER;
	oneMode.subformat_out = JVX_DATAFORMAT_GROUP_AUDIO_CODED_OPUS_NO_CONTAINER;
	inConnection.connectedPartner.lstAllowedModes.push_back(oneMode);

	oneMode.mode = JVX_PROTOCOL_ADVLINK_MODE_PDM_IN_PCM_OUT;
	oneMode.subformat_in = JVX_DATAFORMAT_GROUP_AUDIO_PDM;
	oneMode.subformat_out = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
	inConnection.connectedPartner.lstAllowedModes.push_back(oneMode);
	oneMode.mode = JVX_PROTOCOL_ADVLINK_MODE_PCM_IN_PCM_OUT;
	oneMode.subformat_in = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
	oneMode.subformat_out = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
	inConnection.connectedPartner.lstAllowedModes.push_back(oneMode);

	theEventLoop = NULL;
}

CjvxNetworkMasterDevice::~CjvxNetworkMasterDevice()
{
	JVX_TERMINATE_MUTEX(inConnection.channel.safeAccessChannel);
	JVX_TERMINATE_MUTEX(timingToFile.safeWriteLog);
	CjvxAudioDevice::terminate();
}

void 
CjvxNetworkMasterDevice::setReferences(CjvxNetworkMasterTechnology* par, jvxSize idIf, jvx_connect_socket_type tp)
{
	this->parentRef = par;
	this->idIface = idIf;
	this->tp = tp;
}

jvxErrorType
CjvxNetworkMasterDevice::activate()
{
	jvxErrorType res = CjvxAudioDevice::activate();
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		init_tc(_common_set_min.theHostRef);
		
		_common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_EVENTLOOP, reinterpret_cast<jvxHandle**>(&theEventLoop));

		genNetworkMaster_device::init__integrateiplink__properties_running();
		genNetworkMaster_device::allocate__integrateiplink__properties_running();
		genNetworkMaster_device::register__integrateiplink__properties_running(static_cast<CjvxProperties*>(this));

		genNetworkMaster_device::init__properties_active();
		genNetworkMaster_device::allocate__properties_active();
		genNetworkMaster_device::register__properties_active(static_cast<CjvxProperties*>(this));

		genNetworkMaster_device::init__timinglogtofile__properties_logtofile();
		genNetworkMaster_device::allocate__timinglogtofile__properties_logtofile();
//		genNetworkMaster_device::timinglogtofile.properties_logtofile.logFilePrefix.value = "jvxTimingLog";
		genNetworkMaster_device::register__timinglogtofile__properties_logtofile(static_cast<CjvxProperties*>(this), "jvxAuTMasterNetwork");

		resetPropertiesAfterDisonnect();

		// Connection partner localhost by default
		genNetworkMaster_device::integrateiplink.properties_running.ipAddressServer.value = "localhost";

		// Port of connected server
		genNetworkMaster_device::integrateiplink.properties_running.portServer.value = 56789;

		genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x1;

		genNetworkMaster_device::timinglogtofile.properties_logtofile.groupTitle.value = "Log Timing Events To File";
		genNetworkMaster_device::timinglogtofile.properties_logtofile.logFilePrefix.value = "jvxLogTiming";

		switch (tp)
		{
#ifdef JVX_WITH_PCAP
		case JVX_CONNECT_SOCKET_TYPE_PCAP:
			genNetworkMaster_device::integrateiplink.properties_running.connectionType.value = "RAW PCAP";
			break;
#endif
		case JVX_CONNECT_SOCKET_TYPE_UDP:
			genNetworkMaster_device::integrateiplink.properties_running.connectionType.value = "UDP";
			break;
		default:
			assert(0);
		}

		inConnection.isReady = false;

		dataLogging.theToolsHost = NULL;
		dataLogging.theDataLogger_hdl = NULL;
		dataLogging.theDataLogger_obj = NULL;
		dataLogging.fldWrite_rawaudio = NULL;
		dataLogging.fldWrite_timing = NULL;
		dataLogging.idxWrite_rawaudio = NULL;
		dataLogging.idxWrite_timing = JVX_SIZE_UNSELECTED;
		
		if (_common_set_min.theHostRef)
		{
			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&dataLogging.theToolsHost));
		}

		if (dataLogging.theToolsHost)
		{
			resL = dataLogging.theToolsHost->instance_tool(JVX_COMPONENT_DATALOGGER, &dataLogging.theDataLogger_obj, 0, NULL);
			if ((resL == JVX_NO_ERROR) && dataLogging.theDataLogger_obj)
			{
				resL = dataLogging.theDataLogger_obj->request_specialization(reinterpret_cast<jvxHandle**>(&dataLogging.theDataLogger_hdl), NULL, NULL);
			}
			HjvxDataLogger_Otf::initialize(static_cast<IjvxDataLogger_Otf_report*>(this));
		}

		forceDown = false;

		// Reload configuration if available. Must be very last step since actions may be triggered 
		put_configuration_tc(static_cast<IjvxConfiguration*>(this),
			(_common_set_min.theDescription + genNetworkMaster_device::integrateiplink.properties_running.connectionType.value).c_str());

	}
	return(res);
}

jvxErrorType
CjvxNetworkMasterDevice::select(IjvxObject* theOwner)
{
	jvxErrorType res = CjvxAudioDevice::select( theOwner);
	if (res == JVX_NO_ERROR)
	{

		if (parentRef)
		{
			parentRef->informDeviceSelected();
		}
	}
	return res;
}

jvxErrorType
CjvxNetworkMasterDevice::unselect()
{
	jvxErrorType res = CjvxAudioDevice::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{

		if (parentRef)
		{
			parentRef->informDeviceUnselected();
		}
		res = CjvxAudioDevice::_unselect();
	}
	return res;
}

jvxErrorType
CjvxNetworkMasterDevice::deactivate()
{
	jvxErrorType res = CjvxAudioDevice::_pre_check_deactivate();
	if(res == JVX_NO_ERROR)
	{
		// Important: shutdown the socket in a clean way!

		if (this->socketClientIf)
		{
			forceDown = true;
			this->stopLink();
		}

		if (dataLogging.theDataLogger_obj)
		{
			HjvxDataLogger_Otf::terminate();

			dataLogging.theToolsHost->return_instance_tool(JVX_COMPONENT_DATALOGGER, dataLogging.theDataLogger_obj, 0, NULL);
		}
		dataLogging.theDataLogger_hdl = NULL;
		dataLogging.theDataLogger_obj = NULL;
		dataLogging.fldWrite_rawaudio = NULL;
		dataLogging.fldWrite_timing = NULL;
		dataLogging.idxWrite_rawaudio = NULL;
		dataLogging.idxWrite_timing = JVX_SIZE_UNSELECTED;

		if (dataLogging.theToolsHost)
		{
			_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(dataLogging.theToolsHost));
			dataLogging.theToolsHost = NULL;
		}

		// Store current configuration 
		get_configuration_tc(static_cast<IjvxConfiguration*>(this),
			(_common_set_min.theDescription + genNetworkMaster_device::integrateiplink.properties_running.connectionType.value).c_str());

		genNetworkMaster_device::unregister__timinglogtofile__properties_logtofile(static_cast<CjvxProperties*>(this));
		genNetworkMaster_device::deallocate__timinglogtofile__properties_logtofile();

		genNetworkMaster_device::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genNetworkMaster_device::deallocate__properties_active();

		genNetworkMaster_device::unregister__integrateiplink__properties_running(static_cast<CjvxProperties*>(this));
		genNetworkMaster_device::deallocate__integrateiplink__properties_running();

		theEventLoop->event_clear(JVX_SIZE_UNSELECTED, NULL, static_cast<IjvxEventLoop_backend*>(this));
		_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_EVENTLOOP, reinterpret_cast<jvxHandle*>(theEventLoop));
		theEventLoop = NULL;

		terminate_tc();
		CjvxAudioDevice::deactivate();

	}
	return(res);
}

jvxErrorType 
CjvxNetworkMasterDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	std::string func;
	std::vector<std::string> args;
	jvx_connect_spec_pcap pcap_init;
	jvx_connect_spec_udp udp_init;

	jvxHandle* ptrStart = NULL;
	jvxConnectionPrivateTypeEnum fldTypeStart = JVX_CONNECT_PRIVATE_ARG_TYPE_NONE;

	jvxErrorType res = CjvxAudioDevice::set_property(
		callGate,rawPtr, ident, trans);
	if(res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		if (
			(genNetworkMaster_device::integrateiplink.properties_running.logToFile.globalIdx == propId) &&
			(genNetworkMaster_device::integrateiplink.properties_running.logToFile.category == category))
		{
			if (genNetworkMaster_device::integrateiplink.properties_running.logToFile.value == c_true)
			{
				HjvxDataLogger_Otf::activate("CjvxNetworkMaster_log", "jdl");
			}
			else
			{
				HjvxDataLogger_Otf::deactivate();
			}
		}

		if(
			(genNetworkMaster_device::integrateiplink.properties_running.specifyCommand.globalIdx == propId) &&
			(genNetworkMaster_device::integrateiplink.properties_running.specifyCommand.category == category))
		{
			std::string command = genNetworkMaster_device::integrateiplink.properties_running.specifyCommand.value;
			genNetworkMaster_device::integrateiplink.properties_running.specifyCommand.value = "ok";
			jvxErrorType resL = jvx_parseCommandIntoToken(command, func, args);
			if(resL == JVX_NO_ERROR)
			{
				if(func == "connect")
				{
					if (this->socketClientIf == NULL)
					{
						switch (tp)
						{
#ifdef JVX_WITH_PCAP
						case JVX_CONNECT_SOCKET_TYPE_PCAP:
					
							resL = jvx_stringToMacAddress(genNetworkMaster_device::integrateiplink.properties_running.ipAddressServer.value, pcap_init.dest_mac);
							if (resL == JVX_ERROR_INVALID_ARGUMENT)
							{
								genNetworkMaster_device::integrateiplink.properties_running.ipAddressServer.value = "40:00:00:00:00:00";
								resL = jvx_stringToMacAddress(genNetworkMaster_device::integrateiplink.properties_running.ipAddressServer.value, pcap_init.dest_mac);
							}
							pcap_init.timeoutConnect_msec = genNetworkMaster_device::integrateiplink.properties_running.pcapConnectTout_msec.value;

							// Make sure only packets are 
							pcap_init.receive_mode = 0;
							jvx_bitSet(pcap_init.receive_mode, JVX_PCAP_RECEIVE_FILTER_INCOMING_MAC_SHIFT);

							ptrStart = &pcap_init;
							fldTypeStart = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_PCAP;

							break;
#endif
						case JVX_CONNECT_SOCKET_TYPE_UDP:

							udp_init.connectToPort = genNetworkMaster_device::integrateiplink.properties_running.portServer.value;
							udp_init.family = JVX_IP_V4;
							udp_init.localPort = -1;
							udp_init.targetName = genNetworkMaster_device::integrateiplink.properties_running.ipAddressServer.value;
							ptrStart = &udp_init;
							fldTypeStart = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_UDP;
							break;
						default: 
							assert(0);
						}

						if (resL != JVX_NO_ERROR)
						{
							genNetworkMaster_device::integrateiplink.properties_running.specifyCommand.value = "Could not transform " + genNetworkMaster_device::integrateiplink.properties_running.ipAddressServer.value + " into a valid connection target.";
							res = JVX_ERROR_INVALID_SETTING;
						}
						else
						{
							forceDown = false;
							res = startLink(ptrStart, fldTypeStart);
							assert(res == JVX_NO_ERROR);
						}
					}
				}
				if(func == "disconnect")
				{
					if(this->socketClientIf != NULL)
					{
						this->stopLink();
					}				
				}
			}
		}

		if((genNetworkMaster_device::properties_active.ratesselection.globalIdx == propId) &&
			(genNetworkMaster_device::properties_active.ratesselection.category == category))
		{
			CjvxAudioDevice::properties_active.samplerate.value = inConnection.connectedPartner.srate;
		}

		if((genNetworkMaster_device::properties_active.sizesselection.globalIdx == propId) &&
			(genNetworkMaster_device::properties_active.sizesselection.category == category))
		{
			CjvxAudioDevice::properties_active.buffersize.value = inConnection.connectedPartner.bsize;
		}
	}
	return(res);
}

jvxErrorType
CjvxNetworkMasterDevice::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxApiString fldStr;
	std::string command;
	std::vector<std::string> warns;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGateLoc;
	if (callConf->call_purpose != JVX_CONFIGURATION_PURPOSE_CONFIG_ON_COMPONENT_STARTSTOP)
	{
		res = CjvxAudioDevice::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno);
	}

	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		genNetworkMaster_device::put_configuration__integrateiplink__properties_running(
			callConf, processor, sectionToContainAllSubsectionsForMe,
			&warns);

		genNetworkMaster_device::put_configuration__properties_active(
			callConf, processor, sectionToContainAllSubsectionsForMe,
			&warns);

		if(genNetworkMaster_device::integrateiplink.properties_running.autoConnect.value == c_true)
		{
			command = "connect();";
			fldStr.assign_const(command.c_str(), command.size());

			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
				genNetworkMaster_device::integrateiplink.properties_running.specifyCommand.globalIdx,
				genNetworkMaster_device::integrateiplink.properties_running.specifyCommand.category);
			jvx::propertyDetail::CjvxTranferDetail trans(true);
			callGateLoc.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
			this->set_property(callGateLoc, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
		}
	}
	return(res);
}


jvxErrorType 
CjvxNetworkMasterDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warns;
	jvxErrorType res = JVX_NO_ERROR;
	if (callConf->call_purpose != JVX_CONFIGURATION_PURPOSE_CONFIG_ON_COMPONENT_STARTSTOP)
	{
		res = CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	}
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		genNetworkMaster_device::get_configuration__integrateiplink__properties_running(callConf, processor, sectionWhereToAddAllSubsections);
		genNetworkMaster_device::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);
	}
	return(res);
}

/*
jvxErrorType
CjvxNetworkMasterDevice::growMultiPurposeReceiveField(jvxSize newSize)
{
	if(newSize != inConnection.fldCommunicateReceive_growme.sz)
	{
		if(inConnection.fldCommunicateReceive_growme.ptr)
		{
			JVX_SAFE_DELETE_FLD(inConnection.fldCommunicateReceive_growme.ptr, jvxByte);
			inConnection.fldCommunicateReceive_growme.ptr = NULL;
		}
		inConnection.fldCommunicateReceive_growme.sz = newSize;

		JVX_DSP_SAFE_ALLOCATE_FIELD(inConnection.fldCommunicateReceive_growme.ptr, jvxByte, inConnection.fldCommunicateReceive_growme.sz);
		assert(inConnection.fldCommunicateReceive_growme.sz <= inConnection.fldMaxSize);
	}
	return(JVX_NO_ERROR);
}
*/

/*	
jvxErrorType
CjvxNetworkMasterDevice::sendMessage_handshake(jvxAdvLinkProtocolCommandType tp)
{
	JVX_LOCK_MUTEX(inConnection.safeAccessChannel);
	if(inConnection.
	jvxAdvLinkProtocolCommandType 
						theHeader = (jvxProtocolHeader*)inConnection.fldCommunicatePrepare.shcut;
						theHeader->paketsize = inConnection.fldCommunicatePrepare.sz;
						theHeader->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_DESCR);
						theHeader->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
						sz = theHeader->paketsize;

						inConnection.expect.command = JVX_PROTOCOL_ADVLINK_DESCR;
						inConnection.expect.type = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
						inConnection.expect.firstPartPackage = true;

						res = jvx_connect_client_send_st(this->socketClientIf, inConnection.fldCommunicatePrepare.ptr, &sz, NULL);
						assert(res == JVX_NO_ERROR);
	return(JVX_ERROR_UNSUPPORTED);
}
						*/


jvxErrorType
CjvxNetworkMasterDevice::startLink(jvxHandle* pcap_init, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	inConnection.numBytesIn = 0;
	inConnection.numPacketsIn = 0;

	res = jvx_connect_client_interface_initialize_st(
		parentRef->socketClientFam,
		&this->socketClientIf, 	
		&report_link_cbs,
		reinterpret_cast<jvxHandle*>(this),
		JVX_MASTER_TIMEOUT, 
		this->idIface);
	
	if(res == JVX_NO_ERROR)
	{
		genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x2;

		res = jvx_connect_client_field_constraint_st(
			this->socketClientIf,
			&inConnection.fldPrepend,
			&inConnection.fldMinSize,
			&inConnection.fldMaxSize);

		inConnection.localStateMachineConnect = JVX_STATE_NONE;
	}

	if(res == JVX_NO_ERROR)
	{
		// =====================================================================
		// Allocate all memory
		// =====================================================================
		inConnection.fldCommunicateHandshake_out.ptr = NULL;
		inConnection.fldCommunicateHandshake_out.sz = inConnection.fldPrepend + sizeof(jvxAdvProtocolUnion);
		inConnection.fldCommunicateHandshake_out.sz = JVX_MAX(inConnection.fldCommunicateHandshake_out.sz, inConnection.fldMinSize);
		assert(inConnection.fldCommunicateHandshake_out.sz <= inConnection.fldMaxSize);

		JVX_DSP_SAFE_ALLOCATE_FIELD(inConnection.fldCommunicateHandshake_out.ptr, jvxByte, inConnection.fldCommunicateHandshake_out.sz);
		memset(inConnection.fldCommunicateHandshake_out.ptr, 0, inConnection.fldCommunicateHandshake_out.sz);
		inConnection.fldCommunicateHandshake_out.shcut = inConnection.fldCommunicateHandshake_out.ptr + inConnection.fldPrepend;

		// =====================================================================

		/* Allocate buffer to hold just the minimal header */
		inConnection.fldCommunicateHandshake_in.ptr = NULL;
		inConnection.fldCommunicateHandshake_in.shcut = NULL;
		inConnection.fldCommunicateHandshake_in.sz = inConnection.fldCommunicateHandshake_out.sz;

		JVX_DSP_SAFE_ALLOCATE_FIELD(inConnection.fldCommunicateHandshake_in.ptr, jvxByte, inConnection.fldCommunicateHandshake_in.sz);
		memset(inConnection.fldCommunicateHandshake_in.ptr, 0, inConnection.fldCommunicateHandshake_in.sz);
		inConnection.fldCommunicateHandshake_in.shcut = inConnection.fldCommunicateHandshake_in.ptr + inConnection.fldPrepend;

		inConnection.channel.inUse = false;
		inConnection.channel.expect_further = -1;

		// =====================================================================
		/*
		inConnection.fldCommunicateReceive_growme.ptr = NULL;
		inConnection.fldCommunicateReceive_growme.shcut= NULL;
		inConnection.fldCommunicateReceive_growme.sz = 0;
		jvxSize newSize = inConnection.fldMinSize - (inConnection.fldPrepend + sizeof(jvxProtocolHeader));
		newSize = JVX_MAX(newSize, 0);
		if(newSize > 0)
		{
			growMultiPurposeReceiveField(newSize);
		}
		*/

		// Start message queue incoming messages
		// Start message queue incoming messages
		res = jvx_message_queue_initialize(&inConnection.mQueue_out, JVX_MESSAGE_QUEUE_STATIC_OBJECTS, &report_mqueue_cbs, 
			this, JVX_NUM_MESSAGES_QUEUE, JVX_TIMEOUT_MQ_MAX, inConnection.fldCommunicateHandshake_out.sz);
		assert(res == JVX_NO_ERROR);

		res = jvx_message_queue_start(inConnection.mQueue_out);
		assert(res == JVX_NO_ERROR);

		// Wait for message queue to come up
		//JVX_SLEEP_MS(200);

		// This is the last state change before running the socket thread. All higher states are
		// dealt with in the socket thread - which we do not protect
		genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x4;

		// Connect the socket
		res = jvx_connect_client_interface_connect_st(socketClientIf, pcap_init, whatsthis);


	}
	return(res);
}

jvxErrorType
CjvxNetworkMasterDevice::stopLink()
{
	jvxErrorType res = JVX_NO_ERROR;

	genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x40;
	jvx_connect_client_trigger_shutdown_st(this->socketClientIf);
	jvx_connect_client_optional_wait_for_shutdown_st(this->socketClientIf);
	jvx_connect_client_interface_terminate_st(this->socketClientIf);

	jvx_message_queue_stop(inConnection.mQueue_out);
	jvx_message_queue_terminate(inConnection.mQueue_out);

	//growMultiPurposeReceiveField(0);

	JVX_DSP_SAFE_DELETE_FIELD(inConnection.fldCommunicateHandshake_out.ptr);
	inConnection.fldCommunicateHandshake_out.ptr = NULL;
	inConnection.fldCommunicateHandshake_out.sz = 0;
	inConnection.fldCommunicateHandshake_out.shcut = 0;

	JVX_DSP_SAFE_DELETE_FIELD(inConnection.fldCommunicateHandshake_in.ptr);
	inConnection.fldCommunicateHandshake_in.ptr = NULL;
	inConnection.fldCommunicateHandshake_in.sz = 0;
	inConnection.fldCommunicateHandshake_in.shcut = NULL;

	this->socketClientIf = NULL;
	return(res);
}

jvxErrorType 
CjvxNetworkMasterDevice::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioDevice::is_ready(suc, reasonIfNot);
	if(res == JVX_NO_ERROR)
	{
		assert(suc);
		if(*suc)
		{
			if(!inConnection.isReady)
			{
				*suc = false;
				if (reasonIfNot)
				{
					reasonIfNot->assign("Network Master: Device not Connected.");
				}
			}
		}
	}
	return(res);
}

jvxErrorType 
CjvxNetworkMasterDevice::prepare()
{
	jvxSize cnt = 0;
	jvxSize i, j;
	jvxErrorType res = _prepare_lock();
	if (res == JVX_NO_ERROR)
	{

		CjvxProperties::_update_properties_on_start();
		this->HjvxDataLogger_Otf::start();

		// Set the processing parameters
		_inproc.buffersize = CjvxAudioDevice_genpcg::properties_active.buffersize.value;
		_inproc.samplerate = CjvxAudioDevice_genpcg::properties_active.samplerate.value;
		_inproc.inChannelsSelectionMask = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection();
		_inproc.outChannelsSelectionMask = CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection();
		_inproc.minChanCntInput = 0;
		_inproc.minChanCntOutput = 0;
		_inproc.maxChanCntOutput = 0;
		_inproc.maxChanCntOutput = 0;
		_inproc.format = (jvxDataFormat)CjvxAudioDevice_genpcg::properties_active.format.value;

		genNetworkMaster_device::integrateiplink.properties_running.numberLostFrames.value = 0;
		genNetworkMaster_device::integrateiplink.properties_running.numberLostFramesRemote.value = 0;
		genNetworkMaster_device::integrateiplink.properties_running.numberUnsentFrames.value = 0;

		genNetworkMaster_device::profiling.deltaT_converters.value = 0;
		genNetworkMaster_device::profiling.deltaT_processing.value = 0;
		genNetworkMaster_device::profiling.deltaT_buffers.value = 0;

		genNetworkMaster_device::integrateiplink.properties_running.profile_load_convert.value = 0;
		genNetworkMaster_device::integrateiplink.properties_running.profile_load_process.value = 0;

		// Channel mapping in case not all channes are in use
		cnt = 0;
		for (i = 0; i < CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size(); i++)
		{
			if (jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i))
			{
				inProcessing.channelLink_in.push_back(JVX_SIZE_INT(cnt));
				_inproc.minChanCntInput++;
			}
			else
			{
				inProcessing.channelLink_in.push_back(-1);
			}
			_inproc.maxChanCntInput++;
		}
		_inproc.numInputs = JVX_SIZE_INT32(_inproc.minChanCntInput);


		if (inConnection.connectedPartner.subformat_in.use == JVX_DATAFORMAT_GROUP_AUDIO_PDM)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD(converters.pdm2PcmConverter, jvx_fir_lut, _inproc.numInputs);
			for (i = 0; i < _inproc.numInputs; i++)
			{
				jvx_pdm_fir_lut_initConfig(&converters.pdm2PcmConverter[i]);
				converters.pdm2PcmConverter[i].prm_init.expSz = 3;

				jvx_pdm_fir_lut_prepare(&converters.pdm2PcmConverter[i]);

				converters.pdm2PcmConverter[i].prm_sync.ptr_lookup = JVX_FIR_PDM_DEC_TABLES_d2_ll128_fac9_os16;
				JVX_SAFE_NEW_FLD(converters.pdm2PcmConverter[i].prm_sync.circ_buf, jvxUInt16, JVX_FIR_PDM_DEC_NUM_TABLES_d2_ll128_fac9_os16);
				for (j = 0; j < JVX_FIR_PDM_DEC_NUM_TABLES_d2_ll128_fac9_os16; j++)
				{
					converters.pdm2PcmConverter[i].prm_sync.circ_buf[j] = 43690; // This value is a sequence of 1010101010...10 to start with a "zero" memory in PDM
				}
				converters.pdm2PcmConverter[i].prm_sync.ptr_revAddr = JVX_FIR_PDM_DEC_BITREVERSE_d2_ll128_fac9_os16;
				assert((JVX_FIR_PDM_DEC_NUM_ENTRIES_d2_ll128_fac9_os16*JVX_FIR_PDM_DEC_NUM_TABLES_d2_ll128_fac9_os16) ==
					converters.pdm2PcmConverter[i].derived.lLookup16Bit);
				assert(JVX_FIR_PDM_DEC_NUM_TABLES_d2_ll128_fac9_os16 == converters.pdm2PcmConverter[i].derived.lCirc16Bit);
			}
		}
		cnt = 0;
		for (i = 0; i < CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size(); i++)
		{
			if (jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i))
			{
				inProcessing.channelLink_out.push_back(JVX_SIZE_INT(cnt));
				_inproc.minChanCntOutput++;
			}
			else
			{
				inProcessing.channelLink_out.push_back(-1);
			}
			_inproc.maxChanCntOutput++;

		}
		_inproc.numOutputs = JVX_SIZE_INT32(_inproc.minChanCntOutput);
		_unlock_state();
	}
	return(res);
}

jvxErrorType 
CjvxNetworkMasterDevice::start()
{
	jvxErrorType res = CjvxAudioDevice::start();
	if(res == JVX_NO_ERROR)
	{
		// 
	}
	return(res);
}

jvxErrorType
CjvxNetworkMasterDevice::stop()
{
	jvxProtocolHeader* theHeaderOut = NULL;
	jvxSize sz;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = CjvxAudioDevice::stop(); // <- this should lead to no more messages from IP link since state is switched
	if(res == JVX_NO_ERROR)
	{
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxNetworkMasterDevice::postprocess()
{
	jvxSize i,j;
	jvxSize numNotDeallocated = 0;
	jvxErrorType res = _postprocess_lock();
	jvxErrorType resL = JVX_NO_ERROR; 
	jvxSize cnt = 0;

	if(res == JVX_NO_ERROR)
	{
		this->HjvxDataLogger_Otf::stop();

		CjvxProperties::_update_properties_on_stop();

		this->_unlock_state();
	}

	assert(res == JVX_NO_ERROR);
	return(res);
}

			// END OF INTERFACES

void
CjvxNetworkMasterDevice::constructPropertiesAfterConnect()
{
	jvxSize i;
	jvxCBitField reportflags;

	resetPropertiesAfterDisonnect();

	this->_lock_properties_local();

	for(i = 0; i < inConnection.connectedPartner.inChannels; i++)
	{
		CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(
			inConnection.connectedPartner.description +" #" + jvx_size2String(i));

		jvx_bitSet(CjvxAudioDevice::properties_active.inputchannelselection.value.selection(), i);
		CjvxAudioDevice::properties_active.inputchannelselection.value.exclusive = (jvxBitField)(-1);
	}

	for(i = 0; i < inConnection.connectedPartner.outChannels; i++)
	{
		CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(
			inConnection.connectedPartner.description +" #" + jvx_size2String(i));

		jvx_bitSet(CjvxAudioDevice::properties_active.outputchannelselection.value.selection(), i);
		CjvxAudioDevice::properties_active.outputchannelselection.value.exclusive = (jvxBitField)(-1);
	}

	genNetworkMaster_device::properties_active.sizesselection.value.entries.push_back(jvx_int2String(inConnection.connectedPartner.bsize));
	genNetworkMaster_device::properties_active.sizesselection.value.selection() = 0x1;
	genNetworkMaster_device::properties_active.sizesselection.isValid = true;
	CjvxAudioDevice::properties_active.buffersize.value = inConnection.connectedPartner.bsize;

	genNetworkMaster_device::properties_active.ratesselection.value.entries.push_back(jvx_int2String(inConnection.connectedPartner.srate));
	genNetworkMaster_device::properties_active.ratesselection.value.selection() = 1;
	genNetworkMaster_device::properties_active.ratesselection.isValid = true;
	CjvxAudioDevice::properties_active.samplerate.value = inConnection.connectedPartner.srate;

	CjvxAudioDevice::properties_active.formatselection.value.entries.push_back(jvxDataFormat_txt(inConnection.connectedPartner.format));
	CjvxAudioDevice::properties_active.formatselection.value.selection() = 1;
	CjvxAudioDevice::properties_active.formatselection.isValid = true;

	this->_unlock_properties_local();

	// Once we are here, we may start processing
	genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x10;

	jvx_bitFClear(reportflags);
	jvx_bitSet(reportflags, JVX_REPORT_REQUEST_RECONFIGURE_SHIFT);
//	jvx_bitSet(reportflags, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	this->_report_command_request(reportflags);

	// The next will request an update of the chain
	updateDependentVariables_lock(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);
}

void
CjvxNetworkMasterDevice::resetPropertiesAfterDisonnect()
{
	this->_lock_properties_local();

	CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
	CjvxAudioDevice::properties_active.inputchannelselection.value.selection() = 0;
	CjvxAudioDevice::properties_active.inputchannelselection.value.exclusive = 0;

	CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
	CjvxAudioDevice::properties_active.outputchannelselection.value.selection() = 0;
	CjvxAudioDevice::properties_active.outputchannelselection.value.exclusive = 0;

	CjvxAudioDevice::properties_active.formatselection.value.selection() = 0;
	CjvxAudioDevice::properties_active.formatselection.isValid = false;
	CjvxAudioDevice::properties_active.formatselection.value.entries.clear();

	genNetworkMaster_device::properties_active.sizesselection.value.selection() = 0;
	genNetworkMaster_device::properties_active.sizesselection.isValid = false;
	genNetworkMaster_device::properties_active.sizesselection.value.entries.clear();
	CjvxAudioDevice::properties_active.buffersize.value = 1024;

	genNetworkMaster_device::properties_active.ratesselection.value.selection() = 0;
	genNetworkMaster_device::properties_active.ratesselection.isValid = false;
	genNetworkMaster_device::properties_active.ratesselection.value.entries.clear();
	CjvxAudioDevice::properties_active.samplerate.value = 48000;


	CjvxAudioDevice::properties_active.format.value = JVX_DATAFORMAT_DATA;

	this->_unlock_properties_local();
}

jvxErrorType 
CjvxNetworkMasterDevice::procedeStateMachine(jvxProtocolHeader* hdr)
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxProtocolHeader* theHeaderOut = NULL;
	jvxAdvProtocolTransferP* theHeaderOutPlus = NULL;
	jvxSize sz;
	jvxAdvProtocolDescriptionP* hdrInD = NULL;
	jvxAdvProtocolTransferP* hdrInT = NULL;

	jvxAdvLinkProtocolCommandType inCommand = (jvxAdvLinkProtocolCommandType) (hdr->purpose & 0x3FFF);
	jvxAdvLinkProtocolMessagePurpose inPurpose = (jvxAdvLinkProtocolMessagePurpose) (hdr->purpose & 0xC00);
	jvxBool sendOut = false;
	jvxBool clearChannel = false;
	jvxBool triggerChannel = false;
	jvxCBitField reportflags = 0;

	theHeaderOut = (jvxProtocolHeader*)inConnection.fldCommunicateHandshake_out.shcut;

	if(inCommand == JVX_PROTOCOL_ADVLINK_ERROR_RESET)
	{
		if(inPurpose == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)
		{
			// This should be the first incoming message
			// Start DSP side
			theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_STATUS);
			theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
			
			sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
			sz = JVX_MAX(sz, inConnection.fldMinSize);
			theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

			clearChannel = true;
			sendOut = true;
			triggerChannel = true;
		}
		else
		{
			// Oops..
			assert(0);
		}
	}
	else if(inCommand == JVX_PROTOCOL_ADVLINK_STATUS)
	{
		// Status connected partner is reported
		if(inPurpose == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)
		{
			hdrInT = (jvxAdvProtocolTransferP*)hdr;
			inConnection.localStateMachineConnect = (jvxState)hdrInT->first_transfer_load;

			assert(inConnection.localStateMachineConnect == JVX_STATE_NONE);
				
			theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_INIT);
			theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;

			sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
			sz = JVX_MAX(sz, inConnection.fldMinSize);
			theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

			clearChannel = true;
			sendOut = true;
			triggerChannel = true;
		}
		else
		{
			assert(0);
		}
	}
	else if(
		(inConnection.channel.command_in_expected == inCommand)&&
		(inConnection.channel.purpose_in_expected == inPurpose))
	{
		switch(inConnection.localStateMachineConnect)
		{
		case JVX_STATE_NONE:
			if(inConnection.channel.command_in_expected == JVX_PROTOCOL_ADVLINK_INIT)
			{
				inConnection.localStateMachineConnect = JVX_STATE_INIT;

				theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_DESCR);
				theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
				
				sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
				sz = JVX_MAX(sz, inConnection.fldMinSize);
				theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

				clearChannel = true;
				sendOut = true;
			}
			else
			{
				assert(0);
			}
			break;
		case JVX_STATE_INIT:
			switch(inConnection.channel.command_in_expected)
			{
			case JVX_PROTOCOL_ADVLINK_DESCR:

				hdrInD = (jvxAdvProtocolDescriptionP*)hdr;
				inConnection.connectedPartner.description = (const char*)hdrInD->description;

				theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_MASK);
				theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
				
				sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
				sz = JVX_MAX(sz, inConnection.fldMinSize);
				theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

				clearChannel = true;
				sendOut = true;

				break;
			case JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_MASK:
				hdrInT = (jvxAdvProtocolTransferP*)hdr;
				inConnection.connectedPartner.triggerRole = (jvxAdvLinkProtocolClockTriggerRole)hdrInT->first_transfer_load;

				theHeaderOutPlus = (jvxAdvProtocolTransferP*)theHeaderOut;
				if (JVX_CHECK_SIZE_UNSELECTED(jvx_bitfieldSelection2Id(genNetworkMaster_device::integrateiplink.properties_running.selectionHardwareModeActive.value.selection(),
					genNetworkMaster_device::integrateiplink.properties_running.selectionHardwareModeActive.value.entries.size())))
				{
					jvx_bitSet(genNetworkMaster_device::integrateiplink.properties_running.selectionHardwareModeActive.value.selection(), 0);
				}
				
				theHeaderOutPlus->header.purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_SPECIFY_MODE);
				theHeaderOutPlus->header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
				inConnection.connectedPartner.idTestMode = 0;

				while (inConnection.connectedPartner.idTestMode < inConnection.connectedPartner.lstAllowedModes.size())
				{
					if (jvx_bitTest(genNetworkMaster_device::integrateiplink.properties_running.selectionHardwareModeActive.value.selection(), inConnection.connectedPartner.idTestMode))
					{
						break;
					}
					inConnection.connectedPartner.idTestMode++;
				}

				assert(inConnection.connectedPartner.idTestMode < inConnection.connectedPartner.lstAllowedModes.size());
				theHeaderOutPlus->first_transfer_load = (jvxUInt32)inConnection.connectedPartner.lstAllowedModes[inConnection.connectedPartner.idTestMode].mode;

				sz = sizeof(jvxAdvProtocolTransferP) + inConnection.fldPrepend;
				sz = JVX_MAX(sz, inConnection.fldMinSize);
				theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

				clearChannel = true;
				sendOut = true;

				break;
			case JVX_PROTOCOL_ADVLINK_SPECIFY_MODE:
				hdrInT = (jvxAdvProtocolTransferP*)hdr;
				if (hdrInT->first_transfer_load == 1)
				{
					inConnection.connectedPartner.subformat_in.use = inConnection.connectedPartner.lstAllowedModes[inConnection.connectedPartner.idTestMode].subformat_in;
					inConnection.connectedPartner.subformat_out.use = inConnection.connectedPartner.lstAllowedModes[inConnection.connectedPartner.idTestMode].subformat_out;

					theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_ACTIVATE);
					theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
					
					sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
					sz = JVX_MAX(sz, inConnection.fldMinSize);
					theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

					_lock_properties_local();
					CjvxProperties::_update_property_access_type(
						JVX_PROPERTY_ACCESS_READ_ONLY, genNetworkMaster_device::integrateiplink.properties_running.selectionHardwareModeActive);
					_unlock_properties_local();
				}
				else
				{
					inConnection.connectedPartner.idTestMode++; // The previous mode was not accepted, procede to next
					while (inConnection.connectedPartner.idTestMode < inConnection.connectedPartner.lstAllowedModes.size())
					{
						if (jvx_bitTest(genNetworkMaster_device::integrateiplink.properties_running.selectionHardwareModeActive.value.selection(), inConnection.connectedPartner.idTestMode))
						{
							break;
						}
						inConnection.connectedPartner.idTestMode++;
					}
					assert(inConnection.connectedPartner.idTestMode < inConnection.connectedPartner.lstAllowedModes.size());

					theHeaderOutPlus = (jvxAdvProtocolTransferP*)theHeaderOut;

					theHeaderOutPlus->header.purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_SPECIFY_MODE);
					theHeaderOutPlus->header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
					theHeaderOutPlus->first_transfer_load = (jvxUInt32)inConnection.connectedPartner.lstAllowedModes[inConnection.connectedPartner.idTestMode].mode;

					sz = sizeof(jvxAdvProtocolTransferP) + inConnection.fldPrepend;
					sz = JVX_MAX(sz, inConnection.fldMinSize);
					theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);
				}

				clearChannel = true;
				sendOut = true;

				break;
			case JVX_PROTOCOL_ADVLINK_ACTIVATE:
				inConnection.localStateMachineConnect = JVX_STATE_ACTIVE;
				
				theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_SRATE);
				theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
				
				sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
				sz = JVX_MAX(sz, inConnection.fldMinSize);
				theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

				clearChannel = true;
				sendOut = true;

				break;
			case JVX_PROTOCOL_ADVLINK_TERM:
				inConnection.localStateMachineConnect = JVX_STATE_NONE;	

				break;
			default:
				assert(0);
			}
			break;
		case JVX_STATE_ACTIVE:
			switch(inConnection.channel.command_in_expected)
			{
			case JVX_PROTOCOL_ADVLINK_SRATE:
				
				hdrInT = (jvxAdvProtocolTransferP*)hdr;
				inConnection.connectedPartner.srate = JVX_SIZE_INT32(hdrInT->first_transfer_load);

				theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_BSIZE);
				theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
				
				sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
				sz = JVX_MAX(sz, inConnection.fldMinSize);
				theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

				clearChannel = true;
				sendOut = true;

				break;
			case JVX_PROTOCOL_ADVLINK_BSIZE:
				hdrInT = (jvxAdvProtocolTransferP*)hdr;
				inConnection.connectedPartner.bsize = JVX_SIZE_INT32(hdrInT->first_transfer_load);

				theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_FORMAT);
				theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
				
				sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
				sz = JVX_MAX(sz, inConnection.fldMinSize);
				theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

				clearChannel = true;
				sendOut = true;

				break;

			case JVX_PROTOCOL_ADVLINK_FORMAT:

				hdrInT = (jvxAdvProtocolTransferP*)hdr;
				inConnection.connectedPartner.format = (jvxDataFormat)hdrInT->first_transfer_load;
				
				theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_SLAVE_TO_MASTER);
				theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
				
				sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
				sz = JVX_MAX(sz, inConnection.fldMinSize);
				theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

				clearChannel = true;
				sendOut = true;

				break;
			case JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_SLAVE_TO_MASTER:

				hdrInT = (jvxAdvProtocolTransferP*)hdr;
				inConnection.connectedPartner.inChannels = (jvxSize)hdrInT->first_transfer_load;

				theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_MASTER_TO_SLAVE);
				theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
				
				sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
				sz = JVX_MAX(sz, inConnection.fldMinSize);
				theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

				clearChannel = true;
				sendOut = true;
		

				break;
			case JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_MASTER_TO_SLAVE:

				hdrInT = (jvxAdvProtocolTransferP*)hdr;
				inConnection.connectedPartner.outChannels = (jvxSize)hdrInT->first_transfer_load;

				theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_CHANNEL_FRAME_TYPE);
				theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
				
				sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
				sz = JVX_MAX(sz, inConnection.fldMinSize);
				theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

				clearChannel = true;
				sendOut = true;

				break;

			case JVX_PROTOCOL_ADVLINK_CHANNEL_FRAME_TYPE:

				hdrInT = (jvxAdvProtocolTransferP*)hdr;
				inConnection.connectedPartner.frameFormat = (jvxAdvLinkProtocolChannelFrameType)hdrInT->first_transfer_load;
				inConnection.isReady = true;

				clearChannel = true;

				// If we are here, create channels, samplerates etc.
				if (theEventLoop)
				{
					TjvxEventLoopElement tElm;
					tElm.target_be = static_cast<IjvxEventLoop_backend*>(this);
					//tElm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
					tElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
					tElm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
					tElm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC;
					/*
					tElm.autoDeleteOnProcess = false;
					tElm.delta_t = JVX_SIZE_UNSELECTED;
					tElm.message_id = JVX_SIZE_UNSELECTED;
					tElm.oneHdlBlock = NULL;
					tElm.origin_fe = NULL;
					tElm.param = NULL;
					tElm.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;
					tElm.priv_be = NULL;
					tElm.priv_fe = NULL;
					tElm.rescheduleEvent = false;
					tElm.thread_id = 0;
					tElm.timestamp_run_us = 0;
					*/
					theEventLoop->event_schedule(&tElm, &resL, NULL);
				}
				else
				{
					std::cout << "Warning, this implementation is not thread safe!" << std::endl;
					update_parameters(true);
				}

				break;
			case JVX_PROTOCOL_ADVLINK_START:
				clearChannel = true;
				inConnection.localStateMachineConnect = JVX_STATE_PROCESSING;
				inProcessing.logId = 0;
				genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x20;
				_report_property_has_changed(genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.category,
					genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.globalIdx,
					true, 0, 1, JVX_COMPONENT_UNKNOWN);
				this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
				break;

			case JVX_PROTOCOL_ADVLINK_DEACTIVATE:
				
				inConnection.localStateMachineConnect = JVX_STATE_INIT;
				
				theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_TERM);
				theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
				
				sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
				sz = JVX_MAX(sz, inConnection.fldMinSize);
				theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

				clearChannel = true;
				sendOut = true;

				break;



			case JVX_PROTOCOL_ADVLINK_STOP:
				std::cout << "Stop ethernet transport in active." << std::endl;
				inConnection.localStateMachineConnect = JVX_STATE_ACTIVE;
				genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x10;
				_report_property_has_changed(genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.category,
					genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.globalIdx,
					true, 0, 1, JVX_COMPONENT_UNKNOWN);
				this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
				clearChannel = true;
				break;
				

			default: 
				assert(0);
			}
			break;
		case JVX_STATE_COMPLETE:
			
			switch (inConnection.channel.command_in_expected)
			{
			case JVX_PROTOCOL_ADVLINK_STOP:
				std::cout << "Stop ethernet transport in complete state." << std::endl;
				inConnection.localStateMachineConnect = JVX_STATE_ACTIVE;
				genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x10;
				_report_property_has_changed(genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.category,
					genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.globalIdx,
					true, 0, 1, JVX_COMPONENT_UNKNOWN);
				this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
				clearChannel = true;
				break;


			default:
				assert(0);
			}
			break;

		case JVX_STATE_PROCESSING:
			switch(inConnection.channel.command_in_expected)
			{
			case JVX_PROTOCOL_ADVLINK_STOP:
				std::cout << "Stop ethernet transport in processing." << std::endl;
				inConnection.localStateMachineConnect = JVX_STATE_ACTIVE;
				genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x10;
				_report_property_has_changed(genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.category,
					genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.globalIdx,
					true, 0, 1, JVX_COMPONENT_UNKNOWN);
				this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
				clearChannel = true;
				break;
			default:
				assert(0);
			}
			break;
		default:
			break;
		}
	}
	
	// Do what is required based on what state machine found
	if(clearChannel)
	{
		JVX_LOCK_MUTEX(inConnection.channel.safeAccessChannel);
		inConnection.channel.inUse = false;
		JVX_UNLOCK_MUTEX(inConnection.channel.safeAccessChannel);
	}
	if(sendOut)
	{
#ifdef VERBOSE_OUTPUT
		std::cout << "Message added to queue!" << std::endl;
#endif

		assert(sz <= inConnection.fldCommunicateHandshake_out.sz);
		resL = jvx_message_queue_add_message_to_queue(inConnection.mQueue_out, 0, inConnection.fldCommunicateHandshake_out.ptr, sz, NULL);
		assert(resL == JVX_NO_ERROR);
	}
	if(triggerChannel)
	{
#ifdef VERBOSE_OUTPUT
		std::cout << "Output queue triggered" << std::endl;
#endif

		jvx_message_queue_trigger_queue(inConnection.mQueue_out);
	}

	return(res);
}

jvxErrorType 
CjvxNetworkMasterDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i, j;
	jvxSize cnt = 0;
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;

	_common_set_ocon.theData_out.con_params.buffersize = _inproc.buffersize;
	_common_set_ocon.theData_out.con_params.format = _inproc.format;
	_common_set_ocon.theData_out.con_data.buffers = NULL;
	_common_set_ocon.theData_out.con_data.number_buffers = genNetworkMaster_device::properties_active.numberBuffersDataXChange.value;
	_common_set_ocon.theData_out.con_params.number_channels = _inproc.numInputs;
	_common_set_ocon.theData_out.con_params.rate = _inproc.samplerate;
	//_common_set_ocon.theData_out.sender.source = reinterpret_cast<IjvxObject*>(this);
	//_common_set_ocon.theData_out.sender.tp = _common_set.theComponentType;
	res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		resL = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(resL == JVX_NO_ERROR);
		goto leave_error;
	}

#if 0
	/* Thread controller module entries */
	if (genNetworkMaster_device::properties_active.controlThreads.value.selection() & 0x2)
	{
		if (_common_set_min.theHostRef)
		{
			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&runtime.threads.theToolsHost));
			if ((resL == JVX_NO_ERROR) && runtime.threads.theToolsHost)
			{
				resL = runtime.threads.theToolsHost->reference_tool(JVX_COMPONENT_THREADCONTROLLER, 0, &runtime.threads.coreHdl.object, JVX_STATE_DONTCARE);
				if ((resL == JVX_NO_ERROR) && (runtime.threads.coreHdl.object))
				{
					resL = runtime.threads.coreHdl.object->request_specialization(reinterpret_cast<jvxHandle**>(&runtime.threads.coreHdl.hdl), NULL, NULL);
					if ((resL == JVX_NO_ERROR) && runtime.threads.coreHdl.hdl)
					{
						jvxState stat = JVX_STATE_NONE;
						runtime.threads.coreHdl.hdl->state(&stat);
						if (stat < JVX_STATE_ACTIVE)
						{
							runtime.threads.theToolsHost->return_reference_tool(JVX_COMPONENT_THREADCONTROLLER, runtime.threads.coreHdl.object);
							runtime.threads.coreHdl.hdl = NULL;
							runtime.threads.coreHdl.object = NULL;
						}
					}
				}
			}
		}
	}

	if (runtime.threads.coreHdl.hdl)
	{
		runtime.threads.syncAudioThreadAssoc = false;
		resL = runtime.threads.coreHdl.hdl->register_tc_thread(&runtime.threads.syncAudioThreadId);
		if (resL != JVX_NO_ERROR)
		{
			this->_report_text_message("Failed to control audio thread, no thread control used.", JVX_REPORT_PRIORITY_WARNING);
		}
	}
#endif

	JVX_GET_TICKCOUNT_US_SETREF(&converters.tStamp);
	converters.timeOld = JVX_SIZE_UNSELECTED;
	inProcessing.numberBuffers = 2;
	converters.deltaT_theo = (jvxData)_inproc.buffersize / (jvxData)_inproc.samplerate * 1e6;


	if (res == JVX_NO_ERROR)
	{
		jvxSize qSize = inProcessing.tx_size;
#ifndef JVX_PCAP_DIRECT_SEND			
		res = jvx_connect_client_start_queue_st(this->socketClientIf, qSize, _common_set_icon.theData_in->con_params.number_channels);
		assert(res == JVX_NO_ERROR);
#endif
	}

	// this->_unlock_state();

	return(res);

leave_error:

	return(res);
}

jvxErrorType
CjvxNetworkMasterDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i, j;
	jvxSize cnt = 0;
	jvxErrorType res = JVX_NO_ERROR;

#if 0
	/* Thread controller module entries -> What to do here?*/
	if (genNetworkMaster_device::properties_active.controlThreads.value.selection() & 0x2)

#endif

#ifndef JVX_PCAP_DIRECT_SEND
		res = jvx_connect_client_stop_queue_st(this->socketClientIf);
#endif

	res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	if (inConnection.connectedPartner.subformat_in.use == JVX_DATAFORMAT_GROUP_AUDIO_PDM)
	{		
		for (i = 0; i < _inproc.numInputs; i++)
		{
			jvx_pdm_fir_lut_postprocess(&converters.pdm2PcmConverter[i]);
			JVX_SAFE_DELETE_FLD(converters.pdm2PcmConverter[i].prm_sync.circ_buf, jvxUInt16);
		}

		JVX_DSP_SAFE_DELETE_FIELD(converters.pdm2PcmConverter);
		converters.pdm2PcmConverter = NULL;
	}

	inProcessing.channelLink_in.clear();
	inProcessing.channelLink_out.clear();
	return(res);
}

jvxErrorType
CjvxNetworkMasterDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	jvxBool nonBlockingThreads = false;
	jvxProtocolHeader* theHeaderOut = NULL;
	jvxSize sz;

	res = CjvxConnectionMaster::_start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		resL = CjvxConnectionMaster::_stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(resL == JVX_NO_ERROR);
		goto leave_error;
	}

	JVX_GET_TICKCOUNT_US_SETREF(&tStamp);
	inProcessing.idSendIn = 0;
	inProcessing.idSendOut = 0;
	inProcessing.fillHeight = 0;
	inProcessing.numberBuffersMax = JVX_MIN(
		_common_set_icon.theData_in->con_data.number_buffers - _common_set_icon.theData_in->con_pipeline.num_additional_pipleline_stages,
		_common_set_ocon.theData_out.con_data.number_buffers - _common_set_ocon.theData_out.con_pipeline.num_additional_pipleline_stages);
		*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr = 0;
	*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr = 0;

	if (JVX_EVALUATE_BITFIELD(genNetworkMaster_device::properties_active.selectionRunNonBlocking.value.selection() & 0x1))
	{
		nonBlockingThreads = true;
	}

	// Start the threads
	res = jvx_thread_initialize(&theThread.theThreadHandle,
		&theThread.theThreadHandler,
		reinterpret_cast<jvxHandle*>(this), false,
		nonBlockingThreads);
	assert(res == JVX_NO_ERROR);

	res = jvx_thread_start(theThread.theThreadHandle, JVX_INFINITE_MS);
	assert(res == JVX_NO_ERROR);

	res = jvx_thread_set_priority(theThread.theThreadHandle, JVX_THREAD_PRIORITY_REALTIME);
	assert(res == JVX_NO_ERROR);

	// Increase priority of receive thread
	res = jvx_connect_client_set_thread_priority_st(socketClientIf, JVX_THREAD_PRIORITY_REALTIME);
	assert(res == JVX_NO_ERROR);

	if (res == JVX_NO_ERROR)
	{

		JVX_GET_TICKCOUNT_US_SETREF(&inProcessing.theTimestamp);
		inProcessing.timestamp_previous = -1;
		inProcessing.deltaT_theory_us = (jvxData)_common_set_ocon.theData_out.con_params.buffersize / (jvxData)_common_set_ocon.theData_out.con_params.rate * 1000.0 * 1000.0;
		inProcessing.deltaT_average_us = 0;
	}

	inConnection.oneChannelInOut.channelMask = 0;
	inProcessing.channelMaskTarget = (1 << inConnection.connectedPartner.inChannels) - 1;
	inProcessing.idSequenceExpected = -1;
	inConnection.oneChannelInOut.idxChannel = 0;

	// Here, send the start message
	theHeaderOut = (jvxProtocolHeader*)inConnection.fldCommunicateHandshake_out.shcut;

	if (inConnection.localStateMachineConnect == JVX_STATE_PROCESSING)
	{
		std::cout << "Warning: previous session was not propperly terinated." << std::endl;
	}
	switch (inConnection.localStateMachineConnect)
	{
	case JVX_STATE_ACTIVE:

		theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_START);
		theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
		
		sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
		sz = JVX_MAX(sz, inConnection.fldMinSize);
		theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

		resL = jvx_message_queue_add_message_to_queue(inConnection.mQueue_out, 0, inConnection.fldCommunicateHandshake_out.ptr,
			inConnection.fldCommunicateHandshake_out.sz, NULL);
		assert(resL == JVX_NO_ERROR);
		break;
	default:
		assert(0);
	}

	return(res);

leave_error:
	return res;
}

jvxErrorType
CjvxNetworkMasterDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxProtocolHeader* theHeaderOut = NULL;
	jvxSize sz;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	// Stop the threads
	res = jvx_thread_stop(theThread.theThreadHandle);
	assert(res == JVX_NO_ERROR);

	res = jvx_thread_terminate(theThread.theThreadHandle);
	assert(res == JVX_NO_ERROR);

	theHeaderOut = (jvxProtocolHeader*)inConnection.fldCommunicateHandshake_out.shcut;
	switch (inConnection.localStateMachineConnect)
	{
	case JVX_STATE_PROCESSING:

		inConnection.localStateMachineConnect = JVX_STATE_COMPLETE;

		theHeaderOut->purpose = (JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_STOP);
		theHeaderOut->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;

		sz = sizeof(jvxProtocolHeader) + inConnection.fldPrepend;
		sz = JVX_MAX(sz, inConnection.fldMinSize);
		theHeaderOut->paketsize = JVX_SIZE_UINT32(sz - inConnection.fldPrepend);

		resL = jvx_message_queue_add_message_to_queue(inConnection.mQueue_out, 0, inConnection.fldCommunicateHandshake_out.ptr,
			inConnection.fldCommunicateHandshake_out.sz, NULL);
		assert(resL == JVX_NO_ERROR);

		// Wait until the stop packet was received..
		for (i = 0; i < 20; i++)
		{
			if (inConnection.localStateMachineConnect != JVX_STATE_COMPLETE)
			{
				break;
			}
			JVX_SLEEP_MS(100);
		}

		if (inConnection.localStateMachineConnect == JVX_STATE_COMPLETE)
		{
			std::cout << __FUNCTION__ << "Warning: timeout when stopping processing" << std::endl;
			inConnection.localStateMachineConnect = JVX_STATE_ACTIVE;
		}

		break;
	case JVX_STATE_ACTIVE:
		std::cout << "It seems that processing did not start properly, reconnect and restart!" << std::endl;
		break;
	default:
		assert(0);
	}

	res = CjvxConnectionMaster::_stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Stopping chain failed." << std::endl;
	}

	return res;
}

jvxErrorType
CjvxNetworkMasterDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	
	_common_set_icon.theData_in->con_params.buffersize = _inproc.buffersize;
	_common_set_icon.theData_in->con_params.format = _inproc.format;
	_common_set_icon.theData_in->con_data.buffers = NULL;
	_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(
		genNetworkMaster_device::properties_active.numberBuffersDataXChange.value,
		_common_set_icon.theData_in->con_data.number_buffers);
	_common_set_icon.theData_in->con_params.number_channels = _inproc.numOutputs;
	_common_set_icon.theData_in->con_params.rate = _inproc.samplerate;

	// now, the receiver to sender side
	res = jvx_allocateDataLinkDescriptor(_common_set_icon.theData_in, false);
	inProcessing.tx_fields = NULL;

	inProcessing.tx_size = jvxDataFormat_size[_common_set_icon.theData_in->con_params.format] *
		_common_set_icon.theData_in->con_params.buffersize;
	inProcessing.tx_size += sizeof(jvxProtocolHeader) + sizeof(jvxAdvProtocolDataChunkHeader) + inConnection.fldPrepend;

	JVX_SAFE_NEW_FLD(inProcessing.tx_fields, jvxByte**, _common_set_icon.theData_in->con_data.number_buffers);

	for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
	{
		JVX_SAFE_NEW_FLD(inProcessing.tx_fields[i], jvxByte*, _common_set_icon.theData_in->con_params.number_channels);
		for (j = 0; j < _common_set_icon.theData_in->con_params.number_channels; j++)
		{
			inProcessing.tx_fields[i][j] = NULL;
			JVX_SAFE_NEW_FLD(inProcessing.tx_fields[i][j], jvxByte, inProcessing.tx_size);
			memset(inProcessing.tx_fields[i][j], 0, inProcessing.tx_size);
			jvxAdvProtocolDataP* hdr = (jvxAdvProtocolDataP*)(inProcessing.tx_fields[i][j] + inConnection.fldPrepend);
			hdr->dheader.header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK;
			hdr->dheader.header.paketsize = JVX_SIZE_INT32(inProcessing.tx_size - inConnection.fldPrepend);
			hdr->dheader.header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST | JVX_PROTOCOL_ADVLINK_SEND_DATA;
			hdr->dheader.adv_data_header.channel_mask = ((jvxUInt64)1 << j);
			hdr->dheader.adv_data_header.sequence_id = 0;
			_common_set_icon.theData_in->con_data.buffers[i][j] =
				inProcessing.tx_fields[i][j] + sizeof(jvxProtocolHeader) + sizeof(jvxAdvProtocolDataChunkHeader) + inConnection.fldPrepend;
		}
	}

	res = jvx_allocateDataLinkPipelineControl(_common_set_icon.theData_in); 
	*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr = 0;

	return res;
}

jvxErrorType
CjvxNetworkMasterDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;

	// res = _data_link_stop();
	assert(res == JVX_NO_ERROR);

	for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
	{
		for (j = 0; j < _common_set_icon.theData_in->con_params.number_channels; j++)
		{
			_common_set_icon.theData_in->con_data.buffers[i][j] = NULL;
			JVX_SAFE_DELETE_FLD(inProcessing.tx_fields[i][j], jvxByte);
			inProcessing.tx_fields[i][j] = NULL;
		}
		JVX_SAFE_DELETE_FLD(inProcessing.tx_fields[i], jvxByte*);
	}
	JVX_SAFE_DELETE_FLD(inProcessing.tx_fields, jvxByte**);
	inProcessing.tx_fields = NULL;
	inProcessing.tx_size = 0;
	// now, the receiver to sender side

	res = jvx_deallocateDataLinkPipelineControl(_common_set_icon.theData_in); 
	res = jvx_deallocateDataLinkDescriptor(_common_set_icon.theData_in, false);

	return res;
}

jvxErrorType
CjvxNetworkMasterDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	return res;
}

jvxErrorType
CjvxNetworkMasterDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	// This is the return from the link list
	res = shift_buffer_pipeline_idx_on_start( pipeline_offset,  idx_stage, tobeAccessedByStage,
		clbk, priv_ptr);

	// This is the return from the link list
	return res;
}

jvxErrorType
CjvxNetworkMasterDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	_common_set_icon.theData_in->con_pipeline.do_lock(_common_set_icon.theData_in->con_pipeline.lock_hdl);
	_common_set_icon.theData_in->con_pipeline.reserve_buffer_pipeline_stage[
		*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr].idProcess = JVX_SIZE_UNSELECTED;
	inProcessing.fillHeight--;
	*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr =
		(*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr + 1) %
		_common_set_icon.theData_in->con_data.number_buffers;
	_common_set_icon.theData_in->con_pipeline.do_unlock(_common_set_icon.theData_in->con_pipeline.lock_hdl);

	return res;
}

jvxErrorType
CjvxNetworkMasterDevice::report_prepare()
{
	std::vector<std::string> errMessages;
	HjvxDataLogger::_activate(dataLogging.theDataLogger_hdl, LOGFILE_NUMBER_FRAMES_MEMORY, false, LOGFILE_STOP_TIMEOUT_MS);

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxNetworkMasterDevice::report_start(std::string fName)
{
	jvxSize i;
	std::string txt;
	std::vector<std::string> errMessages;
	JVX_DSP_SAFE_ALLOCATE_FIELD(dataLogging.idxWrite_rawaudio, jvxSize, _inproc.numInputs);
	JVX_DSP_SAFE_ALLOCATE_FIELD(dataLogging.fldWrite_rawaudio, jvxByte*, _inproc.numInputs);
	for (i = 0; i < _inproc.numInputs; i++)
	{
		HjvxDataLogger::_add_line(
			"Raw input channel #" + jvx_size2String(i), 
			this->_inproc.buffersize,
			_inproc.format,
			JVX_SIZE_INT32(i),
			dataLogging.idxWrite_rawaudio[i]);

	}

	HjvxDataLogger::_add_line("timing/events", JVX_MASTER_NETWORK_NUMBER_ENTRIES_TIMING, JVX_DATAFORMAT_32BIT_LE, 0, dataLogging.idxWrite_timing);
	HjvxDataLogger::_add_session_tag("audio/in/channels", jvx_int2String(_inproc.numInputs));

	genNetworkMaster_device::timinglogtofile.properties_logtofile.fillHeightBuffer.value.val() = 0;

	HjvxDataLogger::_start(fName, errMessages, _common_set_min.theHostRef);
	
	for (i = 0; i < _inproc.numInputs; i++)
	{
		HjvxDataLogger::_field_reference(i, &dataLogging.fldWrite_rawaudio[i]);
	}
	HjvxDataLogger::_field_reference(i, &dataLogging.fldWrite_timing);
	dataLogging.fldWrite_timing = NULL;

	txt = "Successfully started log file " + fName;
	this->_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxNetworkMasterDevice::report_stop(std::string fName)
{
	std::string txt;
	std::vector<std::string> errMessages;
	jvxSize numWrittenMod = 0;
	jvxSize numWrittenFile = 0;
	HjvxDataLogger::_stop(&numWrittenMod, &numWrittenFile, errMessages);
	HjvxDataLogger::_remove_session_tags();
	HjvxDataLogger::_clean_all();
	// Stop logfile if active

	txt = "Successfully stopped log file " + fName;
	if (numWrittenFile < numWrittenMod)
	{
		txt += ", written ";
		txt += jvx_size2String(numWrittenMod) + "Bytes to module and ";
		txt += jvx_size2String(numWrittenFile) + " bytes to file (delta = ";
		txt += jvx_size2String(numWrittenMod - numWrittenFile) + ").";
	}
	else
	{
		txt += ".";
	}
	this->_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxNetworkMasterDevice::report_postprocess()
{
	HjvxDataLogger::_deactivate();

	return(JVX_NO_ERROR);
}

void
CjvxNetworkMasterDevice::update_parameters(jvxBool isConnected)
{
	if (isConnected)
	{
		this->constructPropertiesAfterConnect();

		if (genNetworkMaster_device::integrateiplink.properties_running.autoStart.value == c_true)
		{
			this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TRY_TRIGGER_START_SEQUENCER_SHIFT);
		}
	}
	else
	{
		jvxCBitField reportflags = 0;

		resetPropertiesAfterDisonnect();

		genNetworkMaster_device::integrateiplink.properties_running.connectionStatus.value.selection() = 0x1;

		jvx_bitFClear(reportflags);
		jvx_bitSet(reportflags, JVX_REPORT_REQUEST_RECONFIGURE_SHIFT);
		jvx_bitSet(reportflags, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT);
		//this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
		this->_report_command_request(reportflags);
	}
}

jvxErrorType 
CjvxNetworkMasterDevice::report_register_be_commandline(IjvxCommandLine* comLine)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxNetworkMasterDevice::report_readout_be_commandline(IjvxCommandLine* comLine)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxNetworkMasterDevice::process_event(TjvxEventLoopElement* theQueueElement)
{
	switch (theQueueElement->eventType)
	{
	case JVX_EVENTLOOP_EVENT_SPECIFIC:
		update_parameters(true);
		break;
	case JVX_EVENTLOOP_EVENT_SPECIFIC+1:
		update_parameters(false);
		break;
	default:
		assert(0);
	}
	return JVX_NO_ERROR;
}

#ifdef JVX_PROJECT_NAMESPACE
}

#endif

