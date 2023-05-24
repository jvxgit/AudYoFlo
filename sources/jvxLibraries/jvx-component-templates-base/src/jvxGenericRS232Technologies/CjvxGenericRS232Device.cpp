
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

CjvxGenericRS232Device::CjvxGenericRS232Device(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxGenericConnectionDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	// , jvxrtst_local(&jvxrtst_bkp_local.jvxos)
{
}

CjvxGenericRS232Device::~CjvxGenericRS232Device()
{
}


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

void 
CjvxGenericRS232Device::select_connection_specific()
{
	jvxSize i;
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

void
CjvxGenericRS232Device::unselect_connection_specific()
{
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
}

jvxErrorType
CjvxGenericRS232Device::activate_connection_port()
{
	

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

	return this->theConnectionTool->start_port(idDevice, &cfg, JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_CONFIG, static_cast<IjvxConnection_report*>(this));
}

jvxErrorType
CjvxGenericRS232Device::deactivate_connection_port()
{
	// Last chance to transmit a "goodbye message"
	this->goodbye_connection();

	this->theConnectionTool->stop_port(idDevice);

	if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
	{
		jvx_lock_text_log(jvxrtst_bkp);
		jvxrtst << "::" << __FUNCTION__ << ": " << "stopping connection on COM port <" << idDevice << ">," << std::endl;
		jvx_unlock_text_log(jvxrtst_bkp);
	}


#if 0
	jvx_message_queue_stop(theMQueue);
	jvx_message_queue_terminate(theMQueue);
#endif
	CjvxGenericRs232Device_pcg::unregister_all(static_cast<CjvxProperties*>(this));
	CjvxGenericRs232Device_pcg::deallocate_all();
	return JVX_NO_ERROR;
}