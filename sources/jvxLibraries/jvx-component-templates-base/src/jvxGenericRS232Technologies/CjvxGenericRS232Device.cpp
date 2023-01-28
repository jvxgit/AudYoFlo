
#include "jvxGenericRS232Technologies/CjvxGenericRS232Device.h"

#define STAT_LST_BAUDRATES_NUM 5
static jvxSize stat_lst_baudrates[STAT_LST_BAUDRATES_NUM] = { 9600, 19200, 38400, 57600, 115200 };

#define STAT_LST_BITS4BYTES_NUM 5
static jvxSize stat_lst_bits4bytes[STAT_LST_BITS4BYTES_NUM] = { 4,5,6,7,8 };

typedef struct
{
	jvxRs232ParityEnum par;
	const char* nm;
} oneLocEntryParity;
#define STAT_LST_PARITY_NUM 5
#define STAT_ID_PARITY_DEF 3
static oneLocEntryParity stat_lst_parity[STAT_LST_PARITY_NUM] = 
{
	{JVX_RS232_PARITY_EVEN, "Even"},
	{JVX_RS232_PARITY_MARK, "Mark"}, 
	{JVX_RS232_PARITY_ODD, "Odd"}, 
	{JVX_RS232_PARITY_NO, "No"},
	{JVX_RS232_PARITY_SPACE, "Space"}
};

typedef struct
{
	jvxRs232StopbitsEnum stops;
	const char* nm;
} oneLocEntryStopbits;
#define STAT_LST_STOPBITS_NUM 3
#define STAT_ID_STOPBITS_DEF 0
static oneLocEntryStopbits stat_lst_stopbits[STAT_LST_STOPBITS_NUM] =
{
	{JVX_RS232_STOPBITS_ONE, "One"},
	{JVX_RS232_STOPBITS_ONE5, "One5"},
	{JVX_RS232_STOPBITS_TWO, "two"}
};

// ==================================================================================================
// ==================================================================================================

CjvxGenericRS232Device::CjvxGenericRS232Device(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
	// , jvxrtst_local(&jvxrtst_bkp_local.jvxos)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_UNKNOWN);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	
	theRs232Tool = NULL;
	theRs232Obj = NULL;
	idDevice = JVX_SIZE_UNSELECTED;

	theMQueue = NULL;
	theMQueueCallbacks.callback_message_queue_message_in_queue_ready = callback_message_queue_message_in_queue_ready;
	theMQueueCallbacks.callback_message_queue_message_in_queue_timeout = callback_message_queue_message_in_queue_timeout;
	theMQueueCallbacks.callback_message_queue_started = NULL;
	theMQueueCallbacks.callback_message_queue_stopped = NULL;

	callbacks.callback_thread_startup = NULL;
	callbacks.callback_thread_timer_expired = cb_thread_timer_expired;
	callbacks.callback_thread_wokeup = NULL;
	callbacks.callback_thread_stopped = NULL;

	JVX_INITIALIZE_MUTEX(safeAccessChannel);

	runtime.mem_incoming = NULL;
	runtime.sz_mem_incoming = 0;

	message_queue.sz_mqueue_elements = sizeof(oneMessage_hdr);
	message_queue.num_messages_in_queue = 10;
	message_queue.timeout_messages_in_queue_msec = 1000;
	
	observer.timeout_observer_msec = 500;

	mIdMessages = 1;
	channel_fully_locked = false;
	
	output_flags = 0;
	fullPingPong = true;

	i_am_ready = false;

	num_retrans_max = 0;

	output_rs232_on_stdcout = false; // We may bypass rs232 out and copy to console only!
#ifdef JVX_SHOW_QUICK_TIMING
	JVX_GET_TICKCOUNT_US_SETREF(&myTStamp);
	leave = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStamp);
#endif
}

CjvxGenericRS232Device::~CjvxGenericRS232Device()
{
	this->terminate();
	JVX_TERMINATE_MUTEX(safeAccessChannel);

}

void
CjvxGenericRS232Device::setPortId(int id, IjvxConnection* theRs232ToolArg, IjvxObject* theRs232ObjArg)
{
	theRs232Tool = theRs232ToolArg;
	theRs232Obj = theRs232ObjArg;
	idDevice = id;
};

// ====================================================================================================
jvxErrorType
CjvxGenericRS232Device::select(IjvxObject* theOwner)
{
	jvxSize i = 0;
	// oneEventTransfer theTransfer;
	jvxErrorType resL = JVX_NO_ERROR;

	jvxErrorType res = _select(theOwner);
	if (res == JVX_NO_ERROR)
	{
		CjvxGenericRs232Device_pcg::init_all();
		CjvxGenericRs232Device_pcg::allocate_all();
		CjvxGenericRs232Device_pcg::register_all(static_cast<CjvxProperties*>(this));

		_lock_properties_local();
		CjvxGenericRs232Device_pcg::rs232_settings.baudrate.value.entries.clear();
		for (i = 0; i < STAT_LST_BAUDRATES_NUM; i++)
		{
			CjvxGenericRs232Device_pcg::rs232_settings.baudrate.value.entries.push_back(jvx_size2String(stat_lst_baudrates[i]));
		}
		jvx_bitFClear(CjvxGenericRs232Device_pcg::rs232_settings.baudrate.value.selection());
		jvx_bitSet(CjvxGenericRs232Device_pcg::rs232_settings.baudrate.value.selection(), STAT_LST_BAUDRATES_NUM - 1);

		CjvxGenericRs232Device_pcg::rs232_settings.bits4Bytes.value.entries.clear();
		for (i = 0; i < STAT_LST_BITS4BYTES_NUM; i++)
		{
			CjvxGenericRs232Device_pcg::rs232_settings.bits4Bytes.value.entries.push_back(jvx_size2String(stat_lst_bits4bytes[i]));
		}
		jvx_bitFClear(CjvxGenericRs232Device_pcg::rs232_settings.bits4Bytes.value.selection());
		jvx_bitSet(CjvxGenericRs232Device_pcg::rs232_settings.bits4Bytes.value.selection(), STAT_LST_BITS4BYTES_NUM - 1);

		CjvxGenericRs232Device_pcg::rs232_settings.parity.value.entries.clear();
		for (i = 0; i < STAT_LST_PARITY_NUM; i++)
		{
			CjvxGenericRs232Device_pcg::rs232_settings.parity.value.entries.push_back(stat_lst_parity[i].nm);
		}
		jvx_bitFClear(CjvxGenericRs232Device_pcg::rs232_settings.parity.value.selection());
		jvx_bitSet(CjvxGenericRs232Device_pcg::rs232_settings.parity.value.selection(), STAT_ID_PARITY_DEF);

		CjvxGenericRs232Device_pcg::rs232_settings.stopbits.value.entries.clear();
		for (i = 0; i < STAT_LST_STOPBITS_NUM; i++)
		{
			CjvxGenericRs232Device_pcg::rs232_settings.stopbits.value.entries.push_back(stat_lst_stopbits[i].nm);
		}
		jvx_bitFClear(CjvxGenericRs232Device_pcg::rs232_settings.stopbits.value.selection());
		jvx_bitSet(CjvxGenericRs232Device_pcg::rs232_settings.stopbits.value.selection(), STAT_ID_STOPBITS_DEF);

		_unlock_properties_local();
	}
	return res;
}

jvxErrorType
CjvxGenericRS232Device::unselect()
{
	jvxSize i = 0;
	// oneEventTransfer theTransfer;
	jvxErrorType res = JVX_NO_ERROR;

	// Standard
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		stop();
	}
	if (_common_set_min.theState == JVX_STATE_PREPARED)
	{
		postprocess();
	}
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		deactivate();
	}

	// SPecific here
	_lock_properties_local();
	CjvxGenericRs232Device_pcg::rs232_settings.baudrate.value.entries.clear();
	jvx_bitFClear(CjvxGenericRs232Device_pcg::rs232_settings.baudrate.value.selection());

	CjvxGenericRs232Device_pcg::rs232_settings.bits4Bytes.value.entries.clear();
	jvx_bitFClear(CjvxGenericRs232Device_pcg::rs232_settings.bits4Bytes.value.selection());

	CjvxGenericRs232Device_pcg::rs232_settings.parity.value.entries.clear();
	jvx_bitFClear(CjvxGenericRs232Device_pcg::rs232_settings.parity.value.selection());

	CjvxGenericRs232Device_pcg::rs232_settings.stopbits.value.entries.clear();
	jvx_bitFClear(CjvxGenericRs232Device_pcg::rs232_settings.stopbits.value.selection());

	_unlock_properties_local();

	CjvxGenericRs232Device_pcg::unregister_all(static_cast<CjvxProperties*>(this));
	CjvxGenericRs232Device_pcg::deallocate_all();

	res = _unselect();
	return res;
}

// ====================================================================================================


jvxErrorType
CjvxGenericRS232Device::activate()
{
	jvxSize i = 0;
	// oneEventTransfer theTransfer;
	jvxErrorType resL = JVX_NO_ERROR;

	jvxErrorType res = _activate();
	if (res == JVX_NO_ERROR)
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << ": " << "activate " << _common_set.theDescriptor << " device component" << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

		// ==========================================================================================
		// Initialize specific part BEFORE stat of message queues
		// ==========================================================================================
		resL = activate_specific_rs232();

		// ==========================================================================================
		// ==========================================================================================
		JVX_GET_TICKCOUNT_US_SETREF(&tStampData);

		assert(theRs232Tool);

		// Allocate the number of bytes desired by derived class!
		runtime.sz_mem_incoming = CjvxGenericRs232Device_pcg::rs232_runtime.size_bytes_inbuf.value;
		runtime.sz_mem_incoming = JVX_MAX(2, runtime.sz_mem_incoming); // Minimum number of 2 elements required: 1 space and 1 0-char
		if (runtime.sz_mem_incoming == 0)
		{
			runtime.sz_mem_incoming = 1;
		}
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.mem_incoming, jvxByte, runtime.sz_mem_incoming);

		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << ": " << "Starting input buffering:" << std::endl;
			jvxrtst << "-> Size of input buffer: " << runtime.sz_mem_incoming << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

		jvxRs232Config cfg;
		jvxSize idSel = JVX_SIZE_UNSELECTED;

		// Set baudrate
		cfg.bRate = 115200;
		idSel = jvx_bitfieldSelection2Id(CjvxGenericRs232Device_pcg::rs232_settings.baudrate.value.selection(),
			CjvxGenericRs232Device_pcg::rs232_settings.baudrate.value.entries.size());
		assert(idSel < STAT_LST_BAUDRATES_NUM);
		cfg.bRate = stat_lst_baudrates[idSel];

		// Set bits4bytes
		cfg.bits4Byte = 8;
		idSel = jvx_bitfieldSelection2Id(CjvxGenericRs232Device_pcg::rs232_settings.bits4Bytes.value.selection(),
			CjvxGenericRs232Device_pcg::rs232_settings.bits4Bytes.value.entries.size());
		assert(idSel < STAT_LST_BITS4BYTES_NUM);
		cfg.bits4Byte = stat_lst_bits4bytes[idSel];

		cfg.stopBitsEnum = JVX_RS232_STOPBITS_ONE;
		idSel = jvx_bitfieldSelection2Id(CjvxGenericRs232Device_pcg::rs232_settings.stopbits.value.selection(),
			CjvxGenericRs232Device_pcg::rs232_settings.stopbits.value.entries.size());
		assert(idSel < STAT_LST_STOPBITS_NUM);
		cfg.stopBitsEnum = stat_lst_stopbits[idSel].stops;

		cfg.parityEnum = JVX_RS232_PARITY_NO;
		idSel = jvx_bitfieldSelection2Id(CjvxGenericRs232Device_pcg::rs232_settings.parity.value.selection(),
			CjvxGenericRs232Device_pcg::rs232_settings.parity.value.entries.size());
		assert(idSel < STAT_LST_PARITY_NUM);
		cfg.parityEnum = stat_lst_parity[idSel].par;

		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << ": " << "Starting rs-232 port with settings:" << std::endl;
			jvxrtst << "-> Baudrate: " << cfg.bRate << std::endl;
			jvxrtst << "-> Bits 4 bytes: " << cfg.bits4Byte << std::endl;
			jvxrtst << "-> Stop bits: " << std::flush;
			switch (cfg.stopBitsEnum)
			{
			case JVX_RS232_STOPBITS_ONE:
				jvxrtst << "ONESTOPBIT" << std::endl;
				break;
			case JVX_RS232_STOPBITS_ONE5:
				jvxrtst << "ONE_5_STOPBIT" << std::endl;
				break;
			case JVX_RS232_STOPBITS_TWO:
				jvxrtst << "TWOSTOPBIT" << std::endl;
				break;
			}
			jvxrtst << "-> Parity: " << std::flush;
			switch (cfg.parityEnum)
			{
			case JVX_RS232_PARITY_EVEN:
				jvxrtst << "EVENPARITY" << std::endl;
				break;
			case JVX_RS232_PARITY_ODD:
				jvxrtst << "ODDPARITY" << std::endl;
				break;
			case JVX_RS232_PARITY_NO:
				jvxrtst << "NOPARITY" << std::endl;
				break;
			case JVX_RS232_PARITY_SPACE:
				jvxrtst << "SPACEPARITY" << std::endl;
				break;
			}

			jvx_unlock_text_log(jvxrtst_bkp);
		}

		cfg.enFlow = JVX_RS232_NO_FLOWCONTROL;

		cfg.boostPrio = false;

		cfg.reportEnum = 0;
		cfg.secureChannel.transmit.active = c_false;
		cfg.secureChannel.transmit.preallocationSize = 0;

		cfg.secureChannel.receive.active = c_false;
		cfg.secureChannel.receive.maxPreReadSize = 128;

		cfg.secureChannel.startByte = JVX_RS232_SECURE_CHANNEL_START_BYTE;
		cfg.secureChannel.stopByte = JVX_RS232_SECURE_CHANNEL_STOP_BYTE;
		cfg.secureChannel.escByte = JVX_RS232_SECURE_CHANNEL_ESC_BYTE;

		jvx_bitSet(cfg.reportEnum, JVX_RS232_REPORT_CONNECTION_ESTABLISHED_SHIFT);
		jvx_bitSet(cfg.reportEnum, JVX_RS232_REPORT_CONNECTION_STOPPED_SHIFT); // No CTS 

		res = this->theRs232Tool->start_port(idDevice, &cfg, JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_CONFIG, static_cast<IjvxConnection_report*>(this));
			
		if (res == JVX_NO_ERROR)
		{
			// ==========================================================================================
			// Start message queue for all messages
			// ===============================================================================

			message_queue.num_messages_in_queue = CjvxGenericRs232Device_pcg::rs232_runtime.number_entries_mqueue.value;
			message_queue.timeout_messages_in_queue_msec = CjvxGenericRs232Device_pcg::rs232_runtime.timeoutmqueue_msec.value;
			observer.timeout_message_response = CjvxGenericRs232Device_pcg::rs232_runtime.timeoutresponse_msec.value;

			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::" << __FUNCTION__ << ": " << "Starting message queue:" << std::endl;
				jvxrtst << "-> Number of elements: " << message_queue.sz_mqueue_elements << std::endl;
				jvxrtst << "-> Timeout messages: " << message_queue.timeout_messages_in_queue_msec << std::endl;
				jvxrtst << "-> Size of each element in queue: " << message_queue.sz_mqueue_elements << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}

			res = jvx_message_queue_initialize(&theMQueue, JVX_MESSAGE_QUEUE_STATIC_OBJECTS, &theMQueueCallbacks,
				reinterpret_cast<jvxHandle*>(this), message_queue.num_messages_in_queue,
				message_queue.timeout_messages_in_queue_msec, message_queue.sz_mqueue_elements);
			assert(res == JVX_NO_ERROR);

			res = jvx_message_queue_start(theMQueue);
			assert(res == JVX_NO_ERROR);

			// ===============================================================================
			// Start thread for mositoring status of device
			// ===============================================================================
			observer.timeout_observer_msec = CjvxGenericRs232Device_pcg::rs232_runtime.timeoutobserver_msec.value;
			jvx_thread_initialize(&theObserverThread, &callbacks, reinterpret_cast<jvxHandle*>(this), false);
			jvx_thread_start(theObserverThread, observer.timeout_observer_msec);

			// Here, we need to wait a little bit
			JVX_SLEEP_S(1);

			i_am_ready = true;

			// ===============================================================================
			// Pass generic message into message queue
			// ===============================================================================
			activate_init_messages();
		}
		else
		{
			res = JVX_ERROR_COMPONENT_BUSY;
			goto errorI;
		}
	}
	return(res);

errorI:

	// Unroll all actions to close this device again
	// Cleanup dynamic memory
	if (runtime.sz_mem_incoming)
	{
		JVX_DSP_SAFE_DELETE_FIELD(runtime.mem_incoming);
	}
	resL = deactivate_specific_rs232();
	runtime.sz_mem_incoming = 0;

	_deactivate();
	return res;
}

jvxErrorType
CjvxGenericRS232Device::deactivate()
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = _pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		// Lock incoming messages
		i_am_ready = false;
		if (this->theRs232Tool)
		{
			jvx_thread_stop(theObserverThread);
			JVX_SLEEP_MS(200);

			jvx_message_queue_stop(theMQueue);

			// Allow pending events to pass
			JVX_SLEEP_MS(200);

			jvx_thread_terminate(theObserverThread);
			theObserverThread = NULL;

			jvx_message_queue_terminate(theMQueue);

			// Last chance to transmit a "goodbye message"
			this->goodbye_rs232();

			this->theRs232Tool->stop_port(idDevice);

			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::" << __FUNCTION__ << ": " << "stopping connection on COM port <" << idDevice << ">," << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}

			// Cleanup dynamic memory
			if (runtime.sz_mem_incoming)
			{
				JVX_DSP_SAFE_DELETE_FIELD(runtime.mem_incoming);
			}
			runtime.sz_mem_incoming = 0;

#if 0
			jvx_message_queue_stop(theMQueue);
			jvx_message_queue_terminate(theMQueue);
#endif
			resL = deactivate_specific_rs232();

			CjvxGenericRs232Device_pcg::unregister_all(static_cast<CjvxProperties*>(this));
			CjvxGenericRs232Device_pcg::deallocate_all();

			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::" << __FUNCTION__ << ": " << "deactivate " << _common_set.theDescriptor << " device component" << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}

			channel_fully_locked = false;
			mpMessages.clear();

			_deactivate();
		}
	}
	return(res);
}


jvxErrorType
CjvxGenericRS232Device::provide_data_and_length(jvxByte**ptr, jvxSize* maxNumCopy, jvxSize* offset, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	// Set the output buffer, at maximum, copy the number of bytes, otherwise,
	// I/O will wait until other bytes are available (which will no happen!)
	assert(runtime.mem_incoming != NULL);
	assert(runtime.sz_mem_incoming > 0);
	*ptr = runtime.mem_incoming;
	*maxNumCopy = JVX_MIN(runtime.sz_mem_incoming - 1, *maxNumCopy);
	memset(*ptr, 0, *maxNumCopy + 1);
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericRS232Device::report_data_and_read(jvxByte* ptr, jvxSize numRead, jvxSize offset, jvxSize id_port,
	jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType resL = JVX_NO_ERROR;
	oneMessage_hdr theMess;
	jvxSize toProcess = numRead;

	if (!i_am_ready)
	{
		// Skip messages
		return JVX_NO_ERROR;
	}

#ifdef JVX_SHOW_QUICK_TIMING
	enter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStamp);
	std::cout << "::" << enter *0.001 * 0.001 << "sec, delta T leave -> enter, " << __FUNCTION__ << " = " << ( enter - leave) / 1000.0 << " msecs." << std::endl;
#endif

	if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_ALLINCOMING_SHIFT))
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << ": Incoming message from COM port: <" << jvx_prepareStringForLogfile((const char*)ptr) << ">." << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}
	}

	while (1)
	{
		jvxSize idIdentify = JVX_SIZE_UNSELECTED;
		theMess.expectResponse = false;
		theMess.mId = JVX_SIZE_UNSELECTED;
		theMess.uId = JVX_SIZE_UNSELECTED;
		theMess.tp = JVX_GENERIC_RS232_NO_MESSAGE_TYPE;
		theMess.sz_elm = sizeof(oneMessage_hdr);
		theMess.stat = JVX_GENERIC_RS232_STATUS_NONE;

		// Handle incoming data
		resL = this->handle_incoming_data(ptr, toProcess, offset, id_port, addInfo, whatsthis, &theMess, &idIdentify);
		if (resL == JVX_ERROR_POSTPONE)
		{
			// Need more input
			if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_ALLINCOMING_SHIFT))
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvx_lock_text_log(jvxrtst_bkp);
					jvxrtst << "::" << __FUNCTION__ << ": Incoming message from COM port, processing postponed." << std::endl;
					jvx_unlock_text_log(jvxrtst_bkp);
				}
			}

			// Break the while loop
			break;
		}
		else if (resL == JVX_NO_ERROR)
		{
			// Found a valid message, remove message from queue and process.
			// There might be more, hence we will return (no return from while loop)
			if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_ALLINCOMING_SHIFT))
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvx_lock_text_log(jvxrtst_bkp);
					jvxrtst << "::" << __FUNCTION__ << ": Incoming message from COM port complete, removing id = <" << theMess.uId
						<< "> from context list." << std::endl;
					jvx_unlock_text_log(jvxrtst_bkp);
				}
			}

			if (JVX_CHECK_SIZE_SELECTED(theMess.uId))
			{
				resL = remove_data_from_map(&theMess);
				if (resL != JVX_NO_ERROR)
				{
					std::cout << "Tried to remove message from list which is not in the list, id = <" << theMess.uId << ">." << std::endl;;
				}

				if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_ALLINCOMING_SHIFT))
				{
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvx_lock_text_log(jvxrtst_bkp);
						jvxrtst << "::" << __FUNCTION__ << ": Message from COM port complete, removed id = <" << theMess.uId << "> from context list."
							<< std::endl;
						jvx_unlock_text_log(jvxrtst_bkp);
					}
				}
				resL = this->post_message_hook(idIdentify);
			}
		}
		else if (resL == JVX_ERROR_ABORT)
		{
			// Message was not handled by sub component and need no consideration
		}
		else if (resL == JVX_ERROR_EMPTY_LIST)
		{
			// Message was handled by one of the skip rules
		}
		else
		{
			// I have no idea when I would enter this rule
			if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_ERRORS_SHIFT))
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvx_lock_text_log(jvxrtst_bkp);
					if (ptr)
					{
						jvxrtst << "::" << __FUNCTION__ << ": Incoming generic message with error message in handling, txt = <"
							<< (const char*)ptr << ">, error code: " << jvxErrorType_txt(resL) << std::endl;
					}
					else
					{
						jvxrtst << "::" << __FUNCTION__ << ": Subsequent call to message handling returned error, error code: " << jvxErrorType_txt(resL) << std::endl;
					}
					jvx_unlock_text_log(jvxrtst_bkp);
				}
			}
			theMess.tp = JVX_GENERIC_RS232_GENERIC_MESSAGE;
			break;
		}
		toProcess = 0;
		ptr = NULL;
		if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_ALLINCOMING_SHIFT))
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::" << __FUNCTION__ << ": Decoding loop will procede into next iteration since there may be more work to do."
					<< std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}
		}
	}

#ifdef JVX_SHOW_QUICK_TIMING
	leave = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStamp);
	std::cout << "::" << enter * 0.001 * 0.001 << "sec, delta T on enter -> leave, " << __FUNCTION__ << " = " << (leave - enter) / 1000.0 << " msecs." << std::endl;
#endif

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericRS232Device::report_event(jvxBitField eventMask, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericRS232Device::remove_data_from_map(oneMessage_hdr* theMess)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxInt64 tstamp = 0;
	jvxBool trigger_queue = false;

	jvx_message_queue_get_timestamp_us(theMQueue, &tstamp);
	if (theMess->tp != JVX_GENERIC_RS232_NO_MESSAGE_TYPE)
	{
		JVX_LOCK_MUTEX(safeAccessChannel);
		auto elm = mpMessages.find(theMess->uId);
		if (elm != mpMessages.end())
		{
			res = JVX_NO_ERROR;
		

			if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_TIMING_SHIFT))
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvx_lock_text_log(jvxrtst_bkp);
					jvxrtst << "::" << __FUNCTION__ << ": T0 + " << (tstamp - elm->second->timestamp_enter_us) * 0.001 << 
						" (from enter) msec => Removing message uid <" << elm->second->uId << "> - mid <" << elm->second->mId << 
						"> from list of pending messages." << std::endl;
					jvx_unlock_text_log(jvxrtst_bkp);
				}
			}

			// Remove memory of the single element in list
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(elm->second, jvxByte);

			mpMessages.erase(elm);
			if (channel_fully_locked)
			{
				trigger_queue = true;
				channel_fully_locked = false;
			}
		}
		JVX_UNLOCK_MUTEX(safeAccessChannel);
	}
	if (trigger_queue)
	{
		jvx_message_queue_trigger_queue(theMQueue);
	}
	return res;
}

jvxErrorType 
CjvxGenericRS232Device::check_pending_uid(jvxSize uId, jvxSize mTp, oneMessage_hdr** ptr_hdr)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	JVX_LOCK_MUTEX(safeAccessChannel);
	auto elm = mpMessages.find(uId);
	if (elm != mpMessages.end())
	{
		if (!elm->second->expectResponse)
		{
			res = JVX_ERROR_UNSUPPORTED;
		}
		else
		{
			if (elm->second->stat == JVX_GENERIC_RS232_STATUS_SENT)
			{
				if (elm->second->tp == mTp)
				{
					if (ptr_hdr)
					{
						*ptr_hdr = elm->second;
					}
					res = JVX_NO_ERROR;
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
			else
			{
				// std::cout << "Checking for status uid <" << elm->second.hdr.uId << "> - mid >" << elm->second.hdr.mId << "> showed that message has not been sent." << std::endl;
				res = JVX_ERROR_WRONG_STATE;
			}
		}
	}
	JVX_UNLOCK_MUTEX(safeAccessChannel);
	return res;
}
// ========================================================================================
// Callbacks
// ========================================================================================

jvxErrorType
CjvxGenericRS232Device::callback_message_queue_message_in_queue_ready(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_queue, jvxHandle* privateData_message, jvxInt64 timestamp_us)
{
	CjvxGenericRS232Device* this_pointer = (CjvxGenericRS232Device*)privateData_queue;
	if (this_pointer)
	{
		return(this_pointer->cb_message_queue_message_in_queue_ready(contextId, fld, szFld, privateData_message, timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
CjvxGenericRS232Device::callback_message_queue_message_in_queue_timeout(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_queue, jvxHandle* privateData_message, jvxInt64 timestamp_us)
{
	CjvxGenericRS232Device* this_pointer = (CjvxGenericRS232Device*)privateData_queue;
	if (this_pointer)
	{
		return(this_pointer->cb_message_queue_message_in_queue_timeout(contextId, fld, szFld, privateData_message, timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

// Non-static
jvxErrorType
CjvxGenericRS232Device::cb_message_queue_message_in_queue_ready(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_message, jvxInt64 timestamp_us)
{
	jvxErrorType res = JVX_NO_ERROR;
	oneMessage_hdr* messPtr = (oneMessage_hdr*)fld;
	std::string txtMessage;
	jvxBool postponeRequest = false;
	jvxBool retransmit = false;

	JVX_LOCK_MUTEX(safeAccessChannel);
	if (fullPingPong)
	{
		if (channel_fully_locked)
		{
			postponeRequest = true;
		}
		else
		{
			channel_fully_locked = true;
		}
	}
	JVX_UNLOCK_MUTEX(safeAccessChannel);
	if (postponeRequest)
	{
		
		JVX_LOCK_MUTEX(safeAccessChannel);
		auto elm = mpMessages.begin();
		for(; elm != mpMessages.end(); elm++)
		{
			if (elm->second->expectResponse)
			{
				if (elm->second->stat == JVX_GENERIC_RS232_STATUS_RETRANSMIT)
				{
					retransmit = true;
					messPtr = elm->second;
					if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_TIMING_SHIFT))
					{
						if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
						{
							jvx_lock_text_log(jvxrtst_bkp);
							jvxrtst << "::" << __FUNCTION__ << ": RETRANSMIT #" << elm->second->retrans_cnt << ", T0 + " << 
								(timestamp_us - messPtr->timestamp_enter_us) * 0.001 << " (from enter) msec => Update message uid <" <<
								elm->second->uId << "> - mid <" << elm->second->mId << "> to SENT." << std::endl;
							jvx_unlock_text_log(jvxrtst_bkp);
						}
					}
					elm->second->retrans_cnt++;
					break;
				}
			}
		}
		JVX_UNLOCK_MUTEX(safeAccessChannel);

		if (!retransmit)
		{
			// Channel is reserved, come back later
			return JVX_ERROR_POSTPONE;
		}
	}

	if (retransmit)
	{
		prepare_retransmit();
	}

	jvx_message_queue_get_timestamp_us(theMQueue, &messPtr->timestamp_sent_us);
	res = translate_message_token(messPtr, txtMessage);

	if (res == JVX_NO_ERROR)
	{
		if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_ALLOUTGOING_SHIFT))
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::" << __FUNCTION__ << ": Sending message to COM port: <" << jvx_prepareStringForLogfile(txtMessage) << ">." << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}
		}
		jvxByte* ptr = (jvxByte*)txtMessage.c_str();
		jvxSize sz = txtMessage.size();

		// First, handle the internal house keeping
		JVX_LOCK_MUTEX(safeAccessChannel);
		auto elm = mpMessages.find(messPtr->uId);
		if (elm != mpMessages.end())
		{
			if (elm->second->expectResponse)
			{
				if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_TIMING_SHIFT))
				{
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvx_lock_text_log(jvxrtst_bkp);
							jvxrtst << "::" << __FUNCTION__ << ": T0 + " << (timestamp_us - messPtr->timestamp_enter_us) * 0.001 << " (from enter) msec => Update message uid <" << 
								elm->second->uId << "> - mid <" << elm->second->mId << "> to SENT." << std::endl;
							jvx_unlock_text_log(jvxrtst_bkp);
					}
				}
				elm->second->stat = JVX_GENERIC_RS232_STATUS_SENT;
				elm->second->timestamp_sent_us = messPtr->timestamp_sent_us;
			}
			else
			{
				if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_TIMING_SHIFT))
				{
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvx_lock_text_log(jvxrtst_bkp);
						jvxrtst << "::" << __FUNCTION__ << ": T0 + " << 
							(timestamp_us - messPtr->timestamp_enter_us) * 0.001 << " msec => Removing message uid <" << 
							elm->second->uId << "> - mid <" << elm->second->mId << "> from list of pending events." << std::endl;
						jvx_unlock_text_log(jvxrtst_bkp);
					}
				}

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(elm->second, jvxByte);
				mpMessages.erase(elm);
				if (channel_fully_locked)
				{
					channel_fully_locked = false;
				}
			}
		}
		else
		{
			// Message is not in the list, silently accept this
		}
		JVX_UNLOCK_MUTEX(safeAccessChannel);

#ifdef JVX_SHOW_QUICK_TIMING
		jvxTick send = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTStamp);
		std::cout << "::" << send * 0.001 * 0.001 << ", sending message " << txtMessage << std::endl;
#endif

		if (output_rs232_on_stdcout)
		{
			std::string txt(reinterpret_cast<char*>(ptr), sz);
			std::cout << "RS232 ON COUT :: <" << txt << ">" << std::endl;
		}

		// Send it here and avoid race condition!
		theRs232Tool->sendMessage_port(idDevice, ptr, &sz, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
	}
	else
	{
		if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_ERRORS_SHIFT))
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::" << __FUNCTION__ << ": Failed to translate message in submodule." << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}
		}
	}

	if (retransmit)
	{
		assert(res == JVX_NO_ERROR);
		res = JVX_ERROR_POSTPONE;
	}
	return(res);
}

jvxErrorType
CjvxGenericRS232Device::cb_message_queue_message_in_queue_timeout(jvxSize contextId, jvxHandle* fld, jvxSize szFld, jvxHandle* privateData_message, jvxInt64 timestamp_us)
{
	// If we are here, a message has been in the message queue for longer than the allowed time
	return cb_message_queue_message_in_queue_ready(contextId, fld, szFld, privateData_message, timestamp_us);
/*
	jvxErrorType res = JVX_NO_ERROR;
	oneMessage_hdr* messPtr = (oneMessage_hdr*)fld;

	JVX_LOCK_MUTEX(safeAccessChannel);
	auto elm = mpMessages.find(messPtr->uId);
	if (elm != mpMessages.end())
	{
		std::cout << "Remove message uid <" << elm->second.hdr.uId << "> - mid >" << elm->second.hdr.mId << "> from message queue due to m queue timeout." << std::endl;
		deallocate_memory_message(&elm->second);
		mpMessages.erase(elm);
	}
	else
	{
		// Message is not in the list, silently accept this
	}
	JVX_UNLOCK_MUTEX(safeAccessChannel);
	
	return(res);
	*/
}

// =======================================================================================
// =======================================================================================

// ========================================================================
// THREAD FOR OBSERVATIONS
// ========================================================================

jvxErrorType
CjvxGenericRS232Device::cb_thread_timer_expired(jvxHandle* privateData_queue, jvxInt64 timestamp_us, jvxSize* timeout_next)
{
	if (privateData_queue)
	{
		CjvxGenericRS232Device* this_pointer = (CjvxGenericRS232Device*)privateData_queue;
		return(this_pointer->callback_thread_timer_expired(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
CjvxGenericRS232Device::callback_thread_timer_expired(jvxInt64 timestamp_us)
{
	jvxSize deltaT = 0;
	jvxSize deltaT_msec = 0;
	jvxInt64 tstamp = 0;
	jvxBool trigger_queue = false;
	oneMessage_hdr* mess = NULL;
	jvxBool report_remove = false;
	jvxSize idIdentify = JVX_SIZE_UNSELECTED;
	jvx_message_queue_get_timestamp_us(theMQueue, &tstamp);

	while (1)
	{
		jvxBool removedone = false;
		JVX_LOCK_MUTEX(safeAccessChannel);
		auto elm = mpMessages.begin();
		for (; elm != mpMessages.end(); elm++)
		{
			if(elm->second->stat == JVX_GENERIC_RS232_STATUS_SENT)
			{
				// Check those messages in SENT mode only
				deltaT = tstamp - elm->second->timestamp_sent_us;
				deltaT_msec = deltaT / 1000;
				if (deltaT_msec > observer.timeout_message_response)
				{
					if (elm->second->retrans_cnt < num_retrans_max)
					{
						if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_TIMING_SHIFT))
						{
							if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
							{
								jvx_lock_text_log(jvxrtst_bkp);
								jvxrtst << "::" << __FUNCTION__ << ": deltat = " << deltaT_msec << " (from send) msec: Retransmit message uid <" << elm->second->uId <<
									"> - mid <" << elm->second->mId << "> (RETRANSMISSION #" << elm->second->retrans_cnt << ")." << std::endl;
								jvx_unlock_text_log(jvxrtst_bkp);
							}
						}

						if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_TIMEOUT_MESSAGES_COUT))
						{
							// I need to see this message on the console to find problems more quickly
							std::cout << "::" << __FUNCTION__ << ": deltat = " << deltaT_msec << " (from send) msec: Retransmit message uid <" << elm->second->uId <<
								"> - mid <" << elm->second->mId << "> (retransmission nr." << elm->second->retrans_cnt << ")." << std::endl;
						}
						trigger_queue = true;
						elm->second->stat = JVX_GENERIC_RS232_STATUS_RETRANSMIT;
						// Counter will be increment in message loop
					}
					else
					{
						if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_TIMING_SHIFT))
						{
							if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
							{
								jvx_lock_text_log(jvxrtst_bkp);
								jvxrtst << "::" << __FUNCTION__ << ": deltat = " << deltaT_msec << " (from send) msec: Remove message uid <" << elm->second->uId <<
									"> - mid <" << elm->second->mId << "> from message queue due to response timeout." << std::endl;
								jvx_unlock_text_log(jvxrtst_bkp);
							}
						}

						if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_TIMEOUT_MESSAGES_COUT))
						{
							// I need to see this message on the console to find problems more quickly
							std::cout << "::" << __FUNCTION__ << ": deltat = " << deltaT_msec << "msec: Remove message uid <" <<
								elm->second->uId << "> - mid <" << elm->second->mId << "> from message queue due to response timeout." << std::endl;
						}

						mess = elm->second;

						this->handle_cancelled_message(mess, &idIdentify);
						JVX_DSP_SAFE_DELETE_FIELD_TYPE(mess, jvxByte);

						mpMessages.erase(elm);

						if (channel_fully_locked)
						{
							channel_fully_locked = false;
							trigger_queue = true;
						}
						removedone = true;
					}
					break;
				}
			}
		}
		
		JVX_UNLOCK_MUTEX(safeAccessChannel);
		if (removedone == false)
		{
			break;
		}
	}

	if (trigger_queue)
	{
		jvx_message_queue_trigger_queue(theMQueue);
	}
	//
	
	this->report_observer_timeout();

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericRS232Device::clear_send_messages()
{
	// Clear also all pending messages
	jvxErrorType res = jvx_message_queue_trigger_clear_messages_queue(theMQueue);
	return res;
}

jvxErrorType
CjvxGenericRS232Device::clear_inout_matching()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idIdentify = JVX_SIZE_UNSELECTED;
	JVX_LOCK_MUTEX(safeAccessChannel);
	while (1)
	{
		jvxBool removedone = false;
		auto elm = mpMessages.begin();
		for (; elm != mpMessages.end(); elm++)
		{
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "::" << __FUNCTION__ << ": Remove message uid <" << elm->second->uId <<
					"> - mid <" << elm->second->mId << "> from message queue due to clear request." << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}
			

			if (jvx_bitTest(output_flags, JVX_GENERIC_RS232_OUTPUT_TIMEOUT_MESSAGES_COUT))
			{
				// I need to see this message on the console to find problems more quickly
				std::cout << "::" << __FUNCTION__ << ": Remove message uid <" <<
					elm->second->uId << "> - mid <" << elm->second->mId << "> from message queue due to clear request." << std::endl;
			}

			oneMessage_hdr* mess = elm->second;
			if (mess->expectResponse)
			{
				this->handle_cancelled_message(mess, &idIdentify);
			}

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(mess, jvxByte);

			mpMessages.erase(elm);
			removedone = true;
			break;
		}

		if (removedone == false)
		{
			break;
		}
	}
	if (channel_fully_locked)
	{
		channel_fully_locked = false;
	}

	cleared_messages_hook();
	JVX_UNLOCK_MUTEX(safeAccessChannel);
	return res;
}

// ======================================================================================================

jvxErrorType
CjvxGenericRS232Device::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,  const char* filename, jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;

	if (_common_set_min.theState >= JVX_STATE_SELECTED)
	{
		CjvxGenericRs232Device_pcg::put_configuration_all(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);
		for (i = 0; i < warns.size(); i++)
		{
			std::cout << "::" << __FUNCTION__ << ": Warning when loading confiugration from file " << filename << ": " << warns[i] << std::endl;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericRS232Device::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	if (_common_set_min.theState >= JVX_STATE_SELECTED)
	{
		CjvxGenericRs232Device_pcg::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);
	}
	return(JVX_NO_ERROR);
}

