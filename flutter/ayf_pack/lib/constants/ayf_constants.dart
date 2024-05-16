import 'package:flutter/foundation.dart';

enum jvxDataTypeSpecEnum { JVX_DATA_TYPE_SPEC_DOUBLE, JVX_DATA_TYPE_SPEC_FLOAT }

extension jvxDataTypeSpecEEnum on jvxDataTypeSpecEnum {
  static String get formatName => "jvxDataTypeSpec";
  String get txt => describeEnum(this);
  static jvxDataTypeSpecEnum fromInt(int val) {
    jvxDataTypeSpecEnum ss = jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_DOUBLE;
    if (val < jvxDataTypeSpecEnum.values.length) {
      ss = jvxDataTypeSpecEnum.values[val];
    }
    return ss;
  }

  static jvxDataTypeSpecEnum fromString(String val) {
    jvxDataTypeSpecEnum ss = jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_DOUBLE;
    if (val == 'dbl') {
      ss = jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_DOUBLE;
    } else {
      ss = jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_FLOAT;
    }
    return ss;
  }
}

enum jvxComponentTypeClassEnum {
  JVX_COMPONENT_TYPE_NONE,
  JVX_COMPONENT_TYPE_TECHNOLOGY,
  JVX_COMPONENT_TYPE_NODE,
  JVX_COMPONENT_TYPE_HOST,
  JVX_COMPONENT_TYPE_CODEC,
  JVX_COMPONENT_TYPE_TOOL,
  JVX_COMPONENT_TYPE_SIMPLE,
  JVX_COMPONENT_TYPE_DEVICE,
  JVX_COMPONENT_TYPE_PROCESS
}

extension jvxComponentTypeClassEEnum on jvxComponentTypeClassEnum {
  static String get formatName => "jvxComponentTypeClass";
  String get txt => describeEnum(this);
  static jvxComponentTypeClassEnum fromInt(int val) {
    jvxComponentTypeClassEnum ss =
        jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NONE;
    if (val < jvxComponentTypeClassEnum.values.length) {
      ss = jvxComponentTypeClassEnum.values[val];
    }
    return ss;
  }
}

// ====================================================================
// ====================================================================

abstract class jvxReportPriority {
  static const int JVX_REPORT_PRIORITY_NONE = 0;
  static const int JVX_REPORT_PRIORITY_ERROR = 1;
  static const int JVX_REPORT_PRIORITY_WARNING = 2;
  static const int JVX_REPORT_PRIORITY_SUCCESS = 3;
  static const int JVX_REPORT_PRIORITY_INFO = 4;
  static const int JVX_REPORT_PRIORITY_VERBOSE = 5;
  static const int JVX_REPORT_PRIORITY_DEBUG = 6;
  static const int JVX_REPORT_PRIORITY_LIMIT = 7;
}

// enum ffiCallbackIds { JVX_FFI_CALLBACK_REPORT_TEXT }

abstract class jvxPropertyCategoryType {
  static const int JVX_PROPERTY_CATEGORY_UNKNOWN = 0;
  static const int JVX_PROPERTY_CATEGORY_PREDEFINED = 1;
  static const int JVX_PROPERTY_CATEGORY_UNSPECIFIC = 2;
}

// ===========================================================
/*
abstract class jvxDataFormat {
  static const int JVX_DATAFORMAT_NONE = 0;
  static const int JVX_DATAFORMAT_DATA = 1;
  static const int JVX_DATAFORMAT_16BIT_LE = 2;
  static const int JVX_DATAFORMAT_32BIT_LE = 3;
  static const int JVX_DATAFORMAT_64BIT_LE = 4;
  static const int JVX_DATAFORMAT_8BIT = 5;
  static const int JVX_DATAFORMAT_SIZE = 6;
  static const int JVX_DATAFORMAT_STRING = 7;
  static const int JVX_DATAFORMAT_STRING_LIST = 8;
  static const int JVX_DATAFORMAT_SELECTION_LIST = 9;
  static const int JVX_DATAFORMAT_VALUE_IN_RANGE = 10;
  static const int JVX_DATAFORMAT_U16BIT_LE = 11;
  static const int JVX_DATAFORMAT_U32BIT_LE = 12;
  static const int JVX_DATAFORMAT_U64BIT_LE = 13;
  static const int JVX_DATAFORMAT_U8BIT = 14;
  static const int JVX_DATAFORMAT_FLOAT = 15;
  static const int JVX_DATAFORMAT_DOUBLE = 16;
  static const int JVX_DATAFORMAT_BYTE = 17;

  static const int JVX_DATAFORMAT_HANDLE = 18;
  static const int JVX_DATAFORMAT_POINTER = 19;
  static const int JVX_DATAFORMAT_CALLBACK = 20;
  static const int JVX_DATAFORMAT_INTERFACE = 21;
}
*/
enum jvxDataFormatEnum {
  JVX_DATAFORMAT_NONE,
  JVX_DATAFORMAT_DATA,
  JVX_DATAFORMAT_16BIT_LE,
  JVX_DATAFORMAT_32BIT_LE,
  JVX_DATAFORMAT_64BIT_LE,
  JVX_DATAFORMAT_8BIT,
  JVX_DATAFORMAT_SIZE,
  JVX_DATAFORMAT_STRING,
  JVX_DATAFORMAT_STRING_LIST,
  JVX_DATAFORMAT_SELECTION_LIST,
  JVX_DATAFORMAT_VALUE_IN_RANGE,
  JVX_DATAFORMAT_U16BIT_LE,
  JVX_DATAFORMAT_U32BIT_LE,
  JVX_DATAFORMAT_U64BIT_LE,
  JVX_DATAFORMAT_U8BIT,
  JVX_DATAFORMAT_FLOAT,
  JVX_DATAFORMAT_DOUBLE,
  JVX_DATAFORMAT_BYTE,

  JVX_DATAFORMAT_HANDLE,
  JVX_DATAFORMAT_POINTER,
  JVX_DATAFORMAT_CALLBACK,
  JVX_DATAFORMAT_INTERFACE,
  JVX_DATAFORMAT_TEXT_LOG
}

extension jvxDataFormatEEnum on jvxDataFormatEnum {
  static String get formatName => "jvxDataFormat";
  String get txt => describeEnum(this);
  static jvxDataFormatEnum fromInt(int val) {
    jvxDataFormatEnum ss = jvxDataFormatEnum.JVX_DATAFORMAT_NONE;
    if (val < jvxDataFormatEnum.values.length) {
      ss = jvxDataFormatEnum.values[val];
    }
    return ss;
  }
}

// ===========================================================
abstract class jvxPropertyCallPurpose {
  static const int JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC = 0;
  static const int JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS = 1;
  static const int JVX_PROPERTY_CALL_PURPOSE_TAKE_OVER = 2;
  static const int JVX_PROPERTY_CALL_PURPOSE_PROPERTY_POOL = 3;
  static const int JVX_PROPERTY_CALL_PURPOSE_SIZE_ONLY = 4;
  static const int JVX_PROPERTY_CALL_PURPOSE_GET_ACCESS_DATA = 5;
  /*JVX_PROPERTY_CALL_PURPOSE_COMPONENT_RECONFIGURE = 4*/
}

abstract class jvxCommandRequestFlags {
  static const int JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT = 0;
  static const int JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT = 1;
  static const int JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT = 2;
  static const int JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT =
      3;
  static const int JVX_REPORT_REQUEST_TRY_TRIGGER_START_SEQUENCER_SHIFT = 4;
  static const int JVX_REPORT_REQUEST_UPDATE_DATACONNECTIONS_SHIFT_DEP = 5;
  static const int JVX_REPORT_REQUEST_USER_MESSAGE_SHIFT = 6;
  static const int JVX_REPORT_REQUEST_UPDATE_PROPERTIES_SHIFT_DEP = 7;
  static const int JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT = 8;
  static const int JVX_REPORT_REQUEST_RECONFIGURE_SHIFT = 9;
  static const int JVX_REPORT_REQUEST_REACTIVATE_SHIFT = 10;
  static const int
      JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_SUBROUTINE_SHIFT = 11;
  static const int JVX_REPORT_REQUEST_COMMAND_RETRIGGER_SEQUENCER_SHIFT_DEP =
      12;
}

/*
abstract class jvxReportCommandRequest {
  static const int JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC = 0;
  static const int JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST =
      1;
  static const int JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT_LIST = 2;
  static const int JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT = 3;
  static const int JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED = 4;
  static const int JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE = 5;

  static const int JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTIES = 6;

  static const int JVX_REPORT_COMMAND_REQUEST_RESCHEDULE_MAIN = 7;

  static const int JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_SUBDEVICE = 8;
  static const int JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE = 9;

  static const int JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_COMPONENT = 10;
  static const int JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_COMPONENT = 11;

  static const int JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED = 12;
  static const int
      JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED = 13;
  static const int JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN = 14;
  static const int JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH = 15;

  static const int JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE =
      16;

  static const int JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_EVENT = 17;
  static const int JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_CALLBACK = 18;

  static const int
      JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_DISCONNECT_COMPLETE = 19;

  static const int JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY_CONTENT = 20;
  static const int
      JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY_DESCRIPTOR_CONTENT = 21;
}
*/

enum jvxReportCommandRequestEnum {
  JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC,
  JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST,
  JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT_LIST,
  JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT,
  JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED,

  JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE,
  JVX_REPORT_COMMAND_REQUEST_UPDATE_ALL_PROPERTIES,
  JVX_REPORT_COMMAND_REQUEST_RESCHEDULE_MAIN,
  JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_SUBDEVICE,
  JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE,

  JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_COMPONENT,
  JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_COMPONENT,
  JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED,
  JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED,
  JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN,

  JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH,
  JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE,
  JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_EVENT,
  JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_CALLBACK,
  JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_DISCONNECT_COMPLETE,

  JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY,
  JVX_REPORT_COMMAND_REQUEST_REPORT_TEST_SUCCESS,
  JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN_RUN
}

extension jvxReportCommandRequestEEnum on jvxReportCommandRequestEnum {
  String get txt => describeEnum(this);
  static jvxReportCommandRequestEnum fromInt(int val) {
    jvxReportCommandRequestEnum ss =
        jvxReportCommandRequestEnum.JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC;
    assert(val < jvxReportCommandRequestEnum.values.length);
    if (val < jvxReportCommandRequestEnum.values.length) {
      ss = jvxReportCommandRequestEnum.values[val];
    }
    return ss;
  }
}

// ===========================================================================

enum jvxReportCommandDataTypeEnum {
  JVX_REPORT_COMMAND_TYPE_BASE,
  JVX_REPORT_COMMAND_TYPE_SCHEDULE,
  JVX_REPORT_COMMAND_TYPE_IDENT,
  JVX_REPORT_COMMAND_TYPE_UID,
  JVX_REPORT_COMMAND_TYPE_SS,
  JVX_REPORT_COMMAND_TYPE_SEQ
}

extension jvxReportCommandDataTypeEEnum on jvxReportCommandDataTypeEnum {
  String get txt => describeEnum(this);
  static jvxReportCommandDataTypeEnum fromInt(int val) {
    jvxReportCommandDataTypeEnum dt =
        jvxReportCommandDataTypeEnum.JVX_REPORT_COMMAND_TYPE_BASE;
    assert(val < jvxReportCommandDataTypeEnum.values.length);
    if (val < jvxReportCommandDataTypeEnum.values.length) {
      dt = jvxReportCommandDataTypeEnum.values[val];
    }
    return dt;
  }
}
// ===========================================================================

enum jvxReportCommandBroadcastTypeEnum {
  JVX_REPORT_COMMAND_BROADCAST_NONE,
  JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER,
  JVX_REPORT_COMMAND_BROADCAST_AUTOMATION,
  JVX_REPORT_COMMAND_BROADCAST_RESCHEDULED
}

extension jvxReportCommandBroadcastTypeEEnum
    on jvxReportCommandBroadcastTypeEnum {
  String get txt => describeEnum(this);
  static jvxReportCommandBroadcastTypeEnum fromInt(int val) {
    jvxReportCommandBroadcastTypeEnum bc =
        jvxReportCommandBroadcastTypeEnum.JVX_REPORT_COMMAND_BROADCAST_NONE;
    if (val < jvxReportCommandBroadcastTypeEnum.values.length) {
      bc = jvxReportCommandBroadcastTypeEnum.values[val];
    }
    return bc;
  }
}

abstract class jvxSize {
  static const int JVX_SIZE_UNSELECTED = -1;
  static const int JVX_SIZE_DONTCARE = -2;
  static const int JVX_SIZE_SLOT_RETAIN = -3;
  static const int JVX_SIZE_SLOT_OFF_SYSTEM = -4;
  static const int JVX_SIZE_STATE_INCREMENT = -5;
  static const int JVX_SIZE_STATE_DECREMENT = -6;
}

// ===========================================================================

enum jvxStateShiftEnum {
  JVX_STATE_NONE,
  JVX_STATE_INIT,
  JVX_STATE_SELECTED,
  JVX_STATE_ACTIVE,
  JVX_STATE_PREPARED,
  JVX_STATE_PROCESSING,
  JVX_STATE_COMPLETE
}

abstract class jvxState {
  static const int JVX_STATE_NONE = 0x1;
  static const int JVX_STATE_INIT = 0x2;
  static const int JVX_STATE_SELECTED = 0x4;
  static const int JVX_STATE_ACTIVE = 0x8;
  static const int JVX_STATE_PREPARED = 0x10;
  static const int JVX_STATE_PROCESSING = 0x20;
  static const int JVX_STATE_COMPLETE = 0x40;
}

extension jvxStateEInt on jvxState {
  static int fromStringSingle(String value) {
    int retVal = jvxState.JVX_STATE_NONE;
    switch (value) {
      case 'selected':
        retVal = jvxState.JVX_STATE_SELECTED;
        break;
      case 'active':
        retVal = jvxState.JVX_STATE_ACTIVE;
        break;
      case 'prepared':
        retVal = jvxState.JVX_STATE_PREPARED;
        break;
      case 'processing':
        retVal = jvxState.JVX_STATE_PROCESSING;
        break;
    }
    return retVal;
  }
}
// ===========================================================================

abstract class jvxErrorType {
  static const int JVX_NO_ERROR = 0;
  static const int JVX_ERROR_UNKNOWN = 1;
  static const int JVX_ERROR_INVALID_ARGUMENT = 2;
  static const int JVX_ERROR_WRONG_STATE = 3;
  static const int JVX_ERROR_ID_OUT_OF_BOUNDS = 4;
  static const int JVX_ERROR_UNSUPPORTED = 5;
  static const int JVX_ERROR_ELEMENT_NOT_FOUND = 6;
  static const int JVX_ERROR_DUPLICATE_ENTRY = 7;
  static const int JVX_ERROR_OPEN_FILE_FAILED = 8;
  static const int JVX_ERROR_UNEXPECTED = 9;
  static const int JVX_ERROR_NOT_READY = 10;
  static const int JVX_ERROR_COMPONENT_BUSY = 11;
  static const int JVX_ERROR_BUFFER_OVERFLOW = 12;
  static const int JVX_ERROR_BUFFER_UNDERRUN = 13;
  static const int JVX_ERROR_WRONG_STATE_SUBMODULE = 14;
  static const int JVX_ERROR_MESSAGE_QUEUE_FULL = 15;
  static const int JVX_ERROR_PARSE_ERROR = 16;
  static const int JVX_ERROR_WRONG_SECTION_TYPE = 17;
  static const int JVX_ERROR_INTERNAL = 18;
  static const int JVX_ERROR_EMPTY_LIST = 19;
  static const int JVX_ERROR_CALL_SUB_COMPONENT_FAILED = 20;
  static const int JVX_ERROR_THREADING_MISMATCH = 21;
  static const int JVX_ERROR_SYSTEMCALL_FAILED = 22;
  static const int JVX_ERROR_SIZE_MISMATCH = 23;
  static const int JVX_ERROR_NO_ACCESS = 24;
  static const int JVX_ERROR_INVALID_SETTING = 25;
  static const int JVX_ERROR_TIMEOUT = 26;
  static const int JVX_ERROR_ABORT = 27;
  static const int JVX_ERROR_POSTPONE = 28;
  static const int JVX_ERROR_END_OF_FILE = 29;
  static const int JVX_ERROR_STANDARD_EXCEPTION = 30;
  static const int JVX_ERROR_SEQUENCER_TOOK_CONTROL = 31;
  static const int JVX_ERROR_THREAD_ONCE_MORE = 32;
  static const int JVX_ERROR_PROCESS_COMPLETE = 33;
  static const int JVX_ERROR_DSP = 34;
  static const int JVX_ERROR_ALREADY_IN_USE = 35;
  static const int JVX_ERROR_PROTOCOL_ERROR = 36;
  static const int JVX_ERROR_NOT_ENOUGH_INPUT_DATA = 37;
  static const int JVX_ERROR_NOT_ENOUGH_OUTPUT_SPACE = 38;
  static const int JVX_ERROR_COMPROMISE = 39;
  static const int JVX_ERROR_REMOTE = 40;
  static const int JVX_ERROR_ALLOCATION = 41;
  static const int JVX_ERROR_INVALID_FORMAT = 42;
  static const int JVX_ERROR_REQUEST_CALL_AGAIN = 43;
  static const int JVX_ERROR_NOT_IMPLEMENTED = 44;
}

extension jvxErrorTypeEInt on jvxErrorType {
  static int fromStringSingle(String value) {
    int retVal = jvxErrorType.JVX_ERROR_UNEXPECTED;

    switch (value) {
      case 'JVX_NO_ERROR':
        retVal = jvxErrorType.JVX_NO_ERROR;
        break;
      case 'JVX_ERROR_UNKNOWN':
        retVal = jvxErrorType.JVX_ERROR_UNKNOWN;
        break;
      case 'JVX_ERROR_INVALID_ARGUMENT':
        retVal = jvxErrorType.JVX_ERROR_INVALID_ARGUMENT;
        break;
      case 'JVX_ERROR_WRONG_STATE':
        retVal = jvxErrorType.JVX_ERROR_WRONG_STATE;
        break;
      case 'JVX_ERROR_ID_OUT_OF_BOUNDS':
        retVal = jvxErrorType.JVX_ERROR_ID_OUT_OF_BOUNDS;
        break;
      case 'JVX_ERROR_UNSUPPORTED':
        retVal = jvxErrorType.JVX_ERROR_UNSUPPORTED;
        break;
      case 'JVX_ERROR_ELEMENT_NOT_FOUND':
        retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
        break;
      case 'JVX_ERROR_DUPLICATE_ENTRY':
        retVal = jvxErrorType.JVX_ERROR_DUPLICATE_ENTRY;
        break;
      case 'JVX_ERROR_OPEN_FILE_FAILED':
        retVal = jvxErrorType.JVX_ERROR_OPEN_FILE_FAILED;
        break;
      case 'JVX_ERROR_UNEXPECTED':
        retVal = jvxErrorType.JVX_ERROR_UNEXPECTED;
        break;
      case 'JVX_ERROR_NOT_READY':
        retVal = jvxErrorType.JVX_ERROR_NOT_READY;
        break;
      case 'JVX_ERROR_COMPONENT_BUSY':
        retVal = jvxErrorType.JVX_ERROR_COMPONENT_BUSY;
        break;
      case 'JVX_ERROR_BUFFER_OVERFLOW':
        retVal = jvxErrorType.JVX_ERROR_BUFFER_OVERFLOW;
        break;
      case 'JVX_ERROR_BUFFER_UNDERRUN':
        retVal = jvxErrorType.JVX_ERROR_BUFFER_UNDERRUN;
        break;
      case 'JVX_ERROR_WRONG_STATE_SUBMODULE':
        retVal = jvxErrorType.JVX_ERROR_WRONG_STATE_SUBMODULE;
        break;
      case 'JVX_ERROR_MESSAGE_QUEUE_FULL':
        retVal = jvxErrorType.JVX_ERROR_MESSAGE_QUEUE_FULL;
        break;
      case 'JVX_ERROR_PARSE_ERROR':
        retVal = jvxErrorType.JVX_ERROR_PARSE_ERROR;
        break;
      case 'JVX_ERROR_WRONG_SECTION_TYPE':
        retVal = jvxErrorType.JVX_ERROR_WRONG_SECTION_TYPE;
        break;
      case 'JVX_ERROR_INTERNAL':
        retVal = jvxErrorType.JVX_ERROR_INTERNAL;
        break;
      case 'JVX_ERROR_EMPTY_LIST':
        retVal = jvxErrorType.JVX_ERROR_EMPTY_LIST;
        break;
      case 'JVX_ERROR_CALL_SUB_COMPONENT_FAILED':
        retVal = jvxErrorType.JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
        break;
      case 'JVX_ERROR_THREADING_MISMATCH':
        retVal = jvxErrorType.JVX_ERROR_THREADING_MISMATCH;
        break;
      case 'JVX_ERROR_SYSTEMCALL_FAILED':
        retVal = jvxErrorType.JVX_ERROR_SYSTEMCALL_FAILED;
        break;
      case 'JVX_ERROR_SIZE_MISMATCH':
        retVal = jvxErrorType.JVX_ERROR_SIZE_MISMATCH;
        break;
      case 'JVX_ERROR_NO_ACCESS':
        retVal = jvxErrorType.JVX_ERROR_NO_ACCESS;
        break;
      case 'JVX_ERROR_INVALID_SETTING':
        retVal = jvxErrorType.JVX_ERROR_INVALID_SETTING;
        break;
      case 'JVX_ERROR_TIMEOUT':
        retVal = jvxErrorType.JVX_ERROR_TIMEOUT;
        break;
      case 'JVX_ERROR_ABORT':
        retVal = jvxErrorType.JVX_ERROR_ABORT;
        break;
      case 'JVX_ERROR_POSTPONE':
        retVal = jvxErrorType.JVX_ERROR_POSTPONE;
        break;
      case 'JVX_ERROR_END_OF_FILE':
        retVal = jvxErrorType.JVX_ERROR_END_OF_FILE;
        break;
      case 'JVX_ERROR_STANDARD_EXCEPTION':
        retVal = jvxErrorType.JVX_ERROR_STANDARD_EXCEPTION;
        break;
      case 'JVX_ERROR_SEQUENCER_TOOK_CONTROL':
        retVal = jvxErrorType.JVX_ERROR_SEQUENCER_TOOK_CONTROL;
        break;
      case 'JVX_ERROR_THREAD_ONCE_MORE':
        retVal = jvxErrorType.JVX_ERROR_THREAD_ONCE_MORE;
        break;
      case 'JVX_ERROR_PROCESS_COMPLETE':
        retVal = jvxErrorType.JVX_ERROR_PROCESS_COMPLETE;
        break;
      case 'JVX_ERROR_DSP':
        retVal = jvxErrorType.JVX_ERROR_DSP;
        break;
      case 'JVX_ERROR_ALREADY_IN_USE':
        retVal = jvxErrorType.JVX_ERROR_ALREADY_IN_USE;
        break;
      case 'JVX_ERROR_PROTOCOL_ERROR':
        retVal = jvxErrorType.JVX_ERROR_PROTOCOL_ERROR;
        break;
      case 'JVX_ERROR_NOT_ENOUGH_INPUT_DATA':
        retVal = jvxErrorType.JVX_ERROR_NOT_ENOUGH_INPUT_DATA;
        break;
      case 'JVX_ERROR_NOT_ENOUGH_OUTPUT_SPACE':
        retVal = jvxErrorType.JVX_ERROR_NOT_ENOUGH_OUTPUT_SPACE;
        break;
      case 'JVX_ERROR_COMPROMISE':
        retVal = jvxErrorType.JVX_ERROR_COMPROMISE;
        break;
      case 'JVX_ERROR_REMOTE':
        retVal = jvxErrorType.JVX_ERROR_REMOTE;
        break;
      case 'JVX_ERROR_ALLOCATION':
        retVal = jvxErrorType.JVX_ERROR_ALLOCATION;
        break;
      case 'JVX_ERROR_INVALID_FORMAT':
        retVal = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        break;
      case 'JVX_ERROR_REQUEST_CALL_AGAIN':
        retVal = jvxErrorType.JVX_ERROR_REQUEST_CALL_AGAIN;
        break;
      case 'JVX_ERROR_NOT_IMPLEMENTED':
        retVal = jvxErrorType.JVX_ERROR_NOT_IMPLEMENTED;
        break;
    }
    return retVal;
  }

  static String toStringSingle(int errCode) {
    String retVal = "JVX_ERROR_UNEXPECTED";

    switch (errCode) {
      case jvxErrorType.JVX_NO_ERROR:
        retVal = 'JVX_NO_ERROR';
        break;
      case jvxErrorType.JVX_ERROR_UNKNOWN:
        retVal = 'JVX_ERROR_UNKNOWN';
        break;
      case jvxErrorType.JVX_ERROR_INVALID_ARGUMENT:
        retVal = 'JVX_ERROR_INVALID_ARGUMENT';
        break;
      case jvxErrorType.JVX_ERROR_WRONG_STATE:
        retVal = 'JVX_ERROR_WRONG_STATE';
        break;
      case jvxErrorType.JVX_ERROR_ID_OUT_OF_BOUNDS:
        retVal = 'JVX_ERROR_ID_OUT_OF_BOUNDS';
        break;
      case jvxErrorType.JVX_ERROR_UNSUPPORTED:
        retVal = 'JVX_ERROR_UNSUPPORTED';
        break;
      case jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND:
        retVal = 'JVX_ERROR_ELEMENT_NOT_FOUND';
        break;
      case jvxErrorType.JVX_ERROR_DUPLICATE_ENTRY:
        retVal = 'JVX_ERROR_DUPLICATE_ENTRY';
        break;
      case jvxErrorType.JVX_ERROR_OPEN_FILE_FAILED:
        retVal = 'JVX_ERROR_OPEN_FILE_FAILED';
        break;
      case jvxErrorType.JVX_ERROR_UNEXPECTED:
        retVal = 'JVX_ERROR_UNEXPECTED';
        break;
      case jvxErrorType.JVX_ERROR_NOT_READY:
        retVal = 'JVX_ERROR_NOT_READY';
        break;
      case jvxErrorType.JVX_ERROR_COMPONENT_BUSY:
        retVal = 'JVX_ERROR_COMPONENT_BUSY';
        break;
      case jvxErrorType.JVX_ERROR_BUFFER_OVERFLOW:
        retVal = 'JVX_ERROR_BUFFER_OVERFLOW';
        break;
      case jvxErrorType.JVX_ERROR_BUFFER_UNDERRUN:
        retVal = 'JVX_ERROR_BUFFER_UNDERRUN';
        break;
      case jvxErrorType.JVX_ERROR_WRONG_STATE_SUBMODULE:
        retVal = 'JVX_ERROR_WRONG_STATE_SUBMODULE';
        break;
      case jvxErrorType.JVX_ERROR_MESSAGE_QUEUE_FULL:
        retVal = 'JVX_ERROR_MESSAGE_QUEUE_FULL';
        break;
      case jvxErrorType.JVX_ERROR_PARSE_ERROR:
        retVal = 'JVX_ERROR_PARSE_ERROR';
        break;
      case jvxErrorType.JVX_ERROR_WRONG_SECTION_TYPE:
        retVal = 'JVX_ERROR_WRONG_SECTION_TYPE';
        break;
      case jvxErrorType.JVX_ERROR_INTERNAL:
        retVal = 'JVX_ERROR_INTERNAL';
        break;
      case jvxErrorType.JVX_ERROR_EMPTY_LIST:
        retVal = 'JVX_ERROR_EMPTY_LIST';
        break;
      case jvxErrorType.JVX_ERROR_CALL_SUB_COMPONENT_FAILED:
        retVal = 'JVX_ERROR_CALL_SUB_COMPONENT_FAILED';
        break;
      case jvxErrorType.JVX_ERROR_THREADING_MISMATCH:
        retVal = 'JVX_ERROR_THREADING_MISMATCH';
        break;
      case jvxErrorType.JVX_ERROR_SYSTEMCALL_FAILED:
        retVal = 'JVX_ERROR_SYSTEMCALL_FAILED';
        break;
      case jvxErrorType.JVX_ERROR_SIZE_MISMATCH:
        retVal = 'JVX_ERROR_SIZE_MISMATCH';
        break;
      case jvxErrorType.JVX_ERROR_NO_ACCESS:
        retVal = 'JVX_ERROR_NO_ACCESS';
        break;
      case jvxErrorType.JVX_ERROR_INVALID_SETTING:
        retVal = 'JVX_ERROR_INVALID_SETTING';
        break;
      case jvxErrorType.JVX_ERROR_TIMEOUT:
        retVal = 'JVX_ERROR_TIMEOUT';
        break;
      case jvxErrorType.JVX_ERROR_ABORT:
        retVal = 'JVX_ERROR_ABORT';
        break;
      case jvxErrorType.JVX_ERROR_POSTPONE:
        retVal = 'JVX_ERROR_POSTPONE';
        break;
      case jvxErrorType.JVX_ERROR_END_OF_FILE:
        retVal = 'JVX_ERROR_END_OF_FILE';
        break;
      case jvxErrorType.JVX_ERROR_STANDARD_EXCEPTION:
        retVal = 'JVX_ERROR_STANDARD_EXCEPTION';
        break;
      case jvxErrorType.JVX_ERROR_SEQUENCER_TOOK_CONTROL:
        retVal = 'JVX_ERROR_SEQUENCER_TOOK_CONTROL';
        break;
      case jvxErrorType.JVX_ERROR_THREAD_ONCE_MORE:
        retVal = 'JVX_ERROR_THREAD_ONCE_MORE';
        break;
      case jvxErrorType.JVX_ERROR_PROCESS_COMPLETE:
        retVal = 'JVX_ERROR_PROCESS_COMPLETE';
        break;
      case jvxErrorType.JVX_ERROR_DSP:
        retVal = 'JVX_ERROR_DSP';
        break;
      case jvxErrorType.JVX_ERROR_ALREADY_IN_USE:
        retVal = 'JVX_ERROR_ALREADY_IN_USE';
        break;
      case jvxErrorType.JVX_ERROR_PROTOCOL_ERROR:
        retVal = 'JVX_ERROR_PROTOCOL_ERROR';
        break;
      case jvxErrorType.JVX_ERROR_NOT_ENOUGH_INPUT_DATA:
        retVal = 'JVX_ERROR_NOT_ENOUGH_INPUT_DATA';
        break;
      case jvxErrorType.JVX_ERROR_NOT_ENOUGH_OUTPUT_SPACE:
        retVal = 'JVX_ERROR_NOT_ENOUGH_OUTPUT_SPACE';
        break;
      case jvxErrorType.JVX_ERROR_COMPROMISE:
        retVal = 'JVX_ERROR_COMPROMISE';
        break;
      case jvxErrorType.JVX_ERROR_REMOTE:
        retVal = 'JVX_ERROR_REMOTE';
        break;
      case jvxErrorType.JVX_ERROR_ALLOCATION:
        retVal = 'JVX_ERROR_ALLOCATION';
        break;
      case jvxErrorType.JVX_ERROR_INVALID_FORMAT:
        retVal = 'JVX_ERROR_INVALID_FORMAT';
        break;
      case jvxErrorType.JVX_ERROR_REQUEST_CALL_AGAIN:
        retVal = 'JVX_ERROR_REQUEST_CALL_AGAIN';
        break;
      case jvxErrorType.JVX_ERROR_NOT_IMPLEMENTED:
        retVal = 'JVX_ERROR_NOT_IMPLEMENTED';
        break;
    }
    return retVal;
  }
}
// =================================================================

enum jvxStateSwitchEnum {
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
  JVX_STATE_SWITCH_TERMINATE
}

extension jvxStateSwitchEEnum on jvxStateSwitchEnum {
  String get txt => describeEnum(this);
  static jvxStateSwitchEnum fromInt(int val) {
    jvxStateSwitchEnum ss = jvxStateSwitchEnum.JVX_STATE_SWITCH_NONE;
    if (val < jvxStateSwitchEnum.values.length) {
      ss = jvxStateSwitchEnum.values[val];
    }
    return ss;
  }
}

// ==========================================================================

abstract class jvxPropertyDescriptorEnum {
  static const int JVX_PROPERTY_DESCRIPTOR_NONE = 0;
  static const int JVX_PROPERTY_DESCRIPTOR_MIN = 1;
  static const int JVX_PROPERTY_DESCRIPTOR_CORE = 2;
  static const int JVX_PROPERTY_DESCRIPTOR_CONTROL = 3;
  static const int JVX_PROPERTY_DESCRIPTOR_FULL = 4;
  static const int JVX_PROPERTY_DESCRIPTOR_FULL_PLUS = 5;
}

// ==========================================================================

enum ayfBackendAccessEnum { JVX_ACCESS_BACKEND_FFI, JVX_ACCESS_BACKEND_JSON }

enum jvxDeviceCapabilityTypeShift {
  JVX_DEVICE_CAPABILITY_UNKNOWN_SHIFT,
  JVX_DEVICE_CAPABILITY_INPUT_SHIFT,
  JVX_DEVICE_CAPABILITY_OUTPUT_SHIFT,
  JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT,
  JVX_DEVICE_CAPABILITY_OTHER_SHIFT,
  JVX_DEVICE_CAPABILITY_LIMIT_SHIFT
}

// ==========================================================================

enum jvxDeviceDataFlowTypeEnum {
  JVX_DEVICE_DATAFLOW_UNKNOWN,
  JVX_DEVICE_DATAFLOW_ACTIVE,
  JVX_DEVICE_DATAFLOW_PASSIVE,
  JVX_DEVICE_DATAFLOW_OFFLINE,
  JVX_DEVICE_DATAFLOW_CONTROL,
  JVX_DEVICE_DATAFLOW_LIMIT
}

extension jvxDeviceDataFlowTypeEEnum on jvxDeviceDataFlowTypeEnum {
  String get txt => describeEnum(this);
  static jvxDeviceDataFlowTypeEnum fromInt(int val) {
    jvxDeviceDataFlowTypeEnum ss =
        jvxDeviceDataFlowTypeEnum.JVX_DEVICE_DATAFLOW_UNKNOWN;
    if (val < jvxDeviceDataFlowTypeEnum.values.length) {
      ss = jvxDeviceDataFlowTypeEnum.values[val];
    }
    return ss;
  }
}

// ==========================================================================

enum AyfPropertyReportLevel {
  // Nothing is reported via notifyListeners
  AYF_FRONTEND_REPORT_NO_REPORT,

  // Property ssUpdate is increased and reported via notifyListener
  AYF_FRONTEND_REPORT_SINGLE_PROPERTY,

  // Property ssUpdate and component ssUpdate are increased and reported via notifyListener
  AYF_FRONTEND_REPORT_COMPONENT_PROPERTY,

  // Report is via backend. This will inform all connected listeners
  AYF_BACKEND_REPORT_COMPONENT_PROPERTY,

  // Same as AYF_BACKEND_REPORT_COMPONENT_PROPERTY but it collects all set operations
  // and reports once ALL properties have been set - not after each property
  AYF_BACKEND_REPORT_COMPONENT_PROPERTY_COLLECT
}

/*
abstract class AyfFrontendReportLevelEnum {
  static const int AYF_FRONTEND_REPORT_NOTHING = 0;
  static const int AYF_FRONTEND_REPORT_SINGLE_PROPERTY = 0x1;
  static const int AYF_FRONTEND_REPORT_PROPERTY_CACHE_COMPONENT = 0x2;
}
*/

enum AyfBackendReportPropertySetEnum {
  AYF_BACKEND_REPORT_SET_PROPERTY_ACTIVE,
  AYF_BACKEND_REPORT_SET_PROPERTY_INACTIVE
}
