#ifndef __TJVXDATAPROCESSOR_H__
#define __TJVXDATAPROCESSOR_H__

JVX_INTERFACE IjvxObject;
JVX_INTERFACE IjvxInputConnector;
JVX_INTERFACE IjvxOutputConnector;
JVX_INTERFACE IjvxConnectionMaster;

enum class jvxDataLinkDescriptorAllocFlags
{
	JVX_LINKDATA_ALLOCATION_FLAGS_NONE = 0,
	JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT = 1, // from start to end: Set this flag if the buffers as passed from caller provides buffer
	JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT = 2, // from start to end: Set this flag to prevent that chain is a zerocopy chain.
	JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT = 3, 
	// from end to start: Set this flag if we have detected a zerocopy chain. If the current input connector has allocated 
	// its own buffer/pipeline, it is not a zero copy shift pipeline. If the input connector only uses the buffer/pipeline from the
	// next chain element it is a zerocopy pipeline, Typically, each chain starts as a ZEROCOPY pipeline and ends as a non-zerocopy chain.
	JVX_LINKDATA_ALLOCATION_FLAGS_SPACE_USER_HINTS_SHIFT = 4, // If we set this, the receiver needs to allocated space for a concatenation of user hints - one per pipeline stage
	JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT = 5, // If we set this flag, the receiver must foresee fHeight buffers since the buffersize may be variable
	JVX_LINKDATA_ALLOCATION_FLAGS_THREAD_INIT_PRE_RUN = 6, // this flag indicates that the component requires a run of a transfer call prior to any processing IN the processing loop
	JVX_LINKDATA_ALLOCATION_FLAGS_ALLOW_FHEIGHT_INFO_SHIFT = 7 // If we set this flag, the sender checks for a fHeight constraint on "process_connect_start"
} ;

#define JVX_LINKDATA_ALLOCATION_MASK_FORWARD_ELEMENT_TO_ELEMENT ((1 << (int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT))

// The sender may have changed the buffer index and should inform the slave
typedef enum
{
	JVX_LINKDATA_ADDRESS_FLAGS_NONE = 0,
	JVX_LINKDATA_ADDRESS_FLAGS_INDEX_CHANGED = 0x1
} jvxDataLinkDescriptorAddressFlags;

/*
typedef enum
{
	JVX_PROCESSING_PURPOSE_UNSPECIFIC = 0x0,
	JVX_PROCESSING_PURPOSE_AUDIO_IO_N0_7 = 0x1,
	JVX_PROCESSING_PURPOSE_DATA_IO_N0_7 = 0x10
} jvxProcessingPurpose;
*/

#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "typedefs/TpjvxTypes_dataProcessorHintTypes.h"
#endif

typedef enum
{
	JVX_HINT_DESCRIPTOR_OFFLINE = 0x1
} jvxHintDescriptorTypeFlag;

typedef enum
{
	JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_NONE = 0,
	JVX_DATAPROCESSOR_HINT_DESCRIPTOR_NUMBER_LOST_FRAMES_INT32,
	JVX_DATAPROCESSOR_HINT_DESCRIPTOR_ASYNC_FRAME_COUNT_INT32,
	JVX_DATAPROCESSOR_HINT_DESCRIPTOR_PROCESSING_TYPE_CBITFIELD,
	JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_BWD_REFERENCE,
	JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_CONFIGURATION_TEXT_DESCRIPTION
#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "typedefs/TpjvxTypes_dataProcessorHintDescriptor.h"
#endif

} jvxDataProcessorHintDescriptorType;

typedef enum
{
	JVX_DATA_SYNC_FRAME_COUNT_TICK = 0x1, // jvxTick
	JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK = 0x2, // jvxTick
	JVX_DATA_SYNC_DATA_COUNT_TICK = 0x4,
	JVX_DATA_SYNC_DATA_TIMESTAMP_TICK = 0x8,
	JVX_DATA_SYNC_DATA_TIMESTAMP_FLOAT = 0x10
} jvxSyncInformationType;

struct jvxDataProcessorHintDescriptor
{
	jvxHandle* hint = nullptr;
	jvxDataProcessorHintDescriptorType hintDescriptorId = JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_NONE;
	jvxSize uniqueId = JVX_SIZE_UNSELECTED;
	jvxComponentIdentification assocHint = JVX_COMPONENT_UNKNOWN;
	jvxDataProcessorHintDescriptor* next = nullptr;
} ;

struct jvxDataProcessorHintDescriptorSearch
{
	jvxHandle* reference = nullptr;
	jvxDataProcessorHintDescriptorType hintDescriptorId = JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_NONE;
	jvxComponentIdentification assocHint = JVX_COMPONENT_UNKNOWN;
};

// Subformat flags and fields
#define JVX_DATAFORMAT_GROUP_INTERLEAVED_SHIFT 4
#define JVX_DATAFORMAT_GROUP_EXCLUSIVE_TYPE_MASK 0xF 

// Some additional flags for specifying buffers
#define JVX_BUFFER_FLAGS_TAKE_FROM_CALLER_SHIFT 0
#define JVX_BUFFER_FLAGS_TAKE_FROM_CALLER_MASK 1

struct jvxLinkDataDescriptor;

enum class jvxAddressLinkDataEntry
{
	JVX_ADDRESS_BUFFERSIZE_SHIFT = 0,
	JVX_ADDRESS_SAMPLERATE_SHIFT = 1,
	JVX_ADDRESS_NUM_CHANNELS_SHIFT = 2,
	JVX_ADDRESS_FORMAT_SHIFT = 3,
	JVX_ADDRESS_SUBFORMAT_SHIFT = 4,
	JVX_ADDRESS_SEGX_SHIFT = 5,
	JVX_ADDRESS_SEGY_SHIFT = 6,
	JVX_ADDRESS_DATAFLOW_SHIFT = 7,
	JVX_ADDRESS_FORMATSPEC_SHIFT = 8
};

// ==================================================================
// JVXLINKDATAATTACHED JVXLINKDATAATTACHED JVXLINKDATAATTACHED JVXLINKDATAATTACHED
// ==================================================================

class jvxLinkDataAttachedBuffer;

typedef jvxErrorType(*jvx_release_attached)(jvxHandle* priv, jvxLinkDataAttachedBuffer* elm);
typedef enum
{
	JVX_LINKDATA_ATTACHED_NONE,
	JVX_LINKDATA_ATTACHED_REPORT_UPDATE_NUMBER_LOST_FRAMES, // Number of lost frames
	JVX_LINKDATA_ATTACHED_STRING_DETECT // Check string and try to detect
} jvxLinkDataAttachedType;

class jvxLinkDataAttachedChain
{
protected: 
	jvxLinkDataAttachedType tp = JVX_LINKDATA_ATTACHED_NONE;
public:
	
	jvxLinkDataAttachedChain(jvxLinkDataAttachedType tpArg = JVX_LINKDATA_ATTACHED_NONE) : tp(tpArg)
	{};
	jvxLinkDataAttachedChain* next = nullptr;
	
	virtual jvxLinkDataAttachedBuffer* if_buffer() 
	{ 
		return nullptr; 
	};

	virtual jvxHandle* if_specific(jvxLinkDataAttachedType tpArg) 
	{ 
		return nullptr; 
	};
};

class jvxLinkDataAttachedBuffer: public jvxLinkDataAttachedChain
{
public:
	jvxLinkDataAttachedBuffer(jvxLinkDataAttachedType tpArg = JVX_LINKDATA_ATTACHED_NONE) : jvxLinkDataAttachedChain(tpArg) {};
	jvx_release_attached cb_release = nullptr;
	jvxHandle* priv = nullptr;

	virtual jvxLinkDataAttachedBuffer* if_buffer() override { return this; };
};

// T can be either jvxLinkDataAttachedChain or jvxLinkDataAttachedBuffer
// Type JVX_LINKDATA_ATTACHED_REPORT_UPDATE_NUMBER_LOST_FRAMES
class jvxLinkDataAttachedLostFrames: public jvxLinkDataAttachedBuffer
{
public:
	jvxLinkDataAttachedLostFrames() :jvxLinkDataAttachedBuffer(JVX_LINKDATA_ATTACHED_REPORT_UPDATE_NUMBER_LOST_FRAMES)
	{
	};

	jvxSize numLost = 0;
	virtual jvxHandle* if_specific(jvxLinkDataAttachedType tpArg) override
	{ 
		if(tpArg == JVX_LINKDATA_ATTACHED_REPORT_UPDATE_NUMBER_LOST_FRAMES)
		{
			return this;
		}
		return nullptr; 
	};
};

// Type JVX_LINKDATA_ATTACHED_STRING_DETECT
template <class T>
class jvxLinkDataAttachedStringDetect : public T
{
public:

	jvxLinkDataAttachedStringDetect(const char* descrArg, jvxHandle* dataArg) : 
		T(JVX_LINKDATA_ATTACHED_STRING_DETECT),
		descr(descrArg), data(dataArg)
	{};

	jvxHandle* data = nullptr;
	const char* descr = nullptr;

	virtual jvxHandle* if_specific(jvxLinkDataAttachedType tpArg)
	{
		if (tpArg == JVX_LINKDATA_ATTACHED_STRING_DETECT)
		{
			return this;
		}
		return nullptr;
	};
};

// ==================================================================
// ==================================================================

typedef enum
{
	JVX_LINKDATA_COMBINED_NONE = 0,
	JVX_LINKDATA_COMBINED_TSTAMP = 1
} jvxLinkDataCombinedInformationType;

struct jvxLinkDataCombinedInformation
{
	jvxUInt8 modified = c_false;
	jvxUInt8 type = JVX_LINKDATA_COMBINED_NONE;
	jvxUInt16 szFld = sizeof(jvxLinkDataCombinedInformation);
	jvxUInt32 offsetNext = 0;
};

// ==================================================================
// ==================================================================

struct jvxLinkDataDescriptor_con_link
{
	jvxSize uIdConn = JVX_SIZE_UNSELECTED;
	IjvxInputConnector* connect_to = nullptr;
	IjvxOutputConnector* connect_from = nullptr;
	IjvxConnectionMaster* master = nullptr;
	// jvxComponentIdentification tp_master = JVX_COMPONENT_UNKNOWN;
	//jvxDataLinkDescriptorAddressFlags address_flags;
	jvxBool allows_mt = false;
	jvxLinkDataAttachedChain* attached_chain_single_pass = nullptr; // We may attach information to the chain. This info will not be associated to a buffer but to a chain and will be reported as processed once completed
	//jvxLinkDataCombinedInformation** attached_chain_persist = nullptr;
};

struct jvxLinkDataDescriptor_ext
{
	jvxCBitField additional_flags = 0;
	jvxDataFormatGroup subformat = JVX_DATAFORMAT_GROUP_NONE;
	jvxSize segmentation_x = 0;
	jvxSize segmentation_y = 0;
};

// For more efficient processing, sometimes, we need a specific pointer allocation:
struct jvxLinkDataDescriptor_buf
{
	jvxSize prepend = 0; // Prepend a certain numbers of BYTES
	jvxSize append = 0; // Append a certain number of BYTES
	jvxSize align = 0; // Align buffers to a soecific BYTE ALIGNMENT
	jvxCBitField flags = 0; // Specify whatever will be necessary - future
	jvxSize sz = 0;
	jvxHandle*** raw = nullptr; // his is where the field starts WITH all these additiona args
	jvxSize **offset = nullptr; // Here is the real offset to the content data - may deviate from prepend due to alignment
	ldata_allocate f_alloc = nullptr;
	ldata_deallocate f_dealloc = nullptr;
	jvxHandle* f_priv = nullptr;
};
	
/**
 * This struct defines how the linear fields are arranged logically. A field of length (640 * 480) x 1 can be interpreted 
 * as a 2D pixelmap by x = 640 and y = 480. If the segmentation is a fheight, only part of the field is populated.
 * We find the valid pixels by first applying the segmentation followed by the fillheight constraint.
 */ //=======================================================================================================
struct jvxLinkDataDescriptor_segmentation
{
	// Segmentation: A value of UNSELECTED means that there is no segmentation, hence, x = buffersize
	jvxSize x = JVX_SIZE_UNSELECTED;
	
	// Segmentation: A value of UNSELECTED means that there is no segmentation, hence, y = 1
	jvxSize y = JVX_SIZE_UNSELECTED;
};

// Current fheight in buffer y direction
struct jvxLinkDataDescriptor_con_data
{
	//! Number of buffers to allocate. For buffer pipelining, we need more than 1 buffer!
	jvxSize number_buffers = 0;

	//! Field of buffers (deinterleaved always). buffers[buf_idx][channel][sample] 
	jvxHandle*** buffers = nullptr;

	//! This struct contains some additional extensions to, e.g. allow aligned buffers or HW specific buffers with allocator callbacks
	jvxLinkDataDescriptor_buf bExt;

	//! We may attach user hints. This is an old API and is superseded by the attached_.. pointers
	jvxDataProcessorHintDescriptor** user_hints = nullptr;

	//! Allocation flags, defined in enum jvxDataLinkDescriptorAllocFlags 
	jvxCBitField alloc_flags = 0;

	//! Attach information. Attached element will be reported as processed once no longer in use
	jvxLinkDataAttachedChain** attached_buffer_single = nullptr; 

	//! Buffer to hold one or more infos which persist - that is, they will be valid as long as processing is active
	jvxLinkDataCombinedInformation** attached_buffer_persist = nullptr; 

	// Current fheight in buffer x direction
	jvxLinkDataDescriptor_segmentation* fHeights = nullptr;
};

struct jvxLinkDataDescriptor_con_user
{
	jvxDataProcessorHintDescriptor* chain_spec_user_hints = nullptr;
};

struct pipeline_reserve
{
	jvxSize idProcess = JVX_SIZE_UNSELECTED;
	jvxSize idStage_to_be_processed_by_stage = 0;
};

struct jvxLinkDataDescriptor_con_pipeline
{
#ifndef JVX_COMPILE_SMALL
	jvx_try_lock_buffer do_try_lock = nullptr;
	jvx_lock_buffer do_lock = nullptr;
	jvx_unlock_buffer do_unlock = nullptr;
	jvxHandle* lock_hdl = nullptr;
	pipeline_reserve* reserve_buffer_pipeline_stage = nullptr;
#endif
	jvxSize num_additional_pipleline_stages = 0;
	jvxSize* idx_stage_ptr = nullptr;
	const char* debug_ptr = nullptr;
};

struct jvxLinkDataDescriptor_con_sync
{
	jvxHandle* reserve_timestamp = nullptr;
	jvxUInt32 type_timestamp = 0;
};

struct jvxLinkDataDescriptor_con_compat
{
	jvxSize number_buffers = 0;
	jvxSize buffersize = 0;
	jvxSize number_channels = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxSize rate = 0;
	jvxHandle*** from_receiver_buffer_allocated_by_sender = nullptr;

	jvxLinkDataDescriptor_ext ext;
	jvxLinkDataDescriptor_buf bExt;
	jvxDataProcessorHintDescriptor* user_hints = nullptr;
	jvxSize idx_receiver_to_sender = 0;
};

struct jvxLinkDataDescriptor_con_params
{
	//! Size of the (maximum buffer). Typically the buffer is fully filled but it may be filled to a degree specified by segmentation_x and segmentation_y
	jvxSize buffersize = 0;

	//! Rate of the sample arrival if operated at a fixed rate
	jvxSize rate = 0;

	//! Number channels. Channels are in non-interleaved format and are associated to one buffer each. If interleaved data, you need to specify segmentation_y and setup subformat
	jvxSize number_channels = 0;

	// Data format to describe each sample
	jvxDataFormat format = JVX_DATAFORMAT_NONE;

	// Format group
	jvxDataFormatGroup format_group = JVX_DATAFORMAT_GROUP_NONE;

	// Dataflow capability
	jvxDataflow data_flow = JVX_DATAFLOW_PUSH_ACTIVE;

	// A buffer of size buffersize is segmented into x and y according to:
	// -------- segmentation_x -------->
	// | x x x x x x x x x x x x x x x x
	// | x x x x x x x x x x x x x x x x
	// | x x x x x x x x x x x x x x x x
	// | x x x x x x x x x x x x x x x x
	// | x x x x x x x x x x x x x x x x
	// | x x x x x x x x x x x x x x x x
	// s x x x x x x x x x x x x x x x x
	// e x x x x x x x x x x x x x x x x
	// g x x x x x x x x x x x x x x x x
	// m x x x x x x x x x x x x x x x x
	// e x x x x x x x x x x x x x x x x
	// n x x x x x x x x x x x x x x x x
	// t x x x x x x x x x x x x x x x x
	// a x x x x x x x x x x x x x x x x
	// t x x x x x x x x x x x x x x x x
	// i x x x x x x x x x x x x x x x x
	// o x x x x x x x x x x x x x x x x
	// n x x x x x x x x x x x x x x x x
	// _ x x x x x x x x x x x x x x x x
	// y x x x x x x x x x x x x x x x x
	// | x x x x x x x x x x x x x x x x
	// | x x x x x x x x x x x x x x x x
	// | x x x x x x x x x x x x x x x x
	// V x x x x x x x x x x x x x x x x
	//

	// The buffersize is always the MAXIMUM of elements. By specifying segmentation_x and segmentatiion_y 
	// the content may be grouped and furthermore refined.

	// More accurate specification of data buffers. For audio, this should be identical to buffersize
	jvxLinkDataDescriptor_segmentation segmentation;

	// Free parameter to specify properties of the data. in particular, used on "coded" bitstreams
	jvxApiString format_spec;

	// We can announce and accept some aditional connection flags. These flags will also be copied to the connection
	// flags on pprepare!!
	jvxCBitField additional_flags = 0;
};

// =============================================================

struct jvxLinkDataDescriptor
{
	jvxLinkDataDescriptor_con_data con_data;
	jvxLinkDataDescriptor_con_params con_params;
	jvxLinkDataDescriptor_con_compat con_compat;
	jvxLinkDataDescriptor_con_link con_link;
	jvxLinkDataDescriptor_con_pipeline con_pipeline;
	jvxLinkDataDescriptor_con_sync con_sync;
	jvxLinkDataDescriptor_con_user con_user;

	
#ifndef JVX_COMPILE_SMALL
	struct
	{
		jvxInt64 tStamp_us;
		jvxSize frames_lost_since_last_time;
		//jvxSize idConnectionSenderToReceiver;
		//jvxSize idConnectionReceiverToSender;
	} admin;

#endif

#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "typedefs/TpjvxTypes_linkdata.h"
#endif
};

#endif
