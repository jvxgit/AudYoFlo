#ifndef __TJVXTYPES_H__
#define __TJVXTYPES_H__

// From system lib
#include "jvx_system.h"

// Required for bitfield class
jvxUInt64 jvx_txt2UInt64(const char* in, jvxCBool* err);
jvxUInt16 jvx_txt2UInt16(const char* in, jvxCBool* err);
jvxData jvx_txt2Data(const char* in, jvxCBool* err);

#ifdef JVX_NUMBER_32BITS_BITFIELD
#include "typedefs/TjvxBitFieldN.h"
#endif

/** Typedef for the maximum length of text fields */
#define JVX_MAXSTRING 256

#define JVXSTATE_NUM_USE 6

#define JVX_TEXT_NEW_LINE_CHAR "\n"
#define JVX_TEXT_NEW_SEGMENT_CHAR_START "{"
#define JVX_TEXT_NEW_SEGMENT_CHAR_STOP "}"

typedef enum 
{
	JVX_DATA_TYPE_SPEC_DOUBLE, 
	JVX_DATA_TYPE_SPEC_FLOAT,
	JVX_DATA_TYPE_SPEC_LIMIT
} jvxDataTypeSpec;

typedef enum
{
	JVX_STATE_NONE = 0x1,
	JVX_STATE_INIT = 0x2,
	JVX_STATE_SELECTED = 0x4,			// Plugin has been opened but has not yet been initialized
	JVX_STATE_ACTIVE = 0x8,		// Plugin has currently been initialized
	JVX_STATE_PREPARED = 0x10,			// Plugin is ready for processing
	JVX_STATE_PROCESSING = 0x20,		// Plugin is in processing state
	JVX_STATE_COMPLETE = 0x40,		// Plugin is in complete state
	JVX_STATE_DONTCARE = -1 //(JVX_STATE_NONE || JVX_STATE_INIT | JVX_STATE_SELECTED|	JVX_STATE_ACTIVE | JVX_STATE_PREPARED | JVX_STATE_PROCESSING | JVX_STATE_COMPLETE)
} jvxState;

typedef enum
{
	JVX_STATE_SWITCH_NONE,
	JVX_STATE_SWITCH_INIT,
	JVX_STATE_SWITCH_SELECT,
	JVX_STATE_SWITCH_ACTIVATE,
	JVX_STATE_SWITCH_PREPARE,
	JVX_STATE_SWITCH_START,
	JVX_STATE_SWITCH_STOP,
	JVX_STATE_SWITCH_POSTPROCESS,
	JVX_STATE_SWITCH_DEACTIVATE,
	JVX_STATE_SWITCH_UNSELECT,
	JVX_STATE_SWITCH_TERMINATE,
	JVX_STATE_SWITCH_LIMIT
} jvxStateSwitch;

enum class jvxComponentTypeClass
{
	JVX_COMPONENT_TYPE_NONE,
	JVX_COMPONENT_TYPE_TECHNOLOGY,
	JVX_COMPONENT_TYPE_NODE,
	JVX_COMPONENT_TYPE_HOST,
	JVX_COMPONENT_TYPE_CODEC,
	JVX_COMPONENT_TYPE_TOOL,

	JVX_COMPONENT_TYPE_SIMPLE,

	JVX_COMPONENT_TYPE_DEVICE,
	
	JVX_COMPONENT_TYPE_PROCESS,

	JVX_COMPONENT_TYPE_PACKAGE,

	JVX_COMPONENT_TYPE_LIMIT
};

/*
 * If we modify anything here, make sure to update also
 *
 * 1) theClassAssociation (file TjvxTypes.cpp, line 4)
 * 2) jvxComponentType_str_ (file HjvxSystem.cpp, line 7)
 * 3) jvxComponentClassTypeAssoc (file HjvxSystem.cpp, line 124)
 * 
 * 4) In file <AudYoFlo>/sources/jvxLibraries/jvx-helpers/src/HjvxHostsAndProperties.cpp and in included header, e.g.,
 *    in <AudYoFlo>/software/codeFragments/components/Hjvx_caseStatement_nodes.h -> add case statement if required.
 * 5) In file <AudYoFlo>/sources/jvxLibraries/jvx-system-base/include/helpers/HjvxCastProduct.h -> add entry in case statement, e.g., in line 50 for DYNAMIC NODE
 * 6) In file TjvxTypes.h (here), line 169, add an entry in macro JVX_START_SLOTS_BASE
 * 
 * 7) In case flutter is in use: JvxComponentTypeEnum in file "AudYoFlo/flutter/ayf_pack/lib/constants/ayf_constants_audio.dart
 * 
 */
typedef enum
{
	JVX_COMPONENT_UNKNOWN = 0,
	JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY,
	JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE,
	JVX_COMPONENT_SIGNAL_PROCESSING_NODE,
	JVX_COMPONENT_AUDIO_TECHNOLOGY,
	JVX_COMPONENT_AUDIO_DEVICE,
	JVX_COMPONENT_AUDIO_NODE,
	JVX_COMPONENT_VIDEO_TECHNOLOGY,
	JVX_COMPONENT_VIDEO_DEVICE,
	JVX_COMPONENT_VIDEO_NODE,
	JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY,
	JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE,
	JVX_COMPONENT_APPLICATION_CONTROLLER_NODE,
	JVX_COMPONENT_CUSTOM_TECHNOLOGY,
	JVX_COMPONENT_CUSTOM_DEVICE,
	JVX_COMPONENT_CUSTOM_NODE,
	JVX_COMPONENT_EXTERNAL_TECHNOLOGY,
	JVX_COMPONENT_EXTERNAL_DEVICE,
	JVX_COMPONENT_EXTERNAL_NODE,
	JVX_COMPONENT_DYNAMIC_NODE,
	JVX_COMPONENT_PACKAGE,
	JVX_COMPONENT_MIN_HOST, 
	JVX_COMPONENT_FACTORY_HOST,
	JVX_COMPONENT_HOST,
	JVX_COMPONENT_EVENTLOOP,
	JVX_COMPONENT_DATALOGGER,
	JVX_COMPONENT_DATALOGREADER,
	JVX_COMPONENT_CONNECTION,
	JVX_COMPONENT_CONFIG_PROCESSOR,
	JVX_COMPONENT_EXTERNAL_CALL,
	JVX_COMPONENT_THREADCONTROLLER,
	JVX_COMPONENT_THREADS,
	JVX_COMPONENT_SHARED_MEMORY,
	JVX_COMPONENT_TEXT2SPEECH,
	JVX_COMPONENT_SYSTEM_TEXT_LOG,
	JVX_COMPONENT_LOCAL_TEXT_LOG,
	JVX_COMPONENT_CRYPT,
	JVX_COMPONENT_WEBSERVER,
	JVX_COMPONENT_REMOTE_CALL,
	JVX_COMPONENT_PACKETFILTER_RULE,
	JVX_COMPONENT_LOGREMOTEHANDLER

#ifndef JVX_NO_SYSTEM_EXTENSIONS
#define JVX_INCLUDE_COMPONENTS
#include "typedefs/TpjvxTypes_components.h"
#include "typedefs/TpjvxTypes_tools.h"
#undef JVX_INCLUDE_COMPONENTS
#endif

	, JVX_COMPONENT_PROCESSING_GROUP
	, JVX_COMPONENT_PROCESSING_PROCESS
	, JVX_COMPONENT_OFF_HOST
	, JVX_COMPONENT_INTERCEPTOR
	, JVX_COMPONENT_SYSTEM_AUTOMATION
	, JVX_COMPONENT_ALL_LIMIT
}  jvxComponentType;

// Set the split point between the normal and system components
#define JVX_MAIN_COMPONENT_LIMIT JVX_COMPONENT_SYSTEM_AUTOMATION

struct jvxComponentClassAssociation
{
	jvxComponentTypeClass comp_class = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;
	jvxComponentType comp_sec_type = JVX_COMPONENT_UNKNOWN;
	const char* description = nullptr;
	const char* config_token = nullptr;
	const char* description_sec = nullptr;
	jvxComponentTypeClass comp_child_class = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;
};

extern jvxComponentClassAssociation theClassAssociation[JVX_COMPONENT_ALL_LIMIT+1];

#define JVX_START_SLOTS_BASE(arr, slots_max, subslots_max) \
	arr[JVX_COMPONENT_AUDIO_TECHNOLOGY] = slots_max; \
	arr[JVX_COMPONENT_AUDIO_DEVICE] = subslots_max; \
	arr[JVX_COMPONENT_AUDIO_NODE] = slots_max; \
	arr[JVX_COMPONENT_VIDEO_TECHNOLOGY] = slots_max; \
	arr[JVX_COMPONENT_VIDEO_DEVICE] = subslots_max; \
	arr[JVX_COMPONENT_VIDEO_NODE] = slots_max; \
	arr[JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY] = slots_max; \
	arr[JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE] = subslots_max; \
	arr[JVX_COMPONENT_APPLICATION_CONTROLLER_NODE] = slots_max; \
	arr[JVX_COMPONENT_CUSTOM_TECHNOLOGY] = slots_max; \
	arr[JVX_COMPONENT_CUSTOM_DEVICE] = subslots_max; \
	arr[JVX_COMPONENT_CUSTOM_NODE] = slots_max; \
	arr[JVX_COMPONENT_EXTERNAL_TECHNOLOGY] = slots_max; \
	arr[JVX_COMPONENT_EXTERNAL_DEVICE] = subslots_max; \
	arr[JVX_COMPONENT_EXTERNAL_NODE] = slots_max; \
	arr[JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY] = slots_max; \
	arr[JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE] = subslots_max; \
	arr[JVX_COMPONENT_SIGNAL_PROCESSING_NODE] = slots_max; \
	arr[JVX_COMPONENT_DYNAMIC_NODE] = JVX_SIZE_UNSELECTED; \
	arr[JVX_COMPONENT_SYSTEM_AUTOMATION] = slots_max;


#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "typedefs/TpjvxTypes.h"
#endif

typedef enum
{
	JVX_INTERFACE_UNKNOWN = 0,
	JVX_INTERFACE_CONNECTOR_FACTORY,
	JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
	JVX_INTERFACE_CONFIGURATION,
	JVX_INTERFACE_TOOLS_HOST,

	// Report interface is more than IjvxReport as it holds the old report functions
	JVX_INTERFACE_REPORT,

	// ReportSystem interface is the "new" interface for system report messages
	JVX_INTERFACE_REPORT_SYSTEM,

	JVX_INTERFACE_PROPERTIES,
	JVX_INTERFACE_SEQUENCERCONTROL,
	JVX_INTERFACE_SEQUENCER,
	JVX_INTERFACE_CONFIGURATIONLINE,
	JVX_INTERFACE_PROPERTY_POOL,
	JVX_INTERFACE_SCHEDULE,
	JVX_INTERFACE_UNIQUE_ID,
	
	JVX_INTERFACE_HOST,
	JVX_INTERFACE_HOSTTYPEHANDLER,
	JVX_INTERFACE_EVENTLOOP,

	JVX_INTERFACE_QT_WIDGET_HOST,
	JVX_INTERFACE_QT_WIDGET_UI,
	JVX_INTERFACE_BINARY_FAST_DATA_CONTROL,
	JVX_INTERFACE_DATAPROCESSOR_CONNECTOR,
	JVX_INTERFACE_REMOTE_CALL_ASYNC,
	JVX_INTERFACE_PRINTF,	
	JVX_INTERFACE_CONFIGURATION_DONE,
	JVX_INTERFACE_CONFIGURATION_EXTENDER,
	JVX_INTERFACE_DATA_CONNECTIONS,
	JVX_INTERFACE_AUTO_DATA_CONNECT,
	JVX_INTERFACE_HTTP_API,
	JVX_INTERFACE_MANIPULATE,
	JVX_INTERFACE_COMMAND_LINE,
	JVX_INTERFACE_BOOT_STEPS,
	JVX_INTERFACE_COMMAND_INTERPRETER,
	JVX_INTERFACE_PROPERTY_ATTACH,
	JVX_INTERFACE_CONFIGURATION_ATTACH,

	JVX_INTERFACE_PACKAGE,

	JVX_INTERFACE_COMPONENT_HOST_EXT,
	JVX_INTERFACE_COMPONENT_HOST

#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "typedefs/TpjvxTypes_interfaces.h"
#endif

	, JVX_INTERFACE_LIMIT
}  jvxInterfaceType;

typedef enum
{
	JVX_BITFIELD_DLL_PROPERTY_DO_NOT_UNLOAD = 0x1
} jvxDllPropertyBitfield;


typedef enum
{
	JVX_PRIORITY_ABOVE_UNKNOWN,
	JVX_PRIORITY_ABOVE_NORMAL,
	JVX_PRIORITY_BELOW_NORMAL,
	JVX_PRIORITY_HIGH,
	JVX_PRIORITY_IDLE,
	JVX_PRIORITY_NORMAL,
	JVX_PRIORITY_REALTIME
} jvxPriorityType;

enum class jvxDeviceCapabilityTypeShift
{
	JVX_DEVICE_CAPABILITY_UNKNOWN_SHIFT,
	JVX_DEVICE_CAPABILITY_INPUT_SHIFT, 
	JVX_DEVICE_CAPABILITY_OUTPUT_SHIFT, 
	JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT, 
	JVX_DEVICE_CAPABILITY_PROXY_SHIFT,
	JVX_DEVICE_CAPABILITY_OTHER_SHIFT, 
	JVX_DEVICE_CAPABILITY_LIMIT_SHIFT
};

enum class jvxDeviceDataFlowType
{
	JVX_DEVICE_DATAFLOW_UNKNOWN,
	JVX_DEVICE_DATAFLOW_ACTIVE,
	JVX_DEVICE_DATAFLOW_PASSIVE,
	JVX_DEVICE_DATAFLOW_OFFLINE,
	JVX_DEVICE_DATAFLOW_CONTROL,
	JVX_DEVICE_DATAFLOW_LIMIT
};

enum class jvxDeviceCapabilityFlagsShift
{
	// Indicate that device is the default device
	JVX_DEVICE_CAPABILITY_FLAGS_DEFAULT_DEVICE_SHIFT
};

class jvxDeviceCapabilities
{
public:
	jvxCBitField capsFlags = ((jvxCBitField)1 << (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT);
	jvxDeviceDataFlowType flow = jvxDeviceDataFlowType::JVX_DEVICE_DATAFLOW_ACTIVE;
	
	// This property indicates if the device is a maps to a proxy or to a real device. A proxy is never actibated itself, instead, it exposes another device
	jvxBool proxy = false;
	jvxBool selectable = true;
	jvxCBitField16 flags = 0;
};

// This class allows to pass around very specific pointers from native systems towards
// system independent components - so far used only for Android JNI refs
class jvxNativeHostSysPointers
{
public:
	jvxHandle* primary = nullptr;
	jvxHandle* secondary = nullptr;
	JVX_THREAD_ID thread_id = JVX_THREAD_ID_INVALID;
};

#define JVXFEATURE_CLASS_NUM 5

#define JVX_FEATURE_CLASS_NONE 0x0

#define JVX_FEATURE_CLASS_EXTERNAL_CALL_SHIFT 0
#define JVX_FEATURE_CLASS_EXTERNAL_CALL (1 << JVX_FEATURE_CLASS_EXTERNAL_CALL_SHIFT)

#define JVX_FEATURE_CLASS_HOOKUP_CALL_SHIFT 1
#define JVX_FEATURE_CLASS_HOOKUP_CALL (1 << JVX_FEATURE_CLASS_HOOKUP_CALL_SHIFT)

#define JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER_SHIFT 2
#define JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER (1 << JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER_SHIFT)

#define JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE_SHIFT 3
#define JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE (1 <<  JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE_SHIFT)

#define JVX_FEATURE_CLASS_NON_ESSENTIAL_SHIFT 4
#define JVX_FEATURE_CLASS_NON_ESSENTIAL (1 << JVX_FEATURE_CLASS_NON_ESSENTIAL_SHIFT)

typedef enum
{
	JVX_PORT_CONNECTION_NOT_CONNECTED = 0,
	JVX_PORT_CONNECTION_UNKNOWN = (0x0 | 0x3),
	JVX_PORT_CONNECTION_CONNECTED_SENDER_TO_RECEIVER = (0x4 | 0x1),
	JVX_PORT_CONNECTION_CONNECTED_RECEIVER_TO_SENDER = (0x8 | 0x1)
} jvxPortConnectionState;

typedef enum
{
	JVX_WIDGET_HOST_NONE,
	JVX_WIDGET_HOST_QT
} jvxWidgetHostType;

typedef enum
{
	JVX_COMPONENT_ACCESS_UNKNOWN,
	JVX_COMPONENT_ACCESS_SUB_COMPONENT,
	JVX_COMPONENT_ACCESS_STATIC_LIB,
	JVX_COMPONENT_ACCESS_DYNAMIC_LIB,
	JVX_COMPONENT_ACCESS_IMPORT_LIB,
	JVX_COMPONENT_ACCESS_SYSTEM,
	JVX_COMPONENT_ACCESS_LIMIT
} jvxComponentAccessType;

#ifdef JVX_NUMBER_32BITS_BITFIELD
typedef CjvxBitFieldN jvxBitField;
#define JVX_NUMBER_POSITIONS_BITFIELD_MAX (sizeof(jvxUInt32) * JVX_NUMBER_32BITS_BITFIELD)

#else
//! Unsigned 64 bit datatype
typedef jvxCBitField jvxBitField;
#define JVX_NUMBER_POSITIONS_BITFIELD_MAX sizeof(jvxCBitField)
#endif

typedef enum
{
	JVX_USER_DATA_ID_SYSTEM = 0,
	JVX_USER_DATA_ID_SYSTEM_COMPONENT_WIDGET_OFFSET = 64,
	JVX_USER_DATA_ID_USER_OFFSET = 128
} jvxUserDataSystemIds;

typedef enum
{
	JVX_CONFIGURATION_PURPOSE_CONFIG_ON_SYSTEM_STARTSTOP,
	JVX_CONFIGURATION_PURPOSE_CONFIG_ON_COMPONENT_STARTSTOP
} jvxConfigurationCallPurpose;
/*
typedef struct
{
	jvxComponentType tp;
	jvxSize slotid;
	jvxSize slotsubid;
} jvxComponentIdentification;
*/

typedef enum
{
	JVX_ACCESS_PROTOCOL_OK = 0,
	JVX_ACCESS_PROTOCOL_INVALID = 1,
	JVX_ACCESS_PROTOCOL_NO_ACCESS__WRITE = 2,
	JVX_ACCESS_PROTOCOL_NO_ACCESS__READ = 3,
	JVX_ACCESS_PROTOCOL_NO_RIGHT = 4,
	JVX_ACCESS_PROTOCOL_NO_DATA = 5,
	JVX_ACCESS_PROTOCOL_ERROR = 6,
	JVX_ACCESS_PROTOCOL_NO_CALL = 7,
	JVX_ACCESS_PROTOCOL_LIMIT = 8
} jvxAccessProtocol;

typedef enum
{
	JVX_PROPERTY_SELECTIONLIST_ADDRESS_SELECTION,
	JVX_PROPERTY_SELECTIONLIST_ADDRESS_SELECTABLE,
	JVX_PROPERTY_SELECTIONLIST_ADDRESS_EXCLUSIVE
} jvxPropertySelectionListAddress;

/*
 * We have 15 roles and one flag for overlay operations
 */
typedef enum
{
	JVX_ACCESS_ROLE_GROUP_NONE = 0,
	JVX_ACCESS_ROLE_GROUP_0 = 0x1,
	JVX_ACCESS_ROLE_GROUP_1 = 0x2,
	JVX_ACCESS_ROLE_GROUP_2 = 0x4,
	JVX_ACCESS_ROLE_GROUP_3 = 0x8,

	JVX_ACCESS_ROLE_GROUP_4 = 0x10,
	JVX_ACCESS_ROLE_GROUP_5 = 0x20,
	JVX_ACCESS_ROLE_GROUP_6 = 0x40,
	JVX_ACCESS_ROLE_GROUP_7 = 0x80,

	JVX_ACCESS_ROLE_GROUP_8 = 0x100,
	JVX_ACCESS_ROLE_GROUP_9 = 0x200,
	JVX_ACCESS_ROLE_GROUP_10 = 0x400,
	JVX_ACCESS_ROLE_GROUP_11 = 0x800,

	JVX_ACCESS_ROLE_GROUP_12 = 0x1000,
	JVX_ACCESS_ROLE_GROUP_13 = 0x2000,
	JVX_ACCESS_ROLE_GROUP_ACCESS_RIGHTS = 0x4000, // Read & write access flags
	JVX_ACCESS_ROLE_GROUP_CONFIG_MODE = 0x8000, // Read & write config mode

	JVX_ACCESS_ROLE_GROUP_ALL = 0xFFFF
} jvxAccessGroupDefinitions;

#define JVX_ACCESS_ROLE_DEFAULT (JVX_ACCESS_ROLE_GROUP_0 | JVX_ACCESS_ROLE_GROUP_1 | JVX_ACCESS_ROLE_GROUP_2 | JVX_ACCESS_ROLE_GROUP_3 | \
	JVX_ACCESS_ROLE_GROUP_4 | JVX_ACCESS_ROLE_GROUP_5 | JVX_ACCESS_ROLE_GROUP_6  | JVX_ACCESS_ROLE_GROUP_7 | JVX_ACCESS_ROLE_GROUP_8 | \
	JVX_ACCESS_ROLE_GROUP_9 | JVX_ACCESS_ROLE_GROUP_10 | JVX_ACCESS_ROLE_GROUP_11 | JVX_ACCESS_ROLE_GROUP_12 | JVX_ACCESS_ROLE_GROUP_13)

#define JVX_ACCESS_ROLE_ALL (JVX_ACCESS_ROLE_DEFAULT | JVX_ACCESS_ROLE_GROUP_ACCESS_RIGHTS | JVX_ACCESS_ROLE_GROUP_CONFIG_MODE)

#define JVX_EXTRACT_READ_FLAGS(flags64) (flags64 & 0xFFFF)
#define JVX_REPLACE_READ_FLAGS(flags64, flags) ((flags64 | 0xFFFF) & ((jvxAccessRightFlags_rwcd)flags | 0xFFFFFFFFFFFF0000)) 

#define JVX_EXTRACT_WRITE_FLAGS(flags64) ((flags64 >> 16) & 0xFFFF)
#define JVX_REPLACE_WRITE_FLAGS(flags64, flags) ((flags64 | ((jvxAccessRightFlags_rwcd)0xFFFF << 16)) & (((jvxAccessRightFlags_rwcd)flags << 16) | (jvxAccessRightFlags_rwcd)0xFFFFFFFF0000FFFF)) 

#define JVX_EXTRACT_CREATE_FLAGS(flags64) ((flags64 >> 32) & 0xFFFF)
#define JVX_REPLACE_CREATE_FLAGS(flags64, flags) ((flags64 | ((jvxAccessRightFlags_rwcd)0xFFFF << 32)) & (((jvxAccessRightFlags_rwcd)flags << 32) | (jvxAccessRightFlags_rwcd)0xFFFF0000FFFFFFFF))

#define JVX_EXTRACT_DESTROY_FLAGS(flags64) ((flags64 >> 48) & 0xFFFF)
#define JVX_REPLACE_DESTROY_FLAGS(flags64, flags) ((flags64 | ((jvxAccessRightFlags_rwcd)0xFFFF << 48)) & (((jvxAccessRightFlags_rwcd)flags << 48)| (jvxAccessRightFlags_rwcd)0x0000FFFFFFFFFFFF)) 

#define JVX_COMBINE_FLAGS_RCWD(flags_r, flags_w, flags_c, flags_d) ((jvxAccessRightFlags_rwcd)flags_r | ((jvxAccessRightFlags_rwcd)flags_w << 16) | ((jvxAccessRightFlags_rwcd)flags_c << 32) | ((jvxAccessRightFlags_rwcd)flags_d << 48))

#define JVX_ACCESS_ROLE_DEFAULT_RCWD JVX_COMBINE_FLAGS_RCWD(JVX_ACCESS_ROLE_DEFAULT,JVX_ACCESS_ROLE_DEFAULT,JVX_ACCESS_ROLE_DEFAULT,JVX_ACCESS_ROLE_DEFAULT)
#define JVX_ACCESS_ROLE_ALL_RCWD JVX_COMBINE_FLAGS_RCWD(JVX_ACCESS_ROLE_ALL, JVX_ACCESS_ROLE_ALL, JVX_ACCESS_ROLE_ALL, JVX_ACCESS_ROLE_ALL)

//#define JVX_PROPERTY_FLAG_ACCESS_ROLE_MASK 0x3FFF
//#define JVX_PROPERTY_FLAG_ACCESS_ROLE_SHIFT 0

//#define JVX_PROPERTY_FLAG_INTERNAL_PASS_SHIFT 14
//#define JVX_PROPERTY_FLAG_INTERNAL_PASS_MASK (1 << JVX_PROPERTY_FLAG_INTERNAL_PASS_SHIFT)

//#define JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_SHIFT 15
//#define JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_MASK (1 << JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_SHIFT)

// #define JVX_PROPERTY_FLAG_FULL_ALLOW JVX_PROPERTY_FLAG_ACCESS_ROLE_MASK

typedef enum
{
	JVX_CONFIG_MODE_FULL = 0x1,
	JVX_CONFIG_MODE_OVERLAY = 0x2,
	JVX_CONFIG_MODE_ACCESS_RIGHTS = 0x4,
	JVX_CONFIG_MODE_CONFIG_MODE = 0x8
} jvxConfigModeDefinitions;

#define JVX_CONFIG_MODE_DEFAULT (JVX_CONFIG_MODE_FULL | JVX_CONFIG_MODE_OVERLAY)
#define JVX_CONFIG_MODE_ALL (JVX_CONFIG_MODE_DEFAULT | JVX_CONFIG_MODE_ACCESS_RIGHTS | JVX_CONFIG_MODE_CONFIG_MODE)

#define JVX_CONFIG_MODE_OVERLAY_READ (JVX_CONFIG_MODE_OVERLAY | JVX_CONFIG_MODE_ACCESS_RIGHTS)

// #define JVX_CONFIG_MODE_FLAG_FULL_CONFIG 0x1
// #define JVX_CONFIG_MODE_FLAG_OVERLAY_CONFIG 0x2

typedef enum
{
	JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING = 0x0,
	JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONTENT = 0x1,
	JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONTENT = 0x2,
	JVX_PROPERTY_FLAGTAG_OPERATE_READ_ACCESS_RIGHTS = 0x4,
	JVX_PROPERTY_FLAGTAG_OPERATE_READ_CONFIG_MODE = 0x8,
	JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_ACCESS_RIGHTS = 0x10,
	JVX_PROPERTY_FLAGTAG_OPERATE_WRITE_CONFIG_MODE = 0x20
} jvxPropertyFlagsOperationType;

typedef enum
{
	JVX_COMMAND_LINE_ARG_PLUGIN_PATH,
	JVX_COMMAND_LINE_ARG_CONFIG_FILE,
	JVX_COMMAND_LINE_ARG_CONFIG_ADD
} jvxCommandLineArgSpecifyType;

typedef jvxCBitField jvxFlagTag;

typedef jvxErrorType(*jvxPropertyCallback)(jvxHandle* priv, jvxFloatingPointer* content);

typedef enum
{
	JVX_REQUEST_REFERENCE_PROXY_SHIFT = 0x0
} jvxRequestDeviceOptions;

namespace
{
	constexpr jvxSize JVX_MASTER_DELIVER_ACTIVE_SHIFT = 0x1;
	constexpr jvxSize JVX_MASTER_DELIVER_PASSIVE_SHIFT = 0x2;
}

#define JVX_PROPERTIES_CALLBACK_DEFINE(baseclass, callbackname) jvxErrorType \
baseclass::callbackname(jvxHandle* privateData, jvxFloatingPointer* content) \
{ \
	if (privateData) \
	{ \
		baseclass* this_pointer = reinterpret_cast<baseclass*>(privateData); \
		return(this_pointer->ic_ ## callbackname(content)); \
	} \
	return JVX_ERROR_INVALID_ARGUMENT; \
} \
jvxErrorType \
baseclass::ic_ ## callbackname(jvxFloatingPointer* content)

#define JVX_PROPERTIES_CALLBACK_DECLARE(callbackname) \
static jvxErrorType \
callbackname(jvxHandle* privateData, jvxFloatingPointer* content); \
jvxErrorType \
ic_ ## callbackname(jvxFloatingPointer* content)

typedef jvxErrorType(*jvx_try_lock_buffer)(jvxHandle* priv);
typedef void(*jvx_lock_buffer)(jvxHandle* priv);
typedef void(*jvx_unlock_buffer)(jvxHandle* priv);

#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "typedefs/TpjvxTypes_datatypes.h"
#endif

// Callback type to control which component is loaded in dynamic host
typedef void(*jvxLoadModuleFilterCallback)(jvxBool* doNotLoad, const char* module_name, jvxComponentType tp, jvxHandle* priv);

#define JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE const char* description, bool multipleObjects, \
	const char* descriptor, jvxBitField featureClass, \
	const char* module_name, jvxComponentAccessType acTp, \
	jvxComponentType tpComp, const char* descrComp, \
	IjvxObject* templ

#define JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL description, multipleObjects, descriptor, featureClass, module_name, acTp, tpComp, descrComp, templ

#define JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL_EMPTY "local", false, "local", 0, "local", JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_UNKNOWN, "local", NULL

#define JVX_OBJECT_ASSIGN_INTERFACE_FACTORY(ptr) _common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(ptr)

#if (defined JVX_OS_LINUX) || (defined JVX_OS_ANDROID) || (defined JVX_OS_MACOSX) || (defined JVX_OS_IOS) || (defined JVX_OS_EMSCRIPTEN)
#define JVX_SOURCE_CODE_ORIGIN ((std::string)__FUNCTION__ +":" +__FILE__ +":" +Quotes(__LINE__)).c_str()
#else
#define JVX_SOURCE_CODE_ORIGIN __FUNCTION__ ":" __FILE__ ":" Quotes(__LINE__)
#endif

typedef enum 
{
	JVX_GENERIC_CONNECTION_NO_MESSAGE_TYPE,
	JVX_GENERIC_CONNECTION_GENERIC_MESSAGE,
	JVX_GENERIC_CONNECTION_MESSAGE_OFFSET
}jvxGenericConnectionMessageType;

typedef enum 
{
	JVX_GENERIC_CONNECTION_STATUS_NONE,
	JVX_GENERIC_CONNECTION_STATUS_INIT,
	JVX_GENERIC_CONNECTION_STATUS_SENT,
	JVX_GENERIC_CONNECTION_STATUS_RESPONDED,
	JVX_GENERIC_CONNECTION_STATUS_RETRANSMIT
} jvxLaConnectionMessageStatus;

enum class jvxRemoteConnectionQuality
{
	JVX_REMOTE_CONNECTION_QUALITY_NONE,
	JVX_REMOTE_CONNECTION_QUALITY_GOOD,
	JVX_REMOTE_CONNECTION_QUALITY_WEAK,
	JVX_REMOTE_CONNECTION_QUALITY_BAD,
	JVX_REMOTE_CONNECTION_QUALITY_NO_DATA,
	JVX_REMOTE_CONNECTION_QUALITY_NOT_CONNECTED,
	JVX_REMOTE_CONNECTION_QUALITY_NOT_AVAILABLE,
	JVX_REMOTE_CONNECTION_QUALITY_LIMIT
};

enum class jvxSocketsConnectionType
{
	JVX_SOCKET_TYPE_NONE,
	JVX_SOCKET_TYPE_TCP,
	JVX_SOCKET_TYPE_UDP,
	JVX_SOCKET_TYPE_UNIX,
	JVX_SOCKET_TYPE_PCAP,
	JVX_SOCKET_TYPE_BTH,
	JVX_SOCKET_TYPE_LIMIT
};

struct oneMessage_hdr
{
	jvxBool expectResponse;
	jvxSize tp; /* jvxGenericRs232MessageType */
	jvxLaConnectionMessageStatus stat;
	jvxSize uId;
	jvxSize mId;
	jvxSize sz_elm;
	jvxInt64 timestamp_enter_us;
	jvxInt64 timestamp_sent_us;
	jvxSize retrans_cnt;
};

#ifndef JVX_NO_SYSTEM_EXTENSIONS
#define JVX_INCLUDE_SHAREDMEMORY
#include "typedefs/TpjvxTypes_sharedmemory.h"
#undef JVX_INCLUDE_COMPONENTS
#endif

#endif
