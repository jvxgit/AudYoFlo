#ifndef __TJVX_NETWORK_H__
#define __TJVX_NETWORK_H__

// NOTE byte order is little-endian

#define ETH_MIN_SIZE_RAW_SOCKET 64

typedef enum
{
	JVX_PROTOCOL_GLOBAL_ERROR,
	JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK,
	JVX_PROTOCOL_TYPE_PROPERTY_STREAM,
	JVX_PROTOCOL_TYPE_GENERIC_MESSAGE,
	JVX_PROTOCOL_TYPE_REMOTE_CALL,
	JVX_PROTOCOL_TYPE_STRING,
	JVX_PROTOCOL_TYPE_RAW_DATA_STREAM,
	JVX_PROTOCOL_TYPE_RAW_PROPERTY_STREAM,
	JVX_PROTOCOL_TYPE_SIMPLE_AUDIO_DEVICE_LINK
} jvxProtocolType;

// DEFINTIONS FOR PROTOCOL JVX_AUDIO_DEVICE_LINK

#define JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK 0xC000
#define JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK 0x3FFF

#define JVX_ADV_DECRIPTION_LENGTH 32

typedef enum
{
	JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER = 0x4000,
	JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST = 0x8000,
	JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_SPECIFY = 0xC000,
	JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE = 0x0000
} jvxAdvLinkProtocolMessagePurpose;

typedef enum
{
	JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_SOURCE = 0x0, // <- all buffer transfer are triggerd by this device
	JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_DESTINATION = 0x1 // <- this device expects the buffers to be transferred from other side 
} jvxAdvLinkProtocolClockTriggerRole;

typedef enum
{
	JVX_PROTOCOL_ADV_CHANNEL_FRAME_TYPE_NONE,
	JVX_PROTOCOL_ADV_CHANNEL_FRAME_TYPE_NON_INTERLEAVED,
	JVX_PROTOCOL_ADV_CHANNEL_FRAME_TYPE_INTERLEAVED
}  jvxAdvLinkProtocolChannelFrameType;

typedef enum 
{
	JVX_PROTOCOL_ADVLINK_ERROR_RESET = 0,

	JVX_PROTOCOL_ADVLINK_INIT = 1,
	JVX_PROTOCOL_ADVLINK_TERM = 2,

	// Following valid in state JVX_STATE_INIT
	JVX_PROTOCOL_ADVLINK_DESCR = 16,
	JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_MASK = 17,
	JVX_PROTOCOL_ADVLINK_SPECIFY_MODE = 18,
	
	// From state JVX_STATE_INIT to JVX_STATE_ACTIVE
	JVX_PROTOCOL_ADVLINK_ACTIVATE = 32, // Specifies used clock trigger setting
	JVX_PROTOCOL_ADVLINK_DEACTIVATE = 33,

	// Following valid in state JVX_STATE_INIT
	JVX_PROTOCOL_ADVLINK_SRATE = 48,
	JVX_PROTOCOL_ADVLINK_BSIZE = 49,
	JVX_PROTOCOL_ADVLINK_FORMAT = 50,
	JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_SLAVE_TO_MASTER = 51,
	JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_MASTER_TO_SLAVE = 52,
	JVX_PROTOCOL_ADVLINK_CHANNEL_FRAME_TYPE = 53,

	// From state JVX_STATE_ACTIVE to JVX_STATE_PROCESSING
	JVX_PROTOCOL_ADVLINK_START = 64,

	// From state JVX_STATE_PROCESSING to JVX_STATE_ACTIVE
	JVX_PROTOCOL_ADVLINK_STOP = 65,

	// state processing
	JVX_PROTOCOL_ADVLINK_SEND_DATA = 80,

	// all states
	JVX_PROTOCOL_ADVLINK_STATUS = 128
	
} jvxAdvLinkProtocolCommandType;

typedef enum
{
	JVX_PROTOCOL_ADVLINK_MODE_PCM_IN_PCM_OUT = 0,
	JVX_PROTOCOL_ADVLINK_MODE_PDM_IN_PCM_OUT = 1,
	JVX_PROTOCOL_ADVLINK_MODE_AUDIO_CODED_OPUS = 2,
	JVX_PROTOCOL_ADVLINK_MODE_LIMIT
} jvxAdvLinkOperationMode;

typedef enum
{
	JVX_PROTOCOL_GM_ERROR_RESET = 0,

	JVX_PROTOCOL_GM_REQUEST = 1,
	JVX_PROTOCOL_GM_RESPONSE = 2,

	JVX_PROTOCOL_GM_PUSH = 3
} jvxGmProtocolCommandType;

typedef enum
{
	JVX_PROTOCOL_RC_ERROR_RESET = 0

} jvxRcProtocolCommandType;

// Variables defined by jvx software repository:
// 1) Variable definitions that are used in the dsp base library as well in all CPP components: 
//		-> jvx/base/sources/jvxLibraries/jvx-dsp-base/include/jvx_dataformats.h
// 2) Variable definitions that are used in the CPP component system only since those may be CPP 
// 	  code or may be useful only in CPP components (example: jvxState)
//  	-> jvx/base/software/typedefs/TjvxTypes.h
// 3) All OS specific variable type definitions: 
//		-> jvx/base/sources/jvxLibraries/jvx-system-base-c/include/platform/<OS>/jvx_system.h"
//	  	(with OS representing win32, Linux, MacOSx or ios at the moment)

// We need to make sure that struct element boundaries are not extended by 32 bit or similar
// alignments caused by the compiler
#ifdef JVX_OS_WINDOWS
#pragma pack(push, 1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

// Raw socket header. Note that this struct is only used in case of raw sockets,
// in UDB and TCP, there is no raw socket header 
typedef struct
{
	jvxUInt8  dest_mac[6]; // destination MAC address
	jvxUInt8  src_mac[6];  // source MAC address
	jvxUInt8 proto_ident[2];   // 0x4a58 = 'J''X'
} jvxRawSocketHeader;

// Javox Protocol header: Contains just a 2-byte family id
typedef struct
{
	jvxUInt16 proto_family;
} jvxProtocolFamilyHeader; 

// Subtype adv protocol header: extends family header by 2 and 4 byte fields 
typedef struct
{
	jvxProtocolFamilyHeader fam_hdr;
	
	jvxUInt16 purpose;
	jvxUInt32 paketsize;
} jvxProtocolHeader;

// #############################################################
// AUDIO LINK DEVICE
// JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK
// #############################################################

// Definition of a description frame
typedef struct
{
	jvxProtocolHeader header;
	
	jvxUInt8 description[JVX_ADV_DECRIPTION_LENGTH];
} jvxAdvProtocolDescriptionP;

// Frame type for simple passing of different parameters. Note that there
// may also be more that one entry - indicated by packet length
typedef struct
{
	jvxProtocolHeader header;
	
	jvxUInt32 first_transfer_load;
} jvxAdvProtocolTransferP;

typedef struct
{
	// Bit 0 .. Bit 61 for up to 62 channels
	// Bit 62: indicate first buffer, should comply with sequence counter
	// Bit 63: indicate first buffer, should comply with followingsequence counter
	jvxUInt64 channel_mask; 
	jvxUInt32 sequence_id;
} jvxAdvProtocolDataChunkHeader;

typedef struct
{
	jvxProtocolHeader header;
	jvxAdvProtocolDataChunkHeader adv_data_header;
} jvxAdvProtocolDataHeader;

// Frame type for a single audio frame
typedef struct
{
	/*
	jvxProtocolHeader header;
	jvxAdvProtocolDataChunkHeader adv_data_header;
	*/
	jvxAdvProtocolDataHeader dheader;
	jvxUInt8 first_byte_transfer;
} jvxAdvProtocolDataP;

typedef union
{
	jvxAdvProtocolDescriptionP descr;
	jvxAdvProtocolTransferP transfer;
	jvxAdvProtocolDataP aframe;
} jvxAdvProtocolUnion;


// Subtype adv protocol header: extends family header by 2 and 4 byte fields 
typedef struct
{
	jvxProtocolFamilyHeader fam_hdr;
	
	jvxUInt16 propid;
	jvxUInt32 dataformat;
	jvxUInt32 numberElements;
	jvxUInt32 offsetElements;

} jvxPropStreamProtocolHeader;


/*
 * Why I would not use unions: If you use the 
 * sizeof operator it will return the maximum size -
 * even for short frame data types
 
typedef struct {
	UInt8  dest_mac[6]; // destination MAC address
	UInt8  src_mac[6];  // source MAC address
	UInt16 packed_id;   // 0x4a58 = 'J''X'
	UInt16 protocol;    // protocol identifier (Jvx_protocol)
	UInt16 command;     // command identifier (Avdlink_id); MSB distinguishes between request (0) and return (1)
	UInt16 len;         // payload length
	// frame overlay structures
	union {
		jvx_init_frame       init_frame;
		jvx_active_frame     active_frame;
		jvx_processing_frame processing_frame;
	};
} jvx_frame;
*/

// #############################################################
// GENERIC MESSAGE
// JVX_PROTOCOL_TYPE_GENERIC_MESSAGE
// #############################################################

typedef struct
{
	jvxProtocolHeader gm_header;

	jvxUInt16 id;
	jvxUInt16 sub_id;
} jvxGmProtocolDescription;

typedef struct
{
	jvxGmProtocolDescription gm_header;

	jvxUInt8 first_byte_transfer;
} jvxGmProtocolDescriptionP;

// #############################################################
// REMOTE CALL
// JVX_PROTOCOL_TYPE_REMOTE_CALL
// #############################################################

typedef enum
{
	JVX_RC_ERROR = 0,
	JVX_RC_EXCHANGE_START = 1, 
	JVX_RC_DATA_TRANSFER_IN_PRE = 2, 
	JVX_RC_DATA_TRANSFER_IN = 3, 
	JVX_RC_DATA_TRANSFER_OUT_PRE = 4, 
	JVX_RC_TRIGGER_ACTION = 5, 
	JVX_RC_DATA_RESPONSE_OUT = 6, 
	JVX_RC_EXCHANGE_COMPLETE = 7
	
} jvxRcMessageType;

/*
typedef struct
{
	jvxProtocolHeader pHeader;
	jvxRcProtocolLoadHeader load;
} jvxRcProtocolLoadHeader;
*/

// All following structs will be placed right behind the jvxRcProtocolLoadHeader part with no
// dedicated sub struct defined. In the code, all access will be done by pointer shifting

// One transmitted parameter description

#define JVX_RC_LENGTH_REQUEST_TEXTFIELD 32

typedef struct
{
	jvxProtocolHeader loc_header;
	jvxUInt16 processingId;
	jvxUInt16 processingCounter;
	jvxUInt16 crc;
} jvxRcProtocol_simple_message;

typedef struct
{
	jvxRcProtocol_simple_message transfer;
	char description[JVX_RC_LENGTH_REQUEST_TEXTFIELD];
	jvxUInt16 numInParams;
	jvxUInt16 numOutParams;	
} jvxRcProtocol_start_message;

typedef struct
{
	jvxRcProtocol_simple_message transfer;
	jvxUInt16 success;
} jvxRcProtocol_response_message;

typedef struct
{
	jvxRcProtocol_simple_message transfer;
	jvxUInt16 param_id;
	jvxUInt16 segy_id;
} jvxRcProtocol_data_message;

typedef struct
{
	jvxRcProtocol_data_message message;
	jvxUInt8 firstByteLoad;
} jvxRcProtocol_data_message_with_load;

typedef struct
{
	jvxUInt8 format;
	jvxUInt8 float_precision_id;
	jvxUInt16 seg_length_y;
	jvxUInt16 seg_length_x;
} jvxRcProtocol_parameter_description;

typedef struct
{
	jvxRcProtocol_simple_message transfer;
	jvxUInt16 param_id;
	jvxRcProtocol_parameter_description description;
} jvxRcProtocol_data_description_message;

typedef struct
{
	jvxRcProtocol_response_message response;
	jvxUInt16 err_code;
	jvxUInt16 err_id;
	char error[JVX_RC_LENGTH_REQUEST_TEXTFIELD];
} jvxRcProtocol_error_response_message;

// #############################################################
// STRING ??
// JVX_PROTOCOL_TYPE_STRING
// #############################################################

// #############################################################
// RAW DATASTREAM
// JVX_PROTOCOL_TYPE_RAW_STREAM
// #############################################################
typedef enum
{
	JVX_RS_TRANSFER_FIELD = 0
} jvxRsType; 

typedef struct
{
	jvxProtocolHeader loc_header;
	jvxUInt16 stream_id;
	jvxUInt16 packet_tstamp;

} jvxRawStreamHeader;

// #############################################################
// PROPERTY DATASTREAM
// JVX_PROTOCOL_TYPE_PROPERTY_STREAM
// #############################################################

typedef enum
{
	JVX_PS_CONFIGURE_PROPERTY_OBSERVATION = 0,
	JVX_PS_ADD_PROPERTY_TO_OBSERVE = 1,
	JVX_PS_REMOVE_PROPERTY_TO_OBSERVE = 2,
	JVX_PS_TRIGGER_PROPERTY_TRANSFER = 3,
	JVX_PS_SEND_LINEARFIELD = 4,
	JVX_PS_SEND_CIRCFIELD = 5,
	JVX_PS_SEND_INFORM_CHANGE = 6,
	JVX_PS_SEND_INFORM_SYSTEM_CHANGE = 7,
	JVX_PS_SEND_INFORM_PROPERTY_CHANGE = 8,
	JVX_PS_SEND_INFORM_SEQUENCER_EVENT = 9
} jvxPsType;

// ================================================================================

typedef struct
{
	jvxProtocolHeader loc_header;

	jvxUInt16 tick_msec;
	jvxUInt16 ping_count;

	jvxUInt32 user_specific;
} jvxPropertyConfigurePropertySend;


typedef struct
{
	jvxProtocolHeader loc_header;

	jvxUInt16 errcode;
	jvxUInt16 reserved;

	jvxUInt32 user_specific;
} jvxPropertyConfigurePropertySend_response;

// ================================================================================

typedef struct
{
	jvxProtocolHeader loc_header;
	jvxUInt16 property_id;
	jvxUInt32 user_specific;
} jvxPropertyTriggerPropertyTransfer;

// ================================================================================

typedef struct
{
	jvxProtocolHeader loc_header;

	jvxUInt16 component_type;
	jvxUInt16 component_slot;
	
	jvxUInt16 component_subslot;
	jvxUInt16 state_active;

	jvxUInt16 property_offset;
	jvxUInt8 property_format;
	jvxUInt8 property_dec_hint_tp;

	jvxUInt32 property_num_elements;

	jvxUInt32 user_specific;

	//jvxUInt16 state_reserved;

	jvxUInt32 param0;
	jvxUInt32 param1;

	jvxUInt16 cond_update; // Can be "on change", "timeout", "request"
	jvxUInt16 param_cond_update; // Further specification

	jvxUInt16 cnt_report_disconnect;
	jvxUInt16 requires_flow_control;
	jvxUInt16 reserved1;

	jvxUInt16 priority;
	jvxUInt16 reserved2;
	jvxUInt32 port;

	jvxUInt32 num_emit_min;
	jvxUInt32 num_emit_max;

} jvxPropertyPropertyObserveHeader;

typedef struct
{
	jvxProtocolHeader loc_header;

	jvxUInt16 errcode;
	jvxUInt16 stream_id;
	jvxUInt32 user_specific;

	jvxUInt16 component_type;
	jvxUInt16 component_slot;
	jvxUInt16 component_subslot;

} jvxPropertyPropertyObserveHeader_response;

typedef struct
{
	jvxProtocolHeader loc_header;

	jvxUInt16 component_type;
	jvxUInt16 component_slot;
	jvxUInt16 component_subslot;
	jvxUInt16 cat;
	jvxUInt32 prop_id;
} jvxPropertyChangedHeader;

#define JVX_SEQUENCER_EVENT_DESRIPTION_STR_LENGTH 256

typedef struct
{
	jvxProtocolHeader loc_header;

	jvxUInt64 seq_event_mask;
	char seq_step_description[JVX_SEQUENCER_EVENT_DESRIPTION_STR_LENGTH];
	jvxUInt16 seq_id;
	jvxUInt16 seq_step_id;
	jvxUInt16 seq_queue_tp;
	jvxUInt64 seq_elm_tp;
	jvxUInt16 seq_step_fid;
	jvxUInt16 seq_oper_state;
} jvxSequencerEventHeader;
// ================================================================================

typedef struct
{
	jvxProtocolHeader loc_header;
	jvxUInt16 stream_id;
	jvxUInt32 user_specific;
} jvxPropertyRemoveObserveHeader;

typedef struct
{
	jvxProtocolHeader loc_header;
	jvxUInt16 errcode;
	jvxUInt32 user_specific;
} jvxPropertyRemoveObserveHeader_response;

// ================================================================================

typedef struct
{
	jvxRawStreamHeader raw_header;
	
	jvxUInt32 property_num_elements;
	jvxUInt16 property_offset;

	jvxUInt8 property_format;
	jvxUInt8 property_type;

	jvxUInt32 param0;
	jvxUInt32 param1;

	jvxUInt8 requires_flow_response;
	jvxUInt8 reserved0;

	jvxUInt16 reserved1;

} jvxPropertyStreamHeader;

typedef struct
{
	jvxRawStreamHeader raw_header;
	jvxUInt16 errcode;
	jvxUInt16 reserved;
} jvxRawStreamHeader_response;
// ================================================================================

typedef enum
{
	JVX_PROTOCOL_SIMPLEDEVICE_ERROR_RESET = JVX_PROTOCOL_ADVLINK_ERROR_RESET,

	JVX_PROTOCOL_SIMPLEDEVICE_CONFIGURE = JVX_PROTOCOL_ADVLINK_INIT,
	
	/*
	JVX_PROTOCOL_ADVLINK_TERM = 2,

	// Following valid in state JVX_STATE_INIT
	JVX_PROTOCOL_ADVLINK_DESCR = 16,
	JVX_PROTOCOL_ADVLINK_TRIGGER_ROLE_MASK = 17,
	JVX_PROTOCOL_ADVLINK_SPECIFY_MODE = 18,

	// From state JVX_STATE_INIT to JVX_STATE_ACTIVE
	JVX_PROTOCOL_ADVLINK_ACTIVATE = 32, // Specifies used clock trigger setting
	JVX_PROTOCOL_ADVLINK_DEACTIVATE = 33,

	// Following valid in state JVX_STATE_INIT
	JVX_PROTOCOL_ADVLINK_SRATE = 48,
	JVX_PROTOCOL_ADVLINK_BSIZE = 49,
	JVX_PROTOCOL_ADVLINK_FORMAT = 50,
	JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_SLAVE_TO_MASTER = 51,
	JVX_PROTOCOL_ADVLINK_NUM_CHANNELS_MASTER_TO_SLAVE = 52,
	JVX_PROTOCOL_ADVLINK_CHANNEL_FRAME_TYPE = 53,
	*/

	// From state JVX_STATE_ACTIVE to JVX_STATE_PROCESSING
	// JVX_PROTOCOL_SIMPLEDEVICE_START = JVX_PROTOCOL_ADVLINK_START,

	// From state JVX_STATE_PROCESSING to JVX_STATE_ACTIVE
	// JVX_PROTOCOL_SIMPLEDEVICE_STOP = JVX_PROTOCOL_ADVLINK_STOP,

	// state processing
	JVX_PROTOCOL_SIMPLEDEVICE_SEND_DATA = JVX_PROTOCOL_ADVLINK_SEND_DATA,

	// all states
	JVX_PROTOCOL_SIMPLEDEVICE_STATUS = JVX_PROTOCOL_ADVLINK_STATUS

} jvxRemoteDeviceProtocolCommandType;

typedef struct 
{
	jvxProtocolHeader header;

	jvxUInt8 description[JVX_ADV_DECRIPTION_LENGTH];
	jvxUInt32 srate;
	jvxUInt32 bsize;
	jvxUInt16 channels_send;
	jvxUInt16 channels_recv;
	jvxUInt16 format_group;
	jvxUInt16 format;

	// Flag 0: jvxData is double precision
	// Flag 1: jvxData is single precision
	jvxUInt16 cpOptionFlags; 
} jvxRemoteDeviceConfig;


#ifdef JVX_OS_WINDOWS
#pragma pack(pop)
#else
#pragma pack(pop)
#endif 

#endif /* __TJVX_NETWORK_H__ */

