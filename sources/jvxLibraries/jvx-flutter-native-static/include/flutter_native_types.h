#ifndef __FLUTTER_NATIVE_TPES_H__
#define __FLUTTER_NATIVE_TPES_H__

#include "flutter_native_type_translator.h"

typedef enum
{
	JVX_FFI_CALLBACK_ASYNC_REPORT_TEXT = 0,
	JVX_FFI_CALLBACK_SYNC_REPORT_INTERNALS_CHANGED = 1,
	JVX_FFI_CALLBACK_ASYNC_REPORT_COMMAND_REQUEST = 2,
	JVX_FFI_CALLBACK_REQUEST_COMMAND = 3
} ffiCallbackIds;

typedef enum
{
	JVX_DELETE_DATATYPE_ASYNC_CALLBACK = 0,
	JVX_DELETE_DATATYPE_CHAR_ARRAY = 1,
	JVX_DELETE_DATATYPE_INTERNAL_CHANGED = 2,
	JVX_DELETE_DATATYPE_REQUEST_COMMAND = 3,
	JVX_DELETE_DATATYPE_ONE_PROCESS = 4,
	JVX_DELETE_DATATYPE_ONE_PROP_HANDLER = 5,
	JVX_DELETE_DATATYPE_ONE_SEQ_EVENT = 6,
	JVX_DELETE_DATATYPE_ONE_COMPONENT_IDENT = 7,
	JVX_DELETE_DATATYPE_ONE_PROPERTY_DESCRIPTOR_FULL_PLUS = 8,
	JVX_DELETE_DATATYPE_PROCESS_READY = 9,
	JVX_DELETE_DATATYPE_PROPERTY_STRING_LIST = 10,
	JVX_DELETE_DATATYPE_DEVICE_CAPS = 11,
	JVX_DELETE_DATATYPE_SELECTION_OPTION = 12,
	JVX_DELETE_DATATYPE_SS_LIST = 13,
	JVX_DELETE_DATATYPE_VALUE_IN_RANGE = 14,
	JVX_DELETE_DATATYPE_UNKNOWN = 15,
	JVX_DELETE_DATATYPE_LIMIT
} ffiDeleteDatatype;

// ================================================================================

struct one_property_min
{
	int valid_parts;
	int category;
	int globalIdx;
};

struct one_property_core
{
	struct one_property_min pmin;
	int format;
	int num;
	int accessType;
	int decTp;
	int ctxt;
};

struct one_property_control
{
	struct one_property_core core;
	int allowedStateMask;
	int allowedThreadingMask;
	int isValid;
	int invalidateOnStateSwitch;
	int invalidateOnTest;
	int installable;
};

struct one_property_full
{
	struct one_property_control ctrl;
	char* name;
	char* description;
	char* descriptor;
};

struct one_property_full_plus
{
	struct one_property_full full;
	jvxCBitField64 accessFlags;
	jvxCBitField32 configModeFlags;
};

// =====================================================================================

struct component_ident
{
	int tp;
	jvxSize slotid;
	jvxSize slotsubid;
	jvxSize uId;
};

struct report_internal_changed
{
	struct component_ident from;
	struct component_ident to;

	int cat;
	jvxSize propId;
	int onlyContent;
	jvxSize offset_start;
	jvxSize numElements;

	int callpurpose;
}; 

struct retOpaqueHandle
{
	int err_code;
	void* opaque_hdl;
};

struct report_callback
{
	int callback_id;
	int callback_subid;
	void* load_fld;
	size_t sz_fld;
};

struct one_process
{
	int uId;
	char* name;
	int category;
	void* iterator;
};

struct one_property_comp
{
	struct component_ident cpId;
	void* obj_ptr;
	void* prop_ptr;
};

// ===========================================================
struct one_sequencer_event
{
	int event_mask;
	char* description;
	int sequenceId;
	int stepId;
	int qtp;
	int setp;
	int fId;
	int seq_state_id;
	int statSeq;
	int indicateFirstError;	
};

struct process_ready
{
	int err;
	char* reason_if_not;
};

struct func_pointers
{
	// Synchronous callback to come back to dart. Return values indicates error type
	int (*cb_sync)(struct report_callback* report_hdl);
};

// ===========================================================
// ===========================================================

struct one_property_string_list
{
	int nEntries;
	char** pEntries;
};

struct device_capabilities
{
	char* description;
	char* descriptor;
	jvxCBitField64 caps;
	int flow;
	jvxCBitField32 flags;
	int selectable;
	int proxy;
	int slotsubid;
	int stat;
};

struct selection_option
{
	char* description;
	char* descriptor;	
};

struct ss_list
{
	jvxInt32* slots;
	jvxInt32* subslots;
	jvxSize num;
};

struct one_property_value_in_range
{
	double minVal;
	double maxVal;
};


#endif
